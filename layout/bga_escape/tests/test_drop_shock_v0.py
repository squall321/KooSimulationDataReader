# drop_shock_v0 참조 대리모델의 등록/계약/단조성/infer 경로 테스트.
"""drop_shock_v0 검증 — 등록/계약/단조성(4)/drops 역상관/infer 경로.

합성 fixture는 테스트 내부에서 생성하며 외부 파일에 의존하지 않는다.
"""
from __future__ import annotations

from bga_router.metamodel import get_metamodel
# drop_shock_v0는 import 시점에 registry에 등록된다.
import bga_router.metamodel.models.drop_shock_v0  # noqa: F401
from bga_router.metamodel.infer import infer


def _base_drop_features(**overrides):
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


def test_drop_shock_registered_and_describe():
    model = get_metamodel("drop_shock_v0")
    desc = model.describe()
    assert desc["name"] == "drop_shock_v0"
    assert desc["version"] == "0.1.0"
    assert desc["output_keys"][0] == "drop_shock_risk_score"
    assert "estimated_drops_to_failure" in desc["output_keys"]
    assert desc["input_features"] == [
        "top_pkg_area",
        "bot_pkg_area",
        "center_distance_mm",
        "overlap_area_mm2",
        "min_pkg_dim_mm",
        "board_thickness_mm",
    ]


def test_drop_shock_contract():
    model = get_metamodel("drop_shock_v0")
    out = model.predict(_base_drop_features())
    assert "drop_shock_risk_score" in out
    assert "estimated_drops_to_failure" in out
    assert "confidence" in out
    assert "provenance" in out
    assert "flags" in out
    assert 0.0 <= out["drop_shock_risk_score"] <= 1.0
    assert isinstance(out["flags"], list)
    assert out["provenance"].startswith("reference_analytical:drop_shock_v0@")
    assert out["confidence"] == 0.5


def test_drop_shock_monotonic_pkg_size():
    # max(top,bot) 증가 → risk 증가. bot 고정으로 min(=proximity 분모) 불변 유지.
    model = get_metamodel("drop_shock_v0")
    small = model.predict(_base_drop_features(top_pkg_area=100.0))
    large = model.predict(_base_drop_features(top_pkg_area=350.0))
    assert (
        large["drop_shock_risk_score"] > small["drop_shock_risk_score"]
    )


def test_drop_shock_monotonic_board_thickness():
    # board_thickness_mm 감소 → risk 증가.
    model = get_metamodel("drop_shock_v0")
    thin = model.predict(_base_drop_features(board_thickness_mm=0.2))
    thick = model.predict(_base_drop_features(board_thickness_mm=5.0))
    assert thin["drop_shock_risk_score"] > thick["drop_shock_risk_score"]


def test_drop_shock_monotonic_overlap():
    # overlap_area_mm2 증가 → risk 증가.
    model = get_metamodel("drop_shock_v0")
    low = model.predict(_base_drop_features(overlap_area_mm2=5.0))
    high = model.predict(_base_drop_features(overlap_area_mm2=80.0))
    assert high["drop_shock_risk_score"] > low["drop_shock_risk_score"]


def test_drop_shock_monotonic_center_distance():
    # center_distance_mm 증가 → risk 감소.
    model = get_metamodel("drop_shock_v0")
    near = model.predict(_base_drop_features(center_distance_mm=1.0))
    far = model.predict(_base_drop_features(center_distance_mm=25.0))
    assert far["drop_shock_risk_score"] < near["drop_shock_risk_score"]


def test_drop_shock_drops_inverse_to_risk():
    # risk 증가 시 drops 감소(비증가).
    model = get_metamodel("drop_shock_v0")
    low_risk = model.predict(_base_drop_features(overlap_area_mm2=1.0))
    high_risk = model.predict(_base_drop_features(overlap_area_mm2=90.0))
    assert (
        high_risk["estimated_drops_to_failure"]
        <= low_risk["estimated_drops_to_failure"]
    )


def test_drop_shock_infer_path():
    # pair_features 출력 키(center_dist 등)를 사용한 infer 경로.
    feats = {
        "top_pkg_area": 100.0,
        "bot_pkg_area": 100.0,
        "center_dist": 5.0,
        "overlap_area_mm2": 20.0,
        "min_pkg_dim_mm": 10.0,
    }
    result = infer(feats, "drop_shock_v0")
    assert "drop_shock_risk_score" in result
    assert 0.0 <= result["drop_shock_risk_score"] <= 1.0
    assert "input_flags" in result
    assert isinstance(result["input_flags"], list)
    assert result["provenance"].startswith(
        "reference_analytical:drop_shock_v0@"
    )
