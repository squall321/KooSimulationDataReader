# spice-export .lib + 자동 testbench로 ngspice batch 실행 → crosstalk 결과 파싱
"""Phase H-6 — per-recipe SPICE simulation loop.

Takes the .lib from spice-export (G-2), synthesizes a transient
crosstalk testbench per coupled pair, runs ngspice in batch mode, and
parses the victim-net peak coupled voltage.

ngspice가 설치 안 된 환경에선 dry-run만 가능 — build_testbench로
netlist를 생성해 두고 사용자가 어디서든 실행할 수 있게 함.
"""

from __future__ import annotations

import re
import shutil
import subprocess
from pathlib import Path
from typing import Any, Dict, List, Optional, Tuple


def ngspice_available() -> bool:
    return shutil.which('ngspice') is not None


def build_testbench(lib_path: str | Path,
                      aggressor_net: str, victim_net: str, *,
                      prefix: str = 'NET_',
                      rise_ns: float = 0.1,
                      swing_v: float = 1.0,
                      sim_ns: float = 20.0) -> str:
    """Generate a transient crosstalk testbench netlist.

    Aggressor: step source through its SUBCKT, terminated 50Ω.
    Victim:    both ends terminated 50Ω; we probe near-end voltage.
    """
    lib = Path(lib_path)
    agg = prefix + aggressor_net
    vic = prefix + victim_net
    return f"""* Crosstalk testbench — aggressor={aggressor_net} victim={victim_net}
.include {lib}

* Aggressor drive: 0→{swing_v}V step, {rise_ns}ns rise
Vdrv agg_in 0 PULSE(0 {swing_v} 1n {rise_ns}n {rise_ns}n 8n 20n)
Xagg agg_in agg_out 0 {agg}
Ragg agg_out 0 50

* Victim: quiet line, both ends 50Ω
Rvic_near vic_in 0 50
Xvic vic_in vic_out 0 {vic}
Rvic_far vic_out 0 50

.tran 0.01n {sim_ns}n
.control
run
meas tran vpeak_near MAX v(vic_in)
meas tran vpeak_far  MAX v(vic_out)
quit
.endc
.end
"""


_MEAS_RE = re.compile(r'(vpeak_\w+)\s*=\s*([\d.eE+-]+)')


def parse_ngspice_output(stdout: str) -> Dict[str, float]:
    """Extract meas results: {'vpeak_near': V, 'vpeak_far': V}."""
    out: Dict[str, float] = {}
    for m in _MEAS_RE.finditer(stdout):
        try:
            out[m.group(1)] = float(m.group(2))
        except ValueError:
            continue
    return out


def run_crosstalk_sim(lib_path: str | Path,
                        aggressor_net: str, victim_net: str, *,
                        workdir: Optional[str | Path] = None,
                        timeout_s: int = 120,
                        **tb_kwargs) -> Dict[str, Any]:
    """Build testbench + run ngspice -b. Returns:
        {'ok': bool, 'vpeak_near': V|None, 'vpeak_far': V|None,
         'netlist_path': str, 'skip_reason': str|None}
    ngspice 미설치 시 netlist만 쓰고 skip_reason 반환."""
    wd = Path(workdir) if workdir else Path.cwd()
    wd.mkdir(parents=True, exist_ok=True)
    tb_text = build_testbench(lib_path, aggressor_net, victim_net,
                                **tb_kwargs)
    tb_path = wd / f'xtalk_{aggressor_net}_{victim_net}.cir'
    tb_path.write_text(tb_text)
    if not ngspice_available():
        return {'ok': False, 'vpeak_near': None, 'vpeak_far': None,
                 'netlist_path': str(tb_path),
                 'skip_reason': 'ngspice not installed'}
    try:
        proc = subprocess.run(['ngspice', '-b', str(tb_path)],
                                capture_output=True, text=True,
                                timeout=timeout_s, cwd=str(wd))
    except subprocess.TimeoutExpired:
        return {'ok': False, 'vpeak_near': None, 'vpeak_far': None,
                 'netlist_path': str(tb_path),
                 'skip_reason': f'timeout after {timeout_s}s'}
    meas = parse_ngspice_output(proc.stdout)
    ok = proc.returncode == 0 and bool(meas)
    return {
        'ok':            ok,
        'vpeak_near':    meas.get('vpeak_near'),
        'vpeak_far':     meas.get('vpeak_far'),
        'netlist_path':  str(tb_path),
        'skip_reason':   None if ok else
                          f'rc={proc.returncode}, no meas parsed',
    }


def run_crosstalk_batch(eval_result: Dict[str, Any],
                          lib_path: str | Path, *,
                          workdir: str | Path,
                          top_k: int = 5,
                          **tb_kwargs) -> Dict[str, Any]:
    """Run crosstalk sims for the top-K coupling pairs in an eval JSON."""
    coupling = ((eval_result.get('metrics') or {}).get('coupling') or {})
    pairs = coupling.get('top_pairs') or []
    results: List[Dict[str, Any]] = []
    for entry in pairs[:top_k]:
        pair = entry.get('pair') or []
        if len(pair) != 2:
            continue
        agg, vic = pair
        r = run_crosstalk_sim(lib_path, agg, vic,
                                workdir=workdir, **tb_kwargs)
        r['aggressor'] = agg
        r['victim'] = vic
        r['coupled_length_mm'] = entry.get('length_mm')
        results.append(r)
    executed = [r for r in results if r['ok']]
    return {
        'pairs_simulated': len(results),
        'pairs_ok':        len(executed),
        'results':         results,
        'worst_crosstalk_v': (max((r['vpeak_near'] or 0.0)
                                    for r in executed)
                               if executed else None),
    }
