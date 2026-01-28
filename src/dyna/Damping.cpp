#include <koo/dyna/Damping.hpp>
#include <koo/dyna/ModelVisitor.hpp>
#include <koo/dyna/KeywordFactory.hpp>
#include <koo/util/StringUtils.hpp>

namespace koo::dyna {

// ============================================================================
// DampingGlobal
// ============================================================================

bool DampingGlobal::parse(const std::vector<std::string>& lines,
                           util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.valdmp = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.stx = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.sty = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.stz = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.srx = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.sry = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.srz = static_cast<int>(parser.getInt64At(pos).value_or(0));

    return true;
}

std::vector<std::string> DampingGlobal::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.lcid);
    writer.writeDouble(data_.valdmp);
    writer.writeInt(data_.stx);
    writer.writeInt(data_.sty);
    writer.writeInt(data_.stz);
    writer.writeInt(data_.srx);
    writer.writeInt(data_.sry);
    writer.writeInt(data_.srz);

    result.push_back(writer.getLine());
    return result;
}

void DampingGlobal::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DampingGlobal, "*DAMPING_GLOBAL")

// ============================================================================
// DampingPartMass
// ============================================================================

bool DampingPartMass::parse(const std::vector<std::string>& lines,
                             util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    entries_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        Entry entry;
        size_t pos = 0;

        entry.pid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        entry.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.sf = parser.getDoubleAt(pos, realW).value_or(1.0);
        pos += realW;
        entry.flag = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.stx = static_cast<int>(parser.getInt64At(pos).value_or(1));
        pos += intW;
        entry.sty = static_cast<int>(parser.getInt64At(pos).value_or(1));
        pos += intW;
        entry.stz = static_cast<int>(parser.getInt64At(pos).value_or(1));
        pos += intW;
        entry.srx = static_cast<int>(parser.getInt64At(pos).value_or(1));

        if (entry.pid != 0) {
            entries_.push_back(entry);
        }
    }

    return true;
}

std::vector<std::string> DampingPartMass::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& entry : entries_) {
        writer.clear();
        writer.writeInt(entry.pid);
        writer.writeInt(entry.lcid);
        writer.writeDouble(entry.sf);
        writer.writeInt(entry.flag);
        writer.writeInt(entry.stx);
        writer.writeInt(entry.sty);
        writer.writeInt(entry.stz);
        writer.writeInt(entry.srx);
        result.push_back(writer.getLine());
    }

    return result;
}

void DampingPartMass::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DampingPartMass, "*DAMPING_PART_MASS")

// ============================================================================
// DampingPartStiffness
// ============================================================================

bool DampingPartStiffness::parse(const std::vector<std::string>& lines,
                                  util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    entries_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        Entry entry;
        size_t pos = 0;

        entry.pid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        entry.coef = parser.getDoubleAt(pos, realW).value_or(0.0);

        if (entry.pid != 0) {
            entries_.push_back(entry);
        }
    }

    return true;
}

std::vector<std::string> DampingPartStiffness::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& entry : entries_) {
        writer.clear();
        writer.writeInt(entry.pid);
        writer.writeDouble(entry.coef);
        result.push_back(writer.getLine());
    }

    return result;
}

void DampingPartStiffness::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DampingPartStiffness, "*DAMPING_PART_STIFFNESS")

// ============================================================================
// DampingRelative
// ============================================================================

bool DampingRelative::parse(const std::vector<std::string>& lines,
                             util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: PID1, PID2, CDAMP
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.pid1 = parser.getInt64At(pos).value_or(0);
    pos += intW;
    data_.pid2 = parser.getInt64At(pos).value_or(0);
    pos += intW;
    data_.cdamp = static_cast<int>(parser.getInt64At(pos).value_or(0));
    ++lineIdx;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 2: DMPX, DMPY, DMPZ, DMPRX, DMPRY, DMPRZ
    parser.setLine(lines[lineIdx]);
    pos = 0;

    data_.dmpx = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.dmpy = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.dmpz = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.dmprx = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.dmpry = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.dmprz = parser.getDoubleAt(pos, realW).value_or(0.0);

    return true;
}

std::vector<std::string> DampingRelative::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    // Card 1
    writer.writeInt(data_.pid1);
    writer.writeInt(data_.pid2);
    writer.writeInt(data_.cdamp);
    result.push_back(writer.getLine());

    // Card 2
    writer.clear();
    writer.writeDouble(data_.dmpx);
    writer.writeDouble(data_.dmpy);
    writer.writeDouble(data_.dmpz);
    writer.writeDouble(data_.dmprx);
    writer.writeDouble(data_.dmpry);
    writer.writeDouble(data_.dmprz);
    result.push_back(writer.getLine());

    return result;
}

void DampingRelative::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DampingRelative, "*DAMPING_RELATIVE")

// ============================================================================
// DampingFrequencyRange
// ============================================================================

bool DampingFrequencyRange::parse(const std::vector<std::string>& lines,
                                   util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.cdamp = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.flow = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.fhigh = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.psid = static_cast<int>(parser.getInt64At(pos).value_or(0));

    return true;
}

std::vector<std::string> DampingFrequencyRange::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeDouble(data_.cdamp);
    writer.writeDouble(data_.flow);
    writer.writeDouble(data_.fhigh);
    writer.writeInt(data_.psid);

    result.push_back(writer.getLine());
    return result;
}

void DampingFrequencyRange::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DampingFrequencyRange, "*DAMPING_FREQUENCY_RANGE")

// ============================================================================
// DampingFrequencyRangeDeform
// ============================================================================

bool DampingFrequencyRangeDeform::parse(const std::vector<std::string>& lines,
                                         util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;

        data_.cdamp = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.flow = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.fhigh = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.psid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> DampingFrequencyRangeDeform::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeDouble(data_.cdamp);
    writer.writeDouble(data_.flow);
    writer.writeDouble(data_.fhigh);
    writer.writeInt(data_.psid);

    result.push_back(writer.getLine());
    return result;
}

void DampingFrequencyRangeDeform::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DampingFrequencyRangeDeform, "*DAMPING_FREQUENCY_RANGE_DEFORM")

// ============================================================================
// DampingFrequencyRangeRigid
// ============================================================================

bool DampingFrequencyRangeRigid::parse(const std::vector<std::string>& lines,
                                        util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;

        data_.cdamp = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.flow = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.fhigh = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.psid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> DampingFrequencyRangeRigid::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeDouble(data_.cdamp);
    writer.writeDouble(data_.flow);
    writer.writeDouble(data_.fhigh);
    writer.writeInt(data_.psid);

    result.push_back(writer.getLine());
    return result;
}

void DampingFrequencyRangeRigid::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DampingFrequencyRangeRigid, "*DAMPING_FREQUENCY_RANGE_RIGID")

// ============================================================================
// DampingPartMassSet
// ============================================================================

bool DampingPartMassSet::parse(const std::vector<std::string>& lines,
                                util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;

        data_.psid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.sf = parser.getDoubleAt(pos, realW).value_or(1.0);
        pos += realW;
        data_.flag = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.stx = static_cast<int>(parser.getInt64At(pos).value_or(1));
        pos += intW;
        data_.sty = static_cast<int>(parser.getInt64At(pos).value_or(1));
        pos += intW;
        data_.stz = static_cast<int>(parser.getInt64At(pos).value_or(1));
        pos += intW;
        data_.srx = static_cast<int>(parser.getInt64At(pos).value_or(1));
        break;
    }

    return true;
}

std::vector<std::string> DampingPartMassSet::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.psid);
    writer.writeInt(data_.lcid);
    writer.writeDouble(data_.sf);
    writer.writeInt(data_.flag);
    writer.writeInt(data_.stx);
    writer.writeInt(data_.sty);
    writer.writeInt(data_.stz);
    writer.writeInt(data_.srx);

    result.push_back(writer.getLine());
    return result;
}

void DampingPartMassSet::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DampingPartMassSet, "*DAMPING_PART_MASS_SET")

// ============================================================================
// DampingPartStiffnessSet
// ============================================================================

bool DampingPartStiffnessSet::parse(const std::vector<std::string>& lines,
                                     util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;

        data_.psid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.coef = parser.getDoubleAt(pos, realW).value_or(0.0);
        break;
    }

    return true;
}

std::vector<std::string> DampingPartStiffnessSet::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.psid);
    writer.writeDouble(data_.coef);

    result.push_back(writer.getLine());
    return result;
}

void DampingPartStiffnessSet::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DampingPartStiffnessSet, "*DAMPING_PART_STIFFNESS_SET")

// ============================================================================
// DampingModal
// ============================================================================

bool DampingModal::parse(const std::vector<std::string>& lines,
                          util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.mid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.damp = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.freq = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.psid = static_cast<int>(parser.getInt64At(pos).value_or(0));

    return true;
}

std::vector<std::string> DampingModal::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.mid);
    writer.writeDouble(data_.damp);
    writer.writeDouble(data_.freq);
    writer.writeInt(data_.psid);
    result.push_back(writer.getLine());

    return result;
}

void DampingModal::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DampingModal, "*DAMPING_MODAL")

// ============================================================================
// DampingStructural
// ============================================================================

bool DampingStructural::parse(const std::vector<std::string>& lines,
                               util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.psid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.geta = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.sf = parser.getDoubleAt(pos, realW).value_or(1.0);

    return true;
}

std::vector<std::string> DampingStructural::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.psid);
    writer.writeDouble(data_.geta);
    writer.writeInt(data_.lcid);
    writer.writeDouble(data_.sf);
    result.push_back(writer.getLine());

    return result;
}

void DampingStructural::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DampingStructural, "*DAMPING_STRUCTURAL")

} // namespace koo::dyna
