# route_viewer 단위 테스트 — paths_mm 렌더링 / no-fetch / XSS escape
"""Phase I-2 — routed-path viewer tests."""

from __future__ import annotations

import json
from pathlib import Path

import pytest

from bga_router.integrations.route_viewer import (
    render_route_viewer,
    write_route_viewer,
)


def _eval_with_paths():
    return {
        'dataset': 'DS', 'bga': 'B99', 'recipe': 'default',
        'metrics': {
            'paths_mm': {
                'netA': [{'layer': 'COMP',
                           'points': [[0.0, 0.0], [5.0, 0.0]]}],
                'netB': [{'layer': 'LAY2',
                           'points': [[0.0, 1.0], [5.0, 1.0]]},
                          {'layer': 'COMP',
                           'points': [[5.0, 1.0], [6.0, 1.0]]}],
            },
            'rule_check': {'by_field': {
                'width_ok': {'pass': False, 'violators': ['netB'],
                              'note': ''}}},
            'coupling': {'top_pairs': [
                {'pair': ['netA', 'netB'], 'length_mm': 4.5}]},
        },
    }


def test_render_contains_canvas_and_data():
    html_text = render_route_viewer(_eval_with_paths())
    assert '<canvas' in html_text
    assert 'route-data' in html_text
    assert '"netA"' in html_text
    assert '"netB"' in html_text


def test_render_no_fetch():
    html_text = render_route_viewer(_eval_with_paths())
    assert 'fetch(' not in html_text
    assert 'XMLHttpRequest' not in html_text


def test_render_violators_marked():
    html_text = render_route_viewer(_eval_with_paths())
    assert '"violators": ["netB"]' in html_text


def test_render_coupling_pairs_included():
    html_text = render_route_viewer(_eval_with_paths())
    assert 'top_pairs' in html_text
    assert '4.5' in html_text


def test_render_escapes_dataset_name():
    d = _eval_with_paths()
    d['dataset'] = '<script>evil</script>'
    html_text = render_route_viewer(d)
    assert '<script>evil</script>' not in html_text
    assert '&lt;script&gt;' in html_text


def test_write_route_viewer_creates_file(tmp_path):
    src = tmp_path / 'e.json'
    src.write_text(json.dumps(_eval_with_paths()))
    out = tmp_path / 'v.html'
    p = write_route_viewer(src, out)
    assert p.exists()
    assert '<canvas' in p.read_text()


def test_render_empty_paths_graceful():
    html_text = render_route_viewer({'dataset': 'X', 'metrics': {}})
    assert '<canvas' in html_text
