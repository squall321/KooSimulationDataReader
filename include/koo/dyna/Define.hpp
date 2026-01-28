#pragma once

#include <koo/Export.hpp>
#include <koo/dyna/Keyword.hpp>
#include <koo/util/Types.hpp>
#include <utility>

namespace koo::dyna {

/**
 * @brief Base class for *DEFINE keywords
 */
class KOO_API DefineKeyword : public Keyword {
public:
    virtual ~DefineKeyword() = default;
};

/**
 * @brief *DEFINE_CURVE
 *
 * Defines a load curve (time-value pairs).
 */
class KOO_API DefineCurve : public CloneableKeyword<DefineCurve, DefineKeyword> {
public:
    struct Header {
        int lcid = 0;           // Load curve ID
        int sidr = 0;           // Stress initialization
        double sfa = 1.0;       // Scale factor for abscissa
        double sfo = 1.0;       // Scale factor for ordinate
        double offa = 0.0;      // Offset for abscissa
        double offo = 0.0;      // Offset for ordinate
        int dattyp = 0;         // Data type
        int lcint = 0;          // Load curve interpolation
    };

    using Point = std::pair<double, double>;  // (abscissa, ordinate)

    DefineCurve() = default;

    std::string getKeywordName() const override { return "*DEFINE_CURVE"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    // Header access
    Header& getHeader() { return header_; }
    const Header& getHeader() const { return header_; }

    int getCurveId() const { return header_.lcid; }
    void setCurveId(int id) { header_.lcid = id; }

    // Point data access
    std::vector<Point>& getPoints() { return points_; }
    const std::vector<Point>& getPoints() const { return points_; }

    void addPoint(double x, double y) { points_.emplace_back(x, y); }
    void clearPoints() { points_.clear(); }
    size_t getPointCount() const { return points_.size(); }

    // Interpolate value at given abscissa
    double interpolate(double x) const;

private:
    Header header_;
    std::vector<Point> points_;
};

/**
 * @brief *DEFINE_CURVE_TITLE
 *
 * Same as DEFINE_CURVE but with title support.
 */
class KOO_API DefineCurveTitle : public CloneableKeyword<DefineCurveTitle, DefineKeyword> {
public:
    using Point = std::pair<double, double>;

    struct Header {
        int lcid = 0;
        int sidr = 0;
        double sfa = 1.0;
        double sfo = 1.0;
        double offa = 0.0;
        double offo = 0.0;
        int dattyp = 0;
        int lcint = 0;
    };

    DefineCurveTitle() = default;

    std::string getKeywordName() const override { return "*DEFINE_CURVE_TITLE"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    // Title
    const std::string& getTitle() const { return title_; }
    void setTitle(const std::string& title) { title_ = title; }

    // Header access
    Header& getHeader() { return header_; }
    const Header& getHeader() const { return header_; }

    int getCurveId() const { return header_.lcid; }
    void setCurveId(int id) { header_.lcid = id; }

    // Point data access
    std::vector<Point>& getPoints() { return points_; }
    const std::vector<Point>& getPoints() const { return points_; }

    void addPoint(double x, double y) { points_.emplace_back(x, y); }
    void clearPoints() { points_.clear(); }

private:
    std::string title_;
    Header header_;
    std::vector<Point> points_;
};

/**
 * @brief *DEFINE_COORDINATE_NODES
 *
 * Defines a local coordinate system using three nodes.
 */
class KOO_API DefineCoordinateNodes : public CloneableKeyword<DefineCoordinateNodes, DefineKeyword> {
public:
    struct Data {
        int cid = 0;            // Coordinate system ID
        NodeId n1 = 0;          // Origin node
        NodeId n2 = 0;          // Node on local x-axis
        NodeId n3 = 0;          // Node in local xy-plane
        int flag = 0;           // Definition flag
    };

    DefineCoordinateNodes() = default;

    std::string getKeywordName() const override { return "*DEFINE_COORDINATE_NODES"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    Data& getData() { return data_; }
    const Data& getData() const { return data_; }

private:
    Data data_;
};

/**
 * @brief *DEFINE_COORDINATE_VECTOR
 *
 * Defines a local coordinate system using vectors.
 */
class KOO_API DefineCoordinateVector : public CloneableKeyword<DefineCoordinateVector, DefineKeyword> {
public:
    struct Data {
        int cid = 0;            // Coordinate system ID
        double x0 = 0.0;        // Origin x
        double y0 = 0.0;        // Origin y
        double z0 = 0.0;        // Origin z
        double xx = 1.0;        // X-axis x component
        double xy = 0.0;        // X-axis y component
        double xz = 0.0;        // X-axis z component
        double yx = 0.0;        // XY-plane vector x
        double yy = 1.0;        // XY-plane vector y
        double yz = 0.0;        // XY-plane vector z
    };

    DefineCoordinateVector() = default;

    std::string getKeywordName() const override { return "*DEFINE_COORDINATE_VECTOR"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    Data& getData() { return data_; }
    const Data& getData() const { return data_; }

private:
    Data data_;
};

/**
 * @brief *DEFINE_COORDINATE_SYSTEM
 *
 * Defines a local coordinate system by specifying origin and axis directions.
 */
class KOO_API DefineCoordinateSystem : public CloneableKeyword<DefineCoordinateSystem, DefineKeyword> {
public:
    struct Data {
        int cid = 0;            // Coordinate system ID
        double xo = 0.0;        // Origin x
        double yo = 0.0;        // Origin y
        double zo = 0.0;        // Origin z
        double xl = 0.0;        // Point on local x-axis x
        double yl = 0.0;        // Point on local x-axis y
        double zl = 0.0;        // Point on local x-axis z
        double xp = 0.0;        // Point in local xy-plane x
        double yp = 0.0;        // Point in local xy-plane y
        double zp = 0.0;        // Point in local xy-plane z
    };

    DefineCoordinateSystem() = default;

    std::string getKeywordName() const override { return "*DEFINE_COORDINATE_SYSTEM"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    Data& getData() { return data_; }
    const Data& getData() const { return data_; }

private:
    Data data_;
};

/**
 * @brief *DEFINE_BOX
 *
 * Defines a box region for various purposes.
 */
class KOO_API DefineBox : public CloneableKeyword<DefineBox, DefineKeyword> {
public:
    struct Data {
        int boxid = 0;          // Box ID
        double xmin = 0.0;      // Minimum x
        double xmax = 0.0;      // Maximum x
        double ymin = 0.0;      // Minimum y
        double ymax = 0.0;      // Maximum y
        double zmin = 0.0;      // Minimum z
        double zmax = 0.0;      // Maximum z
    };

    DefineBox() = default;

    std::string getKeywordName() const override { return "*DEFINE_BOX"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    Data& getData() { return data_; }
    const Data& getData() const { return data_; }

private:
    Data data_;
};

/**
 * @brief *DEFINE_TRANSFORMATION
 *
 * Defines a transformation matrix.
 */
class KOO_API DefineTransformation : public CloneableKeyword<DefineTransformation, DefineKeyword> {
public:
    struct Data {
        int tranid = 0;         // Transformation ID
        int option = 0;         // Option type
        double a1 = 0.0;        // Parameter 1
        double a2 = 0.0;        // Parameter 2
        double a3 = 0.0;        // Parameter 3
        double a4 = 0.0;        // Parameter 4
        double a5 = 0.0;        // Parameter 5
        double a6 = 0.0;        // Parameter 6
        double a7 = 0.0;        // Parameter 7
    };

    DefineTransformation() = default;

    std::string getKeywordName() const override { return "*DEFINE_TRANSFORMATION"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    Data& getData() { return data_; }
    const Data& getData() const { return data_; }

private:
    Data data_;
};

/**
 * @brief *DEFINE_VECTOR
 *
 * Defines a vector for various purposes.
 */
class KOO_API DefineVector : public CloneableKeyword<DefineVector, DefineKeyword> {
public:
    struct Data {
        int vid = 0;            // Vector ID
        double xt = 0.0;        // Tail x
        double yt = 0.0;        // Tail y
        double zt = 0.0;        // Tail z
        double xh = 0.0;        // Head x
        double yh = 0.0;        // Head y
        double zh = 0.0;        // Head z
        int cid = 0;            // Coordinate system ID
    };

    DefineVector() = default;

    std::string getKeywordName() const override { return "*DEFINE_VECTOR"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    Data& getData() { return data_; }
    const Data& getData() const { return data_; }

private:
    Data data_;
};

/**
 * @brief *DEFINE_TABLE
 *
 * Defines a family of curves for strain rate dependent materials.
 */
class KOO_API DefineTable : public CloneableKeyword<DefineTable, DefineKeyword> {
public:
    struct Entry {
        double value = 0.0;     // Table value (e.g., strain rate)
        int lcid = 0;           // Load curve ID
    };

    DefineTable() = default;

    std::string getKeywordName() const override { return "*DEFINE_TABLE"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    int getTableId() const { return tbid_; }
    void setTableId(int id) { tbid_ = id; }

    std::vector<Entry>& getEntries() { return entries_; }
    const std::vector<Entry>& getEntries() const { return entries_; }

private:
    int tbid_ = 0;
    std::vector<Entry> entries_;
};

/**
 * @brief *DEFINE_FRICTION
 *
 * Defines friction model parameters.
 */
class KOO_API DefineFriction : public CloneableKeyword<DefineFriction, DefineKeyword> {
public:
    struct Data {
        int fid = 0;            // Friction ID
        double fs = 0.0;        // Static friction coefficient
        double fd = 0.0;        // Dynamic friction coefficient
        double dc = 0.0;        // Decay constant
        double vc = 0.0;        // Critical velocity
        double fv = 0.0;        // Viscous friction factor
    };

    DefineFriction() = default;

    std::string getKeywordName() const override { return "*DEFINE_FRICTION"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    Data& getData() { return data_; }
    const Data& getData() const { return data_; }

private:
    Data data_;
};

/**
 * @brief *DEFINE_SD_ORIENTATION
 *
 * Defines spring/damper orientation.
 */
class KOO_API DefineSdOrientation : public CloneableKeyword<DefineSdOrientation, DefineKeyword> {
public:
    struct Data {
        int vid = 0;            // Orientation ID
        int iop = 0;            // Option
        double xt = 0.0;        // X component or tail
        double yt = 0.0;        // Y component or tail
        double zt = 0.0;        // Z component or tail
        double xh = 0.0;        // Head x (if iop=1)
        double yh = 0.0;        // Head y
        double zh = 0.0;        // Head z
    };

    DefineSdOrientation() = default;

    std::string getKeywordName() const override { return "*DEFINE_SD_ORIENTATION"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    Data& getData() { return data_; }
    const Data& getData() const { return data_; }

private:
    Data data_;
};

/**
 * @brief *DEFINE_ELEMENT_DEATH
 *
 * Defines element death criteria.
 */
class KOO_API DefineElementDeath : public CloneableKeyword<DefineElementDeath, DefineKeyword> {
public:
    struct Data {
        int sid = 0;            // Set ID
        double time = 0.0;      // Death time
        int boxid = 0;          // Box ID
        int cycle = 0;          // Cycle for death
    };

    DefineElementDeath() = default;

    std::string getKeywordName() const override { return "*DEFINE_ELEMENT_DEATH"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    std::vector<Data>& getData() { return data_; }
    const std::vector<Data>& getData() const { return data_; }

private:
    std::vector<Data> data_;
};

/**
 * @brief *DEFINE_CONTACT_VOLUME
 *
 * Defines contact volume for contact algorithms.
 */
class KOO_API DefineContactVolume : public CloneableKeyword<DefineContactVolume, DefineKeyword> {
public:
    struct Data {
        int cvid = 0;           // Contact volume ID
        int boxid = 0;          // Box ID
        int pid = 0;            // Part ID
        int type = 0;           // Type
    };

    DefineContactVolume() = default;

    std::string getKeywordName() const override { return "*DEFINE_CONTACT_VOLUME"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    std::vector<Data>& getData() { return data_; }
    const std::vector<Data>& getData() const { return data_; }

private:
    std::vector<Data> data_;
};

/**
 * @brief *DEFINE_CURVE_FUNCTION
 *
 * Defines a curve using a function.
 */
class KOO_API DefineCurveFunction : public CloneableKeyword<DefineCurveFunction, DefineKeyword> {
public:
    struct Data {
        int lcid = 0;           // Load curve ID
        int sidr = 0;           // Stress initialization
        double sfa = 1.0;       // Scale factor for abscissa
        double sfo = 1.0;       // Scale factor for ordinate
        double offa = 0.0;      // Offset for abscissa
        double offo = 0.0;      // Offset for ordinate
    };

    DefineCurveFunction() = default;

    std::string getKeywordName() const override { return "*DEFINE_CURVE_FUNCTION"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    Data& getData() { return data_; }
    const Data& getData() const { return data_; }

    const std::string& getFunction() const { return function_; }
    void setFunction(const std::string& func) { function_ = func; }

private:
    Data data_;
    std::string function_;
};

/**
 * @brief *DEFINE_CONNECTION_PROPERTIES
 *
 * Defines connection properties for spot welds and bolts.
 */
class KOO_API DefineConnectionProperties : public CloneableKeyword<DefineConnectionProperties, DefineKeyword> {
public:
    struct Data {
        int cpid = 0;           // Connection property ID
        int type = 0;           // Connection type
        double dfail = 0.0;     // Displacement at failure
        double efail = 0.0;     // Energy at failure
        double nfail = 0.0;     // Normal force at failure
        double sfail = 0.0;     // Shear force at failure
    };

    DefineConnectionProperties() = default;

    std::string getKeywordName() const override { return "*DEFINE_CONNECTION_PROPERTIES"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    std::vector<Data>& getData() { return data_; }
    const std::vector<Data>& getData() const { return data_; }

private:
    std::vector<Data> data_;
};

/**
 * @brief *DEFINE_DE_MESH_SURFACE
 *
 * Defines discrete element mesh on surface.
 */
class KOO_API DefineDeMeshSurface : public CloneableKeyword<DefineDeMeshSurface, DefineKeyword> {
public:
    struct Data {
        int sid = 0;            // Surface ID
        int pid = 0;            // Part ID
        int type = 0;           // Surface type
        double size = 0.0;      // Element size
    };

    DefineDeMeshSurface() = default;

    std::string getKeywordName() const override { return "*DEFINE_DE_MESH_SURFACE"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    std::vector<Data>& getData() { return data_; }
    const std::vector<Data>& getData() const { return data_; }

private:
    std::vector<Data> data_;
};

/**
 * @brief *DEFINE_CPM_VENT
 *
 * Defines CPM (Corpuscular Particle Method) vent properties for airbag.
 */
class KOO_API DefineCpmVent : public CloneableKeyword<DefineCpmVent, DefineKeyword> {
public:
    struct Data {
        int ventid = 0;         // Vent ID
        int ssid = 0;           // Segment set ID
        double acoef = 0.0;     // A coefficient
        double bcoef = 0.0;     // B coefficient
        double ccoef = 0.0;     // C coefficient
        int lcid = 0;           // Load curve ID
    };

    DefineCpmVent() = default;

    std::string getKeywordName() const override { return "*DEFINE_CPM_VENT"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    Data& getData() { return data_; }
    const Data& getData() const { return data_; }

private:
    Data data_;
};

/**
 * @brief *DEFINE_FUNCTION_TABULATED
 *
 * Defines a tabulated function with multiple variables.
 */
class KOO_API DefineFunctionTabulated : public CloneableKeyword<DefineFunctionTabulated, DefineKeyword> {
public:
    struct Data {
        int funcid = 0;         // Function ID
        int nvar = 0;           // Number of variables
        int lcid = 0;           // Load curve ID for interpolation
        int mvar = 0;           // Method for variable interpolation
    };

    DefineFunctionTabulated() = default;

    std::string getKeywordName() const override { return "*DEFINE_FUNCTION_TABULATED"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    Data& getData() { return data_; }
    const Data& getData() const { return data_; }

private:
    Data data_;
};

/**
 * @brief *DEFINE_CURVE_SMOOTH
 *
 * Defines a smooth curve that is interpolated from input points.
 */
class KOO_API DefineCurveSmooth : public CloneableKeyword<DefineCurveSmooth, DefineKeyword> {
public:
    struct Header {
        int lcid = 0;           // Load curve ID
        int sidr = 0;           // Stress initialization
        double sfa = 1.0;       // Scale factor for abscissa
        double sfo = 1.0;       // Scale factor for ordinate
        double offa = 0.0;      // Offset for abscissa
        double offo = 0.0;      // Offset for ordinate
        int dattyp = 0;         // Data type
        int npts = 0;           // Number of output points (for smoothing)
    };

    using Point = std::pair<double, double>;  // (abscissa, ordinate)

    DefineCurveSmooth() = default;

    std::string getKeywordName() const override { return "*DEFINE_CURVE_SMOOTH"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    // Header access
    Header& getHeader() { return header_; }
    const Header& getHeader() const { return header_; }

    int getCurveId() const { return header_.lcid; }
    void setCurveId(int id) { header_.lcid = id; }

    // Point data access
    std::vector<Point>& getPoints() { return points_; }
    const std::vector<Point>& getPoints() const { return points_; }

    void addPoint(double x, double y) { points_.emplace_back(x, y); }
    void clearPoints() { points_.clear(); }
    size_t getPointCount() const { return points_.size(); }

private:
    Header header_;
    std::vector<Point> points_;
};

/**
 * @brief *DEFINE_HEX_SPOTWELD_ASSEMBLY
 *
 * Defines hex element spotweld assembly.
 */
class KOO_API DefineHexSpotweldAssembly : public CloneableKeyword<DefineHexSpotweldAssembly, DefineKeyword> {
public:
    struct Assembly {
        int assemblyId = 0;       // Assembly ID
        int nquad = 0;            // Number of quads
        int func = 0;             // Function type
        double filter = 0.0;      // Filter value
        std::vector<int> quadIds; // Quad element IDs
    };

    DefineHexSpotweldAssembly() = default;

    std::string getKeywordName() const override { return "*DEFINE_HEX_SPOTWELD_ASSEMBLY"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    std::vector<Assembly>& getAssemblies() { return assemblies_; }
    const std::vector<Assembly>& getAssemblies() const { return assemblies_; }

private:
    std::vector<Assembly> assemblies_;
};

/**
 * @brief *DEFINE_FUNCTION
 *
 * Defines a user-defined mathematical function.
 */
class KOO_API DefineFunction : public CloneableKeyword<DefineFunction, DefineKeyword> {
public:
    struct Data {
        int id = 0;               // Function ID
        std::string expression;   // Function expression
    };

    DefineFunction() = default;

    std::string getKeywordName() const override { return "*DEFINE_FUNCTION"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    Data& getData() { return data_; }
    const Data& getData() const { return data_; }

private:
    Data data_;
};

/**
 * @brief *DEFINE_MATERIAL_HISTORIES
 *
 * Defines material history variables for output.
 */
class KOO_API DefineMaterialHistories : public CloneableKeyword<DefineMaterialHistories, DefineKeyword> {
public:
    struct Data {
        int mid = 0;              // Material ID
        std::vector<int> hisv;    // History variables to output
    };

    DefineMaterialHistories() = default;

    std::string getKeywordName() const override { return "*DEFINE_MATERIAL_HISTORIES"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    Data& getData() { return data_; }
    const Data& getData() const { return data_; }

private:
    Data data_;
};

/**
 * @brief *DEFINE_ADAPTIVE_SOLID_TO_SPH
 *
 * Defines adaptive solid to SPH conversion parameters.
 */
class KOO_API DefineAdaptiveSolidToSph : public CloneableKeyword<DefineAdaptiveSolidToSph, DefineKeyword> {
public:
    struct Data {
        int pid = 0;              // Part ID
        double fail = 0.0;        // Failure strain
        int iopt = 0;             // Option flag
        double sphcell = 0.0;     // SPH cell size
    };

    DefineAdaptiveSolidToSph() = default;

    std::string getKeywordName() const override { return "*DEFINE_ADAPTIVE_SOLID_TO_SPH"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    Data& getData() { return data_; }
    const Data& getData() const { return data_; }

private:
    Data data_;
};

/**
 * @brief *DEFINE_TABLE_2D
 *
 * Defines a 2D lookup table.
 */
class KOO_API DefineTable2D : public CloneableKeyword<DefineTable2D, DefineKeyword> {
public:
    struct Data {
        int id = 0;               // Table ID
        std::vector<std::pair<double, int>> curves; // (value, curve_id) pairs
    };

    DefineTable2D() = default;

    std::string getKeywordName() const override { return "*DEFINE_TABLE_2D"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    Data& getData() { return data_; }
    const Data& getData() const { return data_; }

private:
    Data data_;
};

/**
 * @brief *DEFINE_TABLE_3D
 *
 * Defines a 3D lookup table.
 */
class KOO_API DefineTable3D : public CloneableKeyword<DefineTable3D, DefineKeyword> {
public:
    struct Data {
        int id = 0;               // Table ID
        std::vector<std::pair<double, int>> tables; // (value, table2d_id) pairs
    };

    DefineTable3D() = default;

    std::string getKeywordName() const override { return "*DEFINE_TABLE_3D"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    Data& getData() { return data_; }
    const Data& getData() const { return data_; }

private:
    Data data_;
};

/**
 * @brief *DEFINE_DE_MESH_BEAM
 *
 * Defines discrete element mesh for beam elements.
 */
class KOO_API DefineDeMeshBeam : public CloneableKeyword<DefineDeMeshBeam, DefineKeyword> {
public:
    struct Data {
        int sid = 0;              // Segment set ID
        int pid = 0;              // Part ID
        double rad = 0.0;         // Particle radius
    };

    DefineDeMeshBeam() = default;

    std::string getKeywordName() const override { return "*DEFINE_DE_MESH_BEAM"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    Data& getData() { return data_; }
    const Data& getData() const { return data_; }

private:
    Data data_;
};

/**
 * @brief *DEFINE_SPOTWELD_RUPTURE_PARAMETER
 *
 * Defines spotweld rupture parameters.
 */
class KOO_API DefineSpotweldRuptureParameter : public CloneableKeyword<DefineSpotweldRuptureParameter, DefineKeyword> {
public:
    struct Data {
        int nrr = 0;              // Number of rupture parameters
        double tfail = 0.0;       // Tensile failure force
        double rfail = 0.0;       // Resultant failure force
        double nfail = 0.0;       // Normal failure force
    };

    DefineSpotweldRuptureParameter() = default;

    std::string getKeywordName() const override { return "*DEFINE_SPOTWELD_RUPTURE_PARAMETER"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    Data& getData() { return data_; }
    const Data& getData() const { return data_; }

private:
    Data data_;
};

/**
 * @brief *DEFINE_FORMING_BLANKMESH
 *
 * Defines forming blank mesh parameters.
 */
class KOO_API DefineFormingBlankmesh : public CloneableKeyword<DefineFormingBlankmesh, DefineKeyword> {
public:
    struct Data {
        int nsid = 0;             // Node set ID
        double size = 0.0;        // Target element size
        int itype = 0;            // Element type
    };

    DefineFormingBlankmesh() = default;

    std::string getKeywordName() const override { return "*DEFINE_FORMING_BLANKMESH"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    Data& getData() { return data_; }
    const Data& getData() const { return data_; }

private:
    Data data_;
};

/**
 * @brief *DEFINE_PRESSURE_TUBE
 *
 * Defines pressure tube parameters for airbag modeling.
 */
class KOO_API DefinePressureTube : public CloneableKeyword<DefinePressureTube, DefineKeyword> {
public:
    struct Data {
        int id = 0;               // Pressure tube ID
        double lc = 0.0;          // Length or load curve ID
        int lcid = 0;             // Load curve ID for mass flow
    };

    DefinePressureTube() = default;

    std::string getKeywordName() const override { return "*DEFINE_PRESSURE_TUBE"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    Data& getData() { return data_; }
    const Data& getData() const { return data_; }

private:
    Data data_;
};

/**
 * @brief *DEFINE_SPOTWELD_MULTISCALE
 *
 * Defines multi-scale spotweld parameters.
 */
class KOO_API DefineSpotweldMultiscale : public CloneableKeyword<DefineSpotweldMultiscale, DefineKeyword> {
public:
    struct Data {
        int id = 0;               // Definition ID
        int iopt = 0;             // Option flag
        double tfail = 0.0;       // Tensile failure
    };

    DefineSpotweldMultiscale() = default;

    std::string getKeywordName() const override { return "*DEFINE_SPOTWELD_MULTISCALE"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    Data& getData() { return data_; }
    const Data& getData() const { return data_; }

private:
    Data data_;
};

/**
 * @brief *DEFINE_PBLAST_AIRBURST
 *
 * Defines PBLAST airburst parameters for blast simulations.
 */
class KOO_API DefinePblastAirburst : public CloneableKeyword<DefinePblastAirburst, DefineKeyword> {
public:
    struct Data {
        int bid = 0;              // Blast ID
        double xb = 0.0;          // X coordinate of burst
        double yb = 0.0;          // Y coordinate of burst
        double zb = 0.0;          // Z coordinate of burst
        double tob = 0.0;         // Time of burst
        double wgt = 0.0;         // Weight of explosive
    };

    DefinePblastAirburst() = default;

    std::string getKeywordName() const override { return "*DEFINE_PBLAST_AIRBURST"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    Data& getData() { return data_; }
    const Data& getData() const { return data_; }

private:
    Data data_;
};

/**
 * @brief *DEFINE_PBLAST_SURFACE
 *
 * Defines PBLAST surface blast parameters.
 */
class KOO_API DefinePblastSurface : public CloneableKeyword<DefinePblastSurface, DefineKeyword> {
public:
    struct Data {
        int bid = 0;              // Blast ID
        double xb = 0.0;          // X coordinate of burst
        double yb = 0.0;          // Y coordinate of burst
        double zb = 0.0;          // Z coordinate of burst
        double tob = 0.0;         // Time of burst
        double wgt = 0.0;         // Weight of explosive
    };

    DefinePblastSurface() = default;

    std::string getKeywordName() const override { return "*DEFINE_PBLAST_SURFACE"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    Data& getData() { return data_; }
    const Data& getData() const { return data_; }

private:
    Data data_;
};

/**
 * @brief *DEFINE_FILTER
 *
 * Defines filter parameters for data processing.
 */
class KOO_API DefineFilter : public CloneableKeyword<DefineFilter, DefineKeyword> {
public:
    struct Data {
        int fid = 0;              // Filter ID
        int ftype = 0;            // Filter type
        double freq = 0.0;        // Cutoff frequency
        int nterm = 0;            // Number of terms
    };

    DefineFilter() = default;

    std::string getKeywordName() const override { return "*DEFINE_FILTER"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    Data& getData() { return data_; }
    const Data& getData() const { return data_; }

private:
    Data data_;
};

/**
 * @brief *DEFINE_DEATH_TIMES
 *
 * Defines element death times.
 */
class KOO_API DefineDeathTimes : public CloneableKeyword<DefineDeathTimes, DefineKeyword> {
public:
    struct Data {
        int eid = 0;              // Element ID
        double timed = 0.0;       // Time of death
    };

    DefineDeathTimes() = default;

    std::string getKeywordName() const override { return "*DEFINE_DEATH_TIMES"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    std::vector<Data>& getData() { return data_; }
    const std::vector<Data>& getData() const { return data_; }

private:
    std::vector<Data> data_;
};

/**
 * @brief *DEFINE_BIRTH_TIMES
 *
 * Defines element birth times.
 */
class KOO_API DefineBirthTimes : public CloneableKeyword<DefineBirthTimes, DefineKeyword> {
public:
    struct Data {
        int eid = 0;              // Element ID
        double timeb = 0.0;       // Time of birth
    };

    DefineBirthTimes() = default;

    std::string getKeywordName() const override { return "*DEFINE_BIRTH_TIMES"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    std::vector<Data>& getData() { return data_; }
    const std::vector<Data>& getData() const { return data_; }

private:
    std::vector<Data> data_;
};

/**
 * @brief *DEFINE_GROUND_MOTION
 *
 * Defines ground motion for seismic analysis.
 */
class KOO_API DefineGroundMotion : public CloneableKeyword<DefineGroundMotion, DefineKeyword> {
public:
    struct Data {
        int gmid = 0;             // Ground motion ID
        int lcid = 0;             // Load curve ID
        double sf = 1.0;          // Scale factor
        int idir = 0;             // Direction flag
    };

    DefineGroundMotion() = default;

    std::string getKeywordName() const override { return "*DEFINE_GROUND_MOTION"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    Data& getData() { return data_; }
    const Data& getData() const { return data_; }

private:
    Data data_;
};

/**
 * @brief *DEFINE_LOAD_BLAST_SEGMENT
 *
 * Defines blast load segment.
 */
class KOO_API DefineLoadBlastSegment : public CloneableKeyword<DefineLoadBlastSegment, DefineKeyword> {
public:
    struct Data {
        int bid = 0;              // Blast ID
        int ssid = 0;             // Segment set ID
        double sf = 1.0;          // Scale factor
    };

    DefineLoadBlastSegment() = default;

    std::string getKeywordName() const override { return "*DEFINE_LOAD_BLAST_SEGMENT"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    Data& getData() { return data_; }
    const Data& getData() const { return data_; }

private:
    Data data_;
};

/**
 * @brief *DEFINE_POROUS_CHEMISTRY
 *
 * Defines porous chemistry properties.
 */
class KOO_API DefinePorousChemistry : public CloneableKeyword<DefinePorousChemistry, DefineKeyword> {
public:
    struct Data {
        int pcid = 0;             // Porous chemistry ID
        int nspec = 0;            // Number of species
        double por = 0.0;         // Porosity
        double tort = 1.0;        // Tortuosity
    };

    DefinePorousChemistry() = default;

    std::string getKeywordName() const override { return "*DEFINE_POROUS_CHEMISTRY"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    Data& getData() { return data_; }
    const Data& getData() const { return data_; }

private:
    Data data_;
};

/**
 * @brief *DEFINE_STOCHASTIC_VARIATION
 *
 * Defines stochastic variation parameters.
 */
class KOO_API DefineStochasticVariation : public CloneableKeyword<DefineStochasticVariation, DefineKeyword> {
public:
    struct Data {
        int svid = 0;             // Stochastic variation ID
        int distrib = 0;          // Distribution type
        double mean = 0.0;        // Mean value
        double stddev = 0.0;      // Standard deviation
    };

    DefineStochasticVariation() = default;

    std::string getKeywordName() const override { return "*DEFINE_STOCHASTIC_VARIATION"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    Data& getData() { return data_; }
    const Data& getData() const { return data_; }

private:
    Data data_;
};

/**
 * @brief *DEFINE_HAZ_PROPERTIES
 *
 * Defines heat affected zone properties for welds.
 */
class KOO_API DefineHazProperties : public CloneableKeyword<DefineHazProperties, DefineKeyword> {
public:
    struct Data {
        int hazid = 0;            // HAZ ID
        double width = 0.0;       // HAZ width
        double sf = 1.0;          // Strength reduction factor
    };

    DefineHazProperties() = default;

    std::string getKeywordName() const override { return "*DEFINE_HAZ_PROPERTIES"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    Data& getData() { return data_; }
    const Data& getData() const { return data_; }

private:
    Data data_;
};

/**
 * @brief *DEFINE_CARPET_PLOT
 *
 * Defines carpet plot parameters for optimization.
 */
class KOO_API DefineCarpetPlot : public CloneableKeyword<DefineCarpetPlot, DefineKeyword> {
public:
    struct Data {
        int cpid = 0;             // Carpet plot ID
        int np1 = 0;              // Number of points dim 1
        int np2 = 0;              // Number of points dim 2
        double min1 = 0.0;        // Min value dim 1
        double max1 = 0.0;        // Max value dim 1
        double min2 = 0.0;        // Min value dim 2
        double max2 = 0.0;        // Max value dim 2
    };

    DefineCarpetPlot() = default;

    std::string getKeywordName() const override { return "*DEFINE_CARPET_PLOT"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    Data& getData() { return data_; }
    const Data& getData() const { return data_; }

private:
    Data data_;
};

/**
 * @brief *DEFINE_TRIM_SEED
 *
 * Defines trim seed points.
 */
class KOO_API DefineTrimSeed : public CloneableKeyword<DefineTrimSeed, DefineKeyword> {
public:
    struct Data {
        int tsid = 0;             // Trim seed ID
        double x = 0.0;           // X coordinate
        double y = 0.0;           // Y coordinate
        double z = 0.0;           // Z coordinate
    };

    DefineTrimSeed() = default;

    std::string getKeywordName() const override { return "*DEFINE_TRIM_SEED"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    Data& getData() { return data_; }
    const Data& getData() const { return data_; }

private:
    Data data_;
};

/**
 * @brief *DEFINE_REGION
 *
 * Defines a region for selective output or processing.
 */
class KOO_API DefineRegion : public CloneableKeyword<DefineRegion, DefineKeyword> {
public:
    struct Data {
        int rid = 0;              // Region ID
        double xmin = 0.0;        // Minimum X
        double xmax = 0.0;        // Maximum X
        double ymin = 0.0;        // Minimum Y
        double ymax = 0.0;        // Maximum Y
        double zmin = 0.0;        // Minimum Z
        double zmax = 0.0;        // Maximum Z
    };

    DefineRegion() = default;

    std::string getKeywordName() const override { return "*DEFINE_REGION"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    Data& getData() { return data_; }
    const Data& getData() const { return data_; }

private:
    Data data_;
};

/**
 * @brief *DEFINE_CURVE_COMPENSATED
 *
 * Defines compensated curve for forming springback.
 */
class KOO_API DefineCurveCompensated : public CloneableKeyword<DefineCurveCompensated, DefineKeyword> {
public:
    struct Data {
        int lcid = 0;             // Load curve ID
        int sidr = 0;             // Stress initialization
        double sfa = 1.0;         // Scale factor abscissa
        double sfo = 1.0;         // Scale factor ordinate
        double cfac = 1.0;        // Compensation factor
    };

    DefineCurveCompensated() = default;

    std::string getKeywordName() const override { return "*DEFINE_CURVE_COMPENSATED"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    Data& getData() { return data_; }
    const Data& getData() const { return data_; }

private:
    Data data_;
};

/**
 * @brief *DEFINE_CURVE_ENTITY
 *
 * Defines curve entity for CAD geometry.
 */
class KOO_API DefineCurveEntity : public CloneableKeyword<DefineCurveEntity, DefineKeyword> {
public:
    struct Data {
        int ceid = 0;             // Curve entity ID
        int ctype = 0;            // Curve type
        int np = 0;               // Number of points
    };

    DefineCurveEntity() = default;

    std::string getKeywordName() const override { return "*DEFINE_CURVE_ENTITY"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    Data& getData() { return data_; }
    const Data& getData() const { return data_; }

private:
    Data data_;
};

/**
 * @brief *DEFINE_CURVE_DUPLICATE
 *
 * Duplicates an existing curve definition.
 */
class KOO_API DefineCurveDuplicate : public CloneableKeyword<DefineCurveDuplicate, DefineKeyword> {
public:
    struct Data {
        int newlcid = 0;          // New load curve ID
        int oldlcid = 0;          // Old load curve ID to duplicate
        double sfa = 1.0;         // Scale factor abscissa
        double sfo = 1.0;         // Scale factor ordinate
    };

    DefineCurveDuplicate() = default;

    std::string getKeywordName() const override { return "*DEFINE_CURVE_DUPLICATE"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    Data& getData() { return data_; }
    const Data& getData() const { return data_; }

private:
    Data data_;
};

/**
 * @brief *DEFINE_CURVE_TRIM_2D
 *
 * Defines 2D trim curve for forming.
 */
class KOO_API DefineCurveTrim2d : public CloneableKeyword<DefineCurveTrim2d, DefineKeyword> {
public:
    struct Data {
        int tcid = 0;             // Trim curve ID
        int type = 0;             // Curve type
        int np = 0;               // Number of points
    };

    DefineCurveTrim2d() = default;

    std::string getKeywordName() const override { return "*DEFINE_CURVE_TRIM_2D"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    Data& getData() { return data_; }
    const Data& getData() const { return data_; }

private:
    Data data_;
};

/**
 * @brief *DEFINE_CURVE_TRIM_3D
 *
 * Defines 3D trim curve for forming.
 */
class KOO_API DefineCurveTrim3d : public CloneableKeyword<DefineCurveTrim3d, DefineKeyword> {
public:
    struct Data {
        int tcid = 0;             // Trim curve ID
        int type = 0;             // Curve type
        int np = 0;               // Number of points
    };

    DefineCurveTrim3d() = default;

    std::string getKeywordName() const override { return "*DEFINE_CURVE_TRIM_3D"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    Data& getData() { return data_; }
    const Data& getData() const { return data_; }

private:
    Data data_;
};

} // namespace koo::dyna
