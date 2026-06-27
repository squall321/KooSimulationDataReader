# bga_router.metrics.verifier 단위 + 통합 테스트
"""Verifier checks — one PASS + one FAIL synthetic case per check,
plus a verify_all integration test with hand-computed expected violations.
"""

from __future__ import annotations

import math
from dataclasses import dataclass, field
from typing import Optional

import pytest

from bga_router.metrics.verifier import (
    RuleCheckResult,
    check_layers_membership,
    check_escape_side,
    check_via_budget,
    check_min_length,
    check_bend_class,
    check_clearance,
    check_pair_skew,
    check_pair_gap,
    check_length_group,
    check_keep_out,
    check_split_avoidance,
    check_via_type,
    check_impedance_target,
    check_shield_present,
    check_topology,
    preferred_layer_pct,
    verify_all,
)


# ---------------------------------------------------------------------------
# Fakes
# ---------------------------------------------------------------------------


class _FakeGeom:
    def __init__(self, cell_mm=0.1):
        self.cell_mm = cell_mm

    def cell_to_world(self, ix, iy):
        return ix * self.cell_mm, iy * self.cell_mm


class _FakeGrid:
    def __init__(self, cell_mm=0.1):
        self.geom = _FakeGeom(cell_mm)


@dataclass
class _Rule:
    layers: tuple = ('M1',)
    escape_sides: tuple = ('N', 'S', 'E', 'W')
    width_mm: float = 0.075
    pair_gap_mm: Optional[float] = None
    pair_skew_max_mm: Optional[float] = None
    clearance_mm: Optional[float] = None
    preferred_layers: tuple = ()
    length_match_group: Optional[str] = None
    pair_partner_net: Optional[str] = None
    via_type: Optional[str] = None
    split_avoidance: bool = False
    impedance_target_ohm: Optional[float] = None
    max_via_count: Optional[int] = None
    shield_required: bool = False
    min_trace_length_mm: Optional[float] = None
    net_topology: Optional[str] = None
    keep_out_zones: tuple = ()
    bend_class: Optional[str] = None
    rule_source: str = 'test'


@dataclass
class _PathResult:
    path: list
    via_count: int = 0


@dataclass
class _Task:
    net_name: str
    source: tuple
    sink: tuple
    rule: _Rule


# ---------------------------------------------------------------------------
# Path-only checks — one PASS + one FAIL each
# ---------------------------------------------------------------------------


def test_layers_membership_pass():
    rule = _Rule(layers=('M1', 'M2'))
    path = [('M1', 0, 0), ('M1', 5, 0), ('M2', 5, 0)]
    assert check_layers_membership(path, rule) is True


def test_layers_membership_fail():
    rule = _Rule(layers=('M1',))
    path = [('M1', 0, 0), ('M3', 5, 0)]
    assert check_layers_membership(path, rule) is False


def test_escape_side_pass():
    rule = _Rule(escape_sides=('E',))
    path = [('M1', 0, 0), ('M1', 10, 0)]
    grid = _FakeGrid()
    src_xy = grid.geom.cell_to_world(0, 0)
    assert check_escape_side(path, src_xy, rule) is True


def test_escape_side_fail():
    rule = _Rule(escape_sides=('E',))
    path = [('M1', 0, 0), ('M1', 0, 10)]   # heads north
    grid = _FakeGrid()
    assert check_escape_side(path, grid.geom.cell_to_world(0, 0), rule) is False


def test_via_budget_pass():
    rule = _Rule(max_via_count=2)
    pr = _PathResult(path=[('M1', 0, 0)], via_count=1)
    assert check_via_budget(pr, rule) is True


def test_via_budget_fail():
    rule = _Rule(max_via_count=1)
    pr = _PathResult(path=[('M1', 0, 0)], via_count=3)
    assert check_via_budget(pr, rule) is False


def test_min_length_pass():
    rule = _Rule(min_trace_length_mm=0.5)
    path = [('M1', 0, 0), ('M1', 10, 0)]  # 1.0mm
    assert check_min_length(path, _FakeGrid(), rule) is True


def test_min_length_fail():
    rule = _Rule(min_trace_length_mm=5.0)
    path = [('M1', 0, 0), ('M1', 10, 0)]  # 1.0mm
    assert check_min_length(path, _FakeGrid(), rule) is False


def test_bend_class_low_speed_pass_with_45():
    # 45° turn → interior 135°. low_speed allows 45° bends.
    rule = _Rule(bend_class='low_speed')
    path = [('M1', 0, 0), ('M1', 5, 0), ('M1', 10, 5)]
    assert check_bend_class(path, rule) is True


def test_bend_class_low_speed_fail_on_acute():
    rule = _Rule(bend_class='low_speed')
    # 180° turn back: vectors (5,0) then (-5,0) -> interior 0° (acute)
    path = [('M1', 0, 0), ('M1', 5, 0), ('M1', 0, 0)]
    assert check_bend_class(path, rule) is False


def test_bend_class_hf_diff_fail_on_45():
    rule = _Rule(bend_class='hf_diff')
    path = [('M1', 0, 0), ('M1', 5, 0), ('M1', 10, 5)]  # 45° turn
    assert check_bend_class(path, rule) is False


def test_bend_class_hf_diff_pass_on_90():
    rule = _Rule(bend_class='hf_diff')
    # 90° turn: (5,0) then (0,5) -> interior 90°
    path = [('M1', 0, 0), ('M1', 5, 0), ('M1', 5, 5)]
    assert check_bend_class(path, rule) is True


def test_bend_class_none_returns_na():
    rule = _Rule(bend_class=None)
    path = [('M1', 0, 0), ('M1', 5, 0), ('M1', 0, 0)]
    assert check_bend_class(path, rule) is None


# ---------------------------------------------------------------------------
# Cross-net checks
# ---------------------------------------------------------------------------


def test_clearance_pass():
    grid = _FakeGrid()
    routed = {
        'A': _PathResult(path=[('M1', 0, 0), ('M1', 10, 0)]),
        'B': _PathResult(path=[('M1', 0, 50), ('M1', 10, 50)]),   # 5 mm away
    }
    rules = {'A': _Rule(clearance_mm=0.2), 'B': _Rule(clearance_mm=0.2)}
    r = check_clearance(routed, grid, rules)
    assert r.passed is True


def test_clearance_fail():
    grid = _FakeGrid()
    routed = {
        'A': _PathResult(path=[('M1', 0, 0), ('M1', 10, 0)]),
        'B': _PathResult(path=[('M1', 0, 1), ('M1', 10, 1)]),    # 0.1mm away
    }
    rules = {'A': _Rule(clearance_mm=0.2), 'B': _Rule(clearance_mm=0.2)}
    r = check_clearance(routed, grid, rules)
    assert r.passed is False
    assert 'A' in r.violators or 'B' in r.violators


def test_pair_skew_pass():
    grid = _FakeGrid()
    routed = {
        'P': _PathResult(path=[('M1', 0, 0), ('M1', 10, 0)]),    # 1.0
        'N': _PathResult(path=[('M1', 0, 5), ('M1', 10, 5)]),    # 1.0
    }
    rules = {
        'P': _Rule(pair_partner_net='N', pair_skew_max_mm=0.2),
        'N': _Rule(pair_partner_net='P', pair_skew_max_mm=0.2),
    }
    r = check_pair_skew(routed, grid, rules)
    assert r.passed is True


def test_pair_skew_fail():
    grid = _FakeGrid()
    routed = {
        'P': _PathResult(path=[('M1', 0, 0), ('M1', 10, 0)]),    # 1.0
        'N': _PathResult(path=[('M1', 0, 5), ('M1', 30, 5)]),    # 3.0 → skew 2.0
    }
    rules = {
        'P': _Rule(pair_partner_net='N', pair_skew_max_mm=0.2),
        'N': _Rule(pair_partner_net='P', pair_skew_max_mm=0.2),
    }
    r = check_pair_skew(routed, grid, rules)
    assert r.passed is False


def test_pair_gap_pass():
    grid = _FakeGrid()
    # 0.5mm apart parallel, pair_gap = 0.4 → ok
    routed = {
        'P': _PathResult(path=[('M1', 0, 0), ('M1', 20, 0)]),
        'N': _PathResult(path=[('M1', 0, 5), ('M1', 20, 5)]),
    }
    rules = {
        'P': _Rule(pair_partner_net='N', pair_gap_mm=0.4),
        'N': _Rule(pair_partner_net='P', pair_gap_mm=0.4),
    }
    r = check_pair_gap(routed, grid, rules)
    assert r.passed is True


def test_pair_gap_fail():
    grid = _FakeGrid()
    # 0.1mm apart parallel, pair_gap = 0.3 → fail
    routed = {
        'P': _PathResult(path=[('M1', 0, 0), ('M1', 20, 0)]),
        'N': _PathResult(path=[('M1', 0, 1), ('M1', 20, 1)]),
    }
    rules = {
        'P': _Rule(pair_partner_net='N', pair_gap_mm=0.3),
        'N': _Rule(pair_partner_net='P', pair_gap_mm=0.3),
    }
    r = check_pair_gap(routed, grid, rules)
    assert r.passed is False


def test_length_group_pass():
    grid = _FakeGrid()
    routed = {
        'A': _PathResult(path=[('M1', 0, 0), ('M1', 10, 0)]),
        'B': _PathResult(path=[('M1', 0, 5), ('M1', 11, 5)]),
    }
    rules = {
        'A': _Rule(length_match_group='G', pair_skew_max_mm=0.5),
        'B': _Rule(length_match_group='G', pair_skew_max_mm=0.5),
    }
    r = check_length_group(routed, grid, rules)
    assert r.passed is True


def test_length_group_fail():
    grid = _FakeGrid()
    routed = {
        'A': _PathResult(path=[('M1', 0, 0), ('M1', 10, 0)]),    # 1.0
        'B': _PathResult(path=[('M1', 0, 5), ('M1', 50, 5)]),    # 5.0 → spread 4.0
    }
    rules = {
        'A': _Rule(length_match_group='G', pair_skew_max_mm=0.5),
        'B': _Rule(length_match_group='G', pair_skew_max_mm=0.5),
    }
    r = check_length_group(routed, grid, rules)
    assert r.passed is False


# ---------------------------------------------------------------------------
# External-data-dependent checks
# ---------------------------------------------------------------------------


def test_keep_out_pass():
    rule = _Rule(keep_out_zones=((5.0, 5.0, 6.0, 6.0),))
    path = [('M1', 0, 0), ('M1', 10, 0)]  # y=0, no overlap
    assert check_keep_out(path, _FakeGrid(), rule) is True


def test_keep_out_fail():
    rule = _Rule(keep_out_zones=((0.4, -0.1, 0.6, 0.1),))
    path = [('M1', 0, 0), ('M1', 5, 0)]  # ix=5 → x=0.5, inside the box
    assert check_keep_out(path, _FakeGrid(), rule) is False


def test_keep_out_na_when_no_zones():
    rule = _Rule()
    assert check_keep_out([('M1', 0, 0)], _FakeGrid(), rule) is None


def test_split_avoidance_na_without_plane():
    rule = _Rule(split_avoidance=True)
    assert check_split_avoidance([('M1', 0, 0)], _FakeGrid(), rule) is None


def test_via_type_na_without_stackup():
    rule = _Rule(via_type='tht')
    pr = _PathResult(path=[('M1', 0, 0)], via_count=1)
    assert check_via_type(pr, rule, stackup=None) is None


def test_impedance_target_na():
    rule = _Rule(impedance_target_ohm=50.0)
    assert check_impedance_target([('M1', 0, 0)], rule, stackup=None) is None
    rule2 = _Rule(impedance_target_ohm=None)
    assert check_impedance_target([('M1', 0, 0)], rule2, stackup='x') is None


def test_shield_present_na_when_no_pg_nets():
    grid = _FakeGrid()
    routed = {
        'SIG': _PathResult(path=[('M1', 0, 0), ('M1', 10, 0)]),
    }
    rules = {'SIG': _Rule(shield_required=True, pair_gap_mm=0.3)}
    r = check_shield_present(routed['SIG'].path, routed, rules,
                              rules['SIG'], grid)
    assert r is None


def test_shield_present_pass():
    grid = _FakeGrid()
    routed = {
        'SIG': _PathResult(path=[('M1', 0, 0), ('M1', 20, 0)]),
        'GND1': _PathResult(path=[('M1', 0, 3), ('M1', 20, 3)]),   # +0.3mm
        'GND2': _PathResult(path=[('M1', 0, -3), ('M1', 20, -3)]),  # -0.3mm
    }
    rules = {
        'SIG': _Rule(shield_required=True, pair_gap_mm=0.4),
        'GND1': _Rule(),
        'GND2': _Rule(),
    }
    r = check_shield_present(routed['SIG'].path, routed, rules,
                              rules['SIG'], grid)
    assert r is True


def test_shield_present_fail_one_side_only():
    grid = _FakeGrid()
    routed = {
        'SIG': _PathResult(path=[('M1', 0, 0), ('M1', 20, 0)]),
        'GND1': _PathResult(path=[('M1', 0, 3), ('M1', 20, 3)]),  # only one side
    }
    rules = {
        'SIG': _Rule(shield_required=True, pair_gap_mm=0.4),
        'GND1': _Rule(),
    }
    r = check_shield_present(routed['SIG'].path, routed, rules,
                              rules['SIG'], grid)
    assert r is False


def test_topology_daisy_chain_pass():
    rule = _Rule(net_topology='daisy_chain')
    path = [('M1', 0, 0), ('M1', 1, 0), ('M1', 2, 0)]
    assert check_topology(path, rule) is True


def test_topology_daisy_chain_fail_branch():
    rule = _Rule(net_topology='daisy_chain')
    # Construct a branching pseudo-path by repeating a center cell with
    # an extra spur. Cell (1,0) ends up with degree 3 (neighbors of
    # (0,0), (2,0), (1,1)).
    path = [('M1', 0, 0), ('M1', 1, 0), ('M1', 2, 0),
            ('M1', 1, 0), ('M1', 1, 1)]
    assert check_topology(path, rule) is False


def test_topology_star_na():
    rule = _Rule(net_topology='star')
    assert check_topology([('M1', 0, 0)], rule) is None


def test_topology_none_na():
    rule = _Rule(net_topology=None)
    assert check_topology([('M1', 0, 0)], rule) is None


# ---------------------------------------------------------------------------
# Soft metric
# ---------------------------------------------------------------------------


def test_preferred_layer_pct_full():
    rule = _Rule(layers=('M1', 'M2'), preferred_layers=('M1',))
    path = [('M1', 0, 0), ('M1', 10, 0)]
    assert preferred_layer_pct(path, _FakeGrid(), rule) == pytest.approx(1.0)


def test_preferred_layer_pct_half():
    rule = _Rule(layers=('M1', 'M2'), preferred_layers=('M1',))
    path = [('M1', 0, 0), ('M1', 10, 0), ('M2', 10, 0), ('M2', 20, 0)]
    assert preferred_layer_pct(path, _FakeGrid(), rule) == pytest.approx(0.5)


# ---------------------------------------------------------------------------
# verify_all integration
# ---------------------------------------------------------------------------


def test_verify_all_4_nets_two_pairs_handcount():
    """4 nets: pair PA/NA (skew OK, gap OK), pair PB/NB (skew FAIL, gap OK).

    Hand count of failing checks (single by_field entries that return passed=False):
      - pair_skew_ok  → FAIL (PB/NB skew = 2.0 > 0.2)
    All other checks should be PASS or N/A. Total violations = 1.
    """
    grid = _FakeGrid()
    routed = {
        'PA': _PathResult(path=[('M1', 0, 0), ('M1', 10, 0)]),
        'NA': _PathResult(path=[('M1', 0, 5), ('M1', 10, 5)]),   # gap 0.5mm
        'PB': _PathResult(path=[('M1', 0, 50), ('M1', 10, 50)]),  # 1.0
        'NB': _PathResult(path=[('M1', 0, 55), ('M1', 30, 55)]),  # 3.0 → skew fail
    }
    rule_pa = _Rule(pair_partner_net='NA', pair_skew_max_mm=0.2,
                    pair_gap_mm=0.3)
    rule_na = _Rule(pair_partner_net='PA', pair_skew_max_mm=0.2,
                    pair_gap_mm=0.3)
    rule_pb = _Rule(pair_partner_net='NB', pair_skew_max_mm=0.2,
                    pair_gap_mm=0.3)
    rule_nb = _Rule(pair_partner_net='PB', pair_skew_max_mm=0.2,
                    pair_gap_mm=0.3)
    tasks = [
        _Task('PA', (0, 0, 0), (0, 10, 0), rule_pa),
        _Task('NA', (0, 0, 5), (0, 10, 5), rule_na),
        _Task('PB', (0, 0, 50), (0, 10, 50), rule_pb),
        _Task('NB', (0, 0, 55), (0, 30, 55), rule_nb),
    ]

    class _Spec:
        pass

    out = verify_all(routed, tasks, grid, _Spec(), stackup=None)
    bf = out['by_field']
    assert bf['pair_skew_ok'].passed is False
    assert bf['pair_gap_ok'].passed is True
    assert bf['clearance_ok'].passed is True or bf['clearance_ok'].note.startswith('N/A')
    # Exactly 1 violation
    assert out['violations'] == 1


def test_verify_all_stackup_none_returns_na_for_dependent_checks():
    grid = _FakeGrid()
    routed = {
        'X': _PathResult(path=[('M1', 0, 0), ('M1', 10, 0)]),
    }
    rule = _Rule(via_type='tht', impedance_target_ohm=50.0)
    tasks = [_Task('X', (0, 0, 0), (0, 10, 0), rule)]
    out = verify_all(routed, tasks, grid, object(), stackup=None)
    bf = out['by_field']
    assert bf['via_type_ok'].passed is True
    assert 'N/A' in bf['via_type_ok'].note
    assert bf['impedance_target_ok'].passed is True
    assert 'N/A' in bf['impedance_target_ok'].note
