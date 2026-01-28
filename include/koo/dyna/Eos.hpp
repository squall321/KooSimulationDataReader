#pragma once

#include <koo/Export.hpp>
#include <koo/dyna/Keyword.hpp>
#include <koo/util/Types.hpp>
#include <vector>
#include <string>

namespace koo::dyna {

/**
 * @brief Base class for *EOS keywords
 */
class KOO_API EosKeyword : public Keyword {
public:
    virtual ~EosKeyword() = default;
    virtual int getEosId() const = 0;
};

/**
 * @brief *EOS_LINEAR_POLYNOMIAL
 *
 * Linear polynomial equation of state.
 */
class KOO_API EosLinearPolynomial : public CloneableKeyword<EosLinearPolynomial, EosKeyword> {
public:
    struct Data {
        int eosid = 0;        // EOS ID
        double c0 = 0.0;      // Pressure constant
        double c1 = 0.0;      // Linear coefficient
        double c2 = 0.0;      // Quadratic coefficient
        double c3 = 0.0;      // Cubic coefficient
        double c4 = 0.0;      // Energy coefficient
        double c5 = 0.0;      // Energy coefficient 2
        double c6 = 0.0;      // Energy coefficient 3
        double e0 = 0.0;      // Initial internal energy
        double v0 = 1.0;      // Initial relative volume
    };

    EosLinearPolynomial() = default;

    std::string getKeywordName() const override { return "*EOS_LINEAR_POLYNOMIAL"; }
    int getEosId() const override { return data_.eosid; }

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
 * @brief *EOS_GRUNEISEN
 *
 * Gruneisen equation of state.
 */
class KOO_API EosGruneisen : public CloneableKeyword<EosGruneisen, EosKeyword> {
public:
    struct Data {
        int eosid = 0;        // EOS ID
        double c = 0.0;       // Bulk speed of sound
        double s1 = 0.0;      // Linear Hugoniot slope
        double s2 = 0.0;      // Quadratic Hugoniot slope
        double s3 = 0.0;      // Cubic Hugoniot slope
        double gamao = 0.0;   // Gruneisen gamma
        double a = 0.0;       // First order volume correction
        double e0 = 0.0;      // Initial internal energy
        double v0 = 1.0;      // Initial relative volume
    };

    EosGruneisen() = default;

    std::string getKeywordName() const override { return "*EOS_GRUNEISEN"; }
    int getEosId() const override { return data_.eosid; }

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
 * @brief *EOS_JWL
 *
 * Jones-Wilkins-Lee equation of state for detonation products.
 */
class KOO_API EosJwl : public CloneableKeyword<EosJwl, EosKeyword> {
public:
    struct Data {
        int eosid = 0;        // EOS ID
        double a = 0.0;       // Coefficient A
        double b = 0.0;       // Coefficient B
        double r1 = 0.0;      // Coefficient R1
        double r2 = 0.0;      // Coefficient R2
        double omeg = 0.0;    // Omega coefficient
        double e0 = 0.0;      // Initial internal energy
        double v0 = 1.0;      // Initial relative volume
    };

    EosJwl() = default;

    std::string getKeywordName() const override { return "*EOS_JWL"; }
    int getEosId() const override { return data_.eosid; }

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
 * @brief *EOS_IDEAL_GAS
 *
 * Ideal gas equation of state.
 */
class KOO_API EosIdealGas : public CloneableKeyword<EosIdealGas, EosKeyword> {
public:
    struct Data {
        int eosid = 0;        // EOS ID
        double cv = 0.0;      // Heat capacity at constant volume
        double cp = 0.0;      // Heat capacity at constant pressure
        double t0 = 0.0;      // Initial temperature
        double v0 = 1.0;      // Initial relative volume
    };

    EosIdealGas() = default;

    std::string getKeywordName() const override { return "*EOS_IDEAL_GAS"; }
    int getEosId() const override { return data_.eosid; }

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
 * @brief *EOS_TABULATED_COMPACTION
 *
 * Tabulated compaction equation of state.
 */
class KOO_API EosTabulatedCompaction : public CloneableKeyword<EosTabulatedCompaction, EosKeyword> {
public:
    struct Data {
        int eosid = 0;        // EOS ID
        double gama = 0.0;    // Gamma
        double e0 = 0.0;      // Initial internal energy
        double v0 = 1.0;      // Initial relative volume
        int lcid = 0;         // Load curve ID for pressure vs volumetric strain
        int lcid2 = 0;        // Load curve ID for bulk modulus vs volumetric strain
    };

    EosTabulatedCompaction() = default;

    std::string getKeywordName() const override { return "*EOS_TABULATED_COMPACTION"; }
    int getEosId() const override { return data_.eosid; }

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
 * @brief *EOS_IGNITION_GROWTH
 *
 * Ignition and growth equation of state for reactive materials.
 */
class KOO_API EosIgnitionGrowth : public CloneableKeyword<EosIgnitionGrowth, EosKeyword> {
public:
    struct Data {
        int eosid = 0;        // EOS ID
        double a = 0.0;       // A constant
        double b = 0.0;       // B constant
        double r1 = 0.0;      // R1 constant
        double r2 = 0.0;      // R2 constant
        double omeg = 0.0;    // Omega
        double e0 = 0.0;      // Initial internal energy
        double v0 = 1.0;      // Initial relative volume
    };

    EosIgnitionGrowth() = default;

    std::string getKeywordName() const override { return "*EOS_IGNITION_GROWTH"; }
    int getEosId() const override { return data_.eosid; }

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
 * @brief *EOS_MURNAGHAN
 *
 * Murnaghan equation of state.
 */
class KOO_API EosMurnaghan : public CloneableKeyword<EosMurnaghan, EosKeyword> {
public:
    struct Data {
        int eosid = 0;        // EOS ID
        double k = 0.0;       // Bulk modulus
        double n = 0.0;       // Pressure derivative of bulk modulus
        double ref = 0.0;     // Reference pressure
    };

    EosMurnaghan() = default;

    std::string getKeywordName() const override { return "*EOS_MURNAGHAN"; }
    int getEosId() const override { return data_.eosid; }

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
 * @brief *EOS_TILLOTSON
 *
 * Tillotson equation of state for hypervelocity impact.
 */
class KOO_API EosTillotson : public CloneableKeyword<EosTillotson, EosKeyword> {
public:
    struct Data {
        int eosid = 0;        // EOS ID
        double a = 0.0;       // A coefficient
        double b = 0.0;       // B coefficient
        double alpha = 0.0;   // Alpha
        double beta = 0.0;    // Beta
        double e0 = 0.0;      // Initial internal energy
        double es = 0.0;      // Sublimation energy
        double ess = 0.0;     // Expanded energy
        double rho0 = 0.0;    // Reference density
    };

    EosTillotson() = default;

    std::string getKeywordName() const override { return "*EOS_TILLOTSON"; }
    int getEosId() const override { return data_.eosid; }

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
 * @brief *EOS_TABULATED
 *
 * General tabulated equation of state.
 */
class KOO_API EosTabulated : public CloneableKeyword<EosTabulated, EosKeyword> {
public:
    struct Data {
        int eosid = 0;        // EOS ID
        double gama = 0.0;    // Gamma
        double e0 = 0.0;      // Initial internal energy
        double v0 = 1.0;      // Initial relative volume
        int lcid = 0;         // Load curve ID
    };

    EosTabulated() = default;

    std::string getKeywordName() const override { return "*EOS_TABULATED"; }
    int getEosId() const override { return data_.eosid; }

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
 * @brief *EOS_STIFF_GAS
 *
 * Stiffened gas equation of state.
 */
class KOO_API EosStiffGas : public CloneableKeyword<EosStiffGas, EosKeyword> {
public:
    struct Data {
        int eosid = 0;        // EOS ID
        double gamma = 0.0;   // Ratio of specific heats
        double e0 = 0.0;      // Initial internal energy
        double v0 = 1.0;      // Initial relative volume
        double pref = 0.0;    // Reference pressure
    };

    EosStiffGas() = default;

    std::string getKeywordName() const override { return "*EOS_STIFF_GAS"; }
    int getEosId() const override { return data_.eosid; }

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
 * @brief *EOS_SACK
 *
 * SACK (Sandia ASCI Code) equation of state.
 */
class KOO_API EosSack : public CloneableKeyword<EosSack, EosKeyword> {
public:
    struct Data {
        int eosid = 0;        // EOS ID
        double e0 = 0.0;      // Initial internal energy
        double v0 = 1.0;      // Initial relative volume
        int lcid = 0;         // Load curve ID
    };

    EosSack() = default;

    std::string getKeywordName() const override { return "*EOS_SACK"; }
    int getEosId() const override { return data_.eosid; }

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
 * @brief *EOS_RATIO_OF_POLYNOMIALS
 *
 * Ratio of polynomials equation of state.
 */
class KOO_API EosRatioOfPolynomials : public CloneableKeyword<EosRatioOfPolynomials, EosKeyword> {
public:
    struct Data {
        int eosid = 0;        // EOS ID
        double e0 = 0.0;      // Initial internal energy
        double v0 = 1.0;      // Initial relative volume
        double a0 = 0.0;      // Numerator constant
        double a1 = 0.0;      // Numerator linear coefficient
        double a2 = 0.0;      // Numerator quadratic coefficient
        double b0 = 1.0;      // Denominator constant
        double b1 = 0.0;      // Denominator linear coefficient
    };

    EosRatioOfPolynomials() = default;

    std::string getKeywordName() const override { return "*EOS_RATIO_OF_POLYNOMIALS"; }
    int getEosId() const override { return data_.eosid; }

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
 * @brief *EOS_OSBORNE
 *
 * Osborne equation of state.
 */
class KOO_API EosOsborne : public CloneableKeyword<EosOsborne, EosKeyword> {
public:
    struct Data {
        int eosid = 0;        // EOS ID
        double a1 = 0.0;      // A1 coefficient
        double a2 = 0.0;      // A2 coefficient
        double a3 = 0.0;      // A3 coefficient
        double b1 = 0.0;      // B1 coefficient
        double b2 = 0.0;      // B2 coefficient
        double e0 = 0.0;      // Initial internal energy
        double v0 = 1.0;      // Initial relative volume
    };

    EosOsborne() = default;

    std::string getKeywordName() const override { return "*EOS_OSBORNE"; }
    int getEosId() const override { return data_.eosid; }

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
 * @brief *EOS_PROPELLANT_DEFLAGRATION
 *
 * Propellant deflagration equation of state.
 */
class KOO_API EosPropellantDeflagration : public CloneableKeyword<EosPropellantDeflagration, EosKeyword> {
public:
    struct Data {
        int eosid = 0;        // EOS ID
        double k = 0.0;       // Burn rate coefficient
        double n = 0.0;       // Burn rate exponent
        double e0 = 0.0;      // Initial internal energy
        double v0 = 1.0;      // Initial relative volume
        double rho0 = 0.0;    // Reference density
    };

    EosPropellantDeflagration() = default;

    std::string getKeywordName() const override { return "*EOS_PROPELLANT_DEFLAGRATION"; }
    int getEosId() const override { return data_.eosid; }

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
 * @brief *EOS_SESAME
 *
 * SESAME tabular equation of state.
 */
class KOO_API EosSesame : public CloneableKeyword<EosSesame, EosKeyword> {
public:
    struct Data {
        int eosid = 0;        // EOS ID
        int matid = 0;        // SESAME material ID
        double e0 = 0.0;      // Initial internal energy
        double v0 = 1.0;      // Initial relative volume
    };

    EosSesame() = default;

    std::string getKeywordName() const override { return "*EOS_SESAME"; }
    int getEosId() const override { return data_.eosid; }

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
 * @brief *EOS_GASKET
 *
 * Gasket equation of state for foam/rubber materials.
 */
class KOO_API EosGasket : public CloneableKeyword<EosGasket, EosKeyword> {
public:
    struct Data {
        int eosid = 0;        // EOS ID
        int lcidl = 0;        // Load curve ID for loading
        int lcidu = 0;        // Load curve ID for unloading
        double e0 = 0.0;      // Initial internal energy
        double v0 = 1.0;      // Initial relative volume
    };

    EosGasket() = default;

    std::string getKeywordName() const override { return "*EOS_GASKET"; }
    int getEosId() const override { return data_.eosid; }

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
 * @brief *EOS_USER_DEFINED
 *
 * User defined equation of state.
 */
class KOO_API EosUserDefined : public CloneableKeyword<EosUserDefined, EosKeyword> {
public:
    struct Data {
        int eosid = 0;        // EOS ID
        int nhv = 0;          // Number of history variables
        double p1 = 0.0;      // User parameter 1
        double p2 = 0.0;      // User parameter 2
        double p3 = 0.0;      // User parameter 3
        double p4 = 0.0;      // User parameter 4
    };

    EosUserDefined() = default;

    std::string getKeywordName() const override { return "*EOS_USER_DEFINED"; }
    int getEosId() const override { return data_.eosid; }

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
 * @brief *EOS_POWDER_BURN
 *
 * Powder burn equation of state for propellants.
 */
class KOO_API EosPowderBurn : public CloneableKeyword<EosPowderBurn, EosKeyword> {
public:
    struct Data {
        int eosid = 0;        // EOS ID
        double a = 0.0;       // Pre-exponential factor
        double b = 0.0;       // Activation energy
        double rgas = 0.0;    // Gas constant
        double e0 = 0.0;      // Initial internal energy
        double v0 = 1.0;      // Initial relative volume
    };

    EosPowderBurn() = default;

    std::string getKeywordName() const override { return "*EOS_POWDER_BURN"; }
    int getEosId() const override { return data_.eosid; }

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
 * @brief *EOS_LINEAR_POLYNOMIAL_WITH_ENERGY_LEAK
 *
 * Linear polynomial EOS with energy leak correction.
 */
class KOO_API EosLinearPolynomialWithEnergyLeak : public CloneableKeyword<EosLinearPolynomialWithEnergyLeak, EosKeyword> {
public:
    struct Data {
        int eosid = 0;        // EOS ID
        double c0 = 0.0;      // Pressure constant
        double c1 = 0.0;      // Linear coefficient
        double c2 = 0.0;      // Quadratic coefficient
        double c3 = 0.0;      // Cubic coefficient
        double c4 = 0.0;      // Energy coefficient
        double c5 = 0.0;      // Energy coefficient 2
        double eleak = 0.0;   // Energy leak parameter
    };

    EosLinearPolynomialWithEnergyLeak() = default;

    std::string getKeywordName() const override { return "*EOS_LINEAR_POLYNOMIAL_WITH_ENERGY_LEAK"; }
    int getEosId() const override { return data_.eosid; }

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
 * @brief *EOS_JWLB
 *
 * JWL-B equation of state for afterburning.
 */
class KOO_API EosJwlb : public CloneableKeyword<EosJwlb, EosKeyword> {
public:
    struct Data {
        int eosid = 0;        // EOS ID
        double a = 0.0;       // Coefficient A
        double b = 0.0;       // Coefficient B
        double r1 = 0.0;      // Coefficient R1
        double r2 = 0.0;      // Coefficient R2
        double omeg = 0.0;    // Omega coefficient
        double e0 = 0.0;      // Initial internal energy
        double v0 = 1.0;      // Initial relative volume
    };

    EosJwlb() = default;

    std::string getKeywordName() const override { return "*EOS_JWLB"; }
    int getEosId() const override { return data_.eosid; }

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
