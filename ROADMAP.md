# KooSimulationDataReader - Long-term Roadmap

## Project Vision

**완전 자동화된 CAE(Computer-Aided Engineering) 요소 생성 솔루션**

LS-DYNA 키워드 기반의 C++ 라이브러리로 시작하여, 다양한 FEA 솔버 포맷 지원, 메쉬 생성/변환, CAD 형상 연동까지 확장하는 크로스 플랫폼 시뮬레이션 데이터 처리 프레임워크.

---

## Development Rules (개발 규칙)

### 1. Cross-Platform Support (크로스 플랫폼 지원)
- **Windows, Linux, macOS** 모두 지원 필수
- 플랫폼 종속적 코드는 `#ifdef` 또는 CMake 조건부 컴파일로 분리
- 파일 경로는 `std::filesystem` 사용 (C++17)
- 줄바꿈 문자는 플랫폼 독립적으로 처리 (`\n`, `\r\n` 모두 지원)
- CI/CD에서 3개 플랫폼 모두 빌드/테스트 필수

### 2. Build Targets (빌드 타겟)

```
┌─────────────────────────────────────────────────────────────────────┐
│                        Build Outputs                                 │
├─────────────────────────────────────────────────────────────────────┤
│                                                                      │
│  ┌─────────────────────────────────────────────────────────────┐    │
│  │              Shared Library (DLL/SO/DYLIB)                  │    │
│  │  - koo_sim.dll (Windows)                                    │    │
│  │  - libkoo_sim.so (Linux)                                    │    │
│  │  - libkoo_sim.dylib (macOS)                                 │    │
│  │  → 다른 프로그램에서 링크하여 사용                           │    │
│  └─────────────────────────────────────────────────────────────┘    │
│                              │                                       │
│  ┌─────────────────────────────────────────────────────────────┐    │
│  │              Static Library (필수)                           │    │
│  │  - koo_sim_static.lib / libkoo_sim_static.a                 │    │
│  │  → 정적 링크가 필요한 경우 (임베디드, 단일 바이너리 등)       │    │
│  │  → Shared와 Static 모두 기본 빌드                           │    │
│  └─────────────────────────────────────────────────────────────┘    │
│                              │                                       │
│  ┌─────────────────────────────────────────────────────────────┐    │
│  │              CLI Executable                                  │    │
│  │  - koo_sim_cli (.exe on Windows)                            │    │
│  │  → 명령줄에서 직접 기능 실행                                 │    │
│  │  → 예: koo_sim_cli convert input.k -o output.rad            │    │
│  └─────────────────────────────────────────────────────────────┘    │
│                              │                                       │
│  ┌─────────────────────────────────────────────────────────────┐    │
│  │              Test Executable                                 │    │
│  │  - koo_sim_tests                                            │    │
│  │  → 별도 tests/ 폴더에서 빌드                                 │    │
│  │  → Google Test 기반                                         │    │
│  └─────────────────────────────────────────────────────────────┘    │
│                                                                      │
└─────────────────────────────────────────────────────────────────────┘
```

### 3. Directory Structure Rules (디렉토리 구조 규칙)

```
KooSimulationDataReader/
├── include/              # Public headers (외부에 노출되는 헤더)
│   └── koo/
├── src/                  # Implementation files
├── tests/                # ⚠️ 테스트 코드는 반드시 별도 폴더
│   ├── unit/            # 단위 테스트
│   ├── integration/     # 통합 테스트
│   └── test_data/       # 테스트용 샘플 파일
├── cli/                  # CLI 실행 파일 소스
├── examples/             # 사용 예제 코드
├── cmake/                # CMake 모듈
└── docs/                 # 문서
```

### 4. API Design Rules (API 설계 규칙)

#### 4.1 Export Macros (DLL 익스포트)
```cpp
// include/koo/Export.hpp
#pragma once

#if defined(_WIN32) || defined(_WIN64)
    #ifdef KOO_SIM_EXPORTS
        #define KOO_API __declspec(dllexport)
    #else
        #define KOO_API __declspec(dllimport)
    #endif
#else
    #define KOO_API __attribute__((visibility("default")))
#endif
```

#### 4.2 Public API Classes
```cpp
// 외부 노출 클래스는 반드시 KOO_API 매크로 사용
class KOO_API Model { ... };
class KOO_API KeywordFileReader { ... };
```

#### 4.3 C Interface (Optional - FFI용)
```cpp
// C 인터페이스로 다른 언어에서 호출 가능
extern "C" {
    KOO_API void* koo_model_create();
    KOO_API void koo_model_destroy(void* model);
    KOO_API int koo_model_read_file(void* model, const char* filepath);
    KOO_API int koo_model_write_file(void* model, const char* filepath);
}
```

### 5. Testing Rules (테스트 규칙)

- **모든 public 클래스**는 단위 테스트 필수
- 테스트 파일 명명: `Test<ClassName>.cpp` (예: `TestNode.cpp`)
- 테스트는 `tests/` 폴더에만 위치
- 새 키워드 추가 시 반드시 파싱/쓰기 테스트 포함
- CI에서 모든 테스트 통과 필수

```cpp
// tests/unit/TestNode.cpp
#include <gtest/gtest.h>
#include <koo/dyna/Node.hpp>

TEST(NodeTest, ParseFromCard) {
    // ...
}

TEST(NodeTest, WriteToCard) {
    // ...
}
```

### 6. Code Style Rules (코드 스타일)

- **C++17** 표준 사용
- 헤더 가드: `#pragma once`
- 네임스페이스: `koo::dyna`, `koo::algorithm`, `koo::geometry`, `koo::converter`
- 클래스명: PascalCase (`KeywordFileReader`)
- 함수명: camelCase (`readFile()`)
- 멤버 변수: trailing underscore (`id_`, `name_`)
- 상수: UPPER_SNAKE_CASE (`MAX_NODES`)
- 파일명: PascalCase (`KeywordFileReader.cpp`)

### 7. Documentation Rules (문서화 규칙)

- Public API는 Doxygen 스타일 주석 필수
- 복잡한 알고리즘은 상세 설명 포함
- 사용 예제 코드 포함

```cpp
/**
 * @brief K파일을 읽어서 Model 객체로 변환
 * @param filepath K파일 경로
 * @return 파싱된 Model 객체
 * @throws std::runtime_error 파일을 열 수 없거나 파싱 실패 시
 *
 * @example
 * KeywordFileReader reader;
 * Model model = reader.read("input.k");
 */
Model read(const std::string& filepath);
```

### 8. Version Control Rules (버전 관리)

- **Semantic Versioning**: MAJOR.MINOR.PATCH
- 브랜치 전략: `main`, `develop`, `feature/*`, `bugfix/*`
- 커밋 메시지: Conventional Commits 형식
  - `feat:` 새 기능
  - `fix:` 버그 수정
  - `docs:` 문서
  - `test:` 테스트
  - `refactor:` 리팩토링

---

## Design Patterns (디자인 패턴)

본 프로젝트는 유지보수성, 확장성, 테스트 용이성을 위해 다음 디자인 패턴들을 적용한다.

### Pattern Overview

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                         Design Pattern Architecture                          │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                              │
│  ┌─────────────────────────────────────────────────────────────────────┐    │
│  │                    Creational Patterns (생성)                        │    │
│  │  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐                  │    │
│  │  │   Factory   │  │  Prototype  │  │   Builder   │                  │    │
│  │  │   Method    │  │   (Clone)   │  │  (Complex)  │                  │    │
│  │  └─────────────┘  └─────────────┘  └─────────────┘                  │    │
│  └─────────────────────────────────────────────────────────────────────┘    │
│                                                                              │
│  ┌─────────────────────────────────────────────────────────────────────┐    │
│  │                    Structural Patterns (구조)                        │    │
│  │  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐                  │    │
│  │  │  Composite  │  │   Facade    │  │   Adapter   │                  │    │
│  │  │   (Tree)    │  │ (Simple API)│  │ (Converter) │                  │    │
│  │  └─────────────┘  └─────────────┘  └─────────────┘                  │    │
│  └─────────────────────────────────────────────────────────────────────┘    │
│                                                                              │
│  ┌─────────────────────────────────────────────────────────────────────┐    │
│  │                    Behavioral Patterns (행위)                        │    │
│  │  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐  ┌───────────┐  │    │
│  │  │  Strategy   │  │  Visitor    │  │  Command    │  │ Observer  │  │    │
│  │  │ (Algorithm) │  │ (Traverse)  │  │   (Undo)    │  │  (Event)  │  │    │
│  │  └─────────────┘  └─────────────┘  └─────────────┘  └───────────┘  │    │
│  └─────────────────────────────────────────────────────────────────────┘    │
│                                                                              │
└─────────────────────────────────────────────────────────────────────────────┘
```

---

### 1. Factory Method Pattern (팩토리 메서드)

**용도**: 1500+ 키워드 클래스의 동적 생성

```cpp
namespace koo::dyna {

// 키워드 생성 함수 타입
using KeywordCreator = std::function<std::unique_ptr<Keyword>()>;

/**
 * @brief 키워드 팩토리 - 문자열 이름으로 키워드 객체 생성
 *
 * 싱글톤 패턴과 결합하여 전역 레지스트리 역할
 * 새 키워드 추가 시 팩토리만 수정하면 됨 (OCP 준수)
 */
class KOO_API KeywordFactory {
public:
    static KeywordFactory& instance();

    // 키워드 등록 (컴파일 타임 자동 등록)
    bool registerKeyword(const std::string& name, KeywordCreator creator);

    // 키워드 생성
    std::unique_ptr<Keyword> create(const std::string& name) const;

    // 등록된 키워드 목록
    std::vector<std::string> getRegisteredKeywords() const;

private:
    KeywordFactory() = default;
    std::unordered_map<std::string, KeywordCreator> creators_;
};

// 자동 등록 매크로 - 각 키워드 클래스 파일에서 사용
#define REGISTER_KEYWORD(ClassName, KeywordName)                    \
    namespace {                                                      \
        static const bool _reg_##ClassName = []() {                  \
            KeywordFactory::instance().registerKeyword(              \
                KeywordName,                                         \
                []() { return std::make_unique<ClassName>(); }       \
            );                                                       \
            return true;                                             \
        }();                                                         \
    }

} // namespace koo::dyna

// 사용 예시 (MatElastic.cpp)
REGISTER_KEYWORD(MatElastic, "*MAT_ELASTIC")
REGISTER_KEYWORD(MatElastic, "*MAT_001")  // 별칭도 등록 가능
```

---

### 2. Prototype Pattern (프로토타입)

**용도**: 키워드 객체 복제 (deep copy)

```cpp
namespace koo::dyna {

class Keyword {
public:
    virtual ~Keyword() = default;

    /**
     * @brief 객체 복제 (Prototype Pattern)
     *
     * 모든 키워드는 자신을 복제할 수 있어야 함
     * 모델 복사, Undo/Redo 구현에 필수
     */
    virtual std::unique_ptr<Keyword> clone() const = 0;

    // ... other methods
};

// CRTP를 사용한 clone() 자동 구현
template<typename Derived, typename Base = Keyword>
class CloneableKeyword : public Base {
public:
    std::unique_ptr<Keyword> clone() const override {
        return std::make_unique<Derived>(static_cast<const Derived&>(*this));
    }
};

// 사용 예시
class MatElastic : public CloneableKeyword<MatElastic, Material> {
    // clone()이 자동으로 구현됨
};

} // namespace koo::dyna
```

---

### 3. Builder Pattern (빌더)

**용도**: 복잡한 객체(Model, Contact 등) 단계별 생성

```cpp
namespace koo::dyna {

/**
 * @brief 모델 빌더 - 복잡한 Model 객체를 단계별로 구성
 *
 * Fluent Interface 스타일로 메서드 체이닝 지원
 */
class KOO_API ModelBuilder {
public:
    ModelBuilder& setTitle(const std::string& title);
    ModelBuilder& setUnits(Units units);

    // 노드/요소 추가
    ModelBuilder& addNode(int id, double x, double y, double z);
    ModelBuilder& addShellElement(int id, int pid, std::initializer_list<int> nodes);
    ModelBuilder& addSolidElement(int id, int pid, std::initializer_list<int> nodes);

    // 파트/재료/섹션 추가
    ModelBuilder& addPart(int id, int secid, int mid, const std::string& title = "");
    ModelBuilder& addMaterial(std::unique_ptr<Material> mat);
    ModelBuilder& addSection(std::unique_ptr<Section> sec);

    // 컨트롤 카드
    ModelBuilder& setTerminationTime(double endtime);
    ModelBuilder& setTimestep(double dt);

    // 최종 빌드
    Model build();

private:
    Model model_;
};

// 사용 예시
Model model = ModelBuilder()
    .setTitle("Crash Analysis")
    .setTerminationTime(0.1)
    .addNode(1, 0.0, 0.0, 0.0)
    .addNode(2, 1.0, 0.0, 0.0)
    .addNode(3, 1.0, 1.0, 0.0)
    .addNode(4, 0.0, 1.0, 0.0)
    .addShellElement(1, 1, {1, 2, 3, 4})
    .addPart(1, 1, 1, "Shell Part")
    .build();

} // namespace koo::dyna
```

---

### 4. Composite Pattern (컴포지트)

**용도**: Include 파일 트리 구조, Set 계층 구조

```cpp
namespace koo::dyna {

/**
 * @brief Include 파일 트리 구조
 *
 * K파일은 *INCLUDE로 다른 파일을 포함할 수 있음
 * Composite 패턴으로 트리 구조 표현
 */
class KOO_API IncludeNode {
public:
    virtual ~IncludeNode() = default;
    virtual void accept(ModelVisitor& visitor) = 0;
    virtual std::vector<std::unique_ptr<Keyword>>& getKeywords() = 0;
};

// Leaf: 실제 키워드들을 담은 파일
class KOO_API KeywordFile : public IncludeNode {
    std::filesystem::path filepath_;
    std::vector<std::unique_ptr<Keyword>> keywords_;
public:
    void accept(ModelVisitor& visitor) override;
    std::vector<std::unique_ptr<Keyword>>& getKeywords() override;
};

// Composite: Include로 다른 파일들을 포함
class KOO_API IncludeFile : public IncludeNode {
    std::filesystem::path filepath_;
    std::vector<std::unique_ptr<IncludeNode>> children_;
public:
    void addChild(std::unique_ptr<IncludeNode> child);
    void accept(ModelVisitor& visitor) override;
    std::vector<std::unique_ptr<Keyword>>& getKeywords() override;
};

/**
 * @brief Set 계층 구조 (SET_NODE_ADD 등)
 */
class KOO_API SetComposite : public Set {
    std::vector<std::shared_ptr<Set>> children_;
public:
    void addSet(std::shared_ptr<Set> set);
    std::vector<int> getAllIds() const override;  // 모든 하위 Set의 ID 합침
};

} // namespace koo::dyna
```

---

### 5. Facade Pattern (파사드)

**용도**: 복잡한 내부 구조를 단순한 API로 제공

```cpp
namespace koo {

/**
 * @brief 라이브러리 진입점 - 복잡한 내부를 숨기고 단순한 API 제공
 *
 * 사용자는 이 클래스만 알면 대부분의 작업 가능
 */
class KOO_API KooSim {
public:
    // 파일 I/O (내부적으로 Reader/Writer/Factory 사용)
    static dyna::Model readFile(const std::string& filepath);
    static void writeFile(const dyna::Model& model, const std::string& filepath);

    // 포맷 변환 (내부적으로 Converter 사용)
    static void convert(const std::string& input, const std::string& output,
                       Format inputFormat = Format::Auto,
                       Format outputFormat = Format::Auto);

    // 모델 검증 (내부적으로 Validator 사용)
    static ValidationResult validate(const dyna::Model& model);

    // 유틸리티
    static ModelInfo getInfo(const std::string& filepath);
    static std::vector<std::string> getSupportedFormats();
};

// 사용 예시 - 매우 단순
auto model = KooSim::readFile("input.k");
KooSim::writeFile(model, "output.k");
KooSim::convert("model.k", "model.rad");

} // namespace koo
```

---

### 6. Strategy Pattern (전략)

**용도**: 알고리즘 교체 가능 (메쉬 스무딩, 접촉 검색 등)

```cpp
namespace koo::algorithm {

/**
 * @brief 메쉬 스무딩 전략 인터페이스
 */
class KOO_API SmoothingStrategy {
public:
    virtual ~SmoothingStrategy() = default;
    virtual void smooth(dyna::Model& model, int iterations) = 0;
    virtual std::string getName() const = 0;
};

// 구체적인 전략들
class LaplacianSmoothing : public SmoothingStrategy {
public:
    void smooth(dyna::Model& model, int iterations) override;
    std::string getName() const override { return "Laplacian"; }
};

class TaubinSmoothing : public SmoothingStrategy {
public:
    void smooth(dyna::Model& model, int iterations) override;
    std::string getName() const override { return "Taubin"; }
};

class OptimizationSmoothing : public SmoothingStrategy {
public:
    void smooth(dyna::Model& model, int iterations) override;
    std::string getName() const override { return "Optimization-based"; }
};

/**
 * @brief 메쉬 스무더 - 전략 패턴 컨텍스트
 */
class KOO_API MeshSmoother {
public:
    void setStrategy(std::unique_ptr<SmoothingStrategy> strategy);
    void smooth(dyna::Model& model, int iterations = 5);

private:
    std::unique_ptr<SmoothingStrategy> strategy_;
};

// 사용 예시
MeshSmoother smoother;
smoother.setStrategy(std::make_unique<TaubinSmoothing>());
smoother.smooth(model, 10);

} // namespace koo::algorithm
```

---

### 7. Visitor Pattern (방문자)

**용도**: 키워드/요소 순회 및 처리 (통계, 변환, 검증 등)

```cpp
namespace koo::dyna {

// Forward declarations
class Node;
class ElementShell;
class ElementSolid;
class Material;
class Part;
// ... more keyword types

/**
 * @brief 모델 방문자 인터페이스
 *
 * 새로운 연산 추가 시 Visitor만 구현하면 됨 (OCP 준수)
 * 기존 키워드 클래스 수정 불필요
 */
class KOO_API ModelVisitor {
public:
    virtual ~ModelVisitor() = default;

    virtual void visit(Node& node) = 0;
    virtual void visit(ElementShell& elem) = 0;
    virtual void visit(ElementSolid& elem) = 0;
    virtual void visit(Material& mat) = 0;
    virtual void visit(Part& part) = 0;
    // ... more visit methods
};

// 키워드 클래스에 accept 메서드 추가
class Keyword {
public:
    virtual void accept(ModelVisitor& visitor) = 0;
};

/**
 * @brief 모델 통계 수집 Visitor
 */
class StatisticsVisitor : public ModelVisitor {
public:
    void visit(Node& node) override { nodeCount_++; }
    void visit(ElementShell& elem) override { shellCount_++; }
    void visit(ElementSolid& elem) override { solidCount_++; }
    void visit(Material& mat) override { materialCount_++; }
    void visit(Part& part) override { partCount_++; }

    void printStatistics() const;

private:
    int nodeCount_ = 0;
    int shellCount_ = 0;
    int solidCount_ = 0;
    int materialCount_ = 0;
    int partCount_ = 0;
};

/**
 * @brief 모델 검증 Visitor
 */
class ValidationVisitor : public ModelVisitor {
public:
    void visit(Node& node) override;      // 좌표 유효성 검사
    void visit(ElementShell& elem) override;  // 요소 품질 검사
    void visit(Material& mat) override;   // 재료 속성 검사

    ValidationResult getResult() const;

private:
    std::vector<ValidationError> errors_;
};

/**
 * @brief 좌표 변환 Visitor
 */
class TransformVisitor : public ModelVisitor {
public:
    TransformVisitor(const Transform& transform);
    void visit(Node& node) override;  // 노드 좌표 변환
    // 요소/재료 등은 변환 불필요 - 기본 구현 사용

private:
    Transform transform_;
};

// 사용 예시
StatisticsVisitor stats;
model.accept(stats);
stats.printStatistics();

} // namespace koo::dyna
```

---

### 8. Command Pattern (커맨드)

**용도**: Undo/Redo 지원, 작업 기록

```cpp
namespace koo {

/**
 * @brief 실행 취소 가능한 명령 인터페이스
 */
class KOO_API Command {
public:
    virtual ~Command() = default;
    virtual void execute() = 0;
    virtual void undo() = 0;
    virtual std::string getDescription() const = 0;
};

/**
 * @brief 노드 이동 명령
 */
class MoveNodeCommand : public Command {
public:
    MoveNodeCommand(dyna::Model& model, int nodeId,
                   const Vec3& oldPos, const Vec3& newPos);

    void execute() override;
    void undo() override;
    std::string getDescription() const override;

private:
    dyna::Model& model_;
    int nodeId_;
    Vec3 oldPos_;
    Vec3 newPos_;
};

/**
 * @brief 요소 삭제 명령
 */
class DeleteElementCommand : public Command {
public:
    DeleteElementCommand(dyna::Model& model, int elemId);

    void execute() override;  // 요소 저장 후 삭제
    void undo() override;     // 저장된 요소 복원

private:
    dyna::Model& model_;
    int elemId_;
    std::unique_ptr<dyna::Element> savedElement_;
};

/**
 * @brief 명령 관리자 (Undo/Redo 스택)
 */
class KOO_API CommandManager {
public:
    void execute(std::unique_ptr<Command> cmd);
    void undo();
    void redo();

    bool canUndo() const;
    bool canRedo() const;

    std::vector<std::string> getUndoHistory() const;
    std::vector<std::string> getRedoHistory() const;

private:
    std::stack<std::unique_ptr<Command>> undoStack_;
    std::stack<std::unique_ptr<Command>> redoStack_;
};

} // namespace koo
```

---

### 9. Observer Pattern (관찰자)

**용도**: 모델 변경 시 자동 업데이트 (UI 연동, 자동 저장 등)

```cpp
namespace koo {

/**
 * @brief 모델 변경 이벤트
 */
struct ModelEvent {
    enum class Type {
        NodeAdded, NodeRemoved, NodeModified,
        ElementAdded, ElementRemoved, ElementModified,
        PartAdded, PartRemoved, PartModified,
        MaterialAdded, MaterialRemoved, MaterialModified,
        // ... more event types
    };

    Type type;
    int entityId;
    std::string details;
};

/**
 * @brief 모델 변경 관찰자 인터페이스
 */
class KOO_API ModelObserver {
public:
    virtual ~ModelObserver() = default;
    virtual void onModelChanged(const ModelEvent& event) = 0;
};

/**
 * @brief 관찰 가능한 모델
 */
class KOO_API ObservableModel : public dyna::Model {
public:
    void addObserver(std::shared_ptr<ModelObserver> observer);
    void removeObserver(std::shared_ptr<ModelObserver> observer);

protected:
    void notifyObservers(const ModelEvent& event);

private:
    std::vector<std::weak_ptr<ModelObserver>> observers_;
};

// 사용 예시: 자동 저장 관찰자
class AutoSaveObserver : public ModelObserver {
public:
    AutoSaveObserver(const std::string& backupPath);
    void onModelChanged(const ModelEvent& event) override;
};

// 사용 예시: UI 업데이트 관찰자
class UIUpdateObserver : public ModelObserver {
public:
    void onModelChanged(const ModelEvent& event) override;
};

} // namespace koo
```

---

### 10. Adapter Pattern (어댑터)

**용도**: 외부 라이브러리/포맷 연동

```cpp
namespace koo::converter {

/**
 * @brief 외부 포맷 어댑터 인터페이스
 *
 * 다양한 FEA 포맷을 내부 Model로 변환
 */
class KOO_API FormatAdapter {
public:
    virtual ~FormatAdapter() = default;
    virtual dyna::Model import(const std::string& filepath) = 0;
    virtual void export_(const dyna::Model& model, const std::string& filepath) = 0;
    virtual std::string getFormatName() const = 0;
    virtual std::vector<std::string> getExtensions() const = 0;
};

// OpenRadioss 어댑터
class OpenRadiossAdapter : public FormatAdapter {
public:
    dyna::Model import(const std::string& filepath) override;
    void export_(const dyna::Model& model, const std::string& filepath) override;
    std::string getFormatName() const override { return "OpenRadioss"; }
    std::vector<std::string> getExtensions() const override { return {".rad", "_0000.rad"}; }
};

// Nastran 어댑터
class NastranAdapter : public FormatAdapter {
public:
    dyna::Model import(const std::string& filepath) override;
    void export_(const dyna::Model& model, const std::string& filepath) override;
    std::string getFormatName() const override { return "Nastran"; }
    std::vector<std::string> getExtensions() const override { return {".bdf", ".nas", ".dat"}; }
};

// GMSH 어댑터 (외부 GMSH SDK 래핑)
class GmshAdapter : public FormatAdapter {
public:
    dyna::Model import(const std::string& filepath) override;
    void export_(const dyna::Model& model, const std::string& filepath) override;
    std::string getFormatName() const override { return "GMSH"; }
    std::vector<std::string> getExtensions() const override { return {".msh", ".geo"}; }
};

/**
 * @brief 포맷 어댑터 레지스트리
 */
class KOO_API FormatRegistry {
public:
    static FormatRegistry& instance();

    void registerAdapter(std::unique_ptr<FormatAdapter> adapter);
    FormatAdapter* getAdapterByExtension(const std::string& ext) const;
    FormatAdapter* getAdapterByName(const std::string& name) const;
    std::vector<std::string> getSupportedFormats() const;

private:
    std::vector<std::unique_ptr<FormatAdapter>> adapters_;
};

} // namespace koo::converter
```

---

### Pattern Usage Summary (패턴 사용 요약)

| Pattern | 적용 위치 | 목적 |
|---------|----------|------|
| **Factory Method** | `KeywordFactory` | 1500+ 키워드 동적 생성 |
| **Prototype** | `Keyword::clone()` | 객체 복제, Undo/Redo |
| **Builder** | `ModelBuilder` | 복잡한 모델 구성 |
| **Composite** | `IncludeNode`, `SetComposite` | 트리 구조 표현 |
| **Facade** | `KooSim` | 단순화된 API 제공 |
| **Strategy** | `SmoothingStrategy` 등 | 알고리즘 교체 |
| **Visitor** | `ModelVisitor` | 순회/처리 연산 추가 |
| **Command** | `CommandManager` | Undo/Redo 지원 |
| **Observer** | `ObservableModel` | 변경 알림 (UI 연동) |
| **Adapter** | `FormatAdapter` | 외부 포맷 연동 |

### SOLID 원칙 준수

| 원칙 | 적용 |
|------|------|
| **S** - Single Responsibility | 각 클래스는 하나의 책임만 (Node는 노드 데이터만, Parser는 파싱만) |
| **O** - Open/Closed | Factory, Visitor로 수정 없이 확장 가능 |
| **L** - Liskov Substitution | 모든 Element 서브클래스는 Element로 대체 가능 |
| **I** - Interface Segregation | 작은 인터페이스 분리 (Parseable, Writable, Clonable) |
| **D** - Dependency Inversion | 추상 인터페이스에 의존 (Strategy, Adapter) |

---

## 병렬화 전략 (Parallelization Strategy)

성능이 중요한 대용량 모델 처리를 위해 병렬화를 적극 활용합니다.

### 병렬화 적용 영역

| 영역 | 병렬화 방식 | 설명 |
|------|------------|------|
| **파일 읽기** | Thread Pool | 여러 Include 파일 동시 파싱 |
| **키워드 파싱** | Data Parallelism | 독립적인 키워드 블록 병렬 처리 |
| **노드/요소 처리** | SIMD + 멀티스레드 | 대량 데이터 벡터 연산 |
| **파일 쓰기** | 비동기 I/O | 버퍼링 + 백그라운드 쓰기 |
| **검증/계산** | `std::execution::par` | STL 병렬 알고리즘 활용 |

### Thread Pool 구현

```cpp
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>

class KOO_API ThreadPool {
public:
    explicit ThreadPool(size_t numThreads = std::thread::hardware_concurrency())
        : stop_(false) {
        for (size_t i = 0; i < numThreads; ++i) {
            workers_.emplace_back([this] { workerLoop(); });
        }
    }

    ~ThreadPool() {
        {
            std::unique_lock<std::mutex> lock(mutex_);
            stop_ = true;
        }
        condition_.notify_all();
        for (auto& worker : workers_) {
            worker.join();
        }
    }

    // 작업 제출 (반환값 있음)
    template<typename F, typename... Args>
    auto submit(F&& f, Args&&... args)
        -> std::future<std::invoke_result_t<F, Args...>> {
        using ReturnType = std::invoke_result_t<F, Args...>;

        auto task = std::make_shared<std::packaged_task<ReturnType()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );

        std::future<ReturnType> result = task->get_future();
        {
            std::unique_lock<std::mutex> lock(mutex_);
            tasks_.emplace([task]() { (*task)(); });
        }
        condition_.notify_one();
        return result;
    }

    // 모든 작업 완료 대기
    void waitAll() {
        std::unique_lock<std::mutex> lock(mutex_);
        completed_.wait(lock, [this] {
            return tasks_.empty() && activeWorkers_ == 0;
        });
    }

private:
    void workerLoop() {
        while (true) {
            std::function<void()> task;
            {
                std::unique_lock<std::mutex> lock(mutex_);
                condition_.wait(lock, [this] {
                    return stop_ || !tasks_.empty();
                });
                if (stop_ && tasks_.empty()) return;
                task = std::move(tasks_.front());
                tasks_.pop();
                ++activeWorkers_;
            }
            task();
            {
                std::unique_lock<std::mutex> lock(mutex_);
                --activeWorkers_;
                if (tasks_.empty() && activeWorkers_ == 0) {
                    completed_.notify_all();
                }
            }
        }
    }

    std::vector<std::thread> workers_;
    std::queue<std::function<void()>> tasks_;
    std::mutex mutex_;
    std::condition_variable condition_;
    std::condition_variable completed_;
    size_t activeWorkers_ = 0;
    bool stop_;
};
```

### 병렬 파일 파싱

```cpp
class KOO_API ParallelParser {
public:
    explicit ParallelParser(size_t numThreads = 0)
        : pool_(numThreads == 0 ? std::thread::hardware_concurrency() : numThreads) {}

    // 여러 Include 파일 병렬 파싱
    std::vector<std::unique_ptr<Model>> parseIncludeFiles(
        const std::vector<std::filesystem::path>& files) {

        std::vector<std::future<std::unique_ptr<Model>>> futures;
        futures.reserve(files.size());

        for (const auto& file : files) {
            futures.push_back(pool_.submit([this, &file]() {
                return parseFile(file);
            }));
        }

        std::vector<std::unique_ptr<Model>> results;
        results.reserve(files.size());
        for (auto& future : futures) {
            results.push_back(future.get());
        }
        return results;
    }

    // 단일 파일 내 키워드 블록 병렬 파싱
    std::unique_ptr<Model> parseFileParallel(const std::filesystem::path& file) {
        auto content = readFileContent(file);
        auto blocks = splitIntoKeywordBlocks(content);

        std::vector<std::future<std::unique_ptr<Keyword>>> futures;
        for (const auto& block : blocks) {
            futures.push_back(pool_.submit([&block]() {
                return KeywordFactory::instance().create(block);
            }));
        }

        auto model = std::make_unique<Model>();
        for (auto& future : futures) {
            model->addKeyword(future.get());
        }
        return model;
    }

private:
    ThreadPool pool_;

    std::unique_ptr<Model> parseFile(const std::filesystem::path& file);
    std::string readFileContent(const std::filesystem::path& file);
    std::vector<std::string_view> splitIntoKeywordBlocks(const std::string& content);
};
```

### STL 병렬 알고리즘 활용

```cpp
#include <execution>
#include <algorithm>
#include <numeric>

class KOO_API ParallelOperations {
public:
    // 노드 좌표 변환 (병렬)
    static void transformNodes(std::vector<Node>& nodes,
                               const Matrix4x4& transform) {
        std::for_each(std::execution::par_unseq,
                      nodes.begin(), nodes.end(),
                      [&transform](Node& node) {
                          node.position = transform * node.position;
                      });
    }

    // 요소 품질 계산 (병렬)
    static std::vector<double> calculateElementQualities(
        const std::vector<Element>& elements,
        const std::vector<Node>& nodes) {

        std::vector<double> qualities(elements.size());
        std::transform(std::execution::par,
                       elements.begin(), elements.end(),
                       qualities.begin(),
                       [&nodes](const Element& elem) {
                           return elem.calculateQuality(nodes);
                       });
        return qualities;
    }

    // 노드 검색 (병렬)
    static std::vector<Node*> findNodesInBoundingBox(
        std::vector<Node>& nodes,
        const BoundingBox& bbox) {

        std::vector<Node*> result;
        std::mutex mutex;

        std::for_each(std::execution::par,
                      nodes.begin(), nodes.end(),
                      [&](Node& node) {
                          if (bbox.contains(node.position)) {
                              std::lock_guard<std::mutex> lock(mutex);
                              result.push_back(&node);
                          }
                      });
        return result;
    }

    // 합계/통계 (병렬 리듀스)
    static double calculateTotalVolume(const std::vector<Element>& elements,
                                       const std::vector<Node>& nodes) {
        return std::transform_reduce(
            std::execution::par,
            elements.begin(), elements.end(),
            0.0,
            std::plus<>(),
            [&nodes](const Element& elem) {
                return elem.calculateVolume(nodes);
            });
    }
};
```

### 비동기 파일 I/O

```cpp
#include <fstream>
#include <future>

class KOO_API AsyncFileWriter {
public:
    explicit AsyncFileWriter(const std::filesystem::path& path,
                            size_t bufferSize = 64 * 1024)
        : path_(path), bufferSize_(bufferSize) {
        buffer_.reserve(bufferSize_);
    }

    ~AsyncFileWriter() {
        flush();
        if (writeThread_.valid()) {
            writeThread_.wait();
        }
    }

    // 버퍼에 쓰기 (비동기 플러시)
    void write(std::string_view data) {
        std::lock_guard<std::mutex> lock(mutex_);
        buffer_.append(data);

        if (buffer_.size() >= bufferSize_) {
            flushAsync();
        }
    }

    void writeLine(std::string_view line) {
        write(line);
        write("\n");
    }

    void flush() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (writeThread_.valid()) {
            writeThread_.wait();
        }
        if (!buffer_.empty()) {
            writeToFile(buffer_);
            buffer_.clear();
        }
    }

private:
    void flushAsync() {
        if (writeThread_.valid()) {
            writeThread_.wait();
        }
        std::string toWrite = std::move(buffer_);
        buffer_.clear();
        buffer_.reserve(bufferSize_);

        writeThread_ = std::async(std::launch::async,
                                  [this, data = std::move(toWrite)]() {
                                      writeToFile(data);
                                  });
    }

    void writeToFile(const std::string& data) {
        std::ofstream file(path_, std::ios::app | std::ios::binary);
        file.write(data.data(), data.size());
    }

    std::filesystem::path path_;
    size_t bufferSize_;
    std::string buffer_;
    std::future<void> writeThread_;
    std::mutex mutex_;
};
```

### Thread-Safe 데이터 구조

```cpp
#include <shared_mutex>
#include <unordered_map>

// Thread-safe 노드 컨테이너
template<typename Key, typename Value>
class KOO_API ConcurrentMap {
public:
    void insert(const Key& key, Value value) {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        data_[key] = std::move(value);
    }

    std::optional<Value> find(const Key& key) const {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        auto it = data_.find(key);
        if (it != data_.end()) {
            return it->second;
        }
        return std::nullopt;
    }

    bool contains(const Key& key) const {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        return data_.find(key) != data_.end();
    }

    size_t size() const {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        return data_.size();
    }

    // 병렬 순회용 스냅샷 생성
    std::vector<std::pair<Key, Value>> snapshot() const {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        return {data_.begin(), data_.end()};
    }

private:
    mutable std::shared_mutex mutex_;
    std::unordered_map<Key, Value> data_;
};

// Lock-free 노드 ID 생성기
class KOO_API AtomicIdGenerator {
public:
    AtomicIdGenerator(int64_t start = 1) : nextId_(start) {}

    int64_t next() {
        return nextId_.fetch_add(1, std::memory_order_relaxed);
    }

    int64_t current() const {
        return nextId_.load(std::memory_order_relaxed);
    }

    void reset(int64_t value = 1) {
        nextId_.store(value, std::memory_order_relaxed);
    }

private:
    std::atomic<int64_t> nextId_;
};
```

### 병렬화 설정 옵션

```cpp
struct KOO_API ParallelConfig {
    // 스레드 수 (0 = 자동 감지)
    size_t numThreads = 0;

    // 병렬 처리 임계값 (이 크기 이상일 때만 병렬화)
    size_t parallelThreshold = 1000;

    // 청크 크기 (작업 분할 단위)
    size_t chunkSize = 256;

    // 기능별 활성화
    bool parallelParsing = true;
    bool parallelWriting = true;
    bool parallelComputation = true;

    // 싱글톤 전역 설정
    static ParallelConfig& global() {
        static ParallelConfig config;
        return config;
    }
};

// 사용 예시
void configureParallelization() {
    auto& config = ParallelConfig::global();
    config.numThreads = 8;           // 8 스레드 사용
    config.parallelThreshold = 500;  // 500개 이상일 때 병렬화
    config.parallelParsing = true;   // 파싱 병렬화 활성화
}
```

### 병렬화 가이드라인

1. **최소 단위**: 병렬화 오버헤드를 고려하여 최소 처리 단위 설정
   - 노드/요소: 1000개 이상
   - 키워드 블록: 10개 이상
   - 파일: 2개 이상

2. **메모리 관리**:
   - 각 스레드별 로컬 버퍼 사용
   - 최종 단계에서만 결과 병합
   - 불필요한 복사 최소화

3. **동기화 최소화**:
   - Lock-free 구조 우선
   - Reader-writer lock 활용 (`std::shared_mutex`)
   - 결과 병합 시에만 동기화

4. **NUMA 인식** (선택적):
   - 대용량 데이터 처리 시 메모리 지역성 고려
   - 스레드 affinity 설정 옵션 제공

---

## Architecture Overview

### Modular Architecture (모듈화 아키텍처)

본 프로젝트는 **완전히 독립적인 모듈**로 구성되어, 각 모듈이 서로의 기능에 침범하지 않으면서도 자연스럽게 통합될 수 있는 구조를 채택합니다.

```
┌─────────────────────────────────────────────────────────────────────────────────────────┐
│                              KooSimulationDataReader                                     │
│                                  Unified Framework                                       │
├─────────────────────────────────────────────────────────────────────────────────────────┤
│                                                                                          │
│  ┌────────────────────┐   ┌────────────────────┐   ┌────────────────────┐               │
│  │    koo::dyna       │   │    koo::mcad       │   │    koo::ecad       │               │
│  │  (LS-DYNA/CAE)     │   │   (STEP/MCAD)      │   │   (ODB++/ECAD)     │               │
│  │                    │   │                    │   │                    │               │
│  │  ┌──────────────┐  │   │  ┌──────────────┐  │   │  ┌──────────────┐  │               │
│  │  │   Model      │  │   │  │   Product    │  │   │  │   OdbJob     │  │               │
│  │  │   Node       │  │   │  │   Assembly   │  │   │  │   Step       │  │               │
│  │  │   Element    │  │   │  │   Part       │  │   │  │   Layer      │  │               │
│  │  │   Material   │  │   │  │   Shape      │  │   │  │   Feature    │  │               │
│  │  │   Contact    │  │   │  │   PMI        │  │   │  │   Net        │  │               │
│  │  │   ...        │  │   │  │   ...        │  │   │  │   ...        │  │               │
│  │  └──────────────┘  │   │  └──────────────┘  │   │  └──────────────┘  │               │
│  └─────────┬──────────┘   └─────────┬──────────┘   └─────────┬──────────┘               │
│            │                        │                        │                          │
│            │         ┌──────────────┴──────────────┐         │                          │
│            │         │      Integration Layer       │         │                          │
│            └─────────┤   (Optional Connectors)      ├─────────┘                          │
│                      │                              │                                    │
│                      │  ┌──────────────────────┐   │                                    │
│                      │  │ CadToMeshConverter   │   │  (MCAD → CAE)                      │
│                      │  │ PcbToMeshConverter   │   │  (ECAD → CAE)                      │
│                      │  │ PartGeometryLink     │   │  (CAE ↔ MCAD)                      │
│                      │  └──────────────────────┘   │                                    │
│                      └─────────────────────────────┘                                    │
│                                                                                          │
│  ┌──────────────────────────────────────────────────────────────────────────────────┐   │
│  │                              Algorithm Module                                     │   │
│  │  ┌────────────┐  ┌────────────┐  ┌────────────┐  ┌────────────┐                  │   │
│  │  │   Mesh     │  │  Contact   │  │ Validation │  │  Convert   │                  │   │
│  │  │ Algorithm  │  │ Generator  │  │   Engine   │  │   Engine   │                  │   │
│  │  └────────────┘  └────────────┘  └────────────┘  └────────────┘                  │   │
│  └──────────────────────────────────────────────────────────────────────────────────┘   │
│                                                                                          │
│  ┌──────────────────────────────────────────────────────────────────────────────────┐   │
│  │                               Facade API (koo::KooSim)                            │   │
│  │                        Simple unified interface for all modules                   │   │
│  └──────────────────────────────────────────────────────────────────────────────────┘   │
│                                                                                          │
└─────────────────────────────────────────────────────────────────────────────────────────┘
```

### Module Independence (모듈 독립성)

각 모듈은 **독립적으로 빌드 및 사용 가능**합니다:

```cmake
# 개별 모듈만 사용
find_package(KooSim COMPONENTS dyna)      # LS-DYNA만
find_package(KooSim COMPONENTS mcad)      # MCAD만
find_package(KooSim COMPONENTS ecad)      # ECAD만

# 통합 사용
find_package(KooSim COMPONENTS dyna mcad) # CAE + MCAD 통합
find_package(KooSim)                      # 전체 모듈
```

```cpp
// MCAD만 사용 - CAE 의존성 없음
#include <koo/mcad/Product.hpp>
auto product = koo::mcad::Product::loadStep("model.step");

// ECAD만 사용 - 다른 모듈 의존성 없음
#include <koo/ecad/OdbJob.hpp>
auto job = koo::ecad::OdbReader().read("pcb.odb");

// 통합 사용 (선택적)
#include <koo/mcad/integration/CadToMeshConverter.hpp>
koo::mcad::CadToMeshConverter converter;
koo::dyna::Model mesh = converter.convert(product);
```

### Module Registry (모듈 레지스트리)

새로운 모듈을 자연스럽게 추가할 수 있는 플러그인 아키텍처:

```cpp
namespace koo {

/**
 * @brief 모듈 레지스트리 - 새 모듈 동적 등록
 */
class ModuleRegistry {
public:
    static ModuleRegistry& instance();

    // 모듈 등록
    void registerModule(const std::string& name,
                       std::unique_ptr<ModuleInterface> module);

    // 모듈 조회
    ModuleInterface* getModule(const std::string& name);
    std::vector<std::string> getRegisteredModules() const;

    // 포맷 어댑터 등록
    void registerFormatAdapter(std::unique_ptr<FormatAdapter> adapter);
    FormatAdapter* getAdapterByExtension(const std::string& ext);
};

// 자동 등록 매크로
#define REGISTER_MODULE(ModuleClass, ModuleName) \
    static bool _reg_##ModuleClass = []() { \
        ModuleRegistry::instance().registerModule( \
            ModuleName, std::make_unique<ModuleClass>()); \
        return true; \
    }()

} // namespace koo
```

### Detailed Module Architecture

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                              koo::dyna Module                                │
│                           (LS-DYNA / CAE Core)                               │
├─────────────────────────────────────────────────────────────────────────────┤
│  ┌─────────────────┐  ┌─────────────────┐  ┌─────────────────┐              │
│  │   Reader        │  │   Writer        │  │  Converter      │              │
│  │   Module        │  │   Module        │  │  Module         │              │
│  └──────┬──────────┘  └──────┬──────────┘  └──────┬──────────┘              │
│         │                    │                    │                          │
│  ┌──────┴────────────────────┴────────────────────┴──────┐                  │
│  │                   Core Data Model                      │                  │
│  │  ┌──────────┐ ┌──────────┐ ┌──────────┐ ┌──────────┐  │                  │
│  │  │  Node    │ │ Element  │ │   Part   │ │ Material │  │                  │
│  │  └──────────┘ └──────────┘ └──────────┘ └──────────┘  │                  │
│  │  ┌──────────┐ ┌──────────┐ ┌──────────┐ ┌──────────┐  │                  │
│  │  │ Section  │ │ Contact  │ │ Boundary │ │  Load    │  │                  │
│  │  └──────────┘ └──────────┘ └──────────┘ └──────────┘  │                  │
│  └───────────────────────────────────────────────────────┘                  │
└─────────────────────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────────────────────┐
│                              koo::mcad Module                                │
│                           (STEP / Mechanical CAD)                            │
├─────────────────────────────────────────────────────────────────────────────┤
│  ┌─────────────────────────────────────────────────────────────────────┐    │
│  │                      Product Structure                               │    │
│  │  ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌──────────┐            │    │
│  │  │ Product  │──│ Assembly │──│   Part   │──│   Body   │            │    │
│  │  └──────────┘  └──────────┘  └──────────┘  └──────────┘            │    │
│  └─────────────────────────────────────────────────────────────────────┘    │
│  ┌─────────────────────────────────────────────────────────────────────┐    │
│  │                      Geometry Kernel (OpenCASCADE)                   │    │
│  │  ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌──────────┐            │    │
│  │  │  Solid   │  │   Face   │  │   Edge   │  │  Vertex  │            │    │
│  │  └──────────┘  └──────────┘  └──────────┘  └──────────┘            │    │
│  └─────────────────────────────────────────────────────────────────────┘    │
│  ┌─────────────────────────────────────────────────────────────────────┐    │
│  │                      Engineering Data                                │    │
│  │  ┌──────────┐  ┌──────────┐  ┌──────────┐                          │    │
│  │  │ Material │  │   PMI    │  │   GD&T   │                          │    │
│  │  └──────────┘  └──────────┘  └──────────┘                          │    │
│  └─────────────────────────────────────────────────────────────────────┘    │
└─────────────────────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────────────────────┐
│                              koo::ecad Module                                │
│                            (ODB++ / Electronic CAD)                          │
├─────────────────────────────────────────────────────────────────────────────┤
│  ┌─────────────────────────────────────────────────────────────────────┐    │
│  │                      PCB Structure                                   │    │
│  │  ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌──────────┐            │    │
│  │  │  OdbJob  │──│   Step   │──│  Layer   │──│ Feature  │            │    │
│  │  └──────────┘  └──────────┘  └──────────┘  └──────────┘            │    │
│  └─────────────────────────────────────────────────────────────────────┘    │
│  ┌─────────────────────────────────────────────────────────────────────┐    │
│  │                      EDA Data                                        │    │
│  │  ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌──────────┐            │    │
│  │  │Component │  │   Net    │  │  Package │  │   Pin    │            │    │
│  │  └──────────┘  └──────────┘  └──────────┘  └──────────┘            │    │
│  └─────────────────────────────────────────────────────────────────────┘    │
│  ┌─────────────────────────────────────────────────────────────────────┐    │
│  │                      Layer Types                                     │    │
│  │  ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌──────────┐            │    │
│  │  │  Copper  │  │SolderMask│  │Silkscreen│  │  Drill   │            │    │
│  │  └──────────┘  └──────────┘  └──────────┘  └──────────┘            │    │
│  └─────────────────────────────────────────────────────────────────────┘    │
└─────────────────────────────────────────────────────────────────────────────┘
```

### Strategy Documents (전략 문서)

각 모듈의 상세 전략은 별도 문서를 참조하세요:

- **MCAD Strategy**: [docs/MCAD_STRATEGY.md](docs/MCAD_STRATEGY.md) - STEP/CAD 모듈 상세 설계
- **ECAD Strategy**: [docs/ECAD_STRATEGY.md](docs/ECAD_STRATEGY.md) - ODB++/PCB 모듈 상세 설계

---

## LS-DYNA Keyword Statistics

| Volume | Categories | Keywords | Major Categories |
|--------|-----------|----------|------------------|
| Vol I  | 45 | ~800 | *CONTROL, *DEFINE, *BOUNDARY, *LOAD, *CONTACT, *ELEMENT, *NODE, *PART, *SECTION, *SET |
| Vol II | 2 | ~340 | *MAT (325), *MAT_THERMAL (13) |
| Vol III | 10 | ~360 | *ICFD (104), *DUALCESE (87), *EM (71), *CESE (41) |
| **Total** | **57** | **~1500** | |

---

## C++ Class Hierarchy

### 1. Base Classes

```cpp
namespace koo {
namespace dyna {

// 모든 키워드의 기본 클래스
class Keyword {
public:
    virtual ~Keyword() = default;
    virtual std::string getKeywordName() const = 0;
    virtual void parse(const std::vector<std::string>& lines) = 0;
    virtual std::string write() const = 0;
    virtual std::unique_ptr<Keyword> clone() const = 0;

protected:
    int id_ = 0;
    std::string title_;
};

// ID를 가지는 키워드 (대부분의 키워드)
class IdentifiableKeyword : public Keyword {
protected:
    int id_;
public:
    int getId() const { return id_; }
    void setId(int id) { id_ = id; }
};

// 카드 기반 파싱 유틸리티
class CardParser {
public:
    static int readInt(const std::string& field, int defaultVal = 0);
    static double readDouble(const std::string& field, double defaultVal = 0.0);
    static std::string readString(const std::string& field);
    static std::vector<std::string> splitCard(const std::string& line, int fieldWidth = 10);
};

}} // namespace koo::dyna
```

### 2. Core Data Classes

```cpp
namespace koo {
namespace dyna {

// 노드
class Node : public IdentifiableKeyword {
    double x_, y_, z_;
    int tc_, rc_;  // translation/rotation constraints
};

// 요소 기본 클래스
class Element : public IdentifiableKeyword {
protected:
    int partId_;
    std::vector<int> nodeIds_;
};

class ElementSolid : public Element { /* 8-node hex, 4-node tet, etc */ };
class ElementShell : public Element { /* 3/4-node shell */ };
class ElementBeam : public Element { /* 2-node beam */ };
class ElementDiscrete : public Element { /* spring, damper */ };

// 파트
class Part : public IdentifiableKeyword {
    int sectionId_;
    int materialId_;
    int hourglass_;
    std::string geometryFile_;  // STEP/IGES file path for CAD association
};

// 섹션 기본 클래스
class Section : public IdentifiableKeyword {
public:
    virtual SectionType getType() const = 0;
};

class SectionShell : public Section { /* ELFORM, thickness, etc */ };
class SectionSolid : public Section { /* ELFORM, AET, etc */ };
class SectionBeam : public Section { /* CST, cross-section params */ };

// 재료 기본 클래스
class Material : public IdentifiableKeyword {
protected:
    double density_;
public:
    virtual MaterialType getType() const = 0;
};

class MatElastic : public Material { /* MAT_001 */ };
class MatPlasticKinematic : public Material { /* MAT_003 */ };
class MatPiecewiseLinearPlasticity : public Material { /* MAT_024 */ };
// ... 325+ material types

}} // namespace koo::dyna
```

### 3. Keyword Category Classes

```cpp
namespace koo {
namespace dyna {

// *CONTROL 계열
class ControlKeyword : public Keyword {};
class ControlTimestep : public ControlKeyword {};
class ControlTermination : public ControlKeyword {};
class ControlEnergy : public ControlKeyword {};
// ... 70+ control keywords

// *BOUNDARY 계열
class BoundaryKeyword : public IdentifiableKeyword {};
class BoundarySPC : public BoundaryKeyword {};
class BoundaryPrescribedMotion : public BoundaryKeyword {};
// ... 57 boundary keywords

// *CONTACT 계열
class Contact : public IdentifiableKeyword {};
class ContactAutomatic : public Contact {};
class ContactTiedShellEdgeToSurface : public Contact {};
// ... 14+ contact types with options

// *LOAD 계열
class Load : public IdentifiableKeyword {};
class LoadNodePoint : public Load {};
class LoadBodyZ : public Load {};
class LoadSegment : public Load {};
// ... 53 load keywords

// *DEFINE 계열
class DefineKeyword : public IdentifiableKeyword {};
class DefineCurve : public DefineKeyword {};
class DefineCoordinateSystem : public DefineKeyword {};
class DefineTransformation : public DefineKeyword {};
// ... 142 define keywords

// *SET 계열
class Set : public IdentifiableKeyword {};
class SetNode : public Set {};
class SetPart : public Set {};
class SetSegment : public Set {};
class SetShell : public Set {};
class SetSolid : public Set {};
// ... 30 set keywords

// *DATABASE 계열
class DatabaseKeyword : public Keyword {};
class DatabaseBinaryD3Plot : public DatabaseKeyword {};
class DatabaseExtentBinary : public DatabaseKeyword {};
// ... 42 database keywords

// *INITIAL 계열
class InitialKeyword : public IdentifiableKeyword {};
class InitialVelocity : public InitialKeyword {};
class InitialStress : public InitialKeyword {};
// ... 51 initial keywords

}} // namespace koo::dyna
```

### 4. Manager Classes

```cpp
namespace koo {
namespace dyna {

// 전체 모델 관리
class Model {
public:
    // Accessors
    NodeManager& nodes() { return nodes_; }
    ElementManager& elements() { return elements_; }
    PartManager& parts() { return parts_; }
    MaterialManager& materials() { return materials_; }
    SectionManager& sections() { return sections_; }
    ContactManager& contacts() { return contacts_; }
    LoadManager& loads() { return loads_; }
    BoundaryManager& boundaries() { return boundaries_; }
    SetManager& sets() { return sets_; }
    ControlManager& controls() { return controls_; }
    DatabaseManager& database() { return database_; }
    DefineManager& defines() { return defines_; }

    // I/O
    void readKeywordFile(const std::string& filepath);
    void writeKeywordFile(const std::string& filepath) const;

    // Validation
    ValidationResult validate() const;

private:
    NodeManager nodes_;
    ElementManager elements_;
    PartManager parts_;
    MaterialManager materials_;
    SectionManager sections_;
    ContactManager contacts_;
    LoadManager loads_;
    BoundaryManager boundaries_;
    SetManager sets_;
    ControlManager controls_;
    DatabaseManager database_;
    DefineManager defines_;
    // ... other managers
};

// 개별 Manager 템플릿
template<typename T>
class KeywordManager {
public:
    void add(std::unique_ptr<T> item);
    T* get(int id);
    const T* get(int id) const;
    void remove(int id);
    std::vector<T*> getAll();

    // Iteration
    auto begin() { return items_.begin(); }
    auto end() { return items_.end(); }

private:
    std::unordered_map<int, std::unique_ptr<T>> items_;
};

using NodeManager = KeywordManager<Node>;
using ElementManager = KeywordManager<Element>;
using PartManager = KeywordManager<Part>;
using MaterialManager = KeywordManager<Material>;
using SectionManager = KeywordManager<Section>;

}} // namespace koo::dyna
```

### 5. Parser/Writer Classes

```cpp
namespace koo {
namespace dyna {

// K파일 파서
class KeywordFileReader {
public:
    Model read(const std::string& filepath);
    Model read(std::istream& stream);

private:
    std::unique_ptr<Keyword> parseKeywordBlock(
        const std::string& keywordName,
        const std::vector<std::string>& lines
    );

    KeywordFactory factory_;
};

// K파일 작성기
class KeywordFileWriter {
public:
    void write(const Model& model, const std::string& filepath);
    void write(const Model& model, std::ostream& stream);

    // Options
    void setFieldWidth(int width); // 10 or 20 (large format)
    void setIncludeComments(bool include);
};

// 키워드 팩토리
class KeywordFactory {
public:
    static KeywordFactory& instance();

    void registerKeyword(const std::string& name,
                         std::function<std::unique_ptr<Keyword>()> creator);
    std::unique_ptr<Keyword> create(const std::string& name);

private:
    std::unordered_map<std::string,
                       std::function<std::unique_ptr<Keyword>()>> creators_;
};

// 키워드 자동 등록 매크로
#define REGISTER_KEYWORD(ClassName, KeywordName) \
    static bool _registered_##ClassName = []() { \
        KeywordFactory::instance().registerKeyword(KeywordName, \
            []() { return std::make_unique<ClassName>(); }); \
        return true; \
    }()

}} // namespace koo::dyna
```

### 6. Algorithm Classes

```cpp
namespace koo {
namespace algorithm {

// 메쉬 알고리즘 기본 클래스
class MeshAlgorithm {
public:
    virtual ~MeshAlgorithm() = default;
    virtual void execute(dyna::Model& model) = 0;
};

// 메쉬 품질 개선
class MeshSmoothing : public MeshAlgorithm {
public:
    enum Method { Laplacian, Optimization };
    void setMethod(Method m);
    void setIterations(int n);
    void execute(dyna::Model& model) override;
};

// 메쉬 리파인먼트
class MeshRefinement : public MeshAlgorithm {
public:
    void setTargetSize(double size);
    void setRegion(const BoundingBox& box);
    void execute(dyna::Model& model) override;
};

// 접촉 자동 생성
class AutoContact : public MeshAlgorithm {
public:
    void setContactType(dyna::ContactType type);
    void setMasterParts(const std::vector<int>& partIds);
    void setSlaveParts(const std::vector<int>& partIds);
    void execute(dyna::Model& model) override;
};

// 연결 생성 (Tie, Weld 등)
class ConnectionGenerator : public MeshAlgorithm {
public:
    enum Type { TiedContact, SpotWeld, SeamWeld };
    void setType(Type t);
    void setTolerance(double tol);
    void execute(dyna::Model& model) override;
};

// 질량/관성 계산
class MassProperties : public MeshAlgorithm {
public:
    struct Result {
        double totalMass;
        Vec3 centerOfGravity;
        Mat3x3 inertiaTensor;
    };
    Result calculate(const dyna::Model& model);
    void execute(dyna::Model& model) override;
};

}} // namespace koo::algorithm
```

### 7. Geometry Integration

```cpp
namespace koo {
namespace geometry {

// CAD 형상 기본 클래스
class Shape {
public:
    virtual ~Shape() = default;
    virtual BoundingBox getBoundingBox() const = 0;
    virtual double getVolume() const = 0;
    virtual double getSurfaceArea() const = 0;
};

// STEP 파일 리더 (OpenCASCADE 기반)
class StepReader {
public:
    std::vector<std::shared_ptr<Shape>> read(const std::string& filepath);
};

// 형상-파트 연동
class GeometryPartLink {
public:
    void associatePart(int partId, std::shared_ptr<Shape> shape);
    std::shared_ptr<Shape> getShape(int partId) const;

    // 형상 기반 메쉬 생성
    void generateMesh(int partId, const MeshParameters& params);
};

// 메쉬 파라미터
struct MeshParameters {
    double elementSize;
    int elementType;  // shell, solid
    int meshAlgorithm; // Delaunay, Frontal, etc.
    bool quadDominant;
};

}} // namespace koo::geometry
```

### 8. Format Converters

```cpp
namespace koo {
namespace converter {

// 변환기 인터페이스
class FormatConverter {
public:
    virtual ~FormatConverter() = default;
    virtual dyna::Model importModel(const std::string& filepath) = 0;
    virtual void exportModel(const dyna::Model& model,
                            const std::string& filepath) = 0;
};

// OpenRadioss 변환기
class OpenRadiossConverter : public FormatConverter {
public:
    dyna::Model importModel(const std::string& filepath) override;
    void exportModel(const dyna::Model& model,
                    const std::string& filepath) override;
};

// GMSH 변환기
class GmshConverter : public FormatConverter {
public:
    dyna::Model importModel(const std::string& filepath) override;
    void exportModel(const dyna::Model& model,
                    const std::string& filepath) override;
};

// Nastran 변환기
class NastranConverter : public FormatConverter {
public:
    dyna::Model importModel(const std::string& filepath) override;
    void exportModel(const dyna::Model& model,
                    const std::string& filepath) override;
};

// Abaqus 변환기
class AbaqusConverter : public FormatConverter {
public:
    dyna::Model importModel(const std::string& filepath) override;
    void exportModel(const dyna::Model& model,
                    const std::string& filepath) override;
};

}} // namespace koo::converter
```

---

## Project Structure

```
KooSimulationDataReader/
├── CMakeLists.txt                 # Root CMake configuration
├── cmake/
│   ├── KooSimConfig.cmake.in      # Package config for find_package()
│   ├── FindOpenCASCADE.cmake      # OpenCASCADE finder
│   ├── CompilerWarnings.cmake     # Cross-platform warning flags
│   └── Platform.cmake             # Platform-specific settings
│
├── include/                       # ⭐ Public headers (installed)
│   └── koo/
│       ├── Export.hpp             # DLL export macros
│       ├── Version.hpp            # Version info
│       ├── dyna/
│       │   ├── Keyword.hpp
│       │   ├── Model.hpp
│       │   ├── Node.hpp
│       │   ├── Element.hpp
│       │   ├── Part.hpp
│       │   ├── Material.hpp
│       │   ├── Section.hpp
│       │   ├── Contact.hpp
│       │   ├── Boundary.hpp
│       │   ├── Load.hpp
│       │   ├── Set.hpp
│       │   ├── Control.hpp
│       │   ├── Database.hpp
│       │   ├── Define.hpp
│       │   ├── Initial.hpp
│       │   ├── KeywordFactory.hpp
│       │   ├── KeywordFileReader.hpp
│       │   ├── KeywordFileWriter.hpp
│       │   ├── managers/
│       │   │   ├── NodeManager.hpp
│       │   │   ├── ElementManager.hpp
│       │   │   └── ...
│       │   └── keywords/
│       │       ├── mat/
│       │       │   ├── MatElastic.hpp          # MAT_001
│       │       │   ├── MatPlasticKinematic.hpp # MAT_003
│       │       │   └── ...
│       │       ├── control/
│       │       ├── boundary/
│       │       ├── contact/
│       │       └── ...
│       ├── algorithm/
│       │   ├── MeshAlgorithm.hpp
│       │   ├── MeshSmoothing.hpp
│       │   ├── MeshRefinement.hpp
│       │   ├── AutoContact.hpp
│       │   └── ConnectionGenerator.hpp
│       ├── mcad/                      # ⭐ MCAD Module (STEP/Mechanical CAD)
│       │   ├── Product.hpp           # 최상위 제품 컨테이너
│       │   ├── Assembly.hpp          # 어셈블리 계층
│       │   ├── Part.hpp              # 파트 (단일 부품)
│       │   ├── Instance.hpp          # 인스턴스 (배치 정보)
│       │   ├── geometry/
│       │   │   ├── Shape.hpp         # 형상 기본 클래스
│       │   │   ├── Solid.hpp         # 솔리드
│       │   │   ├── Face.hpp          # 면
│       │   │   ├── Edge.hpp          # 에지
│       │   │   ├── Vertex.hpp        # 정점
│       │   │   └── surfaces/         # 서피스 타입들
│       │   ├── engineering/
│       │   │   ├── Material.hpp      # 재료 정보
│       │   │   ├── PMI.hpp           # 제조 정보
│       │   │   └── GDT.hpp           # 공차 정보
│       │   ├── StepReader.hpp        # STEP 파일 읽기
│       │   ├── StepWriter.hpp        # STEP 파일 쓰기
│       │   └── integration/          # CAE 통합 (선택적)
│       │       ├── CadToMeshConverter.hpp
│       │       └── PartGeometryLink.hpp
│       ├── ecad/                      # ⭐ ECAD Module (ODB++/Electronic CAD)
│       │   ├── OdbJob.hpp            # ODB++ 작업 컨테이너
│       │   ├── Step.hpp              # 스텝 (pcb, panel 등)
│       │   ├── Layer.hpp             # 레이어 데이터
│       │   ├── Feature.hpp           # 피처 (Line, Pad, Arc 등)
│       │   ├── Symbol.hpp            # 심볼 정의
│       │   ├── layers/
│       │   │   ├── CopperLayer.hpp   # 동박 레이어
│       │   │   ├── SolderMaskLayer.hpp
│       │   │   ├── DrillLayer.hpp    # 드릴 레이어
│       │   │   └── ...
│       │   ├── eda/
│       │   │   ├── Component.hpp     # 컴포넌트
│       │   │   ├── Net.hpp           # 넷 정보
│       │   │   └── Package.hpp       # 패키지
│       │   ├── OdbReader.hpp         # ODB++ 읽기
│       │   ├── OdbWriter.hpp         # ODB++ 쓰기
│       │   └── integration/          # CAE 통합 (선택적)
│       │       └── PcbToMeshConverter.hpp
│       ├── converter/
│       │   ├── FormatConverter.hpp
│       │   ├── OpenRadiossConverter.hpp
│       │   ├── GmshConverter.hpp
│       │   └── NastranConverter.hpp
│       └── util/
│           ├── CardParser.hpp
│           ├── StringUtils.hpp
│           └── Types.hpp
│
├── src/                           # ⭐ Implementation files
│   ├── CMakeLists.txt             # Library build config
│   ├── dyna/                      # LS-DYNA module implementation
│   ├── mcad/                      # MCAD module implementation
│   ├── ecad/                      # ECAD module implementation
│   ├── algorithm/
│   ├── converter/
│   ├── util/
│   └── c_api/                     # C interface implementation
│       └── CApi.cpp
│
├── cli/                           # ⭐ CLI executable
│   ├── CMakeLists.txt
│   ├── main.cpp
│   └── commands/
│       ├── ConvertCommand.cpp
│       ├── ValidateCommand.cpp
│       └── InfoCommand.cpp
│
├── tests/                         # ⭐ 테스트 (별도 폴더)
│   ├── CMakeLists.txt             # Test build config
│   ├── unit/
│   │   ├── TestNode.cpp
│   │   ├── TestElement.cpp
│   │   ├── TestMaterial.cpp
│   │   ├── TestKeywordFileReader.cpp
│   │   └── ...
│   ├── integration/
│   │   ├── TestRoundTrip.cpp      # Read → Write → Read 검증
│   │   ├── TestLargeFile.cpp
│   │   └── ...
│   └── test_data/
│       └── k_files/
│           ├── simple_model.k
│           ├── crash_model.k
│           └── ...
│
├── examples/                      # 사용 예제
│   ├── CMakeLists.txt
│   ├── 01_read_write_basic.cpp
│   ├── 02_mesh_conversion.cpp
│   ├── 03_auto_contact_generation.cpp
│   └── 04_cad_integration.cpp
│
├── python/                        # Python bindings (Phase 6)
│   ├── CMakeLists.txt
│   ├── bindings.cpp
│   └── koo_sim/
│       └── __init__.py
│
├── docs/
│   ├── api/                       # Doxygen generated
│   └── manual/                    # User manual
│
├── Manuals/                       # LS-DYNA reference (not installed)
│   ├── LS-DYNA/
│   ├── Text/                      # PDF → Text converted
│   └── ODB/
│
└── .github/
    └── workflows/
        ├── build.yml              # CI: Build on Win/Linux/macOS
        └── test.yml               # CI: Run tests
```

### Build Output Structure

```
build/
├── lib/
│   ├── libkoo_sim.so              # Linux shared library
│   ├── libkoo_sim_static.a        # Linux static library (필수)
│   ├── koo_sim.dll                # Windows shared library
│   ├── koo_sim.lib                # Windows import library (for DLL)
│   ├── koo_sim_static.lib         # Windows static library (필수)
│   └── libkoo_sim.dylib           # macOS shared library
│
├── bin/
│   ├── koo_sim_cli                # CLI executable
│   └── koo_sim_tests              # Test executable
│
└── python/
    └── koo_sim.cpython-*.so       # Python module
```

### Static vs Shared Library 사용 가이드

| 상황 | 권장 라이브러리 |
|------|----------------|
| 플러그인/DLL 형태로 동적 로딩 | Shared (koo_sim.dll/so) |
| 단일 실행 파일로 배포 | Static (koo_sim_static.lib/a) |
| 임베디드 시스템 | Static |
| Python/다른 언어 바인딩 | Shared |
| 개발 중 빠른 빌드 | Shared |

```cmake
# Shared 링크
target_link_libraries(your_app PRIVATE KooSim::koo_sim)

# Static 링크 (명시적)
target_link_libraries(your_app PRIVATE KooSim::koo_sim_static)
```

---

## Development Phases

### Phase 1: Core Foundation (Current Focus)
**목표: LS-DYNA K파일 읽기/쓰기 기본 기능**

#### 1.1 Project Setup
- [ ] CMake 빌드 시스템 구성
- [ ] 크로스 플랫폼 설정 (Windows/Linux/macOS)
- [ ] 테스트 프레임워크 설정 (Google Test)
- [ ] CI/CD 파이프라인 (GitHub Actions)

#### 1.2 Core Classes
- [ ] `Keyword` 기본 클래스 및 인터페이스
- [ ] `CardParser` 유틸리티 (10/20 column format)
- [ ] `KeywordFactory` 패턴 구현
- [ ] `Model` 컨테이너 클래스

#### 1.3 Essential Keywords (우선순위)
**Tier 1 - 필수 (모든 모델에 사용)**
- [ ] `*NODE` - 노드 정의
- [ ] `*ELEMENT_SOLID`, `*ELEMENT_SHELL`, `*ELEMENT_BEAM`
- [ ] `*PART` - 파트 정의
- [ ] `*SECTION_SHELL`, `*SECTION_SOLID`, `*SECTION_BEAM`
- [ ] `*MAT_ELASTIC` (MAT_001)
- [ ] `*MAT_PLASTIC_KINEMATIC` (MAT_003)
- [ ] `*MAT_PIECEWISE_LINEAR_PLASTICITY` (MAT_024)
- [ ] `*SET_NODE`, `*SET_PART`, `*SET_SHELL`, `*SET_SOLID`, `*SET_SEGMENT`
- [ ] `*END`

**Tier 2 - 일반 (대부분의 모델에 사용)**
- [ ] `*CONTROL_TIMESTEP`, `*CONTROL_TERMINATION`, `*CONTROL_ENERGY`
- [ ] `*DATABASE_BINARY_D3PLOT`, `*DATABASE_EXTENT_BINARY`
- [ ] `*BOUNDARY_SPC_SET`, `*BOUNDARY_PRESCRIBED_MOTION`
- [ ] `*LOAD_NODE_SET`, `*LOAD_SEGMENT_SET`, `*LOAD_BODY`
- [ ] `*CONTACT_AUTOMATIC_SINGLE_SURFACE`, `*CONTACT_AUTOMATIC_SURFACE_TO_SURFACE`
- [ ] `*DEFINE_CURVE`, `*DEFINE_COORDINATE_SYSTEM`
- [ ] `*INITIAL_VELOCITY`
- [ ] `*INCLUDE`

**Tier 3 - 확장 (특수 해석용)**
- [ ] 나머지 325개 재료 모델
- [ ] 나머지 Control/Database/Boundary/Load 키워드
- [ ] ALE, SPH, EFG 특수 요소
- [ ] 열해석, 전자기해석 키워드

#### 1.4 I/O System
- [ ] `KeywordFileReader` - K파일 파싱
- [ ] `KeywordFileWriter` - K파일 출력
- [ ] Include 파일 처리
- [ ] 파라미터 치환 (*PARAMETER)
- [ ] 대용량 파일 스트리밍

#### 1.5 Testing
- [ ] 단위 테스트 (각 키워드 클래스)
- [ ] 통합 테스트 (읽기 → 쓰기 라운드트립)
- [ ] 실제 K파일 샘플로 검증

---

### Phase 2: Complete Keyword Coverage
**목표: 모든 1500+ 키워드 지원**

#### 2.1 Vol I Keywords (45 categories, ~800 keywords)
- [ ] *CONTROL (70)
- [ ] *DEFINE (142)
- [ ] *BOUNDARY (57)
- [ ] *LOAD (53)
- [ ] *INITIAL (51)
- [ ] *DATABASE (42)
- [ ] *CONSTRAINED (38)
- [ ] *ALE (37)
- [ ] *ELEMENT (34)
- [ ] *SET (30)
- [ ] *IGA (26)
- [ ] *INTERFACE (21)
- [ ] *EOS (19)
- [ ] *SECTION (17)
- [ ] *FREQUENCY_DOMAIN (18)
- [ ] *CONTACT (14)
- [ ] *ISPG (13)
- [ ] *INCLUDE (13)
- [ ] 나머지...

#### 2.2 Vol II Keywords (2 categories, ~340 keywords)
- [ ] *MAT (325) - 재료 모델 전체
- [ ] *MAT_THERMAL (13)

#### 2.3 Vol III Keywords (10 categories, ~360 keywords)
- [ ] *ICFD (104) - 비압축성 CFD
- [ ] *DUALCESE (87) - 압축성 CFD
- [ ] *EM (71) - 전자기
- [ ] *CESE (41) - 보존법칙 솔버
- [ ] *MESH (14)
- [ ] *CHEMISTRY (13)
- [ ] *BATTERY (10)
- [ ] 나머지...

---

### Phase 3: Algorithm Development
**목표: 메쉬 및 모델 조작 알고리즘**

#### 3.1 Mesh Operations
- [ ] 메쉬 품질 검사 (aspect ratio, jacobian, warpage)
- [ ] 메쉬 스무딩 (Laplacian, Optimization-based)
- [ ] 메쉬 리파인먼트/코어싱
- [ ] 노드/요소 병합
- [ ] 중복 노드 제거
- [ ] 고아 노드 검출

#### 3.2 Model Operations
- [ ] 자동 접촉 생성
- [ ] 연결 생성 (Tied, Spot Weld, Seam Weld)
- [ ] 질량/관성 계산
- [ ] 무게 중심 계산
- [ ] 모델 변환 (이동, 회전, 스케일)
- [ ] 모델 미러링
- [ ] 파트 병합/분할

#### 3.3 Validation
- [ ] 모델 무결성 검사
- [ ] 자유 모서리 검출
- [ ] 관통 검사
- [ ] ID 충돌 검사
- [ ] 참조 무결성 검사

---

### Phase 4: MCAD Module (Mechanical CAD)
**목표: STEP 기반 기계 설계 데이터 고차원 관리 (koo::mcad)**

> 📄 상세 전략: [docs/MCAD_STRATEGY.md](docs/MCAD_STRATEGY.md)

#### 4.1 Core STEP Support
- [ ] OpenCASCADE 통합
- [ ] STEP AP203 읽기/쓰기
- [ ] 기본 형상 클래스 (Solid, Face, Edge, Vertex)
- [ ] 어셈블리 구조 파싱
- [ ] 색상/이름 정보 추출
- [ ] IGES 지원

#### 4.2 Product Structure
- [ ] Product 클래스 (최상위 컨테이너)
- [ ] Assembly 계층 구조
- [ ] Part 및 Instance 관리
- [ ] Transform (위치/방향) 관리
- [ ] 어셈블리 트리 순회

#### 4.3 Advanced Geometry
- [ ] 모든 서피스 타입 지원 (NURBS 등)
- [ ] 모든 커브 타입 지원
- [ ] 불리언 연산 (Fuse, Cut, Common)
- [ ] 필렛/챔퍼 연산
- [ ] 형상 힐링

#### 4.4 Engineering Data (STEP AP242)
- [ ] 재료 정보 읽기/쓰기
- [ ] PMI(Product Manufacturing Information) 지원
- [ ] GD&T(Geometric Dimensioning & Tolerancing) 지원
- [ ] 표면 거칠기 정보
- [ ] 메타데이터 관리

#### 4.5 CAE Integration
- [ ] 표면 메쉬 생성 (CAD → Shell Elements)
- [ ] 볼륨 메쉬 생성 (CAD → Solid Elements)
- [ ] 메쉬 크기 제어 (곡률 적응)
- [ ] Part-Geometry 연결 (PartGeometryLink)
- [ ] CAD 재료 → LS-DYNA 재료 매핑

---

### Phase 5: ECAD Module (Electronic CAD)
**목표: ODB++ 기반 PCB 설계 데이터 완벽 관리 (koo::ecad)**

> 📄 상세 전략: [docs/ECAD_STRATEGY.md](docs/ECAD_STRATEGY.md)

#### 5.1 Core ODB++ Support
- [ ] ODB++ 디렉토리 구조 파싱
- [ ] Matrix 파일 읽기/쓰기
- [ ] Features 파일 파싱 (Line, Pad, Arc, Surface)
- [ ] 기본 심볼 지원 (round, rect, oblong)
- [ ] 압축/압축해제 (zlib)
- [ ] Step & Repeat 지원

#### 5.2 PCB Structure
- [ ] OdbJob 클래스 (최상위 컨테이너)
- [ ] Step 클래스 (pcb, panel 등)
- [ ] Layer 클래스 (Copper, SolderMask, Silkscreen, Drill)
- [ ] Feature 클래스 (Line, Pad, Arc, Surface, Text)
- [ ] Symbol 라이브러리

#### 5.3 EDA Data
- [ ] 컴포넌트 정보 추출
- [ ] 넷리스트 추출 (Net, Pin)
- [ ] 패키지 라이브러리
- [ ] BOM(Bill of Materials) 생성
- [ ] DRC(Design Rule Check) 정보

#### 5.4 Format Converters
- [ ] Gerber → ODB++ 변환
- [ ] ODB++ → Gerber 변환
- [ ] IPC-2581 지원
- [ ] Excellon 드릴 파일

#### 5.5 CAE Integration
- [ ] PCB 3D 모델 생성 (레이어 스택업)
- [ ] 열해석용 메쉬 변환
- [ ] 구조해석용 메쉬 변환
- [ ] 전자기해석 연동 (*EM)

---

### Phase 6: Multi-Format Support
**목표: 다양한 솔버 포맷 지원**

#### 6.1 OpenRadioss
- [ ] Radioss Starter 파일 읽기/쓰기
- [ ] Radioss Engine 파일 읽기/쓰기
- [ ] LS-DYNA ↔ OpenRadioss 변환

#### 6.2 GMSH
- [ ] .msh 파일 읽기/쓰기
- [ ] .geo 파일 생성
- [ ] GMSH API 연동

#### 6.3 Other Formats
- [ ] Nastran BDF
- [ ] Abaqus INP
- [ ] VTK/VTU (시각화용)
- [ ] STL (표면 메쉬)

---

### Phase 7: Advanced Features
**목표: 고급 기능 및 최적화**

#### 7.1 Performance
- [ ] 병렬 파싱 (멀티스레드)
- [ ] 메모리 최적화
- [ ] 대용량 모델 처리 (수백만 요소)

#### 7.2 Scripting Interface
- [ ] Python 바인딩 (pybind11)
- [ ] Python API 설계
- [ ] Jupyter 노트북 예제

#### 7.3 GUI Integration (Optional)
- [ ] Qt 위젯 라이브러리
- [ ] 3D 뷰어 컴포넌트
- [ ] 모델 트리 뷰

---

### Phase Summary (단계 요약)

| Phase | Module | 핵심 목표 | 주요 기술 |
|-------|--------|----------|----------|
| **Phase 1** | koo::dyna | LS-DYNA 기본 I/O | K-file 파싱 |
| **Phase 2** | koo::dyna | 전체 키워드 커버리지 | 1500+ 키워드 |
| **Phase 3** | koo::algorithm | 메쉬/모델 알고리즘 | 스무딩, 접촉 생성 |
| **Phase 4** | **koo::mcad** | **STEP/기계 CAD** | **OpenCASCADE** |
| **Phase 5** | **koo::ecad** | **ODB++/전자 CAD** | **PCB 데이터** |
| **Phase 6** | koo::converter | 멀티 포맷 지원 | Radioss, Nastran |
| **Phase 7** | 전체 | 고급 기능 | 병렬화, Python |

---

## Dependencies

### Required
- **C++17** or later
- **CMake** 3.16+
- **Google Test** (testing)

### Optional
- **OpenCASCADE** 7.6+ (CAD geometry)
- **GMSH SDK** (meshing)
- **Eigen** 3.4+ (linear algebra)
- **spdlog** (logging)
- **fmt** (formatting)
- **pybind11** (Python bindings)

---

## Build Instructions

### Linux / macOS

```bash
# Clone repository
git clone https://github.com/yourusername/KooSimulationDataReader.git
cd KooSimulationDataReader

# Create build directory
mkdir build && cd build

# Configure (basic - shared library + CLI + tests)
cmake .. -DCMAKE_BUILD_TYPE=Release

# Configure options
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_SHARED_LIBS=ON \          # Build shared library (default: ON)
    -DBUILD_STATIC_LIBS=OFF \         # Also build static library
    -DBUILD_CLI=ON \                  # Build CLI executable (default: ON)
    -DBUILD_TESTS=ON \                # Build tests (default: ON)
    -DBUILD_EXAMPLES=ON \             # Build examples
    -DBUILD_PYTHON=OFF \              # Build Python bindings
    -DWITH_OPENCASCADE=OFF            # Enable CAD support

# Build
cmake --build . -j$(nproc)

# Run tests
ctest --output-on-failure

# Install
sudo cmake --install . --prefix /usr/local
```

### Windows (Visual Studio)

```powershell
# Clone repository
git clone https://github.com/yourusername/KooSimulationDataReader.git
cd KooSimulationDataReader

# Create build directory
mkdir build
cd build

# Configure with Visual Studio generator
cmake .. -G "Visual Studio 17 2022" -A x64 `
    -DBUILD_SHARED_LIBS=ON `
    -DBUILD_CLI=ON `
    -DBUILD_TESTS=ON

# Build (Release)
cmake --build . --config Release -j

# Run tests
ctest -C Release --output-on-failure

# Install
cmake --install . --config Release --prefix C:\Program Files\KooSim
```

### Windows (MinGW/MSYS2)

```bash
# Using MSYS2 UCRT64 environment
pacman -S mingw-w64-ucrt-x86_64-cmake mingw-w64-ucrt-x86_64-gcc

mkdir build && cd build
cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
cmake --build . -j$(nproc)
```

### CMake Options

| Option | Default | Description |
|--------|---------|-------------|
| `BUILD_SHARED_LIBS` | ON | Build shared library (.dll/.so/.dylib) |
| `BUILD_STATIC_LIBS` | ON | Build static library (.lib/.a) - **기본 활성화** |
| `BUILD_CLI` | ON | Build CLI executable |
| `BUILD_TESTS` | ON | Build test executable |
| `BUILD_EXAMPLES` | OFF | Build example programs |
| `BUILD_PYTHON` | OFF | Build Python bindings |
| `WITH_OPENCASCADE` | OFF | Enable CAD geometry support |
| `WITH_GMSH` | OFF | Enable GMSH meshing |

### Using as a Library

After installation, use in your CMake project:

```cmake
find_package(KooSim REQUIRED)
target_link_libraries(your_target PRIVATE KooSim::koo_sim)
```

Or without installation (FetchContent):

```cmake
include(FetchContent)
FetchContent_Declare(
    KooSim
    GIT_REPOSITORY https://github.com/yourusername/KooSimulationDataReader.git
    GIT_TAG main
)
FetchContent_MakeAvailable(KooSim)
target_link_libraries(your_target PRIVATE KooSim::koo_sim)
```

---

## Usage Example

```cpp
#include <koo/dyna/Model.hpp>
#include <koo/dyna/KeywordFileReader.hpp>
#include <koo/dyna/KeywordFileWriter.hpp>

int main() {
    using namespace koo::dyna;

    // Read K-file
    KeywordFileReader reader;
    Model model = reader.read("input.k");

    // Access data
    std::cout << "Nodes: " << model.nodes().size() << std::endl;
    std::cout << "Elements: " << model.elements().size() << std::endl;
    std::cout << "Parts: " << model.parts().size() << std::endl;

    // Modify model
    auto* node = model.nodes().get(1);
    node->setX(node->getX() + 10.0);

    // Add new material
    auto mat = std::make_unique<MatElastic>();
    mat->setId(100);
    mat->setDensity(7.85e-9);
    mat->setYoungsModulus(210000.0);
    mat->setPoissonsRatio(0.3);
    model.materials().add(std::move(mat));

    // Write K-file
    KeywordFileWriter writer;
    writer.write(model, "output.k");

    return 0;
}
```

---

## Timeline Estimate

| Phase | Duration | Cumulative |
|-------|----------|------------|
| Phase 1: Core Foundation | 2-3 months | 2-3 months |
| Phase 2: Complete Keywords | 4-6 months | 6-9 months |
| Phase 3: Algorithms | 2-3 months | 8-12 months |
| Phase 4: CAD Integration | 2-3 months | 10-15 months |
| Phase 5: Multi-Format | 2-3 months | 12-18 months |
| Phase 6: Advanced | Ongoing | 18+ months |

---

## Contributing

1. Fork the repository
2. Create feature branch (`git checkout -b feature/amazing-feature`)
3. Commit changes (`git commit -m 'Add amazing feature'`)
4. Push to branch (`git push origin feature/amazing-feature`)
5. Open Pull Request

---

## License

MIT License - see LICENSE file for details.

---

## Contact

- Project Lead: Koo
- Repository: https://github.com/yourusername/KooSimulationDataReader
