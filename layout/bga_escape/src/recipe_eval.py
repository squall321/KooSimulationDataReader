"""Recipe evaluation adapter — Recipe + EDA → metrics.

Recipe DOE의 eval_callback으로 사용. Picklable (multiprocessing 친화).
"""

from __future__ import annotations

import math
import time
from dataclasses import dataclass, field
from typing import Optional

from src.cost_grid import CostGrid, compute_board_geometry
from src.eda_parser import EdaData
from src.net_extractor import extract_routing_tasks
from src.recipe import Recipe, run_recipe
from src.routing_spec import RoutingSpec
from src.skills.registry import discover_skills


# Discover once at import; safe (REGISTRY는 idempotent)
discover_skills()


@dataclass
class ScenarioContext:
    """라우팅 시나리오 정의 — Recipe DOE에 넘기는 입력."""
    eda: EdaData
    spec: RoutingSpec
    pin_to_net: dict
    signal_layers: list
    components_to_route: list = field(default_factory=list)
    cell_mm: float = 0.1
    margin_mm: float = 0.5
    pad_clearance_mm: float = 0.03
    endpoint_unblock_radius_cells: int = 2


def _bboxes(eda, components):
    out = []
    for c in components:
        p = eda.packages.get(c.pkg_name)
        if p is None:
            continue
        hw = max(abs(p.bbox_min_x_mm), abs(p.bbox_max_x_mm))
        hh = max(abs(p.bbox_min_y_mm), abs(p.bbox_max_y_mm))
        out.append((c.x_mm - hw, c.y_mm - hh, c.x_mm + hw, c.y_mm + hh))
    return out


def _unblock_endpoints(grid, tasks, radius: int) -> int:
    n = 0
    for t in tasks:
        for ep in (t.source, t.sink):
            for dx in range(-radius, radius + 1):
                for dy in range(-radius, radius + 1):
                    if grid.unblock_cell(ep.layer, ep.ix + dx, ep.iy + dy):
                        n += 1
    return n


def evaluate_recipe(
    recipe: Recipe,
    context: ScenarioContext,
    *,
    time_budget_s: Optional[float] = None,
    return_internals: bool = False,
) -> dict:
    """단일 (recipe, scenario) 평가 → metrics dict.

    Returns
    -------
    dict:
        routed_count, total_nets, routed_ratio,
        residual_overuse, wall_time_s, total_via_count,
        iterations_total, converged, failed_count, error (if any).

    Raises 안 함 — 오류 발생 시 metrics dict의 error 키에 정보.

    When ``return_internals=True``, an additional ``_internals`` key
    carries ``{'grid', 'routed_paths', 'tasks'}`` for downstream verifier
    use. NOT picklable — keep False for multiprocessing callers
    (container_runner / self_improving_loop). True only from the
    single-process eval driver (``bga_router/eval.py``).
    """
    t0 = time.time()

    class _TO(Exception):
        """Timeout sentinel — raised by SIGALRM handler."""

    try:
        components = (context.components_to_route
                       if context.components_to_route else list(context.eda.components))
        bboxes = _bboxes(context.eda, components)
        if not bboxes:
            return {'error': 'no components', 'wall_time_s': 0.0, 'timeout': False}
        geom = compute_board_geometry(
            bboxes, cell_mm=context.cell_mm, margin_mm=context.margin_mm)
        grid = CostGrid(geom, context.signal_layers)
        grid.add_pad_blockers(components, context.eda.packages,
                               pad_clearance_mm=context.pad_clearance_mm)
        tasks = extract_routing_tasks(
            context.eda, context.spec, grid, context.pin_to_net)
        _unblock_endpoints(grid, tasks,
                            radius=context.endpoint_unblock_radius_cells)

        # time_budget는 SIGALRM으로
        if time_budget_s is not None:
            import signal
            def _h(s, f): raise _TO()
            signal.signal(signal.SIGALRM, _h)
            signal.alarm(int(math.ceil(time_budget_s)))

        try:
            result = run_recipe(grid, tasks, recipe)
        finally:
            if time_budget_s is not None:
                signal.alarm(0)

        total_nets = len(tasks)
        routed = result.n_routed
        failed = result.n_failed
        residual = len(result.residual_overuse)
        # via 누적
        via = 0
        for path_result in result.routed_paths.values():
            via += getattr(path_result, 'via_count', 0)

        out = {
            'routed_count': routed,
            'total_nets': total_nets,
            'routed_ratio': routed / max(1, total_nets),
            'failed_count': failed,
            'residual_overuse': residual,
            'iterations_total': result.iterations_total,
            'wall_time_s': time.time() - t0,
            'total_via_count': via,
            'converged': result.converged,
            'timeout': False,
            'recipe_str': _recipe_str(recipe),
        }
        if return_internals:
            out['_internals'] = {
                'grid': grid,
                'routed_paths': result.routed_paths,
                'tasks': tasks,
                'spec': context.spec,
                'eda': context.eda,
            }
        return out
    except _TO:
        return {
            'error': f'timeout after {time_budget_s}s',
            'wall_time_s': time.time() - t0,
            'timeout': True,
            'recipe_str': _recipe_str(recipe),
        }
    except Exception as e:
        return {
            'error': f'{type(e).__name__}: {str(e)[:200]}',
            'wall_time_s': time.time() - t0,
            'timeout': False,
            'recipe_str': _recipe_str(recipe),
        }


def _recipe_str(r: Recipe) -> str:
    d = '+'.join(r.decomposers) if r.decomposers else 'noDec'
    i = '+'.join(r.initial_solvers) if r.initial_solvers else 'noInit'
    return f'{r.path_solver}|{r.negotiator}|{d}|{r.cost_fn}|{i}'
