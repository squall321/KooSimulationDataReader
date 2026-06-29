# em-dispatch task JSON을 sol_d (2D cross-section solver) 호출로 변환하는 runner
"""Phase E-4 — sol_d auto-receiver.

Pipeline contract:
  em-dispatch (Phase D-5) → per-task JSON in em_tasks/
  E-4 (this module)       → reads each task, invokes
                            simulation/sol_d_cross_section/run.py with
                            matching --net + --output options.

The sol_d package itself stays untouched — this is a wrapper that
fans out one ``run.py --net <name>`` invocation per em_queue entry.
Dry-run mode prints the commands without executing.
"""

from __future__ import annotations

import json
import shlex
import subprocess
from dataclasses import dataclass, field
from pathlib import Path
from typing import Any, Dict, Iterable, List, Optional


REPO_ROOT = Path(__file__).resolve().parents[4]
DEFAULT_SOL_D_DIR = (REPO_ROOT / 'simulation' / 'sol_d_cross_section')


@dataclass
class RunResult:
    task_id: str
    net: str
    cmd: List[str]
    rc: Optional[int] = None
    stdout_tail: str = ''
    stderr_tail: str = ''
    skipped: bool = False
    skip_reason: str = ''


def _tail(s: str, n: int = 20) -> str:
    if not s:
        return ''
    lines = s.splitlines()
    return '\n'.join(lines[-n:])


def build_command(task: Dict[str, Any], *,
                    sol_d_dir: Path = DEFAULT_SOL_D_DIR,
                    em_data_json: Optional[Path] = None,
                    output_dir: Optional[Path] = None,
                    extra_args: Optional[List[str]] = None) -> List[str]:
    """Construct the sol_d run.py invocation for one em-dispatch task."""
    py = 'python'
    run_py = sol_d_dir / 'run.py'
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
                   sol_d_dir: str | Path = DEFAULT_SOL_D_DIR,
                   em_data_json: Optional[str | Path] = None,
                   output_dir: Optional[str | Path] = None,
                   dry_run: bool = True,
                   extra_args: Optional[List[str]] = None,
                   timeout_s: int = 600) -> List[RunResult]:
    """Walk em-dispatch task files, optionally invoke sol_d per task.

    dry_run=True (default): build commands, do NOT execute. RunResult.cmd
    is populated, rc stays None.
    dry_run=False: subprocess.run each; rc + stdout/stderr tails captured.
    """
    tdir = Path(tasks_dir)
    sdir = Path(sol_d_dir)
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
        cmd = build_command(task, sol_d_dir=sdir,
                              em_data_json=em_data, output_dir=odir,
                              extra_args=extra_args)
        rr = RunResult(task_id=task.get('task_id', tf.stem),
                        net=task.get('net', ''),
                        cmd=cmd)
        if dry_run:
            results.append(rr)
            continue
        if em_data is None or not em_data.exists():
            rr.skipped = True
            rr.skip_reason = ('em_data_json missing — cannot invoke '
                              'sol_d without input JSON')
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
            rr.skip_reason = f'sol_d entry not found: {e}'
        results.append(rr)
    return results


def summarize_run(results: List[RunResult]) -> Dict[str, Any]:
    return {
        'total':     len(results),
        'executed':  sum(1 for r in results if r.rc is not None),
        'skipped':   sum(1 for r in results if r.skipped),
        'ok':        sum(1 for r in results if r.rc == 0),
        'failed':    sum(1 for r in results if r.rc is not None and r.rc != 0),
        'sample_cmd': (' '.join(shlex.quote(c) for c in results[0].cmd)
                        if results else ''),
    }
