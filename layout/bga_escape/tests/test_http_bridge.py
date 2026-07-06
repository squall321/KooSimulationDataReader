# HTTP 브리지 단위 테스트 — POST /mcp/ 로 initialize/tools/list/notification/auth
"""Phase M-4 — HTTP bridge tests (실제 http.server를 스레드로 기동)."""

from __future__ import annotations

import json
import threading
import time
import urllib.error
import urllib.request

import pytest

from http.server import ThreadingHTTPServer

from bga_router import http_bridge


def _start_server(token=None):
    http_bridge._EXPECTED_TOKEN = token
    srv = ThreadingHTTPServer(('127.0.0.1', 0), http_bridge._make_handler())
    port = srv.server_address[1]
    t = threading.Thread(target=srv.serve_forever, daemon=True)
    t.start()
    # 기동 대기
    for _ in range(50):
        try:
            urllib.request.urlopen(f'http://127.0.0.1:{port}/health', timeout=1)
            break
        except Exception:
            time.sleep(0.02)
    return srv, port


def _post(port, payload, headers=None):
    data = json.dumps(payload).encode()
    req = urllib.request.Request(
        f'http://127.0.0.1:{port}/mcp/', data=data,
        headers={'Content-Type': 'application/json',
                  'Accept': 'application/json, text/event-stream',
                  **(headers or {})}, method='POST')
    return urllib.request.urlopen(req, timeout=3)


@pytest.fixture
def server():
    srv, port = _start_server()
    yield port
    srv.shutdown()


def test_health_get(server):
    resp = urllib.request.urlopen(f'http://127.0.0.1:{server}/health', timeout=2)
    body = json.loads(resp.read())
    assert body['status'] == 'ok'
    assert body['server'] == 'odb-analyzer'


def test_initialize_over_http(server):
    resp = _post(server, {'jsonrpc': '2.0', 'method': 'initialize', 'id': 1})
    assert resp.status == 200
    assert 'application/json' in resp.headers['Content-Type']
    # stateless — session-id 헤더 미발급
    assert resp.headers.get('mcp-session-id') is None
    body = json.loads(resp.read())
    assert body['result']['serverInfo']['name'] == 'odb-analyzer'


def test_tools_list_over_http(server):
    resp = _post(server, {'jsonrpc': '2.0', 'method': 'tools/list', 'id': 2})
    body = json.loads(resp.read())
    names = {t['name'] for t in body['result']['tools']}
    assert 'analyze_project' in names
    assert 'odb_inspect' in names
    assert len(names) == 19


def test_notification_returns_202(server):
    # id 없는 notification → 202, 빈 본문
    resp = _post(server, {'jsonrpc': '2.0',
                           'method': 'notifications/initialized'})
    assert resp.status == 202
    assert resp.read() == b''


def test_tool_call_over_http(server):
    resp = _post(server, {'jsonrpc': '2.0', 'method': 'tools/call', 'id': 3,
                           'params': {'name': 'metamodel_list',
                                       'arguments': {}}})
    body = json.loads(resp.read())
    inner = json.loads(body['result']['content'][0]['text'])
    assert any(m['name'] == 'thermal_shock_v0' for m in inner['metamodels'])


def test_wrong_path_404(server):
    data = json.dumps({'jsonrpc': '2.0', 'method': 'initialize', 'id': 1}).encode()
    req = urllib.request.Request(f'http://127.0.0.1:{server}/wrong',
                                  data=data, method='POST',
                                  headers={'Content-Type': 'application/json'})
    with pytest.raises(urllib.error.HTTPError) as ei:
        urllib.request.urlopen(req, timeout=2)
    assert ei.value.code == 404


def test_invalid_json_400(server):
    req = urllib.request.Request(f'http://127.0.0.1:{server}/mcp/',
                                  data=b'not json', method='POST',
                                  headers={'Content-Type': 'application/json'})
    with pytest.raises(urllib.error.HTTPError) as ei:
        urllib.request.urlopen(req, timeout=2)
    assert ei.value.code == 400


def test_auth_enforced():
    srv, port = _start_server(token='SECRET')
    try:
        # 토큰 없이 → 401
        with pytest.raises(urllib.error.HTTPError) as ei:
            _post(port, {'jsonrpc': '2.0', 'method': 'tools/list', 'id': 1})
        assert ei.value.code == 401
        # 올바른 토큰 → 200
        resp = _post(port, {'jsonrpc': '2.0', 'method': 'tools/list', 'id': 1},
                      headers={'Authorization': 'Bearer SECRET'})
        assert resp.status == 200
    finally:
        srv.shutdown()
        http_bridge._EXPECTED_TOKEN = None
