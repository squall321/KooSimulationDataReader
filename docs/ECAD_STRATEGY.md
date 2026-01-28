# ECAD Strategy - ODB++ Module

## Overview

ECAD(Electronic Computer-Aided Design) 모듈은 PCB(Printed Circuit Board) 설계 데이터를 완벽하게 관리하기 위한 모듈입니다. ODB++ 포맷을 핵심으로 하여, PCB 설계부터 제조, 검사, 시뮬레이션까지 전 과정을 지원합니다.

---

## ODB++ Format Overview

### What is ODB++?

ODB++는 Mentor Graphics(현 Siemens EDA)가 개발한 PCB 설계 데이터 교환 포맷으로, Gerber 대비 다음과 같은 장점을 가집니다:

- **Single Database**: 모든 PCB 정보가 하나의 데이터베이스에 통합
- **Intelligent Data**: 넷리스트, 컴포넌트, 레이어 스택업 등 설계 의도 포함
- **Manufacturing Ready**: DFM 정보, 테스트 포인트, 어셈블리 정보 포함
- **Industry Standard**: IPC-2581과 함께 업계 표준으로 자리잡음

### ODB++ Directory Structure

```
odb_job/
├── matrix/                     # 전체 레이어 매트릭스 정의
│   └── matrix                  # 레이어 순서, 타입, 속성
├── steps/                      # 스텝(설계 단위) 폴더
│   └── pcb/                    # 각 스텝 (panel, pcb, coupon 등)
│       ├── layers/             # 레이어 데이터
│       │   ├── top/            # Top copper layer
│       │   │   ├── features    # 피처 데이터 (압축)
│       │   │   └── attrlist    # 레이어 속성
│       │   ├── bot/            # Bottom copper layer
│       │   ├── smt/            # Soldermask top
│       │   ├── smb/            # Soldermask bottom
│       │   ├── sst/            # Silkscreen top
│       │   ├── ssb/            # Silkscreen bottom
│       │   ├── drill_1/        # Drill layers
│       │   └── ...
│       ├── eda/                # EDA 데이터
│       │   └── data            # 넷리스트, 컴포넌트 정보
│       ├── profile             # 보드 외곽선
│       ├── stephdr             # 스텝 헤더 정보
│       └── attrlist            # 스텝 속성
├── symbols/                    # 심볼 라이브러리
│   └── {symbol_name}/
│       └── features
├── fonts/                      # 폰트 정의
├── misc/                       # 기타 정보
│   ├── info                    # 작업 정보
│   └── attrlist                # 전역 속성
└── input/                      # 원본 입력 파일 (선택)
```

---

## Module Architecture

### Namespace Structure

```cpp
namespace koo {
namespace ecad {
    // Core classes
    class OdbJob;           // ODB++ 작업 전체 관리
    class Step;             // 스텝 (pcb, panel 등)
    class Layer;            // 레이어 데이터
    class Feature;          // 기본 피처 (라인, 패드, 아크 등)
    class Symbol;           // 심볼 정의
    class Net;              // 넷 정보
    class Component;        // 컴포넌트
    class Package;          // 패키지 정의

    // Layer types
    class CopperLayer;      // 동박 레이어
    class SolderMaskLayer;  // 솔더마스크
    class SilkscreenLayer;  // 실크스크린
    class DrillLayer;       // 드릴 레이어
    class DocumentLayer;    // 문서 레이어

    // Readers/Writers
    class OdbReader;        // ODB++ 읽기
    class OdbWriter;        // ODB++ 쓰기

    // Converters (Phase 2)
    class GerberConverter;  // Gerber ↔ ODB++
    class Ipc2581Converter; // IPC-2581 ↔ ODB++
} // namespace ecad
} // namespace koo
```

### Directory Structure

```
KooSimulationDataReader/
├── include/koo/
│   └── ecad/                       # ECAD public headers
│       ├── OdbJob.hpp
│       ├── Step.hpp
│       ├── Layer.hpp
│       ├── Feature.hpp
│       ├── Symbol.hpp
│       ├── Net.hpp
│       ├── Component.hpp
│       ├── Package.hpp
│       ├── layers/
│       │   ├── CopperLayer.hpp
│       │   ├── SolderMaskLayer.hpp
│       │   ├── DrillLayer.hpp
│       │   └── ...
│       ├── OdbReader.hpp
│       ├── OdbWriter.hpp
│       └── converters/
│           ├── GerberConverter.hpp
│           └── Ipc2581Converter.hpp
├── src/
│   └── ecad/                       # ECAD implementation
│       ├── CMakeLists.txt
│       ├── OdbJob.cpp
│       ├── Step.cpp
│       ├── Layer.cpp
│       ├── Feature.cpp
│       ├── OdbReader.cpp
│       ├── OdbWriter.cpp
│       └── ...
└── tests/
    └── ecad/                       # ECAD tests
        ├── TestOdbReader.cpp
        ├── TestOdbWriter.cpp
        └── test_data/
            └── sample.odb/
```

---

## Core Classes Design

### 1. OdbJob - 최상위 컨테이너

```cpp
namespace koo::ecad {

/**
 * @brief ODB++ 작업 전체를 관리하는 최상위 클래스
 *
 * ODB++ 디렉토리 구조 전체를 메모리에 로드하고 관리합니다.
 */
class KOO_API OdbJob {
public:
    // I/O
    static OdbJob load(const std::filesystem::path& odbPath);
    void save(const std::filesystem::path& odbPath) const;

    // Steps 관리
    Step* getStep(const std::string& name);
    const Step* getStep(const std::string& name) const;
    std::vector<std::string> getStepNames() const;
    Step& addStep(const std::string& name);
    void removeStep(const std::string& name);

    // Matrix (레이어 정의)
    const LayerMatrix& getMatrix() const;
    LayerMatrix& getMatrix();

    // Symbols 라이브러리
    Symbol* getSymbol(const std::string& name);
    const Symbol* getSymbol(const std::string& name) const;
    Symbol& addSymbol(const std::string& name);
    std::vector<std::string> getSymbolNames() const;

    // 전역 속성
    const AttributeList& getAttributes() const;
    void setAttribute(const std::string& key, const std::string& value);

    // 작업 정보
    const JobInfo& getInfo() const;
    void setInfo(const JobInfo& info);

private:
    std::string name_;
    LayerMatrix matrix_;
    std::unordered_map<std::string, std::unique_ptr<Step>> steps_;
    std::unordered_map<std::string, std::unique_ptr<Symbol>> symbols_;
    std::unordered_map<std::string, std::unique_ptr<Font>> fonts_;
    AttributeList attributes_;
    JobInfo info_;
};

} // namespace koo::ecad
```

### 2. Step - 설계 단위

```cpp
namespace koo::ecad {

/**
 * @brief 설계 단위 (PCB, Panel, Coupon 등)
 */
class KOO_API Step {
public:
    // 레이어 접근
    Layer* getLayer(const std::string& name);
    const Layer* getLayer(const std::string& name) const;
    std::vector<std::string> getLayerNames() const;

    // EDA 데이터 (넷리스트, 컴포넌트)
    const EdaData& getEdaData() const;
    EdaData& getEdaData();

    // 프로파일 (보드 외곽선)
    const Profile& getProfile() const;
    Profile& getProfile();

    // 속성
    const AttributeList& getAttributes() const;
    void setAttribute(const std::string& key, const std::string& value);

    // 스텝 정보
    StepType getType() const; // pcb, panel, array, coupon
    const BoundingBox& getDatum() const;

    // 반복 배열 (패널 내 PCB 배치)
    const std::vector<StepRepeat>& getRepeats() const;
    void addRepeat(const StepRepeat& repeat);

private:
    std::string name_;
    StepType type_;
    std::unordered_map<std::string, std::unique_ptr<Layer>> layers_;
    std::unique_ptr<EdaData> edaData_;
    Profile profile_;
    AttributeList attributes_;
    BoundingBox datum_;
    std::vector<StepRepeat> repeats_;
};

} // namespace koo::ecad
```

### 3. Layer - 레이어 데이터

```cpp
namespace koo::ecad {

/**
 * @brief 레이어 기본 클래스
 */
class KOO_API Layer {
public:
    virtual ~Layer() = default;

    // 피처 접근
    const std::vector<std::unique_ptr<Feature>>& getFeatures() const;
    void addFeature(std::unique_ptr<Feature> feature);
    void removeFeature(size_t index);
    void clearFeatures();

    // 레이어 정보
    const std::string& getName() const;
    LayerType getType() const;
    LayerContext getContext() const; // board, misc, document
    Polarity getPolarity() const;    // positive, negative

    // 속성
    const AttributeList& getAttributes() const;
    void setAttribute(const std::string& key, const std::string& value);

protected:
    std::string name_;
    LayerType type_;
    LayerContext context_;
    Polarity polarity_;
    std::vector<std::unique_ptr<Feature>> features_;
    AttributeList attributes_;
};

/**
 * @brief 동박 레이어
 */
class KOO_API CopperLayer : public Layer {
public:
    int getLayerNumber() const; // 레이어 순서 (1 = top, 2 = inner1, ...)
    double getThickness() const; // oz 또는 um

    // 동박 특화 기능
    std::vector<Feature*> getTracesOnNet(const std::string& netName) const;
    std::vector<Feature*> getPadsOnNet(const std::string& netName) const;

private:
    int layerNumber_;
    double thickness_;
};

/**
 * @brief 드릴 레이어
 */
class KOO_API DrillLayer : public Layer {
public:
    DrillType getDrillType() const; // plated, non-plated, via
    int getStartLayer() const;
    int getEndLayer() const;

    // 드릴 특화 기능
    std::vector<DrillHole> getDrillHoles() const;
    std::map<double, int> getDrillSizeHistogram() const;

private:
    DrillType drillType_;
    int startLayer_;
    int endLayer_;
};

} // namespace koo::ecad
```

### 4. Feature - 기본 그래픽 요소

```cpp
namespace koo::ecad {

/**
 * @brief 기본 피처 (ODB++ features 파일의 레코드)
 */
class KOO_API Feature {
public:
    virtual ~Feature() = default;
    virtual FeatureType getType() const = 0;
    virtual BoundingBox getBoundingBox() const = 0;
    virtual std::unique_ptr<Feature> clone() const = 0;

    // 공통 속성
    const std::string& getNetName() const { return netName_; }
    void setNetName(const std::string& net) { netName_ = net; }

    Polarity getPolarity() const { return polarity_; }
    void setPolarity(Polarity p) { polarity_ = p; }

    int getDcode() const { return dcode_; }

protected:
    std::string netName_;
    Polarity polarity_ = Polarity::Positive;
    int dcode_ = 0;
    AttributeList attributes_;
};

/**
 * @brief 라인 피처
 */
class KOO_API LineFeature : public Feature {
public:
    LineFeature(double x1, double y1, double x2, double y2,
                const std::string& symbol);

    FeatureType getType() const override { return FeatureType::Line; }
    BoundingBox getBoundingBox() const override;
    std::unique_ptr<Feature> clone() const override;

    Point2D getStart() const { return {x1_, y1_}; }
    Point2D getEnd() const { return {x2_, y2_}; }
    const std::string& getSymbol() const { return symbol_; }
    double getWidth() const;

private:
    double x1_, y1_, x2_, y2_;
    std::string symbol_; // 심볼 이름 (r50 = 50mil round 등)
};

/**
 * @brief 패드 피처
 */
class KOO_API PadFeature : public Feature {
public:
    PadFeature(double x, double y, const std::string& symbol,
               double rotation = 0.0, bool mirror = false);

    FeatureType getType() const override { return FeatureType::Pad; }
    BoundingBox getBoundingBox() const override;
    std::unique_ptr<Feature> clone() const override;

    Point2D getPosition() const { return {x_, y_}; }
    const std::string& getSymbol() const { return symbol_; }
    double getRotation() const { return rotation_; }
    bool isMirrored() const { return mirror_; }

private:
    double x_, y_;
    std::string symbol_;
    double rotation_;
    bool mirror_;
};

/**
 * @brief 아크 피처
 */
class KOO_API ArcFeature : public Feature {
public:
    ArcFeature(double xs, double ys, double xe, double ye,
               double xc, double yc, const std::string& symbol,
               bool clockwise = false);

    FeatureType getType() const override { return FeatureType::Arc; }
    BoundingBox getBoundingBox() const override;
    std::unique_ptr<Feature> clone() const override;

    Point2D getStart() const { return {xs_, ys_}; }
    Point2D getEnd() const { return {xe_, ye_}; }
    Point2D getCenter() const { return {xc_, yc_}; }
    double getRadius() const;
    bool isClockwise() const { return clockwise_; }

private:
    double xs_, ys_, xe_, ye_, xc_, yc_;
    std::string symbol_;
    bool clockwise_;
};

/**
 * @brief 서피스 피처 (폴리곤 영역)
 */
class KOO_API SurfaceFeature : public Feature {
public:
    FeatureType getType() const override { return FeatureType::Surface; }
    BoundingBox getBoundingBox() const override;
    std::unique_ptr<Feature> clone() const override;

    void addContour(const Contour& contour);
    const std::vector<Contour>& getContours() const { return contours_; }

    // 첫 번째 contour는 외곽, 나머지는 holes
    const Contour& getOutline() const { return contours_[0]; }
    std::vector<Contour> getHoles() const;

    double getArea() const;
    bool contains(double x, double y) const;

private:
    std::vector<Contour> contours_;
};

/**
 * @brief 텍스트 피처
 */
class KOO_API TextFeature : public Feature {
public:
    TextFeature(double x, double y, const std::string& text,
                const std::string& font, double height);

    FeatureType getType() const override { return FeatureType::Text; }
    BoundingBox getBoundingBox() const override;
    std::unique_ptr<Feature> clone() const override;

    Point2D getPosition() const { return {x_, y_}; }
    const std::string& getText() const { return text_; }
    const std::string& getFont() const { return font_; }
    double getHeight() const { return height_; }
    double getRotation() const { return rotation_; }

private:
    double x_, y_;
    std::string text_;
    std::string font_;
    double height_;
    double rotation_ = 0.0;
};

} // namespace koo::ecad
```

### 5. EDA Data - 넷리스트 및 컴포넌트

```cpp
namespace koo::ecad {

/**
 * @brief 컴포넌트 (부품)
 */
class KOO_API Component {
public:
    const std::string& getRefDes() const { return refDes_; }
    const std::string& getPartNumber() const { return partNumber_; }
    const std::string& getPackage() const { return package_; }
    Point2D getPosition() const { return {x_, y_}; }
    double getRotation() const { return rotation_; }
    Side getSide() const { return side_; } // top, bottom

    // 핀 접근
    const std::vector<Pin>& getPins() const { return pins_; }
    const Pin* getPin(const std::string& pinName) const;

    // BOM 속성
    const std::string& getValue() const { return value_; }
    const std::string& getDescription() const { return description_; }

private:
    std::string refDes_;      // U1, R1, C1 등
    std::string partNumber_;
    std::string package_;
    double x_, y_;
    double rotation_;
    Side side_;
    std::vector<Pin> pins_;
    std::string value_;
    std::string description_;
    AttributeList attributes_;
};

/**
 * @brief 핀 정보
 */
struct Pin {
    std::string name;       // 핀 이름 (1, 2, A1, VCC 등)
    std::string netName;    // 연결된 넷
    double x, y;            // 컴포넌트 기준 상대 좌표
    PinType type;           // through_hole, smd, bga
    double padSize;
};

/**
 * @brief 넷 (전기적 연결)
 */
class KOO_API Net {
public:
    const std::string& getName() const { return name_; }

    // 연결된 핀 목록
    const std::vector<PinRef>& getPins() const { return pins_; }
    void addPin(const PinRef& pin);

    // 넷 속성
    NetClass getNetClass() const { return netClass_; }
    bool isPower() const;
    bool isGround() const;

private:
    std::string name_;
    std::vector<PinRef> pins_;
    NetClass netClass_;
    AttributeList attributes_;
};

struct PinRef {
    std::string refDes;     // 컴포넌트 참조
    std::string pinName;    // 핀 이름
};

/**
 * @brief EDA 데이터 컨테이너
 */
class KOO_API EdaData {
public:
    // 컴포넌트 접근
    Component* getComponent(const std::string& refDes);
    const Component* getComponent(const std::string& refDes) const;
    std::vector<std::string> getComponentRefDes() const;
    void addComponent(std::unique_ptr<Component> comp);

    // 넷 접근
    Net* getNet(const std::string& name);
    const Net* getNet(const std::string& name) const;
    std::vector<std::string> getNetNames() const;
    void addNet(std::unique_ptr<Net> net);

    // 패키지 라이브러리
    Package* getPackage(const std::string& name);
    const Package* getPackage(const std::string& name) const;

    // 통계
    size_t getComponentCount() const { return components_.size(); }
    size_t getNetCount() const { return nets_.size(); }
    size_t getPinCount() const;

private:
    std::unordered_map<std::string, std::unique_ptr<Component>> components_;
    std::unordered_map<std::string, std::unique_ptr<Net>> nets_;
    std::unordered_map<std::string, std::unique_ptr<Package>> packages_;
};

} // namespace koo::ecad
```

---

## Reader/Writer Design

### ODB++ Reader

```cpp
namespace koo::ecad {

/**
 * @brief ODB++ 디렉토리 파서
 */
class KOO_API OdbReader {
public:
    struct Options {
        bool loadFeatures = true;       // 피처 데이터 로드
        bool loadEdaData = true;        // EDA 데이터 로드
        bool loadSymbols = true;        // 심볼 라이브러리 로드
        bool decompressFeatures = true; // 압축 해제 (*.z 파일)
        std::vector<std::string> stepFilter; // 특정 스텝만 로드
        std::vector<std::string> layerFilter; // 특정 레이어만 로드
    };

    OdbJob read(const std::filesystem::path& odbPath);
    OdbJob read(const std::filesystem::path& odbPath, const Options& options);

    // 부분 로드
    LayerMatrix readMatrix(const std::filesystem::path& odbPath);
    std::vector<std::string> listSteps(const std::filesystem::path& odbPath);
    Step readStep(const std::filesystem::path& odbPath, const std::string& stepName);
    Layer readLayer(const std::filesystem::path& odbPath,
                   const std::string& stepName, const std::string& layerName);

private:
    void parseMatrix(OdbJob& job, const std::filesystem::path& matrixPath);
    void parseStep(Step& step, const std::filesystem::path& stepPath);
    void parseLayer(Layer& layer, const std::filesystem::path& layerPath);
    void parseFeatures(Layer& layer, const std::filesystem::path& featuresPath);
    void parseEdaData(EdaData& eda, const std::filesystem::path& edaPath);
    void parseSymbol(Symbol& symbol, const std::filesystem::path& symbolPath);

    // Features 파일 파싱
    std::unique_ptr<Feature> parseFeatureLine(const std::string& line);
    std::unique_ptr<LineFeature> parseLine(const std::vector<std::string>& tokens);
    std::unique_ptr<PadFeature> parsePad(const std::vector<std::string>& tokens);
    std::unique_ptr<ArcFeature> parseArc(const std::vector<std::string>& tokens);
    std::unique_ptr<SurfaceFeature> parseSurface(std::istream& stream);

    // 압축 해제
    std::string decompressFile(const std::filesystem::path& compressedPath);
};

} // namespace koo::ecad
```

### ODB++ Writer

```cpp
namespace koo::ecad {

/**
 * @brief ODB++ 디렉토리 출력
 */
class KOO_API OdbWriter {
public:
    struct Options {
        bool compressFeatures = true;   // *.z 압축
        int compressionLevel = 6;       // zlib 압축 레벨 (1-9)
        bool includeInputFiles = false; // input/ 폴더 포함
        OdbVersion version = OdbVersion::V8_1;
    };

    void write(const OdbJob& job, const std::filesystem::path& odbPath);
    void write(const OdbJob& job, const std::filesystem::path& odbPath,
               const Options& options);

private:
    void writeMatrix(const LayerMatrix& matrix, const std::filesystem::path& matrixPath);
    void writeStep(const Step& step, const std::filesystem::path& stepPath);
    void writeLayer(const Layer& layer, const std::filesystem::path& layerPath);
    void writeFeatures(const std::vector<std::unique_ptr<Feature>>& features,
                      const std::filesystem::path& featuresPath, bool compress);
    void writeEdaData(const EdaData& eda, const std::filesystem::path& edaPath);
    void writeSymbol(const Symbol& symbol, const std::filesystem::path& symbolPath);

    // Feature 직렬화
    std::string serializeFeature(const Feature& feature) const;
    std::string serializeLine(const LineFeature& line) const;
    std::string serializePad(const PadFeature& pad) const;
    std::string serializeArc(const ArcFeature& arc) const;
    std::string serializeSurface(const SurfaceFeature& surface) const;

    // 압축
    void compressFile(const std::filesystem::path& inputPath,
                     const std::filesystem::path& outputPath);
};

} // namespace koo::ecad
```

---

## Integration with CAE

### PCB to FEA Mesh

```cpp
namespace koo::ecad {

/**
 * @brief PCB 모델을 FEA 메쉬로 변환
 *
 * PCB의 레이어 스택업을 3D 솔리드 메쉬로 변환하여
 * 열해석, 구조해석에 활용
 */
class KOO_API PcbToMeshConverter {
public:
    struct Options {
        double meshSize = 0.5;          // mm
        bool includeCopper = true;       // 동박 포함
        bool includeSolderMask = true;   // 솔더마스크 포함
        bool includeSubstrate = true;    // FR4 기판 포함
        bool mergeSmallFeatures = true;  // 작은 피처 병합
        double featureMergeThreshold = 0.1; // mm
    };

    dyna::Model convert(const OdbJob& job, const std::string& stepName);
    dyna::Model convert(const OdbJob& job, const std::string& stepName,
                       const Options& options);

    // 개별 레이어 변환
    std::vector<dyna::Element*> convertLayer(const Layer& layer,
                                             double zBottom, double zTop);

    // 스택업 기반 3D 모델 생성
    void buildStackup(dyna::Model& model, const OdbJob& job,
                     const Stackup& stackup);

private:
    // 폴리곤 메쉬화
    void meshPolygon(const SurfaceFeature& surface,
                    std::vector<dyna::Node>& nodes,
                    std::vector<dyna::ElementShell>& elements,
                    double z);

    // 비아 생성
    void createVia(dyna::Model& model, double x, double y,
                  double diameter, int startLayer, int endLayer);
};

} // namespace koo::ecad
```

---

## Modular Design Principles

### 1. Independence from CAE Module

ECAD 모듈은 CAE(dyna) 모듈과 독립적으로 동작합니다:

```cpp
// ECAD만 사용 - CAE 의존성 없음
#include <koo/ecad/OdbJob.hpp>
#include <koo/ecad/OdbReader.hpp>

koo::ecad::OdbReader reader;
auto job = reader.read("pcb_design.odb");

// CAE 통합이 필요한 경우만 별도 헤더 포함
#include <koo/ecad/integration/PcbToMeshConverter.hpp>
```

### 2. Plugin Architecture

새로운 포맷 지원을 플러그인으로 추가 가능:

```cpp
// 포맷 어댑터 인터페이스
class EcadFormatAdapter {
public:
    virtual ~EcadFormatAdapter() = default;
    virtual OdbJob import(const std::filesystem::path& path) = 0;
    virtual void export_(const OdbJob& job, const std::filesystem::path& path) = 0;
    virtual std::string getFormatName() const = 0;
    virtual std::vector<std::string> getExtensions() const = 0;
};

// Gerber 어댑터 (별도 모듈)
class GerberAdapter : public EcadFormatAdapter {
    OdbJob import(const std::filesystem::path& path) override;
    // Gerber 파일들을 ODB++ 구조로 변환
};

// IPC-2581 어댑터 (별도 모듈)
class Ipc2581Adapter : public EcadFormatAdapter {
    OdbJob import(const std::filesystem::path& path) override;
    void export_(const OdbJob& job, const std::filesystem::path& path) override;
};
```

### 3. CMake Configuration

```cmake
# src/ecad/CMakeLists.txt

# Core ECAD library - no dependencies on dyna module
add_library(koo_ecad
    OdbJob.cpp
    Step.cpp
    Layer.cpp
    Feature.cpp
    OdbReader.cpp
    OdbWriter.cpp
    # ...
)

target_include_directories(koo_ecad PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Optional: ECAD-CAE integration (depends on koo_dyna)
if(BUILD_ECAD_CAE_INTEGRATION)
    add_library(koo_ecad_cae_integration
        integration/PcbToMeshConverter.cpp
    )
    target_link_libraries(koo_ecad_cae_integration
        PUBLIC koo_ecad koo_dyna
    )
endif()

# Optional: Gerber converter
if(WITH_GERBER_SUPPORT)
    add_library(koo_ecad_gerber
        converters/GerberConverter.cpp
    )
    target_link_libraries(koo_ecad_gerber PUBLIC koo_ecad)
endif()
```

---

## Development Phases

### Phase 1: Core ODB++ Support
- [ ] ODB++ 디렉토리 구조 파싱
- [ ] Matrix 파일 읽기/쓰기
- [ ] Features 파일 파싱 (Line, Pad, Arc, Surface)
- [ ] 기본 심볼 지원 (round, rect, oblong)
- [ ] 압축/압축해제 (zlib)
- [ ] 단위 테스트

### Phase 2: EDA Data Support
- [ ] EDA/data 파일 파싱
- [ ] 컴포넌트 정보 추출
- [ ] 넷리스트 추출
- [ ] 패키지 라이브러리
- [ ] BOM 생성

### Phase 3: Advanced Features
- [ ] 복잡한 심볼 (user-defined)
- [ ] 폰트 렌더링
- [ ] Step & Repeat 지원
- [ ] 속성 완전 지원

### Phase 4: Format Converters
- [ ] Gerber → ODB++ 변환
- [ ] ODB++ → Gerber 변환
- [ ] IPC-2581 지원
- [ ] Excellon 드릴 파일

### Phase 5: CAE Integration
- [ ] PCB 3D 모델 생성
- [ ] 열해석용 메쉬 변환
- [ ] 구조해석용 메쉬 변환
- [ ] 전자기해석 연동

---

## References

- [ODB++ Specification](https://odbplusplus.com/)
- [IPC-2581](https://www.ipc.org/ipc-2581)
- [Gerber Format](https://www.ucamco.com/gerber)
