# sol_b_runner 단위 테스트 — em-dispatch task → sol_b run.py 명령
"""Phase G-1 — sol_b PEEC runner tests."""

from __future__ import annotations

import json
from pathlib import Path

import pytest

from bga_router.integrations.sol_b_runner import build_command, dispatch_run


def _task(net='DDR4_DQ0_P', solver='sol_b',
            task_id='T_DS__bga__r__DDR4_DQ0_P'):
    return {
        'net':     net,
        'task_id': task_id,
        'layer':   'LAY2',
        'layers':  ['LAY2'],
        'bbox_mm': [0, 0, 5, 1],
        'rule':    {'width_mm': 0.075},
        'reasons': ['tight_coupling'],
        'suggested_solver': solver,
    }


def test_build_command_uses_sol_b_run_py(tmp_path):
    sol_b = tmp_path / 'sol_b_root'
    sol_b.mkdir()
    cmd = build_command(_task(), sol_b_dir=sol_b)
    assert cmd[0] == 'python'
    assert cmd[1].endswith('run.py')
    assert 'sol_b_root' in cmd[1]
    assert '--net' in cmd


def test_dispatch_run_dry_run_only_sol_b_tasks(tmp_path):
    tasks_dir = tmp_path / 'tasks'
    tasks_dir.mkdir()
    (tasks_dir / 'a.json').write_text(json.dumps(_task(net='A', solver='sol_b')))
    (tasks_dir / 'b.json').write_text(json.dumps(_task(net='B', solver='sol_d')))
    results = dispatch_run(tasks_dir, dry_run=True)
    assert len(results) == 2
    sol_b_result = next(r for r in results if r.net == 'A')
    sol_d_result = next(r for r in results if r.net == 'B')
    assert sol_b_result.cmd
    assert sol_d_result.skipped is True
    assert 'suggested_solver != sol_b' in sol_d_result.skip_reason


def test_dispatch_run_defaults_solver_key_missing(tmp_path):
    """Task JSON without suggested_solver → default sol_d → skipped."""
    tasks_dir = tmp_path / 'tasks'
    tasks_dir.mkdir()
    task = _task(net='X')
    task.pop('suggested_solver')
    (tasks_dir / 'x.json').write_text(json.dumps(task))
    results = dispatch_run(tasks_dir, dry_run=True)
    assert len(results) == 1
    assert results[0].skipped is True


def test_dispatch_run_raises_on_missing_tasks_dir(tmp_path):
    with pytest.raises(FileNotFoundError):
        dispatch_run(tmp_path / 'nope', dry_run=True)


def test_dispatch_run_skips_when_em_data_missing(tmp_path):
    tasks_dir = tmp_path / 'tasks'
    tasks_dir.mkdir()
    (tasks_dir / 'a.json').write_text(json.dumps(_task()))
    results = dispatch_run(tasks_dir, dry_run=False,
                             em_data_json=tmp_path / 'nope.json')
    assert results[0].skipped is True
    assert 'em_data_json missing' in results[0].skip_reason
