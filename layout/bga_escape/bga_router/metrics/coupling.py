# net 쌍별 parallel run length (NxN coupling matrix) 계산 — crosstalk 위험도 분석
"""Phase F-1 — per-pair coupling matrix.

shadow_crossings_2d (Phase B cross_net) gave a single design-level
count. For real crosstalk analysis we need per-pair "how many mm did
net A run parallel to net B at gap < threshold". Same-layer
brute-force; O(N² × seg_count²) but acceptable at PoC scale.

Output: a sparse coupling matrix listing pairs that meet a coupling
threshold (parallel within `angle_tol_deg`, gap < `gap_threshold_mm`),
sorted by descending coupling length. Aggressor identification +
crosstalk hotspot map.
"""

from __future__ import annotations

import math
from collections import defaultdict
from typing import Any, Dict, List, Optional, Tuple

from .path_geometry import iter_segments_mm


def _segments_by_layer(routed_paths, grid) -> Dict[str, List[tuple]]:
    """{layer: [(net, x0, y0, x1, y1), ...]} indexed by layer."""
    out: Dict[str, List[tuple]] = defaultdict(list)
    for net, pr in routed_paths.items():
        path = getattr(pr, 'path', None) if not isinstance(pr, list) else pr
        if not path:
            continue
        for layer, x0, y0, x1, y1 in iter_segments_mm(path, grid):
            out[layer].append((net, x0, y0, x1, y1))
    return out


def _parallel_overlap_length_mm(s1, s2, *,
                                   angle_tol_deg: float = 5.0,
                                   gap_threshold_mm: float = 0.30
                                   ) -> Optional[float]:
    """If two same-layer segments are parallel within tolerance AND the
    perpendicular gap is below threshold, return the overlap length in
    the common direction. Else None.
    """
    _na, x0a, y0a, x1a, y1a = s1
    _nb, x0b, y0b, x1b, y1b = s2
    dxa, dya = x1a - x0a, y1a - y0a
    dxb, dyb = x1b - x0b, y1b - y0b
    la = math.hypot(dxa, dya)
    lb = math.hypot(dxb, dyb)
    if la < 1e-9 or lb < 1e-9:
        return None
    cos_a = abs(dxa * dxb + dya * dyb) / (la * lb)
    if cos_a < math.cos(math.radians(angle_tol_deg)):
        return None
    # Perpendicular gap: distance from midpoint of A to line through B.
    mxa, mya = (x0a + x1a) / 2, (y0a + y1a) / 2
    # Line through B in (a, b, c) form: a*x + b*y + c = 0
    a_, b_ = dyb, -dxb
    c_ = dxb * y0b - dyb * x0b
    denom = math.hypot(a_, b_)
    if denom < 1e-12:
        return None
    gap = abs(a_ * mxa + b_ * mya + c_) / denom
    if gap > gap_threshold_mm:
        return None
    # Project both segments onto A's direction; intersect projection ranges.
    ux, uy = dxa / la, dya / la
    def proj(x, y):
        return (x - x0a) * ux + (y - y0a) * uy
    a_lo, a_hi = 0.0, la
    b_lo = min(proj(x0b, y0b), proj(x1b, y1b))
    b_hi = max(proj(x0b, y0b), proj(x1b, y1b))
    overlap_lo = max(a_lo, b_lo)
    overlap_hi = min(a_hi, b_hi)
    if overlap_hi <= overlap_lo:
        return None
    return overlap_hi - overlap_lo


def coupling_matrix(routed_paths, grid, *,
                      angle_tol_deg: float = 5.0,
                      gap_threshold_mm: float = 0.30,
                      ) -> Dict[Tuple[str, str], float]:
    """Per-pair total coupled length in mm. Symmetric — keys are
    (sorted(a, b)) so each pair appears once."""
    by_layer = _segments_by_layer(routed_paths, grid)
    pair_len: Dict[Tuple[str, str], float] = defaultdict(float)
    for layer, segs in by_layer.items():
        n = len(segs)
        for i in range(n):
            ni = segs[i][0]
            for j in range(i + 1, n):
                nj = segs[j][0]
                if ni == nj:
                    continue
                overlap = _parallel_overlap_length_mm(
                    segs[i], segs[j],
                    angle_tol_deg=angle_tol_deg,
                    gap_threshold_mm=gap_threshold_mm)
                if overlap is None:
                    continue
                key = (ni, nj) if ni < nj else (nj, ni)
                pair_len[key] += overlap
    return dict(pair_len)


def aggressors_per_net(matrix: Dict[Tuple[str, str], float], *,
                          min_length_mm: float = 0.5,
                          top_k: int = 5) -> Dict[str, List[Dict[str, Any]]]:
    """For each net, top-K aggressor nets above min_length_mm."""
    per_net: Dict[str, List[Tuple[str, float]]] = defaultdict(list)
    for (a, b), length in matrix.items():
        if length < min_length_mm:
            continue
        per_net[a].append((b, length))
        per_net[b].append((a, length))
    out: Dict[str, List[Dict[str, Any]]] = {}
    for net, pairs in per_net.items():
        pairs.sort(key=lambda x: -x[1])
        out[net] = [{'with': n, 'length_mm': round(L, 4)}
                     for n, L in pairs[:top_k]]
    return out


def summarize_coupling(routed_paths, grid, *,
                         angle_tol_deg: float = 5.0,
                         gap_threshold_mm: float = 0.30,
                         top_pairs: int = 10,
                         min_length_mm: float = 0.5,
                         ) -> Dict[str, Any]:
    """Top-N coupling pairs + per-net aggressor map."""
    matrix = coupling_matrix(routed_paths, grid,
                                angle_tol_deg=angle_tol_deg,
                                gap_threshold_mm=gap_threshold_mm)
    items = sorted(matrix.items(), key=lambda kv: -kv[1])
    top = [{'pair': list(k), 'length_mm': round(v, 4)}
            for k, v in items[:top_pairs] if v >= min_length_mm]
    total_coupled = sum(matrix.values())
    return {
        'top_pairs':        top,
        'pair_count':       len(matrix),
        'total_coupled_mm': round(total_coupled, 4),
        'aggressors_per_net': aggressors_per_net(matrix,
                                                    min_length_mm=min_length_mm,
                                                    top_k=5),
        'thresholds': {
            'angle_tol_deg':    angle_tol_deg,
            'gap_threshold_mm': gap_threshold_mm,
            'min_length_mm':    min_length_mm,
        },
    }
