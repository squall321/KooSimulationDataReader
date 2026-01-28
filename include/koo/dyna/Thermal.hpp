#pragma once

#include <koo/Export.hpp>
#include <koo/dyna/Keyword.hpp>
#include <koo/util/Types.hpp>
#include <vector>
#include <string>

namespace koo::dyna {

/**
 * @brief Base class for *CONTROL_THERMAL keywords
 */
class KOO_API ThermalKeyword : public Keyword {
public:
    virtual ~ThermalKeyword() = default;
};

/**
 * @brief *CONTROL_THERMAL_SOLVER
 *
 * Thermal solver control parameters.
 */
class KOO_API ControlThermalSolver : public CloneableKeyword<ControlThermalSolver, ThermalKeyword> {
public:
    struct Data {
        int atefr = 0;        // Analysis type
        int solver = 0;       // Solver type
        int cg_tol = 0;       // CG tolerance
        int cg_max = 0;       // Max CG iterations
        double fwork = 0.0;   // Work factor
        double etefr = 0.0;   // Energy tolerance
        int msglv = 0;        // Message level
    };

    ControlThermalSolver() = default;

    std::string getKeywordName() const override { return "*CONTROL_THERMAL_SOLVER"; }

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
 * @brief *CONTROL_THERMAL_TIMESTEP
 *
 * Thermal time step control.
 */
class KOO_API ControlThermalTimestep : public CloneableKeyword<ControlThermalTimestep, ThermalKeyword> {
public:
    struct Data {
        double ts = 0.0;      // Time step
        double tip = 0.0;     // Implicit parameter
        int its = 0;          // Implicit time stepping
        double tmin = 0.0;    // Minimum time step
        double tmax = 0.0;    // Maximum time step
        double dtemp = 0.0;   // Temperature change limit
        int tscp = 0;         // Time step control
    };

    ControlThermalTimestep() = default;

    std::string getKeywordName() const override { return "*CONTROL_THERMAL_TIMESTEP"; }

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
 * @brief *CONTROL_THERMAL_NONLINEAR
 *
 * Nonlinear thermal solver control.
 */
class KOO_API ControlThermalNonlinear : public CloneableKeyword<ControlThermalNonlinear, ThermalKeyword> {
public:
    struct Data {
        int refmax = 0;       // Max reformations
        double tol = 0.0;     // Tolerance
        int dtefr = 0;        // Default reform
        int tefr = 0;         // Thermal format
    };

    ControlThermalNonlinear() = default;

    std::string getKeywordName() const override { return "*CONTROL_THERMAL_NONLINEAR"; }

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
 * @brief *BOUNDARY_TEMPERATURE
 *
 * Temperature boundary condition.
 */
class KOO_API BoundaryTemperature : public CloneableKeyword<BoundaryTemperature, ThermalKeyword> {
public:
    struct Entry {
        int nid = 0;          // Node ID
        int lcid = 0;         // Load curve ID
        double temp = 0.0;    // Temperature
    };

    BoundaryTemperature() = default;

    std::string getKeywordName() const override { return "*BOUNDARY_TEMPERATURE"; }

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
 * @brief *BOUNDARY_FLUX
 *
 * Heat flux boundary condition.
 */
class KOO_API BoundaryFlux : public CloneableKeyword<BoundaryFlux, ThermalKeyword> {
public:
    struct Data {
        int ssid = 0;         // Segment set ID
        int lcid = 0;         // Load curve ID
        double flux = 0.0;    // Heat flux value
    };

    BoundaryFlux() = default;

    std::string getKeywordName() const override { return "*BOUNDARY_FLUX"; }

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
 * @brief *BOUNDARY_CONVECTION
 *
 * Convection boundary condition.
 */
class KOO_API BoundaryConvection : public CloneableKeyword<BoundaryConvection, ThermalKeyword> {
public:
    struct Data {
        int ssid = 0;         // Segment set ID
        int lcid = 0;         // Load curve ID for ambient temp
        int lchc = 0;         // Load curve ID for heat transfer coeff
        double hlc = 0.0;     // Heat transfer coefficient
        double tinf = 0.0;    // Ambient temperature
    };

    BoundaryConvection() = default;

    std::string getKeywordName() const override { return "*BOUNDARY_CONVECTION"; }

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
 * @brief *BOUNDARY_RADIATION
 *
 * Radiation boundary condition.
 */
class KOO_API BoundaryRadiation : public CloneableKeyword<BoundaryRadiation, ThermalKeyword> {
public:
    struct Data {
        int ssid = 0;         // Segment set ID
        int lcid = 0;         // Load curve ID for ambient temp
        double emiss = 0.0;   // Emissivity
        double tinf = 0.0;    // Ambient temperature
        double mult = 1.0;    // Stefan-Boltzmann multiplier
    };

    BoundaryRadiation() = default;

    std::string getKeywordName() const override { return "*BOUNDARY_RADIATION"; }

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
 * @brief *INITIAL_TEMPERATURE
 *
 * Initial temperature condition.
 */
class KOO_API InitialTemperature : public CloneableKeyword<InitialTemperature, ThermalKeyword> {
public:
    struct Entry {
        int nid = 0;          // Node ID
        double temp = 0.0;    // Temperature
    };

    InitialTemperature() = default;

    std::string getKeywordName() const override { return "*INITIAL_TEMPERATURE"; }

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
 * @brief *INITIAL_TEMPERATURE_SET
 *
 * Initial temperature for a node set.
 */
class KOO_API InitialTemperatureSet : public CloneableKeyword<InitialTemperatureSet, ThermalKeyword> {
public:
    struct Data {
        int nsid = 0;         // Node set ID
        double temp = 0.0;    // Temperature
    };

    InitialTemperatureSet() = default;

    std::string getKeywordName() const override { return "*INITIAL_TEMPERATURE_SET"; }

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
 * @brief *MAT_THERMAL_ISOTROPIC
 *
 * Isotropic thermal material.
 */
class KOO_API MatThermalIsotropic : public CloneableKeyword<MatThermalIsotropic, ThermalKeyword> {
public:
    struct Data {
        int tmid = 0;         // Thermal material ID
        double tro = 0.0;     // Density
        double tgrlc = 0.0;   // Thermal generation rate
        double tgmult = 0.0;  // Thermal generation multiplier
        double hc = 0.0;      // Specific heat capacity
        double tc = 0.0;      // Thermal conductivity
    };

    MatThermalIsotropic() = default;

    std::string getKeywordName() const override { return "*MAT_THERMAL_ISOTROPIC"; }

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
