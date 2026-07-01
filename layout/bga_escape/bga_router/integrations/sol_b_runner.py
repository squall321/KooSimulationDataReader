# em-dispatch task JSON을 sol_b (3D PEEC solver) 호출로 변환하는 runner
"""Phase G-1 — sol_b PEEC auto-receiver.

Same shape as sol_d_runner (Phase E-4) but points at
``simulation/sol_b_peec/run.py``. em_queue entries with
suggested_solver='sol_b' route here; entries defaulting to 'sol_d'
still go to sol_d_runner.

Dry-run vs exec mode identical to sol_d_runner. Kept as a sibling
module (not shared) so per-solver argument conventions can diverge
later without ABI churn.
"""

from __future__ import annotations

import json
import shlex
import subprocess
from pathlib import Path
from typing import Any, Dict, Iterable, List, Optional

from .sol_d_runner import RunResult, _tail


REPO_ROOT = Path(__file__).resolve().parents[4]
DEFAULT_SOL_B_DIR = (REPO_ROOT / 'simulation' / 'sol_b_peec')


def build_command(task: Dict[str, Any], *,
                    sol_b_dir: Path = DEFAULT_SOL_B_DIR,
                    em_data_json: Optional[Path] = None,
                    output_dir: Optional[Path] = None,
                    extra_args: Optional[List[str]] = None) -> List[str]:
    """Construct the sol_b run.py invocation for one em-dispatch task."""
    py = 'python'
    run_py = sol_b_dir / 'run.py'
    cmd: List[str] = [py, str(run_py)]
    if em_data_json is not None:
        cmd += ['--input', str(em_data_json)]
    net = task.get('net') or ''
    if net:
        cmd += ['--net', net]
    if output_dir is not None:
        cmd += ['--output', str(output_dir / (task.get('task_id') or net))]
    if extra_args:
        cmd += list(extra_args)
    return cmd


def dispatch_run(tasks_dir: str | Path, *,
                   sol_b_dir: str | Path = DEFAULT_SOL_B_DIR,
                   em_data_json: Optional[str | Path] = None,
                   output_dir: Optional[str | Path] = None,
                   dry_run: bool = True,
                   extra_args: Optional[List[str]] = None,
                   timeout_s: int = 1800) -> List[RunResult]:
    """Walk em-dispatch task files; invoke sol_b per task.

    Only tasks whose ``suggested_solver`` is 'sol_b' are processed;
    'sol_d' tasks are left for sol_d_runner. Longer default timeout
    (1800s) matches PEEC's higher runtime.
    """
    tdir = Path(tasks_dir)
    sdir = Path(sol_b_dir)
    odir = Path(output_dir) if output_dir else None
    em_data = Path(em_data_json) if em_data_json else None

    if not tdir.exists():
        raise FileNotFoundError(f'tasks_dir not found: {tdir}')
    results: List[RunResult] = []
    for tf in sorted(tdir.glob('*.json')):
        try:
            task = json.loads(tf.read_text())
        except Exception as e:
            results.append(RunResult(
                task_id=tf.stem, net='', cmd=[], skipped=True,
                skip_reason=f'cannot parse: {type(e).__name__}: {e}'))
            continue
        if (task.get('suggested_solver') or 'sol_d') != 'sol_b':
            results.append(RunResult(
                task_id=task.get('task_id', tf.stem),
                net=task.get('net', ''), cmd=[],
                skipped=True,
                skip_reason='suggested_solver != sol_b'))
            continue
        cmd = build_command(task, sol_b_dir=sdir,
                              em_data_json=em_data,
                              output_dir=odir, extra_args=extra_args)
        rr = RunResult(task_id=task.get('task_id', tf.stem),
                        net=task.get('net', ''), cmd=cmd)
        if dry_run:
            results.append(rr)
            continue
        if em_data is None or not em_data.exists():
            rr.skipped = True
            rr.skip_reason = ('em_data_json missing — cannot invoke '
                              'sol_b without input JSON')
            results.append(rr)
            continue
        try:
            proc = subprocess.run(cmd, capture_output=True, text=True,
                                    timeout=timeout_s, cwd=str(sdir))
            rr.rc = proc.returncode
            rr.stdout_tail = _tail(proc.stdout)
            rr.stderr_tail = _tail(proc.stderr)
        except subprocess.TimeoutExpired:
            rr.rc = -1
            rr.skip_reason = f'timeout after {timeout_s}s'
        except FileNotFoundError as e:
            rr.skipped = True
            rr.skip_reason = f'sol_b entry not found: {e}'
        results.append(rr)
    return results
