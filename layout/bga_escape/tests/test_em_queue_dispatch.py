# em_queue dispatch 어댑터 단위 테스트 — eval JSON → per-net sim task 변환
"""Phase D-5 — em_queue_dispatch tests."""

from __future__ import annotations

import json
from pathlib import Path

import pytest

from bga_router.integrations.em_queue_dispatch import (
    build_tasks_from_result,
    dispatch,
    dispatch_many,
    summarize_dispatch,
)


def _synthetic_result(em_entries):
    return {
        'dataset': 'TEST_DS',
        'bga':     'bga99x',
        'recipe':  'default',
        'metrics': {
            'em_queue': {
                'entries': em_entries,
                'count':   len(em_entries),
            },
        },
    }


def test_build_tasks_empty_when_no_entries():
    r = _synthetic_result([])
    assert build_tasks_from_result(r) == []


def test_build_tasks_populates_task_fields():
    entries = [{
        'net':      'DDR4_DQ0_P',
        'reason':   'tight_coupling+impedance_miss',
        'reasons':  ['tight_coupling', 'impedance_miss'],
        'layer':    'LAY2',
        'layers':   ['LAY2'],
        'bbox_mm':  [0.0, 0.0, 5.0, 1.0],
        'rule':     {'width_mm': 0.075, 'pair_gap_mm': 0.04},
        'suggested_solver': 'sol_d',
    }]
    r = _synthetic_result(entries)
    tasks = build_tasks_from_result(r)
    assert len(tasks) == 1
    t = tasks[0]
    assert t['net'] == 'DDR4_DQ0_P'
    assert t['task_id'] == 'TEST_DS__bga99x__default__DDR4_DQ0_P'
    assert t['source'] == 'bga_router.em_queue'
    assert t['reasons'] == ['tight_coupling', 'impedance_miss']
    assert t['layer'] == 'LAY2'
    assert t['layers'] == ['LAY2']
    assert t['bbox_mm'] == [0.0, 0.0, 5.0, 1.0]
    assert t['rule']['width_mm'] == 0.075
    assert t['suggested_solver'] == 'sol_d'
    assert t['origin'] == {'dataset': 'TEST_DS', 'bga': 'bga99x',
                            'recipe': 'default'}


def test_build_tasks_skips_entries_without_net():
    entries = [{'net': '', 'layer': 'LAY2'},
                {'net': None, 'layer': 'LAY3'},
                {'layer': 'LAY4'}]
    tasks = build_tasks_from_result(_synthetic_result(entries))
    assert tasks == []


def test_dispatch_writes_one_file_per_task(tmp_path):
    entries = [
        {'net': 'A', 'reason': 'tight_coupling',
          'reasons': ['tight_coupling'], 'layer': 'L1', 'layers': ['L1'],
          'bbox_mm': [0, 0, 1, 1], 'rule': {}},
        {'net': 'B', 'reason': 'impedance_miss',
          'reasons': ['impedance_miss'], 'layer': 'L2', 'layers': ['L2'],
          'bbox_mm': [0, 0, 2, 2], 'rule': {}},
    ]
    r = _synthetic_result(entries)
    src = tmp_path / 'eval.json'
    src.write_text(json.dumps(r))
    out_dir = tmp_path / 'tasks'
    written = dispatch(src, out_dir)
    assert len(written) == 2
    names = {p.name for p in written}
    assert 'TEST_DS__bga99x__default__A.json' in names
    payload_a = json.loads((out_dir / 'TEST_DS__bga99x__default__A.json').read_text())
    assert payload_a['net'] == 'A'


def test_dispatch_sanitizes_filenames(tmp_path):
    entries = [{'net': 'NET/With:Bad?Chars',
                  'reason': 'tight_coupling',
                  'reasons': ['tight_coupling'], 'layer': 'L1',
                  'layers': ['L1'], 'bbox_mm': [0, 0, 1, 1], 'rule': {}}]
    r = _synthetic_result(entries)
    src = tmp_path / 'eval.json'
    src.write_text(json.dumps(r))
    written = dispatch(src, tmp_path / 'tasks')
    assert len(written) == 1
    # No /, :, ? in the resulting filename
    name = written[0].name
    assert '/' not in name
    assert ':' not in name
    assert '?' not in name


def test_dispatch_many_combines_two_results(tmp_path):
    r1 = _synthetic_result([{'net': 'X', 'reason': 'tight_coupling',
                              'reasons': ['tight_coupling'],
                              'layer': 'L1', 'layers': ['L1'],
                              'bbox_mm': [0, 0, 1, 1], 'rule': {}}])
    r2 = _synthetic_result([{'net': 'Y', 'reason': 'impedance_miss',
                              'reasons': ['impedance_miss'],
                              'layer': 'L1', 'layers': ['L1'],
                              'bbox_mm': [0, 0, 1, 1], 'rule': {}}])
    p1 = tmp_path / 'r1.json'; p1.write_text(json.dumps(r1))
    p2 = tmp_path / 'r2.json'; p2.write_text(json.dumps(r2))
    written = dispatch_many([p1, p2], tmp_path / 'tasks')
    assert len(written) == 2


def test_summarize_dispatch_handles_empty():
    s = summarize_dispatch([])
    assert s['count'] == 0
    assert s['output_dir'] is None
    assert s['sample'] == []


def test_summarize_dispatch_truncates_sample():
    fake = [Path(f'/tmp/t{i}.json') for i in range(10)]
    s = summarize_dispatch(fake)
    assert s['count'] == 10
    assert len(s['sample']) == 3
