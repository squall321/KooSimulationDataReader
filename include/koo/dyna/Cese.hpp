#pragma once

#include <koo/Export.hpp>
#include <koo/dyna/Keyword.hpp>
#include <koo/util/Types.hpp>
#include <vector>
#include <string>

namespace koo::dyna {

/**
 * @brief Base class for *CESE keywords
 */
class KOO_API CeseKeyword : public Keyword {
public:
    virtual ~CeseKeyword() = default;
};

/**
 * @brief *CESE_CONTROL_TIMESTEP
 *
 * CESE time step control.
 */
class KOO_API CeseControlTimestep : public CloneableKeyword<CeseControlTimestep, CeseKeyword> {
public:
    struct Data {
        int iddt = 0;         // Time step control type
        double cfl = 0.9;     // CFL number
        double dt_init = 0.0; // Initial time step
        double dt_min = 0.0;  // Minimum time step
        double dt_max = 0.0;  // Maximum time step
    };

    CeseControlTimestep() = default;

    std::string getKeywordName() const override { return "*CESE_CONTROL_TIMESTEP"; }

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
 * @brief *CESE_CONTROL_SOLVER
 *
 * CESE solver parameters.
 */
class KOO_API CeseControlSolver : public CloneableKeyword<CeseControlSolver, CeseKeyword> {
public:
    struct Data {
        int idgas = 0;        // Gas model type
        double gamma = 1.4;   // Ratio of specific heats
        double pref = 0.0;    // Reference pressure
        double rhoref = 0.0;  // Reference density
        double tref = 0.0;    // Reference temperature
    };

    CeseControlSolver() = default;

    std::string getKeywordName() const override { return "*CESE_CONTROL_SOLVER"; }

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
 * @brief *CESE_BOUNDARY_PRESCRIBED
 *
 * CESE prescribed boundary condition.
 */
class KOO_API CeseBoundaryPrescribed : public CloneableKeyword<CeseBoundaryPrescribed, CeseKeyword> {
public:
    struct Data {
        int ssid = 0;         // Segment set ID
        int idcomp = 0;       // Component (1=u, 2=v, 3=w, 4=p, 5=rho, 6=T)
        double val = 0.0;     // Value
        int lcid = 0;         // Load curve ID
    };

    CeseBoundaryPrescribed() = default;

    std::string getKeywordName() const override { return "*CESE_BOUNDARY_PRESCRIBED"; }

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
 * @brief *CESE_INITIAL
 *
 * CESE initial conditions.
 */
class KOO_API CeseInitial : public CloneableKeyword<CeseInitial, CeseKeyword> {
public:
    struct Data {
        int pid = 0;          // Part ID (0=all)
        double u = 0.0;       // X-velocity
        double v = 0.0;       // Y-velocity
        double w = 0.0;       // Z-velocity
        double rho = 0.0;     // Density
        double p = 0.0;       // Pressure
        double t = 0.0;       // Temperature
    };

    CeseInitial() = default;

    std::string getKeywordName() const override { return "*CESE_INITIAL"; }

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
 * @brief *CESE_BOUNDARY_REFLECTIVE
 *
 * CESE reflective boundary condition.
 */
class KOO_API CeseBoundaryReflective : public CloneableKeyword<CeseBoundaryReflective, CeseKeyword> {
public:
    struct Data {
        int ssid = 0;         // Segment set ID
        int rtype = 0;        // Reflection type
    };

    CeseBoundaryReflective() = default;

    std::string getKeywordName() const override { return "*CESE_BOUNDARY_REFLECTIVE"; }

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
 * @brief *CESE_BOUNDARY_NON_REFLECTIVE
 *
 * CESE non-reflective boundary condition.
 */
class KOO_API CeseBoundaryNonReflective : public CloneableKeyword<CeseBoundaryNonReflective, CeseKeyword> {
public:
    struct Data {
        int ssid = 0;         // Segment set ID
        double pback = 0.0;   // Back pressure
        int lcid = 0;         // Load curve ID
    };

    CeseBoundaryNonReflective() = default;

    std::string getKeywordName() const override { return "*CESE_BOUNDARY_NON_REFLECTIVE"; }

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
 * @brief *CESE_PART
 *
 * CESE part definition.
 */
class KOO_API CesePart : public CloneableKeyword<CesePart, CeseKeyword> {
public:
    struct Data {
        int pid = 0;          // Part ID
        int mid = 0;          // Material ID
        int eosid = 0;        // EOS ID
    };

    CesePart() = default;

    std::string getKeywordName() const override { return "*CESE_PART"; }

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
 * @brief *CESE_CONTROL_OUTPUT
 *
 * CESE output control.
 */
class KOO_API CeseControlOutput : public CloneableKeyword<CeseControlOutput, CeseKeyword> {
public:
    struct Data {
        int density = 0;      // Output density
        int velocity = 0;     // Output velocity
        int pressure = 0;     // Output pressure
        int temperature = 0;  // Output temperature
        int mach = 0;         // Output Mach number
    };

    CeseControlOutput() = default;

    std::string getKeywordName() const override { return "*CESE_CONTROL_OUTPUT"; }

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
 * @brief *CESE_CONTROL_MESH
 *
 * CESE mesh control parameters.
 */
class KOO_API CeseControlMesh : public CloneableKeyword<CeseControlMesh, CeseKeyword> {
public:
    struct Data {
        int iauto = 0;        // Auto mesh flag
        double mshsiz = 0.0;  // Mesh size
        int nelem = 0;        // Number of elements
        int adapt = 0;        // Adaptive mesh flag
        double refine = 0.0;  // Refinement ratio
    };

    CeseControlMesh() = default;

    std::string getKeywordName() const override { return "*CESE_CONTROL_MESH"; }

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
 * @brief *CESE_MAT
 *
 * CESE material definition.
 */
class KOO_API CeseMat : public CloneableKeyword<CeseMat, CeseKeyword> {
public:
    struct Data {
        int mid = 0;          // Material ID
        double rho = 0.0;     // Density
        double gamma = 1.4;   // Ratio of specific heats
        double cv = 0.0;      // Specific heat at constant volume
        double mu = 0.0;      // Dynamic viscosity
        double kappa = 0.0;   // Thermal conductivity
    };

    CeseMat() = default;

    std::string getKeywordName() const override { return "*CESE_MAT"; }

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
