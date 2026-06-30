# bus_groups 자동 탐지 단위 테스트 — 이름 패턴 기반 그룹화
"""Phase F-2 — bus auto-detection tests."""

from __future__ import annotations

import pytest

from bga_router.metrics.bus_groups import (
    BusGroup,
    assign_groups_for_nets,
    detect_bus_groups,
    summarize_bus_groups,
)


def test_detect_ddr_byte():
    nets = [f'DDR4_DQ{i}' for i in range(8)]
    groups = detect_bus_groups(nets)
    assert len(groups) == 1
    g = groups[0]
    assert g.label == 'DDR4_DQ'
    assert len(g.members) == 8
    assert g.is_diff_pair_bus is False


def test_detect_diff_pair_bus():
    nets = ['PCIE3_RX0_P', 'PCIE3_RX1_P', 'PCIE3_RX2_P', 'PCIE3_RX3_P',
            'PCIE3_RX0_N', 'PCIE3_RX1_N', 'PCIE3_RX2_N', 'PCIE3_RX3_N']
    groups = detect_bus_groups(nets)
    # Two diff-pair buses: ..._P (4 nets) and ..._N (4 nets)
    labels = {g.label for g in groups}
    assert 'PCIE3_RX_P' in labels
    assert 'PCIE3_RX_N' in labels
    for g in groups:
        assert g.is_diff_pair_bus is True
        assert len(g.members) == 4


def test_min_members_filter():
    nets = ['SOLO', 'DDR_DQ0', 'DDR_DQ1']
    groups = detect_bus_groups(nets, min_members=3)
    assert groups == []
    groups2 = detect_bus_groups(nets, min_members=2)
    assert len(groups2) == 1


def test_unindexed_nets_uncovered():
    nets = ['CLK_25M', 'RESET_N', 'LED_STATUS']
    groups = detect_bus_groups(nets)
    assert groups == []


def test_assign_groups_returns_mapping():
    nets = ['DDR_DQ0', 'DDR_DQ1', 'DDR_DQ2', 'CLK_25M']
    mapping = assign_groups_for_nets(nets)
    assert mapping['DDR_DQ0'] == 'DDR_DQ'
    assert 'CLK_25M' not in mapping


def test_summarize_covered_and_uncovered():
    nets = ['DDR_DQ0', 'DDR_DQ1', 'CLK_25M']
    s = summarize_bus_groups(nets)
    assert s['covered_nets'] == 2
    assert s['uncovered_nets'] == ['CLK_25M']
    assert len(s['groups']) == 1
    assert s['groups'][0]['count'] == 2


def test_multiple_buses_sorted_by_count():
    nets = ['A0', 'A1', 'B0', 'B1', 'B2', 'B3']
    groups = detect_bus_groups(nets)
    assert len(groups) == 2
    assert groups[0].label == 'B'      # 4 members first
    assert groups[1].label == 'A'


def test_empty_input():
    assert detect_bus_groups([]) == []
    assert assign_groups_for_nets([]) == {}
    s = summarize_bus_groups([])
    assert s['covered_nets'] == 0
    assert s['groups'] == []


def test_p3_anon_nets_form_one_big_bus():
    """P3 dataset uses net042/net368/... — they ALL share prefix 'net'."""
    nets = [f'net{n}' for n in [42, 368, 369, 373, 476]]
    groups = detect_bus_groups(nets)
    assert len(groups) == 1
    assert groups[0].label == 'net'
    assert len(groups[0].members) == 5
