# em_queue hook 단위 테스트 — marginal flag / impedance violator → queue entry
"""Phase D — EM queue hook tests."""

from __future__ import annotations

from dataclasses import dataclass
from typing import Optional, Tuple

import pytest

from bga_router.metrics.em_queue import build_em_queue


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
    width_mm: float = 0.075
    pair_gap_mm: Optional[float] = None
    pair_partner_net: Optional[str] = None
    impedance_target_ohm: Optional[float] = None
    dielectric_height_mm: Optional[float] = None
    dielectric_er: Optional[float] = None
    copper_thickness_mm: Optional[float] = None


@dataclass
class _PR:
    path: list


def test_em_queue_empty_when_no_marginal_or_violators():
    g = _FakeGrid()
    routed = {'A': _PR([('COMP', 0, 0), ('COMP', 50, 0)])}
    rbn = {'A': _Rule()}
    out = build_em_queue(routed, g, rbn,
                          si_metrics={'marginal_formulas': {}},
                          rule_check={'by_field': {}})
    assert out['count'] == 0
    assert out['entries'] == []
    assert out['reason_breakdown'] == {}


def test_em_queue_picks_up_tight_coupling():
    g = _FakeGrid()
    routed = {'P': _PR([('COMP', 0, 0), ('COMP', 50, 0)])}
    rbn = {'P': _Rule(pair_gap_mm=0.04)}
    si = {'marginal_formulas': {'P': {'tight_coupling': True,
                                         'thin_dielectric': False}}}
    out = build_em_queue(routed, g, rbn, si, rule_check={'by_field': {}})
    assert out['count'] == 1
    e = out['entries'][0]
    assert e['net'] == 'P'
    assert 'tight_coupling' in e['reasons']
    assert e['reason'] == 'tight_coupling'
    assert e['layer'] == 'COMP'
    assert e['bbox_mm'] == [0.0, 0.0, 5.0, 0.0]
    assert e['rule']['pair_gap_mm'] == 0.04
    assert out['reason_breakdown']['tight_coupling'] == 1


def test_em_queue_picks_up_thin_dielectric():
    g = _FakeGrid()
    routed = {'A': _PR([('LAY2', 0, 0), ('LAY2', 100, 0)])}
    rbn = {'A': _Rule()}
    si = {'marginal_formulas': {'A': {'tight_coupling': False,
                                         'thin_dielectric': True}}}
    out = build_em_queue(routed, g, rbn, si, rule_check={'by_field': {}})
    assert out['count'] == 1
    assert 'thin_dielectric' in out['entries'][0]['reasons']


def test_em_queue_picks_up_impedance_violators():
    g = _FakeGrid()
    routed = {'X': _PR([('COMP', 0, 0), ('COMP', 50, 0)])}
    rbn = {'X': _Rule(impedance_target_ohm=50.0)}
    rc = {'by_field': {'impedance_target_ok': {'violators': ['X']}}}
    out = build_em_queue(routed, g, rbn,
                          si_metrics={'marginal_formulas': {}},
                          rule_check=rc)
    assert out['count'] == 1
    assert 'impedance_miss' in out['entries'][0]['reasons']


def test_em_queue_combines_reasons():
    g = _FakeGrid()
    routed = {'Q': _PR([('LAY2', 0, 0), ('LAY2', 50, 0)])}
    rbn = {'Q': _Rule(impedance_target_ohm=85.0, pair_gap_mm=0.03)}
    si = {'marginal_formulas': {'Q': {'tight_coupling': True,
                                         'thin_dielectric': True}}}
    rc = {'by_field': {'impedance_target_ok': {'violators': ['Q']}}}
    out = build_em_queue(routed, g, rbn, si, rc)
    assert out['count'] == 1
    reasons = out['entries'][0]['reasons']
    assert set(reasons) == {'tight_coupling', 'thin_dielectric',
                             'impedance_miss'}
    assert out['entries'][0]['reason'] == '+'.join(reasons)


def test_em_queue_skips_nets_without_path():
    g = _FakeGrid()
    routed = {'A': _PR([]), 'B': _PR([('COMP', 0, 0), ('COMP', 10, 0)])}
    si = {'marginal_formulas': {'A': {'tight_coupling': True},
                                  'B': {'tight_coupling': True}}}
    out = build_em_queue(routed, g, {'A': _Rule(), 'B': _Rule()}, si,
                          rule_check={'by_field': {}})
    nets = {e['net'] for e in out['entries']}
    assert nets == {'B'}


def test_em_queue_layers_in_path_preserves_order():
    g = _FakeGrid()
    routed = {'A': _PR([('COMP', 0, 0), ('LAY2', 0, 0), ('LAY2', 10, 0),
                         ('COMP', 10, 0)])}
    si = {'marginal_formulas': {'A': {'tight_coupling': True}}}
    out = build_em_queue(routed, g, {'A': _Rule()}, si,
                          rule_check={'by_field': {}})
    assert out['entries'][0]['layers'] == ['COMP', 'LAY2']


def test_em_queue_consumption_hint_present():
    g = _FakeGrid()
    out = build_em_queue({}, g, {},
                          si_metrics={'marginal_formulas': {}},
                          rule_check={'by_field': {}})
    assert 'consumption_hint' in out
    assert 'sol_b_peec' in out['consumption_hint']
    assert 'sol_d_cross_section' in out['consumption_hint']
