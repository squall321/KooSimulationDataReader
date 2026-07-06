# AIDataHub client 단위 테스트 — 등록/조회 payload + tag 규칙 (urllib mock)
"""Phase M — AIDataHub client tests (no live server)."""

from __future__ import annotations

import io
import json

import pytest

from bga_router.integrations import aidatahub_client as ac
from bga_router.integrations.aidatahub_client import AIDataHubClient, _tags_for


# ---------------------------------------------------------------------------
# tag 변환
# ---------------------------------------------------------------------------


def test_tags_for_all_fields():
    assert _tags_for('Z3', 'B', 'DV') == ['project:Z3', 'rev:B', 'stage:DV']


def test_tags_for_partial():
    assert _tags_for('Z3', None, None) == ['project:Z3']


def test_tags_for_extra():
    assert _tags_for('Z3', None, None, ['x:1']) == ['project:Z3', 'x:1']


# ---------------------------------------------------------------------------
# urllib mock harness
# ---------------------------------------------------------------------------


class _FakeResp:
    def __init__(self, payload):
        self._b = json.dumps(payload).encode()
    def read(self):
        return self._b
    def __enter__(self):
        return self
    def __exit__(self, *a):
        return False


def _capture(monkeypatch, payload):
    """urlopen을 가로채 마지막 Request를 잡아둔다."""
    calls = {}
    def fake_urlopen(req, timeout=None):
        calls['url'] = req.full_url
        calls['method'] = req.get_method()
        calls['headers'] = dict(req.header_items())
        calls['body'] = req.data.decode() if req.data else None
        return _FakeResp(payload)
    monkeypatch.setattr(ac.urllib.request, 'urlopen', fake_urlopen)
    return calls


# ---------------------------------------------------------------------------
# register
# ---------------------------------------------------------------------------


def test_register_odb_payload_and_tags(monkeypatch):
    calls = _capture(monkeypatch, {'count': 1, 'ok': 1, 'results': [
        {'id': 'CAD-MX-ECAD-2026-0000000001', 'action': 'inserted'}]})
    client = AIDataHubClient(base_url='http://h:8000', api_key='K')
    r = client.register_odb(project='Z3', rev='B', stage='DV',
                             title='Z3 BGA escape rev B')
    assert r['ok'] == 1
    # import 엔드포인트 + X-API-Key
    assert '/api/records/import' in calls['url']
    assert 'auto_seq=true' in calls['url']
    assert any(k.lower() == 'x-api-key' for k in calls['headers'])
    body = json.loads(calls['body'])
    assert body['project'] == 'Z3'
    assert body['version'] == 'B'
    assert 'project:Z3' in body['tags']
    assert 'rev:B' in body['tags']
    assert 'stage:DV' in body['tags']
    assert body['content']['file_format'] == 'ODB++'


def test_register_requires_api_key(monkeypatch):
    _capture(monkeypatch, {})
    client = AIDataHubClient(base_url='http://h:8000', api_key=None)
    with pytest.raises(RuntimeError, match='API key'):
        client.register_odb(project='Z3', rev='B', stage='DV', title='t')


# ---------------------------------------------------------------------------
# query
# ---------------------------------------------------------------------------


def test_find_records_tag_filter(monkeypatch):
    calls = _capture(monkeypatch, {'items': [{'id': 'X'}], 'total': 1})
    client = AIDataHubClient(base_url='http://h:8000')
    r = client.find_records(project='Z3', rev='B', stage='DV')
    assert r['total'] == 1
    # 반복 tag 파라미터
    assert calls['url'].count('tag=') == 3
    assert 'tag=project%3AZ3' in calls['url']
    assert 'data_type=CAD' in calls['url']


def test_get_record(monkeypatch):
    calls = _capture(monkeypatch, {'id': 'CAD-1', 'title': 't'})
    client = AIDataHubClient(base_url='http://h:8000')
    r = client.get_record('CAD-1')
    assert r['id'] == 'CAD-1'
    assert '/api/records/CAD-1' in calls['url']


def test_resolve_odb_path():
    client = AIDataHubClient()
    rec = {'content': {'file_metadata': {'odb_dir': '/data/z3'}}}
    assert client.resolve_odb_path(rec) == '/data/z3'
    assert client.resolve_odb_path({'content': {}}) is None
