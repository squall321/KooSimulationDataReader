# bga_router.metrics.path_geometry 공용 헬퍼 합성 경로 단위 테스트
"""Phase A — geometry helper unit tests.

Synthetic 4-point paths where the expected length / via_count is
computed by hand and checked to 1e-9. No ODB++ fixtures needed.
"""

from __future__ import annotations

import math

import pytest

from bga_router.metrics.path_geometry import (
    path_length_mm,
    length_per_layer_mm,
    count_vias,
    count_layer_transitions,
    iter_segments_mm,
)


class _FakeGeom:
    """Minimal grid.geom stand-in: cell_mm uniform, origin at (0,0)."""
    def __init__(self, cell_mm: float = 0.1):
        self.cell_mm = cell_mm

    def cell_to_world(self, ix: int, iy: int) -> tuple[float, float]:
        return ix * self.cell_mm, iy * self.cell_mm


class _FakeGrid:
    def __init__(self, cell_mm: float = 0.1):
        self.geom = _FakeGeom(cell_mm)


def test_path_length_mm_pure_x_segment():
    grid = _FakeGrid(cell_mm=0.1)
    path = [('COMP', 0, 0), ('COMP', 10, 0)]  # 1.0 mm
    assert path_length_mm(path, grid) == pytest.approx(1.0, abs=1e-9)


def test_path_length_mm_diagonal_segment():
    grid = _FakeGrid(cell_mm=0.05)
    # (0,0) → (10,10) on grid = (0,0) → (0.5, 0.5) mm
    path = [('COMP', 0, 0), ('COMP', 10, 10)]
    assert path_length_mm(path, grid) == pytest.approx(math.hypot(0.5, 0.5),
                                                         abs=1e-9)


def test_path_length_mm_skips_via_segments():
    """Layer transitions are zero-length in plane, must not be counted."""
    grid = _FakeGrid(cell_mm=0.1)
    path = [
        ('COMP', 0, 0),       # start
        ('COMP', 10, 0),      # +1.0 mm on COMP
        ('LAY2', 10, 0),      # via — same xy, layer change → SKIP
        ('LAY2', 10, 10),     # +1.0 mm on LAY2
    ]
    # Expected: 1.0 + 1.0 = 2.0 (via contributes 0)
    assert path_length_mm(path, grid) == pytest.approx(2.0, abs=1e-9)


def test_length_per_layer_splits_correctly():
    grid = _FakeGrid(cell_mm=0.1)
    path = [
        ('COMP', 0, 0),
        ('COMP', 10, 0),      # COMP: 1.0
        ('LAY2', 10, 0),
        ('LAY2', 10, 20),     # LAY2: 2.0
        ('LAY4', 10, 20),
        ('LAY4', 20, 20),     # LAY4: 1.0
    ]
    per_layer = length_per_layer_mm(path, grid)
    assert per_layer['COMP'] == pytest.approx(1.0, abs=1e-9)
    assert per_layer['LAY2'] == pytest.approx(2.0, abs=1e-9)
    assert per_layer['LAY4'] == pytest.approx(1.0, abs=1e-9)
    # Sum equals total
    assert sum(per_layer.values()) == pytest.approx(
        path_length_mm(path, grid), abs=1e-9)


def test_count_vias_counts_each_layer_transition():
    path = [
        ('COMP', 0, 0),
        ('COMP', 1, 0),
        ('LAY2', 1, 0),       # 1st transition
        ('LAY2', 1, 1),
        ('LAY4', 1, 1),       # 2nd transition
        ('COMP', 1, 1),       # 3rd transition (back up)
    ]
    assert count_vias(path) == 3


def test_count_layer_transitions_is_alias():
    path = [('A', 0, 0), ('B', 0, 0), ('A', 0, 0)]
    assert count_layer_transitions(path) == count_vias(path) == 2


def test_empty_path_returns_zero():
    grid = _FakeGrid()
    assert path_length_mm([], grid) == 0.0
    assert length_per_layer_mm([], grid) == {}
    assert count_vias([]) == 0


def test_single_point_path():
    grid = _FakeGrid()
    path = [('COMP', 5, 5)]
    assert path_length_mm(path, grid) == 0.0
    assert count_vias(path) == 0


def test_iter_segments_mm_yields_only_same_layer():
    grid = _FakeGrid(cell_mm=0.1)
    path = [
        ('COMP', 0, 0),
        ('COMP', 10, 0),
        ('LAY2', 10, 0),
        ('LAY2', 10, 10),
    ]
    segs = list(iter_segments_mm(path, grid))
    assert len(segs) == 2  # 1 on COMP, 1 on LAY2; via not yielded
    assert segs[0][0] == 'COMP'
    assert segs[1][0] == 'LAY2'
