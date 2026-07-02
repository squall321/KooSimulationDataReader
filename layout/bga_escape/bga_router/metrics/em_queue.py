# Wadell 공식이 marginal이거나 impedance_target 위반한 net을 EM solver queue에 등록
"""Phase D — EM queue hook.

Closed-form Z0/Zdiff (Phase C) is accurate within ~5% for typical
stackups but loses confidence at:
  - s/h < 0.5 (tight differential coupling)
  - h < 0.05 mm (very thin dielectric)
  - rule.impedance_target_ohm declared and our analytical Z0 misses
    the band by more than tol_pct

This module emits an ``em_queue`` block in the result JSON listing
those nets with enough context (layer, bbox, reason, suggested solver)
that downstream pipelines (simulation/sol_b_peec, simulation/sol_d_cross_section)
can pick them up and run a real full-wave / 2D-FD simulation without
re-traversing the router state.

The eval loop itself stays sub-second; the queue is purely a hand-off.
"""

from __future__ import annotations

import math
from typing import Any, Dict, Iterable, List, Optional, Tuple

from .path_geometry import iter_segments_mm


def _path_bbox_mm(path, grid) -> Optional[Tuple[float, float, float, float]]:
    """Axis-aligned bounding box (xmin, ymin, xmax, ymax) in mm.
    None if path empty / no convertible cells."""
    if not path:
        return None
    xs, ys = [], []
    for _layer, ix, iy in path:
        x, y = grid.geom.cell_to_world(ix, iy)
        xs.append(x)
        ys.append(y)
    if not xs:
        return None
    return (min(xs), min(ys), max(xs), max(ys))


def _dominant_layer(path) -> Optional[str]:
    if not path:
        return None
    counts: Dict[str, int] = {}
    for layer, _ix, _iy in path:
        counts[layer] = counts.get(layer, 0) + 1
    return max(counts, key=counts.get)


def _layers_in_path(path) -> List[str]:
    out: List[str] = []
    seen: set = set()
    for layer, _ix, _iy in path:
        if layer not in seen:
            seen.add(layer)
            out.append(layer)
    return out


def build_em_queue(routed_paths, grid, rules_by_net: Dict[str, Any],
                    si_metrics: Dict[str, Any],
                    rule_check: Dict[str, Any],
                    *, suggested_solver: str = 'sol_d') -> Dict[str, Any]:
    """Inspect Phase C outputs; emit per-net queue entries for nets that
    need a real EM solver.

    Returns a dict ready for json.dumps:
        {
            'entries': [
                {
                    'net':       'DDR4_DQ0_P',
                    'reason':    'tight_coupling',
                    'layer':     'LAY2',
                    'layers':    ['LAY2'],
                    'bbox_mm':   [x_min, y_min, x_max, y_max],
                    'rule':      {'width_mm': 0.075, 'pair_gap_mm': 0.05, ...},
                    'suggested_solver': 'sol_d',
                },
                ...
            ],
            'count': N,
            'reason_breakdown': {'tight_coupling': K, 'impedance_miss': L, ...}
        }
    """
    marginal = (si_metrics or {}).get('marginal_formulas') or {}
    by_field = ((rule_check or {}).get('by_field') or {})
    imp_entry = by_field.get('impedance_target_ok') or {}
    imp_violators = set(imp_entry.get('violators') or [])

    entries: List[Dict[str, Any]] = []
    reason_breakdown: Dict[str, int] = {}

    for net, pr in routed_paths.items():
        path = getattr(pr, 'path', None) if not isinstance(pr, list) else pr
        if not path:
            continue

        reasons: List[str] = []
        flags = marginal.get(net) or {}
        if flags.get('tight_coupling'):
            reasons.append('tight_coupling')
        if flags.get('thin_dielectric'):
            reasons.append('thin_dielectric')
        if flags.get('stripline_wh_bias'):
            reasons.append('stripline_wh_bias')          # Phase H-2
        if net in imp_violators:
            reasons.append('impedance_miss')

        if not reasons:
            continue

        bbox = _path_bbox_mm(path, grid)
        rule = rules_by_net.get(net)
        rule_snapshot: Dict[str, Any] = {}
        if rule is not None:
            for fname in ('width_mm', 'pair_gap_mm', 'pair_partner_net',
                           'impedance_target_ohm', 'dielectric_height_mm',
                           'dielectric_er', 'copper_thickness_mm'):
                v = getattr(rule, fname, None)
                if v is not None:
                    rule_snapshot[fname] = v

        entries.append({
            'net':              net,
            'reason':           '+'.join(reasons),
            'reasons':          reasons,
            'layer':            _dominant_layer(path),
            'layers':           _layers_in_path(path),
            'bbox_mm':          list(bbox) if bbox else None,
            'rule':             rule_snapshot,
            'suggested_solver': suggested_solver,
        })

        for r in reasons:
            reason_breakdown[r] = reason_breakdown.get(r, 0) + 1

    return {
        'entries':           entries,
        'count':             len(entries),
        'reason_breakdown':  reason_breakdown,
        'consumption_hint':  (
            'downstream: simulation/sol_b_peec or simulation/sol_d_cross_section '
            'reads these entries, materializes per-net cross-section, runs '
            'full-wave Z0/insertion-loss/crosstalk. Eval loop stays sub-second.'),
    }
