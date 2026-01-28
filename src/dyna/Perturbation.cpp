#include <koo/dyna/Perturbation.hpp>
#include <koo/dyna/ModelVisitor.hpp>
#include <koo/dyna/KeywordFactory.hpp>
#include <koo/util/StringUtils.hpp>

namespace koo::dyna {

// ============================================================================
// PerturbationMat
// ============================================================================

bool PerturbationMat::parse(const std::vector<std::string>& lines,
                             util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: MID, PIDX, DVAL, MTYPE
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.mid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.pidx = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.dval = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.mtype = static_cast<int>(parser.getInt64At(pos).value_or(0));

    return true;
}

std::vector<std::string> PerturbationMat::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.mid);
    writer.writeInt(data_.pidx);
    writer.writeDouble(data_.dval);
    writer.writeInt(data_.mtype);
    result.push_back(writer.getLine());

    return result;
}

void PerturbationMat::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(PerturbationMat, "*PERTURBATION_MAT")

// ============================================================================
// PerturbationNode
// ============================================================================

bool PerturbationNode::parse(const std::vector<std::string>& lines,
                              util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (size_t lineIdx = 0; lineIdx < lines.size(); ++lineIdx) {
        if (util::CardParser::isCommentLine(lines[lineIdx])) {
            continue;
        }

        parser.setLine(lines[lineIdx]);
        size_t pos = 0;

        Entry entry;
        entry.nid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.dx = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        entry.dy = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        entry.dz = parser.getDoubleAt(pos, realW).value_or(0.0);

        if (entry.nid != 0) {
            entries_.push_back(entry);
        }
    }

    return true;
}

std::vector<std::string> PerturbationNode::write(util::CardParser::Format format) const {
    std::vector<std::string> result;

    for (const auto& entry : entries_) {
        util::CardWriter writer(format);
        writer.writeInt(entry.nid);
        writer.writeDouble(entry.dx);
        writer.writeDouble(entry.dy);
        writer.writeDouble(entry.dz);
        result.push_back(writer.getLine());
    }

    return result;
}

void PerturbationNode::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(PerturbationNode, "*PERTURBATION_NODE")

// ============================================================================
// PerturbationSection
// ============================================================================

bool PerturbationSection::parse(const std::vector<std::string>& lines,
                                 util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: SECID, PIDX, DVAL, STYPE
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.secid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.pidx = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.dval = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.stype = static_cast<int>(parser.getInt64At(pos).value_or(0));

    return true;
}

std::vector<std::string> PerturbationSection::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.secid);
    writer.writeInt(data_.pidx);
    writer.writeDouble(data_.dval);
    writer.writeInt(data_.stype);
    result.push_back(writer.getLine());

    return result;
}

void PerturbationSection::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(PerturbationSection, "*PERTURBATION_SECTION")

// ============================================================================
// PerturbationPart
// ============================================================================

bool PerturbationPart::parse(const std::vector<std::string>& lines,
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

    data_.pid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.pidx = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.dval = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.ptype = static_cast<int>(parser.getInt64At(pos).value_or(0));

    return true;
}

std::vector<std::string> PerturbationPart::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.pid);
    writer.writeInt(data_.pidx);
    writer.writeDouble(data_.dval);
    writer.writeInt(data_.ptype);
    result.push_back(writer.getLine());

    return result;
}

void PerturbationPart::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(PerturbationPart, "*PERTURBATION_PART")

// ============================================================================
// PerturbationLoad
// ============================================================================

bool PerturbationLoad::parse(const std::vector<std::string>& lines,
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
    data_.ltype = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.dval = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.pidx = static_cast<int>(parser.getInt64At(pos).value_or(0));

    return true;
}

std::vector<std::string> PerturbationLoad::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.lcid);
    writer.writeInt(data_.ltype);
    writer.writeDouble(data_.dval);
    writer.writeInt(data_.pidx);
    result.push_back(writer.getLine());

    return result;
}

void PerturbationLoad::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(PerturbationLoad, "*PERTURBATION_LOAD")

// ============================================================================
// PerturbationContact
// ============================================================================

bool PerturbationContact::parse(const std::vector<std::string>& lines,
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

    data_.cid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.pidx = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.dval = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.ctype = static_cast<int>(parser.getInt64At(pos).value_or(0));

    return true;
}

std::vector<std::string> PerturbationContact::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.cid);
    writer.writeInt(data_.pidx);
    writer.writeDouble(data_.dval);
    writer.writeInt(data_.ctype);
    result.push_back(writer.getLine());

    return result;
}

void PerturbationContact::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(PerturbationContact, "*PERTURBATION_CONTACT")

// ============================================================================
// PerturbationThickness
// ============================================================================

bool PerturbationThickness::parse(const std::vector<std::string>& lines,
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
    data_.dthk = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.ptype = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.scale = parser.getDoubleAt(pos, realW).value_or(1.0);

    return true;
}

std::vector<std::string> PerturbationThickness::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.psid);
    writer.writeDouble(data_.dthk);
    writer.writeInt(data_.ptype);
    writer.writeDouble(data_.scale);
    result.push_back(writer.getLine());

    return result;
}

void PerturbationThickness::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(PerturbationThickness, "*PERTURBATION_THICKNESS")

// ============================================================================
// PerturbationElement
// ============================================================================

bool PerturbationElement::parse(const std::vector<std::string>& lines,
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

    data_.eid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.pidx = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.dval = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.etype = static_cast<int>(parser.getInt64At(pos).value_or(0));

    return true;
}

std::vector<std::string> PerturbationElement::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.eid);
    writer.writeInt(data_.pidx);
    writer.writeDouble(data_.dval);
    writer.writeInt(data_.etype);
    result.push_back(writer.getLine());

    return result;
}

void PerturbationElement::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(PerturbationElement, "*PERTURBATION_ELEMENT")

// ============================================================================
// PerturbationGeometry
// ============================================================================

bool PerturbationGeometry::parse(const std::vector<std::string>& lines,
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

    data_.gid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.gtype = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.dx = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.dy = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.dz = parser.getDoubleAt(pos, realW).value_or(0.0);

    return true;
}

std::vector<std::string> PerturbationGeometry::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.gid);
    writer.writeInt(data_.gtype);
    writer.writeDouble(data_.dx);
    writer.writeDouble(data_.dy);
    writer.writeDouble(data_.dz);
    result.push_back(writer.getLine());

    return result;
}

void PerturbationGeometry::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(PerturbationGeometry, "*PERTURBATION_GEOMETRY")

} // namespace koo::dyna
