# 구체 메타모델(thermal_shock_v0/Table/Polynomial) 단위 테스트.
"""메타모델 구현 검증 — 등록/계약/단조성/보간/손계산.

합성 fixture는 테스트 내부에서 생성하며 외부 파일에 의존하지 않는다.
"""
from __future__ import annotations

import json

import pytest

from bga_router.metamodel import get_metamodel
# thermal_shock_v0는 import 시점에 registry에 등록된다.
import bga_router.metamodel.models.thermal_shock_v0  # noqa: F401
from bga_router.metamodel.table import TableMetamodel
from bga_router.metamodel.polynomial import PolynomialMetamodel


def _base_thermal_features(**overrides):
    feats = {
        "top_pkg_area": 100.0,
        "bot_pkg_area": 100.0,
        "center_distance_mm": 5.0,
        "overlap_area_mm2": 20.0,
        "min_pkg_dim_mm": 10.0,
        "board_thickness_mm": 1.0,
    }
    feats.update(overrides)
    return feats


def test_thermal_shock_registered_and_contract():
    model = get_metamodel("thermal_shock_v0")
    out = model.predict(_base_thermal_features())
    assert "thermal_shock_risk_score" in out
    assert "estimated_cycles_to_failure" in out
    assert "confidence" in out
    assert "provenance" in out
    assert "flags" in out
    assert 0.0 <= out["thermal_shock_risk_score"] <= 1.0
    assert 0.0 <= out["confidence"] <= 1.0
    assert isinstance(out["flags"], list)
    assert out["provenance"].startswith("reference_analytical:thermal_shock_v0@")


def test_thermal_shock_monotonic_overlap():
    model = get_metamodel("thermal_shock_v0")
    low = model.predict(_base_thermal_features(overlap_area_mm2=5.0))
    high = model.predict(_base_thermal_features(overlap_area_mm2=80.0))
    assert (
        high["thermal_shock_risk_score"] > low["thermal_shock_risk_score"]
    )


def test_thermal_shock_monotonic_center_distance():
    model = get_metamodel("thermal_shock_v0")
    near = model.predict(_base_thermal_features(center_distance_mm=1.0))
    far = model.predict(_base_thermal_features(center_distance_mm=25.0))
    assert far["thermal_shock_risk_score"] < near["thermal_shock_risk_score"]


def test_thermal_shock_cycles_inverse_to_risk():
    model = get_metamodel("thermal_shock_v0")
    low_risk = model.predict(_base_thermal_features(overlap_area_mm2=1.0))
    high_risk = model.predict(_base_thermal_features(overlap_area_mm2=90.0))
    assert (
        high_risk["estimated_cycles_to_failure"]
        <= low_risk["estimated_cycles_to_failure"]
    )


def test_table_metamodel_interpolation(tmp_path):
    spec = {
        "name": "demo_table",
        "version": "0.1.0",
        "input_features": ["x"],
        "output_keys": ["y"],
        "grid_axis": "x",
        "points": [
            {"x": 0.0, "outputs": {"y": 0.0}},
            {"x": 10.0, "outputs": {"y": 100.0}},
        ],
    }
    path = tmp_path / "table.json"
    path.write_text(json.dumps(spec), encoding="utf-8")

    model = TableMetamodel.from_json(str(path))
    mid = model.predict({"x": 5.0})
    assert mid["y"] == pytest.approx(50.0)
    assert mid["provenance"] == "table:demo_table@0.1.0"
    # 범위 밖 클램프.
    assert model.predict({"x": -3.0})["y"] == pytest.approx(0.0)
    assert model.predict({"x": 99.0})["y"] == pytest.approx(100.0)


def test_polynomial_metamodel_hand_calc(tmp_path):
    # y = 2*x^2 + 3*x + 1
    spec = {
        "name": "demo_poly",
        "version": "0.1.0",
        "input_features": ["x"],
        "output_keys": ["y"],
        "terms": {
            "y": [
                {"coef": 2.0, "powers": {"x": 2}},
                {"coef": 3.0, "powers": {"x": 1}},
                {"coef": 1.0, "powers": {}},
            ]
        },
    }
    path = tmp_path / "poly.json"
    path.write_text(json.dumps(spec), encoding="utf-8")

    model = PolynomialMetamodel.from_json(str(path))
    # x=4 -> 2*16 + 3*4 + 1 = 45
    out = model.predict({"x": 4.0})
    assert out["y"] == pytest.approx(45.0)
    assert out["provenance"] == "polynomial:demo_poly@0.1.0"
    assert out["confidence"] == pytest.approx(0.75)
