# 추출된 피처를 메타모델 입력 벡터로 변환하는 스펙 정의.
from dataclasses import dataclass, field


@dataclass
class FeatureVectorSpec:
    """model 입력 키와 추출 피처 키의 매핑 및 검증 규칙.

    mapping: model_input_key -> extracted_feature_key
    ranges: key -> (min, max) 유효 범위
    defaults: key -> 누락 시 기본값
    """

    mapping: dict
    ranges: dict = field(default_factory=dict)
    defaults: dict = field(default_factory=dict)

    def build(self, extracted: dict) -> tuple:
        """extracted에서 mapping에 따라 피처를 추출해 (feature_dict, flags)를 반환.

        값이 없으면 defaults를 사용하고, defaults에도 없으면 ValueError.
        ranges를 벗어나면 flags에 f'{key}_out_of_range'를 추가하되 추론은 진행한다.
        """
        feature_dict = {}
        flags = []
        missing = []

        for input_key, extracted_key in self.mapping.items():
            if extracted_key in extracted:
                value = extracted[extracted_key]
            elif input_key in self.defaults:
                value = self.defaults[input_key]
            else:
                missing.append((input_key, extracted_key))
                continue
            feature_dict[input_key] = value

        if missing:
            details = ", ".join(
                f"{ik} (from '{ek}')" for ik, ek in missing
            )
            raise ValueError(f"missing required feature(s): {details}")

        for input_key, value in feature_dict.items():
            if input_key in self.ranges:
                lo, hi = self.ranges[input_key]
                if value < lo or value > hi:
                    flags.append(f"{input_key}_out_of_range")

        return feature_dict, flags
