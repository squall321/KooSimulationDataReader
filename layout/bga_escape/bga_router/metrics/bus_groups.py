# net 이름 패턴에서 버스 그룹 (DDR_DQ[0..7] 등) 을 자동 추출하는 그룹화기
"""Phase F-2 — bus auto-detection from net names.

The current `NetRule.length_match_group` is user-declared. Many real
designs don't go through that step and instead rely on naming
conventions: `DDR_DQ0`/`DQ1`/.../`DQ7`, `PCIE0_RX_P`/`PCIE0_TX_P`, etc.
This module extracts those bus groups so the verifier can apply
length-match / coupling analysis to them without manual annotation.

Output is consumed by:
  - check_length_group fallback (when NetRule has no explicit group)
  - coupling matrix (F-1) — same-bus pairs warrant tighter inspection
  - standards (existing) — already classifies per-net; bus shapes the
    pass/fail aggregation
"""

from __future__ import annotations

import re
from collections import defaultdict
from dataclasses import dataclass
from typing import Dict, Iterable, List, Optional, Tuple


# Capture trailing index — bus_label is the prefix, idx is the trailing
# number. Examples:
#   DDR4_DQ0          → bus='DDR4_DQ',         idx=0
#   PCIE0_RX_P        → bus='PCIE_RX_P',       idx=0     (gen number absorbed)
#   ETH_TXD0          → bus='ETH_TXD',         idx=0
#   DDR4_DQ0_P        → bus='DDR4_DQ_P',       idx=0     (P/N suffix preserved)
#   net042            → bus='net',             idx=42
_TRAILING_INDEX_RE = re.compile(r'^(.*?)(\d+)((?:_[PNM])?)$')


@dataclass(frozen=True)
class BusGroup:
    label: str                          # 'DDR4_DQ'
    members: Tuple[str, ...]            # ('DDR4_DQ0', 'DDR4_DQ1', ...)
    is_diff_pair_bus: bool = False      # True iff every member has _P or _N suffix


def _split_index(net_name: str) -> Optional[Tuple[str, int, str]]:
    """Return (prefix, idx, suffix) or None when no trailing index found."""
    m = _TRAILING_INDEX_RE.match(net_name)
    if not m:
        return None
    prefix, idx_str, suffix = m.group(1), m.group(2), m.group(3)
    try:
        idx = int(idx_str)
    except ValueError:
        return None
    return prefix, idx, suffix


def detect_bus_groups(net_names: Iterable[str], *,
                        min_members: int = 2) -> List[BusGroup]:
    """Group nets by prefix-with-trailing-index. Returns groups with
    ≥ min_members. Ordering: descending by member count, then label."""
    buckets: Dict[Tuple[str, str], List[Tuple[int, str]]] = defaultdict(list)
    for name in net_names:
        if not name:
            continue
        sp = _split_index(name)
        if sp is None:
            continue
        prefix, idx, suffix = sp
        buckets[(prefix, suffix)].append((idx, name))

    groups: List[BusGroup] = []
    seen_label: set = set()
    # Sort buckets by descending member count then label for stable output.
    for (prefix, suffix), entries in sorted(
            buckets.items(),
            key=lambda kv: (-len(kv[1]), kv[0])):
        if len(entries) < min_members:
            continue
        entries.sort()  # by idx
        # Single-suffix bus
        members = tuple(name for _i, name in entries)
        label = (prefix + suffix).rstrip('_') or prefix
        if label in seen_label:
            continue
        seen_label.add(label)
        is_diff_bus = bool(suffix) and suffix in ('_P', '_N')
        groups.append(BusGroup(label=label, members=members,
                                 is_diff_pair_bus=is_diff_bus))
    return groups


def assign_groups_for_nets(net_names: Iterable[str], *,
                              min_members: int = 2) -> Dict[str, str]:
    """Return {net_name: bus_label} mapping. Nets not in any bus → not
    in the dict."""
    out: Dict[str, str] = {}
    for g in detect_bus_groups(net_names, min_members=min_members):
        for m in g.members:
            out[m] = g.label
    return out


def summarize_bus_groups(net_names: Iterable[str], *,
                            min_members: int = 2) -> dict:
    """High-level summary for the eval JSON. Returns:
        {
          'groups': [
              {'label': 'DDR4_DQ', 'members': [...], 'count': N,
               'is_diff_pair_bus': bool},
              ...
          ],
          'covered_nets': int,
          'uncovered_nets': [list of net names with no bus],
        }
    """
    names = list(net_names)
    groups = detect_bus_groups(names, min_members=min_members)
    covered: set = set()
    for g in groups:
        covered.update(g.members)
    return {
        'groups': [
            {'label': g.label,
              'members': list(g.members),
              'count': len(g.members),
              'is_diff_pair_bus': g.is_diff_pair_bus}
            for g in groups
        ],
        'covered_nets':   len(covered),
        'uncovered_nets': sorted(n for n in names if n and n not in covered),
    }
