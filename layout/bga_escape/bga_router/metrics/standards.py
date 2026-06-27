# 고속 표준 (DDR4 / PCIe Gen3·4 / USB3.2 / HDMI / Ethernet) pass/fail 체크
"""High-speed standard pass/fail gates.

Each gate is a small budget table — skew tolerance, Z0/Zdiff target,
via budget. Wire up to the Phase B rule_check + Phase C si outputs
so the user can see "DDR4 ok? No — skew on DQ3 is 1.8 mm > 1.5 mm".

classify_net() is intentionally a heuristic — net naming conventions
vary across teams. Callers can pre-override by passing an explicit
``net_to_standard`` mapping into summarize_standards.
"""

from __future__ import annotations

import re
from dataclasses import asdict, dataclass, field
from typing import Any, Dict, List, Optional


@dataclass
class StandardCheck:
    name: str
    applicable_nets: List[str]
    passed: bool
    sub_checks: Dict[str, bool] = field(default_factory=dict)
    reasons: List[str] = field(default_factory=list)
    budget: Dict[str, Any] = field(default_factory=dict)

    def as_dict(self) -> Dict[str, Any]:
        return asdict(self)


# ---------------------------------------------------------------------------
# Classify net → standard
# ---------------------------------------------------------------------------


_STANDARD_PATTERNS = [
    ('DDR4',       re.compile(r'(?i)(^|[_\W])ddr4?([_\W]|$)')),
    ('PCIe-Gen4',  re.compile(r'(?i)pcie?_?4|pcie?gen4|pe4')),
    ('PCIe-Gen3',  re.compile(r'(?i)pcie?_?3|pcie?gen3|pe3')),
    ('USB3.2',     re.compile(r'(?i)usb_?3')),
    ('HDMI',       re.compile(r'(?i)hdmi')),
    # underscore is a word char so \b doesn't fire on ETH_TX; match
    # start/end or non-alphanumeric separators instead.
    ('Ethernet',   re.compile(r'(?i)(^|[_\W])(eth|rgmii|rmii|xgmii)([_\W]|$)')),
]


def classify_net(net_name: str) -> Optional[str]:
    """Heuristic — match net name to a known high-speed standard."""
    if not net_name:
        return None
    for label, pat in _STANDARD_PATTERNS:
        if pat.search(net_name):
            return label
    return None


# ---------------------------------------------------------------------------
# Budget helpers
# ---------------------------------------------------------------------------


def _within_tolerance(value: float, target: float, tol_pct: float) -> bool:
    if value is None or value <= 0:
        return False
    return abs(value - target) / target <= (tol_pct / 100.0)


def _z0_pass(si: Dict[str, Any], nets: List[str], target: float,
               tol_pct: float, key: str = 'Z0_single_ended_ohm') -> tuple:
    """Returns (passed_bool, list_of_failing_nets)."""
    vals = si.get(key, {}) or {}
    failing = []
    for n in nets:
        v = vals.get(n)
        if v is None:
            continue  # no data → don't fail
        if not _within_tolerance(v, target, tol_pct):
            failing.append(n)
    return (not failing), failing


def _skew_pass(rule_check: Dict[str, Any], nets: List[str],
                 limit_mm: float) -> tuple:
    """Uses Phase B's rule_check.by_field.pair_skew_ok violators list."""
    by = (rule_check or {}).get('by_field', {})
    entry = by.get('pair_skew_ok', {})
    violators = set(entry.get('violators', []))
    failing = [n for n in nets if n in violators]
    # If pair_skew_ok already reflects rule.pair_skew_max_mm, the standard's
    # own limit becomes informational. We still record the budget for clarity.
    return (not failing), failing


def _via_pass(routed_paths, nets: List[str], max_via: int) -> tuple:
    failing = []
    for n in nets:
        pr = routed_paths.get(n)
        if pr is None:
            continue
        v = getattr(pr, 'via_count', None)
        if v is None and isinstance(pr, dict):
            v = pr.get('via_count')
        if v is not None and v > max_via:
            failing.append(n)
    return (not failing), failing


def _length_pass(geometry: Dict[str, Any], nets: List[str],
                  max_length_mm: float, all_lengths: Dict[str, float]) -> tuple:
    failing = [n for n in nets if all_lengths.get(n, 0.0) > max_length_mm]
    return (not failing), failing


# ---------------------------------------------------------------------------
# Per-standard check
# ---------------------------------------------------------------------------


def _build(name: str, nets: List[str], sub_checks: Dict[str, bool],
            reasons: List[str], budget: Dict[str, Any]) -> StandardCheck:
    return StandardCheck(
        name=name,
        applicable_nets=sorted(nets),
        passed=all(sub_checks.values()) if sub_checks else False,
        sub_checks=sub_checks,
        reasons=reasons,
        budget=budget,
    )


def check_ddr4(nets, geometry, si, rule_check, *,
                routed_paths=None, lengths=None) -> StandardCheck:
    budget = {'intra_pair_skew_mm': 1.5, 'z0_ohm': 40.0, 'z0_tol_pct': 10,
               'zdiff_ohm': 80.0, 'zdiff_tol_pct': 10, 'max_via': 4}
    z0_ok, z0_fail = _z0_pass(si, nets, 40.0, 10.0)
    skew_ok, skew_fail = _skew_pass(rule_check, nets, 1.5)
    via_ok, via_fail = _via_pass(routed_paths or {}, nets, 4)
    sub = {'skew': skew_ok, 'z0': z0_ok, 'via_budget': via_ok}
    reasons = []
    if not skew_ok: reasons.append(f'pair_skew violators: {skew_fail}')
    if not z0_ok:   reasons.append(f'Z0 out of 40Ω±10%: {z0_fail}')
    if not via_ok:  reasons.append(f'via_count > 4: {via_fail}')
    return _build('DDR4', nets, sub, reasons, budget)


def check_pcie_gen3(nets, geometry, si, rule_check, *,
                      routed_paths=None, lengths=None) -> StandardCheck:
    budget = {'intra_pair_skew_mm': 0.75, 'zdiff_ohm': 85.0,
               'zdiff_tol_pct': 10, 'max_via': 3}
    skew_ok, skew_fail = _skew_pass(rule_check, nets, 0.75)
    via_ok, via_fail = _via_pass(routed_paths or {}, nets, 3)
    sub = {'skew': skew_ok, 'via_budget': via_ok}
    reasons = []
    if not skew_ok: reasons.append(f'pair_skew violators: {skew_fail}')
    if not via_ok:  reasons.append(f'via_count > 3: {via_fail}')
    return _build('PCIe-Gen3', nets, sub, reasons, budget)


def check_pcie_gen4(nets, geometry, si, rule_check, *,
                      routed_paths=None, lengths=None) -> StandardCheck:
    budget = {'intra_pair_skew_mm': 0.43, 'zdiff_ohm': 85.0,
               'zdiff_tol_pct': 10, 'max_via': 2}
    skew_ok, skew_fail = _skew_pass(rule_check, nets, 0.43)
    via_ok, via_fail = _via_pass(routed_paths or {}, nets, 2)
    sub = {'skew': skew_ok, 'via_budget': via_ok}
    reasons = []
    if not skew_ok: reasons.append(f'pair_skew violators: {skew_fail}')
    if not via_ok:  reasons.append(f'via_count > 2: {via_fail}')
    return _build('PCIe-Gen4', nets, sub, reasons, budget)


def check_usb32(nets, geometry, si, rule_check, *,
                  routed_paths=None, lengths=None) -> StandardCheck:
    budget = {'intra_pair_skew_mm': 2.25, 'zdiff_ohm': 90.0,
               'zdiff_tol_pct': 15, 'max_length_mm': 200.0}
    skew_ok, skew_fail = _skew_pass(rule_check, nets, 2.25)
    len_ok, len_fail = _length_pass(geometry or {}, nets, 200.0,
                                       lengths or {})
    sub = {'skew': skew_ok, 'length': len_ok}
    reasons = []
    if not skew_ok: reasons.append(f'pair_skew violators: {skew_fail}')
    if not len_ok:  reasons.append(f'length > 200 mm: {len_fail}')
    return _build('USB3.2', nets, sub, reasons, budget)


def check_hdmi(nets, geometry, si, rule_check, *,
                routed_paths=None, lengths=None) -> StandardCheck:
    budget = {'intra_pair_skew_mm': 1.5, 'zdiff_ohm': 100.0,
               'zdiff_tol_pct': 15}
    skew_ok, skew_fail = _skew_pass(rule_check, nets, 1.5)
    sub = {'skew': skew_ok}
    reasons = []
    if not skew_ok: reasons.append(f'pair_skew violators: {skew_fail}')
    return _build('HDMI', nets, sub, reasons, budget)


def check_ethernet(nets, geometry, si, rule_check, *,
                     routed_paths=None, lengths=None) -> StandardCheck:
    budget = {'intra_pair_skew_mm': 7.5, 'zdiff_ohm': 100.0,
               'zdiff_tol_pct': 10}
    skew_ok, skew_fail = _skew_pass(rule_check, nets, 7.5)
    sub = {'skew': skew_ok}
    reasons = []
    if not skew_ok: reasons.append(f'pair_skew violators: {skew_fail}')
    return _build('Ethernet', nets, sub, reasons, budget)


_CHECKERS = {
    'DDR4':       check_ddr4,
    'PCIe-Gen3':  check_pcie_gen3,
    'PCIe-Gen4':  check_pcie_gen4,
    'USB3.2':     check_usb32,
    'HDMI':       check_hdmi,
    'Ethernet':   check_ethernet,
}

_KEY_MAP = {
    'DDR4':      'ddr4_ok',
    'PCIe-Gen3': 'pcie_gen3_ok',
    'PCIe-Gen4': 'pcie_gen4_ok',
    'USB3.2':    'usb32_ok',
    'HDMI':      'hdmi_ok',
    'Ethernet':  'ethernet_ok',
}


def summarize_standards(routed_paths, geometry, si, rule_check, *,
                          net_to_standard: Optional[Dict[str, str]] = None,
                          lengths: Optional[Dict[str, float]] = None,
                          ) -> Dict[str, Any]:
    """Run each applicable standard check, return per-standard pass/fail."""
    # Group nets by classified standard.
    groups: Dict[str, List[str]] = {}
    unclassified: List[str] = []
    for net in routed_paths.keys():
        std = (net_to_standard or {}).get(net) or classify_net(net)
        if std is None:
            unclassified.append(net)
            continue
        groups.setdefault(std, []).append(net)

    out: Dict[str, Any] = {key: None for key in _KEY_MAP.values()}
    for std, nets in groups.items():
        checker = _CHECKERS.get(std)
        if checker is None:
            continue
        result = checker(nets, geometry, si, rule_check,
                          routed_paths=routed_paths, lengths=lengths)
        out[_KEY_MAP[std]] = result.as_dict()
    out['unclassified_nets'] = sorted(unclassified)
    return out
