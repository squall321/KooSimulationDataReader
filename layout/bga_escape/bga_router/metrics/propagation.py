# net별 전파 지연(ps) + diff-pair / bus 그룹 timing skew 메트릭
"""Phase I-5 — propagation delay metrics (ps).

기존 PropagationDelayMatchPostProcessor는 mutate 시에만 skew를
로깅하고 metrics로 노출되지 않았다. 이 모듈은 그 검증된
`_path_delay_ps` 헬퍼를 재사용해 per-net 전파 지연을 계산하고
eval JSON의 si.propagation_delay_ps로 노출한다.

핵심: 길이(mm)가 아니라 전기적 지연(ps)이 타이밍의 진짜 척도.
서로 다른 레이어(εr 다름)를 지나는 net은 길이가 같아도 지연이 다르다.
"""

from __future__ import annotations

import math
from typing import Any, Dict, List, Optional

from src.skills.post_processors.propagation_delay_match import (
    _path_delay_ps,
)


def _stackup_to_pairs(stackup):
    """StackupSpec → _path_delay_ps가 기대하는 [(name, 'signal'|'plane'), ...]
    리스트로 변환. copper 레이어는 signal, prepreg/core 등은 plane 취급
    (microstrip/stripline 판정을 위해 인접 유전체를 plane으로 본다).
    None / 변환 실패 시 None (fallback εr 사용)."""
    if stackup is None:
        return None
    layers = getattr(stackup, 'layers', None)
    if not layers:
        return None
    pairs = []
    for L in layers:
        kind = getattr(L, 'kind', None)
        name = getattr(L, 'name', None)
        if name is None:
            continue
        pairs.append((name, 'signal' if kind == 'copper' else 'plane'))
    return pairs or None


def net_delay_ps(path, grid, stackup=None) -> float:
    """단일 net의 전파 지연(ps). 검증된 헬퍼 위임.
    stackup은 StackupSpec 또는 이미 변환된 리스트 둘 다 허용."""
    su = (stackup if isinstance(stackup, list)
           else _stackup_to_pairs(stackup))
    return _path_delay_ps(path, grid, su)


def _pair_partner(rule) -> Optional[str]:
    return getattr(rule, 'pair_partner_net', None) if rule else None


def summarize_propagation(routed_paths, grid, rules_by_net: Dict[str, Any],
                            stackup=None) -> Dict[str, Any]:
    """per-net 지연 + diff-pair intra-skew + bus 그룹 skew.

    반환:
      {
        'delay_ps':        {net: ps},
        'diff_pair_skew_ps': {pair_label: ps},   # |delay(P) - delay(N)|
        'bus_skew_ps':     {bus_label: {max, min, skew}},
        'worst_pair_skew': {pair, skew_ps} | None,
      }
    """
    def _path(pr):
        return getattr(pr, 'path', None) if not isinstance(pr, list) else pr

    # stackup을 한 번만 변환해 재사용
    su = stackup if isinstance(stackup, list) else _stackup_to_pairs(stackup)

    delay: Dict[str, float] = {}
    for net, pr in routed_paths.items():
        p = _path(pr)
        if p:
            delay[net] = round(net_delay_ps(p, grid, su), 3)

    # diff-pair intra-skew (ps)
    pair_skew: Dict[str, float] = {}
    seen: set = set()
    for net, rule in rules_by_net.items():
        partner = _pair_partner(rule)
        if not partner or partner not in delay or net not in delay:
            continue
        label = '|'.join(sorted([net, partner]))
        if label in seen:
            continue
        seen.add(label)
        pair_skew[label] = round(abs(delay[net] - delay[partner]), 3)

    # bus group skew via naming (Phase F-2 재사용)
    bus_skew: Dict[str, Any] = {}
    try:
        from bga_router.metrics.bus_groups import assign_groups_for_nets
        bus_map = assign_groups_for_nets(list(delay), min_members=2)
        by_bus: Dict[str, List[float]] = {}
        for net, bus in bus_map.items():
            by_bus.setdefault(bus, []).append(delay[net])
        for bus, ds in by_bus.items():
            if len(ds) >= 2:
                bus_skew[bus] = {
                    'max_ps':  round(max(ds), 3),
                    'min_ps':  round(min(ds), 3),
                    'skew_ps': round(max(ds) - min(ds), 3),
                    'member_count': len(ds),
                }
    except Exception:
        pass

    worst = None
    for label, sk in pair_skew.items():
        if worst is None or sk > worst['skew_ps']:
            worst = {'pair': label, 'skew_ps': sk}

    return {
        'delay_ps':          delay,
        'diff_pair_skew_ps': pair_skew,
        'bus_skew_ps':       bus_skew,
        'worst_pair_skew':   worst,
        'tpd_note':          ('tpd = sqrt(εr_eff)/c0 ≈ 5.6 ps/mm @εr=4.3; '
                               'via는 지연 0 근사'),
    }
