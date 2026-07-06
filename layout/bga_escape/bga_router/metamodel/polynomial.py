# 다항식 계수 기반 메타모델을 정의하는 모듈.
from __future__ import annotations

import json

from .base import Metamodel


class PolynomialMetamodel(Metamodel):
    """출력별 다항식 항들을 평가하는 메타모델.

    JSON 형식.
      {
        "name": ..., "version": ...,
        "input_features": [...], "output_keys": [...],
        "terms": {
          out_key: [{"coef": c, "powers": {feat: exp, ...}}, ...],
          ...
        }
      }
    각 output = sum_over_terms( coef * prod(feat ** exp) ).
    powers가 비어 있으면 상수항(coef)이다.
    """

    kind = "polynomial"

    def __init__(
        self,
        name: str,
        version: str,
        input_features: list,
        output_keys: list,
        terms: dict,
    ):
        self.name = name
        self.version = version
        self.input_features = list(input_features)
        self.output_keys = list(output_keys)
        self.terms = terms

    @classmethod
    def from_json(cls, path: str) -> "PolynomialMetamodel":
        with open(path, "r", encoding="utf-8") as f:
            data = json.load(f)
        return cls(
            name=data["name"],
            version=data["version"],
            input_features=data["input_features"],
            output_keys=data["output_keys"],
            terms=data["terms"],
        )

    def predict(self, features: dict) -> dict:
        result = {}
        for out_key in self.output_keys:
            total = 0.0
            for term in self.terms.get(out_key, []):
                value = float(term["coef"])
                for feat, exp in term.get("powers", {}).items():
                    value *= float(features[feat]) ** exp
                total += value
            result[out_key] = total

        result["confidence"] = 0.75
        result["provenance"] = f"polynomial:{self.name}@{self.version}"
        result["flags"] = []
        return result
