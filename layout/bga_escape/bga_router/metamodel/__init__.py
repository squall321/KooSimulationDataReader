# 메타모델 프레임워크 공개 API를 re-export하는 패키지 초기화 모듈.
from .base import Metamodel
from .registry import (
    register_metamodel,
    get_metamodel,
    list_metamodels,
)
from .feature_spec import FeatureVectorSpec

# import 부작용으로 구체 메타모델(thermal_shock_v0 등)을 registry에 등록한다.
from . import models  # noqa: F401,E402

__all__ = [
    "Metamodel",
    "register_metamodel",
    "get_metamodel",
    "list_metamodels",
    "FeatureVectorSpec",
]
