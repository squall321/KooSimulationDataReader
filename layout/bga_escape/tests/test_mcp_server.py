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
    assert resp['result']['serverInfo']['name'] == 'bga-router'
    assert resp['result']['protocolVersion']


def test_initialized_notification_returns_none():
    resp = mcp_server._handle(
        {'jsonrpc': '2.0', 'method': 'notifications/initialized'})
    assert resp is None


def test_tools_list_exposes_all_six():
    resp = mcp_server._handle(_req('tools/list'))
    names = {t['name'] for t in resp['result']['tools']}
    assert names == {'register_dataset', 'route', 'get_metrics',
                      'em_dispatch', 'net_diff', 'spice_export'}
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
