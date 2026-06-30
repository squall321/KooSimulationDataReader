# Routing eval completeness — Phase E (5건) 완료 보고서

**Date**: 2026-06-30
**Branch**: main
**Commit range**: 2b549ac → 9358d66 (Phase E 일괄 처리 5개 commit)
**Scope**: 사용자 요청 "1다음도 이어서 해" — Phase D 완료 후 잔여 deferred 5건 처리.

## 한 줄 요약

직전 Phase D 묶음 완료 시점에 명시적으로 deferred로 남겼던 5개 항목 (#1 multi-pin / #2 width / #3 via_metadata / #4 sol_d 수신 / #5 aggregator) 을 5개 commit으로 처리. router 본체에 ABI-safe 최소 변경만 들어가고, multi-pin은 진짜 architecture change라 진입 scaffold + design doc으로 분리.

전체 누적: 20 commit, pytest 840/840, eval JSON schema 8 영역 / 70+ 활성 필드.

## 이번 묶음 commit 흐름

| Hash | E# | 내용 | Test 추가 |
|---|---|---|---:|
| 2b549ac | E-1 | aggregator `RoutedClean %` 컬럼 (routed AND violations=0) | 0 |
| 9e04d33 | E-2 | run_recipe에서 PathResult.via_metadata 자동 채움 | 0 |
| ab92a34 | E-3 | run_recipe에서 PathResult.segment_widths_mm 자동 기록 | +3 |
| 610eb99 | E-4 | `em-run` CLI + sol_d_runner — em-dispatch task → sol_d 자동 호출 | +8 |
| 9358d66 | E-5 | RoutingTask.extra_pins scaffold + multi-pin design doc | +3 |

각 commit 자체 green. pytest 826 → 840 (+14 신규 test).

## 5건 deferred — Before / After 매핑

| # | Before (Phase D 끝 시점) | After (E 완료) |
|---|---|---|
| #5 Aggregator `routed_ratio_with_rule_pass` | 5개 추가 컬럼만, 별도 clean ratio 없음 | `RoutedClean %` 컬럼 신규 |
| #3 Router-side via_metadata | PathResult.via_metadata 필드만 있고 항상 빈 list | run_recipe 마지막에 자동 채움. collect_pg_via_xy가 우선 사용 |
| #2 Router-side per-segment width | check_width 채널 fit으로 간접 검증 | segment_widths_mm 필드 신규, run_recipe에서 uniform rule.width_mm 채움 (향후 neck-down 호환 자리) |
| #4 sol_d em-dispatch task 자동 수신 | em-dispatch는 task JSON만 만들고 사용자가 수동 실행 | `bga-router em-run` CLI 추가, dry-run / exec 두 모드 |
| #1 Multi-pin nets | 명시적 deferred — architecture change | data layer scaffold (RoutingTask.extra_pins) + Stage 1-5 design doc |

## 외부 통합 흐름 (E-4 후)

```bash
# 1. route — em_queue 자동 채움
bga-router route --dataset P3_EUR_REV03 --bga bga218f \
    --recipe default --budget 60 --output eval.json

# 2. em-dispatch — em_queue 항목을 sol_d task JSON으로 변환
bga-router em-dispatch --input eval.json --out em_tasks/

# 3. em-run dry-run — 호출 명령 확인
bga-router em-run --tasks-dir em_tasks/ --dry-run

# 4. em-run exec — sol_d 자동 호출 (em_data.json 필요)
bga-router em-run --tasks-dir em_tasks/ \
    --em-data em_data.json --output-dir sol_d_results/
```

이전엔 1→2까지만 자동, 3-4는 수동이었음. 이제 end-to-end 자동화.

## 진짜 deferred (E 묶음 후에도 남는 것)

| 항목 | 사유 |
|---|---|
| Multi-pin nets full implementation (Stage 1-5) | router 본체 재설계 — 별도 PR sequence per design doc |
| Per-net violations breakdown in `violations` count | `RuleCheckResult` API 변경 필요. 현재 width_ok가 15 net 위반인데 design-level은 1로 묶임. |
| Real multi-pin dataset benchmark | 테스트 corpus에 multi-pin spec 없음. Stage 1 PR에서 fixture 추가 |
| sol_b PEEC runner (sol_d만 wrap) | sol_d_runner 패턴 그대로 복제 가능, 별도 PR |
| Stub length back-drill 자동 추천 (≥ threshold 시 alert) | UX-level enhancement |

## 전체 누적 (cca92cb → 9358d66)

| 단계 | pytest | LOC 추가 (대략) |
|---|---:|---:|
| Plan trio (be4a232) | 606 | +588 |
| Phase A (9dbcedb) | 615 | ~250 |
| Phase B (4bccda3) | 685 | ~1300 |
| Phase C (22fabd6) | 752 | ~1500 |
| Phase D-1 ~ D-7 | 826 | ~700 |
| Phase E-1 ~ E-5 | 840 | ~600 |

**총 20 commit / 840 tests / ~5000 LOC** 가 main에 들어감.

## 결론

사용자 요청 "1다음도 이어서 해" 의 의미를 "직전 deferred 5건 + 가능하면 multi-pin도" 로 해석. multi-pin은 진짜 architecture change라 scaffold + 분리 PR design doc으로 처리하고, 나머지 4건은 모두 surgical change로 완료.

진짜 남은 큰 작업은 multi-pin Stage 1-5 (별도 PR 시퀀스 — design doc 참조). 그 외에는 모두 "신규 dataset 들어오면 활성화" 단계의 metadata-driven 자동 동작.
