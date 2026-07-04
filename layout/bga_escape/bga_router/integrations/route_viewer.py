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

    payload = json.dumps({
        'paths':      paths,
        'violators':  sorted(violators),
        'top_pairs':  (coupling.get('top_pairs') or [])[:10],
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
  <h3>Layers</h3><div id="layers"></div>
  <h3>Nets</h3><div id="nets"></div>
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
}}

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

const netsDiv = document.getElementById('nets');
Object.keys(DATA.paths).sort().forEach(net => {{
  const d = document.createElement('div');
  d.className = 'net' + (DATA.violators.includes(net) ? ' viol' : '');
  d.textContent = net + (DATA.violators.includes(net) ? ' ⚠' : '');
  d.onclick = () => {{
    if (selected.has(net)) selected.delete(net);
    else selected.add(net);
    d.classList.toggle('sel');
    draw();
  }};
  netsDiv.appendChild(d);
}});

const pairsDiv = document.getElementById('pairs');
(DATA.top_pairs || []).forEach(p => {{
  const d = document.createElement('div');
  d.className = 'pair';
  d.textContent = p.pair.join(' ↔ ') + ` (${{p.length_mm}}mm)`;
  d.onclick = () => {{
    selected = new Set(p.pair);
    document.querySelectorAll('.net').forEach(el =>
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
