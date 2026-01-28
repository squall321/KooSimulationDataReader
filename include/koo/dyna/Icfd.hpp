#pragma once

#include <koo/Export.hpp>
#include <koo/dyna/Keyword.hpp>
#include <koo/util/Types.hpp>
#include <vector>
#include <string>

namespace koo::dyna {

/**
 * @brief Base class for *ICFD keywords
 */
class KOO_API IcfdKeyword : public Keyword {
public:
    virtual ~IcfdKeyword() = default;
};

/**
 * @brief *ICFD_CONTROL_TIME
 *
 * ICFD time control parameters.
 */
class KOO_API IcfdControlTime : public CloneableKeyword<IcfdControlTime, IcfdKeyword> {
public:
    struct Data {
        double ttm = 0.0;     // Termination time
        double dt = 0.0;      // Time step
        double cfl = 0.0;     // CFL number
        int lcid = 0;         // Load curve for time step
    };

    IcfdControlTime() = default;

    std::string getKeywordName() const override { return "*ICFD_CONTROL_TIME"; }

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
 * @brief *ICFD_CONTROL_GENERAL
 *
 * General ICFD control parameters.
 */
class KOO_API IcfdControlGeneral : public CloneableKeyword<IcfdControlGeneral, IcfdKeyword> {
public:
    struct Data {
        int apts = 0;         // Auto time step flag
        int nptm = 0;         // Max nonlinear iterations
        double dtol = 0.0;    // Divergence tolerance
        double ctol = 0.0;    // Convergence tolerance
        int meth = 0;         // Solution method
    };

    IcfdControlGeneral() = default;

    std::string getKeywordName() const override { return "*ICFD_CONTROL_GENERAL"; }

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
 * @brief *ICFD_PART
 *
 * ICFD part definition.
 */
class KOO_API IcfdPart : public CloneableKeyword<IcfdPart, IcfdKeyword> {
public:
    struct Data {
        int pid = 0;          // Part ID
        int secid = 0;        // Section ID
        int mid = 0;          // Material ID
    };

    IcfdPart() = default;

    std::string getKeywordName() const override { return "*ICFD_PART"; }

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
 * @brief *ICFD_MAT
 *
 * ICFD material definition.
 */
class KOO_API IcfdMat : public CloneableKeyword<IcfdMat, IcfdKeyword> {
public:
    struct Data {
        int mid = 0;          // Material ID
        double rho = 0.0;     // Density
        double mu = 0.0;      // Dynamic viscosity
        double hcf = 0.0;     // Heat capacity factor
        double tcf = 0.0;     // Thermal conductivity
    };

    IcfdMat() = default;

    std::string getKeywordName() const override { return "*ICFD_MAT"; }

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
 * @brief *ICFD_BOUNDARY_PRESCRIBED_VEL
 *
 * ICFD prescribed velocity boundary condition.
 */
class KOO_API IcfdBoundaryPrescribedVel : public CloneableKeyword<IcfdBoundaryPrescribedVel, IcfdKeyword> {
public:
    struct Data {
        int pid = 0;          // Part ID
        int dof = 0;          // Degree of freedom
        double vad = 0.0;     // Value
        int lcid = 0;         // Load curve ID
    };

    IcfdBoundaryPrescribedVel() = default;

    std::string getKeywordName() const override { return "*ICFD_BOUNDARY_PRESCRIBED_VEL"; }

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
 * @brief *ICFD_BOUNDARY_PRESCRIBED_PRE
 *
 * ICFD prescribed pressure boundary condition.
 */
class KOO_API IcfdBoundaryPrescribedPre : public CloneableKeyword<IcfdBoundaryPrescribedPre, IcfdKeyword> {
public:
    struct Data {
        int pid = 0;          // Part ID
        double pval = 0.0;    // Pressure value
        int lcid = 0;         // Load curve ID
        int death = 0;        // Death time
    };

    IcfdBoundaryPrescribedPre() = default;

    std::string getKeywordName() const override { return "*ICFD_BOUNDARY_PRESCRIBED_PRE"; }

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
 * @brief *ICFD_CONTROL_OUTPUT
 *
 * ICFD output control parameters.
 */
class KOO_API IcfdControlOutput : public CloneableKeyword<IcfdControlOutput, IcfdKeyword> {
public:
    struct Data {
        int mession = 0;      // Output mesh flag
        int velocity = 0;     // Velocity output
        int pressure = 0;     // Pressure output
        int tempout = 0;      // Temperature output
        int drag = 0;         // Drag output
    };

    IcfdControlOutput() = default;

    std::string getKeywordName() const override { return "*ICFD_CONTROL_OUTPUT"; }

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
 * @brief *ICFD_CONTROL_MESH
 *
 * ICFD mesh control parameters.
 */
class KOO_API IcfdControlMesh : public CloneableKeyword<IcfdControlMesh, IcfdKeyword> {
public:
    struct Data {
        int mshtyp = 0;       // Mesh type
        double mshsiz = 0.0;  // Mesh size
        int nelem = 0;        // Number of elements
        int adapt = 0;        // Adaptive mesh flag
    };

    IcfdControlMesh() = default;

    std::string getKeywordName() const override { return "*ICFD_CONTROL_MESH"; }

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
 * @brief *ICFD_CONTROL_TURBULENCE
 *
 * ICFD turbulence control parameters.
 */
class KOO_API IcfdControlTurbulence : public CloneableKeyword<IcfdControlTurbulence, IcfdKeyword> {
public:
    struct Data {
        int tmod = 0;         // Turbulence model
        double cmu = 0.0;     // Cmu constant
        double tke = 0.0;     // Turbulent kinetic energy
        double eps = 0.0;     // Dissipation rate
    };

    IcfdControlTurbulence() = default;

    std::string getKeywordName() const override { return "*ICFD_CONTROL_TURBULENCE"; }

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
 * @brief *ICFD_CONTROL_FSI
 *
 * ICFD Fluid-Structure Interaction control parameters.
 */
class KOO_API IcfdControlFsi : public CloneableKeyword<IcfdControlFsi, IcfdKeyword> {
public:
    struct Data {
        int owc = 0;          // One-way coupling flag
        int bt = 0;           // Boundary treatment
        int dt = 0;           // Time step control
        int pession = 0;      // Pressure coupling
        double frcfrq = 0.0;  // Force update frequency
    };

    IcfdControlFsi() = default;

    std::string getKeywordName() const override { return "*ICFD_CONTROL_FSI"; }

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
 * @brief *ICFD_INITIAL
 *
 * ICFD initial conditions.
 */
class KOO_API IcfdInitial : public CloneableKeyword<IcfdInitial, IcfdKeyword> {
public:
    struct Data {
        int pid = 0;          // Part ID
        double vx = 0.0;      // Initial velocity X
        double vy = 0.0;      // Initial velocity Y
        double vz = 0.0;      // Initial velocity Z
        double pre = 0.0;     // Initial pressure
        double temp = 0.0;    // Initial temperature
    };

    IcfdInitial() = default;

    std::string getKeywordName() const override { return "*ICFD_INITIAL"; }

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
 * @brief *ICFD_DEFINE_MESH
 *
 * Defines mesh parameters for ICFD solver.
 */
class KOO_API IcfdDefineMesh : public CloneableKeyword<IcfdDefineMesh, IcfdKeyword> {
public:
    struct Data {
        int pid = 0;            // Part ID
        int mtype = 0;          // Mesh type
        double esize = 0.0;     // Element size
        int nelem = 0;          // Number of elements
        double bl = 0.0;        // Boundary layer thickness
        int nbl = 0;            // Number of boundary layers
        double blr = 1.2;       // Boundary layer ratio
        int remesh = 0;         // Remesh flag
    };

    IcfdDefineMesh() = default;

    std::string getKeywordName() const override { return "*ICFD_DEFINE_MESH"; }

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
