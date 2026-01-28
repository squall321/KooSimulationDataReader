#pragma once

#include <koo/Export.hpp>
#include <koo/dyna/Keyword.hpp>
#include <koo/util/Types.hpp>
#include <vector>
#include <string>

namespace koo::dyna {

/**
 * @brief Base class for *STOCHASTIC keywords
 */
class KOO_API StochasticKeyword : public Keyword {
public:
    virtual ~StochasticKeyword() = default;
};

/**
 * @brief *STOCHASTIC_STRUCTURE_FIELD
 *
 * Stochastic structural field definition.
 */
class KOO_API StochasticStructureField : public CloneableKeyword<StochasticStructureField, StochasticKeyword> {
public:
    struct Data {
        int fieldid = 0;      // Field ID
        int ftype = 0;        // Field type
        double mean = 0.0;    // Mean value
        double stddev = 0.0;  // Standard deviation
        int disttype = 0;     // Distribution type
        double corlen = 0.0;  // Correlation length
    };

    StochasticStructureField() = default;

    std::string getKeywordName() const override { return "*STOCHASTIC_STRUCTURE_FIELD"; }

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
 * @brief *STOCHASTIC_RANDOMIZATION
 *
 * Randomization control for stochastic analysis.
 */
class KOO_API StochasticRandomization : public CloneableKeyword<StochasticRandomization, StochasticKeyword> {
public:
    struct Data {
        int seed = 0;         // Random seed
        int nsamples = 0;     // Number of samples
        int method = 0;       // Sampling method
        int output = 0;       // Output control
    };

    StochasticRandomization() = default;

    std::string getKeywordName() const override { return "*STOCHASTIC_RANDOMIZATION"; }

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
 * @brief *STOCHASTIC_VARIABLE
 *
 * Stochastic variable definition.
 */
class KOO_API StochasticVariable : public CloneableKeyword<StochasticVariable, StochasticKeyword> {
public:
    struct Data {
        int varid = 0;        // Variable ID
        double mean = 0.0;    // Mean value
        double stddev = 0.0;  // Standard deviation
        int disttype = 0;     // Distribution type (0=normal, 1=uniform, etc.)
        double param1 = 0.0;  // Distribution parameter 1
        double param2 = 0.0;  // Distribution parameter 2
    };

    StochasticVariable() = default;

    std::string getKeywordName() const override { return "*STOCHASTIC_VARIABLE"; }

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
 * @brief *STOCHASTIC_CORRELATED_FIELD
 *
 * Correlated random field definition.
 */
class KOO_API StochasticCorrelatedField : public CloneableKeyword<StochasticCorrelatedField, StochasticKeyword> {
public:
    struct Data {
        int fieldid = 0;      // Field ID
        int cortype = 0;      // Correlation type (0=exponential, 1=gaussian)
        double lx = 0.0;      // Correlation length X
        double ly = 0.0;      // Correlation length Y
        double lz = 0.0;      // Correlation length Z
        int nkl = 0;          // Number of KL terms
    };

    StochasticCorrelatedField() = default;

    std::string getKeywordName() const override { return "*STOCHASTIC_CORRELATED_FIELD"; }

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
 * @brief *STOCHASTIC_MATERIAL_PROPERTY
 *
 * Stochastic material property assignment.
 */
class KOO_API StochasticMaterialProperty : public CloneableKeyword<StochasticMaterialProperty, StochasticKeyword> {
public:
    struct Data {
        int mid = 0;          // Material ID
        int propid = 0;       // Property ID (1=E, 2=nu, 3=rho, etc.)
        int varid = 0;        // Stochastic variable ID
        double scale = 1.0;   // Scale factor
    };

    StochasticMaterialProperty() = default;

    std::string getKeywordName() const override { return "*STOCHASTIC_MATERIAL_PROPERTY"; }

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
 * @brief *STOCHASTIC_PART
 *
 * Stochastic part definition.
 */
class KOO_API StochasticPart : public CloneableKeyword<StochasticPart, StochasticKeyword> {
public:
    struct Data {
        int pid = 0;          // Part ID
        int fieldid = 0;      // Field ID
        int maptype = 0;      // Mapping type
        double tol = 0.0;     // Tolerance
    };

    StochasticPart() = default;

    std::string getKeywordName() const override { return "*STOCHASTIC_PART"; }

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
 * @brief *STOCHASTIC_SAMPLE
 *
 * Stochastic sampling parameters.
 */
class KOO_API StochasticSample : public CloneableKeyword<StochasticSample, StochasticKeyword> {
public:
    struct Data {
        int sampleid = 0;     // Sample ID
        int method = 0;       // Sampling method (0=MC, 1=LHS, 2=QMC)
        int nsamples = 0;     // Number of samples
        int seed = 0;         // Random seed
        int output = 0;       // Output control
    };

    StochasticSample() = default;

    std::string getKeywordName() const override { return "*STOCHASTIC_SAMPLE"; }

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
 * @brief *STOCHASTIC_OUTPUT
 *
 * Stochastic analysis output control.
 */
class KOO_API StochasticOutput : public CloneableKeyword<StochasticOutput, StochasticKeyword> {
public:
    struct Data {
        int outtype = 0;      // Output type
        int histbin = 0;      // Histogram bins
        double conf = 0.95;   // Confidence level
        int stats = 0;        // Statistics output flag
        int corr = 0;         // Correlation output flag
    };

    StochasticOutput() = default;

    std::string getKeywordName() const override { return "*STOCHASTIC_OUTPUT"; }

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
 * @brief *STOCHASTIC_MONTE_CARLO_RUN
 *
 * Monte Carlo simulation run control.
 */
class KOO_API StochasticMonteCarloRun : public CloneableKeyword<StochasticMonteCarloRun, StochasticKeyword> {
public:
    struct Data {
        int runid = 0;        // Run ID
        int nruns = 0;        // Number of runs
        int seed = 0;         // Random seed
        int parallel = 0;     // Parallel execution flag
        int restart = 0;      // Restart flag
        int outfreq = 0;      // Output frequency
    };

    StochasticMonteCarloRun() = default;

    std::string getKeywordName() const override { return "*STOCHASTIC_MONTE_CARLO_RUN"; }

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
