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
            'overlay_mm': {
                'pins': [{'net': 'netA', 'kind': 'source', 'layer': 'COMP',
                           'xy': [0.0, 0.0]},
                          {'net': 'netA', 'kind': 'sink', 'layer': 'COMP',
                           'xy': [5.0, 0.0]}],
                'vias': [{'net': 'netB', 'xy': [5.0, 1.0], 'kind': 'signal',
                           'start_layer': 'LAY2', 'end_layer': 'COMP'}],
                'keep_outs': [{'net': 'netA', 'bbox_mm': [2, 2, 4, 4]}],
                'packages': [
                    {'ref_des': 'U1', 'side': 'TOP', 'pin_count': 16,
                     'bbox_mm': [0.0, -1.0, 2.0, 1.0]},
                    {'ref_des': 'U2', 'side': 'BOT', 'pin_count': 4,
                     'bbox_mm': [4.0, 0.0, 6.0, 2.0]},
                ],
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


# ---------------------------------------------------------------------------
# Phase J-2 — overlay + zoom-to-net
# ---------------------------------------------------------------------------


def test_render_includes_overlay_data():
    html_text = render_route_viewer(_eval_with_paths())
    assert '"overlay"' in html_text
    assert '"pins"' in html_text
    assert '"vias"' in html_text
    assert '"keep_outs"' in html_text


def test_render_overlay_toggles_present():
    html_text = render_route_viewer(_eval_with_paths())
    assert 'ov-pins' in html_text
    assert 'ov-vias' in html_text
    assert 'ov-ko' in html_text


def test_render_zoom_to_net_function():
    html_text = render_route_viewer(_eval_with_paths())
    assert 'zoomToNet' in html_text
    assert 'ondblclick' in html_text


def test_render_drawoverlay_present():
    html_text = render_route_viewer(_eval_with_paths())
    assert 'drawOverlay' in html_text


def test_render_overlay_absent_still_valid():
    """overlay_mm 없어도 뷰어는 정상 (빈 배열 fallback)."""
    d = _eval_with_paths()
    del d['metrics']['overlay_mm']
    html_text = render_route_viewer(d)
    assert '<canvas' in html_text
    assert 'drawOverlay' in html_text


# ---------------------------------------------------------------------------
# Phase O — package outlines + refDes labels
# ---------------------------------------------------------------------------


def test_render_includes_package_data():
    html_text = render_route_viewer(_eval_with_paths())
    assert '"packages"' in html_text
    assert '"U1"' in html_text            # refDes flows into inline data
    assert '"ref_des"' in html_text


def test_render_package_toggle_and_helpers_present():
    html_text = render_route_viewer(_eval_with_paths())
    assert 'ov-pkg' in html_text          # packages 토글 체크박스
    assert 'showPkg' in html_text
    assert 'netsInPackage' in html_text   # 클릭 → 주변 net 하이라이트
    assert 'selectedPkg' in html_text


def test_render_packages_side_panel_present():
    html_text = render_route_viewer(_eval_with_paths())
    assert 'id="pkgs"' in html_text
    assert '>Packages ' in html_text      # 사이드 섹션 헤더


def test_render_packages_absent_still_valid():
    """overlay에 packages 키 없어도 뷰어 JS는 정상 방출."""
    d = _eval_with_paths()
    d['metrics']['overlay_mm'].pop('packages', None)
    html_text = render_route_viewer(d)
    assert '<canvas' in html_text
    assert 'netsInPackage' in html_text


def test_render_major_minor_and_hover_present():
    """큰 패키지 상시 라벨 / 작은 패키지 호버 표시 로직이 방출되는지."""
    html_text = render_route_viewer(_eval_with_paths())
    assert 'pkgIsMajor' in html_text
    assert 'pkgMajorMinMm' in html_text
    assert 'hoveredPkg' in html_text
    assert 'toWorld' in html_text        # 호버 히트테스트용 역변환
    assert 'pkg-thresh' in html_text     # 임계값 조절 입력
