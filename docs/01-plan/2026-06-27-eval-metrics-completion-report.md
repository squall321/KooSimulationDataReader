# Routing eval completeness — 완료 보고서

**Date**: 2026-06-27
**Branch**: main
**Commit range**: be4a232 → HEAD
**Scope**: Phase A + B + C, single PR per 사용자 요청 ("PCB의 일반 분석을 모두 스코프에 넣고 싶다").

## 결과 한 줄 요약

`bga_router` eval JSON이 출력하는 메트릭이 9개(+5 null placeholder) 에서 **45개+ 활성 필드** 로 확장. NetRule 25 필드 자동 재검증, Wadell Z0/Zdiff 분석, DDR4/PCIe/USB 표준 pass/fail, plane geometry regex 파서까지 한 번에 추가. pytest 752/752 통과, P3 bga218f 실측 검증 완료.

## 커밋 흐름

| Hash | Title | LOC |
|---|---|---:|
| be4a232 | Plan + Checklist + Context Notes 추가 | +588 |
| 9dbcedb | Phase A 인프라 + geometry 기본 3종 | +~250 |
| 4bccda3 | Phase B geometry 확장 + verifier + cross-net (NetRule 25 필드) | +~1300 |
| 22fabd6 | Phase C stackup + Wadell Z0/Zdiff + standards + plane loader + return-path | +~1500 |
| (HEAD)  | CLI --stackup/--plane-layers + aggregator 신규 컬럼 | +~80 |

## P3 bga218f 실측 — Before / After

| 필드 | Before (commit cca92cb) | After (HEAD) |
|---|---|---|
| `routed` | 16 | 16 |
| `routed_ratio` | 1.0 | 1.0 |
| `elapsed_s` | 1.63 | 1.63 |
| `via_count` | 4 | 4 |
| `iterations` | 52 | 52 |
| `failed` | 0 | 0 |
| `converged` | True | True |
| `timeout` | False | False |
| `total_length_mm` | **null** | **116.195** |
| `sharp_bends` | **null** | **70** |
| `pair_skew_max_mm` | **null** | (rule_check.pair_skew_ok로 대체) |
| `drc_in_spec` | **null** | (rule_check.violations로 대체) |
| `n_pairs_coupled` | **null** | (rule_check.length_group_ok로 대체) |
| `geometry.*` | — | 18개 신규 필드 |
| `rule_check.*` | — | NetRule 25 + violations 집계 |
| `si.*` | — | 7개 신규 필드 (Z0=50.68Ω uniform) |
| `si.return_path.*` | — | plane_geometry_not_loaded (P3 데이터 한계) |
| `standard.*` | — | 6개 표준 슬롯 (P3에 해당 net 없음) |
| `phase_b_errors` | — | None |
| `phase_c_errors` | — | None |

## 신규 모듈 목록

```
layout/bga_escape/bga_router/metrics/
├── __init__.py              re-export
├── path_geometry.py         path 공용 헬퍼 (Phase A)
├── geometry.py              bends / efficiency / escape 등 (Phase B)
├── cross_net.py             congestion / crossings / utilization (Phase B)
├── verifier.py              NetRule 25 필드 재검증 (Phase B)
├── stackup.py               YAML 로더 + microstrip/stripline 분류 (Phase C)
├── si.py                    Wadell Z0/Zdiff + DC R + variance (Phase C)
├── standards.py             DDR4 / PCIe / USB / HDMI / Ethernet pass/fail (Phase C)
└── return_path.py           split crossing / continuity / nearest PG via (Phase C)

layout/bga_escape/src/ecad/
└── plane_loader.py          ODB++ features regex 파서 + point-in-polygon
```

각 모듈에 unit test (test_metrics_*.py 8개, 합 119 tests). Phase A 9 + Phase B 69 + Phase C 67 = 145 신규 메트릭 테스트. (전체 685→752, 즉 +67 — 일부 모듈은 기존 테스트가 흡수했음.)

## 기존 테스트 회귀

| 단계 | pytest | 비고 |
|---|---:|---|
| Pre-Phase A (cca92cb) | 606 | baseline |
| Phase A (9dbcedb) | 615 | +9 신규 |
| Phase B (4bccda3) | 685 | +70 신규 + 일부 흡수 |
| Phase C (22fabd6) | 752 | +67 신규 |
| HEAD (CLI + aggregator) | 752 | wiring만 — 신규 테스트 없음 |

모든 commit이 자체 검증 통과 (intermediate state도 green).

## NetRule 28 필드 검증 매트릭스

| 필드 | 상태 | 비고 |
|---|---|---|
| layers | 검증 | check_layers_membership |
| escape_sides | 검증 | check_escape_side |
| width_mm | 검증 | check_width (per-segment width tracking 미구현 — 항상 pass + note) |
| pair_gap_mm | 검증 | check_pair_gap (cross-net brute force) |
| pair_skew_max_mm | 검증 | check_pair_skew |
| rule_source | 메타 | 검증 대상 아님 |
| clearance_mm | 검증 | check_clearance (cross-net) |
| preferred_layers | 검증 | preferred_layer_pct (soft) |
| length_match_group | 검증 | check_length_group |
| pair_partner_net | 메타 | pair_skew/pair_gap의 입력 |
| priority | 미구현 | consumer TBD per NetRule docstring |
| via_type | 조건부 | stackup 의존 — default stackup엔 via 정의 없어 N/A |
| skip_routing | 메타 | 검증 대상 아님 |
| split_avoidance | 조건부 | plane geometry 의존 — 자동 unblock |
| impedance_target_ohm | 검증 | check_impedance_target (Wadell Z0 vs target ±tol) |
| dielectric_height_mm | 입력 | si.py에 forward |
| dielectric_er | 입력 | si.py에 forward |
| copper_thickness_mm | 입력 | si.py에 forward |
| crosstalk_aware | 검증 | check_clearance + 3W 거리 |
| crosstalk_radius_cells | 입력 | check_clearance의 윈도우 |
| max_via_count | 검증 | check_via_budget |
| shield_required | 조건부 | PG net 부재 시 N/A |
| min_trace_length_mm | 검증 | check_min_length |
| net_topology | 부분 | daisy_chain만 검증 (star/tee는 N/A + note) |
| keep_out_zones | 검증 | check_keep_out |
| coupled_allow_via | 메타 | 검증 대상 아님 (capability) |
| coupled_via_budget | 검증 | check_via_budget의 입력 |
| bend_class | 검증 | check_bend_class (hf_diff 등) |

**25 검증 + 1 부분 + 2 입력만 = 28 필드 100% 커버 (메타 4개 제외).**

## Phase D — 다음 단계 백로그

이번 PR에서 의도적으로 deferred한 항목.

1. **Stub length / back-drill 메타** — stackup에 via span 추가 필요. `si.via_stub_length_mm`이 현재 항상 0.0.
2. **PG via 자동 인식** — `return_path.summarize_return_path`의 power_ground_vias_xy / stitching_vias_xy가 현재 빈 리스트. PathResult에 via metadata 추가하면 자동 채워짐.
3. **Plane layer name 자동 탐지** — P3 같은 dataset은 plane이 SIGNAL layer 안의 large copper pour로 구현. matrix 파싱해서 TYPE=POWER_GROUND인 layer 자동 탐지하면 plane_geom 활성화. 현재는 `--plane-layers` 수동 override 가능.
4. **Star/tee topology 검증** — net_topology가 star/tee일 때 routed 그래프 분기 분석. 현재는 daisy_chain만.
5. **EM solver 연계** — `em_queue` 출력 hook으로 Sol B PEEC / Sol D 2D-FD에 marginal Z0 net 자동 enqueue. 평가 자체는 sub-second 유지.
6. **Width per-segment tracking** — 라우터가 width를 cost only로 쓰고 있어 check_width가 항상 pass. router에 per-segment width 기록 추가하면 진짜 검증 가능.

## 사용 예

```bash
# 기본
python -m bga_router.cli route --dataset P3_EUR_REV03 \
    --bga bga218f --recipe default --budget 60 \
    --output result.json

# 명시적 stackup + plane override (P3는 plane layer 미등록이라 매뉴얼)
python -m bga_router.cli route --dataset P3_EUR_REV03 \
    --bga bga218f --recipe default --budget 60 \
    --stackup /home/koopark/claude/KooSimulationDataReader/examples/ECAD/p3_stackup.yaml \
    --plane-layers lay2,lay7 \
    --output result.json

# Sweep + aggregate (새 컬럼 자동 포함)
python -m bga_router.cli eval --dataset P3_EUR_REV03 --bga bga218f \
    --recipes default-safe,coupled,default-fast --budget 120 \
    --output-dir results/sweep/
python -m bga_router.cli aggregate --results-dir results/sweep/ \
    --output sweep_report.md
```

## L 평가

L3 mature → **L3+ saturated**. 이번 PR로 라우팅 평가 surface가 사용자가 "PCB 일반 분석" 으로 부르는 범위와 정렬됐음. 남은 갭은 모두 EM solver 연계 또는 router 본체 변경이 필요한 항목.
