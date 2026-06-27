# 라우팅 경로의 bend / detour / escape-side 등 형상 품질 지표 (순수 함수)
"""Path-only and path+task geometric quality metrics.

All functions are pure — no I/O, no side effects. A *path* is a
``list[(layer:str, ix:int, iy:int)]`` of grid cells, same shape as in
``bga_router.metrics.path_geometry``.

Public API:
    count_bends, bend_density_per_mm, bbox_straightness, detour_count,
    routing_efficiency, escape_side_fidelity, summarize_geometry.

Conventions:
    - Bends are evaluated only on consecutive *same-layer* triples; via
      transitions break the bend chain.
    - Strict inequalities are used for angle thresholds (90° itself is
      NOT counted as "acute", 135° itself is NOT "sharp"). Documented
      per-function.
"""

from __future__ import annotations

import math
from typing import Iterable, Sequence, Tuple

from bga_router.metrics.path_geometry import (
    path_length_mm,
    length_per_layer_mm,  # re-export-friendly; not used directly here
    count_vias,           # re-export-friendly; not used directly here
    iter_segments_mm,
)


PathPoint = Tuple[str, int, int]


# --------------------------------------------------------------------------- #
# 1) Bend counts                                                              #
# --------------------------------------------------------------------------- #

def _angle_deg(ax: float, ay: float, bx: float, by: float,
               cx: float, cy: float) -> float:
    """Return interior angle at point B (in degrees) of A-B-C polyline.

    180° = perfectly straight, 90° = right angle, 0° = full reverse.
    Returns 180.0 when either segment has zero length (degenerate).
    """
    v1x, v1y = bx - ax, by - ay
    v2x, v2y = cx - bx, cy - by
    n1 = math.hypot(v1x, v1y)
    n2 = math.hypot(v2x, v2y)
    if n1 == 0.0 or n2 == 0.0:
        return 180.0
    cos = (v1x * v2x + v1y * v2y) / (n1 * n2)
    # Interior angle = 180 - turn_angle. turn_angle = acos(cos).
    cos = max(-1.0, min(1.0, cos))
    turn = math.degrees(math.acos(cos))
    return 180.0 - turn


def count_bends(path: Sequence[PathPoint], grid) -> dict:
    """Count bends in the path, classified by sharpness.

    Returns dict with three counters:
      - ``sharp_bends``: angle < 135°  (turn sharper than 45°)
      - ``acute_bends``: angle <  90°  (turn sharper than 90°; subset of sharp)
      - ``total_bends``: angle < 179°  (anything not approximately straight)

    Triples that span a layer transition (via) are skipped.
    """
    sharp = 0
    acute = 0
    total = 0
    n = len(path)
    for i in range(n - 2):
        la, ia, ja = path[i]
        lb, ib, jb = path[i + 1]
        lc, ic, jc = path[i + 2]
        if la != lb or lb != lc:
            continue
        ax, ay = grid.geom.cell_to_world(ia, ja)
        bx, by = grid.geom.cell_to_world(ib, jb)
        cx, cy = grid.geom.cell_to_world(ic, jc)
        ang = _angle_deg(ax, ay, bx, by, cx, cy)
        if ang < 179.0:
            total += 1
        if ang < 135.0:
            sharp += 1
        if ang < 90.0:
            acute += 1
    return {'sharp_bends': sharp, 'acute_bends': acute, 'total_bends': total}


# --------------------------------------------------------------------------- #
# 2) Bend density per mm                                                      #
# --------------------------------------------------------------------------- #

def bend_density_per_mm(path: Sequence[PathPoint], grid) -> float:
    """total_bends per mm of in-plane path length. 0.0 if path length 0."""
    length = path_length_mm(path, grid)
    if length == 0.0:
        return 0.0
    return count_bends(path, grid)['total_bends'] / length


# --------------------------------------------------------------------------- #
# 3) Bounding-box straightness                                                #
# --------------------------------------------------------------------------- #

def _bbox_area(points: Iterable[Tuple[float, float]]) -> float:
    pts = list(points)
    if not pts:
        return 0.0
    xs = [p[0] for p in pts]
    ys = [p[1] for p in pts]
    return (max(xs) - min(xs)) * (max(ys) - min(ys))


def bbox_straightness(path: Sequence[PathPoint], grid) -> float:
    """Path bbox area / straight-line bbox area (endpoints).

    1.0 = perfectly straight (or degenerate zero-area). > 1.0 = path
    deviates from the direct endpoint line.
    """
    if len(path) < 2:
        return 1.0
    pts = [grid.geom.cell_to_world(ix, iy) for (_l, ix, iy) in path]
    path_area = _bbox_area(pts)
    start = pts[0]
    end = pts[-1]
    line_area = _bbox_area([start, end])
    if line_area == 0.0:
        # Endpoints coincide OR they lie on an axis-aligned line → degenerate.
        # Either way path can't beat it; treat as straight unless path itself
        # has nonzero bbox (then return path_area / epsilon would explode).
        # Spec: "0 area straight path → return 1.0".
        if path_area == 0.0:
            return 1.0
        # Endpoints axis-aligned but path detours sideways. Fall back to a
        # capped sentinel rather than path_area / 1e-9 (which produces values
        # in the millions and ruins downstream max/mean aggregation).
        return 1e3
    return path_area / line_area


# --------------------------------------------------------------------------- #
# 4) Detour count (doubling back)                                             #
# --------------------------------------------------------------------------- #

def detour_count(path: Sequence[PathPoint], grid) -> int:
    """Number of times consecutive same-layer direction vectors have a
    negative dot product (angle > 90° between them = path doubles back).
    """
    segs = list(iter_segments_mm(path, grid))
    n = 0
    for i in range(len(segs) - 1):
        l1, x0, y0, x1, y1 = segs[i]
        l2, x2, y2, x3, y3 = segs[i + 1]
        if l1 != l2:
            continue
        # Segments are consecutive in the iterator if they share a layer;
        # in iter_segments_mm both same-layer adjacent yields share a
        # vertex (x1,y1) == (x2,y2).
        dx1, dy1 = x1 - x0, y1 - y0
        dx2, dy2 = x3 - x2, y3 - y2
        if dx1 * dx2 + dy1 * dy2 < 0.0:
            n += 1
    return n


# --------------------------------------------------------------------------- #
# 5) Routing efficiency (path / manhattan)                                    #
# --------------------------------------------------------------------------- #

def routing_efficiency(path: Sequence[PathPoint], grid,
                       start_xy: Tuple[float, float],
                       end_xy: Tuple[float, float]) -> float:
    """path_length_mm / euclidean(start, end).

    The router is 8-connected (cardinal + 45° diagonal moves), so manhattan
    overstates the geometric lower bound on path length and would let
    "efficiency" drop below 1.0 for any diagonal route. The euclidean
    distance is the true lower bound for any 2D path; values therefore
    satisfy ``efficiency >= 1.0`` modulo floating-point noise.

    Degenerate case: when start and end coincide (or are within numerical
    noise of each other), the ratio is undefined. We return 1.0 — vacuously
    "as efficient as possible" — rather than letting the value explode via
    ``length / epsilon``.
    """
    length = path_length_mm(path, grid)
    eucl = math.hypot(end_xy[0] - start_xy[0], end_xy[1] - start_xy[1])
    if eucl < 1e-6:
        return 1.0
    return length / eucl


# --------------------------------------------------------------------------- #
# 6) Escape-side fidelity                                                     #
# --------------------------------------------------------------------------- #

def escape_side_fidelity(path: Sequence[PathPoint], grid,
                         source_xy: Tuple[float, float],
                         escape_sides: Sequence[str]) -> bool:
    """Did the path's first meaningful exit (>0.5 mm net displacement
    from source) leave in one of the requested cardinal sides?

    Empty ``escape_sides`` → no constraint, returns True.
    Returns True if no point ever moves >0.5 mm from source (degenerate
    short stub — nothing to violate).
    """
    if not escape_sides:
        return True
    sx, sy = source_xy
    sides = {s.upper() for s in escape_sides}
    for (_l, ix, iy) in path:
        x, y = grid.geom.cell_to_world(ix, iy)
        dx = x - sx
        dy = y - sy
        if math.hypot(dx, dy) > 0.5:
            # Pick dominant axis for the exit direction.
            if abs(dx) >= abs(dy):
                direction = 'E' if dx > 0 else 'W'
            else:
                direction = 'N' if dy > 0 else 'S'
            return direction in sides
    return True


# --------------------------------------------------------------------------- #
# Aggregate                                                                   #
# --------------------------------------------------------------------------- #

def summarize_geometry(routed_paths: dict, grid, tasks: Sequence) -> dict:
    """Aggregate geometric metrics across all nets in ``routed_paths``.

    ``routed_paths``: dict[net_name → PathResult] where PathResult.path
    is the cell list. ``tasks``: iterable of RoutingTask with
    ``.net_name``, ``.source = (layer, ix, iy)``,
    ``.sink = (layer, ix, iy)``, and ``.rule.escape_sides``.

    Nets without a matching task skip the escape-side check (counted as
    'not applicable' — excluded from both numerator and denominator).
    """
    task_by_net = {t.net_name: t for t in tasks}

    sharp_total = 0
    acute_total = 0
    bends_total = 0
    detour_total = 0

    weighted_density_num = 0.0
    weighted_density_den = 0.0

    straightness_vals: list = []
    efficiency_vals: list = []

    fidelity_ok = 0
    fidelity_checked = 0

    for net_name, result in routed_paths.items():
        path = getattr(result, 'path', result)
        if not path:
            continue

        bends = count_bends(path, grid)
        sharp_total += bends['sharp_bends']
        acute_total += bends['acute_bends']
        bends_total += bends['total_bends']

        detour_total += detour_count(path, grid)

        length = path_length_mm(path, grid)
        if length > 0.0:
            weighted_density_num += bends['total_bends']
            weighted_density_den += length

        straightness_vals.append(bbox_straightness(path, grid))

        task = task_by_net.get(net_name)
        if task is not None:
            src_layer, src_ix, src_iy = task.source
            snk_layer, snk_ix, snk_iy = task.sink
            sx, sy = grid.geom.cell_to_world(src_ix, src_iy)
            ex, ey = grid.geom.cell_to_world(snk_ix, snk_iy)
            efficiency_vals.append(
                routing_efficiency(path, grid, (sx, sy), (ex, ey))
            )

            escape_sides = getattr(task.rule, 'escape_sides', ())
            if escape_sides:
                fidelity_checked += 1
                if escape_side_fidelity(path, grid, (sx, sy), escape_sides):
                    fidelity_ok += 1

    bbox_mean = (sum(straightness_vals) / len(straightness_vals)
                 if straightness_vals else 0.0)
    bbox_max = max(straightness_vals) if straightness_vals else 0.0

    eff_mean = (sum(efficiency_vals) / len(efficiency_vals)
                if efficiency_vals else 0.0)
    eff_max = max(efficiency_vals) if efficiency_vals else 0.0

    density = (weighted_density_num / weighted_density_den
               if weighted_density_den > 0.0 else 0.0)

    fidelity_pct = (fidelity_ok / fidelity_checked
                    if fidelity_checked > 0 else 1.0)

    return {
        'sharp_bends': sharp_total,
        'acute_bends': acute_total,
        'total_bends': bends_total,
        'bend_density_per_mm': density,
        'bbox_straightness_mean': bbox_mean,
        'bbox_straightness_max': bbox_max,
        'detour_count': detour_total,
        'routing_efficiency_mean': eff_mean,
        'routing_efficiency_max': eff_max,
        'escape_side_fidelity_pct': fidelity_pct,
    }
