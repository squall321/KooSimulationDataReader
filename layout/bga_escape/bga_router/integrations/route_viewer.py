# routed path를 Canvas에 그리는 인터랙티브 단일 HTML 뷰어 (inline data)
"""Phase I-2 — routed-path viewer.

eval JSON의 metrics.paths_mm (per-net mm polylines) 을 Canvas에 그림.

기능.
  - 레이어별 색상 + 레이어 토글
  - net 목록 클릭 → highlight (외곽 glow)
  - width_ok violator net 붉은 강조
  - coupling top pair는 목록에 표시, 클릭 시 두 net 동시 highlight
  - 줌 (마우스 휠) / 팬 (드래그)

데이터는 inline JSON — fetch 없음, file:// 안전 (프로젝트 규칙).
"""

from __future__ import annotations

import html
import json
from pathlib import Path
from typing import Any, Dict


_LAYER_COLORS = ['#1976d2', '#d32f2f', '#388e3c', '#f57c00',
                  '#7b1fa2', '#00838f', '#5d4037', '#455a64']


def build_net_report(m: Dict[str, Any]) -> Dict[str, Any]:
    """넷별 해석·문제점 리포트를 집계한다 (뷰어 호버 인스펙터용).

    이미 eval이 계산한 넷별 데이터(임피던스/via stub/DC저항/전파지연/DRC 위반/
    크로스토크/버스)를 넷 이름으로 모아 하나의 dict로 만든다. 뷰어는 이걸
    inline으로 받아 호버 시 해석 결과를 표시한다.
    """
    si = m.get('si') or {}
    by_field = (m.get('rule_check') or {}).get('by_field') or {}
    top_pairs = (m.get('coupling') or {}).get('top_pairs') or []
    paths = m.get('paths_mm') or {}
    overlay = m.get('overlay_mm') or {}
    z0 = si.get('Z0_single_ended_ohm') or {}
    stub = si.get('via_stub_length_mm') or {}
    dcr = si.get('branch_dc_resistance_mohm') or {}
    delay = (si.get('propagation') or {}).get('delay_ps') or {}
    marg = si.get('marginal_formulas') or {}

    # 넷별 via 개수 (overlay.vias)
    vias_per_net: Dict[str, int] = {}
    for v in (overlay.get('vias') or []):
        n = v.get('net')
        if n:
            vias_per_net[n] = vias_per_net.get(n, 0) + 1

    # 넷별 크로스토크 파트너 (top_pairs 역인덱스)
    partners: Dict[str, list] = {}
    for p in top_pairs:
        pr = p.get('pair') or []
        if len(pr) == 2:
            a, b = pr
            partners.setdefault(a, []).append(
                {'net': b, 'length_mm': p.get('length_mm')})
            partners.setdefault(b, []).append(
                {'net': a, 'length_mm': p.get('length_mm')})

    # 넷별 DRC 위반 필드 (violators 역인덱스)
    violations: Dict[str, list] = {}
    for field, e in by_field.items():
        for n in (e.get('violators') or []):
            violations.setdefault(n, []).append(field)

    # 넷별 버스 그룹 라벨
    bus_of: Dict[str, str] = {}
    for g in ((m.get('bus_groups') or {}).get('groups') or []):
        for n in (g.get('members') or []):
            bus_of[n] = g.get('label')

    def _length_mm(net: str) -> float:
        total = 0.0
        for seg in paths.get(net, []):
            pts = seg.get('points') or []
            for i in range(1, len(pts)):
                dx = pts[i][0] - pts[i - 1][0]
                dy = pts[i][1] - pts[i - 1][1]
                total += (dx * dx + dy * dy) ** 0.5
        return round(total, 3)

    nets = set(paths) | set(z0) | set(violations)
    reports: Dict[str, Any] = {}
    for n in sorted(nets):
        si_flags = [k for k, v in (marg.get(n) or {}).items() if v]
        v = violations.get(n, [])
        cpl = partners.get(n, [])
        reports[n] = {
            'length_mm': _length_mm(n),
            'vias': vias_per_net.get(n, 0),
            'z0_ohm': z0.get(n),
            'via_stub_mm': stub.get(n),
            'dc_res_mohm': dcr.get(n),
            'delay_ps': delay.get(n),
            'violations': v,
            'si_flags': si_flags,
            'coupling': cpl,
            'bus': bus_of.get(n),
            'verdict': 'violation' if v else ('warn' if (si_flags or cpl)
                                              else 'ok'),
        }
    return reports


def _region_bbox(paths: Dict[str, Any], overlay: Dict[str, Any],
                  margin: float = 3.0):
    """라우팅 영역 bbox(+margin)를 paths+pins+vias 좌표에서 잡는다.

    원본 동박을 이 영역으로 잘라 전체 보드 폭주를 막는다. 좌표 없으면 None.
    """
    xs: list = []
    ys: list = []
    for segs in paths.values():
        for seg in segs:
            for x, y in seg.get('points', []):
                xs.append(x); ys.append(y)
    for mk in (overlay.get('pins') or []) + (overlay.get('vias') or []):
        xy = mk.get('xy')
        if xy:
            xs.append(xy[0]); ys.append(xy[1])
    if not xs:
        return None
    return (min(xs) - margin, min(ys) - margin,
            max(xs) + margin, max(ys) + margin)


def build_copper_overlay(em_data: Dict[str, Any], *, region_bbox=None,
                          max_polys: int = 6000) -> Dict[str, Any]:
    """원본 ODB++ 동박(em_data: layers→nets→polygons)을 뷰어용으로 평탄화.

    반환: {'polys': [{net, layer, outer:[[x,y],...]}], 'truncated': bool}.
    region_bbox=(x0,y0,x1,y1)를 주면 폴리곤 bbox가 이 영역과 겹치는 것만 남긴다.
    max_polys를 넘으면 잘라내고 truncated=True(무음 절단 금지 — 호출측이 표기).
    """
    layers = (em_data or {}).get('layers') or {}
    out: list = []
    truncated = False
    for lname, ldata in layers.items():
        nets = (ldata or {}).get('nets') or {}
        for net, ndata in nets.items():
            for poly in (ndata or {}).get('polygons', []):
                outer = poly.get('outer') or []
                if len(outer) < 3:
                    continue
                if region_bbox is not None:
                    xs = [p[0] for p in outer]
                    ys = [p[1] for p in outer]
                    if (max(xs) < region_bbox[0] or min(xs) > region_bbox[2] or
                            max(ys) < region_bbox[1] or
                            min(ys) > region_bbox[3]):
                        continue
                out.append({
                    'net': net, 'layer': lname,
                    'outer': [[round(p[0], 4), round(p[1], 4)]
                              for p in outer]})
                if len(out) >= max_polys:
                    return {'polys': out, 'truncated': True}
    return {'polys': out, 'truncated': truncated}


def render_route_viewer(result: Dict[str, Any],
                         em_data: Dict[str, Any] = None) -> str:
    m = result.get('metrics') or {}
    paths = m.get('paths_mm') or {}
    rule_check = m.get('rule_check') or {}
    coupling = m.get('coupling') or {}
    dataset = html.escape(str(result.get('dataset', '?')))
    bga = html.escape(str(result.get('bga', '?')))

    violators: set = set()
    for e in (rule_check.get('by_field') or {}).values():
        violators.update(e.get('violators') or [])

    overlay = m.get('overlay_mm') or {}
    copper = {'polys': [], 'truncated': False}
    layer_order: list = []
    if em_data:
        copper = build_copper_overlay(
            em_data, region_bbox=_region_bbox(paths, overlay))
        # stackup 순서(위→아래) = z_top 오름차순.
        lz = []
        for ln, ld in (em_data.get('layers') or {}).items():
            zt = (ld or {}).get('z_top')
            lz.append((zt if zt is not None else 0.0, ln))
        layer_order = [ln for _, ln in sorted(lz)]
    payload = json.dumps({
        'paths':      paths,
        'violators':  sorted(violators),
        'top_pairs':  (coupling.get('top_pairs') or [])[:10],
        'overlay':    overlay,
        'reports':    build_net_report(m),
        'widths':     m.get('net_width_mm') or {},
        'copper':     copper['polys'],
        'copper_truncated': copper['truncated'],
        'layer_order': layer_order,
    }, default=str)

    copper_ui = ''
    if copper['polys']:
        n = len(copper['polys'])
        note = f'{n}+ (일부 생략)' if copper['truncated'] else f'{n} polys'
        copper_ui = ('<div style="color:#8aa0b2;font-size:10px;margin:1px 0 3px">'
                     f'ODB++ 원본 동박 — {note}</div>')

    return f"""<meta charset="utf-8">
<title>Route viewer — {dataset} / {bga}</title>
<style>
  body {{ margin: 0; font-family: -apple-system, 'Segoe UI', sans-serif;
          display: flex; height: 100vh; }}
  #side {{ width: 260px; overflow-y: auto; border-right: 1px solid #ddd;
           padding: 10px; font-size: 12px; }}
  #cv {{ flex: 1; position: relative; }}
  canvas {{ display: block; background: #101418; }}
  #report {{ position: absolute; left: 10px; bottom: 10px; width: 300px;
             max-height: 62%; overflow-y: auto; display: none;
             background: rgba(16,20,24,0.92); color: #e8eef2;
             border: 1px solid #2b3440; border-radius: 6px;
             padding: 9px 11px; font-size: 11.5px; line-height: 1.5;
             font-family: ui-monospace, 'SFMono-Regular', Menlo, monospace;
             box-shadow: 0 4px 18px rgba(0,0,0,0.45); }}
  #report .rhead {{ font-size: 13px; font-weight: 600; margin-bottom: 5px;
                    display: flex; align-items: center; gap: 7px; }}
  #report .dot {{ width: 9px; height: 9px; border-radius: 50%;
                  display: inline-block; }}
  #report .bus {{ color: #8aa0b2; font-weight: 400; font-size: 10.5px; }}
  #report table {{ width: 100%; border-collapse: collapse; }}
  #report td {{ padding: 1px 0; }}
  #report td.k {{ color: #8aa0b2; padding-right: 8px; white-space: nowrap; }}
  #report td.v {{ text-align: right; font-variant-numeric: tabular-nums; }}
  #report .prob {{ color: #ff6b6b; margin-top: 5px; }}
  #report .siflag {{ color: #ffb74d; margin-top: 3px; }}
  #report .cpl {{ color: #7fb0e8; margin-top: 3px; }}
  #report .ok {{ color: #66bb6a; margin-top: 5px; }}
  .net {{ cursor: pointer; padding: 2px 6px; border-radius: 4px; }}
  .net:hover {{ background: #eef; }}
  .net.sel {{ background: #cde; font-weight: 600; }}
  .net.viol {{ color: #c62828; }}
  .layer-toggle {{ margin: 2px 0; }}
  h3 {{ margin: 8px 0 4px; font-size: 13px; }}
  .pair {{ cursor: pointer; padding: 2px 6px; color: #555; }}
  .pair:hover {{ background: #fee; }}
</style>
<div id="side">
  <h3>{dataset} / {bga}</h3>
  <h3>Overlays</h3>
  <div><label><input type="checkbox" id="ov-pins" checked> pins</label></div>
  <div><label><input type="checkbox" id="ov-vias" checked> vias</label></div>
  <div><label><input type="checkbox" id="ov-ko" checked> keep-outs</label></div>
  <div><label><input type="checkbox" id="ov-pkg" checked> packages</label></div>
  <div><label><input type="checkbox" id="ov-copper" checked> orig. copper</label></div>
  {copper_ui}
  <h3>Layers</h3><div id="layers"></div>
  <h3>Packages <span style="font-weight:400;color:#888">(click = nearby nets, dbl = zoom)</span></h3>
  <div style="color:#888;margin:2px 0 4px">label ≥ <input id="pkg-thresh" type="number"
    value="2" min="0" step="0.5" style="width:46px"> mm
    <span style="font-size:10px">(smaller: hover)</span></div>
  <div style="color:#888;margin:2px 0 4px">side:
    <label><input type="radio" name="pkgside" value="all" checked> <span style="color:#ce93d8">■</span>both</label>
    <label><input type="radio" name="pkgside" value="top"> top</label>
    <label><input type="radio" name="pkgside" value="bot"> <span style="color:#26c6da">■</span>bot</label></div>
  <div id="pkgs"></div>
  <h3>Nets <span style="font-weight:400;color:#888">(dbl-click = zoom)</span></h3>
  <div id="nets"></div>
  <h3>Top coupling pairs</h3><div id="pairs"></div>
</div>
<div id="cv"><canvas id="c"></canvas><div id="report"></div></div>

<script id="route-data" type="application/json">{payload}</script>
<script>
const DATA = JSON.parse(document.getElementById('route-data').textContent);
const COLORS = {json.dumps(_LAYER_COLORS)};
const REPORTS = DATA.reports || {{}};
const COPPER = DATA.copper || [];
const WIDTHS = DATA.widths || {{}};
const LAYER_ORDER = DATA.layer_order || [];
const canvas = document.getElementById('c');
const ctx = canvas.getContext('2d');
let selected = new Set();
let selectedPkg = null;
let hoveredPkg = null;
let hoveredNet = null;
let pkgSide = 'all';   // all | top | bot — 패키지 앞/뒷면 필터
function pkgSideVisible(pk) {{
  if (pkgSide === 'all') return true;
  if (pkgSide === 'bot') return pk.side === 'BOT';
  return pk.side !== 'BOT';   // top
}}
let pkgMajorMinMm = 2.0;   // 최대 치수 ≥ 이 값이면 '큰 패키지'(라벨 상시)
let layerVisible = {{}};
let scale = 1, offX = 0, offY = 0, dragging = false, lastX = 0, lastY = 0;

// --- bounds ---
let minX = 1e9, minY = 1e9, maxX = -1e9, maxY = -1e9;
const layerSet = new Set();
for (const net in DATA.paths) {{
  for (const seg of DATA.paths[net]) {{
    layerSet.add(seg.layer);
    for (const [x, y] of seg.points) {{
      if (x < minX) minX = x; if (x > maxX) maxX = x;
      if (y < minY) minY = y; if (y > maxY) maxY = y;
    }}
  }}
}}
// 트레이스+동박이 실제로 있는 레이어를 stackup(위→아래) 순서로 통합.
const contentLayers = new Set([...layerSet, ...COPPER.map(p => p.layer)]);
const layers = (LAYER_ORDER.length
  ? LAYER_ORDER.filter(L => contentLayers.has(L))
  : [...contentLayers].sort());
for (const L of contentLayers) if (!layers.includes(L)) layers.push(L);
layers.forEach(L => layerVisible[L] = true);
const layerColor = {{}};
layers.forEach((L, i) => layerColor[L] = COLORS[i % COLORS.length]);
// 같은 레이어의 트레이스와 원본 동박은 같은 색을 쓴다.
const copperColor = layerColor;

function fit() {{
  canvas.width = canvas.parentElement.clientWidth;
  canvas.height = window.innerHeight;
  const w = maxX - minX || 1, h = maxY - minY || 1;
  scale = Math.min(canvas.width / w, canvas.height / h) * 0.9;
  offX = (canvas.width - w * scale) / 2 - minX * scale;
  offY = (canvas.height + h * scale) / 2 + minY * scale;
}}
function toPx(x, y) {{ return [x * scale + offX, -y * scale + offY]; }}
function toWorld(px, py) {{ return [(px - offX) / scale, (offY - py) / scale]; }}

const overlay = DATA.overlay || {{pins: [], vias: [], keep_outs: [], packages: []}};
let showPins = true, showVias = true, showKO = true, showPkg = true;
let showCopper = true;

function pkgMaxDimMm(pk) {{
  const bb = pk.bbox_mm || [0, 0, 0, 0];
  return Math.max(Math.abs(bb[2] - bb[0]), Math.abs(bb[3] - bb[1]));
}}
// '큰 패키지' = 최대 치수 임계 이상 또는 핀 8개 이상(큰 IC/커넥터).
function pkgIsMajor(pk) {{
  return pkgMaxDimMm(pk) >= pkgMajorMinMm || (pk.pin_count || 0) >= 8;
}}

// --- net 인스펙터: 호버한 넷의 해석·문제점 리포트 ---
const reportEl = document.getElementById('report');
function _fmt(x, unit) {{
  if (x === null || x === undefined) return '—';
  const s = (typeof x === 'number')
    ? (Math.abs(x) >= 100 ? x.toFixed(0) : x.toFixed(2)) : x;
  return s + (unit || '');
}}
function distToSeg(px, py, x0, y0, x1, y1) {{
  const dx = x1 - x0, dy = y1 - y0;
  const l2 = dx * dx + dy * dy;
  let t = l2 ? ((px - x0) * dx + (py - y0) * dy) / l2 : 0;
  t = Math.max(0, Math.min(1, t));
  return Math.hypot(px - (x0 + t * dx), py - (y0 + t * dy));
}}
function renderNetReport(net) {{
  const r = REPORTS[net];
  if (!r) return '';
  const dc = r.verdict === 'violation' ? '#ff6b6b'
           : r.verdict === 'warn' ? '#ffb74d' : '#66bb6a';
  let h = '<div class="rhead"><span class="dot" style="background:' + dc +
          '"></span>' + net +
          (r.bus ? ' <span class="bus">' + r.bus + '</span>' : '') + '</div>';
  h += '<table>' +
    '<tr><td class="k">length</td><td class="v">' + _fmt(r.length_mm, ' mm') + '</td></tr>' +
    '<tr><td class="k">vias</td><td class="v">' + _fmt(r.vias) + '</td></tr>' +
    '<tr><td class="k">Z0 single</td><td class="v">' + _fmt(r.z0_ohm, ' \\u03a9') + '</td></tr>' +
    '<tr><td class="k">prop delay</td><td class="v">' + _fmt(r.delay_ps, ' ps') + '</td></tr>' +
    '<tr><td class="k">via stub</td><td class="v">' + _fmt(r.via_stub_mm, ' mm') + '</td></tr>' +
    '<tr><td class="k">DC res</td><td class="v">' + _fmt(r.dc_res_mohm, ' m\\u03a9') + '</td></tr>' +
    '</table>';
  if (r.violations && r.violations.length)
    h += '<div class="prob">\\u26a0 DRC 위반: ' + r.violations.join(', ') + '</div>';
  if (r.si_flags && r.si_flags.length)
    h += '<div class="siflag">\\u25c7 SI 마진: ' + r.si_flags.join(', ') + '</div>';
  if (r.coupling && r.coupling.length)
    h += '<div class="cpl">\\u2194 coupling: ' + r.coupling.map(c =>
         c.net + ' (' + _fmt(c.length_mm, 'mm') + ')').join(', ') + '</div>';
  if ((!r.violations || !r.violations.length) &&
      (!r.si_flags || !r.si_flags.length))
    h += '<div class="ok">\\u2713 DRC/SI 플래그 없음</div>';
  return h;
}}
function showReport(net) {{
  if (net && REPORTS[net]) {{
    reportEl.innerHTML = renderNetReport(net);
    reportEl.style.display = 'block';
  }} else {{
    reportEl.style.display = 'none';
  }}
}}

function _visibleForNet(net) {{
  return !selected.size || selected.has(net);
}}

function drawOverlay() {{
  // package outlines + refDes labels (behind net markers).
  // 큰 패키지: 외곽 + 라벨 상시. 작은 패키지: 흐린 외곽만; 호버(또는 선택)
  // 시 밝은 외곽 + 라벨. 조밀한 보드에서 라벨 난립을 막는다.
  if (showPkg) {{
    ctx.save();
    ctx.font = '11px monospace';
    const labels = [];   // 라벨은 모든 외곽 위에 그리도록 2-pass.
    for (const pk of (overlay.packages || [])) {{
      if (!pkgSideVisible(pk)) continue;
      const bb = pk.bbox_mm; if (!bb) continue;
      const [px0, py0] = toPx(bb[0], bb[1]);
      const [px1, py1] = toPx(bb[2], bb[3]);
      const rx = Math.min(px0, px1), ry = Math.min(py0, py1);
      const rw = Math.abs(px1 - px0), rh = Math.abs(py1 - py0);
      const isSel = pk.ref_des === selectedPkg;
      const isHov = pk.ref_des === hoveredPkg;
      const major = pkgIsMajor(pk);
      const col = isSel ? '#ffee58'
                : isHov ? '#fff59d'
                : pk.side === 'BOT' ? '#26c6da' : '#ce93d8';
      // 작은 패키지는 흐리게(호버 지점 파악용), 큰/선택/호버는 선명하게.
      ctx.globalAlpha = (major || isSel || isHov) ? 1.0 : 0.3;
      ctx.strokeStyle = col;
      ctx.lineWidth = isSel ? 2.5 : (isHov ? 2.0 : (major ? 1.5 : 1.0));
      ctx.strokeRect(rx, ry, rw, rh);
      ctx.globalAlpha = 1.0;
      if (major || isSel || isHov) labels.push([rx, ry, pk.ref_des || '', col]);
    }}
    for (const [rx, ry, label, col] of labels) {{
      const tw = ctx.measureText(label).width;
      ctx.fillStyle = 'rgba(16,20,24,0.78)';
      ctx.fillRect(rx, ry - 13, tw + 6, 13);
      ctx.fillStyle = col;
      ctx.fillText(label, rx + 3, ry - 3);
    }}
    ctx.restore();
  }}
  // keep-out zones (behind markers)
  if (showKO) {{
    for (const ko of (overlay.keep_outs || [])) {{
      if (!_visibleForNet(ko.net)) continue;
      const [x0, y0, x1, y1] = ko.bbox_mm;
      const [px0, py0] = toPx(x0, y0);
      const [px1, py1] = toPx(x1, y1);
      ctx.strokeStyle = '#ff9800';
      ctx.setLineDash([4, 3]);
      ctx.lineWidth = 1;
      ctx.strokeRect(Math.min(px0, px1), Math.min(py0, py1),
                     Math.abs(px1 - px0), Math.abs(py1 - py0));
      ctx.setLineDash([]);
    }}
  }}
  // vias (diamonds)
  if (showVias) {{
    for (const v of (overlay.vias || [])) {{
      if (!_visibleForNet(v.net)) continue;
      const [px, py] = toPx(v.xy[0], v.xy[1]);
      ctx.fillStyle = v.kind === 'power_ground' ? '#4caf50' : '#e0e0e0';
      ctx.beginPath();
      ctx.moveTo(px, py - 4); ctx.lineTo(px + 4, py);
      ctx.lineTo(px, py + 4); ctx.lineTo(px - 4, py);
      ctx.closePath(); ctx.fill();
    }}
  }}
  // pins (circles: source hollow, sink filled)
  if (showPins) {{
    for (const p of (overlay.pins || [])) {{
      if (!_visibleForNet(p.net)) continue;
      const [px, py] = toPx(p.xy[0], p.xy[1]);
      ctx.beginPath();
      ctx.arc(px, py, 3.5, 0, 2 * Math.PI);
      ctx.strokeStyle = '#ffca28';
      ctx.lineWidth = 1.5;
      if (p.kind === 'sink') {{ ctx.fillStyle = '#ffca28'; ctx.fill(); }}
      else ctx.stroke();
    }}
  }}
}}

// 원본 ODB++ 동박(fill 폴리곤)을 트레이스 뒤 베이스 레이어로 그린다.
// 폴리곤이 많아(수천) hover마다 재렌더하면 렉이 걸리므로 오프스크린 캔버스에
// 캐시해 blit한다. 팬은 blit 평행이동, 줌/레이어 변경만 캐시 재생성, hover는
// 해당 넷 폴리곤만 라이브 하이라이트한다.
let copperCanvas = null, copperScaleKey = null, copperLayerKey = '';
let copperOffX0 = 0, copperOffY0 = 0;
function _copperPath(c, pts) {{
  c.beginPath();
  for (let i = 0; i < pts.length; i++) {{
    const [px, py] = toPx(pts[i][0], pts[i][1]);
    if (i === 0) c.moveTo(px, py); else c.lineTo(px, py);
  }}
  c.closePath();
}}
function buildCopperCache() {{
  if (!copperCanvas) copperCanvas = document.createElement('canvas');
  copperCanvas.width = canvas.width;
  copperCanvas.height = canvas.height;
  const c = copperCanvas.getContext('2d');
  c.clearRect(0, 0, copperCanvas.width, copperCanvas.height);
  copperOffX0 = offX; copperOffY0 = offY;
  for (const poly of COPPER) {{
    if (poly.layer in layerVisible && !layerVisible[poly.layer]) continue;
    const pts = poly.outer;
    if (!pts || pts.length < 3) continue;
    _copperPath(c, pts);
    const col = copperColor[poly.layer] || '#6b7a86';
    c.fillStyle = col; c.globalAlpha = 0.2; c.fill();
    c.strokeStyle = col; c.globalAlpha = 0.4; c.lineWidth = 0.7; c.stroke();
  }}
  c.globalAlpha = 1.0;
  copperScaleKey = scale;
  copperLayerKey = JSON.stringify(layerVisible);
}}
function drawCopper() {{
  if (copperScaleKey !== scale ||
      copperLayerKey !== JSON.stringify(layerVisible) || !copperCanvas ||
      copperCanvas.width !== canvas.width ||
      copperCanvas.height !== canvas.height) {{
    buildCopperCache();
  }}
  ctx.drawImage(copperCanvas, offX - copperOffX0, offY - copperOffY0);
  // 호버한 넷의 원본 동박만 밝게(라이브, 소량).
  if (hoveredNet) {{
    for (const poly of COPPER) {{
      if (poly.net !== hoveredNet) continue;
      if (poly.layer in layerVisible && !layerVisible[poly.layer]) continue;
      const pts = poly.outer;
      if (!pts || pts.length < 3) continue;
      _copperPath(ctx, pts);
      const col = copperColor[poly.layer] || '#6b7a86';
      ctx.fillStyle = col; ctx.globalAlpha = 0.5; ctx.fill();
      ctx.strokeStyle = col; ctx.globalAlpha = 0.9; ctx.lineWidth = 1.5;
      ctx.stroke();
      ctx.globalAlpha = 1.0;
    }}
  }}
}}

function draw() {{
  ctx.clearRect(0, 0, canvas.width, canvas.height);
  if (showCopper) drawCopper();
  ctx.lineJoin = 'round';
  ctx.lineCap = 'round';
  for (const net in DATA.paths) {{
    const isSel = selected.has(net) || net === hoveredNet;
    const isViol = DATA.violators.includes(net);
    // 실제 트레이스 폭(mm)을 화면 배율로 — 줌하면 두께가 커진다. 최소 1.2px.
    const wpx = Math.max(1.2, (WIDTHS[net] || 0.075) * scale);
    for (const seg of DATA.paths[net]) {{
      if (!layerVisible[seg.layer]) continue;
      ctx.beginPath();
      seg.points.forEach(([x, y], i) => {{
        const [px, py] = toPx(x, y);
        if (i === 0) ctx.moveTo(px, py); else ctx.lineTo(px, py);
      }});
      ctx.lineWidth = isSel ? Math.max(wpx, 3) : wpx;
      ctx.strokeStyle = isSel ? '#ffee58'
                       : isViol ? '#ef5350'
                       : layerColor[seg.layer];
      ctx.globalAlpha = (selected.size && !isSel) ? 0.25 : 1.0;
      ctx.stroke();
      ctx.globalAlpha = 1.0;
    }}
  }}
  drawOverlay();
}}

function zoomToNet(net) {{
  // bbox of this net's path points
  let nx0 = 1e9, ny0 = 1e9, nx1 = -1e9, ny1 = -1e9;
  for (const seg of (DATA.paths[net] || [])) {{
    for (const [x, y] of seg.points) {{
      if (x < nx0) nx0 = x; if (x > nx1) nx1 = x;
      if (y < ny0) ny0 = y; if (y > ny1) ny1 = y;
    }}
  }}
  if (nx1 < nx0) return;
  const w = (nx1 - nx0) || 1, h = (ny1 - ny0) || 1;
  scale = Math.min(canvas.width / w, canvas.height / h) * 0.6;
  const cx = (nx0 + nx1) / 2, cy = (ny0 + ny1) / 2;
  offX = canvas.width / 2 - cx * scale;
  offY = canvas.height / 2 + cy * scale;
  draw();
}}

// overlay toggles
document.getElementById('ov-pins').onchange = e => {{
  showPins = e.target.checked; draw();
}};
document.getElementById('ov-vias').onchange = e => {{
  showVias = e.target.checked; draw();
}};
document.getElementById('ov-ko').onchange = e => {{
  showKO = e.target.checked; draw();
}};
document.getElementById('ov-pkg').onchange = e => {{
  showPkg = e.target.checked; draw();
}};
{{
  const cb = document.getElementById('ov-copper');
  if (cb) cb.onchange = e => {{ showCopper = e.target.checked; draw(); }};
}}
document.getElementById('pkg-thresh').oninput = e => {{
  const v = parseFloat(e.target.value);
  if (!isNaN(v)) {{ pkgMajorMinMm = v; draw(); }}
}};
document.querySelectorAll('input[name="pkgside"]').forEach(r => {{
  r.onchange = e => {{
    pkgSide = e.target.value;
    document.querySelectorAll('#pkgs .net').forEach(el => {{
      const s = el.dataset.side;
      const vis = pkgSide === 'all'
        || (pkgSide === 'bot' ? s === 'BOT' : s !== 'BOT');
      el.style.display = vis ? '' : 'none';
    }});
    draw();
  }};
}});

// --- side panel ---
const layersDiv = document.getElementById('layers');
function syncLayerChecks() {{
  document.querySelectorAll('#layers input[type=checkbox]').forEach(cb => {{
    cb.checked = !!layerVisible[cb.dataset.layer];
  }});
}}
{{
  const hdr = document.createElement('div');
  hdr.style.cssText = 'margin:0 0 3px;color:#888;font-size:10px';
  hdr.innerHTML = '<a href="#" id="lay-all">all</a> · '
    + '<a href="#" id="lay-none">none</a> '
    + '<span style="color:#667">· 위→아래(stackup)</span>';
  layersDiv.appendChild(hdr);
  hdr.querySelector('#lay-all').onclick = ev => {{ ev.preventDefault();
    layers.forEach(L => layerVisible[L] = true); syncLayerChecks(); draw(); }};
  hdr.querySelector('#lay-none').onclick = ev => {{ ev.preventDefault();
    layers.forEach(L => layerVisible[L] = false); syncLayerChecks(); draw(); }};
}}
layers.forEach(L => {{
  const d = document.createElement('div');
  d.className = 'layer-toggle';
  d.innerHTML = `<label><input type="checkbox" data-layer="${{L}}" checked> `
    + `<span style="color:${{layerColor[L]}}">■</span> ${{L}}</label>`
    + ` <a href="#" class="solo" style="color:#7fb0e8;font-size:10px">solo</a>`;
  d.querySelector('input').onchange = e => {{
    layerVisible[L] = e.target.checked; draw();
  }};
  // solo: 이 층만 보기 ↔ 다시 누르면 전체 복원.
  d.querySelector('.solo').onclick = ev => {{
    ev.preventDefault();
    const onlyThis = layers.every(x => (x === L) === !!layerVisible[x]);
    layers.forEach(x => layerVisible[x] = onlyThis ? true : (x === L));
    syncLayerChecks(); draw();
  }};
  layersDiv.appendChild(d);
}});

// --- packages panel: click = highlight nets whose pins sit inside the
//     package bbox (see the routing state *around* that component); dbl = zoom.
function netsInPackage(pk) {{
  const bb = pk.bbox_mm || [0, 0, 0, 0];
  const lox = Math.min(bb[0], bb[2]), hix = Math.max(bb[0], bb[2]);
  const loy = Math.min(bb[1], bb[3]), hiy = Math.max(bb[1], bb[3]);
  const s = new Set();
  for (const p of (overlay.pins || [])) {{
    const x = p.xy[0], y = p.xy[1];
    if (x >= lox && x <= hix && y >= loy && y <= hiy) s.add(p.net);
  }}
  return s;
}}
function zoomToBBox(bb) {{
  const w = Math.abs(bb[2] - bb[0]) || 1, h = Math.abs(bb[3] - bb[1]) || 1;
  scale = Math.min(canvas.width / w, canvas.height / h) * 0.5;
  const cx = (bb[0] + bb[2]) / 2, cy = (bb[1] + bb[3]) / 2;
  offX = canvas.width / 2 - cx * scale;
  offY = canvas.height / 2 + cy * scale;
  draw();
}}
function syncNetHighlight() {{
  document.querySelectorAll('#nets .net').forEach(el =>
    el.classList.toggle('sel',
      selected.has(el.textContent.replace(' ⚠', ''))));
}}
const pkgsDiv = document.getElementById('pkgs');
(overlay.packages || []).slice().sort((a, b) =>
    (a.ref_des || '').localeCompare(b.ref_des || '')).forEach(pk => {{
  const d = document.createElement('div');
  d.className = 'net';
  d.dataset.side = pk.side;
  const badge = pk.side === 'BOT' ? 'B' : 'T';
  d.textContent = pk.ref_des + '  [' + badge + '·' + (pk.pin_count || 0) + 'p]';
  d.onclick = () => {{
    if (selectedPkg === pk.ref_des) {{
      selectedPkg = null; selected = new Set();
    }} else {{
      selectedPkg = pk.ref_des; selected = netsInPackage(pk);
    }}
    document.querySelectorAll('#pkgs .net').forEach(el =>
      el.classList.toggle('sel', el === d && selectedPkg !== null));
    syncNetHighlight();
    draw();
  }};
  d.ondblclick = () => {{ if (pk.bbox_mm) zoomToBBox(pk.bbox_mm); }};
  pkgsDiv.appendChild(d);
}});

const netsDiv = document.getElementById('nets');
Object.keys(DATA.paths).sort().forEach(net => {{
  const d = document.createElement('div');
  d.className = 'net' + (DATA.violators.includes(net) ? ' viol' : '');
  d.textContent = net + (DATA.violators.includes(net) ? ' ⚠' : '');
  d.onclick = () => {{
    selectedPkg = null;
    if (selected.has(net)) selected.delete(net);
    else selected.add(net);
    d.classList.toggle('sel');
    showReport(net);
    draw();
  }};
  d.onmouseenter = () => {{ showReport(net); }};
  d.ondblclick = () => {{ zoomToNet(net); }};
  netsDiv.appendChild(d);
}});

const pairsDiv = document.getElementById('pairs');
(DATA.top_pairs || []).forEach(p => {{
  const d = document.createElement('div');
  d.className = 'pair';
  d.textContent = p.pair.join(' ↔ ') + ` (${{p.length_mm}}mm)`;
  d.onclick = () => {{
    selectedPkg = null;
    selected = new Set(p.pair);
    document.querySelectorAll('#nets .net').forEach(el =>
      el.classList.toggle('sel',
        selected.has(el.textContent.replace(' ⚠', ''))));
    draw();
  }};
  pairsDiv.appendChild(d);
}});

// --- zoom / pan ---
canvas.addEventListener('wheel', e => {{
  e.preventDefault();
  const f = e.deltaY < 0 ? 1.15 : 1 / 1.15;
  offX = e.offsetX - (e.offsetX - offX) * f;
  offY = e.offsetY - (e.offsetY - offY) * f;
  scale *= f;
  draw();
}});
canvas.addEventListener('mousedown', e => {{
  dragging = true; lastX = e.offsetX; lastY = e.offsetY;
}});
canvas.addEventListener('mousemove', e => {{
  if (dragging) {{
    offX += e.offsetX - lastX; offY += e.offsetY - lastY;
    lastX = e.offsetX; lastY = e.offsetY;
    draw();
    return;
  }}
  let changed = false;
  // 1) net 트레이스 호버(얇은 타깃) → 해석 리포트 + 하이라이트.
  let netHit = null, best = 6;   // px 허용오차
  for (const net in DATA.paths) {{
    for (const seg of DATA.paths[net]) {{
      const pts = seg.points;
      for (let i = 1; i < pts.length; i++) {{
        const a = toPx(pts[i - 1][0], pts[i - 1][1]);
        const b = toPx(pts[i][0], pts[i][1]);
        const dd = distToSeg(e.offsetX, e.offsetY, a[0], a[1], b[0], b[1]);
        if (dd < best) {{ best = dd; netHit = net; }}
      }}
    }}
  }}
  if (netHit !== hoveredNet) {{
    hoveredNet = netHit; changed = true;
    if (netHit) showReport(netHit);
  }}
  canvas.style.cursor = netHit ? 'pointer' : 'default';
  // 2) 패키지 호버(면적) → 작은 패키지 라벨.
  let pkgHit = null;
  if (showPkg) {{
    const [wx, wy] = toWorld(e.offsetX, e.offsetY);
    let hitArea = Infinity;
    for (const pk of (overlay.packages || [])) {{
      if (!pkgSideVisible(pk)) continue;
      const bb = pk.bbox_mm; if (!bb) continue;
      const lox = Math.min(bb[0], bb[2]), hix = Math.max(bb[0], bb[2]);
      const loy = Math.min(bb[1], bb[3]), hiy = Math.max(bb[1], bb[3]);
      if (wx >= lox && wx <= hix && wy >= loy && wy <= hiy) {{
        const a = (hix - lox) * (hiy - loy);
        if (a < hitArea) {{ hitArea = a; pkgHit = pk.ref_des; }}
      }}
    }}
  }}
  if (pkgHit !== hoveredPkg) {{ hoveredPkg = pkgHit; changed = true; }}
  if (changed) draw();
}});
window.addEventListener('mouseup', () => dragging = false);
window.addEventListener('resize', () => {{ fit(); draw(); }});

fit(); draw();
</script>
"""


def write_route_viewer(result_path: str | Path, out_path: str | Path,
                        em_data_path: str | Path = None) -> Path:
    data = json.loads(Path(result_path).read_text())
    em_data = None
    if em_data_path:
        em_data = json.loads(Path(em_data_path).read_text())
    text = render_route_viewer(data, em_data=em_data)
    op = Path(out_path)
    op.parent.mkdir(parents=True, exist_ok=True)
    op.write_text(text)
    return op
