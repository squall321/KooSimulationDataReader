# eval JSON을 단일 HTML dashboard로 렌더 (inline data — fetch 금지, file:// 안전)
"""Phase H-8 — single-file HTML dashboard generator.

Renders one route-eval JSON into a self-contained HTML page:
  - Summary tiles (routed ratio / length / vias / violations)
  - rule_check per-field table (violators listed)
  - SI table (Z0 / DC-R / via-stub per net)
  - Coupling top pairs
  - bus_groups / net_clusters / dependencies overview

The eval data is INLINED as a JSON <script> block — no network calls,
so the file opens cleanly from file:// without CORS issues.
"""

from __future__ import annotations

import html
import json
from pathlib import Path
from typing import Any, Dict


def _esc(s: Any) -> str:
    return html.escape(str(s))


def render_dashboard(result: Dict[str, Any]) -> str:
    m = result.get('metrics') or {}
    geometry = m.get('geometry') or {}
    rule_check = m.get('rule_check') or {}
    si = m.get('si') or {}
    coupling = m.get('coupling') or {}
    dataset = _esc(result.get('dataset', '?'))
    bga = _esc(result.get('bga', '?'))
    recipe = _esc(result.get('recipe', '?'))

    payload = json.dumps(result, default=str)

    routed = m.get('routed', 0)
    total = m.get('total', 0)
    ratio = (m.get('routed_ratio') or 0) * 100
    viol = rule_check.get('violations_expanded',
                            rule_check.get('violations', 0))

    # rule_check rows
    rc_rows = []
    for fname, e in sorted((rule_check.get('by_field') or {}).items()):
        status = 'PASS' if e.get('pass') else 'FAIL'
        cls = 'ok' if e.get('pass') else 'bad'
        violators = ', '.join(e.get('violators') or []) or '-'
        note = _esc(e.get('note') or '')
        rc_rows.append(
            f'<tr><td>{_esc(fname)}</td>'
            f'<td class="{cls}">{status}</td>'
            f'<td>{_esc(violators)}</td><td>{note}</td></tr>')

    # SI rows
    z0 = si.get('Z0_single_ended_ohm') or {}
    dcr = si.get('branch_dc_resistance_mohm') or {}
    stub = si.get('via_stub_length_mm') or {}
    si_rows = []
    for net in sorted(set(z0) | set(dcr) | set(stub)):
        si_rows.append(
            f'<tr><td>{_esc(net)}</td>'
            f'<td>{z0.get(net, "-")}</td>'
            f'<td>{dcr.get(net, "-")}</td>'
            f'<td>{stub.get(net, "-")}</td></tr>')

    # coupling rows
    cp_rows = []
    for e in (coupling.get('top_pairs') or [])[:10]:
        pair = ' ↔ '.join(e.get('pair') or [])
        cp_rows.append(
            f'<tr><td>{_esc(pair)}</td>'
            f'<td>{e.get("length_mm", "-")}</td></tr>')

    return f"""<meta charset="utf-8">
<title>BGA eval — {dataset} / {bga} / {recipe}</title>
<style>
  body {{ font-family: -apple-system, 'Segoe UI', sans-serif; margin: 24px;
          background: #fafafa; color: #1a1a1a; }}
  h1 {{ font-size: 20px; }} h2 {{ font-size: 16px; margin-top: 28px; }}
  .tiles {{ display: flex; gap: 12px; flex-wrap: wrap; }}
  .tile {{ background: #fff; border: 1px solid #e0e0e0; border-radius: 8px;
           padding: 14px 18px; min-width: 130px; }}
  .tile .v {{ font-size: 22px; font-weight: 600; }}
  .tile .k {{ font-size: 12px; color: #777; }}
  table {{ border-collapse: collapse; background: #fff; width: 100%;
           font-size: 13px; }}
  th, td {{ border: 1px solid #e4e4e4; padding: 6px 10px; text-align: left; }}
  th {{ background: #f0f0f0; }}
  td.ok  {{ color: #0a7d38; font-weight: 600; }}
  td.bad {{ color: #c62828; font-weight: 600; }}
  .wrap {{ overflow-x: auto; }}
</style>
<h1>BGA routing eval — {dataset} / {bga} <small>({recipe})</small></h1>

<div class="tiles">
  <div class="tile"><div class="v">{routed}/{total}</div>
    <div class="k">routed / total</div></div>
  <div class="tile"><div class="v">{ratio:.1f}%</div>
    <div class="k">routed ratio</div></div>
  <div class="tile"><div class="v">{geometry.get('total_length_mm', '-')}</div>
    <div class="k">total length (mm)</div></div>
  <div class="tile"><div class="v">{m.get('via_count', '-')}</div>
    <div class="k">vias</div></div>
  <div class="tile"><div class="v">{geometry.get('sharp_bends', '-')}</div>
    <div class="k">sharp bends</div></div>
  <div class="tile"><div class="v">{viol}</div>
    <div class="k">rule violations</div></div>
</div>

<h2>Rule check</h2>
<div class="wrap"><table>
<tr><th>field</th><th>status</th><th>violators</th><th>note</th></tr>
{''.join(rc_rows)}
</table></div>

<h2>Signal integrity (per net)</h2>
<div class="wrap"><table>
<tr><th>net</th><th>Z0 (Ω)</th><th>DC R (mΩ)</th><th>via stub (mm)</th></tr>
{''.join(si_rows)}
</table></div>

<h2>Coupling — top pairs</h2>
<div class="wrap"><table>
<tr><th>pair</th><th>coupled length (mm)</th></tr>
{''.join(cp_rows)}
</table></div>

<script id="eval-data" type="application/json">{payload}</script>
<script>
  // Full eval JSON is inlined above for programmatic access:
  //   JSON.parse(document.getElementById('eval-data').textContent)
  // No network calls — page works from file:// without CORS issues.
</script>
"""


def write_dashboard(result_path: str | Path, out_path: str | Path) -> Path:
    data = json.loads(Path(result_path).read_text())
    html_text = render_dashboard(data)
    op = Path(out_path)
    op.parent.mkdir(parents=True, exist_ok=True)
    op.write_text(html_text)
    return op
