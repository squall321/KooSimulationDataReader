#pragma once

#include <koo/Export.hpp>
#include <koo/dyna/Keyword.hpp>
#include <koo/util/Types.hpp>
#include <vector>
#include <unordered_map>

namespace koo::dyna {

/**
 * @brief Part data structure
 */
struct KOO_API PartData {
    PartId id = 0;
    SectionId secid = 0;
    MaterialId mid = 0;
    int eosid = 0;
    int hgid = 0;
    int grav = 0;
    int adpopt = 0;
    int tmid = 0;
    std::string title;

    PartData() = default;
    PartData(PartId id_, SectionId secid_, MaterialId mid_,
             const std::string& title_ = "")
        : id(id_), secid(secid_), mid(mid_), title(title_) {}
};

/**
 * @brief *PART keyword
 */
class KOO_API Part : public CloneableKeyword<Part> {
public:
    Part() = default;

    std::string getKeywordName() const override { return "*PART"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    // Part access
    void addPart(const PartData& part);
    void addPart(PartId id, SectionId secid, MaterialId mid,
                 const std::string& title = "");

    bool hasPart(PartId id) const;
    PartData* getPart(PartId id);
    const PartData* getPart(PartId id) const;

    void removePart(PartId id);
    void clear();

    // Iteration
    const std::vector<PartData>& getParts() const { return parts_; }
    std::vector<PartData>& getParts() { return parts_; }
    size_t getPartCount() const { return parts_.size(); }

private:
    void rebuildIndex();

    std::vector<PartData> parts_;
    std::unordered_map<PartId, size_t> idIndex_;
};

/**
 * @brief Composite part layer data
 */
struct KOO_API PartCompositeLayer {
    MaterialId mid = 0;     // Material ID for this layer
    double thick = 0.0;     // Thickness of layer
    double b = 0.0;         // Material angle
    int ithid = 0;          // Integration rule ID
};

/**
 * @brief *PART_COMPOSITE keyword
 *
 * Defines a composite part with multiple material layers.
 */
class KOO_API PartComposite : public CloneableKeyword<PartComposite> {
public:
    struct Data {
        PartId pid = 0;         // Part ID
        int elform = 0;         // Element formulation
        int shrf = 0;           // Shear factor
        int nloc = 0;           // Nodal location
        double marea = 0.0;     // Non-structural mass per area
        int hgid = 0;           // Hourglass ID
        int adpopt = 0;         // Adaptive option
        int ithelfrm = 0;       // Thermal shell formulation
        std::string title;
        std::vector<PartCompositeLayer> layers;
    };

    PartComposite() = default;

    std::string getKeywordName() const override { return "*PART_COMPOSITE"; }

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
 * @brief *PART_INERTIA keyword
 *
 * Defines inertia properties for a part directly.
 */
class KOO_API PartInertia : public CloneableKeyword<PartInertia> {
public:
    struct Data {
        PartId pid = 0;         // Part ID
        SectionId secid = 0;    // Section ID
        MaterialId mid = 0;     // Material ID
        int eosid = 0;          // EOS ID
        int hgid = 0;           // Hourglass ID
        int grav = 0;           // Gravity load flag
        int adpopt = 0;         // Adaptive option
        int tmid = 0;           // Thermal material ID
        std::string title;
        // Inertia card
        double xc = 0.0;        // X center of mass
        double yc = 0.0;        // Y center of mass
        double zc = 0.0;        // Z center of mass
        double tm = 0.0;        // Total mass
        int ircs = 0;           // Inertia reference coordinate system
        int nodeid = 0;         // Optional node ID
        // Inertia tensor
        double ixx = 0.0;
        double ixy = 0.0;
        double ixz = 0.0;
        double iyy = 0.0;
        double iyz = 0.0;
        double izz = 0.0;
        // Velocity
        double vx = 0.0;
        double vy = 0.0;
        double vz = 0.0;
        // Angular velocity
        double vxr = 0.0;
        double vyr = 0.0;
        double vzr = 0.0;
    };

    PartInertia() = default;

    std::string getKeywordName() const override { return "*PART_INERTIA"; }

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
 * @brief *PART_CONTACT keyword
 *
 * Defines contact parameters for a specific part.
 */
class KOO_API PartContact : public CloneableKeyword<PartContact> {
public:
    struct Data {
        PartId pid = 0;
        double fs = 0.0;    // Static friction coefficient
        double fd = 0.0;    // Dynamic friction coefficient
        double dc = 0.0;    // Exponential decay coefficient
        double vc = 0.0;    // Viscous damping coefficient
        int opteflag = 0;   // Optional erosion flag
        double sfs = 1.0;   // Scale factor for slave stiffness
        double sfm = 1.0;   // Scale factor for master stiffness
        std::string title;
    };

    PartContact() = default;

    std::string getKeywordName() const override { return "*PART_CONTACT"; }

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
 * @brief *PART_MOVE keyword
 *
 * Defines a translation and/or rotation of a part.
 */
class KOO_API PartMove : public CloneableKeyword<PartMove> {
public:
    struct Data {
        PartId pid = 0;
        double xmov = 0.0;  // X translation
        double ymov = 0.0;  // Y translation
        double zmov = 0.0;  // Z translation
        int cid = 0;        // Coordinate system ID for rotation
        double xx = 0.0;    // X rotation angle
        double yy = 0.0;    // Y rotation angle
        double zz = 0.0;    // Z rotation angle
    };

    PartMove() = default;

    std::string getKeywordName() const override { return "*PART_MOVE"; }

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
 * @brief *PART_STIFFNESS keyword
 *
 * Defines stiffness scaling for a part.
 */
class KOO_API PartStiffness : public CloneableKeyword<PartStiffness> {
public:
    struct Data {
        PartId pid = 0;
        double k = 0.0;     // Stiffness value
        int lcid = 0;       // Load curve ID
        double sf = 1.0;    // Scale factor
    };

    PartStiffness() = default;

    std::string getKeywordName() const override { return "*PART_STIFFNESS"; }

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
 * @brief *PART_ANNEAL keyword
 *
 * Defines annealing parameters for a part to remove residual stresses.
 */
class KOO_API PartAnneal : public CloneableKeyword<PartAnneal> {
public:
    struct Data {
        PartId pid = 0;
        double tanneal = 0.0;   // Anneal time
    };

    PartAnneal() = default;

    std::string getKeywordName() const override { return "*PART_ANNEAL"; }

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
 * @brief *PART_SENSOR keyword
 *
 * Defines a sensor for a part.
 */
class KOO_API PartSensor : public CloneableKeyword<PartSensor> {
public:
    struct Data {
        PartId pid = 0;
        int sensid = 0;     // Sensor ID
        int typeid_ = 0;    // Sensor type
    };

    PartSensor() = default;

    std::string getKeywordName() const override { return "*PART_SENSOR"; }

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
 * @brief *PART_AVERAGED keyword
 *
 * Defines averaged part for output.
 */
class KOO_API PartAveraged : public CloneableKeyword<PartAveraged> {
public:
    struct Data {
        PartId pid = 0;
        SectionId secid = 0;
        MaterialId mid = 0;
        int eosid = 0;
        int hgid = 0;
        int grav = 0;
        int adpopt = 0;
        int tmid = 0;
        std::string title;
    };

    PartAveraged() = default;

    std::string getKeywordName() const override { return "*PART_AVERAGED"; }

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
 * @brief *PART_DUPLICATE keyword
 *
 * Creates a duplicate of an existing part.
 */
class KOO_API PartDuplicate : public CloneableKeyword<PartDuplicate> {
public:
    struct Data {
        PartId pid = 0;         // New part ID
        PartId pidcopy = 0;     // Part ID to copy from
    };

    PartDuplicate() = default;

    std::string getKeywordName() const override { return "*PART_DUPLICATE"; }

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
 * @brief *PART_MODES keyword
 *
 * Defines modal properties for a part.
 */
class KOO_API PartModes : public CloneableKeyword<PartModes> {
public:
    struct ModeData {
        int modeid = 0;
        double freq = 0.0;      // Frequency
        double damp = 0.0;      // Damping ratio
    };

    struct Data {
        PartId pid = 0;
        std::string title;
        std::vector<ModeData> modes;
    };

    PartModes() = default;

    std::string getKeywordName() const override { return "*PART_MODES"; }

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
 * @brief *PART_ADAPTIVE_FAILURE keyword
 *
 * Defines adaptive failure criteria for a part.
 */
class KOO_API PartAdaptiveFailure : public CloneableKeyword<PartAdaptiveFailure> {
public:
    struct Data {
        PartId pid = 0;
        int ifail = 0;      // Failure flag
        double fs = 0.0;    // Failure strain
        double fd = 0.0;    // Damage parameter
    };

    PartAdaptiveFailure() = default;

    std::string getKeywordName() const override { return "*PART_ADAPTIVE_FAILURE"; }

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
 * @brief *PART_COMPOSITE_TSHELL keyword
 *
 * Defines a composite thick shell part with multiple material layers.
 */
class KOO_API PartCompositeTshell : public CloneableKeyword<PartCompositeTshell> {
public:
    struct Data {
        PartId pid = 0;         // Part ID
        int elform = 0;         // Element formulation
        int shrf = 0;           // Shear factor
        int nloc = 0;           // Nodal location
        double marea = 0.0;     // Non-structural mass per area
        int hgid = 0;           // Hourglass ID
        int adpopt = 0;         // Adaptive option
        int ithelfrm = 0;       // Thermal shell formulation
        std::string title;
        std::vector<PartCompositeLayer> layers;
    };

    PartCompositeTshell() = default;

    std::string getKeywordName() const override { return "*PART_COMPOSITE_TSHELL"; }

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
 * @brief *PARTICLE_BLAST keyword
 *
 * Defines particle blast parameters.
 */
class KOO_API ParticleBlast : public CloneableKeyword<ParticleBlast> {
public:
    struct Data {
        int pid = 0;            // Particle ID
        double mass = 0.0;      // Particle mass
        double xc = 0.0;        // X center
        double yc = 0.0;        // Y center
        double zc = 0.0;        // Z center
        double energy = 0.0;    // Initial energy
    };

    ParticleBlast() = default;

    std::string getKeywordName() const override { return "*PARTICLE_BLAST"; }

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
 * @brief *PARTS_DISTRIBUTE keyword
 *
 * Distributes parts across processors.
 */
class KOO_API PartsDistribute : public CloneableKeyword<PartsDistribute> {
public:
    struct Data {
        int method = 0;         // Distribution method
        int nprc = 0;           // Number of processors
    };

    PartsDistribute() = default;

    std::string getKeywordName() const override { return "*PARTS_DISTRIBUTE"; }

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
 * @brief *PARTSET_DISTRIBUTE keyword
 *
 * Distributes part sets across processors.
 */
class KOO_API PartsetDistribute : public CloneableKeyword<PartsetDistribute> {
public:
    struct Data {
        int psid = 0;           // Part set ID
        int method = 0;         // Distribution method
    };

    PartsetDistribute() = default;

    std::string getKeywordName() const override { return "*PARTSET_DISTRIBUTE"; }

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
 * @brief *PART_STACKED_ELEMENTS keyword
 *
 * Defines stacked elements for a part.
 */
class KOO_API PartStackedElements : public CloneableKeyword<PartStackedElements> {
public:
    struct Data {
        PartId pid = 0;
        int nlayers = 0;        // Number of layers
        int icoord = 0;         // Coordinate system flag
    };

    PartStackedElements() = default;

    std::string getKeywordName() const override { return "*PART_STACKED_ELEMENTS"; }

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
