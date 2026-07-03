# DFT (Design-for-Test) 메트릭 — test point 후보 / probe 접근성 / AOI 가시성
"""Phase H-7 — test-point coverage / AOI DFT metrics.

Real EMS lines need:
  1. **ICT test-point coverage** — every net should expose at least one
     probe-able point (outer layer, ≥ 0.9 mm from neighbours, ≥ 1 mm
     from tall parts). We approximate with outer-layer path cells that
     have open surroundings.
  2. **AOI visibility** — nets fully buried (no outer-layer segment)
     are invisible to Automated Optical Inspection.
  3. **Probe pitch** — two candidate test points closer than the probe
     pitch (1.27 mm / 50 mil standard) can't both be probed.

These are approximations at the routing level; final DFT sign-off
needs part height + fixture data we don't model.
"""

from __future__ import annotations

import math
from typing import Any, Dict, Iterable, List, Optional, Tuple

from .path_geometry import iter_segments_mm


DEFAULT_PROBE_CLEAR_MM = 0.9      # min spacing to neighbouring copper
DEFAULT_PROBE_PITCH_MM = 1.27     # 50 mil standard bed-of-nails pitch


def _outer_layers(routed_paths) -> Tuple[str, ...]:
    """Heuristic — outermost = first/last layer names seen in paths.
    Callers can override via outer_layers arg of summarize_dft."""
    seen: List[str] = []
    for pr in routed_paths.values():
        path = getattr(pr, 'path', None) or []
        for layer, _ix, _iy in path:
            if layer not in seen:
                seen.append(layer)
    if not seen:
        return ()
    # Assume COMP / *top* / first-seen is outer; explicit list preferred.
    outer = [L for L in seen
              if 'comp' in L.lower() or 'top' in L.lower()
              or 'bot' in L.lower() or 'sold' in L.lower()]
    return tuple(outer) if outer else (seen[0],)


def candidate_test_points(routed_paths, grid, *,
                            outer_layers: Optional[Iterable[str]] = None,
                            min_clear_mm: float = DEFAULT_PROBE_CLEAR_MM
                            ) -> Dict[str, List[Tuple[float, float]]]:
    """Per net, xy positions on outer layers with enough clearance.
    Clearance test: no OTHER net's path point within min_clear_mm."""
    outer = tuple(outer_layers) if outer_layers else _outer_layers(routed_paths)
    outer_set = set(outer)

    # Gather every outer-layer point per net (in mm)
    pts_by_net: Dict[str, List[Tuple[float, float]]] = {}
    all_pts: List[Tuple[str, float, float]] = []
    for net, pr in routed_paths.items():
        path = getattr(pr, 'path', None) or []
        pts: List[Tuple[float, float]] = []
        for layer, ix, iy in path:
            if layer not in outer_set:
                continue
            x, y = grid.geom.cell_to_world(ix, iy)
            pts.append((x, y))
            all_pts.append((net, x, y))
        if pts:
            pts_by_net[net] = pts

    # Clearance filter — brute force acceptable at PoC scale.
    out: Dict[str, List[Tuple[float, float]]] = {}
    for net, pts in pts_by_net.items():
        good: List[Tuple[float, float]] = []
        for (x, y) in pts:
            clear = True
            for other_net, ox, oy in all_pts:
                if other_net == net:
                    continue
                if math.hypot(x - ox, y - oy) < min_clear_mm:
                    clear = False
                    break
            if clear:
                good.append((round(x, 4), round(y, 4)))
        if good:
            out[net] = good
    return out


def probe_pitch_conflicts(test_points: Dict[str, List[Tuple[float, float]]],
                            *, pitch_mm: float = DEFAULT_PROBE_PITCH_MM
                            ) -> List[Dict[str, Any]]:
    """Pairs of candidate points (different nets) closer than probe pitch."""
    flat: List[Tuple[str, float, float]] = []
    for net, pts in test_points.items():
        # One representative point per net (first candidate) — bed-of-nails
        # only needs ONE probe per net.
        if pts:
            x, y = pts[0]
            flat.append((net, x, y))
    conflicts: List[Dict[str, Any]] = []
    for i in range(len(flat)):
        for j in range(i + 1, len(flat)):
            na, xa, ya = flat[i]
            nb, xb, yb = flat[j]
            d = math.hypot(xa - xb, ya - yb)
            if d < pitch_mm:
                conflicts.append({
                    'nets': [na, nb],
                    'distance_mm': round(d, 4),
                })
    return conflicts


def aoi_visibility(routed_paths, *,
                     outer_layers: Optional[Iterable[str]] = None
                     ) -> Dict[str, bool]:
    """Per net — True iff at least one path segment is on an outer layer."""
    outer = tuple(outer_layers) if outer_layers else _outer_layers(routed_paths)
    outer_set = set(outer)
    out: Dict[str, bool] = {}
    for net, pr in routed_paths.items():
        path = getattr(pr, 'path', None) or []
        out[net] = any(layer in outer_set for layer, _ix, _iy in path)
    return out


def summarize_dft(routed_paths, grid, *,
                    outer_layers: Optional[Iterable[str]] = None,
                    min_clear_mm: float = DEFAULT_PROBE_CLEAR_MM,
                    pitch_mm: float = DEFAULT_PROBE_PITCH_MM
                    ) -> Dict[str, Any]:
    tps = candidate_test_points(routed_paths, grid,
                                   outer_layers=outer_layers,
                                   min_clear_mm=min_clear_mm)
    visibility = aoi_visibility(routed_paths, outer_layers=outer_layers)
    conflicts = probe_pitch_conflicts(tps, pitch_mm=pitch_mm)
    total = len(routed_paths)
    covered = len(tps)
    return {
        'test_point_coverage_pct':  round(covered / total * 100.0, 2)
                                     if total else 0.0,
        'nets_with_test_point':     covered,
        'nets_without_test_point':  sorted(n for n in routed_paths
                                             if n not in tps),
        'aoi_visible_count':        sum(1 for v in visibility.values() if v),
        'aoi_buried_nets':          sorted(n for n, v in visibility.items()
                                             if not v),
        'probe_pitch_conflicts':    conflicts,
        'thresholds': {
            'min_clear_mm': min_clear_mm,
            'pitch_mm':     pitch_mm,
        },
    }
