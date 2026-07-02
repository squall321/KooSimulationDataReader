# driver_resolver 단위 테스트 — pin-ref ↔ net_name lookup
"""Phase G-4 — driver_pin resolver tests."""

from __future__ import annotations

from dataclasses import dataclass, field
from typing import Dict, Optional

import pytest

from bga_router.metrics.driver_resolver import (
    build_pin_lookup,
    parse_pin_ref,
    resolve_driver_map,
    resolve_driver_net,
    summarize_driver_resolution,
)


@dataclass
class _Comp:
    ref_des: str
    pin_to_net: Dict[str, str] = field(default_factory=dict)


@dataclass
class _Rule:
    driver_pin: Optional[str] = None


# ---------------------------------------------------------------------------
# parse_pin_ref
# ---------------------------------------------------------------------------


def test_parse_dot_separator():
    assert parse_pin_ref('U200.A14') == ('U200', 'A14')


def test_parse_colon_separator():
    assert parse_pin_ref('U200:A14') == ('U200', 'A14')


def test_parse_plain_net_name_returns_none():
    assert parse_pin_ref('DDR4_CLK0') is None


def test_parse_empty_returns_none():
    assert parse_pin_ref('') is None
    assert parse_pin_ref(None) is None


# ---------------------------------------------------------------------------
# resolve_driver_net
# ---------------------------------------------------------------------------


def test_resolve_pin_ref_to_net():
    comps = [_Comp('U200', {'A14': 'DDR4_CLK0'})]
    assert resolve_driver_net('U200.A14', comps) == 'DDR4_CLK0'


def test_resolve_unknown_pin_returns_none():
    comps = [_Comp('U200', {'A14': 'DDR4_CLK0'})]
    assert resolve_driver_net('U200.Z9', comps) is None


def test_resolve_plain_net_passes_through():
    comps = [_Comp('U200', {})]
    assert resolve_driver_net('DDR4_CLK0', comps) == 'DDR4_CLK0'


def test_resolve_component_without_pin_map():
    comps = [_Comp('U200')]
    assert resolve_driver_net('U200.A1', comps) is None


# ---------------------------------------------------------------------------
# build_pin_lookup + summarize
# ---------------------------------------------------------------------------


def test_build_pin_lookup_multiple_components():
    comps = [
        _Comp('U200', {'A14': 'NET1', 'B15': 'NET2'}),
        _Comp('U300', {'D1': 'NET3'}),
    ]
    lookup = build_pin_lookup(comps)
    assert lookup[('U200', 'A14')] == 'NET1'
    assert lookup[('U300', 'D1')] == 'NET3'


def test_resolve_driver_map_only_declared():
    comps = [_Comp('U200', {'A14': 'CLK0'})]
    rules = {
        'CLK0':  _Rule(driver_pin='U200.A14'),
        'DQ0':   _Rule(),                        # no driver
        'DQ1':   _Rule(driver_pin='U200.B99'),   # unresolvable
        'CTRL':  _Rule(driver_pin='CTRL_NET'),   # plain net
    }
    m = resolve_driver_map(rules, comps)
    assert m['CLK0'] == 'CLK0'
    assert m['CTRL'] == 'CTRL_NET'
    assert 'DQ0' not in m
    assert 'DQ1' not in m


def test_summarize_driver_resolution_reports_counts():
    comps = [_Comp('U1', {'A': 'X'})]
    rules = {
        'X':  _Rule(driver_pin='U1.A'),
        'Y':  _Rule(driver_pin='U1.MISSING'),
        'Z':  _Rule(),
    }
    s = summarize_driver_resolution(rules, comps)
    assert s['declared_count'] == 2
    assert s['resolved_count'] == 1
    assert s['unresolved_count'] == 1
    assert s['components_scanned'] == 1
