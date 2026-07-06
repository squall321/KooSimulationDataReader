# sol_d/sol_b가 남긴 Touchstone(.s2p) 결과를 파싱해 analytical Z0 vs simulated Z0 correlation 산출
"""Phase H-4 — simulation result aggregator.

After ``em-run`` executes sol_d / sol_b, each task leaves a Touchstone
(``.s2p``) file in its per-task output directory. This module walks
those files, parses the S-parameters, extracts a reference S11-derived
Z0 (near-DC), and folds the results back into a fresh
``metrics.si.simulated_*`` block that can be diffed against the
Wadell analytical value.

Kept dependency-light — no scikit-rf import. Reads Touchstone text
directly (v1 format, 2-port).
"""

from __future__ import annotations

import cmath
import math
import re
from pathlib import Path
from typing import Any, Dict, Iterable, List, Optional, Tuple


# One-line Touchstone v1 datapoint: freq  s11r s11i  s21r s21i  s12r s12i  s22r s22i
_DATA_RE = re.compile(
    r'^\s*([\d.eE+-]+)\s+'          # freq
    r'([\d.eE+-]+)\s+([\d.eE+-]+)\s+'
    r'([\d.eE+-]+)\s+([\d.eE+-]+)\s+'
    r'([\d.eE+-]+)\s+([\d.eE+-]+)\s+'
    r'([\d.eE+-]+)\s+([\d.eE+-]+)\s*$'
)


def parse_touchstone_s2p(path: Path) -> Optional[Dict[str, Any]]:
    """Parse a 2-port Touchstone v1 file. Returns:
        {'freqs_hz': [...], 's11': [complex], 's21': [complex],
         's12': [complex], 's22': [complex], 'z0_ref': float}
    Returns None if the file is unparseable."""
    if not path.exists():
        return None
    freqs: List[float] = []
    s11: List[complex] = []
    s21: List[complex] = []
    s12: List[complex] = []
    s22: List[complex] = []
    freq_scale = 1.0
    z0_ref = 50.0
    fmt = 'RI'   # RI / MA / DB — we only handle RI here
    try:
        text = path.read_text(errors='replace')
    except OSError:
        return None
    for raw in text.splitlines():
        line = raw.strip()
        if not line or line.startswith('!'):
            continue
        if line.startswith('#'):
            tokens = line.upper().split()
            for i, t in enumerate(tokens):
                if t == 'HZ':      freq_scale = 1.0
                elif t == 'KHZ':   freq_scale = 1e3
                elif t == 'MHZ':   freq_scale = 1e6
                elif t == 'GHZ':   freq_scale = 1e9
                elif t == 'R' and i + 1 < len(tokens):
                    try:
                        z0_ref = float(tokens[i + 1])
                    except ValueError:
                        pass
                elif t in ('RI', 'MA', 'DB'):
                    fmt = t
            continue
        m = _DATA_RE.match(line)
        if not m:
            continue
        f = float(m.group(1)) * freq_scale
        vals = [float(m.group(i)) for i in range(2, 10)]
        if fmt == 'RI':
            s11.append(complex(vals[0], vals[1]))
            s21.append(complex(vals[2], vals[3]))
            s12.append(complex(vals[4], vals[5]))
            s22.append(complex(vals[6], vals[7]))
        elif fmt == 'MA':
            def _c(mag, ang_deg):
                a = math.radians(ang_deg)
                return mag * cmath.exp(1j * a)
            s11.append(_c(vals[0], vals[1]))
            s21.append(_c(vals[2], vals[3]))
            s12.append(_c(vals[4], vals[5]))
            s22.append(_c(vals[6], vals[7]))
        else:  # DB
            def _c(db, ang_deg):
                mag = 10.0 ** (db / 20.0)
                a = math.radians(ang_deg)
                return mag * cmath.exp(1j * a)
            s11.append(_c(vals[0], vals[1]))
            s21.append(_c(vals[2], vals[3]))
            s12.append(_c(vals[4], vals[5]))
            s22.append(_c(vals[6], vals[7]))
        freqs.append(f)
    if not freqs:
        return None
    return {'freqs_hz': freqs, 's11': s11, 's21': s21,
             's12': s12, 's22': s22, 'z0_ref': z0_ref}


def z0_from_s11(s11_lo: complex, z0_ref: float) -> Optional[float]:
    """Convert reflection coefficient near DC → Z0 in ohms.
    Z = Z0_ref * (1 + Γ) / (1 - Γ). Returns |Z|."""
    denom = 1.0 - s11_lo
    if abs(denom) < 1e-12:
        return None
    z = z0_ref * (1.0 + s11_lo) / denom
    return abs(z)


def insertion_loss_db(s21: List[complex]) -> Optional[float]:
    """Loss at the lowest frequency (near-DC): |S21| in dB."""
    if not s21:
        return None
    return 20.0 * math.log10(max(1e-12, abs(s21[0])))


def _s21_db(s21_c: complex) -> float:
    return 20.0 * math.log10(max(1e-12, abs(s21_c)))


def insertion_loss_curve(freqs_hz: List[float], s21: List[complex]
                           ) -> Dict[str, Any]:
    """Phase J-1 — full-band S21 insertion-loss curve.

    Returns:
      {
        'freqs_ghz':      [...],
        'il_db':          [...],           # |S21| in dB at each freq
        'il_at_1ghz_db':  float|None,      # interpolated
        'il_at_5ghz_db':  float|None,
        'f3db_ghz':       float|None,      # first freq where IL <= -3 dB
        'worst_il_db':    float,           # most negative
      }
    """
    if not freqs_hz or not s21:
        return {'freqs_ghz': [], 'il_db': [], 'il_at_1ghz_db': None,
                 'il_at_5ghz_db': None, 'f3db_ghz': None,
                 'worst_il_db': None}
    ghz = [f / 1e9 for f in freqs_hz]
    il = [_s21_db(c) for c in s21]

    def _interp_at(target_ghz: float) -> Optional[float]:
        if target_ghz < ghz[0] or target_ghz > ghz[-1]:
            return None
        for i in range(1, len(ghz)):
            if ghz[i] >= target_ghz:
                f0, f1 = ghz[i - 1], ghz[i]
                y0, y1 = il[i - 1], il[i]
                if f1 == f0:
                    return round(y1, 4)
                t = (target_ghz - f0) / (f1 - f0)
                return round(y0 + t * (y1 - y0), 4)
        return round(il[-1], 4)

    # -3 dB crossing (first frequency dropping to/below -3 dB)
    f3db = None
    for i, y in enumerate(il):
        if y <= -3.0:
            f3db = round(ghz[i], 4)
            break

    return {
        'freqs_ghz':     [round(g, 4) for g in ghz],
        'il_db':         [round(y, 4) for y in il],
        'il_at_1ghz_db': _interp_at(1.0),
        'il_at_5ghz_db': _interp_at(5.0),
        'f3db_ghz':      f3db,
        'worst_il_db':   round(min(il), 4),
    }


def _z0_from_solver_summary(task_dir: Path, net: str) -> Optional[float]:
    """sol_d/sol_b가 남긴 summary.json의 field-solve Z0를 우선 사용.

    Phase I-4 실검증에서 확인: near-DC S11 역산 Z0는 electrically-short
    라인에서 reference 저항 (50Ω) 으로 수렴해 부정확 (예: sol_d 자체
    2D 솔브 58.5Ω인데 S11 역산은 50.05Ω). summary가 있으면 그 값이
    ground truth."""
    import json as _json
    for cand in (task_dir / 'summary.json',
                  task_dir.parent / 'summary.json'):
        if not cand.exists():
            continue
        try:
            data = _json.loads(cand.read_text())
        except Exception:
            continue
        # sol_d summary shape (Phase I-4 실검증 확인):
        #   {'nets': {net: {'Z0_avg': ..., 'eps_eff_avg': ...}}}
        # 방어적으로 Z0_avg / z0_ohm / Z0 순으로 탐색.
        nets = data.get('nets')
        if isinstance(nets, dict) and net in nets:
            entry = nets[net]
            for key in ('Z0_avg', 'z0_ohm', 'Z0', 'z0'):
                z = entry.get(key)
                if isinstance(z, (int, float)):
                    return float(z)
        for r in (data.get('results') or []):
            if r.get('net') == net:
                for key in ('Z0_avg', 'z0_ohm', 'Z0', 'z0'):
                    z = r.get(key)
                    if isinstance(z, (int, float)):
                        return float(z)
    return None


def collect_sim_results(sim_output_root: str | Path,
                          net_of_task: Dict[str, str]
                          ) -> Dict[str, Dict[str, Any]]:
    """Walk sim_output_root/<task_id>/*.s2p; return per-net simulation data.
    net_of_task maps task_id → net_name (from em-dispatch output)."""
    root = Path(sim_output_root)
    if not root.exists():
        return {}
    out: Dict[str, Dict[str, Any]] = {}
    for task_id, net in net_of_task.items():
        task_dir = root / task_id
        if not task_dir.exists():
            continue
        s2p_files = list(task_dir.glob('*.s2p'))
        if not s2p_files:
            continue
        # Prefer non-empty; take first match.
        for f in s2p_files:
            parsed = parse_touchstone_s2p(f)
            if not parsed:
                continue
            z0_s11 = z0_from_s11(parsed['s11'][0], parsed['z0_ref'])
            z0_summary = _z0_from_solver_summary(task_dir, net)
            # Phase I-4: solver summary Z0가 있으면 우선 (field-solve
            # ground truth); 없으면 S11 역산 (short-line 한계 명시).
            z0 = z0_summary if z0_summary is not None else z0_s11
            loss_db = insertion_loss_db(parsed['s21'])
            # Phase J-1 — full-band S21 loss curve.
            il_curve = insertion_loss_curve(parsed['freqs_hz'], parsed['s21'])
            out[net] = {
                'source_file':          str(f),
                'freq_points':          len(parsed['freqs_hz']),
                'simulated_z0_ohm':     round(z0, 3) if z0 else None,
                'z0_source':            ('solver_summary'
                                          if z0_summary is not None
                                          else 's11_inversion'),
                'z0_s11_ohm':           round(z0_s11, 3) if z0_s11 else None,
                'insertion_loss_db':    round(loss_db, 4) if loss_db else None,
                'insertion_loss_curve': il_curve,
                'z0_ref':               parsed['z0_ref'],
            }
            break
    return out


def summarize_sim_aggregation(eval_result: Dict[str, Any],
                                 sim_output_root: str | Path,
                                 tasks_dir: Optional[str | Path] = None
                                 ) -> Dict[str, Any]:
    """Fold sim results into an eval-style block. Also correlates against
    analytical Wadell Z0 to compute delta_pct per net."""
    metrics = eval_result.get('metrics') or {}
    analytical_z0 = ((metrics.get('si') or {}).get('Z0_single_ended_ohm')
                        or {})
    # Build task_id → net map from em-dispatch tasks directory
    net_of_task: Dict[str, str] = {}
    if tasks_dir is not None:
        import json as _json
        for tf in Path(tasks_dir).glob('*.json'):
            try:
                t = _json.loads(tf.read_text())
                if t.get('net') and t.get('task_id'):
                    net_of_task[t['task_id']] = t['net']
            except Exception:
                continue
    per_net = collect_sim_results(sim_output_root, net_of_task)
    correlations = []
    for net, sim_entry in per_net.items():
        an_z = analytical_z0.get(net)
        sim_z = sim_entry.get('simulated_z0_ohm')
        if an_z is not None and sim_z is not None and an_z > 0:
            delta_pct = (sim_z - an_z) / an_z * 100.0
            correlations.append({
                'net':               net,
                'analytical_z0_ohm': an_z,
                'simulated_z0_ohm':  sim_z,
                'delta_pct':         round(delta_pct, 3),
            })
    correlations.sort(key=lambda x: -abs(x['delta_pct']))
    return {
        'sim_results':          per_net,
        'net_count':            len(per_net),
        'correlations':         correlations,
        'biggest_delta':        correlations[:5],
    }
