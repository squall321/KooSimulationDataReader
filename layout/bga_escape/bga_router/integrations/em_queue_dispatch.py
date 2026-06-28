# em_queue 항목을 sol_d / sol_b 솔버 입력 task JSON으로 변환하는 어댑터
"""Phase D-5 — EM queue downstream dispatch.

Take the ``em_queue`` block produced by ``bga_router/eval.py`` and emit
one task JSON per net for ``simulation/sol_d_cross_section`` (2D-FD) or
``simulation/sol_b_peec`` (3D PEEC). The solver packages themselves
stay untouched — this adapter is the bridge between routing eval and
SI simulation, replacing the manual hand-off the user had before.

Task JSON shape (sol_d-compatible — see
``simulation/sol_d_cross_section/src/json_loader.py``):

    {
        "task_id":        "<dataset>__<bga>__<recipe>__<net>",
        "source":         "bga_router.em_queue",
        "reason":         "tight_coupling+impedance_miss",
        "net":            "DDR4_DQ0_P",
        "layer":          "LAY2",
        "layers":         ["LAY2"],
        "bbox_mm":        [x_min, y_min, x_max, y_max],
        "rule":           {... NetRule snapshot ...},
        "stackup_yaml":   "<absolute path or empty if default>",
        "suggested_solver": "sol_d"
    }
"""

from __future__ import annotations

import json
from pathlib import Path
from typing import Any, Dict, Iterable, List, Optional


def build_tasks_from_result(result: Dict[str, Any]) -> List[Dict[str, Any]]:
    """Take a single ``run_route`` result JSON and return the per-net
    sim-task dicts for everything in metrics.em_queue.entries."""
    metrics = result.get('metrics') or {}
    em = metrics.get('em_queue') or {}
    entries = em.get('entries') or []
    dataset = result.get('dataset') or ''
    bga     = result.get('bga') or ''
    recipe  = result.get('recipe') or ''
    out: List[Dict[str, Any]] = []
    for e in entries:
        net = e.get('net')
        if not net:
            continue
        task = {
            'task_id':          f'{dataset}__{bga}__{recipe}__{net}',
            'source':           'bga_router.em_queue',
            'reason':           e.get('reason'),
            'reasons':          list(e.get('reasons') or []),
            'net':              net,
            'layer':            e.get('layer'),
            'layers':           list(e.get('layers') or []),
            'bbox_mm':          list(e.get('bbox_mm') or []),
            'rule':             dict(e.get('rule') or {}),
            'suggested_solver': e.get('suggested_solver') or 'sol_d',
            'origin': {
                'dataset': dataset, 'bga': bga, 'recipe': recipe,
            },
        }
        out.append(task)
    return out


def dispatch(result_path: str | Path, out_dir: str | Path,
              *, format: str = 'json') -> List[Path]:
    """Read an eval result JSON; write one task file per em_queue entry.
    Returns the list of written file paths. Creates ``out_dir`` if absent.
    """
    rp = Path(result_path)
    od = Path(out_dir)
    od.mkdir(parents=True, exist_ok=True)
    data = json.loads(rp.read_text())
    tasks = build_tasks_from_result(data)
    written: List[Path] = []
    for t in tasks:
        fname = f"{t['task_id']}.{format}"
        # sanitize: keep only safe characters
        fname = ''.join(c if (c.isalnum() or c in '._-') else '_' for c in fname)
        out_file = od / fname
        if format == 'json':
            out_file.write_text(json.dumps(t, indent=2, default=str))
        else:
            raise ValueError(f'unsupported format: {format!r}')
        written.append(out_file)
    return written


def dispatch_many(result_paths: Iterable[str | Path], out_dir: str | Path,
                    *, format: str = 'json') -> List[Path]:
    """Same as ``dispatch`` over many eval result files."""
    out_all: List[Path] = []
    for rp in result_paths:
        out_all.extend(dispatch(rp, out_dir, format=format))
    return out_all


def summarize_dispatch(written: List[Path]) -> Dict[str, Any]:
    """One-line summary suitable for CLI feedback."""
    return {
        'count':       len(written),
        'output_dir':  str(written[0].parent) if written else None,
        'sample':      [str(p.name) for p in written[:3]],
    }
