#pragma once

#include <koo/Export.hpp>
#include <koo/dyna/Keyword.hpp>
#include <koo/util/Types.hpp>
#include <vector>
#include <string>

namespace koo::dyna {

/**
 * @brief Base class for *MAT_ADD keywords
 */
class KOO_API MatAddKeyword : public Keyword {
public:
    virtual ~MatAddKeyword() = default;
    virtual int getMid() const = 0;
};

/**
 * @brief *MAT_ADD_EROSION
 *
 * Adds erosion criteria to a material.
 */
class KOO_API MatAddErosion : public CloneableKeyword<MatAddErosion, MatAddKeyword> {
public:
    struct Data {
        int mid = 0;          // Material ID
        double excl = 0.0;    // Exclusion flag
        double mxpres = 0.0;  // Max pressure
        double mneps = 0.0;   // Min principal strain
        double effeps = 0.0;  // Effective plastic strain
        double voleps = 0.0;  // Volumetric strain
        double numfip = 0.0;  // Number of failed integration points
        double ncs = 1.0;     // Number of element failures before deletion
        double mnpres = 0.0;  // Min pressure
        double sigp1 = 0.0;   // Max principal stress
        double sigvm = 0.0;   // Von Mises stress
        double mxeps = 0.0;   // Max principal strain
        double epssh = 0.0;   // Shear strain
        double sigth = 0.0;   // Threshold stress
        double impulse = 0.0; // Impulse
        double fatefr = 0.0;  // Failure time
    };

    MatAddErosion() = default;

    std::string getKeywordName() const override { return "*MAT_ADD_EROSION"; }
    int getMid() const override { return data_.mid; }

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
 * @brief *MAT_ADD_FATIGUE
 *
 * Adds fatigue properties to a material.
 */
class KOO_API MatAddFatigue : public CloneableKeyword<MatAddFatigue, MatAddKeyword> {
public:
    struct Data {
        int mid = 0;          // Material ID
        int lcid = 0;         // Load curve ID for S-N data
        double beta = 0.0;    // Basquin exponent
        double se = 0.0;      // Endurance limit
        double ne = 0.0;      // Endurance cycle limit
        double sf = 0.0;      // Fatigue strength coefficient
        double nf = 0.0;      // Fatigue strength exponent
        int natefr = 0;       // Number of accumulated cycles before deletion
    };

    MatAddFatigue() = default;

    std::string getKeywordName() const override { return "*MAT_ADD_FATIGUE"; }
    int getMid() const override { return data_.mid; }

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
 * @brief *MAT_ADD_THERMAL_EXPANSION
 *
 * Adds thermal expansion to a material.
 */
class KOO_API MatAddThermalExpansion : public CloneableKeyword<MatAddThermalExpansion, MatAddKeyword> {
public:
    struct Data {
        int mid = 0;          // Material ID
        int lcid = 0;         // Load curve ID for coefficient vs temp
        double alpha = 0.0;   // Coefficient of thermal expansion
        double tref = 0.0;    // Reference temperature
    };

    MatAddThermalExpansion() = default;

    std::string getKeywordName() const override { return "*MAT_ADD_THERMAL_EXPANSION"; }
    int getMid() const override { return data_.mid; }

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
 * @brief *MAT_ADD_DAMAGE_GISSMO
 *
 * Adds GISSMO damage model to material.
 */
class KOO_API MatAddDamageGissmo : public CloneableKeyword<MatAddDamageGissmo, MatAddKeyword> {
public:
    struct Data {
        int mid = 0;          // Material ID
        int numfip = 0;       // Number of integration points
        double dmgexp = 0.0;  // Damage exponent
        double dcrit = 0.0;   // Critical damage
        double fadexp = 0.0;  // Fading exponent
        int lcdmg = 0;        // Damage curve
        int lcfad = 0;        // Fading curve
        double l_in = 0.0;    // Instability strain
        int lcreg = 0;        // Regularization curve
        double ecrit = 0.0;   // Critical energy
    };

    MatAddDamageGissmo() = default;

    std::string getKeywordName() const override { return "*MAT_ADD_DAMAGE_GISSMO"; }
    int getMid() const override { return data_.mid; }

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
 * @brief *MAT_ADD_COHESIVE
 *
 * Adds cohesive zone properties to a material.
 */
class KOO_API MatAddCohesive : public CloneableKeyword<MatAddCohesive, MatAddKeyword> {
public:
    struct Data {
        int mid = 0;          // Material ID
        double t = 0.0;       // Peak traction
        double gic = 0.0;     // Mode I fracture energy
        double giic = 0.0;    // Mode II fracture energy
        double xmu = 0.0;     // Mixed mode exponent
        double t_mult = 1.0;  // Traction multiplier
    };

    MatAddCohesive() = default;

    std::string getKeywordName() const override { return "*MAT_ADD_COHESIVE"; }
    int getMid() const override { return data_.mid; }

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
 * @brief *MAT_ADD_PERMEABILITY
 *
 * Adds permeability properties to a material.
 */
class KOO_API MatAddPermeability : public CloneableKeyword<MatAddPermeability, MatAddKeyword> {
public:
    struct Data {
        int mid = 0;          // Material ID
        double perm = 0.0;    // Permeability coefficient
        int lcperm = 0;       // Load curve for permeability
        double void0 = 0.0;   // Initial void ratio
        double alpha = 0.0;   // Compressibility coefficient
    };

    MatAddPermeability() = default;

    std::string getKeywordName() const override { return "*MAT_ADD_PERMEABILITY"; }
    int getMid() const override { return data_.mid; }

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
 * @brief *MAT_ADD_PORE_AIR
 *
 * Adds pore air properties to a material.
 */
class KOO_API MatAddPoreAir : public CloneableKeyword<MatAddPoreAir, MatAddKeyword> {
public:
    struct Data {
        int mid = 0;          // Material ID
        double bulk = 0.0;    // Bulk modulus of air
        double pore_pr = 0.0; // Initial pore pressure
        int lcbulk = 0;       // Load curve for bulk modulus
    };

    MatAddPoreAir() = default;

    std::string getKeywordName() const override { return "*MAT_ADD_PORE_AIR"; }
    int getMid() const override { return data_.mid; }

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
 * @brief *MAT_ADD_INELASTICITY
 *
 * Adds inelasticity model to a material.
 */
class KOO_API MatAddInelasticity : public CloneableKeyword<MatAddInelasticity, MatAddKeyword> {
public:
    struct Data {
        int mid = 0;          // Material ID
        int itype = 0;        // Inelasticity type
        double h = 0.0;       // Hardening modulus
        double c = 0.0;       // Viscous coefficient
        double p = 0.0;       // Strain rate exponent
        int lcss = 0;         // Load curve for stress-strain
    };

    MatAddInelasticity() = default;

    std::string getKeywordName() const override { return "*MAT_ADD_INELASTICITY"; }
    int getMid() const override { return data_.mid; }

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
