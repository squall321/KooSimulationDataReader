# Phase L — metamodel 파이프라인 checklist

Companion to `2026-07-07-phase-L-metamodel-plan.md`.

## L-1 패키지 intrinsic 피처

- [ ] `bga_router/metrics/package_features.py` 신규
- [ ] `load_packages(eda_or_odb)` — EDA 파서에서 Component+Package 로드
- [ ] `resolve_side(odb_dir)` — comp_+_top/bot components 파일로 side_by_ref
- [ ] `package_intrinsic(comp, pkg, side)` → {ref_des, pkg_name, side,
      center_xy, width_mm, height_mm, area_mm2, pin_count, pitch_mm,
      aspect_ratio, rotation_deg}
- [ ] 단위 테스트 (합성 Component/Package)

## L-2 관계 피처

- [ ] `neighbors(pkgs, ref, radius_mm, same_side=True)` — 거리+edge거리
- [ ] `opposite_side_packages(pkgs, ref)` — 반대면 + 겹침 면적
- [ ] `pairwise(pkgs, a, b)` — 중심/edge 거리, 겹침, side 일치
- [ ] `board_context(pkgs, ref, board_bbox)` — 면적비율, edge거리
- [ ] `bbox_overlap_area(bbox_a, bbox_b)` 헬퍼
- [ ] `summarize_packages(...)` — 전체 intrinsic + top/bottom 스택 쌍 목록
- [ ] 손계산 테스트 (거리/겹침/side)

## L-3 메타모델 프레임워크

- [ ] `bga_router/metamodel/__init__.py`
- [ ] `base.py` — Metamodel ABC (name/input_features/output_keys/predict/describe)
- [ ] `registry.py` — @register_metamodel + get + list
- [ ] `feature_spec.py` — FeatureVectorSpec (매핑/범위검증/누락처리)
- [ ] 테스트 (등록/조회/입력검증)

## L-4 Analytical + thermal_shock_v0

- [ ] `analytical.py` — AnalyticalMetamodel (callable 공식 래핑)
- [ ] `models/thermal_shock_v0.py` 등록 — CTE×크기×근접 휴리스틱
- [ ] 출력 provenance='reference_analytical' 명시
- [ ] property test (겹침↑→risk↑, 거리↑→risk↓ 단조성)

## L-5 Table + Polynomial (JSON 로드)

- [ ] `table.py` — TableMetamodel (JSON 룩업/선형보간)
- [ ] `polynomial.py` — PolynomialMetamodel (JSON 계수, 다항 평가)
- [ ] optional `sklearn_model.py` / `onnx_model.py` (라이브러리 있을 때만)
- [ ] JSON 모델 로드 테스트

## L-6 추론

- [ ] `infer.py` — infer(odb/eda, model, target?, partner?)
- [ ] infer_all_pairs — 상/하단 쌍 스캔 → 위험 순위
- [ ] 테스트 (합성 배치 → 추론)

## L-7 MCP + CLI

- [ ] MCP `package_features` tool
- [ ] MCP `metamodel_infer` tool
- [ ] MCP `metamodel_list` tool
- [ ] CLI `pkg-features` 서브명령
- [ ] CLI `metamodel` 서브명령
- [ ] MCP tool 목록 테스트 갱신

## L-8 학습 형식 문서

- [ ] `docs/metamodel-training-format.md`
- [ ] `metamodel/train_stub.py` (numpy 있으면 poly fit → JSON export)

## 검증 게이트

- [ ] 전체 pytest green
- [ ] P3 8 BGA 상/하단 쌍 추출 + thermal_shock 추론 스모크
- [ ] MCP 라이브 세션에서 metamodel_infer 호출 성공
