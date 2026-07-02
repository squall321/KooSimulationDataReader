# NetRule.driver_pin ('U200.A14') → 실제 net_name 을 EDA pin_to_net에서 lookup
"""Phase G-4 — driver_pin ↔ net_name resolver.

F-3 dependency graph는 driver_pin을 hint string으로만 저장.
이번엔 EdaData.components[*].pin_to_net 을 스캔해 실제 net_name을
lookup하고, dependency_graph를 driver 명시된 그룹으로 다시 빌드.

Format:
  'U200.A14'         → components에서 ref_des=U200 찾고, pin_to_net['A14'] 반환
  'U200:A14'         → 콜론 구분자도 허용
  net_name 직접       → 이미 net이면 그대로 반환 (fall-through)
"""

from __future__ import annotations

import re
from typing import Any, Dict, Iterable, Optional, Tuple


_PIN_REF_RE = re.compile(r'^([A-Za-z0-9_+]+)[.:]([A-Za-z0-9_]+)$')


def parse_pin_ref(ref: str) -> Optional[Tuple[str, str]]:
    """Parse 'U200.A14' → ('U200', 'A14'). Returns None if not pin-ref shape."""
    if not ref:
        return None
    m = _PIN_REF_RE.match(ref)
    if m:
        return m.group(1), m.group(2)
    return None


def build_pin_lookup(components: Iterable[Any]) -> Dict[Tuple[str, str], str]:
    """{(ref_des, pin_name): net_name} across all components."""
    out: Dict[Tuple[str, str], str] = {}
    for c in components:
        rd = getattr(c, 'ref_des', None)
        p2n = getattr(c, 'pin_to_net', None)
        if not rd or not p2n:
            continue
        for pin, net in p2n.items():
            out[(rd, pin)] = net
    return out


def resolve_driver_net(driver_ref: str,
                          components: Iterable[Any]) -> Optional[str]:
    """Resolve `driver_pin` string to a net_name via components' pin_to_net.

    - 'U200.A14' pin-ref → lookup
    - Anything else → returned as-is (already a net_name)
    Returns None only when the pin-ref does not resolve.
    """
    if not driver_ref:
        return None
    parts = parse_pin_ref(driver_ref)
    if parts is None:
        return driver_ref            # assume already a net_name
    lookup = build_pin_lookup(components)
    return lookup.get(parts)


def resolve_driver_map(rules_by_net: Dict[str, Any],
                          components: Iterable[Any]) -> Dict[str, str]:
    """{net_name: resolved_driver_net} for every rule that declares a
    driver_pin. Unresolved entries omitted."""
    lookup = build_pin_lookup(components)
    out: Dict[str, str] = {}
    for net, rule in rules_by_net.items():
        drv = getattr(rule, 'driver_pin', None)
        if not drv:
            continue
        parts = parse_pin_ref(drv)
        if parts is None:
            out[net] = drv
            continue
        resolved = lookup.get(parts)
        if resolved:
            out[net] = resolved
    return out


def summarize_driver_resolution(rules_by_net: Dict[str, Any],
                                   components: Iterable[Any]) -> Dict[str, Any]:
    """Report resolution status for eval JSON."""
    resolved_map = resolve_driver_map(rules_by_net, components)
    declared = [net for net, r in rules_by_net.items()
                 if getattr(r, 'driver_pin', None)]
    return {
        'declared_count':      len(declared),
        'resolved_count':      len(resolved_map),
        'unresolved_count':    len(declared) - len(resolved_map),
        'resolved':            resolved_map,
        'components_scanned':  len(list(components)),
    }
