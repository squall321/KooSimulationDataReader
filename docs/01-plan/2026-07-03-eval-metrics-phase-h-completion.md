# Routing eval completeness — Phase H "중요한 것 순서대로 전부" 완료 보고서

**Date**: 2026-07-03
**Branch**: main
**Scope**: 사용자 요청 "순서대로 다 해야지" — 우선순위 목록 10건 전부 처리.

## 한 줄 요약

우선순위 정리에서 나온 10개 항목 (즉시 4 + 큰 작업 3 + nice-to-have 3)
을 모두 landing. **pytest 1010/1010** (1000 돌파), 누적 43 commit.
이제 tgz 입력부터 MCP 자동화 / 시뮬 correlation / crosstalk SPICE /
DFT / thermal-EMI / multi-pin Stage 2까지 전부 연결.

## 이번 묶음 commit 흐름

| H# | 내용 | Tests | 핵심 |
|---|---|---:|---|
| H-1 | em_data.json auto-generate (odb_to_em_json wiring) | +5 | em-run --auto-em-data |
| H-2 | Wadell Z0 validation vs Polar Si9000 | +14 | stripline 10-15% bias 발견+플래그 |
| H-3 | HDI micro-via stack DRC + drill aspect | +10 | filled-and-capped / 8:1 / 1:1 |
| H-4 | Sim 결과 aggregation (Touchstone parser) | +12 | analytical↔simulated Δ% |
| H-5 | bga-router MCP 서버 (stdlib JSON-RPC) | +11 | tgz→평가 MCP 한 call |
| H-6 | ngspice crosstalk loop + xtalk-sim CLI | +7 | top pair testbench 자동 생성 |
| H-7 | DFT (test point / AOI / probe pitch) | +6 | coverage % + buried nets |
| H-8 | 단일 HTML dashboard (inline data) | +7 | file:// 안전, no fetch |
| H-9 | Thermal (IPC-2152) + EMI proxy | +9 | 전류용량 + loop area |
| H-10 | Multi-pin Stage 2 (branched topo + arrival skew) | +10 | Dijkstra driver→pin |

## 최종 CLI (11 서브명령)

| 명령 | 기능 |
|---|---|
| register | dataset 등록 |
| route | 평가 실행 (17개 영역) |
| eval | recipe sweep |
| aggregate | markdown 리포트 |
| em-dispatch | em_queue → solver task |
| em-run | sol_d/sol_b dispatch (--auto-em-data 포함) |
| sim-agg | Touchstone → analytical correlation |
| net-diff | recipe 비교 |
| spice-export | SPICE .lib |
| xtalk-sim | ngspice crosstalk batch |
| dashboard | 단일 HTML 뷰 |

+ **MCP 서버**: `python -m bga_router.mcp_server` — 6 tools
(register_dataset / route / get_metrics / em_dispatch / net_diff /
spice_export). tgz 자동 해제 포함.

## eval JSON 최종 영역 (17)

geometry / rule_check (per-net breakdown) / si (simulated 포함) /
standard / em_queue / bus_groups / coupling / net_clusters /
dependencies / driver_resolution / hdi_drc / dft / thermal_emi /
multi_pin (조건부) / stackup_default_used / phase_b_errors /
phase_c_errors

## H-2 정직한 발견

Wadell/Hammerstad closed-form 검증 결과.
- **Microstrip: ±8% 이내** (5 케이스, FR-4 / Rogers / ceramic) — 신뢰 가능.
- **Stripline: 10-15% systematic underestimate** (w/h 0.3~0.7 대칭
  구조). → `si.marginal_formulas.<net>.stripline_wh_bias` 플래그
  신설, em_queue가 자동으로 Sol D 시뮬 대상 지정.

## 전체 누적 (cca92cb → HEAD)

| Phase | 커밋 수 | pytest |
|---|---:|---:|
| Plan trio + A + B + C | 6 | 752 |
| D (7건) | 8 | 826 |
| E (5건) | 6 | 840 |
| F (5건) | 6 | 890 |
| G (5건) | 6 | 919 |
| **H (10건)** | **10+1** | **1010** |

**~44 commit / 1010 tests / ~10000 LOC.**

## 진짜 남는 것 (이제 정말 architecture-only)

| 항목 | 사유 |
|---|---|
| Multi-pin Stage 3-5 (chain-extension / Steiner solver) | router solver 코어 재설계 |
| Full-wave EM inline (Sol C FDTD 등) | 기존 판단 유지 — <50um trace에서 openEMS 불가 (메모리 참조) |
| Fixture-aware DFT sign-off | part height + fixture 데이터 모델 없음 |
| 실측 correlation (VNA measurement) | 하드웨어 필요 |

## 결론

"더 해야 할 중요한 것" 목록의 소프트웨어로 가능한 항목은 전부 처리.
- 유일한 대형 잔여는 multi-pin Stage 3-5 (router solver 재설계) —
  실제 multi-pin dataset이 corpus에 들어올 때가 착수 시점.
- H-2에서 stripline analytical 한계를 정량화하고 자동 우회 경로
  (em_queue → Sol D) 를 연결한 것이 이번 묶음의 가장 가치 있는 발견.
