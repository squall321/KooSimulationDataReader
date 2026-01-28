#pragma once

#include <koo/Export.hpp>
#include <koo/dyna/Keyword.hpp>
#include <koo/util/Types.hpp>
#include <vector>
#include <string>

namespace koo::dyna {

/**
 * @brief Base class for *ALE keywords
 */
class KOO_API AleKeyword : public Keyword {
public:
    virtual ~AleKeyword() = default;
};

/**
 * @brief *ALE_MULTI-MATERIAL_GROUP
 *
 * Defines multi-material ALE groups.
 */
class KOO_API AleMultiMaterialGroup : public CloneableKeyword<AleMultiMaterialGroup, AleKeyword> {
public:
    struct Entry {
        int sid = 0;          // Set ID
        int idtype = 0;       // ID type (0=part, 1=set)
        int gpname = 0;       // Group name
    };

    AleMultiMaterialGroup() = default;

    std::string getKeywordName() const override { return "*ALE_MULTI-MATERIAL_GROUP"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    std::vector<Entry>& getEntries() { return entries_; }
    const std::vector<Entry>& getEntries() const { return entries_; }

private:
    std::vector<Entry> entries_;
};

/**
 * @brief *ALE_SMOOTHING
 *
 * Controls mesh smoothing for ALE elements.
 */
class KOO_API AleSmoothing : public CloneableKeyword<AleSmoothing, AleKeyword> {
public:
    struct Data {
        int pid = 0;          // Part ID or part set ID
        int afac = 0;         // Smoothing weight factor
        double efac = 0.0;    // Exponential decay factor
        int dfac = 0;         // Diffusion method
        int start = 0;        // Start time for smoothing
        double end = 0.0;     // End time for smoothing
        int prid = 0;         // Part ID for reference
    };

    AleSmoothing() = default;

    std::string getKeywordName() const override { return "*ALE_SMOOTHING"; }

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
 * @brief *ALE_REFERENCE_SYSTEM_NODE
 *
 * Defines ALE reference system based on nodes.
 */
class KOO_API AleReferenceSystemNode : public CloneableKeyword<AleReferenceSystemNode, AleKeyword> {
public:
    struct Data {
        int id = 0;           // Reference system ID
        int nid = 0;          // Node ID
        int bctran = 0;       // Translation BC
        int bcexp = 0;        // Expansion BC
        int bcrot = 0;        // Rotation BC
        double pession = 0.0; // Expansion coefficient
        int pression = 0;     // Pressure node set
        int nsid = 0;         // Node set ID
    };

    AleReferenceSystemNode() = default;

    std::string getKeywordName() const override { return "*ALE_REFERENCE_SYSTEM_NODE"; }

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
 * @brief *ALE_REFERENCE_SYSTEM_GROUP
 *
 * Defines ALE reference system based on part groups.
 */
class KOO_API AleReferenceSystemGroup : public CloneableKeyword<AleReferenceSystemGroup, AleKeyword> {
public:
    struct Data {
        int id = 0;           // Reference system ID
        int pid = 0;          // Part set ID
        int bctran = 0;       // Translation BC
        int bcexp = 0;        // Expansion BC
        int bcrot = 0;        // Rotation BC
        double pession = 0.0; // Expansion coefficient
        int pression = 0;     // Pressure node set
    };

    AleReferenceSystemGroup() = default;

    std::string getKeywordName() const override { return "*ALE_REFERENCE_SYSTEM_GROUP"; }

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
 * @brief *ALE_FSI_SWITCH_MMG
 *
 * FSI switch for multi-material group.
 */
class KOO_API AleFsiSwitchMmg : public CloneableKeyword<AleFsiSwitchMmg, AleKeyword> {
public:
    struct Data {
        int id = 0;           // ID
        int ammgid = 0;       // ALE multi-material group ID
        int swtype = 0;       // Switch type
        double btime = 0.0;   // Birth time
        double dtime = 0.0;   // Death time
    };

    AleFsiSwitchMmg() = default;

    std::string getKeywordName() const override { return "*ALE_FSI_SWITCH_MMG"; }

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
 * @brief *ALE_AMBIENT_HYDROSTATIC
 *
 * Hydrostatic ambient conditions for ALE.
 */
class KOO_API AleAmbientHydrostatic : public CloneableKeyword<AleAmbientHydrostatic, AleKeyword> {
public:
    struct Data {
        int pid = 0;          // Part ID
        double density = 0.0; // Fluid density
        double gx = 0.0;      // Gravity x
        double gy = 0.0;      // Gravity y
        double gz = 0.0;      // Gravity z
        double refp = 0.0;    // Reference pressure
    };

    AleAmbientHydrostatic() = default;

    std::string getKeywordName() const override { return "*ALE_AMBIENT_HYDROSTATIC"; }

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
 * @brief *ALE_STRUCTURED_MESH
 *
 * Define structured mesh for ALE.
 */
class KOO_API AleStructuredMesh : public CloneableKeyword<AleStructuredMesh, AleKeyword> {
public:
    struct Data {
        int mshid = 0;        // Mesh ID
        int dpid = 0;         // Default part ID
        int nbxi = 0;         // Number of elements in X
        int nbyi = 0;         // Number of elements in Y
        int nbzi = 0;         // Number of elements in Z
        int icase = 0;        // Case type
    };

    AleStructuredMesh() = default;

    std::string getKeywordName() const override { return "*ALE_STRUCTURED_MESH"; }

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
 * @brief *ALE_STRUCTURED_MESH_CONTROL_POINTS
 *
 * Control points for structured mesh.
 */
class KOO_API AleStructuredMeshControlPoints : public CloneableKeyword<AleStructuredMeshControlPoints, AleKeyword> {
public:
    struct Entry {
        int cpid = 0;         // Control point ID
        double x = 0.0;       // X coordinate
        double y = 0.0;       // Y coordinate
        double z = 0.0;       // Z coordinate
    };

    AleStructuredMeshControlPoints() = default;

    std::string getKeywordName() const override { return "*ALE_STRUCTURED_MESH_CONTROL_POINTS"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    std::vector<Entry>& getEntries() { return entries_; }
    const std::vector<Entry>& getEntries() const { return entries_; }

private:
    std::vector<Entry> entries_;
};

/**
 * @brief *ALE_FAIL_SWITCH_MMG
 *
 * ALE material failure with multi-material group switching.
 */
class KOO_API AleFailSwitchMmg : public CloneableKeyword<AleFailSwitchMmg, AleKeyword> {
public:
    struct Data {
        int id = 0;           // ID
        int ammgid = 0;       // ALE multi-material group ID
        int tammgid = 0;      // Target multi-material group ID
        double fval = 0.0;    // Failure value
        int ftyp = 0;         // Failure type
    };

    AleFailSwitchMmg() = default;

    std::string getKeywordName() const override { return "*ALE_FAIL_SWITCH_MMG"; }

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
 * @brief *ALE_ESSENTIAL_BOUNDARY
 *
 * Essential boundary conditions for ALE.
 */
class KOO_API AleEssentialBoundary : public CloneableKeyword<AleEssentialBoundary, AleKeyword> {
public:
    struct Data {
        int nsid = 0;         // Node set ID
        int ssid = 0;         // Segment set ID
        int dofx = 0;         // DOF X constraint
        int dofy = 0;         // DOF Y constraint
        int dofz = 0;         // DOF Z constraint
        int pid = 0;          // Part ID
    };

    AleEssentialBoundary() = default;

    std::string getKeywordName() const override { return "*ALE_ESSENTIAL_BOUNDARY"; }

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
 * @brief *ALE_COUPLING_NODAL_CONSTRAINT
 *
 * Coupling constraint between ALE and Lagrangian nodes.
 */
class KOO_API AleCouplingNodalConstraint : public CloneableKeyword<AleCouplingNodalConstraint, AleKeyword> {
public:
    struct Data {
        int slave = 0;          // Slave node set ID (ALE)
        int master = 0;         // Master node set ID (Lagrangian)
        int ctype = 0;          // Coupling type (1=kinematic, 2=penalty)
        double mcoup = 0.0;     // Mass coupling factor
        double start = 0.0;     // Start time
        double end = 1e28;      // End time
        double pfac = 0.1;      // Penalty factor
    };

    AleCouplingNodalConstraint() = default;

    std::string getKeywordName() const override { return "*ALE_COUPLING_NODAL_CONSTRAINT"; }

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
