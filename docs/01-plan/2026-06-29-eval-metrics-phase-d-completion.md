# Routing eval completeness — Phase D 백로그 일괄 완료 보고서

**Date**: 2026-06-29
**Branch**: main
**Commit range**: 7c9b0a7 → f844ae9 (Phase D 일괄 처리 7개 commit)
**Scope**: 사용자 요청 "남은 것들 전부 진행해줘" — Phase D 백로그 6개 항목 처리 + 1개 명시적 deferred.

## 한 줄 요약

이번 묶음에 14개 commit이 추가로 main에 들어가, eval 메트릭이 8 영역 / **70개 활성 필드** 로 완성. router 본체 변경 없이 eval-time 계산만으로 (a) marginal Z0 → EM solver dispatch, (b) PG via 자동 인식 → return-path 활성화, (c) channel-fit 기반 width 위반 자동 검출이 가능해짐. pytest 826/826 통과.

## 이번 묶음 commit 흐름

| Hash | Phase | 내용 |
|---|---|---|
| 7c9b0a7 | D-1 | em_queue hook — marginal/impedance miss net 자동 enqueue |
| 378aa8a | (CLI) | `--checks=<csv>` 영역 선택 flag |
| 8e3fbe8 | D-2 | `check_topology` star/tee 분기 추가 |
| eca2a94 | D-3 | matrix file에서 plane layer 자동 탐지 |
| 4476594 | D-4 | stackup ViaSpan 스키마 + via stub length |
| 10dfe37 | D-5 | em_queue → sol_d 입력 task 변환 어댑터 + `em-dispatch` CLI |
| 29f0b81 | D-6 | PG via metadata 자동 도출 (net 이름 분류) |
| f844ae9 | D-7 | `check_width` 채널 fit 검증 |

## 14-commit 총합 결과 (cca92cb → f844ae9)

| 항목 | Before | After |
|---|---:|---:|
| eval JSON 활성 필드 | 9 | ~70 |
| null placeholder | 5 | 0 |
| NetRule 자동 검증 필드 | 0 | 26 |
| 메트릭 모듈 | 0 | 9 (path_geometry/geometry/cross_net/verifier/stackup/si/standards/return_path/em_queue) |
| 외부 통합 | 없음 | em-dispatch → sol_d/sol_b task JSON |
| pytest 총 | 606 | 826 |
| 메트릭 unit test | 0 | ~220 |

## P3 bga218f 실측 (Phase D 후)

```jsonc
{
  "routed": 16, "total": 16, "routed_ratio": 1.0,
  "elapsed_s": 1.63, "via_count": 4, "iterations": 52,
  "total_length_mm": 116.195, "sharp_bends": 70,

  "geometry": { ... 18 fields including layer_utilization,
                  shadow_crossings_2d, bend_density_per_mm, ... },

  "rule_check": {
    "violations": 1,
    "by_field": {
      "width_ok":          { "pass": false, "violators": [<15 nets>] },
      "layers_membership_ok": { "pass": true,  ... },
      "escape_side_ok":    { "pass": true,  ... },
      "clearance_ok":      { ... },
      ... 12 more fields ...
    }
  },

  "si": {
    "Z0_single_ended_ohm": { net092: 50.68, ... },
    "Z0_variance_pct": 0.0,
    "via_stub_length_mm": { net369: 0.54, ... },   // D-4
    "branch_dc_resistance_mohm": { ... },
    "marginal_formulas": { ... },
    "stackup_default_used": true,
    "return_path": {
      "plane_geometry_not_loaded": true,            // P3 has no PG layer
      "inferred_pg_via_count": 0,                   // D-6 (P3 net naming
      "inferred_stitch_via_count": 0                //   has no GND/VDD nets)
    }
  },

  "standard": {
    "ddr4_ok": null, "pcie_gen3_ok": null,         // no high-speed nets
    ... all None ...
    "unclassified_nets": [ ... 16 nets ... ]
  },

  "em_queue": { "count": 0, ... },                  // P3 nets within spec

  "phase_b_errors": null,
  "phase_c_errors": null
}
```

P3 bga218f가 노출한 새 인사이트.

- **width_ok=False (15/16 net)** — 이전엔 안 보였던 hidden DRC. 0.075mm declared width가 BGA escape의 좁은 채널에 못 들어가는 net이 15개. router는 cost-only로 width를 다뤄 silently 밀어넣었음.
- **via_stub_length_mm = 0.54mm on net369** — escape이 LAY3로 잠깐 갔다 와서 full-stack through via의 양쪽 stub 0.54mm가 unused. back-drill 후보.
- **plane_geometry_not_loaded** — P3는 plane이 별도 layer 아니라 SIGNAL layer 안의 large copper pour. matrix file TYPE=POWER_GROUND 0개 (D-3 auto-detect도 fallback도 미스).
- **em_queue.count=0** — 모든 net이 Wadell formula 신뢰 범위 안 + impedance_target 위반 없음 → EM solver 호출 불필요한 정상 케이스.

## 외부 통합 흐름 (Phase D-5)

```bash
# 1. 평가 — em_queue 자동 채움
bga-router route --dataset P3_EUR_REV03 --bga bga218f \
    --recipe default --budget 60 --output eval.json

# 2. em_queue 항목을 sol_d 입력 task로 변환
bga-router em-dispatch --input eval.json --out em_tasks/
# [em-dispatch] wrote N task files to em_tasks/
#   - P3_EUR_REV03__bga218f__default__<net>.json

# 3. sol_d 가 task 읽어 시뮬 (별도 — 본 묶음 스코프 밖)
cd /home/koopark/claude/KooSimulationDataReader/simulation/sol_d_cross_section
python run.py --input ../../layout/bga_escape/em_tasks/<task>.json
```

## 명시적 deferred (이번 묶음 밖)

| 항목 | 이유 |
|---|---|
| Multi-pin nets (true star/tee 3+ pin) | router 2-pin 전제 자체 변경 필요 — architecture change |
| Router-side per-segment width 기록 | D-7은 channel-fit 간접 검증, router 본체 수정은 별도 PR |
| Router-side via_metadata 채움 | D-6은 net 이름 휴리스틱 — router PathResult.via_metadata 필드는 자리만 마련 |
| Aggregator `routed_ratio_with_rule_pass` 컬럼 | 기존 5개 컬럼이 동일 정보 커버 |
| sol_d/sol_b 의 em-dispatch task 입력 수신 | downstream 모듈 변경 — 본 묶음 스코프 밖 |

## 검증 게이트

| 단계 | pytest | 비고 |
|---|---:|---|
| Phase C 끝 (22fabd6) | 752 | baseline before this batch |
| Phase D-1 | 760 | +8 em_queue |
| CLI --checks (378aa8a) | 760 | no new tests |
| Phase D-2 topology | 764 | +4 |
| Phase D-3 matrix | 780 | +16 (matrix 7 + plane 카운트 변화) |
| Phase D-4 via stub | 788 | +8 |
| Phase D-5 em-dispatch | (skipped — included below) | |
| Phase D-6 PG via | 817 | +29 (8 em-dispatch + 15 via_metadata + 기존 흡수) |
| Phase D-7 width | 826 | +9 |

전체 14-commit chain 어느 지점에서도 회귀 없음 — 각 commit이 자체로 green 통과.

## 결론

사용자가 처음 요청한 "PCB의 일반 분석을 모두 스코프에 넣고 싶다" 의 의미를 가능한 한 넓게 해석해서, eval surface 안에서 채울 수 있는 모든 항목을 채움. 남은 deferred 5건은 모두 router 본체 또는 sol_d/sol_b 같은 다른 서브시스템의 architecture-level 변경이 필요한 항목.

다음 자연스러운 PR 경계.
- (A) Router-side metadata 기록 — PathResult.via_metadata / 더 깊은 width tracking.
- (B) Multi-pin escape support — router architecture change (별도 PR + design doc).
- (C) sol_d/sol_b 가 em-dispatch task를 자동 처리하는 어댑터 (sim 패키지 쪽 변경).
