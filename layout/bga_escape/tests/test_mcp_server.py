# MCP 서버 단위 테스트 — JSON-RPC 핸들링 + tool 라우팅
"""Phase H-5 — MCP server tests (no subprocess; direct _handle calls)."""

from __future__ import annotations

import json
from pathlib import Path

import pytest

from bga_router import mcp_server


def _req(method, params=None, req_id=1):
    r = {'jsonrpc': '2.0', 'method': method, 'id': req_id}
    if params is not None:
        r['params'] = params
    return r


# ---------------------------------------------------------------------------
# protocol handshake
# ---------------------------------------------------------------------------


def test_initialize_returns_server_info():
    resp = mcp_server._handle(_req('initialize'))
    # ODB++ 분석기로 등록 — 정체성이 name/title/description에 드러나야 한다.
    assert resp['result']['serverInfo']['name'] == 'odb-analyzer'
    assert 'ODB' in resp['result']['serverInfo']['title']
    assert resp['result']['protocolVersion']


def test_initialized_notification_returns_none():
    resp = mcp_server._handle(
        {'jsonrpc': '2.0', 'method': 'notifications/initialized'})
    assert resp is None


def test_tools_list_exposes_all_eighteen():
    resp = mcp_server._handle(_req('tools/list'))
    names = {t['name'] for t in resp['result']['tools']}
    assert names == {'register_dataset', 'route', 'get_metrics',
                      'em_dispatch', 'net_diff', 'spice_export',
                      # Phase I-1 additions
                      'dashboard', 'sim_agg', 'em_run', 'xtalk_sim',
                      # Phase I-2 / I-3
                      'route_viewer', 'pdn',
                      # Phase J-1
                      'si_report',
                      # Phase K
                      'odb_inspect', 'odb_analyze',
                      # Phase L
                      'package_features', 'metamodel_list', 'metamodel_infer'}
    # Every tool has a schema + description
    for t in resp['result']['tools']:
        assert t['inputSchema']['type'] == 'object'
        assert t['description']


def test_unknown_method_errors():
    resp = mcp_server._handle(_req('bogus/method'))
    assert 'error' in resp
    assert resp['error']['code'] == -32601


def test_unknown_tool_errors():
    resp = mcp_server._handle(
        _req('tools/call', {'name': 'nope', 'arguments': {}}))
    assert 'error' in resp


# ---------------------------------------------------------------------------
# tool calls (direct)
# ---------------------------------------------------------------------------


def test_get_metrics_tool(tmp_path):
    eval_json = tmp_path / 'eval.json'
    eval_json.write_text(json.dumps({
        'metrics': {'routed': 16, 'total': 16,
                     'geometry': {'total_length_mm': 116.19}}}))
    resp = mcp_server._handle(_req('tools/call', {
        'name': 'get_metrics',
        'arguments': {'eval_path': str(eval_json), 'section': 'geometry'},
    }))
    body = json.loads(resp['result']['content'][0]['text'])
    assert body['geometry']['total_length_mm'] == 116.19


def test_get_metrics_summary_mode(tmp_path):
    eval_json = tmp_path / 'eval.json'
    eval_json.write_text(json.dumps({
        'metrics': {'routed': 5, 'total': 8, 'si': {}, 'geometry': {}}}))
    resp = mcp_server._handle(_req('tools/call', {
        'name': 'get_metrics',
        'arguments': {'eval_path': str(eval_json)},
    }))
    body = json.loads(resp['result']['content'][0]['text'])
    assert body['routed'] == 5
    assert 'si' in body['sections']


def test_tool_error_wrapped_as_isError(tmp_path):
    resp = mcp_server._handle(_req('tools/call', {
        'name': 'get_metrics',
        'arguments': {'eval_path': str(tmp_path / 'nope.json')},
    }))
    assert resp['result'].get('isError') is True
    assert 'ERROR' in resp['result']['content'][0]['text']


def test_em_dispatch_tool(tmp_path):
    eval_json = tmp_path / 'eval.json'
    eval_json.write_text(json.dumps({
        'dataset': 'D', 'bga': 'B', 'recipe': 'R',
        'metrics': {'em_queue': {'entries': [
            {'net': 'X', 'reason': 'tight_coupling',
              'reasons': ['tight_coupling'], 'layer': 'L1',
              'layers': ['L1'], 'bbox_mm': [0, 0, 1, 1], 'rule': {}},
        ], 'count': 1}}}))
    resp = mcp_server._handle(_req('tools/call', {
        'name': 'em_dispatch',
        'arguments': {'eval_path': str(eval_json),
                       'out_dir': str(tmp_path / 'tasks')},
    }))
    body = json.loads(resp['result']['content'][0]['text'])
    assert body['count'] == 1


def test_net_diff_tool(tmp_path):
    a = tmp_path / 'a.json'
    b = tmp_path / 'b.json'
    a.write_text(json.dumps({'recipe': 'A', 'metrics': {'si': {
        'Z0_single_ended_ohm': {'n': 50.0}}}}))
    b.write_text(json.dumps({'recipe': 'B', 'metrics': {'si': {
        'Z0_single_ended_ohm': {'n': 52.0}}}}))
    resp = mcp_server._handle(_req('tools/call', {
        'name': 'net_diff',
        'arguments': {'eval_a': str(a), 'eval_b': str(b)},
    }))
    body = json.loads(resp['result']['content'][0]['text'])
    assert 'markdown' in body
    assert body['summary']['nets_common'] == 1


def test_spice_export_tool(tmp_path):
    eval_json = tmp_path / 'eval.json'
    eval_json.write_text(json.dumps({
        'dataset': 'D', 'metrics': {'si': {
            'Z0_single_ended_ohm': {'n': 50.0},
            'branch_dc_resistance_mohm': {'n': 10.0}},
            'coupling': {}}}))
    out = tmp_path / 'x.lib'
    resp = mcp_server._handle(_req('tools/call', {
        'name': 'spice_export',
        'arguments': {'eval_path': str(eval_json), 'out_path': str(out)},
    }))
    body = json.loads(resp['result']['content'][0]['text'])
    assert Path(body['lib_path']).exists()


# ---------------------------------------------------------------------------
# Phase I-1 tools
# ---------------------------------------------------------------------------


def _full_eval(tmp_path):
    p = tmp_path / 'eval.json'
    p.write_text(json.dumps({
        'dataset': 'D', 'bga': 'B', 'recipe': 'R',
        'metrics': {
            'routed': 2, 'total': 2, 'routed_ratio': 1.0,
            'geometry': {'total_length_mm': 10.0, 'sharp_bends': 1},
            'rule_check': {'violations': 0, 'by_field': {}},
            'si': {'Z0_single_ended_ohm': {'a': 50.0}},
            'coupling': {'top_pairs': [
                {'pair': ['a', 'b'], 'length_mm': 1.0}]},
        }}))
    return p


def test_dashboard_tool(tmp_path):
    src = _full_eval(tmp_path)
    out = tmp_path / 'd.html'
    resp = mcp_server._handle(_req('tools/call', {
        'name': 'dashboard',
        'arguments': {'eval_path': str(src), 'out_path': str(out)},
    }))
    body = json.loads(resp['result']['content'][0]['text'])
    assert Path(body['html_path']).exists()


def test_sim_agg_tool_no_sim_dir_graceful(tmp_path):
    src = _full_eval(tmp_path)
    out = tmp_path / 'merged.json'
    resp = mcp_server._handle(_req('tools/call', {
        'name': 'sim_agg',
        'arguments': {'eval_path': str(src),
                       'sim_dir': str(tmp_path / 'empty_sim'),
                       'out_path': str(out)},
    }))
    body = json.loads(resp['result']['content'][0]['text'])
    assert body['net_count'] == 0
    merged = json.loads(out.read_text())
    assert 'simulated' in merged['metrics']['si']


def test_em_run_tool_dry_run(tmp_path):
    tasks = tmp_path / 'tasks'
    tasks.mkdir()
    (tasks / 't.json').write_text(json.dumps({
        'net': 'X', 'task_id': 'T1', 'suggested_solver': 'sol_d'}))
    resp = mcp_server._handle(_req('tools/call', {
        'name': 'em_run',
        'arguments': {'tasks_dir': str(tasks), 'dry_run': True},
    }))
    body = json.loads(resp['result']['content'][0]['text'])
    assert body['total'] == 1
    assert body['solver'] == 'sol_d'


def test_xtalk_sim_tool_writes_netlists(tmp_path):
    src = _full_eval(tmp_path)
    lib = tmp_path / 'x.lib'
    lib.write_text('* lib')
    resp = mcp_server._handle(_req('tools/call', {
        'name': 'xtalk_sim',
        'arguments': {'eval_path': str(src), 'lib_path': str(lib),
                       'workdir': str(tmp_path / 'wd'), 'top_k': 1},
    }))
    body = json.loads(resp['result']['content'][0]['text'])
    assert body['pairs_simulated'] == 1
    assert 'ngspice_available' in body


# ---------------------------------------------------------------------------
# Phase K — ODB direct-read tools
# ---------------------------------------------------------------------------


def _synth_odb_json(tmp_path):
    d = tmp_path / 'odbjson'
    d.mkdir()
    (d / 'index.json').write_text(json.dumps({
        'job': 'JOB', 'step': 'S',
        'bbox': {'xmin': 0, 'ymin': 0, 'xmax': 10, 'ymax': 5},
        'region': None,
        'layers': [{'name': 'lay1', 'file': 'lay1.json', 'total': 3,
                     'lines': 1, 'pads': 2, 'arcs': 0, 'surfaces': 0,
                     'bbox': {'xmin': 0, 'ymin': 0, 'xmax': 10, 'ymax': 5}}],
    }))
    (d / 'lay1.json').write_text(json.dumps({
        'name': 'lay1', 'units': 'MM', 'type': 'Signal',
        'bbox': {'xmin': 0, 'ymin': 0, 'xmax': 10, 'ymax': 5},
        'counts': {'lines': 1, 'pads': 2, 'arcs': 0, 'surfaces': 0},
        'symbols': {'r100': {'type': 'Round', 'w': 0.1, 'h': 0.1}},
        'features': [{'t': 'P', 'x': 1, 'y': 1, 'sym': 'r100', 'pol': 'P'}],
    }))
    return d


def test_odb_inspect_tool(tmp_path):
    d = _synth_odb_json(tmp_path)
    resp = mcp_server._handle(_req('tools/call', {
        'name': 'odb_inspect',
        'arguments': {'json_dir': str(d)},
    }))
    body = json.loads(resp['result']['content'][0]['text'])
    assert body['structure']['job'] == 'JOB'
    assert body['structure']['layer_count'] == 1
    assert 'symbols' in body


def test_odb_inspect_requires_input():
    resp = mcp_server._handle(_req('tools/call', {
        'name': 'odb_inspect', 'arguments': {},
    }))
    assert resp['result'].get('isError') is True


def test_odb_analyze_tool(tmp_path):
    em = tmp_path / 'em.json'
    em.write_text(json.dumps({
        'job': 'J', 'units': 'MM',
        'layers': {
            'COMP': {'z_bottom': 0, 'z_top': 0.035, 'nets': {
                'netA': {'polygons': [{'type': 'fill',
                    'outer': [[0, 0], [1, 0], [1, 1], [0, 1]]}]}}},
            'LAY2': {'z_bottom': 0.1, 'z_top': 0.135, 'nets': {
                'netA': {'polygons': [{'type': 'fill',
                    'outer': [[2, 2], [3, 2], [3, 3], [2, 3]]}]}}},
        }}))
    resp = mcp_server._handle(_req('tools/call', {
        'name': 'odb_analyze',
        'arguments': {'em_data': str(em)},
    }))
    body = json.loads(resp['result']['content'][0]['text'])
    assert body['net_count'] == 1
    assert 'netA' in body['multi_layer_nets']   # spans COMP + LAY2


def test_odb_analyze_requires_input():
    resp = mcp_server._handle(_req('tools/call', {
        'name': 'odb_analyze', 'arguments': {},
    }))
    assert resp['result'].get('isError') is True


# ---------------------------------------------------------------------------
# Phase L — metamodel tools
# ---------------------------------------------------------------------------


def test_metamodel_list_tool():
    resp = mcp_server._handle(_req('tools/call', {
        'name': 'metamodel_list', 'arguments': {},
    }))
    body = json.loads(resp['result']['content'][0]['text'])
    names = {m['name'] for m in body['metamodels']}
    assert 'thermal_shock_v0' in names


def test_metamodel_infer_direct_features():
    feats = {
        'top_pkg_area': 100.0, 'bot_pkg_area': 90.0,
        'center_distance_mm': 2.0, 'overlap_area_mm2': 60.0,
        'min_pkg_dim_mm': 8.0, 'board_thickness_mm': 1.0,
    }
    resp = mcp_server._handle(_req('tools/call', {
        'name': 'metamodel_infer',
        'arguments': {'metamodel': 'thermal_shock_v0', 'features': feats},
    }))
    body = json.loads(resp['result']['content'][0]['text'])
    assert 'thermal_shock_risk_score' in body
    assert 'provenance' in body
    assert body['provenance'].startswith('reference_analytical')


def test_metamodel_infer_requires_input():
    resp = mcp_server._handle(_req('tools/call', {
        'name': 'metamodel_infer', 'arguments': {},
    }))
    assert resp['result'].get('isError') is True


def test_package_features_requires_input():
    resp = mcp_server._handle(_req('tools/call', {
        'name': 'package_features', 'arguments': {},
    }))
    assert resp['result'].get('isError') is True
