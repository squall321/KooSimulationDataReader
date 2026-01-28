#pragma once

#include <koo/Export.hpp>
#include <koo/dyna/Keyword.hpp>
#include <koo/util/Types.hpp>
#include <vector>
#include <string>

namespace koo::dyna {

/**
 * @brief Base class for *HOURGLASS keywords
 */
class KOO_API HourglassKeyword : public Keyword {
public:
    virtual ~HourglassKeyword() = default;
};

/**
 * @brief *HOURGLASS
 *
 * Defines hourglass control for shell and solid elements.
 */
class KOO_API Hourglass : public CloneableKeyword<Hourglass, HourglassKeyword> {
public:
    struct Data {
        int hgid = 0;         // Hourglass ID
        int ihq = 1;          // Hourglass control type
        double qm = 0.1;      // Hourglass coefficient
        int ibq = 0;          // Bulk viscosity type
        double q1 = 1.5;      // Quadratic bulk viscosity
        double q2 = 0.06;     // Linear bulk viscosity
        double qb_vdc = 0.1;  // Hourglass coefficient for shell bend/dc
        double qw = 0.1;      // Hourglass coefficient for shell warp
    };

    Hourglass() = default;

    std::string getKeywordName() const override { return "*HOURGLASS"; }

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
 * @brief *HOURGLASS_TITLE
 *
 * Hourglass control with title.
 */
class KOO_API HourglassTitle : public CloneableKeyword<HourglassTitle, HourglassKeyword> {
public:
    HourglassTitle() = default;

    std::string getKeywordName() const override { return "*HOURGLASS_TITLE"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    const std::string& getTitle() const { return title_; }
    void setTitle(const std::string& title) { title_ = title; }

    Hourglass::Data& getData() { return data_; }
    const Hourglass::Data& getData() const { return data_; }

private:
    std::string title_;
    Hourglass::Data data_;
};

/**
 * @brief *HOURGLASS_SHELL
 *
 * Hourglass control specifically for shell elements.
 */
class KOO_API HourglassShell : public CloneableKeyword<HourglassShell, HourglassKeyword> {
public:
    struct Data {
        int hgid = 0;         // Hourglass ID
        int ihq = 1;          // Hourglass control type
        double qm = 0.1;      // Hourglass coefficient
        double qb = 0.1;      // Bending hourglass coefficient
        double qw = 0.1;      // Warping hourglass coefficient
    };

    HourglassShell() = default;

    std::string getKeywordName() const override { return "*HOURGLASS_SHELL"; }

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
 * @brief *HOURGLASS_SOLID
 *
 * Hourglass control specifically for solid elements.
 */
class KOO_API HourglassSolid : public CloneableKeyword<HourglassSolid, HourglassKeyword> {
public:
    struct Data {
        int hgid = 0;         // Hourglass ID
        int ihq = 1;          // Hourglass control type
        double qm = 0.1;      // Hourglass coefficient
        int ibq = 0;          // Bulk viscosity type
        double q1 = 1.5;      // Quadratic bulk viscosity
        double q2 = 0.06;     // Linear bulk viscosity
    };

    HourglassSolid() = default;

    std::string getKeywordName() const override { return "*HOURGLASS_SOLID"; }

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
 * @brief *HOURGLASS_ID
 *
 * Hourglass control with ID.
 */
class KOO_API HourglassId : public CloneableKeyword<HourglassId, HourglassKeyword> {
public:
    HourglassId() = default;

    std::string getKeywordName() const override { return "*HOURGLASS_ID"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    int getId() const { return id_; }
    void setId(int id) { id_ = id; }

    const std::string& getHeading() const { return heading_; }
    void setHeading(const std::string& heading) { heading_ = heading; }

    Hourglass::Data& getData() { return data_; }
    const Hourglass::Data& getData() const { return data_; }

private:
    int id_ = 0;
    std::string heading_;
    Hourglass::Data data_;
};

/**
 * @brief *HOURGLASS_THICKNESS_CHANGE
 *
 * Hourglass control for thickness change.
 */
class KOO_API HourglassThicknessChange : public CloneableKeyword<HourglassThicknessChange, HourglassKeyword> {
public:
    struct Data {
        int hgid = 0;         // Hourglass ID
        double thkchg = 0.0;  // Thickness change factor
        int istupd = 0;       // Stress update flag
        int ithkup = 0;       // Thickness update flag
    };

    HourglassThicknessChange() = default;

    std::string getKeywordName() const override { return "*HOURGLASS_THICKNESS_CHANGE"; }

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
 * @brief *HOURGLASS_PART
 *
 * Part-specific hourglass control.
 */
class KOO_API HourglassPart : public CloneableKeyword<HourglassPart, HourglassKeyword> {
public:
    struct Data {
        int pid = 0;          // Part ID
        int ihq = 1;          // Hourglass control type
        double qm = 0.1;      // Hourglass coefficient
        int ibq = 0;          // Bulk viscosity type
        double q1 = 1.5;      // Quadratic bulk viscosity
        double q2 = 0.06;     // Linear bulk viscosity
    };

    HourglassPart() = default;

    std::string getKeywordName() const override { return "*HOURGLASS_PART"; }

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
 * @brief *HOURGLASS_BEAM
 *
 * Hourglass control for beam elements.
 */
class KOO_API HourglassBeam : public CloneableKeyword<HourglassBeam, HourglassKeyword> {
public:
    struct Data {
        int hgid = 0;         // Hourglass ID
        int ihq = 1;          // Hourglass control type
        double qm = 0.1;      // Hourglass coefficient
        double qb = 0.1;      // Bending hourglass coefficient
        double qt = 0.1;      // Torsional hourglass coefficient
    };

    HourglassBeam() = default;

    std::string getKeywordName() const override { return "*HOURGLASS_BEAM"; }

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
