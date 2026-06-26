# Eval metrics completeness — context notes

Append-only log of decisions made during the work and the reasoning
behind each. Next session — yours or someone else's — should be able
to pick up without re-deriving any of these.

## 2026-06-26 — kickoff

### Why three phases in one PR
The user explicitly said "PCB의 일반 분석을 모두 스코프에 넣고 싶다".
Splitting into 3 PRs would force the verifier (B) to import
geometry (A) at a half-finished state and the standards check (C) to
fake-import skew (B). One PR with three internal modules keeps the
integration test honest.

If the diff becomes too large for review, the splittable sub-PRs
listed in section 7 of the plan are the fallback. We do NOT split
preemptively.

### Why not Phase D inline
EM solver runs (Sol B PEEC / Sol D 2D-FD) take seconds to minutes per
net. `eval` is a sub-second loop today; making it minutes would force
the user to re-architect around async/queues. We add only a queue
hook (emit `em_queue` list) so eval stays sub-second and downstream
tooling can drain it.

### Why a new module path `bga_router/metrics/`
The existing `bga_router/eval.py` is 70 lines of orchestration; it
shouldn't grow to 800. Putting geometry / verifier / si / stackup as
sibling modules keeps each under ~300 lines and makes them
independently testable. Each metric function gets imported, not
implemented inline.

### Wadell vs Hammerstad-Jensen vs 2D field solver
Closed-form options for Z0:
- Hammerstad-Jensen (1980) — accurate to ±1% on microstrip in the
  common-stackup regime.
- Wadell (1991) — broader coverage (stripline, embedded microstrip,
  coupled), accuracy ±2-5% on standard stackups.
- IPC-2141 formulas — older, ±10%, mostly superseded.

Choosing Wadell because:
- Coverage matches our needs (single + diff, microstrip + stripline).
- Closed-form, no iteration.
- Per-formula `marginal` flag for the regime where it's known
  imprecise (s/h < 0.5 tightly-coupled, h < 50 μm).

User opting for higher accuracy uses the `em_queue` → Sol D path.

### Stackup default
If no `stackup.yaml` ships with a dataset, we default to a generic
4-layer FR-4 stackup (35 μm Cu, 100 μm dielectric, εr=4.2) and set
`stackup_default_used: true` in the output. This is honest about
"these Z0 numbers are nominal" without forcing the user to write a
stackup just to get geometry/rule_check metrics.

Reason this default isn't bigger (8-layer / HDI): the BGA escape PoC
datasets (`P3_EUR_REV03`, `rigidflex`) are 2-layer escapes effectively.
A 4-layer microstrip+stripline default exercises both formulas and
covers them.

### Clearance check uses Shapely STRtree
For 1000-net designs, naive O(N²) all-pairs distance is 1M comparisons.
Shapely's STRtree is O(N log N) for spatial queries and is already a
transitive dep (used by `cost_grid`). Acceptable for the target scale.

### NetRule fields we cannot fully verify yet
- `width_ok`: the router currently doesn't track per-segment rendered
  width — width is only used as a soft cost in routing. We ship
  `width_ok` returning `pass: true, note: "width tracking not yet
  per-segment"` and flag it in this notes file as a follow-up.
- `topology_ok` for star/tee topologies: the routed graph has the
  vertex structure but `net_topology` field on NetRule is a string
  enum; we need a topology-classification function. Initial
  implementation handles `daisy_chain` only; others return `N/A` with
  reason.

These limitations are documented in the test cases too — they assert
the explicit `note` field rather than silently passing.

### Why `rule_check.by_field` is a dict not a list
Aggregators (and human readers) want to ask "did clearance pass?"
without iterating. `by_field["clearance_ok"]` is the natural shape.
Violators are listed inside each entry.

### Why CLI flag `--checks` defaults to ALL
The whole point of this work is to expose the metrics. Opt-out (default
ON) for any field that's CPU-heavy in the future is the better
direction than opt-in (default OFF). If clearance check becomes too
slow on 1000-net designs, we make `clearance_ok` opt-out via
`--checks=geometry,rule_check:basic,si,standard` syntax.

### Test fixture strategy
The unit-test fixtures are HAND-CRAFTED 2-net / 4-net path lists in
Python — NOT full ODB++ datasets. Reasons:
- Per-metric expected value can be computed by hand and checked to 1e-9.
- A 200-line synthetic-fixture test runs in 5 ms; a P3 full-dataset
  test runs in seconds. Unit budget matters.
- Real-dataset regression tests go in `tests/regression/` and run on
  `make ci`, not in the per-commit loop.

### Existing post-processor metrics we LIFT, not reimplement
Already computed inside post-processors but never propagated to
`metrics`:
- `length_match_post_processor.py` computes per-group skew → goes
  directly into `rule_check.length_group_ok` and into `pair_skew_ok`.
- `manufacturing_drc.py` has clearance and acid-trap detectors →
  feeds `rule_check.clearance_ok` and `geometry.acute_bends`.
- `diff_pair_post_processor.py` measures coupling length and
  uncoupled percentage → feeds future `si.coupling_length_mm`
  (cataloged but not in this PR's deliverable list).
- `propagation_delay_match.py` computes per-trace delay → not yet
  needed; cataloged for Phase D when timing-domain metrics get added.

The pattern: each post-processor exposes a `get_diagnostics() -> dict`
that the new `metrics/` modules call. We do NOT duplicate the
algorithms.

### Decisions deferred to runtime (not pre-committed)
- The exact mapping from NetRule.net_topology / net naming → which
  standard check to run. First pass: prefix match (DDR4_*, PCIE_*,
  USB3_*). Will revisit when a real dataset with named nets shows up.
- Stackup auto-detection from ODB++ stackup file (when present in
  `steps/<step>/stackup`) — Phase C currently uses the per-dataset
  YAML override. If ODB++ stackup readers land before this PR ships,
  we wire it through.

### Naming convention for new modules
- `bga_router/metrics/geometry.py`
- `bga_router/metrics/verifier.py`
- `bga_router/metrics/si.py`
- `bga_router/metrics/stackup.py`
- `bga_router/metrics/standards.py`
- `bga_router/metrics/__init__.py` re-exports the public API only.

Each module has a 1-line Korean header per CLAUDE.md rule 6.

### Verification cadence
- After each Phase: run full `pytest` suite + 1 baseline regression
  (`route --dataset P3_EUR_REV03 --bga bga218f --recipe default`).
- Existing 9 metrics must be byte-identical (regression bar).
- New metrics: per-field unit tests must pass.

### Commit cadence
Per CLAUDE.md rule 9 (semantic commits): one commit per logical chunk.
Plan:
1. `eval metrics: Plan + Checklist + Context Notes 추가`
2. `eval metrics: geometry 모듈 + placeholder 5건 채움` (Phase A)
3. `eval metrics: verifier.py 골격 + 쉬운 rule_check 5건` (Phase B1)
4. `eval metrics: clearance + topology + impedance check` (Phase B2)
5. `eval metrics: StackupSpec + Wadell Z0 single/diff` (Phase C1)
6. `eval metrics: return path + DC + standards` (Phase C2)
7. `eval metrics: CLI --checks/--stackup + aggregator 컬럼` (wiring)

Each commit must pass tests on its own (don't merge a broken
intermediate state).

## 2026-06-26 — Phase 0 investigation findings + 결정 5건

### Path 데이터 구조 (확정)

`PathResult` (`src/skills/protocols.py:44-52`), `path` 필드는
`list[(layer:str, ix:int, iy:int)]` — **grid 셀 좌표**, mm 아님.
mm 변환은 `grid.geom.cell_to_world(ix, iy)`, pitch 는 `grid.geom.cell_mm`
단일 필드. Via 는 명시 엔트리가 아니라 연속 셀의 layer 변경으로 암묵
표현 (`single_a_star.py:74-79`).

`RecipeResult.routed_paths: dict[net_name → PathResult]`
(`src/recipe.py:127`). Via_count 는 `PathResult.via_count` 에 이미 집계됨.

### 통합 seam (확정)

`bga_router/eval.py:138` 한 줄이 metrics 채우는 지점.
현재 거기 scope: `recipe`, `ctx`, `base`(dict) 뿐. `grid`, `routed_paths`,
`tasks` 는 모두 `evaluate_recipe` 내부에서 폐기됨.

**해결**: `evaluate_recipe` 반환 dict 에 `grid`, `routed_paths`, `tasks`
추가 thread (추가-only, 기존 caller 영향 없음). 이후 `eval.py:138` 에서
새 verifier 호출에 forwarding.

### NetRule 필드 수 정정 (25 → 28)

초기 계획서는 25 필드라고 했지만 `src/routing_spec.py` 실측 결과 **28**.
추가 발견: `rule_source`(메타), `coupled_allow_via`(capability),
`coupled_via_budget`(verify 가능), `crosstalk_aware`/`crosstalk_radius_cells`(3W
검사).

검증 분류 최종.

- **25 OK** — verifier 즉시 가능 (Phase B 스코프).
- **1 Phase C** — `impedance_target_ohm` (stackup 의존).
- **2 BLOCKED-then-resolved** — `via_type`, `split_avoidance` (plane geometry
  필요했으나 plane loader 가 이번 PR 에 포함되면서 unblock).

### 결정 1: Plane geometry loader 이번 PR 포함 (사용자 결정)

사용자가 명시적으로 "이번 PR 에 포함" 선택. PR 크기 1.5~2배가 되지만
"PCB 일반 분석 모두 스코프" 의 약속을 95% → 100% 로 끌어올림. plane
polygon ODB++ features 파일에서 regex 로 파싱 (~150 LOC, src/ecad/plane_loader.py
신설). C++ 바인딩은 우회.

영향 — Phase C 에 `plane_split_crossings`,
`reference_plane_changes_no_stitch`, `return_path_continuity_score`,
`split_avoidance_ok` 가 즉시 산출 가능 항목으로 승격.

### 결정 2: C++ pybind11 vs Python regex (선택 — regex)

BGA 라우터는 현재 C++ 바인딩이 0개. pybind11 추가는 새 빌드 의존성
도입. Regex 파서는 surgical — ODB++ features 파일 형식이 안정적
(L/P/A/T/S/SE/B 6 종) 이라 정합성 리스크 낮음. 향후 binding 도입
시 regex 모듈을 호환 wrapper 로 유지.

### 결정 3: Stackup YAML 경로 규약 (선택 — registry.yaml 키)

`examples/ECAD/p3_stackup.yaml` 이 이미 존재 (8층 FR-4, εr/t/tanδ 완비).
C++ `OdbToStepConverter.cpp:806-847` 가 같은 스키마를 이미 읽는 중.
`datasets/registry.yaml` 에 `stackup_yaml: path/to/stackup.yaml` 키 추가.
부재 시 default 4-layer FR-4 fallback + `stackup_default_used: true`.

### 결정 4: `evaluate_recipe` 시그니처 변경 — 추가-only

`scripts/bga_eval.py:410` legacy main 이 반환 dict 키를 가정하는지
확인. 추가-only (기존 키 유지, 새 키만 추가) 면 안전. 첫 commit
직전 grep 으로 caller 사이드 안전성 확인.

### 결정 5: length_match 재구현 (선택 — 재구현)

`length_match_post_processor` 클래스는 mutate 시에만 로깅하고
in-spec 그룹에 침묵. `_build_groups` + `_path_length_mm` 헬퍼만 차용해
verifier 가 직접 group spread 계산. ~10 LOC. 향후 그룹화 규칙
복잡해지면 클래스 호출로 회귀 결정 가능.

### Post-processor lift 패턴 (확정)

| 모듈 | 권장 | 비고 |
| --- | --- | --- |
| `manufacturing_drc` | **CALL mutate=False** | `_detect_bends`, `_via_drc` pure 함수. `extra['drc']` 에 bend/via 결과 |
| `length_match_post_processor` | **REIMPLEMENT ~10 LOC** | 결정 5 |
| `diff_pair_post_processor` | **CALL mutate=False** | `extra['diff_pair']` 에 skew_mm/tolerance/in_spec 항상 기록 |
| `propagation_delay_match` | **CALL mutate=False** | 모듈 자체가 "Diagnostic-only", `extra['delay_match']` |

부수 효과 — `_path_length_mm` 가 3개 모듈에 verbatim 중복.
`bga_router/metrics/path_geometry.py` 로 공용 promote 권장 (Phase A 첫 작업).

### 사이드카 키 오염 (주의)

post-processor 가 mutate=False 라도 `extra['__iter_*']` 같은 진단 키를
남길 수 있음. verifier 가 `__`-prefix strip 을 일관 적용해야 함.
verifier 모듈의 `_clean_extra(d) -> dict` 헬퍼로 처리.

### Phase 순서 갱신

1. **Phase A (인프라)** — `evaluate_recipe` 반환 확장, `eval.py:138`
   wiring, `metrics/path_geometry.py` 공용 헬퍼.
2. **Phase B (path-only)** — geometry + rule_check (NetRule 25 필드).
3. **Phase C (stackup-필요)** — `stackup_loader.py` + Wadell Z0 +
   impedance_target_ok + standard.* 체크.
4. **Phase C2 (plane-필요)** — `plane_loader.py` (regex) +
   return-path 메트릭 4 + via_type / split_avoidance verifier.
5. **CLI + aggregator wiring**.
