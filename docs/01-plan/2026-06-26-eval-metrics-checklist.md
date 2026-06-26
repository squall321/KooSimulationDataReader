# Eval metrics completeness — checklist

Companion to `2026-06-26-eval-metrics-completeness-plan.md`. Tick as
each item lands. Keep this file as the single source of truth for
"what's done".

## Phase 0 — investigation (gate Phase A)

- [ ] Map `AStarResult` / routed-path data structure (where lives, what fields)
- [ ] Locate `extract_extended_metrics` call site + return contract
- [ ] Inventory existing metric-like computations in post-processors:
      `manufacturing_drc.py`, `length_match_post_processor.py`,
      `diff_pair_post_processor.py`, `propagation_delay_match.py` —
      lift, don't reimplement
- [ ] Pick the integration seam (single function that gets called with
      `(routed_paths, NetRule_map, grid, stackup)` and returns dict)
- [ ] Decision: in-tree module path. Candidate: `bga_router/metrics/`

## Phase A — geometry

`bga_router/metrics/geometry.py` (new module).

- [ ] `total_length_mm(path, pitch_mm) -> float`
- [ ] `length_per_layer_mm(path, pitch_mm) -> dict[str, float]`
- [ ] `via_count_per_net(path) -> int` (lift from AStarResult)
- [ ] `layer_transition_count(path) -> int`
- [ ] `sharp_bends(path, threshold_deg=135) -> int`
- [ ] `acute_bends(path, threshold_deg=90) -> int`
- [ ] `bend_density_per_mm(path, pitch_mm) -> float`
- [ ] `bbox_straightness(path) -> float`
- [ ] `routing_efficiency(path, start_xy, end_xy, pitch_mm) -> float`
- [ ] `detour_count(path) -> int`
- [ ] `escape_side_fidelity_pct(path, requested_sides) -> float`
- [ ] `channel_congestion_max(all_paths, grid) -> int`
- [ ] `shadow_crossings_2d(all_paths) -> int`
- [ ] `layer_utilization(all_paths, grid) -> dict[str, float]`
- [ ] Expose `residual_overuse` from base dict into output JSON
- [ ] Wire into `extract_extended_metrics` under key `geometry.*`
- [ ] Unit tests: synthetic 4-segment path with hand-computed values

## Phase B — NetRule vs result verifier

`bga_router/metrics/verifier.py` (new module).

API: `verify(routed_paths: dict[str, Path], rules: dict[str, NetRule],
all_paths, grid, stackup) -> dict[str, RuleCheckResult]`

- [ ] `layers_membership_ok` — every segment.layer ∈ rule.layers
- [ ] `escape_side_ok` — first exit direction vs rule.escape_sides
- [ ] `width_ok` — rendered width matches rule.width_mm (placeholder
      until per-segment width tracking lands; bool always true for now,
      flag in context-notes)
- [ ] `pair_gap_ok` — partner-net perpendicular distance vs pair_gap_mm
- [ ] `pair_skew_ok` — abs(len(P)-len(N)) ≤ rule.pair_skew_max_mm
- [ ] `clearance_ok` — same-layer other-net polyline min distance
      ≥ rule.clearance_mm. Uses STRtree spatial index.
- [ ] `length_group_ok` — length_match_group max-min ≤ tolerance
- [ ] `via_budget_ok` — via_count ≤ rule.max_via_count
- [ ] `min_length_ok` — total_length ≥ rule.min_trace_length_mm
- [ ] `shield_present_ok` — rule.shield_required → both-side GND polylines exist
- [ ] `keep_out_ok` — path ∩ rule.keep_out_zones = ∅
- [ ] `topology_ok` — net_topology (daisy/star/tee) matches routed graph
- [ ] `split_avoidance_ok` — plane-split overlap == 0 when avoidance requested
- [ ] `bend_class_ok` — bend_class threshold (90° / 45°) honored
- [ ] `impedance_target_ok` — analytical Z0 (from Phase C) within tol
- [ ] `preferred_layer_pct` — soft metric (0..1)
- [ ] Aggregate `violations` count
- [ ] Per-violation: `{net, field, expected, actual, why}` payload
- [ ] Wire into `extract_extended_metrics` under key `rule_check.*`
- [ ] Unit tests: 1 PASS + 1 FAIL synthetic case per rule

## Phase C — analytical SI + standards

`bga_router/metrics/stackup.py` + `bga_router/metrics/si.py`.

### C.1 — StackupSpec
- [ ] `StackupSpec` dataclass (per-layer t_mm, h_mm, εr, role, refs)
- [ ] `load_stackup(yaml_path) -> StackupSpec`
- [ ] Default 4-layer FR-4 fallback when YAML absent
- [ ] `stackup_default_used: true` flag in output

### C.2 — closed-form impedance
- [ ] `z0_microstrip_wadell(w, t, h, er) -> float`
- [ ] `z0_stripline_wadell(w, t, h, er) -> float`
- [ ] `zdiff_microstrip_wadell(w, t, h, s, er) -> float`
- [ ] `zdiff_stripline_wadell(w, t, h, s, er) -> float`
- [ ] `z0_for_net(path, stackup, width_mm) -> float`
- [ ] `z0_variance_pct(path_z0_segments) -> float`
- [ ] `via_stub_length_mm(path, stackup) -> float`
- [ ] Mark `z0_formula_marginal: true` when s/h < 0.5 or h < 50 μm

### C.3 — return path + DC
- [ ] `return_path_continuity_score(path, plane_layers) -> float`
- [ ] `plane_split_crossings(path, plane_split_polys) -> int`
- [ ] `reference_plane_changes_no_stitch(path, plane_layers, vias) -> int`
- [ ] `nearest_return_via_distance_mm(path, vias) -> float`
- [ ] `branch_dc_resistance_mohm(path, copper_thickness, width) -> float`

### C.4 — standard checks
- [ ] `check_ddr4(metrics) -> StandardResult`
- [ ] `check_pcie_gen3(metrics) -> StandardResult`
- [ ] `check_usb32(metrics) -> StandardResult`
- [ ] `check_hdmi(metrics) -> StandardResult` (optional)
- [ ] Net-class → standard mapping (via NetRule.net_topology or naming)
- [ ] Wire into output under `si.*` + `standard.*`

### C.5 — tests + docs
- [ ] Wadell formulas: verify against published reference values
      (Polar Si9000 table — pick 5 known stackups, compute, assert ±2%)
- [ ] Property test: Z0 monotonic with w (narrower → higher Z0)
- [ ] DDR4 pass/fail: feed in a known-good DQ trace, must pass

## Phase D — EM queue hooks (NOT inline)

- [ ] `em_queue` block in output JSON listing (net, region_bbox, layers)
      for nets that failed `impedance_target_ok` or have
      `z0_formula_marginal: true`
- [ ] Document downstream consumption: `simulation/sol_d_cross_section/`
      can read the queue and run per-net

## CLI

- [ ] `route` / `eval` accept `--checks=geometry,rule_check,si,standard`
      (default: all)
- [ ] `route` / `eval` accept `--stackup <path/to/stackup.yaml>` override
- [ ] Aggregator gains `routed_ratio_with_rule_pass` column
      (= routed AND rule_check.violations == 0)

## Validation gate

- [ ] Existing 9 metrics byte-identical on `P3_EUR_REV03 bga218f` baseline
- [ ] All 6 existing recipes still pass the regression bar
- [ ] New tests added for every Phase A/B/C field above
- [ ] Aggregator markdown report renders without column overflow

## Out of scope (explicitly NOT in this PR)

- Full-wave EM solver inline (deferred to Phase D queue consumption)
- Thermal / EMI radiation
- AOI / test-point access
- 3D field-solver Z0 (Wadell is the target for this PR)
- Multi-stackup interpolation (single stackup per dataset for now)
