# MCAD Strategy - STEP/CAD Module

## Overview

MCAD(Mechanical Computer-Aided Design) 모듈은 기계 설계 CAD 데이터를 고차원적으로 관리하기 위한 모듈입니다. STEP(ISO 10303)을 핵심 포맷으로 하여, 형상 데이터뿐만 아니라 어셈블리 구조, PMI(Product Manufacturing Information), 재료 정보 등 설계 의도를 완벽히 보존하는 데이터 구조를 제공합니다.

---

## STEP Format Overview

### What is STEP?

STEP(Standard for the Exchange of Product Data)은 ISO 10303 표준으로, CAD 데이터 교환의 국제 표준입니다:

- **AP203**: Configuration Controlled 3D Design (형상 + 어셈블리)
- **AP214**: Core Data for Automotive Mechanical Design Processes
- **AP242**: Managed Model-based 3D Engineering (PMI, GD&T 포함)
- **AP243**: MoSSEC - Model-based 3D Engineering for Simulation

### STEP vs Other Formats

| Format | Geometry | Assembly | PMI | Materials | Metadata |
|--------|----------|----------|-----|-----------|----------|
| **STEP AP242** | ✅ | ✅ | ✅ | ✅ | ✅ |
| IGES | ✅ | ❌ | ❌ | ❌ | ❌ |
| Parasolid | ✅ | ✅ | ❌ | ❌ | ⚠️ |
| JT | ✅ | ✅ | ✅ | ⚠️ | ✅ |
| ACIS | ✅ | ⚠️ | ❌ | ❌ | ❌ |

---

## Module Architecture

### High-Level Data Model

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                           MCAD Data Model                                    │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                              │
│  ┌─────────────────────────────────────────────────────────────────────┐    │
│  │                        Product Structure                             │    │
│  │  ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌──────────┐            │    │
│  │  │ Product  │──│ Assembly │──│  Part    │──│  Body    │            │    │
│  │  └──────────┘  └──────────┘  └──────────┘  └──────────┘            │    │
│  │       │             │             │             │                   │    │
│  │       │        ┌────┴────┐   ┌────┴────┐   ┌────┴────┐             │    │
│  │       │        │Instance │   │Features │   │  Shape  │             │    │
│  │       │        │Transform│   │ (PMI)   │   │  (BRep) │             │    │
│  │       │        └─────────┘   └─────────┘   └─────────┘             │    │
│  └─────────────────────────────────────────────────────────────────────┘    │
│                                                                              │
│  ┌─────────────────────────────────────────────────────────────────────┐    │
│  │                        Geometry Kernel                               │    │
│  │  ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌──────────┐            │    │
│  │  │  Solid   │  │  Shell   │  │   Face   │  │   Edge   │            │    │
│  │  │  (BRep)  │  │          │  │ (Surface)│  │  (Curve) │            │    │
│  │  └──────────┘  └──────────┘  └──────────┘  └──────────┘            │    │
│  │                                                                      │    │
│  │  ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌──────────┐            │    │
│  │  │  Vertex  │  │  Loop    │  │  Wire    │  │ Compound │            │    │
│  │  │  (Point) │  │          │  │          │  │          │            │    │
│  │  └──────────┘  └──────────┘  └──────────┘  └──────────┘            │    │
│  └─────────────────────────────────────────────────────────────────────┘    │
│                                                                              │
│  ┌─────────────────────────────────────────────────────────────────────┐    │
│  │                     Engineering Attributes                           │    │
│  │  ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌──────────┐            │    │
│  │  │ Material │  │   PMI    │  │ Metadata │  │Properties│            │    │
│  │  │          │  │ (GD&T)   │  │          │  │          │            │    │
│  │  └──────────┘  └──────────┘  └──────────┘  └──────────┘            │    │
│  └─────────────────────────────────────────────────────────────────────┘    │
│                                                                              │
└─────────────────────────────────────────────────────────────────────────────┘
```

### Namespace Structure

```cpp
namespace koo {
namespace mcad {
    // Product Structure
    class Product;          // 최상위 제품
    class Assembly;         // 어셈블리
    class Part;             // 파트 (단일 부품)
    class Instance;         // 인스턴스 (배치 정보)

    // Geometry (OpenCASCADE 래핑)
    class Shape;            // 기본 형상 클래스
    class Solid;            // 솔리드
    class Shell;            // 셸 (면의 집합)
    class Face;             // 면
    class Edge;             // 에지
    class Vertex;           // 정점
    class Wire;             // 와이어 (에지의 집합)
    class Compound;         // 복합 형상

    // Surfaces
    class Surface;          // 서피스 기본
    class Plane;            // 평면
    class Cylinder;         // 원통면
    class Cone;             // 원뿔면
    class Sphere;           // 구면
    class Torus;            // 토러스
    class BSplineSurface;   // B-스플라인 서피스
    class NurbsSurface;     // NURBS 서피스

    // Curves
    class Curve;            // 커브 기본
    class Line;             // 직선
    class Circle;           // 원
    class Ellipse;          // 타원
    class BSplineCurve;     // B-스플라인 커브
    class NurbsCurve;       // NURBS 커브

    // Engineering Data
    class Material;         // 재료 정보
    class PMI;              // Product Manufacturing Information
    class GDT;              // Geometric Dimensioning & Tolerancing
    class Annotation;       // 주석

    // I/O
    class StepReader;       // STEP 파일 읽기
    class StepWriter;       // STEP 파일 쓰기
    class IgesReader;       // IGES 파일 읽기
    class IgesWriter;       // IGES 파일 쓰기

} // namespace mcad
} // namespace koo
```

### Directory Structure

```
KooSimulationDataReader/
├── include/koo/
│   └── mcad/                       # MCAD public headers
│       ├── Product.hpp
│       ├── Assembly.hpp
│       ├── Part.hpp
│       ├── Instance.hpp
│       ├── geometry/
│       │   ├── Shape.hpp
│       │   ├── Solid.hpp
│       │   ├── Shell.hpp
│       │   ├── Face.hpp
│       │   ├── Edge.hpp
│       │   ├── Vertex.hpp
│       │   ├── surfaces/
│       │   │   ├── Surface.hpp
│       │   │   ├── Plane.hpp
│       │   │   ├── Cylinder.hpp
│       │   │   └── ...
│       │   └── curves/
│       │       ├── Curve.hpp
│       │       ├── Line.hpp
│       │       └── ...
│       ├── engineering/
│       │   ├── Material.hpp
│       │   ├── PMI.hpp
│       │   └── GDT.hpp
│       ├── StepReader.hpp
│       ├── StepWriter.hpp
│       ├── IgesReader.hpp
│       └── IgesWriter.hpp
├── src/
│   └── mcad/                       # MCAD implementation
│       ├── CMakeLists.txt
│       ├── Product.cpp
│       ├── Assembly.cpp
│       ├── geometry/
│       │   └── ...
│       ├── StepReader.cpp
│       └── ...
└── tests/
    └── mcad/                       # MCAD tests
        ├── TestStepReader.cpp
        ├── TestGeometry.cpp
        └── test_data/
            ├── simple_box.step
            ├── assembly.step
            └── ...
```

---

## Core Classes Design

### 1. Product - 최상위 컨테이너

```cpp
namespace koo::mcad {

/**
 * @brief 최상위 제품 구조
 *
 * STEP 파일의 전체 제품 구조를 관리합니다.
 * 어셈블리 계층, 파트, 재료 정보 등을 포함합니다.
 */
class KOO_API Product {
public:
    // I/O
    static Product loadStep(const std::filesystem::path& stepPath);
    static Product loadIges(const std::filesystem::path& igesPath);
    void saveStep(const std::filesystem::path& stepPath,
                 StepVersion version = StepVersion::AP242) const;
    void saveIges(const std::filesystem::path& igesPath) const;

    // 제품 정보
    const std::string& getName() const { return name_; }
    void setName(const std::string& name) { name_ = name; }
    const std::string& getId() const { return id_; }

    // 루트 어셈블리/파트 접근
    Assembly* getRootAssembly();
    const Assembly* getRootAssembly() const;
    bool hasAssembly() const { return rootAssembly_ != nullptr; }

    // 단일 파트인 경우
    Part* getRootPart();
    const Part* getRootPart() const;

    // 모든 파트 (플랫 리스트)
    std::vector<Part*> getAllParts();
    std::vector<const Part*> getAllParts() const;
    Part* findPart(const std::string& name);

    // 모든 인스턴스 (플랫 리스트)
    std::vector<Instance*> getAllInstances();

    // 재료 라이브러리
    Material* getMaterial(const std::string& name);
    const Material* getMaterial(const std::string& name) const;
    void addMaterial(std::unique_ptr<Material> material);
    std::vector<std::string> getMaterialNames() const;

    // 전역 속성
    const std::map<std::string, std::string>& getProperties() const;
    void setProperty(const std::string& key, const std::string& value);

    // 바운딩 박스 (전체 제품)
    BoundingBox getBoundingBox() const;

    // 통계
    ProductStatistics getStatistics() const;

private:
    std::string name_;
    std::string id_;
    std::unique_ptr<Assembly> rootAssembly_;
    std::unique_ptr<Part> rootPart_; // 어셈블리 없이 단일 파트인 경우
    std::unordered_map<std::string, std::unique_ptr<Material>> materials_;
    std::map<std::string, std::string> properties_;
};

struct ProductStatistics {
    size_t assemblyCount;
    size_t partCount;
    size_t instanceCount;
    size_t faceCount;
    size_t edgeCount;
    size_t vertexCount;
    double totalVolume;
    double totalSurfaceArea;
};

} // namespace koo::mcad
```

### 2. Assembly - 어셈블리 구조

```cpp
namespace koo::mcad {

/**
 * @brief 어셈블리 (부품들의 계층 구조)
 */
class KOO_API Assembly {
public:
    // 기본 정보
    const std::string& getName() const { return name_; }
    void setName(const std::string& name) { name_ = name; }
    const std::string& getId() const { return id_; }

    // 하위 어셈블리
    std::vector<Assembly*> getSubAssemblies();
    std::vector<const Assembly*> getSubAssemblies() const;
    Assembly& addSubAssembly(const std::string& name);
    void removeSubAssembly(const std::string& name);

    // 파트 인스턴스
    std::vector<Instance*> getInstances();
    std::vector<const Instance*> getInstances() const;
    Instance& addInstance(Part* part, const Transform& transform);
    Instance& addInstance(Part* part, const std::string& instanceName,
                         const Transform& transform);
    void removeInstance(const std::string& instanceName);

    // 부모
    Assembly* getParent() { return parent_; }
    const Assembly* getParent() const { return parent_; }
    bool isRoot() const { return parent_ == nullptr; }

    // 계층 순회
    void traverse(std::function<void(Assembly&)> visitor);
    void traverse(std::function<void(const Assembly&)> visitor) const;

    // 전체 변환 (루트부터 현재까지)
    Transform getGlobalTransform() const;

    // 바운딩 박스
    BoundingBox getBoundingBox() const;
    BoundingBox getGlobalBoundingBox() const;

    // 속성
    const std::map<std::string, std::string>& getProperties() const;
    void setProperty(const std::string& key, const std::string& value);

private:
    std::string name_;
    std::string id_;
    Assembly* parent_ = nullptr;
    std::vector<std::unique_ptr<Assembly>> subAssemblies_;
    std::vector<std::unique_ptr<Instance>> instances_;
    std::map<std::string, std::string> properties_;
};

} // namespace koo::mcad
```

### 3. Part - 파트 (단일 부품)

```cpp
namespace koo::mcad {

/**
 * @brief 파트 - 단일 부품의 형상 및 속성
 */
class KOO_API Part {
public:
    // 기본 정보
    const std::string& getName() const { return name_; }
    void setName(const std::string& name) { name_ = name; }
    const std::string& getId() const { return id_; }
    const std::string& getPartNumber() const { return partNumber_; }
    void setPartNumber(const std::string& pn) { partNumber_ = pn; }

    // 형상 (Body)
    Shape* getShape();
    const Shape* getShape() const;
    void setShape(std::unique_ptr<Shape> shape);

    // 여러 바디를 가진 경우
    std::vector<Solid*> getSolids();
    std::vector<const Solid*> getSolids() const;
    void addSolid(std::unique_ptr<Solid> solid);

    // 형상 분석
    std::vector<Face*> getFaces();
    std::vector<Edge*> getEdges();
    std::vector<Vertex*> getVertices();

    // 면 필터링
    std::vector<Face*> getPlanarFaces();
    std::vector<Face*> getCylindricalFaces();
    std::vector<Face*> getFacesByColor(const Color& color);

    // 재료
    Material* getMaterial() { return material_; }
    const Material* getMaterial() const { return material_; }
    void setMaterial(Material* material) { material_ = material; }

    // PMI (제조 정보)
    std::vector<PMI*> getPMI();
    std::vector<const PMI*> getPMI() const;
    void addPMI(std::unique_ptr<PMI> pmi);

    // 기하 속성
    BoundingBox getBoundingBox() const;
    double getVolume() const;
    double getSurfaceArea() const;
    Point3D getCenterOfMass() const;
    Matrix3x3 getInertiaMatrix() const;

    // 렌더링 속성
    const Color& getColor() const { return color_; }
    void setColor(const Color& color) { color_ = color; }
    double getTransparency() const { return transparency_; }
    void setTransparency(double t) { transparency_ = t; }

    // 속성
    const std::map<std::string, std::string>& getProperties() const;
    void setProperty(const std::string& key, const std::string& value);

private:
    std::string name_;
    std::string id_;
    std::string partNumber_;
    std::unique_ptr<Shape> shape_;
    std::vector<std::unique_ptr<Solid>> solids_;
    Material* material_ = nullptr;
    std::vector<std::unique_ptr<PMI>> pmiList_;
    Color color_;
    double transparency_ = 0.0;
    std::map<std::string, std::string> properties_;
};

} // namespace koo::mcad
```

### 4. Instance - 인스턴스 (배치 정보)

```cpp
namespace koo::mcad {

/**
 * @brief 파트의 인스턴스 (위치/방향 정보 포함)
 *
 * 동일한 Part를 여러 위치에 배치할 때 사용
 */
class KOO_API Instance {
public:
    // 기본 정보
    const std::string& getName() const { return name_; }
    void setName(const std::string& name) { name_ = name; }

    // 참조하는 파트
    Part* getPart() { return part_; }
    const Part* getPart() const { return part_; }

    // 변환 (로컬)
    const Transform& getTransform() const { return transform_; }
    void setTransform(const Transform& t) { transform_ = t; }

    // 개별 변환 요소
    Point3D getPosition() const;
    void setPosition(const Point3D& pos);
    Matrix3x3 getRotation() const;
    void setRotation(const Matrix3x3& rot);
    void setRotation(double rx, double ry, double rz); // Euler angles

    // 전역 변환 (어셈블리 계층 고려)
    Transform getGlobalTransform() const;

    // 바운딩 박스 (변환 적용)
    BoundingBox getBoundingBox() const;

    // 부모 어셈블리
    Assembly* getParentAssembly() { return parent_; }
    const Assembly* getParentAssembly() const { return parent_; }

    // 가시성
    bool isVisible() const { return visible_; }
    void setVisible(bool v) { visible_ = v; }

    // 오버라이드 속성 (인스턴스별 속성)
    const Color& getColorOverride() const { return colorOverride_; }
    void setColorOverride(const Color& color) { colorOverride_ = color; }
    bool hasColorOverride() const { return hasColorOverride_; }

private:
    std::string name_;
    Part* part_;
    Assembly* parent_;
    Transform transform_;
    bool visible_ = true;
    Color colorOverride_;
    bool hasColorOverride_ = false;
};

} // namespace koo::mcad
```

### 5. Shape - 형상 기본 클래스

```cpp
namespace koo::mcad {

/**
 * @brief 형상 기본 클래스 (OpenCASCADE TopoDS_Shape 래퍼)
 */
class KOO_API Shape {
public:
    virtual ~Shape() = default;

    // 타입
    virtual ShapeType getType() const = 0;

    // 기하 속성
    virtual BoundingBox getBoundingBox() const = 0;
    virtual bool isValid() const = 0;
    virtual bool isNull() const = 0;

    // 토폴로지 쿼리
    virtual std::vector<Face*> getFaces() = 0;
    virtual std::vector<Edge*> getEdges() = 0;
    virtual std::vector<Vertex*> getVertices() = 0;

    // 변환
    virtual void transform(const Transform& t) = 0;
    virtual std::unique_ptr<Shape> transformed(const Transform& t) const = 0;

    // 복제
    virtual std::unique_ptr<Shape> clone() const = 0;

    // OpenCASCADE 접근 (고급 사용자용)
    virtual void* getNativeHandle() = 0;
    virtual const void* getNativeHandle() const = 0;

protected:
    // OpenCASCADE shape 저장
    // TopoDS_Shape occShape_;
};

/**
 * @brief 솔리드 (닫힌 3D 형상)
 */
class KOO_API Solid : public Shape {
public:
    ShapeType getType() const override { return ShapeType::Solid; }

    // 솔리드 특화
    double getVolume() const;
    double getSurfaceArea() const;
    Point3D getCenterOfMass() const;
    Matrix3x3 getInertiaMatrix() const;

    // 셸 접근
    std::vector<Shell*> getShells();

    // 불리언 연산
    std::unique_ptr<Solid> fuse(const Solid& other) const;
    std::unique_ptr<Solid> cut(const Solid& other) const;
    std::unique_ptr<Solid> common(const Solid& other) const;

    // 필렛/챔퍼
    std::unique_ptr<Solid> fillet(const std::vector<Edge*>& edges, double radius) const;
    std::unique_ptr<Solid> chamfer(const std::vector<Edge*>& edges, double distance) const;
};

/**
 * @brief 면 (서피스로 정의된 영역)
 */
class KOO_API Face : public Shape {
public:
    ShapeType getType() const override { return ShapeType::Face; }

    // 서피스 접근
    Surface* getSurface();
    const Surface* getSurface() const;
    SurfaceType getSurfaceType() const;

    // 면 특화
    double getArea() const;
    Vector3D getNormal(double u, double v) const;
    Vector3D getNormalAtCenter() const;
    Point3D getCentroid() const;

    // 경계 와이어
    std::vector<Wire*> getWires();
    Wire* getOuterWire();
    std::vector<Wire*> getInnerWires(); // 구멍

    // 에지
    std::vector<Edge*> getEdges();

    // UV 파라미터
    std::pair<double, double> getUVBounds() const;
    Point3D evaluate(double u, double v) const;

    // 속성
    bool isPlane() const;
    bool isCylinder() const;
    bool isCone() const;
    bool isSphere() const;

    // 렌더링
    const Color& getColor() const { return color_; }
    void setColor(const Color& color) { color_ = color; }

private:
    Color color_;
};

/**
 * @brief 에지 (커브로 정의된 선)
 */
class KOO_API Edge : public Shape {
public:
    ShapeType getType() const override { return ShapeType::Edge; }

    // 커브 접근
    Curve* getCurve();
    const Curve* getCurve() const;
    CurveType getCurveType() const;

    // 에지 특화
    double getLength() const;
    Point3D getStartPoint() const;
    Point3D getEndPoint() const;
    Point3D getMidPoint() const;

    // 정점
    Vertex* getStartVertex();
    Vertex* getEndVertex();

    // 인접 면
    std::vector<Face*> getAdjacentFaces();

    // 파라미터
    std::pair<double, double> getParameterBounds() const;
    Point3D evaluate(double t) const;
    Vector3D getTangent(double t) const;

    // 속성
    bool isLine() const;
    bool isCircle() const;
    bool isEllipse() const;
    bool isSeam() const; // 솔기 에지 (주기적 면에서)
};

} // namespace koo::mcad
```

### 6. Material - 재료 정보

```cpp
namespace koo::mcad {

/**
 * @brief 재료 정보
 *
 * STEP AP242에서 정의된 재료 속성을 저장
 */
class KOO_API Material {
public:
    // 기본 정보
    const std::string& getName() const { return name_; }
    void setName(const std::string& name) { name_ = name; }
    const std::string& getDescription() const { return description_; }

    // 물리적 속성
    double getDensity() const { return density_; } // kg/m³
    void setDensity(double d) { density_ = d; }

    // 기계적 속성
    double getYoungsModulus() const { return youngsModulus_; } // Pa
    void setYoungsModulus(double e) { youngsModulus_ = e; }
    double getPoissonsRatio() const { return poissonsRatio_; }
    void setPoissonsRatio(double nu) { poissonsRatio_ = nu; }
    double getYieldStrength() const { return yieldStrength_; } // Pa
    void setYieldStrength(double ys) { yieldStrength_ = ys; }
    double getUltimateStrength() const { return ultimateStrength_; } // Pa
    void setUltimateStrength(double us) { ultimateStrength_ = us; }

    // 열적 속성
    double getThermalConductivity() const { return thermalConductivity_; } // W/(m·K)
    void setThermalConductivity(double k) { thermalConductivity_ = k; }
    double getSpecificHeat() const { return specificHeat_; } // J/(kg·K)
    void setSpecificHeat(double cp) { specificHeat_ = cp; }
    double getThermalExpansion() const { return thermalExpansion_; } // 1/K
    void setThermalExpansion(double alpha) { thermalExpansion_ = alpha; }

    // 렌더링
    const Color& getColor() const { return color_; }
    void setColor(const Color& color) { color_ = color; }

    // CAE 재료 매핑
    // LS-DYNA 재료로 변환 시 사용
    std::string suggestDynaMaterial() const;
    std::map<std::string, double> getDynaParameters() const;

private:
    std::string name_;
    std::string description_;

    // Physical
    double density_ = 0.0;

    // Mechanical
    double youngsModulus_ = 0.0;
    double poissonsRatio_ = 0.0;
    double yieldStrength_ = 0.0;
    double ultimateStrength_ = 0.0;

    // Thermal
    double thermalConductivity_ = 0.0;
    double specificHeat_ = 0.0;
    double thermalExpansion_ = 0.0;

    // Rendering
    Color color_;
};

} // namespace koo::mcad
```

### 7. PMI - 제조 정보

```cpp
namespace koo::mcad {

/**
 * @brief Product Manufacturing Information
 *
 * GD&T, 치수, 주석 등 제조 정보
 */
class KOO_API PMI {
public:
    virtual ~PMI() = default;
    virtual PMIType getType() const = 0;

    // 연결된 형상
    std::vector<Face*> getAssociatedFaces() const;
    std::vector<Edge*> getAssociatedEdges() const;
    void addAssociatedGeometry(Shape* shape);

    // 위치 (3D 공간에서)
    const Point3D& getPosition() const { return position_; }
    void setPosition(const Point3D& pos) { position_ = pos; }

    // 뷰 방향
    const Vector3D& getNormal() const { return normal_; }
    void setNormal(const Vector3D& n) { normal_ = n; }

protected:
    Point3D position_;
    Vector3D normal_;
    std::vector<Shape*> associatedGeometry_;
};

/**
 * @brief 치수
 */
class KOO_API Dimension : public PMI {
public:
    PMIType getType() const override { return PMIType::Dimension; }

    DimensionType getDimensionType() const { return dimensionType_; }
    double getNominalValue() const { return nominalValue_; }
    double getUpperTolerance() const { return upperTolerance_; }
    double getLowerTolerance() const { return lowerTolerance_; }
    const std::string& getUnit() const { return unit_; }

private:
    DimensionType dimensionType_; // linear, angular, radial
    double nominalValue_;
    double upperTolerance_;
    double lowerTolerance_;
    std::string unit_;
};

/**
 * @brief GD&T (Geometric Dimensioning and Tolerancing)
 */
class KOO_API GeometricTolerance : public PMI {
public:
    PMIType getType() const override { return PMIType::GeometricTolerance; }

    GDTType getGDTType() const { return gdtType_; }
    double getToleranceValue() const { return toleranceValue_; }
    const std::string& getDatumReference() const { return datumReference_; }
    MaterialCondition getMaterialCondition() const { return materialCondition_; }

private:
    GDTType gdtType_; // flatness, perpendicularity, position, etc.
    double toleranceValue_;
    std::string datumReference_;
    MaterialCondition materialCondition_; // MMC, LMC, RFS
};

/**
 * @brief 데이텀 (기준)
 */
class KOO_API Datum : public PMI {
public:
    PMIType getType() const override { return PMIType::Datum; }

    const std::string& getLabel() const { return label_; }
    void setLabel(const std::string& label) { label_ = label; }

private:
    std::string label_; // A, B, C, etc.
};

/**
 * @brief 표면 거칠기
 */
class KOO_API SurfaceFinish : public PMI {
public:
    PMIType getType() const override { return PMIType::SurfaceFinish; }

    double getRaValue() const { return raValue_; } // μm
    double getRzValue() const { return rzValue_; } // μm
    const std::string& getMachiningProcess() const { return process_; }

private:
    double raValue_;
    double rzValue_;
    std::string process_;
};

} // namespace koo::mcad
```

---

## Reader/Writer Design

### STEP Reader

```cpp
namespace koo::mcad {

/**
 * @brief STEP 파일 리더 (OpenCASCADE 기반)
 */
class KOO_API StepReader {
public:
    struct Options {
        bool loadAssemblyStructure = true;  // 어셈블리 계층 로드
        bool loadColors = true;             // 색상 정보 로드
        bool loadNames = true;              // 이름 정보 로드
        bool loadMaterials = true;          // 재료 정보 로드 (AP242)
        bool loadPMI = true;                // PMI 로드 (AP242)
        double healingTolerance = 0.001;    // 형상 힐링 허용치
        bool performHealing = true;         // 형상 힐링 수행
    };

    Product read(const std::filesystem::path& stepPath);
    Product read(const std::filesystem::path& stepPath, const Options& options);

    // 부분 로드
    std::vector<std::string> listRootShapes(const std::filesystem::path& stepPath);
    Shape readShape(const std::filesystem::path& stepPath, const std::string& shapeName);

    // 진단 정보
    struct DiagnosticInfo {
        StepVersion version;
        std::string originatingSystem;
        std::vector<std::string> warnings;
        std::vector<std::string> errors;
        int entityCount;
    };
    DiagnosticInfo getDiagnostics() const { return diagnostics_; }

private:
    void parseAssemblyStructure(Product& product, /* OCC handle */);
    void parseColors(Product& product, /* OCC handle */);
    void parseMaterials(Product& product, /* OCC handle */);
    void parsePMI(Product& product, /* OCC handle */);

    DiagnosticInfo diagnostics_;
};

} // namespace koo::mcad
```

### STEP Writer

```cpp
namespace koo::mcad {

/**
 * @brief STEP 파일 작성기
 */
class KOO_API StepWriter {
public:
    struct Options {
        StepVersion version = StepVersion::AP242;
        bool writeAssemblyStructure = true;
        bool writeColors = true;
        bool writeNames = true;
        bool writeMaterials = true;
        bool writePMI = true;
        std::string authorName;
        std::string organizationName;
        std::string applicationName = "KooSimulationDataReader";
    };

    void write(const Product& product, const std::filesystem::path& stepPath);
    void write(const Product& product, const std::filesystem::path& stepPath,
              const Options& options);

private:
    void writeAssemblyStructure(const Assembly& assembly, /* OCC handle */);
    void writeColors(const Product& product, /* OCC handle */);
    void writeMaterials(const Product& product, /* OCC handle */);
    void writePMI(const Product& product, /* OCC handle */);
};

} // namespace koo::mcad
```

---

## Integration with CAE

### CAD to FEA Mesh

```cpp
namespace koo::mcad {

/**
 * @brief CAD 형상을 FEA 메쉬로 변환
 */
class KOO_API CadToMeshConverter {
public:
    struct MeshOptions {
        double elementSize = 1.0;           // 기본 요소 크기
        double minElementSize = 0.1;        // 최소 요소 크기
        double maxElementSize = 10.0;       // 최대 요소 크기
        int elementOrder = 1;               // 1차 또는 2차 요소
        MeshType meshType = MeshType::Auto; // shell, solid, auto
        bool quadDominant = false;          // 사각형 우선 (shell)
        double curvatureAdaptation = 0.5;   // 곡률 적응 계수
        bool preserveSharpEdges = true;     // 날카로운 에지 보존
        double sharpAngle = 30.0;           // 날카로운 에지 판단 각도
    };

    // 전체 제품 변환
    dyna::Model convert(const Product& product);
    dyna::Model convert(const Product& product, const MeshOptions& options);

    // 개별 파트 변환
    void convertPart(const Part& part, dyna::Model& model,
                    int partId, const MeshOptions& options);

    // 어셈블리 변환 (인스턴스별로 파트 생성)
    void convertAssembly(const Assembly& assembly, dyna::Model& model,
                        const MeshOptions& options);

    // 표면 메쉬만 생성
    void createSurfaceMesh(const Part& part, dyna::Model& model,
                          int partId, const MeshOptions& options);

    // 볼륨 메쉬 생성
    void createVolumeMesh(const Part& part, dyna::Model& model,
                         int partId, const MeshOptions& options);

    // 재료 자동 할당
    void assignMaterials(const Product& product, dyna::Model& model);

private:
    // 메쉬 크기 맵 생성
    void computeSizeMap(const Part& part, const MeshOptions& options);

    // OpenCASCADE 메셔 설정
    void configureMesher(const MeshOptions& options);
};

} // namespace koo::mcad
```

### Part-Geometry Association

```cpp
namespace koo::mcad {

/**
 * @brief CAE 파트와 CAD 형상 연결
 *
 * 메쉬 재생성, 형상 변경 추적에 활용
 */
class KOO_API PartGeometryLink {
public:
    // 연결 생성
    void link(dyna::Part* feaPart, Part* cadPart);
    void link(dyna::Part* feaPart, Face* cadFace);

    // 조회
    Part* getCadPart(dyna::Part* feaPart) const;
    dyna::Part* getFeaPart(Part* cadPart) const;

    // 면-요소 매핑
    std::vector<dyna::Element*> getElementsOnFace(Face* face) const;
    Face* getFaceOfElement(dyna::Element* element) const;

    // 에지-노드 매핑
    std::vector<dyna::Node*> getNodesOnEdge(Edge* edge) const;

    // 메쉬 업데이트 (형상 변경 후)
    void updateMesh(Part* cadPart, dyna::Model& model,
                   const CadToMeshConverter::MeshOptions& options);

private:
    std::unordered_map<dyna::Part*, Part*> partToGeometry_;
    std::unordered_map<Part*, dyna::Part*> geometryToPart_;
    std::unordered_map<Face*, std::vector<dyna::Element*>> faceToElements_;
    std::unordered_map<Edge*, std::vector<dyna::Node*>> edgeToNodes_;
};

} // namespace koo::mcad
```

---

## Modular Design Principles

### 1. Independence from CAE Module

MCAD 모듈은 CAE(dyna) 모듈과 독립적으로 동작합니다:

```cpp
// MCAD만 사용 - CAE 의존성 없음
#include <koo/mcad/Product.hpp>
#include <koo/mcad/StepReader.hpp>

koo::mcad::StepReader reader;
auto product = reader.read("assembly.step");

// 형상 분석
for (auto* part : product.getAllParts()) {
    std::cout << part->getName() << ": "
              << part->getVolume() << " mm³\n";
}
```

### 2. OpenCASCADE Abstraction

OpenCASCADE를 직접 노출하지 않고 추상화:

```cpp
// 사용자는 OpenCASCADE API를 몰라도 됨
auto product = koo::mcad::Product::loadStep("model.step");

// 고급 사용자를 위한 네이티브 접근
#ifdef KOO_MCAD_EXPOSE_OCC
    TopoDS_Shape occShape = *static_cast<TopoDS_Shape*>(
        product.getRootPart()->getShape()->getNativeHandle()
    );
#endif
```

### 3. CMake Configuration

```cmake
# src/mcad/CMakeLists.txt

# OpenCASCADE 찾기
find_package(OpenCASCADE REQUIRED)

# Core MCAD library
add_library(koo_mcad
    Product.cpp
    Assembly.cpp
    Part.cpp
    geometry/Shape.cpp
    geometry/Solid.cpp
    geometry/Face.cpp
    StepReader.cpp
    StepWriter.cpp
    # ...
)

target_include_directories(koo_mcad PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(koo_mcad PRIVATE
    ${OpenCASCADE_LIBRARIES}
)

# Optional: MCAD-CAE integration (depends on koo_dyna)
if(BUILD_MCAD_CAE_INTEGRATION)
    add_library(koo_mcad_cae_integration
        integration/CadToMeshConverter.cpp
        integration/PartGeometryLink.cpp
    )
    target_link_libraries(koo_mcad_cae_integration
        PUBLIC koo_mcad koo_dyna
    )
endif()

# Optional: IGES support
add_library(koo_mcad_iges
    IgesReader.cpp
    IgesWriter.cpp
)
target_link_libraries(koo_mcad_iges PUBLIC koo_mcad)
```

---

## Development Phases

### Phase 1: Core STEP Support
- [ ] OpenCASCADE 통합
- [ ] STEP AP203 읽기/쓰기
- [ ] 기본 형상 클래스 (Solid, Face, Edge, Vertex)
- [ ] 어셈블리 구조 파싱
- [ ] 색상/이름 정보 추출
- [ ] 단위 테스트

### Phase 2: Advanced Geometry
- [ ] 모든 서피스 타입 지원 (NURBS 등)
- [ ] 모든 커브 타입 지원
- [ ] 불리언 연산
- [ ] 필렛/챔퍼 연산
- [ ] 형상 힐링

### Phase 3: Engineering Data
- [ ] STEP AP242 지원
- [ ] 재료 정보 읽기/쓰기
- [ ] PMI/GD&T 지원
- [ ] 메타데이터 관리

### Phase 4: CAE Integration
- [ ] 표면 메쉬 생성
- [ ] 볼륨 메쉬 생성
- [ ] 메쉬 크기 제어
- [ ] Part-Geometry 연결
- [ ] 재료 자동 매핑

### Phase 5: Additional Formats
- [ ] IGES 지원
- [ ] Parasolid 지원 (선택)
- [ ] JT 지원 (선택)
- [ ] ACIS 지원 (선택)

---

## Dependencies

### Required
- **OpenCASCADE** 7.6+ (핵심 형상 커널)
- **C++17**

### Optional
- **Netgen** (볼륨 메쉬 생성)
- **GMSH** (대안 메셔)

---

## References

- [ISO 10303 (STEP)](https://www.iso.org/standard/84819.html)
- [OpenCASCADE Technology](https://www.opencascade.com/)
- [AP242 Overview](https://www.cax-if.org/documents/ap242_overview.pdf)
