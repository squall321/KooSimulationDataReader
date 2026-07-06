# Phase I-4 — 실제 sol_d field-solve 결과로 analytical↔simulated correlation 회귀 검증
"""Phase I-4 real end-to-end verification.

sol_d_cross_section을 P3 net114에 실제로 돌린 산출물
(tests/fixtures/sol_d_net114/{net114.s2p, summary.json}) 을 고정해
sim-agg의 correlation 계산이 진짜 solver 값을 쓰는지 검증한다.

실측 (2026-07-06):
  - sol_d 2D field-solve Z0_avg = 58.48 Ω (num_sections=10, res=0.01mm)
  - near-DC S11 역산 Z0 = 50.05 Ω (electrically-short 라인 한계)
  - analytical Wadell (default 4-layer FR-4, w=0.075) ≈ 50.68 Ω

→ analytical vs field-solve delta ≈ +15% : H-2에서 예측한 stripline
  systematic bias의 실제 확인. sim-agg는 solver_summary Z0를 우선해야
  이 correlation이 의미를 갖는다 (S11 역산만 쓰면 50 vs 50.68 = ~1%로
  bias가 가려짐).
"""

from __future__ import annotations

import json
from pathlib import Path

import pytest

from bga_router.integrations.sim_aggregator import (
    collect_sim_results,
    summarize_sim_aggregation,
)


FIXTURE = Path(__file__).parent / 'fixtures' / 'sol_d_net114'


def _sim_tree(tmp_path):
    """Copy the real sol_d artifacts into a task-id dir layout."""
    task_dir = tmp_path / 'T_net114'
    task_dir.mkdir(parents=True)
    (task_dir / 'net114.s2p').write_text(
        (FIXTURE / 'net114.s2p').read_text())
    (task_dir / 'summary.json').write_text(
        (FIXTURE / 'summary.json').read_text())
    return tmp_path


def test_fixture_present():
    assert (FIXTURE / 'net114.s2p').exists()
    assert (FIXTURE / 'summary.json').exists()


def test_collect_uses_field_solve_z0(tmp_path):
    root = _sim_tree(tmp_path)
    out = collect_sim_results(root, {'T_net114': 'net114'})
    assert 'net114' in out
    r = out['net114']
    # solver summary Z0_avg = 58.48, not the S11-inverted ~50
    assert r['z0_source'] == 'solver_summary'
    assert r['simulated_z0_ohm'] == pytest.approx(58.48, abs=0.1)
    # S11 inversion preserved alongside for transparency
    assert r['z0_s11_ohm'] == pytest.approx(50.05, abs=1.0)
    assert r['freq_points'] == 201


def test_correlation_reveals_stripline_bias(tmp_path):
    root = _sim_tree(tmp_path)
    tasks = tmp_path / 'tasks'; tasks.mkdir()
    (tasks / 'T_net114.json').write_text(
        json.dumps({'net': 'net114', 'task_id': 'T_net114'}))
    # analytical Wadell value for this net
    eval_data = {'metrics': {'si': {
        'Z0_single_ended_ohm': {'net114': 50.68}}}}
    agg = summarize_sim_aggregation(eval_data, root, tasks_dir=tasks)
    assert agg['net_count'] == 1
    corr = agg['correlations'][0]
    assert corr['net'] == 'net114'
    assert corr['analytical_z0_ohm'] == pytest.approx(50.68)
    assert corr['simulated_z0_ohm'] == pytest.approx(58.48, abs=0.1)
    # The gap the analytical formula could NOT see: ~15%.
    assert corr['delta_pct'] > 10.0
