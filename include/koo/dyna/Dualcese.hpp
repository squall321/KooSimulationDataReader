#pragma once

#include <koo/Export.hpp>
#include <koo/dyna/Keyword.hpp>
#include <koo/util/Types.hpp>
#include <vector>
#include <string>

namespace koo::dyna {

/**
 * @brief Base class for *DUALCESE keywords
 */
class KOO_API DualceseKeyword : public Keyword {
public:
    virtual ~DualceseKeyword() = default;
};

/**
 * @brief *DUALCESE_CONTROL_TIMESTEP
 *
 * Dual CESE time step control.
 */
class KOO_API DualceseControlTimestep : public CloneableKeyword<DualceseControlTimestep, DualceseKeyword> {
public:
    struct Data {
        int iddt = 0;         // Time step control
        double cfl = 0.9;     // CFL number
        double dtinit = 0.0;  // Initial time step
        double dtmin = 0.0;   // Minimum time step
        double dtmax = 0.0;   // Maximum time step
    };

    DualceseControlTimestep() = default;

    std::string getKeywordName() const override { return "*DUALCESE_CONTROL_TIMESTEP"; }

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
 * @brief *DUALCESE_CONTROL_GENERAL
 *
 * Dual CESE general control parameters.
 */
class KOO_API DualceseControlGeneral : public CloneableKeyword<DualceseControlGeneral, DualceseKeyword> {
public:
    struct Data {
        int idmeth = 0;       // Method type
        int imesh = 0;        // Mesh type
        int ibc = 0;          // Boundary type
        int ichem = 0;        // Chemistry flag
        int iturb = 0;        // Turbulence flag
    };

    DualceseControlGeneral() = default;

    std::string getKeywordName() const override { return "*DUALCESE_CONTROL_GENERAL"; }

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
 * @brief *DUALCESE_BOUNDARY_PRESCRIBED_VEL
 *
 * Dual CESE prescribed velocity boundary.
 */
class KOO_API DualceseBoundaryPrescribedVel : public CloneableKeyword<DualceseBoundaryPrescribedVel, DualceseKeyword> {
public:
    struct Data {
        int ssid = 0;         // Segment set ID
        int dof = 0;          // Degree of freedom
        double vel = 0.0;     // Velocity
        int lcid = 0;         // Load curve ID
    };

    DualceseBoundaryPrescribedVel() = default;

    std::string getKeywordName() const override { return "*DUALCESE_BOUNDARY_PRESCRIBED_VEL"; }

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
 * @brief *DUALCESE_BOUNDARY_PRESCRIBED_PRE
 *
 * Dual CESE prescribed pressure boundary.
 */
class KOO_API DualceseBoundaryPrescribedPre : public CloneableKeyword<DualceseBoundaryPrescribedPre, DualceseKeyword> {
public:
    struct Data {
        int ssid = 0;         // Segment set ID
        double pres = 0.0;    // Pressure value
        int lcid = 0;         // Load curve ID
    };

    DualceseBoundaryPrescribedPre() = default;

    std::string getKeywordName() const override { return "*DUALCESE_BOUNDARY_PRESCRIBED_PRE"; }

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
 * @brief *DUALCESE_INITIAL
 *
 * Dual CESE initial conditions.
 */
class KOO_API DualceseInitial : public CloneableKeyword<DualceseInitial, DualceseKeyword> {
public:
    struct Data {
        int pid = 0;          // Part ID
        double rho = 0.0;     // Density
        double u = 0.0;       // X-velocity
        double v = 0.0;       // Y-velocity
        double w = 0.0;       // Z-velocity
        double p = 0.0;       // Pressure
    };

    DualceseInitial() = default;

    std::string getKeywordName() const override { return "*DUALCESE_INITIAL"; }

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
 * @brief *DUALCESE_PART
 *
 * Dual CESE part definition.
 */
class KOO_API DualcesePart : public CloneableKeyword<DualcesePart, DualceseKeyword> {
public:
    struct Data {
        int pid = 0;          // Part ID
        int mid = 0;          // Material ID
        int eosid = 0;        // EOS ID
    };

    DualcesePart() = default;

    std::string getKeywordName() const override { return "*DUALCESE_PART"; }

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
 * @brief *DUALCESE_CONTROL_SOLVER
 *
 * Dual CESE solver control.
 */
class KOO_API DualceseControlSolver : public CloneableKeyword<DualceseControlSolver, DualceseKeyword> {
public:
    struct Data {
        int isolver = 0;      // Solver type
        double gamma = 1.4;   // Ratio of specific heats
        double pref = 0.0;    // Reference pressure
        double rhoref = 0.0;  // Reference density
    };

    DualceseControlSolver() = default;

    std::string getKeywordName() const override { return "*DUALCESE_CONTROL_SOLVER"; }

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
 * @brief *DUALCESE_BOUNDARY_REFLECTIVE
 *
 * Dual CESE reflective boundary condition.
 */
class KOO_API DualceseBoundaryReflective : public CloneableKeyword<DualceseBoundaryReflective, DualceseKeyword> {
public:
    struct Data {
        int ssid = 0;         // Segment set ID
        int rtype = 0;        // Reflection type
        double coef = 1.0;    // Reflection coefficient
    };

    DualceseBoundaryReflective() = default;

    std::string getKeywordName() const override { return "*DUALCESE_BOUNDARY_REFLECTIVE"; }

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
 * @brief *DUALCESE_MAT
 *
 * Dual CESE material definition.
 */
class KOO_API DualceseMat : public CloneableKeyword<DualceseMat, DualceseKeyword> {
public:
    struct Data {
        int mid = 0;          // Material ID
        double rho = 0.0;     // Density
        double mu = 0.0;      // Dynamic viscosity
        double pr = 0.0;      // Prandtl number
        double cv = 0.0;      // Specific heat at constant volume
    };

    DualceseMat() = default;

    std::string getKeywordName() const override { return "*DUALCESE_MAT"; }

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
