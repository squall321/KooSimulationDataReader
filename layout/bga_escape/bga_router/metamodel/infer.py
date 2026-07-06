# 패키지 피처를 메타모델 입력으로 변환해 추론을 수행하는 오케스트레이터 모듈.
"""infer — 피처 추출과 메타모델 예측을 잇는 추론 오케스트레이터.

계층 구성.
- ``DEFAULT_SPECS``: 메타모델별 ``FeatureVectorSpec`` (ODB 추출 키 ↔ 모델 입력 키).
- ``pair_features``: 상/하단 두 ``PackageInstance``에서 쌍 피처를 계산.
- ``infer``: 단일 피처 dict를 스펙으로 검증한 뒤 등록된 메타모델로 예측.
- ``infer_pair`` / ``infer_all_pairs``: 패키지 쌍 단위 추론과 위험 순위화.

thermal_shock_v0 참조 모델은 import 시점에 registry에 등록된다.
"""
from __future__ import annotations

from ..metrics.package_features import (
    PackageInstance,
    bbox_of,
    bbox_overlap_area,
    center_distance_mm,
    summarize_packages,
)
from .feature_spec import FeatureVectorSpec
from .registry import get_metamodel

# import 부작용으로 thermal_shock_v0을 registry에 등록한다(참조 대리모델).
from .models import thermal_shock_v0 as _thermal_shock_v0  # noqa: F401


# 메타모델별 입력 벡터 스펙. mapping: model_input_key -> extracted_feature_key.
DEFAULT_SPECS: dict = {
    "thermal_shock_v0": FeatureVectorSpec(
        mapping={
            "top_pkg_area": "top_pkg_area",
            "bot_pkg_area": "bot_pkg_area",
            "center_distance_mm": "center_dist",
            "overlap_area_mm2": "overlap_area_mm2",
            "min_pkg_dim_mm": "min_pkg_dim_mm",
            "board_thickness_mm": "board_thickness_mm",
        },
        defaults={"board_thickness_mm": 1.0},
    ),
}


def pair_features(top_pkg: PackageInstance, bot_pkg: PackageInstance) -> dict:
    """상/하단 두 패키지에서 쌍 피처를 계산해 dict로 반환.

    ``package_features``의 ``bbox_overlap_area`` / ``center_distance_mm`` /
    ``bbox_of``를 재사용한다. ``min_pkg_dim_mm``은 두 패키지 각각의
    min(width, height) 중 더 작은 값이다.
    """
    top_area = top_pkg.area_mm2
    bot_area = bot_pkg.area_mm2
    overlap = bbox_overlap_area(bbox_of(top_pkg), bbox_of(bot_pkg))
    denom = min(top_area, bot_area)
    overlap_ratio = (overlap / denom) if denom > 0 else 0.0
    min_pkg_dim = min(
        min(top_pkg.width_mm, top_pkg.height_mm),
        min(bot_pkg.width_mm, bot_pkg.height_mm),
    )
    return {
        "top_pkg_area": top_area,
        "bot_pkg_area": bot_area,
        "center_dist": center_distance_mm(top_pkg, bot_pkg),
        "overlap_area_mm2": overlap,
        "overlap_ratio": overlap_ratio,
        "min_pkg_dim_mm": min_pkg_dim,
    }


def infer(
    features: dict,
    metamodel_name: str,
    *,
    spec: FeatureVectorSpec = None,
    board_thickness_mm: float = None,
) -> dict:
    """추출 피처(features)를 스펙으로 검증한 뒤 메타모델로 예측.

    spec이 없으면 ``DEFAULT_SPECS[metamodel_name]``을 사용한다.
    ``board_thickness_mm``을 주면 features의 해당 값을 덮어쓴다.
    반환 dict에는 스펙 검증에서 나온 ``input_flags``가 포함된다.
    """
    if spec is None:
        if metamodel_name not in DEFAULT_SPECS:
            known = sorted(DEFAULT_SPECS.keys())
            raise KeyError(
                f"no default spec for '{metamodel_name}'. known: {known}"
            )
        spec = DEFAULT_SPECS[metamodel_name]

    if board_thickness_mm is not None:
        features = {**features, "board_thickness_mm": board_thickness_mm}

    feature_dict, flags = spec.build(features)
    model = get_metamodel(metamodel_name)
    result = model.predict(feature_dict)
    result["input_flags"] = flags
    return result


def infer_pair(
    top_pkg: PackageInstance,
    bot_pkg: PackageInstance,
    metamodel_name: str = "thermal_shock_v0",
    **kw,
) -> dict:
    """상/하단 패키지 쌍에서 피처를 뽑아 추론하고 refDes를 부착."""
    feats = pair_features(top_pkg, bot_pkg)
    result = infer(feats, metamodel_name, **kw)
    result["top_ref"] = top_pkg.ref_des
    result["bot_ref"] = bot_pkg.ref_des
    return result


def _risk_value(result: dict, model) -> float:
    """정렬 기준 값을 뽑는다. 'risk_score'가 있으면 그것, 없으면 첫 output."""
    if "risk_score" in result:
        return result["risk_score"]
    if model.output_keys:
        return result.get(model.output_keys[0], 0.0)
    return 0.0


def infer_all_pairs(
    pkgs: list,
    metamodel_name: str = "thermal_shock_v0",
    *,
    min_overlap_ratio: float = 0.0,
    **kw,
) -> list:
    """겹치는 모든 상/하단 쌍을 추론해 위험 순위(내림차순)로 반환.

    ``summarize_packages``의 ``stacked_pairs``를 순회하며 ``overlap_ratio``가
    ``min_overlap_ratio`` 이상인 쌍만 추론한다. 각 결과는 JSON 직렬화 가능
    (refDes/수치/문자열/리스트만 포함).
    """
    summary = summarize_packages(pkgs)
    top_lookup = {p.ref_des: p for p in pkgs if p.side == "TOP"}
    bot_lookup = {p.ref_des: p for p in pkgs if p.side == "BOT"}
    model = get_metamodel(metamodel_name)

    results = []
    for pair in summary["stacked_pairs"]:
        if pair["overlap_ratio"] < min_overlap_ratio:
            continue
        top_pkg = top_lookup.get(pair["top_ref"])
        bot_pkg = bot_lookup.get(pair["bot_ref"])
        if top_pkg is None or bot_pkg is None:
            continue
        results.append(infer_pair(top_pkg, bot_pkg, metamodel_name, **kw))

    results.sort(key=lambda r: _risk_value(r, model), reverse=True)
    return results
