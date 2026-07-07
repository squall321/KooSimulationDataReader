# 구체 메타모델 인스턴스들을 담는 패키지 초기화 모듈.
# import 부작용으로 각 구체 모델을 registry에 등록한다.
from . import thermal_shock_v0  # noqa: F401
from . import drop_shock_v0  # noqa: F401
