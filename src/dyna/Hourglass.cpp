#include <koo/dyna/Hourglass.hpp>
#include <koo/dyna/ModelVisitor.hpp>
#include <koo/dyna/KeywordFactory.hpp>
#include <koo/util/StringUtils.hpp>

namespace koo::dyna {

// ============================================================================
// Hourglass
// ============================================================================

bool Hourglass::parse(const std::vector<std::string>& lines,
                       util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: HGID, IHQ, QM, IBQ, Q1, Q2, QB/VDC, QW
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.hgid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.ihq = static_cast<int>(parser.getInt64At(pos).value_or(1));
    pos += intW;
    data_.qm = parser.getDoubleAt(pos, realW).value_or(0.1);
    pos += realW;
    data_.ibq = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.q1 = parser.getDoubleAt(pos, realW).value_or(1.5);
    pos += realW;
    data_.q2 = parser.getDoubleAt(pos, realW).value_or(0.06);
    pos += realW;
    data_.qb_vdc = parser.getDoubleAt(pos, realW).value_or(0.1);
    pos += realW;
    data_.qw = parser.getDoubleAt(pos, realW).value_or(0.1);

    return true;
}

std::vector<std::string> Hourglass::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.hgid);
    writer.writeInt(data_.ihq);
    writer.writeDouble(data_.qm);
    writer.writeInt(data_.ibq);
    writer.writeDouble(data_.q1);
    writer.writeDouble(data_.q2);
    writer.writeDouble(data_.qb_vdc);
    writer.writeDouble(data_.qw);
    result.push_back(writer.getLine());

    return result;
}

void Hourglass::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(Hourglass, "*HOURGLASS")

// ============================================================================
// HourglassTitle
// ============================================================================

bool HourglassTitle::parse(const std::vector<std::string>& lines,
                            util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    // First non-comment line is the title
    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    title_ = util::StringUtils::trim(lines[lineIdx]);
    ++lineIdx;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: HGID, IHQ, QM, IBQ, Q1, Q2, QB/VDC, QW
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.hgid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.ihq = static_cast<int>(parser.getInt64At(pos).value_or(1));
    pos += intW;
    data_.qm = parser.getDoubleAt(pos, realW).value_or(0.1);
    pos += realW;
    data_.ibq = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.q1 = parser.getDoubleAt(pos, realW).value_or(1.5);
    pos += realW;
    data_.q2 = parser.getDoubleAt(pos, realW).value_or(0.06);
    pos += realW;
    data_.qb_vdc = parser.getDoubleAt(pos, realW).value_or(0.1);
    pos += realW;
    data_.qw = parser.getDoubleAt(pos, realW).value_or(0.1);

    return true;
}

std::vector<std::string> HourglassTitle::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    result.push_back(title_);

    writer.writeInt(data_.hgid);
    writer.writeInt(data_.ihq);
    writer.writeDouble(data_.qm);
    writer.writeInt(data_.ibq);
    writer.writeDouble(data_.q1);
    writer.writeDouble(data_.q2);
    writer.writeDouble(data_.qb_vdc);
    writer.writeDouble(data_.qw);
    result.push_back(writer.getLine());

    return result;
}

void HourglassTitle::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(HourglassTitle, "*HOURGLASS_TITLE")

// ============================================================================
// HourglassShell
// ============================================================================

bool HourglassShell::parse(const std::vector<std::string>& lines,
                            util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;

        data_.hgid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.ihq = static_cast<int>(parser.getInt64At(pos).value_or(1));
        pos += intW;
        data_.qm = parser.getDoubleAt(pos, realW).value_or(0.1);
        pos += realW;
        data_.qb = parser.getDoubleAt(pos, realW).value_or(0.1);
        pos += realW;
        data_.qw = parser.getDoubleAt(pos, realW).value_or(0.1);
        break;
    }

    return true;
}

std::vector<std::string> HourglassShell::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.hgid);
    writer.writeInt(data_.ihq);
    writer.writeDouble(data_.qm);
    writer.writeDouble(data_.qb);
    writer.writeDouble(data_.qw);
    result.push_back(writer.getLine());

    return result;
}

void HourglassShell::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(HourglassShell, "*HOURGLASS_SHELL")

// ============================================================================
// HourglassSolid
// ============================================================================

bool HourglassSolid::parse(const std::vector<std::string>& lines,
                            util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;

        data_.hgid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.ihq = static_cast<int>(parser.getInt64At(pos).value_or(1));
        pos += intW;
        data_.qm = parser.getDoubleAt(pos, realW).value_or(0.1);
        pos += realW;
        data_.ibq = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.q1 = parser.getDoubleAt(pos, realW).value_or(1.5);
        pos += realW;
        data_.q2 = parser.getDoubleAt(pos, realW).value_or(0.06);
        break;
    }

    return true;
}

std::vector<std::string> HourglassSolid::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.hgid);
    writer.writeInt(data_.ihq);
    writer.writeDouble(data_.qm);
    writer.writeInt(data_.ibq);
    writer.writeDouble(data_.q1);
    writer.writeDouble(data_.q2);
    result.push_back(writer.getLine());

    return result;
}

void HourglassSolid::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(HourglassSolid, "*HOURGLASS_SOLID")

// ============================================================================
// HourglassId
// ============================================================================

bool HourglassId::parse(const std::vector<std::string>& lines,
                         util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    // First non-comment line is the ID and heading
    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: ID, HEADING
    parser.setLine(lines[lineIdx]);
    id_ = static_cast<int>(parser.getInt64At(0).value_or(0));
    if (lines[lineIdx].size() > intW) {
        heading_ = util::StringUtils::trim(lines[lineIdx].substr(intW));
    }
    ++lineIdx;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 2: HGID, IHQ, QM, IBQ, Q1, Q2, QB/VDC, QW
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.hgid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.ihq = static_cast<int>(parser.getInt64At(pos).value_or(1));
    pos += intW;
    data_.qm = parser.getDoubleAt(pos, realW).value_or(0.1);
    pos += realW;
    data_.ibq = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.q1 = parser.getDoubleAt(pos, realW).value_or(1.5);
    pos += realW;
    data_.q2 = parser.getDoubleAt(pos, realW).value_or(0.06);
    pos += realW;
    data_.qb_vdc = parser.getDoubleAt(pos, realW).value_or(0.1);
    pos += realW;
    data_.qw = parser.getDoubleAt(pos, realW).value_or(0.1);

    return true;
}

std::vector<std::string> HourglassId::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(id_);
    std::string line1 = writer.getLine();
    if (!heading_.empty()) {
        line1 += heading_;
    }
    result.push_back(line1);

    util::CardWriter writer2(format);
    writer2.writeInt(data_.hgid);
    writer2.writeInt(data_.ihq);
    writer2.writeDouble(data_.qm);
    writer2.writeInt(data_.ibq);
    writer2.writeDouble(data_.q1);
    writer2.writeDouble(data_.q2);
    writer2.writeDouble(data_.qb_vdc);
    writer2.writeDouble(data_.qw);
    result.push_back(writer2.getLine());

    return result;
}

void HourglassId::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(HourglassId, "*HOURGLASS_ID")

// ============================================================================
// HourglassThicknessChange
// ============================================================================

bool HourglassThicknessChange::parse(const std::vector<std::string>& lines,
                                      util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: HGID, THKCHG, ISTUPD, ITHKUP
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.hgid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.thkchg = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.istupd = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.ithkup = static_cast<int>(parser.getInt64At(pos).value_or(0));

    return true;
}

std::vector<std::string> HourglassThicknessChange::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.hgid);
    writer.writeDouble(data_.thkchg);
    writer.writeInt(data_.istupd);
    writer.writeInt(data_.ithkup);
    result.push_back(writer.getLine());

    return result;
}

void HourglassThicknessChange::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(HourglassThicknessChange, "*HOURGLASS_THICKNESS_CHANGE")

// ============================================================================
// HourglassPart
// ============================================================================

bool HourglassPart::parse(const std::vector<std::string>& lines,
                           util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: PID, IHQ, QM, IBQ, Q1, Q2
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.pid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.ihq = static_cast<int>(parser.getInt64At(pos).value_or(1));
    pos += intW;
    data_.qm = parser.getDoubleAt(pos, realW).value_or(0.1);
    pos += realW;
    data_.ibq = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.q1 = parser.getDoubleAt(pos, realW).value_or(1.5);
    pos += realW;
    data_.q2 = parser.getDoubleAt(pos, realW).value_or(0.06);

    return true;
}

std::vector<std::string> HourglassPart::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.pid);
    writer.writeInt(data_.ihq);
    writer.writeDouble(data_.qm);
    writer.writeInt(data_.ibq);
    writer.writeDouble(data_.q1);
    writer.writeDouble(data_.q2);
    result.push_back(writer.getLine());

    return result;
}

void HourglassPart::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(HourglassPart, "*HOURGLASS_PART")

// ============================================================================
// HourglassBeam
// ============================================================================

bool HourglassBeam::parse(const std::vector<std::string>& lines,
                           util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: HGID, IHQ, QM, QB, QT
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.hgid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.ihq = static_cast<int>(parser.getInt64At(pos).value_or(1));
    pos += intW;
    data_.qm = parser.getDoubleAt(pos, realW).value_or(0.1);
    pos += realW;
    data_.qb = parser.getDoubleAt(pos, realW).value_or(0.1);
    pos += realW;
    data_.qt = parser.getDoubleAt(pos, realW).value_or(0.1);

    return true;
}

std::vector<std::string> HourglassBeam::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.hgid);
    writer.writeInt(data_.ihq);
    writer.writeDouble(data_.qm);
    writer.writeDouble(data_.qb);
    writer.writeDouble(data_.qt);
    result.push_back(writer.getLine());

    return result;
}

void HourglassBeam::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(HourglassBeam, "*HOURGLASS_BEAM")

} // namespace koo::dyna
