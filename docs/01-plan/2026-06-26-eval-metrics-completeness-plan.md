# Routing eval completeness — Phase A/B/C plan

**Date**: 2026-06-26
**Scope**: Add the full set of real-PCB routing evaluation metrics to
`bga_router` eval output. User intent: "PCB의 일반 분석을 모두 스코프에 넣고 싶다".

## TL;DR

Today the eval JSON exposes 9 useful metrics and leaves 5 advertised
placeholders as `null`. **Zero** of NetRule's 25 declared constraints
are re-verified against the routed result. The advertised
`drc_in_spec`/`pair_skew_max_mm`/`n_pairs_coupled` mean nothing.

This plan adds three layers in one PR (Phase D — full-wave EM solver
— stays deferred and gets queue hooks only):

- **Phase A** — geometry (`geometry.*`) — placeholder fill + new
  derived metrics. ~15 fields. Effort: S.
- **Phase B** — NetRule vs result auto-verify (`rule_check.*`) — 16
  fields covering every NetRule constraint. Effort: M.
- **Phase C** — analytical SI (`si.*`, `standard.*`) — closed-form
  Z0/Zdiff + standard pass/fail (DDR4/PCIe Gen3/USB3.2). ~10 fields.
  Effort: M.

Total: ~40 new evaluation fields. No new external solver dependency.

## Why one PR

These three phases are coupled in code paths:
- The verifier (B) needs the geometry metrics (A) as inputs.
- The standards pass/fail (C) reads both length (A) and skew (B).
- Splitting buys nothing because the touched files overlap heavily.

If size becomes a review concern, splittable axes are documented in
section 7.

## Output JSON schema additions

```jsonc
{
  "metrics": {
    // existing 9 stay unchanged
    "routed": 218, "total": 218, "routed_ratio": 1.0,
    "elapsed_s": 1.234, "via_count": 421, "iterations": 7,
    "failed": 0, "converged": true, "timeout": false,

    // PHASE A — geometry
    "geometry": {
      "total_length_mm":            42.71,
      "length_per_layer_mm":        {"COMP": 28.31, "LAY2": 14.40},
      "layer_transition_count":     421,
      "sharp_bends":                12,         // < 135°
      "acute_bends":                0,          // < 90°
      "bend_density_per_mm":        0.28,
      "bbox_straightness":          0.71,       // path_bbox_area / route_bbox_ideal
      "routing_efficiency":         1.18,       // actual_len / xy_manhattan
      "detour_count":               2,
      "escape_side_fidelity_pct":   1.0,        // matches requested escape_sides
      "channel_congestion_max":     8,
      "shadow_crossings_2d":        0,
      "layer_utilization":          {"COMP": 0.31, "LAY2": 0.18},
      "residual_overuse":           0           // exposed from base dict
    },

    // PHASE B — rule check (one bool + reason per NetRule field)
    "rule_check": {
      "violations":                 3,
      "by_field": {
        "layers_membership_ok":     {"pass": true,  "violators": []},
        "escape_side_ok":           {"pass": true,  "violators": []},
        "width_ok":                 {"pass": false, "violators": ["net042"]},
        "pair_gap_ok":              {"pass": true,  "violators": []},
        "pair_skew_ok":             {"pass": false, "violators": ["DQ0_P/N"]},
        "clearance_ok":             {"pass": true,  "violators": []},
        "length_group_ok":          {"pass": true,  "violators": []},
        "via_budget_ok":            {"pass": true,  "violators": []},
        "min_length_ok":            {"pass": true,  "violators": []},
        "shield_present_ok":        {"pass": true,  "violators": []},
        "keep_out_ok":              {"pass": true,  "violators": []},
        "topology_ok":              {"pass": true,  "violators": []},
        "split_avoidance_ok":       {"pass": true,  "violators": []},
        "bend_class_ok":            {"pass": true,  "violators": []},
        "impedance_target_ok":      {"pass": false, "violators": ["net077"]},
        "preferred_layer_pct":      0.91
      }
    },

    // PHASE C — analytical SI + standards
    "si": {
      "Z0_single_ended_ohm":        {"net042": 50.3, "net077": 41.2},
      "Zdiff_ohm":                  {"DQ0": 99.8},
      "Z0_variance_pct":            2.1,
      "via_stub_length_mm":         {"net042": 0.0},
      "return_path_continuity_score": 0.97,
      "plane_split_crossings":      0,
      "reference_plane_changes_no_stitch": 1,
      "nearest_return_via_distance_mm": {"net042": 1.20},
      "branch_dc_resistance_mohm":  {"VDD_CORE": 2.8}
    },
    "standard": {
      "ddr4_ok":   {"pass": true,  "checks": {"skew": true, "z0": true, "via": true}},
      "pcie_gen3_ok": null,
      "usb32_ok":  null
    }
  }
}
```

Backward compatibility: all existing keys stay where they are. The
aggregator's columns are unchanged; new columns are opt-in.

## Stackup input (Phase C prerequisite)

`Z0_single_ended_ohm` / `Zdiff_ohm` need: trace width `w`, copper
thickness `t`, dielectric height `h`, relative permittivity `εr`,
diff-pair gap `s`. NetRule already carries `width_mm` and
`pair_gap_mm`. The other three need a stackup source.

**Decision**: ship a `StackupSpec` Python dataclass + optional
`stackup.yaml` per dataset. Default to a generic 4-layer FR-4 stackup
when the file is absent, with a `stackup_default_used: true` flag in
the output so users know the Z0 numbers are nominal.

```yaml
# datasets/<name>/stackup.yaml
layers:
  - {name: COMP, type: signal, t_mm: 0.035, h_mm: 0.10, er: 4.2, role: microstrip}
  - {name: GND1, type: plane,  t_mm: 0.035}
  - {name: LAY2, type: signal, t_mm: 0.035, h_mm: 0.12, er: 4.2, role: stripline,
     ref_above: GND1, ref_below: PWR1}
  ...
```

## Non-goals

- **Full-wave EM** (Sol B/D integration in the eval loop). Phase D.
  We add an `em_queue` hook that emits a list of `(net, regions)` to
  enqueue, but we do NOT run them inline — eval stays sub-second.
- **3D field solver for Z0** (Polar Si9000 / HyperLynx). Wadell
  closed-form is the target accuracy for Phase C (±5% on typical
  stackups, much worse on edge cases — documented).
- **Thermal / EMI radiation** — separate axis, deferred.
- **AOI / test-point access** — outside routing scope.

## Risk + open questions

1. **Closed-form Z0 accuracy on edge stackups** — Wadell breaks down
   for tightly-coupled differential striplines (s/h < 0.5) and very
   thin dielectrics (h < 50 μm). We will flag these as
   `z0_formula_marginal: true` and recommend Sol D for the affected
   nets.
2. **Plane voiding / split detection** needs the plane layer geometry
   to be readable. The C++ ECAD reader exposes plane layers but
   `bga_router` may not currently load them. If not, scope this
   subset (`si.return_path_*`, `si.plane_split_*`) for a follow-up
   sub-phase.
3. **Clearance checks (B)** require all-net spatial index over routed
   polylines. Cost is O(N log N) per layer; for 1000-net designs we'll
   need to time-budget the eval. Mitigate by making `rule_check`
   opt-in via `--checks` flag, default ON.

## Splitting fallback (if PR gets too big)

| Sub-PR | Scope | Reviewable size |
|---|---|---|
| A1 | placeholders fill (3 fields) | tiny |
| A2 | new geometry derived fields (12 fields) | small |
| B1 | verifier.py skeleton + 5 easy rule_checks | small |
| B2 | clearance + topology + impedance checks | medium |
| C1 | StackupSpec + Wadell Z0 single + diff | medium |
| C2 | return-path + DC resistance + standards | medium |

Default plan: ship A+B1+C1 as one PR, then a follow-up for B2+C2.

## Validation strategy

- **Unit**: per-metric tests with synthetic 2-net + 4-net inputs where
  the expected value is computed by hand and checked to 1e-9.
- **Property**: routed_ratio == 1.0 OR every unrouted net surfaces in
  `rule_check.by_field.*.violators` with a `routing_failed` reason.
- **Regression**: run all 6 existing recipes on `P3_EUR_REV03 bga218f`
  and `rigidflex BGA_49_A`; assert the new fields fill and the existing
  9 fields are byte-identical to current.
- **Adversarial**: 1 hand-crafted scenario per Phase A and per
  rule_check where the metric MUST flag (90° angle, partner-net 0.4mm
  gap with `pair_gap_mm=0.2` constraint, etc.).
