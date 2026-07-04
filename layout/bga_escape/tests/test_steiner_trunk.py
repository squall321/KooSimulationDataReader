# Stage 4 Steiner-lite 단위 테스트 — own-net trunk 재사용 discount 검증
"""Phase H-10 Stage 4 — trunk reuse tests.

Y-net (star 분해): center pin 왼쪽, 두 load가 오른쪽 위/아래로 갈라짐.
두 spoke가 초반 구간을 공유할 수 있는 기하. trunk_reuse_factor<1.0이면
두 번째 spoke가 첫 spoke의 trunk를 타고 가다 갈라지고, 1.0 (off)이면
독립 최단경로 → 공유 cell 수로 검증.
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


def _grid(nx=80, ny=40, cell=0.1, trunk_reuse=0.25):
    geom = GridGeometry(x_min_mm=0.0, y_min_mm=0.0,
                          x_max_mm=nx * cell, y_max_mm=ny * cell,
                          cell_mm=cell)
    g = CostGrid(geom, ['L1'])
    g.trunk_reuse_factor = trunk_reuse
    return g


def _ep(ix, iy, net='YNET', pin='1'):
    return NetEndpoint(net_name=net, ref_des='U1', pin_name=pin,
                        layer='L1', ix=ix, iy=iy)


def _rule():
    return NetRule(layers=('L1',), escape_sides=('N', 'S', 'E', 'W'),
                    width_mm=0.075, net_topology='star')


def _recipe():
    return Recipe(
        path_solver='AStarOctile',
        negotiator='PathFinderNegotiator',
        cost_fn='StandardCost',
        skill_params={'PathFinderNegotiator':
                          {'max_iter': 50, 'h_increment': 3.0}},
    )


def _y_net_tasks():
    """Star: center (5,20) → loadA (70,15), center → loadB (70,25).
    두 spoke가 +x 방향으로 거의 평행 — trunk 공유 여지가 큼."""
    center = _ep(5, 20, pin='1')
    load_a = _ep(70, 15, pin='2')
    load_b = _ep(70, 25, pin='3')
    pins = (center, load_a, load_b)
    rule = _rule()
    t1 = RoutingTask(net_name='YNET', source=center, sink=load_a,
                      rule=rule, extra_pins=pins)
    t2 = RoutingTask(net_name='YNET', source=center, sink=load_b,
                      rule=rule)
    return [t1, t2]


def _distinct_cells(result):
    pr = result.routed_paths['YNET']
    return set(tuple(c) for c in pr.path)


def test_cell_cost_discount_when_own_net_occupies():
    """단위: own-net 점유 cell은 base*factor, foreign은 full 페널티."""
    from src.skills.protocols import RoutingRequest
    g = _grid(trunk_reuse=0.25)
    # Own-net stamp
    g.stamp('L1', [(10, 10)], 'YNET')
    req = RoutingRequest(net_name='YNET', source=('L1', 0, 0),
                           sink=('L1', 20, 20),
                           allowed_layers=('L1',))
    own_cost = g.cell_cost('L1', 10, 10, iteration=0, request=req)
    empty_cost = g.cell_cost('L1', 11, 10, iteration=0, request=req)
    # Own-net trunk cell must be cheaper than fresh copper.
    assert own_cost < empty_cost
    assert own_cost == pytest.approx(g.base_cost * 0.25, abs=1e-9)


def test_cell_cost_no_discount_for_foreign_net():
    from src.skills.protocols import RoutingRequest
    g = _grid(trunk_reuse=0.25)
    g.stamp('L1', [(10, 10)], 'OTHER')
    req = RoutingRequest(net_name='YNET', source=('L1', 0, 0),
                           sink=('L1', 20, 20),
                           allowed_layers=('L1',))
    foreign_cost = g.cell_cost('L1', 10, 10, iteration=0, request=req)
    empty_cost = g.cell_cost('L1', 11, 10, iteration=0, request=req)
    # Foreign occupancy costs MORE (present penalty), never less.
    assert foreign_cost > empty_cost


def test_cell_cost_factor_1_disables_discount():
    from src.skills.protocols import RoutingRequest
    g = _grid(trunk_reuse=1.0)
    g.stamp('L1', [(10, 10)], 'YNET')
    req = RoutingRequest(net_name='YNET', source=('L1', 0, 0),
                           sink=('L1', 20, 20),
                           allowed_layers=('L1',))
    own_cost = g.cell_cost('L1', 10, 10, iteration=0, request=req)
    empty_cost = g.cell_cost('L1', 11, 10, iteration=0, request=req)
    # factor=1.0 → own cell same as empty (own-present still excluded)
    assert own_cost == pytest.approx(empty_cost, abs=1e-9)


def test_y_net_trunk_sharing_reduces_copper():
    """E2E: discount ON이 OFF보다 distinct cell 수가 적거나 같아야 하고,
    공유 trunk가 실제로 생겨야 한다 (ON에서 겹침 > 0)."""
    tasks_on = _y_net_tasks()
    res_on = run_recipe(_grid(trunk_reuse=0.25), tasks_on, _recipe())
    assert res_on.n_failed == 0
    cells_on = _distinct_cells(res_on)

    tasks_off = _y_net_tasks()
    res_off = run_recipe(_grid(trunk_reuse=1.0), tasks_off, _recipe())
    assert res_off.n_failed == 0
    cells_off = _distinct_cells(res_off)

    # Total path entries (with junction dupes) vs distinct cells —
    # shared trunk shows as (entries - distinct) > 0.
    pr_on = res_on.routed_paths['YNET']
    shared_on = len(pr_on.path) - len(cells_on)

    # Steiner-lite must not INCREASE copper.
    assert len(cells_on) <= len(cells_off)
    # And with this Y geometry the trunk must actually be shared.
    assert shared_on > 0, (
        f'expected shared trunk cells with discount on; '
        f'path={len(pr_on.path)} distinct={len(cells_on)}')


def test_two_pin_net_unaffected_by_discount():
    """2-pin net은 자기 cell 재방문이 없으므로 factor와 무관하게 동일 경로."""
    def _two_pin(g):
        r = NetRule(layers=('L1',), escape_sides=('N', 'S', 'E', 'W'),
                     width_mm=0.075)
        t = RoutingTask(net_name='P2', source=_ep(5, 20, net='P2'),
                         sink=_ep(70, 20, net='P2'), rule=r)
        return run_recipe(g, [t], _recipe())

    res_a = _two_pin(_grid(trunk_reuse=0.25))
    res_b = _two_pin(_grid(trunk_reuse=1.0))
    assert res_a.routed_paths['P2'].path == res_b.routed_paths['P2'].path
