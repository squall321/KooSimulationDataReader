# bga_router 기능을 MCP tool로 노출하는 stdio JSON-RPC 서버 (의존성 zero)
"""Phase H-5 — bga_router MCP server.

Exposes the routing-eval pipeline as MCP tools so Claude (or any MCP
client) can drive the whole flow without shelling out:

  Tools:
    register_dataset(tgz_or_dir, name)      — tgz 자동 해제 + registry 등록
    route(dataset, bga, recipe, budget_s)   — eval 실행 → result JSON
    get_metrics(eval_path, section)         — 결과 JSON에서 특정 영역 추출
    em_dispatch(eval_path, out_dir)         — em_queue → task JSONs
    net_diff(eval_a, eval_b)                — 두 recipe 비교 markdown
    spice_export(eval_path, out_path)       — SPICE .lib 생성

Implements MCP over stdio using plain JSON-RPC 2.0 — no external SDK
required. Register in claude mcp config as:

    {
      "mcpServers": {
        "bga-router": {
          "command": "python",
          "args": ["-m", "bga_router.mcp_server"],
          "cwd": "<repo>/layout/bga_escape"
        }
      }
    }
"""

from __future__ import annotations

import json
import sys
import tarfile
import tempfile
import traceback
from pathlib import Path
from typing import Any, Dict, List, Optional


PROTOCOL_VERSION = '2024-11-05'
SERVER_NAME = 'bga-router'
SERVER_VERSION = '1.0.0'


# ---------------------------------------------------------------------------
# Tool implementations
# ---------------------------------------------------------------------------


def tool_register_dataset(arguments: Dict[str, Any]) -> Dict[str, Any]:
    """tgz path or dir path → registry entry."""
    src = arguments.get('path')
    name = arguments.get('name')
    if not src or not name:
        raise ValueError('register_dataset requires path + name')
    p = Path(src)
    if not p.exists():
        raise FileNotFoundError(f'{p} not found')
    # Auto-extract tgz / tar.gz
    if p.suffix in ('.tgz', '.gz') or p.name.endswith('.tar.gz'):
        extract_dir = Path(tempfile.mkdtemp(prefix='bga_mcp_odb_'))
        with tarfile.open(p) as tf:
            tf.extractall(extract_dir, filter='data')
        # If the archive has a single top dir, descend into it
        entries = list(extract_dir.iterdir())
        odb_root = entries[0] if len(entries) == 1 and entries[0].is_dir() \
            else extract_dir
    else:
        odb_root = p
    from . import registry as _registry
    entry = _registry.register_dataset(
        odb_path=str(odb_root), name=name, dataset_type='dir',
        step=arguments.get('step', 'mentor'),
        overwrite=bool(arguments.get('overwrite', True)))
    return {
        'name':       name,
        'odb_root':   str(odb_root),
        'known_bgas': [
            {'name': b.get('name'), 'ref_des': b.get('ref_des'),
              'pins': b.get('pin_count'), 'pitch_mm': b.get('pitch_mm')}
            for b in entry.get('known_bgas', [])
        ],
    }


def tool_route(arguments: Dict[str, Any]) -> Dict[str, Any]:
    from . import eval as _eval
    from .cli import _resolve_bga
    dataset = arguments.get('dataset')
    bga = arguments.get('bga')
    if not dataset or not bga:
        raise ValueError('route requires dataset + bga')
    # Resolve BGA substring shorthand ('bga218f' → full pkg name),
    # matching CLI behaviour so MCP callers can use short names too.
    bga = _resolve_bga(dataset, bga)
    recipe = arguments.get('recipe', 'default')
    budget = int(arguments.get('budget_s', 60))
    output = arguments.get('output')
    result = _eval.run_route(dataset, bga, recipe, budget_s=budget)
    if output:
        Path(output).write_text(json.dumps(result, indent=2, default=str))
    # Return a compact summary (full JSON too large for tool result)
    m = result.get('metrics') or {}
    return {
        'output_path':   output,
        'routed':        m.get('routed'),
        'total':         m.get('total'),
        'routed_ratio':  m.get('routed_ratio'),
        'elapsed_s':     m.get('elapsed_s'),
        'error':         result.get('error'),
        'sections':      sorted(k for k, v in m.items()
                                  if isinstance(v, dict)),
        'rule_violations': (m.get('rule_check') or {}).get(
            'violations_expanded'),
        'em_queue_count': (m.get('em_queue') or {}).get('count'),
    }


def tool_get_metrics(arguments: Dict[str, Any]) -> Dict[str, Any]:
    path = arguments.get('eval_path')
    if not path:
        raise ValueError('get_metrics requires eval_path')
    data = json.loads(Path(path).read_text())
    m = data.get('metrics') or {}
    section = arguments.get('section')
    if section:
        return {section: m.get(section)}
    # Top-level summary if no section requested
    return {
        'sections': sorted(m.keys()),
        'routed': m.get('routed'), 'total': m.get('total'),
    }


def tool_em_dispatch(arguments: Dict[str, Any]) -> Dict[str, Any]:
    from .integrations.em_queue_dispatch import dispatch, summarize_dispatch
    path = arguments.get('eval_path')
    out = arguments.get('out_dir')
    if not path or not out:
        raise ValueError('em_dispatch requires eval_path + out_dir')
    written = dispatch(path, out)
    return summarize_dispatch(written)


def tool_net_diff(arguments: Dict[str, Any]) -> Dict[str, Any]:
    from .metrics.net_diff import compare_results, render_markdown
    a_path = arguments.get('eval_a')
    b_path = arguments.get('eval_b')
    if not a_path or not b_path:
        raise ValueError('net_diff requires eval_a + eval_b')
    a = json.loads(Path(a_path).read_text())
    b = json.loads(Path(b_path).read_text())
    diff = compare_results(a, b)
    return {
        'markdown':  render_markdown(diff),
        'summary':   diff['summary'],
    }


def tool_spice_export(arguments: Dict[str, Any]) -> Dict[str, Any]:
    from .integrations.spice_export import write_spice_lib
    path = arguments.get('eval_path')
    out = arguments.get('out_path')
    if not path or not out:
        raise ValueError('spice_export requires eval_path + out_path')
    p = write_spice_lib(path, out)
    return {'lib_path': str(p), 'size_bytes': p.stat().st_size}


def tool_odb_inspect(arguments: Dict[str, Any]) -> Dict[str, Any]:
    """ODB++ 구조 직독 요약 (재라우팅 없이). odb_to_json 자동 실행."""
    from .integrations.odb_inspect import inspect_odb
    from .integrations.odb_json_builder import build_odb_json_if_missing
    json_dir = arguments.get('json_dir')
    odb_dir = arguments.get('odb_dir')
    if not json_dir and not odb_dir:
        raise ValueError('odb_inspect requires json_dir or odb_dir')
    if not json_dir:
        # derive a json dir next to the odb and build it
        json_dir = str(Path(odb_dir).parent / (Path(odb_dir).name + '_json'))
    if odb_dir:
        build_odb_json_if_missing(json_dir, odb_dir=odb_dir)
    return inspect_odb(json_dir)


def tool_odb_analyze(arguments: Dict[str, Any]) -> Dict[str, Any]:
    """ODB++ 기존 배선(net 폴리곤) 직독 분석. odb_to_em_json 자동 실행."""
    from .metrics.odb_traces import load_em_data, summarize_traces
    from .integrations.em_data_builder import build_em_data_if_missing
    em_data_path = arguments.get('em_data')
    odb_dir = arguments.get('odb_dir')
    stackup_yaml = arguments.get('stackup_yaml')
    if not em_data_path and not odb_dir:
        raise ValueError('odb_analyze requires em_data or odb_dir')
    if not em_data_path:
        em_data_path = str(Path(odb_dir).parent /
                            (Path(odb_dir).name + '_em.json'))
    if odb_dir:
        if not stackup_yaml:
            raise ValueError('odb_analyze with odb_dir requires stackup_yaml '
                             'for em_data extraction')
        build_em_data_if_missing(em_data_path, odb_dir=odb_dir,
                                   stackup_yaml=stackup_yaml)
    em = load_em_data(em_data_path)
    return summarize_traces(em, max_nets=int(arguments.get('max_nets', 0)) or None)


def tool_si_report(arguments: Dict[str, Any]) -> Dict[str, Any]:
    from .integrations.si_report import write_si_report
    path = arguments.get('eval_path')
    out = arguments.get('out_path')
    if not path or not out:
        raise ValueError('si_report requires eval_path + out_path')
    p = write_si_report(path, out)
    return {'html_path': str(p), 'size_bytes': p.stat().st_size,
             'hint': 'S21 insertion-loss 곡선/net. sim-agg 먼저 실행 필요.'}


def tool_pdn(arguments: Dict[str, Any]) -> Dict[str, Any]:
    from .integrations.pdn_runner import summarize_pdn
    eval_path = arguments.get('eval_path')
    if not eval_path:
        raise ValueError('pdn requires eval_path')
    data = json.loads(Path(eval_path).read_text())
    return summarize_pdn(
        data,
        em_data_json=arguments.get('em_data'),
        output_dir=arguments.get('output_dir', 'pdn_out'),
        dry_run=bool(arguments.get('dry_run', True)))


def tool_route_viewer(arguments: Dict[str, Any]) -> Dict[str, Any]:
    from .integrations.route_viewer import write_route_viewer
    path = arguments.get('eval_path')
    out = arguments.get('out_path')
    if not path or not out:
        raise ValueError('route_viewer requires eval_path + out_path')
    p = write_route_viewer(path, out)
    return {'html_path': str(p), 'size_bytes': p.stat().st_size,
             'hint': 'Canvas viewer — 레이어 토글 / net highlight / '
                     'coupling pair / zoom-pan. file:// 안전.'}


def tool_dashboard(arguments: Dict[str, Any]) -> Dict[str, Any]:
    from .integrations.dashboard import write_dashboard
    path = arguments.get('eval_path')
    out = arguments.get('out_path')
    if not path or not out:
        raise ValueError('dashboard requires eval_path + out_path')
    p = write_dashboard(path, out)
    return {'html_path': str(p), 'size_bytes': p.stat().st_size,
             'hint': 'file:// 로 브라우저에서 바로 열림 (inline data)'}


def tool_sim_agg(arguments: Dict[str, Any]) -> Dict[str, Any]:
    from .integrations.sim_aggregator import summarize_sim_aggregation
    eval_path = arguments.get('eval_path')
    sim_dir = arguments.get('sim_dir')
    if not eval_path or not sim_dir:
        raise ValueError('sim_agg requires eval_path + sim_dir')
    data = json.loads(Path(eval_path).read_text())
    agg = summarize_sim_aggregation(data, sim_dir,
                                       tasks_dir=arguments.get('tasks_dir'))
    out_path = arguments.get('out_path') or eval_path
    data.setdefault('metrics', {}).setdefault('si', {})
    data['metrics']['si']['simulated'] = agg
    Path(out_path).write_text(json.dumps(data, indent=2, default=str))
    return {
        'merged_path':    str(out_path),
        'net_count':      agg['net_count'],
        'biggest_delta':  agg['biggest_delta'],
    }


def tool_em_run(arguments: Dict[str, Any]) -> Dict[str, Any]:
    solver = arguments.get('solver', 'sol_d')
    if solver == 'sol_b':
        from .integrations.sol_b_runner import dispatch_run
    else:
        from .integrations.sol_d_runner import dispatch_run
    from .integrations.sol_d_runner import summarize_run
    tasks_dir = arguments.get('tasks_dir')
    if not tasks_dir:
        raise ValueError('em_run requires tasks_dir')
    results = dispatch_run(
        tasks_dir,
        em_data_json=arguments.get('em_data'),
        output_dir=arguments.get('output_dir'),
        dry_run=bool(arguments.get('dry_run', True)),
        timeout_s=int(arguments.get('timeout_s', 600)),
    )
    summary = summarize_run(results)
    summary['solver'] = solver
    summary['skipped_reasons'] = sorted(
        {r.skip_reason for r in results if r.skipped and r.skip_reason})
    return summary


def tool_xtalk_sim(arguments: Dict[str, Any]) -> Dict[str, Any]:
    from .integrations.ngspice_runner import (ngspice_available,
                                                  run_crosstalk_batch)
    eval_path = arguments.get('eval_path')
    lib = arguments.get('lib_path')
    workdir = arguments.get('workdir')
    if not eval_path or not lib or not workdir:
        raise ValueError('xtalk_sim requires eval_path + lib_path + workdir')
    data = json.loads(Path(eval_path).read_text())
    out = run_crosstalk_batch(data, lib, workdir=workdir,
                                top_k=int(arguments.get('top_k', 5)))
    out['ngspice_available'] = ngspice_available()
    # RunResult-free serializable form
    out['results'] = [
        {k: v for k, v in r.items()} if isinstance(r, dict) else r
        for r in out['results']]
    return out


_TOOLS = {
    'register_dataset': {
        'fn': tool_register_dataset,
        'description': 'Register an ODB++ dataset (dir or .tgz auto-extracted) '
                        'into the bga-router registry. Returns discovered BGAs.',
        'inputSchema': {
            'type': 'object',
            'properties': {
                'path':      {'type': 'string', 'description': 'ODB++ dir or .tgz path'},
                'name':      {'type': 'string', 'description': 'Registry slug'},
                'step':      {'type': 'string', 'default': 'mentor'},
                'overwrite': {'type': 'boolean', 'default': True},
            },
            'required': ['path', 'name'],
        },
    },
    'route': {
        'fn': tool_route,
        'description': 'Run BGA escape routing + full PCB evaluation '
                        '(geometry/rule_check/si/standard/coupling/...). '
                        'Returns compact summary; write full JSON via output.',
        'inputSchema': {
            'type': 'object',
            'properties': {
                'dataset':  {'type': 'string'},
                'bga':      {'type': 'string'},
                'recipe':   {'type': 'string', 'default': 'default'},
                'budget_s': {'type': 'integer', 'default': 60},
                'output':   {'type': 'string',
                              'description': 'Path to write full result JSON'},
            },
            'required': ['dataset', 'bga'],
        },
    },
    'get_metrics': {
        'fn': tool_get_metrics,
        'description': 'Read a section (geometry / rule_check / si / standard '
                        '/ coupling / bus_groups / net_clusters / dependencies '
                        '/ em_queue / hdi_drc) from a route result JSON.',
        'inputSchema': {
            'type': 'object',
            'properties': {
                'eval_path': {'type': 'string'},
                'section':   {'type': 'string'},
            },
            'required': ['eval_path'],
        },
    },
    'em_dispatch': {
        'fn': tool_em_dispatch,
        'description': 'Convert em_queue entries in a result JSON to per-net '
                        'solver task files (sol_d / sol_b).',
        'inputSchema': {
            'type': 'object',
            'properties': {
                'eval_path': {'type': 'string'},
                'out_dir':   {'type': 'string'},
            },
            'required': ['eval_path', 'out_dir'],
        },
    },
    'net_diff': {
        'fn': tool_net_diff,
        'description': 'Per-net comparison of two route results '
                        '(Z0 / DC-R / via-stub deltas).',
        'inputSchema': {
            'type': 'object',
            'properties': {
                'eval_a': {'type': 'string'},
                'eval_b': {'type': 'string'},
            },
            'required': ['eval_a', 'eval_b'],
        },
    },
    'spice_export': {
        'fn': tool_spice_export,
        'description': 'Export a SPICE .lib (lumped RLC + K coupling) from a '
                        'route result JSON.',
        'inputSchema': {
            'type': 'object',
            'properties': {
                'eval_path': {'type': 'string'},
                'out_path':  {'type': 'string'},
            },
            'required': ['eval_path', 'out_path'],
        },
    },
    'odb_inspect': {
        'fn': tool_odb_inspect,
        'description': 'ODB++ 구조 직독 요약 (재라우팅 없이) — 레이어/타입/'
                        'feature 통계 + 심볼(드릴/패드 사이즈) 요약. odb_dir '
                        '주면 odb_to_json 자동 실행, json_dir 주면 그대로 읽음.',
        'inputSchema': {
            'type': 'object',
            'properties': {
                'json_dir': {'type': 'string',
                              'description': 'odb_to_json 출력 디렉터리'},
                'odb_dir':  {'type': 'string',
                              'description': 'ODB++ 디렉터리 (자동 추출)'},
            },
        },
    },
    'odb_analyze': {
        'fn': tool_odb_analyze,
        'description': 'ODB++ 기존 배선(net 폴리곤) 직독 분석 — net별 레이어 '
                        'span / via 유무 / bbox / 근사 길이 + signal/plane 분류. '
                        'odb_dir+stackup_yaml 주면 odb_to_em_json 자동 실행.',
        'inputSchema': {
            'type': 'object',
            'properties': {
                'em_data':      {'type': 'string',
                                  'description': 'em_data.json 경로'},
                'odb_dir':      {'type': 'string'},
                'stackup_yaml': {'type': 'string'},
                'max_nets':     {'type': 'integer',
                                  'description': '분석 net 상한 (0=전부)'},
            },
        },
    },
    'si_report': {
        'fn': tool_si_report,
        'description': 'Frequency-domain SI report — per-net S21 insertion '
                        'loss curves (SVG sparklines) + IL@1/5GHz + f3dB. '
                        'Requires metrics.si.simulated (run sim_agg first).',
        'inputSchema': {
            'type': 'object',
            'properties': {
                'eval_path': {'type': 'string'},
                'out_path':  {'type': 'string'},
            },
            'required': ['eval_path', 'out_path'],
        },
    },
    'pdn': {
        'fn': tool_pdn,
        'description': 'DC IR-drop analysis for power/ground nets via the '
                        'pdn_dc 2D solver. dry_run=true (default) lists PG '
                        'nets + commands without executing.',
        'inputSchema': {
            'type': 'object',
            'properties': {
                'eval_path':  {'type': 'string'},
                'em_data':    {'type': 'string'},
                'output_dir': {'type': 'string', 'default': 'pdn_out'},
                'dry_run':    {'type': 'boolean', 'default': True},
            },
            'required': ['eval_path'],
        },
    },
    'route_viewer': {
        'fn': tool_route_viewer,
        'description': 'Interactive Canvas viewer of routed paths — layer '
                        'toggle / net highlight / violation marks / coupling '
                        'pair select / zoom-pan. Single HTML, file:// safe.',
        'inputSchema': {
            'type': 'object',
            'properties': {
                'eval_path': {'type': 'string'},
                'out_path':  {'type': 'string'},
            },
            'required': ['eval_path', 'out_path'],
        },
    },
    'dashboard': {
        'fn': tool_dashboard,
        'description': 'Render a route result JSON to a standalone HTML '
                        'dashboard (inline data, opens from file://).',
        'inputSchema': {
            'type': 'object',
            'properties': {
                'eval_path': {'type': 'string'},
                'out_path':  {'type': 'string'},
            },
            'required': ['eval_path', 'out_path'],
        },
    },
    'sim_agg': {
        'fn': tool_sim_agg,
        'description': 'Fold sol_d/sol_b Touchstone results back into the '
                        'eval JSON; computes analytical-vs-simulated Z0 '
                        'delta per net.',
        'inputSchema': {
            'type': 'object',
            'properties': {
                'eval_path': {'type': 'string'},
                'sim_dir':   {'type': 'string'},
                'tasks_dir': {'type': 'string'},
                'out_path':  {'type': 'string',
                               'description': 'defaults to eval_path (in-place)'},
            },
            'required': ['eval_path', 'sim_dir'],
        },
    },
    'em_run': {
        'fn': tool_em_run,
        'description': 'Dispatch em-dispatch task files to sol_d (2D-FD) or '
                        'sol_b (3D PEEC). dry_run=true (default) builds '
                        'commands without executing.',
        'inputSchema': {
            'type': 'object',
            'properties': {
                'tasks_dir':  {'type': 'string'},
                'solver':     {'type': 'string', 'enum': ['sol_d', 'sol_b'],
                                'default': 'sol_d'},
                'em_data':    {'type': 'string'},
                'output_dir': {'type': 'string'},
                'dry_run':    {'type': 'boolean', 'default': True},
                'timeout_s':  {'type': 'integer', 'default': 600},
            },
            'required': ['tasks_dir'],
        },
    },
    'xtalk_sim': {
        'fn': tool_xtalk_sim,
        'description': 'Generate + run ngspice crosstalk testbenches for the '
                        'top coupling pairs. Without ngspice installed, '
                        'netlists are written and marked skipped.',
        'inputSchema': {
            'type': 'object',
            'properties': {
                'eval_path': {'type': 'string'},
                'lib_path':  {'type': 'string'},
                'workdir':   {'type': 'string'},
                'top_k':     {'type': 'integer', 'default': 5},
            },
            'required': ['eval_path', 'lib_path', 'workdir'],
        },
    },
}


# ---------------------------------------------------------------------------
# JSON-RPC over stdio
# ---------------------------------------------------------------------------


def _handle(request: Dict[str, Any]) -> Optional[Dict[str, Any]]:
    """Process one JSON-RPC request; return response dict or None for
    notifications."""
    method = request.get('method', '')
    req_id = request.get('id')
    params = request.get('params') or {}

    def _ok(result):
        return {'jsonrpc': '2.0', 'id': req_id, 'result': result}

    def _err(code, message):
        return {'jsonrpc': '2.0', 'id': req_id,
                 'error': {'code': code, 'message': message}}

    if method == 'initialize':
        return _ok({
            'protocolVersion': PROTOCOL_VERSION,
            'capabilities': {'tools': {}},
            'serverInfo': {'name': SERVER_NAME, 'version': SERVER_VERSION},
        })
    if method == 'notifications/initialized':
        return None
    if method == 'tools/list':
        return _ok({'tools': [
            {'name': name,
              'description': spec['description'],
              'inputSchema': spec['inputSchema']}
            for name, spec in _TOOLS.items()
        ]})
    if method == 'tools/call':
        tool_name = params.get('name')
        arguments = params.get('arguments') or {}
        spec = _TOOLS.get(tool_name)
        if spec is None:
            return _err(-32601, f'unknown tool: {tool_name}')
        try:
            result = spec['fn'](arguments)
            return _ok({'content': [
                {'type': 'text',
                  'text': json.dumps(result, indent=2, default=str)}
            ]})
        except Exception as e:
            return _ok({'content': [
                {'type': 'text',
                  'text': f'ERROR {type(e).__name__}: {e}\n'
                          f'{traceback.format_exc()[-800:]}'}
            ], 'isError': True})
    if req_id is not None:
        return _err(-32601, f'unknown method: {method}')
    return None


def main() -> int:
    """stdio loop — one JSON-RPC message per line."""
    for raw in sys.stdin:
        line = raw.strip()
        if not line:
            continue
        try:
            request = json.loads(line)
        except json.JSONDecodeError:
            continue
        response = _handle(request)
        if response is not None:
            sys.stdout.write(json.dumps(response) + '\n')
            sys.stdout.flush()
    return 0


if __name__ == '__main__':
    sys.exit(main())
