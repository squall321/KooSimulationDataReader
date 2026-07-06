# 메타모델 인스턴스를 이름으로 등록/조회하는 레지스트리.
from .base import Metamodel

_REGISTRY: dict = {}


def register_metamodel(model: Metamodel) -> Metamodel:
    """메타모델을 이름으로 등록. 동일 이름이 있으면 덮어쓰기. model을 반환."""
    _REGISTRY[model.name] = model
    return model


def get_metamodel(name: str) -> Metamodel:
    """이름으로 메타모델을 조회. 없으면 알려진 목록을 포함한 KeyError."""
    if name not in _REGISTRY:
        known = sorted(_REGISTRY.keys())
        raise KeyError(
            f"metamodel '{name}' not found. known metamodels: {known}"
        )
    return _REGISTRY[name]


def list_metamodels() -> list:
    """등록된 모든 메타모델의 describe() 목록을 반환."""
    return [model.describe() for model in _REGISTRY.values()]
