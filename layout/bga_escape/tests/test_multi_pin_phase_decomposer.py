# MultiPinPhaseDecomposer 단위 테스트 — 2-pin first / multi-pin last 분리
"""Phase H-10 Stage 5 — multi-pin phase decomposer tests."""

from __future__ import annotations

import pytest

from src.cost_grid import CostGrid
from src.cost_grid_coords import GridGeometry
from src.net_extractor import NetEndpoint, RoutingTask
from src.routing_spec import NetRule
from src.recipe import Recipe, run_recipe
from src.skills.registry import REGISTRY, discover_skills

discover_skills()


def _grid(nx=80, ny=40, cell=0.1):
    geom = GridGeometry(x_min_mm=0.0, y_min_mm=0.0,
                          x_max_mm=nx * cell, y_max_mm=ny * cell,
                          cell_mm=cell)
    return CostGrid(geom, ['L1'])


def _ep(ix, iy, net, pin='1'):
    return NetEndpoint(net_name=net, ref_des='U1', pin_name=pin,
                        layer='L1', ix=ix, iy=iy)


def _rule(topology=None):
    return NetRule(layers=('L1',), escape_sides=('N', 'S', 'E', 'W'),
                    width_mm=0.075, net_topology=topology)


def _mixed_tasks():
    """2 two-pin nets + 1 three-pin (2 sub-tasks) net."""
    r2 = _rule()
    rs = _rule('star')
    a = RoutingTask('SIG_A', _ep(5, 5, 'SIG_A'), _ep(70, 5, 'SIG_A'), r2)
    b = RoutingTask('SIG_B', _ep(5, 35, 'SIG_B'), _ep(70, 35, 'SIG_B'), r2)
    pins = (_ep(5, 20, 'YNET'), _ep(70, 15, 'YNET'), _ep(70, 25, 'YNET'))
    y1 = RoutingTask('YNET', pins[0], pins[1], rs, extra_pins=pins)
    y2 = RoutingTask('YNET', pins[0], pins[2], rs)
    return [a, y1, b, y2]


def _decomposer():
    return REGISTRY.get('decomposer', 'MultiPinPhaseDecomposer')


def test_registered_in_registry():
    d = _decomposer()
    assert d.name == 'MultiPinPhaseDecomposer'


def test_splits_two_pin_and_multi_pin_phases():
    g = _grid()
    groups = _decomposer().decompose(_mixed_tasks(), g, config={})
    assert len(groups) == 2
    g0, g1 = groups
    assert g0.phase == 0 and g0.metadata['kind'] == 'two_pin'
    assert g1.phase == 1 and g1.metadata['kind'] == 'multi_pin'
    assert {t.net_name for t in g0.tasks} == {'SIG_A', 'SIG_B'}
    assert {t.net_name for t in g1.tasks} == {'YNET'}
    assert g1.metadata['nets'] == ['YNET']


def test_same_net_subtasks_stay_adjacent_in_order():
    g = _grid()
    groups = _decomposer().decompose(_mixed_tasks(), g, config={})
    multi = groups[1].tasks
    # Both YNET sub-tasks, original relative order preserved
    assert [t.net_name for t in multi] == ['YNET', 'YNET']
    assert multi[0].extra_pins is not None       # first sub-task first


def test_all_two_pin_single_group():
    g = _grid()
    r = _rule()
    tasks = [RoutingTask(f'N{i}', _ep(5, 5 + i, f'N{i}'),
                           _ep(70, 5 + i, f'N{i}'), r)
              for i in range(3)]
    groups = _decomposer().decompose(tasks, g, config={})
    assert len(groups) == 1
    assert groups[0].metadata['kind'] == 'two_pin'


def test_all_multi_pin_single_group_phase0():
    g = _grid()
    rs = _rule('star')
    pins = (_ep(5, 20, 'Y'), _ep(70, 15, 'Y'), _ep(70, 25, 'Y'))
    tasks = [RoutingTask('Y', pins[0], pins[1], rs, extra_pins=pins),
              RoutingTask('Y', pins[0], pins[2], rs)]
    groups = _decomposer().decompose(tasks, g, config={})
    assert len(groups) == 1
    assert groups[0].phase == 0                  # only group
    assert groups[0].metadata['kind'] == 'multi_pin'


def test_empty_input():
    g = _grid()
    assert _decomposer().decompose([], g, config={}) == []


def test_e2e_recipe_with_decomposer_routes_all():
    """Mixed 시나리오를 decomposer 포함 recipe로 end-to-end 라우팅."""
    recipe = Recipe(
        path_solver='AStarOctile',
        negotiator='PathFinderNegotiator',
        decomposers=('MultiPinPhaseDecomposer',),
        cost_fn='StandardCost',
        skill_params={'PathFinderNegotiator':
                          {'max_iter': 50, 'h_increment': 3.0}},
    )
    g = _grid()
    result = run_recipe(g, _mixed_tasks(), recipe)
    assert result.n_failed == 0
    assert set(result.routed_paths) == {'SIG_A', 'SIG_B', 'YNET'}
    # YNET merged path covers both spokes
    ynet = result.routed_paths['YNET']
    distinct = set(tuple(c) for c in ynet.path)
    assert len(ynet.path) > len(distinct)        # trunk shared (Stage 4)
