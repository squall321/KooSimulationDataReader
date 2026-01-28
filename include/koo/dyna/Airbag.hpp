#pragma once

#include <koo/Export.hpp>
#include <koo/dyna/Keyword.hpp>
#include <koo/util/Types.hpp>
#include <vector>
#include <string>

namespace koo::dyna {

/**
 * @brief Base class for *AIRBAG keywords
 */
class KOO_API AirbagKeyword : public Keyword {
public:
    virtual ~AirbagKeyword() = default;
};

/**
 * @brief *AIRBAG_SIMPLE_PRESSURE_VOLUME
 *
 * Simple airbag model using pressure-volume relationship.
 */
class KOO_API AirbagSimplePressureVolume : public CloneableKeyword<AirbagSimplePressureVolume, AirbagKeyword> {
public:
    struct Data {
        int sid = 0;          // Set ID for airbag part
        int sidtyp = 0;       // Set type (0=segment, 1=part)
        int rbid = 0;         // Rigid body ID
        double vsca = 1.0;    // Volume scale factor
        double psca = 1.0;    // Pressure scale factor
        double vini = 0.0;    // Initial volume
        double mwd = 0.0;     // Mass weighted damping
        double spsf = 0.0;    // Stagnation pressure scale factor
        int cn = 0;           // Heat capacity ratio curve
        int beta = 0;         // Shape factor for venting
        int lcid = 0;         // Load curve ID for pressure vs. time
        int lciddr = 0;       // Load curve ID for discharge coefficient
    };

    AirbagSimplePressureVolume() = default;

    std::string getKeywordName() const override { return "*AIRBAG_SIMPLE_PRESSURE_VOLUME"; }

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
 * @brief *AIRBAG_HYBRID
 *
 * Hybrid airbag model with gas dynamics.
 */
class KOO_API AirbagHybrid : public CloneableKeyword<AirbagHybrid, AirbagKeyword> {
public:
    struct Data {
        int sid = 0;          // Set ID
        int sidtyp = 0;       // Set type
        int rbid = 0;         // Rigid body ID
        double vsca = 1.0;    // Volume scale factor
        double psca = 1.0;    // Pressure scale factor
        double vini = 0.0;    // Initial volume
        double mwd = 0.0;     // Mass weighted damping
        double spsf = 0.0;    // Stagnation pressure scale factor
        double atmost = 0.0;  // Atmospheric temperature
        double atmosp = 0.0;  // Atmospheric pressure
        double gc = 0.0;      // Universal gas constant
        int cc = 0;           // Mass flow rate curve
        int cci = 0;          // Inflator curve
    };

    AirbagHybrid() = default;

    std::string getKeywordName() const override { return "*AIRBAG_HYBRID"; }

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
 * @brief *AIRBAG_PARTICLE
 *
 * Corpuscular Particle Method (CPM) airbag.
 */
class KOO_API AirbagParticle : public CloneableKeyword<AirbagParticle, AirbagKeyword> {
public:
    struct Data {
        int sid = 0;          // Set ID
        int sidtyp = 0;       // Set type
        int rbid = 0;         // Rigid body ID
        double vsca = 1.0;    // Volume scale factor
        double psca = 1.0;    // Pressure scale factor
        double vini = 0.0;    // Initial volume
        double mwd = 0.0;     // Mass weighted damping
        double spsf = 0.0;    // Stagnation pressure scale factor
        int np = 0;           // Number of particles
        int unit = 0;         // Unit system
        double visca = 0.0;   // Viscosity
        double tatm = 0.0;    // Atmospheric temperature
        double patm = 0.0;    // Atmospheric pressure
    };

    AirbagParticle() = default;

    std::string getKeywordName() const override { return "*AIRBAG_PARTICLE"; }

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
 * @brief *AIRBAG_REFERENCE_GEOMETRY
 *
 * Defines reference geometry for airbag folding.
 */
class KOO_API AirbagReferenceGeometry : public CloneableKeyword<AirbagReferenceGeometry, AirbagKeyword> {
public:
    struct Data {
        int id = 0;           // Reference geometry ID
        std::string title;    // Optional title
        int birth = 0;        // Birth time flag
        double tbirth = 0.0;  // Time of birth
        double tdeath = 0.0;  // Time of death
    };

    AirbagReferenceGeometry() = default;

    std::string getKeywordName() const override { return "*AIRBAG_REFERENCE_GEOMETRY"; }

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
 * @brief *AIRBAG_SIMPLE_AIRBAG_MODEL
 *
 * Simple airbag model for inflator gas dynamics.
 */
class KOO_API AirbagSimpleAirbagModel : public CloneableKeyword<AirbagSimpleAirbagModel, AirbagKeyword> {
public:
    struct Data {
        int sid = 0;          // Set ID for airbag part
        int sidtyp = 0;       // Set type (0=segment, 1=part)
        int rbid = 0;         // Rigid body ID
        double vsca = 1.0;    // Volume scale factor
        double psca = 1.0;    // Pressure scale factor
        double vini = 0.0;    // Initial volume
        double mwd = 0.0;     // Mass weighted damping
        double spsf = 0.0;    // Stagnation pressure scale factor
        double cv = 0.0;      // Heat capacity at constant volume
        double cp = 0.0;      // Heat capacity at constant pressure
        double t = 0.0;       // Temperature of inflator gas
        int lcid = 0;         // Load curve ID for mass flow rate
        double mu = 0.0;      // Shape factor for exit flow
        double pe = 0.0;      // Ambient pressure
        double ro = 0.0;      // Ambient density
    };

    AirbagSimpleAirbagModel() = default;

    std::string getKeywordName() const override { return "*AIRBAG_SIMPLE_AIRBAG_MODEL"; }

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
 * @brief *AIRBAG_WANG_NEFSKE
 *
 * Wang-Nefske airbag model.
 */
class KOO_API AirbagWangNefske : public CloneableKeyword<AirbagWangNefske, AirbagKeyword> {
public:
    struct Data {
        int sid = 0;          // Set ID
        int sidtyp = 0;       // Set type
        int rbid = 0;         // Rigid body ID
        double vsca = 1.0;    // Volume scale factor
        double psca = 1.0;    // Pressure scale factor
        double vini = 0.0;    // Initial volume
        double mwd = 0.0;     // Mass weighted damping
        double spsf = 0.0;    // Stagnation pressure scale factor
        double cn = 0.0;      // Vent coefficient
        double beta = 0.0;    // Shape factor for vent
        int lcid = 0;         // Load curve for mass flow rate
        int lciddr = 0;       // Load curve for discharge coeff
        double cv = 0.0;      // Heat capacity at constant volume
        double cp = 0.0;      // Heat capacity at constant pressure
        double t = 0.0;       // Temperature of inflator gas
    };

    AirbagWangNefske() = default;

    std::string getKeywordName() const override { return "*AIRBAG_WANG_NEFSKE"; }

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
 * @brief *AIRBAG_LINEAR_FLUID
 *
 * Linear fluid airbag model.
 */
class KOO_API AirbagLinearFluid : public CloneableKeyword<AirbagLinearFluid, AirbagKeyword> {
public:
    struct Data {
        int sid = 0;          // Set ID
        int sidtyp = 0;       // Set type
        int rbid = 0;         // Rigid body ID
        double vsca = 1.0;    // Volume scale factor
        double psca = 1.0;    // Pressure scale factor
        double vini = 0.0;    // Initial volume
        double bulk = 0.0;    // Bulk modulus
        double ro = 0.0;      // Initial density
        double pe = 0.0;      // Initial pressure
        int lcid = 0;         // Load curve for pressure vs time
    };

    AirbagLinearFluid() = default;

    std::string getKeywordName() const override { return "*AIRBAG_LINEAR_FLUID"; }

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
 * @brief *AIRBAG_ADIABATIC_GAS_MODEL
 *
 * Adiabatic gas airbag model.
 */
class KOO_API AirbagAdiabaticGasModel : public CloneableKeyword<AirbagAdiabaticGasModel, AirbagKeyword> {
public:
    struct Data {
        int sid = 0;          // Set ID
        int sidtyp = 0;       // Set type
        int rbid = 0;         // Rigid body ID
        double vsca = 1.0;    // Volume scale factor
        double psca = 1.0;    // Pressure scale factor
        double vini = 0.0;    // Initial volume
        double mwd = 0.0;     // Mass weighted damping
        double spsf = 0.0;    // Stagnation pressure scale factor
        double gamma = 1.4;   // Heat capacity ratio
        double pe = 0.0;      // Initial pressure
        double ro = 0.0;      // Initial density
    };

    AirbagAdiabaticGasModel() = default;

    std::string getKeywordName() const override { return "*AIRBAG_ADIABATIC_GAS_MODEL"; }

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
 * @brief *AIRBAG_INTERACTION
 *
 * Airbag interaction definition (for multi-chamber airbags).
 */
class KOO_API AirbagInteraction : public CloneableKeyword<AirbagInteraction, AirbagKeyword> {
public:
    struct Data {
        int master = 0;       // Master airbag ID
        int slave = 0;        // Slave airbag ID
        double area = 0.0;    // Vent area
        int lcid = 0;         // Load curve for vent coefficient
        int shape = 0;        // Vent shape
        double mu = 0.0;      // Discharge coefficient
    };

    AirbagInteraction() = default;

    std::string getKeywordName() const override { return "*AIRBAG_INTERACTION"; }

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
 * @brief *AIRBAG_LOAD_CURVE
 *
 * Airbag with pressure/mass input via load curves.
 */
class KOO_API AirbagLoadCurve : public CloneableKeyword<AirbagLoadCurve, AirbagKeyword> {
public:
    struct Data {
        int sid = 0;          // Set ID for airbag part
        int sidtyp = 0;       // Set type (0=segment, 1=part)
        int rbid = 0;         // Rigid body ID
        double vsca = 1.0;    // Volume scale factor
        double psca = 1.0;    // Pressure scale factor
        double vini = 0.0;    // Initial volume
        double mwd = 0.0;     // Mass weighted damping
        double spsf = 0.0;    // Stagnation pressure scale factor
        // Card 2
        int cn = 0;           // Heat capacity curve/constant
        int beta = 0;         // Shape factor for venting
        int lcid = 0;         // Load curve for inflator mass flow
        int lciddr = 0;       // Load curve for discharge coefficient
        double lcidt = 0.0;   // Load curve for inflator temperature
        double lcidr = 0.0;   // Load curve for gas constant
        int gcflag = 0;       // Gas composition flag
        int gcid = 0;         // Gas composition ID
    };

    AirbagLoadCurve() = default;

    std::string getKeywordName() const override { return "*AIRBAG_LOAD_CURVE"; }

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
