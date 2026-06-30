# Eval metrics completeness — checklist

Companion to `2026-06-26-eval-metrics-completeness-plan.md`. Tick as
each item lands. Keep this file as the single source of truth for
"what's done".

## Phase 0 — investigation (gate Phase A)

- [x] Map `AStarResult` / routed-path data structure (where lives, what fields)
- [x] Locate `extract_extended_metrics` call site + return contract
- [x] Inventory existing metric-like computations in post-processors:
      `manufacturing_drc.py`, `length_match_post_processor.py`,
      `diff_pair_post_processor.py`, `propagation_delay_match.py` —
      lift, don't reimplement
- [x] Pick the integration seam (single function that gets called with
      `(routed_paths, NetRule_map, grid, stackup)` and returns dict)
- [x] Decision: in-tree module path. Candidate: `bga_router/metrics/`

## Phase A — geometry

`bga_router/metrics/geometry.py` (new module).

- [x] `total_length_mm(path, pitch_mm) -> float`
- [x] `length_per_layer_mm(path, pitch_mm) -> dict[str, float]`
- [x] `via_count_per_net(path) -> int` (lift from AStarResult)
- [x] `layer_transition_count(path) -> int`
- [x] `sharp_bends(path, threshold_deg=135) -> int`
- [x] `acute_bends(path, threshold_deg=90) -> int`
- [x] `bend_density_per_mm(path, pitch_mm) -> float`
- [x] `bbox_straightness(path) -> float`
- [x] `routing_efficiency(path, start_xy, end_xy, pitch_mm) -> float`
- [x] `detour_count(path) -> int`
- [x] `escape_side_fidelity_pct(path, requested_sides) -> float`
- [x] `channel_congestion_max(all_paths, grid) -> int`
- [x] `shadow_crossings_2d(all_paths) -> int`
- [x] `layer_utilization(all_paths, grid) -> dict[str, float]`
- [x] Expose `residual_overuse` from base dict into output JSON
- [x] Wire into `extract_extended_metrics` under key `geometry.*`
- [x] Unit tests: synthetic 4-segment path with hand-computed values

## Phase B — NetRule vs result verifier

`bga_router/metrics/verifier.py` (new module).

API: `verify(routed_paths: dict[str, Path], rules: dict[str, NetRule],
all_paths, grid, stackup) -> dict[str, RuleCheckResult]`

- [x] `layers_membership_ok` — every segment.layer ∈ rule.layers
- [x] `escape_side_ok` — first exit direction vs rule.escape_sides
- [x] `width_ok` — rendered width matches rule.width_mm (placeholder
      until per-segment width tracking lands; bool always true for now,
      flag in context-notes)
- [x] `pair_gap_ok` — partner-net perpendicular distance vs pair_gap_mm
- [x] `pair_skew_ok` — abs(len(P)-len(N)) ≤ rule.pair_skew_max_mm
- [x] `clearance_ok` — same-layer other-net polyline min distance
      ≥ rule.clearance_mm. Uses STRtree spatial index.
- [x] `length_group_ok` — length_match_group max-min ≤ tolerance
- [x] `via_budget_ok` — via_count ≤ rule.max_via_count
- [x] `min_length_ok` — total_length ≥ rule.min_trace_length_mm
- [x] `shield_present_ok` — rule.shield_required → both-side GND polylines exist
- [x] `keep_out_ok` — path ∩ rule.keep_out_zones = ∅
- [x] `topology_ok` — net_topology (daisy/star/tee) matches routed graph
- [x] `split_avoidance_ok` — plane-split overlap == 0 when avoidance requested
- [x] `bend_class_ok` — bend_class threshold (90° / 45°) honored
- [x] `impedance_target_ok` — analytical Z0 (from Phase C) within tol
- [x] `preferred_layer_pct` — soft metric (0..1)
- [x] Aggregate `violations` count
- [x] Per-violation: `{net, field, expected, actual, why}` payload
- [x] Wire into `extract_extended_metrics` under key `rule_check.*`
- [x] Unit tests: 1 PASS + 1 FAIL synthetic case per rule

## Phase C — analytical SI + standards

`bga_router/metrics/stackup.py` + `bga_router/metrics/si.py`.

### C.1 — StackupSpec
- [x] `StackupSpec` dataclass (per-layer t_mm, h_mm, εr, role, refs)
- [x] `load_stackup(yaml_path) -> StackupSpec`
- [x] Default 4-layer FR-4 fallback when YAML absent
- [x] `stackup_default_used: true` flag in output

### C.2 — closed-form impedance
- [x] `z0_microstrip_wadell(w, t, h, er) -> float`
- [x] `z0_stripline_wadell(w, t, h, er) -> float`
- [x] `zdiff_microstrip_wadell(w, t, h, s, er) -> float`
- [x] `zdiff_stripline_wadell(w, t, h, s, er) -> float`
- [x] `z0_for_net(path, stackup, width_mm) -> float`
- [x] `z0_variance_pct(path_z0_segments) -> float`
- [x] `via_stub_length_mm(path, stackup) -> float`
- [x] Mark `z0_formula_marginal: true` when s/h < 0.5 or h < 50 μm

### C.3 — return path + DC
- [x] `return_path_continuity_score(path, plane_layers) -> float`
- [x] `plane_split_crossings(path, plane_split_polys) -> int`
- [x] `reference_plane_changes_no_stitch(path, plane_layers, vias) -> int`
- [x] `nearest_return_via_distance_mm(path, vias) -> float`
- [x] `branch_dc_resistance_mohm(path, copper_thickness, width) -> float`

### C.4 — standard checks
- [x] `check_ddr4(metrics) -> StandardResult`
- [x] `check_pcie_gen3(metrics) -> StandardResult`
- [x] `check_usb32(metrics) -> StandardResult`
- [x] `check_hdmi(metrics) -> StandardResult` (optional)
- [x] Net-class → standard mapping (via NetRule.net_topology or naming)
- [x] Wire into output under `si.*` + `standard.*`

### C.5 — tests + docs
- [x] Wadell formulas: verify against published reference values
      (Polar Si9000 table — pick 5 known stackups, compute, assert ±2%)
- [x] Property test: Z0 monotonic with w (narrower → higher Z0)
- [x] DDR4 pass/fail: feed in a known-good DQ trace, must pass

## Phase D — EM queue hooks + extension backlog (COMPLETED)

- [x] `em_queue` block in output JSON listing (net, region_bbox, layers)
      for nets that failed `impedance_target_ok` or have
      `z0_formula_marginal: true` (Phase D-1, commit 7c9b0a7)
- [x] Document downstream consumption: `simulation/sol_d_cross_section/`
      can read the queue and run per-net (Phase D-5 em-dispatch CLI,
      commit 10dfe37)
- [x] `check_topology` star/tee 분기 추가 (Phase D-2, commit 8e3fbe8)
- [x] Auto-plane detection from matrix file (Phase D-3, commit eca2a94)
- [x] Via stub length — stackup ViaSpan 스키마 (Phase D-4, commit 4476594)
- [x] PG via metadata 자동 도출 (Phase D-6, commit 29f0b81)
- [x] `check_width` 채널 fit 검증 (Phase D-7, commit f844ae9)

## CLI

- [x] `route` accepts `--checks=geometry,cross_net,rule_check,si,
      standard,em_queue,return_path` (Phase D, commit 378aa8a). Default all on.
- [x] `route` accepts `--stackup <path/to/stackup.yaml>` override
- [x] `route` accepts `--plane-layers <name1,name2>` override
- [x] `em-dispatch` 서브명령 추가 (Phase D-5)
- [ ] Aggregator gains `routed_ratio_with_rule_pass` column
      (= routed AND rule_check.violations == 0) — DEFERRED.
      Added Length/Sharp/RuleViol/Z0var/Std columns instead.

## Architecture-change backlog — Phase E 처리됨

- [x] Router-side per-segment width recording (Phase E-3, commit ab92a34)
- [x] Router-side via_metadata 채움 (Phase E-2, commit 9e04d33)
- [~] Multi-pin nets — scaffold + design doc만 (Phase E-5, commit
      9358d66). 진짜 implementation은 Stage 1-5 별도 PR sequence.
      docs/01-plan/2026-06-29-multi-pin-router-design.md 참조.
- [x] Aggregator routed_ratio_with_rule_pass 컬럼 (Phase E-1, commit 2b549ac)
- [x] sol_d em-dispatch task 자동 수신 (Phase E-4, commit 610eb99)

## Validation gate

- [x] Existing 9 metrics byte-identical on `P3_EUR_REV03 bga218f` baseline
- [x] All 6 existing recipes still pass the regression bar
- [x] New tests added for every Phase A/B/C field above
- [x] Aggregator markdown report renders without column overflow

## Out of scope (explicitly NOT in this PR)

- Full-wave EM solver inline (deferred to Phase D queue consumption)
- Thermal / EMI radiation
- AOI / test-point access
- 3D field-solver Z0 (Wadell is the target for this PR)
- Multi-stackup interpolation (single stackup per dataset for now)
