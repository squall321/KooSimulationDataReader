# Wadell Z0/Zdiff + DC R + variance 단위 테스트 (closed-form, no solver)
"""Phase C1 — analytical SI metrics tests."""

from __future__ import annotations

import math
from dataclasses import dataclass
from typing import Optional, Tuple

import pytest

from bga_router.metrics.si import (
    branch_dc_resistance_mohm,
    is_marginal_formula,
    summarize_si,
    z0_for_net,
    z0_microstrip,
    z0_stripline,
    z0_variance_pct,
    zdiff_for_pair,
    zdiff_microstrip,
    zdiff_stripline,
)
from bga_router.metrics.stackup import default_stackup, load_stackup_yaml
from pathlib import Path


# ---------------------------------------------------------------------------
# Fake grid + fake rule
# ---------------------------------------------------------------------------


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
    width_mm: float
    pair_gap_mm: Optional[float] = None
    pair_partner_net: Optional[str] = None


@dataclass
class _PathResult:
    path: list


# ---------------------------------------------------------------------------
# Z0 closed-form regression tests against published Polar Si9000 values
# ---------------------------------------------------------------------------


def test_z0_microstrip_50ohm_target():
    # 0.150 mm trace, 0.035 mm Cu, 0.10 mm h, εr=4.2 → ~50 Ω on Si9000
    z = z0_microstrip(0.15, 0.035, 0.10, 4.2)
    assert z == pytest.approx(50.0, abs=5.0)


def test_z0_microstrip_monotonic_with_width():
    z_narrow = z0_microstrip(0.10, 0.035, 0.10, 4.2)
    z_mid    = z0_microstrip(0.15, 0.035, 0.10, 4.2)
    z_wide   = z0_microstrip(0.20, 0.035, 0.10, 4.2)
    assert z_narrow > z_mid > z_wide


def test_z0_microstrip_continuous_at_wh_boundary():
    """w/h=1 is the branch in Hammerstad; check both sides are close."""
    z_below = z0_microstrip(0.099, 0.035, 0.10, 4.2)
    z_above = z0_microstrip(0.101, 0.035, 0.10, 4.2)
    assert abs(z_below - z_above) < 1.0


def test_z0_stripline_lower_than_microstrip():
    z_ms = z0_microstrip(0.15, 0.035, 0.10, 4.2)
    z_sl = z0_stripline(0.15, 0.035, 0.20, 4.2)
    assert z_sl < z_ms


def test_zdiff_microstrip_around_100ohm():
    zd = zdiff_microstrip(0.10, 0.035, 0.10, 0.10, 4.2)
    assert 80.0 < zd < 130.0


def test_zdiff_stripline_around_100ohm():
    zd = zdiff_stripline(0.12, 0.035, 0.30, 0.15, 4.2)
    assert 70.0 < zd < 130.0


def test_z0_safe_at_degenerate_input():
    assert z0_microstrip(0.0, 0.035, 0.1, 4.2) == 0.0
    assert z0_stripline(0.15, 0.035, 0.0, 4.2) == 0.0


# ---------------------------------------------------------------------------
# z0_for_net + zdiff_for_pair (path + stackup wired)
# ---------------------------------------------------------------------------


def test_z0_for_net_microstrip_layer():
    s = default_stackup()
    g = _FakeGrid(cell_mm=0.1)
    path = [('COMP', 0, 0), ('COMP', 50, 0)]
    rule = _Rule(width_mm=0.15)
    z = z0_for_net(path, g, rule, s)
    assert z is not None
    assert 40.0 < z < 70.0  # microstrip range


def test_z0_for_net_returns_none_when_width_missing():
    s = default_stackup()
    g = _FakeGrid()
    path = [('COMP', 0, 0), ('COMP', 50, 0)]
    rule = _Rule(width_mm=0.0)
    assert z0_for_net(path, g, rule, s) is None


def test_z0_for_net_returns_none_when_layer_unknown():
    s = default_stackup()
    g = _FakeGrid()
    path = [('GHOST_LAYER', 0, 0), ('GHOST_LAYER', 50, 0)]
    rule = _Rule(width_mm=0.15)
    assert z0_for_net(path, g, rule, s) is None


def test_zdiff_for_pair_returns_finite():
    s = default_stackup()
    g = _FakeGrid(cell_mm=0.05)
    # Two parallel paths 0.15 mm apart on COMP (microstrip)
    path_p = [('COMP', 0, 0), ('COMP', 200, 0)]
    path_n = [('COMP', 0, 3), ('COMP', 200, 3)]  # 3 cells * 0.05 = 0.15
    rule = _Rule(width_mm=0.075, pair_gap_mm=0.15,
                  pair_partner_net='DDR_DQ0_N')
    zd = zdiff_for_pair(path_p, path_n, g, rule, s)
    assert zd is not None
    assert 60.0 < zd < 150.0


# ---------------------------------------------------------------------------
# DC resistance + variance + marginal flags
# ---------------------------------------------------------------------------


def test_branch_dc_resistance_10mm_trace():
    """10 mm * 0.075 mm wide * 0.035 mm thick.
    R = ρ * L / A = 1.68e-5 ohm·mm * 10 mm / (0.075 * 0.035) mm²
      = 64 mΩ"""
    s = default_stackup()
    g = _FakeGrid(cell_mm=0.1)
    path = [('COMP', 0, 0), ('COMP', 100, 0)]   # 10 mm
    rule = _Rule(width_mm=0.075)
    r = branch_dc_resistance_mohm(path, g, rule, s)
    assert r is not None
    assert 60.0 < r < 70.0


def test_branch_dc_resistance_none_when_width_zero():
    s = default_stackup()
    g = _FakeGrid()
    path = [('COMP', 0, 0), ('COMP', 50, 0)]
    rule = _Rule(width_mm=0.0)
    assert branch_dc_resistance_mohm(path, g, rule, s) is None


def test_z0_variance_pct_zero_when_uniform():
    s = default_stackup()
    g = _FakeGrid(cell_mm=0.1)
    routed = {
        'A': _PathResult([('COMP', 0, 0), ('COMP', 50, 0)]),
        'B': _PathResult([('COMP', 0, 5), ('COMP', 50, 5)]),
    }
    rules = {'A': _Rule(width_mm=0.15), 'B': _Rule(width_mm=0.15)}
    assert z0_variance_pct(routed, g, rules, s) == 0.0


def test_z0_variance_pct_positive_when_widths_differ():
    s = default_stackup()
    g = _FakeGrid(cell_mm=0.1)
    routed = {
        'A': _PathResult([('COMP', 0, 0), ('COMP', 50, 0)]),
        'B': _PathResult([('COMP', 0, 5), ('COMP', 50, 5)]),
    }
    rules = {'A': _Rule(width_mm=0.10), 'B': _Rule(width_mm=0.20)}
    v = z0_variance_pct(routed, g, rules, s)
    assert v > 5.0  # different widths → noticeable variance


def test_is_marginal_formula_tight_coupling():
    s = default_stackup()
    rule = _Rule(width_mm=0.10, pair_gap_mm=0.04)  # s/h=0.4 < 0.5
    flags = is_marginal_formula(rule, s, 'COMP')
    assert flags['tight_coupling'] is True


def test_is_marginal_formula_thin_dielectric():
    """default stackup has h=0.10mm, not thin → False."""
    s = default_stackup()
    flags = is_marginal_formula(_Rule(width_mm=0.10), s, 'COMP')
    assert flags['thin_dielectric'] is False


# ---------------------------------------------------------------------------
# summarize_si end-to-end
# ---------------------------------------------------------------------------


def test_summarize_si_populates_all_buckets():
    s = default_stackup()
    g = _FakeGrid(cell_mm=0.1)
    routed = {
        'A': _PathResult([('COMP', 0, 0), ('COMP', 50, 0)]),
        'B': _PathResult([('LAY2', 0, 0), ('LAY2', 50, 0)]),
    }
    rules = {'A': _Rule(width_mm=0.15), 'B': _Rule(width_mm=0.10)}
    out = summarize_si(routed, g, rules, s)
    assert set(out['Z0_single_ended_ohm']) == {'A', 'B'}
    assert 'A' in out['branch_dc_resistance_mohm']
    assert out['stackup_default_used'] is True
    assert isinstance(out['Z0_variance_pct'], float)


def test_summarize_si_pair_label():
    s = default_stackup()
    g = _FakeGrid(cell_mm=0.05)
    routed = {
        'P': _PathResult([('COMP', 0, 0), ('COMP', 200, 0)]),
        'N': _PathResult([('COMP', 0, 3), ('COMP', 200, 3)]),
    }
    rules = {
        'P': _Rule(width_mm=0.075, pair_gap_mm=0.15, pair_partner_net='N'),
        'N': _Rule(width_mm=0.075, pair_gap_mm=0.15, pair_partner_net='P'),
    }
    out = summarize_si(routed, g, rules, s)
    # Pair label is '|'.join(sorted([net, partner]))
    assert 'N|P' in out['Zdiff_ohm']
