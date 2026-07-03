"""Phase 4 Layer 2C — PathFinder Negotiation Loop.

PCB 라우팅의 본질적 알고리즘 (McMurchie & Ebeling, FPGA 1995). 휴리스틱이
아니라 수렴이 *증명된* 정식 방법:

  1. Initial pass: 모든 net이 충돌 무시하고 최단경로 → 일부 cell이 overused.
  2. Iteration loop:
     a. overused cell의 history_cost 누적 (영구 학습).
     b. ALL net rip-up + 재라우팅 (p_factor 증가로 충돌 회피 점점 강화).
     c. overused가 없으면 수렴 → 종료.
  3. max_iter 도달 시 unconverged 반환 (실패가 아니라 capacity 부족 신호 →
     Layer 4 진단으로 위임).

"왜 모든 net rip-up?"
  부분 rip-up은 부분 정보만으로 결정 → 순서 의존성 = 휴리스틱 = 꼼수.
  PathFinder의 수렴 보장은 전체 rip-up + 누적 history_cost가 핵심.

이 모듈은 PathFinder의 outer loop만 담당. Successor 생성, cost 계산,
A* 자체는 Layer 1/2A/2B의 책임.
"""

from __future__ import annotations

from dataclasses import dataclass, field
from typing import Iterable, Optional

from src.cost_grid import CostGrid
from src.net_extractor import RoutingTask
from src.single_a_star import AStarRequest, AStarResult, find_path


@dataclass
class NegotiationConfig:
    """PathFinder 튜닝 파라미터. CostGrid의 base/via/p/h factor와는 별개로
    iteration 횟수 / history increment만 여기서 제어."""
    max_iter: int = 50
    h_increment: float = 1.0


@dataclass
class TaskOutcome:
    """한 task의 최종 라우팅 결과 (수렴/미수렴 무관)."""
    task: RoutingTask
    result: Optional[AStarResult]   # None이면 어떤 iteration에서도 path를 못 찾음


@dataclass
class NegotiationResult:
    """PathFinder 종료 시점의 전체 상태.

    converged=True면 residual_overuse가 비어 있음 (PathFinder 정리).
    converged=False면 residual_overuse가 어디에 충돌이 남았는지를 알려줌
    (Layer 4 진단의 입력).
    """
    converged: bool
    iterations_used: int             # initial pass 포함 (initial = iter 0)
    routed: list                     # list[TaskOutcome] — result is not None
    failed: list                     # list[TaskOutcome] — result is None (path 없음)
    residual_overuse: list = field(default_factory=list)  # (layer, ix, iy)


def _route_one(grid: CostGrid, task: RoutingTask, iteration: int
               ) -> Optional[AStarResult]:
    """단일 net의 A* 호출. 차동쌍 special-case는 Layer 3에서 처리하므로
    여기서는 task.is_pair을 무시하고 single-net A*로만 푼다.

    PathFinder는 이 함수에 직접 의존하지 않고 `route_one_callback`을 통해
    호출할 수도 있게 만들었다 — Layer 3 coupled router를 끼워 넣을 때
    이 콜백만 교체하면 된다.
    """
    src = task.source
    snk = task.sink
    req = AStarRequest(
        net_name=task.net_name,
        source=(src.layer, src.ix, src.iy),
        sink=(snk.layer, snk.ix, snk.iy),
        rule=task.rule,
        iteration=iteration,
    )
    return find_path(grid, req)


# Path의 stamping/unstamping은 layer가 바뀌면 별도로 호출해야 한다 —
# 1C의 stamp_path/unstamp_path는 한 layer만 받기 때문. 다중 layer path를
# 분해해서 layer별로 묶어서 stamp 처리.
def _group_path_by_layer(path: list) -> list:
    """[(layer, ix, iy), ...] → [(layer, [(ix, iy), ...]), ...]
    연속된 같은 layer cell들을 묶는다. layer 전환점은 양쪽에 모두 포함시켜
    via cell이 양 layer 모두에 stamp되도록 한다."""
    if not path:
        return []
    grouped: list = []
    cur_layer = path[0][0]
    cur_cells: list = [(path[0][1], path[0][2])]
    for i in range(1, len(path)):
        lay, ix, iy = path[i]
        if lay == cur_layer:
            cur_cells.append((ix, iy))
        else:
            # via: 직전 layer의 마지막 cell은 cur_cells에 이미 있음.
            # 이 layer의 첫 cell도 일단 새 그룹의 첫 원소로 추가.
            grouped.append((cur_layer, cur_cells))
            cur_layer = lay
            cur_cells = [(ix, iy)]
    grouped.append((cur_layer, cur_cells))
    return grouped


def _stamp_full_path(grid: CostGrid, net_name: str, path: list) -> None:
    """다중 layer path 전체를 stamp."""
    for layer, cells in _group_path_by_layer(path):
        grid.stamp(layer, cells, net_name)


def _unstamp_full_path(grid: CostGrid, net_name: str, path: list) -> None:
    """_stamp_full_path의 역연산."""
    for layer, cells in _group_path_by_layer(path):
        grid.unstamp(layer, cells, net_name)


def pathfinder_route(
    grid: CostGrid,
    tasks: Iterable[RoutingTask],
    config: NegotiationConfig = NegotiationConfig(),
    *,
    route_one_callback=None,
) -> NegotiationResult:
    """PathFinder negotiation outer loop.

    Parameters
    ----------
    grid : 이미 blocker가 stamp된 CostGrid.
    tasks : 라우팅 작업 목록. 여러 net이 같은 cell을 원해도 OK
        (PathFinder가 negotiation으로 풀어냄).
    config : iteration 한계 / history 증가량.
    route_one_callback : 옵션. (grid, task, iteration) -> Optional[AStarResult].
        Layer 3 coupled router를 끼워 넣을 때 여기를 교체.
        기본은 single-net A*.

    Returns
    -------
    NegotiationResult — converged 여부, 각 task의 결과, residual overuse.
    """
    if route_one_callback is None:
        route_one_callback = _route_one

    tasks = list(tasks)

    # ----- Initial pass (iteration 0) -----
    # 모두에게 최단경로. 충돌은 다음 iteration이 풀게 둠.
    # Phase H-10 Stage 3 — key by task INDEX, not net_name: multi-pin
    # nets decompose into sub-tasks that share net_name, and a net_name
    # key silently dropped every earlier segment.
    outcomes: dict = {}            # task_index -> TaskOutcome
    for idx, task in enumerate(tasks):
        result = route_one_callback(grid, task, 0)
        outcomes[idx] = TaskOutcome(task=task, result=result)
        if result is not None:
            _stamp_full_path(grid, task.net_name, result.path)

    iterations_used = 0

    # ----- Negotiation iterations -----
    for it in range(1, config.max_iter + 1):
        overused = grid.overused_keys()
        if not overused:
            iterations_used = it - 1   # 직전 pass가 마지막 의미 있는 작업
            converged = True
            return _build_result(converged, iterations_used, outcomes,
                                  residual=[])

        # 영구 학습: 충돌 cell이 점점 비싸짐
        grid.bump_history(overused, config.h_increment)

        # 전체 rip-up + 재라우팅 (순서는 결정적이지만 history_cost가 결과를 지배)
        for idx, task in enumerate(tasks):
            outcome = outcomes[idx]
            if outcome.result is not None:
                _unstamp_full_path(grid, task.net_name, outcome.result.path)
                outcome.result = None  # 다시 그릴 때까지 path 없음

        for idx, task in enumerate(tasks):
            new = route_one_callback(grid, task, it)
            outcomes[idx].result = new
            if new is not None:
                _stamp_full_path(grid, task.net_name, new.path)

        iterations_used = it

    # max_iter 도달: 미수렴
    converged = False
    residual = grid.overused_keys()
    return _build_result(converged, iterations_used, outcomes, residual)


def _build_result(converged: bool, iterations_used: int,
                   outcomes: dict, residual: list) -> NegotiationResult:
    routed: list = []
    failed: list = []
    for oc in outcomes.values():
        (routed if oc.result is not None else failed).append(oc)
    return NegotiationResult(
        converged=converged,
        iterations_used=iterations_used,
        routed=routed,
        failed=failed,
        residual_overuse=residual,
    )
