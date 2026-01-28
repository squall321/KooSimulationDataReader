#pragma once

#include <koo/Export.hpp>
#include <koo/dyna/Keyword.hpp>
#include <koo/util/Types.hpp>
#include <vector>
#include <string>

namespace koo::dyna {

/**
 * @brief Base class for *INTEGRATION keywords
 */
class KOO_API IntegrationKeyword : public Keyword {
public:
    virtual ~IntegrationKeyword() = default;
};

/**
 * @brief *INTEGRATION_SHELL
 *
 * Through-thickness integration for shell elements.
 */
class KOO_API IntegrationShell : public CloneableKeyword<IntegrationShell, IntegrationKeyword> {
public:
    struct Data {
        int irid = 0;         // Integration rule ID
        int nip = 0;          // Number of integration points
        double s = 0.0;       // Scale factor
        int pid = 0;          // Part ID (optional, 0=all)
        double wf = 0.0;      // Weight factor
    };

    IntegrationShell() = default;

    std::string getKeywordName() const override { return "*INTEGRATION_SHELL"; }

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
 * @brief *INTEGRATION_BEAM
 *
 * Through-section integration for beam elements.
 */
class KOO_API IntegrationBeam : public CloneableKeyword<IntegrationBeam, IntegrationKeyword> {
public:
    struct Data {
        int irid = 0;         // Integration rule ID
        int nip = 0;          // Number of integration points
        int ra = 0;           // Rule type axis A
        int icst = 0;         // Cross section type
        double ts1 = 0.0;     // Thickness scale 1
        double ts2 = 0.0;     // Thickness scale 2
    };

    IntegrationBeam() = default;

    std::string getKeywordName() const override { return "*INTEGRATION_BEAM"; }

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
 * @brief *INTEGRATION_SOLID
 *
 * Integration rules for solid elements.
 */
class KOO_API IntegrationSolid : public CloneableKeyword<IntegrationSolid, IntegrationKeyword> {
public:
    struct Data {
        int irid = 0;         // Integration rule ID
        int nip = 0;          // Number of integration points
        int nxdof = 0;        // Extra DOF
        int nhsv = 0;         // Number of history variables
    };

    IntegrationSolid() = default;

    std::string getKeywordName() const override { return "*INTEGRATION_SOLID"; }

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
 * @brief *INTEGRATION_POINT_TRANSFORM
 *
 * Transform integration points.
 */
class KOO_API IntegrationPointTransform : public CloneableKeyword<IntegrationPointTransform, IntegrationKeyword> {
public:
    struct Data {
        int irid = 0;         // Integration rule ID
        int itype = 0;        // Transform type
        double xi1 = 0.0;     // XI coordinate 1
        double xi2 = 0.0;     // XI coordinate 2
        double xi3 = 0.0;     // XI coordinate 3
        double wgt = 0.0;     // Weight
    };

    IntegrationPointTransform() = default;

    std::string getKeywordName() const override { return "*INTEGRATION_POINT_TRANSFORM"; }

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
 * @brief *INTEGRATION_SHELL_THICKNESS
 *
 * Shell thickness integration.
 */
class KOO_API IntegrationShellThickness : public CloneableKeyword<IntegrationShellThickness, IntegrationKeyword> {
public:
    struct Data {
        int irid = 0;         // Integration rule ID
        int nip = 0;          // Number of integration points
        double t1 = 0.0;      // Thickness coordinate 1
        double wf1 = 0.0;     // Weight factor 1
        double t2 = 0.0;      // Thickness coordinate 2
        double wf2 = 0.0;     // Weight factor 2
    };

    IntegrationShellThickness() = default;

    std::string getKeywordName() const override { return "*INTEGRATION_SHELL_THICKNESS"; }

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
 * @brief *INTEGRATION_TSHELL
 *
 * Thick shell integration.
 */
class KOO_API IntegrationTshell : public CloneableKeyword<IntegrationTshell, IntegrationKeyword> {
public:
    struct Data {
        int irid = 0;         // Integration rule ID
        int nip = 0;          // Number of integration points
        int nt = 0;           // Number of through-thickness points
        int ni = 0;           // Number of in-plane points
    };

    IntegrationTshell() = default;

    std::string getKeywordName() const override { return "*INTEGRATION_TSHELL"; }

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
 * @brief *INTEGRATION_POINT
 *
 * User-defined integration point.
 */
class KOO_API IntegrationPoint : public CloneableKeyword<IntegrationPoint, IntegrationKeyword> {
public:
    struct Data {
        int irid = 0;         // Integration rule ID
        int ptid = 0;         // Point ID
        double xi = 0.0;      // XI coordinate
        double eta = 0.0;     // ETA coordinate
        double zeta = 0.0;    // ZETA coordinate
        double wgt = 1.0;     // Weight
    };

    IntegrationPoint() = default;

    std::string getKeywordName() const override { return "*INTEGRATION_POINT"; }

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
