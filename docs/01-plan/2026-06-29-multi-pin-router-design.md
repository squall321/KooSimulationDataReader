# Multi-pin nets — router architecture extension design

**Date**: 2026-06-29
**Status**: design-stage (E-5 scaffold landed; full implementation deferred)
**Related**: `bga_router/metrics/verifier.check_topology` (Phase D-2),
`RoutingTask.extra_pins` field (Phase E-5).

## Why this is a separate PR

The BGA escape router has assumed 2-pin nets (source + sink) since
inception. Every solver, negotiator, decomposer, and post-processor
expects `RoutingTask.source` / `.sink` as the entire net. Real PCB
designs routinely carry 3+ pin nets — DDR command/address buses,
fan-out clock trees, power/ground rails, multi-load reset signals.

For these, the current router falls back to 2-pin escape on the first
source→sink pair it sees, silently dropping the additional loads.
`check_topology` flags the mismatch (Phase D-2 — `star`/`tee` declared
on a chain → False), but the routing itself is wrong.

Fixing this requires touching:
- net_extractor (build multi-pin RoutingTask)
- decomposers (groups containing multi-pin nets)
- solvers (chain extension vs Steiner-tree solve)
- negotiators (cross-pair interactions for multi-load)
- post-processors (length-match / DRC on branched paths)
- verifier (topology + length-match generalized to branched paths)

Estimated surface: 500-1500 LOC across 8-10 modules + 100+ tests.
Out of scope for this PR which is eval-metric focused.

## Phase E-5 scaffold (this PR)

A minimal landing: `RoutingTask.extra_pins: Optional[Tuple[NetEndpoint, ...]] = None`.

- Carries 3+ loads when set by future net_extractor extension.
- Current router (2-pin) ignores it. No behavior change.
- check_topology and length-match verifiers can grow to consume it
  without ABI churn.

## Recommended PR sequence (future)

### Stage 1 — data layer
- net_extractor returns `extra_pins` for nets with 3+ pin endpoints.
- Test: synthetic 3-pin RoutingSpec → RoutingTask.extra_pins populated.
- No solver change; existing tests pass byte-identical.

### Stage 2 — verifier upgrade
- check_topology: when extra_pins set + routed_paths has branched
  graph matching star/tee/daisy_chain pattern with N+1 leaves,
  return True. When unbranched, return False with reason
  "multi-pin declared but routed as 2-pin chain".
- check_length_group: include extra_pins' branches in length-match
  group spread calc.

### Stage 3 — chain extension solver (cheap path)
- post_processor extends 2-pin chain to nearest extra_pin via
  greedy nearest-neighbour. Works for daisy_chain only.
- Acceptable for DDR address buses where order is fixed.

### Stage 4 — Steiner-tree solver (full)
- New skill: `SteinerTreeSolver` registered in skill registry.
- Solves min-cost Steiner tree connecting all (source, sink, *extra_pins).
- Negotiator handles multi-net rip-up with branched paths.
- Required for star/tee topology and clock fan-out.

### Stage 5 — congestion-aware multi-pin
- Decomposer assigns multi-pin nets to dedicated phase (often last).
- Cost grid weights account for branched-path congestion patterns.

Each stage is its own PR with rollback safety (Stage 1 alone changes
nothing; Stage 2 fails fewer routes; Stage 3+ change actual paths).

## Why we are stopping at E-5 here

The current eval can ALREADY measure the gap between declared multi-pin
intent and 2-pin reality (`check_topology` returns False on mismatch).
The user-visible value of finishing E-5 to Stage 4 is non-trivial but
the work is too large for this batch. Better to ship the eval surface
honestly — "we see the gap, we don't yet fix it" — than to ship a
half-built router.

`RoutingTask.extra_pins=None` everywhere today is the honest answer:
no dataset in the test corpus has multi-pin nets surfaced. P3, rigidflex,
and the synthetic fixtures all assume 2-pin. When a real multi-pin
dataset lands, the data-layer (Stage 1) becomes the natural first PR.

## Tests + acceptance for Stage 1 (data layer PR)

- Synthetic spec with a 3-pin net → RoutingTask.extra_pins has 1 entry.
- 2-pin spec (current) → RoutingTask.extra_pins is None (byte-identical).
- net_extractor doesn't crash on missing extra-pin metadata.
- Existing 837 pytest suite remains green.

## Notes / open questions

- **Source/sink choice for multi-pin**: should source be the driving
  pin (need design intent) or just the first pin in pin order? The
  router needs SOMETHING to start from; defer the driver-vs-load
  decision to NetRule extension (`driver_pin: str | None`).
- **Differential pair × multi-pin**: rare in practice (diff-pair is
  inherently 2-net × 2-pin = 4 endpoints). If it appears, treat each
  half as independent multi-pin chain.
- **Power/ground**: PG nets have 100+ pins; should NOT be routed by
  the signal router. NetRule.skip_routing already handles this.
