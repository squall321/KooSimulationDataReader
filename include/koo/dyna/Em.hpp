#pragma once

#include <koo/Export.hpp>
#include <koo/dyna/Keyword.hpp>
#include <koo/util/Types.hpp>
#include <vector>
#include <string>

namespace koo::dyna {

/**
 * @brief Base class for *EM keywords
 */
class KOO_API EmKeyword : public Keyword {
public:
    virtual ~EmKeyword() = default;
};

/**
 * @brief *EM_CONTROL
 *
 * Electromagnetic solver control parameters.
 */
class KOO_API EmControl : public CloneableKeyword<EmControl, EmKeyword> {
public:
    struct Data {
        int etefrmw = 0;      // EM-thermal coupling flag
        int mession = 0;      // Magnetic field session
        double emdt = 0.0;    // EM time step
        double emdt_min = 0.0;// Min EM time step
        double emdt_max = 0.0;// Max EM time step
        int ncyclem = 1;      // EM solver cycles
    };

    EmControl() = default;

    std::string getKeywordName() const override { return "*EM_CONTROL"; }

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
 * @brief *EM_MAT_001
 *
 * Electromagnetic material type 1 (conductor).
 */
class KOO_API EmMat001 : public CloneableKeyword<EmMat001, EmKeyword> {
public:
    struct Data {
        int mid = 0;          // Material ID
        double sigma = 0.0;   // Electrical conductivity
        double eosmu = 0.0;   // Magnetic permeability
        double eosep = 0.0;   // Electric permittivity
    };

    EmMat001() = default;

    std::string getKeywordName() const override { return "*EM_MAT_001"; }

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
 * @brief *EM_CIRCUIT
 *
 * Electromagnetic circuit definition.
 */
class KOO_API EmCircuit : public CloneableKeyword<EmCircuit, EmKeyword> {
public:
    struct Data {
        int circid = 0;       // Circuit ID
        int circtyp = 0;      // Circuit type
        int lcid = 0;         // Load curve ID
        double r = 0.0;       // Resistance
        double l = 0.0;       // Inductance
        double c = 0.0;       // Capacitance
        double v0 = 0.0;      // Initial voltage
        double i0 = 0.0;      // Initial current
    };

    EmCircuit() = default;

    std::string getKeywordName() const override { return "*EM_CIRCUIT"; }

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
 * @brief *EM_BOUNDARY
 *
 * Electromagnetic boundary conditions.
 */
class KOO_API EmBoundary : public CloneableKeyword<EmBoundary, EmKeyword> {
public:
    struct Data {
        int ssid = 0;         // Segment set ID
        int btype = 0;        // Boundary type
        double val = 0.0;     // Boundary value
        int lcid = 0;         // Load curve ID
    };

    EmBoundary() = default;

    std::string getKeywordName() const override { return "*EM_BOUNDARY"; }

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
 * @brief *EM_SOLVER_FEM
 *
 * FEM electromagnetic solver parameters.
 */
class KOO_API EmSolverFem : public CloneableKeyword<EmSolverFem, EmKeyword> {
public:
    struct Data {
        int mession = 0;      // Solver session
        int reltol = 0;       // Relative tolerance flag
        double tol = 0.0;     // Tolerance
        int maxiter = 0;      // Max iterations
        int pcond = 0;        // Preconditioner
    };

    EmSolverFem() = default;

    std::string getKeywordName() const override { return "*EM_SOLVER_FEM"; }

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
 * @brief *EM_OUTPUT
 *
 * Electromagnetic output control.
 */
class KOO_API EmOutput : public CloneableKeyword<EmOutput, EmKeyword> {
public:
    struct Data {
        int mession = 0;      // Output session
        int magfld = 0;       // Magnetic field output
        int efld = 0;         // Electric field output
        int jfld = 0;         // Current density output
        int joule = 0;        // Joule heating output
    };

    EmOutput() = default;

    std::string getKeywordName() const override { return "*EM_OUTPUT"; }

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
 * @brief *EM_ISOPOTENTIAL
 *
 * Electromagnetic isopotential boundary.
 */
class KOO_API EmIsopotential : public CloneableKeyword<EmIsopotential, EmKeyword> {
public:
    struct Data {
        int setid = 0;        // Set ID
        int settype = 0;      // Set type
        double val = 0.0;     // Potential value
        int lcid = 0;         // Load curve ID
    };

    EmIsopotential() = default;

    std::string getKeywordName() const override { return "*EM_ISOPOTENTIAL"; }

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
 * @brief *EM_CONTACT
 *
 * Electromagnetic contact interface.
 */
class KOO_API EmContact : public CloneableKeyword<EmContact, EmKeyword> {
public:
    struct Data {
        int contid = 0;       // Contact ID
        int ssid1 = 0;        // Slave set ID
        int ssid2 = 0;        // Master set ID
        int conttype = 0;     // Contact type
        double perm = 0.0;    // Contact permeance
    };

    EmContact() = default;

    std::string getKeywordName() const override { return "*EM_CONTACT"; }

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
 * @brief *EM_MAT_002
 *
 * Electromagnetic material type 2 (insulator).
 */
class KOO_API EmMat002 : public CloneableKeyword<EmMat002, EmKeyword> {
public:
    struct Data {
        int mid = 0;          // Material ID
        double eosmu = 0.0;   // Magnetic permeability
        double eosep = 0.0;   // Electric permittivity
        double sigma = 0.0;   // Electrical conductivity (low for insulator)
    };

    EmMat002() = default;

    std::string getKeywordName() const override { return "*EM_MAT_002"; }

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
 * @brief *EM_CONTROL_COUPLING
 *
 * Electromagnetic coupling control parameters.
 */
class KOO_API EmControlCoupling : public CloneableKeyword<EmControlCoupling, EmKeyword> {
public:
    struct Data {
        int couptype = 0;     // Coupling type
        int mechsol = 0;      // Mechanical solver flag
        int thermsol = 0;     // Thermal solver flag
        double tstart = 0.0;  // Coupling start time
        double tend = 0.0;    // Coupling end time
    };

    EmControlCoupling() = default;

    std::string getKeywordName() const override { return "*EM_CONTROL_COUPLING"; }

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
 * @brief *EM_RANDLES_BATMAC
 *
 * Randles circuit model for battery macro simulation.
 */
class KOO_API EmRandlesBatmac : public CloneableKeyword<EmRandlesBatmac, EmKeyword> {
public:
    struct Data {
        int batmacid = 0;       // Battery macro ID
        int rdlid = 0;          // Randles circuit ID
        int functype = 0;       // Function type
        double r0 = 0.0;        // Internal resistance R0
        double r1 = 0.0;        // Resistance R1
        double c1 = 0.0;        // Capacitance C1
        double r2 = 0.0;        // Resistance R2
        double c2 = 0.0;        // Capacitance C2
        int soc_lcid = 0;       // Load curve for SOC
        int temp_lcid = 0;      // Load curve for temperature
    };

    EmRandlesBatmac() = default;

    std::string getKeywordName() const override { return "*EM_RANDLES_BATMAC"; }

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
