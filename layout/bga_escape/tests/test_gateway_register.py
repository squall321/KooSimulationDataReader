# 게이트웨이 upstream 등록 payload 단위 테스트
"""Phase M — gateway upstream registration tests."""

from __future__ import annotations

import json

import pytest

from bga_router.integrations import gateway_register as gr


def test_stdio_payload_alias_underscore():
    p = gr.build_stdio_upstream()
    # alias는 dash 불가 → 언더스코어
    assert p['alias'] == 'odb_analyzer'
    assert '-' not in p['alias']
    assert p['transport'] == 'stdio'
    assert p['command'] == 'python'
    assert p['command_args'] == ['-m', 'bga_router.mcp_server']


def test_http_payload_shape():
    p = gr.build_http_upstream('http://localhost:9040/mcp/')
    assert p['transport'] == 'http'
    assert p['url'] == 'http://localhost:9040/mcp/'
    assert p['auth']['type'] == 'bearer'
    assert p['auth']['env_var'] == 'ODB_MCP_TOKEN'
    assert p['enabled'] is True


def test_register_strips_underscore_keys(monkeypatch):
    captured = {}
    class _Resp:
        def read(self): return b'{"alias":"odb_analyzer","ok":true}'
        def __enter__(self): return self
        def __exit__(self, *a): return False
    def fake_urlopen(req, timeout=None):
        captured['body'] = json.loads(req.data.decode())
        captured['url'] = req.full_url
        return _Resp()
    monkeypatch.setattr(gr.urllib.request, 'urlopen', fake_urlopen)
    payload = gr.build_stdio_upstream()   # _note 포함
    r = gr.register_upstream(payload, base_url='http://h:8000', api_key='K')
    assert r['ok'] is True
    # _note는 전송에서 제거
    assert '_note' not in captured['body']
    assert captured['body']['alias'] == 'odb_analyzer'
    assert '/api/mcp/upstreams' in captured['url']


def test_bridge_command_shape():
    cmd = gr.bridge_command()
    # 자체 브리지 모듈을 실행 — 외부 mcp-proxy 의존 제거
    assert '-m' in cmd and 'bga_router.http_bridge' in cmd
    assert '9040' in cmd
