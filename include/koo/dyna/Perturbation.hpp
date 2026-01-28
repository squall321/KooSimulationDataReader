#pragma once

#include <koo/Export.hpp>
#include <koo/dyna/Keyword.hpp>
#include <koo/util/Types.hpp>
#include <vector>
#include <string>

namespace koo::dyna {

/**
 * @brief Base class for *PERTURBATION keywords
 */
class KOO_API PerturbationKeyword : public Keyword {
public:
    virtual ~PerturbationKeyword() = default;
};

/**
 * @brief *PERTURBATION_MAT
 *
 * Material perturbation for sensitivity analysis.
 */
class KOO_API PerturbationMat : public CloneableKeyword<PerturbationMat, PerturbationKeyword> {
public:
    struct Data {
        int mid = 0;          // Material ID
        int pidx = 0;         // Parameter index
        double dval = 0.0;    // Delta value
        int mtype = 0;        // Material type
    };

    PerturbationMat() = default;

    std::string getKeywordName() const override { return "*PERTURBATION_MAT"; }

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
 * @brief *PERTURBATION_NODE
 *
 * Node perturbation for shape sensitivity.
 */
class KOO_API PerturbationNode : public CloneableKeyword<PerturbationNode, PerturbationKeyword> {
public:
    struct Entry {
        int nid = 0;          // Node ID
        double dx = 0.0;      // X perturbation
        double dy = 0.0;      // Y perturbation
        double dz = 0.0;      // Z perturbation
    };

    PerturbationNode() = default;

    std::string getKeywordName() const override { return "*PERTURBATION_NODE"; }

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
 * @brief *PERTURBATION_SECTION
 *
 * Section perturbation.
 */
class KOO_API PerturbationSection : public CloneableKeyword<PerturbationSection, PerturbationKeyword> {
public:
    struct Data {
        int secid = 0;        // Section ID
        int pidx = 0;         // Parameter index
        double dval = 0.0;    // Delta value
        int stype = 0;        // Section type
    };

    PerturbationSection() = default;

    std::string getKeywordName() const override { return "*PERTURBATION_SECTION"; }

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
 * @brief *PERTURBATION_PART
 *
 * Part perturbation for sensitivity analysis.
 */
class KOO_API PerturbationPart : public CloneableKeyword<PerturbationPart, PerturbationKeyword> {
public:
    struct Data {
        int pid = 0;          // Part ID
        int pidx = 0;         // Parameter index
        double dval = 0.0;    // Delta value
        int ptype = 0;        // Part type
    };

    PerturbationPart() = default;

    std::string getKeywordName() const override { return "*PERTURBATION_PART"; }

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
 * @brief *PERTURBATION_LOAD
 *
 * Load perturbation.
 */
class KOO_API PerturbationLoad : public CloneableKeyword<PerturbationLoad, PerturbationKeyword> {
public:
    struct Data {
        int lcid = 0;         // Load curve ID
        int ltype = 0;        // Load type
        double dval = 0.0;    // Delta value
        int pidx = 0;         // Parameter index
    };

    PerturbationLoad() = default;

    std::string getKeywordName() const override { return "*PERTURBATION_LOAD"; }

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
 * @brief *PERTURBATION_CONTACT
 *
 * Contact perturbation.
 */
class KOO_API PerturbationContact : public CloneableKeyword<PerturbationContact, PerturbationKeyword> {
public:
    struct Data {
        int cid = 0;          // Contact ID
        int pidx = 0;         // Parameter index
        double dval = 0.0;    // Delta value
        int ctype = 0;        // Contact type
    };

    PerturbationContact() = default;

    std::string getKeywordName() const override { return "*PERTURBATION_CONTACT"; }

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
 * @brief *PERTURBATION_THICKNESS
 *
 * Thickness perturbation for shell elements.
 */
class KOO_API PerturbationThickness : public CloneableKeyword<PerturbationThickness, PerturbationKeyword> {
public:
    struct Data {
        int psid = 0;         // Part set ID
        double dthk = 0.0;    // Delta thickness
        int ptype = 0;        // Perturbation type
        double scale = 1.0;   // Scale factor
    };

    PerturbationThickness() = default;

    std::string getKeywordName() const override { return "*PERTURBATION_THICKNESS"; }

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
 * @brief *PERTURBATION_ELEMENT
 *
 * Element perturbation for sensitivity analysis.
 */
class KOO_API PerturbationElement : public CloneableKeyword<PerturbationElement, PerturbationKeyword> {
public:
    struct Data {
        int eid = 0;          // Element ID
        int pidx = 0;         // Parameter index
        double dval = 0.0;    // Delta value
        int etype = 0;        // Element type
    };

    PerturbationElement() = default;

    std::string getKeywordName() const override { return "*PERTURBATION_ELEMENT"; }

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
 * @brief *PERTURBATION_GEOMETRY
 *
 * Geometry perturbation for shape sensitivity.
 */
class KOO_API PerturbationGeometry : public CloneableKeyword<PerturbationGeometry, PerturbationKeyword> {
public:
    struct Data {
        int gid = 0;          // Geometry ID
        int gtype = 0;        // Geometry type
        double dx = 0.0;      // X perturbation
        double dy = 0.0;      // Y perturbation
        double dz = 0.0;      // Z perturbation
    };

    PerturbationGeometry() = default;

    std::string getKeywordName() const override { return "*PERTURBATION_GEOMETRY"; }

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
