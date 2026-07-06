# infer 오케스트레이터(pair_features/infer/infer_pair/infer_all_pairs)의 계약 테스트.
import subprocess
import sys

import pytest

from bga_router.metrics.package_features import PackageInstance
from bga_router.metamodel.infer import (
    pair_features,
    infer,
    infer_pair,
    infer_all_pairs,
)


def _pkg(ref, side, cx, cy, w, h):
    """합성 PackageInstance 생성 헬퍼. area는 w*h로 계산."""
    return PackageInstance(
        ref_des=ref,
        pkg_name=f"PKG_{ref}",
        side=side,
        side_source="comp_layer",
        cx_mm=cx,
        cy_mm=cy,
        width_mm=w,
        height_mm=h,
        area_mm2=w * h,
        pin_count=100,
        pitch_mm=0.5,
        rotation_deg=0.0,
    )


def test_infer_pair_has_risk_score():
    top = _pkg("U1", "TOP", 0.0, 0.0, 10.0, 10.0)
    bot = _pkg("U2", "BOT", 0.0, 0.0, 10.0, 10.0)  # 완전 겹침
    result = infer_pair(top, bot)
    assert "thermal_shock_risk_score" in result
    assert 0.0 <= result["thermal_shock_risk_score"] <= 1.0
    assert result["top_ref"] == "U1"
    assert result["bot_ref"] == "U2"
    assert "estimated_cycles_to_failure" in result
    assert result["provenance"].startswith("reference_analytical")
    assert isinstance(result["input_flags"], list)


def test_infer_all_pairs_orders_by_risk():
    # 강하게 겹치는 큰 쌍과, 거의 안 겹치는 작은 쌍.
    top_a = _pkg("A_T", "TOP", 0.0, 0.0, 12.0, 12.0)
    bot_a = _pkg("A_B", "BOT", 0.0, 0.0, 12.0, 12.0)   # 완전 겹침
    top_b = _pkg("B_T", "TOP", 50.0, 50.0, 4.0, 4.0)
    bot_b = _pkg("B_B", "BOT", 53.0, 53.0, 4.0, 4.0)   # 살짝 겹침, 멀리
    pkgs = [top_a, bot_a, top_b, bot_b]

    results = infer_all_pairs(pkgs)
    assert len(results) == 2
    # 큰 겹침 쌍이 위험 순위 상위.
    assert results[0]["top_ref"] == "A_T"
    assert (
        results[0]["thermal_shock_risk_score"]
        > results[1]["thermal_shock_risk_score"]
    )


def test_infer_all_pairs_min_overlap_filter():
    top_a = _pkg("A_T", "TOP", 0.0, 0.0, 12.0, 12.0)
    bot_a = _pkg("A_B", "BOT", 0.0, 0.0, 12.0, 12.0)   # 완전 겹침(ratio=1)
    top_b = _pkg("B_T", "TOP", 50.0, 50.0, 4.0, 4.0)
    bot_b = _pkg("B_B", "BOT", 53.0, 53.0, 4.0, 4.0)   # 작은 겹침
    pkgs = [top_a, bot_a, top_b, bot_b]

    # ratio 0.9 이상만 → 강한 겹침 쌍만 남는다.
    results = infer_all_pairs(pkgs, min_overlap_ratio=0.9)
    assert len(results) == 1
    assert results[0]["top_ref"] == "A_T"


def test_pair_features_min_dim():
    top = _pkg("U1", "TOP", 0.0, 0.0, 10.0, 6.0)
    bot = _pkg("U2", "BOT", 0.0, 0.0, 8.0, 4.0)
    feats = pair_features(top, bot)
    # 각 패키지 min(w,h): top=6, bot=4 → 둘 중 최소 = 4.
    assert feats["min_pkg_dim_mm"] == 4.0
    assert feats["top_pkg_area"] == 60.0
    assert feats["overlap_area_mm2"] > 0.0


def test_infer_missing_feature_uses_default():
    # board_thickness_mm를 뺀 피처. default(1.0)로 보충되어야 한다.
    feats = {
        "top_pkg_area": 100.0,
        "bot_pkg_area": 100.0,
        "center_dist": 0.0,
        "overlap_area_mm2": 100.0,
        "min_pkg_dim_mm": 10.0,
    }
    result = infer(feats, "thermal_shock_v0")
    assert "thermal_shock_risk_score" in result
    assert result["input_flags"] == []


def test_infer_missing_required_feature_raises():
    # min_pkg_dim_mm 누락 + default 없음 → ValueError.
    feats = {
        "top_pkg_area": 100.0,
        "bot_pkg_area": 100.0,
        "center_dist": 0.0,
        "overlap_area_mm2": 100.0,
    }
    with pytest.raises(ValueError):
        infer(feats, "thermal_shock_v0")


def test_package_import_auto_registers_model():
    # metamodel 패키지만 import해도(infer 미import) thermal_shock_v0가 등록돼야 한다.
    # 같은 프로세스는 다른 테스트가 이미 등록해 가려질 수 있어 서브프로세스로 격리 검증.
    code = (
        "import bga_router.metamodel as m; "
        "names=[d['name'] for d in m.list_metamodels()]; "
        "assert 'thermal_shock_v0' in names, names; "
        "m.get_metamodel('thermal_shock_v0')"
    )
    # cwd를 bga_escape 루트로 고정 — full-suite 실행 시 pytest rootdir이
    # 상위로 잡혀 bga_router import가 실패하던 문제 방지.
    from pathlib import Path as _P
    root = _P(__file__).resolve().parents[1]
    proc = subprocess.run(
        [sys.executable, "-c", code],
        capture_output=True,
        text=True,
        cwd=str(root),
    )
    assert proc.returncode == 0, proc.stderr


def test_board_thickness_override_changes_result():
    top = _pkg("U1", "TOP", 0.0, 0.0, 10.0, 10.0)
    bot = _pkg("U2", "BOT", 0.0, 0.0, 10.0, 10.0)
    thin = infer_pair(top, bot, board_thickness_mm=0.2)
    thick = infer_pair(top, bot, board_thickness_mm=5.0)
    # 얇은 보드일수록 risk가 더 크다(thin_factor 보정).
    assert (
        thin["thermal_shock_risk_score"]
        > thick["thermal_shock_risk_score"]
    )
