#pragma once

#include <koo/Export.hpp>
#include <koo/dyna/Keyword.hpp>
#include <koo/util/Types.hpp>
#include <vector>

namespace koo::dyna {

/**
 * @brief Base class for *DAMPING keywords
 */
class KOO_API DampingKeyword : public Keyword {
public:
    virtual ~DampingKeyword() = default;
};

/**
 * @brief *DAMPING_GLOBAL
 *
 * Defines global system damping.
 */
class KOO_API DampingGlobal : public CloneableKeyword<DampingGlobal, DampingKeyword> {
public:
    struct Data {
        int lcid = 0;           // Load curve ID for damping
        double valdmp = 0.0;    // Constant damping value
        int stx = 0;            // Flag for x-translation
        int sty = 0;            // Flag for y-translation
        int stz = 0;            // Flag for z-translation
        int srx = 0;            // Flag for x-rotation
        int sry = 0;            // Flag for y-rotation
        int srz = 0;            // Flag for z-rotation
    };

    DampingGlobal() = default;

    std::string getKeywordName() const override { return "*DAMPING_GLOBAL"; }

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
 * @brief *DAMPING_PART_MASS
 *
 * Defines mass-weighted damping for a part.
 */
class KOO_API DampingPartMass : public CloneableKeyword<DampingPartMass, DampingKeyword> {
public:
    struct Entry {
        PartId pid = 0;         // Part ID
        int lcid = 0;           // Load curve ID
        double sf = 1.0;        // Scale factor
        int flag = 0;           // Damping type flag
        int stx = 1;
        int sty = 1;
        int stz = 1;
        int srx = 1;
        int sry = 1;
        int srz = 1;
    };

    DampingPartMass() = default;

    std::string getKeywordName() const override { return "*DAMPING_PART_MASS"; }

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
 * @brief *DAMPING_PART_STIFFNESS
 *
 * Defines stiffness-weighted damping for a part.
 */
class KOO_API DampingPartStiffness : public CloneableKeyword<DampingPartStiffness, DampingKeyword> {
public:
    struct Entry {
        PartId pid = 0;         // Part ID
        double coef = 0.0;      // Damping coefficient
    };

    DampingPartStiffness() = default;

    std::string getKeywordName() const override { return "*DAMPING_PART_STIFFNESS"; }

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
 * @brief *DAMPING_RELATIVE
 *
 * Defines relative damping between parts.
 */
class KOO_API DampingRelative : public CloneableKeyword<DampingRelative, DampingKeyword> {
public:
    struct Data {
        PartId pid1 = 0;        // Part 1 ID
        PartId pid2 = 0;        // Part 2 ID
        int cdamp = 0;          // Coordinate system for damping
        double dmpx = 0.0;      // Damping in x
        double dmpy = 0.0;      // Damping in y
        double dmpz = 0.0;      // Damping in z
        double dmprx = 0.0;     // Rotational damping in x
        double dmpry = 0.0;     // Rotational damping in y
        double dmprz = 0.0;     // Rotational damping in z
    };

    DampingRelative() = default;

    std::string getKeywordName() const override { return "*DAMPING_RELATIVE"; }

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
 * @brief *DAMPING_FREQUENCY_RANGE
 *
 * Defines frequency-dependent damping.
 */
class KOO_API DampingFrequencyRange : public CloneableKeyword<DampingFrequencyRange, DampingKeyword> {
public:
    struct Data {
        double cdamp = 0.0;     // Critical damping ratio
        double flow = 0.0;      // Lower frequency bound
        double fhigh = 0.0;     // Upper frequency bound
        int psid = 0;           // Part set ID
    };

    DampingFrequencyRange() = default;

    std::string getKeywordName() const override { return "*DAMPING_FREQUENCY_RANGE"; }

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
 * @brief *DAMPING_FREQUENCY_RANGE_DEFORM
 *
 * Defines frequency-dependent damping for deformable parts.
 */
class KOO_API DampingFrequencyRangeDeform : public CloneableKeyword<DampingFrequencyRangeDeform, DampingKeyword> {
public:
    struct Data {
        double cdamp = 0.0;     // Critical damping ratio
        double flow = 0.0;      // Lower frequency bound
        double fhigh = 0.0;     // Upper frequency bound
        int psid = 0;           // Part set ID
    };

    DampingFrequencyRangeDeform() = default;

    std::string getKeywordName() const override { return "*DAMPING_FREQUENCY_RANGE_DEFORM"; }

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
 * @brief *DAMPING_FREQUENCY_RANGE_RIGID
 *
 * Defines frequency-dependent damping for rigid bodies.
 */
class KOO_API DampingFrequencyRangeRigid : public CloneableKeyword<DampingFrequencyRangeRigid, DampingKeyword> {
public:
    struct Data {
        double cdamp = 0.0;     // Critical damping ratio
        double flow = 0.0;      // Lower frequency bound
        double fhigh = 0.0;     // Upper frequency bound
        int psid = 0;           // Part set ID
    };

    DampingFrequencyRangeRigid() = default;

    std::string getKeywordName() const override { return "*DAMPING_FREQUENCY_RANGE_RIGID"; }

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
 * @brief *DAMPING_PART_MASS_SET
 *
 * Defines mass-weighted damping for a part set.
 */
class KOO_API DampingPartMassSet : public CloneableKeyword<DampingPartMassSet, DampingKeyword> {
public:
    struct Data {
        int psid = 0;           // Part set ID
        int lcid = 0;           // Load curve ID
        double sf = 1.0;        // Scale factor
        int flag = 0;           // Damping type flag
        int stx = 1;
        int sty = 1;
        int stz = 1;
        int srx = 1;
    };

    DampingPartMassSet() = default;

    std::string getKeywordName() const override { return "*DAMPING_PART_MASS_SET"; }

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
 * @brief *DAMPING_PART_STIFFNESS_SET
 *
 * Defines stiffness-weighted damping for a part set.
 */
class KOO_API DampingPartStiffnessSet : public CloneableKeyword<DampingPartStiffnessSet, DampingKeyword> {
public:
    struct Data {
        int psid = 0;           // Part set ID
        double coef = 0.0;      // Damping coefficient
    };

    DampingPartStiffnessSet() = default;

    std::string getKeywordName() const override { return "*DAMPING_PART_STIFFNESS_SET"; }

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
 * @brief *DAMPING_MODAL
 *
 * Defines modal damping for specific modes.
 */
class KOO_API DampingModal : public CloneableKeyword<DampingModal, DampingKeyword> {
public:
    struct Data {
        int mid = 0;            // Mode ID
        double damp = 0.0;      // Damping ratio
        double freq = 0.0;      // Frequency
        int psid = 0;           // Part set ID
    };

    DampingModal() = default;

    std::string getKeywordName() const override { return "*DAMPING_MODAL"; }

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
 * @brief *DAMPING_STRUCTURAL
 *
 * Defines structural damping properties.
 */
class KOO_API DampingStructural : public CloneableKeyword<DampingStructural, DampingKeyword> {
public:
    struct Data {
        int psid = 0;           // Part set ID
        double geta = 0.0;      // Structural damping coefficient
        int lcid = 0;           // Load curve ID
        double sf = 1.0;        // Scale factor
    };

    DampingStructural() = default;

    std::string getKeywordName() const override { return "*DAMPING_STRUCTURAL"; }

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
