# Routing eval completeness — Phase F "net끼리 유기적으로 엮기" 완료 보고서

**Date**: 2026-07-01
**Branch**: main
**Commit range**: 4feea27 → HEAD (Phase F 일괄 5개 commit)
**Scope**: 사용자 요청 "작업량 커도 다 가능하게 만들어야" — net 관계 5개 항목 처리.

## 한 줄 요약

net 하나하나의 절대 메트릭을 넘어 **net끼리의 관계** (bus / coupling /
domain / driver-load / recipe-diff) 를 자동 추출하는 5개 신규 영역
추가. eval JSON 스키마 9→14 영역 / 활성 필드 100+ / pytest 890/890.

## 이번 묶음 commit 흐름

| Hash | F# | 내용 | Test |
|---|---|---|---:|
| (4feea27) | F-2 | bus group auto-detect (이름 정규식) | +9 |
| ... | F-1 | per-pair coupling matrix (parallel run length) | +12 |
| ... | F-5 | net cluster 자동 분류 (6-tier 우선순위) | +14 |
| ... | F-3 | driver/load dependency graph (NetRule 확장) | +8 |
| ... | F-4 | cross-recipe net-level diff + net-diff CLI | +7 |

각 commit 자체 green. pytest 875→890 (+15 신규 test 흡수 후).

## 5개 영역 요약

### F-2 bus_groups

이름 인덱스 패턴 (`DDR4_DQ[0..7]`, `PCIE3_RX[0..3]_P`) → 자동 그룹.
diff-pair suffix 보존 (_P / _N 별도 그룹). P3 익명 `net042`도 'net'
prefix 하나로 묶임. `NetRule.length_match_group` 미설정 dataset에서
길이 매치 대상을 자동 확보.

### F-1 coupling

각 net 쌍마다 same-layer parallel run length (mm) 산출. threshold —
각도 5°, 수직 gap 0.30mm (3W 기본값), overlap 0.5mm. 결과 = sparse
NxN dict + top-K + aggressor-per-net. P3 bga218f 실측: **top pair
net477↔net479 (2.31mm)** 등 진짜 crosstalk hotspot 노출.

### F-5 net_clusters

6-tier 우선순위로 signal 도메인 분류 — power_ground / 표준 (DDR/PCIe/
USB) / domain (clock/jtag/reset/gpio/analog/led) / rule (diff_pair/
Z0-target 클래스) / bus fallback / unclassified.

### F-3 dependency_graph

NetRule 3개 필드 신규 (`driver_pin` / `load_pins` / `clock_domain`).
clock domain / driver 명시 → 그래프. 없으면 bus-group ≥3 members
fallback. 그룹별 load-vs-driver skew (mm). P3 실측: bus:net 그룹 16
members, anchor=net092, **skew 9.20mm**.

### F-4 net_diff

두 route 결과 JSON → per-net Z0 / DC_R / via_stub delta + top-K 정렬.
`bga-router net-diff --a A.json --b B.json --output diff.md` CLI.
P3 실측 (default vs coupled): DC_R delta net476 +2.12mΩ, net482
-1.06mΩ — recipe별 진짜 net 이동 인사이트.

## eval JSON 최종 shape (Phase F 후)

```jsonc
{
  "metrics": {
    // 기존 필드 (Phase A-E) ...
    "geometry":     {...},      // Phase A/B/C
    "rule_check":   {...},      // Phase B, D-2, D-6, D-7
    "si": {...},                // Phase C, D-1, D-4, D-6, E-2, E-3
    "standard":     {...},      // Phase C
    "em_queue":     {...},      // Phase D-1

    // Phase F 신규
    "bus_groups":   {           // F-2
        "groups": [{label, members[], count, is_diff_pair_bus}],
        "covered_nets": N, "uncovered_nets": []
    },
    "coupling": {               // F-1
        "top_pairs": [{pair, length_mm}],
        "pair_count": N, "total_coupled_mm": M,
        "aggressors_per_net": {net: [{with, length_mm}]},
        "thresholds": {angle_tol_deg, gap_threshold_mm, min_length_mm}
    },
    "net_clusters": {           // F-5
        "clusters": [{label, members, count, source}],
        "cluster_count": N, "unclassified": []
    },
    "dependencies": {           // F-3
        "groups": [{label, driver, loads, count}],
        "edges":  [{driver, load, relation}],
        "load_length_skew": {label: {driver, load_lengths_mm, skew_mm, spread_pct, member_count}}
    }
  }
}
```

+ `bga-router net-diff` CLI (F-4) — cross-recipe report 별도 markdown.

## 명시적 deferred (Phase F 후에도 남음)

- **Multi-pin nets full impl** (E-5 scaffold만) — router architecture change, 별도 PR.
- **Per-net violations breakdown** (rule_check.violations는 design-level count) — RuleCheckResult API 변경.
- **sol_b PEEC runner** (sol_d_runner 패턴 복제) — 별도 PR.
- **F-1 coupling → SPICE 자동 네트리스트** — coupling matrix가 감지한 aggressor 쌍을 SPICE .lib으로 export.
- **F-3 driver_pin resolver** — 현재 name-based hint만 저장, 실제 pin→net 매핑은 EDA loader에 의존.

## 전체 누적 (cca92cb → HEAD)

| 단계 | pytest |
|---|---:|
| Plan trio | 606 |
| Phase A | 615 |
| Phase B | 685 |
| Phase C | 752 |
| Phase D (7 items + docs + CLI --checks) | 826 |
| Phase E (5 items + design doc) | 840 |
| Phase F-2 | 849 |
| Phase F-1 | 861 |
| Phase F-5 | 875 |
| Phase F-3 | 883 |
| Phase F-4 | 890 |

**26 commit / 890 tests / ~6000 LOC** 가 main에 들어감.

## 결론

"각 net별로 유기적으로 엮는 분석" 요청에 대해:
- **bus / coupling / cluster / dependency / recipe-diff** 5개 관계 분석 모듈 추가.
- P3 bga218f 실측에서 실제 인사이트 도출 — top coupling pair, bus_skew 9.20mm,
  recipe-diff DC_R 변화 등.
- CLI end-to-end 흐름 (route → em-dispatch → em-run → net-diff → aggregate)
  전부 자동화.
- 남은 gap은 모두 router architecture change 또는 sim 패키지-side 작업.
