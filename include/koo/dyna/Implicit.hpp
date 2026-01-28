#pragma once

#include <koo/Export.hpp>
#include <koo/dyna/Keyword.hpp>
#include <koo/util/Types.hpp>
#include <vector>
#include <string>

namespace koo::dyna {

/**
 * @brief Base class for *CONTROL_IMPLICIT keywords
 */
class KOO_API ImplicitKeyword : public Keyword {
public:
    virtual ~ImplicitKeyword() = default;
};

/**
 * @brief *CONTROL_IMPLICIT_GENERAL
 *
 * General implicit solver control.
 */
class KOO_API ControlImplicitGeneral : public CloneableKeyword<ControlImplicitGeneral, ImplicitKeyword> {
public:
    struct Data {
        int imflag = 0;       // Implicit flag
        double dt0 = 0.0;     // Initial time step
        int imform = 0;       // Implicit formulation
        int nsbs = 1;         // Number of sub-steps
        int igs = 0;          // Geometric stiffness
        int cnstn = 0;        // Constraint handling
        int form = 0;         // Mass matrix formulation
        int zero_v = 0;       // Zero velocity
    };

    ControlImplicitGeneral() = default;

    std::string getKeywordName() const override { return "*CONTROL_IMPLICIT_GENERAL"; }

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
 * @brief *CONTROL_IMPLICIT_AUTO
 *
 * Automatic time step control for implicit.
 */
class KOO_API ControlImplicitAuto : public CloneableKeyword<ControlImplicitAuto, ImplicitKeyword> {
public:
    struct Data {
        int iatefr = 0;       // Auto time step flag
        int tefr = 0;         // Time integration type
        double dtmin = 0.0;   // Minimum time step
        double dtmax = 0.0;   // Maximum time step
        double dtexp = 0.0;   // Desired expansion
        double kfail = 0.0;   // Fail reduction
        int kcycle = 0;       // Reform iteration
    };

    ControlImplicitAuto() = default;

    std::string getKeywordName() const override { return "*CONTROL_IMPLICIT_AUTO"; }

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
 * @brief *CONTROL_IMPLICIT_SOLUTION
 *
 * Implicit solution method control.
 */
class KOO_API ControlImplicitSolution : public CloneableKeyword<ControlImplicitSolution, ImplicitKeyword> {
public:
    struct Data {
        int nsolvr = 0;       // Nonlinear solver
        int ilimit = 0;       // Iteration limit
        double maxref = 0.0;  // Max reformations
        double dtefr = 0.0;   // Displacement tol
        double etefr = 0.0;   // Energy tolerance
        double rtefr = 0.0;   // Residual tolerance
        double lstol = 0.0;   // Line search tolerance
        double abstol = 0.0;  // Absolute tolerance
    };

    ControlImplicitSolution() = default;

    std::string getKeywordName() const override { return "*CONTROL_IMPLICIT_SOLUTION"; }

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
 * @brief *CONTROL_IMPLICIT_SOLVER
 *
 * Linear equation solver control.
 */
class KOO_API ControlImplicitSolver : public CloneableKeyword<ControlImplicitSolver, ImplicitKeyword> {
public:
    struct Data {
        int lsolvr = 0;       // Linear solver
        int lprint = 0;       // Print level
        int negev = 0;        // Negative eigenvalue
        int order = 0;        // Ordering method
        double dtefr = 0.0;   // Diagonal tolerance
        int msglv = 0;        // Message level
    };

    ControlImplicitSolver() = default;

    std::string getKeywordName() const override { return "*CONTROL_IMPLICIT_SOLVER"; }

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
 * @brief *CONTROL_IMPLICIT_DYNAMICS
 *
 * Implicit dynamics control.
 */
class KOO_API ControlImplicitDynamics : public CloneableKeyword<ControlImplicitDynamics, ImplicitKeyword> {
public:
    struct Data {
        int imass = 0;        // Mass matrix type
        double gamma = 0.5;   // Newmark gamma
        double beta = 0.25;   // Newmark beta
        int tdybir = 0;       // Dynamic birth
        int tdydth = 0;       // Dynamic death
        double tdyburt = 0.0; // Birth time
        double tdydtht = 0.0; // Death time
    };

    ControlImplicitDynamics() = default;

    std::string getKeywordName() const override { return "*CONTROL_IMPLICIT_DYNAMICS"; }

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
 * @brief *CONTROL_IMPLICIT_EIGENVALUE
 *
 * Eigenvalue analysis control.
 */
class KOO_API ControlImplicitEigenvalue : public CloneableKeyword<ControlImplicitEigenvalue, ImplicitKeyword> {
public:
    struct Data {
        int neig = 0;         // Number of eigenvalues
        int center = 0;       // Shift center
        int lflag = 0;        // Load flag
        int lftend = 0;       // Load at end time
        double shfscl = 0.0;  // Shift scale
        int mtefr = 0;        // Method flag
        int mastset = 0;      // Master set
        int mshflag = 0;      // Mass shift flag
    };

    ControlImplicitEigenvalue() = default;

    std::string getKeywordName() const override { return "*CONTROL_IMPLICIT_EIGENVALUE"; }

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
 * @brief *CONTROL_IMPLICIT_BUCKLE
 *
 * Buckling analysis control.
 */
class KOO_API ControlImplicitBuckle : public CloneableKeyword<ControlImplicitBuckle, ImplicitKeyword> {
public:
    struct Data {
        int nmode = 0;        // Number of modes
        int tefr = 0;         // Method flag
        double eitefr = 0.0;  // Eigenvalue tolerance
        int mshflag = 0;      // Mass shift
        double sfact = 1.0;   // Scale factor
    };

    ControlImplicitBuckle() = default;

    std::string getKeywordName() const override { return "*CONTROL_IMPLICIT_BUCKLE"; }

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
 * @brief *CONTROL_IMPLICIT_STABILIZATION
 *
 * Implicit stabilization control.
 */
class KOO_API ControlImplicitStabilization : public CloneableKeyword<ControlImplicitStabilization, ImplicitKeyword> {
public:
    struct Data {
        int ias = 0;          // Stabilization type
        double scale = 0.0;   // Scale factor
        double tstart = 0.0;  // Start time
        double tend = 0.0;    // End time
        int form = 0;         // Formulation
    };

    ControlImplicitStabilization() = default;

    std::string getKeywordName() const override { return "*CONTROL_IMPLICIT_STABILIZATION"; }

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
 * @brief *CONTROL_IMPLICIT_MODAL_DYNAMIC
 *
 * Modal dynamics control.
 */
class KOO_API ControlImplicitModalDynamic : public CloneableKeyword<ControlImplicitModalDynamic, ImplicitKeyword> {
public:
    struct Data {
        int neig = 0;         // Number of modes
        int mdmin = 0;        // Minimum mode
        int mdmax = 0;        // Maximum mode
        double dampf = 0.0;   // Damping factor
        int lcdam = 0;        // Load curve for damping
    };

    ControlImplicitModalDynamic() = default;

    std::string getKeywordName() const override { return "*CONTROL_IMPLICIT_MODAL_DYNAMIC"; }

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
