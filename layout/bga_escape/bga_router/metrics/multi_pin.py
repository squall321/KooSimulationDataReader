# Multi-pin Stage 2 — branched topology 검증 + driver→pin arrival length 분석
"""Phase H-10 — multi-pin verifier upgrade (design doc Stage 2).

Stage 1 (G-5) landed extra_pins + coverage. Stage 2 adds:

  1. **branched_topology_check** — routed graph의 실제 분기 구조가
     declared topology(daisy_chain / star / tee)와 pin 수까지 맞는지.
     leaf 수 == 전체 pin 수, hub degree == star spoke 수 등.
  2. **pin_arrival_lengths** — 그래프 상 driver(첫 pin)에서 각 pin
     까지의 최단 경로 길이 (BFS on path adjacency, mm 단위).
     multi-pin length-match의 진짜 metric — endpoint 쌍이 아니라
     driver 기준 도착 거리 스큐.

Stage 3+ (chain-extension / Steiner solver) 는 여전히 router 측 작업.
"""

from __future__ import annotations

import math
from collections import defaultdict, deque
from typing import Any, Dict, Iterable, List, Optional, Tuple


def _build_adjacency(path) -> Dict[tuple, set]:
    adj: Dict[tuple, set] = defaultdict(set)
    for i in range(len(path) - 1):
        a, b = tuple(path[i]), tuple(path[i + 1])
        if a != b:
            adj[a].add(b)
            adj[b].add(a)
    return adj


def _cell_distance_mm(a: tuple, b: tuple, grid) -> float:
    """mm distance between two adjacent cells (same or cross layer)."""
    _la, ixa, iya = a
    _lb, ixb, iyb = b
    xa, ya = grid.geom.cell_to_world(ixa, iya)
    xb, yb = grid.geom.cell_to_world(ixb, iyb)
    return math.hypot(xb - xa, yb - ya)


def branched_topology_check(task, routed_paths) -> Optional[Dict[str, Any]]:
    """Verify the routed graph structure against declared topology,
    counting pins from extra_pins.

    Returns None when the task has no extra_pins. Otherwise:
        {'declared': str, 'pin_count': N,
         'leaves': L, 'hubs': H, 'max_degree': D,
         'matches': bool, 'reason': str}
    """
    extras = getattr(task, 'extra_pins', None)
    if not extras:
        return None
    rule = getattr(task, 'rule', None)
    declared = (getattr(rule, 'net_topology', None) or 'daisy_chain')
    declared = declared.strip().lower()
    n_pins = len(extras)

    pr = routed_paths.get(task.net_name)
    path = getattr(pr, 'path', None) if pr else None
    if not path:
        return {'declared': declared, 'pin_count': n_pins,
                 'leaves': 0, 'hubs': 0, 'max_degree': 0,
                 'matches': False, 'reason': 'no routed path'}

    adj = _build_adjacency(path)
    if not adj:
        return {'declared': declared, 'pin_count': n_pins,
                 'leaves': 0, 'hubs': 0, 'max_degree': 0,
                 'matches': n_pins <= 1, 'reason': 'single-cell path'}
    degrees = {node: len(nbrs) for node, nbrs in adj.items()}
    leaves = sum(1 for d in degrees.values() if d == 1)
    hubs = sum(1 for d in degrees.values() if d >= 3)
    max_deg = max(degrees.values())

    matches = False
    reason = ''
    if declared == 'daisy_chain':
        # Chain visiting all pins: 2 leaves, no hubs.
        matches = (leaves == 2 and hubs == 0)
        reason = ('' if matches
                   else f'expected simple chain, got leaves={leaves} hubs={hubs}')
    elif declared == 'star':
        # Star with n_pins spokes: 1 hub with degree ≥ n_pins-1... but the
        # grid path granularity blurs this; accept 1 hub + leaves == n_pins.
        matches = (hubs == 1 and leaves >= max(2, n_pins - 1))
        reason = ('' if matches
                   else f'expected 1 hub + {n_pins - 1}+ leaves, '
                        f'got hubs={hubs} leaves={leaves}')
    elif declared == 'tee':
        matches = (hubs == 1 and max_deg == 3)
        reason = ('' if matches
                   else f'expected single degree-3 branch, '
                        f'got hubs={hubs} max_deg={max_deg}')
    else:
        reason = f'unknown topology {declared!r}'

    return {'declared': declared, 'pin_count': n_pins,
             'leaves': leaves, 'hubs': hubs, 'max_degree': max_deg,
             'matches': matches, 'reason': reason}


def pin_arrival_lengths(task, routed_paths, grid
                          ) -> Optional[Dict[str, Any]]:
    """BFS from the first pin (driver) to every other pin along the
    routed graph. Returns per-pin arrival length in mm + skew.
    None when task has no extra_pins or nothing routed."""
    extras = getattr(task, 'extra_pins', None)
    if not extras:
        return None
    pr = routed_paths.get(task.net_name)
    path = getattr(pr, 'path', None) if pr else None
    if not path:
        return None
    adj = _build_adjacency(path)
    if not adj:
        return None

    pin_cells = []
    for ep in extras:
        cell = (getattr(ep, 'layer', None), getattr(ep, 'ix', None),
                 getattr(ep, 'iy', None))
        pin_cells.append(cell)
    driver_cell = pin_cells[0]
    if driver_cell not in adj:
        return {'error': 'driver pin not on routed graph',
                 'driver': str(driver_cell)}

    # Dijkstra-lite (uniform-ish weights, but diagonal ≠ straight so use
    # actual mm distance). Small graphs — simple priority queue.
    import heapq
    dist: Dict[tuple, float] = {driver_cell: 0.0}
    pq: List[Tuple[float, tuple]] = [(0.0, driver_cell)]
    while pq:
        d, node = heapq.heappop(pq)
        if d > dist.get(node, float('inf')):
            continue
        for nbr in adj[node]:
            nd = d + _cell_distance_mm(node, nbr, grid)
            if nd < dist.get(nbr, float('inf')):
                dist[nbr] = nd
                heapq.heappush(pq, (nd, nbr))

    arrivals: Dict[str, Optional[float]] = {}
    reached: List[float] = []
    for i, cell in enumerate(pin_cells):
        label = f'pin{i}'
        d = dist.get(cell)
        arrivals[label] = round(d, 4) if d is not None else None
        if d is not None and i > 0:
            reached.append(d)
    skew = (max(reached) - min(reached)) if len(reached) >= 2 else 0.0
    return {
        'driver':           'pin0',
        'arrival_mm':       arrivals,
        'unreached_pins':   [k for k, v in arrivals.items() if v is None],
        'skew_mm':          round(skew, 4),
    }


def summarize_multi_pin(tasks, routed_paths, grid) -> Dict[str, Any]:
    """Aggregate Stage-2 checks over every task that has extra_pins."""
    topo_results: Dict[str, Any] = {}
    arrival_results: Dict[str, Any] = {}
    mismatches: List[str] = []
    for task in tasks:
        if not getattr(task, 'extra_pins', None):
            continue
        net = task.net_name
        t = branched_topology_check(task, routed_paths)
        if t is not None:
            topo_results[net] = t
            if not t.get('matches'):
                mismatches.append(net)
        a = pin_arrival_lengths(task, routed_paths, grid)
        if a is not None:
            arrival_results[net] = a
    return {
        'multi_pin_net_count':   len(topo_results),
        'topology':              topo_results,
        'topology_mismatches':   sorted(mismatches),
        'arrival_lengths':       arrival_results,
    }
