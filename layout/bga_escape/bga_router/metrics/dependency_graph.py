# NetRule의 driver/load/clock_domain로 net 의존성 그래프를 만들고 timing skew 계산
"""Phase F-3 — driver/load dependency graph.

Build a directed graph keyed by clock_domain (or driver net) so the
verifier can compute "every DDR_DQ net's length from DDR_CLK rising
edge". Without explicit driver/load metadata in NetRule, this falls
back to bus-group + pair-partner heuristics. Best-effort, never raises.
"""

from __future__ import annotations

from collections import defaultdict
from dataclasses import dataclass, field
from typing import Any, Dict, Iterable, List, Optional, Set, Tuple

from .bus_groups import assign_groups_for_nets


@dataclass(frozen=True)
class DependencyEdge:
    driver_net: str
    load_net: str
    relation: str            # 'clock_domain' | 'bus_group' | 'diff_pair'


@dataclass(frozen=True)
class DependencyGroup:
    label: str               # 'DDR4_CLK0' or 'bus:DDR4_DQ' etc.
    driver: Optional[str]    # net_name of the driver, or None
    loads: Tuple[str, ...]   # net_names of the loads


def build_dependency_groups(net_names: Iterable[str],
                              rules_by_net: Dict[str, Any]
                              ) -> List[DependencyGroup]:
    """Group nets by their declared/inferred clock domain."""
    names = list(net_names)
    by_clock: Dict[str, List[str]] = defaultdict(list)
    declared_driver: Dict[str, str] = {}

    for net in names:
        rule = rules_by_net.get(net)
        if rule is None:
            continue
        cd = getattr(rule, 'clock_domain', None)
        if cd:
            by_clock[cd].append(net)
            # The net whose name matches the clock_domain literal is the
            # driver; if no exact match, the first net registered remains
            # without a driver (None) and we report the group anyway.
            if net == cd:
                declared_driver[cd] = net
        if getattr(rule, 'driver_pin', None):
            # Even without clock_domain, declaring a driver implies this
            # net IS the driver of its own group.
            by_clock.setdefault('net:' + net, []).append(net)
            declared_driver['net:' + net] = net
        for load in getattr(rule, 'load_pins', ()) or ():
            # load_pins are pin names; we don't resolve them to nets
            # here (would require the EDA mapping). Record as note.
            # The driver/load relation is captured via explicit fields
            # elsewhere; this dict is just for clock_domain clustering.
            pass

    # Fallback — bus group of size ≥3 forms an implicit "bus_group"
    # dependency cluster (no explicit driver).
    bus_map = assign_groups_for_nets(names, min_members=3)
    bus_clusters: Dict[str, List[str]] = defaultdict(list)
    for net, bus in bus_map.items():
        bus_clusters['bus:' + bus].append(net)
    # Only add bus clusters NOT already covered by an explicit clock_domain
    already_covered: Set[str] = set()
    for nets in by_clock.values():
        already_covered.update(nets)
    for label, nets in bus_clusters.items():
        uncovered = [n for n in nets if n not in already_covered]
        if len(uncovered) >= 3:
            by_clock[label].extend(uncovered)

    out: List[DependencyGroup] = []
    for label, nets in sorted(by_clock.items(), key=lambda kv: (-len(kv[1]), kv[0])):
        driver = declared_driver.get(label)
        loads = tuple(sorted(n for n in nets if n != driver))
        out.append(DependencyGroup(label=label, driver=driver, loads=loads))
    return out


def build_edges(groups: List[DependencyGroup]) -> List[DependencyEdge]:
    """One DependencyEdge per (driver → load) pair within each group."""
    edges: List[DependencyEdge] = []
    for g in groups:
        if g.driver is None:
            relation = ('bus_group' if g.label.startswith('bus:')
                         else 'clock_domain')
            # No explicit driver — pick the lexically-first member as
            # synthetic anchor so downstream tools have something to use.
            anchor = g.loads[0] if g.loads else None
            for n in g.loads:
                if anchor and n != anchor:
                    edges.append(DependencyEdge(
                        driver_net=anchor, load_net=n,
                        relation=relation))
        else:
            for n in g.loads:
                edges.append(DependencyEdge(
                    driver_net=g.driver, load_net=n,
                    relation='clock_domain'))
    return edges


def compute_load_lengths(routed_paths, grid,
                            groups: List[DependencyGroup]) -> Dict[str, Any]:
    """For each group, compute load-vs-driver length skew (mm).
    Returns { label: {'driver': ..., 'load_lengths_mm': {net: mm},
                       'skew_mm': max-min, 'spread_pct': ...} }.
    Falls back to first member as driver when group.driver is None.
    """
    from .path_geometry import path_length_mm
    out: Dict[str, Any] = {}
    for g in groups:
        members = list(g.loads)
        if g.driver:
            members.insert(0, g.driver)
        lengths: Dict[str, float] = {}
        for net in members:
            pr = routed_paths.get(net)
            if pr is None:
                continue
            path = getattr(pr, 'path', None)
            if not path:
                continue
            lengths[net] = round(path_length_mm(path, grid), 4)
        if not lengths:
            continue
        vals = list(lengths.values())
        mn, mx = min(vals), max(vals)
        spread_pct = ((mx - mn) / mx * 100.0) if mx > 0 else 0.0
        out[g.label] = {
            'driver':           g.driver,
            'load_lengths_mm':  lengths,
            'skew_mm':          round(mx - mn, 4),
            'spread_pct':       round(spread_pct, 3),
            'member_count':     len(lengths),
        }
    return out


def summarize_dependencies(net_names: Iterable[str], rules_by_net: dict,
                              routed_paths, grid) -> Dict[str, Any]:
    """High-level summary for the eval JSON."""
    groups = build_dependency_groups(net_names, rules_by_net)
    edges = build_edges(groups)
    skews = compute_load_lengths(routed_paths, grid, groups)
    return {
        'groups': [
            {'label':  g.label,
              'driver': g.driver,
              'loads':  list(g.loads),
              'count':  len(g.loads) + (1 if g.driver else 0)}
            for g in groups
        ],
        'edges': [
            {'driver': e.driver_net,
              'load':   e.load_net,
              'relation': e.relation}
            for e in edges
        ],
        'load_length_skew': skews,
    }
