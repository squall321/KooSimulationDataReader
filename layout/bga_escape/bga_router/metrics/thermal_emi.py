# Thermal (IPC-2152 전류용량) + EMI proxy (loop area / edge radiation) 메트릭
"""Phase H-9 — thermal / EMI first-order metrics.

Full thermal simulation and EMI radiation analysis need field solvers
far outside this scope. What a routing evaluation CAN do first-order:

Thermal (IPC-2152 기반).
  - current_capacity_a: 트레이스 width/thickness에서 허용 전류 (10°C
    rise 기준 외층 근사식). I = k * ΔT^0.44 * A^0.725 (A in mil²).
  - power_net 트레이스가 목표 전류 대비 부족하면 flag.

EMI proxy.
  - loop_area_proxy_mm2: 신호 path의 bbox 면적. 리턴 경로가 바로 아래
    있다고 가정 못 하면 loop 면적 ∝ 방사. plane 있으면 h × length.
  - edge_proximity: board bbox 가장자리 근처 (< 3W) 를 지나는 고속
    net은 edge radiation 위험.

이건 solver가 아니라 review-checklist 수준의 first-order 지표 —
결과 JSON에 명시적으로 'first_order_estimate': true 마킹.
"""

from __future__ import annotations

import math
from typing import Any, Dict, Iterable, List, Optional, Tuple

from .path_geometry import iter_segments_mm, path_length_mm


# IPC-2152 외층 근사 계수 (I = k * dT^0.44 * A^0.725, A in mil^2)
_IPC_K_OUTER = 0.048
_IPC_K_INNER = 0.024
_MM_TO_MIL = 39.3701


def current_capacity_a(width_mm: float, thickness_mm: float, *,
                         delta_t_c: float = 10.0,
                         outer_layer: bool = True) -> Optional[float]:
    """IPC-2152 근사 허용 전류 (A). None if invalid geometry."""
    if width_mm <= 0 or thickness_mm <= 0:
        return None
    area_mil2 = (width_mm * _MM_TO_MIL) * (thickness_mm * _MM_TO_MIL)
    k = _IPC_K_OUTER if outer_layer else _IPC_K_INNER
    return k * (delta_t_c ** 0.44) * (area_mil2 ** 0.725)


def loop_area_proxy_mm2(path, grid, *,
                          dielectric_h_mm: float = 0.1) -> float:
    """First-order radiating loop area: trace length × dielectric height.
    Assumes return current directly beneath (best case). Real loop area
    is larger when the plane is split — that's what plane_split_crossings
    (Phase C) flags."""
    L = path_length_mm(path, grid)
    return L * dielectric_h_mm


def edge_proximity_flags(routed_paths, grid, *,
                           board_bbox_mm: Optional[Tuple[float, float, float, float]] = None,
                           margin_mm: float = 1.0
                           ) -> Dict[str, bool]:
    """Per net — True if any point comes within margin of the board edge.
    board_bbox defaults to the union bbox of all paths."""
    # Determine board bbox
    if board_bbox_mm is None:
        xs: List[float] = []
        ys: List[float] = []
        for pr in routed_paths.values():
            path = getattr(pr, 'path', None) or []
            for layer, ix, iy in path:
                x, y = grid.geom.cell_to_world(ix, iy)
                xs.append(x)
                ys.append(y)
        if not xs:
            return {}
        board_bbox_mm = (min(xs), min(ys), max(xs), max(ys))
    x0, y0, x1, y1 = board_bbox_mm
    out: Dict[str, bool] = {}
    for net, pr in routed_paths.items():
        path = getattr(pr, 'path', None) or []
        near = False
        for layer, ix, iy in path:
            x, y = grid.geom.cell_to_world(ix, iy)
            if (x - x0 < margin_mm or x1 - x < margin_mm
                    or y - y0 < margin_mm or y1 - y < margin_mm):
                near = True
                break
        out[net] = near
    return out


def summarize_thermal_emi(routed_paths, grid, rules_by_net: Dict[str, Any],
                             stackup=None, *,
                             target_current_a: float = 0.5,
                             delta_t_c: float = 10.0) -> Dict[str, Any]:
    """First-order thermal + EMI summary. Marked as estimate."""
    thermal: Dict[str, Any] = {}
    under_capacity: List[str] = []
    for net, pr in routed_paths.items():
        path = getattr(pr, 'path', None) or []
        if not path:
            continue
        rule = rules_by_net.get(net)
        w = getattr(rule, 'width_mm', None) if rule else None
        t = None
        if stackup is not None:
            # dominant layer thickness
            layers = [L for L, _i, _j in path]
            if layers:
                dom = max(set(layers), key=layers.count)
                t = stackup.copper_thickness_mm(dom)
        if t is None:
            t = 0.035
        if w:
            cap = current_capacity_a(w, t, delta_t_c=delta_t_c)
            thermal[net] = {
                'width_mm': w,
                'copper_mm': t,
                'current_capacity_a': round(cap, 4) if cap else None,
            }
            if cap is not None and cap < target_current_a:
                under_capacity.append(net)

    # EMI proxies
    h = 0.1
    if stackup is not None:
        # first dielectric under the first signal layer
        sigs = stackup.signal_layer_names() if hasattr(
            stackup, 'signal_layer_names') else ()
        if sigs:
            d = stackup.dielectric_below(sigs[0])
            if d:
                h = d.thickness_mm
    loop_areas: Dict[str, float] = {}
    for net, pr in routed_paths.items():
        path = getattr(pr, 'path', None) or []
        if path:
            loop_areas[net] = round(
                loop_area_proxy_mm2(path, grid, dielectric_h_mm=h), 4)
    edges = edge_proximity_flags(routed_paths, grid)

    return {
        'first_order_estimate': True,
        'thermal': {
            'per_net':             thermal,
            'under_capacity_nets': sorted(under_capacity),
            'target_current_a':    target_current_a,
            'delta_t_c':           delta_t_c,
        },
        'emi': {
            'loop_area_proxy_mm2':  loop_areas,
            'worst_loop_net':       (max(loop_areas, key=loop_areas.get)
                                      if loop_areas else None),
            'edge_proximity_nets':  sorted(n for n, v in edges.items() if v),
            'dielectric_h_mm':      h,
        },
    }
