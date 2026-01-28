#pragma once

#include <koo/Export.hpp>
#include <koo/dyna/Keyword.hpp>
#include <koo/util/Types.hpp>

namespace koo::dyna {

/**
 * @brief Base class for *BOUNDARY keywords
 */
class KOO_API BoundaryKeyword : public Keyword {
public:
    virtual ~BoundaryKeyword() = default;
};

/**
 * @brief SPC (Single Point Constraint) data
 */
struct SpcData {
    NodeId nid = 0;             // Node ID
    int dofx = 0;               // X translation constraint (0=free, 1=constrained)
    int dofy = 0;               // Y translation constraint
    int dofz = 0;               // Z translation constraint
    int dofrx = 0;              // X rotation constraint
    int dofry = 0;              // Y rotation constraint
    int dofrz = 0;              // Z rotation constraint
    int cid = 0;                // Coordinate system ID (optional)
};

/**
 * @brief *BOUNDARY_SPC_NODE
 *
 * Single Point Constraint on individual nodes.
 */
class KOO_API BoundarySpcNode : public CloneableKeyword<BoundarySpcNode, BoundaryKeyword> {
public:
    BoundarySpcNode() = default;

    std::string getKeywordName() const override { return "*BOUNDARY_SPC_NODE"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    // Data access
    std::vector<SpcData>& getConstraints() { return constraints_; }
    const std::vector<SpcData>& getConstraints() const { return constraints_; }

    void addConstraint(const SpcData& spc) { constraints_.push_back(spc); }
    void addConstraint(NodeId nid, int dofx, int dofy, int dofz,
                      int dofrx = 0, int dofry = 0, int dofrz = 0);
    void clear() { constraints_.clear(); }

private:
    std::vector<SpcData> constraints_;
};

/**
 * @brief *BOUNDARY_SPC_SET
 *
 * Single Point Constraint on node sets.
 */
class KOO_API BoundarySpcSet : public CloneableKeyword<BoundarySpcSet, BoundaryKeyword> {
public:
    struct Data {
        int nsid = 0;           // Node set ID
        int dofx = 0;
        int dofy = 0;
        int dofz = 0;
        int dofrx = 0;
        int dofry = 0;
        int dofrz = 0;
        int cid = 0;
    };

    BoundarySpcSet() = default;

    std::string getKeywordName() const override { return "*BOUNDARY_SPC_SET"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    std::vector<Data>& getConstraints() { return constraints_; }
    const std::vector<Data>& getConstraints() const { return constraints_; }

    void addConstraint(const Data& data) { constraints_.push_back(data); }
    void clear() { constraints_.clear(); }

private:
    std::vector<Data> constraints_;
};

/**
 * @brief Prescribed motion data
 */
struct PrescribedMotionData {
    int id = 0;                 // Node/set ID
    int dof = 0;                // Degree of freedom (1-6)
    int vad = 0;                // 0=velocity, 1=acceleration, 2=displacement
    int lcid = 0;               // Load curve ID
    double sf = 1.0;            // Scale factor
    int vid = 0;                // Vector ID for direction
    int death = 0;              // Deactivation time
    int birth = 0;              // Activation time
};

/**
 * @brief *BOUNDARY_PRESCRIBED_MOTION_NODE
 *
 * Prescribed motion on individual nodes.
 */
class KOO_API BoundaryPrescribedMotionNode : public CloneableKeyword<BoundaryPrescribedMotionNode, BoundaryKeyword> {
public:
    BoundaryPrescribedMotionNode() = default;

    std::string getKeywordName() const override { return "*BOUNDARY_PRESCRIBED_MOTION_NODE"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    std::vector<PrescribedMotionData>& getMotions() { return motions_; }
    const std::vector<PrescribedMotionData>& getMotions() const { return motions_; }

    void addMotion(const PrescribedMotionData& motion) { motions_.push_back(motion); }
    void clear() { motions_.clear(); }

private:
    std::vector<PrescribedMotionData> motions_;
};

/**
 * @brief *BOUNDARY_PRESCRIBED_MOTION_SET
 *
 * Prescribed motion on node sets.
 */
class KOO_API BoundaryPrescribedMotionSet : public CloneableKeyword<BoundaryPrescribedMotionSet, BoundaryKeyword> {
public:
    BoundaryPrescribedMotionSet() = default;

    std::string getKeywordName() const override { return "*BOUNDARY_PRESCRIBED_MOTION_SET"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    std::vector<PrescribedMotionData>& getMotions() { return motions_; }
    const std::vector<PrescribedMotionData>& getMotions() const { return motions_; }

    void addMotion(const PrescribedMotionData& motion) { motions_.push_back(motion); }
    void clear() { motions_.clear(); }

private:
    std::vector<PrescribedMotionData> motions_;
};

/**
 * @brief *BOUNDARY_PRESCRIBED_MOTION_RIGID
 *
 * Prescribed motion on rigid bodies.
 */
class KOO_API BoundaryPrescribedMotionRigid : public CloneableKeyword<BoundaryPrescribedMotionRigid, BoundaryKeyword> {
public:
    BoundaryPrescribedMotionRigid() = default;

    std::string getKeywordName() const override { return "*BOUNDARY_PRESCRIBED_MOTION_RIGID"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    std::vector<PrescribedMotionData>& getMotions() { return motions_; }
    const std::vector<PrescribedMotionData>& getMotions() const { return motions_; }

    void addMotion(const PrescribedMotionData& motion) { motions_.push_back(motion); }
    void clear() { motions_.clear(); }

private:
    std::vector<PrescribedMotionData> motions_;
};

/**
 * @brief *BOUNDARY_THERMAL_NODE
 *
 * Thermal boundary conditions on nodes.
 */
class KOO_API BoundaryThermalNode : public CloneableKeyword<BoundaryThermalNode, BoundaryKeyword> {
public:
    struct Data {
        int nid = 0;
        int lcid = 0;
        double sf = 1.0;
    };

    BoundaryThermalNode() = default;

    std::string getKeywordName() const override { return "*BOUNDARY_THERMAL_NODE"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    std::vector<Data>& getConstraints() { return constraints_; }
    const std::vector<Data>& getConstraints() const { return constraints_; }

private:
    std::vector<Data> constraints_;
};

/**
 * @brief *BOUNDARY_THERMAL_SET
 *
 * Thermal boundary conditions on node sets.
 */
class KOO_API BoundaryThermalSet : public CloneableKeyword<BoundaryThermalSet, BoundaryKeyword> {
public:
    struct Data {
        int nsid = 0;
        int lcid = 0;
        double sf = 1.0;
    };

    BoundaryThermalSet() = default;

    std::string getKeywordName() const override { return "*BOUNDARY_THERMAL_SET"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    std::vector<Data>& getConstraints() { return constraints_; }
    const std::vector<Data>& getConstraints() const { return constraints_; }

private:
    std::vector<Data> constraints_;
};

/**
 * @brief *BOUNDARY_CONVECTION_SET
 *
 * Convection boundary conditions.
 */
class KOO_API BoundaryConvectionSet : public CloneableKeyword<BoundaryConvectionSet, BoundaryKeyword> {
public:
    struct Data {
        int ssid = 0;
        int pseteflag = 0;
        int hlcid = 0;
        int hmult = 0;
        int tlcid = 0;
        int tmult = 0;
        double loc = 0.0;
    };

    BoundaryConvectionSet() = default;

    std::string getKeywordName() const override { return "*BOUNDARY_CONVECTION_SET"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    std::vector<Data>& getConditions() { return conditions_; }
    const std::vector<Data>& getConditions() const { return conditions_; }

private:
    std::vector<Data> conditions_;
};

/**
 * @brief *BOUNDARY_RADIATION_SET
 *
 * Radiation boundary conditions.
 */
class KOO_API BoundaryRadiationSet : public CloneableKeyword<BoundaryRadiationSet, BoundaryKeyword> {
public:
    struct Data {
        int ssid = 0;
        int pseteflag = 0;
        int type = 0;
        int elcid = 0;
        int tlcid = 0;
        double loc = 0.0;
    };

    BoundaryRadiationSet() = default;

    std::string getKeywordName() const override { return "*BOUNDARY_RADIATION_SET"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    std::vector<Data>& getConditions() { return conditions_; }
    const std::vector<Data>& getConditions() const { return conditions_; }

private:
    std::vector<Data> conditions_;
};

/**
 * @brief *BOUNDARY_FLUX_SET
 *
 * Heat flux boundary conditions on segment sets.
 */
class KOO_API BoundaryFluxSet : public CloneableKeyword<BoundaryFluxSet, BoundaryKeyword> {
public:
    struct Data {
        int ssid = 0;
        int pseteflag = 0;
        int lcid = 0;
        double sf = 1.0;
        double loc = 0.0;
    };

    BoundaryFluxSet() = default;

    std::string getKeywordName() const override { return "*BOUNDARY_FLUX_SET"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    std::vector<Data>& getConditions() { return conditions_; }
    const std::vector<Data>& getConditions() const { return conditions_; }

private:
    std::vector<Data> conditions_;
};

/**
 * @brief *BOUNDARY_NON_REFLECTING
 *
 * Non-reflecting boundary for wave propagation.
 */
class KOO_API BoundaryNonReflecting : public CloneableKeyword<BoundaryNonReflecting, BoundaryKeyword> {
public:
    struct Data {
        int ssid = 0;
        int ad = 0;
        int as = 0;
    };

    BoundaryNonReflecting() = default;

    std::string getKeywordName() const override { return "*BOUNDARY_NON_REFLECTING"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    std::vector<Data>& getConditions() { return conditions_; }
    const std::vector<Data>& getConditions() const { return conditions_; }

private:
    std::vector<Data> conditions_;
};

/**
 * @brief *BOUNDARY_SPC_SET_BIRTH_DEATH
 *
 * SPC with birth/death time control.
 */
class KOO_API BoundarySpcSetBirthDeath : public CloneableKeyword<BoundarySpcSetBirthDeath, BoundaryKeyword> {
public:
    struct Data {
        int nsid = 0;
        int cid = 0;
        int dofx = 0;
        int dofy = 0;
        int dofz = 0;
        int dofrx = 0;
        int dofry = 0;
        int dofrz = 0;
        double birth = 0.0;
        double death = 1.0e28;
    };

    BoundarySpcSetBirthDeath() = default;

    std::string getKeywordName() const override { return "*BOUNDARY_SPC_SET_BIRTH_DEATH"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    std::vector<Data>& getConstraints() { return constraints_; }
    const std::vector<Data>& getConstraints() const { return constraints_; }

private:
    std::vector<Data> constraints_;
};

/**
 * @brief *BOUNDARY_PRESCRIBED_MOTION_SET_BOX
 *
 * Prescribed motion on node sets with box activation.
 */
class KOO_API BoundaryPrescribedMotionSetBox : public CloneableKeyword<BoundaryPrescribedMotionSetBox, BoundaryKeyword> {
public:
    struct Data {
        int nsid = 0;           // Node set ID
        int dof = 0;            // Degree of freedom
        int vad = 0;            // Motion type
        int lcid = 0;           // Load curve ID
        double sf = 1.0;        // Scale factor
        int vid = 0;            // Vector ID
        double death = 1e28;    // Death time
        double birth = 0.0;     // Birth time
        int boxid = 0;          // Box ID
    };

    BoundaryPrescribedMotionSetBox() = default;

    std::string getKeywordName() const override { return "*BOUNDARY_PRESCRIBED_MOTION_SET_BOX"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    std::vector<Data>& getMotions() { return motions_; }
    const std::vector<Data>& getMotions() const { return motions_; }

private:
    std::vector<Data> motions_;
};

/**
 * @brief *BOUNDARY_CYCLIC
 *
 * Cyclic symmetry boundary conditions.
 */
class KOO_API BoundaryCyclic : public CloneableKeyword<BoundaryCyclic, BoundaryKeyword> {
public:
    struct Data {
        int nsid1 = 0;          // First node set ID
        int nsid2 = 0;          // Second node set ID
        int nid = 0;            // Node on axis
        double cx = 0.0;        // X component of axis
        double cy = 0.0;        // Y component of axis
        double cz = 0.0;        // Z component of axis
    };

    BoundaryCyclic() = default;

    std::string getKeywordName() const override { return "*BOUNDARY_CYCLIC"; }

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
 * @brief *BOUNDARY_SLIDING_PLANE
 *
 * Sliding plane boundary for frictionless sliding.
 */
class KOO_API BoundarySlidingPlane : public CloneableKeyword<BoundarySlidingPlane, BoundaryKeyword> {
public:
    struct Data {
        int nsid = 0;           // Node set ID
        int vector = 0;         // Vector ID
        double xt = 0.0;        // X of tail
        double yt = 0.0;        // Y of tail
        double zt = 0.0;        // Z of tail
        double xh = 0.0;        // X of head
        double yh = 0.0;        // Y of head
        double zh = 1.0;        // Z of head
    };

    BoundarySlidingPlane() = default;

    std::string getKeywordName() const override { return "*BOUNDARY_SLIDING_PLANE"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    std::vector<Data>& getConditions() { return conditions_; }
    const std::vector<Data>& getConditions() const { return conditions_; }

private:
    std::vector<Data> conditions_;
};

/**
 * @brief *BOUNDARY_AMBIENT_EOS
 *
 * Ambient equation of state boundary for ALE.
 */
class KOO_API BoundaryAmbientEos : public CloneableKeyword<BoundaryAmbientEos, BoundaryKeyword> {
public:
    struct Data {
        int ssid = 0;           // Segment set ID
        int eos = 0;            // EOS ID
        double r0 = 0.0;        // Reference density
        double p0 = 0.0;        // Reference pressure
        double t0 = 0.0;        // Reference temperature
        double e0 = 0.0;        // Reference internal energy
    };

    BoundaryAmbientEos() = default;

    std::string getKeywordName() const override { return "*BOUNDARY_AMBIENT_EOS"; }

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
 * @brief *BOUNDARY_FLUX_TRAJECTORY
 *
 * Heat flux along trajectory boundary.
 */
class KOO_API BoundaryFluxTrajectory : public CloneableKeyword<BoundaryFluxTrajectory, BoundaryKeyword> {
public:
    struct Data {
        int pid = 0;            // Part ID
        int ssid = 0;           // Segment set ID
        int lcid = 0;           // Load curve ID
        double sf = 1.0;        // Scale factor
        double loc = 0.0;       // Location
    };

    BoundaryFluxTrajectory() = default;

    std::string getKeywordName() const override { return "*BOUNDARY_FLUX_TRAJECTORY"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    std::vector<Data>& getConditions() { return conditions_; }
    const std::vector<Data>& getConditions() const { return conditions_; }

private:
    std::vector<Data> conditions_;
};

/**
 * @brief *BOUNDARY_PAP
 *
 * Pressure boundary for ALE parts.
 */
class KOO_API BoundaryPap : public CloneableKeyword<BoundaryPap, BoundaryKeyword> {
public:
    struct Data {
        int ssid = 0;           // Segment set ID
        int lcid = 0;           // Load curve ID
        double sf = 1.0;        // Scale factor
        double death = 1e28;    // Death time
        double birth = 0.0;     // Birth time
    };

    BoundaryPap() = default;

    std::string getKeywordName() const override { return "*BOUNDARY_PAP"; }

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
 * @brief *BOUNDARY_ACOUSTIC_IMPEDANCE
 *
 * Acoustic impedance boundary conditions.
 */
class KOO_API BoundaryAcousticImpedance : public CloneableKeyword<BoundaryAcousticImpedance, BoundaryKeyword> {
public:
    struct Data {
        int ssid = 0;           // Segment set ID
        double imp = 0.0;       // Impedance value
        int lcid = 0;           // Load curve ID for impedance
        double sf = 1.0;        // Scale factor
    };

    BoundaryAcousticImpedance() = default;

    std::string getKeywordName() const override { return "*BOUNDARY_ACOUSTIC_IMPEDANCE"; }

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
 * @brief *BOUNDARY_PRESCRIBED_ACCELEROMETER
 *
 * Apply prescribed accelerations from accelerometer data to a node set.
 */
class KOO_API BoundaryPrescribedAccelerometer : public CloneableKeyword<BoundaryPrescribedAccelerometer, BoundaryKeyword> {
public:
    struct Data {
        int nsid = 0;           // Node set ID
        int dof = 0;            // Degree of freedom (1-3)
        int accid = 0;          // Accelerometer ID
        double sf = 1.0;        // Scale factor
        double vid = 0;         // Vector ID for direction
        double death = 1e28;    // Death time
        double birth = 0.0;     // Birth time
    };

    BoundaryPrescribedAccelerometer() = default;

    std::string getKeywordName() const override { return "*BOUNDARY_PRESCRIBED_ACCELEROMETER"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    std::vector<Data>& getConditions() { return conditions_; }
    const std::vector<Data>& getConditions() const { return conditions_; }

private:
    std::vector<Data> conditions_;
};

/**
 * @brief *BOUNDARY_PRESCRIBED_ORIENTATION_RIGID
 *
 * Prescribed orientation for rigid body.
 */
class KOO_API BoundaryPrescribedOrientationRigid : public CloneableKeyword<BoundaryPrescribedOrientationRigid, BoundaryKeyword> {
public:
    struct Data {
        int pid = 0;            // Part ID
        int lcid = 0;           // Load curve ID
        int cid = 0;            // Coordinate system ID
        double sf = 1.0;        // Scale factor
        int dir = 0;            // Direction
    };

    BoundaryPrescribedOrientationRigid() = default;

    std::string getKeywordName() const override { return "*BOUNDARY_PRESCRIBED_ORIENTATION_RIGID"; }

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
 * @brief *BOUNDARY_SPC_SYMMETRY_PLANE
 *
 * SPC conditions for symmetry planes.
 */
class KOO_API BoundarySpcSymmetryPlane : public CloneableKeyword<BoundarySpcSymmetryPlane, BoundaryKeyword> {
public:
    struct Data {
        int nsid = 0;           // Node set ID
        int plane = 0;          // Symmetry plane (1=YZ, 2=ZX, 3=XY)
    };

    BoundarySpcSymmetryPlane() = default;

    std::string getKeywordName() const override { return "*BOUNDARY_SPC_SYMMETRY_PLANE"; }

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
 * @brief *BOUNDARY_PRECRACK
 *
 * Pre-crack definition for crack propagation.
 */
class KOO_API BoundaryPrecrack : public CloneableKeyword<BoundaryPrecrack, BoundaryKeyword> {
public:
    struct Data {
        int pid = 0;            // Part ID
        NodeId n1 = 0;          // Node 1
        NodeId n2 = 0;          // Node 2
        NodeId n3 = 0;          // Node 3
        NodeId n4 = 0;          // Node 4
    };

    BoundaryPrecrack() = default;

    std::string getKeywordName() const override { return "*BOUNDARY_PRECRACK"; }

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
 * @brief *BOUNDARY_MCOL
 *
 * MCOL boundary condition for coupled analysis.
 */
class KOO_API BoundaryMcol : public CloneableKeyword<BoundaryMcol, BoundaryKeyword> {
public:
    struct Data {
        int id = 0;             // ID
        int type = 0;           // Type
        int psid = 0;           // Part set ID
        double sf = 1.0;        // Scale factor
    };

    BoundaryMcol() = default;

    std::string getKeywordName() const override { return "*BOUNDARY_MCOL"; }

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
 * @brief *BOUNDARY_PRESCRIBED_FINAL_GEOMETRY
 *
 * Prescribe final geometry for forming analysis.
 */
class KOO_API BoundaryPrescribedFinalGeometry : public CloneableKeyword<BoundaryPrescribedFinalGeometry, BoundaryKeyword> {
public:
    struct Data {
        int psid = 0;           // Part set ID
        int ssid = 0;           // Segment set ID for target geometry
        double tol = 0.0;       // Tolerance
        int lcid = 0;           // Load curve for time dependency
    };

    BoundaryPrescribedFinalGeometry() = default;

    std::string getKeywordName() const override { return "*BOUNDARY_PRESCRIBED_FINAL_GEOMETRY"; }

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
 * @brief *BOUNDARY_SPH_SYMMETRY_PLANE
 *
 * SPH symmetry plane boundary condition.
 */
class KOO_API BoundarySphSymmetryPlane : public CloneableKeyword<BoundarySphSymmetryPlane, BoundaryKeyword> {
public:
    struct Data {
        int plane = 0;          // Plane type (1=XY, 2=YZ, 3=ZX)
        double coord = 0.0;     // Plane coordinate
        int dir = 0;            // Direction (-1 or 1)
    };

    BoundarySphSymmetryPlane() = default;

    std::string getKeywordName() const override { return "*BOUNDARY_SPH_SYMMETRY_PLANE"; }

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
 * @brief *BOUNDARY_SPH_FLOW
 *
 * SPH flow boundary condition.
 */
class KOO_API BoundarySphFlow : public CloneableKeyword<BoundarySphFlow, BoundaryKeyword> {
public:
    struct Data {
        int ssid = 0;           // Segment set ID
        int lcid = 0;           // Load curve ID
        double sf = 1.0;        // Scale factor
        int opt = 0;            // Flow option
    };

    BoundarySphFlow() = default;

    std::string getKeywordName() const override { return "*BOUNDARY_SPH_FLOW"; }

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
 * @brief *BOUNDARY_SPH_NON_REFLECTING
 *
 * SPH non-reflecting boundary condition.
 */
class KOO_API BoundarySphNonReflecting : public CloneableKeyword<BoundarySphNonReflecting, BoundaryKeyword> {
public:
    struct Data {
        int ssid = 0;           // Segment set ID
        int opt = 0;            // Option
        double damp = 0.0;      // Damping factor
    };

    BoundarySphNonReflecting() = default;

    std::string getKeywordName() const override { return "*BOUNDARY_SPH_NON_REFLECTING"; }

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
 * @brief *BOUNDARY_ACOUSTIC_COUPLING
 *
 * Acoustic-structural coupling boundary.
 */
class KOO_API BoundaryAcousticCoupling : public CloneableKeyword<BoundaryAcousticCoupling, BoundaryKeyword> {
public:
    struct Data {
        int ssid = 0;           // Segment set ID
        int psid = 0;           // Part set ID for acoustic
        int ctype = 0;          // Coupling type
        double sf = 1.0;        // Scale factor
    };

    BoundaryAcousticCoupling() = default;

    std::string getKeywordName() const override { return "*BOUNDARY_ACOUSTIC_COUPLING"; }

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
 * @brief *BOUNDARY_ACOUSTIC_FREE_SURFACE
 *
 * Free surface boundary for acoustic analysis.
 */
class KOO_API BoundaryAcousticFreeSurface : public CloneableKeyword<BoundaryAcousticFreeSurface, BoundaryKeyword> {
public:
    struct Data {
        int ssid = 0;           // Segment set ID
        double pres = 0.0;      // Reference pressure
    };

    BoundaryAcousticFreeSurface() = default;

    std::string getKeywordName() const override { return "*BOUNDARY_ACOUSTIC_FREE_SURFACE"; }

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
 * @brief *BOUNDARY_ACOUSTIC_NON_REFLECTING
 *
 * Non-reflecting boundary for acoustic analysis.
 */
class KOO_API BoundaryAcousticNonReflecting : public CloneableKeyword<BoundaryAcousticNonReflecting, BoundaryKeyword> {
public:
    struct Data {
        int ssid = 0;           // Segment set ID
        int ntype = 0;          // Non-reflecting type
    };

    BoundaryAcousticNonReflecting() = default;

    std::string getKeywordName() const override { return "*BOUNDARY_ACOUSTIC_NON_REFLECTING"; }

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
 * @brief *BOUNDARY_ACOUSTIC_MAPPING
 *
 * Acoustic mapping boundary.
 */
class KOO_API BoundaryAcousticMapping : public CloneableKeyword<BoundaryAcousticMapping, BoundaryKeyword> {
public:
    struct Data {
        int ssid = 0;           // Segment set ID
        int msid = 0;           // Mapping set ID
        int mtype = 0;          // Mapping type
    };

    BoundaryAcousticMapping() = default;

    std::string getKeywordName() const override { return "*BOUNDARY_ACOUSTIC_MAPPING"; }

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
 * @brief *BOUNDARY_ALE_MAPPING
 *
 * ALE mapping boundary condition.
 */
class KOO_API BoundaryAleMapping : public CloneableKeyword<BoundaryAleMapping, BoundaryKeyword> {
public:
    struct Data {
        int ssid = 0;           // Segment set ID
        int msid = 0;           // Mapping set ID
        int mtype = 0;          // Mapping type
        double sf = 1.0;        // Scale factor
    };

    BoundaryAleMapping() = default;

    std::string getKeywordName() const override { return "*BOUNDARY_ALE_MAPPING"; }

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
 * @brief *BOUNDARY_AMBIENT
 *
 * Ambient boundary condition.
 */
class KOO_API BoundaryAmbient : public CloneableKeyword<BoundaryAmbient, BoundaryKeyword> {
public:
    struct Data {
        int ssid = 0;           // Segment set ID
        int ammgid = 0;         // Ambient material group ID
        double rho = 0.0;       // Reference density
        double temp = 0.0;      // Temperature
        double pres = 0.0;      // Pressure
    };

    BoundaryAmbient() = default;

    std::string getKeywordName() const override { return "*BOUNDARY_AMBIENT"; }

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
 * @brief *BOUNDARY_DE_NON_REFLECTING
 *
 * Non-reflecting boundary for discrete elements.
 */
class KOO_API BoundaryDeNonReflecting : public CloneableKeyword<BoundaryDeNonReflecting, BoundaryKeyword> {
public:
    struct Data {
        int ssid = 0;           // Segment set ID
        int opt = 0;            // Option
    };

    BoundaryDeNonReflecting() = default;

    std::string getKeywordName() const override { return "*BOUNDARY_DE_NON_REFLECTING"; }

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
 * @brief *BOUNDARY_ELEMENT_METHOD
 *
 * Boundary element method definition.
 */
class KOO_API BoundaryElementMethod : public CloneableKeyword<BoundaryElementMethod, BoundaryKeyword> {
public:
    struct Data {
        int id = 0;             // BEM ID
        int type = 0;           // BEM type
        int ssid = 0;           // Segment set ID
        double sf = 1.0;        // Scale factor
    };

    BoundaryElementMethod() = default;

    std::string getKeywordName() const override { return "*BOUNDARY_ELEMENT_METHOD"; }

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
 * @brief *BOUNDARY_ELEMENT_METHOD_CONTROL
 *
 * Control parameters for boundary element method.
 */
class KOO_API BoundaryElementMethodControl : public CloneableKeyword<BoundaryElementMethodControl, BoundaryKeyword> {
public:
    struct Data {
        int bemid = 0;          // BEM ID
        int solver = 0;         // Solver type
        double tol = 1e-6;      // Tolerance
        int maxiter = 100;      // Maximum iterations
    };

    BoundaryElementMethodControl() = default;

    std::string getKeywordName() const override { return "*BOUNDARY_ELEMENT_METHOD_CONTROL"; }

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
 * @brief *BOUNDARY_NON_REFLECTING_2D
 *
 * 2D non-reflecting boundary.
 */
class KOO_API BoundaryNonReflecting2D : public CloneableKeyword<BoundaryNonReflecting2D, BoundaryKeyword> {
public:
    struct Data {
        int ssid = 0;           // Segment set ID
        int ad = 0;             // Absorbing type
        int as = 0;             // Absorbing option
    };

    BoundaryNonReflecting2D() = default;

    std::string getKeywordName() const override { return "*BOUNDARY_NON_REFLECTING_2D"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    std::vector<Data>& getConditions() { return conditions_; }
    const std::vector<Data>& getConditions() const { return conditions_; }

private:
    std::vector<Data> conditions_;
};

/**
 * @brief *BOUNDARY_PORE_FLUID
 *
 * Pore fluid boundary condition.
 */
class KOO_API BoundaryPoreFluid : public CloneableKeyword<BoundaryPoreFluid, BoundaryKeyword> {
public:
    struct Data {
        int nsid = 0;           // Node set ID
        int lcid = 0;           // Load curve ID
        double sf = 1.0;        // Scale factor
    };

    BoundaryPoreFluid() = default;

    std::string getKeywordName() const override { return "*BOUNDARY_PORE_FLUID"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    std::vector<Data>& getConditions() { return conditions_; }
    const std::vector<Data>& getConditions() const { return conditions_; }

private:
    std::vector<Data> conditions_;
};

/**
 * @brief *BOUNDARY_PRESCRIBED_MOTION_RIGID_LOCAL
 *
 * Prescribed motion for rigid bodies in local coordinates.
 */
class KOO_API BoundaryPrescribedMotionRigidLocal : public CloneableKeyword<BoundaryPrescribedMotionRigidLocal, BoundaryKeyword> {
public:
    struct Data {
        int pid = 0;            // Part ID
        int dof = 0;            // Degree of freedom
        int vad = 0;            // Velocity/acceleration/displacement
        int lcid = 0;           // Load curve ID
        double sf = 1.0;        // Scale factor
        int vid = 0;            // Vector ID
        double death = 1e28;    // Death time
        double birth = 0.0;     // Birth time
    };

    BoundaryPrescribedMotionRigidLocal() = default;

    std::string getKeywordName() const override { return "*BOUNDARY_PRESCRIBED_MOTION_RIGID_LOCAL"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    std::vector<Data>& getMotions() { return motions_; }
    const std::vector<Data>& getMotions() const { return motions_; }

private:
    std::vector<Data> motions_;
};

/**
 * @brief *BOUNDARY_PRESSURE_OUTFLOW
 *
 * Pressure outflow boundary for ALE.
 */
class KOO_API BoundaryPressureOutflow : public CloneableKeyword<BoundaryPressureOutflow, BoundaryKeyword> {
public:
    struct Data {
        int ssid = 0;           // Segment set ID
        int lcid = 0;           // Load curve ID
        double sf = 1.0;        // Scale factor
        double pref = 0.0;      // Reference pressure
    };

    BoundaryPressureOutflow() = default;

    std::string getKeywordName() const override { return "*BOUNDARY_PRESSURE_OUTFLOW"; }

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
 * @brief *BOUNDARY_PWP
 *
 * Pore water pressure boundary condition.
 */
class KOO_API BoundaryPwp : public CloneableKeyword<BoundaryPwp, BoundaryKeyword> {
public:
    struct Data {
        int nsid = 0;           // Node set ID
        int lcid = 0;           // Load curve ID
        double sf = 1.0;        // Scale factor
        double ref = 0.0;       // Reference value
    };

    BoundaryPwp() = default;

    std::string getKeywordName() const override { return "*BOUNDARY_PWP"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    std::vector<Data>& getConditions() { return conditions_; }
    const std::vector<Data>& getConditions() const { return conditions_; }

private:
    std::vector<Data> conditions_;
};

/**
 * @brief *BOUNDARY_PWP_NODE
 *
 * Pore water pressure on individual nodes.
 */
class KOO_API BoundaryPwpNode : public CloneableKeyword<BoundaryPwpNode, BoundaryKeyword> {
public:
    struct Data {
        NodeId nid = 0;         // Node ID
        int lcid = 0;           // Load curve ID
        double sf = 1.0;        // Scale factor
    };

    BoundaryPwpNode() = default;

    std::string getKeywordName() const override { return "*BOUNDARY_PWP_NODE"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    std::vector<Data>& getConditions() { return conditions_; }
    const std::vector<Data>& getConditions() const { return conditions_; }

private:
    std::vector<Data> conditions_;
};

/**
 * @brief *BOUNDARY_PZEPOT
 *
 * Piezoelectric potential boundary.
 */
class KOO_API BoundaryPzepot : public CloneableKeyword<BoundaryPzepot, BoundaryKeyword> {
public:
    struct Data {
        int nsid = 0;           // Node set ID
        int lcid = 0;           // Load curve ID
        double sf = 1.0;        // Scale factor
    };

    BoundaryPzepot() = default;

    std::string getKeywordName() const override { return "*BOUNDARY_PZEPOT"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    std::vector<Data>& getConditions() { return conditions_; }
    const std::vector<Data>& getConditions() const { return conditions_; }

private:
    std::vector<Data> conditions_;
};

/**
 * @brief *BOUNDARY_RADIATION_SEGMENT
 *
 * Radiation boundary on individual segments.
 */
class KOO_API BoundaryRadiationSegment : public CloneableKeyword<BoundaryRadiationSegment, BoundaryKeyword> {
public:
    struct Data {
        NodeId n1 = 0;          // Node 1
        NodeId n2 = 0;          // Node 2
        NodeId n3 = 0;          // Node 3
        NodeId n4 = 0;          // Node 4
        int type = 0;           // Radiation type
        int elcid = 0;          // Emissivity load curve
        int tlcid = 0;          // Temperature load curve
    };

    BoundaryRadiationSegment() = default;

    std::string getKeywordName() const override { return "*BOUNDARY_RADIATION_SEGMENT"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    std::vector<Data>& getConditions() { return conditions_; }
    const std::vector<Data>& getConditions() const { return conditions_; }

private:
    std::vector<Data> conditions_;
};

/**
 * @brief *BOUNDARY_SALE_MESH_FACE
 *
 * SALE mesh face boundary condition.
 */
class KOO_API BoundarySaleMeshFace : public CloneableKeyword<BoundarySaleMeshFace, BoundaryKeyword> {
public:
    struct Data {
        int ssid = 0;           // Segment set ID
        int opt = 0;            // Option
        int lcid = 0;           // Load curve ID
        double sf = 1.0;        // Scale factor
    };

    BoundarySaleMeshFace() = default;

    std::string getKeywordName() const override { return "*BOUNDARY_SALE_MESH_FACE"; }

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
 * @brief *BOUNDARY_SPC
 *
 * Single point constraint (generic).
 */
class KOO_API BoundarySpc : public CloneableKeyword<BoundarySpc, BoundaryKeyword> {
public:
    struct Data {
        NodeId nid = 0;         // Node ID
        int cid = 0;            // Coordinate system ID
        int dofx = 0;           // DOF X
        int dofy = 0;           // DOF Y
        int dofz = 0;           // DOF Z
        int dofrx = 0;          // DOF RX
        int dofry = 0;          // DOF RY
        int dofrz = 0;          // DOF RZ
    };

    BoundarySpc() = default;

    std::string getKeywordName() const override { return "*BOUNDARY_SPC"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    std::vector<Data>& getConstraints() { return constraints_; }
    const std::vector<Data>& getConstraints() const { return constraints_; }

private:
    std::vector<Data> constraints_;
};

/**
 * @brief *BOUNDARY_SPH_NOSLIP
 *
 * SPH no-slip boundary condition.
 */
class KOO_API BoundarySphNoslip : public CloneableKeyword<BoundarySphNoslip, BoundaryKeyword> {
public:
    struct Data {
        int ssid = 0;           // Segment set ID
        int type = 0;           // Boundary type
    };

    BoundarySphNoslip() = default;

    std::string getKeywordName() const override { return "*BOUNDARY_SPH_NOSLIP"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    std::vector<Data>& getConditions() { return conditions_; }
    const std::vector<Data>& getConditions() const { return conditions_; }

private:
    std::vector<Data> conditions_;
};

/**
 * @brief *BOUNDARY_SPH_PERIODIC
 *
 * SPH periodic boundary condition.
 */
class KOO_API BoundarySphPeriodic : public CloneableKeyword<BoundarySphPeriodic, BoundaryKeyword> {
public:
    struct Data {
        int ssid1 = 0;          // First segment set ID
        int ssid2 = 0;          // Second segment set ID
        int dir = 0;            // Direction
    };

    BoundarySphPeriodic() = default;

    std::string getKeywordName() const override { return "*BOUNDARY_SPH_PERIODIC"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    std::vector<Data>& getConditions() { return conditions_; }
    const std::vector<Data>& getConditions() const { return conditions_; }

private:
    std::vector<Data> conditions_;
};

/**
 * @brief *BOUNDARY_SYMMETRY_FAILURE
 *
 * Symmetry plane with failure capability.
 */
class KOO_API BoundarySymmetryFailure : public CloneableKeyword<BoundarySymmetryFailure, BoundaryKeyword> {
public:
    struct Data {
        int nsid = 0;           // Node set ID
        int plane = 0;          // Symmetry plane
        double fail = 0.0;      // Failure criterion
    };

    BoundarySymmetryFailure() = default;

    std::string getKeywordName() const override { return "*BOUNDARY_SYMMETRY_FAILURE"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    std::vector<Data>& getConditions() { return conditions_; }
    const std::vector<Data>& getConditions() const { return conditions_; }

private:
    std::vector<Data> conditions_;
};

/**
 * @brief *BOUNDARY_TEMPERATURE_RSW
 *
 * Resistance spot weld temperature boundary.
 */
class KOO_API BoundaryTemperatureRsw : public CloneableKeyword<BoundaryTemperatureRsw, BoundaryKeyword> {
public:
    struct Data {
        int nsid = 0;           // Node set ID
        int lcid = 0;           // Load curve ID
        double sf = 1.0;        // Scale factor
        int opt = 0;            // Option
    };

    BoundaryTemperatureRsw() = default;

    std::string getKeywordName() const override { return "*BOUNDARY_TEMPERATURE_RSW"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    std::vector<Data>& getConditions() { return conditions_; }
    const std::vector<Data>& getConditions() const { return conditions_; }

private:
    std::vector<Data> conditions_;
};

/**
 * @brief *BOUNDARY_TEMPERATURE_TRAJECTORY
 *
 * Temperature boundary along a trajectory.
 */
class KOO_API BoundaryTemperatureTrajectory : public CloneableKeyword<BoundaryTemperatureTrajectory, BoundaryKeyword> {
public:
    struct Data {
        int pid = 0;            // Part ID
        int nsid = 0;           // Node set ID
        int lcid = 0;           // Load curve ID
        double sf = 1.0;        // Scale factor
    };

    BoundaryTemperatureTrajectory() = default;

    std::string getKeywordName() const override { return "*BOUNDARY_TEMPERATURE_TRAJECTORY"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    std::vector<Data>& getConditions() { return conditions_; }
    const std::vector<Data>& getConditions() const { return conditions_; }

private:
    std::vector<Data> conditions_;
};

/**
 * @brief *BOUNDARY_THERMAL_BULKNODE
 *
 * Thermal bulk node boundary.
 */
class KOO_API BoundaryThermalBulknode : public CloneableKeyword<BoundaryThermalBulknode, BoundaryKeyword> {
public:
    struct Data {
        NodeId nid = 0;         // Node ID
        int lcid = 0;           // Load curve ID
        double sf = 1.0;        // Scale factor
        double hlc = 0.0;       // Heat transfer coefficient
    };

    BoundaryThermalBulknode() = default;

    std::string getKeywordName() const override { return "*BOUNDARY_THERMAL_BULKNODE"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    std::vector<Data>& getConditions() { return conditions_; }
    const std::vector<Data>& getConditions() const { return conditions_; }

private:
    std::vector<Data> conditions_;
};

/**
 * @brief *BOUNDARY_THERMAL_WELD
 *
 * Thermal weld boundary.
 */
class KOO_API BoundaryThermalWeld : public CloneableKeyword<BoundaryThermalWeld, BoundaryKeyword> {
public:
    struct Data {
        int nsid = 0;           // Node set ID
        int lcid = 0;           // Load curve ID
        double sf = 1.0;        // Scale factor
        int type = 0;           // Weld type
    };

    BoundaryThermalWeld() = default;

    std::string getKeywordName() const override { return "*BOUNDARY_THERMAL_WELD"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    std::vector<Data>& getConditions() { return conditions_; }
    const std::vector<Data>& getConditions() const { return conditions_; }

private:
    std::vector<Data> conditions_;
};

/**
 * @brief *BOUNDARY_THERMAL_WELD_TRAJECTORY
 *
 * Thermal weld along trajectory boundary.
 */
class KOO_API BoundaryThermalWeldTrajectory : public CloneableKeyword<BoundaryThermalWeldTrajectory, BoundaryKeyword> {
public:
    struct Data {
        int pid = 0;            // Part ID
        int nsid = 0;           // Node set ID
        int lcid = 0;           // Load curve ID
        double sf = 1.0;        // Scale factor
        double vel = 0.0;       // Welding velocity
    };

    BoundaryThermalWeldTrajectory() = default;

    std::string getKeywordName() const override { return "*BOUNDARY_THERMAL_WELD_TRAJECTORY"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    std::vector<Data>& getConditions() { return conditions_; }
    const std::vector<Data>& getConditions() const { return conditions_; }

private:
    std::vector<Data> conditions_;
};

/**
 * @brief *BOUNDARY_USA_SURFACE
 *
 * USA (Underwater Shock Analysis) surface boundary.
 */
class KOO_API BoundaryUsaSurface : public CloneableKeyword<BoundaryUsaSurface, BoundaryKeyword> {
public:
    struct Data {
        int ssid = 0;           // Segment set ID
        int type = 0;           // Surface type
        int opt = 0;            // Option
    };

    BoundaryUsaSurface() = default;

    std::string getKeywordName() const override { return "*BOUNDARY_USA_SURFACE"; }

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
 * @brief *BOUNDARY_PRESCRIBED_ACCELEROMETER_RIGID
 *
 * Prescribed accelerometer for rigid bodies.
 */
class KOO_API BoundaryPrescribedAccelerometerRigid : public CloneableKeyword<BoundaryPrescribedAccelerometerRigid, BoundaryKeyword> {
public:
    struct Data {
        int pid = 0;            // Part ID
        int dof = 0;            // Degree of freedom
        int accid = 0;          // Accelerometer ID
        double sf = 1.0;        // Scale factor
        int vid = 0;            // Vector ID
        double death = 1e28;    // Death time
        double birth = 0.0;     // Birth time
    };

    BoundaryPrescribedAccelerometerRigid() = default;

    std::string getKeywordName() const override { return "*BOUNDARY_PRESCRIBED_ACCELEROMETER_RIGID"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    std::vector<Data>& getConditions() { return conditions_; }
    const std::vector<Data>& getConditions() const { return conditions_; }

private:
    std::vector<Data> conditions_;
};

/**
 * @brief *BOUNDARY_FLUIDM_FREE_SURFACE
 *
 * Fluid M free surface boundary.
 */
class KOO_API BoundaryFluidmFreeSurface : public CloneableKeyword<BoundaryFluidmFreeSurface, BoundaryKeyword> {
public:
    struct Data {
        int ssid = 0;           // Segment set ID
        double pref = 0.0;      // Reference pressure
    };

    BoundaryFluidmFreeSurface() = default;

    std::string getKeywordName() const override { return "*BOUNDARY_FLUIDM_FREE_SURFACE"; }

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
 * @brief *BOUNDARY_COUPLED
 *
 * Coupled boundary condition.
 */
class KOO_API BoundaryCoupled : public CloneableKeyword<BoundaryCoupled, BoundaryKeyword> {
public:
    struct Data {
        int ssid = 0;           // Segment set ID
        int psid = 0;           // Part set ID
        int opt = 0;            // Option
        double sf = 1.0;        // Scale factor
    };

    BoundaryCoupled() = default;

    std::string getKeywordName() const override { return "*BOUNDARY_COUPLED"; }

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
 * @brief *BOUNDARY_PRESCRIBED_MOTION_NODE_ID
 *
 * Prescribed motion on node with ID.
 */
class KOO_API BoundaryPrescribedMotionNodeId : public CloneableKeyword<BoundaryPrescribedMotionNodeId, BoundaryKeyword> {
public:
    struct Data {
        int id = 0;             // ID
        std::string heading;    // Heading
        NodeId nid = 0;         // Node ID
        int dof = 0;            // Degree of freedom
        int vad = 0;            // Velocity/acceleration/displacement
        int lcid = 0;           // Load curve ID
        double sf = 1.0;        // Scale factor
        int vid = 0;            // Vector ID
        double death = 1e28;    // Death time
        double birth = 0.0;     // Birth time
    };

    BoundaryPrescribedMotionNodeId() = default;

    std::string getKeywordName() const override { return "*BOUNDARY_PRESCRIBED_MOTION_NODE_ID"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    std::vector<Data>& getMotions() { return motions_; }
    const std::vector<Data>& getMotions() const { return motions_; }

private:
    std::vector<Data> motions_;
};

/**
 * @brief *BOUNDARY_PRESCRIBED_MOTION_SET_LINE
 *
 * Prescribed motion on set along a line.
 */
class KOO_API BoundaryPrescribedMotionSetLine : public CloneableKeyword<BoundaryPrescribedMotionSetLine, BoundaryKeyword> {
public:
    struct Data {
        int nsid = 0;           // Node set ID
        int dof = 0;            // Degree of freedom
        int vad = 0;            // Velocity/acceleration/displacement
        int lcid = 0;           // Load curve ID
        double sf = 1.0;        // Scale factor
        int vid = 0;            // Vector ID
        double death = 1e28;    // Death time
        double birth = 0.0;     // Birth time
        double x1 = 0.0;        // X of first point
        double y1 = 0.0;        // Y of first point
        double z1 = 0.0;        // Z of first point
        double x2 = 0.0;        // X of second point
        double y2 = 0.0;        // Y of second point
        double z2 = 0.0;        // Z of second point
    };

    BoundaryPrescribedMotionSetLine() = default;

    std::string getKeywordName() const override { return "*BOUNDARY_PRESCRIBED_MOTION_SET_LINE"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    std::vector<Data>& getMotions() { return motions_; }
    const std::vector<Data>& getMotions() const { return motions_; }

private:
    std::vector<Data> motions_;
};

} // namespace koo::dyna
