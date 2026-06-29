# sol_d_runner 단위 테스트 — em-dispatch task → sol_d run.py 명령 변환
"""Phase E-4 — sol_d auto-receiver tests."""

from __future__ import annotations

import json
from pathlib import Path

import pytest

from bga_router.integrations.sol_d_runner import (
    build_command,
    dispatch_run,
    summarize_run,
)


def _task(net='DDR4_DQ0_P', task_id='T_DS__bga__r__DDR4_DQ0_P'):
    return {
        'net':     net,
        'task_id': task_id,
        'layer':   'LAY2',
        'layers':  ['LAY2'],
        'bbox_mm': [0, 0, 5, 1],
        'rule':    {'width_mm': 0.075},
        'reasons': ['tight_coupling'],
    }


def test_build_command_basic(tmp_path):
    task = _task()
    sol_d = tmp_path / 'sol_d_root'
    sol_d.mkdir()
    cmd = build_command(task, sol_d_dir=sol_d)
    assert cmd[0] == 'python'
    assert cmd[1].endswith('run.py')
    assert '--net' in cmd
    assert 'DDR4_DQ0_P' in cmd


def test_build_command_with_input_and_output(tmp_path):
    task = _task()
    em_data = tmp_path / 'em.json'; em_data.write_text('{}')
    out = tmp_path / 'out'
    cmd = build_command(task, sol_d_dir=tmp_path / 'sd',
                          em_data_json=em_data, output_dir=out)
    assert '--input' in cmd
    assert str(em_data) in cmd
    assert '--output' in cmd
    assert task['task_id'] in cmd[cmd.index('--output') + 1]


def test_build_command_passes_extra_args(tmp_path):
    cmd = build_command(_task(), sol_d_dir=tmp_path,
                          extra_args=['--no-parallel', '--verbose'])
    assert '--no-parallel' in cmd
    assert '--verbose' in cmd


def test_dispatch_run_dry_run_lists_commands(tmp_path):
    tasks_dir = tmp_path / 'tasks'
    tasks_dir.mkdir()
    (tasks_dir / 'a.json').write_text(json.dumps(_task(net='A')))
    (tasks_dir / 'b.json').write_text(json.dumps(_task(net='B')))
    results = dispatch_run(tasks_dir, dry_run=True)
    assert len(results) == 2
    nets = {r.net for r in results}
    assert nets == {'A', 'B'}
    # dry-run: rc not set
    assert all(r.rc is None for r in results)


def test_dispatch_run_handles_unparseable_task(tmp_path):
    tasks_dir = tmp_path / 'tasks'
    tasks_dir.mkdir()
    (tasks_dir / 'bad.json').write_text('not valid json [[')
    results = dispatch_run(tasks_dir, dry_run=True)
    assert len(results) == 1
    assert results[0].skipped is True
    assert 'cannot parse' in results[0].skip_reason


def test_dispatch_run_skips_when_em_data_missing(tmp_path):
    tasks_dir = tmp_path / 'tasks'
    tasks_dir.mkdir()
    (tasks_dir / 'a.json').write_text(json.dumps(_task()))
    # exec mode but em_data missing → skipped
    results = dispatch_run(tasks_dir, dry_run=False,
                             em_data_json=tmp_path / 'nope.json')
    assert results[0].skipped is True
    assert 'em_data_json missing' in results[0].skip_reason


def test_dispatch_run_raises_when_tasks_dir_missing(tmp_path):
    with pytest.raises(FileNotFoundError):
        dispatch_run(tmp_path / 'does_not_exist', dry_run=True)


def test_summarize_run_counts():
    from bga_router.integrations.sol_d_runner import RunResult
    rs = [
        RunResult(task_id='t1', net='A', cmd=['x'], rc=0),
        RunResult(task_id='t2', net='B', cmd=['x'], rc=1),
        RunResult(task_id='t3', net='C', cmd=[], skipped=True,
                    skip_reason='x'),
    ]
    s = summarize_run(rs)
    assert s['total'] == 3
    assert s['ok'] == 1
    assert s['failed'] == 1
    assert s['skipped'] == 1
    assert s['executed'] == 2  # rc set on 2 of them
