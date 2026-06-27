# return-path 메트릭 단위 테스트 — split crossing / continuity / 인접 PG via 거리
"""Phase C2 — return-path tests."""

from __future__ import annotations

from dataclasses import dataclass
from typing import Optional, Tuple

import pytest

from bga_router.metrics.return_path import (
    nearest_return_via_distance_mm,
    plane_split_crossings,
    reference_plane_changes_no_stitch,
    return_path_continuity_score,
    summarize_return_path,
)
from src.ecad.plane_loader import (
    PlaneGeometry,
    PlaneLayer,
    PlanePolygon,
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


def _solid_plane(layer_name: str = 'GND') -> PlaneGeometry:
    """20x20 mm solid plane covering origin region."""
    p = PlanePolygon(
        layer_name=layer_name,
        net_name='GND',
        outer_xy_mm=((0, 0), (20, 0), (20, 20), (0, 20)),
        holes_xy_mm=(),
        is_positive=True,
    )
    return PlaneGeometry(
        layers={layer_name: PlaneLayer(layer_name=layer_name, polygons=(p,))},
        units_mm=True)


def _split_plane(layer_name: str = 'GND') -> PlaneGeometry:
    """Two disjoint 5x5 squares with a gap from x=5 to x=15."""
    p1 = PlanePolygon(layer_name, 'GND1',
                       ((0, 0), (5, 0), (5, 5), (0, 5)), (), True)
    p2 = PlanePolygon(layer_name, 'GND2',
                       ((15, 0), (20, 0), (20, 5), (15, 5)), (), True)
    return PlaneGeometry(
        layers={layer_name: PlaneLayer(layer_name, (p1, p2))},
        units_mm=True)


# ---------------------------------------------------------------------------
# plane_geometry_not_loaded fallback
# ---------------------------------------------------------------------------


def test_summarize_returns_not_loaded_when_no_planes():
    g = _FakeGrid()
    empty = PlaneGeometry(layers={}, units_mm=True)
    out = summarize_return_path({}, g, empty, [], [], 'GND')
    assert out == {'plane_geometry_not_loaded': True}


def test_plane_split_crossings_not_loaded_flag():
    g = _FakeGrid()
    empty = PlaneGeometry(layers={}, units_mm=True)
    out = plane_split_crossings({}, g, empty, 'GND')
    assert out['plane_geometry_not_loaded'] is True


# ---------------------------------------------------------------------------
# Continuity score
# ---------------------------------------------------------------------------


def test_continuity_score_full_over_solid_plane():
    g = _FakeGrid(cell_mm=0.1)
    plane = _solid_plane()
    # Path from (1,1) to (5,1) mm, entirely inside the 20x20 plane.
    routed = {'A': _PR([('LAY2', 10, 10), ('LAY2', 50, 10)])}
    s = return_path_continuity_score(routed, g, plane, 'GND')
    assert s == pytest.approx(1.0, abs=1e-9)


def test_continuity_score_drops_over_split():
    g = _FakeGrid(cell_mm=0.1)
    plane = _split_plane()
    # Path from (1,1) → (19,1): starts inside p1, crosses gap, ends in p2
    routed = {'A': _PR([('LAY2', 10, 10), ('LAY2', 190, 10)])}
    s = return_path_continuity_score(routed, g, plane, 'GND')
    assert s < 1.0


# ---------------------------------------------------------------------------
# plane_split_crossings
# ---------------------------------------------------------------------------


def test_plane_split_crossings_zero_on_solid():
    g = _FakeGrid(cell_mm=0.1)
    plane = _solid_plane()
    routed = {'A': _PR([('LAY2', 10, 10), ('LAY2', 50, 10)])}
    out = plane_split_crossings(routed, g, plane, 'GND')
    assert out['count'] == 0


def test_plane_split_crossings_nonzero_over_gap():
    g = _FakeGrid(cell_mm=0.1)
    plane = _split_plane()
    routed = {'A': _PR([('LAY2', 10, 10), ('LAY2', 190, 10)])}
    out = plane_split_crossings(routed, g, plane, 'GND')
    assert out['count'] > 0
    assert 'A' in out['by_net']


# ---------------------------------------------------------------------------
# reference_plane_changes_no_stitch
# ---------------------------------------------------------------------------


def test_no_stitch_count_zero_when_no_layer_change():
    g = _FakeGrid()
    routed = {'A': _PR([('LAY2', 0, 0), ('LAY2', 10, 0)])}
    out = reference_plane_changes_no_stitch(routed, g, [], 1.0)
    assert out['count'] == 0


def test_no_stitch_count_increments_on_via_without_nearby_stitch():
    g = _FakeGrid(cell_mm=0.1)
    routed = {'A': _PR([('LAY2', 0, 0), ('LAY3', 0, 0), ('LAY3', 10, 0)])}
    # Via at (0,0); stitching via far away → no_stitch++
    out = reference_plane_changes_no_stitch(routed, g, [(100.0, 100.0)], 1.0)
    assert out['count'] == 1


def test_no_stitch_count_zero_when_stitch_nearby():
    g = _FakeGrid(cell_mm=0.1)
    routed = {'A': _PR([('LAY2', 0, 0), ('LAY3', 0, 0), ('LAY3', 10, 0)])}
    # Via at (0,0); stitching via 0.5 mm away → within threshold
    out = reference_plane_changes_no_stitch(routed, g, [(0.5, 0.0)], 1.0)
    assert out['count'] == 0


# ---------------------------------------------------------------------------
# nearest_return_via_distance_mm
# ---------------------------------------------------------------------------


def test_nearest_pg_via_distance_basic():
    """8 samples on a 1.0 mm segment → step 0.125 mm. Place a via at
    (0.0, 0.5) and the closest sample point is the start (0,0), giving
    distance 0.5."""
    g = _FakeGrid(cell_mm=0.1)
    routed = {'A': _PR([('LAY2', 0, 0), ('LAY2', 10, 0)])}
    pg_vias = [(0.0, 0.5)]
    out = nearest_return_via_distance_mm(routed, g, pg_vias)
    assert out['by_net']['A'] == pytest.approx(0.5, abs=1e-9)
    assert out['mean_mm'] == out['max_mm']


def test_nearest_pg_via_no_vias_returns_nones():
    g = _FakeGrid()
    out = nearest_return_via_distance_mm({'A': _PR([])}, g, [])
    assert out['by_net'] == {}
    assert out['mean_mm'] is None
    assert out['max_mm'] is None
