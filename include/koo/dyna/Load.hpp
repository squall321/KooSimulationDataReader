#pragma once

#include <koo/Export.hpp>
#include <koo/dyna/Keyword.hpp>
#include <koo/util/Types.hpp>

namespace koo::dyna {

/**
 * @brief Base class for *LOAD keywords
 */
class KOO_API LoadKeyword : public Keyword {
public:
    virtual ~LoadKeyword() = default;
};

/**
 * @brief Load data for node-based loads
 */
struct NodeLoadData {
    NodeId nid = 0;             // Node ID
    int dof = 0;                // Degree of freedom (1-6)
    int lcid = 0;               // Load curve ID
    double sf = 1.0;            // Scale factor
    int cid = 0;                // Coordinate system ID
    int m1 = 0;                 // Method 1
    int m2 = 0;                 // Method 2
    int m3 = 0;                 // Method 3
};

/**
 * @brief *LOAD_NODE_POINT
 *
 * Point load on individual nodes.
 */
class KOO_API LoadNodePoint : public CloneableKeyword<LoadNodePoint, LoadKeyword> {
public:
    LoadNodePoint() = default;

    std::string getKeywordName() const override { return "*LOAD_NODE_POINT"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    std::vector<NodeLoadData>& getLoads() { return loads_; }
    const std::vector<NodeLoadData>& getLoads() const { return loads_; }

    void addLoad(const NodeLoadData& load) { loads_.push_back(load); }
    void clear() { loads_.clear(); }

private:
    std::vector<NodeLoadData> loads_;
};

/**
 * @brief *LOAD_NODE_SET
 *
 * Point load on node sets.
 */
class KOO_API LoadNodeSet : public CloneableKeyword<LoadNodeSet, LoadKeyword> {
public:
    struct Data {
        int nsid = 0;           // Node set ID
        int dof = 0;            // Degree of freedom
        int lcid = 0;           // Load curve ID
        double sf = 1.0;        // Scale factor
        int cid = 0;            // Coordinate system ID
    };

    LoadNodeSet() = default;

    std::string getKeywordName() const override { return "*LOAD_NODE_SET"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    std::vector<Data>& getLoads() { return loads_; }
    const std::vector<Data>& getLoads() const { return loads_; }

    void addLoad(const Data& load) { loads_.push_back(load); }
    void clear() { loads_.clear(); }

private:
    std::vector<Data> loads_;
};

/**
 * @brief *LOAD_BODY_Z (gravity in Z direction)
 *
 * Body force (e.g., gravity) applied to all parts.
 */
class KOO_API LoadBodyZ : public CloneableKeyword<LoadBodyZ, LoadKeyword> {
public:
    struct Data {
        int lcid = 0;           // Load curve ID
        double sf = 1.0;        // Scale factor
        int lciddr = 0;         // Dynamic relaxation load curve
        double xc = 0.0;        // X coordinate of rotation center
        double yc = 0.0;        // Y coordinate of rotation center
        double zc = 0.0;        // Z coordinate of rotation center
    };

    LoadBodyZ() = default;

    std::string getKeywordName() const override { return "*LOAD_BODY_Z"; }

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
 * @brief *LOAD_BODY_X
 */
class KOO_API LoadBodyX : public CloneableKeyword<LoadBodyX, LoadKeyword> {
public:
    struct Data {
        int lcid = 0;
        double sf = 1.0;
        int lciddr = 0;
        double xc = 0.0;
        double yc = 0.0;
        double zc = 0.0;
    };

    LoadBodyX() = default;

    std::string getKeywordName() const override { return "*LOAD_BODY_X"; }

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
 * @brief *LOAD_BODY_Y
 */
class KOO_API LoadBodyY : public CloneableKeyword<LoadBodyY, LoadKeyword> {
public:
    struct Data {
        int lcid = 0;
        double sf = 1.0;
        int lciddr = 0;
        double xc = 0.0;
        double yc = 0.0;
        double zc = 0.0;
    };

    LoadBodyY() = default;

    std::string getKeywordName() const override { return "*LOAD_BODY_Y"; }

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
 * @brief Segment load data
 */
struct SegmentLoadData {
    int lcid = 0;               // Load curve ID
    double sf = 1.0;            // Scale factor
    double at = 0.0;            // Activation time
    NodeId n1 = 0;              // Node 1
    NodeId n2 = 0;              // Node 2
    NodeId n3 = 0;              // Node 3
    NodeId n4 = 0;              // Node 4 (0 for triangular segments)
};

/**
 * @brief *LOAD_SEGMENT
 *
 * Pressure load on segment faces.
 */
class KOO_API LoadSegment : public CloneableKeyword<LoadSegment, LoadKeyword> {
public:
    LoadSegment() = default;

    std::string getKeywordName() const override { return "*LOAD_SEGMENT"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    std::vector<SegmentLoadData>& getLoads() { return loads_; }
    const std::vector<SegmentLoadData>& getLoads() const { return loads_; }

    void addLoad(const SegmentLoadData& load) { loads_.push_back(load); }
    void clear() { loads_.clear(); }

private:
    std::vector<SegmentLoadData> loads_;
};

/**
 * @brief *LOAD_SEGMENT_SET
 *
 * Pressure load on segment set.
 */
class KOO_API LoadSegmentSet : public CloneableKeyword<LoadSegmentSet, LoadKeyword> {
public:
    struct Data {
        int ssid = 0;           // Segment set ID
        int lcid = 0;           // Load curve ID
        double sf = 1.0;        // Scale factor
        double at = 0.0;        // Activation time
    };

    LoadSegmentSet() = default;

    std::string getKeywordName() const override { return "*LOAD_SEGMENT_SET"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    std::vector<Data>& getLoads() { return loads_; }
    const std::vector<Data>& getLoads() const { return loads_; }

    void addLoad(const Data& load) { loads_.push_back(load); }
    void clear() { loads_.clear(); }

private:
    std::vector<Data> loads_;
};

/**
 * @brief *LOAD_SHELL_SET
 *
 * Pressure load on shell element set.
 */
class KOO_API LoadShellSet : public CloneableKeyword<LoadShellSet, LoadKeyword> {
public:
    struct Data {
        int esid = 0;           // Element set ID
        int lcid = 0;           // Load curve ID
        double sf = 1.0;        // Scale factor
        double at = 0.0;        // Activation time
    };

    LoadShellSet() = default;

    std::string getKeywordName() const override { return "*LOAD_SHELL_SET"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    std::vector<Data>& getLoads() { return loads_; }
    const std::vector<Data>& getLoads() const { return loads_; }

    void addLoad(const Data& load) { loads_.push_back(load); }
    void clear() { loads_.clear(); }

private:
    std::vector<Data> loads_;
};

/**
 * @brief *LOAD_RIGID_BODY
 *
 * Load applied to rigid body.
 */
class KOO_API LoadRigidBody : public CloneableKeyword<LoadRigidBody, LoadKeyword> {
public:
    struct Data {
        PartId pid = 0;         // Part ID of rigid body
        int dof = 0;            // Degree of freedom
        int lcid = 0;           // Load curve ID
        double sf = 1.0;        // Scale factor
        int cid = 0;            // Coordinate system ID
        int m1 = 0;             // Method 1
        int m2 = 0;             // Method 2
        int m3 = 0;             // Method 3
    };

    LoadRigidBody() = default;

    std::string getKeywordName() const override { return "*LOAD_RIGID_BODY"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    std::vector<Data>& getLoads() { return loads_; }
    const std::vector<Data>& getLoads() const { return loads_; }

    void addLoad(const Data& load) { loads_.push_back(load); }
    void clear() { loads_.clear(); }

private:
    std::vector<Data> loads_;
};

/**
 * @brief *LOAD_THERMAL_VARIABLE
 *
 * Variable thermal load.
 */
class KOO_API LoadThermalVariable : public CloneableKeyword<LoadThermalVariable, LoadKeyword> {
public:
    struct Data {
        int nsid = 0;           // Node set ID
        int lcid = 0;           // Load curve ID
        double sf = 1.0;        // Scale factor
        int death = 0;          // Death time flag
    };

    LoadThermalVariable() = default;

    std::string getKeywordName() const override { return "*LOAD_THERMAL_VARIABLE"; }

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
 * @brief *LOAD_MOTION_NODE
 *
 * Prescribed motion on nodes.
 */
class KOO_API LoadMotionNode : public CloneableKeyword<LoadMotionNode, LoadKeyword> {
public:
    struct Data {
        NodeId nid = 0;         // Node ID
        int dof = 0;            // Degree of freedom
        int vad = 0;            // Velocity/acceleration/displacement flag
        int lcid = 0;           // Load curve ID
        double sf = 1.0;        // Scale factor
        int vid = 0;            // Vector ID
        int death = 0;          // Death time
        int birth = 0;          // Birth time
    };

    LoadMotionNode() = default;

    std::string getKeywordName() const override { return "*LOAD_MOTION_NODE"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    std::vector<Data>& getLoads() { return loads_; }
    const std::vector<Data>& getLoads() const { return loads_; }

private:
    std::vector<Data> loads_;
};

/**
 * @brief *LOAD_BEAM_SET
 *
 * Distributed load on beam elements.
 */
class KOO_API LoadBeamSet : public CloneableKeyword<LoadBeamSet, LoadKeyword> {
public:
    struct Data {
        int bsid = 0;           // Beam set ID
        int dir = 0;            // Load direction
        int lcid = 0;           // Load curve ID
        double sf = 1.0;        // Scale factor
        int cid = 0;            // Coordinate system
    };

    LoadBeamSet() = default;

    std::string getKeywordName() const override { return "*LOAD_BEAM_SET"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    std::vector<Data>& getLoads() { return loads_; }
    const std::vector<Data>& getLoads() const { return loads_; }

private:
    std::vector<Data> loads_;
};

/**
 * @brief *LOAD_BODY_PARTS
 *
 * Body force on specific part set.
 */
class KOO_API LoadBodyParts : public CloneableKeyword<LoadBodyParts, LoadKeyword> {
public:
    struct Data {
        int psid = 0;           // Part set ID
        int lcidx = 0;          // Load curve X direction
        int lcidy = 0;          // Load curve Y direction
        int lcidz = 0;          // Load curve Z direction
        double sfx = 1.0;       // Scale factor X
        double sfy = 1.0;       // Scale factor Y
        double sfz = 1.0;       // Scale factor Z
    };

    LoadBodyParts() = default;

    std::string getKeywordName() const override { return "*LOAD_BODY_PARTS"; }

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
 * @brief *LOAD_THERMAL_CONSTANT
 *
 * Constant temperature load.
 */
class KOO_API LoadThermalConstant : public CloneableKeyword<LoadThermalConstant, LoadKeyword> {
public:
    struct Data {
        int nsid = 0;           // Node set ID
        double temp = 0.0;      // Temperature value
    };

    LoadThermalConstant() = default;

    std::string getKeywordName() const override { return "*LOAD_THERMAL_CONSTANT"; }

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
 * @brief *LOAD_GRAVITY_PART
 *
 * Gravity load on specific parts.
 */
class KOO_API LoadGravityPart : public CloneableKeyword<LoadGravityPart, LoadKeyword> {
public:
    struct Data {
        int pid = 0;            // Part ID
        int n1 = 0;             // First node for direction
        int n2 = 0;             // Second node for direction
        int lcid = 0;           // Load curve ID
        double sf = 1.0;        // Scale factor
        int cid = 0;            // Coordinate system
    };

    LoadGravityPart() = default;

    std::string getKeywordName() const override { return "*LOAD_GRAVITY_PART"; }

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
 * @brief *LOAD_DENSITY_DEPTH
 *
 * Density based on depth for geotechnical applications.
 */
class KOO_API LoadDensityDepth : public CloneableKeyword<LoadDensityDepth, LoadKeyword> {
public:
    struct Data {
        int pid = 0;            // Part ID
        double d = 0.0;         // Depth at which density starts
        double rho = 0.0;       // Density
        double c = 0.0;         // Gravity constant
    };

    LoadDensityDepth() = default;

    std::string getKeywordName() const override { return "*LOAD_DENSITY_DEPTH"; }

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
 * @brief *LOAD_SEISMIC_SSI
 *
 * Seismic soil-structure interaction load.
 */
class KOO_API LoadSeismicSsi : public CloneableKeyword<LoadSeismicSsi, LoadKeyword> {
public:
    struct Data {
        int ssid = 0;           // Segment set ID
        int gmset = 0;          // Ground motion set
        int sf = 1;             // Scale factor flag
        int btype = 0;          // Boundary type
    };

    LoadSeismicSsi() = default;

    std::string getKeywordName() const override { return "*LOAD_SEISMIC_SSI"; }

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
 * @brief *LOAD_BLAST_ENHANCED
 *
 * Enhanced blast load model.
 */
class KOO_API LoadBlastEnhanced : public CloneableKeyword<LoadBlastEnhanced, LoadKeyword> {
public:
    struct Data {
        int bid = 0;            // Blast ID
        double m = 0.0;         // Mass of explosive
        double xbo = 0.0;       // X coordinate of blast
        double ybo = 0.0;       // Y coordinate of blast
        double zbo = 0.0;       // Z coordinate of blast
        double tbo = 0.0;       // Time of detonation
        int unit = 0;           // Unit system
        int blast = 0;          // Blast model type
    };

    LoadBlastEnhanced() = default;

    std::string getKeywordName() const override { return "*LOAD_BLAST_ENHANCED"; }

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
 * @brief *LOAD_SPCFORCE
 *
 * Force at SPC locations.
 */
class KOO_API LoadSpcForce : public CloneableKeyword<LoadSpcForce, LoadKeyword> {
public:
    struct Data {
        int nsid = 0;           // Node set ID
        int dof = 0;            // Degree of freedom
        int lcid = 0;           // Load curve ID
        double sf = 1.0;        // Scale factor
        int cid = 0;            // Coordinate system
    };

    LoadSpcForce() = default;

    std::string getKeywordName() const override { return "*LOAD_SPCFORCE"; }

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
 * @brief *LOAD_SURFACE_STRESS
 *
 * Surface stress load.
 */
class KOO_API LoadSurfaceStress : public CloneableKeyword<LoadSurfaceStress, LoadKeyword> {
public:
    struct Data {
        int ssid = 0;           // Segment set ID
        int lcidxx = 0;         // Load curve for sigmaxx
        int lcidyy = 0;         // Load curve for sigmayy
        int lcidzz = 0;         // Load curve for sigmazz
        int lcidxy = 0;         // Load curve for sigmaxy
        int lcidyz = 0;         // Load curve for sigmayz
        int lcidzx = 0;         // Load curve for sigmazx
    };

    LoadSurfaceStress() = default;

    std::string getKeywordName() const override { return "*LOAD_SURFACE_STRESS"; }

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
 * @brief *LOAD_MOVING_PRESSURE
 *
 * Moving pressure load for vehicle dynamics.
 */
class KOO_API LoadMovingPressure : public CloneableKeyword<LoadMovingPressure, LoadKeyword> {
public:
    struct Data {
        int pid = 0;            // Part ID
        int lcid = 0;           // Load curve ID for magnitude
        double sf = 1.0;        // Scale factor
        int dirl = 0;           // Direction of load
        double vel = 0.0;       // Velocity of moving load
        int lcidv = 0;          // Load curve for velocity
    };

    LoadMovingPressure() = default;

    std::string getKeywordName() const override { return "*LOAD_MOVING_PRESSURE"; }

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
 * @brief *LOAD_BRODE
 *
 * Brode blast wave loading.
 */
class KOO_API LoadBrode : public CloneableKeyword<LoadBrode, LoadKeyword> {
public:
    struct Data {
        int ssid = 0;           // Segment set ID
        double xb = 0.0;        // X coordinate of burst
        double yb = 0.0;        // Y coordinate of burst
        double zb = 0.0;        // Z coordinate of burst
        double tb = 0.0;        // Time of burst
        double p0 = 0.0;        // Ambient pressure
        double e0 = 0.0;        // Energy of explosion
    };

    LoadBrode() = default;

    std::string getKeywordName() const override { return "*LOAD_BRODE"; }

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
 * @brief *LOAD_THERMAL_BINOUT
 *
 * Thermal load from binout file.
 */
class KOO_API LoadThermalBinout : public CloneableKeyword<LoadThermalBinout, LoadKeyword> {
public:
    struct Data {
        int lcid = 0;           // Load curve ID
        double sf = 1.0;        // Scale factor
        int psid = 0;           // Part set ID
        std::string filename;   // Binout filename
    };

    LoadThermalBinout() = default;

    std::string getKeywordName() const override { return "*LOAD_THERMAL_BINOUT"; }

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
 * @brief *LOAD_ERODING_PART_SET
 *
 * Load applied to eroding part set.
 */
class KOO_API LoadErodingPartSet : public CloneableKeyword<LoadErodingPartSet, LoadKeyword> {
public:
    struct Data {
        int psid = 0;           // Part set ID
        int lcid = 0;           // Load curve ID
        double sf = 1.0;        // Scale factor
        int dof = 0;            // Degree of freedom
        int cid = 0;            // Coordinate system ID
    };

    LoadErodingPartSet() = default;

    std::string getKeywordName() const override { return "*LOAD_ERODING_PART_SET"; }

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
 * @brief *LOAD_HEAT_GENERATION_SET
 *
 * Heat generation load for thermal analysis.
 */
class KOO_API LoadHeatGenerationSet : public CloneableKeyword<LoadHeatGenerationSet, LoadKeyword> {
public:
    struct Data {
        int psid = 0;           // Part set ID
        int lcid = 0;           // Load curve ID for heat generation
        double sf = 1.0;        // Scale factor
        int birth = 0;          // Birth time load curve
        int death = 0;          // Death time load curve
    };

    LoadHeatGenerationSet() = default;

    std::string getKeywordName() const override { return "*LOAD_HEAT_GENERATION_SET"; }

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
 * @brief *LOAD_MASK
 *
 * Load mask for selective load application.
 */
class KOO_API LoadMask : public CloneableKeyword<LoadMask, LoadKeyword> {
public:
    struct Data {
        int id = 0;             // Mask ID
        int type = 0;           // Mask type
        int ssid = 0;           // Segment set ID
        int box = 0;            // Box ID
        double value = 0.0;     // Mask value
    };

    LoadMask() = default;

    std::string getKeywordName() const override { return "*LOAD_MASK"; }

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
 * @brief *LOAD_REMOVE_PART
 *
 * Remove load from parts.
 */
class KOO_API LoadRemovePart : public CloneableKeyword<LoadRemovePart, LoadKeyword> {
public:
    struct Data {
        int pid = 0;            // Part ID
        double time = 0.0;      // Time of removal
    };

    LoadRemovePart() = default;

    std::string getKeywordName() const override { return "*LOAD_REMOVE_PART"; }

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
 * @brief *LOAD_SUPERPLASTIC_FORMING
 *
 * Superplastic forming load.
 */
class KOO_API LoadSuperplasticForming : public CloneableKeyword<LoadSuperplasticForming, LoadKeyword> {
public:
    struct Data {
        int psid = 0;           // Part set ID
        int lcid = 0;           // Load curve ID for pressure
        double maxp = 0.0;      // Maximum pressure
        double rate = 0.0;      // Target strain rate
        int dtefac = 0;         // Time step factor
    };

    LoadSuperplasticForming() = default;

    std::string getKeywordName() const override { return "*LOAD_SUPERPLASTIC_FORMING"; }

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
 * @brief *LOAD_STEADY_STATE_ROLLING
 *
 * Steady state rolling load.
 */
class KOO_API LoadSteadyStateRolling : public CloneableKeyword<LoadSteadyStateRolling, LoadKeyword> {
public:
    struct Data {
        int psid = 0;           // Part set ID
        double radius = 0.0;    // Rolling radius
        double omega = 0.0;     // Angular velocity
        double vel = 0.0;       // Translational velocity
        int lcid = 0;           // Load curve ID
        int cid = 0;            // Coordinate system ID
    };

    LoadSteadyStateRolling() = default;

    std::string getKeywordName() const override { return "*LOAD_STEADY_STATE_ROLLING"; }

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
 * @brief *LOAD_SEGMENT_NONUNIFORM
 *
 * Non-uniform pressure load on segments.
 */
class KOO_API LoadSegmentNonuniform : public CloneableKeyword<LoadSegmentNonuniform, LoadKeyword> {
public:
    struct Data {
        int ssid = 0;           // Segment set ID
        int lcid = 0;           // Load curve ID
        double sf = 1.0;        // Scale factor
        int at = 0;             // Application type
        int lcidn1 = 0;         // Load curve for node 1
        int lcidn2 = 0;         // Load curve for node 2
        int lcidn3 = 0;         // Load curve for node 3
        int lcidn4 = 0;         // Load curve for node 4
    };

    LoadSegmentNonuniform() = default;

    std::string getKeywordName() const override { return "*LOAD_SEGMENT_NONUNIFORM"; }

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
 * @brief *LOAD_ALE_CONVECTION
 *
 * ALE convection load.
 */
class KOO_API LoadAleConvection : public CloneableKeyword<LoadAleConvection, LoadKeyword> {
public:
    struct Data {
        int ssid = 0;           // Segment set ID
        int lcid = 0;           // Load curve ID for heat transfer
        double sf = 1.0;        // Scale factor
        double tamb = 0.0;      // Ambient temperature
    };

    LoadAleConvection() = default;

    std::string getKeywordName() const override { return "*LOAD_ALE_CONVECTION"; }

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
 * @brief *LOAD_NEGATIVE_VOLUME
 *
 * Negative volume load for airbag analysis.
 */
class KOO_API LoadNegativeVolume : public CloneableKeyword<LoadNegativeVolume, LoadKeyword> {
public:
    struct Data {
        int psid = 0;           // Part set ID
        int option = 0;         // Control option
        double scale = 1.0;     // Scale factor
    };

    LoadNegativeVolume() = default;

    std::string getKeywordName() const override { return "*LOAD_NEGATIVE_VOLUME"; }

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
 * @brief *LOAD_ACOUSTIC_SOURCE
 *
 * Acoustic source load.
 */
class KOO_API LoadAcousticSource : public CloneableKeyword<LoadAcousticSource, LoadKeyword> {
public:
    struct Data {
        int sid = 0;            // Source ID
        NodeId nid = 0;         // Node ID
        int lcid = 0;           // Load curve ID
        double sf = 1.0;        // Scale factor
        int type = 0;           // Source type
    };

    LoadAcousticSource() = default;

    std::string getKeywordName() const override { return "*LOAD_ACOUSTIC_SOURCE"; }

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
 * @brief *LOAD_BEAM
 *
 * Distributed load on beam elements.
 */
class KOO_API LoadBeam : public CloneableKeyword<LoadBeam, LoadKeyword> {
public:
    struct Data {
        ElementId eid = 0;      // Element ID
        int dir = 0;            // Load direction
        int lcid = 0;           // Load curve ID
        double sf = 1.0;        // Scale factor
        int cid = 0;            // Coordinate system
    };

    LoadBeam() = default;

    std::string getKeywordName() const override { return "*LOAD_BEAM"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    std::vector<Data>& getLoads() { return loads_; }
    const std::vector<Data>& getLoads() const { return loads_; }

private:
    std::vector<Data> loads_;
};

/**
 * @brief *LOAD_BLAST
 *
 * Basic blast wave load.
 */
class KOO_API LoadBlast : public CloneableKeyword<LoadBlast, LoadKeyword> {
public:
    struct Data {
        int bid = 0;            // Blast ID
        double m = 0.0;         // Mass of explosive
        double xbo = 0.0;       // X coordinate of blast
        double ybo = 0.0;       // Y coordinate of blast
        double zbo = 0.0;       // Z coordinate of blast
        double tbo = 0.0;       // Time of detonation
    };

    LoadBlast() = default;

    std::string getKeywordName() const override { return "*LOAD_BLAST"; }

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
 * @brief *LOAD_BLAST_SEGMENT_SET
 *
 * Blast load on segment set.
 */
class KOO_API LoadBlastSegmentSet : public CloneableKeyword<LoadBlastSegmentSet, LoadKeyword> {
public:
    struct Data {
        int bid = 0;            // Blast ID
        int ssid = 0;           // Segment set ID
        double sf = 1.0;        // Scale factor
    };

    LoadBlastSegmentSet() = default;

    std::string getKeywordName() const override { return "*LOAD_BLAST_SEGMENT_SET"; }

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
 * @brief *LOAD_BODY_GENERALIZED
 *
 * Generalized body force.
 */
class KOO_API LoadBodyGeneralized : public CloneableKeyword<LoadBodyGeneralized, LoadKeyword> {
public:
    struct Data {
        int lcid = 0;           // Load curve ID
        double sf = 1.0;        // Scale factor
        int cid = 0;            // Coordinate system
        double ax = 0.0;        // Acceleration X
        double ay = 0.0;        // Acceleration Y
        double az = 0.0;        // Acceleration Z
    };

    LoadBodyGeneralized() = default;

    std::string getKeywordName() const override { return "*LOAD_BODY_GENERALIZED"; }

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
 * @brief *LOAD_BODY_POROUS
 *
 * Body force for porous materials.
 */
class KOO_API LoadBodyPorous : public CloneableKeyword<LoadBodyPorous, LoadKeyword> {
public:
    struct Data {
        int psid = 0;           // Part set ID
        int lcidx = 0;          // Load curve X
        int lcidy = 0;          // Load curve Y
        int lcidz = 0;          // Load curve Z
        double sfx = 1.0;       // Scale factor X
        double sfy = 1.0;       // Scale factor Y
        double sfz = 1.0;       // Scale factor Z
    };

    LoadBodyPorous() = default;

    std::string getKeywordName() const override { return "*LOAD_BODY_POROUS"; }

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
 * @brief *LOAD_EXPANSION_PRESSURE
 *
 * Expansion pressure load.
 */
class KOO_API LoadExpansionPressure : public CloneableKeyword<LoadExpansionPressure, LoadKeyword> {
public:
    struct Data {
        int psid = 0;           // Part set ID
        int lcid = 0;           // Load curve ID
        double sf = 1.0;        // Scale factor
        int opt = 0;            // Option
    };

    LoadExpansionPressure() = default;

    std::string getKeywordName() const override { return "*LOAD_EXPANSION_PRESSURE"; }

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
 * @brief *LOAD_HEAT_CONTROLLER
 *
 * Heat controller load.
 */
class KOO_API LoadHeatController : public CloneableKeyword<LoadHeatController, LoadKeyword> {
public:
    struct Data {
        int hcid = 0;           // Heat controller ID
        int nsid = 0;           // Node set ID
        int lcid = 0;           // Load curve ID
        double sf = 1.0;        // Scale factor
        double target = 0.0;    // Target temperature
    };

    LoadHeatController() = default;

    std::string getKeywordName() const override { return "*LOAD_HEAT_CONTROLLER"; }

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
 * @brief *LOAD_HEAT_EXOTHERMIC_REACTION
 *
 * Heat from exothermic reaction.
 */
class KOO_API LoadHeatExothermicReaction : public CloneableKeyword<LoadHeatExothermicReaction, LoadKeyword> {
public:
    struct Data {
        int psid = 0;           // Part set ID
        int lcid = 0;           // Load curve ID
        double sf = 1.0;        // Scale factor
        double q0 = 0.0;        // Heat generation rate
    };

    LoadHeatExothermicReaction() = default;

    std::string getKeywordName() const override { return "*LOAD_HEAT_EXOTHERMIC_REACTION"; }

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
 * @brief *LOAD_HEAT_GENERATION
 *
 * Heat generation load.
 */
class KOO_API LoadHeatGeneration : public CloneableKeyword<LoadHeatGeneration, LoadKeyword> {
public:
    struct Data {
        ElementId eid = 0;      // Element ID
        int lcid = 0;           // Load curve ID
        double sf = 1.0;        // Scale factor
    };

    LoadHeatGeneration() = default;

    std::string getKeywordName() const override { return "*LOAD_HEAT_GENERATION"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    std::vector<Data>& getLoads() { return loads_; }
    const std::vector<Data>& getLoads() const { return loads_; }

private:
    std::vector<Data> loads_;
};

/**
 * @brief *LOAD_NODE
 *
 * Generic node load (base).
 */
class KOO_API LoadNode : public CloneableKeyword<LoadNode, LoadKeyword> {
public:
    struct Data {
        NodeId nid = 0;         // Node ID
        int dof = 0;            // Degree of freedom
        int lcid = 0;           // Load curve ID
        double sf = 1.0;        // Scale factor
        int cid = 0;            // Coordinate system
    };

    LoadNode() = default;

    std::string getKeywordName() const override { return "*LOAD_NODE"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    std::vector<Data>& getLoads() { return loads_; }
    const std::vector<Data>& getLoads() const { return loads_; }

private:
    std::vector<Data> loads_;
};

/**
 * @brief *LOAD_PZE
 *
 * Piezoelectric load.
 */
class KOO_API LoadPze : public CloneableKeyword<LoadPze, LoadKeyword> {
public:
    struct Data {
        int nsid = 0;           // Node set ID
        int lcid = 0;           // Load curve ID
        double sf = 1.0;        // Scale factor
        int opt = 0;            // Option
    };

    LoadPze() = default;

    std::string getKeywordName() const override { return "*LOAD_PZE"; }

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
 * @brief *LOAD_SEGMENT_FILE
 *
 * Segment load from file.
 */
class KOO_API LoadSegmentFile : public CloneableKeyword<LoadSegmentFile, LoadKeyword> {
public:
    struct Data {
        int ssid = 0;           // Segment set ID
        std::string filename;   // File name
        double sf = 1.0;        // Scale factor
    };

    LoadSegmentFile() = default;

    std::string getKeywordName() const override { return "*LOAD_SEGMENT_FILE"; }

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
 * @brief *LOAD_SEGMENT_SET_ANGLE
 *
 * Segment set load with angle dependence.
 */
class KOO_API LoadSegmentSetAngle : public CloneableKeyword<LoadSegmentSetAngle, LoadKeyword> {
public:
    struct Data {
        int ssid = 0;           // Segment set ID
        int lcid = 0;           // Load curve ID
        double sf = 1.0;        // Scale factor
        int angle = 0;          // Angle type
        int lcang = 0;          // Angle load curve
    };

    LoadSegmentSetAngle() = default;

    std::string getKeywordName() const override { return "*LOAD_SEGMENT_SET_ANGLE"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    std::vector<Data>& getLoads() { return loads_; }
    const std::vector<Data>& getLoads() const { return loads_; }

private:
    std::vector<Data> loads_;
};

/**
 * @brief *LOAD_SEISMIC_SSI_AUX
 *
 * Auxiliary seismic SSI load.
 */
class KOO_API LoadSeismicSsiAux : public CloneableKeyword<LoadSeismicSsiAux, LoadKeyword> {
public:
    struct Data {
        int ssid = 0;           // Segment set ID
        int gmset = 0;          // Ground motion set
        int type = 0;           // Type
    };

    LoadSeismicSsiAux() = default;

    std::string getKeywordName() const override { return "*LOAD_SEISMIC_SSI_AUX"; }

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
 * @brief *LOAD_SHELL_ELEMENT
 *
 * Load on individual shell elements.
 */
class KOO_API LoadShellElement : public CloneableKeyword<LoadShellElement, LoadKeyword> {
public:
    struct Data {
        ElementId eid = 0;      // Element ID
        int lcid = 0;           // Load curve ID
        double sf = 1.0;        // Scale factor
        int at = 0;             // Application type
    };

    LoadShellElement() = default;

    std::string getKeywordName() const override { return "*LOAD_SHELL_ELEMENT"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    std::vector<Data>& getLoads() { return loads_; }
    const std::vector<Data>& getLoads() const { return loads_; }

private:
    std::vector<Data> loads_;
};

/**
 * @brief *LOAD_STIFFEN_PART
 *
 * Stiffen part load.
 */
class KOO_API LoadStiffenPart : public CloneableKeyword<LoadStiffenPart, LoadKeyword> {
public:
    struct Data {
        int psid = 0;           // Part set ID
        int lcid = 0;           // Load curve ID
        double sf = 1.0;        // Scale factor
    };

    LoadStiffenPart() = default;

    std::string getKeywordName() const override { return "*LOAD_STIFFEN_PART"; }

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
 * @brief *LOAD_THERMAL_D3PLOT
 *
 * Thermal load from d3plot file.
 */
class KOO_API LoadThermalD3plot : public CloneableKeyword<LoadThermalD3plot, LoadKeyword> {
public:
    struct Data {
        int lcid = 0;           // Load curve ID
        double sf = 1.0;        // Scale factor
        int psid = 0;           // Part set ID
        std::string filename;   // D3plot filename
    };

    LoadThermalD3plot() = default;

    std::string getKeywordName() const override { return "*LOAD_THERMAL_D3PLOT"; }

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
 * @brief *LOAD_THERMAL_LOAD_CURVE
 *
 * Thermal load using load curve.
 */
class KOO_API LoadThermalLoadCurve : public CloneableKeyword<LoadThermalLoadCurve, LoadKeyword> {
public:
    struct Data {
        int nsid = 0;           // Node set ID
        int lcid = 0;           // Load curve ID
        double sf = 1.0;        // Scale factor
    };

    LoadThermalLoadCurve() = default;

    std::string getKeywordName() const override { return "*LOAD_THERMAL_LOAD_CURVE"; }

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
 * @brief *LOAD_THERMAL_RSW
 *
 * Thermal resistance spot weld load.
 */
class KOO_API LoadThermalRsw : public CloneableKeyword<LoadThermalRsw, LoadKeyword> {
public:
    struct Data {
        int nsid = 0;           // Node set ID
        int lcid = 0;           // Load curve ID
        double sf = 1.0;        // Scale factor
        int opt = 0;            // Option
    };

    LoadThermalRsw() = default;

    std::string getKeywordName() const override { return "*LOAD_THERMAL_RSW"; }

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
 * @brief *LOAD_THERMAL_TOPAZ
 *
 * Thermal load from TOPAZ.
 */
class KOO_API LoadThermalTopaz : public CloneableKeyword<LoadThermalTopaz, LoadKeyword> {
public:
    struct Data {
        int lcid = 0;           // Load curve ID
        double sf = 1.0;        // Scale factor
        std::string filename;   // TOPAZ filename
    };

    LoadThermalTopaz() = default;

    std::string getKeywordName() const override { return "*LOAD_THERMAL_TOPAZ"; }

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
 * @brief *LOAD_THERMAL_VARIABLE_BEAM
 *
 * Variable thermal load on beam elements.
 */
class KOO_API LoadThermalVariableBeam : public CloneableKeyword<LoadThermalVariableBeam, LoadKeyword> {
public:
    struct Data {
        int bsid = 0;           // Beam set ID
        int lcid = 0;           // Load curve ID
        double sf = 1.0;        // Scale factor
    };

    LoadThermalVariableBeam() = default;

    std::string getKeywordName() const override { return "*LOAD_THERMAL_VARIABLE_BEAM"; }

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
 * @brief *LOAD_THERMAL_VARIABLE_NODE
 *
 * Variable thermal load on nodes.
 */
class KOO_API LoadThermalVariableNode : public CloneableKeyword<LoadThermalVariableNode, LoadKeyword> {
public:
    struct Data {
        NodeId nid = 0;         // Node ID
        int lcid = 0;           // Load curve ID
        double sf = 1.0;        // Scale factor
    };

    LoadThermalVariableNode() = default;

    std::string getKeywordName() const override { return "*LOAD_THERMAL_VARIABLE_NODE"; }

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
 * @brief *LOAD_THERMAL_VARIABLE_SHELL
 *
 * Variable thermal load on shell elements.
 */
class KOO_API LoadThermalVariableShell : public CloneableKeyword<LoadThermalVariableShell, LoadKeyword> {
public:
    struct Data {
        int esid = 0;           // Element set ID
        int lcid = 0;           // Load curve ID
        double sf = 1.0;        // Scale factor
    };

    LoadThermalVariableShell() = default;

    std::string getKeywordName() const override { return "*LOAD_THERMAL_VARIABLE_SHELL"; }

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
 * @brief *LOAD_VOLUME_LOSS
 *
 * Volume loss load.
 */
class KOO_API LoadVolumeLoss : public CloneableKeyword<LoadVolumeLoss, LoadKeyword> {
public:
    struct Data {
        int psid = 0;           // Part set ID
        int lcid = 0;           // Load curve ID
        double sf = 1.0;        // Scale factor
        int opt = 0;            // Option
    };

    LoadVolumeLoss() = default;

    std::string getKeywordName() const override { return "*LOAD_VOLUME_LOSS"; }

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
 * @brief *LOAD_BODY_VECTOR
 *
 * Body force in vector direction.
 */
class KOO_API LoadBodyVector : public CloneableKeyword<LoadBodyVector, LoadKeyword> {
public:
    struct Data {
        int lcid = 0;           // Load curve ID
        double sf = 1.0;        // Scale factor
        int vid = 0;            // Vector ID
        double vx = 0.0;        // X component of direction
        double vy = 0.0;        // Y component of direction
        double vz = 0.0;        // Z component of direction
    };

    LoadBodyVector() = default;

    std::string getKeywordName() const override { return "*LOAD_BODY_VECTOR"; }

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
 * @brief *LOAD_SEGMENT_ID
 *
 * Segment load with ID.
 */
class KOO_API LoadSegmentId : public CloneableKeyword<LoadSegmentId, LoadKeyword> {
public:
    struct Data {
        int id = 0;             // Load ID
        std::string heading;    // Heading
        int lcid = 0;           // Load curve ID
        double sf = 1.0;        // Scale factor
        double at = 0.0;        // Activation time
        NodeId n1 = 0;          // Node 1
        NodeId n2 = 0;          // Node 2
        NodeId n3 = 0;          // Node 3
        NodeId n4 = 0;          // Node 4
    };

    LoadSegmentId() = default;

    std::string getKeywordName() const override { return "*LOAD_SEGMENT_ID"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    std::vector<Data>& getLoads() { return loads_; }
    const std::vector<Data>& getLoads() const { return loads_; }

private:
    std::vector<Data> loads_;
};

/**
 * @brief *LOAD_THERMAL_CONSTANT_NODE
 *
 * Constant thermal load on nodes.
 */
class KOO_API LoadThermalConstantNode : public CloneableKeyword<LoadThermalConstantNode, LoadKeyword> {
public:
    struct Data {
        NodeId nid = 0;         // Node ID
        double temp = 0.0;      // Temperature
    };

    LoadThermalConstantNode() = default;

    std::string getKeywordName() const override { return "*LOAD_THERMAL_CONSTANT_NODE"; }

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
 * @brief *LOAD_THERMAL_ELEMENT
 *
 * Thermal load on elements.
 */
class KOO_API LoadThermalElement : public CloneableKeyword<LoadThermalElement, LoadKeyword> {
public:
    struct Data {
        ElementId eid = 0;      // Element ID
        int lcid = 0;           // Load curve ID
        double sf = 1.0;        // Scale factor
    };

    LoadThermalElement() = default;

    std::string getKeywordName() const override { return "*LOAD_THERMAL_ELEMENT"; }

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
 * @brief *LOAD_THERMAL_VARIABLE_ELEMENT
 *
 * Variable thermal load on elements.
 */
class KOO_API LoadThermalVariableElement : public CloneableKeyword<LoadThermalVariableElement, LoadKeyword> {
public:
    struct Data {
        ElementId eid = 0;      // Element ID
        int lcid = 0;           // Load curve ID
        double sf = 1.0;        // Scale factor
    };

    LoadThermalVariableElement() = default;

    std::string getKeywordName() const override { return "*LOAD_THERMAL_VARIABLE_ELEMENT"; }

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
 * @brief *LOAD_DENSITY
 *
 * Density based loading.
 */
class KOO_API LoadDensity : public CloneableKeyword<LoadDensity, LoadKeyword> {
public:
    struct Data {
        int psid = 0;           // Part set ID
        int lcid = 0;           // Load curve ID
        double sf = 1.0;        // Scale factor
    };

    LoadDensity() = default;

    std::string getKeywordName() const override { return "*LOAD_DENSITY"; }

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
 * @brief *LOAD_GRAVITY
 *
 * Global gravity load.
 */
class KOO_API LoadGravity : public CloneableKeyword<LoadGravity, LoadKeyword> {
public:
    struct Data {
        int lcid = 0;           // Load curve ID
        double sf = 1.0;        // Scale factor
        double gx = 0.0;        // Gravity in X
        double gy = 0.0;        // Gravity in Y
        double gz = 0.0;        // Gravity in Z
    };

    LoadGravity() = default;

    std::string getKeywordName() const override { return "*LOAD_GRAVITY"; }

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
 * @brief *LOAD_SSA
 *
 * Steady-state acceleration load.
 */
class KOO_API LoadSsa : public CloneableKeyword<LoadSsa, LoadKeyword> {
public:
    struct Data {
        int psid = 0;           // Part set ID
        int lcidx = 0;          // Load curve X
        int lcidy = 0;          // Load curve Y
        int lcidz = 0;          // Load curve Z
        double sfx = 1.0;       // Scale factor X
        double sfy = 1.0;       // Scale factor Y
        double sfz = 1.0;       // Scale factor Z
    };

    LoadSsa() = default;

    std::string getKeywordName() const override { return "*LOAD_SSA"; }

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
 * @brief *LOAD_SSA_GRAVITY
 *
 * Steady-state gravity acceleration.
 */
class KOO_API LoadSsaGravity : public CloneableKeyword<LoadSsaGravity, LoadKeyword> {
public:
    struct Data {
        int psid = 0;           // Part set ID
        int lcid = 0;           // Load curve ID
        double sf = 1.0;        // Scale factor
        double gx = 0.0;        // Gravity X
        double gy = 0.0;        // Gravity Y
        double gz = 0.0;        // Gravity Z
    };

    LoadSsaGravity() = default;

    std::string getKeywordName() const override { return "*LOAD_SSA_GRAVITY"; }

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
 * @brief *LOAD_SEISMIC
 *
 * Seismic load.
 */
class KOO_API LoadSeismic : public CloneableKeyword<LoadSeismic, LoadKeyword> {
public:
    struct Data {
        int nsid = 0;           // Node set ID
        int gmset = 0;          // Ground motion set
        int dof = 0;            // Degree of freedom
        int sf = 1;             // Scale factor
    };

    LoadSeismic() = default;

    std::string getKeywordName() const override { return "*LOAD_SEISMIC"; }

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
 * @brief *LOAD_BODY_RX
 *
 * Rotational body force around X axis.
 */
class KOO_API LoadBodyRx : public CloneableKeyword<LoadBodyRx, LoadKeyword> {
public:
    struct Data {
        int lcid = 0;           // Load curve ID
        double sf = 1.0;        // Scale factor
        double xc = 0.0;        // X center
        double yc = 0.0;        // Y center
        double zc = 0.0;        // Z center
    };

    LoadBodyRx() = default;

    std::string getKeywordName() const override { return "*LOAD_BODY_RX"; }

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
 * @brief *LOAD_BODY_RY
 *
 * Rotational body force around Y axis.
 */
class KOO_API LoadBodyRy : public CloneableKeyword<LoadBodyRy, LoadKeyword> {
public:
    struct Data {
        int lcid = 0;           // Load curve ID
        double sf = 1.0;        // Scale factor
        double xc = 0.0;        // X center
        double yc = 0.0;        // Y center
        double zc = 0.0;        // Z center
    };

    LoadBodyRy() = default;

    std::string getKeywordName() const override { return "*LOAD_BODY_RY"; }

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
 * @brief *LOAD_BODY_RZ
 *
 * Rotational body force around Z axis.
 */
class KOO_API LoadBodyRz : public CloneableKeyword<LoadBodyRz, LoadKeyword> {
public:
    struct Data {
        int lcid = 0;           // Load curve ID
        double sf = 1.0;        // Scale factor
        double xc = 0.0;        // X center
        double yc = 0.0;        // Y center
        double zc = 0.0;        // Z center
    };

    LoadBodyRz() = default;

    std::string getKeywordName() const override { return "*LOAD_BODY_RZ"; }

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
 * @brief *LOAD_THERMAL_VARIABLE_SOLID
 *
 * Variable thermal load on solid elements.
 */
class KOO_API LoadThermalVariableSolid : public CloneableKeyword<LoadThermalVariableSolid, LoadKeyword> {
public:
    struct Data {
        int esid = 0;           // Element set ID
        int lcid = 0;           // Load curve ID
        double sf = 1.0;        // Scale factor
    };

    LoadThermalVariableSolid() = default;

    std::string getKeywordName() const override { return "*LOAD_THERMAL_VARIABLE_SOLID"; }

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
 * @brief *LOAD_THERMAL_VARIABLE_TSHELL
 *
 * Variable thermal load on thick shell elements.
 */
class KOO_API LoadThermalVariableTshell : public CloneableKeyword<LoadThermalVariableTshell, LoadKeyword> {
public:
    struct Data {
        int esid = 0;           // Element set ID
        int lcid = 0;           // Load curve ID
        double sf = 1.0;        // Scale factor
    };

    LoadThermalVariableTshell() = default;

    std::string getKeywordName() const override { return "*LOAD_THERMAL_VARIABLE_TSHELL"; }

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
 * @brief *LOAD_RIGID_BODY_INERTIA
 *
 * Inertia load on rigid body.
 */
class KOO_API LoadRigidBodyInertia : public CloneableKeyword<LoadRigidBodyInertia, LoadKeyword> {
public:
    struct Data {
        PartId pid = 0;         // Part ID of rigid body
        int lcid = 0;           // Load curve ID
        double sf = 1.0;        // Scale factor
        double ax = 0.0;        // Acceleration X
        double ay = 0.0;        // Acceleration Y
        double az = 0.0;        // Acceleration Z
    };

    LoadRigidBodyInertia() = default;

    std::string getKeywordName() const override { return "*LOAD_RIGID_BODY_INERTIA"; }

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
 * @brief *LOAD_SEGMENT_DATA
 *
 * Segment load from data file.
 */
class KOO_API LoadSegmentData : public CloneableKeyword<LoadSegmentData, LoadKeyword> {
public:
    struct Data {
        int ssid = 0;           // Segment set ID
        int lcid = 0;           // Load curve ID
        double sf = 1.0;        // Scale factor
        std::string filename;   // Data filename
    };

    LoadSegmentData() = default;

    std::string getKeywordName() const override { return "*LOAD_SEGMENT_DATA"; }

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
 * @brief *LOAD_BODY_PART
 *
 * Body force on single part.
 */
class KOO_API LoadBodyPart : public CloneableKeyword<LoadBodyPart, LoadKeyword> {
public:
    struct Data {
        PartId pid = 0;         // Part ID
        int lcidx = 0;          // Load curve X direction
        int lcidy = 0;          // Load curve Y direction
        int lcidz = 0;          // Load curve Z direction
        double sfx = 1.0;       // Scale factor X
        double sfy = 1.0;       // Scale factor Y
        double sfz = 1.0;       // Scale factor Z
    };

    LoadBodyPart() = default;

    std::string getKeywordName() const override { return "*LOAD_BODY_PART"; }

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
 * @brief *LOAD_BODY_PART_SET
 *
 * Body force on part set.
 */
class KOO_API LoadBodyPartSet : public CloneableKeyword<LoadBodyPartSet, LoadKeyword> {
public:
    struct Data {
        int psid = 0;           // Part set ID
        int lcidx = 0;          // Load curve X direction
        int lcidy = 0;          // Load curve Y direction
        int lcidz = 0;          // Load curve Z direction
        double sfx = 1.0;       // Scale factor X
        double sfy = 1.0;       // Scale factor Y
        double sfz = 1.0;       // Scale factor Z
    };

    LoadBodyPartSet() = default;

    std::string getKeywordName() const override { return "*LOAD_BODY_PART_SET"; }

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
 * @brief *LOAD_FLUID_PRESSURE
 *
 * Fluid pressure load.
 */
class KOO_API LoadFluidPressure : public CloneableKeyword<LoadFluidPressure, LoadKeyword> {
public:
    struct Data {
        int ssid = 0;           // Segment set ID
        int lcid = 0;           // Load curve ID
        double sf = 1.0;        // Scale factor
        double pref = 0.0;      // Reference pressure
    };

    LoadFluidPressure() = default;

    std::string getKeywordName() const override { return "*LOAD_FLUID_PRESSURE"; }

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
 * @brief *LOAD_INTERIOR_PRESSURE
 *
 * Interior pressure load.
 */
class KOO_API LoadInteriorPressure : public CloneableKeyword<LoadInteriorPressure, LoadKeyword> {
public:
    struct Data {
        int psid = 0;           // Part set ID
        int lcid = 0;           // Load curve ID
        double sf = 1.0;        // Scale factor
    };

    LoadInteriorPressure() = default;

    std::string getKeywordName() const override { return "*LOAD_INTERIOR_PRESSURE"; }

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
 * @brief *LOAD_TYREPRESS
 *
 * Tire pressure load.
 */
class KOO_API LoadTyrePress : public CloneableKeyword<LoadTyrePress, LoadKeyword> {
public:
    struct Data {
        int pid = 0;            // Part ID
        int lcid = 0;           // Load curve ID
        double sf = 1.0;        // Scale factor
        double pressure = 0.0;  // Initial pressure
    };

    LoadTyrePress() = default;

    std::string getKeywordName() const override { return "*LOAD_TYREPRESS"; }

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
 * @brief *LOAD_AIRBAG_PRESSURE
 *
 * Airbag pressure load.
 */
class KOO_API LoadAirbagPressure : public CloneableKeyword<LoadAirbagPressure, LoadKeyword> {
public:
    struct Data {
        int psid = 0;           // Part set ID
        int lcid = 0;           // Load curve ID
        double sf = 1.0;        // Scale factor
        int type = 0;           // Airbag type
    };

    LoadAirbagPressure() = default;

    std::string getKeywordName() const override { return "*LOAD_AIRBAG_PRESSURE"; }

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
 * @brief *LOAD_LANCZOS
 *
 * Lanczos modal load.
 */
class KOO_API LoadLanczos : public CloneableKeyword<LoadLanczos, LoadKeyword> {
public:
    struct Data {
        int nsid = 0;           // Node set ID
        int dof = 0;            // Degree of freedom
        int lcid = 0;           // Load curve ID
        double sf = 1.0;        // Scale factor
    };

    LoadLanczos() = default;

    std::string getKeywordName() const override { return "*LOAD_LANCZOS"; }

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
 * @brief *LOAD_PRESSURE_PENETRATION
 *
 * Pressure penetration load.
 */
class KOO_API LoadPressurePenetration : public CloneableKeyword<LoadPressurePenetration, LoadKeyword> {
public:
    struct Data {
        int ssid = 0;           // Segment set ID
        int lcid = 0;           // Load curve ID
        double sf = 1.0;        // Scale factor
        double pmin = 0.0;      // Minimum pressure
        double pmax = 0.0;      // Maximum pressure
    };

    LoadPressurePenetration() = default;

    std::string getKeywordName() const override { return "*LOAD_PRESSURE_PENETRATION"; }

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
 * @brief *LOAD_RAIL
 *
 * Rail load.
 */
class KOO_API LoadRail : public CloneableKeyword<LoadRail, LoadKeyword> {
public:
    struct Data {
        int nsid = 0;           // Node set ID
        int lcid = 0;           // Load curve ID
        double sf = 1.0;        // Scale factor
        int type = 0;           // Rail type
    };

    LoadRail() = default;

    std::string getKeywordName() const override { return "*LOAD_RAIL"; }

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
 * @brief *LOAD_TRACK_TURN
 *
 * Track turn load.
 */
class KOO_API LoadTrackTurn : public CloneableKeyword<LoadTrackTurn, LoadKeyword> {
public:
    struct Data {
        int pid = 0;            // Part ID
        int lcid = 0;           // Load curve ID
        double sf = 1.0;        // Scale factor
        double radius = 0.0;    // Turn radius
    };

    LoadTrackTurn() = default;

    std::string getKeywordName() const override { return "*LOAD_TRACK_TURN"; }

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
 * @brief *LOAD_WHEEL_PATCH
 *
 * Wheel patch load.
 */
class KOO_API LoadWheelPatch : public CloneableKeyword<LoadWheelPatch, LoadKeyword> {
public:
    struct Data {
        int pid = 0;            // Part ID
        int lcid = 0;           // Load curve ID
        double sf = 1.0;        // Scale factor
        double xc = 0.0;        // X center
        double yc = 0.0;        // Y center
        double zc = 0.0;        // Z center
    };

    LoadWheelPatch() = default;

    std::string getKeywordName() const override { return "*LOAD_WHEEL_PATCH"; }

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
 * @brief *LOAD_WAVE
 *
 * Wave load.
 */
class KOO_API LoadWave : public CloneableKeyword<LoadWave, LoadKeyword> {
public:
    struct Data {
        int ssid = 0;           // Segment set ID
        int lcid = 0;           // Load curve ID
        double sf = 1.0;        // Scale factor
        int wtype = 0;          // Wave type
        double amp = 0.0;       // Amplitude
        double freq = 0.0;      // Frequency
    };

    LoadWave() = default;

    std::string getKeywordName() const override { return "*LOAD_WAVE"; }

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
 * @brief *LOAD_PENDULUM
 *
 * Pendulum load.
 */
class KOO_API LoadPendulum : public CloneableKeyword<LoadPendulum, LoadKeyword> {
public:
    struct Data {
        PartId pid = 0;         // Part ID
        int lcid = 0;           // Load curve ID
        double sf = 1.0;        // Scale factor
        double length = 0.0;    // Pendulum length
    };

    LoadPendulum() = default;

    std::string getKeywordName() const override { return "*LOAD_PENDULUM"; }

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
 * @brief *LOAD_THERMAL_TOPAZ3D
 *
 * Thermal load from TOPAZ3D.
 */
class KOO_API LoadThermalTopaz3d : public CloneableKeyword<LoadThermalTopaz3d, LoadKeyword> {
public:
    struct Data {
        int lcid = 0;           // Load curve ID
        double sf = 1.0;        // Scale factor
        std::string filename;   // TOPAZ3D filename
    };

    LoadThermalTopaz3d() = default;

    std::string getKeywordName() const override { return "*LOAD_THERMAL_TOPAZ3D"; }

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
 * @brief *LOAD_THERMAL_BODY
 *
 * Thermal body load.
 */
class KOO_API LoadThermalBody : public CloneableKeyword<LoadThermalBody, LoadKeyword> {
public:
    struct Data {
        int psid = 0;           // Part set ID
        int lcid = 0;           // Load curve ID
        double sf = 1.0;        // Scale factor
    };

    LoadThermalBody() = default;

    std::string getKeywordName() const override { return "*LOAD_THERMAL_BODY"; }

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
 * @brief *LOAD_SOFT_ELEMENT
 *
 * Soft element load.
 */
class KOO_API LoadSoftElement : public CloneableKeyword<LoadSoftElement, LoadKeyword> {
public:
    struct Data {
        ElementId eid = 0;      // Element ID
        int lcid = 0;           // Load curve ID
        double sf = 1.0;        // Scale factor
    };

    LoadSoftElement() = default;

    std::string getKeywordName() const override { return "*LOAD_SOFT_ELEMENT"; }

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
 * @brief *LOAD_SURFACE_STRESS_SEGMENT
 *
 * Surface stress on segment.
 */
class KOO_API LoadSurfaceStressSegment : public CloneableKeyword<LoadSurfaceStressSegment, LoadKeyword> {
public:
    struct Data {
        int ssid = 0;           // Segment set ID
        int lcid = 0;           // Load curve ID
        double sf = 1.0;        // Scale factor
        double sigmaxx = 0.0;   // Stress XX
        double sigmayy = 0.0;   // Stress YY
        double sigmaxy = 0.0;   // Stress XY
    };

    LoadSurfaceStressSegment() = default;

    std::string getKeywordName() const override { return "*LOAD_SURFACE_STRESS_SEGMENT"; }

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
 * @brief *LOAD_MOTION_NODE_SET
 *
 * Prescribed motion on node set.
 */
class KOO_API LoadMotionNodeSet : public CloneableKeyword<LoadMotionNodeSet, LoadKeyword> {
public:
    struct Data {
        int nsid = 0;           // Node set ID
        int dof = 0;            // Degree of freedom
        int vad = 0;            // Velocity/acceleration/displacement flag
        int lcid = 0;           // Load curve ID
        double sf = 1.0;        // Scale factor
        int vid = 0;            // Vector ID
    };

    LoadMotionNodeSet() = default;

    std::string getKeywordName() const override { return "*LOAD_MOTION_NODE_SET"; }

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
 * @brief *LOAD_RADIATION
 *
 * Radiation heat load.
 */
class KOO_API LoadRadiation : public CloneableKeyword<LoadRadiation, LoadKeyword> {
public:
    struct Data {
        int ssid = 0;           // Segment set ID
        int lcid = 0;           // Load curve ID
        double sf = 1.0;        // Scale factor
        double emiss = 0.0;     // Emissivity
        double tamb = 0.0;      // Ambient temperature
    };

    LoadRadiation() = default;

    std::string getKeywordName() const override { return "*LOAD_RADIATION"; }

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
 * @brief *LOAD_CONVECTION
 *
 * Convection heat load.
 */
class KOO_API LoadConvection : public CloneableKeyword<LoadConvection, LoadKeyword> {
public:
    struct Data {
        int ssid = 0;           // Segment set ID
        int lcid = 0;           // Load curve ID for h
        double sf = 1.0;        // Scale factor
        double h = 0.0;         // Heat transfer coefficient
        double tamb = 0.0;      // Ambient temperature
    };

    LoadConvection() = default;

    std::string getKeywordName() const override { return "*LOAD_CONVECTION"; }

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
 * @brief *LOAD_HEAT_FLUX
 *
 * Heat flux load.
 */
class KOO_API LoadHeatFlux : public CloneableKeyword<LoadHeatFlux, LoadKeyword> {
public:
    struct Data {
        int ssid = 0;           // Segment set ID
        int lcid = 0;           // Load curve ID
        double sf = 1.0;        // Scale factor
        double flux = 0.0;      // Heat flux value
    };

    LoadHeatFlux() = default;

    std::string getKeywordName() const override { return "*LOAD_HEAT_FLUX"; }

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
 * @brief *LOAD_SPC
 *
 * Single point constraint load.
 */
class KOO_API LoadSpc : public CloneableKeyword<LoadSpc, LoadKeyword> {
public:
    struct Data {
        NodeId nid = 0;         // Node ID
        int dof = 0;            // Degree of freedom
        int lcid = 0;           // Load curve ID
        double sf = 1.0;        // Scale factor
    };

    LoadSpc() = default;

    std::string getKeywordName() const override { return "*LOAD_SPC"; }

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
 * @brief *LOAD_SPC_SET
 *
 * SPC load on node set.
 */
class KOO_API LoadSpcSet : public CloneableKeyword<LoadSpcSet, LoadKeyword> {
public:
    struct Data {
        int nsid = 0;           // Node set ID
        int dof = 0;            // Degree of freedom
        int lcid = 0;           // Load curve ID
        double sf = 1.0;        // Scale factor
    };

    LoadSpcSet() = default;

    std::string getKeywordName() const override { return "*LOAD_SPC_SET"; }

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
 * @brief *LOAD_SEGMENT_PRESSURE
 *
 * Pressure load on segment.
 */
class KOO_API LoadSegmentPressure : public CloneableKeyword<LoadSegmentPressure, LoadKeyword> {
public:
    struct Data {
        int ssid = 0;           // Segment set ID
        int lcid = 0;           // Load curve ID
        double sf = 1.0;        // Scale factor
        double p0 = 0.0;        // Initial pressure
    };

    LoadSegmentPressure() = default;

    std::string getKeywordName() const override { return "*LOAD_SEGMENT_PRESSURE"; }

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
 * @brief *LOAD_AIRMIX
 *
 * Air mix load.
 */
class KOO_API LoadAirmix : public CloneableKeyword<LoadAirmix, LoadKeyword> {
public:
    struct Data {
        int psid = 0;           // Part set ID
        int lcid = 0;           // Load curve ID
        double sf = 1.0;        // Scale factor
    };

    LoadAirmix() = default;

    std::string getKeywordName() const override { return "*LOAD_AIRMIX"; }

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
