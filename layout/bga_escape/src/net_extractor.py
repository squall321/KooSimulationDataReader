"""Phase 4 Layer 2B — Net Extractor.

Convert EDA placement + routing spec + pin→net mapping into a flat list of
:class:`RoutingTask` units that the PathFinder negotiation loop (Layer 2C)
can iterate over.

Algorithm — see ``docs/02-design/2026-05-27-phase4-layer2-interface.md``
section 2B for the immutable contract. Summary:

  1. Group ``pin_to_net`` by net_name → endpoints.
  2. 2-pin net → one ``RoutingTask`` (source/sink deterministically ordered).
  3. 1-pin (dangling) net → silently skipped (caller can detect by absence).
  4. 3+ pin net (Phase 43-3+) → decomposed into 2-pin sub-tasks based on
     ``NetRule.net_topology``:
       - ``'daisy_chain'`` (default) → greedy nearest-neighbour walk → N-1 segments
       - ``'star'``                  → 1-median center → N-1 spokes from center
       - ``'tee'``                   → synthetic centroid branch → up to N segments
     Unknown topology values raise ``ValueError`` (Phase 44 hardening).
  5. Endpoint world coordinates follow the same convention as
     ``cost_grid_blockers._rotate_then_translate``: mirror x first, then
     CCW rotation, then translate by the component placement.
  6. ``spec.resolve`` decides the per-net :class:`NetRule`. Errors propagate.
  7. Diff-pair detection (``_P``/``_N`` or ``+``/``-`` suffix) links pair
     halves via ``pair_partner``. Both halves must resolve to the *same*
     :class:`NetRule` (otherwise ``NotImplementedError`` — coupled router
     needs a consistent spec at this phase). Unpaired partners are treated
     as single nets.
"""

from __future__ import annotations

import math
from collections import defaultdict
from dataclasses import dataclass
from typing import Dict, List, Optional, Tuple

from src.cost_grid import CostGrid
from src.eda_parser import Component, EdaData, Package, PackagePin
from src.routing_spec import NetRule, RoutingSpec


# ---------------------------------------------------------------------------
# Public dataclasses (contract — see Phase 4 Layer 2 interface doc)
# ---------------------------------------------------------------------------


@dataclass
class NetEndpoint:
    """One end of a net — a pin on a component, resolved to a grid cell.

    Phase 45-10: ``is_synthetic=True`` marks endpoints that do NOT
    correspond to any real component pin (currently: the centroid branch
    cell produced by ``_decompose_tee`` for multi-pin nets with
    ``net_topology='tee'``). Downstream consumers that resolve
    ``ref_des`` back through ``eda.components`` or
    ``packages.pins`` MUST skip synthetic endpoints — they will not be
    found and the lookup will raise / produce nonsense placements.

    Real pins should leave the default ``is_synthetic=False``.
    """
    net_name: str
    ref_des: str
    pin_name: str
    layer: str
    ix: int
    iy: int
    is_synthetic: bool = False


@dataclass
class RoutingTask:
    """One net's routing job: source/sink + the resolved NetRule.

    ``pair_partner`` is set (and ``is_pair=True``) when this net is one half
    of a differential pair whose other half is also present in the task list.

    Phase E-5 — ``extra_pins`` carries additional NetEndpoint(s) for nets
    that have 3+ load pins (DDR address bus, fan-out clocks, etc.). The
    current 2-pin escape router IGNORES this field — it is consumer-side
    metadata for downstream tools (e.g. star/tee topology verifier,
    future multi-pin router). source/sink stay as primary endpoints
    for backward-compat. See docs/01-plan/2026-06-29-multi-pin-router-design.md.
    """
    net_name: str
    source: NetEndpoint
    sink: NetEndpoint
    rule: NetRule
    is_pair: bool = False
    pair_partner: Optional['RoutingTask'] = None
    extra_pins: Optional[Tuple[NetEndpoint, ...]] = None


# ---------------------------------------------------------------------------
# Geometry helper (same convention as cost_grid_blockers._rotate_then_translate)
# ---------------------------------------------------------------------------


def _rotate_then_translate(
    local_x_mm: float,
    local_y_mm: float,
    rotation_deg: float,
    mirrored: bool,
    origin_x_mm: float,
    origin_y_mm: float,
) -> Tuple[float, float]:
    """Mirror x (if requested), rotate CCW by ``rotation_deg``, translate.

    Matches the convention used elsewhere in the toolchain (notably
    ``cost_grid_blockers``) so pad blockers and pin endpoints always agree.
    """
    x = -local_x_mm if mirrored else local_x_mm
    y = local_y_mm
    theta = math.radians(rotation_deg)
    cos_t = math.cos(theta)
    sin_t = math.sin(theta)
    rx = cos_t * x - sin_t * y
    ry = sin_t * x + cos_t * y
    return origin_x_mm + rx, origin_y_mm + ry


# ---------------------------------------------------------------------------
# Diff-pair name detection
# ---------------------------------------------------------------------------


def _rules_match_ignoring_pair_metadata(a: NetRule, b: NetRule) -> bool:
    """Phase 34 fix: compare two NetRules for diff-pair compatibility,
    ignoring fields that legitimately differ between paired halves:

      - ``pair_partner_net`` — each half points at the other.
      - ``rule_source`` — debug-only, differs by net name.

    Everything else (layers, escape_sides, width_mm, length_match_group,
    pair_gap_mm, pair_skew_max_mm, clearance_mm, priority, via_type,
    skip_routing, preferred_layers) must match for the pair to be
    coupled-routable.
    """
    import dataclasses as _dc
    a_norm = _dc.replace(a, pair_partner_net=None, rule_source='')
    b_norm = _dc.replace(b, pair_partner_net=None, rule_source='')
    return a_norm == b_norm


def _split_pair_name(net_name: str) -> Optional[Tuple[str, str]]:
    """Return ``(base, side)`` with side in {'P','N'}, or None when the name
    is not one half of a differential pair.

    Recognised suffixes:
      ``_P`` / ``_N``  → P / N
      ``+`` / ``-``    → P / N
    """
    if net_name.endswith('_P'):
        return net_name[:-2], 'P'
    if net_name.endswith('_N'):
        return net_name[:-2], 'N'
    if net_name.endswith('+'):
        return net_name[:-1], 'P'
    if net_name.endswith('-'):
        return net_name[:-1], 'N'
    return None


# ---------------------------------------------------------------------------
# Internal: build one endpoint
# ---------------------------------------------------------------------------


def _decompose_daisy_chain(
    endpoints: List['NetEndpoint'],
) -> List[Tuple['NetEndpoint', 'NetEndpoint']]:
    """Order endpoints by a nearest-neighbour walk → emit N-1 segments.

    Start from the geometrically-extreme point (min ix+iy) for determinism,
    then greedily pick the closest unvisited endpoint as the next step.
    Returns a list of (source, sink) pairs ready for RoutingTask.
    """
    if len(endpoints) < 2:
        return []
    remaining = list(endpoints)
    # Deterministic start: smallest (ix+iy, ix, iy, ref_des, pin_name)
    remaining.sort(key=lambda e: (e.ix + e.iy, e.ix, e.iy,
                                    e.ref_des, e.pin_name))
    ordered = [remaining.pop(0)]
    while remaining:
        prev = ordered[-1]
        # Pick nearest by Chebyshev distance, tiebreak deterministic.
        def _d(e):
            return (max(abs(e.ix - prev.ix), abs(e.iy - prev.iy)),
                     e.ix, e.iy, e.ref_des, e.pin_name)
        remaining.sort(key=_d)
        ordered.append(remaining.pop(0))
    return [(ordered[i], ordered[i + 1]) for i in range(len(ordered) - 1)]


def _bfs_snap_to_unblocked(
    grid: CostGrid, layer: str, ix0: int, iy0: int, *, max_radius: int = 20,
) -> Tuple[int, int]:
    """Return the nearest (Chebyshev) unblocked, in-bounds cell at or
    around ``(ix0, iy0)`` on ``layer``. Raises ``ValueError`` if no
    such cell exists within ``max_radius`` cells.

    Used by ``_decompose_tee`` (Phase 45-24) so the synthetic branch
    endpoint never lands on a pad blocker.
    """
    if (grid.geom.contains_cell(ix0, iy0)
            and not grid.is_blocked(layer, ix0, iy0)):
        return ix0, iy0
    seen = {(ix0, iy0)}
    frontier = [(ix0, iy0)]
    for r in range(1, max_radius + 1):
        next_frontier: List[Tuple[int, int]] = []
        for (cx, cy) in frontier:
            for dx in (-1, 0, 1):
                for dy in (-1, 0, 1):
                    if dx == 0 and dy == 0:
                        continue
                    nx, ny = cx + dx, cy + dy
                    if (nx, ny) in seen:
                        continue
                    seen.add((nx, ny))
                    if not grid.geom.contains_cell(nx, ny):
                        continue
                    if not grid.is_blocked(layer, nx, ny):
                        return nx, ny
                    next_frontier.append((nx, ny))
        frontier = next_frontier
    raise ValueError(
        f"tee branch BFS-snap: no unblocked cell within {max_radius} "
        f"cells of ({ix0}, {iy0}) on layer {layer!r}; "
        f"branch cannot be placed"
    )


def _decompose_tee(
    endpoints: List['NetEndpoint'],
    grid: Optional[CostGrid] = None,
) -> List[Tuple['NetEndpoint', 'NetEndpoint']]:
    """Tee topology: synthesize a branch point at the geometric centroid,
    then emit N segments (branch → each pin).

    Differs from star (which picks a real pin as center) — the branch is a
    virtual cell at the mean (ix, iy) of all endpoints, on the first
    endpoint's layer. All sub-tasks share the original net_name so the
    branch cell can be shared.

    Phase 45-24: when ``grid`` is supplied, the synthetic branch cell is
    BFS-snapped to the nearest unblocked cell if the raw centroid lands
    on a blocker. Raises ``ValueError`` if no unblocked cell exists within
    20 cells (board edge or fully-blocked region).

    When ``grid`` is None (older tests / standalone use), no snap is
    performed and a blocked centroid will simply cause the segment solve
    to fail downstream.
    """
    if len(endpoints) < 2:
        return []
    layer = endpoints[0].layer
    # Phase 44 review fix #19: round() instead of // — Python floor
    # division biases toward -inf for negative-cell pins (asymmetric
    # behaviour vs the positive case). round() matches the geometric
    # centroid for both signs.
    n = len(endpoints)
    avg_ix = round(sum(e.ix for e in endpoints) / n)
    avg_iy = round(sum(e.iy for e in endpoints) / n)
    # Phase 45-24: snap to nearest unblocked cell if grid is provided.
    if grid is not None:
        avg_ix, avg_iy = _bfs_snap_to_unblocked(grid, layer, avg_ix, avg_iy)
    # Use a stable synthetic ref_des / pin_name so downstream tooling can
    # recognise these. Phase 45-10: is_synthetic=True is the contract;
    # downstream consumers that look up ref_des/pin_name MUST skip these.
    branch = NetEndpoint(
        net_name=endpoints[0].net_name,
        ref_des=f'__tee_branch_{endpoints[0].net_name}__',
        pin_name='__branch__',
        layer=layer,
        ix=avg_ix,
        iy=avg_iy,
        is_synthetic=True,
    )
    # Deterministic leaf order
    leaves = sorted(endpoints, key=lambda e: (e.ix, e.iy, e.ref_des, e.pin_name))
    # Phase 44 review fix #13: skip a leaf that coincides with the
    # synthetic branch — emitting a (branch, branch) segment would create
    # a zero-length routing task and trip the solver.
    return [(branch, leaf) for leaf in leaves
             if (leaf.ix, leaf.iy) != (branch.ix, branch.iy)]


def _decompose_star(
    endpoints: List['NetEndpoint'],
) -> List[Tuple['NetEndpoint', 'NetEndpoint']]:
    """Pick centroid endpoint → emit N-1 spokes from it to each leaf.

    Center selection: endpoint whose sum of Chebyshev distances to all
    others is minimal (i.e. graph-theoretic 1-median over Chebyshev).
    Deterministic tiebreak by (ix, iy, ref_des, pin_name).
    """
    if len(endpoints) < 2:
        return []
    best_idx = 0
    best_score = None
    for i, e in enumerate(endpoints):
        score = sum(max(abs(e.ix - o.ix), abs(e.iy - o.iy))
                     for j, o in enumerate(endpoints) if j != i)
        key = (score, e.ix, e.iy, e.ref_des, e.pin_name)
        if best_score is None or key < best_score:
            best_score = key
            best_idx = i
    center = endpoints[best_idx]
    leaves = [e for j, e in enumerate(endpoints) if j != best_idx]
    # Deterministic leaf order
    leaves.sort(key=lambda e: (e.ix, e.iy, e.ref_des, e.pin_name))
    return [(center, leaf) for leaf in leaves]


def _build_endpoint(
    net_name: str,
    ref_des: str,
    pin_name: str,
    component: Component,
    package: Package,
    rule: NetRule,
    grid: CostGrid,
) -> NetEndpoint:
    """Resolve (ref_des, pin_name) → :class:`NetEndpoint`."""
    pin: Optional[PackagePin] = None
    for p in package.pins:
        if p.name == pin_name:
            pin = p
            break
    if pin is None:
        raise ValueError(
            f"pin {pin_name!r} not found in package {package.name!r} "
            f"(component {ref_des!r}, net {net_name!r}); "
            f"package has {len(package.pins)} pin(s)"
        )

    world_x, world_y = _rotate_then_translate(
        pin.x_mm, pin.y_mm,
        component.rotation_deg, component.mirrored,
        component.x_mm, component.y_mm,
    )
    ix, iy = grid.geom.world_to_cell(world_x, world_y)

    if not rule.layers:
        # spec.resolve should already have rejected this, but guard anyway.
        raise ValueError(
            f"net {net_name!r} on {ref_des!r}: resolved NetRule has empty "
            f"layers — cannot pick a starting layer"
        )
    return NetEndpoint(
        net_name=net_name,
        ref_des=ref_des,
        pin_name=pin_name,
        layer=rule.layers[0],
        ix=ix,
        iy=iy,
    )


# ---------------------------------------------------------------------------
# Public API
# ---------------------------------------------------------------------------


def extract_routing_tasks(
    eda: EdaData,
    spec: RoutingSpec,
    grid: CostGrid,
    pin_to_net: Dict[Tuple[str, str], str],
) -> List[RoutingTask]:
    """Build the flat list of routing tasks for PathFinder.

    Parameters
    ----------
    eda
        Parsed ODB++ EDA data (components + packages).
    spec
        Routing spec; ``spec.resolve(net, ref_des)`` decides per-net layer
        candidates, escape sides, widths, etc.
    grid
        Cost grid providing geometry for world→cell conversion.
    pin_to_net
        Mapping ``(ref_des, pin_name) → net_name``.

    Returns
    -------
    list[RoutingTask]
        One task per 2-pin net. Dangling (1-pin) nets are skipped silently —
        callers can detect them by counting input pins vs output tasks.

    Raises
    ------
    ValueError
        Unknown ref_des in ``pin_to_net``; unknown package; pin not present
        in package.
    NotImplementedError
        Net has 3+ endpoints (multi-pin nets out of scope), or diff-pair
        halves resolve to different NetRules (coupled router needs a
        consistent spec at this phase).
    RoutingSpecError
        Propagated from ``spec.resolve`` when a net is not covered.
    """
    # Build ref_des → Component lookup once.
    comp_by_ref: Dict[str, Component] = {c.ref_des: c for c in eda.components}

    # 1. Group pin_to_net by net_name.
    net_to_endpoints: Dict[str, List[Tuple[str, str]]] = defaultdict(list)
    for (ref_des, pin_name), net_name in pin_to_net.items():
        if ref_des not in comp_by_ref:
            raise ValueError(
                f"pin_to_net references unknown ref_des {ref_des!r} "
                f"(pin {pin_name!r}, net {net_name!r}); "
                f"EDA has {len(comp_by_ref)} component(s)"
            )
        net_to_endpoints[net_name].append((ref_des, pin_name))

    # 2. For each net, build the RoutingTask (if 2-pin) or skip/raise.
    tasks: List[RoutingTask] = []
    # Preserve a deterministic order across nets too — sort net_name.
    for net_name in sorted(net_to_endpoints.keys()):
        endpoints = net_to_endpoints[net_name]
        n = len(endpoints)
        if n == 1:
            # Dangling — skip silently (documented in module docstring).
            continue

        # Phase 43-3: multi-pin nets (n>=3) are decomposed into 2-pin
        # sub-tasks driven by NetRule.net_topology:
        #   - 'daisy_chain' (default): nearest-neighbour walk → N-1 sequential segments
        #   - 'star': center pin → N-1 spokes
        #   - 'tee': PoC = daisy_chain (proper branch-point logic deferred)
        # All sub-tasks share the original net_name, so per-net occupancy
        # checks (own_net set difference) allow them to share cells —
        # exactly what a real multi-pin net needs (one logical net, many
        # physical segments).
        # spec.resolve is called once on the first endpoint; all sub-tasks
        # see the same NetRule.
        first_ref = sorted(endpoints)[0][0]
        rule = spec.resolve(net_name, first_ref)
        if not rule.layers:
            raise ValueError(
                f"net {net_name!r}: resolved NetRule has empty layers; "
                f"spec.resolve should have rejected this"
            )

        endpoints_built: List[NetEndpoint] = []
        for ref_des, pin_name in endpoints:
            comp = comp_by_ref[ref_des]
            pkg = eda.packages.get(comp.pkg_name)
            if pkg is None:
                raise ValueError(
                    f"component {ref_des!r} references unknown package "
                    f"{comp.pkg_name!r}; EDA knows "
                    f"{sorted(eda.packages.keys())[:5]}..."
                )
            endpoints_built.append(
                _build_endpoint(net_name, ref_des, pin_name, comp, pkg,
                                  rule, grid)
            )

        if n == 2:
            # Original 2-pin path: deterministic source/sink ordering.
            ep_sorted = sorted(endpoints_built,
                                 key=lambda e: (e.ref_des, e.pin_name))
            tasks.append(RoutingTask(
                net_name=net_name,
                source=ep_sorted[0],
                sink=ep_sorted[1],
                rule=rule,
            ))
        else:
            # Phase 44 review fix #12: strict topology validation — typos
            # like 'star ' or 'STAR' or unknown 'mesh' should fail loud
            # rather than silently default to daisy_chain.
            topology = (rule.net_topology or 'daisy_chain').strip().lower()
            if topology not in ('daisy_chain', 'star', 'tee'):
                raise ValueError(
                    f"net {net_name!r}: unsupported net_topology "
                    f"{rule.net_topology!r}; "
                    f"expected one of 'daisy_chain' / 'star' / 'tee'"
                )
            if topology == 'star':
                segments = _decompose_star(endpoints_built)
            elif topology == 'tee':
                # Phase 45-24: pass grid so the branch snaps off blockers.
                # Phase 45-review fix H3: a single net whose centroid has
                # no unblocked cell within max_radius must NOT abort the
                # entire extraction (other nets are independent). Skip
                # this one net with a warning, keep going.
                try:
                    segments = _decompose_tee(endpoints_built, grid)
                except ValueError as e:
                    import warnings
                    warnings.warn(
                        f"tee decomposition failed for net {net_name!r}: "
                        f"{e}; skipping this net. Other nets still extracted.",
                        RuntimeWarning, stacklevel=2,
                    )
                    continue
            else:
                segments = _decompose_daisy_chain(endpoints_built)
            # Phase G-5 — Stage 1 data layer: attach the full endpoint
            # tuple to the FIRST sub-task via extra_pins so downstream
            # tools (verifier / dependency_graph / standards) can see
            # the true multi-pin cardinality without reconstructing it.
            # Router itself still consumes source/sink of each sub-task.
            extras_tuple = tuple(endpoints_built)
            for i, (src_ep, snk_ep) in enumerate(segments):
                tasks.append(RoutingTask(
                    net_name=net_name,
                    source=src_ep,
                    sink=snk_ep,
                    rule=rule,
                    extra_pins=extras_tuple if i == 0 else None,
                ))

    # 3. Diff-pair linking.
    _link_diff_pairs(tasks)
    return tasks


def _link_diff_pairs(tasks: List[RoutingTask]) -> None:
    """Find ``_P``/``_N`` (or ``+``/``-``) pairs and link them.

    Phase 3.4b: ``rule.pair_partner_net`` is consulted first as an explicit
    override (lets users pair nets whose names don't follow the suffix
    convention, e.g. ``USB_DP``/``USB_DM``). The ``_P``/``_N`` heuristic is
    a fall-back for tasks without an explicit override.

    Both halves must resolve to the same :class:`NetRule`; mismatch raises
    :class:`NotImplementedError`. Unpaired partners (only one side present)
    are left as ordinary single-net tasks.
    """
    by_name: Dict[str, RoutingTask] = {t.net_name: t for t in tasks}

    # 1. Explicit pair_partner_net overrides (Phase 3.4b).
    linked_via_override: set = set()
    for t in tasks:
        partner_name = t.rule.pair_partner_net
        if not partner_name:
            continue
        partner = by_name.get(partner_name)
        if partner is None:
            # Partner not in task list (maybe single-pin, dangling, etc.) —
            # treat as a lone half, no linking.
            continue
        if t.pair_partner is partner:
            continue   # already linked from the partner side
        if partner.rule.pair_partner_net not in (None, t.net_name):
            raise ValueError(
                f"diff-pair override mismatch: {t.net_name!r}.pair_partner_net"
                f"={partner_name!r}, but {partner_name!r}.pair_partner_net="
                f"{partner.rule.pair_partner_net!r}"
            )
        # Phase 34 fix: pair halves naturally carry asymmetric
        # pair_partner_net (each points at the other) and may have
        # different rule_source strings. Compare rules ignoring these
        # two fields — everything else (layers, width, length_match_group)
        # must match.
        if not _rules_match_ignoring_pair_metadata(t.rule, partner.rule):
            raise NotImplementedError(
                f"diff-pair (explicit) {t.net_name!r}/{partner.net_name!r}: "
                f"halves resolve to different NetRules; coupled routing "
                f"requires a consistent spec"
            )
        t.is_pair = True
        partner.is_pair = True
        t.pair_partner = partner
        partner.pair_partner = t
        linked_via_override.add(t.net_name)
        linked_via_override.add(partner.net_name)

    # 2. Fallback: _P/_N / +/- suffix heuristic for the rest.
    by_base: Dict[str, Dict[str, RoutingTask]] = defaultdict(dict)
    for t in tasks:
        if t.net_name in linked_via_override:
            continue
        sp = _split_pair_name(t.net_name)
        if sp is None:
            continue
        base, side = sp
        # Reject duplicate sides on the same base (would mean two nets named
        # CLK_P, CLK_P — caller fed inconsistent data).
        if side in by_base[base]:
            raise ValueError(
                f"diff-pair base {base!r} has two {side!r}-side nets: "
                f"{by_base[base][side].net_name!r} and {t.net_name!r}"
            )
        by_base[base][side] = t

    for base, sides in by_base.items():
        if 'P' not in sides or 'N' not in sides:
            # Lone half — treat as single-net task.
            continue
        p_task = sides['P']
        n_task = sides['N']
        if p_task.rule != n_task.rule:
            raise NotImplementedError(
                f"diff-pair {base!r}: halves {p_task.net_name!r} and "
                f"{n_task.net_name!r} resolve to different NetRules "
                f"(P.rule={p_task.rule}, N.rule={n_task.rule}); "
                f"coupled routing requires a consistent spec — make both "
                f"halves match in the routing spec"
            )
        p_task.is_pair = True
        n_task.is_pair = True
        p_task.pair_partner = n_task
        n_task.pair_partner = p_task
        # Phase 53-1: also update task.rule with explicit pair_partner_net
        # so CoupledNegotiator._split_pairs_and_solos (which reads rule,
        # not the task's pair_partner field) sees the suffix-detected
        # pair. Without this, suffix-named pairs were silently invisible
        # to the coupled solver — it just routed both as solo nets.
        import dataclasses as _dc
        if not p_task.rule.pair_partner_net:
            p_task.rule = _dc.replace(p_task.rule,
                                        pair_partner_net=n_task.net_name)
        if not n_task.rule.pair_partner_net:
            n_task.rule = _dc.replace(n_task.rule,
                                        pair_partner_net=p_task.net_name)
