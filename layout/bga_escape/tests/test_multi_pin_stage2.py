# Multi-pin Stage 2 단위 테스트 — branched topology + arrival length skew
"""Phase H-10 — multi-pin Stage 2 tests."""

from __future__ import annotations

from dataclasses import dataclass
from typing import Optional, Tuple

import pytest

from bga_router.metrics.multi_pin import (
    branched_topology_check,
    pin_arrival_lengths,
    summarize_multi_pin,
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
class _EP:
    layer: str
    ix: int
    iy: int


@dataclass
class _Rule:
    net_topology: Optional[str] = None


@dataclass
class _Task:
    net_name: str
    source: _EP
    sink: _EP
    rule: _Rule
    extra_pins: Optional[Tuple[_EP, ...]] = None


@dataclass
class _PR:
    path: list


def _chain_task(topology='daisy_chain'):
    """3-pin chain: (0,0) → (5,0) → (10,0)."""
    pins = (_EP('L', 0, 0), _EP('L', 5, 0), _EP('L', 10, 0))
    task = _Task('N', pins[0], pins[1], _Rule(net_topology=topology),
                  extra_pins=pins)
    path = [('L', i, 0) for i in range(11)]
    return task, {'N': _PR(path)}


def _star_task():
    """Hub at (5,0); spokes to (0,0), (10,0), (5,5)."""
    pins = (_EP('L', 5, 0), _EP('L', 0, 0), _EP('L', 10, 0), _EP('L', 5, 5))
    task = _Task('N', pins[0], pins[1], _Rule(net_topology='star'),
                  extra_pins=pins)
    # Star path — walk out and back to hub three times.
    path = ([('L', i, 0) for i in range(5, -1, -1)]      # 5→0
             + [('L', i, 0) for i in range(1, 11)]         # back out to 10
             + [('L', i, 0) for i in range(9, 4, -1)]      # return to 5
             + [('L', 5, j) for j in range(1, 6)])          # up to (5,5)
    return task, {'N': _PR(path)}


# ---------------------------------------------------------------------------
# branched_topology_check
# ---------------------------------------------------------------------------


def test_topology_none_without_extra_pins():
    t = _Task('N', _EP('L', 0, 0), _EP('L', 1, 0), _Rule())
    assert branched_topology_check(t, {}) is None


def test_daisy_chain_matches():
    task, routed = _chain_task('daisy_chain')
    r = branched_topology_check(task, routed)
    assert r['matches'] is True
    assert r['pin_count'] == 3
    assert r['leaves'] == 2
    assert r['hubs'] == 0


def test_star_declared_on_chain_fails():
    task, routed = _chain_task('star')
    r = branched_topology_check(task, routed)
    assert r['matches'] is False
    assert 'hub' in r['reason']


def test_star_with_real_hub_matches():
    task, routed = _star_task()
    r = branched_topology_check(task, routed)
    assert r['hubs'] == 1
    assert r['matches'] is True


def test_no_routed_path_reports_reason():
    task, _ = _chain_task()
    r = branched_topology_check(task, {})
    assert r['matches'] is False
    assert r['reason'] == 'no routed path'


# ---------------------------------------------------------------------------
# pin_arrival_lengths
# ---------------------------------------------------------------------------


def test_arrival_lengths_chain():
    g = _FakeGrid(cell_mm=0.1)
    task, routed = _chain_task()
    r = pin_arrival_lengths(task, routed, g)
    assert r['arrival_mm']['pin0'] == pytest.approx(0.0)
    assert r['arrival_mm']['pin1'] == pytest.approx(0.5, abs=1e-9)
    assert r['arrival_mm']['pin2'] == pytest.approx(1.0, abs=1e-9)
    # skew = max - min over non-driver pins = 1.0 - 0.5
    assert r['skew_mm'] == pytest.approx(0.5, abs=1e-9)
    assert r['unreached_pins'] == []


def test_arrival_unreached_pin_flagged():
    g = _FakeGrid()
    pins = (_EP('L', 0, 0), _EP('L', 5, 0), _EP('L', 99, 99))
    task = _Task('N', pins[0], pins[1], _Rule(), extra_pins=pins)
    routed = {'N': _PR([('L', i, 0) for i in range(6)])}
    r = pin_arrival_lengths(task, routed, g)
    assert 'pin2' in r['unreached_pins']


def test_arrival_driver_off_graph_reports_error():
    g = _FakeGrid()
    pins = (_EP('L', 50, 50), _EP('L', 0, 0))
    task = _Task('N', pins[0], pins[1], _Rule(), extra_pins=pins)
    routed = {'N': _PR([('L', 0, 0), ('L', 1, 0)])}
    r = pin_arrival_lengths(task, routed, g)
    assert 'error' in r


# ---------------------------------------------------------------------------
# summarize_multi_pin
# ---------------------------------------------------------------------------


def test_summarize_aggregates_and_flags_mismatches():
    g = _FakeGrid()
    good_task, good_routed = _chain_task('daisy_chain')
    bad_task, bad_routed = _chain_task('star')
    bad_task = _Task('M', bad_task.source, bad_task.sink,
                       _Rule(net_topology='star'),
                       extra_pins=bad_task.extra_pins)
    routed = {**good_routed, 'M': bad_routed['N']}
    out = summarize_multi_pin([good_task, bad_task], routed, g)
    assert out['multi_pin_net_count'] == 2
    assert out['topology_mismatches'] == ['M']
    assert 'N' in out['arrival_lengths']


def test_summarize_skips_2pin_tasks():
    g = _FakeGrid()
    t = _Task('X', _EP('L', 0, 0), _EP('L', 1, 0), _Rule())
    out = summarize_multi_pin([t], {}, g)
    assert out['multi_pin_net_count'] == 0
