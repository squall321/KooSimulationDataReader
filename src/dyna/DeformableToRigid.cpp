#include <koo/dyna/DeformableToRigid.hpp>
#include <koo/dyna/ModelVisitor.hpp>
#include <koo/dyna/KeywordFactory.hpp>
#include <koo/util/StringUtils.hpp>

namespace koo::dyna {

// ============================================================================
// DeformableToRigid
// ============================================================================

bool DeformableToRigid::parse(const std::vector<std::string>& lines,
                               util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: PID, PTYPE, CID, TC, TRC, OPT
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.pid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.ptype = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.cid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.tc = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.trc = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.opt = static_cast<int>(parser.getInt64At(pos).value_or(0));

    return true;
}

std::vector<std::string> DeformableToRigid::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.pid);
    writer.writeInt(data_.ptype);
    writer.writeInt(data_.cid);
    writer.writeDouble(data_.tc);
    writer.writeDouble(data_.trc);
    writer.writeInt(data_.opt);
    result.push_back(writer.getLine());

    return result;
}

void DeformableToRigid::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DeformableToRigid, "*DEFORMABLE_TO_RIGID")

// ============================================================================
// DeformableToRigidAutomatic
// ============================================================================

bool DeformableToRigidAutomatic::parse(const std::vector<std::string>& lines,
                                        util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: PID, PTYPE, CID, MTC, MTO, TC, TRC
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.pid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.ptype = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.cid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.mtc = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.mto = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.tc = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.trc = parser.getDoubleAt(pos, realW).value_or(0.0);

    return true;
}

std::vector<std::string> DeformableToRigidAutomatic::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.pid);
    writer.writeInt(data_.ptype);
    writer.writeInt(data_.cid);
    writer.writeDouble(data_.mtc);
    writer.writeInt(data_.mto);
    writer.writeDouble(data_.tc);
    writer.writeDouble(data_.trc);
    result.push_back(writer.getLine());

    return result;
}

void DeformableToRigidAutomatic::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DeformableToRigidAutomatic, "*DEFORMABLE_TO_RIGID_AUTOMATIC")

// ============================================================================
// DeformableToRigidInertia
// ============================================================================

bool DeformableToRigidInertia::parse(const std::vector<std::string>& lines,
                                      util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: PID, PTYPE, CID, TM, IRCS, ITCS
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.pid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.ptype = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.cid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.tm = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.ircs = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.itcs = parser.getDoubleAt(pos, realW).value_or(0.0);

    return true;
}

std::vector<std::string> DeformableToRigidInertia::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.pid);
    writer.writeInt(data_.ptype);
    writer.writeInt(data_.cid);
    writer.writeDouble(data_.tm);
    writer.writeDouble(data_.ircs);
    writer.writeDouble(data_.itcs);
    result.push_back(writer.getLine());

    return result;
}

void DeformableToRigidInertia::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DeformableToRigidInertia, "*DEFORMABLE_TO_RIGID_INERTIA")

// ============================================================================
// DeformableToRigidTitle
// ============================================================================

bool DeformableToRigidTitle::parse(const std::vector<std::string>& lines,
                                    util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: Title
    data_.title = util::StringUtils::trim(lines[lineIdx]);
    ++lineIdx;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 2: PID, PTYPE, CID, TC, TRC, OPT
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.pid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.ptype = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.cid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.tc = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.trc = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.opt = static_cast<int>(parser.getInt64At(pos).value_or(0));

    return true;
}

std::vector<std::string> DeformableToRigidTitle::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    result.push_back(data_.title);

    writer.writeInt(data_.pid);
    writer.writeInt(data_.ptype);
    writer.writeInt(data_.cid);
    writer.writeDouble(data_.tc);
    writer.writeDouble(data_.trc);
    writer.writeInt(data_.opt);
    result.push_back(writer.getLine());

    return result;
}

void DeformableToRigidTitle::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DeformableToRigidTitle, "*DEFORMABLE_TO_RIGID_TITLE")

// ============================================================================
// RigidToDeformable
// ============================================================================

bool RigidToDeformable::parse(const std::vector<std::string>& lines,
                               util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: PID, PTYPE, TC, OPT
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.pid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.ptype = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.tc = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.opt = static_cast<int>(parser.getInt64At(pos).value_or(0));

    return true;
}

std::vector<std::string> RigidToDeformable::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.pid);
    writer.writeInt(data_.ptype);
    writer.writeDouble(data_.tc);
    writer.writeInt(data_.opt);
    result.push_back(writer.getLine());

    return result;
}

void RigidToDeformable::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(RigidToDeformable, "*RIGID_TO_DEFORMABLE")

// ============================================================================
// DeformableToRigidAutomaticId
// ============================================================================

bool DeformableToRigidAutomaticId::parse(const std::vector<std::string>& lines,
                                          util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: ID, PID, PTYPE, CID, MTC, MTO, TC, TRC
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.id = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.pid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.ptype = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.cid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.mtc = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.mto = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.tc = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.trc = parser.getDoubleAt(pos, realW).value_or(0.0);

    return true;
}

std::vector<std::string> DeformableToRigidAutomaticId::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.id);
    writer.writeInt(data_.pid);
    writer.writeInt(data_.ptype);
    writer.writeInt(data_.cid);
    writer.writeDouble(data_.mtc);
    writer.writeInt(data_.mto);
    writer.writeDouble(data_.tc);
    writer.writeDouble(data_.trc);
    result.push_back(writer.getLine());

    return result;
}

void DeformableToRigidAutomaticId::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DeformableToRigidAutomaticId, "*DEFORMABLE_TO_RIGID_AUTOMATIC_ID")

} // namespace koo::dyna
