#include <koo/dyna/Constrained.hpp>
#include <koo/dyna/ModelVisitor.hpp>
#include <koo/dyna/KeywordFactory.hpp>
#include <koo/util/StringUtils.hpp>

namespace koo::dyna {

// ============================================================================
// ConstrainedNodalRigidBody
// ============================================================================

bool ConstrainedNodalRigidBody::parse(const std::vector<std::string>& lines,
                                       util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.pid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.cid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.nsid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.pnode = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.iprt = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.drflag = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.rrflag = static_cast<int>(parser.getInt64At(pos).value_or(0));

    return true;
}

std::vector<std::string> ConstrainedNodalRigidBody::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.pid);
    writer.writeInt(data_.cid);
    writer.writeInt(data_.nsid);
    writer.writeInt(data_.pnode);
    writer.writeInt(data_.iprt);
    writer.writeInt(data_.drflag);
    writer.writeInt(data_.rrflag);

    result.push_back(writer.getLine());
    return result;
}

void ConstrainedNodalRigidBody::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ConstrainedNodalRigidBody, "*CONSTRAINED_NODAL_RIGID_BODY")

// ============================================================================
// ConstrainedExtraNodesNode
// ============================================================================

bool ConstrainedExtraNodesNode::parse(const std::vector<std::string>& lines,
                                       util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    entries_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        Entry entry;
        size_t pos = 0;

        entry.pid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        entry.nid = parser.getInt64At(pos).value_or(0);

        if (entry.pid != 0 || entry.nid != 0) {
            entries_.push_back(entry);
        }
    }

    return true;
}

std::vector<std::string> ConstrainedExtraNodesNode::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& entry : entries_) {
        writer.clear();
        writer.writeInt(entry.pid);
        writer.writeInt(entry.nid);
        result.push_back(writer.getLine());
    }

    return result;
}

void ConstrainedExtraNodesNode::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ConstrainedExtraNodesNode, "*CONSTRAINED_EXTRA_NODES_NODE")

// ============================================================================
// ConstrainedExtraNodesSet
// ============================================================================

bool ConstrainedExtraNodesSet::parse(const std::vector<std::string>& lines,
                                      util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    entries_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        Entry entry;
        size_t pos = 0;

        entry.pid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        entry.nsid = static_cast<int>(parser.getInt64At(pos).value_or(0));

        if (entry.pid != 0 || entry.nsid != 0) {
            entries_.push_back(entry);
        }
    }

    return true;
}

std::vector<std::string> ConstrainedExtraNodesSet::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& entry : entries_) {
        writer.clear();
        writer.writeInt(entry.pid);
        writer.writeInt(entry.nsid);
        result.push_back(writer.getLine());
    }

    return result;
}

void ConstrainedExtraNodesSet::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ConstrainedExtraNodesSet, "*CONSTRAINED_EXTRA_NODES_SET")

// ============================================================================
// ConstrainedRigidBodies
// ============================================================================

bool ConstrainedRigidBodies::parse(const std::vector<std::string>& lines,
                                    util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    entries_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        Entry entry;
        size_t pos = 0;

        entry.pidm = parser.getInt64At(pos).value_or(0);
        pos += intW;
        entry.pids = parser.getInt64At(pos).value_or(0);

        if (entry.pidm != 0 || entry.pids != 0) {
            entries_.push_back(entry);
        }
    }

    return true;
}

std::vector<std::string> ConstrainedRigidBodies::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& entry : entries_) {
        writer.clear();
        writer.writeInt(entry.pidm);
        writer.writeInt(entry.pids);
        result.push_back(writer.getLine());
    }

    return result;
}

void ConstrainedRigidBodies::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ConstrainedRigidBodies, "*CONSTRAINED_RIGID_BODIES")

// ============================================================================
// ConstrainedJointSpherical
// ============================================================================

bool ConstrainedJointSpherical::parse(const std::vector<std::string>& lines,
                                       util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: JID, PIDB, PIDA
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.jid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.pidb = parser.getInt64At(pos).value_or(0);
    pos += intW;
    data_.pida = parser.getInt64At(pos).value_or(0);
    ++lineIdx;

    // Skip comments
    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 2: N1, N2, N3, N4, N5, N6
    parser.setLine(lines[lineIdx]);
    pos = 0;

    data_.n1 = parser.getInt64At(pos).value_or(0);
    pos += intW;
    data_.n2 = parser.getInt64At(pos).value_or(0);
    pos += intW;
    data_.n3 = parser.getInt64At(pos).value_or(0);
    pos += intW;
    data_.n4 = parser.getInt64At(pos).value_or(0);
    pos += intW;
    data_.n5 = parser.getInt64At(pos).value_or(0);
    pos += intW;
    data_.n6 = parser.getInt64At(pos).value_or(0);

    return true;
}

std::vector<std::string> ConstrainedJointSpherical::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    // Card 1
    writer.writeInt(data_.jid);
    writer.writeInt(data_.pidb);
    writer.writeInt(data_.pida);
    result.push_back(writer.getLine());

    // Card 2
    writer.clear();
    writer.writeInt(data_.n1);
    writer.writeInt(data_.n2);
    writer.writeInt(data_.n3);
    writer.writeInt(data_.n4);
    writer.writeInt(data_.n5);
    writer.writeInt(data_.n6);
    result.push_back(writer.getLine());

    return result;
}

void ConstrainedJointSpherical::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ConstrainedJointSpherical, "*CONSTRAINED_JOINT_SPHERICAL")

// ============================================================================
// ConstrainedJointRevolute
// ============================================================================

bool ConstrainedJointRevolute::parse(const std::vector<std::string>& lines,
                                      util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.jid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.pidb = parser.getInt64At(pos).value_or(0);
    pos += intW;
    data_.pida = parser.getInt64At(pos).value_or(0);
    ++lineIdx;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    parser.setLine(lines[lineIdx]);
    pos = 0;

    data_.n1 = parser.getInt64At(pos).value_or(0);
    pos += intW;
    data_.n2 = parser.getInt64At(pos).value_or(0);
    pos += intW;
    data_.n3 = parser.getInt64At(pos).value_or(0);
    pos += intW;
    data_.n4 = parser.getInt64At(pos).value_or(0);
    pos += intW;
    data_.n5 = parser.getInt64At(pos).value_or(0);
    pos += intW;
    data_.n6 = parser.getInt64At(pos).value_or(0);

    return true;
}

std::vector<std::string> ConstrainedJointRevolute::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.jid);
    writer.writeInt(data_.pidb);
    writer.writeInt(data_.pida);
    result.push_back(writer.getLine());

    writer.clear();
    writer.writeInt(data_.n1);
    writer.writeInt(data_.n2);
    writer.writeInt(data_.n3);
    writer.writeInt(data_.n4);
    writer.writeInt(data_.n5);
    writer.writeInt(data_.n6);
    result.push_back(writer.getLine());

    return result;
}

void ConstrainedJointRevolute::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ConstrainedJointRevolute, "*CONSTRAINED_JOINT_REVOLUTE")

// ============================================================================
// ConstrainedJointCylindrical
// ============================================================================

bool ConstrainedJointCylindrical::parse(const std::vector<std::string>& lines,
                                         util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.jid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.pidb = parser.getInt64At(pos).value_or(0);
    pos += intW;
    data_.pida = parser.getInt64At(pos).value_or(0);
    ++lineIdx;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    parser.setLine(lines[lineIdx]);
    pos = 0;

    data_.n1 = parser.getInt64At(pos).value_or(0);
    pos += intW;
    data_.n2 = parser.getInt64At(pos).value_or(0);
    pos += intW;
    data_.n3 = parser.getInt64At(pos).value_or(0);
    pos += intW;
    data_.n4 = parser.getInt64At(pos).value_or(0);
    pos += intW;
    data_.n5 = parser.getInt64At(pos).value_or(0);
    pos += intW;
    data_.n6 = parser.getInt64At(pos).value_or(0);

    return true;
}

std::vector<std::string> ConstrainedJointCylindrical::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.jid);
    writer.writeInt(data_.pidb);
    writer.writeInt(data_.pida);
    result.push_back(writer.getLine());

    writer.clear();
    writer.writeInt(data_.n1);
    writer.writeInt(data_.n2);
    writer.writeInt(data_.n3);
    writer.writeInt(data_.n4);
    writer.writeInt(data_.n5);
    writer.writeInt(data_.n6);
    result.push_back(writer.getLine());

    return result;
}

void ConstrainedJointCylindrical::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ConstrainedJointCylindrical, "*CONSTRAINED_JOINT_CYLINDRICAL")

// ============================================================================
// ConstrainedSpotweld
// ============================================================================

bool ConstrainedSpotweld::parse(const std::vector<std::string>& lines,
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

        entry.n1 = parser.getInt64At(pos).value_or(0);
        pos += intW;
        entry.n2 = parser.getInt64At(pos).value_or(0);
        pos += intW;
        entry.sn = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        entry.ss = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        entry.n = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        entry.m = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        entry.tf = parser.getDoubleAt(pos, realW).value_or(0.0);

        if (entry.n1 != 0 || entry.n2 != 0) {
            entries_.push_back(entry);
        }
    }

    return true;
}

std::vector<std::string> ConstrainedSpotweld::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& entry : entries_) {
        writer.clear();
        writer.writeInt(entry.n1);
        writer.writeInt(entry.n2);
        writer.writeDouble(entry.sn);
        writer.writeDouble(entry.ss);
        writer.writeDouble(entry.n);
        writer.writeDouble(entry.m);
        writer.writeDouble(entry.tf);
        result.push_back(writer.getLine());
    }

    return result;
}

void ConstrainedSpotweld::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ConstrainedSpotweld, "*CONSTRAINED_SPOTWELD")

// ============================================================================
// ConstrainedLinear
// ============================================================================

bool ConstrainedLinear::parse(const std::vector<std::string>& lines,
                               util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    constraints_.clear();
    Constraint current;

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        Term term;
        size_t pos = 0;

        term.nid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        term.dof = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        term.coef = parser.getDoubleAt(pos, realW).value_or(0.0);

        if (term.nid == 0 && !current.terms.empty()) {
            // End of constraint
            constraints_.push_back(current);
            current.terms.clear();
        } else if (term.nid != 0) {
            current.terms.push_back(term);
        }
    }

    if (!current.terms.empty()) {
        constraints_.push_back(current);
    }

    return true;
}

std::vector<std::string> ConstrainedLinear::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& constraint : constraints_) {
        for (const auto& term : constraint.terms) {
            writer.clear();
            writer.writeInt(term.nid);
            writer.writeInt(term.dof);
            writer.writeDouble(term.coef);
            result.push_back(writer.getLine());
        }
        // Terminator line
        writer.clear();
        writer.writeInt(0);
        writer.writeInt(0);
        writer.writeDouble(0.0);
        result.push_back(writer.getLine());
    }

    return result;
}

void ConstrainedLinear::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ConstrainedLinear, "*CONSTRAINED_LINEAR")

// ============================================================================
// ConstrainedJointTranslational
// ============================================================================

bool ConstrainedJointTranslational::parse(const std::vector<std::string>& lines,
                                           util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.jid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.pidb = parser.getInt64At(pos).value_or(0);
    pos += intW;
    data_.pida = parser.getInt64At(pos).value_or(0);
    ++lineIdx;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    parser.setLine(lines[lineIdx]);
    pos = 0;

    data_.n1 = parser.getInt64At(pos).value_or(0);
    pos += intW;
    data_.n2 = parser.getInt64At(pos).value_or(0);
    pos += intW;
    data_.n3 = parser.getInt64At(pos).value_or(0);
    pos += intW;
    data_.n4 = parser.getInt64At(pos).value_or(0);
    pos += intW;
    data_.n5 = parser.getInt64At(pos).value_or(0);
    pos += intW;
    data_.n6 = parser.getInt64At(pos).value_or(0);

    return true;
}

std::vector<std::string> ConstrainedJointTranslational::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.jid);
    writer.writeInt(data_.pidb);
    writer.writeInt(data_.pida);
    result.push_back(writer.getLine());

    writer.clear();
    writer.writeInt(data_.n1);
    writer.writeInt(data_.n2);
    writer.writeInt(data_.n3);
    writer.writeInt(data_.n4);
    writer.writeInt(data_.n5);
    writer.writeInt(data_.n6);
    result.push_back(writer.getLine());

    return result;
}

void ConstrainedJointTranslational::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ConstrainedJointTranslational, "*CONSTRAINED_JOINT_TRANSLATIONAL")

// ============================================================================
// ConstrainedJointUniversal
// ============================================================================

bool ConstrainedJointUniversal::parse(const std::vector<std::string>& lines,
                                       util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.jid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.pidb = parser.getInt64At(pos).value_or(0);
    pos += intW;
    data_.pida = parser.getInt64At(pos).value_or(0);
    ++lineIdx;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    parser.setLine(lines[lineIdx]);
    pos = 0;

    data_.n1 = parser.getInt64At(pos).value_or(0);
    pos += intW;
    data_.n2 = parser.getInt64At(pos).value_or(0);
    pos += intW;
    data_.n3 = parser.getInt64At(pos).value_or(0);
    pos += intW;
    data_.n4 = parser.getInt64At(pos).value_or(0);
    pos += intW;
    data_.n5 = parser.getInt64At(pos).value_or(0);
    pos += intW;
    data_.n6 = parser.getInt64At(pos).value_or(0);

    return true;
}

std::vector<std::string> ConstrainedJointUniversal::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.jid);
    writer.writeInt(data_.pidb);
    writer.writeInt(data_.pida);
    result.push_back(writer.getLine());

    writer.clear();
    writer.writeInt(data_.n1);
    writer.writeInt(data_.n2);
    writer.writeInt(data_.n3);
    writer.writeInt(data_.n4);
    writer.writeInt(data_.n5);
    writer.writeInt(data_.n6);
    result.push_back(writer.getLine());

    return result;
}

void ConstrainedJointUniversal::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ConstrainedJointUniversal, "*CONSTRAINED_JOINT_UNIVERSAL")

// ============================================================================
// ConstrainedJointStiffness
// ============================================================================

bool ConstrainedJointStiffness::parse(const std::vector<std::string>& lines,
                                       util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t cardNum = 0;
    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);

        if (cardNum == 0) {
            // Card 1: JSID, LCIDPH, LCIDTH, LCIDPS, LCIDTQ
            size_t pos = 0;
            data_.jsid = static_cast<int>(parser.getInt64At(pos).value_or(0));
            pos += intW;
            data_.lcidph = static_cast<int>(parser.getInt64At(pos).value_or(0));
            pos += intW;
            data_.lcidth = static_cast<int>(parser.getInt64At(pos).value_or(0));
            pos += intW;
            data_.lcidps = static_cast<int>(parser.getInt64At(pos).value_or(0));
            pos += intW;
            data_.lcidtq = static_cast<int>(parser.getInt64At(pos).value_or(0));
        } else if (cardNum == 1) {
            // Card 2: DLCIDPH, DLCIDTH, DLCIDPS, DLCIDTQ
            size_t pos = 0;
            data_.dlcidph = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.dlcidth = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.dlcidps = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.dlcidtq = parser.getDoubleAt(pos, realW).value_or(0.0);
        }

        cardNum++;
        if (cardNum >= 2) break;
    }

    return true;
}

std::vector<std::string> ConstrainedJointStiffness::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    // Card 1
    writer.writeInt(data_.jsid);
    writer.writeInt(data_.lcidph);
    writer.writeInt(data_.lcidth);
    writer.writeInt(data_.lcidps);
    writer.writeInt(data_.lcidtq);
    result.push_back(writer.getLine());

    // Card 2
    writer.clear();
    writer.writeDouble(data_.dlcidph);
    writer.writeDouble(data_.dlcidth);
    writer.writeDouble(data_.dlcidps);
    writer.writeDouble(data_.dlcidtq);
    result.push_back(writer.getLine());

    return result;
}

void ConstrainedJointStiffness::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ConstrainedJointStiffness, "*CONSTRAINED_JOINT_STIFFNESS")

// ============================================================================
// ConstrainedShellToSolid
// ============================================================================

bool ConstrainedShellToSolid::parse(const std::vector<std::string>& lines,
                                     util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);

        // Card 1: SSET, MSET, NCOUP
        size_t pos = 0;
        data_.sset = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.mset = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.ncoup = static_cast<int>(parser.getInt64At(pos).value_or(0));

        break;
    }

    return true;
}

std::vector<std::string> ConstrainedShellToSolid::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.sset);
    writer.writeInt(data_.mset);
    writer.writeInt(data_.ncoup);
    result.push_back(writer.getLine());

    return result;
}

void ConstrainedShellToSolid::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ConstrainedShellToSolid, "*CONSTRAINED_SHELL_TO_SOLID")

// ============================================================================
// ConstrainedGeneralizedWeldNode
// ============================================================================

bool ConstrainedGeneralizedWeldNode::parse(const std::vector<std::string>& lines,
                                            util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    entries_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);
        Entry entry;
        size_t pos = 0;

        entry.n1 = parser.getInt64At(pos).value_or(0);
        pos += intW;
        entry.n2 = parser.getInt64At(pos).value_or(0);
        pos += intW;
        entry.n3 = parser.getInt64At(pos).value_or(0);
        pos += intW;
        entry.n4 = parser.getInt64At(pos).value_or(0);
        pos += intW;
        entry.n5 = parser.getInt64At(pos).value_or(0);
        pos += intW;
        entry.n6 = parser.getInt64At(pos).value_or(0);
        pos += intW;
        entry.n7 = parser.getInt64At(pos).value_or(0);
        pos += intW;
        entry.n8 = parser.getInt64At(pos).value_or(0);

        if (entry.n1 != 0) {
            entries_.push_back(entry);
        }
    }

    return true;
}

std::vector<std::string> ConstrainedGeneralizedWeldNode::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& entry : entries_) {
        writer.clear();
        writer.writeInt(entry.n1);
        writer.writeInt(entry.n2);
        writer.writeInt(entry.n3);
        writer.writeInt(entry.n4);
        writer.writeInt(entry.n5);
        writer.writeInt(entry.n6);
        writer.writeInt(entry.n7);
        writer.writeInt(entry.n8);
        result.push_back(writer.getLine());
    }

    return result;
}

void ConstrainedGeneralizedWeldNode::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ConstrainedGeneralizedWeldNode, "*CONSTRAINED_GENERALIZED_WELD_NODE")

// ============================================================================
// ConstrainedJointPlanar
// ============================================================================

bool ConstrainedJointPlanar::parse(const std::vector<std::string>& lines,
                                    util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.jid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.pidb = parser.getInt64At(pos).value_or(0);
    pos += intW;
    data_.pida = parser.getInt64At(pos).value_or(0);
    ++lineIdx;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    parser.setLine(lines[lineIdx]);
    pos = 0;

    data_.n1 = parser.getInt64At(pos).value_or(0);
    pos += intW;
    data_.n2 = parser.getInt64At(pos).value_or(0);
    pos += intW;
    data_.n3 = parser.getInt64At(pos).value_or(0);
    pos += intW;
    data_.n4 = parser.getInt64At(pos).value_or(0);
    pos += intW;
    data_.n5 = parser.getInt64At(pos).value_or(0);
    pos += intW;
    data_.n6 = parser.getInt64At(pos).value_or(0);

    return true;
}

std::vector<std::string> ConstrainedJointPlanar::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.jid);
    writer.writeInt(data_.pidb);
    writer.writeInt(data_.pida);
    result.push_back(writer.getLine());

    writer.clear();
    writer.writeInt(data_.n1);
    writer.writeInt(data_.n2);
    writer.writeInt(data_.n3);
    writer.writeInt(data_.n4);
    writer.writeInt(data_.n5);
    writer.writeInt(data_.n6);
    result.push_back(writer.getLine());

    return result;
}

void ConstrainedJointPlanar::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ConstrainedJointPlanar, "*CONSTRAINED_JOINT_PLANAR")

// ============================================================================
// ConstrainedBeamInSolid
// ============================================================================

bool ConstrainedBeamInSolid::parse(const std::vector<std::string>& lines,
                                    util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: COUPID, BSID, SSID, NCOUP, CDIR
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.coupid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.bsid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.ssid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.ncoup = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.cdir = static_cast<int>(parser.getInt64At(pos).value_or(0));

    return true;
}

std::vector<std::string> ConstrainedBeamInSolid::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.coupid);
    writer.writeInt(data_.bsid);
    writer.writeInt(data_.ssid);
    writer.writeInt(data_.ncoup);
    writer.writeInt(data_.cdir);
    result.push_back(writer.getLine());

    return result;
}

void ConstrainedBeamInSolid::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ConstrainedBeamInSolid, "*CONSTRAINED_BEAM_IN_SOLID")

// ============================================================================
// ConstrainedLagrangeInSolid
// ============================================================================

bool ConstrainedLagrangeInSolid::parse(const std::vector<std::string>& lines,
                                        util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: SLAVE, MASTER, SSTYP, MSTYP, NQUAD, CTYPE, DIREC, MCOUP
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.slave = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.master = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.sstyp = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.mstyp = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.nquad = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.ctype = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.direc = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.mcoup = static_cast<int>(parser.getInt64At(pos).value_or(0));

    return true;
}

std::vector<std::string> ConstrainedLagrangeInSolid::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.slave);
    writer.writeInt(data_.master);
    writer.writeInt(data_.sstyp);
    writer.writeInt(data_.mstyp);
    writer.writeInt(data_.nquad);
    writer.writeDouble(data_.ctype);
    writer.writeInt(data_.direc);
    writer.writeInt(data_.mcoup);
    result.push_back(writer.getLine());

    return result;
}

void ConstrainedLagrangeInSolid::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ConstrainedLagrangeInSolid, "*CONSTRAINED_LAGRANGE_IN_SOLID")

// ============================================================================
// ConstrainedTieBreak
// ============================================================================

bool ConstrainedTieBreak::parse(const std::vector<std::string>& lines,
                                 util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: SSID, MSID, SSTYP, MSTYP, NFLS, SFLS, EN, ES
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.ssid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.msid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.sstyp = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.mstyp = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.nfls = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.sfls = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.en = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.es = parser.getDoubleAt(pos, realW).value_or(0.0);

    return true;
}

std::vector<std::string> ConstrainedTieBreak::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.ssid);
    writer.writeInt(data_.msid);
    writer.writeInt(data_.sstyp);
    writer.writeInt(data_.mstyp);
    writer.writeDouble(data_.nfls);
    writer.writeDouble(data_.sfls);
    writer.writeDouble(data_.en);
    writer.writeDouble(data_.es);
    result.push_back(writer.getLine());

    return result;
}

void ConstrainedTieBreak::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ConstrainedTieBreak, "*CONSTRAINED_TIE-BREAK")

// ============================================================================
// ConstrainedRigidBodyStoppers
// ============================================================================

bool ConstrainedRigidBodyStoppers::parse(const std::vector<std::string>& lines,
                                           util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    data_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        Data entry;
        size_t pos = 0;

        entry.pid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        entry.dir = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.lcmin = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        entry.lcmax = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        entry.vid = static_cast<int>(parser.getInt64At(pos).value_or(0));

        if (entry.pid != 0) {
            data_.push_back(entry);
        }
    }

    return true;
}

std::vector<std::string> ConstrainedRigidBodyStoppers::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& entry : data_) {
        writer.clear();
        writer.writeInt(entry.pid);
        writer.writeInt(entry.dir);
        writer.writeDouble(entry.lcmin);
        writer.writeDouble(entry.lcmax);
        writer.writeInt(entry.vid);
        result.push_back(writer.getLine());
    }

    return result;
}

void ConstrainedRigidBodyStoppers::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ConstrainedRigidBodyStoppers, "*CONSTRAINED_RIGID_BODY_STOPPERS")

// ============================================================================
// ConstrainedNodeSet
// ============================================================================

bool ConstrainedNodeSet::parse(const std::vector<std::string>& lines,
                                util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    data_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        Data entry;
        size_t pos = 0;

        entry.nsid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.dof = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.cid = static_cast<int>(parser.getInt64At(pos).value_or(0));

        if (entry.nsid != 0) {
            data_.push_back(entry);
        }
    }

    return true;
}

std::vector<std::string> ConstrainedNodeSet::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& entry : data_) {
        writer.clear();
        writer.writeInt(entry.nsid);
        writer.writeInt(entry.dof);
        writer.writeInt(entry.cid);
        result.push_back(writer.getLine());
    }

    return result;
}

void ConstrainedNodeSet::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ConstrainedNodeSet, "*CONSTRAINED_NODE_SET")

// ============================================================================
// ConstrainedInterpolation
// ============================================================================

bool ConstrainedInterpolation::parse(const std::vector<std::string>& lines,
                                      util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size()) {
        while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
            ++lineIdx;
        }
        if (lineIdx >= lines.size()) break;

        // Card 1: ICID, DNID, DDOF, CITEFLAG
        Data entry;
        parser.setLine(lines[lineIdx]);
        size_t pos = 0;

        entry.icid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.dnid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.ddof = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.citeflag = static_cast<int>(parser.getInt64At(pos).value_or(0));
        ++lineIdx;

        if (entry.icid == 0 && entry.dnid == 0) break;

        // Read independent node cards (up to 4 per card)
        while (lineIdx < lines.size() && !util::CardParser::isCommentLine(lines[lineIdx])) {
            parser.setLine(lines[lineIdx]);
            pos = 0;

            for (int i = 0; i < 4; ++i) {
                NodeId nid = static_cast<NodeId>(parser.getInt64At(pos).value_or(0));
                pos += intW;
                double coef = parser.getDoubleAt(pos, realW).value_or(0.0);
                pos += realW;

                if (nid != 0) {
                    entry.independentNodes.emplace_back(nid, coef);
                }
            }

            // Check if this line looks like a new constraint header
            parser.setLine(lines[lineIdx]);
            int potentialIcid = static_cast<int>(parser.getInt64At(0).value_or(0));
            if (potentialIcid != 0 && entry.independentNodes.empty()) {
                break;
            }

            ++lineIdx;

            // If the last node coefficient was 0, we're done with this constraint
            if (entry.independentNodes.empty() || entry.independentNodes.back().first == 0) {
                break;
            }
        }

        data_.push_back(entry);
    }

    return true;
}

std::vector<std::string> ConstrainedInterpolation::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& entry : data_) {
        // Header card
        writer.clear();
        writer.writeInt(entry.icid);
        writer.writeInt(entry.dnid);
        writer.writeInt(entry.ddof);
        writer.writeInt(entry.citeflag);
        result.push_back(writer.getLine());

        // Independent node cards (4 per line)
        size_t nodeCount = entry.independentNodes.size();
        for (size_t i = 0; i < nodeCount; i += 4) {
            writer.clear();
            for (size_t j = 0; j < 4 && (i + j) < nodeCount; ++j) {
                writer.writeInt(static_cast<int>(entry.independentNodes[i + j].first));
                writer.writeDouble(entry.independentNodes[i + j].second);
            }
            result.push_back(writer.getLine());
        }
    }

    return result;
}

void ConstrainedInterpolation::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ConstrainedInterpolation, "*CONSTRAINED_INTERPOLATION")

// ============================================================================
// ConstrainedGlobal
// ============================================================================

bool ConstrainedGlobal::parse(const std::vector<std::string>& lines,
                               util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size()) {
        while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
            ++lineIdx;
        }
        if (lineIdx >= lines.size()) break;

        // Header card: EQID, RHS
        Equation eq;
        parser.setLine(lines[lineIdx]);
        size_t pos = 0;

        eq.eqid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        eq.rhs = parser.getDoubleAt(pos, realW).value_or(0.0);
        ++lineIdx;

        if (eq.eqid == 0) break;

        // Term cards: NID, DOF, COEF (3 terms per card)
        while (lineIdx < lines.size()) {
            if (util::CardParser::isCommentLine(lines[lineIdx])) {
                ++lineIdx;
                continue;
            }

            parser.setLine(lines[lineIdx]);
            pos = 0;

            bool hasTerms = false;
            for (int i = 0; i < 3; ++i) {
                NodeId nid = static_cast<NodeId>(parser.getInt64At(pos).value_or(0));
                pos += intW;
                int dof = static_cast<int>(parser.getInt64At(pos).value_or(0));
                pos += intW;
                double coef = parser.getDoubleAt(pos, realW).value_or(0.0);
                pos += realW;

                if (nid != 0) {
                    Term term;
                    term.nid = nid;
                    term.dof = dof;
                    term.coef = coef;
                    eq.terms.push_back(term);
                    hasTerms = true;
                }
            }

            ++lineIdx;
            if (!hasTerms) break;
        }

        equations_.push_back(eq);
    }

    return true;
}

std::vector<std::string> ConstrainedGlobal::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& eq : equations_) {
        // Header card
        writer.clear();
        writer.writeInt(eq.eqid);
        writer.writeDouble(eq.rhs);
        result.push_back(writer.getLine());

        // Term cards (3 per line)
        size_t termCount = eq.terms.size();
        for (size_t i = 0; i < termCount; i += 3) {
            writer.clear();
            for (size_t j = 0; j < 3 && (i + j) < termCount; ++j) {
                writer.writeInt(static_cast<int>(eq.terms[i + j].nid));
                writer.writeInt(eq.terms[i + j].dof);
                writer.writeDouble(eq.terms[i + j].coef);
            }
            result.push_back(writer.getLine());
        }
    }

    return result;
}

void ConstrainedGlobal::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ConstrainedGlobal, "*CONSTRAINED_GLOBAL")

} // namespace koo::dyna
