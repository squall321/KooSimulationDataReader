# Routing eval completeness — Phase G "남은 backlog 전부" 완료 보고서

**Date**: 2026-07-02
**Branch**: main
**Commit range**: 012890d → 758889f (Phase G 5개 commit)
**Scope**: 사용자 요청 "남은작업 다 해 일단" — Phase F 후 잔여 5건 처리.

## 한 줄 요약

Phase F 완료 시점에 명시적으로 deferred로 남겼던 5개 backlog 항목
(sol_b runner / SPICE export / per-net violations / driver resolver /
multi-pin Stage 1) 을 각각 commit으로 처리. multi-pin은 여전히
Stage 1 (data layer + coverage verifier) 만 — Stage 2-5는 별도 PR
sequence. 전체 누적 32 commit, pytest 919/919.

## 이번 묶음 commit 흐름

| Hash | G# | 내용 | Test |
|---|---|---|---:|
| 012890d | G-1 | sol_b PEEC runner + em-run --solver flag | +5 |
| bc55d62 | G-2 | coupling → SPICE netlist export + CLI | +8 |
| 7df86ef | G-3 | rule_check per-net violations breakdown | 0 |
| cc5e87e | G-4 | driver_pin ↔ net_name resolver (EDA pin_to_net) | +11 |
| 758889f | G-5 | multi-pin Stage 1 (extra_pins auto-populate + coverage) | +5 |

각 commit 자체 green. pytest 903 → 919 (+16 신규).

## 각 G-* 처리 결과

### G-1 sol_b PEEC runner

sol_d_runner (Phase E-4) 패턴을 sol_b PEEC로 복제. em_queue task의
`suggested_solver` 값에 따라 sol_d 또는 sol_b로 자동 라우팅.

```bash
bga-router em-run --tasks-dir tasks/ --solver sol_d  # 2D-FD
bga-router em-run --tasks-dir tasks/ --solver sol_b  # 3D PEEC
```

### G-2 SPICE netlist export

F-1 coupling matrix + Z0/DC R/via_stub 조합. 각 net을 π RLC
SUBCKT, coupled pair를 K coupling card로 export. LTspice/ngspice
로드 가능.

```bash
bga-router spice-export --input eval.json --output board.lib
```

P3 실측: **16 SUBCKT + 14 K cards** (top: K_net477_net479=0.045).

### G-3 per-net violations breakdown

`rule_check.violations` 는 design-level 스칼라 1 만 노출했음. 실제로는
15 net이 width DRC 위반이었지만 안 보였음.

신규 필드.
- `violations_expanded` = 15 (모든 field violator 합)
- `nets_with_violation` = 15
- `violations_by_net` = [{net, violation_count, fields[]}] 내림차순
- `by_field.<rule>.violator_count`

Aggregator `RoutedClean %` 도 per-net 정확도로 재계산.

### G-4 driver_pin resolver

F-3의 `NetRule.driver_pin` 을 hint string으로만 저장했음. 이번엔
`EdaData.components[*].pin_to_net` 스캔해서 `'U200.A14'` → 실제
net_name 자동 lookup.

```python
metrics.driver_resolution = {
    'declared_count':     N,        # NetRule에 driver_pin 명시된 net 수
    'resolved_count':     N,        # 실제 net_name으로 resolve 성공
    'unresolved_count':   0,
    'resolved':           {net: driver_net, ...},
    'components_scanned': 558,
}
```

P3 실측: 558 components 스캔, declared=0 (P3에 driver_pin 명시 없음).

### G-5 multi-pin Stage 1

E-5에서 `RoutingTask.extra_pins` scaffold만 landing. 이번엔:
- `extract_routing_tasks` 가 3+ pin net 분해 시 첫 sub-task에 원본
  endpoints 자동 저장.
- `check_multi_pin_coverage(task, routed_paths)` 신규 verifier —
  extra_pins의 각 pin 셀이 routed path에 존재하는지 확인.
- `verify_all`에 `multi_pin_coverage_ok` 필드 추가.

Multi-pin Stage 2-5 (Steiner tree solver / congestion-aware / etc.) 는
여전히 별도 PR sequence — design doc 참조.

## eval JSON 최종 shape (Phase G 후)

15개 영역, 활성 필드 120+.

```jsonc
{
  "metrics": {
    // Phase A/B
    "geometry":     {...}, "sharp_bends": N, "total_length_mm": ..., "residual_overuse": ...,
    "rule_check": {
        "violations":          1,           // legacy count
        "violations_expanded": 15,          // G-3 신규
        "nets_with_violation": 15,          // G-3 신규
        "violations_by_net":   [...],       // G-3 신규
        "by_field": {...}                    // includes violator_count per field
    },
    // Phase C
    "si": {...},
    "standard":     {...},
    "em_queue":     {...},
    "stackup_default_used": true,
    // Phase D-3 (matrix), D-6 (PG via), D-7 (channel-fit width)
    // (all folded into si.return_path / rule_check.width_ok)
    // Phase F
    "bus_groups":   {...},
    "coupling":     {...},
    "net_clusters": {...},
    "dependencies": {...},
    // Phase G-4
    "driver_resolution": {
        "declared_count": N, "resolved_count": N, ...
    }
    // 신규 CLI: net-diff, spice-export, em-run --solver
  }
}
```

## CLI 명령어 전체 (32 commit 후)

| 명령 | Phase | 기능 |
|---|---|---|
| `bga-router register` | (기존) | dataset 등록 (tgz는 tar xzf 후 --type dir) |
| `bga-router route` | A/B/C/D/E/F/G | 단일 recipe eval + 모든 metrics 채움 |
| `bga-router eval` | (기존) | 다중 recipe sweep |
| `bga-router aggregate` | C/E-1/G-3 | 마크다운 리포트 + RoutedClean 컬럼 |
| `bga-router em-dispatch` | D-5 | em_queue → per-net task JSON |
| `bga-router em-run` | E-4/G-1 | sol_d 또는 sol_b 자동 dispatch |
| `bga-router net-diff` | F-4 | 두 recipe 비교 마크다운 |
| `bga-router spice-export` | G-2 | eval JSON → SPICE .lib |

## 전체 누적 (cca92cb → HEAD)

| 단계 | pytest | 실측 인사이트 (P3 bga218f) |
|---|---:|---|
| Plan trio | 606 | - |
| Phase A | 615 | total_length_mm=116.19 (첫 실제 값) |
| Phase B | 685 | sharp_bends=70, rule_check by_field 16개 |
| Phase C | 752 | Z0=50.68Ω, DDR4/PCIe 표준 슬롯 활성 |
| Phase D | 826 | width DRC 15/16 위반 발견, via_stub 0.54mm |
| Phase E | 840 | end-to-end CLI (route→em-dispatch→em-run) |
| Phase F | 890 | coupling 14 pairs, bus_skew 9.20mm |
| Phase G | 919 | violations_expanded=15, 16 SPICE SUBCKT emit |

**32 commit / 919 tests / ~7000 LOC** 가 main에 들어감.

## 진짜로 남는 것 (Phase G 후에도 architecture-level)

| 항목 | 사유 |
|---|---|
| Multi-pin Stage 2-5 | router solver 재설계 (design doc 참조) |
| sol_d / sol_b 결과 파일 자동 aggregation | sim 패키지 쪽 output schema 변경 필요 |
| em-dispatch의 em_data.json 자동 생성 | odb_to_em_json 통합 필요 (C++ 쪽 작업) |
| Interactive dashboard | 별도 web app 스코프 |
| Per-recipe SPICE simulation loop | ngspice batch mode 통합 |

## 결론

"남은작업 다 해" 요청에 대해 5개 backlog 항목 모두 처리. multi-pin은
Stage 1 (data layer)까지만 landing — Stage 2-5는 router
architecture change라 별도 design doc 기반 PR sequence 유지.

남은 항목은 모두 (1) router solver 재설계 또는 (2) 별도 서브시스템
(sim 패키지 / dashboard / C++) 변경. 현재 eval 메트릭 surface 안에서
채울 수 있는 모든 것은 완료.
