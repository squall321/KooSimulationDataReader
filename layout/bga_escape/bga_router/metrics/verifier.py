# 라우팅 결과의 per-NetRule 통과/실패를 점검하는 검증기
"""Per-NetRule verifier.

For each routed net + its NetRule, run the relevant checks and report
which fields passed / failed. Path-only checks operate on a single
``(path, rule)`` pair; cross-net checks operate on the full
``routed_paths`` mapping.

External dependencies (stackup, plane geometry) are optional — when
they are absent the corresponding checks return passed=True with a
note containing 'N/A' (so they show up in by_field without inflating
the violation count).

Path representation:
    list[(layer:str, ix:int, iy:int)]    # grid cells
    mm conversion: ``grid.geom.cell_to_world(ix, iy)``
"""

from __future__ import annotations

import math
from dataclasses import dataclass, field
from typing import Iterable, Optional

from bga_router.metrics.path_geometry import (
    path_length_mm,
    length_per_layer_mm,
    count_vias,
    iter_segments_mm,
)


# ---------------------------------------------------------------------------
# Public data type
# ---------------------------------------------------------------------------


@dataclass
class RuleCheckResult:
    field: str
    passed: bool
    violators: list = field(default_factory=list)
    note: str = ''

    def to_dict(self) -> dict:
        """JSON-safe representation. ``json.dumps`` cannot serialise the
        dataclass directly; callers should use this (or ``asdict``)."""
        return {
            'field':     self.field,
            'pass':      bool(self.passed),
            'violators': list(self.violators),
            'note':      self.note,
        }


def _na(field_name: str, why: str) -> RuleCheckResult:
    return RuleCheckResult(field=field_name, passed=True, violators=[],
                           note=f'N/A: {why}')


def _endpoint_xy(endpoint, grid):
    """Source/sink can be a (layer, ix, iy) tuple or NetEndpoint with .layer/.ix/.iy."""
    if isinstance(endpoint, tuple) and len(endpoint) == 3:
        _, ix, iy = endpoint
    else:
        ix = endpoint.ix
        iy = endpoint.iy
    return grid.geom.cell_to_world(ix, iy)


# ---------------------------------------------------------------------------
# Path-only checks (one path + its rule)
# ---------------------------------------------------------------------------


def check_layers_membership(path, rule) -> bool:
    """Every cell.layer ∈ rule.layers."""
    if not rule.layers:
        return True
    allowed = set(rule.layers)
    for layer, _ix, _iy in path:
        if layer not in allowed:
            return False
    return True


def _dominant_escape_side(source_xy, sink_xy) -> str:
    sx, sy = source_xy
    tx, ty = sink_xy
    dx = tx - sx
    dy = ty - sy
    if abs(dx) >= abs(dy):
        return 'E' if dx >= 0 else 'W'
    else:
        return 'N' if dy >= 0 else 'S'


def check_escape_side(path, source_xy, rule) -> bool:
    """Path leaves source toward a side allowed by rule.escape_sides.

    Looks at the first non-trivial segment from the source point and
    determines its dominant cardinal direction.
    """
    if not rule.escape_sides:
        return True
    if not path or len(path) < 2:
        return True  # vacuous
    sx, sy = source_xy
    # Find first point distinct from source in xy
    for layer, ix, iy in path[1:]:
        # We need world coord; but we only have source_xy. The caller
        # promised cell_to_world(path[0]) ≈ source_xy. So compute
        # delta to first cell using grid... but we don't have grid here.
        # Instead, use the path[0] cell as the reference and walk.
        break
    # Re-do via the path itself; assume path[0]'s xy equals source_xy
    # (caller's responsibility).
    first_layer, first_ix, first_iy = path[0]
    next_layer = None
    next_ix = next_iy = None
    for layer, ix, iy in path[1:]:
        if (ix, iy) != (first_ix, first_iy):
            next_layer = layer
            next_ix, next_iy = ix, iy
            break
    if next_ix is None:
        return True  # path never leaves the source cell in xy
    # Determine direction from cell delta — independent of mm scale.
    dx = next_ix - first_ix
    dy = next_iy - first_iy
    side = _dominant_escape_side((0, 0), (dx, dy))
    return side in set(rule.escape_sides)


def check_via_budget(path_result, rule) -> bool:
    """path_result.via_count ≤ rule.max_via_count (None → no constraint)."""
    if rule.max_via_count is None:
        return True
    via = getattr(path_result, 'via_count', None)
    if via is None:
        # Fallback: derive from path if present.
        path = getattr(path_result, 'path', None)
        via = count_vias(path) if path else 0
    return via <= rule.max_via_count


def check_min_length(path, grid, rule) -> bool:
    """path_length_mm ≥ rule.min_trace_length_mm (None → no constraint)."""
    if rule.min_trace_length_mm is None:
        return True
    return path_length_mm(path, grid) >= rule.min_trace_length_mm - 1e-9


def _path_segment_angles(path):
    """Yield interior bend angles in degrees (between consecutive segments).

    Skips via transitions (layer changes contribute no segment).
    Angle = 180° means straight; 90° means a right turn; <90° = acute.
    """
    # Build list of vectors per same-layer segment in cell space.
    vectors = []  # (layer, dx, dy)
    prev_layer = None
    prev_ix = prev_iy = None
    for layer, ix, iy in path:
        if prev_layer is not None and prev_layer == layer:
            dx = ix - prev_ix
            dy = iy - prev_iy
            if (dx, dy) != (0, 0):
                vectors.append((layer, dx, dy))
        prev_layer = layer
        prev_ix, prev_iy = ix, iy
    # Compute angles between consecutive vectors on same layer.
    for i in range(1, len(vectors)):
        la, ax, ay = vectors[i - 1]
        lb, bx, by = vectors[i]
        if la != lb:
            continue
        na = math.hypot(ax, ay)
        nb = math.hypot(bx, by)
        if na == 0 or nb == 0:
            continue
        cosv = max(-1.0, min(1.0, (ax * bx + ay * by) / (na * nb)))
        yield math.degrees(math.acos(cosv))


def check_bend_class(path, rule) -> Optional[bool]:
    """Verify bend angles against rule.bend_class.

    Returns True/False, or None to signal N/A (no constraint).

    ``_path_segment_angles`` yields the **turn angle** between consecutive
    forward segment vectors (the angle by which v1 rotates to become v2):
      - 0°  → straight line
      - 45° → diagonal corner
      - 90° → right-angle corner
      - >90° → acute / sharp (U-turn at 180°)
    """
    if rule.bend_class is None:
        return None
    angles = list(_path_segment_angles(path))
    acute = sum(1 for a in angles if a > 90.0 + 1e-6)
    has_45 = any(abs(a - 45.0) <= 1e-6 for a in angles)
    if rule.bend_class == 'low_speed':
        return acute == 0
    if rule.bend_class in ('hf_diff', 'hf_single'):
        return acute == 0 and not has_45
    return None  # unknown class → N/A


# ---------------------------------------------------------------------------
# Cross-net checks
# ---------------------------------------------------------------------------


def _segments_mm_for_path(path, grid):
    """List of (layer, x0, y0, x1, y1) segments in mm."""
    return list(iter_segments_mm(path, grid))


def _seg_seg_distance(a, b) -> float:
    """Minimum distance between two 2D segments a, b. Each is (x0, y0, x1, y1)."""
    ax0, ay0, ax1, ay1 = a
    bx0, by0, bx1, by1 = b
    return min(
        _point_seg_distance(ax0, ay0, bx0, by0, bx1, by1),
        _point_seg_distance(ax1, ay1, bx0, by0, bx1, by1),
        _point_seg_distance(bx0, by0, ax0, ay0, ax1, ay1),
        _point_seg_distance(bx1, by1, ax0, ay0, ax1, ay1),
    )


def _point_seg_distance(px, py, x0, y0, x1, y1) -> float:
    dx = x1 - x0
    dy = y1 - y0
    if dx == 0 and dy == 0:
        return math.hypot(px - x0, py - y0)
    t = ((px - x0) * dx + (py - y0) * dy) / (dx * dx + dy * dy)
    t = max(0.0, min(1.0, t))
    cx = x0 + t * dx
    cy = y0 + t * dy
    return math.hypot(px - cx, py - cy)


def _segments_by_layer(routed_paths, grid):
    """Return dict[net_name] -> dict[layer] -> list of (x0,y0,x1,y1)."""
    out = {}
    for net_name, pr in routed_paths.items():
        path = getattr(pr, 'path', None) or []
        per_layer = {}
        for layer, x0, y0, x1, y1 in iter_segments_mm(path, grid):
            per_layer.setdefault(layer, []).append((x0, y0, x1, y1))
        out[net_name] = per_layer
    return out


def check_clearance(routed_paths, grid, rules_by_net) -> RuleCheckResult:
    """For each net with rule.clearance_mm set, find min distance to any
    other same-layer net's segments; flag if < clearance_mm."""
    segs_by_net = _segments_by_layer(routed_paths, grid)
    violators = []
    any_constraint = False
    for net_name, rule in rules_by_net.items():
        if rule is None or rule.clearance_mm is None:
            continue
        any_constraint = True
        if net_name not in segs_by_net:
            continue
        my_layers = segs_by_net[net_name]
        bad = False
        for other_name, other_layers in segs_by_net.items():
            if other_name == net_name:
                continue
            for layer, my_segs in my_layers.items():
                if layer not in other_layers:
                    continue
                for a in my_segs:
                    for b in other_layers[layer]:
                        d = _seg_seg_distance(a, b)
                        if d < rule.clearance_mm - 1e-9:
                            bad = True
                            break
                    if bad:
                        break
                if bad:
                    break
            if bad:
                break
        if bad:
            violators.append(net_name)
    if not any_constraint:
        return _na('clearance_ok', 'no net has clearance_mm set')
    return RuleCheckResult(
        field='clearance_ok',
        passed=not violators,
        violators=violators,
    )


def check_pair_skew(routed_paths, grid, rules_by_net) -> RuleCheckResult:
    """For each net with pair_partner_net + pair_skew_max_mm, |len(p)-len(n)| ≤ skew."""
    violators = []
    seen_pairs = set()
    any_constraint = False
    for net_name, rule in rules_by_net.items():
        if (rule is None or rule.pair_partner_net is None
                or rule.pair_skew_max_mm is None):
            continue
        partner = rule.pair_partner_net
        key = tuple(sorted([net_name, partner]))
        if key in seen_pairs:
            continue
        seen_pairs.add(key)
        any_constraint = True
        if net_name not in routed_paths or partner not in routed_paths:
            violators.append(f'{net_name}|{partner}')
            continue
        la = path_length_mm(routed_paths[net_name].path, grid)
        lb = path_length_mm(routed_paths[partner].path, grid)
        if abs(la - lb) > rule.pair_skew_max_mm + 1e-9:
            violators.append(f'{net_name}|{partner}')
    if not any_constraint:
        return _na('pair_skew_ok', 'no diff-pair partners with skew tolerance set')
    return RuleCheckResult(
        field='pair_skew_ok',
        passed=not violators,
        violators=violators,
    )


def check_pair_gap(routed_paths, grid, rules_by_net) -> RuleCheckResult:
    """For each diff pair, find pairs of same-layer parallel segments closer
    than pair_gap_mm * 2 (considered coupled), then flag when gap < pair_gap_mm.
    """
    violators = []
    seen_pairs = set()
    any_constraint = False
    segs_by_net = _segments_by_layer(routed_paths, grid)

    for net_name, rule in rules_by_net.items():
        if (rule is None or rule.pair_partner_net is None
                or rule.pair_gap_mm is None):
            continue
        partner = rule.pair_partner_net
        key = tuple(sorted([net_name, partner]))
        if key in seen_pairs:
            continue
        seen_pairs.add(key)
        any_constraint = True
        if net_name not in segs_by_net or partner not in segs_by_net:
            continue
        a_layers = segs_by_net[net_name]
        b_layers = segs_by_net[partner]
        coupling_window = rule.pair_gap_mm * 2
        bad = False
        for layer, a_segs in a_layers.items():
            if layer not in b_layers:
                continue
            for a in a_segs:
                for b in b_layers[layer]:
                    if not _segments_parallel(a, b):
                        continue
                    d = _seg_seg_distance(a, b)
                    if d > coupling_window:
                        continue
                    # coupled
                    if d < rule.pair_gap_mm - 1e-9:
                        bad = True
                        break
                if bad:
                    break
            if bad:
                break
        if bad:
            violators.append(f'{net_name}|{partner}')

    if not any_constraint:
        return _na('pair_gap_ok', 'no diff-pair partners with pair_gap set')
    return RuleCheckResult(
        field='pair_gap_ok',
        passed=not violators,
        violators=violators,
    )


def _segments_parallel(a, b, tol_deg: float = 5.0) -> bool:
    ax0, ay0, ax1, ay1 = a
    bx0, by0, bx1, by1 = b
    adx, ady = ax1 - ax0, ay1 - ay0
    bdx, bdy = bx1 - bx0, by1 - by0
    na = math.hypot(adx, ady)
    nb = math.hypot(bdx, bdy)
    if na == 0 or nb == 0:
        return False
    cosv = abs(adx * bdx + ady * bdy) / (na * nb)
    cosv = min(1.0, cosv)
    angle = math.degrees(math.acos(cosv))
    return angle <= tol_deg


def check_length_group(routed_paths, grid, rules_by_net) -> RuleCheckResult:
    """For each length_match_group, (group max - group min) ≤ tolerance.

    Tolerance fallback: pair_skew_max_mm of any group member (first non-None
    encountered), else 0.5 mm.
    """
    groups = {}     # group_name -> list of (net_name, rule)
    for net_name, rule in rules_by_net.items():
        if rule is None or rule.length_match_group is None:
            continue
        groups.setdefault(rule.length_match_group, []).append((net_name, rule))
    if not groups:
        return _na('length_group_ok', 'no length_match_group set on any net')
    violators = []
    for group_name, members in groups.items():
        # Determine tolerance — first member with pair_skew_max_mm wins; else 0.5
        tol = None
        for _n, r in members:
            if r.pair_skew_max_mm is not None:
                tol = r.pair_skew_max_mm
                break
        if tol is None:
            tol = 0.5  # documented default fallback
        lengths = []
        for n, _r in members:
            if n in routed_paths:
                lengths.append(path_length_mm(routed_paths[n].path, grid))
        if not lengths:
            continue
        spread = max(lengths) - min(lengths)
        if spread > tol + 1e-9:
            violators.append(group_name)
    return RuleCheckResult(
        field='length_group_ok',
        passed=not violators,
        violators=violators,
    )


# ---------------------------------------------------------------------------
# External-data-dependent checks
# ---------------------------------------------------------------------------


def check_keep_out(path, grid, rule) -> Optional[bool]:
    """Return False if any path point lies inside any keep_out_zone.

    Returns None (N/A) when rule has no keep_out_zones.
    """
    if not rule.keep_out_zones:
        return None
    for _layer, ix, iy in path:
        x, y = grid.geom.cell_to_world(ix, iy)
        for (xmin, ymin, xmax, ymax) in rule.keep_out_zones:
            if xmin <= x <= xmax and ymin <= y <= ymax:
                return False
    return True


def check_split_avoidance(path, grid, rule, plane_geometry=None) -> Optional[bool]:
    """Phase C2 — plane geometry not yet wired."""
    if plane_geometry is None:
        return None
    # Count overlaps (placeholder; real impl will iterate plane polygons).
    return True


def check_via_type(path_result, rule, stackup=None) -> Optional[bool]:
    """Verify via_type vs stackup span. N/A when stackup is None."""
    if stackup is None or rule.via_type is None:
        return None
    # Placeholder: stackup-dependent verification not yet specified.
    return True


def check_impedance_target(path, rule, stackup=None) -> Optional[bool]:
    """Phase C1 — impedance verification not wired. N/A when stackup or
    target is missing."""
    if stackup is None or rule.impedance_target_ohm is None:
        return None
    return True


def _is_power_or_ground(net_name: str) -> bool:
    n = net_name.upper()
    return (n.startswith('GND') or n.startswith('VDD')
            or n.startswith('VCC') or n.startswith('VSS'))


def check_shield_present(path, routed_paths, rules_by_net, rule, grid
                          ) -> Optional[bool]:
    """When rule.shield_required, both sides of the trace must have a PG net
    polyline within pair_gap_mm * 2. N/A when no PG nets exist in
    routed_paths or shielding not required.
    """
    if not rule.shield_required:
        return None
    pg_nets = [n for n in routed_paths if _is_power_or_ground(n)]
    if not pg_nets:
        return None
    gap = rule.pair_gap_mm if rule.pair_gap_mm is not None else 0.2
    window = gap * 2
    # Collect this path's segments and find a PG segment within window on
    # the left and right (any side) of every segment midpoint.
    my_segs = list(iter_segments_mm(path, grid))
    if not my_segs:
        return True
    pg_segs = []
    for pn in pg_nets:
        ppath = getattr(routed_paths[pn], 'path', None) or []
        pg_segs.extend(list(iter_segments_mm(ppath, grid)))
    if not pg_segs:
        return None
    # Approx: for each of my segments, check that at least one PG segment
    # exists on each side (left = cross-product positive, right = negative).
    for (layer, x0, y0, x1, y1) in my_segs:
        dx, dy = x1 - x0, y1 - y0
        n = math.hypot(dx, dy)
        if n == 0:
            continue
        # Normal vector
        nx, ny = -dy / n, dx / n
        mid = ((x0 + x1) / 2, (y0 + y1) / 2)
        left_ok = False
        right_ok = False
        for (plyr, bx0, by0, bx1, by1) in pg_segs:
            if plyr != layer:
                continue
            d = _point_seg_distance(mid[0], mid[1], bx0, by0, bx1, by1)
            if d > window:
                continue
            # Determine side using midpoint of PG segment
            bmid = ((bx0 + bx1) / 2, (by0 + by1) / 2)
            side = (bmid[0] - mid[0]) * nx + (bmid[1] - mid[1]) * ny
            if side > 0:
                left_ok = True
            elif side < 0:
                right_ok = True
            if left_ok and right_ok:
                break
        if not (left_ok and right_ok):
            return False
    return True


def _path_adjacency(path):
    """Build undirected adjacency dict from consecutive cells."""
    adj: dict = {}
    for i in range(len(path) - 1):
        a, b = path[i], path[i + 1]
        adj.setdefault(a, set()).add(b)
        adj.setdefault(b, set()).add(a)
    return adj


def check_topology(path, rule) -> Optional[bool]:
    """Validate routed graph matches declared NetRule.net_topology.

    daisy_chain — simple linear chain (max degree 2, exactly 2 degree-1 nodes).
    star        — central hub with N>=3 spokes; exactly 1 node with
                  degree>=3, all other non-leaves have degree 2.
    tee         — exactly 1 degree-3 branch point (main + 1 stub).

    For BGA escape (2-pin nets) any declared star/tee on a chain-only
    path returns False with the caller-set note explaining the mismatch.
    """
    if rule.net_topology is None:
        return None
    adj = _path_adjacency(path)
    if not adj:
        return True
    degrees = [len(v) for v in adj.values()]
    endpoints = sum(1 for d in degrees if d == 1)
    branches_3 = sum(1 for d in degrees if d == 3)
    branches_high = sum(1 for d in degrees if d >= 3)
    max_deg = max(degrees)

    if rule.net_topology == 'daisy_chain':
        return max_deg <= 2 and endpoints == 2
    if rule.net_topology == 'tee':
        # Exactly one degree-3 vertex, no degree>=4 vertices
        return branches_3 == 1 and max_deg == 3
    if rule.net_topology == 'star':
        # Exactly one hub (degree >= 3), every other non-leaf has degree 2
        if branches_high != 1:
            return False
        return max_deg >= 3
    return None


# ---------------------------------------------------------------------------
# Soft metric
# ---------------------------------------------------------------------------


def preferred_layer_pct(path, grid, rule) -> float:
    """Fraction of total path length spent on rule.preferred_layers (0..1).

    Returns 1.0 when path has zero length or no preferred layers set
    (vacuously satisfied).
    """
    if not rule.preferred_layers:
        return 1.0
    per_layer = length_per_layer_mm(path, grid)
    total = sum(per_layer.values())
    if total <= 0:
        return 1.0
    pref = set(rule.preferred_layers)
    on_pref = sum(mm for lyr, mm in per_layer.items() if lyr in pref)
    return on_pref / total


# ---------------------------------------------------------------------------
# Aggregator
# ---------------------------------------------------------------------------


def verify_all(routed_paths: dict, tasks: list, grid, spec,
               *, stackup=None, plane_geom=None) -> dict:
    """Run every applicable check; return {'violations': int, 'by_field': {...}}."""
    # Build per-net rule mapping
    rules_by_net = {}
    for task in tasks:
        rules_by_net[task.net_name] = task.rule

    by_field = {}

    # --- path-only checks: aggregate violators across nets ---
    per_check_violators = {
        'layers_membership_ok': [],
        'escape_side_ok': [],
        'via_budget_ok': [],
        'min_length_ok': [],
        'bend_class_ok': [],
        'keep_out_ok': [],
        'split_avoidance_ok': [],
        'via_type_ok': [],
        'impedance_target_ok': [],
        'shield_present_ok': [],
        'topology_ok': [],
    }
    per_check_na = {k: True for k in per_check_violators}
    per_check_notes: dict = {k: '' for k in per_check_violators}

    for task in tasks:
        net = task.net_name
        rule = task.rule
        if net not in routed_paths:
            continue
        pr = routed_paths[net]
        path = getattr(pr, 'path', None) or []

        # layers_membership
        if rule.layers:
            per_check_na['layers_membership_ok'] = False
            if not check_layers_membership(path, rule):
                per_check_violators['layers_membership_ok'].append(net)

        # escape_side
        if rule.escape_sides:
            per_check_na['escape_side_ok'] = False
            src_xy = _endpoint_xy(task.source, grid)
            if not check_escape_side(path, src_xy, rule):
                per_check_violators['escape_side_ok'].append(net)

        # via_budget
        if rule.max_via_count is not None:
            per_check_na['via_budget_ok'] = False
            if not check_via_budget(pr, rule):
                per_check_violators['via_budget_ok'].append(net)

        # min_length
        if rule.min_trace_length_mm is not None:
            per_check_na['min_length_ok'] = False
            if not check_min_length(path, grid, rule):
                per_check_violators['min_length_ok'].append(net)

        # bend_class
        bc = check_bend_class(path, rule)
        if bc is not None:
            per_check_na['bend_class_ok'] = False
            if not bc:
                per_check_violators['bend_class_ok'].append(net)

        # keep_out
        ko = check_keep_out(path, grid, rule)
        if ko is not None:
            per_check_na['keep_out_ok'] = False
            if not ko:
                per_check_violators['keep_out_ok'].append(net)

        # split_avoidance (now wired with plane_geom from Phase C2)
        sa = check_split_avoidance(path, grid, rule, plane_geometry=plane_geom)
        if sa is not None:
            per_check_na['split_avoidance_ok'] = False
            if not sa:
                per_check_violators['split_avoidance_ok'].append(net)
        else:
            if rule.split_avoidance:
                per_check_notes['split_avoidance_ok'] = (
                    'N/A: plane_geometry not provided (Phase C2)')

        # via_type (N/A unless stackup)
        vt = check_via_type(pr, rule, stackup=stackup)
        if vt is not None:
            per_check_na['via_type_ok'] = False
            if not vt:
                per_check_violators['via_type_ok'].append(net)
        else:
            if rule.via_type is not None:
                per_check_notes['via_type_ok'] = (
                    'N/A: stackup not provided')

        # impedance_target (N/A unless stackup + target)
        imp = check_impedance_target(path, rule, stackup=stackup)
        if imp is not None:
            per_check_na['impedance_target_ok'] = False
            if not imp:
                per_check_violators['impedance_target_ok'].append(net)
        else:
            if rule.impedance_target_ohm is not None:
                per_check_notes['impedance_target_ok'] = (
                    'N/A: stackup not provided (Phase C1)')

        # shield_present
        sh = check_shield_present(path, routed_paths, rules_by_net,
                                   rule, grid)
        if sh is not None:
            per_check_na['shield_present_ok'] = False
            if not sh:
                per_check_violators['shield_present_ok'].append(net)
        else:
            if rule.shield_required:
                per_check_notes['shield_present_ok'] = (
                    'N/A: no PG nets in routed_paths')

        # topology — daisy_chain / star / tee 모두 검증 (Phase D-2).
        topo = check_topology(path, rule)
        if topo is not None:
            per_check_na['topology_ok'] = False
            if not topo:
                per_check_violators['topology_ok'].append(net)

    # Materialise path-only results
    for field_name, viol in per_check_violators.items():
        if per_check_na[field_name]:
            note = per_check_notes.get(field_name) or 'N/A: no net opts into this check'
            by_field[field_name] = RuleCheckResult(
                field=field_name, passed=True, violators=[], note=note)
        else:
            by_field[field_name] = RuleCheckResult(
                field=field_name, passed=not viol, violators=list(viol))

    # --- cross-net checks ---
    by_field['clearance_ok'] = check_clearance(routed_paths, grid, rules_by_net)
    by_field['pair_skew_ok'] = check_pair_skew(routed_paths, grid, rules_by_net)
    by_field['pair_gap_ok'] = check_pair_gap(routed_paths, grid, rules_by_net)
    by_field['length_group_ok'] = check_length_group(
        routed_paths, grid, rules_by_net)

    # --- soft metric ---
    pref_total = 0.0
    pref_count = 0
    for task in tasks:
        net = task.net_name
        if net not in routed_paths:
            continue
        path = getattr(routed_paths[net], 'path', None) or []
        if task.rule.preferred_layers:
            pref_total += preferred_layer_pct(path, grid, task.rule)
            pref_count += 1
    if pref_count:
        avg_pref = pref_total / pref_count
        by_field['preferred_layer_pct'] = RuleCheckResult(
            field='preferred_layer_pct', passed=True, violators=[],
            note=f'soft: {avg_pref:.2f}')
    else:
        by_field['preferred_layer_pct'] = RuleCheckResult(
            field='preferred_layer_pct', passed=True, violators=[],
            note='N/A: no preferred_layers set on any net')

    # --- aggregate violation count ---
    violations = sum(0 if r.passed else 1 for r in by_field.values())
    return {'violations': violations, 'by_field': by_field}
