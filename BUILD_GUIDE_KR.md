# KooSimulationDataReader 빌드 가이드

## 🎯 모듈별 독립 빌드 방법

이 프로젝트는 **DYNA 키워드 파싱**과 **ECAD/ODB++** 모듈을 독립적으로 빌드하고 테스트할 수 있습니다.

---

## 📦 빌드 옵션

| 옵션 | 기본값 | 설명 |
|-----|-------|------|
| `BUILD_DYNA_MODULE` | ON | LS-DYNA 키워드 파싱 모듈 |
| `BUILD_ECAD_MODULE` | ON | ECAD/ODB++ 모듈 |
| `BUILD_SHARED_LIBS` | ON | 공유 라이브러리 빌드 |
| `BUILD_STATIC_LIBS` | ON | 정적 라이브러리 빌드 |
| `BUILD_CLI` | ON | CLI 실행 파일 |
| `BUILD_TESTS` | ON | 테스트 빌드 |

---

## 🔧 DYNA 키워드만 빌드 (추천)

ECAD를 제외하고 LS-DYNA 키워드 파싱만 빌드하고 디버깅할 때:

```bash
# 빌드 디렉토리 생성
mkdir build-dyna-only
cd build-dyna-only

# CMake 구성 (ECAD 제외)
cmake .. \
  -DBUILD_DYNA_MODULE=ON \
  -DBUILD_ECAD_MODULE=OFF \
  -DCMAKE_BUILD_TYPE=Debug

# 빌드
cmake --build . -j$(nproc)

# DYNA 테스트만 실행
./bin/koo_dyna_tests

# 특정 테스트만 실행
./bin/koo_dyna_tests --gtest_filter=KeywordFileReaderTest.*
```

### 생성되는 파일:
- `lib/libkoo_dyna.a` - DYNA 전용 정적 라이브러리
- `bin/koo_dyna_tests` - DYNA 테스트 실행 파일

---

## 🖥️ ECAD만 빌드

ECAD/ODB++ 모듈만 빌드할 때:

```bash
mkdir build-ecad-only
cd build-ecad-only

cmake .. \
  -DBUILD_DYNA_MODULE=OFF \
  -DBUILD_ECAD_MODULE=ON \
  -DCMAKE_BUILD_TYPE=Debug

cmake --build . -j$(nproc)

# ECAD 테스트 실행
./bin/koo_ecad_tests
```

### 생성되는 파일:
- `lib/libkoo_ecad.a` - ECAD 전용 정적 라이브러리
- `bin/koo_ecad_tests` - ECAD 테스트 실행 파일

---

## 🌐 전체 통합 빌드

모든 모듈을 포함한 전체 빌드:

```bash
mkdir build
cd build

cmake .. \
  -DBUILD_DYNA_MODULE=ON \
  -DBUILD_ECAD_MODULE=ON \
  -DCMAKE_BUILD_TYPE=Release

cmake --build . -j$(nproc)

# 통합 테스트 실행
./bin/koo_sim_tests

# 개별 모듈 테스트도 가능
./bin/koo_dyna_tests
./bin/koo_ecad_tests
```

### 생성되는 파일:
- `lib/libkoo_sim.so` - 통합 공유 라이브러리
- `lib/libkoo_sim_static.a` - 통합 정적 라이브러리
- `bin/koo_sim_cli` - CLI 실행 파일
- `bin/koo_sim_tests` - 전체 테스트

---

## 🐛 VSCode 디버깅 설정

`.vscode/launch.json`과 `.vscode/tasks.json`이 자동으로 생성되었습니다.

### 디버깅 실행 방법:

1. **VSCode에서 F5 누르기**
2. 디버그 구성 선택:
   - `(gdb) DYNA Tests 디버그` - DYNA 키워드 테스트
   - `(gdb) ECAD Tests 디버그` - ECAD 테스트
   - `(gdb) DYNA 특정 테스트 디버그` - 특정 테스트만

### 빌드 태스크 실행:

- `Ctrl+Shift+B` → `Build DYNA Only` 선택
- `Ctrl+Shift+P` → `Tasks: Run Task` → 원하는 빌드 선택

---

## 📊 구현된 LS-DYNA 키워드 (510개)

### 주요 카테고리:

| 카테고리 | 개수 | 파일 |
|---------|------|------|
| **재료 (MATERIAL)** | 55+ | [Material.hpp](include/koo/dyna/Material.hpp) |
| **제어 (CONTROL)** | 35 | [Control.hpp](include/koo/dyna/Control.hpp) |
| **데이터베이스 (DATABASE)** | 32 | [Database.hpp](include/koo/dyna/Database.hpp) |
| **접촉 (CONTACT)** | 25 | [Contact.hpp](include/koo/dyna/Contact.hpp) |
| **정의 (DEFINE)** | 24 | [Define.hpp](include/koo/dyna/Define.hpp) |
| **하중 (LOAD)** | 21 | [Load.hpp](include/koo/dyna/Load.hpp) |
| **구속 (CONSTRAINED)** | 20 | [Constrained.hpp](include/koo/dyna/Constrained.hpp) |
| **경계조건 (BOUNDARY)** | 19 | [Boundary.hpp](include/koo/dyna/Boundary.hpp) |
| **집합 (SET)** | 18 | [Set.hpp](include/koo/dyna/Set.hpp) |
| **단면 (SECTION)** | 16+ | [Section.hpp](include/koo/dyna/Section.hpp) |
| **초기조건 (INITIAL)** | 15 | [Initial.hpp](include/koo/dyna/Initial.hpp) |
| **나머지** | 250+ | 기타 특수 모듈 |

**전체 목록:** [KEYWORD_PROGRESS.md](docs/KEYWORD_PROGRESS.md)

---

## 🧪 테스트 구조

### DYNA 테스트 파일 (`tests/unit/`):
- `TestCardParser.cpp` - 카드 파싱 (고정폭 필드)
- `TestKeywordFileReader.cpp` - K-파일 읽기
- `TestKeywordFileWriter.cpp` - K-파일 쓰기
- `TestNode.cpp` - 노드 데이터
- `TestElement.cpp` - 요소 데이터
- `TestPart.cpp` - 파트 관리
- `TestMaterial.cpp` - 재료 속성
- `TestSection.cpp` - 단면 속성
- `TestModel.cpp` - 모델 컨테이너

### ECAD 테스트 파일:
- `TestOdbJob.cpp` - ODB++ 작업
- `TestOdbWriter.cpp` - ODB++ 쓰기
- `TestFeature.cpp` - 피처 데이터
- `TestSymbol.cpp` - 심볼 정의
- `TestLayer.cpp` - 레이어 관리
- `TestEdaData.cpp` - EDA 넷리스트

---

## 📝 예제 K-파일 테스트

```cpp
#include <koo/dyna/KeywordFileReader.hpp>
#include <koo/dyna/Model.hpp>

int main() {
    using namespace koo::dyna;

    // K-파일 읽기
    KeywordFileReader reader;
    auto model = reader.read("example.k");

    // 노드 접근
    auto* nodes = model->getKeywordOfType<Node>();
    std::cout << "노드 개수: " << nodes->nodes.size() << "\n";

    // 재료 찾기
    auto* mat = model->findMaterial(1);
    if (mat) {
        std::cout << "재료 1: " << mat->getKeywordName() << "\n";
    }

    // K-파일로 다시 쓰기
    KeywordFileWriter writer;
    writer.write(*model, "output.k");

    return 0;
}
```

---

## 🚀 릴리스 빌드

```bash
mkdir build-release
cd build-release

cmake .. \
  -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_DYNA_MODULE=ON \
  -DBUILD_ECAD_MODULE=OFF \
  -DBUILD_TESTS=OFF

cmake --build . -j$(nproc)
sudo cmake --install .
```

---

## 📂 프로젝트 구조

```
KooSimulationDataReader/
├── include/koo/dyna/          # LS-DYNA 퍼블릭 API (42개 헤더)
├── src/dyna/                  # LS-DYNA 구현 (41개 .cpp)
├── include/koo/ecad/          # ECAD 퍼블릭 API (9개 헤더)
├── src/ecad/                  # ECAD 구현 (8개 .cpp)
├── include/koo/util/          # 유틸리티 (3개 헤더)
├── src/util/                  # 유틸리티 구현
├── tests/unit/                # 단위 테스트 (16개)
├── tests/integration/         # 통합 테스트
├── cli/                       # CLI 도구
├── docs/                      # 문서
│   ├── KEYWORD_PROGRESS.md   # 510개 키워드 진행상황
│   ├── ECAD_STRATEGY.md      # ECAD 아키텍처
│   └── MCAD_STRATEGY.md      # MCAD 로드맵
└── ROADMAP.md                 # 장기 개발 계획
```

---

## 🔍 디버깅 팁

### GDB로 직접 디버깅:
```bash
cd build-dyna-only
gdb ./bin/koo_dyna_tests

(gdb) break KeywordFileReader::read
(gdb) run
(gdb) next
(gdb) print model->keywords_.size()
```

### 특정 테스트 케이스만 실행:
```bash
# 하나의 테스트만
./bin/koo_dyna_tests --gtest_filter=KeywordFileReaderTest.ParseSimpleFile

# 패턴 매칭
./bin/koo_dyna_tests --gtest_filter=*Material*

# 테스트 목록 확인
./bin/koo_dyna_tests --gtest_list_tests
```

### Valgrind 메모리 체크:
```bash
valgrind --leak-check=full ./bin/koo_dyna_tests
```

---

## 📧 문의

- 이슈: GitHub Issues
- 문서: [docs/](docs/) 디렉토리 참조

---

## 📜 라이선스

TBD (프로젝트에 맞게 수정 필요)
