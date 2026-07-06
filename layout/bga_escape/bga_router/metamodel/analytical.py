# 순수 함수 기반 해석적(analytical) 메타모델 래퍼를 정의하는 모듈.
from __future__ import annotations

from .base import Metamodel


class AnalyticalMetamodel(Metamodel):
    """dict->dict 순수 함수를 감싸 Metamodel 인터페이스로 노출하는 래퍼.

    fn은 features(dict)를 받아 output_keys에 해당하는 출력 dict를 반환한다.
    predict()가 confidence/provenance/flags를 부착해 계약을 완성한다.
    """

    kind = "analytical"

    def __init__(
        self,
        name: str,
        version: str,
        input_features: list,
        output_keys: list,
        fn,
        confidence: float = 0.6,
    ):
        self.name = name
        self.version = version
        self.input_features = list(input_features)
        self.output_keys = list(output_keys)
        self._fn = fn
        self._confidence = float(confidence)

    def predict(self, features: dict) -> dict:
        outputs = self._fn(features)
        result = dict(outputs)
        result["confidence"] = float(self._confidence)
        result["provenance"] = f"reference_analytical:{self.name}@{self.version}"
        result["flags"] = list(result.get("flags", []))
        return result
