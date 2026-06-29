"""Phase 5 — Recipe (Skill 조합 + Hyperparameter).

Recipe 하나가 라우터의 모든 선택을 결정. DOE는 Recipe 자체를 sweep dim에
포함시켜 진짜 best 조합을 자동 발견한다.

run_recipe()가 단일 진입점. 향후 unbpr_v1~v4 모두 Recipe로 wrap 가능.
"""

from __future__ import annotations

import copy
from dataclasses import dataclass, field
from typing import Any, Optional

from src.skills.protocols import (
    NegotiationOutcome, NetGroup, ParallelContext, PathResult, RecipeError,
    RoutingRequest,
)
from src.skills.registry import REGISTRY, SkillRegistry


@dataclass(frozen=True)
class Recipe:
    """알고리즘 조합 + per-skill hyperparameter.

    Field 의미:
      path_solver: registry key, 단일.
      negotiator: registry key, 단일.
      decomposers: tuple of registry keys, 순서대로 적용.
      cost_fn: registry key, 단일 (PathSolver가 사용; None이면 grid 기본).
      initial_solvers: tuple of registry keys, 시도 순서 (모두 fail → cold).
      post_processors: tuple, 순서대로 적용.
      skill_params: {skill_name -> {param: value}}
      parallel_modes: {skill_name -> mode_string}
    """
    path_solver: str
    negotiator: str
    decomposers: tuple = ()
    cost_fn: str = 'StandardCost'
    initial_solvers: tuple = ()
    post_processors: tuple = ()
    skill_params: dict = field(default_factory=dict)
    parallel_modes: dict = field(default_factory=dict)
    iter_max: int = 1   # Phase 34c: outer post-process iteration cap.
                        # 1 == today's single-pass behaviour. >1 enables the
                        # iterative refine loop: post-processors that report
                        # mutations (via `extra['_iter_changes']` on the
                        # returned dict) trigger another round, until either
                        # nothing changed or the cap is hit.

    def __post_init__(self):
        # frozen dataclass에서 mutable field 안전 처리.
        # Phase 44 review fix #1: deep-copy nested dicts so two recipes
        # built from the same skill_params source (e.g. default-fast
        # spawned from _DEFAULT_SAFE) don't share inner dict objects.
        # Without this, a runtime mutation of one recipe's nested params
        # (max_iter bump, mutate toggle, etc.) silently changes the other.
        object.__setattr__(self, 'skill_params',
                             copy.deepcopy(self.skill_params))
        object.__setattr__(self, 'parallel_modes',
                             copy.deepcopy(self.parallel_modes))

    def validate(self, registry: SkillRegistry = None) -> None:
        """모든 referenced skill이 등록되었는지, parallel_mode가 지원되는지 검증.
        Fail → RecipeError."""
        registry = registry or REGISTRY

        # 단일 skills
        for cat, name in [('path_solver', self.path_solver),
                           ('negotiator', self.negotiator),
                           ('cost_fn', self.cost_fn)]:
            try:
                registry.get(cat, name)
            except Exception as e:
                raise RecipeError(
                    f"Recipe references unregistered {cat} '{name}': {e}"
                ) from e

        # Tuple skills
        for cat, names, plural in [
            ('decomposer', self.decomposers, 'decomposers'),
            ('initial_solver', self.initial_solvers, 'initial_solvers'),
            ('post_processor', self.post_processors, 'post_processors'),
        ]:
            for n in names:
                try:
                    registry.get(cat, n)
                except Exception as e:
                    raise RecipeError(
                        f"Recipe.{plural} references unregistered "
                        f"{cat} '{n}': {e}"
                    ) from e

        # Parallel modes
        for skill_name, mode in self.parallel_modes.items():
            skill = None
            for cat in registry.all_categories():
                try:
                    s = registry.get(cat, skill_name)
                    skill = s
                    break
                except Exception:
                    continue
            if skill is None:
                raise RecipeError(
                    f"Recipe.parallel_modes references unknown skill "
                    f"'{skill_name}'"
                )
            supported = getattr(skill, 'parallel_modes', ('sequential',))
            if mode not in supported:
                raise RecipeError(
                    f"Skill '{skill_name}' does not support parallel_mode "
                    f"'{mode}'; supported: {supported}"
                )

    def params_for(self, skill_name: str) -> dict:
        return self.skill_params.get(skill_name, {})

    def parallel_mode_for(self, skill_name: str) -> str:
        return self.parallel_modes.get(skill_name, 'sequential')


@dataclass
class RecipeResult:
    """run_recipe 출력."""
    recipe: Recipe
    routed_paths: dict          # net_name -> PathResult
    failed: dict                # net_name -> reason
    residual_overuse: list
    iterations_total: int
    wall_time_s: float
    diagnostics: dict = field(default_factory=dict)

    @property
    def converged(self) -> bool:
        return not self.residual_overuse and not self.failed

    @property
    def n_routed(self) -> int:
        return len(self.routed_paths)

    @property
    def n_failed(self) -> int:
        return len(self.failed)


# ---------------------------------------------------------------------------
# run_recipe — 단일 진입점
# ---------------------------------------------------------------------------


def run_recipe(grid, tasks: list, recipe: Recipe, *,
                registry: Optional[SkillRegistry] = None,
                parallel_ctx: Optional[ParallelContext] = None,
                ) -> RecipeResult:
    """주어진 recipe로 라우팅 실행.

    Parameters
    ----------
    grid : CostGrid (이미 blocker stamped, endpoint unblocked).
    tasks : list[RoutingTask] from net_extractor.
    recipe : 사용할 알고리즘 조합.
    registry : 기본 = 전역 REGISTRY.
    parallel_ctx : 병렬 자원 (None이면 serial).

    Flow:
      1. recipe.validate
      2. PathSolver.preprocess(grid)
      3. Decompose tasks → list[NetGroup]
      4. For each phase (group of NetGroups with same phase number):
           For each group (parallel within phase if backend allows):
             For each task:
               Try each initial_solver, then path_solver
             Negotiate within group
      5. Post-process (in order)
      6. Build RecipeResult.
    """
    import time

    registry = registry or REGISTRY
    recipe.validate(registry)

    if parallel_ctx is None:
        parallel_ctx = ParallelContext(backend='serial', n_workers=1)

    t0 = time.time()

    # 1. PathSolver preprocess
    path_solver = registry.get('path_solver', recipe.path_solver)
    if hasattr(path_solver, 'preprocess'):
        path_solver.preprocess(grid)

    # Phase 50-A: cost-fn config plumbing.
    # The path-solver hot-loop calls cost_fn(grid, ..., request=request)
    # without threading recipe.skill_params through. To deliver per-skill
    # config (e.g. CorridorBiasCost.bias_factor) without changing every
    # path-solver call signature, recipe injects config onto the cost-fn
    # instance via an OPTIONAL .configure(config: dict) method (duck-typed).
    # Cost-fns that implement configure() read from self._config in __call__.
    # Cost-fns without configure() are unaffected (backwards compatible).
    cost_fn_skill = registry.get('cost_fn', recipe.cost_fn)
    if hasattr(cost_fn_skill, 'configure'):
        cost_fn_skill.configure(recipe.params_for(cost_fn_skill.name))

    # 2. Initial solvers (order)
    initial_solvers = [registry.get('initial_solver', n)
                        for n in recipe.initial_solvers]

    # Phase 49-review fix HIGH-#2: pre-run initial solvers PER TASK to
    # collect their PathResult.extra (e.g. GlobalCoarseRouter's
    # 'coarse_corridor_fine_cells'); merge those into the task's NetRule
    # via a side-channel attribute that build_extra surfaces into
    # RoutingRequest.extra. Without this hook, downstream cost-fns that
    # consume initial-solver outputs (CorridorBiasCost) are inert.
    if initial_solvers:
        for tk in tasks:
            for solver in initial_solvers:
                if not hasattr(solver, 'find_initial'):
                    continue
                try:
                    from src.skills.protocols import RoutingRequest as _RR
                    req = _RR(
                        net_name=tk.net_name,
                        source=(tk.source.layer, tk.source.ix, tk.source.iy),
                        sink=(tk.sink.layer, tk.sink.ix, tk.sink.iy),
                        allowed_layers=tuple(tk.rule.layers) if tk.rule else (),
                        width_mm=(getattr(tk.rule, 'width_mm', 0.075)
                                    if tk.rule else 0.075),
                    )
                    pr = solver.find_initial(
                        grid, req,
                        config=recipe.params_for(solver.name),
                    )
                except Exception:
                    pr = None
                if pr is None or not getattr(pr, 'extra', None):
                    continue
                # Stash the initial-solver extras on the task object;
                # build_extra picks them up via task._initial_solver_extras.
                try:
                    existing = getattr(tk, '_initial_solver_extras', {})
                    existing = dict(existing) if existing else {}
                    for k, v in pr.extra.items():
                        existing.setdefault(k, v)
                    object.__setattr__(tk, '_initial_solver_extras', existing)
                except Exception:
                    pass
                # Use the first solver that returned something
                break

    # 3. Decompose
    decomposers = [registry.get('decomposer', n) for n in recipe.decomposers]
    groups: list = []
    if decomposers:
        # 순차 적용 (각 decomposer 출력이 다음 입력)
        # 단순화: 첫 decomposer만 호출, 추가는 future work
        d = decomposers[0]
        groups = d.decompose(tasks, grid,
                              config=recipe.params_for(d.name))
    else:
        # No decomposer → 모든 task을 하나의 group (phase 0)
        groups = [NetGroup(group_id='all', phase=0, tasks=list(tasks))]

    # Group을 phase별로 분류
    phases: dict = {}
    for g in groups:
        phases.setdefault(g.phase, []).append(g)

    # 4. Negotiator
    negotiator = registry.get('negotiator', recipe.negotiator)

    routed: dict = {}
    failed: dict = {}
    iter_total = 0
    residual: list = []

    for phase_num in sorted(phases.keys()):
        phase_groups = phases[phase_num]
        # Group 단위로 negotiate. 같은 phase 내 group은 병렬 가능
        # (단순화: 일단 sequential, parallel은 추후 backend 통합 시).
        for group in phase_groups:
            outcome = negotiator.negotiate(
                grid, group.tasks, path_solver,
                config={
                    **recipe.params_for(negotiator.name),
                    'initial_solvers': initial_solvers,
                },
                parallel_mode=recipe.parallel_mode_for(negotiator.name),
                parallel_ctx=parallel_ctx,
            )
            routed.update(outcome.routed)
            failed.update({k: v for k, v in outcome.failed.items()})
            iter_total += outcome.iterations
            residual.extend(outcome.residual_overuse)

    # 5. Post-process (Phase 34c iterative + Phase 40a re-negotiate loop).
    #
    # Two sidecar channels:
    #   results['__iter_changes__'][pp.name] : int — count of path-mutations
    #     a post-processor applied this round (LengthMatch serpentine etc).
    #   results['__iter_reroute__'] : set[net_name] — Phase 40a — set of
    #     nets the post-processor wants re-routed by the configured
    #     negotiator + path_solver (not patched inline by the post-processor
    #     itself). DRC/Delay/etc. populate this; recipe loop unstamps the
    #     listed nets and re-negotiates that subset using the recipe's
    #     normal negotiator stack so all cost biases (Plane/Impedance/
    #     Crosstalk) are honored on the re-route.
    iter_max = max(1, int(recipe.iter_max))
    pp_skills = [registry.get('post_processor', n)
                 for n in recipe.post_processors]
    iter_rounds_used = 0
    iter_changes_per_round: list = []
    iter_reroutes_per_round: list = []
    for round_idx in range(iter_max):
        # Reset sidecars at the start of every round.
        routed.pop('__iter_changes__', None)
        routed.pop('__iter_reroute__', None)

        for pp in pp_skills:
            pp_cfg = dict(recipe.params_for(pp.name))
            if iter_max > 1:
                pp_cfg.setdefault('iter_round', round_idx)
            # Phase 51-4: surface full task list to post-processors that
            # need it (e.g. PowerGroundPlaneStitcher walks ALL tasks,
            # not just routed_paths, to handle skip_routing nets that
            # were never sent to the negotiator).
            pp_cfg.setdefault('all_tasks', tasks)
            routed = pp.postprocess(grid, routed, config=pp_cfg)

        iter_rounds_used = round_idx + 1
        changes = routed.pop('__iter_changes__', {}) if iter_max > 1 else {}
        reroute = routed.pop('__iter_reroute__', set()) if iter_max > 1 else set()
        iter_changes_per_round.append(changes)
        iter_reroutes_per_round.append(list(reroute))

        if iter_max == 1:
            break

        # Phase 40a: honor reroute requests by unstamp + partial re-negotiate
        # using the recipe's configured negotiator + path_solver so all
        # cost biases (Plane/Impedance/Crosstalk) apply.
        #
        # Diff-pair safety: if a reroute target is half of a pair (rule
        # has pair_partner_net AND the partner is in our task list), pull
        # the partner into the reroute set too — otherwise the partner's
        # stamp blocks the leader's re-route and we get double-stamp /
        # unstamp-missing errors when the negotiator tries to joint-solve.
        if reroute:
            task_by_name_all = {t.net_name: t for t in tasks}
            expanded_reroute = set(reroute)
            for n in list(reroute):
                t = task_by_name_all.get(n)
                if t is None:
                    continue
                partner = getattr(t.rule, 'pair_partner_net', None)
                if partner and partner in task_by_name_all:
                    expanded_reroute.add(partner)
            task_by_name = {n: task_by_name_all[n] for n in expanded_reroute
                              if n in task_by_name_all}
            if task_by_name:
                # Phase 40a v2: unstamp by querying grid for cells that net
                # actually occupies, NOT by trusting PathResult.path (which
                # may have drifted if a post-processor mutated path but
                # forgot to restamp). The grid is the ground truth.
                for net_name in list(task_by_name.keys()):
                    occupied = grid.cells_occupied_by(net_name)
                    if occupied:
                        # Group by layer for unstamp API
                        by_layer: dict = {}
                        for (layer, ix, iy) in occupied:
                            by_layer.setdefault(layer, []).append((ix, iy))
                        for layer, cells in by_layer.items():
                            grid.unstamp(layer, cells, net_name)
                # Re-run negotiator on just these tasks
                reroute_tasks = list(task_by_name.values())
                outcome = negotiator.negotiate(
                    grid, reroute_tasks, path_solver,
                    config={
                        **recipe.params_for(negotiator.name),
                        'initial_solvers': initial_solvers,
                    },
                    parallel_mode=recipe.parallel_mode_for(negotiator.name),
                    parallel_ctx=parallel_ctx,
                )
                # Merge re-routed nets back into the result dict
                for n, new_pr in outcome.routed.items():
                    routed[n] = new_pr
                # Track any failures from the partial re-route
                for n, reason in outcome.failed.items():
                    failed[n] = f'reroute_round_{round_idx}: {reason}'
                iter_total += outcome.iterations

        if not any(int(v) > 0 for v in changes.values()) and not reroute:
            break  # nothing changed and no reroute requested → converged

    # Phase 47-1 fix: when iter_max > 1 the LAST round may have rerouted
    # nets AFTER the post-processor pass; those rerouted PathResults
    # have fresh (empty) extras and lost the diagnostic annotations
    # (e.g. GuardTraceInjector's 'shield' key). Re-run post-processors
    # once more in diagnostic-only mode so the final result is consistent.
    # mutate=False is forced to prevent further history bumps / reroute
    # requests on this consistency pass.
    if iter_max > 1 and pp_skills:
        routed.pop('__iter_changes__', None)
        routed.pop('__iter_reroute__', None)
        for pp in pp_skills:
            pp_cfg = dict(recipe.params_for(pp.name))
            pp_cfg['mutate'] = False
            pp_cfg.setdefault('iter_round', iter_max)
            # Phase 51-review fix MEDIUM-#5: also surface all_tasks here
            # so PowerGroundPlaneStitcher (which depends on it) doesn't
            # silently no-op on the final consistency pass.
            pp_cfg.setdefault('all_tasks', tasks)
            try:
                routed = pp.postprocess(grid, routed, config=pp_cfg)
            except Exception:
                pass   # final pass is best-effort; never fail the recipe
        routed.pop('__iter_changes__', None)
        routed.pop('__iter_reroute__', None)

    # 6. Diagnostics (registered 'diagnostic' skills 자동 실행 옵션)
    diag: dict = {}
    for diag_name in registry.list('diagnostic'):
        try:
            d = registry.get('diagnostic', diag_name)
            diag[diag_name] = d.diagnose(grid, routed, tasks)
        except Exception:
            # 진단은 실패해도 결과에 영향 X
            diag[diag_name] = {'error': 'diagnostic raised'}

    if iter_max > 1:
        diag['__iter__'] = {
            'iter_max': iter_max,
            'rounds_used': iter_rounds_used,
            'changes_per_round': iter_changes_per_round,
            'reroutes_per_round': iter_reroutes_per_round,
        }

    # Phase E-2 — populate PathResult.via_metadata on every routed net.
    # Best-effort, never raises: derive from path + net classification.
    try:
        from bga_router.metrics.path_geometry import derive_via_metadata
        for net_name, pr in routed.items():
            if getattr(pr, 'via_metadata', None):
                continue  # already populated upstream
            path = getattr(pr, 'path', None)
            if path:
                try:
                    pr.via_metadata = derive_via_metadata(path, grid, net_name)
                except Exception:
                    pass
    except Exception:
        # bga_router not on sys.path (legacy callers); skip — eval will
        # re-derive when needed.
        pass

    return RecipeResult(
        recipe=recipe,
        routed_paths=routed,
        failed=failed,
        residual_overuse=residual,
        iterations_total=iter_total,
        wall_time_s=time.time() - t0,
        diagnostics=diag,
    )
