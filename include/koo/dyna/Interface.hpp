#pragma once

#include <koo/Export.hpp>
#include <koo/dyna/Keyword.hpp>
#include <koo/util/Types.hpp>
#include <vector>
#include <string>

namespace koo::dyna {

/**
 * @brief Base class for *INTERFACE keywords
 */
class KOO_API InterfaceKeyword : public Keyword {
public:
    virtual ~InterfaceKeyword() = default;
};

/**
 * @brief *INTERFACE_LINKING_DISCRETE
 *
 * Discrete element coupling interface.
 */
class KOO_API InterfaceLinkingDiscrete : public CloneableKeyword<InterfaceLinkingDiscrete, InterfaceKeyword> {
public:
    struct Data {
        int ifid = 0;         // Interface ID
        int slavset = 0;      // Slave node set
        int mastset = 0;      // Master node set
        int ctype = 0;        // Coupling type
        double stiff = 0.0;   // Coupling stiffness
        double damp = 0.0;    // Coupling damping
    };

    InterfaceLinkingDiscrete() = default;

    std::string getKeywordName() const override { return "*INTERFACE_LINKING_DISCRETE"; }

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
 * @brief *INTERFACE_SPRINGBACK_SEAMLESS
 *
 * Seamless springback interface.
 */
class KOO_API InterfaceSpringbackSeamless : public CloneableKeyword<InterfaceSpringbackSeamless, InterfaceKeyword> {
public:
    struct Data {
        int psid = 0;         // Part set ID
        int ptype = 0;        // Part set type
        int ftype = 0;        // Forming type
        int nshv = 0;         // Number of shell history vars
    };

    InterfaceSpringbackSeamless() = default;

    std::string getKeywordName() const override { return "*INTERFACE_SPRINGBACK_SEAMLESS"; }

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
 * @brief *INTERFACE_COMPENSATION_NEW
 *
 * Die compensation interface.
 */
class KOO_API InterfaceCompensationNew : public CloneableKeyword<InterfaceCompensationNew, InterfaceKeyword> {
public:
    struct Data {
        int psid = 0;         // Part set ID
        int method = 0;       // Compensation method
        double scale = 1.0;   // Scale factor
        int niter = 1;        // Number of iterations
        double tol = 0.0;     // Tolerance
    };

    InterfaceCompensationNew() = default;

    std::string getKeywordName() const override { return "*INTERFACE_COMPENSATION_NEW"; }

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
 * @brief *INTERFACE_BLANKSIZE
 *
 * Interface for blank size determination.
 */
class KOO_API InterfaceBlanksize : public CloneableKeyword<InterfaceBlanksize, InterfaceKeyword> {
public:
    struct Data {
        int id = 0;           // Interface ID
        int psid = 0;         // Part set ID
        int method = 0;       // Method for blank size
        double scale = 1.0;   // Scale factor
        int niter = 1;        // Number of iterations
    };

    InterfaceBlanksize() = default;

    std::string getKeywordName() const override { return "*INTERFACE_BLANKSIZE"; }

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
 * @brief *INTERFACE_COMPONENT
 *
 * Interface component definition.
 */
class KOO_API InterfaceComponent : public CloneableKeyword<InterfaceComponent, InterfaceKeyword> {
public:
    struct Data {
        int id = 0;           // Interface ID
        int compid = 0;       // Component ID
        int type = 0;         // Component type
        int pid = 0;          // Part ID
        double thick = 0.0;   // Thickness
    };

    InterfaceComponent() = default;

    std::string getKeywordName() const override { return "*INTERFACE_COMPONENT"; }

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
 * @brief *INTERFACE_SPRINGBACK_LSDYNA
 *
 * LS-DYNA springback interface.
 */
class KOO_API InterfaceSpringbackLsdyna : public CloneableKeyword<InterfaceSpringbackLsdyna, InterfaceKeyword> {
public:
    struct Data {
        int psid = 0;         // Part set ID
        int ptype = 0;        // Part set type
        int thickflag = 0;    // Thickness flag
        int strsflag = 0;     // Stress flag
        int histflag = 0;     // History variable flag
    };

    InterfaceSpringbackLsdyna() = default;

    std::string getKeywordName() const override { return "*INTERFACE_SPRINGBACK_LSDYNA"; }

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
 * @brief *INTERFACE_SSI
 *
 * Soil-structure interaction interface.
 */
class KOO_API InterfaceSsi : public CloneableKeyword<InterfaceSsi, InterfaceKeyword> {
public:
    struct Data {
        int id = 0;           // Interface ID
        int ssid = 0;         // Slave segment set ID
        int msid = 0;         // Master segment set ID
        int fric = 0;         // Friction flag
        double fs = 0.0;      // Static friction coefficient
        double fd = 0.0;      // Dynamic friction coefficient
    };

    InterfaceSsi() = default;

    std::string getKeywordName() const override { return "*INTERFACE_SSI"; }

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
 * @brief *INTERFACE_LINKING
 *
 * General interface linking.
 */
class KOO_API InterfaceLinking : public CloneableKeyword<InterfaceLinking, InterfaceKeyword> {
public:
    struct Data {
        int id = 0;           // Interface ID
        int slave = 0;        // Slave set
        int master = 0;       // Master set
        int stype = 0;        // Slave set type
        int mtype = 0;        // Master set type
        double pen = 0.0;     // Penalty scale
    };

    InterfaceLinking() = default;

    std::string getKeywordName() const override { return "*INTERFACE_LINKING"; }

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
 * @brief *INTERFACE_FRICTIONAL_SLIDELINE
 *
 * Frictional slideline interface.
 */
class KOO_API InterfaceFrictionalSlideline : public CloneableKeyword<InterfaceFrictionalSlideline, InterfaceKeyword> {
public:
    struct Data {
        int id = 0;           // Interface ID
        int ssid = 0;         // Slave segment set
        int msid = 0;         // Master segment set
        double fs = 0.0;      // Static friction coefficient
        double fd = 0.0;      // Dynamic friction coefficient
        double dc = 0.0;      // Exponential decay coefficient
    };

    InterfaceFrictionalSlideline() = default;

    std::string getKeywordName() const override { return "*INTERFACE_FRICTIONAL_SLIDELINE"; }

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
