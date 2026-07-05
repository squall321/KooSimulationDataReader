# pdn_runner 단위 테스트 — PG net 선별 / 명령 생성 / dry-run / graceful notes
"""Phase I-3 — PDN IR-drop integration tests."""

from __future__ import annotations

import json
from pathlib import Path

import pytest

from bga_router.integrations.pdn_runner import (
    build_command,
    pick_pg_nets,
    run_pdn_for_net,
    summarize_pdn,
)


def _eval_with_nets(nets):
    return {'metrics': {'paths_mm': {n: [] for n in nets}}}


def test_pick_pg_nets_filters_by_naming():
    e = _eval_with_nets(['GND_1', 'VDD_CORE', 'DDR4_DQ0', 'net042'])
    assert pick_pg_nets(e) == ['GND_1', 'VDD_CORE']


def test_pick_pg_nets_empty_for_anonymous():
    e = _eval_with_nets(['net042', 'net368'])
    assert pick_pg_nets(e) == []


def test_build_command_shape(tmp_path):
    cmd = build_command('GND', em_data_json=tmp_path / 'em.json',
                          output_dir=tmp_path / 'out',
                          layers='COMP', resolution_mm=0.1,
                          source='0,0,3.3', sink='5,5,1.0')
    assert 'run_pdn_dc.py' in cmd[1]
    assert '--net' in cmd and 'GND' in cmd
    assert '--layers' in cmd and 'COMP' in cmd
    assert '--source' in cmd and '0,0,3.3' in cmd


def test_run_pdn_dry_run_no_exec(tmp_path):
    r = run_pdn_for_net('GND', em_data_json=tmp_path / 'em.json',
                          output_dir=tmp_path / 'out', dry_run=True)
    assert r['rc'] is None
    assert r['cmd']
    assert r['skipped'] is False


def test_run_pdn_exec_skips_missing_em_data(tmp_path):
    r = run_pdn_for_net('GND', em_data_json=tmp_path / 'nope.json',
                          output_dir=tmp_path / 'out', dry_run=False)
    assert r['skipped'] is True
    assert 'em_data_json missing' in r['skip_reason']


def test_summarize_no_pg_nets_note():
    out = summarize_pdn(_eval_with_nets(['net042']))
    assert out['pg_net_count'] == 0
    assert 'no power/ground nets' in out['note']


def test_summarize_pg_but_no_em_data_note():
    out = summarize_pdn(_eval_with_nets(['GND_1']))
    assert out['pg_net_count'] == 1
    assert 'em_data_json not provided' in out['note']


def test_summarize_dry_run_lists_commands(tmp_path):
    out = summarize_pdn(_eval_with_nets(['GND_1', 'VDD_A']),
                          em_data_json=tmp_path / 'em.json',
                          output_dir=tmp_path / 'out',
                          dry_run=True)
    assert out['pg_net_count'] == 2
    assert len(out['runs']) == 2
    assert all(r['rc'] is None for r in out['runs'])
    assert out['worst_ir_drop'] is None
