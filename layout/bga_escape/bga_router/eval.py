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
              budget_s: int = 60,
              *,
              stackup_yaml: Optional[str] = None,
              plane_layer_names: Optional[tuple] = None,
              checks: Optional[tuple] = None) -> Dict[str, Any]:
    """Run one (dataset, bga, recipe) and return a single result JSON.

    Phase C/D kwargs:
      stackup_yaml — explicit path overriding registry.stackup_yaml.
      plane_layer_names — tuple of substrings (case-insensitive); when
        supplied, only layers whose name contains one of these are
        treated as plane layers by the loader.
      checks — tuple of enabled check areas. None = all on. Choices:
        geometry, cross_net, rule_check, si, standard, em_queue,
        return_path. Use to skip expensive checks on large designs.
    """
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
                _populate_extended_metrics(result.metrics, internals, base,
                                            entry=entry,
                                            stackup_yaml_override=stackup_yaml,
                                            plane_layer_names=plane_layer_names,
                                            checks=checks)
            except Exception as e:
                result.metrics.setdefault('phase_b_errors', {})['_outer'] = (
                    f'{type(e).__name__}: {str(e)[:200]}'
                )
    except Exception as e:
        result.error = f'{type(e).__name__}: {str(e)[:200]}'
        result.metrics = {'elapsed_s': round(time.time() - t0, 3)}

    return result.to_json()


_ALL_CHECKS = frozenset((
    'geometry', 'cross_net', 'rule_check', 'si', 'standard',
    'em_queue', 'return_path',
))


def _populate_extended_metrics(metrics: Dict[str, Any],
                                 internals: Dict[str, Any],
                                 base: Dict[str, Any],
                                 *, entry=None,
                                 stackup_yaml_override: Optional[str] = None,
                                 plane_layer_names: Optional[tuple] = None,
                                 checks: Optional[tuple] = None
                                 ) -> None:
    enabled = (set(checks) & _ALL_CHECKS) if checks else _ALL_CHECKS
    def _on(name: str) -> bool:
        return name in enabled
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
    if _on('geometry'):
        try:
            geo_extras = summarize_geometry(routed_paths, grid, tasks_for_geo)
            for k, v in geo_extras.items():
                # do not overwrite total_length_mm produced above
                if k == 'total_length_mm':
                    continue
                metrics['geometry'][k] = v
        except Exception as e:
            phase_b_errors['summarize_geometry'] = f'{type(e).__name__}: {e}'

    if _on('cross_net'):
        try:
            cross_extras = summarize_cross_net(routed_paths, grid)
            for k, v in cross_extras.items():
                metrics['geometry'][k] = v
        except Exception as e:
            phase_b_errors['summarize_cross_net'] = f'{type(e).__name__}: {e}'

    # Legacy placeholder sharp_bends — fill from summarize_geometry result.
    if 'sharp_bends' in geo_extras:
        metrics['sharp_bends'] = geo_extras['sharp_bends']

    # Phase C — stackup + plane_geom (both optional). Resolve once per run.
    phase_c_errors: Dict[str, str] = {}

    stackup = None
    try:
        from bga_router.metrics.stackup import (default_stackup,
                                                  load_for_dataset,
                                                  load_stackup_yaml)
        if stackup_yaml_override:
            stackup = load_stackup_yaml(stackup_yaml_override)
        else:
            stackup = load_for_dataset(entry)
        if stackup.is_default:
            metrics['stackup_default_used'] = True
    except Exception as e:
        phase_c_errors['stackup'] = f'{type(e).__name__}: {e}'
        try:
            from bga_router.metrics.stackup import default_stackup
            stackup = default_stackup()
            metrics['stackup_default_used'] = True
        except Exception:
            stackup = None

    plane_geom = None
    try:
        from src.ecad.plane_loader import PlaneGeometry, load_plane_geometry
        if entry is not None:
            step = getattr(entry, 'step', None) or 'mentor'
            kwargs: Dict[str, Any] = {'step': step}
            if plane_layer_names:
                kwargs['plane_layer_names'] = plane_layer_names
            plane_geom = load_plane_geometry(getattr(entry, 'path', '.'),
                                              **kwargs)
        else:
            plane_geom = PlaneGeometry(layers={}, units_mm=True)
    except Exception as e:
        phase_c_errors['plane_loader'] = f'{type(e).__name__}: {e}'
        try:
            from src.ecad.plane_loader import PlaneGeometry
            plane_geom = PlaneGeometry(layers={}, units_mm=True)
        except Exception:
            plane_geom = None

    # rule_check — now wired with stackup + plane_geom (impedance_target,
    # via_type, split_avoidance promote from N/A when data is available).
    if _on('rule_check'):
        try:
            rc = verify_all(routed_paths, tasks, grid, spec=spec,
                             stackup=stackup, plane_geom=plane_geom)
            by_field_serialized = {}
            # Phase G-3 — per-net violation count instead of design-level 1.
            # For each field, add the number of violator nets to the total.
            per_net_violation_count: Dict[str, int] = {}
            expanded_total = 0
            for fname, r in rc['by_field'].items():
                violators_list = list(r.violators)
                by_field_serialized[fname] = {
                    'pass':      bool(r.passed),
                    'violators': violators_list,
                    'violator_count': len(violators_list),
                    'note':      r.note,
                }
                # per-field expanded contribution
                expanded_total += len(violators_list)
                for net in violators_list:
                    per_net_violation_count[net] = (
                        per_net_violation_count.get(net, 0) + 1)
            # by_net breakdown — sorted descending by count for reporting
            by_net = [
                {'net': n, 'violation_count': c,
                  'fields': sorted(
                      f for f, e in by_field_serialized.items()
                      if n in e['violators'])}
                for n, c in sorted(per_net_violation_count.items(),
                                     key=lambda kv: (-kv[1], kv[0]))
            ]
            metrics['rule_check'] = {
                'violations':          rc['violations'],
                # Phase G-3 additions
                'violations_expanded': expanded_total,
                'violations_by_net':   by_net,
                'nets_with_violation': len(per_net_violation_count),
                'by_field':            by_field_serialized,
            }
        except Exception as e:
            phase_b_errors['verify_all'] = f'{type(e).__name__}: {e}'

    # Phase C — analytical SI
    if _on('si'):
        try:
            from bga_router.metrics.si import summarize_si
            rules_by_net = {t.net_name: t.rule for t in tasks}
            if stackup is not None:
                metrics['si'] = summarize_si(routed_paths, grid,
                                               rules_by_net, stackup)
        except Exception as e:
            phase_c_errors['summarize_si'] = f'{type(e).__name__}: {e}'

    # Phase C — return-path metrics. PG vias + stitching vias derived
    # from path + net classification (Phase D-6).
    if _on('return_path'):
        try:
            from bga_router.metrics.return_path import summarize_return_path
            from bga_router.metrics.path_geometry import (
                collect_pg_via_xy, collect_stitching_via_xy,
            )
            if plane_geom is not None:
                pg_vias = collect_pg_via_xy(routed_paths, grid)
                stitch_vias = collect_stitching_via_xy(routed_paths, grid)
                rp = summarize_return_path(
                    routed_paths, grid, plane_geom,
                    power_ground_vias_xy=pg_vias,
                    stitching_vias_xy=stitch_vias,
                    reference_layer='GND')
                # Surface counts at top of return-path block for visibility
                if isinstance(rp, dict):
                    rp.setdefault('inferred_pg_via_count', len(pg_vias))
                    rp.setdefault('inferred_stitch_via_count', len(stitch_vias))
                metrics.setdefault('si', {})['return_path'] = rp
        except Exception as e:
            phase_c_errors['summarize_return_path'] = f'{type(e).__name__}: {e}'

    # Phase F-2 — bus group auto-detect from net names
    try:
        from bga_router.metrics.bus_groups import summarize_bus_groups
        net_names = list(routed_paths.keys())
        metrics['bus_groups'] = summarize_bus_groups(net_names)
    except Exception as e:
        phase_c_errors['bus_groups'] = f'{type(e).__name__}: {e}'

    # Phase F-1 — per-pair coupling matrix
    try:
        from bga_router.metrics.coupling import summarize_coupling
        metrics['coupling'] = summarize_coupling(routed_paths, grid)
    except Exception as e:
        phase_c_errors['coupling'] = f'{type(e).__name__}: {e}'

    # Phase F-5 — net cluster auto-classification (signal domain)
    try:
        from bga_router.metrics.net_clusters import summarize_net_clusters
        rbn_for_cluster = {t.net_name: t.rule for t in tasks}
        metrics['net_clusters'] = summarize_net_clusters(
            list(routed_paths.keys()),
            rules_by_net=rbn_for_cluster)
    except Exception as e:
        phase_c_errors['net_clusters'] = f'{type(e).__name__}: {e}'

    # Phase F-3 — driver/load dependency graph
    try:
        from bga_router.metrics.dependency_graph import summarize_dependencies
        rbn_for_dep = {t.net_name: t.rule for t in tasks}
        metrics['dependencies'] = summarize_dependencies(
            list(routed_paths.keys()), rbn_for_dep, routed_paths, grid)
    except Exception as e:
        phase_c_errors['dependencies'] = f'{type(e).__name__}: {e}'

    # Phase G-4 — driver_pin ↔ net_name resolver
    try:
        from bga_router.metrics.driver_resolver import (
            summarize_driver_resolution)
        rbn_for_drv = {t.net_name: t.rule for t in tasks}
        eda = internals.get('eda')
        components = getattr(eda, 'components', []) if eda else []
        metrics['driver_resolution'] = summarize_driver_resolution(
            rbn_for_drv, components)
    except Exception as e:
        phase_c_errors['driver_resolution'] = f'{type(e).__name__}: {e}'

    # Phase D — EM queue hook (marginal Z0 / impedance miss → solver)
    if _on('em_queue'):
        try:
            from bga_router.metrics.em_queue import build_em_queue
            rbn = {t.net_name: t.rule for t in tasks}
            metrics['em_queue'] = build_em_queue(
                routed_paths, grid, rbn,
                metrics.get('si') or {}, metrics.get('rule_check') or {})
        except Exception as e:
            phase_c_errors['em_queue'] = f'{type(e).__name__}: {e}'

    # Phase C — high-speed standard pass/fail
    if _on('standard'):
        try:
            from bga_router.metrics.standards import summarize_standards
            # Pre-compute per-net lengths for length-budget checks (USB3.2).
            lengths = {}
            for net, pr in routed_paths.items():
                path = getattr(pr, 'path', None)
                if path:
                    lengths[net] = round(path_length_mm(path, grid), 4)
            metrics['standard'] = summarize_standards(
                routed_paths,
                metrics.get('geometry', {}),
                metrics.get('si', {}),
                metrics.get('rule_check', {}),
                lengths=lengths,
            )
        except Exception as e:
            phase_c_errors['summarize_standards'] = f'{type(e).__name__}: {e}'

    if phase_b_errors:
        metrics['phase_b_errors'] = phase_b_errors
    if phase_c_errors:
        metrics['phase_c_errors'] = phase_c_errors


def eval_dataset(dataset_name: str,
                 bga: str,
                 recipes: List[str],
                 budget_s: int = 60) -> List[Dict[str, Any]]:
    """Cross-recipe sweep for one BGA. Returns a list of result JSONs."""
    out: List[Dict[str, Any]] = []
    for r in recipes:
        out.append(run_route(dataset_name, bga, r, budget_s=budget_s))
    return out
