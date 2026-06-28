# 라우팅 경로의 길이 / via / 레이어 전이 등 순수 기하 계산을 모은 공용 헬퍼
"""Shared path-geometry helpers used by the metrics package and by the
existing post-processors (manufacturing_drc / length_match /
diff_pair / propagation_delay) which previously duplicated the same
``_path_length_mm`` implementation three times.

A path is a ``list[(layer:str, ix:int, iy:int)]`` of grid cells; mm
conversion uses ``grid.geom.cell_to_world(ix, iy)``. Vias are inferred
from layer transitions between consecutive cells.
"""

from __future__ import annotations

import math
from typing import Iterable, Iterator, Tuple


PathPoint = Tuple[str, int, int]              # (layer, ix, iy)
SegmentMm = Tuple[str, float, float, float, float]  # (layer, x0, y0, x1, y1)


def iter_segments_mm(path: Iterable[PathPoint], grid) -> Iterator[SegmentMm]:
    """Yield consecutive same-layer segments in mm world coordinates.

    A layer transition (via) is skipped — it contributes no mm length to
    the trace; it is counted separately via ``count_vias`` /
    ``count_layer_transitions``.
    """
    prev_layer = None
    prev_xy = None
    for layer, ix, iy in path:
        x, y = grid.geom.cell_to_world(ix, iy)
        if prev_layer is not None and prev_layer == layer:
            px, py = prev_xy
            yield (layer, px, py, x, y)
        prev_layer = layer
        prev_xy = (x, y)


def path_length_mm(path: Iterable[PathPoint], grid) -> float:
    """Total in-plane length, summed across all layers (vias excluded)."""
    total = 0.0
    for _layer, x0, y0, x1, y1 in iter_segments_mm(path, grid):
        total += math.hypot(x1 - x0, y1 - y0)
    return total


def length_per_layer_mm(path: Iterable[PathPoint], grid) -> dict[str, float]:
    """Per-layer trace length in mm. Layer absent in path → not present."""
    out: dict[str, float] = {}
    for layer, x0, y0, x1, y1 in iter_segments_mm(path, grid):
        out[layer] = out.get(layer, 0.0) + math.hypot(x1 - x0, y1 - y0)
    return out


def count_vias(path: Iterable[PathPoint]) -> int:
    """Count layer transitions in the path. One transition = one via."""
    n = 0
    prev_layer = None
    for layer, _ix, _iy in path:
        if prev_layer is not None and prev_layer != layer:
            n += 1
        prev_layer = layer
    return n


# Alias — semantically the same metric in current router (one via per
# transition, no stacked-via accounting yet).
count_layer_transitions = count_vias


# ---------------------------------------------------------------------------
# Phase D-6 — via metadata derivation from path + net classification
# ---------------------------------------------------------------------------


_PG_PREFIXES = ('GND', 'VSS', 'VDD', 'VCC', 'PWR', 'PG')


def is_power_ground_net(net_name: str) -> bool:
    """Heuristic PG net classification by name prefix / contains.

    Returns True if name starts with or equals a PG keyword token.
    Matches common conventions: 'GND', 'GND_DIG', 'VDD_CORE', 'VCC18',
    'VSS_AVDD', 'PWR_RAIL_1V8'. Whole-string 'GND' / 'VDD' also caught.
    """
    if not net_name:
        return False
    up = net_name.upper()
    for pref in _PG_PREFIXES:
        if up == pref or up.startswith(pref + '_') or up.startswith(pref):
            # Disambiguate accidental prefix match like 'GNDPULL' — strict:
            # at least require the next char (if any) to be _, digit, or end
            if len(up) == len(pref):
                return True
            nxt = up[len(pref)]
            if nxt in '_' or nxt.isdigit():
                return True
    return False


def derive_via_metadata(path, grid, net_name: str) -> list:
    """Return per-transition via metadata derived from path geometry +
    net classification. Used by eval-time pipeline to populate
    PathResult.via_metadata when the router itself didn't.

    Each entry: {'xy_mm': (x,y), 'start_layer': str, 'end_layer': str,
                 'kind': 'signal' | 'power_ground'}.
    """
    if not path:
        return []
    kind = 'power_ground' if is_power_ground_net(net_name) else 'signal'
    out: list = []
    prev_layer = None
    prev_ix = prev_iy = None
    for layer, ix, iy in path:
        if prev_layer is not None and prev_layer != layer:
            x, y = grid.geom.cell_to_world(ix, iy)
            out.append({
                'xy_mm':       (x, y),
                'start_layer': prev_layer,
                'end_layer':   layer,
                'kind':        kind,
            })
        prev_layer = layer
        prev_ix, prev_iy = ix, iy
    return out


def collect_pg_via_xy(routed_paths, grid) -> list:
    """Aggregate (x, y) mm for every PG-net via across the design.
    Used as input to summarize_return_path's power_ground_vias_xy arg."""
    pts: list = []
    for net, pr in routed_paths.items():
        path = getattr(pr, 'path', None) if not isinstance(pr, list) else pr
        if not path or not is_power_ground_net(net):
            continue
        for v in derive_via_metadata(path, grid, net):
            pts.append(v['xy_mm'])
    return pts


def collect_stitching_via_xy(routed_paths, grid) -> list:
    """Same as collect_pg_via_xy — in this router stitching vias are
    just PG vias placed to provide a return path. Separate function so
    callers can swap in a different policy later (e.g. only vias within
    a band around signal-layer transitions count as stitches)."""
    return collect_pg_via_xy(routed_paths, grid)
