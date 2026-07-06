# Phase L — metamodel context notes

Append-only 결정 기록.

## 2026-07-07 — kickoff

### 왜 별도 계층인가
사용자 비전은 "ODB 물리 피처 → ML surrogate → 시뮬 사전 추론". 이건
배선 평가(Phase A~J)와 다른 축이다. 배선 평가는 net/copper 중심,
메타모델은 **패키지 물리 배치** 중심. 그래서 `metamodel/` 별도 패키지 +
`package_features.py` 별도 모듈.

### side 판정 — mirrored 신뢰 불가
registry.py 주석에 명시: Mentor export는 BOT 부품도 mirrored=False로
나올 수 있다. 정답은 `layers/comp_+_top/components` /
`comp_+_bot/components` 에 나타나는가. 이 로직을 resolve_side로 재사용.
파일 없으면 mirrored fallback + side_source 플래그.

### 메타모델 pluggable — 학습 모델 드롭인
지금은 학습 데이터도 학습 모델도 없다. 그래서:
1. 프레임워크(base/registry/feature_spec)를 먼저 완성.
2. reference `AnalyticalMetamodel`로 동작을 증명 (thermal_shock_v0).
3. 실제 모델은 JSON(계수/테이블) 또는 pkl/onnx로 드롭인.
모든 reference 출력에 provenance='reference_analytical' 붙여 학습 모델
결과와 혼동 방지. 학습 모델은 provenance='trained:<name>@<version>'.

### 의존성 정책
surrogate는 경량 우선: analytical(공식) / polynomial(JSON 계수) /
table(JSON 룩업) 은 stdlib+math만. sklearn/onnx는 optional import —
없으면 그 모델 타입만 skip, 프레임워크는 정상.

### FeatureVectorSpec — 추출↔입력 명시 매핑
ODB 추출 피처 이름과 모델 입력 이름이 다를 수 있다. FeatureVectorSpec이
{model_input: extracted_key} 매핑 + 단위 + [min,max] 범위 + default를
선언. 누락 → 명시적 에러(silent 0 금지). 범위 밖 → 결과에 out_of_range
플래그(추론은 하되 신뢰도 낮춤).

### thermal_shock_v0 공식 (reference)
Coffin-Manson 계열 근사. 열충격 위험 ∝ (CTE 불일치) × (패키지 크기) ×
(근접도). 근접도 = 반대면 겹침 면적 / min 패키지 면적. 큰 패키지 2개가
상/하단에 겹쳐 있으면 solder joint 열피로 위험 최대. 이건 **물리적으로
그럴듯한 placeholder** — 실제 학습 모델이 정량 교체. estimated_cycles는
risk_score의 역함수 근사.

### 왜 infer_all_pairs
사용자 시나리오가 "두 패키지가 상/하단에 있고 크기+거리로 메타모델".
그래서 단일 쌍뿐 아니라 보드 전체 상/하단 겹침 쌍을 스캔해 위험 순위를
내는 infer_all_pairs가 실사용 진입점.

### Phase K와의 관계
Phase K(odb_inspect/odb_traces)는 회로/레이어 직독. Phase L은 패키지
직독. 둘 다 "재라우팅 없이 ODB 원본 분석"이라는 사용자 우선순위에 부합.
Phase K 워크플로우 완료 후 통합 → 그 다음 Phase L 착수.

### 커밋 단위 (semantic)
- L-1+L-2: 패키지 피처 추출 (intrinsic+relational) 한 커밋
- L-3: 메타모델 프레임워크
- L-4: analytical + thermal_shock_v0
- L-5: table + polynomial
- L-6+L-7: infer + MCP/CLI 배선
- L-8: 학습 문서 + train_stub
