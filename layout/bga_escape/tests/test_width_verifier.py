# Phase D-7 check_width 단위 테스트 — declared width가 채널에 물리적으로 맞는지
"""check_width verifier — physical channel-fit test."""

from __future__ import annotations

from dataclasses import dataclass
from typing import Optional, Tuple

import pytest

from bga_router.metrics.verifier import check_width


class _FakeGeom:
    def __init__(self, cell_mm: float = 0.05):
        self.cell_mm = cell_mm

    def cell_to_world(self, ix: int, iy: int) -> Tuple[float, float]:
        return ix * self.cell_mm, iy * self.cell_mm


class _FakeGrid:
    """is_blocked returns True iff (layer, ix, iy) ∈ blocked set."""
    def __init__(self, cell_mm: float = 0.05, blocked: Optional[set] = None):
        self.geom = _FakeGeom(cell_mm)
        self.blocked = blocked or set()

    def is_blocked(self, layer: str, ix: int, iy: int) -> bool:
        return (layer, ix, iy) in self.blocked


@dataclass
class _Rule:
    width_mm: float


def test_check_width_na_when_no_width():
    g = _FakeGrid()
    assert check_width([('A', 0, 0)], g, _Rule(width_mm=0.0)) is None


def test_check_width_na_when_no_rule():
    g = _FakeGrid()
    assert check_width([('A', 0, 0)], g, None) is None


def test_check_width_na_when_empty_path():
    g = _FakeGrid()
    assert check_width([], g, _Rule(width_mm=0.1)) is None


def test_check_width_passes_in_empty_grid():
    """No blockers anywhere → declared width fits trivially."""
    g = _FakeGrid(cell_mm=0.05)
    path = [('A', 10, 10), ('A', 20, 10)]
    rule = _Rule(width_mm=0.10)  # half-width 0.05 mm = 1 cell radius
    assert check_width(path, g, rule) is True


def test_check_width_fails_when_blocker_within_half_width():
    """Blocker at distance < half-width → fails."""
    g = _FakeGrid(cell_mm=0.05,
                    blocked={('A', 11, 10)})       # 1 cell next to path
    path = [('A', 10, 10), ('A', 20, 10)]
    # width 0.10 mm → half-width 0.05 mm → radius 1 cell. Blocker is 1 cell
    # away (in radius) on a non-self cell → fail
    rule = _Rule(width_mm=0.10)
    assert check_width(path, g, rule) is False


def test_check_width_passes_when_blocker_outside_half_width():
    """Blocker at distance > half-width → still passes."""
    g = _FakeGrid(cell_mm=0.05,
                    blocked={('A', 14, 10)})       # 4 cells away
    path = [('A', 10, 10), ('A', 12, 10)]
    rule = _Rule(width_mm=0.10)  # radius 1 cell
    assert check_width(path, g, rule) is True


def test_check_width_ignores_same_net_cells():
    """A path cell adjacent to ITSELF doesn't count as blocker."""
    g = _FakeGrid(cell_mm=0.05,
                    blocked={('A', 11, 10), ('A', 12, 10)})
    # If we tag these as same-net via the path, they shouldn't fail.
    # Path includes (11,10) and (12,10).
    path = [('A', 10, 10), ('A', 11, 10), ('A', 12, 10), ('A', 13, 10)]
    rule = _Rule(width_mm=0.10)
    assert check_width(path, g, rule) is True


def test_check_width_subcell_width_passes_trivially():
    """width < cell_mm/2 → radius 0 → vacuous pass."""
    g = _FakeGrid(cell_mm=0.10)
    rule = _Rule(width_mm=0.02)  # half-width 0.01 → radius 0
    path = [('A', 5, 5)]
    assert check_width(path, g, rule) is True


def test_check_width_larger_radius_catches_distant_blocker():
    """Wider trace needs more room — radius grows, distant blocker
    suddenly counts as too close."""
    # Blocker at (13,10), path at (11,10). Distance dx=2.
    g = _FakeGrid(cell_mm=0.05, blocked={('A', 13, 10)})
    path = [('A', 11, 10)]
    # width 0.10 → half=0.05 → radius ceil(0.05/0.05)=1 cell → blocker out
    assert check_width(path, g, _Rule(width_mm=0.10)) is True
    # width 0.20 → half=0.10 → radius ceil(0.10/0.05)=2 cells → blocker in
    assert check_width(path, g, _Rule(width_mm=0.20)) is False
