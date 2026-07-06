# 메타모델 추상 기반 클래스를 정의하는 모듈.
from abc import ABC, abstractmethod


class Metamodel(ABC):
    """예측 메타모델의 공통 인터페이스.

    서브클래스는 name/version/input_features/output_keys 속성과
    predict()를 구현한다.
    """

    name: str = ""
    version: str = ""
    input_features: list = []
    output_keys: list = []
    kind: str = "metamodel"

    @abstractmethod
    def predict(self, features: dict) -> dict:
        """features를 받아 예측 결과 dict를 반환.

        반환 dict는 각 output 값과 함께 다음 키를 포함해야 한다.
        - 'confidence': float 0~1
        - 'provenance': str
        - 'flags': list[str]
        """
        ...

    def describe(self) -> dict:
        """메타모델 메타데이터를 반환."""
        return {
            "name": self.name,
            "version": self.version,
            "input_features": list(self.input_features),
            "output_keys": list(self.output_keys),
            "kind": getattr(self, "kind", "metamodel"),
        }
