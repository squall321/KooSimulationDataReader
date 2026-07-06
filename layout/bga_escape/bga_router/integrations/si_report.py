# S21 insertion loss 주파수 곡선을 SVG 스파크라인으로 그리는 단일 HTML SI 리포트
"""Phase J-1 — frequency-domain SI report.

sim-agg가 채운 metrics.si.simulated.sim_results[net].insertion_loss_curve
를 net별 SVG 스파크라인 + 요약 표로 렌더. 인라인 SVG — 외부 라이브러리
없음, file:// 안전.

각 net 행: Z0 / IL@1GHz / IL@5GHz / f3dB / worst IL + 곡선 미니그래프.
"""

from __future__ import annotations

import html
import json
from pathlib import Path
from typing import Any, Dict, List, Optional


def _svg_sparkline(freqs_ghz: List[float], il_db: List[float], *,
                     width: int = 200, height: int = 40) -> str:
    """IL(dB) vs freq 미니 곡선. y축은 [worst, 0] 정규화."""
    if not freqs_ghz or not il_db or len(freqs_ghz) != len(il_db):
        return '<span style="color:#999">no data</span>'
    fmin, fmax = min(freqs_ghz), max(freqs_ghz)
    ymin = min(il_db + [-0.1])
    ymax = 0.0
    fr = (fmax - fmin) or 1.0
    yr = (ymax - ymin) or 1.0
    pts = []
    for f, y in zip(freqs_ghz, il_db):
        px = (f - fmin) / fr * width
        py = height - (y - ymin) / yr * height   # y down
        pts.append(f'{px:.1f},{py:.1f}')
    poly = ' '.join(pts)
    return (f'<svg width="{width}" height="{height}" '
            f'style="background:#0d1117;border-radius:3px">'
            f'<polyline fill="none" stroke="#58a6ff" stroke-width="1.5" '
            f'points="{poly}"/></svg>')


def render_si_report(result: Dict[str, Any]) -> str:
    m = result.get('metrics') or {}
    si = m.get('si') or {}
    simulated = si.get('simulated') or {}
    sim_results = simulated.get('sim_results') or {}
    dataset = html.escape(str(result.get('dataset', '?')))
    bga = html.escape(str(result.get('bga', '?')))

    rows: List[str] = []
    for net in sorted(sim_results):
        r = sim_results[net]
        curve = r.get('insertion_loss_curve') or {}
        spark = _svg_sparkline(curve.get('freqs_ghz') or [],
                                 curve.get('il_db') or [])
        def _fmt(v, unit=''):
            return f'{v}{unit}' if v is not None else '—'
        rows.append(
            f'<tr>'
            f'<td>{html.escape(net)}</td>'
            f'<td>{_fmt(r.get("simulated_z0_ohm"), " Ω")}</td>'
            f'<td>{_fmt(curve.get("il_at_1ghz_db"), " dB")}</td>'
            f'<td>{_fmt(curve.get("il_at_5ghz_db"), " dB")}</td>'
            f'<td>{_fmt(curve.get("f3db_ghz"), " GHz")}</td>'
            f'<td>{_fmt(curve.get("worst_il_db"), " dB")}</td>'
            f'<td>{spark}</td>'
            f'</tr>')

    if not rows:
        rows.append('<tr><td colspan="7" style="color:#999">'
                     'no simulated results — run sim-agg first '
                     '(metrics.si.simulated missing)</td></tr>')

    payload = json.dumps(result, default=str)
    return f"""<meta charset="utf-8">
<title>SI report — {dataset} / {bga}</title>
<style>
  body {{ font-family: -apple-system, 'Segoe UI', sans-serif; margin: 24px;
          background: #fafafa; color: #1a1a1a; }}
  h1 {{ font-size: 20px; }}
  table {{ border-collapse: collapse; background: #fff; width: 100%;
           font-size: 13px; }}
  th, td {{ border: 1px solid #e4e4e4; padding: 6px 10px; text-align: left; }}
  th {{ background: #f0f0f0; }}
  .wrap {{ overflow-x: auto; }}
  .note {{ color: #777; font-size: 12px; margin: 8px 0; }}
</style>
<h1>Frequency-domain SI — {dataset} / {bga}</h1>
<p class="note">S21 insertion loss per net (from sol_d/sol_b Touchstone).
곡선 y축 = [worst IL, 0] dB, x축 = frequency sweep.</p>
<div class="wrap"><table>
<tr><th>net</th><th>Z0</th><th>IL@1GHz</th><th>IL@5GHz</th>
    <th>f3dB</th><th>worst IL</th><th>S21 curve</th></tr>
{''.join(rows)}
</table></div>
<script id="si-data" type="application/json">{payload}</script>
"""


def write_si_report(result_path: str | Path, out_path: str | Path) -> Path:
    data = json.loads(Path(result_path).read_text())
    text = render_si_report(data)
    op = Path(out_path)
    op.parent.mkdir(parents=True, exist_ok=True)
    op.write_text(text)
    return op
