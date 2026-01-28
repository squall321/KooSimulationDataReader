#pragma once

#include <koo/Export.hpp>
#include <koo/dyna/Keyword.hpp>
#include <koo/util/Types.hpp>
#include <vector>
#include <string>

namespace koo::dyna {

/**
 * @brief Base class for *DEFORMABLE_TO_RIGID keywords
 */
class KOO_API DeformableToRigidKeyword : public Keyword {
public:
    virtual ~DeformableToRigidKeyword() = default;
};

/**
 * @brief *DEFORMABLE_TO_RIGID
 *
 * Switches parts from deformable to rigid during analysis.
 */
class KOO_API DeformableToRigid : public CloneableKeyword<DeformableToRigid, DeformableToRigidKeyword> {
public:
    struct Data {
        int pid = 0;          // Part ID
        int ptype = 0;        // Part set type (0=part, 1=part set)
        int cid = 0;          // Coordinate system ID
        double tc = 0.0;      // Time to switch to rigid
        double trc = 0.0;     // Time to switch back to deformable
        int opt = 0;          // Option (0=switch, 1=instant)
    };

    DeformableToRigid() = default;

    std::string getKeywordName() const override { return "*DEFORMABLE_TO_RIGID"; }

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
 * @brief *DEFORMABLE_TO_RIGID_AUTOMATIC
 *
 * Automatic switching based on criteria.
 */
class KOO_API DeformableToRigidAutomatic : public CloneableKeyword<DeformableToRigidAutomatic, DeformableToRigidKeyword> {
public:
    struct Data {
        int pid = 0;          // Part ID
        int ptype = 0;        // Part set type
        int cid = 0;          // Coordinate system ID
        double mtc = 0.0;     // Metric threshold for switching
        int mto = 0;          // Metric option
        double tc = 0.0;      // Time to activate
        double trc = 0.0;     // Time to deactivate
    };

    DeformableToRigidAutomatic() = default;

    std::string getKeywordName() const override { return "*DEFORMABLE_TO_RIGID_AUTOMATIC"; }

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
 * @brief *DEFORMABLE_TO_RIGID_INERTIA
 *
 * Switch with inertia properties preservation.
 */
class KOO_API DeformableToRigidInertia : public CloneableKeyword<DeformableToRigidInertia, DeformableToRigidKeyword> {
public:
    struct Data {
        int pid = 0;          // Part ID
        int ptype = 0;        // Part set type
        int cid = 0;          // Coordinate system ID
        double tm = 0.0;      // Translational mass
        double ircs = 0.0;    // Inertia about R-axis
        double itcs = 0.0;    // Inertia about T-axis
    };

    DeformableToRigidInertia() = default;

    std::string getKeywordName() const override { return "*DEFORMABLE_TO_RIGID_INERTIA"; }

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
 * @brief *DEFORMABLE_TO_RIGID_TITLE
 *
 * Deformable to rigid with title.
 */
class KOO_API DeformableToRigidTitle : public CloneableKeyword<DeformableToRigidTitle, DeformableToRigidKeyword> {
public:
    struct Data {
        std::string title;
        int pid = 0;
        int ptype = 0;
        int cid = 0;
        double tc = 0.0;
        double trc = 0.0;
        int opt = 0;
    };

    DeformableToRigidTitle() = default;

    std::string getKeywordName() const override { return "*DEFORMABLE_TO_RIGID_TITLE"; }

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
 * @brief *RIGID_TO_DEFORMABLE
 *
 * Switches parts from rigid back to deformable.
 */
class KOO_API RigidToDeformable : public CloneableKeyword<RigidToDeformable, DeformableToRigidKeyword> {
public:
    struct Data {
        int pid = 0;          // Part ID
        int ptype = 0;        // Part set type
        double tc = 0.0;      // Time to switch
        int opt = 0;          // Option
    };

    RigidToDeformable() = default;

    std::string getKeywordName() const override { return "*RIGID_TO_DEFORMABLE"; }

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
 * @brief *DEFORMABLE_TO_RIGID_AUTOMATIC_ID
 *
 * Automatic switching with ID specification.
 */
class KOO_API DeformableToRigidAutomaticId : public CloneableKeyword<DeformableToRigidAutomaticId, DeformableToRigidKeyword> {
public:
    struct Data {
        int id = 0;           // ID
        int pid = 0;          // Part ID
        int ptype = 0;        // Part set type
        int cid = 0;          // Coordinate system ID
        double mtc = 0.0;     // Metric threshold
        int mto = 0;          // Metric option
        double tc = 0.0;      // Time to activate
        double trc = 0.0;     // Time to deactivate
    };

    DeformableToRigidAutomaticId() = default;

    std::string getKeywordName() const override { return "*DEFORMABLE_TO_RIGID_AUTOMATIC_ID"; }

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
