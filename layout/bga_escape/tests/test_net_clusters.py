# net cluster 자동 분류 단위 테스트 — PG / standard / domain / rule / bus / fallback
"""Phase F-5 — net cluster classification tests."""

from __future__ import annotations

from dataclasses import dataclass
from typing import Optional

import pytest

from bga_router.metrics.net_clusters import (
    classify_net,
    summarize_net_clusters,
)


@dataclass
class _Rule:
    pair_partner_net: Optional[str] = None
    impedance_target_ohm: Optional[float] = None
    shield_required: bool = False


# ---------------------------------------------------------------------------
# classify_net
# ---------------------------------------------------------------------------


def test_pg_wins_over_anything():
    label, source = classify_net('GND_1')
    assert (label, source) == ('power_ground', 'pg')


def test_standard_wins_over_domain():
    label, source = classify_net('DDR4_DQ0_P')
    assert (label, source) == ('DDR4', 'standard')


def test_clock_domain():
    label, source = classify_net('CLK_25M')
    assert (label, source) == ('clock', 'domain')


def test_jtag_domain():
    label, source = classify_net('JTAG_TCK')
    assert (label, source) == ('jtag', 'domain')


def test_reset_domain():
    label, source = classify_net('RESET_N')
    assert (label, source) == ('reset', 'domain')


def test_led_domain():
    label, source = classify_net('LED_STATUS')
    assert (label, source) == ('led', 'domain')


def test_gpio_domain():
    label, source = classify_net('GPIO0')
    assert (label, source) == ('gpio', 'domain')


def test_rule_classifies_diff_pair():
    rule = _Rule(pair_partner_net='N')
    label, source = classify_net('SIG_P', rule=rule)
    assert (label, source) == ('diff_pair', 'rule')


def test_rule_classifies_z0_target():
    rule = _Rule(impedance_target_ohm=50.0)
    label, source = classify_net('SIG_42', rule=rule)
    assert (label, source) == ('single_ended_50ohm', 'rule')


def test_bus_fallback_when_no_other_match():
    label, source = classify_net('mysig_5', bus_label='mysig')
    assert (label, source) == ('bus:mysig', 'bus')


def test_unclassified_when_nothing_matches():
    label, source = classify_net('SOLITARY_PIN')
    assert (label, source) == ('unclassified', 'unclassified')


# ---------------------------------------------------------------------------
# summarize_net_clusters
# ---------------------------------------------------------------------------


def test_summarize_mixed_pool():
    nets = [
        'DDR4_DQ0', 'DDR4_DQ1',
        'GND_1', 'GND_2',
        'CLK_25M',
        'PCIE3_RX0_P', 'PCIE3_RX0_N',
        'LED_STATUS',
        'mysig5', 'mysig6',         # bus fallback
    ]
    s = summarize_net_clusters(nets)
    labels = {c['label']: c for c in s['clusters']}
    assert 'DDR4' in labels
    assert labels['DDR4']['count'] == 2
    assert 'power_ground' in labels
    assert 'PCIe-Gen3' in labels
    assert 'clock' in labels
    assert 'led' in labels
    # bus fallback
    bus_labels = [k for k in labels if k.startswith('bus:')]
    assert any('mysig' in b for b in bus_labels)


def test_summarize_p3_like_anonymous_nets():
    """P3 익명 net042/net368/... → all bus:net cluster."""
    nets = ['net042', 'net368', 'net369']
    s = summarize_net_clusters(nets)
    labels = {c['label'] for c in s['clusters']}
    assert 'bus:net' in labels
    assert s['unclassified'] == []


def test_summarize_empty():
    s = summarize_net_clusters([])
    assert s['cluster_count'] == 0
    assert s['clusters'] == []
    assert s['unclassified'] == []
