# net 이름 + 메트릭 패턴을 모아 signal domain (DDR/PCIe/USB/PG/GPIO/clock 등) 으로 분류
"""Phase F-5 — net cluster auto-classification.

standards.classify_net 은 high-speed 표준 (DDR/PCIe/USB/...) 만 분류.
이 모듈은 더 넓게: GPIO, clock, low-speed control, power/ground,
analog, JTAG, 그리고 unclassified-but-grouped 까지 net domain을 분류.
재구성 결과는 reporting + 가시화 + recipe 우선순위 결정에 활용 가능.

분류 전략 (우선순위 순).
  1. 명시적 PG 키워드 (path_geometry.is_power_ground_net)
  2. 명시적 high-speed 표준 (standards.classify_net)
  3. 추가 도메인 이름 매치 (clock, jtag, gpio, analog, control)
  4. NetRule 시그너처 휴리스틱 (impedance_target ohm 값 / pair_partner_net)
  5. bus group 기반 fallback (`net042` 같은 익명도 'bus:net' 클러스터)
  6. 그 외 → 'unclassified'
"""

from __future__ import annotations

import re
from collections import defaultdict
from dataclasses import dataclass, field
from typing import Any, Dict, Iterable, List, Optional, Tuple

from .bus_groups import assign_groups_for_nets
from .path_geometry import is_power_ground_net
from .standards import classify_net as classify_standard


# 추가 도메인 패턴 — standards.classify_net에 없는 것들.
_DOMAIN_PATTERNS: List[Tuple[str, re.Pattern]] = [
    ('clock',    re.compile(r'(?i)(^|[_\W])(clk|clock|xtal|osc)([_\W]|$)')),
    ('jtag',     re.compile(r'(?i)(^|[_\W])(tck|tms|tdi|tdo|trst|jtag)([_\W]|$)')),
    ('reset',    re.compile(r'(?i)(^|[_\W])(rst|reset)([_\W]|$)')),
    ('gpio',     re.compile(r'(?i)(^|[_\W])(gpio|io[_\d])')),
    ('analog',   re.compile(r'(?i)(^|[_\W])(adc|dac|aout|ain|analog)([_\W]|$)')),
    ('control',  re.compile(r'(?i)(^|[_\W])(en|enable|cs|sel|ctrl|mode|trig)([_\W]|$)')),
    ('led',      re.compile(r'(?i)(^|[_\W])(led|lamp|indicator)([_\W]|$)')),
]


@dataclass(frozen=True)
class NetCluster:
    label: str
    members: Tuple[str, ...]
    source: str             # 'pg' | 'standard' | 'domain' | 'bus' | 'unclassified'

    def as_dict(self) -> Dict[str, Any]:
        return {
            'label':   self.label,
            'members': list(self.members),
            'count':   len(self.members),
            'source':  self.source,
        }


def _classify_domain(net_name: str) -> Optional[str]:
    """Match against the secondary domain pattern list."""
    if not net_name:
        return None
    for label, pat in _DOMAIN_PATTERNS:
        if pat.search(net_name):
            return label
    return None


def _classify_via_rule(net_name: str, rule) -> Optional[str]:
    """Use NetRule signature when name alone is uninformative."""
    if rule is None:
        return None
    if getattr(rule, 'pair_partner_net', None):
        return 'diff_pair'
    target = getattr(rule, 'impedance_target_ohm', None)
    if target is not None:
        if 40 <= target <= 60:
            return 'single_ended_50ohm'
        if 80 <= target <= 110:
            return 'diff_100ohm_class'
    if getattr(rule, 'shield_required', False):
        return 'shielded'
    return None


def classify_net(net_name: str, rule=None,
                  bus_label: Optional[str] = None) -> Tuple[str, str]:
    """Return (cluster_label, source)."""
    if not net_name:
        return ('unclassified', 'unclassified')
    if is_power_ground_net(net_name):
        return ('power_ground', 'pg')
    std = classify_standard(net_name)
    if std is not None:
        return (std, 'standard')
    dom = _classify_domain(net_name)
    if dom is not None:
        return (dom, 'domain')
    rule_cluster = _classify_via_rule(net_name, rule)
    if rule_cluster is not None:
        return (rule_cluster, 'rule')
    if bus_label:
        return ('bus:' + bus_label, 'bus')
    return ('unclassified', 'unclassified')


def summarize_net_clusters(net_names: Iterable[str], *,
                              rules_by_net: Optional[Dict[str, Any]] = None,
                              bus_min_members: int = 2) -> Dict[str, Any]:
    """Group nets into clusters by domain. Returns:
        {
          'clusters': [NetCluster.as_dict(), ...],
          'cluster_count': int,
          'unclassified': [net names],
        }
    """
    rules_by_net = rules_by_net or {}
    names = [n for n in net_names if n]
    bus_map = assign_groups_for_nets(names, min_members=bus_min_members)
    buckets: Dict[Tuple[str, str], List[str]] = defaultdict(list)
    for name in names:
        rule = rules_by_net.get(name)
        bus_label = bus_map.get(name)
        label, source = classify_net(name, rule, bus_label)
        buckets[(label, source)].append(name)
    clusters: List[NetCluster] = []
    for (label, source), members in sorted(buckets.items(),
                                              key=lambda kv: (-len(kv[1]),
                                                              kv[0][0])):
        clusters.append(NetCluster(label=label,
                                    members=tuple(sorted(members)),
                                    source=source))
    unclassified = [c.members for c in clusters if c.label == 'unclassified']
    flat_unclassified = sorted(
        n for group in unclassified for n in group)
    return {
        'clusters':       [c.as_dict() for c in clusters],
        'cluster_count':  len(clusters),
        'unclassified':   flat_unclassified,
    }
