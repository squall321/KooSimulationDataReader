# net_diff 단위 테스트 — 두 recipe 결과의 per-net delta + markdown 렌더링
"""Phase F-4 — net-level diff tests."""

from __future__ import annotations

import pytest

from bga_router.metrics.net_diff import compare_results, render_markdown


def _result(recipe='r', z0=None, dcr=None, stub=None,
              dataset='DS', bga='bga99x'):
    m = {'si': {}}
    if z0 is not None:
        m['si']['Z0_single_ended_ohm'] = z0
    if dcr is not None:
        m['si']['branch_dc_resistance_mohm'] = dcr
    if stub is not None:
        m['si']['via_stub_length_mm'] = stub
    return {'dataset': dataset, 'bga': bga, 'recipe': recipe, 'metrics': m}


def test_compare_computes_z0_deltas():
    ra = _result('A', z0={'net1': 50.0, 'net2': 55.0})
    rb = _result('B', z0={'net1': 52.0, 'net2': 55.0})
    diff = compare_results(ra, rb)
    assert diff['per_net']['net1']['z0_ohm_delta'] == pytest.approx(2.0, abs=1e-9)
    assert diff['per_net']['net2']['z0_ohm_delta'] == pytest.approx(0.0, abs=1e-9)


def test_compare_flags_only_in_a_or_b():
    ra = _result('A', z0={'net1': 50.0, 'onlyA': 60.0})
    rb = _result('B', z0={'net1': 50.0, 'onlyB': 45.0})
    diff = compare_results(ra, rb)
    assert diff['per_net']['onlyA']['only_in'] == 'a'
    assert diff['per_net']['onlyB']['only_in'] == 'b'
    assert diff['per_net']['net1']['only_in'] is None
    assert 'onlyA' in diff['summary']['nets_only_in_a']
    assert 'onlyB' in diff['summary']['nets_only_in_b']


def test_compare_biggest_delta_topk_sorted():
    ra = _result('A', z0={'a': 50.0, 'b': 50.0, 'c': 50.0})
    rb = _result('B', z0={'a': 55.0, 'b': 40.0, 'c': 51.0})
    diff = compare_results(ra, rb)
    tops = diff['summary']['z0_biggest_delta']
    # abs sorted: b (|−10|=10) > a (5) > c (1)
    assert tops[0]['net'] == 'b'
    assert tops[1]['net'] == 'a'
    assert tops[2]['net'] == 'c'


def test_compare_all_metrics_present():
    ra = _result('A', z0={'n': 50}, dcr={'n': 10}, stub={'n': 0.1})
    rb = _result('B', z0={'n': 51}, dcr={'n': 12}, stub={'n': 0.3})
    diff = compare_results(ra, rb)
    e = diff['per_net']['n']
    assert e['z0_ohm_delta'] == pytest.approx(1.0)
    assert e['dc_r_mohm_delta'] == pytest.approx(2.0)
    assert e['via_stub_mm_delta'] == pytest.approx(0.2)


def test_compare_same_recipe_labels_disambiguated():
    ra = _result('same', z0={'n': 50})
    rb = _result('same', z0={'n': 51})
    diff = compare_results(ra, rb)
    assert diff['recipe_a'] == 'same'
    assert diff['recipe_b'] == 'same:B'


def test_render_markdown_produces_expected_headings():
    ra = _result('A', z0={'n': 50}, dcr={'n': 10}, stub={'n': 0.1})
    rb = _result('B', z0={'n': 51}, dcr={'n': 12}, stub={'n': 0.3})
    diff = compare_results(ra, rb)
    md = render_markdown(diff)
    assert 'Net-level diff' in md
    assert 'Z0 biggest deltas' in md
    assert 'DC resistance biggest deltas' in md
    assert 'Via stub biggest deltas' in md


def test_compare_handles_empty_metrics():
    ra = _result('A')
    rb = _result('B')
    diff = compare_results(ra, rb)
    assert diff['per_net'] == {}
    assert diff['summary']['nets_common'] == 0
