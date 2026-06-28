# 라우팅 경로의 analytical SI 메트릭 — Wadell Z0/Zdiff + DC R + 변동/한계 플래그
"""Closed-form (no EM solver) signal-integrity metrics.

Z0 / Zdiff formulas are the standard Hammerstad-Jensen microstrip and
Wadell stripline closed forms used by Saturn PCB / Polar Si9000. The
edge-coupled diff approximations follow the same lineage. Tighter
coupling regimes (s/h < 0.5) and very thin dielectrics (h < 0.05 mm)
fall outside the formula's validated range and are flagged via
``is_marginal_formula``.
"""

from __future__ import annotations

import math
from typing import Dict, Iterable, Optional, Tuple

from .path_geometry import iter_segments_mm, path_length_mm
from .stackup import StackupSpec


# ---------------------------------------------------------------------------
# Closed-form Z0 / Zdiff
# ---------------------------------------------------------------------------


def _er_eff_microstrip(w: float, h: float, er: float) -> float:
    """Hammerstad effective εr for microstrip."""
    if w <= 0 or h <= 0:
        return er
    u = w / h
    return (er + 1) / 2 + (er - 1) / 2 * (1 + 12 / u) ** -0.5


def z0_microstrip(w_mm: float, t_mm: float, h_mm: float, er: float) -> float:
    """Hammerstad-Jensen microstrip Z0 (Ω). Branches at w/h=1."""
    if w_mm <= 0 or h_mm <= 0 or er <= 0:
        return 0.0
    # Effective width correction for trace thickness (Hammerstad).
    if t_mm > 0:
        if w_mm > h_mm / (2 * math.pi):
            we = w_mm + (t_mm / math.pi) * (
                1 + math.log(2 * h_mm / t_mm))
        else:
            we = w_mm + (t_mm / math.pi) * (
                1 + math.log(4 * math.pi * w_mm / t_mm))
    else:
        we = w_mm
    u = we / h_mm
    er_eff = _er_eff_microstrip(we, h_mm, er)
    if u <= 1.0:
        return (60.0 / math.sqrt(er_eff)) * math.log(8.0 / u + u / 4.0)
    return (120.0 * math.pi) / (math.sqrt(er_eff) *
                                  (u + 1.393 + 0.667 * math.log(u + 1.444)))


def z0_stripline(w_mm: float, t_mm: float, h_mm: float, er: float) -> float:
    """Wadell symmetric stripline Z0 (Ω). ``h`` = total plane-to-plane
    dielectric height; trace centered between planes."""
    if w_mm <= 0 or h_mm <= 0 or er <= 0:
        return 0.0
    denom = 0.67 * math.pi * (t_mm + 0.8 * w_mm)
    if denom <= 0:
        return 0.0
    return (60.0 / math.sqrt(er)) * math.log(4.0 * h_mm / denom)


def zdiff_microstrip(w_mm: float, t_mm: float, h_mm: float,
                      s_mm: float, er: float) -> float:
    """Edge-coupled differential microstrip approximation."""
    z0 = z0_microstrip(w_mm, t_mm, h_mm, er)
    if z0 <= 0 or h_mm <= 0 or s_mm < 0:
        return 0.0
    return 2.0 * z0 * (1.0 - 0.48 * math.exp(-0.96 * s_mm / h_mm))


def zdiff_stripline(w_mm: float, t_mm: float, h_mm: float,
                     s_mm: float, er: float) -> float:
    """Edge-coupled differential stripline approximation."""
    z0 = z0_stripline(w_mm, t_mm, h_mm, er)
    if z0 <= 0 or h_mm <= 0 or s_mm < 0:
        return 0.0
    return 2.0 * z0 * (1.0 - 0.347 * math.exp(-2.9 * s_mm / h_mm))


# ---------------------------------------------------------------------------
# Per-net Z0 / per-pair Zdiff
# ---------------------------------------------------------------------------


def _dominant_layer(path) -> Optional[str]:
    """Return the layer with the most cells in this path."""
    if not path:
        return None
    counts: Dict[str, int] = {}
    for layer, _ix, _iy in path:
        counts[layer] = counts.get(layer, 0) + 1
    return max(counts, key=counts.get)


def _h_for_layer(stackup: StackupSpec, layer: str) -> Optional[float]:
    d = stackup.dielectric_below(layer)
    if d is None:
        d = stackup.dielectric_above(layer)
    return d.thickness_mm if d else None


def _er_for_layer(stackup: StackupSpec, layer: str) -> Optional[float]:
    d = stackup.dielectric_below(layer)
    if d is None:
        d = stackup.dielectric_above(layer)
    return d.er if d and d.er is not None else None


def z0_for_net(path, grid, rule, stackup: StackupSpec) -> Optional[float]:
    layer = _dominant_layer(path)
    if layer is None:
        return None
    w = getattr(rule, 'width_mm', None) if rule else None
    if not w or w <= 0:
        return None
    t = stackup.copper_thickness_mm(layer)
    if t is None:
        return None
    h = _h_for_layer(stackup, layer)
    er = _er_for_layer(stackup, layer)
    if h is None or er is None:
        return None
    if stackup.microstrip_layer(layer):
        return z0_microstrip(w, t, h, er)
    if stackup.stripline_layer(layer):
        # Stripline h is plane-to-plane (sum of dielectrics above+below).
        d_above = stackup.dielectric_above(layer)
        d_below = stackup.dielectric_below(layer)
        h_total = ((d_above.thickness_mm if d_above else 0.0)
                    + (d_below.thickness_mm if d_below else 0.0))
        if h_total <= 0:
            return None
        return z0_stripline(w, t, h_total, er)
    return None


def _pair_gap_estimate(path_p, path_n, grid) -> Optional[float]:
    """Median perpendicular gap between parallel same-layer segments
    of two diff-pair paths. Returns None if no parallel pairs found.
    """
    segs_p = list(iter_segments_mm(path_p, grid))
    segs_n = list(iter_segments_mm(path_n, grid))
    gaps: list = []
    for lp, x0p, y0p, x1p, y1p in segs_p:
        for ln, x0n, y0n, x1n, y1n in segs_n:
            if lp != ln:
                continue
            dxp, dyp = x1p - x0p, y1p - y0p
            dxn, dyn = x1n - x0n, y1n - y0n
            mp = math.hypot(dxp, dyp)
            mn = math.hypot(dxn, dyn)
            if mp < 1e-9 or mn < 1e-9:
                continue
            cos = abs(dxp * dxn + dyp * dyn) / (mp * mn)
            if cos < math.cos(math.radians(5)):
                continue
            # Perpendicular distance from P midpoint to N segment line.
            mxp, myp = (x0p + x1p) / 2, (y0p + y1p) / 2
            # Line N: ax + by + c = 0 from two points
            a = dyn
            b = -dxn
            c = dxn * y0n - dyn * x0n
            denom = math.hypot(a, b)
            if denom < 1e-12:
                continue
            d = abs(a * mxp + b * myp + c) / denom
            gaps.append(d)
    if not gaps:
        return None
    gaps.sort()
    return gaps[len(gaps) // 2]


def zdiff_for_pair(path_p, path_n, grid, rule, stackup: StackupSpec) -> Optional[float]:
    layer = _dominant_layer(path_p)
    if layer is None:
        return None
    w = getattr(rule, 'width_mm', None) if rule else None
    if not w or w <= 0:
        return None
    t = stackup.copper_thickness_mm(layer)
    h = _h_for_layer(stackup, layer)
    er = _er_for_layer(stackup, layer)
    if t is None or h is None or er is None:
        return None
    s = getattr(rule, 'pair_gap_mm', None) if rule else None
    if s is None or s <= 0:
        s = _pair_gap_estimate(path_p, path_n, grid)
    if s is None or s <= 0:
        return None
    if stackup.microstrip_layer(layer):
        return zdiff_microstrip(w, t, h, s, er)
    if stackup.stripline_layer(layer):
        d_above = stackup.dielectric_above(layer)
        d_below = stackup.dielectric_below(layer)
        h_total = ((d_above.thickness_mm if d_above else 0.0)
                    + (d_below.thickness_mm if d_below else 0.0))
        return zdiff_stripline(w, t, h_total, s, er)
    return None


# ---------------------------------------------------------------------------
# Variance + DC R + stubs + marginal-formula flag
# ---------------------------------------------------------------------------


def z0_variance_pct(routed_paths, grid, rules_by_net: dict,
                     stackup: StackupSpec) -> float:
    vals = []
    for net, pr in routed_paths.items():
        path = getattr(pr, 'path', None) if not isinstance(pr, list) else pr
        if not path:
            continue
        rule = rules_by_net.get(net)
        z = z0_for_net(path, grid, rule, stackup)
        if z is not None and z > 0:
            vals.append(z)
    if len(vals) < 2:
        return 0.0
    mean = sum(vals) / len(vals)
    return (max(vals) - min(vals)) / mean * 100.0 if mean > 0 else 0.0


def via_stub_length_mm(path, grid, rule, stackup: StackupSpec) -> float:
    """Sum of un-used via barrel length (back-drill candidate) across all
    vias in this path. For each layer transition, look up the declared
    via span in the stackup and subtract the actually-used span.

    Resolution rule (Phase D-4):
      1. If rule.via_type names a ViaSpan, use it.
      2. Else if exactly one via is declared, use it.
      3. Else use 'through' (full stack).
    Returns 0.0 when stackup has no via declarations.
    """
    if not stackup.vias:
        return 0.0
    via = None
    via_type_name = getattr(rule, 'via_type', None) if rule else None
    if via_type_name:
        via = stackup.via_by_name(via_type_name)
    if via is None and len(stackup.vias) == 1:
        via = stackup.vias[0]
    if via is None:
        via = stackup.via_by_name('through')
    if via is None:
        return 0.0
    total = 0.0
    prev_layer = None
    for layer, _ix, _iy in path:
        if prev_layer is not None and prev_layer != layer:
            stub = stackup.stub_length_for_via(via, prev_layer, layer)
            if stub is not None:
                total += stub
        prev_layer = layer
    return total


# Bulk copper resistivity at 20 °C (ohm·mm)
_RHO_CU = 1.68e-5


def branch_dc_resistance_mohm(path, grid, rule, stackup: StackupSpec
                                ) -> Optional[float]:
    """R = ρ * L / (w * t). Returns mΩ."""
    L_mm = path_length_mm(path, grid)
    w = getattr(rule, 'width_mm', None) if rule else None
    if not w or w <= 0:
        return None
    layer = _dominant_layer(path)
    t = stackup.copper_thickness_mm(layer) if layer else None
    if not t or t <= 0:
        return None
    ohm = _RHO_CU * L_mm / (w * t)
    return ohm * 1000.0


def is_marginal_formula(rule, stackup: StackupSpec, layer: str) -> Dict[str, bool]:
    h = _h_for_layer(stackup, layer) or 0.0
    s = getattr(rule, 'pair_gap_mm', None) if rule else None
    tight = bool(s and h > 0 and (s / h) < 0.5)
    thin = bool(h > 0 and h < 0.05)
    return {'tight_coupling': tight, 'thin_dielectric': thin}


# ---------------------------------------------------------------------------
# Aggregate
# ---------------------------------------------------------------------------


def summarize_si(routed_paths, grid, rules_by_net: dict,
                  stackup: StackupSpec) -> dict:
    z0: Dict[str, float] = {}
    zdiff: Dict[str, float] = {}
    stub: Dict[str, float] = {}
    rdc: Dict[str, float] = {}
    marginal: Dict[str, Dict[str, bool]] = {}

    pair_seen: set = set()

    def _path_of(p):
        return getattr(p, 'path', None) if not isinstance(p, list) else p

    for net, pr in routed_paths.items():
        path = _path_of(pr)
        if not path:
            continue
        rule = rules_by_net.get(net)
        layer = _dominant_layer(path)

        z = z0_for_net(path, grid, rule, stackup)
        if z is not None:
            z0[net] = round(z, 3)

        s = via_stub_length_mm(path, grid, rule, stackup)
        stub[net] = round(s, 4)

        r = branch_dc_resistance_mohm(path, grid, rule, stackup)
        if r is not None:
            rdc[net] = round(r, 3)

        if layer:
            marginal[net] = is_marginal_formula(rule, stackup, layer)

        partner = getattr(rule, 'pair_partner_net', None) if rule else None
        if partner and partner in routed_paths:
            label = '|'.join(sorted([net, partner]))
            if label in pair_seen:
                continue
            pair_seen.add(label)
            path_n = _path_of(routed_paths[partner])
            if path_n:
                zd = zdiff_for_pair(path, path_n, grid, rule, stackup)
                if zd is not None:
                    zdiff[label] = round(zd, 3)

    return {
        'Z0_single_ended_ohm':         z0,
        'Zdiff_ohm':                   zdiff,
        'Z0_variance_pct':             round(
            z0_variance_pct(routed_paths, grid, rules_by_net, stackup), 3),
        'via_stub_length_mm':          stub,
        'branch_dc_resistance_mohm':   rdc,
        'marginal_formulas':           marginal,
        'stackup_default_used':        stackup.is_default,
    }
