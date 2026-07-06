# Phase L — ODB++ 패키지 물리 피처 → ML 메타모델 추론 파이프라인

**Date**: 2026-07-07
**Scope**: 사용자 비전 — "ODB++에서 회로뿐 아니라 패키지 크기/주변/반대면 등
다양한 물리 상태를 추출하고, 그것을 ML 기반 surrogate(메타모델)의 입력으로
연결해 시뮬레이션 경험이 담긴 결과(예: 열충격)를 사전 추론하는 워크플로우."

## 1. 목표와 배경

### 왜 필요한가
Full 시뮬레이션(열, 응력, 열충격)은 비싸다. 하지만 **패키지 배치의 물리적
관계**(상/하단 패키지 크기, 중심 거리, 겹침 면적, CTE 불일치 등)만으로도
이미 학습된 surrogate 모델이 결과를 근사할 수 있다. ODB++에서 그 입력 피처를
자동 추출하면, 설계 단계에서 시뮬 없이 위험도를 미리 본다.

### 핵심 데이터 흐름
```
ODB++ (.tgz)
  → EDA 파서 (Component/Package 물리 정보)
  → 패키지 피처 추출 (intrinsic + relational)
  → 메타모델 입력 벡터 (FeatureVectorSpec으로 매핑/검증)
  → 메타모델 추론 (pluggable surrogate)
  → 예측 결과 {risk_score, estimated_cycles, confidence, provenance}
```

## 2. 확보된 데이터 (실사 완료)

`src/eda_parser.py`.
- `Package`: name, pitch_mm, bbox_min/max_x/y_mm (→ 크기 W×H, 면적).
- `Component`: ref_des, pkg_name, x_mm, y_mm, rotation_deg, mirrored.
- **side(TOP/BOT) 판정**: `mirrored` 플래그는 Mentor에서 신뢰 불가.
  정답은 `steps/<step>/layers/comp_+_top/components` vs `comp_+_bot/components`
  에 어느 쪽에 나타나는가. `registry.py`가 이미 `side_by_pkg`로 구현.

`bga_router/registry.py`.
- `known_bgas`: name, ref_des, side(TOP/BOT), pin_count, pitch_mm.

→ 패키지 intrinsic 피처 + side + 좌표 전부 이미 접근 가능. Phase L은
**추출 로직 + 관계 계산 + 메타모델 프레임워크**만 신규.

## 3. 아키텍처 (3 계층)

### 계층 1 — 패키지 피처 추출 (`bga_router/metrics/package_features.py`)

**Intrinsic (패키지 단위)**.
- ref_des, pkg_name, side (TOP/BOT), center_xy, width/height_mm, area_mm2,
  pin_count, pitch_mm, aspect_ratio, rotation_deg.

**Relational (관계 — 메타모델의 핵심 입력)**.
- `neighbors(ref, radius_mm, same_side=True)` — 반경 내 같은 면 패키지 +
  중심거리 + edge 거리.
- `opposite_side_packages(ref)` — **반대면 패키지** + 중심거리 + bbox 겹침
  면적(열충격의 top-bottom 스택 관계 핵심).
- `pairwise(ref_a, ref_b)` — 중심-중심 거리, edge-edge 거리, 겹침 면적,
  side 일치 여부.
- `board_context(ref)` — 보드 대비 면적 비율, 보드 edge까지 최단거리.

**출력**: JSON 직렬화 가능한 dict. MCP/CLI로 그대로 노출.

### 계층 2 — 메타모델 프레임워크 (`bga_router/metamodel/`)

의존성 zero가 기본. sklearn/onnx는 있으면 쓰고 없으면 graceful skip.

- `base.py` — `Metamodel` ABC:
  - `.name: str`
  - `.input_features: list[str]` — 필요한 피처 키 (검증용)
  - `.output_keys: list[str]`
  - `.predict(features: dict) -> dict` — 예측 + confidence + provenance
  - `.describe() -> dict` — 모델 메타데이터
- `registry.py` — 이름으로 등록/조회 (`@register_metamodel`).
- 구체 구현체:
  - `AnalyticalMetamodel` — 공식 기반 reference (학습 모델의 placeholder).
  - `TableMetamodel` — JSON 룩업/보간 테이블 로드 (학습 결과 직렬화 형태).
  - `PolynomialMetamodel` — JSON 계수 로드 (다항 회귀 surrogate).
  - `SklearnMetamodel` (optional) — pickle 로드, sklearn 있을 때만.
  - `OnnxMetamodel` (optional) — onnxruntime 있을 때만.
- `feature_spec.py` — `FeatureVectorSpec`: 추출 피처 → 모델 입력 매핑 +
  범위 검증 + 누락 처리. ODB 추출과 모델 입력을 명시적으로 연결.

### 계층 3 — 참조 열충격 메타모델 (`thermal_shock_v0`)

구체 예시 (사용자 시나리오). `AnalyticalMetamodel` 기반.
- 입력: top_pkg_area, bot_pkg_area, center_distance_mm, overlap_area_mm2,
  min_pkg_dim_mm, board_thickness_mm(스택업), max_pitch_mm.
- 출력: thermal_shock_risk_score(0~1), estimated_cycles_to_failure,
  confidence, provenance.
- surrogate 공식: CTE 불일치 × 크기 × 근접도 휴리스틱 (Coffin-Manson 계열
  근사). **명시적으로 reference/placeholder** — 실제 학습 모델이 교체.

## 4. 추론 워크플로우 + 인터페이스

- `bga_router/metamodel/infer.py`:
  - `infer(odb_dir_or_eda, metamodel_name, target_pkg=None, partner_pkg=None)`
    → 피처 추출 → 모델 선택 → 입력 검증 → predict.
  - `infer_all_pairs(...)` — 상/하단 패키지 쌍 전부 스캔 → 위험 순위.
- MCP tools 신규: `package_features`, `metamodel_infer`, `metamodel_list`.
- CLI 신규: `pkg-features`, `metamodel`.

## 5. 확장성 (실제 학습 모델 투입 경로)

- 학습 모델은 `bga_router/metamodel/models/*.json`(계수/테이블) 또는
  `*.pkl`/`*.onnx`로 드롭인.
- `docs/metamodel-training-format.md` — 학습 데이터 형식 명세:
  `{features: {...}, targets: {...}}` 리스트. 실제 시뮬 결과가 이 형식으로
  쌓이면 재학습 → JSON export → 드롭인.
- `train_stub.py` — 다항 회귀 fit → JSON export 예시 (numpy 있으면).

## 6. 서브 단계 (구현 순서)

| # | 내용 | 산출물 | effort |
|---|---|---|---|
| L-1 | 패키지 intrinsic 피처 추출 | package_features.py 일부 | S |
| L-2 | 관계 피처 (neighbors/opposite/pairwise/board) | package_features.py 완성 | M |
| L-3 | 메타모델 base + registry + FeatureVectorSpec | metamodel/base,registry,feature_spec | M |
| L-4 | AnalyticalMetamodel + thermal_shock_v0 참조 | metamodel/analytical, models/thermal_shock_v0 | M |
| L-5 | TableMetamodel + PolynomialMetamodel (JSON 로드) | metamodel/table,polynomial | M |
| L-6 | infer.py + infer_all_pairs | metamodel/infer | S |
| L-7 | MCP tools + CLI 배선 | mcp_server, cli | S |
| L-8 | 학습 형식 문서 + train_stub | docs, train_stub | S |

## 7. 비목표 (이번 스코프 밖)

- 실제 열/응력 유한요소 시뮬 (별도 solver — 메타모델이 대체하려는 대상).
- 대규모 학습 데이터 수집/라벨링.
- GPU 추론 / 딥러닝 프레임워크. surrogate는 경량 (analytical/poly/table/
  optional sklearn) 우선.
- 3D 패키지 높이/열저항 상세 모델 (2D bbox + 스택업 두께로 근사).

## 8. 리스크 + 결정

- **side 판정 신뢰성**: mirrored 대신 comp_+_top/bot 파일 사용 (registry
  방식 재사용). 파일 없으면 mirrored fallback + 플래그.
- **학습 모델 부재**: reference analytical 모델로 프레임워크를 완성하고
  실제 모델은 드롭인 인터페이스로. surrogate 결과에 항상
  `provenance: 'reference_analytical'` 명시 — 학습 모델 결과와 혼동 방지.
- **피처 정규화**: FeatureVectorSpec에서 단위/범위 명시. 학습 모델의
  정규화 파라미터도 JSON에 함께 저장.

## 9. 검증 전략

- 각 관계 함수: 합성 2~3 패키지 배치로 손계산 검증 (거리/겹침/side).
- thermal_shock_v0: 큰 패키지 겹침 → 높은 risk, 작고 멀리 → 낮은 risk
  단조성 property test.
- FeatureVectorSpec: 누락 피처 → 명시적 에러, 범위 밖 → 경고 플래그.
- P3 실측: 8 BGA에서 상/하단 쌍 추출 → 추론 스모크.
- 전체 pytest green 유지.
