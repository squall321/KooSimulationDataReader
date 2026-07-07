# tool_analyze_project 엔드투엔드 통합 테스트 — AIDataHub HTTP 경계만 스텁하고 나머지는 실제로 흐른다.
"""Phase — analyze_project E2E.

AIDataHub urlopen 만 monkeypatch 로 가로채고 resolve_odb_path/odb_inspect 는
실제 코드를 태운다. inspect 경로는 <odb_dir>_json/index.json 을 미리 만들어
C++ 바이너리 없이 build_odb_json_if_missing 이 no-op 로 흐르게 한다.
"""

from __future__ import annotations

import json
import os

import pytest

from bga_router.integrations import aidatahub_client as ac
from bga_router.mcp_server import tool_analyze_project


# ---------------------------------------------------------------------------
# urllib mock harness (test_aidatahub_client.py 와 동일 방식)
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


def _patch_urlopen(monkeypatch, payload):
    """urlopen 을 가로채 마지막 Request URL 을 잡아두고 payload 를 돌려준다."""
    calls = {}
    def fake_urlopen(req, timeout=None):
        calls['url'] = req.full_url
        return _FakeResp(payload)
    monkeypatch.setattr(ac.urllib.request, 'urlopen', fake_urlopen)
    return calls


# ---------------------------------------------------------------------------
# 합성 ODB json 픽스처 (test_odb_inspect.py _write_synthetic 구조 재사용)
# ---------------------------------------------------------------------------


def _write_synthetic(root):
    """root 에 index.json + 2개 레이어 json 을 만든다."""
    index = {
        "job": "SYN_JOB",
        "step": "SYN_STEP",
        "bbox": {"xmin": -1.0, "ymin": -2.0, "xmax": 9.0, "ymax": 3.0},
        "region": None,
        "layers": [
            {"name": "lay2", "file": "lay2.json", "total": 30,
             "lines": 10, "pads": 15, "arcs": 2, "surfaces": 3,
             "bbox": {"xmin": 0, "ymin": 0, "xmax": 5, "ymax": 5}},
            {"name": "lay3", "file": "lay3.json", "total": 8,
             "lines": 3, "pads": 5, "arcs": 0, "surfaces": 0,
             "bbox": {"xmin": 0, "ymin": 0, "xmax": 5, "ymax": 5}},
        ],
    }
    lay2 = {
        "name": "lay2", "units": "MM", "type": "Signal",
        "bbox": {"xmin": 0, "ymin": 0, "xmax": 5, "ymax": 5},
        "counts": {"lines": 10, "pads": 15, "arcs": 2, "surfaces": 3},
        "symbols": {
            "r100": {"type": "Round", "w": 0.1, "h": 0.1},
            "r200": {"type": "Round", "w": 0.2, "h": 0.2},
            "oval600x900": {"type": "Oblong", "w": 0.6, "h": 0.9},
        },
        "features": [{"t": "P", "x": 1.0, "y": 1.0, "sym": "r100", "pol": "P"}],
    }
    lay3 = {
        "name": "lay3", "units": "MM", "type": "Power",
        "bbox": {"xmin": 0, "ymin": 0, "xmax": 5, "ymax": 5},
        "counts": {"lines": 3, "pads": 5, "arcs": 0, "surfaces": 0},
        "symbols": {
            "r100": {"type": "Round", "w": 0.1, "h": 0.1},
            "rect300": {"type": "Rectangle", "w": 0.3, "h": 0.3},
        },
        "features": [{"t": "L", "x": 0.0, "y": 0.0, "sym": "r100", "pol": "P"}],
    }
    with open(os.path.join(root, "index.json"), "w") as f:
        json.dump(index, f)
    with open(os.path.join(root, "lay2.json"), "w") as f:
        json.dump(lay2, f)
    with open(os.path.join(root, "lay3.json"), "w") as f:
        json.dump(lay3, f)


# ---------------------------------------------------------------------------
# 테스트 케이스
# ---------------------------------------------------------------------------


def test_requires_project():
    with pytest.raises(ValueError):
        tool_analyze_project({})


def test_not_found(monkeypatch):
    _patch_urlopen(monkeypatch, {'items': [], 'total': 0})
    result = tool_analyze_project({'project': 'Z3'})
    assert result['found'] == 0
    assert 'register' in result['note']


def test_found_but_no_odb_path(monkeypatch):
    _patch_urlopen(monkeypatch, {
        'items': [{'id': 'CAD-1', 'title': 't', 'content': {}}],
        'total': 1})
    result = tool_analyze_project({'project': 'Z3'})
    assert result['found'] == 1
    assert 'analysis' not in result
    assert result.get('note')


def test_inspect_e2e(monkeypatch, tmp_path):
    # 합성 <odb_dir>_json 픽스처 — build_odb_json_if_missing 이 no-op 이 되도록.
    odb_dir = tmp_path / 'myodb'
    json_dir = tmp_path / 'myodb_json'
    json_dir.mkdir()
    _write_synthetic(str(json_dir))

    calls = _patch_urlopen(monkeypatch, {
        'items': [{'id': 'CAD-1', 'title': 'z3',
                   'content': {'file_metadata': {'odb_dir': str(odb_dir)}}}],
        'total': 1})

    r = tool_analyze_project({'project': 'Z3', 'rev': 'B', 'stage': 'DV',
                              'analysis': 'inspect'})

    assert r['found'] == 1
    assert r['odb_dir'].endswith('myodb')
    assert 'analysis' in r
    # inspect_odb 는 {'structure': {...}, 'symbols': {...}} 를 돌려준다.
    analysis = r['analysis']
    assert 'structure' in analysis
    structure = analysis['structure']
    assert structure['job'] == 'SYN_JOB'
    assert structure['step'] == 'SYN_STEP'
    assert structure['layer_count'] == 2
    layer_names = {ly['name'] for ly in structure['layers']}
    assert layer_names == {'lay2', 'lay3'}
    # 쿼리 계약 — tag=project%3AZ3 포함.
    assert 'tag=project%3AZ3' in calls['url']
