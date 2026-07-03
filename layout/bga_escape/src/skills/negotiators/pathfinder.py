"""PathFinderNegotiator — v1의 negotiation_router를 Negotiator protocol로 wrap.

PathFinder negotiation은 v1의 핵심. 그대로 사용. PathSolver는 protocol
객체로 주입되므로 다양한 A* 변형과 조합 가능.
"""

from __future__ import annotations

from typing import Optional

from src.negotiation_router import NegotiationConfig, pathfinder_route
from src.single_a_star import AStarRequest, find_path as _v1_find_path
from src.skills.protocols import (
    NegotiationOutcome, ParallelContext, PathResult, PathSolver,
)
from src.skills.registry import register_skill
from src.skills.negotiators._rule_extra import build_extra


@register_skill('negotiator')
class PathFinderNegotiator:
    """McMurchie & Ebeling 1995. v1 baseline."""

    name = 'PathFinderNegotiator'
    parallel_modes = ('sequential',)

    def negotiate(self, grid, tasks: list, path_solver: PathSolver, *,
                   config: dict,
                   parallel_mode: str = 'sequential',
                   parallel_ctx: Optional[ParallelContext] = None
                   ) -> NegotiationOutcome:
        max_iter = config.get('max_iter', 50)
        h_increment = config.get('h_increment', 3.0)
        nego_config = NegotiationConfig(max_iter=max_iter,
                                          h_increment=h_increment)

        # PathSolver를 v1 callback으로 변환
        task_extras: dict = {}  # net_name → extra (for PathResult copy below)
        def _route_one(grid, task, iteration):
            from src.skills.protocols import RoutingRequest
            extra = build_extra(task)
            task_extras[task.net_name] = extra
            req = RoutingRequest(
                net_name=task.net_name,
                source=(task.source.layer, task.source.ix, task.source.iy),
                sink=(task.sink.layer, task.sink.ix, task.sink.iy),
                allowed_layers=task.rule.layers,
                width_mm=task.rule.width_mm,
                iteration=iteration,
                extra=extra,
            )
            res = path_solver.find_path(grid, req,
                                          parallel_mode=parallel_mode,
                                          parallel_ctx=parallel_ctx)
            if res is None:
                return None
            # v1 NegotiationRouter expects AStarResult-shape; PathResult
            # 와 path 필드는 호환. extend with via_count/total_cost.
            class _Adapter:
                pass
            a = _Adapter()
            a.path = res.path
            a.total_cost = res.total_cost
            a.expanded_cells = res.expanded_cells
            a.via_count = res.via_count
            return a

        result = pathfinder_route(grid, tasks, nego_config,
                                    route_one_callback=_route_one)

        routed = {}
        failed = {}
        for oc in result.routed:
            net = oc.task.net_name
            # Phase 34 fix: surface per-net rule extras on PathResult
            pr_extra = {k: v for k, v in task_extras.get(net, {}).items()
                          if v is not None and v != () and v is not False}
            existing = routed.get(net)
            if existing is not None:
                # Phase H-10 Stage 3 — multi-pin sub-tasks share net_name.
                # Merge sub-paths instead of overwriting so downstream
                # metrics (branched topology / arrival lengths / coverage)
                # see the whole net, not just the last segment.
                existing.path = list(existing.path) + list(oc.result.path)
                existing.total_cost += oc.result.total_cost
                existing.expanded_cells += oc.result.expanded_cells
                existing.via_count += oc.result.via_count
                continue
            routed[net] = PathResult(
                path=oc.result.path,
                total_cost=oc.result.total_cost,
                expanded_cells=oc.result.expanded_cells,
                via_count=oc.result.via_count,
                skill_name=f'{self.name}+{path_solver.name}',
                extra=pr_extra,
            )
        for oc in result.failed:
            failed[oc.task.net_name] = 'no path found'

        return NegotiationOutcome(
            routed=routed, failed=failed,
            iterations=result.iterations_used,
            residual_overuse=result.residual_overuse,
            converged=result.converged,
        )
