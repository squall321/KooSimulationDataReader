# bga_router.metrics.cross_net 다중 net 기하 메트릭 단위 테스트
"""Synthetic fixtures for cross-net metrics.

We avoid ODB++ fixtures and build minimal grids by hand. Each test
constructs the smallest routed_paths dict that exercises one metric.
"""

from __future__ import annotations

import pytest

from bga_router.metrics.cross_net import (
    channel_congestion,
    shadow_crossings_2d,
    layer_utilization,
    summarize_cross_net,
)


class _FakeGeom:
    def __init__(self, cell_mm: float = 0.1, nx: int = 100, ny: int = 100):
        self.cell_mm = cell_mm
        self.nx = nx
        self.ny = ny
        self.x_min_mm = 0.0
        self.y_min_mm = 0.0
        self.x_max_mm = nx * cell_mm
        self.y_max_mm = ny * cell_mm

    def cell_to_world(self, ix: int, iy: int) -> tuple[float, float]:
        return ix * self.cell_mm, iy * self.cell_mm


class _FakeGrid:
    def __init__(self, cell_mm: float = 0.1, nx: int = 100, ny: int = 100):
        self.geom = _FakeGeom(cell_mm, nx, ny)


def test_two_nets_no_crossing_max_congestion_is_one():
    """Two nets on disjoint cells on the same layer → no cell ever has 2 nets."""
    grid = _FakeGrid()
    routed = {
        'NET_A': [('COMP', 0, 0), ('COMP', 1, 0), ('COMP', 2, 0)],
        'NET_B': [('COMP', 0, 5), ('COMP', 1, 5), ('COMP', 2, 5)],
    }
    out = channel_congestion(routed, grid)
    assert out['max_congestion'] == 1


def test_two_nets_no_crossing_shadow_crossings_zero():
    grid = _FakeGrid()
    routed = {
        'NET_A': [('COMP', 0, 0), ('COMP', 10, 0)],
        'NET_B': [('COMP', 0, 5), ('COMP', 10, 5)],
    }
    assert shadow_crossings_2d(routed, grid) == 0


def test_two_nets_share_cell_gives_congestion_two():
    """Two nets landing on (COMP, 5, 5) → max_congestion=2 at that cell."""
    grid = _FakeGrid()
    routed = {
        'NET_A': [('COMP', 0, 5), ('COMP', 5, 5), ('COMP', 10, 5)],
        'NET_B': [('COMP', 5, 0), ('COMP', 5, 5), ('COMP', 5, 10)],
    }
    out = channel_congestion(routed, grid)
    assert out['max_congestion'] == 2
    assert out['at_cell'] == ('COMP', 5, 5)


def test_right_angle_crossing_counts_one():
    """Two segments at 90 degrees crossing at one interior point → 1 crossing."""
    grid = _FakeGrid()
    routed = {
        # Horizontal segment on COMP from (0,5) to (10,5)
        'NET_A': [('COMP', 0, 5), ('COMP', 10, 5)],
        # Vertical segment on LAY2 from (5,0) to (5,10) — different layer
        # but shadow_crossings ignores layer.
        'NET_B': [('LAY2', 5, 0), ('LAY2', 5, 10)],
    }
    assert shadow_crossings_2d(routed, grid) == 1


def test_layer_utilization_one_net_ten_cells_on_100x100():
    """10 distinct cells on a 100x100 grid (10000 total) → 0.001."""
    grid = _FakeGrid(cell_mm=0.1, nx=100, ny=100)
    routed = {
        'NET_A': [('COMP', i, 0) for i in range(10)],
    }
    util = layer_utilization(routed, grid)
    assert util == {'COMP': pytest.approx(0.001, abs=1e-12)}


def test_summarize_cross_net_returns_json_serializable():
    """Smoke: summarize wraps the others; at_cell becomes a list, not tuple."""
    grid = _FakeGrid()
    routed = {
        'NET_A': [('COMP', 0, 5), ('COMP', 5, 5), ('COMP', 10, 5)],
        'NET_B': [('COMP', 5, 0), ('COMP', 5, 5), ('COMP', 5, 10)],
    }
    s = summarize_cross_net(routed, grid)
    assert s['channel_congestion_max'] == 2
    assert s['channel_congestion_at_cell'] == ['COMP', 5, 5]
    assert isinstance(s['layer_utilization'], dict)
    assert isinstance(s['shadow_crossings_2d'], int)


def test_empty_routed_paths():
    grid = _FakeGrid()
    assert channel_congestion({}, grid) == {
        'max_congestion': 0, 'at_cell': None}
    assert shadow_crossings_2d({}, grid) == 0
    assert layer_utilization({}, grid) == {}


def test_path_result_object_with_path_attribute():
    """Accept PathResult-shaped objects (.path attribute), per eval.py pattern."""
    grid = _FakeGrid()

    class _PR:
        def __init__(self, path):
            self.path = path

    routed = {
        'NET_A': _PR([('COMP', 0, 0), ('COMP', 10, 0)]),
        'NET_B': _PR([('COMP', 5, 0), ('COMP', 5, 10)]),
    }
    assert shadow_crossings_2d(routed, grid) == 1
