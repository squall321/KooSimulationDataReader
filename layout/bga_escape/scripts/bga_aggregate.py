#!/usr/bin/env python3
"""Phase 41b — multi-ODB eval aggregator.

Walks a directory of ``bga_eval`` JSON outputs and emits a markdown report
(dataset × bga × recipe → key metrics).

Examples
--------

    ./scripts/bga_aggregate.py \
        --results-dir /home/koopark/.../data/eval \
        --output      /home/koopark/.../data/eval/REPORT.md

    # stdout
    ./scripts/bga_aggregate.py --results-dir data/eval
"""

from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = HERE.parent


# ---------------------------------------------------------------------------
# I/O
# ---------------------------------------------------------------------------


def load_results(results_dir: Path) -> list:
    """Yield (path, payload) for every JSON file under ``results_dir``
    that looks like a bga_eval output."""
    out = []
    # Two schemas are supported (Phase 41 fix):
    #   v1 (legacy)  — one file per (dataset, bga); 'recipes' is dict
    #                  {recipe_name: metrics_dict}.
    #   v1.1 (Phase 41 bga_router eval) — one file per (dataset, bga, recipe);
    #                  fields {dataset, bga, recipe (singular), metrics, ...}.
    #
    # We coalesce v1.1 records by (dataset, bga) so the downstream table
    # generation sees a unified shape with multiple recipes per row.
    v1_records: list = []
    v11_buckets: dict = {}  # (dataset, bga) -> {recipe: metrics}
    v11_path_repr: dict = {}  # (dataset, bga) -> representative Path
    for p in sorted(results_dir.rglob('*.json')):
        try:
            with p.open() as fh:
                data = json.load(fh)
        except (OSError, json.JSONDecodeError):
            continue
        if not isinstance(data, dict):
            continue
        # v1 detection
        if {'dataset', 'bga', 'recipes'} <= set(data):
            v1_records.append((p, data))
            continue
        # v1.1 detection
        if {'dataset', 'bga', 'recipe', 'metrics'} <= set(data):
            key = (data['dataset'], data['bga'])
            v11_buckets.setdefault(key, {})[data['recipe']] = data['metrics']
            v11_path_repr.setdefault(key, p)
    # Coalesce v1.1 buckets into v1-shaped payloads for downstream code
    for (dataset, bga), recipes_map in v11_buckets.items():
        synth = {'dataset': dataset, 'bga': bga, 'recipes': recipes_map}
        v1_records.append((v11_path_repr[(dataset, bga)], synth))
    return v1_records


# ---------------------------------------------------------------------------
# Formatting
# ---------------------------------------------------------------------------


def _fmt_n(v) -> str:
    if v is None:
        return 'n/a'
    if isinstance(v, bool):
        return 'yes' if v else 'no'
    if isinstance(v, float):
        return f'{v:.2f}'
    return str(v)


def _fmt_ratio(r) -> str:
    if r is None:
        return 'n/a'
    try:
        return f'{r * 100:5.1f}%'
    except (TypeError, ValueError):
        return 'n/a'


def build_markdown(records: list) -> str:
    """Build the full markdown report from a list of (path, payload)."""
    lines: list = []
    lines.append('# BGA Eval Aggregate Report')
    lines.append('')
    lines.append(f'Generated from **{len(records)}** result file(s).')
    lines.append('')

    if not records:
        lines.append('_No results found._')
        return '\n'.join(lines)

    # Per-dataset section.
    by_dataset: dict = {}
    for path, payload in records:
        by_dataset.setdefault(payload['dataset'], []).append((path, payload))

    for dataset in sorted(by_dataset):
        lines.append(f'## Dataset: `{dataset}`')
        lines.append('')
        # Phase B/C additions: Length, Sharp, RuleViol, Z0var, Std,
        # RoutedClean% (Phase E-1).
        lines.append('| BGA | Recipe | Routed/Total | %  | RoutedClean % | '
                     'Elapsed (s) | Vias | Iter | Length (mm) | Sharp | '
                     'RuleViol | Z0var % | Std | Status |')
        lines.append('|-----|--------|--------------|----|---------------|'
                     '-------------|------|------|-------------|-------|'
                     '----------|---------|-----|--------|')
        rows = []
        for path, payload in by_dataset[dataset]:
            bga = payload['bga']
            for recipe_name, m in payload.get('recipes', {}).items():
                rows.append((bga, recipe_name, m, path.name))
        rows.sort(key=lambda r: (r[0], r[1]))
        for bga, recipe_name, m, fname in rows:
            status = 'error' if 'error' in m else (
                'timeout' if m.get('timeout') else
                ('converged' if m.get('converged') else 'partial')
            )
            routed = m.get('routed', 0)
            total = m.get('total', 0)
            geometry = m.get('geometry') or {}
            rule_check = m.get('rule_check') or {}
            si = m.get('si') or {}
            standard = m.get('standard') or {}
            length_mm = geometry.get('total_length_mm') or m.get('total_length_mm')
            sharp = geometry.get('sharp_bends')
            if sharp is None:
                sharp = m.get('sharp_bends')
            rule_viol = rule_check.get('violations')
            z0var = si.get('Z0_variance_pct')
            # Phase E-1: ratio of nets routed AND with zero rule_check
            # violations. Approximation — rule_viol is design-level count,
            # not per-net; treat 0 violations as "fully clean" and apply
            # routed_ratio as the floor.
            ratio = m.get('routed_ratio') or 0
            clean_ratio = ratio if (rule_viol == 0) else 0.0
            # Compact standards summary: count passed/failed/null
            std_keys = ('ddr4_ok', 'pcie_gen3_ok', 'pcie_gen4_ok',
                         'usb32_ok', 'hdmi_ok', 'ethernet_ok')
            std_results = [standard.get(k) for k in std_keys]
            std_active = [r for r in std_results if r is not None]
            if std_active:
                pass_count = sum(1 for r in std_active if r.get('passed'))
                std_repr = f'{pass_count}/{len(std_active)}'
            else:
                std_repr = '-'
            lines.append(
                f"| {bga} | {recipe_name} | {routed}/{total} | "
                f"{_fmt_ratio(m.get('routed_ratio'))} | "
                f"{_fmt_ratio(clean_ratio)} | "
                f"{_fmt_n(m.get('elapsed_s'))} | "
                f"{_fmt_n(m.get('via_count'))} | "
                f"{_fmt_n(m.get('iterations'))} | "
                f"{_fmt_n(length_mm)} | "
                f"{_fmt_n(sharp)} | "
                f"{_fmt_n(rule_viol)} | "
                f"{_fmt_n(z0var)} | "
                f"{std_repr} | "
                f"{status} |"
            )
        lines.append('')

    # Cross-recipe matrix (routed_ratio per recipe per (dataset, bga)).
    all_recipes: set = set()
    for _p, payload in records:
        all_recipes.update(payload.get('recipes', {}).keys())
    recipe_cols = sorted(all_recipes)

    if recipe_cols:
        lines.append('## Cross-Recipe Matrix — routed_ratio (%)')
        lines.append('')
        header = '| Dataset / BGA | ' + ' | '.join(recipe_cols) + ' |'
        sep    = '|' + '---|' * (1 + len(recipe_cols))
        lines.append(header)
        lines.append(sep)
        for dataset in sorted(by_dataset):
            for path, payload in by_dataset[dataset]:
                bga = payload['bga']
                cells = []
                for r in recipe_cols:
                    m = payload.get('recipes', {}).get(r)
                    cells.append(_fmt_ratio(m.get('routed_ratio'))
                                  if m else 'n/a')
                lines.append(f'| {dataset} / {bga} | ' + ' | '.join(cells)
                              + ' |')
        lines.append('')

    return '\n'.join(lines)


# ---------------------------------------------------------------------------
# CLI
# ---------------------------------------------------------------------------


def parse_args(argv=None) -> argparse.Namespace:
    p = argparse.ArgumentParser(
        description='Aggregate bga_eval JSON outputs into a markdown report.',
    )
    p.add_argument('--results-dir', required=True,
                    help='Directory containing bga_eval JSON files '
                         '(searched recursively).')
    p.add_argument('--output',
                    help='Output markdown path. Stdout if omitted.')
    return p.parse_args(argv)


def main(argv=None) -> int:
    args = parse_args(argv)
    results_dir = Path(args.results_dir)
    if not results_dir.is_dir():
        print(f'ERROR: {results_dir} is not a directory.', file=sys.stderr)
        return 2

    records = load_results(results_dir)
    md = build_markdown(records)

    if args.output:
        out = Path(args.output)
        out.parent.mkdir(parents=True, exist_ok=True)
        out.write_text(md)
        print(f'wrote {out}  ({len(records)} record(s))')
    else:
        print(md)
    return 0


if __name__ == '__main__':
    sys.exit(main())
