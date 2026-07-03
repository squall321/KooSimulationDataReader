# dashboard HTML 렌더링 단위 테스트 — inline data / no fetch / 주요 섹션 포함
"""Phase H-8 — dashboard generator tests."""

from __future__ import annotations

import json
from pathlib import Path

import pytest

from bga_router.integrations.dashboard import render_dashboard, write_dashboard


def _synth_eval():
    return {
        'dataset': 'DS', 'bga': 'B99', 'recipe': 'default',
        'metrics': {
            'routed': 16, 'total': 16, 'routed_ratio': 1.0,
            'via_count': 4,
            'geometry': {'total_length_mm': 116.19, 'sharp_bends': 70},
            'rule_check': {
                'violations': 1, 'violations_expanded': 15,
                'by_field': {
                    'width_ok': {'pass': False,
                                  'violators': ['net368'], 'note': ''},
                    'clearance_ok': {'pass': True,
                                      'violators': [], 'note': ''},
                }},
            'si': {
                'Z0_single_ended_ohm': {'net368': 50.68},
                'branch_dc_resistance_mohm': {'net368': 12.3},
                'via_stub_length_mm': {'net368': 0.0},
            },
            'coupling': {'top_pairs': [
                {'pair': ['net477', 'net479'], 'length_mm': 2.31}]},
        },
    }


def test_render_contains_tiles_and_tables():
    html_text = render_dashboard(_synth_eval())
    assert '16/16' in html_text
    assert '100.0%' in html_text
    assert 'width_ok' in html_text
    assert 'net368' in html_text
    assert 'net477' in html_text and 'net479' in html_text


def test_render_no_fetch_calls():
    """CLAUDE.md 메모리 규칙 — file:// 안전성: fetch 금지."""
    html_text = render_dashboard(_synth_eval())
    assert 'fetch(' not in html_text
    assert 'XMLHttpRequest' not in html_text


def test_render_inlines_full_json():
    html_text = render_dashboard(_synth_eval())
    assert 'application/json' in html_text
    assert '"Z0_single_ended_ohm"' in html_text


def test_render_escapes_html_in_net_names():
    d = _synth_eval()
    d['metrics']['si']['Z0_single_ended_ohm'] = {'<script>x</script>': 50.0}
    html_text = render_dashboard(d)
    assert '<script>x</script></td>' not in html_text
    assert '&lt;script&gt;' in html_text


def test_render_pass_fail_classes():
    html_text = render_dashboard(_synth_eval())
    assert 'class="bad">FAIL' in html_text
    assert 'class="ok">PASS' in html_text


def test_write_dashboard_creates_file(tmp_path):
    src = tmp_path / 'eval.json'
    src.write_text(json.dumps(_synth_eval()))
    out = tmp_path / 'dash.html'
    p = write_dashboard(src, out)
    assert p.exists()
    assert '16/16' in p.read_text()


def test_render_empty_metrics_graceful():
    html_text = render_dashboard({'dataset': 'X', 'metrics': {}})
    assert 'BGA routing eval' in html_text
