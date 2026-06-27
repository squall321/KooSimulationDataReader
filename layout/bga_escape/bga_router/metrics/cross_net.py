# 라우팅 전체 net 집합을 가로지르는 혼잡도 / 그림자 교차 / 레이어 사용률 메트릭
"""Cross-net geometric metrics.

These metrics need the *full* set of routed paths (not just one) — they
quantify how nets interact spatially:

* ``channel_congestion``  — max distinct nets sharing any (layer, ix, iy)
  cell. >1 means multiple nets land on the same cell on the same layer
  (a hard short if the router didn't insert a via or stagger).
* ``shadow_crossings_2d`` — 2D segment crossings ignoring layer.
  Different-net segments that overlap in xy projection count as one
  crossing; if either net later swaps layers at that xy point those
  nets would touch.
* ``layer_utilization``   — per-layer fraction of grid cells touched.

A path is ``list[(layer:str, ix:int, iy:int)]``; mm conversion uses
``grid.geom.cell_to_world(ix, iy)`` exactly like
``bga_router.metrics.path_geometry``.

``routed_paths`` is the ``dict[net_name → PathResult]`` carried on
``RecipeResult``; ``PathResult.path`` is the cell list. We accept either
form (object-with-.path, or raw list) defensively, matching the pattern
in ``bga_router/eval.py``.
"""

from __future__ import annotations

from typing import Any

from bga_router.metrics.path_geometry import iter_segments_mm


def _extract_path(path_or_result: Any) -> list | None:
    """Normalize: either a PathResult (with .path) or a raw cell list."""
    if path_or_result is None:
        return None
    inner = getattr(path_or_result, 'path', None)
    if inner is not None:
        return inner
    # Fallback — caller passed the raw list directly.
    return path_or_result


def _grid_nx_ny(grid) -> tuple[int, int]:
    """Return (nx, ny) from grid.geom, deriving if attributes missing."""
    geom = grid.geom
    nx = getattr(geom, 'nx', None)
    ny = getattr(geom, 'ny', None)
    if nx is not None and ny is not None:
        return nx, ny
    # Fallback — derive from bounds + cell_mm.
    nx = int(round((geom.x_max_mm - geom.x_min_mm) / geom.cell_mm))
    ny = int(round((geom.y_max_mm - geom.y_min_mm) / geom.cell_mm))
    return nx, ny


def channel_congestion(routed_paths, grid) -> dict:
    """Max number of distinct nets occupying any single (layer, ix, iy) cell.

    Returns ``{'max_congestion': int, 'at_cell': (layer, ix, iy)}``.
    Empty input → ``{'max_congestion': 0, 'at_cell': None}``.
    """
    cell_nets: dict[tuple, set[str]] = {}
    for net_name, path_or_result in routed_paths.items():
        path = _extract_path(path_or_result)
        if not path:
            continue
        # Distinct cells visited by this net (avoid double-counting if the
        # same net revisits a cell — we want "distinct nets per cell").
        seen_this_net: set[tuple] = set()
        for layer, ix, iy in path:
            key = (layer, ix, iy)
            if key in seen_this_net:
                continue
            seen_this_net.add(key)
            cell_nets.setdefault(key, set()).add(net_name)

    if not cell_nets:
        return {'max_congestion': 0, 'at_cell': None}

    best_cell, best_set = max(cell_nets.items(), key=lambda kv: len(kv[1]))
    return {'max_congestion': len(best_set), 'at_cell': best_cell}


def _segments_intersect(
    x1: float, y1: float, x2: float, y2: float,
    x3: float, y3: float, x4: float, y4: float,
) -> bool:
    """Proper 2D segment intersection test.

    Returns True iff segments (p1,p2) and (p3,p4) share a point. Endpoint
    touches count as intersections (a Y-junction between two different
    nets is still a contact in xy).
    """
    def ccw(ax, ay, bx, by, cx, cy):
        return (bx - ax) * (cy - ay) - (by - ay) * (cx - ax)

    d1 = ccw(x3, y3, x4, y4, x1, y1)
    d2 = ccw(x3, y3, x4, y4, x2, y2)
    d3 = ccw(x1, y1, x2, y2, x3, y3)
    d4 = ccw(x1, y1, x2, y2, x4, y4)

    if ((d1 > 0 and d2 < 0) or (d1 < 0 and d2 > 0)) and \
       ((d3 > 0 and d4 < 0) or (d3 < 0 and d4 > 0)):
        return True

    # Collinear / endpoint-touch cases.
    def on_segment(ax, ay, bx, by, cx, cy):
        return (min(ax, bx) <= cx <= max(ax, bx)
                and min(ay, by) <= cy <= max(ay, by))

    if d1 == 0 and on_segment(x3, y3, x4, y4, x1, y1):
        return True
    if d2 == 0 and on_segment(x3, y3, x4, y4, x2, y2):
        return True
    if d3 == 0 and on_segment(x1, y1, x2, y2, x3, y3):
        return True
    if d4 == 0 and on_segment(x1, y1, x2, y2, x4, y4):
        return True
    return False


def shadow_crossings_2d(routed_paths, grid) -> int:
    """Count xy-plane segment crossings between segments of *different* nets.

    Layer is ignored — the "shadow" projection onto xy is what we test.
    Same-net segments (turns, overlaps within one trace) are not counted.

    Implementation: O(N²) worst case over all segments, with a precomputed
    AABB quick-reject before the exact intersection test. On real BGA data
    the bboxes rarely overlap, so the inner check fires on a tiny fraction
    of pairs and the wall-clock cost stays well under a second for a 16-net
    board with a few thousand segments.
    """
    # Collect (net_name, x0, y0, x1, y1, xmin, ymin, xmax, ymax).
    segs: list = []
    for net_name, path_or_result in routed_paths.items():
        path = _extract_path(path_or_result)
        if not path:
            continue
        for _layer, x0, y0, x1, y1 in iter_segments_mm(path, grid):
            xmin = x0 if x0 < x1 else x1
            xmax = x0 if x0 > x1 else x1
            ymin = y0 if y0 < y1 else y1
            ymax = y0 if y0 > y1 else y1
            segs.append((net_name, x0, y0, x1, y1, xmin, ymin, xmax, ymax))

    count = 0
    n = len(segs)
    for i in range(n):
        ni, x1, y1, x2, y2, axmin, aymin, axmax, aymax = segs[i]
        for j in range(i + 1, n):
            nj, x3, y3, x4, y4, bxmin, bymin, bxmax, bymax = segs[j]
            if ni == nj:
                continue
            # AABB quick reject — same-segment-length cost as one
            # subtraction-and-compare, ~10× faster than full intersect.
            if axmax < bxmin or bxmax < axmin:
                continue
            if aymax < bymin or bymax < aymin:
                continue
            if _segments_intersect(x1, y1, x2, y2, x3, y3, x4, y4):
                count += 1
    return count


def layer_utilization(routed_paths, grid) -> dict[str, float]:
    """Per-layer fraction of grid cells touched by any routed path.

    Total cells per layer = ``grid.geom.nx * grid.geom.ny``. Returns
    ``{layer_name: fraction}`` for every layer that appears in any path.
    """
    nx, ny = _grid_nx_ny(grid)
    total = nx * ny
    if total <= 0:
        return {}

    touched: dict[str, set[tuple[int, int]]] = {}
    for _net_name, path_or_result in routed_paths.items():
        path = _extract_path(path_or_result)
        if not path:
            continue
        for layer, ix, iy in path:
            touched.setdefault(layer, set()).add((ix, iy))

    return {layer: len(cells) / total for layer, cells in touched.items()}


def summarize_cross_net(routed_paths, grid) -> dict:
    """Aggregate cross-net metrics into a single JSON-serializable dict."""
    cong = channel_congestion(routed_paths, grid)
    at = cong['at_cell']
    return {
        'channel_congestion_max':     cong['max_congestion'],
        'channel_congestion_at_cell': [at[0], at[1], at[2]] if at else None,
        'shadow_crossings_2d':        shadow_crossings_2d(routed_paths, grid),
        'layer_utilization':          layer_utilization(routed_paths, grid),
    }
