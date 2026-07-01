# 두 recipe의 eval 결과에서 net-level 차이 (Z0 / length / bends / rule_check) 를 계산
"""Phase F-4 — cross-recipe net-level diff.

Given two eval result JSONs (same dataset+bga, different recipes),
compute per-net delta so users can answer "net042가 recipe A에선
Z0 50 ohm, B에선 47 ohm — B가 더 안 좋다" 같은 질문.

Input: two result dicts from bga_router.eval.run_route.
Output: per-net diff dict + summary stats.
"""

from __future__ import annotations

from dataclasses import dataclass
from typing import Any, Dict, List, Optional


def _get_si_map(result: dict, key: str) -> Dict[str, float]:
    si = (result.get('metrics') or {}).get('si') or {}
    m = si.get(key) or {}
    return {k: v for k, v in m.items() if isinstance(v, (int, float))}


def _per_net_lengths(result: dict) -> Dict[str, float]:
    """Path length per net — from si.branch_dc_resistance already covers
    net→length in an indirect way; simpler: use metrics.geometry.length
    per net when available. If not, aggregate from si.branch_dc_resistance
    proxy (fall through — best-effort)."""
    # No per-net length in geometry today (only design total). Approximate
    # by inverting DC R: R = rho*L/w*t → L ~ R (if w,t constant).
    return {}


def compare_results(result_a: dict, result_b: dict, *,
                      recipe_a: Optional[str] = None,
                      recipe_b: Optional[str] = None
                      ) -> Dict[str, Any]:
    """Per-net delta between two results. Both must be from the same
    (dataset, bga). Returns:

        {
          'recipe_a': str, 'recipe_b': str,
          'per_net': {
              net_name: {
                'z0_ohm_a': ..., 'z0_ohm_b': ..., 'z0_delta': ...,
                'zdiff_ohm_a': ..., 'zdiff_ohm_b': ..., ...
                'dc_r_mohm_a': ..., 'dc_r_mohm_b': ..., 'dc_r_delta': ...,
                'via_stub_a': ..., 'via_stub_b': ..., 'via_stub_delta': ...,
                'only_in': 'a' | 'b' | None
              }
          },
          'summary': {
            'nets_common': N,
            'nets_only_in_a': [names],
            'nets_only_in_b': [names],
            'z0_biggest_delta': [{'net': ..., 'delta': ...}, ...],
            ...
          }
        }
    """
    ra = recipe_a or result_a.get('recipe') or 'A'
    rb = recipe_b or result_b.get('recipe') or 'B'
    if ra == rb:
        rb = rb + ':B'

    z0_a = _get_si_map(result_a, 'Z0_single_ended_ohm')
    z0_b = _get_si_map(result_b, 'Z0_single_ended_ohm')
    dcr_a = _get_si_map(result_a, 'branch_dc_resistance_mohm')
    dcr_b = _get_si_map(result_b, 'branch_dc_resistance_mohm')
    stub_a = _get_si_map(result_a, 'via_stub_length_mm')
    stub_b = _get_si_map(result_b, 'via_stub_length_mm')

    all_nets = set(z0_a) | set(z0_b) | set(dcr_a) | set(dcr_b) | set(stub_a) | set(stub_b)
    per_net: Dict[str, Dict[str, Any]] = {}
    only_in_a: List[str] = []
    only_in_b: List[str] = []
    for net in sorted(all_nets):
        in_a = net in z0_a or net in dcr_a or net in stub_a
        in_b = net in z0_b or net in dcr_b or net in stub_b
        entry: Dict[str, Any] = {}
        for label, ma, mb in [
                ('z0_ohm',      z0_a,  z0_b),
                ('dc_r_mohm',   dcr_a, dcr_b),
                ('via_stub_mm', stub_a, stub_b)]:
            va = ma.get(net)
            vb = mb.get(net)
            entry[label + '_a'] = va
            entry[label + '_b'] = vb
            if va is not None and vb is not None:
                entry[label + '_delta'] = round(vb - va, 4)
        if in_a and not in_b:
            entry['only_in'] = 'a'
            only_in_a.append(net)
        elif in_b and not in_a:
            entry['only_in'] = 'b'
            only_in_b.append(net)
        else:
            entry['only_in'] = None
        per_net[net] = entry

    # Top-K biggest deltas
    def _topk(field: str, k: int = 10):
        items = []
        for net, e in per_net.items():
            d = e.get(field)
            if d is None:
                continue
            items.append((abs(d), net, d))
        items.sort(key=lambda x: -x[0])
        return [{'net': n, 'delta': round(d, 4)} for _abs, n, d in items[:k]]

    return {
        'recipe_a':        ra,
        'recipe_b':        rb,
        'dataset':         result_a.get('dataset'),
        'bga':             result_a.get('bga'),
        'per_net':         per_net,
        'summary': {
            'nets_common':      len(all_nets) - len(only_in_a) - len(only_in_b),
            'nets_only_in_a':   only_in_a,
            'nets_only_in_b':   only_in_b,
            'z0_biggest_delta': _topk('z0_ohm_delta'),
            'dc_r_biggest_delta': _topk('dc_r_mohm_delta'),
            'via_stub_biggest_delta': _topk('via_stub_mm_delta'),
        },
    }


def render_markdown(diff: Dict[str, Any], *,
                      top_k: int = 10) -> str:
    """Human-readable markdown report of a compare_results output."""
    lines: List[str] = []
    lines.append('# Net-level diff — '
                  f'`{diff.get("recipe_a")}` vs `{diff.get("recipe_b")}`')
    lines.append('')
    lines.append(f"Dataset: `{diff.get('dataset')}`  BGA: "
                  f"`{diff.get('bga')}`")
    lines.append('')
    s = diff['summary']
    lines.append(f"- nets common: **{s['nets_common']}**")
    lines.append(f"- nets only in A: {len(s['nets_only_in_a'])}")
    lines.append(f"- nets only in B: {len(s['nets_only_in_b'])}")
    lines.append('')
    lines.append('## Z0 biggest deltas (Ω)')
    lines.append('')
    lines.append('| net | Δ (B - A) |')
    lines.append('|---|---:|')
    for e in s['z0_biggest_delta'][:top_k]:
        lines.append(f"| `{e['net']}` | {e['delta']:+.3f} |")
    lines.append('')
    lines.append('## DC resistance biggest deltas (mΩ)')
    lines.append('')
    lines.append('| net | Δ (B - A) |')
    lines.append('|---|---:|')
    for e in s['dc_r_biggest_delta'][:top_k]:
        lines.append(f"| `{e['net']}` | {e['delta']:+.3f} |")
    lines.append('')
    lines.append('## Via stub biggest deltas (mm)')
    lines.append('')
    lines.append('| net | Δ (B - A) |')
    lines.append('|---|---:|')
    for e in s['via_stub_biggest_delta'][:top_k]:
        lines.append(f"| `{e['net']}` | {e['delta']:+.3f} |")
    return '\n'.join(lines)
