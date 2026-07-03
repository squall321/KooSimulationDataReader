# thermal/EMI first-order 메트릭 단위 테스트
"""Phase H-9 — thermal / EMI tests."""

from __future__ import annotations

from dataclasses import dataclass
from typing import Optional, Tuple

import pytest

from bga_router.metrics.thermal_emi import (
    current_capacity_a,
    edge_proximity_flags,
    loop_area_proxy_mm2,
    summarize_thermal_emi,
)
from bga_router.metrics.stackup import default_stackup


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


@dataclass
class _PR:
    path: list


# ---------------------------------------------------------------------------
# IPC-2152 current capacity
# ---------------------------------------------------------------------------


def test_current_capacity_reasonable_for_standard_trace():
    """0.25mm × 35um outer trace @10°C rise → ~1A ballpark (IPC-2152)."""
    cap = current_capacity_a(0.25, 0.035)
    assert cap is not None
    assert 0.5 < cap < 3.0


def test_current_capacity_scales_with_width():
    narrow = current_capacity_a(0.075, 0.035)
    wide = current_capacity_a(0.50, 0.035)
    assert wide > narrow * 2


def test_current_capacity_inner_lower_than_outer():
    outer = current_capacity_a(0.25, 0.035, outer_layer=True)
    inner = current_capacity_a(0.25, 0.035, outer_layer=False)
    assert inner < outer


def test_current_capacity_none_for_invalid():
    assert current_capacity_a(0.0, 0.035) is None
    assert current_capacity_a(0.25, 0.0) is None


# ---------------------------------------------------------------------------
# EMI proxies
# ---------------------------------------------------------------------------


def test_loop_area_scales_with_length_and_h():
    g = _FakeGrid(cell_mm=0.1)
    path = [('L', 0, 0), ('L', 100, 0)]  # 10 mm
    a1 = loop_area_proxy_mm2(path, g, dielectric_h_mm=0.1)
    a2 = loop_area_proxy_mm2(path, g, dielectric_h_mm=0.2)
    assert a1 == pytest.approx(1.0, abs=1e-9)   # 10 × 0.1
    assert a2 == pytest.approx(2.0, abs=1e-9)


def test_edge_proximity_flags_near_edge():
    g = _FakeGrid(cell_mm=0.1)
    routed = {
        'EDGE':   _PR([('L', 0, 0), ('L', 100, 0)]),      # on bbox border
        'CENTER': _PR([('L', 40, 30), ('L', 60, 30)]),
    }
    flags = edge_proximity_flags(routed, g,
                                    board_bbox_mm=(0, 0, 10, 6),
                                    margin_mm=1.0)
    assert flags['EDGE'] is True
    assert flags['CENTER'] is False


# ---------------------------------------------------------------------------
# summarize
# ---------------------------------------------------------------------------


def test_summarize_marks_first_order():
    g = _FakeGrid()
    s = summarize_thermal_emi({}, g, {})
    assert s['first_order_estimate'] is True


def test_summarize_flags_under_capacity():
    g = _FakeGrid(cell_mm=0.1)
    routed = {'PWR': _PR([('COMP', 0, 0), ('COMP', 100, 0)])}
    rules = {'PWR': _Rule(width_mm=0.075)}   # thin → low capacity
    s = summarize_thermal_emi(routed, g, rules,
                                 stackup=default_stackup(),
                                 target_current_a=5.0)   # demanding target
    assert 'PWR' in s['thermal']['under_capacity_nets']


def test_summarize_worst_loop_identified():
    g = _FakeGrid(cell_mm=0.1)
    routed = {
        'SHORT': _PR([('COMP', 0, 0), ('COMP', 10, 0)]),
        'LONG':  _PR([('COMP', 0, 10), ('COMP', 200, 10)]),
    }
    rules = {n: _Rule() for n in routed}
    s = summarize_thermal_emi(routed, g, rules, stackup=default_stackup())
    assert s['emi']['worst_loop_net'] == 'LONG'
