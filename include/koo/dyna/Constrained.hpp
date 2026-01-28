#pragma once

#include <koo/Export.hpp>
#include <koo/dyna/Keyword.hpp>
#include <koo/util/Types.hpp>
#include <vector>

namespace koo::dyna {

/**
 * @brief Base class for *CONSTRAINED keywords
 */
class KOO_API ConstrainedKeyword : public Keyword {
public:
    virtual ~ConstrainedKeyword() = default;
};

/**
 * @brief *CONSTRAINED_NODAL_RIGID_BODY
 *
 * Defines a nodal rigid body - a set of nodes constrained to move as a rigid body.
 */
class KOO_API ConstrainedNodalRigidBody : public CloneableKeyword<ConstrainedNodalRigidBody, ConstrainedKeyword> {
public:
    struct Data {
        int pid = 0;            // Part ID for rigid body
        int cid = 0;            // Coordinate system ID
        int nsid = 0;           // Node set ID
        int pnode = 0;          // Principal node
        int iprt = 0;           // Print flag
        int drflag = 0;         // Dynamic relaxation flag
        int rrflag = 0;         // Rigid rotation flag
    };

    ConstrainedNodalRigidBody() = default;

    std::string getKeywordName() const override { return "*CONSTRAINED_NODAL_RIGID_BODY"; }

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
 * @brief *CONSTRAINED_EXTRA_NODES_NODE
 *
 * Adds extra nodes to a rigid body.
 */
class KOO_API ConstrainedExtraNodesNode : public CloneableKeyword<ConstrainedExtraNodesNode, ConstrainedKeyword> {
public:
    struct Entry {
        PartId pid = 0;         // Part ID of rigid body
        NodeId nid = 0;         // Node ID to add
    };

    ConstrainedExtraNodesNode() = default;

    std::string getKeywordName() const override { return "*CONSTRAINED_EXTRA_NODES_NODE"; }

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
 * @brief *CONSTRAINED_EXTRA_NODES_SET
 *
 * Adds extra nodes (by set) to a rigid body.
 */
class KOO_API ConstrainedExtraNodesSet : public CloneableKeyword<ConstrainedExtraNodesSet, ConstrainedKeyword> {
public:
    struct Entry {
        PartId pid = 0;         // Part ID of rigid body
        int nsid = 0;           // Node set ID to add
    };

    ConstrainedExtraNodesSet() = default;

    std::string getKeywordName() const override { return "*CONSTRAINED_EXTRA_NODES_SET"; }

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
 * @brief *CONSTRAINED_RIGID_BODIES
 *
 * Merges two rigid bodies.
 */
class KOO_API ConstrainedRigidBodies : public CloneableKeyword<ConstrainedRigidBodies, ConstrainedKeyword> {
public:
    struct Entry {
        PartId pidm = 0;        // Master rigid body part ID
        PartId pids = 0;        // Slave rigid body part ID
    };

    ConstrainedRigidBodies() = default;

    std::string getKeywordName() const override { return "*CONSTRAINED_RIGID_BODIES"; }

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
 * @brief *CONSTRAINED_JOINT_SPHERICAL
 *
 * Spherical joint constraint.
 */
class KOO_API ConstrainedJointSpherical : public CloneableKeyword<ConstrainedJointSpherical, ConstrainedKeyword> {
public:
    struct Data {
        int jid = 0;            // Joint ID
        PartId pidb = 0;        // Part ID of body B
        PartId pida = 0;        // Part ID of body A
        NodeId n1 = 0;          // Node 1
        NodeId n2 = 0;          // Node 2
        NodeId n3 = 0;          // Node 3
        NodeId n4 = 0;          // Node 4
        NodeId n5 = 0;          // Node 5
        NodeId n6 = 0;          // Node 6
    };

    ConstrainedJointSpherical() = default;

    std::string getKeywordName() const override { return "*CONSTRAINED_JOINT_SPHERICAL"; }

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
 * @brief *CONSTRAINED_JOINT_REVOLUTE
 *
 * Revolute joint constraint.
 */
class KOO_API ConstrainedJointRevolute : public CloneableKeyword<ConstrainedJointRevolute, ConstrainedKeyword> {
public:
    struct Data {
        int jid = 0;
        PartId pidb = 0;
        PartId pida = 0;
        NodeId n1 = 0;
        NodeId n2 = 0;
        NodeId n3 = 0;
        NodeId n4 = 0;
        NodeId n5 = 0;
        NodeId n6 = 0;
    };

    ConstrainedJointRevolute() = default;

    std::string getKeywordName() const override { return "*CONSTRAINED_JOINT_REVOLUTE"; }

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
 * @brief *CONSTRAINED_JOINT_CYLINDRICAL
 *
 * Cylindrical joint constraint.
 */
class KOO_API ConstrainedJointCylindrical : public CloneableKeyword<ConstrainedJointCylindrical, ConstrainedKeyword> {
public:
    struct Data {
        int jid = 0;
        PartId pidb = 0;
        PartId pida = 0;
        NodeId n1 = 0;
        NodeId n2 = 0;
        NodeId n3 = 0;
        NodeId n4 = 0;
        NodeId n5 = 0;
        NodeId n6 = 0;
    };

    ConstrainedJointCylindrical() = default;

    std::string getKeywordName() const override { return "*CONSTRAINED_JOINT_CYLINDRICAL"; }

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
 * @brief *CONSTRAINED_SPOTWELD
 *
 * Spotweld constraint.
 */
class KOO_API ConstrainedSpotweld : public CloneableKeyword<ConstrainedSpotweld, ConstrainedKeyword> {
public:
    struct Entry {
        NodeId n1 = 0;          // Node 1
        NodeId n2 = 0;          // Node 2
        double sn = 0.0;        // Normal failure force
        double ss = 0.0;        // Shear failure force
        double n = 0.0;         // Exponent for failure criterion
        double m = 0.0;         // Exponent for failure criterion
        double tf = 0.0;        // Failure time
    };

    ConstrainedSpotweld() = default;

    std::string getKeywordName() const override { return "*CONSTRAINED_SPOTWELD"; }

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
 * @brief *CONSTRAINED_LINEAR
 *
 * Linear constraint equations.
 */
class KOO_API ConstrainedLinear : public CloneableKeyword<ConstrainedLinear, ConstrainedKeyword> {
public:
    struct Term {
        NodeId nid = 0;         // Node ID
        int dof = 0;            // Degree of freedom
        double coef = 0.0;      // Coefficient
    };

    struct Constraint {
        std::vector<Term> terms;
    };

    ConstrainedLinear() = default;

    std::string getKeywordName() const override { return "*CONSTRAINED_LINEAR"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    std::vector<Constraint>& getConstraints() { return constraints_; }
    const std::vector<Constraint>& getConstraints() const { return constraints_; }

private:
    std::vector<Constraint> constraints_;
};

/**
 * @brief *CONSTRAINED_JOINT_TRANSLATIONAL
 *
 * Translational (prismatic) joint constraint.
 */
class KOO_API ConstrainedJointTranslational : public CloneableKeyword<ConstrainedJointTranslational, ConstrainedKeyword> {
public:
    struct Data {
        int jid = 0;
        PartId pidb = 0;
        PartId pida = 0;
        NodeId n1 = 0;
        NodeId n2 = 0;
        NodeId n3 = 0;
        NodeId n4 = 0;
        NodeId n5 = 0;
        NodeId n6 = 0;
    };

    ConstrainedJointTranslational() = default;

    std::string getKeywordName() const override { return "*CONSTRAINED_JOINT_TRANSLATIONAL"; }

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
 * @brief *CONSTRAINED_JOINT_UNIVERSAL
 *
 * Universal joint constraint.
 */
class KOO_API ConstrainedJointUniversal : public CloneableKeyword<ConstrainedJointUniversal, ConstrainedKeyword> {
public:
    struct Data {
        int jid = 0;
        PartId pidb = 0;
        PartId pida = 0;
        NodeId n1 = 0;
        NodeId n2 = 0;
        NodeId n3 = 0;
        NodeId n4 = 0;
        NodeId n5 = 0;
        NodeId n6 = 0;
    };

    ConstrainedJointUniversal() = default;

    std::string getKeywordName() const override { return "*CONSTRAINED_JOINT_UNIVERSAL"; }

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
 * @brief *CONSTRAINED_JOINT_STIFFNESS
 *
 * Joint stiffness definition.
 */
class KOO_API ConstrainedJointStiffness : public CloneableKeyword<ConstrainedJointStiffness, ConstrainedKeyword> {
public:
    struct Data {
        int jsid = 0;           // Joint stiffness ID
        int lcidph = 0;         // Load curve for phi rotation
        int lcidth = 0;         // Load curve for theta rotation
        int lcidps = 0;         // Load curve for psi rotation
        int lcidtq = 0;         // Load curve for translational
        double dlcidph = 0.0;   // Damping for phi
        double dlcidth = 0.0;   // Damping for theta
        double dlcidps = 0.0;   // Damping for psi
        double dlcidtq = 0.0;   // Damping for translational
    };

    ConstrainedJointStiffness() = default;

    std::string getKeywordName() const override { return "*CONSTRAINED_JOINT_STIFFNESS"; }

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
 * @brief *CONSTRAINED_SHELL_TO_SOLID
 *
 * Shell to solid constraint.
 */
class KOO_API ConstrainedShellToSolid : public CloneableKeyword<ConstrainedShellToSolid, ConstrainedKeyword> {
public:
    struct Data {
        int sset = 0;           // Shell node set ID
        int mset = 0;           // Solid element set ID
        int ncoup = 0;          // Number of coupling points
    };

    ConstrainedShellToSolid() = default;

    std::string getKeywordName() const override { return "*CONSTRAINED_SHELL_TO_SOLID"; }

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
 * @brief *CONSTRAINED_GENERALIZED_WELD_NODE
 *
 * Generalized weld constraints on nodes.
 */
class KOO_API ConstrainedGeneralizedWeldNode : public CloneableKeyword<ConstrainedGeneralizedWeldNode, ConstrainedKeyword> {
public:
    struct Entry {
        NodeId n1 = 0;
        NodeId n2 = 0;
        NodeId n3 = 0;
        NodeId n4 = 0;
        NodeId n5 = 0;
        NodeId n6 = 0;
        NodeId n7 = 0;
        NodeId n8 = 0;
    };

    ConstrainedGeneralizedWeldNode() = default;

    std::string getKeywordName() const override { return "*CONSTRAINED_GENERALIZED_WELD_NODE"; }

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
 * @brief *CONSTRAINED_JOINT_PLANAR
 *
 * Planar joint constraint.
 */
class KOO_API ConstrainedJointPlanar : public CloneableKeyword<ConstrainedJointPlanar, ConstrainedKeyword> {
public:
    struct Data {
        int jid = 0;
        PartId pidb = 0;
        PartId pida = 0;
        NodeId n1 = 0;
        NodeId n2 = 0;
        NodeId n3 = 0;
        NodeId n4 = 0;
        NodeId n5 = 0;
        NodeId n6 = 0;
    };

    ConstrainedJointPlanar() = default;

    std::string getKeywordName() const override { return "*CONSTRAINED_JOINT_PLANAR"; }

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
 * @brief *CONSTRAINED_BEAM_IN_SOLID
 *
 * Beam in solid coupling constraint.
 */
class KOO_API ConstrainedBeamInSolid : public CloneableKeyword<ConstrainedBeamInSolid, ConstrainedKeyword> {
public:
    struct Data {
        int coupid = 0;       // Coupling ID
        int bsid = 0;         // Beam set ID
        int ssid = 0;         // Solid set ID
        int ncoup = 0;        // Number of coupling points
        int cdir = 0;         // Coupling direction
    };

    ConstrainedBeamInSolid() = default;

    std::string getKeywordName() const override { return "*CONSTRAINED_BEAM_IN_SOLID"; }

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
 * @brief *CONSTRAINED_LAGRANGE_IN_SOLID
 *
 * Lagrangian body in ALE/Eulerian solid constraint.
 */
class KOO_API ConstrainedLagrangeInSolid : public CloneableKeyword<ConstrainedLagrangeInSolid, ConstrainedKeyword> {
public:
    struct Data {
        int slave = 0;        // Slave part/set ID
        int master = 0;       // Master part/set ID
        int sstyp = 0;        // Slave set type
        int mstyp = 0;        // Master set type
        int nquad = 0;        // Quadrature points
        double ctype = 0.0;   // Coupling type
        int direc = 0;        // Coupling direction
        int mcoup = 0;        // Mass coupling
    };

    ConstrainedLagrangeInSolid() = default;

    std::string getKeywordName() const override { return "*CONSTRAINED_LAGRANGE_IN_SOLID"; }

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
 * @brief *CONSTRAINED_TIE-BREAK
 *
 * Tie-break constraint for contact surfaces.
 */
class KOO_API ConstrainedTieBreak : public CloneableKeyword<ConstrainedTieBreak, ConstrainedKeyword> {
public:
    struct Data {
        int ssid = 0;         // Slave set ID
        int msid = 0;         // Master set ID
        int sstyp = 0;        // Slave set type
        int mstyp = 0;        // Master set type
        double nfls = 0.0;    // Normal failure stress
        double sfls = 0.0;    // Shear failure stress
        double en = 0.0;      // Normal exponent
        double es = 0.0;      // Shear exponent
    };

    ConstrainedTieBreak() = default;

    std::string getKeywordName() const override { return "*CONSTRAINED_TIE-BREAK"; }

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
 * @brief *CONSTRAINED_RIGID_BODY_STOPPERS
 *
 * Rigid body motion stoppers (limits motion in certain directions).
 */
class KOO_API ConstrainedRigidBodyStoppers : public CloneableKeyword<ConstrainedRigidBodyStoppers, ConstrainedKeyword> {
public:
    struct Data {
        PartId pid = 0;       // Part ID
        int dir = 0;          // Direction (1=X, 2=Y, 3=Z, 4=RX, 5=RY, 6=RZ)
        double lcmin = 0.0;   // Minimum limit
        double lcmax = 0.0;   // Maximum limit
        int vid = 0;          // Vector ID
    };

    ConstrainedRigidBodyStoppers() = default;

    std::string getKeywordName() const override { return "*CONSTRAINED_RIGID_BODY_STOPPERS"; }

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
 * @brief *CONSTRAINED_NODE_SET
 *
 * Constrains a node set to move as a unit.
 */
class KOO_API ConstrainedNodeSet : public CloneableKeyword<ConstrainedNodeSet, ConstrainedKeyword> {
public:
    struct Data {
        int nsid = 0;         // Node set ID
        int dof = 0;          // Degree of freedom
        int cid = 0;          // Coordinate system ID
    };

    ConstrainedNodeSet() = default;

    std::string getKeywordName() const override { return "*CONSTRAINED_NODE_SET"; }

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
 * @brief *CONSTRAINED_INTERPOLATION
 *
 * Interpolation constraint connecting slave nodes to master nodes.
 */
class KOO_API ConstrainedInterpolation : public CloneableKeyword<ConstrainedInterpolation, ConstrainedKeyword> {
public:
    struct Data {
        int icid = 0;           // Constraint ID
        int dnid = 0;           // Dependent node ID
        int ddof = 0;           // Dependent degree of freedom
        int citeflag = 0;       // Independent term evaluation flag
        std::vector<std::pair<NodeId, double>> independentNodes; // (node ID, coefficient)
    };

    ConstrainedInterpolation() = default;

    std::string getKeywordName() const override { return "*CONSTRAINED_INTERPOLATION"; }

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
 * @brief *CONSTRAINED_GLOBAL
 *
 * Global constraint equations (linear constraints across the entire model).
 */
class KOO_API ConstrainedGlobal : public CloneableKeyword<ConstrainedGlobal, ConstrainedKeyword> {
public:
    struct Term {
        NodeId nid = 0;         // Node ID
        int dof = 0;            // Degree of freedom (1-6)
        double coef = 0.0;      // Coefficient
    };

    struct Equation {
        int eqid = 0;           // Equation ID
        double rhs = 0.0;       // Right-hand side value
        std::vector<Term> terms;
    };

    ConstrainedGlobal() = default;

    std::string getKeywordName() const override { return "*CONSTRAINED_GLOBAL"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    std::vector<Equation>& getEquations() { return equations_; }
    const std::vector<Equation>& getEquations() const { return equations_; }

private:
    std::vector<Equation> equations_;
};

} // namespace koo::dyna
