#include <koo/dyna/Set.hpp>
#include <koo/dyna/ModelVisitor.hpp>
#include <koo/dyna/KeywordFactory.hpp>
#include <koo/util/StringUtils.hpp>

namespace koo::dyna {

// ============================================================================
// SetNode
// ============================================================================

bool SetNode::parse(const std::vector<std::string>& lines,
                     util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    nodes_.clear();
    size_t lineIdx = 0;

    // Skip comments to find header
    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: SID
    parser.setLine(lines[lineIdx]);
    sid_ = static_cast<int>(parser.getInt64At(0).value_or(0));
    ++lineIdx;

    // Parse node IDs (8 per card)
    for (; lineIdx < lines.size(); ++lineIdx) {
        if (util::CardParser::isCommentLine(lines[lineIdx])) continue;

        parser.setLine(lines[lineIdx]);
        for (size_t i = 0; i < 8; ++i) {
            auto nid = parser.getInt64At(i * intW);
            if (nid && *nid != 0) {
                nodes_.push_back(*nid);
            }
        }
    }

    return true;
}

std::vector<std::string> SetNode::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    // Header card
    writer.writeInt(sid_);
    result.push_back(writer.getLine());

    // Node IDs (8 per card)
    size_t count = 0;
    writer.clear();
    for (auto nid : nodes_) {
        writer.writeInt(nid);
        ++count;
        if (count == 8) {
            result.push_back(writer.getLine());
            writer.clear();
            count = 0;
        }
    }
    if (count > 0) {
        result.push_back(writer.getLine());
    }

    return result;
}

void SetNode::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(SetNode, "*SET_NODE")

// ============================================================================
// SetNodeList
// ============================================================================

bool SetNodeList::parse(const std::vector<std::string>& lines,
                         util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    nodes_.clear();
    size_t lineIdx = 0;

    // Skip comments
    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: SID, DA1, DA2, DA3, DA4, SOLVER
    parser.setLine(lines[lineIdx]);
    sid_ = static_cast<int>(parser.getInt64At(0).value_or(0));
    ++lineIdx;

    // Parse node IDs (8 per card)
    for (; lineIdx < lines.size(); ++lineIdx) {
        if (util::CardParser::isCommentLine(lines[lineIdx])) continue;

        parser.setLine(lines[lineIdx]);
        for (size_t i = 0; i < 8; ++i) {
            auto nid = parser.getInt64At(i * intW);
            if (nid && *nid != 0) {
                nodes_.push_back(*nid);
            }
        }
    }

    return true;
}

std::vector<std::string> SetNodeList::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    // Header
    writer.writeInt(sid_);
    result.push_back(writer.getLine());

    // Node IDs
    size_t count = 0;
    writer.clear();
    for (auto nid : nodes_) {
        writer.writeInt(nid);
        ++count;
        if (count == 8) {
            result.push_back(writer.getLine());
            writer.clear();
            count = 0;
        }
    }
    if (count > 0) {
        result.push_back(writer.getLine());
    }

    return result;
}

void SetNodeList::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(SetNodeList, "*SET_NODE_LIST")

// ============================================================================
// SetNodeListTitle
// ============================================================================

bool SetNodeListTitle::parse(const std::vector<std::string>& lines,
                              util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    nodes_.clear();
    size_t lineIdx = 0;

    // Skip comments
    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Title line
    title_ = util::StringUtils::trim(lines[lineIdx]);
    ++lineIdx;

    // Skip comments
    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // SID card
    parser.setLine(lines[lineIdx]);
    sid_ = static_cast<int>(parser.getInt64At(0).value_or(0));
    ++lineIdx;

    // Parse node IDs
    for (; lineIdx < lines.size(); ++lineIdx) {
        if (util::CardParser::isCommentLine(lines[lineIdx])) continue;

        parser.setLine(lines[lineIdx]);
        for (size_t i = 0; i < 8; ++i) {
            auto nid = parser.getInt64At(i * intW);
            if (nid && *nid != 0) {
                nodes_.push_back(*nid);
            }
        }
    }

    return true;
}

std::vector<std::string> SetNodeListTitle::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    // Title
    result.push_back(title_);

    // Header
    writer.writeInt(sid_);
    result.push_back(writer.getLine());

    // Node IDs
    size_t count = 0;
    writer.clear();
    for (auto nid : nodes_) {
        writer.writeInt(nid);
        ++count;
        if (count == 8) {
            result.push_back(writer.getLine());
            writer.clear();
            count = 0;
        }
    }
    if (count > 0) {
        result.push_back(writer.getLine());
    }

    return result;
}

void SetNodeListTitle::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(SetNodeListTitle, "*SET_NODE_LIST_TITLE")

// ============================================================================
// SetPart
// ============================================================================

bool SetPart::parse(const std::vector<std::string>& lines,
                     util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    parts_.clear();
    size_t lineIdx = 0;

    // Skip comments
    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // SID
    parser.setLine(lines[lineIdx]);
    sid_ = static_cast<int>(parser.getInt64At(0).value_or(0));
    ++lineIdx;

    // Parse part IDs
    for (; lineIdx < lines.size(); ++lineIdx) {
        if (util::CardParser::isCommentLine(lines[lineIdx])) continue;

        parser.setLine(lines[lineIdx]);
        for (size_t i = 0; i < 8; ++i) {
            auto pid = parser.getInt64At(i * intW);
            if (pid && *pid != 0) {
                parts_.push_back(*pid);
            }
        }
    }

    return true;
}

std::vector<std::string> SetPart::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(sid_);
    result.push_back(writer.getLine());

    size_t count = 0;
    writer.clear();
    for (auto pid : parts_) {
        writer.writeInt(pid);
        ++count;
        if (count == 8) {
            result.push_back(writer.getLine());
            writer.clear();
            count = 0;
        }
    }
    if (count > 0) {
        result.push_back(writer.getLine());
    }

    return result;
}

void SetPart::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(SetPart, "*SET_PART")

// ============================================================================
// SetPartList
// ============================================================================

bool SetPartList::parse(const std::vector<std::string>& lines,
                         util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    parts_.clear();
    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    parser.setLine(lines[lineIdx]);
    sid_ = static_cast<int>(parser.getInt64At(0).value_or(0));
    ++lineIdx;

    for (; lineIdx < lines.size(); ++lineIdx) {
        if (util::CardParser::isCommentLine(lines[lineIdx])) continue;

        parser.setLine(lines[lineIdx]);
        for (size_t i = 0; i < 8; ++i) {
            auto pid = parser.getInt64At(i * intW);
            if (pid && *pid != 0) {
                parts_.push_back(*pid);
            }
        }
    }

    return true;
}

std::vector<std::string> SetPartList::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(sid_);
    result.push_back(writer.getLine());

    size_t count = 0;
    writer.clear();
    for (auto pid : parts_) {
        writer.writeInt(pid);
        ++count;
        if (count == 8) {
            result.push_back(writer.getLine());
            writer.clear();
            count = 0;
        }
    }
    if (count > 0) {
        result.push_back(writer.getLine());
    }

    return result;
}

void SetPartList::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(SetPartList, "*SET_PART_LIST")

// ============================================================================
// SetPartListTitle
// ============================================================================

bool SetPartListTitle::parse(const std::vector<std::string>& lines,
                              util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    parts_.clear();
    size_t lineIdx = 0;

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

    parser.setLine(lines[lineIdx]);
    sid_ = static_cast<int>(parser.getInt64At(0).value_or(0));
    ++lineIdx;

    for (; lineIdx < lines.size(); ++lineIdx) {
        if (util::CardParser::isCommentLine(lines[lineIdx])) continue;

        parser.setLine(lines[lineIdx]);
        for (size_t i = 0; i < 8; ++i) {
            auto pid = parser.getInt64At(i * intW);
            if (pid && *pid != 0) {
                parts_.push_back(*pid);
            }
        }
    }

    return true;
}

std::vector<std::string> SetPartListTitle::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    result.push_back(title_);

    writer.writeInt(sid_);
    result.push_back(writer.getLine());

    size_t count = 0;
    writer.clear();
    for (auto pid : parts_) {
        writer.writeInt(pid);
        ++count;
        if (count == 8) {
            result.push_back(writer.getLine());
            writer.clear();
            count = 0;
        }
    }
    if (count > 0) {
        result.push_back(writer.getLine());
    }

    return result;
}

void SetPartListTitle::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(SetPartListTitle, "*SET_PART_LIST_TITLE")

// ============================================================================
// SetSegment
// ============================================================================

bool SetSegment::parse(const std::vector<std::string>& lines,
                        util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    segments_.clear();
    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    parser.setLine(lines[lineIdx]);
    sid_ = static_cast<int>(parser.getInt64At(0).value_or(0));
    ++lineIdx;

    for (; lineIdx < lines.size(); ++lineIdx) {
        if (util::CardParser::isCommentLine(lines[lineIdx])) continue;

        parser.setLine(lines[lineIdx]);
        Segment seg;
        size_t pos = 0;

        seg.n1 = parser.getInt64At(pos).value_or(0);
        pos += intW;
        seg.n2 = parser.getInt64At(pos).value_or(0);
        pos += intW;
        seg.n3 = parser.getInt64At(pos).value_or(0);
        pos += intW;
        seg.n4 = parser.getInt64At(pos).value_or(0);

        if (seg.n1 != 0) {
            segments_.push_back(seg);
        }
    }

    return true;
}

std::vector<std::string> SetSegment::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(sid_);
    result.push_back(writer.getLine());

    for (const auto& seg : segments_) {
        writer.clear();
        writer.writeInt(seg.n1);
        writer.writeInt(seg.n2);
        writer.writeInt(seg.n3);
        writer.writeInt(seg.n4);
        result.push_back(writer.getLine());
    }

    return result;
}

void SetSegment::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(SetSegment, "*SET_SEGMENT")

// ============================================================================
// SetSegmentTitle
// ============================================================================

bool SetSegmentTitle::parse(const std::vector<std::string>& lines,
                             util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    segments_.clear();
    size_t lineIdx = 0;

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

    parser.setLine(lines[lineIdx]);
    sid_ = static_cast<int>(parser.getInt64At(0).value_or(0));
    ++lineIdx;

    for (; lineIdx < lines.size(); ++lineIdx) {
        if (util::CardParser::isCommentLine(lines[lineIdx])) continue;

        parser.setLine(lines[lineIdx]);
        Segment seg;
        size_t pos = 0;

        seg.n1 = parser.getInt64At(pos).value_or(0);
        pos += intW;
        seg.n2 = parser.getInt64At(pos).value_or(0);
        pos += intW;
        seg.n3 = parser.getInt64At(pos).value_or(0);
        pos += intW;
        seg.n4 = parser.getInt64At(pos).value_or(0);

        if (seg.n1 != 0) {
            segments_.push_back(seg);
        }
    }

    return true;
}

std::vector<std::string> SetSegmentTitle::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    result.push_back(title_);

    writer.writeInt(sid_);
    result.push_back(writer.getLine());

    for (const auto& seg : segments_) {
        writer.clear();
        writer.writeInt(seg.n1);
        writer.writeInt(seg.n2);
        writer.writeInt(seg.n3);
        writer.writeInt(seg.n4);
        result.push_back(writer.getLine());
    }

    return result;
}

void SetSegmentTitle::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(SetSegmentTitle, "*SET_SEGMENT_TITLE")

// ============================================================================
// SetShell
// ============================================================================

bool SetShell::parse(const std::vector<std::string>& lines,
                      util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    elements_.clear();
    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    parser.setLine(lines[lineIdx]);
    sid_ = static_cast<int>(parser.getInt64At(0).value_or(0));
    ++lineIdx;

    for (; lineIdx < lines.size(); ++lineIdx) {
        if (util::CardParser::isCommentLine(lines[lineIdx])) continue;

        parser.setLine(lines[lineIdx]);
        for (size_t i = 0; i < 8; ++i) {
            auto eid = parser.getInt64At(i * intW);
            if (eid && *eid != 0) {
                elements_.push_back(*eid);
            }
        }
    }

    return true;
}

std::vector<std::string> SetShell::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(sid_);
    result.push_back(writer.getLine());

    size_t count = 0;
    writer.clear();
    for (auto eid : elements_) {
        writer.writeInt(eid);
        ++count;
        if (count == 8) {
            result.push_back(writer.getLine());
            writer.clear();
            count = 0;
        }
    }
    if (count > 0) {
        result.push_back(writer.getLine());
    }

    return result;
}

void SetShell::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(SetShell, "*SET_SHELL")

// ============================================================================
// SetShellList
// ============================================================================

bool SetShellList::parse(const std::vector<std::string>& lines,
                          util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    elements_.clear();
    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    parser.setLine(lines[lineIdx]);
    sid_ = static_cast<int>(parser.getInt64At(0).value_or(0));
    ++lineIdx;

    for (; lineIdx < lines.size(); ++lineIdx) {
        if (util::CardParser::isCommentLine(lines[lineIdx])) continue;

        parser.setLine(lines[lineIdx]);
        for (size_t i = 0; i < 8; ++i) {
            auto eid = parser.getInt64At(i * intW);
            if (eid && *eid != 0) {
                elements_.push_back(*eid);
            }
        }
    }

    return true;
}

std::vector<std::string> SetShellList::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(sid_);
    result.push_back(writer.getLine());

    size_t count = 0;
    writer.clear();
    for (auto eid : elements_) {
        writer.writeInt(eid);
        ++count;
        if (count == 8) {
            result.push_back(writer.getLine());
            writer.clear();
            count = 0;
        }
    }
    if (count > 0) {
        result.push_back(writer.getLine());
    }

    return result;
}

void SetShellList::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(SetShellList, "*SET_SHELL_LIST")

// ============================================================================
// SetSolid
// ============================================================================

bool SetSolid::parse(const std::vector<std::string>& lines,
                      util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    elements_.clear();
    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    parser.setLine(lines[lineIdx]);
    sid_ = static_cast<int>(parser.getInt64At(0).value_or(0));
    ++lineIdx;

    for (; lineIdx < lines.size(); ++lineIdx) {
        if (util::CardParser::isCommentLine(lines[lineIdx])) continue;

        parser.setLine(lines[lineIdx]);
        for (size_t i = 0; i < 8; ++i) {
            auto eid = parser.getInt64At(i * intW);
            if (eid && *eid != 0) {
                elements_.push_back(*eid);
            }
        }
    }

    return true;
}

std::vector<std::string> SetSolid::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(sid_);
    result.push_back(writer.getLine());

    size_t count = 0;
    writer.clear();
    for (auto eid : elements_) {
        writer.writeInt(eid);
        ++count;
        if (count == 8) {
            result.push_back(writer.getLine());
            writer.clear();
            count = 0;
        }
    }
    if (count > 0) {
        result.push_back(writer.getLine());
    }

    return result;
}

void SetSolid::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(SetSolid, "*SET_SOLID")

// ============================================================================
// SetSolidList
// ============================================================================

bool SetSolidList::parse(const std::vector<std::string>& lines,
                          util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    elements_.clear();
    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    parser.setLine(lines[lineIdx]);
    sid_ = static_cast<int>(parser.getInt64At(0).value_or(0));
    ++lineIdx;

    for (; lineIdx < lines.size(); ++lineIdx) {
        if (util::CardParser::isCommentLine(lines[lineIdx])) continue;

        parser.setLine(lines[lineIdx]);
        for (size_t i = 0; i < 8; ++i) {
            auto eid = parser.getInt64At(i * intW);
            if (eid && *eid != 0) {
                elements_.push_back(*eid);
            }
        }
    }

    return true;
}

std::vector<std::string> SetSolidList::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(sid_);
    result.push_back(writer.getLine());

    size_t count = 0;
    writer.clear();
    for (auto eid : elements_) {
        writer.writeInt(eid);
        ++count;
        if (count == 8) {
            result.push_back(writer.getLine());
            writer.clear();
            count = 0;
        }
    }
    if (count > 0) {
        result.push_back(writer.getLine());
    }

    return result;
}

void SetSolidList::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(SetSolidList, "*SET_SOLID_LIST")

// ============================================================================
// SetNodeGenerate
// ============================================================================

bool SetNodeGenerate::parse(const std::vector<std::string>& lines,
                             util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    ranges_.clear();
    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    parser.setLine(lines[lineIdx]);
    sid_ = static_cast<int>(parser.getInt64At(0).value_or(0));
    ++lineIdx;

    for (; lineIdx < lines.size(); ++lineIdx) {
        if (util::CardParser::isCommentLine(lines[lineIdx])) continue;

        parser.setLine(lines[lineIdx]);
        Range r;
        size_t pos = 0;

        r.nid1 = parser.getInt64At(pos).value_or(0);
        pos += intW;
        r.nid2 = parser.getInt64At(pos).value_or(0);
        pos += intW;
        r.dnidn = static_cast<int>(parser.getInt64At(pos).value_or(1));

        if (r.nid1 != 0) {
            ranges_.push_back(r);
        }
    }

    return true;
}

std::vector<std::string> SetNodeGenerate::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(sid_);
    result.push_back(writer.getLine());

    for (const auto& r : ranges_) {
        writer.clear();
        writer.writeInt(r.nid1);
        writer.writeInt(r.nid2);
        writer.writeInt(r.dnidn);
        result.push_back(writer.getLine());
    }

    return result;
}

void SetNodeGenerate::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(SetNodeGenerate, "*SET_NODE_GENERATE")

// ============================================================================
// SetPartGenerate
// ============================================================================

bool SetPartGenerate::parse(const std::vector<std::string>& lines,
                             util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    ranges_.clear();
    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    parser.setLine(lines[lineIdx]);
    sid_ = static_cast<int>(parser.getInt64At(0).value_or(0));
    ++lineIdx;

    for (; lineIdx < lines.size(); ++lineIdx) {
        if (util::CardParser::isCommentLine(lines[lineIdx])) continue;

        parser.setLine(lines[lineIdx]);
        Range r;
        size_t pos = 0;

        r.pid1 = parser.getInt64At(pos).value_or(0);
        pos += intW;
        r.pid2 = parser.getInt64At(pos).value_or(0);
        pos += intW;
        r.dpidp = static_cast<int>(parser.getInt64At(pos).value_or(1));

        if (r.pid1 != 0) {
            ranges_.push_back(r);
        }
    }

    return true;
}

std::vector<std::string> SetPartGenerate::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(sid_);
    result.push_back(writer.getLine());

    for (const auto& r : ranges_) {
        writer.clear();
        writer.writeInt(r.pid1);
        writer.writeInt(r.pid2);
        writer.writeInt(r.dpidp);
        result.push_back(writer.getLine());
    }

    return result;
}

void SetPartGenerate::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(SetPartGenerate, "*SET_PART_GENERATE")

// ============================================================================
// SetShellGenerate
// ============================================================================

bool SetShellGenerate::parse(const std::vector<std::string>& lines,
                              util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    ranges_.clear();
    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    parser.setLine(lines[lineIdx]);
    sid_ = static_cast<int>(parser.getInt64At(0).value_or(0));
    ++lineIdx;

    for (; lineIdx < lines.size(); ++lineIdx) {
        if (util::CardParser::isCommentLine(lines[lineIdx])) continue;

        parser.setLine(lines[lineIdx]);
        Range r;
        size_t pos = 0;

        r.eid1 = parser.getInt64At(pos).value_or(0);
        pos += intW;
        r.eid2 = parser.getInt64At(pos).value_or(0);
        pos += intW;
        r.deide = static_cast<int>(parser.getInt64At(pos).value_or(1));

        if (r.eid1 != 0) {
            ranges_.push_back(r);
        }
    }

    return true;
}

std::vector<std::string> SetShellGenerate::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(sid_);
    result.push_back(writer.getLine());

    for (const auto& r : ranges_) {
        writer.clear();
        writer.writeInt(r.eid1);
        writer.writeInt(r.eid2);
        writer.writeInt(r.deide);
        result.push_back(writer.getLine());
    }

    return result;
}

void SetShellGenerate::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(SetShellGenerate, "*SET_SHELL_GENERATE")

// ============================================================================
// SetSolidGenerate
// ============================================================================

bool SetSolidGenerate::parse(const std::vector<std::string>& lines,
                              util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    ranges_.clear();
    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    parser.setLine(lines[lineIdx]);
    sid_ = static_cast<int>(parser.getInt64At(0).value_or(0));
    ++lineIdx;

    for (; lineIdx < lines.size(); ++lineIdx) {
        if (util::CardParser::isCommentLine(lines[lineIdx])) continue;

        parser.setLine(lines[lineIdx]);
        Range r;
        size_t pos = 0;

        r.eid1 = parser.getInt64At(pos).value_or(0);
        pos += intW;
        r.eid2 = parser.getInt64At(pos).value_or(0);
        pos += intW;
        r.deide = static_cast<int>(parser.getInt64At(pos).value_or(1));

        if (r.eid1 != 0) {
            ranges_.push_back(r);
        }
    }

    return true;
}

std::vector<std::string> SetSolidGenerate::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(sid_);
    result.push_back(writer.getLine());

    for (const auto& r : ranges_) {
        writer.clear();
        writer.writeInt(r.eid1);
        writer.writeInt(r.eid2);
        writer.writeInt(r.deide);
        result.push_back(writer.getLine());
    }

    return result;
}

void SetSolidGenerate::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(SetSolidGenerate, "*SET_SOLID_GENERATE")

// ============================================================================
// SetBeam
// ============================================================================

bool SetBeam::parse(const std::vector<std::string>& lines,
                     util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    elements_.clear();
    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    parser.setLine(lines[lineIdx]);
    sid_ = static_cast<int>(parser.getInt64At(0).value_or(0));
    ++lineIdx;

    for (; lineIdx < lines.size(); ++lineIdx) {
        if (util::CardParser::isCommentLine(lines[lineIdx])) continue;

        parser.setLine(lines[lineIdx]);
        for (size_t i = 0; i < 8; ++i) {
            auto eid = parser.getInt64At(i * intW);
            if (eid && *eid != 0) {
                elements_.push_back(*eid);
            }
        }
    }

    return true;
}

std::vector<std::string> SetBeam::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(sid_);
    result.push_back(writer.getLine());

    size_t count = 0;
    writer.clear();
    for (auto eid : elements_) {
        writer.writeInt(eid);
        ++count;
        if (count == 8) {
            result.push_back(writer.getLine());
            writer.clear();
            count = 0;
        }
    }
    if (count > 0) {
        result.push_back(writer.getLine());
    }

    return result;
}

void SetBeam::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(SetBeam, "*SET_BEAM")

// ============================================================================
// SetBeamGenerate
// ============================================================================

bool SetBeamGenerate::parse(const std::vector<std::string>& lines,
                             util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    ranges_.clear();
    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    parser.setLine(lines[lineIdx]);
    sid_ = static_cast<int>(parser.getInt64At(0).value_or(0));
    ++lineIdx;

    for (; lineIdx < lines.size(); ++lineIdx) {
        if (util::CardParser::isCommentLine(lines[lineIdx])) continue;

        parser.setLine(lines[lineIdx]);
        Range r;
        size_t pos = 0;

        r.eid1 = parser.getInt64At(pos).value_or(0);
        pos += intW;
        r.eid2 = parser.getInt64At(pos).value_or(0);
        pos += intW;
        r.deide = static_cast<int>(parser.getInt64At(pos).value_or(1));

        if (r.eid1 != 0) {
            ranges_.push_back(r);
        }
    }

    return true;
}

std::vector<std::string> SetBeamGenerate::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(sid_);
    result.push_back(writer.getLine());

    for (const auto& r : ranges_) {
        writer.clear();
        writer.writeInt(r.eid1);
        writer.writeInt(r.eid2);
        writer.writeInt(r.deide);
        result.push_back(writer.getLine());
    }

    return result;
}

void SetBeamGenerate::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(SetBeamGenerate, "*SET_BEAM_GENERATE")

// ============================================================================
// SetDiscrete
// ============================================================================

bool SetDiscrete::parse(const std::vector<std::string>& lines,
                         util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    elements_.clear();
    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    parser.setLine(lines[lineIdx]);
    sid_ = static_cast<int>(parser.getInt64At(0).value_or(0));
    ++lineIdx;

    for (; lineIdx < lines.size(); ++lineIdx) {
        if (util::CardParser::isCommentLine(lines[lineIdx])) continue;

        parser.setLine(lines[lineIdx]);
        for (size_t i = 0; i < 8; ++i) {
            auto eid = parser.getInt64At(i * intW);
            if (eid && *eid != 0) {
                elements_.push_back(*eid);
            }
        }
    }

    return true;
}

std::vector<std::string> SetDiscrete::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(sid_);
    result.push_back(writer.getLine());

    size_t count = 0;
    writer.clear();
    for (auto eid : elements_) {
        writer.writeInt(eid);
        ++count;
        if (count == 8) {
            result.push_back(writer.getLine());
            writer.clear();
            count = 0;
        }
    }
    if (count > 0) {
        result.push_back(writer.getLine());
    }

    return result;
}

void SetDiscrete::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(SetDiscrete, "*SET_DISCRETE")

// ============================================================================
// SetNodeAdd
// ============================================================================

bool SetNodeAdd::parse(const std::vector<std::string>& lines,
                        util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    nodes_.clear();
    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    parser.setLine(lines[lineIdx]);
    sid_ = static_cast<int>(parser.getInt64At(0).value_or(0));
    ++lineIdx;

    for (; lineIdx < lines.size(); ++lineIdx) {
        if (util::CardParser::isCommentLine(lines[lineIdx])) continue;

        parser.setLine(lines[lineIdx]);
        for (size_t i = 0; i < 8; ++i) {
            auto nid = parser.getInt64At(i * intW);
            if (nid && *nid != 0) {
                nodes_.push_back(*nid);
            }
        }
    }

    return true;
}

std::vector<std::string> SetNodeAdd::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(sid_);
    result.push_back(writer.getLine());

    size_t count = 0;
    writer.clear();
    for (auto nid : nodes_) {
        writer.writeInt(nid);
        ++count;
        if (count == 8) {
            result.push_back(writer.getLine());
            writer.clear();
            count = 0;
        }
    }
    if (count > 0) {
        result.push_back(writer.getLine());
    }

    return result;
}

void SetNodeAdd::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(SetNodeAdd, "*SET_NODE_ADD")

// ============================================================================
// SetPartAdd
// ============================================================================

bool SetPartAdd::parse(const std::vector<std::string>& lines,
                        util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    parts_.clear();
    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    parser.setLine(lines[lineIdx]);
    sid_ = static_cast<int>(parser.getInt64At(0).value_or(0));
    ++lineIdx;

    for (; lineIdx < lines.size(); ++lineIdx) {
        if (util::CardParser::isCommentLine(lines[lineIdx])) continue;

        parser.setLine(lines[lineIdx]);
        for (size_t i = 0; i < 8; ++i) {
            auto pid = parser.getInt64At(i * intW);
            if (pid && *pid != 0) {
                parts_.push_back(*pid);
            }
        }
    }

    return true;
}

std::vector<std::string> SetPartAdd::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(sid_);
    result.push_back(writer.getLine());

    size_t count = 0;
    writer.clear();
    for (auto pid : parts_) {
        writer.writeInt(pid);
        ++count;
        if (count == 8) {
            result.push_back(writer.getLine());
            writer.clear();
            count = 0;
        }
    }
    if (count > 0) {
        result.push_back(writer.getLine());
    }

    return result;
}

void SetPartAdd::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(SetPartAdd, "*SET_PART_ADD")

// ============================================================================
// SetNodeColumn
// ============================================================================

bool SetNodeColumn::parse(const std::vector<std::string>& lines,
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
    data_.sid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.da1 = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.da2 = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.da3 = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.da4 = static_cast<int>(parser.getInt64At(pos).value_or(0));

    return true;
}

std::vector<std::string> SetNodeColumn::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.sid);
    writer.writeInt(data_.da1);
    writer.writeInt(data_.da2);
    writer.writeInt(data_.da3);
    writer.writeInt(data_.da4);
    result.push_back(writer.getLine());

    return result;
}

void SetNodeColumn::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(SetNodeColumn, "*SET_NODE_COLUMN")

// ============================================================================
// SetSegmentGeneral
// ============================================================================

bool SetSegmentGeneral::parse(const std::vector<std::string>& lines,
                               util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    data_.clear();
    size_t lineIdx = 0;

    for (; lineIdx < lines.size(); ++lineIdx) {
        if (util::CardParser::isCommentLine(lines[lineIdx]) ||
            util::CardParser::isKeywordLine(lines[lineIdx])) {
            continue;
        }

        parser.setLine(lines[lineIdx]);
        size_t pos = 0;

        Data d;
        d.sid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        d.option = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        d.e1 = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        d.e2 = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        d.e3 = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        d.e4 = static_cast<int>(parser.getInt64At(pos).value_or(0));

        if (d.sid != 0) {
            data_.push_back(d);
        }
    }

    return true;
}

std::vector<std::string> SetSegmentGeneral::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& d : data_) {
        writer.clear();
        writer.writeInt(d.sid);
        writer.writeInt(d.option);
        writer.writeInt(d.e1);
        writer.writeInt(d.e2);
        writer.writeInt(d.e3);
        writer.writeInt(d.e4);
        result.push_back(writer.getLine());
    }

    return result;
}

void SetSegmentGeneral::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(SetSegmentGeneral, "*SET_SEGMENT_GENERAL")

// ============================================================================
// Set2dShell
// ============================================================================

bool Set2dShell::parse(const std::vector<std::string>& lines,
                        util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    elements_.clear();
    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    parser.setLine(lines[lineIdx]);
    sid_ = static_cast<int>(parser.getInt64At(0).value_or(0));
    ++lineIdx;

    for (; lineIdx < lines.size(); ++lineIdx) {
        if (util::CardParser::isCommentLine(lines[lineIdx])) continue;

        parser.setLine(lines[lineIdx]);
        for (size_t i = 0; i < 8; ++i) {
            auto eid = parser.getInt64At(i * intW);
            if (eid && *eid != 0) {
                elements_.push_back(*eid);
            }
        }
    }

    return true;
}

std::vector<std::string> Set2dShell::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(sid_);
    result.push_back(writer.getLine());

    size_t count = 0;
    writer.clear();
    for (auto eid : elements_) {
        writer.writeInt(eid);
        ++count;
        if (count == 8) {
            result.push_back(writer.getLine());
            writer.clear();
            count = 0;
        }
    }
    if (count > 0) {
        result.push_back(writer.getLine());
    }

    return result;
}

void Set2dShell::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(Set2dShell, "*SET_2D_SHELL")

// ============================================================================
// SetTshell
// ============================================================================

bool SetTshell::parse(const std::vector<std::string>& lines,
                       util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    elements_.clear();
    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    parser.setLine(lines[lineIdx]);
    sid_ = static_cast<int>(parser.getInt64At(0).value_or(0));
    ++lineIdx;

    for (; lineIdx < lines.size(); ++lineIdx) {
        if (util::CardParser::isCommentLine(lines[lineIdx])) continue;

        parser.setLine(lines[lineIdx]);
        for (size_t i = 0; i < 8; ++i) {
            auto eid = parser.getInt64At(i * intW);
            if (eid && *eid != 0) {
                elements_.push_back(*eid);
            }
        }
    }

    return true;
}

std::vector<std::string> SetTshell::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(sid_);
    result.push_back(writer.getLine());

    size_t count = 0;
    writer.clear();
    for (auto eid : elements_) {
        writer.writeInt(eid);
        ++count;
        if (count == 8) {
            result.push_back(writer.getLine());
            writer.clear();
            count = 0;
        }
    }
    if (count > 0) {
        result.push_back(writer.getLine());
    }

    return result;
}

void SetTshell::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(SetTshell, "*SET_TSHELL")

// ============================================================================
// SetTshellList
// ============================================================================

bool SetTshellList::parse(const std::vector<std::string>& lines,
                           util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    elements_.clear();
    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    parser.setLine(lines[lineIdx]);
    sid_ = static_cast<int>(parser.getInt64At(0).value_or(0));
    ++lineIdx;

    for (; lineIdx < lines.size(); ++lineIdx) {
        if (util::CardParser::isCommentLine(lines[lineIdx])) continue;

        parser.setLine(lines[lineIdx]);
        for (size_t i = 0; i < 8; ++i) {
            auto eid = parser.getInt64At(i * intW);
            if (eid && *eid != 0) {
                elements_.push_back(*eid);
            }
        }
    }

    return true;
}

std::vector<std::string> SetTshellList::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(sid_);
    result.push_back(writer.getLine());

    size_t count = 0;
    writer.clear();
    for (auto eid : elements_) {
        writer.writeInt(eid);
        ++count;
        if (count == 8) {
            result.push_back(writer.getLine());
            writer.clear();
            count = 0;
        }
    }
    if (count > 0) {
        result.push_back(writer.getLine());
    }

    return result;
}

void SetTshellList::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(SetTshellList, "*SET_TSHELL_LIST")

// ============================================================================
// SetBeamList
// ============================================================================

bool SetBeamList::parse(const std::vector<std::string>& lines,
                         util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    elements_.clear();
    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    parser.setLine(lines[lineIdx]);
    sid_ = static_cast<int>(parser.getInt64At(0).value_or(0));
    ++lineIdx;

    for (; lineIdx < lines.size(); ++lineIdx) {
        if (util::CardParser::isCommentLine(lines[lineIdx])) continue;

        parser.setLine(lines[lineIdx]);
        for (size_t i = 0; i < 8; ++i) {
            auto eid = parser.getInt64At(i * intW);
            if (eid && *eid != 0) {
                elements_.push_back(*eid);
            }
        }
    }

    return true;
}

std::vector<std::string> SetBeamList::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(sid_);
    result.push_back(writer.getLine());

    size_t count = 0;
    writer.clear();
    for (auto eid : elements_) {
        writer.writeInt(eid);
        ++count;
        if (count == 8) {
            result.push_back(writer.getLine());
            writer.clear();
            count = 0;
        }
    }
    if (count > 0) {
        result.push_back(writer.getLine());
    }

    return result;
}

void SetBeamList::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(SetBeamList, "*SET_BEAM_LIST")

// ============================================================================
// SetDiscreteList
// ============================================================================

bool SetDiscreteList::parse(const std::vector<std::string>& lines,
                             util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    elements_.clear();
    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    parser.setLine(lines[lineIdx]);
    sid_ = static_cast<int>(parser.getInt64At(0).value_or(0));
    ++lineIdx;

    for (; lineIdx < lines.size(); ++lineIdx) {
        if (util::CardParser::isCommentLine(lines[lineIdx])) continue;

        parser.setLine(lines[lineIdx]);
        for (size_t i = 0; i < 8; ++i) {
            auto eid = parser.getInt64At(i * intW);
            if (eid && *eid != 0) {
                elements_.push_back(*eid);
            }
        }
    }

    return true;
}

std::vector<std::string> SetDiscreteList::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(sid_);
    result.push_back(writer.getLine());

    size_t count = 0;
    writer.clear();
    for (auto eid : elements_) {
        writer.writeInt(eid);
        ++count;
        if (count == 8) {
            result.push_back(writer.getLine());
            writer.clear();
            count = 0;
        }
    }
    if (count > 0) {
        result.push_back(writer.getLine());
    }

    return result;
}

void SetDiscreteList::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(SetDiscreteList, "*SET_DISCRETE_LIST")

// ============================================================================
// SetNodeGeneral
// ============================================================================

bool SetNodeGeneral::parse(const std::vector<std::string>& lines,
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
    data_.sid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.da1 = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.da2 = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.da3 = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.da4 = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.solver = parser.getDoubleAt(pos, realW).value_or(0.0);

    return true;
}

std::vector<std::string> SetNodeGeneral::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.sid);
    writer.writeInt(data_.da1);
    writer.writeInt(data_.da2);
    writer.writeInt(data_.da3);
    writer.writeInt(data_.da4);
    writer.writeDouble(data_.solver);
    result.push_back(writer.getLine());

    return result;
}

void SetNodeGeneral::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(SetNodeGeneral, "*SET_NODE_GENERAL")

// ============================================================================
// SetPartTree
// ============================================================================

bool SetPartTree::parse(const std::vector<std::string>& lines,
                         util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    data_.clear();

    for (size_t lineIdx = 0; lineIdx < lines.size(); ++lineIdx) {
        if (util::CardParser::isCommentLine(lines[lineIdx]) ||
            util::CardParser::isKeywordLine(lines[lineIdx])) {
            continue;
        }

        parser.setLine(lines[lineIdx]);
        size_t pos = 0;

        Data d;
        d.sid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        d.pid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        d.level = static_cast<int>(parser.getInt64At(pos).value_or(0));

        if (d.sid != 0) {
            data_.push_back(d);
        }
    }

    return true;
}

std::vector<std::string> SetPartTree::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& d : data_) {
        writer.clear();
        writer.writeInt(d.sid);
        writer.writeInt(d.pid);
        writer.writeInt(d.level);
        result.push_back(writer.getLine());
    }

    return result;
}

void SetPartTree::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(SetPartTree, "*SET_PART_TREE")

// ============================================================================
// SetMultiMaterialGroupList
// ============================================================================

bool SetMultiMaterialGroupList::parse(const std::vector<std::string>& lines,
                                       util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    groups_.clear();
    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    parser.setLine(lines[lineIdx]);
    sid_ = static_cast<int>(parser.getInt64At(0).value_or(0));
    ++lineIdx;

    for (; lineIdx < lines.size(); ++lineIdx) {
        if (util::CardParser::isCommentLine(lines[lineIdx])) continue;

        parser.setLine(lines[lineIdx]);
        for (size_t i = 0; i < 8; ++i) {
            auto gid = parser.getInt64At(i * intW);
            if (gid && *gid != 0) {
                groups_.push_back(static_cast<int>(*gid));
            }
        }
    }

    return true;
}

std::vector<std::string> SetMultiMaterialGroupList::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(sid_);
    result.push_back(writer.getLine());

    size_t count = 0;
    writer.clear();
    for (auto gid : groups_) {
        writer.writeInt(gid);
        ++count;
        if (count == 8) {
            result.push_back(writer.getLine());
            writer.clear();
            count = 0;
        }
    }
    if (count > 0) {
        result.push_back(writer.getLine());
    }

    return result;
}

void SetMultiMaterialGroupList::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(SetMultiMaterialGroupList, "*SET_MULTI_MATERIAL_GROUP_LIST")

// ============================================================================
// SetIgaEdgeUvw
// ============================================================================

bool SetIgaEdgeUvw::parse(const std::vector<std::string>& lines,
                           util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    data_.clear();

    for (size_t lineIdx = 0; lineIdx < lines.size(); ++lineIdx) {
        if (util::CardParser::isCommentLine(lines[lineIdx]) ||
            util::CardParser::isKeywordLine(lines[lineIdx])) {
            continue;
        }

        parser.setLine(lines[lineIdx]);
        size_t pos = 0;

        Data d;
        d.sid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        d.pid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        d.u = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        d.v = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        d.w = parser.getDoubleAt(pos, realW).value_or(0.0);

        if (d.sid != 0) {
            data_.push_back(d);
        }
    }

    return true;
}

std::vector<std::string> SetIgaEdgeUvw::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& d : data_) {
        writer.clear();
        writer.writeInt(d.sid);
        writer.writeInt(d.pid);
        writer.writeDouble(d.u);
        writer.writeDouble(d.v);
        writer.writeDouble(d.w);
        result.push_back(writer.getLine());
    }

    return result;
}

void SetIgaEdgeUvw::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(SetIgaEdgeUvw, "*SET_IGA_EDGE_UVW")

// ============================================================================
// SetIgaFaceUvw
// ============================================================================

bool SetIgaFaceUvw::parse(const std::vector<std::string>& lines,
                           util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    data_.clear();

    for (size_t lineIdx = 0; lineIdx < lines.size(); ++lineIdx) {
        if (util::CardParser::isCommentLine(lines[lineIdx]) ||
            util::CardParser::isKeywordLine(lines[lineIdx])) {
            continue;
        }

        parser.setLine(lines[lineIdx]);
        size_t pos = 0;

        Data d;
        d.sid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        d.pid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        d.u = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        d.v = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        d.w = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        d.idir = static_cast<int>(parser.getInt64At(pos).value_or(0));

        if (d.sid != 0) {
            data_.push_back(d);
        }
    }

    return true;
}

std::vector<std::string> SetIgaFaceUvw::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& d : data_) {
        writer.clear();
        writer.writeInt(d.sid);
        writer.writeInt(d.pid);
        writer.writeDouble(d.u);
        writer.writeDouble(d.v);
        writer.writeDouble(d.w);
        writer.writeInt(d.idir);
        result.push_back(writer.getLine());
    }

    return result;
}

void SetIgaFaceUvw::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(SetIgaFaceUvw, "*SET_IGA_FACE_UVW")

// ============================================================================
// SetNodeIntersect
// ============================================================================

bool SetNodeIntersect::parse(const std::vector<std::string>& lines,
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
    data_.sid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.sid1 = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.sid2 = static_cast<int>(parser.getInt64At(pos).value_or(0));

    return true;
}

std::vector<std::string> SetNodeIntersect::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.sid);
    writer.writeInt(data_.sid1);
    writer.writeInt(data_.sid2);
    result.push_back(writer.getLine());

    return result;
}

void SetNodeIntersect::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(SetNodeIntersect, "*SET_NODE_INTERSECT")

// ============================================================================
// SetPartIntersect
// ============================================================================

bool SetPartIntersect::parse(const std::vector<std::string>& lines,
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
    data_.sid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.sid1 = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.sid2 = static_cast<int>(parser.getInt64At(pos).value_or(0));

    return true;
}

std::vector<std::string> SetPartIntersect::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.sid);
    writer.writeInt(data_.sid1);
    writer.writeInt(data_.sid2);
    result.push_back(writer.getLine());

    return result;
}

void SetPartIntersect::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(SetPartIntersect, "*SET_PART_INTERSECT")

// ============================================================================
// SetSeatbelt
// ============================================================================

bool SetSeatbelt::parse(const std::vector<std::string>& lines,
                         util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    elements_.clear();
    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    parser.setLine(lines[lineIdx]);
    sid_ = static_cast<int>(parser.getInt64At(0).value_or(0));
    ++lineIdx;

    for (; lineIdx < lines.size(); ++lineIdx) {
        if (util::CardParser::isCommentLine(lines[lineIdx])) continue;

        parser.setLine(lines[lineIdx]);
        for (size_t i = 0; i < 8; ++i) {
            auto eid = parser.getInt64At(i * intW);
            if (eid && *eid != 0) {
                elements_.push_back(*eid);
            }
        }
    }

    return true;
}

std::vector<std::string> SetSeatbelt::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(sid_);
    result.push_back(writer.getLine());

    size_t count = 0;
    writer.clear();
    for (auto eid : elements_) {
        writer.writeInt(eid);
        ++count;
        if (count == 8) {
            result.push_back(writer.getLine());
            writer.clear();
            count = 0;
        }
    }
    if (count > 0) {
        result.push_back(writer.getLine());
    }

    return result;
}

void SetSeatbelt::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(SetSeatbelt, "*SET_SEATBELT")

// ============================================================================
// SetShellListTitle
// ============================================================================

bool SetShellListTitle::parse(const std::vector<std::string>& lines,
                               util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    elements_.clear();
    size_t lineIdx = 0;

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

    parser.setLine(lines[lineIdx]);
    sid_ = static_cast<int>(parser.getInt64At(0).value_or(0));
    ++lineIdx;

    for (; lineIdx < lines.size(); ++lineIdx) {
        if (util::CardParser::isCommentLine(lines[lineIdx])) continue;

        parser.setLine(lines[lineIdx]);
        for (size_t i = 0; i < 8; ++i) {
            auto eid = parser.getInt64At(i * intW);
            if (eid && *eid != 0) {
                elements_.push_back(*eid);
            }
        }
    }

    return true;
}

std::vector<std::string> SetShellListTitle::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    result.push_back(title_);

    writer.writeInt(sid_);
    result.push_back(writer.getLine());

    size_t count = 0;
    writer.clear();
    for (auto eid : elements_) {
        writer.writeInt(eid);
        ++count;
        if (count == 8) {
            result.push_back(writer.getLine());
            writer.clear();
            count = 0;
        }
    }
    if (count > 0) {
        result.push_back(writer.getLine());
    }

    return result;
}

void SetShellListTitle::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(SetShellListTitle, "*SET_SHELL_LIST_TITLE")

// ============================================================================
// SetSolidListTitle
// ============================================================================

bool SetSolidListTitle::parse(const std::vector<std::string>& lines,
                               util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    elements_.clear();
    size_t lineIdx = 0;

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

    parser.setLine(lines[lineIdx]);
    sid_ = static_cast<int>(parser.getInt64At(0).value_or(0));
    ++lineIdx;

    for (; lineIdx < lines.size(); ++lineIdx) {
        if (util::CardParser::isCommentLine(lines[lineIdx])) continue;

        parser.setLine(lines[lineIdx]);
        for (size_t i = 0; i < 8; ++i) {
            auto eid = parser.getInt64At(i * intW);
            if (eid && *eid != 0) {
                elements_.push_back(*eid);
            }
        }
    }

    return true;
}

std::vector<std::string> SetSolidListTitle::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    result.push_back(title_);

    writer.writeInt(sid_);
    result.push_back(writer.getLine());

    size_t count = 0;
    writer.clear();
    for (auto eid : elements_) {
        writer.writeInt(eid);
        ++count;
        if (count == 8) {
            result.push_back(writer.getLine());
            writer.clear();
            count = 0;
        }
    }
    if (count > 0) {
        result.push_back(writer.getLine());
    }

    return result;
}

void SetSolidListTitle::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(SetSolidListTitle, "*SET_SOLID_LIST_TITLE")

// ============================================================================
// SetBeamListTitle
// ============================================================================

bool SetBeamListTitle::parse(const std::vector<std::string>& lines,
                              util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    elements_.clear();
    size_t lineIdx = 0;

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

    parser.setLine(lines[lineIdx]);
    sid_ = static_cast<int>(parser.getInt64At(0).value_or(0));
    ++lineIdx;

    for (; lineIdx < lines.size(); ++lineIdx) {
        if (util::CardParser::isCommentLine(lines[lineIdx])) continue;

        parser.setLine(lines[lineIdx]);
        for (size_t i = 0; i < 8; ++i) {
            auto eid = parser.getInt64At(i * intW);
            if (eid && *eid != 0) {
                elements_.push_back(*eid);
            }
        }
    }

    return true;
}

std::vector<std::string> SetBeamListTitle::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    result.push_back(title_);

    writer.writeInt(sid_);
    result.push_back(writer.getLine());

    size_t count = 0;
    writer.clear();
    for (auto eid : elements_) {
        writer.writeInt(eid);
        ++count;
        if (count == 8) {
            result.push_back(writer.getLine());
            writer.clear();
            count = 0;
        }
    }
    if (count > 0) {
        result.push_back(writer.getLine());
    }

    return result;
}

void SetBeamListTitle::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(SetBeamListTitle, "*SET_BEAM_LIST_TITLE")

// ============================================================================
// SetDiscreteListTitle
// ============================================================================

bool SetDiscreteListTitle::parse(const std::vector<std::string>& lines,
                                  util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    elements_.clear();
    size_t lineIdx = 0;

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

    parser.setLine(lines[lineIdx]);
    sid_ = static_cast<int>(parser.getInt64At(0).value_or(0));
    ++lineIdx;

    for (; lineIdx < lines.size(); ++lineIdx) {
        if (util::CardParser::isCommentLine(lines[lineIdx])) continue;

        parser.setLine(lines[lineIdx]);
        for (size_t i = 0; i < 8; ++i) {
            auto eid = parser.getInt64At(i * intW);
            if (eid && *eid != 0) {
                elements_.push_back(*eid);
            }
        }
    }

    return true;
}

std::vector<std::string> SetDiscreteListTitle::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    result.push_back(title_);

    writer.writeInt(sid_);
    result.push_back(writer.getLine());

    size_t count = 0;
    writer.clear();
    for (auto eid : elements_) {
        writer.writeInt(eid);
        ++count;
        if (count == 8) {
            result.push_back(writer.getLine());
            writer.clear();
            count = 0;
        }
    }
    if (count > 0) {
        result.push_back(writer.getLine());
    }

    return result;
}

void SetDiscreteListTitle::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(SetDiscreteListTitle, "*SET_DISCRETE_LIST_TITLE")

// ============================================================================
// SetNodeTitle
// ============================================================================

bool SetNodeTitle::parse(const std::vector<std::string>& lines,
                          util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    nodes_.clear();
    size_t lineIdx = 0;

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

    parser.setLine(lines[lineIdx]);
    sid_ = static_cast<int>(parser.getInt64At(0).value_or(0));
    ++lineIdx;

    for (; lineIdx < lines.size(); ++lineIdx) {
        if (util::CardParser::isCommentLine(lines[lineIdx])) continue;

        parser.setLine(lines[lineIdx]);
        for (size_t i = 0; i < 8; ++i) {
            auto nid = parser.getInt64At(i * intW);
            if (nid && *nid != 0) {
                nodes_.push_back(*nid);
            }
        }
    }

    return true;
}

std::vector<std::string> SetNodeTitle::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    result.push_back(title_);

    writer.writeInt(sid_);
    result.push_back(writer.getLine());

    size_t count = 0;
    writer.clear();
    for (auto nid : nodes_) {
        writer.writeInt(nid);
        ++count;
        if (count == 8) {
            result.push_back(writer.getLine());
            writer.clear();
            count = 0;
        }
    }
    if (count > 0) {
        result.push_back(writer.getLine());
    }

    return result;
}

void SetNodeTitle::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(SetNodeTitle, "*SET_NODE_TITLE")

// ============================================================================
// SetPartTitle
// ============================================================================

bool SetPartTitle::parse(const std::vector<std::string>& lines,
                          util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    parts_.clear();
    size_t lineIdx = 0;

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

    parser.setLine(lines[lineIdx]);
    sid_ = static_cast<int>(parser.getInt64At(0).value_or(0));
    ++lineIdx;

    for (; lineIdx < lines.size(); ++lineIdx) {
        if (util::CardParser::isCommentLine(lines[lineIdx])) continue;

        parser.setLine(lines[lineIdx]);
        for (size_t i = 0; i < 8; ++i) {
            auto pid = parser.getInt64At(i * intW);
            if (pid && *pid != 0) {
                parts_.push_back(*pid);
            }
        }
    }

    return true;
}

std::vector<std::string> SetPartTitle::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    result.push_back(title_);

    writer.writeInt(sid_);
    result.push_back(writer.getLine());

    size_t count = 0;
    writer.clear();
    for (auto pid : parts_) {
        writer.writeInt(pid);
        ++count;
        if (count == 8) {
            result.push_back(writer.getLine());
            writer.clear();
            count = 0;
        }
    }
    if (count > 0) {
        result.push_back(writer.getLine());
    }

    return result;
}

void SetPartTitle::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(SetPartTitle, "*SET_PART_TITLE")

// ============================================================================
// SetShellGenerateTitle
// ============================================================================

bool SetShellGenerateTitle::parse(const std::vector<std::string>& lines,
                                   util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    ranges_.clear();
    size_t lineIdx = 0;

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

    parser.setLine(lines[lineIdx]);
    sid_ = static_cast<int>(parser.getInt64At(0).value_or(0));
    ++lineIdx;

    for (; lineIdx < lines.size(); ++lineIdx) {
        if (util::CardParser::isCommentLine(lines[lineIdx])) continue;

        parser.setLine(lines[lineIdx]);
        Range r;
        r.eid1 = parser.getInt64At(0).value_or(0);
        r.eid2 = parser.getInt64At(intW).value_or(0);
        r.deide = static_cast<int>(parser.getInt64At(2 * intW).value_or(1));
        if (r.eid1 != 0 || r.eid2 != 0) {
            ranges_.push_back(r);
        }
    }

    return true;
}

std::vector<std::string> SetShellGenerateTitle::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    result.push_back(title_);

    writer.writeInt(sid_);
    result.push_back(writer.getLine());

    for (const auto& r : ranges_) {
        writer.clear();
        writer.writeInt(r.eid1);
        writer.writeInt(r.eid2);
        writer.writeInt(r.deide);
        result.push_back(writer.getLine());
    }

    return result;
}

void SetShellGenerateTitle::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(SetShellGenerateTitle, "*SET_SHELL_GENERATE_TITLE")

// ============================================================================
// SetSolidGenerateTitle
// ============================================================================

bool SetSolidGenerateTitle::parse(const std::vector<std::string>& lines,
                                   util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    ranges_.clear();
    size_t lineIdx = 0;

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

    parser.setLine(lines[lineIdx]);
    sid_ = static_cast<int>(parser.getInt64At(0).value_or(0));
    ++lineIdx;

    for (; lineIdx < lines.size(); ++lineIdx) {
        if (util::CardParser::isCommentLine(lines[lineIdx])) continue;

        parser.setLine(lines[lineIdx]);
        Range r;
        r.eid1 = parser.getInt64At(0).value_or(0);
        r.eid2 = parser.getInt64At(intW).value_or(0);
        r.deide = static_cast<int>(parser.getInt64At(2 * intW).value_or(1));
        if (r.eid1 != 0 || r.eid2 != 0) {
            ranges_.push_back(r);
        }
    }

    return true;
}

std::vector<std::string> SetSolidGenerateTitle::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    result.push_back(title_);

    writer.writeInt(sid_);
    result.push_back(writer.getLine());

    for (const auto& r : ranges_) {
        writer.clear();
        writer.writeInt(r.eid1);
        writer.writeInt(r.eid2);
        writer.writeInt(r.deide);
        result.push_back(writer.getLine());
    }

    return result;
}

void SetSolidGenerateTitle::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(SetSolidGenerateTitle, "*SET_SOLID_GENERATE_TITLE")

// ============================================================================
// SetBeamGenerateTitle
// ============================================================================

bool SetBeamGenerateTitle::parse(const std::vector<std::string>& lines,
                                  util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    ranges_.clear();
    size_t lineIdx = 0;

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

    parser.setLine(lines[lineIdx]);
    sid_ = static_cast<int>(parser.getInt64At(0).value_or(0));
    ++lineIdx;

    for (; lineIdx < lines.size(); ++lineIdx) {
        if (util::CardParser::isCommentLine(lines[lineIdx])) continue;

        parser.setLine(lines[lineIdx]);
        Range r;
        r.eid1 = parser.getInt64At(0).value_or(0);
        r.eid2 = parser.getInt64At(intW).value_or(0);
        r.deide = static_cast<int>(parser.getInt64At(2 * intW).value_or(1));
        if (r.eid1 != 0 || r.eid2 != 0) {
            ranges_.push_back(r);
        }
    }

    return true;
}

std::vector<std::string> SetBeamGenerateTitle::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    result.push_back(title_);

    writer.writeInt(sid_);
    result.push_back(writer.getLine());

    for (const auto& r : ranges_) {
        writer.clear();
        writer.writeInt(r.eid1);
        writer.writeInt(r.eid2);
        writer.writeInt(r.deide);
        result.push_back(writer.getLine());
    }

    return result;
}

void SetBeamGenerateTitle::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(SetBeamGenerateTitle, "*SET_BEAM_GENERATE_TITLE")

// ============================================================================
// SetNodeGenerateTitle
// ============================================================================

bool SetNodeGenerateTitle::parse(const std::vector<std::string>& lines,
                                  util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    ranges_.clear();
    size_t lineIdx = 0;

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

    parser.setLine(lines[lineIdx]);
    sid_ = static_cast<int>(parser.getInt64At(0).value_or(0));
    ++lineIdx;

    for (; lineIdx < lines.size(); ++lineIdx) {
        if (util::CardParser::isCommentLine(lines[lineIdx])) continue;

        parser.setLine(lines[lineIdx]);
        Range r;
        r.nid1 = parser.getInt64At(0).value_or(0);
        r.nid2 = parser.getInt64At(intW).value_or(0);
        r.dnidn = static_cast<int>(parser.getInt64At(2 * intW).value_or(1));
        if (r.nid1 != 0 || r.nid2 != 0) {
            ranges_.push_back(r);
        }
    }

    return true;
}

std::vector<std::string> SetNodeGenerateTitle::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    result.push_back(title_);

    writer.writeInt(sid_);
    result.push_back(writer.getLine());

    for (const auto& r : ranges_) {
        writer.clear();
        writer.writeInt(r.nid1);
        writer.writeInt(r.nid2);
        writer.writeInt(r.dnidn);
        result.push_back(writer.getLine());
    }

    return result;
}

void SetNodeGenerateTitle::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(SetNodeGenerateTitle, "*SET_NODE_GENERATE_TITLE")

// ============================================================================
// SetPartGenerateTitle
// ============================================================================

bool SetPartGenerateTitle::parse(const std::vector<std::string>& lines,
                                  util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    ranges_.clear();
    size_t lineIdx = 0;

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

    parser.setLine(lines[lineIdx]);
    sid_ = static_cast<int>(parser.getInt64At(0).value_or(0));
    ++lineIdx;

    for (; lineIdx < lines.size(); ++lineIdx) {
        if (util::CardParser::isCommentLine(lines[lineIdx])) continue;

        parser.setLine(lines[lineIdx]);
        Range r;
        r.pid1 = parser.getInt64At(0).value_or(0);
        r.pid2 = parser.getInt64At(intW).value_or(0);
        r.dpidp = static_cast<int>(parser.getInt64At(2 * intW).value_or(1));
        if (r.pid1 != 0 || r.pid2 != 0) {
            ranges_.push_back(r);
        }
    }

    return true;
}

std::vector<std::string> SetPartGenerateTitle::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    result.push_back(title_);

    writer.writeInt(sid_);
    result.push_back(writer.getLine());

    for (const auto& r : ranges_) {
        writer.clear();
        writer.writeInt(r.pid1);
        writer.writeInt(r.pid2);
        writer.writeInt(r.dpidp);
        result.push_back(writer.getLine());
    }

    return result;
}

void SetPartGenerateTitle::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(SetPartGenerateTitle, "*SET_PART_GENERATE_TITLE")

// ============================================================================
// SetBeamAdd
// ============================================================================

bool SetBeamAdd::parse(const std::vector<std::string>& lines,
                       util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    elements_.clear();
    bool foundSid = false;

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);
        if (!foundSid) {
            sid_ = static_cast<int>(parser.getInt64At(0).value_or(0));
            foundSid = true;
            continue;
        }

        for (size_t i = 0; i < 8; ++i) {
            auto eid = parser.getInt64At(i * intW);
            if (eid && *eid != 0) {
                elements_.push_back(*eid);
            }
        }
    }

    return true;
}

std::vector<std::string> SetBeamAdd::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(sid_);
    result.push_back(writer.getLine());

    size_t count = 0;
    writer.clear();
    for (auto eid : elements_) {
        writer.writeInt(eid);
        ++count;
        if (count == 8) {
            result.push_back(writer.getLine());
            writer.clear();
            count = 0;
        }
    }
    if (count > 0) {
        result.push_back(writer.getLine());
    }

    return result;
}

void SetBeamAdd::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(SetBeamAdd, "*SET_BEAM_ADD")

// ============================================================================
// SetBeamGeneral
// ============================================================================

bool SetBeamGeneral::parse(const std::vector<std::string>& lines,
                           util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);
        size_t pos = 0;
        data_.sid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.option = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.da1 = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.da2 = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> SetBeamGeneral::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.sid);
    writer.writeInt(data_.option);
    writer.writeInt(data_.da1);
    writer.writeInt(data_.da2);
    result.push_back(writer.getLine());

    return result;
}

void SetBeamGeneral::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(SetBeamGeneral, "*SET_BEAM_GENERAL")

// ============================================================================
// SetBeamIntersect
// ============================================================================

bool SetBeamIntersect::parse(const std::vector<std::string>& lines,
                             util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);
        size_t pos = 0;
        data_.sid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.sid1 = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.sid2 = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> SetBeamIntersect::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.sid);
    writer.writeInt(data_.sid1);
    writer.writeInt(data_.sid2);
    result.push_back(writer.getLine());

    return result;
}

void SetBeamIntersect::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(SetBeamIntersect, "*SET_BEAM_INTERSECT")

// ============================================================================
// SetBox
// ============================================================================

bool SetBox::parse(const std::vector<std::string>& lines,
                   util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);
        size_t pos = 0;
        data_.boxid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.xmin = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.xmax = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.ymin = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.ymax = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.zmin = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.zmax = parser.getDoubleAt(pos, realW).value_or(0.0);
        break;
    }

    return true;
}

std::vector<std::string> SetBox::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.boxid);
    writer.writeDouble(data_.xmin);
    writer.writeDouble(data_.xmax);
    writer.writeDouble(data_.ymin);
    writer.writeDouble(data_.ymax);
    writer.writeDouble(data_.zmin);
    writer.writeDouble(data_.zmax);
    result.push_back(writer.getLine());

    return result;
}

void SetBox::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(SetBox, "*SET_BOX")

// ============================================================================
// SetDiscreteAdd
// ============================================================================

bool SetDiscreteAdd::parse(const std::vector<std::string>& lines,
                           util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    elements_.clear();
    bool foundSid = false;

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);
        if (!foundSid) {
            sid_ = static_cast<int>(parser.getInt64At(0).value_or(0));
            foundSid = true;
            continue;
        }

        for (size_t i = 0; i < 8; ++i) {
            auto eid = parser.getInt64At(i * intW);
            if (eid && *eid != 0) {
                elements_.push_back(*eid);
            }
        }
    }

    return true;
}

std::vector<std::string> SetDiscreteAdd::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(sid_);
    result.push_back(writer.getLine());

    size_t count = 0;
    writer.clear();
    for (auto eid : elements_) {
        writer.writeInt(eid);
        ++count;
        if (count == 8) {
            result.push_back(writer.getLine());
            writer.clear();
            count = 0;
        }
    }
    if (count > 0) {
        result.push_back(writer.getLine());
    }

    return result;
}

void SetDiscreteAdd::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(SetDiscreteAdd, "*SET_DISCRETE_ADD")

// ============================================================================
// SetDiscreteGeneral
// ============================================================================

bool SetDiscreteGeneral::parse(const std::vector<std::string>& lines,
                               util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);
        size_t pos = 0;
        data_.sid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.option = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.da1 = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.da2 = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> SetDiscreteGeneral::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.sid);
    writer.writeInt(data_.option);
    writer.writeInt(data_.da1);
    writer.writeInt(data_.da2);
    result.push_back(writer.getLine());

    return result;
}

void SetDiscreteGeneral::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(SetDiscreteGeneral, "*SET_DISCRETE_GENERAL")

// ============================================================================
// SetIgaEdge
// ============================================================================

bool SetIgaEdge::parse(const std::vector<std::string>& lines,
                       util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    data_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);
        size_t pos = 0;

        auto sid = parser.getInt64At(pos);
        if (!sid) continue;

        Data entry;
        entry.sid = static_cast<int>(*sid);
        pos += intW;
        entry.pid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.edge = static_cast<int>(parser.getInt64At(pos).value_or(0));
        data_.push_back(entry);
    }

    return true;
}

std::vector<std::string> SetIgaEdge::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& entry : data_) {
        writer.clear();
        writer.writeInt(entry.sid);
        writer.writeInt(entry.pid);
        writer.writeInt(entry.edge);
        result.push_back(writer.getLine());
    }

    return result;
}

void SetIgaEdge::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(SetIgaEdge, "*SET_IGA_EDGE")

// ============================================================================
// SetIgaFace
// ============================================================================

bool SetIgaFace::parse(const std::vector<std::string>& lines,
                       util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    data_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);
        size_t pos = 0;

        auto sid = parser.getInt64At(pos);
        if (!sid) continue;

        Data entry;
        entry.sid = static_cast<int>(*sid);
        pos += intW;
        entry.pid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.face = static_cast<int>(parser.getInt64At(pos).value_or(0));
        data_.push_back(entry);
    }

    return true;
}

std::vector<std::string> SetIgaFace::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& entry : data_) {
        writer.clear();
        writer.writeInt(entry.sid);
        writer.writeInt(entry.pid);
        writer.writeInt(entry.face);
        result.push_back(writer.getLine());
    }

    return result;
}

void SetIgaFace::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(SetIgaFace, "*SET_IGA_FACE")

// ============================================================================
// SetIgaPointUvw
// ============================================================================

bool SetIgaPointUvw::parse(const std::vector<std::string>& lines,
                           util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    data_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);
        size_t pos = 0;

        auto sid = parser.getInt64At(pos);
        if (!sid) continue;

        Data entry;
        entry.sid = static_cast<int>(*sid);
        pos += intW;
        entry.pid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.u = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        entry.v = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        entry.w = parser.getDoubleAt(pos, realW).value_or(0.0);
        data_.push_back(entry);
    }

    return true;
}

std::vector<std::string> SetIgaPointUvw::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& entry : data_) {
        writer.clear();
        writer.writeInt(entry.sid);
        writer.writeInt(entry.pid);
        writer.writeDouble(entry.u);
        writer.writeDouble(entry.v);
        writer.writeDouble(entry.w);
        result.push_back(writer.getLine());
    }

    return result;
}

void SetIgaPointUvw::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(SetIgaPointUvw, "*SET_IGA_POINT_UVW")

// ============================================================================
// SetMode
// ============================================================================

bool SetMode::parse(const std::vector<std::string>& lines,
                    util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    modes_.clear();
    bool foundSid = false;

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);
        if (!foundSid) {
            sid_ = static_cast<int>(parser.getInt64At(0).value_or(0));
            foundSid = true;
            continue;
        }

        for (size_t i = 0; i < 8; ++i) {
            auto mode = parser.getInt64At(i * intW);
            if (mode && *mode != 0) {
                modes_.push_back(static_cast<int>(*mode));
            }
        }
    }

    return true;
}

std::vector<std::string> SetMode::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(sid_);
    result.push_back(writer.getLine());

    size_t count = 0;
    writer.clear();
    for (auto mode : modes_) {
        writer.writeInt(mode);
        ++count;
        if (count == 8) {
            result.push_back(writer.getLine());
            writer.clear();
            count = 0;
        }
    }
    if (count > 0) {
        result.push_back(writer.getLine());
    }

    return result;
}

void SetMode::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(SetMode, "*SET_MODE")

// ============================================================================
// SetMulti
// ============================================================================

bool SetMulti::parse(const std::vector<std::string>& lines,
                     util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    data_.sets.clear();
    bool foundHeader = false;

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);
        if (!foundHeader) {
            size_t pos = 0;
            data_.sid = static_cast<int>(parser.getInt64At(pos).value_or(0));
            pos += intW;
            data_.settype = static_cast<int>(parser.getInt64At(pos).value_or(0));
            foundHeader = true;
            continue;
        }

        for (size_t i = 0; i < 8; ++i) {
            auto setid = parser.getInt64At(i * intW);
            if (setid && *setid != 0) {
                data_.sets.push_back(static_cast<int>(*setid));
            }
        }
    }

    return true;
}

std::vector<std::string> SetMulti::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.sid);
    writer.writeInt(data_.settype);
    result.push_back(writer.getLine());

    size_t count = 0;
    writer.clear();
    for (auto setid : data_.sets) {
        writer.writeInt(setid);
        ++count;
        if (count == 8) {
            result.push_back(writer.getLine());
            writer.clear();
            count = 0;
        }
    }
    if (count > 0) {
        result.push_back(writer.getLine());
    }

    return result;
}

void SetMulti::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(SetMulti, "*SET_MULTI")

// ============================================================================
// SetPartColumn
// ============================================================================

bool SetPartColumn::parse(const std::vector<std::string>& lines,
                          util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);
        size_t pos = 0;
        data_.sid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.da1 = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.da2 = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.da3 = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.da4 = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> SetPartColumn::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.sid);
    writer.writeInt(data_.da1);
    writer.writeInt(data_.da2);
    writer.writeInt(data_.da3);
    writer.writeInt(data_.da4);
    result.push_back(writer.getLine());

    return result;
}

void SetPartColumn::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(SetPartColumn, "*SET_PART_COLUMN")

// ============================================================================
// SetPartGeneral
// ============================================================================

bool SetPartGeneral::parse(const std::vector<std::string>& lines,
                           util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);
        size_t pos = 0;
        data_.sid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.option = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.da1 = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.da2 = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> SetPartGeneral::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.sid);
    writer.writeInt(data_.option);
    writer.writeInt(data_.da1);
    writer.writeInt(data_.da2);
    result.push_back(writer.getLine());

    return result;
}

void SetPartGeneral::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(SetPartGeneral, "*SET_PART_GENERAL")

// ============================================================================
// SetPartListGenerate
// ============================================================================

bool SetPartListGenerate::parse(const std::vector<std::string>& lines,
                                util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    ranges_.clear();
    bool foundSid = false;

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);
        if (!foundSid) {
            sid_ = static_cast<int>(parser.getInt64At(0).value_or(0));
            foundSid = true;
            continue;
        }

        Range r;
        r.pid1 = parser.getInt64At(0).value_or(0);
        r.pid2 = parser.getInt64At(intW).value_or(0);
        r.dpidp = static_cast<int>(parser.getInt64At(2 * intW).value_or(1));
        if (r.pid1 != 0 || r.pid2 != 0) {
            ranges_.push_back(r);
        }
    }

    return true;
}

std::vector<std::string> SetPartListGenerate::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(sid_);
    result.push_back(writer.getLine());

    for (const auto& r : ranges_) {
        writer.clear();
        writer.writeInt(r.pid1);
        writer.writeInt(r.pid2);
        writer.writeInt(r.dpidp);
        result.push_back(writer.getLine());
    }

    return result;
}

void SetPartListGenerate::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(SetPartListGenerate, "*SET_PART_LIST_GENERATE")

// ============================================================================
// SetSegmentAdd
// ============================================================================

bool SetSegmentAdd::parse(const std::vector<std::string>& lines,
                          util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    segments_.clear();
    bool foundSid = false;

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);
        if (!foundSid) {
            sid_ = static_cast<int>(parser.getInt64At(0).value_or(0));
            foundSid = true;
            continue;
        }

        Segment seg;
        seg.n1 = parser.getInt64At(0).value_or(0);
        seg.n2 = parser.getInt64At(intW).value_or(0);
        seg.n3 = parser.getInt64At(2 * intW).value_or(0);
        seg.n4 = parser.getInt64At(3 * intW).value_or(0);
        if (seg.n1 != 0) {
            segments_.push_back(seg);
        }
    }

    return true;
}

std::vector<std::string> SetSegmentAdd::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(sid_);
    result.push_back(writer.getLine());

    for (const auto& seg : segments_) {
        writer.clear();
        writer.writeInt(seg.n1);
        writer.writeInt(seg.n2);
        writer.writeInt(seg.n3);
        writer.writeInt(seg.n4);
        result.push_back(writer.getLine());
    }

    return result;
}

void SetSegmentAdd::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(SetSegmentAdd, "*SET_SEGMENT_ADD")

// ============================================================================
// SetSegmentIntersect
// ============================================================================

bool SetSegmentIntersect::parse(const std::vector<std::string>& lines,
                                util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);
        size_t pos = 0;
        data_.sid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.sid1 = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.sid2 = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> SetSegmentIntersect::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.sid);
    writer.writeInt(data_.sid1);
    writer.writeInt(data_.sid2);
    result.push_back(writer.getLine());

    return result;
}

void SetSegmentIntersect::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(SetSegmentIntersect, "*SET_SEGMENT_INTERSECT")

// ============================================================================
// SetShellAdd
// ============================================================================

bool SetShellAdd::parse(const std::vector<std::string>& lines,
                        util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    elements_.clear();
    bool foundSid = false;

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);
        if (!foundSid) {
            sid_ = static_cast<int>(parser.getInt64At(0).value_or(0));
            foundSid = true;
            continue;
        }

        for (size_t i = 0; i < 8; ++i) {
            auto eid = parser.getInt64At(i * intW);
            if (eid && *eid != 0) {
                elements_.push_back(*eid);
            }
        }
    }

    return true;
}

std::vector<std::string> SetShellAdd::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(sid_);
    result.push_back(writer.getLine());

    size_t count = 0;
    writer.clear();
    for (auto eid : elements_) {
        writer.writeInt(eid);
        ++count;
        if (count == 8) {
            result.push_back(writer.getLine());
            writer.clear();
            count = 0;
        }
    }
    if (count > 0) {
        result.push_back(writer.getLine());
    }

    return result;
}

void SetShellAdd::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(SetShellAdd, "*SET_SHELL_ADD")

// ============================================================================
// SetShellGeneral
// ============================================================================

bool SetShellGeneral::parse(const std::vector<std::string>& lines,
                            util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);
        size_t pos = 0;
        data_.sid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.option = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.da1 = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.da2 = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> SetShellGeneral::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.sid);
    writer.writeInt(data_.option);
    writer.writeInt(data_.da1);
    writer.writeInt(data_.da2);
    result.push_back(writer.getLine());

    return result;
}

void SetShellGeneral::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(SetShellGeneral, "*SET_SHELL_GENERAL")

// ============================================================================
// SetShellIntersect
// ============================================================================

bool SetShellIntersect::parse(const std::vector<std::string>& lines,
                              util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);
        size_t pos = 0;
        data_.sid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.sid1 = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.sid2 = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> SetShellIntersect::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.sid);
    writer.writeInt(data_.sid1);
    writer.writeInt(data_.sid2);
    result.push_back(writer.getLine());

    return result;
}

void SetShellIntersect::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(SetShellIntersect, "*SET_SHELL_INTERSECT")

// ============================================================================
// SetSolidAdd
// ============================================================================

bool SetSolidAdd::parse(const std::vector<std::string>& lines,
                        util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    elements_.clear();
    bool foundSid = false;

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);
        if (!foundSid) {
            sid_ = static_cast<int>(parser.getInt64At(0).value_or(0));
            foundSid = true;
            continue;
        }

        for (size_t i = 0; i < 8; ++i) {
            auto eid = parser.getInt64At(i * intW);
            if (eid && *eid != 0) {
                elements_.push_back(*eid);
            }
        }
    }

    return true;
}

std::vector<std::string> SetSolidAdd::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(sid_);
    result.push_back(writer.getLine());

    size_t count = 0;
    writer.clear();
    for (auto eid : elements_) {
        writer.writeInt(eid);
        ++count;
        if (count == 8) {
            result.push_back(writer.getLine());
            writer.clear();
            count = 0;
        }
    }
    if (count > 0) {
        result.push_back(writer.getLine());
    }

    return result;
}

void SetSolidAdd::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(SetSolidAdd, "*SET_SOLID_ADD")

// ============================================================================
// SetSolidGeneral
// ============================================================================

bool SetSolidGeneral::parse(const std::vector<std::string>& lines,
                            util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);
        size_t pos = 0;
        data_.sid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.option = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.da1 = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.da2 = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> SetSolidGeneral::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.sid);
    writer.writeInt(data_.option);
    writer.writeInt(data_.da1);
    writer.writeInt(data_.da2);
    result.push_back(writer.getLine());

    return result;
}

void SetSolidGeneral::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(SetSolidGeneral, "*SET_SOLID_GENERAL")

// ============================================================================
// SetSolidIntersect
// ============================================================================

bool SetSolidIntersect::parse(const std::vector<std::string>& lines,
                              util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);
        size_t pos = 0;
        data_.sid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.sid1 = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.sid2 = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> SetSolidIntersect::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.sid);
    writer.writeInt(data_.sid1);
    writer.writeInt(data_.sid2);
    result.push_back(writer.getLine());

    return result;
}

void SetSolidIntersect::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(SetSolidIntersect, "*SET_SOLID_INTERSECT")

// ============================================================================
// SetTshellGeneral
// ============================================================================

bool SetTshellGeneral::parse(const std::vector<std::string>& lines,
                             util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);
        size_t pos = 0;
        data_.sid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.option = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.da1 = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.da2 = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> SetTshellGeneral::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.sid);
    writer.writeInt(data_.option);
    writer.writeInt(data_.da1);
    writer.writeInt(data_.da2);
    result.push_back(writer.getLine());

    return result;
}

void SetTshellGeneral::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(SetTshellGeneral, "*SET_TSHELL_GENERAL")

} // namespace koo::dyna
