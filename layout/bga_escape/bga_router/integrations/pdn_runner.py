# pdn_dc (2D DC IR-drop solver) 를 bga_router 파이프라인에 연결하는 어댑터
"""Phase I-3 — PDN DC IR-drop integration.

simulation/pdn_dc/run_pdn_dc.py는 em_data.json에서 power plane을 추출해
2D DC 전압 강하를 푼다 (Phase 3b PoC 산출물). 이 어댑터는:

  1. eval JSON의 net 목록에서 PG net 자동 선별
     (is_power_ground_net — Phase D-6 휴리스틱 재사용).
  2. PG net마다 run_pdn_dc.py subprocess 호출.
  3. summary.json (max_drop_mV, resistance 등) 파싱해
     metrics.pi 블록으로 병합.

pdn_dc 패키지 자체는 안 건드림. dry-run 기본.
"""

from __future__ import annotations

import json
import subprocess
from pathlib import Path
from typing import Any, Dict, List, Optional


REPO_ROOT = Path(__file__).resolve().parents[4]
DEFAULT_PDN_DIR = REPO_ROOT / 'simulation' / 'pdn_dc'


def pick_pg_nets(eval_result: Dict[str, Any]) -> List[str]:
    """eval JSON의 routed net 중 PG로 분류되는 net 목록."""
    from bga_router.metrics.path_geometry import is_power_ground_net
    m = eval_result.get('metrics') or {}
    paths = m.get('paths_mm') or {}
    si = m.get('si') or {}
    nets = set(paths) | set(si.get('Z0_single_ended_ohm') or {})
    return sorted(n for n in nets if is_power_ground_net(n))


def build_command(net: str, *,
                    em_data_json: Path,
                    output_dir: Path,
                    pdn_dir: Path = DEFAULT_PDN_DIR,
                    layers: Optional[str] = None,
                    resolution_mm: float = 0.2,
                    source: Optional[str] = None,
                    sink: Optional[str] = None) -> List[str]:
    cmd = ['python', str(pdn_dir / 'run_pdn_dc.py'),
            '--input', str(em_data_json),
            '--net', net,
            '--resolution', str(resolution_mm),
            '--output', str(output_dir / net)]
    if layers:
        cmd += ['--layers', layers]
    if source:
        cmd += ['--source', source]
    if sink:
        cmd += ['--sink', sink]
    return cmd


def run_pdn_for_net(net: str, *,
                      em_data_json: str | Path,
                      output_dir: str | Path,
                      pdn_dir: str | Path = DEFAULT_PDN_DIR,
                      dry_run: bool = True,
                      timeout_s: int = 300,
                      **kwargs) -> Dict[str, Any]:
    """단일 PG net IR-drop 실행. dry_run이면 명령만."""
    em = Path(em_data_json)
    od = Path(output_dir)
    od.mkdir(parents=True, exist_ok=True)
    cmd = build_command(net, em_data_json=em, output_dir=od,
                          pdn_dir=Path(pdn_dir), **kwargs)
    entry: Dict[str, Any] = {'net': net, 'cmd': cmd,
                               'rc': None, 'summary': None,
                               'skipped': False, 'skip_reason': None}
    if dry_run:
        return entry
    if not em.exists():
        entry['skipped'] = True
        entry['skip_reason'] = 'em_data_json missing'
        return entry
    try:
        proc = subprocess.run(cmd, capture_output=True, text=True,
                                timeout=timeout_s, cwd=str(pdn_dir))
        entry['rc'] = proc.returncode
        summary_path = od / net / 'summary.json'
        if proc.returncode == 0 and summary_path.exists():
            entry['summary'] = json.loads(summary_path.read_text())
        elif proc.returncode != 0:
            entry['skip_reason'] = proc.stderr[-300:]
    except subprocess.TimeoutExpired:
        entry['rc'] = -1
        entry['skip_reason'] = f'timeout after {timeout_s}s'
    return entry


def summarize_pdn(eval_result: Dict[str, Any], *,
                    em_data_json: Optional[str | Path] = None,
                    output_dir: Optional[str | Path] = None,
                    dry_run: bool = True,
                    **kwargs) -> Dict[str, Any]:
    """PG net 전체 IR-drop 배치 + 요약. eval의 metrics.pi 형태 반환.

    PG net이 없거나 em_data가 없으면 명시적 사유와 함께 빈 결과.
    """
    pg_nets = pick_pg_nets(eval_result)
    if not pg_nets:
        return {
            'pg_net_count': 0,
            'note': ('no power/ground nets identified by naming '
                     'heuristic — PI analysis needs GND/VDD/VCC/VSS/PWR '
                     'net names or explicit net list'),
            'runs': [],
        }
    if em_data_json is None:
        return {
            'pg_net_count': len(pg_nets),
            'pg_nets': pg_nets,
            'note': 'em_data_json not provided — run em-run --auto-em-data '
                     'first, then pdn analysis',
            'runs': [],
        }
    runs = []
    for net in pg_nets:
        runs.append(run_pdn_for_net(
            net, em_data_json=em_data_json,
            output_dir=output_dir or Path('.') / 'pdn_out',
            dry_run=dry_run, **kwargs))
    executed = [r for r in runs if r.get('summary')]
    worst = None
    for r in executed:
        drop = (r['summary'] or {}).get('max_drop_mV')
        if drop is not None and (worst is None or drop > worst['max_drop_mV']):
            worst = {'net': r['net'], 'max_drop_mV': drop}
    return {
        'pg_net_count':  len(pg_nets),
        'pg_nets':       pg_nets,
        'runs':          runs,
        'executed':      len(executed),
        'worst_ir_drop': worst,
        'first_order_note': ('2D sheet DC solve — via/stackup 3D 효과 '
                              '미포함. sign-off엔 full PDN tool 필요.'),
    }
