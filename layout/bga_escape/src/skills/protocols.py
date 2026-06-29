"""Phase 5 — Skill Protocols.

모든 라우팅 알고리즘이 따르는 표준 인터페이스. Protocol 기반 duck-typing
이라 inheritance 강제 없음. `@runtime_checkable` 로 isinstance check 가능.

병렬성은 1급 시민:
  - 각 skill은 `parallel_modes: tuple[str, ...]` 로 지원 모드 명시.
  - 'sequential' (필수), 'multi_thread', 'multi_process', 'mpi', 'gpu' 중 일부.
  - Runner가 mode를 요청, skill이 모르는 mode면 fallback 또는 raise.

새 skill 추가:
  1. src/skills/<category>/<name>.py 작성
  2. Protocol 구현
  3. @register_skill('<category>') 데코레이터로 자동 등록
  4. src/skills/__init__.py 또는 plugin loader에서 import만 하면 끝.

기존 모듈 (single_a_star, negotiation_router 등)은 wrap만 한다 — 그대로 둠.
"""

from __future__ import annotations

from dataclasses import dataclass, field
from typing import Any, Optional, Protocol, runtime_checkable


# ---------------------------------------------------------------------------
# 공통 데이터 형식
# ---------------------------------------------------------------------------


@dataclass
class RoutingRequest:
    """단일 net 라우팅 요청. PathSolver / InitialSolver 입력."""
    net_name: str
    source: tuple                # (layer:str, ix:int, iy:int)
    sink: tuple
    allowed_layers: tuple        # spec.layers (hard constraint)
    width_mm: float = 0.075
    iteration: int = 0           # PathFinder iter (cost 함수가 사용)
    # 자유 확장 (skill별 특수 정보)
    extra: dict = field(default_factory=dict)


@dataclass
class PathResult:
    """단일 net 라우팅 결과."""
    path: list                   # list[(layer:str, ix:int, iy:int)]
    total_cost: float
    expanded_cells: int = 0
    via_count: int = 0
    skill_name: str = ''         # 누가 풀었는지 진단용
    extra: dict = field(default_factory=dict)
    # Phase D-6 — via metadata per transition for return-path / stub /
    # standards analysis. Each entry: {
    #   'xy_mm':       (x, y),
    #   'start_layer': str,
    #   'end_layer':   str,
    #   'kind':        'signal' | 'power_ground' | 'stitch' | None,
    # }
    # Populated by the path solver; safe default = empty list.
    via_metadata: list = field(default_factory=list)
    # Phase E-3 — per-segment rendered width in mm. One entry per
    # same-layer segment; via transitions are not present. Router
    # currently treats width as a cost weight only and emits a uniform
    # rule.width_mm for every segment, but the schema is here for
    # future neck-down / variable-width support without ABI churn.
    segment_widths_mm: list = field(default_factory=list)


@dataclass
class NetGroup:
    """Decomposer 출력. 같은 phase의 group들은 병렬 가능."""
    group_id: str
    phase: int                   # 0이 가장 먼저
    tasks: list                  # list[RoutingTask] from net_extractor
    region_bbox: Optional[tuple] = None      # (x_min, y_min, x_max, y_max) mm — group이 다루는 영역
    metadata: dict = field(default_factory=dict)


@dataclass
class NegotiationOutcome:
    """Negotiator 출력 (한 group)."""
    routed: dict                 # net_name -> PathResult
    failed: dict                 # net_name -> reason string
    iterations: int
    residual_overuse: list
    converged: bool
    extra: dict = field(default_factory=dict)


@dataclass
class ParallelContext:
    """Skill 안에서 사용 가능한 병렬 자원 정보."""
    backend: str                 # 'serial' | 'multiprocessing' | 'mpi' | 'gpu'
    n_workers: int
    worker_pool: Any = None      # backend-specific pool object (Optional)
    extra: dict = field(default_factory=dict)


# ---------------------------------------------------------------------------
# Protocols
# ---------------------------------------------------------------------------


@runtime_checkable
class PathSolver(Protocol):
    """단일 net A* (또는 변형)."""
    name: str
    parallel_modes: tuple        # 예: ('sequential', 'multi_thread')

    def preprocess(self, grid) -> None:
        """선택적 전처리 (JPS+ jump table 등). 멱등."""
        ...

    def find_path(self, grid, request: RoutingRequest, *,
                   parallel_mode: str = 'sequential',
                   parallel_ctx: Optional[ParallelContext] = None
                   ) -> Optional[PathResult]:
        ...


@runtime_checkable
class Negotiator(Protocol):
    """다중 net 충돌 해소."""
    name: str
    parallel_modes: tuple

    def negotiate(self, grid, tasks: list, path_solver: PathSolver, *,
                   config: dict,
                   parallel_mode: str = 'sequential',
                   parallel_ctx: Optional[ParallelContext] = None
                   ) -> NegotiationOutcome:
        ...


@runtime_checkable
class Decomposer(Protocol):
    """Task / Space 분해."""
    name: str

    def decompose(self, tasks: list, grid, *, config: dict
                   ) -> list:
        """list[NetGroup] 반환. 같은 phase 그룹은 병렬 가능, 다른 phase는 순차."""
        ...


@runtime_checkable
class CostFn(Protocol):
    """Cell entry cost. PathSolver가 grid.cell_cost 대신 호출 가능.

    Phase 34a: optional ``request`` kwarg gives cost functions access to
    the active :class:`RoutingRequest` (priority, extras, net_ctx). Skills
    that don't care can absorb it via ``**ctx`` — adding the explicit
    kwarg is recommended but not required.
    """
    name: str

    def __call__(self, grid, layer: str, ix: int, iy: int, *,
                  iteration: int, prev_layer: Optional[str] = None,
                  request: Optional['RoutingRequest'] = None,
                  **ctx) -> float:
        ...


@runtime_checkable
class InitialSolver(Protocol):
    """초기해 후보 (없으면 PathSolver가 cold start)."""
    name: str

    def initial_path(self, grid, request: RoutingRequest
                      ) -> Optional[PathResult]:
        ...


@runtime_checkable
class PostProcessor(Protocol):
    """라우팅 결과 후처리 (smoothing / length match / regional rip-up 등)."""
    name: str

    def postprocess(self, grid, results: dict, *, config: dict
                     ) -> dict:
        """results: net_name -> PathResult. 수정된 dict 반환."""
        ...


@runtime_checkable
class Diagnostic(Protocol):
    """진단/측정 skill. Cost나 path에 영향 X, 정보만 추출."""
    name: str

    def diagnose(self, grid, results: dict, tasks: list) -> dict:
        ...


# ---------------------------------------------------------------------------
# Error types
# ---------------------------------------------------------------------------


class SkillError(Exception):
    """Skill 자체의 오류."""


class RecipeError(Exception):
    """Recipe 구성 오류 (등록 안 된 skill, parallel mode 불일치 등)."""
