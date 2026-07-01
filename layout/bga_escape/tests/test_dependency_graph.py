# dependency_graph 단위 테스트 — driver/load + clock_domain + bus_group fallback
"""Phase F-3 — dependency graph tests."""

from __future__ import annotations

from dataclasses import dataclass
from typing import Optional, Tuple

import pytest

from bga_router.metrics.dependency_graph import (
    build_dependency_groups,
    build_edges,
    compute_load_lengths,
    summarize_dependencies,
)


class _FakeGeom:
    def __init__(self, cell_mm: float = 0.1):
        self.cell_mm = cell_mm

    def cell_to_world(self, ix: int, iy: int) -> Tuple[float, float]:
        return ix * self.cell_mm, iy * self.cell_mm


class _FakeGrid:
    def __init__(self, cell_mm: float = 0.1):
        self.geom = _FakeGeom(cell_mm)


@dataclass
class _Rule:
    driver_pin: Optional[str] = None
    load_pins: tuple = ()
    clock_domain: Optional[str] = None
    pair_partner_net: Optional[str] = None


@dataclass
class _PR:
    path: list


# ---------------------------------------------------------------------------
# build_dependency_groups
# ---------------------------------------------------------------------------


def test_clock_domain_cluster():
    rules = {
        'CLK0': _Rule(clock_domain='CLK0'),
        'DQ0':  _Rule(clock_domain='CLK0'),
        'DQ1':  _Rule(clock_domain='CLK0'),
    }
    groups = build_dependency_groups(['CLK0', 'DQ0', 'DQ1'], rules)
    assert len(groups) == 1
    g = groups[0]
    assert g.label == 'CLK0'
    assert g.driver == 'CLK0'
    assert set(g.loads) == {'DQ0', 'DQ1'}


def test_bus_fallback_when_no_clock_domain():
    rules = {n: _Rule() for n in ['BUS0', 'BUS1', 'BUS2', 'BUS3']}
    groups = build_dependency_groups(['BUS0', 'BUS1', 'BUS2', 'BUS3'], rules)
    assert len(groups) == 1
    g = groups[0]
    assert g.label == 'bus:BUS'
    assert g.driver is None
    assert len(g.loads) == 4


def test_no_grouping_for_solo_nets():
    rules = {'A': _Rule(), 'B': _Rule()}
    groups = build_dependency_groups(['A', 'B'], rules)
    # 2 members not enough for bus cluster (min=3 in this builder)
    assert groups == []


def test_driver_pin_only_creates_group():
    rules = {'CTRL': _Rule(driver_pin='U200.X1')}
    groups = build_dependency_groups(['CTRL'], rules)
    assert any(g.label == 'net:CTRL' and g.driver == 'CTRL' for g in groups)


# ---------------------------------------------------------------------------
# build_edges
# ---------------------------------------------------------------------------


def test_build_edges_clock_domain_relation():
    rules = {
        'CLK0': _Rule(clock_domain='CLK0'),
        'DQ0':  _Rule(clock_domain='CLK0'),
        'DQ1':  _Rule(clock_domain='CLK0'),
    }
    groups = build_dependency_groups(['CLK0', 'DQ0', 'DQ1'], rules)
    edges = build_edges(groups)
    assert len(edges) == 2
    assert all(e.driver_net == 'CLK0' for e in edges)
    assert all(e.relation == 'clock_domain' for e in edges)


def test_build_edges_bus_group_uses_anchor():
    rules = {n: _Rule() for n in ['BUS0', 'BUS1', 'BUS2']}
    groups = build_dependency_groups(['BUS0', 'BUS1', 'BUS2'], rules)
    edges = build_edges(groups)
    # No declared driver — synthetic anchor = first member
    drivers = {e.driver_net for e in edges}
    assert drivers == {'BUS0'}
    assert all(e.relation == 'bus_group' for e in edges)


# ---------------------------------------------------------------------------
# compute_load_lengths
# ---------------------------------------------------------------------------


def test_compute_load_lengths_basic():
    g = _FakeGrid(cell_mm=0.1)
    routed = {
        'CLK0': _PR([('M1', 0, 0), ('M1', 50, 0)]),  # 5 mm
        'DQ0':  _PR([('M1', 0, 0), ('M1', 100, 0)]), # 10 mm
        'DQ1':  _PR([('M1', 0, 0), ('M1', 80, 0)]),  # 8 mm
    }
    rules = {
        'CLK0': _Rule(clock_domain='CLK0'),
        'DQ0':  _Rule(clock_domain='CLK0'),
        'DQ1':  _Rule(clock_domain='CLK0'),
    }
    groups = build_dependency_groups(list(routed), rules)
    skews = compute_load_lengths(routed, g, groups)
    assert 'CLK0' in skews
    s = skews['CLK0']
    assert s['driver'] == 'CLK0'
    assert s['member_count'] == 3
    assert s['skew_mm'] == pytest.approx(5.0, abs=1e-9)


def test_summarize_dependencies_end_to_end():
    g = _FakeGrid()
    routed = {
        'BUS0': _PR([('M1', 0, 0), ('M1', 50, 0)]),
        'BUS1': _PR([('M1', 0, 0), ('M1', 60, 0)]),
        'BUS2': _PR([('M1', 0, 0), ('M1', 70, 0)]),
    }
    rules = {n: _Rule() for n in routed}
    out = summarize_dependencies(list(routed), rules, routed, g)
    assert 'groups' in out
    assert 'edges' in out
    assert 'load_length_skew' in out
    bus_label = 'bus:BUS'
    assert bus_label in out['load_length_skew']
