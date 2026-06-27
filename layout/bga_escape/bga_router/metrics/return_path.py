# Return-path 품질 메트릭 — plane split crossing / continuity / 인접 PG via 거리
"""Phase C2 — return-path quality metrics.

These need plane geometry. When plane geometry is unavailable (no plane
layers discovered in the ODB++ source), each function falls back to a
'plane_geometry_not_loaded' result rather than failing.
"""

from __future__ import annotations

import math
from typing import Dict, Iterable, List, Optional, Tuple

from src.ecad.plane_loader import (
    PlaneGeometry,
    find_nearest_return_via_distance,
    is_point_inside_plane,
)

from .path_geometry import iter_segments_mm


XY = Tuple[float, float]


def _sample_segments(routed_paths, grid, samples_per_segment: int = 8
                       ) -> Dict[str, List[XY]]:
    """For each net, return a list of xy sample points along its same-layer
    segments. Layer transitions are skipped (return path is per-segment)."""
    out: Dict[str, List[XY]] = {}
    for net, pr in routed_paths.items():
        path = getattr(pr, 'path', None) if not isinstance(pr, list) else pr
        if not path:
            continue
        pts: List[XY] = []
        for _layer, x0, y0, x1, y1 in iter_segments_mm(path, grid):
            for k in range(samples_per_segment + 1):
                t = k / samples_per_segment
                pts.append((x0 + t * (x1 - x0), y0 + t * (y1 - y0)))
        if pts:
            out[net] = pts
    return out


def plane_split_crossings(routed_paths, grid, plane_geom: PlaneGeometry,
                            reference_layer: str) -> dict:
    """Count sample points that fall OUTSIDE the reference plane.
    'count' is the total across all nets; 'by_net' is per-net.
    """
    if not plane_geom or not plane_geom.layers:
        return {'plane_geometry_not_loaded': True}
    sampled = _sample_segments(routed_paths, grid)
    by_net: Dict[str, int] = {}
    total = 0
    for net, pts in sampled.items():
        miss = sum(1 for (x, y) in pts
                     if not is_point_inside_plane(plane_geom,
                                                   reference_layer, x, y))
        if miss:
            by_net[net] = miss
            total += miss
    return {'count': total, 'by_net': by_net}


def reference_plane_changes_no_stitch(routed_paths, grid,
                                        stitching_vias_xy: Iterable[XY],
                                        stitch_threshold_mm: float = 1.0
                                        ) -> dict:
    """For each layer transition, find the nearest stitching via.
    If > threshold, count as 'no_stitch'."""
    vias = list(stitching_vias_xy)
    by_net: Dict[str, int] = {}
    total = 0
    for net, pr in routed_paths.items():
        path = getattr(pr, 'path', None) if not isinstance(pr, list) else pr
        if not path:
            continue
        prev_layer = None
        prev_xy: Optional[XY] = None
        miss = 0
        for layer, ix, iy in path:
            x, y = grid.geom.cell_to_world(ix, iy)
            if prev_layer is not None and layer != prev_layer:
                # Via at this transition — find nearest stitch via.
                if vias:
                    d = min(math.hypot(x - vx, y - vy) for vx, vy in vias)
                    if d > stitch_threshold_mm:
                        miss += 1
                else:
                    miss += 1
            prev_layer = layer
            prev_xy = (x, y)
        if miss:
            by_net[net] = miss
            total += miss
    return {'count': total, 'by_net': by_net}


def return_path_continuity_score(routed_paths, grid,
                                   plane_geom: PlaneGeometry,
                                   reference_layer: str) -> float:
    """Length-weighted fraction of path that has the reference plane
    solid beneath. 1.0 = perfect, 0.0 = entirely over splits."""
    if not plane_geom or not plane_geom.layers:
        return 1.0
    total_len = 0.0
    bad_len = 0.0
    for net, pr in routed_paths.items():
        path = getattr(pr, 'path', None) if not isinstance(pr, list) else pr
        if not path:
            continue
        for _layer, x0, y0, x1, y1 in iter_segments_mm(path, grid):
            seg_len = math.hypot(x1 - x0, y1 - y0)
            total_len += seg_len
            # Sample 3 points along segment; if any is outside the plane,
            # mark the whole segment as a split-crossing.
            mid = ((x0 + x1) / 2, (y0 + y1) / 2)
            inside_all = (
                is_point_inside_plane(plane_geom, reference_layer, x0, y0)
                and is_point_inside_plane(plane_geom, reference_layer, *mid)
                and is_point_inside_plane(plane_geom, reference_layer, x1, y1))
            if not inside_all:
                bad_len += seg_len
    if total_len <= 0:
        return 1.0
    return max(0.0, 1.0 - bad_len / total_len)


def nearest_return_via_distance_mm(routed_paths, grid,
                                     power_ground_vias_xy: Iterable[XY]
                                     ) -> dict:
    """Per-net nearest distance to any PG via. Returns dict
    {'by_net': dict, 'mean_mm': float|None, 'max_mm': float|None}."""
    vias = list(power_ground_vias_xy)
    if not vias:
        return {'by_net': {}, 'mean_mm': None, 'max_mm': None}
    sampled = _sample_segments(routed_paths, grid)
    by_net: Dict[str, float] = {}
    for net, pts in sampled.items():
        d = find_nearest_return_via_distance(None, pts, vias)
        if d is not None:
            by_net[net] = round(d, 4)
    if not by_net:
        return {'by_net': {}, 'mean_mm': None, 'max_mm': None}
    vals = list(by_net.values())
    return {
        'by_net': by_net,
        'mean_mm': round(sum(vals) / len(vals), 4),
        'max_mm':  round(max(vals), 4),
    }


def summarize_return_path(routed_paths, grid,
                            plane_geom: PlaneGeometry,
                            power_ground_vias_xy: Iterable[XY],
                            stitching_vias_xy: Iterable[XY],
                            reference_layer: str = 'GND') -> dict:
    if not plane_geom or not plane_geom.layers:
        return {'plane_geometry_not_loaded': True}
    return {
        'plane_split_crossings':
            plane_split_crossings(routed_paths, grid, plane_geom,
                                    reference_layer),
        'reference_plane_changes_no_stitch':
            reference_plane_changes_no_stitch(routed_paths, grid,
                                                stitching_vias_xy),
        'return_path_continuity_score':
            return_path_continuity_score(routed_paths, grid, plane_geom,
                                           reference_layer),
        'nearest_return_via_distance_mm':
            nearest_return_via_distance_mm(routed_paths, grid,
                                             power_ground_vias_xy),
    }
