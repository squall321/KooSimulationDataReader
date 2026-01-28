# OpenRadioss DYNAIN 기능 분석

## 개요

OpenRadioss는 LS-DYNA 키워드 입력 파일을 읽고 실행할 수 있는 기능과 함께, LS-DYNA 형식의 DYNAIN 파일을 출력하는 기능을 제공합니다. 이를 통해 LS-DYNA와 OpenRadioss 간의 양방향 호환성을 지원합니다.

---

## 1. LS-DYNA 키워드 입력 지원

### 1.1 지원되는 LS-DYNA 버전

OpenRadioss는 여러 버전의 LS-DYNA 키워드 형식을 지원합니다:

```
hm_cfg_files/config/CFG/
├── Keyword971/           # LS-DYNA R5 (기본)
├── Keyword971_R6.1/      # LS-DYNA R6.1
├── Keyword971_R7.1/      # LS-DYNA R7.1
├── Keyword971_R8.0/      # LS-DYNA R8.0
├── Keyword971_R9.3/      # LS-DYNA R9.3
├── Keyword971_R10.1/     # LS-DYNA R10.1
├── Keyword971_R11.1/     # LS-DYNA R11.1
├── Keyword971_R11.2/     # LS-DYNA R11.2
├── Keyword971_R12.0/     # LS-DYNA R12.0
├── Keyword971_R13.0/     # LS-DYNA R13.0
└── Keyword971_R14.1/     # LS-DYNA R14.1 (최신)
```

### 1.2 CFG 파일 구조

각 LS-DYNA 버전 디렉토리에는 다음과 같은 CFG 파일들이 있습니다:

```
Keyword971_R14.1/
├── data_hierarchy.cfg    # 데이터 계층 구조 정의
├── MAT/                  # 재료 모델 (material)
│   ├── mat_001.cfg       # *MAT_ELASTIC
│   ├── mat_003.cfg       # *MAT_PLASTIC_KINEMATIC
│   ├── mat_024.cfg       # *MAT_PIECEWISE_LINEAR_PLASTICITY
│   └── ...
├── PROPERTY/             # 요소 속성 (section, property)
│   ├── sect_shell.cfg    # *SECTION_SHELL
│   ├── sect_solid.cfg    # *SECTION_SOLID
│   ├── EOS1.cfg          # *EOS_LINEAR_POLYNOMIAL
│   └── ...
├── CONTROL_CARDS/        # 제어 카드
│   ├── control_contact.cfg
│   ├── control_timestep.cfg
│   └── ...
├── LOADS/                # 하중 및 경계조건
├── SETS/                 # 집합 정의
└── CONTACT/              # 접촉 정의
```

### 1.3 키워드 변환 메커니즘

#### CFG 파일의 역할

CFG(Configuration) 파일은 LS-DYNA 키워드를 OpenRadioss 내부 데이터 구조로 변환하는 규칙을 정의합니다:

```cfg
// 예: mat_001.cfg (LS-DYNA *MAT_ELASTIC)
SKEYWORDS_IDENTIFIER(COMMON)
{
    KEYWORD_STR                   = "*MAT_ELASTIC";
    KEYWORD_OPTION                = "";
}

DEFAULTS(COMMON)
{
    E       = 210000.0;  // Young's modulus
    PR      = 0.3;       // Poisson's ratio
    RHO     = 7.85e-9;   // Density
}

FORMAT(radioss2025)
{
    // OpenRadioss로 변환될 때의 형식
    HEADER("/MAT/LAW1/%d",_ID_);
    CARD("%-100s",TITLE);
    COMMENT("#              RHO_I");
    CARD("%20lg",RHO);
    COMMENT("#                  E                  NU");
    CARD("%20lg%20lg",E,PR);
}
```

#### 변환 흐름

```
LS-DYNA 입력 파일 (.k, .key, .dyn)
         ↓
    CFG 파일 파싱
         ↓
  내부 데이터 구조 생성
         ↓
  OpenRadioss 엔진 실행
```

### 1.4 지원되는 주요 키워드

#### 재료 모델 (MAT)
- `*MAT_ELASTIC` → `/MAT/LAW1`
- `*MAT_PLASTIC_KINEMATIC` → `/MAT/LAW2`
- `*MAT_PIECEWISE_LINEAR_PLASTICITY` → `/MAT/LAW36`
- `*MAT_JOHNSON_COOK` → `/MAT/LAW2` + `/MAT/LAW4`
- 100개 이상의 재료 모델 지원

#### 요소 속성 (SECTION)
- `*SECTION_SHELL`
- `*SECTION_SOLID`
- `*SECTION_BEAM`
- `*SECTION_DISCRETE`

#### 제어 카드 (CONTROL)
- `*CONTROL_TERMINATION`
- `*CONTROL_TIMESTEP`
- `*CONTROL_CONTACT`
- `*CONTROL_ENERGY`
- `*CONTROL_OUTPUT`

#### 접촉 (CONTACT)
- `*CONTACT_AUTOMATIC_SURFACE_TO_SURFACE`
- `*CONTACT_AUTOMATIC_SINGLE_SURFACE`
- `*CONTACT_NODES_TO_SURFACE`

#### 하중 및 경계조건
- `*BOUNDARY_SPC_NODE`
- `*LOAD_NODE_POINT`
- `*LOAD_BODY_FORCE`
- `*INITIAL_VELOCITY`

#### 집합 정의
- `*SET_NODE_LIST`
- `*SET_SHELL_LIST`
- `*SET_SOLID_LIST`
- `*SET_PART_LIST`

---

## 2. DYNAIN 파일 출력 기능

### 2.1 DYNAIN이란?

DYNAIN은 LS-DYNA의 재시작(restart) 파일 형식으로, 시뮬레이션의 특정 시점에서의 모든 상태(위치, 속도, 응력, 변형률 등)를 저장합니다. OpenRadioss는 이 형식으로 결과를 출력하여 LS-DYNA에서 후처리하거나 재시작할 수 있습니다.

### 2.2 DYNAIN 출력 키워드

#### 기본 구문

```
/DYNAIN/DT/dt_dynain/part_ID1/part_ID2/.../part_IDn
```

또는 모든 파트에 대해:

```
/DYNAIN/DT/ALL
dt_dynain
```

#### 파라미터

| 파라미터 | 타입 | 설명 |
|----------|------|------|
| dt_dynain | FLOAT | DYNAIN 파일 출력 시간 간격 (초) |
| part_ID | INT | 출력할 파트 ID (복수 지정 가능) |
| ALL | - | 모든 파트 출력 |

#### 예제

```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
# 특정 파트만 출력 (파트 1, 2, 3)
/DYNAIN/DT
#            dt_dynain
               0.001
# part_ID1  part_ID2  part_ID3
         1         2         3

# 모든 파트 출력
/DYNAIN/DT/ALL
#            dt_dynain
               0.0005
```

### 2.3 DYNAIN 파일 구조

출력되는 DYNAIN 파일은 다음과 같은 구조를 가집니다:

```
<rootname>_<nnnn>.dynain

예: crash_model_0001.dynain
    crash_model_0002.dynain
    ...
```

#### 파일 내용

```
$RADIOSS DYNAIN FILE crash_model_0001.dynain
*KEYWORD
*NODE
$# nodeid               x               y               z      tc      rc
       1       0.000000       0.000000       0.000000       0       0
       2       1.000000       0.000000       0.000000       0       0
      ...
*ELEMENT_SHELL
$#   eid     pid      n1      n2      n3      n4      n5      n6      n7      n8
       1       1       1       2       5       4       0       0       0       0
       2       1       2       3       6       5       0       0       0       0
      ...
*INITIAL_VELOCITY_NODE
$# nid     vx     vy     vz
     1  1.234  0.000  0.000
    ...
*INITIAL_STRESS_SHELL
$# eid    nip      t       s1       s2       s3       s4       s5       s6
     1      1     0   123.45   -45.67    0.00   12.34    0.00    0.00
     1      2     0   125.34   -46.23    0.00   13.12    0.00    0.00
    ...
*INITIAL_STRAIN_SHELL
$# eid    nip      t      eps1     eps2     eps3     eps4     eps5     eps6
     1      1     0   0.00123  -0.00045  0.00000  0.00012  0.00000  0.00000
    ...
*END
```

### 2.4 출력되는 상태 변수

#### 노드 정보
- 좌표 (X, Y, Z)
- 속도 (VX, VY, VZ)
- 가속도 (AX, AY, AZ)

#### 쉘 요소 정보
- 응력 성분 (σ₁, σ₂, σ₃, τ₁₂, τ₁₃, τ₂₃)
  - 각 적분점별로 저장
  - 각 층(layer)별로 저장 (복합재의 경우)
- 변형률 성분 (ε₁, ε₂, ε₃, γ₁₂, γ₁₃, γ₂₃)
- 두께 변화
- 플라스틱 변형률

#### 솔리드 요소 정보
- 응력 텐서 (6개 성분)
- 변형률 텐서 (6개 성분)
- 플라스틱 변형률

### 2.5 주요 소스 코드 분석

#### Starter (전처리기) - check_dynain.F

위치: `starter/source/starter/check_dynain.F`

**목적**: DYNAIN 출력을 위한 사전 검사 수행

**주요 기능**:

1. **엔진 파일 확인**
   - `<rootname>_0001.rad` 또는 `<rootname>D01` 파일 읽기
   - `/DYNAIN/DT` 키워드 검색

2. **파트 ID 검증**
   ```fortran
   IF(KEYA(1:14)=='/DYNAIN/DT/ALL') THEN
     ! 모든 파트 출력
     NPRT = NPART
     IPRTALL = 1
   ELSEIF(KEYA(1:10)=='/DYNAIN/DT') THEN
     ! 지정된 파트만 출력
     READ(71,*,END=20) T0,DT0
     ! 파트 ID 리스트 읽기
   ENDIF
   ```

3. **요소 ID 충돌 검사**
   - 3절점 쉘과 4절점 쉘이 같은 ID를 가지는지 확인
   - LS-DYNA는 같은 ID를 허용하지 않으므로 에러 발생
   ```fortran
   IF(NELIDC(INDXC(I))==NELIDTG(INDXTG(J))) THEN
     ! ERROR: 4 node shells and 3 node shells have same user ID
     CALL ANCMSG(MSGID=1910, MSGTYPE=MSGERROR, ...)
   ENDIF
   ```

#### Engine (계산 엔진) - gendynain.F

위치: `engine/source/output/dynain/gendynain.F`

**목적**: DYNAIN 파일 생성

**주요 기능**:

1. **파일 생성**
   ```fortran
   FILNAM=ROOTNAM(1:ROOTLEN)//'_'//CHSTAT//'.dynain'
   OPEN(UNIT=IUDYNAIN, FILE=FILNAM, ...)
   WRITE(IUDYNAIN,'(2A)')'$RADIOSS DYNAIN FILE ',FILNAM
   ```

2. **노드 데이터 출력**
   ```fortran
   CALL DYNAIN_NODE(X, NUMNOD, ITAB, ITABG, LENG,
                    NODGLOB, WEIGHT, NODTAG, DYNAIN_DATA)
   ```
   - 좌표, 속도, 가속도 출력

3. **쉘 요소 데이터 출력**
   ```fortran
   ! 단일 프로세서
   CALL DYNAIN_SHEL_MP(ITAB, ITABG, LENG, IGEO, IXC,
                       IXTG, IPARTC, IPARTTG, DYNAIN_DATA, ...)

   ! 병렬 처리 (SPMD)
   CALL DYNAIN_SHEL_SPMD(ITAB, ITABG, LENG, IGEO, IXC,
                         IXTG, IPARTC, IPARTTG, DYNAIN_DATA, ...)
   ```

4. **응력/변형률 출력**
   ```fortran
   ! 기하학적 변형률 기반
   IF(DYNAIN_DATA%DYNAIN_C(4)==1) THEN
     CALL DYNAIN_C_STRSG(ELBUF_TAB, IPARG, IGEO, IXC,
                         IXTG, WA, WAP0, ...)
   ENDIF

   ! 대수적 변형률 기반
   IF(DYNAIN_DATA%DYNAIN_C(5)==1) THEN
     CALL DYNAIN_C_STRAG(ELBUF_TAB, IPARG, IPM, IGEO, IXC,
                         IXTG, WA, WAP0, ...)
   ENDIF
   ```

5. **파일 종료**
   ```fortran
   WRITE(IUDYNAIN,'(A)')'*END'
   CLOSE(UNIT=IUDYNAIN)
   WRITE(IOUT,1000) FILNAM
   ```

#### 병렬 처리 지원

DYNAIN 출력은 MPI 병렬 실행 환경에서도 동작합니다:

```fortran
IF(NSPMD > 1) CALL SPMD_OUTPITAB(ITAB, WEIGHT, NODGLOB, ITABG)
```

- 각 프로세서의 데이터를 수집
- 마스터 프로세서 (ISPMD==0)가 파일 작성

---

## 3. 사용 시나리오

### 3.1 LS-DYNA 입력 → OpenRadioss 실행

**목적**: LS-DYNA 형식의 기존 모델을 OpenRadioss에서 실행

**절차**:

1. **입력 파일 준비**
   ```bash
   # LS-DYNA 키워드 파일
   model.k
   ```

2. **OpenRadioss Starter 실행**
   ```bash
   ./starter_linux64_gf -i model.k
   ```
   - CFG 파일을 사용하여 LS-DYNA 키워드를 OpenRadioss 형식으로 변환
   - `<rootname>_0000.rad`, `<rootname>_0001.rad` 생성

3. **OpenRadioss Engine 실행**
   ```bash
   ./engine_linux64_gf -i <rootname>_0001.rad
   ```

**제한사항**:
- 모든 LS-DYNA 키워드가 지원되는 것은 아님
- 일부 고급 기능(예: implicit solver 옵션)은 변환되지 않을 수 있음
- 경고 메시지를 확인하여 미지원 키워드 파악

### 3.2 OpenRadioss → DYNAIN → LS-DYNA

**목적**: OpenRadioss 시뮬레이션 결과를 LS-DYNA에서 후처리

**절차**:

1. **OpenRadioss 입력 파일 작성**
   ```
   /DYNAIN/DT/ALL
   0.001
   ```

2. **OpenRadioss 실행**
   ```bash
   ./starter_linux64_gf -i model_0000.rad
   ./engine_linux64_gf -i model_0001.rad
   ```
   - `model_0001.dynain`, `model_0002.dynain`, ... 생성

3. **LS-DYNA에서 읽기**
   ```
   *KEYWORD
   *INCLUDE
   model_0001.dynain
   *CONTROL_TERMINATION
   $#  endtim    endcyc     dtmin    endeng    endmas
      0.010         0       0.0       0.0       0.0
   *END
   ```

**응용**:
- OpenRadioss의 빠른 explicit 솔버로 초기 단계 계산
- LS-DYNA로 후처리 또는 후속 해석 수행
- 다른 CAE 툴과의 연동

### 3.3 DYNAIN을 이용한 재시작

**목적**: 시뮬레이션 중간에서 재시작

**절차**:

1. **초기 시뮬레이션**
   ```
   /DYNAIN/DT/ALL
   0.002  # 2ms마다 DYNAIN 출력
   ```
   - t=0.002s: `model_0001.dynain`
   - t=0.004s: `model_0002.dynain`
   - ...

2. **특정 시점에서 재시작**
   - `model_0002.dynain`을 새 모델의 초기 조건으로 사용
   - 다른 경계조건이나 하중 적용 가능

---

## 4. 내부 데이터 구조

### 4.1 DYNAIN_DATABASE 구조체

위치: 공통 모듈

```fortran
TYPE DYNAIN_DATABASE
  INTEGER :: IDYNAINF          ! DYNAIN 파일 번호
  INTEGER :: DYNAIN_CHECK      ! 검사 플래그
  INTEGER :: ZIPDYNAIN         ! 압축 플래그
  INTEGER :: MX_DYNAIN         ! 최대 옵션 수
  INTEGER :: DYNAIN_C(10)      ! 출력 옵션 배열
  INTEGER :: DYNAIN_NUMELC     ! 4절점 쉘 수
  INTEGER :: DYNAIN_NUMELTG    ! 3절점 쉘 수
  INTEGER :: DYNAIN_NUMELC_G   ! 전체 4절점 쉘 수 (병렬)
  INTEGER :: DYNAIN_NUMELTG_G  ! 전체 3절점 쉘 수 (병렬)
END TYPE
```

### 4.2 DYNAIN_C 옵션

| 인덱스 | 옵션 | 설명 |
|--------|------|------|
| 1 | - | (예약됨) |
| 2 | - | (예약됨) |
| 3 | - | (예약됨) |
| 4 | STRSG | 기하학적 변형률 출력 |
| 5 | STRAG | 대수적 변형률 출력 |

---

## 5. 주요 서브루틴 설명

### 5.1 starter/source/starter/check_dynain.F

```fortran
SUBROUTINE CHECK_DYNAIN(IPART, IPARTC, IPARTTG, IXC, IXTG, DYNAIN_CHECK)
```

**목적**: DYNAIN 출력 전 검증

**입력**:
- `IPART`: 파트 정보 배열
- `IPARTC`: 4절점 쉘의 파트 ID
- `IPARTTG`: 3절점 쉘의 파트 ID
- `IXC`: 4절점 쉘 연결 정보
- `IXTG`: 3절점 쉘 연결 정보

**출력**:
- `DYNAIN_CHECK`: 검사 결과 (0=미사용, 1=사용)

**주요 로직**:
1. 엔진 파일(`_0001.rad` 또는 `D01`) 열기
2. `/DYNAIN/DT` 키워드 검색
3. 파트 ID 파싱 및 검증
4. 요소 ID 충돌 검사 (3절점 vs 4절점)

### 5.2 engine/source/output/dynain/gendynain.F

```fortran
SUBROUTINE GENDYNAIN(X, ELBUF_TAB, BUFEL, IXC, IXTG, ...)
```

**목적**: DYNAIN 파일 생성

**주요 호출 서브루틴**:

1. **DYNAIN_NODE**: 노드 좌표 및 속도 출력
2. **DYNAIN_SHEL_MP**: 쉘 연결 정보 (단일 프로세서)
3. **DYNAIN_SHEL_SPMD**: 쉘 연결 정보 (병렬)
4. **DYNAIN_SIZE_C**: 버퍼 크기 계산
5. **DYNAIN_C_STRSG**: 기하학적 변형률 기반 응력/변형률
6. **DYNAIN_C_STRAG**: 대수적 변형률 기반 응력/변형률

### 5.3 engine/source/output/dynain/dynain_node.F

**목적**: 노드 데이터 출력

**출력 형식**:
```
*NODE
$# nodeid               x               y               z      tc      rc
     101       1.234567       2.345678       3.456789       0       0
```

### 5.4 engine/source/output/dynain/dynain_shel_mp.F / dynain_shel_spmd.F

**목적**: 쉘 요소 연결 정보 출력

**출력 형식**:
```
*ELEMENT_SHELL
$#   eid     pid      n1      n2      n3      n4
     201       1     101     102     105     104
```

### 5.5 engine/source/output/dynain/dynain_c_strsg.F

**목적**: 쉘 요소 응력/변형률 출력 (기하학적 변형률 기반)

**출력 형식**:
```
*INITIAL_STRESS_SHELL
$# eid    nip      t       s1       s2       s3       s4       s5       s6
   201      1     0  123.456  -67.890   0.000  12.345   0.000   0.000
   201      2     0  125.678  -68.901   0.000  13.456   0.000   0.000
```

- `nip`: 적분점 번호 (1~NPT)
- `t`: 층 번호 (복합재의 경우)
- `s1, s2`: 면내 주응력
- `s3`: 두께 방향 응력 (보통 0)
- `s4, s5, s6`: 전단 응력

---

## 6. 병렬 처리 (SPMD)

### 6.1 병렬 실행 시 DYNAIN 출력

OpenRadioss는 MPI 기반 영역 분할(domain decomposition) 병렬화를 지원합니다.

**문제점**:
- 각 프로세서가 자신의 영역 데이터만 보유
- 전체 모델 정보를 하나의 DYNAIN 파일로 통합 필요

**해결 방법**:

1. **데이터 수집**
   ```fortran
   IF(NSPMD > 1) CALL SPMD_OUTPITAB(ITAB, WEIGHT, NODGLOB, ITABG)
   ```
   - 각 프로세서의 노드/요소 데이터를 마스터(proc 0)로 전송

2. **마스터만 파일 작성**
   ```fortran
   IF(ISPMD == 0) THEN
     OPEN(UNIT=IUDYNAIN, FILE=FILNAM, ...)
     WRITE(IUDYNAIN, ...) ! 노드, 요소, 응력 등
     CLOSE(UNIT=IUDYNAIN)
   ENDIF
   ```

3. **전역 카운트**
   ```fortran
   DYNAIN_DATA%DYNAIN_NUMELC_G  = DYNAIN_DATA%DYNAIN_NUMELC  ! 전체 4절점 쉘
   DYNAIN_DATA%DYNAIN_NUMELTG_G = DYNAIN_DATA%DYNAIN_NUMELTG ! 전체 3절점 쉘
   ```

---

## 7. 에러 처리

### 7.1 주요 에러 메시지

#### MSGID=1908: 파트 ID 미발견
```
ERROR ID: 1908
** ERROR: PART ID XXXX NOT FOUND FOR DYNAIN OUTPUT
```

**원인**: `/DYNAIN/DT`에 지정된 파트 ID가 모델에 존재하지 않음

**해결**: 파트 ID 확인 및 수정

#### MSGID=1909: 파트 ID 미지정
```
ERROR ID: 1909
** ERROR: NO PART ID SPECIFIED FOR DYNAIN OUTPUT
```

**원인**: `/DYNAIN/DT` 키워드에 파트 ID가 없음

**해결**: 파트 ID 추가 또는 `/DYNAIN/DT/ALL` 사용

#### MSGID=1910: 요소 ID 충돌
```
ERROR ID: 1910
** ERROR: DYNAIN FILE CAN NOT BE WRITTEN
THESE 4 NODE SHELLS AND 3 NODE SHELLS HAVE SAME USER ID
```

**원인**: 3절점 쉘과 4절점 쉘이 동일한 요소 ID 사용

**해결**: 요소 ID 재할당 (LS-DYNA는 동일 ID 허용하지 않음)

### 7.2 경고 메시지

#### 미지원 LS-DYNA 키워드
```
** WARNING: KEYWORD *XXX_YYY IS NOT SUPPORTED
```

**원인**: 해당 LS-DYNA 키워드가 OpenRadioss에서 미지원

**해결**:
- OpenRadioss 등가 키워드로 수정
- 또는 해당 기능 제거

---

## 8. 성능 고려사항

### 8.1 DYNAIN 출력 빈도

**권장사항**:
- DYNAIN 출력은 I/O 집약적 작업
- 너무 자주 출력하면 시뮬레이션 속도 저하
- 일반적으로 0.5~2.0ms 간격 권장

**예**:
```
# 좋음 (1ms 간격)
/DYNAIN/DT/ALL
0.001

# 나쁨 (0.01ms 간격 - 너무 잦음)
/DYNAIN/DT/ALL
0.00001
```

### 8.2 파일 크기

대규모 모델의 경우 DYNAIN 파일이 매우 커질 수 있습니다:

| 모델 크기 | DYNAIN 파일 크기 (추정) |
|-----------|-------------------------|
| 10K 요소 | ~10 MB |
| 100K 요소 | ~100 MB |
| 1M 요소 | ~1 GB |

**압축 옵션**:
```fortran
DYNAIN_DATA%ZIPDYNAIN = 1  ! 압축 활성화
```

### 8.3 병렬 실행 시 오버헤드

병렬 실행 시 DYNAIN 출력은 다음 단계를 거칩니다:
1. 각 프로세서에서 로컬 데이터 수집
2. MPI 통신으로 마스터에 전송
3. 마스터에서 파일 작성

→ 프로세서 수가 많을수록 통신 오버헤드 증가

---

## 9. 디버깅 팁

### 9.1 DYNAIN 출력 확인

```bash
# DYNAIN 파일이 생성되었는지 확인
ls -lh *.dynain

# 파일 내용 간단히 확인
head -n 50 model_0001.dynain
grep "*ELEMENT_SHELL" model_0001.dynain
grep "*NODE" model_0001.dynain
```

### 9.2 LS-DYNA로 검증

```bash
# LS-DYNA로 DYNAIN 파일 읽기 테스트
ls-dyna i=test_dynain.k memory=100m
```

`test_dynain.k`:
```
*KEYWORD
*INCLUDE
model_0001.dynain
*CONTROL_TERMINATION
0.001
*END
```

### 9.3 로그 파일 확인

Starter 출력:
```
DYNAIN FILE CHECK: OK
NO ELEMENT ID CONFLICTS FOUND
```

Engine 출력:
```
DYNAIN FILE: model_0001.dynain WRITTEN
DYNAIN FILE: model_0002.dynain WRITTEN
...
```

---

## 10. 제한사항 및 알려진 이슈

### 10.1 지원되지 않는 요소 타입

#### DYNAIN 출력 지원 현황

**✅ 지원됨 (출력 가능)**:
- 4절점 쉘 (`*ELEMENT_SHELL`, 4 nodes)
- 3절점 쉘 (`*ELEMENT_SHELL`, 3 nodes)

**❌ 미지원 (출력 불가)**:
- 솔리드 요소 (`*ELEMENT_SOLID`) - **코드 미구현**
- 빔 요소 (`*ELEMENT_BEAM`) - **코드 미구현**
- 스프링/댐퍼 요소 - **코드 미구현**

**중요**: LS-DYNA 입력 파일 읽기는 솔리드 요소 포함 모든 요소 타입을 **지원**합니다. 단, OpenRadioss에서 계산 후 DYNAIN으로 **출력**하는 기능만 쉘 요소로 제한됩니다.

```
LS-DYNA 입력 읽기:    ✅ Solid/Beam/Shell 모두 지원
                           ↓
OpenRadioss 계산:      ✅ Solid/Beam/Shell 모두 계산
                           ↓
DYNAIN 출력:          ❌ Shell만 가능, Solid/Beam 불가
```

### 10.2 복합재 층별 데이터

- 복합재(/PROP/TYPE9, TYPE10, TYPE11)의 층별 응력/변형률 출력 지원
- 단, LS-DYNA의 `*PART_COMPOSITE` 형식으로 변환 필요

### 10.3 요소 ID 제약

- LS-DYNA는 3절점 쉘과 4절점 쉘이 동일한 ID를 가질 수 없음
- OpenRadioss는 허용하므로, DYNAIN 출력 시 에러 발생 가능
- `check_dynain.F`에서 사전 검사 수행

### 10.4 초기 조건 제약

DYNAIN에 출력되는 초기 조건:
- ✅ 속도 (INITIAL_VELOCITY_NODE)
- ✅ 응력 (INITIAL_STRESS_SHELL)
- ✅ 변형률 (INITIAL_STRAIN_SHELL)
- ❌ 온도 (미지원)
- ❌ 플라스틱 변형률 (부분 지원)

---

## 11. 향후 개선 사항

### 11.1 계획된 기능

- [ ] 솔리드 요소 DYNAIN 출력 지원
- [ ] 빔 요소 DYNAIN 출력 지원
- [ ] 온도 필드 출력
- [ ] 압축 DYNAIN 파일 지원 개선
- [ ] 이진(binary) DYNAIN 형식 지원

### 11.2 성능 개선

- [ ] 병렬 I/O 최적화
- [ ] 대규모 모델용 청크(chunk) 기반 출력
- [ ] 선택적 데이터 출력 (응력만, 변형률만 등)

---

## 12. 참고 자료

### 12.1 소스 코드 위치

```
OpenRadioss/
├── starter/source/starter/
│   ├── check_dynain.F              # DYNAIN 출력 검증
│   └── lectur.F                    # 메인 읽기 루틴
├── engine/source/output/dynain/
│   ├── gendynain.F                 # DYNAIN 생성 메인
│   ├── dynain_node.F               # 노드 출력
│   ├── dynain_shel_mp.F            # 쉘 출력 (단일)
│   ├── dynain_shel_spmd.F          # 쉘 출력 (병렬)
│   ├── dynain_c_strsg.F            # 응력/변형률 (기하)
│   ├── dynain_c_strag.F            # 응력/변형률 (대수)
│   └── dynain_size.F               # 버퍼 크기 계산
└── hm_cfg_files/config/CFG/
    └── Keyword971*/                # LS-DYNA CFG 파일
```

### 12.2 관련 문서

- LS-DYNA Keyword User's Manual
- OpenRadioss User Manual
- OpenRadioss Theory Manual

### 12.3 테스트 케이스

```
OpenRadioss/qa-tests/
├── dynain/
│   ├── test_dynain_shell3/       # 3절점 쉘 테스트
│   ├── test_dynain_shell4/       # 4절점 쉘 테스트
│   └── test_dynain_composite/    # 복합재 테스트
```

---

## 요약

OpenRadioss의 DYNAIN 기능은 다음을 가능하게 합니다:

1. **LS-DYNA 입력 파일 읽기**
   - CFG 파일을 통한 키워드 변환
   - 100개 이상의 LS-DYNA 키워드 지원
   - 다양한 LS-DYNA 버전 호환 (R6.1 ~ R14.1)

2. **LS-DYNA 형식 출력**
   - DYNAIN 파일 생성 (재시작/후처리용)
   - 노드, 요소, 응력, 변형률 등 전체 상태 저장
   - 병렬 실행 환경 지원

3. **상호운용성**
   - OpenRadioss ↔ LS-DYNA 간 데이터 교환
   - 하이브리드 워크플로우 가능
   - 기존 LS-DYNA 모델 재사용

이를 통해 사용자는 두 솔버의 장점을 모두 활용할 수 있습니다.
