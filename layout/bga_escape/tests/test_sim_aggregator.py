# sim_aggregator 단위 테스트 — Touchstone 파싱 + analytical Z0 correlation
"""Phase H-4 — simulation result aggregator tests."""

from __future__ import annotations

import cmath
import json
import math
from pathlib import Path

import pytest

from bga_router.integrations.sim_aggregator import (
    collect_sim_results,
    insertion_loss_db,
    parse_touchstone_s2p,
    summarize_sim_aggregation,
    z0_from_s11,
)


# ---------------------------------------------------------------------------
# parse_touchstone_s2p
# ---------------------------------------------------------------------------


def _write_s2p(path: Path, header: str, rows):
    lines = ['! Comment', header]
    for f, s11r, s11i, s21r, s21i, s12r, s12i, s22r, s22i in rows:
        lines.append(f'{f} {s11r} {s11i} {s21r} {s21i} '
                      f'{s12r} {s12i} {s22r} {s22i}')
    path.write_text('\n'.join(lines))


def test_parse_ri_2point_file(tmp_path):
    p = tmp_path / 'a.s2p'
    _write_s2p(p, '# Hz S RI R 50',
                 [(1e6, 0.1, 0.0, 0.9, 0.0, 0.9, 0.0, 0.1, 0.0),
                  (1e9, 0.2, 0.0, 0.8, 0.0, 0.8, 0.0, 0.2, 0.0)])
    r = parse_touchstone_s2p(p)
    assert r is not None
    assert r['freqs_hz'] == [1e6, 1e9]
    assert r['z0_ref'] == 50.0
    assert len(r['s11']) == 2


def test_parse_ghz_scale(tmp_path):
    p = tmp_path / 'a.s2p'
    _write_s2p(p, '# GHz S RI R 50',
                 [(1.0, 0.1, 0.0, 0.9, 0.0, 0.9, 0.0, 0.1, 0.0)])
    r = parse_touchstone_s2p(p)
    assert r['freqs_hz'][0] == 1e9


def test_parse_missing_file_returns_none(tmp_path):
    assert parse_touchstone_s2p(tmp_path / 'nope.s2p') is None


def test_parse_ma_format(tmp_path):
    p = tmp_path / 'a.s2p'
    _write_s2p(p, '# Hz S MA R 50',
                 [(1e6, 0.1, 0.0, 0.9, 0.0, 0.9, 0.0, 0.1, 0.0)])
    r = parse_touchstone_s2p(p)
    assert r is not None
    assert abs(r['s11'][0]) == pytest.approx(0.1, abs=1e-9)


# ---------------------------------------------------------------------------
# z0_from_s11 + insertion_loss_db
# ---------------------------------------------------------------------------


def test_z0_from_s11_zero_reflection_returns_ref():
    z = z0_from_s11(complex(0, 0), 50.0)
    assert z == pytest.approx(50.0, abs=1e-6)


def test_z0_from_s11_positive_gamma_returns_higher():
    z = z0_from_s11(complex(0.2, 0), 50.0)
    # (1+0.2)/(1-0.2) = 1.5 → 75Ω
    assert z == pytest.approx(75.0, abs=0.5)


def test_z0_from_s11_returns_none_at_singularity():
    assert z0_from_s11(complex(1, 0), 50.0) is None


def test_insertion_loss_db_zero_when_no_loss():
    loss = insertion_loss_db([complex(1, 0)])
    assert loss == pytest.approx(0.0, abs=1e-6)


def test_insertion_loss_db_empty_returns_none():
    assert insertion_loss_db([]) is None


# ---------------------------------------------------------------------------
# collect_sim_results + summarize
# ---------------------------------------------------------------------------


def _make_sim_tree(tmp_path, task_id='T1', z0_target=50.0):
    d = tmp_path / task_id
    d.mkdir(parents=True)
    # gamma = (Z-Z0)/(Z+Z0), for Z=50/Z0_ref=50 → 0
    p = d / 'result.s2p'
    _write_s2p(p, '# Hz S RI R 50',
                 [(1e6, 0.0, 0.0, 0.9, 0.0, 0.9, 0.0, 0.0, 0.0)])
    return d


def test_collect_sim_results_finds_task(tmp_path):
    _make_sim_tree(tmp_path, task_id='T1')
    out = collect_sim_results(tmp_path, {'T1': 'netA'})
    assert 'netA' in out
    assert out['netA']['simulated_z0_ohm'] == pytest.approx(50.0, abs=0.5)


def test_collect_sim_results_returns_empty_when_no_tree(tmp_path):
    assert collect_sim_results(tmp_path / 'nope', {}) == {}


def test_summarize_correlations_computed(tmp_path):
    # sim/T1 → net='A' with sim Z0 ≈ 50
    _make_sim_tree(tmp_path / 'sim', task_id='T1')
    # tasks dir with T1 → net A
    tasks = tmp_path / 'tasks'; tasks.mkdir()
    (tasks / 'T1.json').write_text(json.dumps({'net': 'A', 'task_id': 'T1'}))
    # analytical A = 55Ω
    eval_data = {'metrics': {'si': {
        'Z0_single_ended_ohm': {'A': 55.0}}}}
    agg = summarize_sim_aggregation(eval_data, tmp_path / 'sim',
                                       tasks_dir=tasks)
    assert agg['net_count'] == 1
    assert agg['correlations']
    entry = agg['correlations'][0]
    assert entry['net'] == 'A'
    assert entry['delta_pct'] < 0     # sim(50) < analytical(55)
