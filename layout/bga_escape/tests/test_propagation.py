# propagation delay (ps) 메트릭 단위 테스트 — per-net 지연 / diff-pair / bus skew
"""Phase I-5 — propagation delay tests."""

from __future__ import annotations

from dataclasses import dataclass
from typing import Optional, Tuple

import pytest

from bga_router.metrics.propagation import (
    net_delay_ps,
    summarize_propagation,
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
    pair_partner_net: Optional[str] = None


@dataclass
class _PR:
    path: list


def test_net_delay_positive_for_real_path():
    g = _FakeGrid(cell_mm=0.1)
    path = [('COMP', 0, 0), ('COMP', 100, 0)]   # 10 mm
    d = net_delay_ps(path, g)
    # ~5.6 ps/mm × 10 mm ≈ 56 ps (default εr_eff)
    assert 40.0 < d < 80.0


def test_net_delay_zero_for_single_point():
    g = _FakeGrid()
    assert net_delay_ps([('COMP', 5, 5)], g) == 0.0


def test_net_delay_via_contributes_zero():
    g = _FakeGrid(cell_mm=0.1)
    straight = [('COMP', 0, 0), ('COMP', 100, 0)]
    with_via = [('COMP', 0, 0), ('COMP', 100, 0),
                 ('LAY2', 100, 0)]     # via at end, no extra length
    assert net_delay_ps(straight, g) == pytest.approx(
        net_delay_ps(with_via, g), abs=1e-6)


def test_summarize_per_net_delay():
    g = _FakeGrid(cell_mm=0.1)
    routed = {
        'A': _PR([('COMP', 0, 0), ('COMP', 100, 0)]),
        'B': _PR([('COMP', 0, 5), ('COMP', 50, 5)]),
    }
    rules = {'A': _Rule(), 'B': _Rule()}
    out = summarize_propagation(routed, g, rules)
    assert set(out['delay_ps']) == {'A', 'B'}
    # A is longer → larger delay
    assert out['delay_ps']['A'] > out['delay_ps']['B']


def test_diff_pair_skew_ps():
    g = _FakeGrid(cell_mm=0.1)
    routed = {
        'DQ_P': _PR([('COMP', 0, 0), ('COMP', 100, 0)]),   # 10 mm
        'DQ_N': _PR([('COMP', 0, 2), ('COMP', 80, 2)]),    # 8 mm
    }
    rules = {
        'DQ_P': _Rule(pair_partner_net='DQ_N'),
        'DQ_N': _Rule(pair_partner_net='DQ_P'),
    }
    out = summarize_propagation(routed, g, rules)
    label = 'DQ_N|DQ_P'
    assert label in out['diff_pair_skew_ps']
    # 2mm difference × ~5.6 ps/mm ≈ 11 ps
    assert 5.0 < out['diff_pair_skew_ps'][label] < 20.0
    assert out['worst_pair_skew']['pair'] == label


def test_bus_skew_grouping():
    g = _FakeGrid(cell_mm=0.1)
    routed = {
        'DDR_DQ0': _PR([('COMP', 0, 0), ('COMP', 100, 0)]),
        'DDR_DQ1': _PR([('COMP', 0, 5), ('COMP', 60, 5)]),
    }
    rules = {n: _Rule() for n in routed}
    out = summarize_propagation(routed, g, rules)
    assert 'DDR_DQ' in out['bus_skew_ps']
    bus = out['bus_skew_ps']['DDR_DQ']
    assert bus['member_count'] == 2
    assert bus['skew_ps'] > 0


def test_summarize_empty():
    g = _FakeGrid()
    out = summarize_propagation({}, g, {})
    assert out['delay_ps'] == {}
    assert out['worst_pair_skew'] is None
