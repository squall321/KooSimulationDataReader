#pragma once

#include <koo/Export.hpp>
#include <koo/dyna/Keyword.hpp>
#include <koo/util/Types.hpp>
#include <vector>
#include <string>

namespace koo::dyna {

/**
 * @brief Base class for *CHEMISTRY keywords
 */
class KOO_API ChemistryKeyword : public Keyword {
public:
    virtual ~ChemistryKeyword() = default;
};

/**
 * @brief *CHEMISTRY_CONTROL_INFLATOR
 *
 * Chemistry inflator control parameters.
 */
class KOO_API ChemistryControlInflator : public CloneableKeyword<ChemistryControlInflator, ChemistryKeyword> {
public:
    struct Data {
        int sid = 0;          // Surface ID
        int infid = 0;        // Inflator ID
        double atefr = 0.0;   // Activation time
        int lcid = 0;         // Mass flow rate curve
        int lcidt = 0;        // Temperature curve
    };

    ChemistryControlInflator() = default;

    std::string getKeywordName() const override { return "*CHEMISTRY_CONTROL_INFLATOR"; }

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
 * @brief *CHEMISTRY_COMPOSITION
 *
 * Chemistry species composition.
 */
class KOO_API ChemistryComposition : public CloneableKeyword<ChemistryComposition, ChemistryKeyword> {
public:
    struct Data {
        int compid = 0;       // Composition ID
        int nspec = 0;        // Number of species
    };

    struct Species {
        int specid = 0;       // Species ID
        double mfrac = 0.0;   // Mass fraction
    };

    ChemistryComposition() = default;

    std::string getKeywordName() const override { return "*CHEMISTRY_COMPOSITION"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    Data& getData() { return data_; }
    const Data& getData() const { return data_; }

    std::vector<Species>& getSpecies() { return species_; }
    const std::vector<Species>& getSpecies() const { return species_; }

private:
    Data data_;
    std::vector<Species> species_;
};

/**
 * @brief *CHEMISTRY_INFLATOR_PROPERTIES
 *
 * Chemistry inflator properties.
 */
class KOO_API ChemistryInflatorProperties : public CloneableKeyword<ChemistryInflatorProperties, ChemistryKeyword> {
public:
    struct Data {
        int infid = 0;        // Inflator ID
        double chmass = 0.0;  // Total chemical mass
        double chtemp = 0.0;  // Initial chemical temperature
        double chpres = 0.0;  // Initial chemical pressure
        int compid = 0;       // Composition ID
    };

    ChemistryInflatorProperties() = default;

    std::string getKeywordName() const override { return "*CHEMISTRY_INFLATOR_PROPERTIES"; }

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
 * @brief *CHEMISTRY_CONTROL_ZND
 *
 * Chemistry ZND detonation model control.
 */
class KOO_API ChemistryControlZnd : public CloneableKeyword<ChemistryControlZnd, ChemistryKeyword> {
public:
    struct Data {
        int zndid = 0;        // ZND ID
        double dcjvel = 0.0;  // CJ detonation velocity
        double pcj = 0.0;     // CJ pressure
        double rhocj = 0.0;   // CJ density
        int lcid = 0;         // Load curve ID
    };

    ChemistryControlZnd() = default;

    std::string getKeywordName() const override { return "*CHEMISTRY_CONTROL_ZND"; }

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
 * @brief *CHEMISTRY_MODEL
 *
 * Chemistry reaction model.
 */
class KOO_API ChemistryModel : public CloneableKeyword<ChemistryModel, ChemistryKeyword> {
public:
    struct Data {
        int modid = 0;        // Model ID
        int mtype = 0;        // Model type
        double atemp = 0.0;   // Activation temperature
        double acoef = 0.0;   // Pre-exponential factor
        double ecoef = 0.0;   // Exponent coefficient
    };

    ChemistryModel() = default;

    std::string getKeywordName() const override { return "*CHEMISTRY_MODEL"; }

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
 * @brief *CHEMISTRY_SPECIES
 *
 * Chemistry species definition.
 */
class KOO_API ChemistrySpecies : public CloneableKeyword<ChemistrySpecies, ChemistryKeyword> {
public:
    struct Data {
        int specid = 0;       // Species ID
        double molwt = 0.0;   // Molecular weight
        double hform = 0.0;   // Heat of formation
        double cp = 0.0;      // Specific heat
    };

    ChemistrySpecies() = default;

    std::string getKeywordName() const override { return "*CHEMISTRY_SPECIES"; }

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
 * @brief *CHEMISTRY_REACTION
 *
 * Chemistry reaction definition.
 */
class KOO_API ChemistryReaction : public CloneableKeyword<ChemistryReaction, ChemistryKeyword> {
public:
    struct Data {
        int rxnid = 0;        // Reaction ID
        int modid = 0;        // Model ID
        double arrhenius_a = 0.0;  // Arrhenius A
        double arrhenius_b = 0.0;  // Arrhenius B
        double arrhenius_e = 0.0;  // Activation energy
    };

    ChemistryReaction() = default;

    std::string getKeywordName() const override { return "*CHEMISTRY_REACTION"; }

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
 * @brief *CHEMISTRY_CONTROL
 *
 * General chemistry control parameters.
 */
class KOO_API ChemistryControl : public CloneableKeyword<ChemistryControl, ChemistryKeyword> {
public:
    struct Data {
        int ctrlid = 0;       // Control ID
        int method = 0;       // Solution method
        double dtmin = 0.0;   // Minimum time step
        double dtmax = 0.0;   // Maximum time step
        int maxiter = 0;      // Maximum iterations
        double tol = 0.0;     // Tolerance
    };

    ChemistryControl() = default;

    std::string getKeywordName() const override { return "*CHEMISTRY_CONTROL"; }

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
 * @brief *CHEMISTRY_DETONATION
 *
 * Chemistry detonation parameters.
 */
class KOO_API ChemistryDetonation : public CloneableKeyword<ChemistryDetonation, ChemistryKeyword> {
public:
    struct Data {
        int detid = 0;        // Detonation ID
        double xdet = 0.0;    // Detonation point x
        double ydet = 0.0;    // Detonation point y
        double zdet = 0.0;    // Detonation point z
        double tdet = 0.0;    // Detonation time
        double dvel = 0.0;    // Detonation velocity
    };

    ChemistryDetonation() = default;

    std::string getKeywordName() const override { return "*CHEMISTRY_DETONATION"; }

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
