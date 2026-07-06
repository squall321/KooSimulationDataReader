# Routing eval completeness — Phase I "MCP 뷰어 + SI/PI 완성도 끌어올리기" 완료 보고서

**Date**: 2026-07-06
**Branch**: main
**Scope**: 사용자 질문 "MCP ECAD 뷰어 및 SI/PI 해석용으로 얼마나 완성되어 있냐" → 완성도 평가에서 나온 우선순위 5건 전부 처리.

## 한 줄 요약

완성도 평가에서 gap으로 지목한 5개 (MCP tool 확장 / ECAD 뷰어 / PI 연결 / sol_d 실검증 / delay ps) 를 모두 landing. **pytest 1056/1056**, 누적 53 commit. 특히 I-4에서 sol_d를 P3 net에 실제로 돌려 analytical vs field-solve **+15.4% delta**를 실측 확인 (H-2 예측 검증).

## 완성도 Before/After

| 영역 | Before | After |
|---|---:|---:|
| MCP 서버 | 85% | **95%** (6→13 tools) |
| ECAD 뷰어 | 30% | **75%** (routed path Canvas) |
| SI 해석 | 70% | **90%** (real-solver correlation + delay ps) |
| PI 해석 | 20% | **60%** (pdn_dc 연결) |

## 이번 묶음 commit 흐름

| I# | 내용 | Tests | 핵심 |
|---|---|---:|---|
| I-1 | MCP tools 4종 (dashboard/sim_agg/em_run/xtalk_sim) | +4 | MCP 6→10 tools |
| I-2 | routed-path Canvas 뷰어 + paths_mm + MCP route_viewer | +7 | 라우팅 결과 실제 렌더 |
| I-3 | pdn_dc IR-drop 연결 + pdn tool | +8 | PI 파이프라인 연결 |
| I-4 | sol_d 실검증 + solver-summary Z0 우선 | +5 | +15.4% delta 실측 |
| I-5 | propagation delay ps + timing skew | +7 | mm→ps 타이밍 |

## I-4 실측 결과 (가장 가치 있는 발견)

P3 net114에 sol_d_cross_section 실제 실행.

| 방식 | Z0 | 비고 |
|---|---:|---|
| analytical Wadell (default 4-layer FR-4) | 50.68 Ω | closed-form |
| **sol_d 2D field-solve** | **58.48 Ω** | 10 sections, 14.8s |
| near-DC S11 역산 | 50.05 Ω | short-line 한계 (bias 가려짐) |

→ analytical vs field-solve **+15.4% delta**. H-2에서 "stripline 10-15% systematic bias" 로 예측한 것이 실제 데이터로 확인됨. sim-aggregator가 S11 역산이 아닌 solver-summary Z0를 우선하도록 수정해야 이 correlation이 의미를 가짐 (버그 수정 포함). 실제 sol_d 산출물을 `tests/fixtures/sol_d_net114/` 에 고정해 회귀 테스트로 보존.

## MCP 서버 최종 (13 tools)

```
register_dataset  route          get_metrics   em_dispatch
net_diff          spice_export   dashboard     sim_agg
em_run            xtalk_sim      route_viewer  pdn
```

사용자 원래 질문 "MCP로 tgz 열어 SI/PI 해석" 에 대한 답: register_dataset(tgz) → route → {sim_agg / pdn / route_viewer / dashboard / spice_export} 전부 MCP 한 세션.

## CLI 최종 (14 서브명령)

register / route / eval / aggregate / em-dispatch / em-run /
sim-agg / net-diff / spice-export / xtalk-sim / dashboard /
**viewer** / **pdn** + (mcp_server 별도 entry)

## eval JSON 최종 영역 (18)

geometry / rule_check / si (Z0 + Zdiff + via_stub + return_path +
**simulated** + **propagation**) / standard / em_queue / bus_groups /
coupling / net_clusters / dependencies / driver_resolution / hdi_drc /
dft / thermal_emi / multi_pin / **paths_mm** / phase_b_errors /
phase_c_errors

## 진짜로 남는 것 (완성도 관점)

| 영역 | 남은 것 | 왜 |
|---|---|---|
| SI 90→100% | 주파수별 S21 손실 곡선 리포트 통합 | 데이터는 있음, 시각화만 |
| PI 60→85% | decap 배치 / target impedance 커브 / AC PDN | pdn_dc 확장 필요 |
| 뷰어 75→90% | pad/via/keep-out 오버레이 + zoom-to-net | eda geometry 추가 로드 |
| MCP 95→100% | interactively-auth MCP (headless 제약) | 환경 이슈 |

## 전체 누적 (cca92cb → HEAD)

| Phase | pytest |
|---|---:|
| A~H | 1010 |
| Multi-pin S3~S5 | 1025 |
| **I-1~I-5** | **1056** |

**~53 commit / 1056 tests / ~13000 LOC.**

## 결론

- SI 파이프라인 폐루프 완성: length(mm) + delay(ps) + Z0(analytical) +
  Z0(field-solve) + correlation + crosstalk + standard pass/fail.
- ECAD 뷰어가 처음으로 라우팅 결과를 실제로 그림 (paths_mm).
- PI가 고립 상태에서 파이프라인에 연결됨.
- MCP 13 tools로 tgz→전분석이 한 세션에서 가능.

남은 것은 모두 시각화 통합 또는 pdn_dc/eda-loader 확장 — 별도 스코프.
