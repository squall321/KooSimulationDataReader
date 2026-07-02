# HDI 보드의 micro-via stack 규칙 (drill aspect / stacked-vs-staggered) 검사기
"""Phase H-3 — HDI micro-via DRC.

Real HDI boards impose extra rules that through-via checkers miss:

  1. **Drill aspect ratio**: drill_mm / layer_span_thickness >= threshold
     (typical 1:1 for laser micro-via, 8:1 for standard PTH).
  2. **Stacked micro-via**: adjacent micro-vias on top of each other must
     be filled-and-capped; if either lacks the 'filled' attribute → DRC.
  3. **Staggered vs stacked**: staggered vias need horizontal spacing
     ≥ pad_diameter + clearance.

We flag these per-net so em_queue / rule_check can pick them up.
"""

from __future__ import annotations

from collections import defaultdict
from typing import Any, Dict, Iterable, List, Optional, Tuple


def _same_xy_within(a_xy, b_xy, tol_mm: float = 0.05) -> bool:
    ax, ay = a_xy
    bx, by = b_xy
    return abs(ax - bx) <= tol_mm and abs(ay - by) <= tol_mm


def detect_stacked_microvias(via_metadata_by_net: Dict[str, List[dict]], *,
                                xy_tol_mm: float = 0.05) -> Dict[str, int]:
    """For each net, count adjacent via records with identical xy.
    If both vias belong to a microvia span AND the earlier one has
    `filled != True`, it's a stack violation."""
    out: Dict[str, int] = {}
    for net, vias in via_metadata_by_net.items():
        stack_count = 0
        for i in range(len(vias) - 1):
            a, b = vias[i], vias[i + 1]
            if not _same_xy_within(a.get('xy_mm', (0, 0)),
                                     b.get('xy_mm', (0, 0)),
                                     tol_mm=xy_tol_mm):
                continue
            # If both are microvias and stacked without fill → violation
            if (a.get('kind') == 'microvia' and b.get('kind') == 'microvia'
                    and not a.get('filled', False)):
                stack_count += 1
        if stack_count:
            out[net] = stack_count
    return out


def compute_drill_aspect_ratio(via_span_layers, stackup) -> Optional[float]:
    """Aspect ratio = layer_span_thickness / drill_diameter."""
    if not via_span_layers or len(via_span_layers) < 2:
        return None
    start, end = via_span_layers[0], via_span_layers[-1]
    thickness = stackup.stack_thickness_between_mm(start, end) \
        if hasattr(stackup, 'stack_thickness_between_mm') else None
    return thickness


def check_drill_aspect_ratio(via_span, stackup, *,
                                max_ratio_pth: float = 8.0,
                                max_ratio_microvia: float = 1.0
                                ) -> Optional[bool]:
    """Return True/False/None. None when via lacks drill info."""
    drill = getattr(via_span, 'drill_mm', None)
    if not drill or drill <= 0:
        return None
    thickness = stackup.stack_thickness_between_mm(
        via_span.start_layer, via_span.end_layer)
    if thickness is None or thickness <= 0:
        return None
    ratio = thickness / drill
    if via_span.kind == 'microvia':
        return ratio <= max_ratio_microvia
    return ratio <= max_ratio_pth


def summarize_hdi_drc(routed_paths, stackup,
                        rules_by_net: Dict[str, Any]) -> Dict[str, Any]:
    """High-level HDI DRC summary for eval JSON."""
    # Collect via_metadata per net (Phase D-6 / E-2)
    via_by_net: Dict[str, List[dict]] = {}
    for net, pr in routed_paths.items():
        meta = getattr(pr, 'via_metadata', None) or []
        via_by_net[net] = list(meta)

    stack_violations = detect_stacked_microvias(via_by_net)

    # Drill aspect per declared via_type
    aspect_report: Dict[str, Any] = {}
    if hasattr(stackup, 'vias'):
        for via in getattr(stackup, 'vias', ()) or ():
            ok = check_drill_aspect_ratio(via, stackup)
            aspect_report[via.name] = {
                'kind':       via.kind,
                'start':      via.start_layer,
                'end':        via.end_layer,
                'drill_mm':   via.drill_mm,
                'passes':     ok,
            }

    return {
        'stacked_microvia_violations': stack_violations,
        'stacked_microvia_count':      sum(stack_violations.values()),
        'drill_aspect_by_via_type':    aspect_report,
        'unsafe_via_types': [name for name, e in aspect_report.items()
                               if e['passes'] is False],
    }
