#pragma once

#include <koo/Export.hpp>
#include <koo/dyna/Keyword.hpp>
#include <koo/util/Types.hpp>
#include <vector>
#include <string>

namespace koo::dyna {

/**
 * @brief Base class for *SPH keywords
 */
class KOO_API SphKeyword : public Keyword {
public:
    virtual ~SphKeyword() = default;
};

/**
 * @brief *CONTROL_SPH
 *
 * SPH solver control parameters.
 */
class KOO_API ControlSph : public CloneableKeyword<ControlSph, SphKeyword> {
public:
    struct Data {
        int ncbs = 0;         // Contact buffer size
        int boxid = 0;        // Box ID
        int dt2msf = 0;       // Mass scaling flag
        double stefr = 0.0;   // Start time
        int pstefr = 0;       // Part set for start time
        int swsf = 0;         // Shift warning suppression
        int ishift = 0;       // Particle shifting
        double memory = 0.0;  // Memory allocation
    };

    ControlSph() = default;

    std::string getKeywordName() const override { return "*CONTROL_SPH"; }

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
 * @brief *SECTION_SPH
 *
 * SPH section definition.
 */
class KOO_API SectionSph : public CloneableKeyword<SectionSph, SphKeyword> {
public:
    struct Data {
        int secid = 0;        // Section ID
        double cslh = 0.0;    // Smoothing length scale
        double hmin = 0.0;    // Min smoothing length
        double hmax = 0.0;    // Max smoothing length
        double sphini = 0.0;  // Initial smoothing length
        double death = 0.0;   // Death time
        double start = 0.0;   // Start time
    };

    SectionSph() = default;

    std::string getKeywordName() const override { return "*SECTION_SPH"; }

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
 * @brief *ELEMENT_SPH
 *
 * SPH element (particle) definition.
 */
class KOO_API ElementSph : public CloneableKeyword<ElementSph, SphKeyword> {
public:
    struct Entry {
        int nid = 0;          // Node ID
        int pid = 0;          // Part ID
        double mass = 0.0;    // Nodal mass
    };

    ElementSph() = default;

    std::string getKeywordName() const override { return "*ELEMENT_SPH"; }

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
 * @brief *DEFINE_SPH_MESH_BOX
 *
 * SPH mesh box definition.
 */
class KOO_API DefineSphMeshBox : public CloneableKeyword<DefineSphMeshBox, SphKeyword> {
public:
    struct Data {
        int boxid = 0;        // Box ID
        double xmin = 0.0;    // Min X
        double xmax = 0.0;    // Max X
        double ymin = 0.0;    // Min Y
        double ymax = 0.0;    // Max Y
        double zmin = 0.0;    // Min Z
        double zmax = 0.0;    // Max Z
    };

    DefineSphMeshBox() = default;

    std::string getKeywordName() const override { return "*DEFINE_SPH_MESH_BOX"; }

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
 * @brief *DEFINE_SPH_AMBIENT_DRAG
 *
 * SPH ambient drag definition.
 */
class KOO_API DefineSphAmbientDrag : public CloneableKeyword<DefineSphAmbientDrag, SphKeyword> {
public:
    struct Data {
        int pid = 0;          // Part ID
        double cd = 0.0;      // Drag coefficient
        double rho = 0.0;     // Ambient density
        double vx = 0.0;      // Ambient velocity x
        double vy = 0.0;      // Ambient velocity y
        double vz = 0.0;      // Ambient velocity z
    };

    DefineSphAmbientDrag() = default;

    std::string getKeywordName() const override { return "*DEFINE_SPH_AMBIENT_DRAG"; }

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
 * @brief *DEFINE_SPH_TO_SPH_COUPLING
 *
 * SPH to SPH coupling definition.
 */
class KOO_API DefineSphToSphCoupling : public CloneableKeyword<DefineSphToSphCoupling, SphKeyword> {
public:
    struct Data {
        int pid1 = 0;         // First part ID
        int pid2 = 0;         // Second part ID
        int ctype = 0;        // Coupling type
        double stiff = 0.0;   // Coupling stiffness
        double damp = 0.0;    // Coupling damping
    };

    DefineSphToSphCoupling() = default;

    std::string getKeywordName() const override { return "*DEFINE_SPH_TO_SPH_COUPLING"; }

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
 * @brief *DEFINE_SPH_INJECTION
 *
 * SPH particle injection definition.
 */
class KOO_API DefineSphInjection : public CloneableKeyword<DefineSphInjection, SphKeyword> {
public:
    struct Data {
        int id = 0;           // Injection ID
        int pid = 0;          // Part ID
        int nsid = 0;         // Node set ID for injection surface
        double rate = 0.0;    // Mass flow rate
        double vx = 0.0;      // Injection velocity x
        double vy = 0.0;      // Injection velocity y
        double vz = 0.0;      // Injection velocity z
        double temp = 0.0;    // Temperature
    };

    DefineSphInjection() = default;

    std::string getKeywordName() const override { return "*DEFINE_SPH_INJECTION"; }

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
 * @brief *DATABASE_SPH_FLOW
 *
 * SPH flow database output.
 */
class KOO_API DatabaseSphFlow : public CloneableKeyword<DatabaseSphFlow, SphKeyword> {
public:
    struct Data {
        double dt = 0.0;      // Output interval
        int lcdt = 0;         // Load curve for output interval
        int binary = 0;       // Binary output flag
        int lcur = 0;         // Curve number
    };

    DatabaseSphFlow() = default;

    std::string getKeywordName() const override { return "*DATABASE_SPH_FLOW"; }

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
