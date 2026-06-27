# 고속 표준 pass/fail (DDR4/PCIe/USB/HDMI/Ethernet) 단위 테스트
"""Phase C1 — standards check tests."""

from __future__ import annotations

from dataclasses import dataclass
from typing import Optional

import pytest

from bga_router.metrics.standards import (
    check_ddr4,
    check_ethernet,
    check_hdmi,
    check_pcie_gen3,
    check_pcie_gen4,
    check_usb32,
    classify_net,
    summarize_standards,
)


@dataclass
class _PR:
    via_count: int = 0


def test_classify_net_known_patterns():
    assert classify_net('DDR4_DQ0_P') == 'DDR4'
    assert classify_net('PCIE3_RX0_P') == 'PCIe-Gen3'
    assert classify_net('PCIE4_RX0_P') == 'PCIe-Gen4'
    assert classify_net('USB3_TX_P') == 'USB3.2'
    assert classify_net('HDMI_D0_P') == 'HDMI'
    assert classify_net('ETH_TX_P') == 'Ethernet'
    assert classify_net('RGMII_TXD0') == 'Ethernet'


def test_classify_net_unknown_returns_none():
    assert classify_net('LED_STATUS') is None
    assert classify_net('LED_TX') is None       # not 'eth' standalone
    assert classify_net('') is None
    assert classify_net('VCC_CORE') is None


def test_classify_net_gen4_takes_precedence_over_gen3():
    # PCIE4 should NOT match PCIe-Gen3
    assert classify_net('PCIE4_RX0') == 'PCIe-Gen4'


# ---------------------------------------------------------------------------
# DDR4 — pass / fail cases
# ---------------------------------------------------------------------------


def test_check_ddr4_all_pass():
    nets = ['DDR4_DQ0_P', 'DDR4_DQ0_N']
    geometry = {}
    si = {'Z0_single_ended_ohm': {'DDR4_DQ0_P': 40.0, 'DDR4_DQ0_N': 40.0}}
    rule_check = {'by_field': {'pair_skew_ok': {'violators': []}}}
    routed = {'DDR4_DQ0_P': _PR(via_count=2), 'DDR4_DQ0_N': _PR(via_count=2)}
    r = check_ddr4(nets, geometry, si, rule_check, routed_paths=routed)
    assert r.passed is True
    assert r.reasons == []
    assert set(r.sub_checks) == {'skew', 'z0', 'via_budget'}


def test_check_ddr4_skew_fail():
    nets = ['DDR4_DQ0_P', 'DDR4_DQ0_N']
    si = {'Z0_single_ended_ohm': {'DDR4_DQ0_P': 40.0, 'DDR4_DQ0_N': 40.0}}
    rule_check = {'by_field': {'pair_skew_ok': {'violators': ['DDR4_DQ0_P']}}}
    routed = {'DDR4_DQ0_P': _PR(via_count=2), 'DDR4_DQ0_N': _PR(via_count=2)}
    r = check_ddr4(nets, {}, si, rule_check, routed_paths=routed)
    assert r.passed is False
    assert any('skew' in s.lower() for s in r.reasons)


def test_check_ddr4_z0_out_of_band():
    nets = ['DDR4_DQ0_P']
    si = {'Z0_single_ended_ohm': {'DDR4_DQ0_P': 80.0}}  # way off 40 Ω
    rule_check = {'by_field': {'pair_skew_ok': {'violators': []}}}
    r = check_ddr4(nets, {}, si, rule_check, routed_paths={})
    assert r.sub_checks['z0'] is False
    assert any('Z0' in s for s in r.reasons)


def test_check_ddr4_via_fail():
    nets = ['DDR4_DQ0_P']
    routed = {'DDR4_DQ0_P': _PR(via_count=10)}
    rule_check = {'by_field': {'pair_skew_ok': {'violators': []}}}
    r = check_ddr4(nets, {}, {'Z0_single_ended_ohm': {'DDR4_DQ0_P': 40}},
                    rule_check, routed_paths=routed)
    assert r.sub_checks['via_budget'] is False


# ---------------------------------------------------------------------------
# PCIe Gen3 vs Gen4 strictness
# ---------------------------------------------------------------------------


def test_pcie_gen4_stricter_via():
    nets = ['PCIE_RX0_P']
    routed = {'PCIE_RX0_P': _PR(via_count=3)}
    rc = {'by_field': {'pair_skew_ok': {'violators': []}}}
    g3 = check_pcie_gen3(nets, {}, {}, rc, routed_paths=routed)
    g4 = check_pcie_gen4(nets, {}, {}, rc, routed_paths=routed)
    assert g3.sub_checks['via_budget'] is True   # ≤3
    assert g4.sub_checks['via_budget'] is False  # ≤2


# ---------------------------------------------------------------------------
# USB / HDMI / Ethernet
# ---------------------------------------------------------------------------


def test_usb32_length_fail():
    nets = ['USB3_RX_P']
    lengths = {'USB3_RX_P': 250.0}
    rc = {'by_field': {'pair_skew_ok': {'violators': []}}}
    r = check_usb32(nets, {}, {}, rc, routed_paths={}, lengths=lengths)
    assert r.sub_checks['length'] is False


def test_hdmi_skew_pass_with_no_violators():
    nets = ['HDMI_D0_P', 'HDMI_D0_N']
    rc = {'by_field': {'pair_skew_ok': {'violators': []}}}
    r = check_hdmi(nets, {}, {}, rc)
    assert r.passed is True


def test_ethernet_skew_violators():
    nets = ['ETH_TXD0_P', 'ETH_TXD0_N']
    rc = {'by_field': {'pair_skew_ok': {'violators': ['ETH_TXD0_P']}}}
    r = check_ethernet(nets, {}, {}, rc)
    assert r.passed is False


# ---------------------------------------------------------------------------
# summarize_standards
# ---------------------------------------------------------------------------


def test_summarize_standards_routes_to_correct_checkers():
    routed = {
        'DDR4_DQ0_P': _PR(via_count=2),
        'DDR4_DQ0_N': _PR(via_count=2),
        'PCIE3_RX0_P': _PR(via_count=2),
        'PCIE3_RX0_N': _PR(via_count=2),
        'LED_STATUS': _PR(via_count=0),
    }
    si = {'Z0_single_ended_ohm': {'DDR4_DQ0_P': 40.0, 'DDR4_DQ0_N': 40.0}}
    rc = {'by_field': {'pair_skew_ok': {'violators': []}}}
    out = summarize_standards(routed, {}, si, rc)
    assert out['ddr4_ok'] is not None
    assert out['pcie_gen3_ok'] is not None
    assert out['pcie_gen4_ok'] is None
    assert out['usb32_ok'] is None
    assert 'LED_STATUS' in out['unclassified_nets']


def test_summarize_standards_empty_input():
    out = summarize_standards({}, {}, {}, {})
    assert out['ddr4_ok'] is None
    assert out['unclassified_nets'] == []


def test_summarize_standards_explicit_mapping_overrides_classify():
    routed = {'MY_BUS_0': _PR(via_count=2)}
    rc = {'by_field': {'pair_skew_ok': {'violators': []}}}
    out = summarize_standards(routed, {}, {}, rc,
                                net_to_standard={'MY_BUS_0': 'DDR4'})
    assert out['ddr4_ok'] is not None
    assert out['unclassified_nets'] == []
