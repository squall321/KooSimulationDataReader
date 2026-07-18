# route_viewer 단위 테스트 — paths_mm 렌더링 / no-fetch / XSS escape
"""Phase I-2 — routed-path viewer tests."""

from __future__ import annotations

import json
from pathlib import Path

import pytest

from bga_router.integrations.route_viewer import (
    build_copper_overlay,
    build_net_report,
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


# ---------------------------------------------------------------------------
# Phase P — net 인스펙터 (호버 → 해석·문제점 리포트)
# ---------------------------------------------------------------------------


def _metrics_for_report():
    return {
        'paths_mm': {'netB': [{'layer': 'L',
                                'points': [[0, 0], [3, 0], [3, 4]]}]},   # len 7
        'rule_check': {'by_field': {
            'width_ok': {'pass': False, 'violators': ['netB']},
            'bend_class_ok': {'pass': False, 'violators': ['netB']},
            'keep_out_ok': {'pass': True, 'violators': []}}},
        'coupling': {'top_pairs': [{'pair': ['netA', 'netB'],
                                     'length_mm': 2.5}]},
        'overlay_mm': {'vias': [{'net': 'netB'}]},
        'si': {'Z0_single_ended_ohm': {'netB': 48.2},
               'via_stub_length_mm': {'netB': 0.5},
               'branch_dc_resistance_mohm': {'netB': 90.0},
               'propagation': {'delay_ps': {'netB': 61.0}},
               'marginal_formulas': {'netB': {'tight_coupling': True,
                                               'thin_dielectric': False}}},
        'bus_groups': {'groups': [{'label': 'bus:data',
                                    'members': ['netA', 'netB']}]},
    }


def test_build_net_report_aggregates_per_net():
    rep = build_net_report(_metrics_for_report())
    b = rep['netB']
    assert b['length_mm'] == 7.0                       # 3 + 4
    assert b['vias'] == 1
    assert b['z0_ohm'] == 48.2
    assert b['delay_ps'] == 61.0
    assert set(b['violations']) == {'width_ok', 'bend_class_ok'}
    assert b['si_flags'] == ['tight_coupling']         # False 플래그는 제외
    assert b['coupling'] == [{'net': 'netA', 'length_mm': 2.5}]
    assert b['bus'] == 'bus:data'
    assert b['verdict'] == 'violation'                 # 위반 있으면 violation


def test_build_net_report_verdict_ok_and_warn():
    # 경로 있는 넷 + coupling 파트너, 위반 없음 → warn
    m = {'paths_mm': {'netW': [{'layer': 'L', 'points': [[0, 0], [2, 0]]}]},
         'coupling': {'top_pairs': [{'pair': ['netW', 'netX'],
                                      'length_mm': 1.2}]}}
    assert build_net_report(m)['netW']['verdict'] == 'warn'
    # 위반·플래그·coupling 전무 → ok
    m2 = {'paths_mm': {'netC': [{'layer': 'L', 'points': [[0, 0], [1, 0]]}]}}
    assert build_net_report(m2)['netC']['verdict'] == 'ok'


def test_render_includes_net_report_inspector():
    html_text = render_route_viewer({'dataset': 'D', 'bga': 'B',
                                      'metrics': _metrics_for_report()})
    assert '"reports"' in html_text        # inline 리포트 데이터
    assert 'renderNetReport' in html_text  # 렌더 함수
    assert 'showReport' in html_text
    assert 'distToSeg' in html_text        # 트레이스 호버 히트테스트
    assert 'id="report"' in html_text      # 리포트 패널
    assert 'hoveredNet' in html_text


# ---------------------------------------------------------------------------
# Phase Q — 원본 ODB++ 동박 오버레이
# ---------------------------------------------------------------------------


def _em_data():
    return {'layers': {
        'COMP': {'z_bottom': 0, 'z_top': 0.03, 'nets': {
            'netB': {'polygons': [{'type': 'fill',
                                    'outer': [[0, 0], [2, 0], [2, 1], [0, 1]]}]},
            'gnd': {'polygons': [{'type': 'fill',
                                   'outer': [[100, 100], [102, 100],
                                             [102, 101]]}]}}},
        'LAY2': {'z_bottom': -0.1, 'z_top': -0.07, 'nets': {
            'netB': {'polygons': [{'type': 'fill',
                                    'outer': [[0, 0], [1, 0], [1, 1]]}]}}}}}


def test_build_copper_overlay_flatten():
    cop = build_copper_overlay(_em_data())
    assert cop['truncated'] is False
    polys = cop['polys']
    assert len(polys) == 3                 # netB×2 + gnd×1
    assert all({'net', 'layer', 'outer'} <= set(p) for p in polys)
    assert {p['layer'] for p in polys} == {'COMP', 'LAY2'}


def test_build_copper_overlay_region_filter():
    # 원점 주변 영역 → (100,100)의 gnd 폴리곤 제외.
    cop = build_copper_overlay(_em_data(), region_bbox=(-1, -1, 5, 5))
    nl = {(p['net'], p['layer']) for p in cop['polys']}
    assert ('netB', 'COMP') in nl
    assert ('gnd', 'COMP') not in nl


def test_build_copper_overlay_truncation():
    big = {'layers': {'L': {'nets': {
        f'n{i}': {'polygons': [{'outer': [[0, 0], [1, 0], [1, 1]]}]}
        for i in range(10)}}}}
    cop = build_copper_overlay(big, max_polys=5)
    assert cop['truncated'] is True
    assert len(cop['polys']) == 5


def test_build_copper_overlay_per_layer_cap():
    # 레이어별 상한 — 한 층이 예산을 독식하지 않고 층마다 공평하게 잘린다.
    em = {'layers': {
        'A': {'nets': {f'n{i}': {'polygons': [
            {'outer': [[0, 0], [1, 0], [1, 1]]}]} for i in range(6)}},
        'B': {'nets': {f'm{i}': {'polygons': [
            {'outer': [[0, 0], [1, 0], [1, 1]]}]} for i in range(6)}}}}
    cop = build_copper_overlay(em, per_layer_max=4)
    from collections import Counter
    c = Counter(p['layer'] for p in cop['polys'])
    assert c['A'] == 4 and c['B'] == 4         # 층마다 4개로 제한
    assert cop['truncated'] is True
    assert set(cop['truncated_layers']) == {'A', 'B'}


def test_render_includes_copper_when_em_data():
    html_text = render_route_viewer(_eval_with_paths(), em_data=_em_data())
    assert 'drawCopper' in html_text
    assert 'buildCopperCache' in html_text
    assert 'ov-copper' in html_text
    assert '"copper"' in html_text


def test_render_without_em_data_copper_empty():
    html_text = render_route_viewer(_eval_with_paths())
    assert '"copper": []' in html_text     # em_data 없으면 빈 배열
    assert 'drawCopper' in html_text       # JS는 항상 방출


# ---------------------------------------------------------------------------
# Phase R — 실폭 트레이스 · 패키지 앞/뒷면 필터 · 레이어 뷰어(solo/다층)
# ---------------------------------------------------------------------------


def test_render_real_trace_width():
    d = _eval_with_paths()
    d['metrics']['net_width_mm'] = {'netA': 0.09, 'netB': 0.075}
    html_text = render_route_viewer(d)
    assert '"widths"' in html_text          # 넷별 폭 데이터
    assert 'WIDTHS' in html_text
    assert '* scale' in html_text           # 폭을 배율로 스케일(실폭)


def test_render_package_side_filter():
    html_text = render_route_viewer(_eval_with_paths())
    assert 'pkgSideVisible' in html_text
    assert 'name="pkgside"' in html_text    # both/top/bot 라디오
    assert "value=\"top\"" in html_text
    assert "value=\"bot\"" in html_text


def test_render_layer_viewer_controls():
    html_text = render_route_viewer(_eval_with_paths())
    assert 'syncLayerChecks' in html_text
    assert 'lay-all' in html_text           # all/none
    assert 'lay-none' in html_text
    assert 'class="solo"' in html_text      # 단층 solo
    assert 'LAYER_ORDER' in html_text       # stackup 순서
