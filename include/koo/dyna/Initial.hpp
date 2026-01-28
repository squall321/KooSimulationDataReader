#pragma once

#include <koo/Export.hpp>
#include <koo/dyna/Keyword.hpp>
#include <koo/util/Types.hpp>
#include <vector>

namespace koo::dyna {

/**
 * @brief Base class for *INITIAL keywords
 */
class KOO_API InitialKeyword : public Keyword {
public:
    virtual ~InitialKeyword() = default;
};

/**
 * @brief *INITIAL_VELOCITY
 *
 * Defines initial translational velocities for nodes.
 */
class KOO_API InitialVelocity : public CloneableKeyword<InitialVelocity, InitialKeyword> {
public:
    struct NodeVelocity {
        NodeId nid = 0;         // Node ID
        double vx = 0.0;        // X velocity
        double vy = 0.0;        // Y velocity
        double vz = 0.0;        // Z velocity
    };

    InitialVelocity() = default;

    std::string getKeywordName() const override { return "*INITIAL_VELOCITY"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    // Data access
    std::vector<NodeVelocity>& getVelocities() { return velocities_; }
    const std::vector<NodeVelocity>& getVelocities() const { return velocities_; }

    void addVelocity(NodeId nid, double vx, double vy, double vz) {
        velocities_.push_back({nid, vx, vy, vz});
    }
    void clearVelocities() { velocities_.clear(); }

private:
    std::vector<NodeVelocity> velocities_;
};

/**
 * @brief *INITIAL_VELOCITY_NODE
 *
 * Defines initial velocities for specific nodes.
 */
class KOO_API InitialVelocityNode : public CloneableKeyword<InitialVelocityNode, InitialKeyword> {
public:
    struct NodeVelocity {
        NodeId nid = 0;
        double vx = 0.0;
        double vy = 0.0;
        double vz = 0.0;
        double vrx = 0.0;       // Rotational X velocity
        double vry = 0.0;       // Rotational Y velocity
        double vrz = 0.0;       // Rotational Z velocity
    };

    InitialVelocityNode() = default;

    std::string getKeywordName() const override { return "*INITIAL_VELOCITY_NODE"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    std::vector<NodeVelocity>& getVelocities() { return velocities_; }
    const std::vector<NodeVelocity>& getVelocities() const { return velocities_; }

    void addVelocity(const NodeVelocity& vel) { velocities_.push_back(vel); }

private:
    std::vector<NodeVelocity> velocities_;
};

/**
 * @brief *INITIAL_VELOCITY_GENERATION
 *
 * Defines initial velocities using generation parameters.
 */
class KOO_API InitialVelocityGeneration : public CloneableKeyword<InitialVelocityGeneration, InitialKeyword> {
public:
    struct Data {
        int nsid = 0;           // Node set ID
        int nsidex = 0;         // Excluded node set ID
        int boxid = 0;          // Box ID for node selection
        int irigid = 0;         // Rigid body option
        double vx = 0.0;        // X velocity
        double vy = 0.0;        // Y velocity
        double vz = 0.0;        // Z velocity
        double vrx = 0.0;       // Rotational X velocity
        double vry = 0.0;       // Rotational Y velocity
        double vrz = 0.0;       // Rotational Z velocity
        int icid = 0;           // Local coordinate system
        double omega = 0.0;     // Angular velocity magnitude
        double xc = 0.0;        // Center of rotation x
        double yc = 0.0;        // Center of rotation y
        double zc = 0.0;        // Center of rotation z
        double ax = 0.0;        // Axis of rotation x
        double ay = 0.0;        // Axis of rotation y
        double az = 0.0;        // Axis of rotation z
        int phase = 0;          // Phase of velocity
    };

    InitialVelocityGeneration() = default;

    std::string getKeywordName() const override { return "*INITIAL_VELOCITY_GENERATION"; }

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
 * @brief *INITIAL_STRESS_SHELL
 *
 * Defines initial stresses for shell elements.
 */
class KOO_API InitialStressShell : public CloneableKeyword<InitialStressShell, InitialKeyword> {
public:
    struct StressData {
        ElementId eid = 0;      // Element ID
        int nplane = 0;         // Number of integration points in plane
        int nthick = 0;         // Number of integration points through thickness
        int nhisv = 0;          // Number of history variables
        int large = 0;          // Large format flag
        // Stress components at integration points would follow
        std::vector<double> stresses;
    };

    InitialStressShell() = default;

    std::string getKeywordName() const override { return "*INITIAL_STRESS_SHELL"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    std::vector<StressData>& getStresses() { return stresses_; }
    const std::vector<StressData>& getStresses() const { return stresses_; }

private:
    std::vector<StressData> stresses_;
};

/**
 * @brief *INITIAL_STRESS_SOLID
 *
 * Defines initial stresses for solid elements.
 */
class KOO_API InitialStressSolid : public CloneableKeyword<InitialStressSolid, InitialKeyword> {
public:
    struct StressData {
        ElementId eid = 0;      // Element ID
        int nint = 0;           // Number of integration points
        int nhisv = 0;          // Number of history variables
        int large = 0;          // Large format flag
        std::vector<double> stresses;
    };

    InitialStressSolid() = default;

    std::string getKeywordName() const override { return "*INITIAL_STRESS_SOLID"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    std::vector<StressData>& getStresses() { return stresses_; }
    const std::vector<StressData>& getStresses() const { return stresses_; }

private:
    std::vector<StressData> stresses_;
};

/**
 * @brief *INITIAL_STRAIN_SHELL
 *
 * Defines initial strains for shell elements.
 */
class KOO_API InitialStrainShell : public CloneableKeyword<InitialStrainShell, InitialKeyword> {
public:
    struct StrainData {
        ElementId eid = 0;
        int nplane = 0;
        int nthick = 0;
        double eps_xx = 0.0;
        double eps_yy = 0.0;
        double eps_zz = 0.0;
        double eps_xy = 0.0;
        double eps_yz = 0.0;
        double eps_zx = 0.0;
    };

    InitialStrainShell() = default;

    std::string getKeywordName() const override { return "*INITIAL_STRAIN_SHELL"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    std::vector<StrainData>& getStrains() { return strains_; }
    const std::vector<StrainData>& getStrains() const { return strains_; }

private:
    std::vector<StrainData> strains_;
};

/**
 * @brief *INITIAL_STRAIN_SOLID
 *
 * Defines initial strains for solid elements.
 */
class KOO_API InitialStrainSolid : public CloneableKeyword<InitialStrainSolid, InitialKeyword> {
public:
    struct StrainData {
        ElementId eid = 0;
        int nint = 0;
        double eps_xx = 0.0;
        double eps_yy = 0.0;
        double eps_zz = 0.0;
        double eps_xy = 0.0;
        double eps_yz = 0.0;
        double eps_zx = 0.0;
    };

    InitialStrainSolid() = default;

    std::string getKeywordName() const override { return "*INITIAL_STRAIN_SOLID"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    std::vector<StrainData>& getStrains() { return strains_; }
    const std::vector<StrainData>& getStrains() const { return strains_; }

private:
    std::vector<StrainData> strains_;
};

/**
 * @brief *INITIAL_FOAM_REFERENCE_GEOMETRY
 *
 * Defines reference geometry for foam materials.
 */
class KOO_API InitialFoamReferenceGeometry : public CloneableKeyword<InitialFoamReferenceGeometry, InitialKeyword> {
public:
    struct Data {
        int pid = 0;            // Part ID
        int nid = 0;            // Node ID
        double x = 0.0;         // X coordinate
        double y = 0.0;         // Y coordinate
        double z = 0.0;         // Z coordinate
    };

    InitialFoamReferenceGeometry() = default;

    std::string getKeywordName() const override { return "*INITIAL_FOAM_REFERENCE_GEOMETRY"; }

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
 * @brief *INITIAL_DETONATION
 *
 * Defines initial detonation point for explosives.
 */
class KOO_API InitialDetonation : public CloneableKeyword<InitialDetonation, InitialKeyword> {
public:
    struct Data {
        int pid = 0;            // Part ID
        double x = 0.0;         // X coordinate of detonation
        double y = 0.0;         // Y coordinate of detonation
        double z = 0.0;         // Z coordinate of detonation
        double lt = 0.0;        // Lighting time
    };

    InitialDetonation() = default;

    std::string getKeywordName() const override { return "*INITIAL_DETONATION"; }

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
 * @brief *INITIAL_AIRBAG_PARTICLE_POSITION
 *
 * Defines initial positions for airbag particles.
 */
class KOO_API InitialAirbagParticlePosition : public CloneableKeyword<InitialAirbagParticlePosition, InitialKeyword> {
public:
    struct Data {
        int id = 0;             // Particle ID
        double x = 0.0;         // X position
        double y = 0.0;         // Y position
        double z = 0.0;         // Z position
        double u = 0.0;         // X velocity
        double v = 0.0;         // Y velocity
        double w = 0.0;         // Z velocity
        double mass = 0.0;      // Mass
    };

    InitialAirbagParticlePosition() = default;

    std::string getKeywordName() const override { return "*INITIAL_AIRBAG_PARTICLE_POSITION"; }

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
 * @brief *INITIAL_AXIAL_FORCE_BEAM
 *
 * Defines initial axial force for beam elements.
 */
class KOO_API InitialAxialForceBeam : public CloneableKeyword<InitialAxialForceBeam, InitialKeyword> {
public:
    struct Entry {
        ElementId eid = 0;      // Element ID
        double axial = 0.0;     // Axial force
    };

    InitialAxialForceBeam() = default;

    std::string getKeywordName() const override { return "*INITIAL_AXIAL_FORCE_BEAM"; }

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
 * @brief *INITIAL_ALE_PRESSURE
 *
 * Defines initial ALE pressure for parts.
 */
class KOO_API InitialAlePressure : public CloneableKeyword<InitialAlePressure, InitialKeyword> {
public:
    struct Entry {
        PartId pid = 0;         // Part ID
        double pressure = 0.0;  // Pressure value
    };

    InitialAlePressure() = default;

    std::string getKeywordName() const override { return "*INITIAL_ALE_PRESSURE"; }

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
 * @brief *INITIAL_GAS_MIXTURE
 *
 * Defines initial gas mixture for airbag/chamber simulations.
 */
class KOO_API InitialGasMixture : public CloneableKeyword<InitialGasMixture, InitialKeyword> {
public:
    struct Data {
        int bagid = 0;          // Airbag ID
        int gasid = 0;          // Gas ID
        double mf = 0.0;        // Mass fraction
        double temp = 0.0;      // Temperature
        double p = 0.0;         // Pressure
    };

    InitialGasMixture() = default;

    std::string getKeywordName() const override { return "*INITIAL_GAS_MIXTURE"; }

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
 * @brief *INITIAL_STRESS_BEAM
 *
 * Defines initial stresses for beam elements.
 */
class KOO_API InitialStressBeam : public CloneableKeyword<InitialStressBeam, InitialKeyword> {
public:
    struct StressData {
        ElementId eid = 0;        // Element ID
        int nint = 0;             // Number of integration points
        int nhisv = 0;            // Number of history variables
        double axial = 0.0;       // Axial stress
        double shear_s = 0.0;     // Shear stress (s-direction)
        double shear_t = 0.0;     // Shear stress (t-direction)
        double moment_s = 0.0;    // Moment (s-direction)
        double moment_t = 0.0;    // Moment (t-direction)
        double torsion = 0.0;     // Torsional moment
    };

    InitialStressBeam() = default;

    std::string getKeywordName() const override { return "*INITIAL_STRESS_BEAM"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    std::vector<StressData>& getStresses() { return stresses_; }
    const std::vector<StressData>& getStresses() const { return stresses_; }

private:
    std::vector<StressData> stresses_;
};

/**
 * @brief *INITIAL_VELOCITY_SET
 *
 * Defines initial velocities for node sets.
 */
class KOO_API InitialVelocitySet : public CloneableKeyword<InitialVelocitySet, InitialKeyword> {
public:
    struct Data {
        int nsid = 0;             // Node set ID
        double vx = 0.0;          // X velocity
        double vy = 0.0;          // Y velocity
        double vz = 0.0;          // Z velocity
        double vrx = 0.0;         // Rotational X velocity
        double vry = 0.0;         // Rotational Y velocity
        double vrz = 0.0;         // Rotational Z velocity
    };

    InitialVelocitySet() = default;

    std::string getKeywordName() const override { return "*INITIAL_VELOCITY_SET"; }

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
 * @brief *INITIAL_STRESS_SECTION
 *
 * Defines initial stresses for a section.
 */
class KOO_API InitialStressSection : public CloneableKeyword<InitialStressSection, InitialKeyword> {
public:
    struct Data {
        int psid = 0;             // Part set ID
        int secid = 0;            // Section ID
        double sigxx = 0.0;       // Normal stress in x
        double sigyy = 0.0;       // Normal stress in y
        double sigzz = 0.0;       // Normal stress in z
        double sigxy = 0.0;       // Shear stress xy
        double sigyz = 0.0;       // Shear stress yz
        double sigzx = 0.0;       // Shear stress zx
    };

    InitialStressSection() = default;

    std::string getKeywordName() const override { return "*INITIAL_STRESS_SECTION"; }

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
 * @brief *INITIAL_STRESS_TSHELL
 *
 * Defines initial stresses for thick shell elements.
 */
class KOO_API InitialStressTshell : public CloneableKeyword<InitialStressTshell, InitialKeyword> {
public:
    struct StressData {
        ElementId eid = 0;        // Element ID
        int nplane = 0;           // Number of in-plane integration points
        int nthick = 0;           // Number of through-thickness integration points
        int nhisv = 0;            // Number of history variables
        int large = 0;            // Large format flag
        std::vector<double> stresses;  // Stress data
    };

    InitialStressTshell() = default;

    std::string getKeywordName() const override { return "*INITIAL_STRESS_TSHELL"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    std::vector<StressData>& getStresses() { return stresses_; }
    const std::vector<StressData>& getStresses() const { return stresses_; }

private:
    std::vector<StressData> stresses_;
};

/**
 * @brief *INITIAL_HISTORY_NODE
 *
 * Defines initial history variables for nodes.
 */
class KOO_API InitialHistoryNode : public CloneableKeyword<InitialHistoryNode, InitialKeyword> {
public:
    struct Entry {
        NodeId nid = 0;           // Node ID
        int nhv = 0;              // Number of history variables
        std::vector<double> history;  // History variable values
    };

    InitialHistoryNode() = default;

    std::string getKeywordName() const override { return "*INITIAL_HISTORY_NODE"; }

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
 * @brief *INITIAL_HISTORY_SOLID
 *
 * Defines initial history variables for solid elements.
 */
class KOO_API InitialHistorySolid : public CloneableKeyword<InitialHistorySolid, InitialKeyword> {
public:
    struct Entry {
        ElementId eid = 0;        // Element ID
        int nint = 0;             // Number of integration points
        int nhv = 0;              // Number of history variables per integration point
        std::vector<double> history;  // History variable values
    };

    InitialHistorySolid() = default;

    std::string getKeywordName() const override { return "*INITIAL_HISTORY_SOLID"; }

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
 * @brief *INITIAL_HISTORY_SHELL
 *
 * Defines initial history variables for shell elements.
 */
class KOO_API InitialHistoryShell : public CloneableKeyword<InitialHistoryShell, InitialKeyword> {
public:
    struct Entry {
        ElementId eid = 0;        // Element ID
        int nplane = 0;           // Number of in-plane integration points
        int nthick = 0;           // Number of through-thickness integration points
        int nhv = 0;              // Number of history variables
        std::vector<double> history;  // History variable values
    };

    InitialHistoryShell() = default;

    std::string getKeywordName() const override { return "*INITIAL_HISTORY_SHELL"; }

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
 * @brief *INITIAL_STRESS_DEPTH
 *
 * Defines initial stress as a function of depth.
 */
class KOO_API InitialStressDepth : public CloneableKeyword<InitialStressDepth, InitialKeyword> {
public:
    struct Data {
        int psid = 0;             // Part set ID
        double sigxx = 0.0;       // Normal stress gradient x
        double sigyy = 0.0;       // Normal stress gradient y
        double sigzz = 0.0;       // Normal stress gradient z
        double sigxy = 0.0;       // Shear stress gradient xy
        double sigyz = 0.0;       // Shear stress gradient yz
        double sigzx = 0.0;       // Shear stress gradient zx
        double z0 = 0.0;          // Reference z coordinate
    };

    InitialStressDepth() = default;

    std::string getKeywordName() const override { return "*INITIAL_STRESS_DEPTH"; }

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
 * @brief *INITIAL_STRESS_DISCRETE
 *
 * Defines initial stresses for discrete elements.
 */
class KOO_API InitialStressDiscrete : public CloneableKeyword<InitialStressDiscrete, InitialKeyword> {
public:
    struct Entry {
        ElementId eid = 0;        // Element ID
        double def = 0.0;         // Deflection
        double ddef = 0.0;        // Deflection rate
    };

    InitialStressDiscrete() = default;

    std::string getKeywordName() const override { return "*INITIAL_STRESS_DISCRETE"; }

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
 * @brief *INITIAL_IMPULSE_MINE
 *
 * Defines initial impulse from mine blast.
 */
class KOO_API InitialImpulseMine : public CloneableKeyword<InitialImpulseMine, InitialKeyword> {
public:
    struct Data {
        int nsid = 0;             // Node set ID
        double xc = 0.0;          // Center x
        double yc = 0.0;          // Center y
        double zc = 0.0;          // Center z
        double impulse = 0.0;     // Impulse magnitude
        double lt = 0.0;          // Lighting time
        double rt = 0.0;          // Rise time
    };

    InitialImpulseMine() = default;

    std::string getKeywordName() const override { return "*INITIAL_IMPULSE_MINE"; }

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
 * @brief *INITIAL_PWP_DEPTH
 *
 * Defines initial pore water pressure as function of depth.
 */
class KOO_API InitialPwpDepth : public CloneableKeyword<InitialPwpDepth, InitialKeyword> {
public:
    struct Data {
        int psid = 0;             // Part set ID
        double pwp0 = 0.0;        // PWP at reference
        double dpwpdz = 0.0;      // PWP gradient
        double z0 = 0.0;          // Reference z coordinate
    };

    InitialPwpDepth() = default;

    std::string getKeywordName() const override { return "*INITIAL_PWP_DEPTH"; }

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
 * @brief *INITIAL_CONTACT_WEAR
 *
 * Defines initial wear for contact surfaces.
 */
class KOO_API InitialContactWear : public CloneableKeyword<InitialContactWear, InitialKeyword> {
public:
    struct Entry {
        int cid = 0;              // Contact ID
        int segid = 0;            // Segment ID
        double wear = 0.0;        // Wear value
    };

    InitialContactWear() = default;

    std::string getKeywordName() const override { return "*INITIAL_CONTACT_WEAR"; }

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
 * @brief *INITIAL_INTERNAL_DOF_SOLID
 *
 * Defines initial internal DOF for solid elements.
 */
class KOO_API InitialInternalDofSolid : public CloneableKeyword<InitialInternalDofSolid, InitialKeyword> {
public:
    struct Entry {
        ElementId eid = 0;        // Element ID
        int nint = 0;             // Number of integration points
        int ndof = 0;             // Number of DOFs
        std::vector<double> dofs; // DOF values
    };

    InitialInternalDofSolid() = default;

    std::string getKeywordName() const override { return "*INITIAL_INTERNAL_DOF_SOLID"; }

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
 * @brief *INITIAL_VOLUME_FRACTION
 *
 * Defines initial volume fraction for multi-material elements.
 */
class KOO_API InitialVolumeFraction : public CloneableKeyword<InitialVolumeFraction, InitialKeyword> {
public:
    struct Data {
        int ammgid = 0;           // Multi-material group ID
        int esid = 0;             // Element set ID
        double vf = 1.0;          // Volume fraction
    };

    InitialVolumeFraction() = default;

    std::string getKeywordName() const override { return "*INITIAL_VOLUME_FRACTION"; }

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
 * @brief *INITIAL_VOLUME_FRACTION_GEOMETRY
 *
 * Defines initial volume fraction using geometric shapes.
 */
class KOO_API InitialVolumeFractionGeometry : public CloneableKeyword<InitialVolumeFractionGeometry, InitialKeyword> {
public:
    struct Data {
        int ammgid = 0;           // Multi-material group ID
        int conttyp = 0;          // Container type
        int filltyp = 0;          // Fill type
        double xc = 0.0;          // Center x
        double yc = 0.0;          // Center y
        double zc = 0.0;          // Center z
        double radius = 0.0;      // Radius
        double height = 0.0;      // Height
    };

    InitialVolumeFractionGeometry() = default;

    std::string getKeywordName() const override { return "*INITIAL_VOLUME_FRACTION_GEOMETRY"; }

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
 * @brief *INITIAL_ALE_MAPPING
 *
 * Defines initial ALE mapping from a previous simulation.
 */
class KOO_API InitialAleMapping : public CloneableKeyword<InitialAleMapping, InitialKeyword> {
public:
    struct Data {
        std::string filename;     // File name
        int amgid = 0;            // ALE multi-material group ID
        int psid = 0;             // Part set ID
        double sx = 1.0;          // Scale factor x
        double sy = 1.0;          // Scale factor y
        double sz = 1.0;          // Scale factor z
    };

    InitialAleMapping() = default;

    std::string getKeywordName() const override { return "*INITIAL_ALE_MAPPING"; }

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
 * @brief *INITIAL_SPC_ROTATION_ANGLE
 *
 * Defines initial SPC rotation angle.
 */
class KOO_API InitialSpcRotationAngle : public CloneableKeyword<InitialSpcRotationAngle, InitialKeyword> {
public:
    struct Entry {
        NodeId nid = 0;           // Node ID
        double angle = 0.0;       // Rotation angle
    };

    InitialSpcRotationAngle() = default;

    std::string getKeywordName() const override { return "*INITIAL_SPC_ROTATION_ANGLE"; }

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
 * @brief *INITIAL_MOMENTUM
 *
 * Defines initial momentum for parts.
 */
class KOO_API InitialMomentum : public CloneableKeyword<InitialMomentum, InitialKeyword> {
public:
    struct Data {
        int pid = 0;              // Part ID
        double mx = 0.0;          // Momentum x
        double my = 0.0;          // Momentum y
        double mz = 0.0;          // Momentum z
    };

    InitialMomentum() = default;

    std::string getKeywordName() const override { return "*INITIAL_MOMENTUM"; }

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
 * @brief *INITIAL_ANGULAR_MOMENTUM
 *
 * Defines initial angular momentum for parts.
 */
class KOO_API InitialAngularMomentum : public CloneableKeyword<InitialAngularMomentum, InitialKeyword> {
public:
    struct Data {
        int pid = 0;              // Part ID
        double lx = 0.0;          // Angular momentum x
        double ly = 0.0;          // Angular momentum y
        double lz = 0.0;          // Angular momentum z
    };

    InitialAngularMomentum() = default;

    std::string getKeywordName() const override { return "*INITIAL_ANGULAR_MOMENTUM"; }

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
 * @brief *INITIAL_SPH_MASS_FRACTION
 *
 * Defines initial mass fraction for SPH particles.
 */
class KOO_API InitialSphMassFraction : public CloneableKeyword<InitialSphMassFraction, InitialKeyword> {
public:
    struct Entry {
        NodeId nid = 0;           // Node ID
        int ammgid = 0;           // Material group ID
        double mf = 1.0;          // Mass fraction
    };

    InitialSphMassFraction() = default;

    std::string getKeywordName() const override { return "*INITIAL_SPH_MASS_FRACTION"; }

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
 * @brief *INITIAL_ROTATIONAL_VELOCITY
 *
 * Defines initial rotational velocity about an axis.
 */
class KOO_API InitialRotationalVelocity : public CloneableKeyword<InitialRotationalVelocity, InitialKeyword> {
public:
    struct Data {
        int nsid = 0;             // Node set ID
        double omega = 0.0;       // Angular velocity
        double xc = 0.0;          // Center x
        double yc = 0.0;          // Center y
        double zc = 0.0;          // Center z
        double ax = 0.0;          // Axis x
        double ay = 0.0;          // Axis y
        double az = 0.0;          // Axis z
    };

    InitialRotationalVelocity() = default;

    std::string getKeywordName() const override { return "*INITIAL_ROTATIONAL_VELOCITY"; }

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
