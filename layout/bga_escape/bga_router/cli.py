"""argparse CLI for the Phase 41 eval framework.

Subcommands:
    route       run one (dataset, bga, recipe) -> JSON file or stdout
    eval        sweep recipes for one (dataset, bga) -> JSON files
    aggregate   walk a results dir -> markdown summary
    register    add a new ODB++ dataset to datasets/registry.yaml

Examples
--------

    python -m bga_router.cli register \\
        --odb /data/.../P3_EUR_REV03Sample \\
        --name P3_EUR_REV03_test

    python -m bga_router.cli eval \\
        --dataset P3_EUR_REV03_test --bga bga218f \\
        --recipes baseline,phase32 --budget 60
"""
from __future__ import annotations

import argparse
import json
import sys
import time
from pathlib import Path

from . import aggregate as _aggregate
from . import eval as _eval
from . import registry as _registry


# ---------------------------------------------------------------------------
# Subcommands
# ---------------------------------------------------------------------------


def cmd_register(args: argparse.Namespace) -> int:
    entry = _registry.register_dataset(
        odb_path=args.odb,
        name=args.name,
        dataset_type=args.type,
        step=args.step,
        overwrite=args.overwrite,
    )
    n_top = sum(1 for b in entry.get('known_bgas', [])
                if (b.get('side') or '').upper() == 'TOP')
    n_bot = sum(1 for b in entry.get('known_bgas', [])
                if (b.get('side') or '').upper() == 'BOT')
    print(f'[register] {args.name}: {len(entry.get("known_bgas", []))} '
          f'BGAs discovered (TOP={n_top}, BOT={n_bot})')
    for b in entry.get('known_bgas', []):
        print(f"  - {b.get('name'):50s}  ref={b.get('ref_des'):8s}  "
              f"side={b.get('side')}  pins={b.get('pin_count')}  "
              f"pitch={b.get('pitch_mm')}mm")
    return 0


def _resolve_bga(dataset_name: str, bga_arg: str) -> str:
    """Allow a substring shorthand: '--bga bga218f' -> the full pkg name."""
    try:
        d = _registry.get_dataset(dataset_name)
    except KeyError:
        return bga_arg
    for b in d.get('known_bgas') or []:
        for field in ('name', 'ref_des'):
            v = b.get(field) or ''
            if v == bga_arg:
                return b.get('name', bga_arg)
    # substring fallback (case-insensitive on the package name)
    lo = bga_arg.lower()
    matches = [b for b in (d.get('known_bgas') or [])
               if lo in (b.get('name') or '').lower()]
    if len(matches) == 1:
        return matches[0]['name']
    return bga_arg


def cmd_route(args: argparse.Namespace) -> int:
    bga = _resolve_bga(args.dataset, args.bga)
    # Phase 53-3: --auto-recipe replaces the heuristic recipe choice.
    # Pool defaults to RecipeSelector's evidence-based pool; --pool overrides.
    recipe_name = args.recipe
    selection_meta = None
    if getattr(args, 'auto_recipe', False):
        from src.recipe_selector import select_recipe
        from src.eda_parser import load_eda_with_placement
        from src.real_target_scenario import build_scenario_real_targets
        from src.routing_spec import RoutingSpec
        from src.cost_grid import CostGrid, compute_board_geometry
        from src.net_extractor import extract_routing_tasks
        from src.routing_spec import NetRule

        # Build the (grid, tasks) just to feed the selector. This duplicates
        # some of run_route's setup but keeps the selector free of run_route
        # internals. Could be DRYed later (Phase 54).
        d = _registry.get_dataset(args.dataset)
        eda_root = d['path']
        step = d.get('step', 'mentor')
        eda_path = f'{eda_root}/steps/{step}/eda/data'
        eda = load_eda_with_placement(eda_path)
        target, partners, pin_to_net, _ = build_scenario_real_targets(eda, bga)
        # Use a generic 2-layer stackup if dataset doesn't expose one — same
        # convention the validation scripts use.
        layers = args.layers.split(',') if args.layers else ['COMP', 'LAY2']
        spec = RoutingSpec(
            defaults=NetRule(layers=tuple(layers),
                              escape_sides=('N','S','E','W'),
                              width_mm=0.075),
            components={})
        bboxes = []
        for c in [target] + partners:
            p = eda.packages.get(c.pkg_name)
            if p is None: continue
            hw = max(abs(p.bbox_min_x_mm), abs(p.bbox_max_x_mm))
            hh = max(abs(p.bbox_min_y_mm), abs(p.bbox_max_y_mm))
            bboxes.append((c.x_mm-hw, c.y_mm-hh, c.x_mm+hw, c.y_mm+hh))
        geom = compute_board_geometry(bboxes, cell_mm=0.05, margin_mm=1.0)
        grid = CostGrid(geom, layers)
        grid.add_pad_blockers([target]+partners, eda.packages, pad_clearance_mm=0.03)
        tasks = extract_routing_tasks(eda, spec, grid, pin_to_net)
        pool = (args.pool.split(',') if args.pool
                  else ('default-safe', 'coupled', 'default-fast'))
        sel = select_recipe(grid, tasks,
                             recipe_pool=pool,
                             parallel_runner=args.parallel)
        recipe_name = sel.recipe
        selection_meta = {
            'recipe': sel.recipe,
            'rationale': sel.rationale,
            'features': sel.features,
        }
        print(f'[auto-recipe] selected {sel.recipe!r}: {sel.rationale}')
    checks_arg = getattr(args, 'checks', None)
    checks_tuple = (tuple(c.strip() for c in checks_arg.split(',') if c.strip())
                     if checks_arg else None)
    result = _eval.run_route(args.dataset, bga, recipe_name,
                             budget_s=args.budget,
                             stackup_yaml=getattr(args, 'stackup', None),
                             plane_layer_names=(
                                 tuple(args.plane_layers.split(','))
                                 if getattr(args, 'plane_layers', None)
                                 else None),
                             checks=checks_tuple)
    if selection_meta is not None:
        result['auto_recipe_selection'] = selection_meta
    _emit(result, args.output)
    return 0


def cmd_eval(args: argparse.Namespace) -> int:
    bga = _resolve_bga(args.dataset, args.bga)
    recipes = [r.strip() for r in args.recipes.split(',') if r.strip()]
    results = _eval.eval_dataset(args.dataset, bga, recipes,
                                 budget_s=args.budget)
    # Phase 41 fix: --output (single file with all recipes) OR
    # --output-dir (one file per recipe). One must be picked but never both.
    if args.output:
        # Coalesce into a v1-style single file: {dataset, bga, recipes: {...}}
        coalesced = {
            'schema_version': '1.0-aggregated',
            'dataset': args.dataset,
            'bga': bga,
            'budget_s': args.budget,
            'recipes': {r: (payload.get('metrics') or payload)
                         for r, payload in zip(recipes, results)},
            'raw': results,
        }
        p = Path(args.output)
        p.parent.mkdir(parents=True, exist_ok=True)
        p.write_text(json.dumps(coalesced, indent=2, default=str))
        print(f'  -> {p}')
    elif args.output_dir:
        out_dir = Path(args.output_dir)
        out_dir.mkdir(parents=True, exist_ok=True)
        ts = time.strftime('%Y%m%dT%H%M%S')
        for r, payload in zip(recipes, results):
            p = out_dir / f'{args.dataset}__{bga}__{r}__{ts}.json'
            p.write_text(json.dumps(payload, indent=2, default=str))
            print(f'  -> {p}')
    else:
        json.dump(results, sys.stdout, indent=2, default=str)
        sys.stdout.write('\n')

    # Console summary
    print()
    print(f'[eval] dataset={args.dataset} bga={bga} '
          f'recipes={recipes} budget={args.budget}s')
    for r, payload in zip(recipes, results):
        m = payload.get('metrics') or {}
        if payload.get('error'):
            print(f'  {r:18s}  ERROR: {payload["error"]}')
        else:
            print(f'  {r:18s}  routed={m.get("routed", 0)}/'
                  f'{m.get("total", 0)}  '
                  f'ratio={(m.get("routed_ratio") or 0)*100:.1f}%  '
                  f'elapsed={m.get("elapsed_s", 0):.2f}s')
    return 0


def cmd_aggregate(args: argparse.Namespace) -> int:
    md = _aggregate.aggregate_results(args.results_dir)
    _emit_text(md, args.output)
    return 0


def cmd_em_dispatch(args: argparse.Namespace) -> int:
    from .integrations.em_queue_dispatch import dispatch, summarize_dispatch
    written = dispatch(args.input, args.out)
    summary = summarize_dispatch(written)
    print(f"[em-dispatch] wrote {summary['count']} task files to "
          f"{summary['output_dir']}")
    for s in summary['sample']:
        print(f'  - {s}')
    if summary['count'] > 3:
        print(f'  ... and {summary["count"] - 3} more')
    return 0


def cmd_net_diff(args: argparse.Namespace) -> int:
    from .metrics.net_diff import compare_results, render_markdown
    a = json.loads(Path(args.a).read_text())
    b = json.loads(Path(args.b).read_text())
    diff = compare_results(a, b,
                             recipe_a=a.get('recipe'),
                             recipe_b=b.get('recipe'))
    md = render_markdown(diff, top_k=args.top_k)
    _emit_text(md, args.output)
    return 0


def cmd_em_run(args: argparse.Namespace) -> int:
    from .integrations.sol_d_runner import summarize_run
    if getattr(args, 'solver', 'sol_d') == 'sol_b':
        from .integrations.sol_b_runner import dispatch_run
    else:
        from .integrations.sol_d_runner import dispatch_run
    results = dispatch_run(
        args.tasks_dir,
        em_data_json=args.em_data,
        output_dir=args.output_dir,
        dry_run=args.dry_run,
        timeout_s=args.timeout,
    )
    summary = summarize_run(results)
    mode = 'DRY-RUN' if args.dry_run else 'EXEC'
    print(f"[em-run:{args.solver}] {mode}: total={summary['total']} "
          f"executed={summary['executed']} ok={summary['ok']} "
          f"failed={summary['failed']} skipped={summary['skipped']}")
    if args.dry_run:
        for r in results[:3]:
            if not r.cmd:
                continue
            import shlex
            print('  $ ' + ' '.join(shlex.quote(c) for c in r.cmd))
        if len(results) > 3:
            print(f'  ... and {len(results) - 3} more')
    return 0 if summary['failed'] == 0 else 1


def _emit(payload, output_path):
    if output_path:
        p = Path(output_path)
        p.parent.mkdir(parents=True, exist_ok=True)
        p.write_text(json.dumps(payload, indent=2, default=str))
        print(f'  -> {p}')
    else:
        json.dump(payload, sys.stdout, indent=2, default=str)
        sys.stdout.write('\n')


def _emit_text(text, output_path):
    if output_path:
        p = Path(output_path)
        p.parent.mkdir(parents=True, exist_ok=True)
        p.write_text(text)
        print(f'  -> {p}')
    else:
        sys.stdout.write(text + '\n')


# ---------------------------------------------------------------------------
# Parser
# ---------------------------------------------------------------------------


def build_parser() -> argparse.ArgumentParser:
    p = argparse.ArgumentParser(
        prog='bga-router',
        description='Phase 41 BGA escape eval framework CLI.',
    )
    sub = p.add_subparsers(dest='cmd', required=True)

    # register
    p_reg = sub.add_parser('register', help='Register an ODB++ dataset.')
    p_reg.add_argument('--odb', required=True,
                       help='Path to the ODB++ tree (the directory '
                            'containing steps/).')
    p_reg.add_argument('--name', required=True,
                       help='Slug for this dataset in registry.yaml.')
    p_reg.add_argument('--type', default='dir', choices=['dir', 'zip'])
    p_reg.add_argument('--step', default='mentor')
    p_reg.add_argument('--overwrite', action='store_true')
    p_reg.set_defaults(func=cmd_register)

    # route
    p_rt = sub.add_parser('route', help='Run one (dataset, bga, recipe).')
    p_rt.add_argument('--dataset', required=True)
    p_rt.add_argument('--bga', required=True)
    p_rt.add_argument('--recipe', default='default',
                       help='Recipe name (default: "default" — the Phase 42-validated '
                            'safe recipe; same as "phase37_mutating" alias).')
    p_rt.add_argument('--budget', type=int, default=60)
    p_rt.add_argument('--output', help='Output JSON path (stdout if omitted).')
    # Phase 53-3: auto-recipe selection via RecipeSelector
    p_rt.add_argument('--auto-recipe', action='store_true', dest='auto_recipe',
                       help='Pick recipe via RecipeSelector instead of --recipe '
                            '(evidence-based heuristic; overrides --recipe).')
    p_rt.add_argument('--pool', default=None,
                       help='Comma-separated recipe pool for --auto-recipe '
                            '(default: default-safe,coupled,default-fast).')
    p_rt.add_argument('--parallel', action='store_true',
                       help='Hint to --auto-recipe: parallel runner available '
                            '(triggers default-fast for ≥30 net BGAs).')
    p_rt.add_argument('--layers', default=None,
                       help='Comma-separated signal layers for --auto-recipe '
                            'scenario build (default: COMP,LAY2).')
    # Phase C overrides — stackup + plane-layer detection
    p_rt.add_argument('--stackup', default=None,
                       help='Override path to stackup YAML (Phase C). '
                            'Default: registry.stackup_yaml or 4-layer FR-4.')
    p_rt.add_argument('--plane-layers', default=None, dest='plane_layers',
                       help='Comma-separated layer name substrings to treat '
                            'as plane layers (Phase C return-path metrics). '
                            'Default: GND,VDD,VSS,VCC,PWR.')
    p_rt.add_argument('--checks', default=None,
                       help='Comma-separated check areas to enable. '
                            'Choices: geometry,cross_net,rule_check,si,'
                            'standard,em_queue,return_path. Default: all on. '
                            'Use to skip expensive cross-net checks on large '
                            'designs (e.g. --checks=geometry,rule_check,si).')
    p_rt.set_defaults(func=cmd_route)

    # eval (sweep)
    p_ev = sub.add_parser('eval', help='Sweep recipes for one (dataset, bga).')
    p_ev.add_argument('--dataset', required=True)
    p_ev.add_argument('--bga', required=True)
    p_ev.add_argument('--recipes', required=True,
                      help='Comma-separated recipe list.')
    p_ev.add_argument('--budget', type=int, default=60)
    p_ev.add_argument('--output',
                      help='Single JSON file with all recipes coalesced '
                           '(v1 aggregator-compatible schema). Mutually '
                           'exclusive with --output-dir.')
    p_ev.add_argument('--output-dir',
                      help='Directory to write per-recipe JSON files '
                           '(v1.1 schema). Mutually exclusive with --output.')
    p_ev.set_defaults(func=cmd_eval)

    # aggregate
    p_ag = sub.add_parser('aggregate', help='Build markdown report.')
    p_ag.add_argument('--results-dir', required=True)
    p_ag.add_argument('--output', help='Output markdown path.')
    p_ag.set_defaults(func=cmd_aggregate)

    # em-dispatch — Phase D-5: em_queue → per-net sim task files
    p_em = sub.add_parser('em-dispatch',
                            help='Convert em_queue entries to per-net sim '
                                 'task files for sol_b / sol_d.')
    p_em.add_argument('--input', required=True,
                       help='Eval result JSON path (from `route` or one of '
                            '`eval --output-dir/...`).')
    p_em.add_argument('--out', required=True,
                       help='Output directory for per-net task JSONs.')
    p_em.set_defaults(func=cmd_em_dispatch)

    # em-run — Phase E-4: dispatch per-task sol_d invocations
    p_er = sub.add_parser('em-run',
                            help='Run sol_d for each em-dispatch task.')
    p_er.add_argument('--tasks-dir', required=True, dest='tasks_dir',
                       help='Directory of em-dispatch task JSONs (from '
                            '`em-dispatch --out`).')
    p_er.add_argument('--em-data', default=None, dest='em_data',
                       help='em_data.json input fed to sol_d (must be '
                            'extracted separately via odb_to_em_json).')
    p_er.add_argument('--output-dir', default=None, dest='output_dir',
                       help='Where to put sol_d per-task output dirs.')
    p_er.add_argument('--dry-run', action='store_true', dest='dry_run',
                       help='Print commands instead of executing.')
    p_er.add_argument('--timeout', type=int, default=600,
                       help='Per-task timeout in seconds (default 600).')
    p_er.add_argument('--solver', default='sol_d',
                       choices=['sol_d', 'sol_b'],
                       help='Which solver to invoke (default sol_d). '
                            'sol_b routes only tasks whose '
                            'suggested_solver is sol_b.')
    p_er.set_defaults(func=cmd_em_run)

    # net-diff — Phase F-4: two eval results → per-net markdown diff
    p_nd = sub.add_parser('net-diff',
                            help='Per-net diff between two route JSONs.')
    p_nd.add_argument('--a', required=True, help='Result JSON A path.')
    p_nd.add_argument('--b', required=True, help='Result JSON B path.')
    p_nd.add_argument('--output', default=None,
                       help='Output markdown path (stdout if omitted).')
    p_nd.add_argument('--top-k', type=int, default=10,
                       help='Top-K biggest deltas per section.')
    p_nd.set_defaults(func=cmd_net_diff)

    return p


def main(argv=None) -> int:
    args = build_parser().parse_args(argv)
    return args.func(args)


if __name__ == '__main__':
    sys.exit(main())
