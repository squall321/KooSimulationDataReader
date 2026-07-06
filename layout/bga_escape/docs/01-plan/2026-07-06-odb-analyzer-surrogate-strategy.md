# ODB++ 설계 분석기 · 대리모델 기반 사전 검증 체계

문서 성격 — 전략 개요 및 산출물 정리(개조식).
기준일 — 2026-07-06.
대상 시스템 — `odb-analyzer` MCP(구 bga-router) · `bga_router` 패키지.
검증 상태 — 전체 회귀 1,159건 통과.

---

## 1. 배경과 목표

- 문제 정의.
  - 설계 검증(DV) 단계에서야 SI/PI/EMI·낙하·충격·열충격·AP on/off 등 물리 시뮬레이션을 수행함.
  - 시뮬레이션은 준비·해석에 장시간이 소요되어 설계 반영이 사후적임.
  - 설계 데이터(ODB++)를 사람이 직접 열람·해석해야 하므로 접근 비용이 큼.
- 목표.
  - 시뮬레이션 결과를 누적·학습하여 **대리모델(surrogate)** 로 대체함.
  - 설계 시점에 사전 평가를 수행하여 검증을 **좌향 이동(shift-left)** 함.
  - LLM이 ODB++ 설계 데이터에 직접 접근·분석하도록 도구화함.

---

## 2. 4단계 전략 로드맵

| 단계 | 명칭 | 내용 | 상태 |
|------|------|------|------|
| 1 | 체크리스트 자동화 | 설계 규칙·정합성 점검을 코드로 자동화함. | 완료 |
| 2 | MCP 기반 AX화 | 분석기를 MCP로 노출하여 개발자가 LLM으로 업무를 수행함. | 완료 |
| 3 | 시뮬레이션 누적·대리모델화 | 메커니즘별 결과를 축적하여 surrogate로 구성함. | 진행 |
| 4 | 사전 시뮬 평가·피드백 | DV 이전 설계 시점에 사전 평가·피드백을 제공함. | 착수 |

- 각 단계는 이전 단계의 산출물을 입력으로 사용함.
- 1·2단계는 실동작 산출물이 확보됨.
- 3단계는 대리모델 프레임워크와 첫 참조 모델(열충격)을 확보함.
- 4단계는 과제 메타데이터 연동(AIDataHub)과 게이트웨이 노출(HTTP 브리지)로 착수함.

---

## 3. 현재 진행 상황

### 3.1 메커니즘별 대리모델 (Phase L)

- 프레임워크 — `bga_router/metamodel/`.
  - `base.Metamodel` — 추상 인터페이스. `predict(features) → {출력, confidence, provenance, flags}`.
  - `registry` — 이름 기반 등록·조회. `models/` import 시 자동 등록됨.
  - `feature_spec.FeatureVectorSpec` — 추출값→입력벡터 사상. 필수 누락 시 `ValueError`, 범위 이탈 시 `{key}_out_of_range` 플래그.
  - `analytical`·`table`·`polynomial` — 해석식/표/다항 계열 모델 슬롯.
- 첫 참조 모델 — `models/thermal_shock_v0`(열충격).
  - Coffin-Manson 근사 기반. `provenance='reference_analytical:...'`, `confidence≈0.55`.
  - 근사 수준을 provenance·confidence로 명시하여 과신을 방지함.
- 추론 파이프라인 — `metamodel/infer.py`.
  - `pair_features` — 상·하면 패키지쌍 특징 결합.
  - `infer`·`infer_pair`·`infer_all_pairs` — 단건·쌍·전체쌍(겹침비 임계) 추론.
- 특징 추출 — `metrics/package_features.py`.
  - `PackageInstance`(ref_des, side, 중심·치수, 핀수, 피치, 회전).
  - 면 판정(`resolve_side`), 대향면 매칭, 인접·겹침 계산, 보드 컨텍스트 요약.

### 3.2 ODB++ 직접 열람·분석 (Phase K)

- `integrations/odb_inspect.py` — ODB++ 구조(레이어·넷·패키지·심볼) 직접 열람.
- `metrics/odb_traces.py` — 트레이스 기하 추출·분석.
- `integrations/odb_json_builder.py` — 분석 입력용 JSON 구성.
- 심볼 단위 규칙 준수 — 미터계 심볼은 micron(÷1000→mm), 임페리얼은 mil(×0.0254→mm).

### 3.3 보고자료 정보 보강 (Phase J)

- `integrations/si_report.py` — SI 주파수 응답 리포트. SVG 스파크라인으로 경향 시각화.
- `route_viewer.py` — 뷰어 Canvas 오버레이(핀·비아·keep-out, 넷별 mm 폴리라인).
- `eval.py` — `paths_mm`·`overlay_mm`·`si.propagation` 블록 추가.
- 목적 — LLM이 대리모델 결과를 정확히 해석하도록 컨텍스트를 증강함.

### 3.4 과제 데이터 연동 (Phase M)

- `integrations/aidatahub_client.py` — AIDataHub REST 연동(urllib, X-API-Key).
  - 과제 등록 — `register_odb(project, rev, stage, ...)`. `project=과제명(Z3)`, `version=Rev`, `stage=개발단계(DV)`.
  - **검색 가능성 확보** — project/version은 저장되나 질의 불가하므로 `tags=['project:Z3','rev:B','stage:DV']`에 동시 기입함.
  - 조회 — `find_records`·`get_record`·`resolve_odb_path`.
- `integrations/gateway_register.py` — MCP federation 게이트웨이 upstream 등록 payload/호출.

---

## 4. MCP 노출 (odb-analyzer)

- 정체성 — 이 시스템은 라우터가 아니라 **ODB++ 설계 분석기** 임.
  - `SERVER_NAME='odb-analyzer'`, `VERSION='1.1.0'`, `TITLE='ODB++ 설계 분석기'`.
- 도구 — 19종.
  - 등록/분석 — register_dataset, analyze_project, odb_inspect, odb_analyze, package_features.
  - 대리모델 — metamodel_list, metamodel_infer.
  - 라우팅/평가 — route, get_metrics, net_diff, spice_export, dashboard, sim_agg, route_viewer.
  - 물리 — em_dispatch, em_run, xtalk_sim, pdn, si_report.
- 핵심 함수 — `_handle(request) → response|None`. 순수 함수로 stdio·HTTP 양쪽에서 재사용함.

---

## 5. HTTP 브리지 (Phase M-4)

- 필요성 — AIDataHub MCP federation은 Streamable HTTP transport만 실동작하며 stdio는 미구현임.
- 구현 — `bga_router/http_bridge.py`.
  - stdlib `http.server`만 사용. 외부 의존 zero.
  - 단일 엔드포인트 `POST /mcp/`. `_handle` 순수 함수를 재사용함.
  - request(id 있음) → HTTP 200 + `application/json` + JSON-RPC 응답 1개.
  - notification(id 없음) → HTTP 202 + 빈 본문.
  - stateless — `mcp-session-id` 응답 헤더 미발급.
  - 옵션 bearer 인증 — `Authorization: Bearer <token>`(`ODB_MCP_TOKEN`).
- 실행 — `python -m bga_router.http_bridge --port 9040` (cwd=bga_escape).
- 등록 — `gateway_register.build_http_upstream('http://localhost:9040/mcp/')`.
- 검증 — 라이브 핸드쉐이크로 initialize/tools_list/tools_call/notification 확인. 단위 8건 통과.

---

## 6. 워크플로우

```
과제명(Z3)/Rev/DV 질의
      │
      ▼
odb-analyzer MCP ──(HTTP 브리지 POST /mcp/)── federation 게이트웨이
      │
      ▼
AIDataHub 조회(tags: project/rev/stage) → ODB++ 경로 resolve
      │
      ▼
ODB++ 직접 분석(Phase K) → 특징 추출(package_features)
      │
      ▼
대리모델 추론(metamodel_infer) → 사전 평가·피드백
```

---

## 7. 검증 현황

- 전체 회귀 — 1,159건 통과.
- HTTP 브리지 — 단위 8건(health/initialize/tools_list/tools_call/notification/404/400/auth) 통과.
- 라이브 핸드쉐이크 — 실 프로세스 기동 후 initialize→200, tools_list→200(19종), tools_call→200, notification→202 확인.

---

## 8. 미결 항목

- 실 AIDataHub 서버 미기동 — 테스트 ODB의 실제 등록(Z3/Rev/DV)은 서버 기동 후 수행 필요.
- 실 게이트웨이 등록 — federation 인스턴스에 http upstream(`http://<host>:9040/mcp/`) 등록은 운영 배포 시 수행.
- 대리모델 확장 — 현재 열충격 1종. SI/PI/EMI·낙하·충격 등은 시뮬레이션 누적에 따라 순차 추가.
