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


def render_route_viewer(result: Dict[str, Any]) -> str:
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
    payload = json.dumps({
        'paths':      paths,
        'violators':  sorted(violators),
        'top_pairs':  (coupling.get('top_pairs') or [])[:10],
        'overlay':    overlay,
    }, default=str)

    return f"""<meta charset="utf-8">
<title>Route viewer — {dataset} / {bga}</title>
<style>
  body {{ margin: 0; font-family: -apple-system, 'Segoe UI', sans-serif;
          display: flex; height: 100vh; }}
  #side {{ width: 260px; overflow-y: auto; border-right: 1px solid #ddd;
           padding: 10px; font-size: 12px; }}
  #cv {{ flex: 1; }}
  canvas {{ display: block; background: #101418; }}
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
  <h3>Layers</h3><div id="layers"></div>
  <h3>Packages <span style="font-weight:400;color:#888">(click = nearby nets, dbl = zoom)</span></h3>
  <div id="pkgs"></div>
  <h3>Nets <span style="font-weight:400;color:#888">(dbl-click = zoom)</span></h3>
  <div id="nets"></div>
  <h3>Top coupling pairs</h3><div id="pairs"></div>
</div>
<div id="cv"><canvas id="c"></canvas></div>

<script id="route-data" type="application/json">{payload}</script>
<script>
const DATA = JSON.parse(document.getElementById('route-data').textContent);
const COLORS = {json.dumps(_LAYER_COLORS)};
const canvas = document.getElementById('c');
const ctx = canvas.getContext('2d');
let selected = new Set();
let selectedPkg = null;
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
const layers = [...layerSet].sort();
layers.forEach(L => layerVisible[L] = true);
const layerColor = {{}};
layers.forEach((L, i) => layerColor[L] = COLORS[i % COLORS.length]);

function fit() {{
  canvas.width = canvas.parentElement.clientWidth;
  canvas.height = window.innerHeight;
  const w = maxX - minX || 1, h = maxY - minY || 1;
  scale = Math.min(canvas.width / w, canvas.height / h) * 0.9;
  offX = (canvas.width - w * scale) / 2 - minX * scale;
  offY = (canvas.height + h * scale) / 2 + minY * scale;
}}
function toPx(x, y) {{ return [x * scale + offX, -y * scale + offY]; }}

const overlay = DATA.overlay || {{pins: [], vias: [], keep_outs: [], packages: []}};
let showPins = true, showVias = true, showKO = true, showPkg = true;

function _visibleForNet(net) {{
  return !selected.size || selected.has(net);
}}

function drawOverlay() {{
  // package outlines + refDes labels (behind net markers)
  if (showPkg) {{
    ctx.save();
    ctx.font = '11px monospace';
    for (const pk of (overlay.packages || [])) {{
      const bb = pk.bbox_mm; if (!bb) continue;
      const [px0, py0] = toPx(bb[0], bb[1]);
      const [px1, py1] = toPx(bb[2], bb[3]);
      const rx = Math.min(px0, px1), ry = Math.min(py0, py1);
      const rw = Math.abs(px1 - px0), rh = Math.abs(py1 - py0);
      // TOP = mauve, BOT = cyan. Selected package = bright outline.
      const col = pk.ref_des === selectedPkg ? '#ffee58'
                : pk.side === 'BOT' ? '#26c6da' : '#ce93d8';
      ctx.strokeStyle = col;
      ctx.lineWidth = pk.ref_des === selectedPkg ? 2.5 : 1.5;
      ctx.strokeRect(rx, ry, rw, rh);
      const label = pk.ref_des || '';
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

function draw() {{
  ctx.clearRect(0, 0, canvas.width, canvas.height);
  for (const net in DATA.paths) {{
    const isSel = selected.has(net);
    const isViol = DATA.violators.includes(net);
    for (const seg of DATA.paths[net]) {{
      if (!layerVisible[seg.layer]) continue;
      ctx.beginPath();
      seg.points.forEach(([x, y], i) => {{
        const [px, py] = toPx(x, y);
        if (i === 0) ctx.moveTo(px, py); else ctx.lineTo(px, py);
      }});
      ctx.lineWidth = isSel ? 4 : 1.5;
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

// --- side panel ---
const layersDiv = document.getElementById('layers');
layers.forEach(L => {{
  const d = document.createElement('div');
  d.className = 'layer-toggle';
  d.innerHTML = `<label><input type="checkbox" checked> ` +
    `<span style="color:${{layerColor[L]}}">■</span> ${{L}}</label>`;
  d.querySelector('input').onchange = e => {{
    layerVisible[L] = e.target.checked; draw();
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
    draw();
  }};
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
  if (!dragging) return;
  offX += e.offsetX - lastX; offY += e.offsetY - lastY;
  lastX = e.offsetX; lastY = e.offsetY;
  draw();
}});
window.addEventListener('mouseup', () => dragging = false);
window.addEventListener('resize', () => {{ fit(); draw(); }});

fit(); draw();
</script>
"""


def write_route_viewer(result_path: str | Path, out_path: str | Path) -> Path:
    data = json.loads(Path(result_path).read_text())
    text = render_route_viewer(data)
    op = Path(out_path)
    op.parent.mkdir(parents=True, exist_ok=True)
    op.write_text(text)
    return op
