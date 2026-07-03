# 합성 3-pin net end-to-end 라우팅 + Stage 2 메트릭 활성화 검증
"""Phase H-10 follow-up (Stage 3 reality check).

Synthetic 3-pin daisy-chain net routed through the REAL pipeline:
CostGrid + extract_routing_tasks + run_recipe. Verifies:

  1. net_extractor decomposes 3-pin → 2 sub-tasks + extra_pins on first.
  2. run_recipe routes both segments (shared net_name → shared cells OK).
  3. Stage 2 metrics (branched_topology_check / pin_arrival_lengths)
     operate on the real merged routed path.
  4. Same-net cell sharing means no self-conflict.

This is the honest test of what the design doc called 'Stage 3' —
if this passes, chain-extension already works via decomposition and
the remaining gap is only Steiner-style trunk sharing (Stage 4).
"""

from __future__ import annotations

import pytest

from src.cost_grid import CostGrid
from src.cost_grid_coords import GridGeometry
from src.net_extractor import NetEndpoint, RoutingTask
from src.routing_spec import NetRule
from src.recipe import Recipe, run_recipe
from src.skills.registry import discover_skills

discover_skills()
from bga_router.metrics.multi_pin import (
    branched_topology_check,
    pin_arrival_lengths,
    summarize_multi_pin,
)


def _grid(nx=60, ny=40, cell=0.1):
    geom = GridGeometry(x_min_mm=0.0, y_min_mm=0.0,
                          x_max_mm=nx * cell, y_max_mm=ny * cell,
                          cell_mm=cell)
    return CostGrid(geom, ['L1'])


def _ep(ix, iy, net='BUS0', pin='1'):
    return NetEndpoint(net_name=net, ref_des='U1', pin_name=pin,
                        layer='L1', ix=ix, iy=iy)


def _rule():
    return NetRule(layers=('L1',), escape_sides=('N', 'S', 'E', 'W'),
                    width_mm=0.075, net_topology='daisy_chain')


def _recipe():
    return Recipe(
        path_solver='AStarOctile',
        negotiator='PathFinderNegotiator',
        cost_fn='StandardCost',
        skill_params={'PathFinderNegotiator':
                          {'max_iter': 50, 'h_increment': 3.0}},
    )


def _three_pin_tasks():
    """3-pin daisy chain — matches net_extractor's decomposition output:
    two 2-pin sub-tasks sharing net_name, first carries extra_pins."""
    pins = (_ep(5, 20, pin='1'), _ep(30, 20, pin='2'), _ep(55, 20, pin='3'))
    rule = _rule()
    t1 = RoutingTask(net_name='BUS0', source=pins[0], sink=pins[1],
                      rule=rule, extra_pins=pins)
    t2 = RoutingTask(net_name='BUS0', source=pins[1], sink=pins[2],
                      rule=rule)
    return [t1, t2], pins


def test_multi_pin_routes_end_to_end():
    grid = _grid()
    tasks, pins = _three_pin_tasks()
    result = run_recipe(grid, tasks, _recipe())
    # Both sub-tasks routed; routed_paths keyed by net_name so the
    # second segment's PathResult survives (documented Stage-1 behavior).
    assert result.n_failed == 0
    assert 'BUS0' in result.routed_paths


def test_stage2_arrival_on_merged_path():
    """Merge both sub-paths and confirm arrival analysis sees all 3 pins."""
    grid = _grid()
    tasks, pins = _three_pin_tasks()
    result = run_recipe(grid, tasks, _recipe())

    # routed_paths['BUS0'] holds the LAST sub-segment only. Stage-3
    # merged view: concatenate sub-paths.
    # run_recipe returns one PathResult per net (2nd overwrote 1st).
    # For a true merged analysis we re-run per-task through the
    # negotiator... instead here we emulate the merged path directly:
    from src.skills.protocols import PathResult
    merged_cells = []
    # Re-route each task individually against a fresh grid to keep the
    # test deterministic and collect both segments.
    for t in tasks:
        g2 = _grid()
        r2 = run_recipe(g2, [t], _recipe())
        pr = r2.routed_paths.get('BUS0')
        assert pr is not None, 'sub-task failed to route'
        merged_cells.extend(pr.path)
    merged = {'BUS0': PathResult(path=merged_cells, total_cost=0.0)}

    task1 = tasks[0]
    topo = branched_topology_check(task1, merged)
    assert topo is not None
    assert topo['pin_count'] == 3
    # daisy chain through 3 collinear pins → simple chain
    assert topo['matches'] is True, topo['reason']

    arr = pin_arrival_lengths(task1, merged, grid)
    assert arr is not None
    assert arr['unreached_pins'] == []
    # pin1 is between pin0 and pin2 → arrival monotone
    assert arr['arrival_mm']['pin1'] < arr['arrival_mm']['pin2']


def test_summarize_multi_pin_over_real_route():
    """Stage 3 완성 검증 — negotiator가 same-net sub-path를 merge하므로
    routed_paths['BUS0'] 하나에 3-pin 전체 경로가 들어있고, Stage 2
    메트릭이 그 위에서 바로 동작한다."""
    grid = _grid()
    tasks, pins = _three_pin_tasks()
    result = run_recipe(grid, tasks, _recipe())
    out = summarize_multi_pin(tasks, result.routed_paths, grid)
    assert out['multi_pin_net_count'] == 1
    assert 'BUS0' in out['topology']
    topo = out['topology']['BUS0']
    # Merged 3-pin daisy chain → simple chain over ALL pins.
    assert topo['pin_count'] == 3
    assert topo['matches'] is True, topo['reason']
    # Arrival lengths reach every pin from the driver.
    arr = out['arrival_lengths']['BUS0']
    assert arr['unreached_pins'] == []
    assert arr['skew_mm'] > 0
