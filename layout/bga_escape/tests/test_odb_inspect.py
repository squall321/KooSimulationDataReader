# odb_inspect 모듈 단위 테스트 — 합성 fixture 로 요약/심볼/통합 검증.
"""odb_inspect 테스트.

tmp_path 에 합성 index.json + 레이어 json 을 만들어 summarize/symbol/inspect 를
검증한다. 실제 /tmp/p3_layers.json/ 가 있으면 optional 로 추가 검증한다.
"""

from __future__ import annotations

import json
import os

import pytest

from bga_router.integrations import odb_inspect


def _write_synthetic(root):
    """tmp_path 에 index.json + 2개 레이어 json 을 만든다."""
    index = {
        "job": "SYN_JOB",
        "step": "SYN_STEP",
        "bbox": {"xmin": -1.0, "ymin": -2.0, "xmax": 9.0, "ymax": 3.0},
        "region": None,
        "layers": [
            {"name": "lay2", "file": "lay2.json", "total": 30,
             "lines": 10, "pads": 15, "arcs": 2, "surfaces": 3,
             "bbox": {"xmin": 0, "ymin": 0, "xmax": 5, "ymax": 5}},
            {"name": "lay3", "file": "lay3.json", "total": 8,
             "lines": 3, "pads": 5, "arcs": 0, "surfaces": 0,
             "bbox": {"xmin": 0, "ymin": 0, "xmax": 5, "ymax": 5}},
        ],
    }
    lay2 = {
        "name": "lay2", "units": "MM", "type": "Signal",
        "bbox": {"xmin": 0, "ymin": 0, "xmax": 5, "ymax": 5},
        "counts": {"lines": 10, "pads": 15, "arcs": 2, "surfaces": 3},
        "symbols": {
            "r100": {"type": "Round", "w": 0.1, "h": 0.1},
            "r200": {"type": "Round", "w": 0.2, "h": 0.2},
            "oval600x900": {"type": "Oblong", "w": 0.6, "h": 0.9},
        },
        "features": [{"t": "P", "x": 1.0, "y": 1.0, "sym": "r100", "pol": "P"}],
    }
    lay3 = {
        "name": "lay3", "units": "MM", "type": "Power",
        "bbox": {"xmin": 0, "ymin": 0, "xmax": 5, "ymax": 5},
        "counts": {"lines": 3, "pads": 5, "arcs": 0, "surfaces": 0},
        "symbols": {
            "r100": {"type": "Round", "w": 0.1, "h": 0.1},
            "rect300": {"type": "Rectangle", "w": 0.3, "h": 0.3},
        },
        "features": [{"t": "L", "x": 0.0, "y": 0.0, "sym": "r100", "pol": "P"}],
    }
    with open(os.path.join(root, "index.json"), "w") as f:
        json.dump(index, f)
    with open(os.path.join(root, "lay2.json"), "w") as f:
        json.dump(lay2, f)
    with open(os.path.join(root, "lay3.json"), "w") as f:
        json.dump(lay3, f)


def test_load_index_missing(tmp_path):
    with pytest.raises(FileNotFoundError):
        odb_inspect.load_odb_index(str(tmp_path))


def test_load_index(tmp_path):
    _write_synthetic(str(tmp_path))
    index = odb_inspect.load_odb_index(str(tmp_path))
    assert index["job"] == "SYN_JOB"
    assert len(index["layers"]) == 2


def test_summarize_basic(tmp_path):
    _write_synthetic(str(tmp_path))
    summ = odb_inspect.summarize_odb(str(tmp_path))

    assert summ["job"] == "SYN_JOB"
    assert summ["step"] == "SYN_STEP"
    assert summ["board_bbox_mm"] == [-1.0, -2.0, 9.0, 3.0]
    # width = 9 - (-1) = 10, height = 3 - (-2) = 5
    assert summ["board_size_mm"] == [10.0, 5.0]
    assert summ["layer_count"] == 2
    assert summ["total_features"] == 38
    assert summ["feature_totals"] == {
        "lines": 13, "pads": 20, "arcs": 2, "surfaces": 3,
    }


def test_summarize_reads_layer_type(tmp_path):
    _write_synthetic(str(tmp_path))
    summ = odb_inspect.summarize_odb(str(tmp_path))
    by_name = {ly["name"]: ly for ly in summ["layers"]}
    assert by_name["lay2"]["type"] == "Signal"
    assert by_name["lay3"]["type"] == "Power"
    assert by_name["lay2"]["pads"] == 15


def test_summarize_without_layer_json(tmp_path):
    """개별 layer json 이 없어도 index 통계만으로 동작하고 type 은 None."""
    _write_synthetic(str(tmp_path))
    os.remove(os.path.join(str(tmp_path), "lay2.json"))
    os.remove(os.path.join(str(tmp_path), "lay3.json"))
    summ = odb_inspect.summarize_odb(str(tmp_path))
    assert summ["layer_count"] == 2
    assert summ["total_features"] == 38
    for ly in summ["layers"]:
        assert ly["type"] is None


def test_summarize_skips_corrupt_layer_json(tmp_path):
    """레이어 json 파싱 실패는 skip — 해당 레이어는 index 통계만."""
    _write_synthetic(str(tmp_path))
    with open(os.path.join(str(tmp_path), "lay2.json"), "w") as f:
        f.write("{ this is not valid json ")
    summ = odb_inspect.summarize_odb(str(tmp_path))
    by_name = {ly["name"]: ly for ly in summ["layers"]}
    assert by_name["lay2"]["type"] is None
    assert by_name["lay3"]["type"] == "Power"
    # index 통계는 여전히 유지
    assert by_name["lay2"]["total"] == 30


def test_symbol_summary(tmp_path):
    _write_synthetic(str(tmp_path))
    syms = odb_inspect.symbol_summary(str(tmp_path))
    # Round w: 0.1 (lay2+lay3), 0.2 (lay2) -> sorted unique
    assert syms["drill_sizes_mm"] == [0.1, 0.2]
    # Oblong x1, Rectangle x1
    assert syms["pad_shapes"] == {"Oblong": 1, "Rectangle": 1}
    # unique symbol names: r100, r200, oval600x900, rect300
    assert syms["unique_symbols"] == 4


def test_inspect_odb(tmp_path):
    _write_synthetic(str(tmp_path))
    result = odb_inspect.inspect_odb(str(tmp_path))
    assert "structure" in result
    assert "symbols" in result
    assert result["structure"]["job"] == "SYN_JOB"
    assert result["symbols"]["drill_sizes_mm"] == [0.1, 0.2]


P3_DIR = "/tmp/p3_layers.json"


@pytest.mark.skipif(
    not os.path.isfile(os.path.join(P3_DIR, "index.json")),
    reason="real P3 fixture not present",
)
def test_real_p3_optional():
    summ = odb_inspect.summarize_odb(P3_DIR)
    assert summ["job"] == "P3_EUR_REV03"
    assert summ["layer_count"] > 0
    assert summ["total_features"] > 0
    w, h = summ["board_size_mm"]
    assert w > 0 and h > 0

    syms = odb_inspect.symbol_summary(P3_DIR)
    assert syms["unique_symbols"] > 0
    assert isinstance(syms["drill_sizes_mm"], list)
