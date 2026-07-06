# 단일 입력축 기반 룩업 테이블 선형보간 메타모델을 정의하는 모듈.
from __future__ import annotations

import json

from .base import Metamodel


class TableMetamodel(Metamodel):
    """단일 입력축(grid_axis)에 대해 샘플 포인트를 선형보간하는 메타모델.

    JSON 형식.
      {
        "name": ..., "version": ...,
        "input_features": [...], "output_keys": [...],
        "grid_axis": "<단일 입력축 키>",
        "points": [{"x": <값>, "outputs": {out_key: val, ...}}, ...]
      }
    범위 밖 입력은 양 끝 포인트로 클램프한다.
    """

    kind = "table"

    def __init__(
        self,
        name: str,
        version: str,
        input_features: list,
        output_keys: list,
        grid_axis: str,
        points: list,
    ):
        self.name = name
        self.version = version
        self.input_features = list(input_features)
        self.output_keys = list(output_keys)
        self.grid_axis = grid_axis
        # x 기준 오름차순 정렬.
        self.points = sorted(points, key=lambda p: p["x"])
        if not self.points:
            raise ValueError("TableMetamodel requires at least one point")

    @classmethod
    def from_json(cls, path: str) -> "TableMetamodel":
        with open(path, "r", encoding="utf-8") as f:
            data = json.load(f)
        return cls(
            name=data["name"],
            version=data["version"],
            input_features=data["input_features"],
            output_keys=data["output_keys"],
            grid_axis=data["grid_axis"],
            points=data["points"],
        )

    def predict(self, features: dict) -> dict:
        x = float(features[self.grid_axis])
        pts = self.points
        flags = []

        lo = pts[0]
        hi = pts[-1]
        if x <= lo["x"]:
            if x < lo["x"]:
                flags.append(f"{self.grid_axis}_clamped_low")
            outputs = dict(lo["outputs"])
        elif x >= hi["x"]:
            if x > hi["x"]:
                flags.append(f"{self.grid_axis}_clamped_high")
            outputs = dict(hi["outputs"])
        else:
            # x를 감싸는 두 포인트를 찾아 선형보간.
            outputs = {}
            for i in range(len(pts) - 1):
                a = pts[i]
                b = pts[i + 1]
                if a["x"] <= x <= b["x"]:
                    span = b["x"] - a["x"]
                    t = 0.0 if span == 0 else (x - a["x"]) / span
                    for key in self.output_keys:
                        va = a["outputs"][key]
                        vb = b["outputs"][key]
                        outputs[key] = va + t * (vb - va)
                    break

        result = dict(outputs)
        result["confidence"] = 0.7
        result["provenance"] = f"table:{self.name}@{self.version}"
        result["flags"] = flags
        return result
