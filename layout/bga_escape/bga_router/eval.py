"""Per-BGA eval driver (Phase 41 wrapper).

Reuses the recipe library + scenario builder from ``scripts/bga_eval.py``
so we have a single source of truth. The CLI also re-exports these
helpers via subcommands.

Public surface:
    RECIPES            - dict[str, Recipe]
    run_route(...)     - one (dataset, bga, recipe) -> ResultJSON dict
    eval_dataset(...)  - cross-recipe sweep -> list[ResultJSON]
"""
from __future__ import annotations

import sys
import time
from dataclasses import asdict, dataclass, field
from pathlib import Path
from typing import Any, Dict, List, Optional

from . import registry as _registry

PROJECT_ROOT = Path(__file__).resolve().parent.parent

# Import legacy modules. ``scripts/bga_eval.py`` already wires sys.path so
# we mirror that here.
sys.path.insert(0, str(PROJECT_ROOT))
sys.path.insert(0, str(PROJECT_ROOT / 'scripts'))


# Defer-import everything heavy so `bga-router --help` is snappy.
def _lazy_legacy():
    """Import the legacy bga_eval module lazily; return (module, exc)."""
    try:
        import bga_eval as legacy   # scripts/bga_eval.py
        return legacy, None
    except Exception as exc:
        return None, exc


# ---------------------------------------------------------------------------
# Recipe library — sourced from scripts/bga_eval.py to stay in sync.
# We expose them here so callers don't need to know about the legacy path.
# ---------------------------------------------------------------------------

def _recipe_library() -> Dict[str, Any]:
    legacy, exc = _lazy_legacy()
    if legacy is None:
        raise RuntimeError(
            f'Could not import scripts/bga_eval.py: {exc}. The legacy '
            f'src/ tree must be on sys.path.'
        )
    return legacy.RECIPES


RECIPES: Dict[str, Any]

def __getattr__(attr):
    if attr == 'RECIPES':
        return _recipe_library()
    raise AttributeError(attr)


# ---------------------------------------------------------------------------
# Result shape — light dataclass for clarity.
# ---------------------------------------------------------------------------


@dataclass
class RouteResult:
    dataset: str
    dataset_path: str
    bga: str
    recipe: str
    budget_s: int
    n_components: int = 0
    n_candidate_nets: int = 0
    metrics: Dict[str, Any] = field(default_factory=dict)
    error: Optional[str] = None

    def to_json(self) -> Dict[str, Any]:
        return {
            'schema_version': '1.1',
            **asdict(self),
        }


# ---------------------------------------------------------------------------
# Public API
# ---------------------------------------------------------------------------


def _resolve_dataset(name_or_path: str):
    """Return a legacy DatasetEntry for ``name_or_path``."""
    legacy, exc = _lazy_legacy()
    if legacy is None:
        raise RuntimeError(f'legacy bga_eval import failed: {exc}')
    reg = _registry.load_registry()
    return legacy.resolve_dataset(name_or_path, reg)


def run_route(odb: str, bga: str, recipe_name: str,
              budget_s: int = 60) -> Dict[str, Any]:
    """Run one (dataset, bga, recipe) and return a single result JSON."""
    legacy, exc = _lazy_legacy()
    if legacy is None:
        raise RuntimeError(f'legacy bga_eval import failed: {exc}')

    entry = _resolve_dataset(odb)
    recipes = legacy.RECIPES
    if recipe_name not in recipes:
        raise KeyError(
            f'Unknown recipe {recipe_name!r}. Known: {sorted(recipes)}'
        )
    recipe = recipes[recipe_name]

    result = RouteResult(
        dataset=entry.name,
        dataset_path=str(entry.path),
        bga=bga,
        recipe=recipe_name,
        budget_s=budget_s,
    )

    t0 = time.time()
    try:
        ctx = legacy.build_context(entry, bga)
    except Exception as e:
        result.error = f'context_build_failed: {type(e).__name__}: {e}'
        result.metrics = {'elapsed_s': round(time.time() - t0, 3)}
        return result.to_json()

    result.n_components = len(ctx.components_to_route)
    result.n_candidate_nets = len(ctx.pin_to_net) // 2

    try:
        from src.recipe_eval import evaluate_recipe  # type: ignore
        # Phase A — request `_internals` so verifier / geometry / SI
        # modules can introspect the routed paths after metrics extract.
        base = evaluate_recipe(recipe, ctx, time_budget_s=budget_s,
                                return_internals=True)
        internals = base.pop('_internals', None)
        result.metrics = legacy.extract_extended_metrics(base)
        if internals is not None:
            # Phase B metrics enrichment is best-effort. If it crashes we
            # keep the base metrics and surface the error under
            # phase_b_errors['_outer'] instead of wiping the result.
            try:
                _populate_extended_metrics(result.metrics, internals, base)
            except Exception as e:
                result.metrics.setdefault('phase_b_errors', {})['_outer'] = (
                    f'{type(e).__name__}: {str(e)[:200]}'
                )
    except Exception as e:
        result.error = f'{type(e).__name__}: {str(e)[:200]}'
        result.metrics = {'elapsed_s': round(time.time() - t0, 3)}

    return result.to_json()


def _populate_extended_metrics(metrics: Dict[str, Any],
                                 internals: Dict[str, Any],
                                 base: Dict[str, Any]) -> None:
    """Phase A hook — fills geometry.* sub-dict and the placeholder fields
    (``total_length_mm``, ``sharp_bends`` …) that ``extract_extended_metrics``
    leaves as ``None``. Phase B (rule_check) and Phase C (si / standard)
    add their sub-dicts here too.
    """
    grid = internals['grid']
    routed_paths = internals['routed_paths']
    tasks = internals['tasks']
    spec = internals.get('spec')

    from bga_router.metrics.path_geometry import (
        path_length_mm,
        length_per_layer_mm,
        count_vias,
    )

    total_length = 0.0
    per_layer: Dict[str, float] = {}
    transitions = 0
    for path_result in routed_paths.values():
        path = getattr(path_result, 'path', None)
        if not path:
            continue
        total_length += path_length_mm(path, grid)
        for layer, mm in length_per_layer_mm(path, grid).items():
            per_layer[layer] = per_layer.get(layer, 0.0) + mm
        transitions += count_vias(path)

    metrics['geometry'] = {
        'total_length_mm':        round(total_length, 4),
        'length_per_layer_mm':    {k: round(v, 4) for k, v in per_layer.items()},
        'layer_transition_count': transitions,
        'residual_overuse':       int(base.get('residual_overuse', 0) or 0),
    }
    # Fill the legacy placeholder so downstream consumers stop seeing null.
    if metrics.get('total_length_mm') is None:
        metrics['total_length_mm'] = metrics['geometry']['total_length_mm']

    # Phase B — fold extra geometry, cross-net, and rule_check sub-dicts.
    from bga_router.metrics.geometry import summarize_geometry
    from bga_router.metrics.cross_net import summarize_cross_net
    from bga_router.metrics.verifier import verify_all

    # ``summarize_geometry`` expects task.source / task.sink to be
    # (layer, ix, iy) tuples (see geometry.py:266). Real RoutingTask
    # uses NetEndpoint objects, so wrap them in a lightweight shim.
    class _TaskTupleView:
        __slots__ = ('net_name', 'source', 'sink', 'rule')
        def __init__(self, t):
            self.net_name = t.net_name
            self.source = (t.source.layer, t.source.ix, t.source.iy)
            self.sink = (t.sink.layer, t.sink.ix, t.sink.iy)
            self.rule = t.rule

    # Phase B metrics are best-effort. A bug in any of these aggregators
    # must NOT clobber the base metrics — leave the corresponding sub-dict
    # absent and record the error in metrics['phase_b_errors'].
    phase_b_errors: Dict[str, str] = {}

    try:
        tasks_for_geo = [_TaskTupleView(t) for t in tasks]
    except Exception as e:
        tasks_for_geo = []
        phase_b_errors['task_shim'] = f'{type(e).__name__}: {e}'

    geo_extras: Dict[str, Any] = {}
    try:
        geo_extras = summarize_geometry(routed_paths, grid, tasks_for_geo)
        for k, v in geo_extras.items():
            # do not overwrite total_length_mm produced above
            if k == 'total_length_mm':
                continue
            metrics['geometry'][k] = v
    except Exception as e:
        phase_b_errors['summarize_geometry'] = f'{type(e).__name__}: {e}'

    try:
        cross_extras = summarize_cross_net(routed_paths, grid)
        for k, v in cross_extras.items():
            metrics['geometry'][k] = v
    except Exception as e:
        phase_b_errors['summarize_cross_net'] = f'{type(e).__name__}: {e}'

    # Legacy placeholder sharp_bends — fill from summarize_geometry result.
    if 'sharp_bends' in geo_extras:
        metrics['sharp_bends'] = geo_extras['sharp_bends']

    try:
        rc = verify_all(routed_paths, tasks, grid, spec=spec, stackup=None)
        by_field_serialized = {}
        for fname, r in rc['by_field'].items():
            by_field_serialized[fname] = {
                'pass':      bool(r.passed),
                'violators': list(r.violators),
                'note':      r.note,
            }
        metrics['rule_check'] = {
            'violations': rc['violations'],
            'by_field':   by_field_serialized,
        }
    except Exception as e:
        phase_b_errors['verify_all'] = f'{type(e).__name__}: {e}'

    if phase_b_errors:
        metrics['phase_b_errors'] = phase_b_errors


def eval_dataset(dataset_name: str,
                 bga: str,
                 recipes: List[str],
                 budget_s: int = 60) -> List[Dict[str, Any]]:
    """Cross-recipe sweep for one BGA. Returns a list of result JSONs."""
    out: List[Dict[str, Any]] = []
    for r in recipes:
        out.append(run_route(dataset_name, bga, r, budget_s=budget_s))
    return out
