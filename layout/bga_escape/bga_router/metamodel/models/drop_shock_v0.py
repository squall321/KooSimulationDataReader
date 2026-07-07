# 낙하/기계충격(drop shock) 위험도를 근사하는 참조 해석적 메타모델을 정의/등록하는 모듈.
"""drop_shock_v0 — 낙하/기계충격 참조 대리모델.

주의: 여기 사용된 공식은 JESD22-B111 계열의 낙하 시험을 흉내낸 *물리적으로
그럴듯한 placeholder*이다. 실제 신뢰성 예측용으로 검증된 계수가 아니며,
단조성과 대략적 스케일만 보장하는 reference 모델이다. 그래서 confidence는
낮게 고정한다.

단조성 계약.
- max(top_pkg_area, bot_pkg_area) 증가 → risk 증가 (큰 패키지 = 큰 DNP =
  코너 조인트 응력 상승).
- board_thickness_mm 감소 → risk 증가 (얇은 보드 = 굽힘 곡률 상승 = 응력 상승).
- overlap_area_mm2 증가 → risk 증가 (반대면 back-to-back 실장 = 국부 응력 집중).
- center_distance_mm 증가 → risk 감소 (약한 완화항, 작은 가중치).
"""
from __future__ import annotations

from ..analytical import AnalyticalMetamodel
from ..registry import register_metamodel

# 상수 가중치(합이 1을 넘지 않도록 잡되, clamp로 최종 방어).
_W_SIZE = 0.45        # 큰 쪽 패키지 크기(DNP) 기여
_W_FLEX = 0.30        # 얇은 보드 굽힘 기여
_W_PROXIMITY = 0.30   # back-to-back 근접도 기여
_W_DISTANCE = 0.20    # center distance 완화 기여

_BASE_DROPS = 200     # risk=0일 때 기준 낙하 횟수(placeholder base)
_DROP_EXPONENT = 3.0  # risk↑ → drops 급감(JESD22-B111 성격)

# 정규화 스케일(입력이 대략 이 값 근처에서 0~1로 사상되도록 하는 상수).
_SIZE_SCALE_MM2 = 400.0   # 20mm x 20mm 패키지를 대략 1로 정규화
_DIST_SCALE_MM = 30.0     # 30mm 이격을 대략 1로 정규화


def _clamp01(x: float) -> float:
    if x < 0.0:
        return 0.0
    if x > 1.0:
        return 1.0
    return x


def _drop_shock_fn(features: dict) -> dict:
    top_area = float(features["top_pkg_area"])
    bot_area = float(features["bot_pkg_area"])
    center_dist = float(features["center_distance_mm"])
    overlap = float(features["overlap_area_mm2"])
    min_dim = float(features["min_pkg_dim_mm"])  # 계약상 입력(유효성 참조로만).
    board_thk = float(features["board_thickness_mm"])  # 계약상 입력.

    max_area = max(top_area, bot_area)
    min_area = min(top_area, bot_area)

    # 크기 정규화: 큰 쪽 패키지 면적(DNP 대용)을 기준 스케일로 정규화.
    size_norm = _clamp01(max_area / _SIZE_SCALE_MM2)

    # 굽힘 정규화: 보드가 얇을수록 1에 가까워지는 항(board_thk>=0에서 0~1).
    flex_norm = 1.0 / (1.0 + max(0.0, board_thk))

    # 근접도: 겹침 면적 / 작은 패키지 면적. 0~1로 클램프.
    proximity = _clamp01(overlap / max(1e-6, min_area))

    # 이격 정규화: 멀수록 1에 가까워지는 완화항.
    dist_norm = _clamp01(center_dist / _DIST_SCALE_MM)

    min_dim_ok = 1.0 if min_dim > 0.0 else 1.0  # min_dim은 유효성 참조로만 사용.

    risk = _clamp01(
        (
            _W_SIZE * size_norm
            + _W_FLEX * flex_norm
            + _W_PROXIMITY * proximity
            - _W_DISTANCE * dist_norm
        )
        * min_dim_ok
    )

    drops = round(_BASE_DROPS * (1.0 - risk) ** _DROP_EXPONENT)

    return {
        "drop_shock_risk_score": risk,
        "estimated_drops_to_failure": drops,
    }


drop_shock_v0 = AnalyticalMetamodel(
    name="drop_shock_v0",
    version="0.1.0",
    input_features=[
        "top_pkg_area",
        "bot_pkg_area",
        "center_distance_mm",
        "overlap_area_mm2",
        "min_pkg_dim_mm",
        "board_thickness_mm",
    ],
    output_keys=[
        "drop_shock_risk_score",
        "estimated_drops_to_failure",
    ],
    fn=_drop_shock_fn,
    confidence=0.5,
)

register_metamodel(drop_shock_v0)
