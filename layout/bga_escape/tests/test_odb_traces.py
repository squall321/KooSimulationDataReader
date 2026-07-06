# odb_traces 단위 테스트 — 합성 em_data로 net 분석/분류/요약 검증
"""Tests for :mod:`bga_router.metrics.odb_traces`.

Synthetic ``em_data`` is built inline (no external /tmp dependency). An
optional real P3 fixture at ``/tmp/p3_em_full.json`` is exercised via
``pytest.skip`` when absent.
"""

from __future__ import annotations

import os

import pytest

from bga_router.metrics.odb_traces import (
    analyze_net,
    classify_net,
    load_em_data,
    net_bbox_mm,
    net_layers,
    polygon_perimeter_mm,
    summarize_traces,
)


def _fill(outer):
    return {"type": "fill", "outer": outer, "holes": []}


@pytest.fixture
def em_data():
    # net_A: COMP + LAY2 (2층, via 추정). net_B: COMP 1층.
    # GND_1: power_ground.
    return {
        "job": "SYN", "units": "MM", "region": None,
        "stackup": {},
        "layers": {
            "COMP": {"z_bottom": 0.0, "z_top": 0.035, "nets": {
                "net_A": {"polygons": [_fill([[0, 0], [2, 0], [2, 1], [0, 1]])]},
                "net_B": {"polygons": [_fill([[5, 5], [6, 5], [6, 6], [5, 6]])]},
                "GND_1": {"polygons": [_fill([[0, 0], [10, 0], [10, 10], [0, 10]])]},
            }},
            "LAY2": {"z_bottom": 0.1, "z_top": 0.135, "nets": {
                "net_A": {"polygons": [_fill([[1, 1], [3, 1], [3, 2], [1, 2]])]},
            }},
        },
    }


def test_polygon_perimeter_unit_square():
    assert polygon_perimeter_mm([[0, 0], [1, 0], [1, 1], [0, 1]]) == 4.0


def test_polygon_perimeter_explicitly_closed_ring():
    # 첫 점을 끝에 반복해도 닫힘 세그먼트 길이가 0이라 결과 동일하다.
    assert polygon_perimeter_mm([[0, 0], [1, 0], [1, 1], [0, 1], [0, 0]]) == 4.0


def test_net_layers(em_data):
    assert net_layers(em_data, "net_A") == ["COMP", "LAY2"]
    assert net_layers(em_data, "net_B") == ["COMP"]
    assert net_layers(em_data, "missing") == []


def test_net_bbox_union(em_data):
    # net_A: COMP bbox (0,0,2,1) ∪ LAY2 bbox (1,1,3,2) = (0,0,3,2)
    assert net_bbox_mm(em_data, "net_A") == (0.0, 0.0, 3.0, 2.0)


def test_analyze_net_A_has_via(em_data):
    a = analyze_net(em_data, "net_A")
    assert a["net"] == "net_A"
    assert a["layers"] == ["COMP", "LAY2"]
    assert a["layer_span"] == 2
    assert a["has_via"] is True
    assert a["polygon_count"] == 2
    assert a["bbox_mm"] == [0.0, 0.0, 3.0, 2.0]
    assert a["extent_mm"] == [3.0, 2.0]
    # 두 단위 폴리곤 둘레: 2x1 사각형 둘레 6.0 + 6.0 = 12.0
    assert a["total_perimeter_mm"] == pytest.approx(12.0)
    assert a["est_trace_length_mm"] == pytest.approx((3.0 ** 2 + 2.0 ** 2) ** 0.5)


def test_analyze_net_B_no_via(em_data):
    b = analyze_net(em_data, "net_B")
    assert b["layer_span"] == 1
    assert b["has_via"] is False
    assert b["polygon_count"] == 1


def test_classify_power_ground(em_data):
    assert classify_net(em_data, "GND_1") == "power_ground"
    assert classify_net(em_data, "net_A") == "signal"
    assert classify_net(em_data, "net_B") == "signal"


def test_summarize_traces(em_data):
    s = summarize_traces(em_data)
    assert s["net_count"] == 3
    assert s["layer_names"] == ["COMP", "LAY2"]
    assert s["by_class"] == {"signal": 2, "power_ground": 1, "plane_candidate": 0}
    assert s["multi_layer_nets"] == ["net_A"]
    assert set(s["nets"]) == {"net_A", "net_B", "GND_1"}


def test_summarize_max_nets_caps_detail_not_counts(em_data):
    s = summarize_traces(em_data, max_nets=1)
    # 상세는 1개만, 카운트/분류는 전체 반영.
    assert len(s["nets"]) == 1
    assert s["net_count"] == 3
    assert s["by_class"]["power_ground"] == 1
    assert s["multi_layer_nets"] == ["net_A"]


def test_analyze_net_no_geometry_does_not_crash():
    # net이 nets에 등록됐지만 폴리곤이 없거나 outer가 비어도 크래시 없이
    # degenerate zero bbox를 반환해야 한다.
    em = {"layers": {"COMP": {"nets": {
        "empty_net": {"polygons": []},
        "no_outer": {"polygons": [{"type": "fill", "outer": [], "holes": []}]},
    }}}}
    for name in ("empty_net", "no_outer"):
        a = analyze_net(em, name)
        assert a["bbox_mm"] == [0.0, 0.0, 0.0, 0.0]
        assert a["extent_mm"] == [0.0, 0.0]
        assert a["est_trace_length_mm"] == 0.0


def test_summarize_traces_counts_geometryless_net_without_crash():
    # 지오메트리 없는 net도 net_count/by_class에 반영되고, 상세 패스가
    # 크래시하지 않아야 한다 (docstring의 "stays honest" 보장).
    em = {"layers": {"COMP": {"nets": {
        "empty_net": {"polygons": []},
        "good": {"polygons": [_fill([[0, 0], [1, 0], [1, 1], [0, 1]])]},
    }}}}
    s = summarize_traces(em)
    assert s["net_count"] == 2
    assert set(s["nets"]) == {"empty_net", "good"}
    assert s["nets"]["empty_net"]["est_trace_length_mm"] == 0.0


def test_optional_real_p3_fixture():
    path = "/tmp/p3_em_full.json"
    if not os.path.exists(path):
        pytest.skip("real P3 em_data fixture not present")
    data = load_em_data(path)
    s = summarize_traces(data, max_nets=20)
    assert s["net_count"] > 0
