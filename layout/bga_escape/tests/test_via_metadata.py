# Phase D-6 PG via metadata 단위 테스트 — net 분류 + via 좌표 수집
"""Phase D-6 — derived via_metadata + PG via aggregation tests."""

from __future__ import annotations

from dataclasses import dataclass
from typing import Tuple

import pytest

from bga_router.metrics.path_geometry import (
    collect_pg_via_xy,
    collect_stitching_via_xy,
    derive_via_metadata,
    is_power_ground_net,
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
class _PR:
    path: list


# ---------------------------------------------------------------------------
# is_power_ground_net
# ---------------------------------------------------------------------------


@pytest.mark.parametrize('name', [
    'GND', 'GND_1', 'GND_DIG', 'VSS', 'VSS_AVDD',
    'VDD', 'VDD_CORE', 'VCC18', 'VCC',
    'PWR', 'PWR_RAIL_1V8', 'PG_NET_42',
])
def test_pg_classification_positive(name):
    assert is_power_ground_net(name) is True


@pytest.mark.parametrize('name', [
    'DDR4_DQ0_P', 'PCIE_RX0_N', 'USB3_TX_P', 'HDMI_D0_P',
    'LED_STATUS', 'CLK_25M', 'net042',
])
def test_pg_classification_negative(name):
    assert is_power_ground_net(name) is False


def test_pg_classification_empty_returns_false():
    assert is_power_ground_net('') is False
    assert is_power_ground_net(None) is False


def test_pg_classification_strict_against_partial_match():
    """'GNDPULL' shouldn't false-positive as PG (no _ / digit / end)."""
    # Actually 'GNDPULL': prefix 'GND' + next char 'P' (not digit/_/end)
    # Should be FALSE per strict rule.
    assert is_power_ground_net('GNDPULL') is False


# ---------------------------------------------------------------------------
# derive_via_metadata
# ---------------------------------------------------------------------------


def test_derive_via_metadata_no_transitions():
    g = _FakeGrid()
    out = derive_via_metadata(
        [('COMP', 0, 0), ('COMP', 10, 0)], g, 'A')
    assert out == []


def test_derive_via_metadata_signal_via():
    g = _FakeGrid()
    out = derive_via_metadata(
        [('COMP', 0, 0), ('LAY2', 0, 0)], g, 'DDR4_DQ0_P')
    assert len(out) == 1
    e = out[0]
    assert e['kind'] == 'signal'
    assert e['start_layer'] == 'COMP'
    assert e['end_layer'] == 'LAY2'
    assert e['xy_mm'] == (0.0, 0.0)


def test_derive_via_metadata_pg_via():
    g = _FakeGrid()
    out = derive_via_metadata(
        [('COMP', 1, 1), ('LAY2', 1, 1), ('LAY3', 1, 1)], g, 'GND_DIG')
    assert len(out) == 2
    assert all(e['kind'] == 'power_ground' for e in out)


def test_derive_via_metadata_empty_path():
    g = _FakeGrid()
    assert derive_via_metadata([], g, 'A') == []


# ---------------------------------------------------------------------------
# collect_pg_via_xy / collect_stitching_via_xy
# ---------------------------------------------------------------------------


def test_collect_pg_via_aggregates_only_pg_nets():
    g = _FakeGrid()
    routed = {
        'DDR4_DQ0_P': _PR([('COMP', 0, 0), ('LAY2', 0, 0)]),  # signal — skip
        'GND_1':      _PR([('COMP', 1, 0), ('LAY2', 1, 0)]),  # PG — include
        'VDD_CORE':   _PR([('LAY2', 2, 0), ('LAY3', 2, 0)]),  # PG — include
    }
    pts = collect_pg_via_xy(routed, g)
    # Only one via per PG path → 2 points total
    assert len(pts) == 2
    assert (0.1, 0.0) in pts
    assert (0.2, 0.0) in pts


def test_collect_stitching_via_same_policy_as_pg():
    g = _FakeGrid()
    routed = {'GND_1': _PR([('COMP', 5, 0), ('LAY2', 5, 0)])}
    assert collect_pg_via_xy(routed, g) == collect_stitching_via_xy(routed, g)


def test_collect_pg_via_empty_when_no_pg_nets():
    g = _FakeGrid()
    routed = {'DDR4_DQ0_P': _PR([('COMP', 0, 0), ('LAY2', 0, 0)])}
    assert collect_pg_via_xy(routed, g) == []


def test_collect_pg_via_handles_path_without_transitions():
    g = _FakeGrid()
    routed = {'GND': _PR([('COMP', 0, 0), ('COMP', 5, 0)])}
    assert collect_pg_via_xy(routed, g) == []
