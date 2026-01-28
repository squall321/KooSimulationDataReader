#include <koo/dyna/Element.hpp>
#include <koo/dyna/ModelVisitor.hpp>
#include <koo/dyna/KeywordFactory.hpp>
#include <koo/util/StringUtils.hpp>

namespace koo::dyna {

// ShellElementData implementation
ShellElementData::ShellElementData(ElementId id_, PartId pid_,
                                   NodeId n1, NodeId n2, NodeId n3, NodeId n4) {
    type = ElementType::Shell;
    id = id_;
    pid = pid_;
    nodeIds = {n1, n2, n3, n4};
}

// SolidElementData implementation
SolidElementData::SolidElementData(ElementId id_, PartId pid_,
                                   NodeId n1, NodeId n2, NodeId n3, NodeId n4,
                                   NodeId n5, NodeId n6, NodeId n7, NodeId n8) {
    type = ElementType::Solid;
    id = id_;
    pid = pid_;
    nodeIds = {n1, n2, n3, n4, n5, n6, n7, n8};
}

// ElementShell implementation
bool ElementShell::parse(const std::vector<std::string>& lines,
                         util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();    // Always 10
    const size_t realW = parser.getRealWidth();  // 10 (Standard) or 20 (Large)

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);

        // Shell element: eid(I), pid(I), n1(I), n2(I), n3(I), n4(I), [thic(E), beta(E)]
        size_t pos = 0;

        auto id = parser.getInt64At(pos);
        if (!id) continue;
        pos += intW;

        ShellElementData elem;
        elem.id = *id;
        elem.pid = parser.getInt64At(pos).value_or(0);
        pos += intW;

        for (int i = 0; i < 4; ++i) {
            elem.nodeIds.push_back(parser.getInt64At(pos).value_or(0));
            pos += intW;
        }

        // Optional thickness and beta (real fields)
        elem.thickness = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        elem.beta = parser.getDoubleAt(pos, realW).value_or(0.0);

        addElement(elem);
    }

    return true;
}

std::vector<std::string> ElementShell::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& elem : elements_) {
        writer.clear();
        writer.writeInt(elem.id);
        writer.writeInt(elem.pid);

        for (size_t i = 0; i < 4 && i < elem.nodeIds.size(); ++i) {
            writer.writeInt(elem.nodeIds[i]);
        }
        // Pad with zeros if less than 4 nodes
        for (size_t i = elem.nodeIds.size(); i < 4; ++i) {
            writer.writeInt(0);
        }

        if (elem.thickness != 0.0 || elem.beta != 0.0) {
            writer.writeDouble(elem.thickness);
            writer.writeDouble(elem.beta);
        }

        result.push_back(writer.getLine());
    }

    return result;
}

void ElementShell::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

void ElementShell::addElement(const ShellElementData& elem) {
    auto it = idIndex_.find(elem.id);
    if (it != idIndex_.end()) {
        elements_[it->second] = elem;
    } else {
        idIndex_[elem.id] = elements_.size();
        elements_.push_back(elem);
    }
}

void ElementShell::addElement(ElementId id, PartId pid,
                              NodeId n1, NodeId n2, NodeId n3, NodeId n4) {
    addElement(ShellElementData(id, pid, n1, n2, n3, n4));
}

bool ElementShell::hasElement(ElementId id) const {
    return idIndex_.find(id) != idIndex_.end();
}

ShellElementData* ElementShell::getElement(ElementId id) {
    auto it = idIndex_.find(id);
    if (it != idIndex_.end()) {
        return &elements_[it->second];
    }
    return nullptr;
}

const ShellElementData* ElementShell::getElement(ElementId id) const {
    auto it = idIndex_.find(id);
    if (it != idIndex_.end()) {
        return &elements_[it->second];
    }
    return nullptr;
}

void ElementShell::removeElement(ElementId id) {
    auto it = idIndex_.find(id);
    if (it != idIndex_.end()) {
        size_t index = it->second;
        elements_.erase(elements_.begin() + static_cast<std::ptrdiff_t>(index));
        rebuildIndex();
    }
}

void ElementShell::clear() {
    elements_.clear();
    idIndex_.clear();
}

void ElementShell::rebuildIndex() {
    idIndex_.clear();
    for (size_t i = 0; i < elements_.size(); ++i) {
        idIndex_[elements_[i].id] = i;
    }
}

// ElementSolid implementation
bool ElementSolid::parse(const std::vector<std::string>& lines,
                         util::CardParser::Format format) {
    size_t fieldWidth = (format == util::CardParser::Format::Large) ? 20 : 10;
    size_t lineIdx = 0;

    while (lineIdx < lines.size()) {
        const auto& line = lines[lineIdx];

        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            ++lineIdx;
            continue;
        }

        // Solid element Card 1: EID, PID, N1-N6 (or N1-N8 for single-line format)
        if (line.length() < fieldWidth * 2) {
            ++lineIdx;
            continue;
        }

        auto id = util::StringUtils::parseInt64(
            util::StringUtils::trim(line.substr(0, fieldWidth)));
        if (!id) {
            ++lineIdx;
            continue;
        }

        SolidElementData elem;
        elem.id = *id;

        if (line.length() >= fieldWidth * 2) {
            auto pidOpt = util::StringUtils::parseInt64(
                util::StringUtils::trim(line.substr(fieldWidth, fieldWidth)));
            elem.pid = pidOpt.value_or(0);
        }

        // Read nodes from Card 1
        // Count how many valid node fields are on this line
        int nodesOnCard1 = 0;
        for (int i = 0; i < 8; ++i) {
            size_t start = static_cast<size_t>(2 + i) * fieldWidth;
            if (start < line.length()) {
                size_t len = std::min(fieldWidth, line.length() - start);
                std::string fieldStr = util::StringUtils::trim(line.substr(start, len));
                if (!fieldStr.empty()) {
                    auto nidOpt = util::StringUtils::parseInt64(fieldStr);
                    elem.nodeIds.push_back(nidOpt.value_or(0));
                    if (nidOpt && *nidOpt != 0) {
                        nodesOnCard1 = i + 1;
                    }
                } else {
                    elem.nodeIds.push_back(0);
                }
            } else {
                break;
            }
        }
        ++lineIdx;

        // Check if this is a 10-node element (Card 2 with N7-N10 or N9-N10)
        // If we have 6 nodes on Card 1, look for Card 2 with 4 more nodes
        // Skip comment lines
        while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
            ++lineIdx;
        }

        if (lineIdx < lines.size() && nodesOnCard1 == 6) {
            const auto& card2 = lines[lineIdx];
            // Check if next line is not a keyword and not starting with a new element ID
            // (heuristic: if it starts with a large number, it's probably a new element)
            if (!util::CardParser::isKeywordLine(card2)) {
                auto possibleEid = util::StringUtils::parseInt64(
                    util::StringUtils::trim(card2.substr(0, std::min(fieldWidth, card2.length()))));

                // If the first field is empty or much smaller than current EID, assume it's Card 2
                // This is a heuristic - Card 2 for 10-node elements starts with N7
                if (!possibleEid || *possibleEid < 1000000) {
                    // Read up to 4 more nodes from Card 2 (N7, N8, N9, N10)
                    // But first, check if elem already has 6, 7, or 8 nodes
                    // If it has 8, we don't read Card 2
                    if (elem.nodeIds.size() == 6) {
                        for (int i = 0; i < 4; ++i) {
                            size_t start = static_cast<size_t>(i) * fieldWidth;
                            if (start < card2.length()) {
                                size_t len = std::min(fieldWidth, card2.length() - start);
                                std::string fieldStr = util::StringUtils::trim(card2.substr(start, len));
                                if (!fieldStr.empty()) {
                                    auto nidOpt = util::StringUtils::parseInt64(fieldStr);
                                    elem.nodeIds.push_back(nidOpt.value_or(0));
                                } else {
                                    elem.nodeIds.push_back(0);
                                }
                            } else {
                                break;
                            }
                        }
                        ++lineIdx;
                    }
                }
            }
        }

        addElement(elem);
    }

    return true;
}

std::vector<std::string> ElementSolid::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    size_t fieldWidth = (format == util::CardParser::Format::Large) ? 20 : 10;

    for (const auto& elem : elements_) {
        size_t numNodes = elem.nodeIds.size();

        if (numNodes == 10) {
            // 10-node element: write across two cards
            // Card 1: EID, PID, N1-N6
            std::string card1;
            card1 += util::StringUtils::formatInt(elem.id, fieldWidth);
            card1 += util::StringUtils::formatInt(elem.pid, fieldWidth);
            for (size_t i = 0; i < 6; ++i) {
                NodeId nid = (i < elem.nodeIds.size()) ? elem.nodeIds[i] : 0;
                card1 += util::StringUtils::formatInt(nid, fieldWidth);
            }
            result.push_back(card1);

            // Card 2: N7, N8, N9, N10
            std::string card2;
            for (size_t i = 6; i < 10; ++i) {
                NodeId nid = (i < elem.nodeIds.size()) ? elem.nodeIds[i] : 0;
                card2 += util::StringUtils::formatInt(nid, fieldWidth);
            }
            result.push_back(card2);
        } else {
            // 8-node or fewer: write on one line
            // Card 1: EID, PID, N1-N8
            std::string line;
            line += util::StringUtils::formatInt(elem.id, fieldWidth);
            line += util::StringUtils::formatInt(elem.pid, fieldWidth);

            for (size_t i = 0; i < 8; ++i) {
                NodeId nid = (i < elem.nodeIds.size()) ? elem.nodeIds[i] : 0;
                line += util::StringUtils::formatInt(nid, fieldWidth);
            }

            result.push_back(line);
        }
    }

    return result;
}

void ElementSolid::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

void ElementSolid::addElement(const SolidElementData& elem) {
    auto it = idIndex_.find(elem.id);
    if (it != idIndex_.end()) {
        elements_[it->second] = elem;
    } else {
        idIndex_[elem.id] = elements_.size();
        elements_.push_back(elem);
    }
}

void ElementSolid::addElement(ElementId id, PartId pid,
                              NodeId n1, NodeId n2, NodeId n3, NodeId n4,
                              NodeId n5, NodeId n6, NodeId n7, NodeId n8) {
    addElement(SolidElementData(id, pid, n1, n2, n3, n4, n5, n6, n7, n8));
}

bool ElementSolid::hasElement(ElementId id) const {
    return idIndex_.find(id) != idIndex_.end();
}

SolidElementData* ElementSolid::getElement(ElementId id) {
    auto it = idIndex_.find(id);
    if (it != idIndex_.end()) {
        return &elements_[it->second];
    }
    return nullptr;
}

const SolidElementData* ElementSolid::getElement(ElementId id) const {
    auto it = idIndex_.find(id);
    if (it != idIndex_.end()) {
        return &elements_[it->second];
    }
    return nullptr;
}

void ElementSolid::removeElement(ElementId id) {
    auto it = idIndex_.find(id);
    if (it != idIndex_.end()) {
        size_t index = it->second;
        elements_.erase(elements_.begin() + static_cast<std::ptrdiff_t>(index));
        rebuildIndex();
    }
}

void ElementSolid::clear() {
    elements_.clear();
    idIndex_.clear();
}

void ElementSolid::rebuildIndex() {
    idIndex_.clear();
    for (size_t i = 0; i < elements_.size(); ++i) {
        idIndex_[elements_[i].id] = i;
    }
}

// ============================================================================
// ElementBeam
// ============================================================================

bool ElementBeam::parse(const std::vector<std::string>& lines,
                        util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);

        // Card: EID, PID, N1, N2, N3, RT1, RR1, RT2, RR2, LOCAL
        size_t pos = 0;

        auto id = parser.getInt64At(pos);
        if (!id) continue;
        pos += intW;

        BeamElementData elem;
        elem.id = *id;
        elem.pid = parser.getInt64At(pos).value_or(0);
        pos += intW;

        NodeId n1 = parser.getInt64At(pos).value_or(0);
        pos += intW;
        NodeId n2 = parser.getInt64At(pos).value_or(0);
        pos += intW;
        elem.nodeIds = {n1, n2};

        elem.n3 = parser.getInt64At(pos).value_or(0);
        pos += intW;
        elem.rt1 = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        elem.rr1 = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        elem.rt2 = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        elem.rr2 = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        elem.local = static_cast<int>(parser.getInt64At(pos).value_or(2));

        addElement(elem);
    }

    return true;
}

std::vector<std::string> ElementBeam::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& elem : elements_) {
        writer.clear();
        writer.writeInt(elem.id);
        writer.writeInt(elem.pid);

        NodeId n1 = elem.nodeIds.size() > 0 ? elem.nodeIds[0] : 0;
        NodeId n2 = elem.nodeIds.size() > 1 ? elem.nodeIds[1] : 0;
        writer.writeInt(n1);
        writer.writeInt(n2);
        writer.writeInt(elem.n3);
        writer.writeInt(elem.rt1);
        writer.writeInt(elem.rr1);
        writer.writeInt(elem.rt2);

        result.push_back(writer.getLine());
    }

    return result;
}

void ElementBeam::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

void ElementBeam::addElement(const BeamElementData& elem) {
    auto it = idIndex_.find(elem.id);
    if (it != idIndex_.end()) {
        elements_[it->second] = elem;
    } else {
        idIndex_[elem.id] = elements_.size();
        elements_.push_back(elem);
    }
}

void ElementBeam::rebuildIndex() {
    idIndex_.clear();
    for (size_t i = 0; i < elements_.size(); ++i) {
        idIndex_[elements_[i].id] = i;
    }
}

// ============================================================================
// ElementDiscrete
// ============================================================================

bool ElementDiscrete::parse(const std::vector<std::string>& lines,
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

        // Card: EID, PID, N1, N2, VID, S, PF, OFFSET
        size_t pos = 0;

        auto id = parser.getInt64At(pos);
        if (!id) continue;
        pos += intW;

        DiscreteElementData elem;
        elem.id = *id;
        elem.pid = parser.getInt64At(pos).value_or(0);
        pos += intW;

        NodeId n1 = parser.getInt64At(pos).value_or(0);
        pos += intW;
        NodeId n2 = parser.getInt64At(pos).value_or(0);
        pos += intW;
        elem.nodeIds = n2 != 0 ? std::vector<NodeId>{n1, n2} : std::vector<NodeId>{n1};

        elem.vid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        elem.s = parser.getDoubleAt(pos, realW).value_or(1.0);
        pos += realW;
        elem.pf = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        elem.offset = parser.getDoubleAt(pos, realW).value_or(0.0);

        addElement(elem);
    }

    return true;
}

std::vector<std::string> ElementDiscrete::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& elem : elements_) {
        writer.clear();
        writer.writeInt(elem.id);
        writer.writeInt(elem.pid);

        NodeId n1 = elem.nodeIds.size() > 0 ? elem.nodeIds[0] : 0;
        NodeId n2 = elem.nodeIds.size() > 1 ? elem.nodeIds[1] : 0;
        writer.writeInt(n1);
        writer.writeInt(n2);
        writer.writeInt(elem.vid);
        writer.writeDouble(elem.s);
        writer.writeInt(elem.pf);
        writer.writeDouble(elem.offset);

        result.push_back(writer.getLine());
    }

    return result;
}

void ElementDiscrete::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

void ElementDiscrete::addElement(const DiscreteElementData& elem) {
    auto it = idIndex_.find(elem.id);
    if (it != idIndex_.end()) {
        elements_[it->second] = elem;
    } else {
        idIndex_[elem.id] = elements_.size();
        elements_.push_back(elem);
    }
}

void ElementDiscrete::rebuildIndex() {
    idIndex_.clear();
    for (size_t i = 0; i < elements_.size(); ++i) {
        idIndex_[elements_[i].id] = i;
    }
}

// ============================================================================
// ElementSeatbelt
// ============================================================================

bool ElementSeatbelt::parse(const std::vector<std::string>& lines,
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

        // Card: EID, PID, N1, N2, SBRID, LENGTH, LMIN
        size_t pos = 0;

        auto id = parser.getInt64At(pos);
        if (!id) continue;
        pos += intW;

        SeatbeltElementData elem;
        elem.id = *id;
        elem.pid = parser.getInt64At(pos).value_or(0);
        pos += intW;

        NodeId n1 = parser.getInt64At(pos).value_or(0);
        pos += intW;
        NodeId n2 = parser.getInt64At(pos).value_or(0);
        pos += intW;
        elem.nodeIds = {n1, n2};

        elem.sbrid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        elem.length = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        elem.lmin = parser.getDoubleAt(pos, realW).value_or(0.0);

        addElement(elem);
    }

    return true;
}

std::vector<std::string> ElementSeatbelt::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& elem : elements_) {
        writer.clear();
        writer.writeInt(elem.id);
        writer.writeInt(elem.pid);

        NodeId n1 = elem.nodeIds.size() > 0 ? elem.nodeIds[0] : 0;
        NodeId n2 = elem.nodeIds.size() > 1 ? elem.nodeIds[1] : 0;
        writer.writeInt(n1);
        writer.writeInt(n2);
        writer.writeInt(elem.sbrid);
        writer.writeDouble(elem.length);
        writer.writeDouble(elem.lmin);

        result.push_back(writer.getLine());
    }

    return result;
}

void ElementSeatbelt::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

void ElementSeatbelt::addElement(const SeatbeltElementData& elem) {
    auto it = idIndex_.find(elem.id);
    if (it != idIndex_.end()) {
        elements_[it->second] = elem;
    } else {
        idIndex_[elem.id] = elements_.size();
        elements_.push_back(elem);
    }
}

void ElementSeatbelt::rebuildIndex() {
    idIndex_.clear();
    for (size_t i = 0; i < elements_.size(); ++i) {
        idIndex_[elements_[i].id] = i;
    }
}

// ============================================================================
// ElementMass
// ============================================================================

bool ElementMass::parse(const std::vector<std::string>& lines,
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

        // Card: EID, NID, MASS
        size_t pos = 0;

        auto id = parser.getInt64At(pos);
        if (!id) continue;
        pos += intW;

        MassElementData elem;
        elem.id = *id;

        NodeId nid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        elem.nodeIds = {nid};

        elem.mass = parser.getDoubleAt(pos, realW).value_or(0.0);

        addElement(elem);
    }

    return true;
}

std::vector<std::string> ElementMass::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& elem : elements_) {
        writer.clear();
        writer.writeInt(elem.id);

        NodeId nid = elem.nodeIds.size() > 0 ? elem.nodeIds[0] : 0;
        writer.writeInt(nid);
        writer.writeDouble(elem.mass);

        result.push_back(writer.getLine());
    }

    return result;
}

void ElementMass::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

void ElementMass::addElement(const MassElementData& elem) {
    auto it = idIndex_.find(elem.id);
    if (it != idIndex_.end()) {
        elements_[it->second] = elem;
    } else {
        idIndex_[elem.id] = elements_.size();
        elements_.push_back(elem);
    }
}

void ElementMass::rebuildIndex() {
    idIndex_.clear();
    for (size_t i = 0; i < elements_.size(); ++i) {
        idIndex_[elements_[i].id] = i;
    }
}

// ============================================================================
// ElementInertia
// ============================================================================

bool ElementInertia::parse(const std::vector<std::string>& lines,
                            util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size()) {
        if (util::CardParser::isCommentLine(lines[lineIdx]) ||
            util::CardParser::isKeywordLine(lines[lineIdx])) {
            ++lineIdx;
            continue;
        }

        parser.setLine(lines[lineIdx]);
        size_t pos = 0;

        auto id = parser.getInt64At(pos);
        if (!id) {
            ++lineIdx;
            continue;
        }

        InertiaElementData elem;
        elem.id = *id;
        pos += intW;
        elem.pid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        NodeId nid = parser.getInt64At(pos).value_or(0);
        elem.nodeIds = {nid};
        pos += intW;
        elem.mass = parser.getDoubleAt(pos, realW).value_or(0.0);
        ++lineIdx;

        // Card 2: IXX, IXY, IXZ, IYY, IYZ, IZZ
        while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
            ++lineIdx;
        }
        if (lineIdx < lines.size()) {
            parser.setLine(lines[lineIdx]);
            elem.ixx = parser.getDoubleAt(0, realW).value_or(0.0);
            elem.ixy = parser.getDoubleAt(realW, realW).value_or(0.0);
            elem.ixz = parser.getDoubleAt(2 * realW, realW).value_or(0.0);
            elem.iyy = parser.getDoubleAt(3 * realW, realW).value_or(0.0);
            elem.iyz = parser.getDoubleAt(4 * realW, realW).value_or(0.0);
            elem.izz = parser.getDoubleAt(5 * realW, realW).value_or(0.0);
            ++lineIdx;
        }

        addElement(elem);
    }

    return true;
}

std::vector<std::string> ElementInertia::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& elem : elements_) {
        writer.clear();
        writer.writeInt(elem.id);
        writer.writeInt(elem.pid);
        NodeId nid = elem.nodeIds.size() > 0 ? elem.nodeIds[0] : 0;
        writer.writeInt(nid);
        writer.writeDouble(elem.mass);
        result.push_back(writer.getLine());

        writer.clear();
        writer.writeDouble(elem.ixx);
        writer.writeDouble(elem.ixy);
        writer.writeDouble(elem.ixz);
        writer.writeDouble(elem.iyy);
        writer.writeDouble(elem.iyz);
        writer.writeDouble(elem.izz);
        result.push_back(writer.getLine());
    }

    return result;
}

void ElementInertia::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

void ElementInertia::addElement(const InertiaElementData& elem) {
    auto it = idIndex_.find(elem.id);
    if (it != idIndex_.end()) {
        elements_[it->second] = elem;
    } else {
        idIndex_[elem.id] = elements_.size();
        elements_.push_back(elem);
    }
}

void ElementInertia::rebuildIndex() {
    idIndex_.clear();
    for (size_t i = 0; i < elements_.size(); ++i) {
        idIndex_[elements_[i].id] = i;
    }
}

// ============================================================================
// ElementTshell
// ============================================================================

bool ElementTshell::parse(const std::vector<std::string>& lines,
                           util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size()) {
        if (util::CardParser::isCommentLine(lines[lineIdx]) ||
            util::CardParser::isKeywordLine(lines[lineIdx])) {
            ++lineIdx;
            continue;
        }

        parser.setLine(lines[lineIdx]);
        size_t pos = 0;

        auto id = parser.getInt64At(pos);
        if (!id) {
            ++lineIdx;
            continue;
        }

        TshellElementData elem;
        elem.id = *id;
        pos += intW;
        elem.pid = parser.getInt64At(pos).value_or(0);
        pos += intW;

        // 8 nodes for thick shell
        for (int i = 0; i < 8 && pos < lines[lineIdx].size(); ++i) {
            NodeId nid = parser.getInt64At(pos).value_or(0);
            if (nid != 0) {
                elem.nodeIds.push_back(nid);
            }
            pos += intW;
        }
        ++lineIdx;

        // Card 2: Thicknesses at corner nodes
        while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
            ++lineIdx;
        }
        if (lineIdx < lines.size()) {
            parser.setLine(lines[lineIdx]);
            elem.thick1 = parser.getDoubleAt(0, realW).value_or(0.0);
            elem.thick2 = parser.getDoubleAt(realW, realW).value_or(0.0);
            elem.thick3 = parser.getDoubleAt(2 * realW, realW).value_or(0.0);
            elem.thick4 = parser.getDoubleAt(3 * realW, realW).value_or(0.0);
            ++lineIdx;
        }

        addElement(elem);
    }

    return true;
}

std::vector<std::string> ElementTshell::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& elem : elements_) {
        writer.clear();
        writer.writeInt(elem.id);
        writer.writeInt(elem.pid);
        for (size_t i = 0; i < 8; ++i) {
            NodeId nid = i < elem.nodeIds.size() ? elem.nodeIds[i] : 0;
            writer.writeInt(nid);
        }
        result.push_back(writer.getLine());

        writer.clear();
        writer.writeDouble(elem.thick1);
        writer.writeDouble(elem.thick2);
        writer.writeDouble(elem.thick3);
        writer.writeDouble(elem.thick4);
        result.push_back(writer.getLine());
    }

    return result;
}

void ElementTshell::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

void ElementTshell::addElement(const TshellElementData& elem) {
    auto it = idIndex_.find(elem.id);
    if (it != idIndex_.end()) {
        elements_[it->second] = elem;
    } else {
        idIndex_[elem.id] = elements_.size();
        elements_.push_back(elem);
    }
}

void ElementTshell::rebuildIndex() {
    idIndex_.clear();
    for (size_t i = 0; i < elements_.size(); ++i) {
        idIndex_[elements_[i].id] = i;
    }
}

// ============================================================================
// ElementShellThickness
// ============================================================================

bool ElementShellThickness::parse(const std::vector<std::string>& lines,
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

        auto eid = parser.getInt64At(pos);
        if (!eid) continue;

        ShellThicknessData data;
        data.eid = *eid;
        pos += intW;
        data.thick1 = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data.thick2 = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data.thick3 = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data.thick4 = parser.getDoubleAt(pos, realW).value_or(0.0);

        data_.push_back(data);
    }

    return true;
}

std::vector<std::string> ElementShellThickness::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& data : data_) {
        writer.clear();
        writer.writeInt(data.eid);
        writer.writeDouble(data.thick1);
        writer.writeDouble(data.thick2);
        writer.writeDouble(data.thick3);
        writer.writeDouble(data.thick4);
        result.push_back(writer.getLine());
    }

    return result;
}

void ElementShellThickness::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

// ============================================================================
// ElementBeamOrientation
// ============================================================================

bool ElementBeamOrientation::parse(const std::vector<std::string>& lines,
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

        auto eid = parser.getInt64At(pos);
        if (!eid) continue;

        BeamOrientationData data;
        data.eid = *eid;
        pos += intW;
        data.ux = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data.uy = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data.uz = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data.vx = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data.vy = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data.vz = parser.getDoubleAt(pos, realW).value_or(0.0);

        data_.push_back(data);
    }

    return true;
}

std::vector<std::string> ElementBeamOrientation::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& data : data_) {
        writer.clear();
        writer.writeInt(data.eid);
        writer.writeDouble(data.ux);
        writer.writeDouble(data.uy);
        writer.writeDouble(data.uz);
        writer.writeDouble(data.vx);
        writer.writeDouble(data.vy);
        writer.writeDouble(data.vz);
        result.push_back(writer.getLine());
    }

    return result;
}

void ElementBeamOrientation::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

// ============================================================================
// ElementMassPartSet
// ============================================================================

bool ElementMassPartSet::parse(const std::vector<std::string>& lines,
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

        Data data;
        data.psid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.mass = parser.getDoubleAt(pos, realW).value_or(0.0);

        data_.push_back(data);
    }

    return true;
}

std::vector<std::string> ElementMassPartSet::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& data : data_) {
        writer.clear();
        writer.writeInt(data.psid);
        writer.writeDouble(data.mass);
        result.push_back(writer.getLine());
    }

    return result;
}

void ElementMassPartSet::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

// ============================================================================
// ElementSeatbeltAccelerometer
// ============================================================================

bool ElementSeatbeltAccelerometer::parse(const std::vector<std::string>& lines,
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

        SeatbeltAccelerometerData data;
        data.id = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data.nid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data.sbacid = static_cast<int>(parser.getInt64At(pos).value_or(0));

        data_.push_back(data);
    }

    return true;
}

std::vector<std::string> ElementSeatbeltAccelerometer::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& data : data_) {
        writer.clear();
        writer.writeInt(data.id);
        writer.writeInt(data.nid);
        writer.writeInt(data.sbacid);
        result.push_back(writer.getLine());
    }

    return result;
}

void ElementSeatbeltAccelerometer::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

// ============================================================================
// ElementSeatbeltPretensioner
// ============================================================================

bool ElementSeatbeltPretensioner::parse(const std::vector<std::string>& lines,
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

        SeatbeltPretensionerData data;
        data.id = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data.sbprid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data.sbsid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data.time = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data.pull = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data.llcid = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data.ulcid = parser.getDoubleAt(pos, realW).value_or(0.0);

        data_.push_back(data);
    }

    return true;
}

std::vector<std::string> ElementSeatbeltPretensioner::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& data : data_) {
        writer.clear();
        writer.writeInt(data.id);
        writer.writeInt(data.sbprid);
        writer.writeInt(data.sbsid);
        writer.writeDouble(data.time);
        writer.writeDouble(data.pull);
        writer.writeDouble(data.llcid);
        writer.writeDouble(data.ulcid);
        result.push_back(writer.getLine());
    }

    return result;
}

void ElementSeatbeltPretensioner::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

// ============================================================================
// ElementSeatbeltRetractor
// ============================================================================

bool ElementSeatbeltRetractor::parse(const std::vector<std::string>& lines,
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

        SeatbeltRetractorData data;
        data.id = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data.sbrid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data.nid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data.sbsid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data.tdel = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data.pull = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data.llcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.lmin = parser.getDoubleAt(pos, realW).value_or(0.0);

        data_.push_back(data);
    }

    return true;
}

std::vector<std::string> ElementSeatbeltRetractor::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& data : data_) {
        writer.clear();
        writer.writeInt(data.id);
        writer.writeInt(data.sbrid);
        writer.writeInt(data.nid);
        writer.writeInt(data.sbsid);
        writer.writeDouble(data.tdel);
        writer.writeDouble(data.pull);
        writer.writeInt(data.llcid);
        writer.writeDouble(data.lmin);
        result.push_back(writer.getLine());
    }

    return result;
}

void ElementSeatbeltRetractor::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

// ============================================================================
// ElementSeatbeltSensor
// ============================================================================

bool ElementSeatbeltSensor::parse(const std::vector<std::string>& lines,
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

        SeatbeltSensorData data;
        data.id = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data.sbsid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data.sbt = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.sbsid1 = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data.sbsid2 = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data.accel = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data.tdel = parser.getDoubleAt(pos, realW).value_or(0.0);

        data_.push_back(data);
    }

    return true;
}

std::vector<std::string> ElementSeatbeltSensor::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& data : data_) {
        writer.clear();
        writer.writeInt(data.id);
        writer.writeInt(data.sbsid);
        writer.writeInt(data.sbt);
        writer.writeInt(data.sbsid1);
        writer.writeInt(data.sbsid2);
        writer.writeDouble(data.accel);
        writer.writeDouble(data.tdel);
        result.push_back(writer.getLine());
    }

    return result;
}

void ElementSeatbeltSensor::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

// ============================================================================
// ElementPlotel
// ============================================================================

bool ElementPlotel::parse(const std::vector<std::string>& lines,
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

        auto id = parser.getInt64At(pos);
        if (!id) continue;

        PlotelElementData data;
        data.id = *id;
        pos += intW;
        data.n1 = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data.n2 = parser.getInt64At(pos).value_or(0);

        data_.push_back(data);
    }

    return true;
}

std::vector<std::string> ElementPlotel::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& data : data_) {
        writer.clear();
        writer.writeInt(data.id);
        writer.writeInt(data.n1);
        writer.writeInt(data.n2);
        result.push_back(writer.getLine());
    }

    return result;
}

void ElementPlotel::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

// ============================================================================
// ElementBearing
// ============================================================================

bool ElementBearing::parse(const std::vector<std::string>& lines,
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

        auto id = parser.getInt64At(pos);
        if (!id) continue;

        BearingElementData data;
        data.id = *id;
        pos += intW;
        data.pid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data.n1 = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data.n2 = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data.vid = static_cast<int>(parser.getInt64At(pos).value_or(0));

        data_.push_back(data);
    }

    return true;
}

std::vector<std::string> ElementBearing::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& data : data_) {
        writer.clear();
        writer.writeInt(data.id);
        writer.writeInt(data.pid);
        writer.writeInt(data.n1);
        writer.writeInt(data.n2);
        writer.writeInt(data.vid);
        result.push_back(writer.getLine());
    }

    return result;
}

void ElementBearing::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

// ============================================================================
// ElementLancing
// ============================================================================

bool ElementLancing::parse(const std::vector<std::string>& lines,
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

        auto eid = parser.getInt64At(pos);
        if (!eid) continue;

        LancingElementData data;
        data.eid = *eid;
        pos += intW;
        data.n1 = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.n2 = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.n3 = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.n4 = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.pid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.tbirth = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data.tdeath = parser.getDoubleAt(pos, realW).value_or(1e28);

        data_.push_back(data);
    }

    return true;
}

std::vector<std::string> ElementLancing::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& data : data_) {
        writer.clear();
        writer.writeInt(data.eid);
        writer.writeInt(data.n1);
        writer.writeInt(data.n2);
        writer.writeInt(data.n3);
        writer.writeInt(data.n4);
        writer.writeInt(data.pid);
        writer.writeDouble(data.tbirth);
        writer.writeDouble(data.tdeath);
        result.push_back(writer.getLine());
    }

    return result;
}

void ElementLancing::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

// ============================================================================
// ElementGeneralizedShell
// ============================================================================

bool ElementGeneralizedShell::parse(const std::vector<std::string>& lines,
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
        GeneralizedShellElementData data;
        size_t pos = 0;

        data.eid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data.pid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data.n1 = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data.n2 = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data.n3 = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data.n4 = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data.n5 = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data.n6 = parser.getInt64At(pos).value_or(0);

        if (data.eid != 0) {
            data_.push_back(data);
        }
    }

    return true;
}

std::vector<std::string> ElementGeneralizedShell::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& data : data_) {
        writer.clear();
        writer.writeInt(data.eid);
        writer.writeInt(data.pid);
        writer.writeInt(data.n1);
        writer.writeInt(data.n2);
        writer.writeInt(data.n3);
        writer.writeInt(data.n4);
        writer.writeInt(data.n5);
        writer.writeInt(data.n6);
        result.push_back(writer.getLine());
    }

    return result;
}

void ElementGeneralizedShell::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

// ============================================================================
// ElementSolidOrtho
// ============================================================================

bool ElementSolidOrtho::parse(const std::vector<std::string>& lines,
                               util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    data_.clear();
    size_t lineIdx = 0;

    while (lineIdx < lines.size()) {
        if (util::CardParser::isCommentLine(lines[lineIdx]) ||
            util::CardParser::isKeywordLine(lines[lineIdx])) {
            ++lineIdx;
            continue;
        }

        parser.setLine(lines[lineIdx]);
        size_t pos = 0;

        auto eid = parser.getInt64At(pos);
        if (!eid) {
            ++lineIdx;
            continue;
        }

        Data data;
        data.eid = *eid;
        pos += intW;
        data.pid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data.n1 = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data.n2 = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data.n3 = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data.n4 = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data.n5 = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data.n6 = parser.getInt64At(pos).value_or(0);
        ++lineIdx;

        // Card 2: N7, N8
        while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
            ++lineIdx;
        }
        if (lineIdx < lines.size()) {
            parser.setLine(lines[lineIdx]);
            data.n7 = parser.getInt64At(0).value_or(0);
            data.n8 = parser.getInt64At(intW).value_or(0);
            ++lineIdx;
        }

        // Card 3: A1, A2, A3, D1, D2, D3
        while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
            ++lineIdx;
        }
        if (lineIdx < lines.size()) {
            parser.setLine(lines[lineIdx]);
            data.a1 = parser.getDoubleAt(0, realW).value_or(0.0);
            data.a2 = parser.getDoubleAt(realW, realW).value_or(0.0);
            data.a3 = parser.getDoubleAt(2 * realW, realW).value_or(0.0);
            data.d1 = parser.getDoubleAt(3 * realW, realW).value_or(0.0);
            data.d2 = parser.getDoubleAt(4 * realW, realW).value_or(0.0);
            data.d3 = parser.getDoubleAt(5 * realW, realW).value_or(0.0);
            ++lineIdx;
        }

        data_.push_back(data);
    }

    return true;
}

std::vector<std::string> ElementSolidOrtho::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& data : data_) {
        writer.clear();
        writer.writeInt(data.eid);
        writer.writeInt(data.pid);
        writer.writeInt(data.n1);
        writer.writeInt(data.n2);
        writer.writeInt(data.n3);
        writer.writeInt(data.n4);
        writer.writeInt(data.n5);
        writer.writeInt(data.n6);
        result.push_back(writer.getLine());

        writer.clear();
        writer.writeInt(data.n7);
        writer.writeInt(data.n8);
        result.push_back(writer.getLine());

        writer.clear();
        writer.writeDouble(data.a1);
        writer.writeDouble(data.a2);
        writer.writeDouble(data.a3);
        writer.writeDouble(data.d1);
        writer.writeDouble(data.d2);
        writer.writeDouble(data.d3);
        result.push_back(writer.getLine());
    }

    return result;
}

void ElementSolidOrtho::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

// ============================================================================
// ElementBeamPulley
// ============================================================================

bool ElementBeamPulley::parse(const std::vector<std::string>& lines,
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

        auto eid = parser.getInt64At(pos);
        if (!eid) continue;

        Data data;
        data.eid = *eid;
        pos += intW;
        data.pid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data.n1 = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data.n2 = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data.n3 = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data.radius = parser.getDoubleAt(pos, realW).value_or(0.0);

        data_.push_back(data);
    }

    return true;
}

std::vector<std::string> ElementBeamPulley::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& data : data_) {
        writer.clear();
        writer.writeInt(data.eid);
        writer.writeInt(data.pid);
        writer.writeInt(data.n1);
        writer.writeInt(data.n2);
        writer.writeInt(data.n3);
        writer.writeDouble(data.radius);
        result.push_back(writer.getLine());
    }

    return result;
}

void ElementBeamPulley::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

// ============================================================================
// ElementShellComposite
// ============================================================================

bool ElementShellComposite::parse(const std::vector<std::string>& lines,
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

        auto eid = parser.getInt64At(pos);
        if (!eid) continue;

        Data data;
        data.eid = *eid;
        pos += intW;
        data.pid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data.n1 = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data.n2 = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data.n3 = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data.n4 = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data.icomp = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.setefg = static_cast<int>(parser.getInt64At(pos).value_or(0));

        data_.push_back(data);
    }

    return true;
}

std::vector<std::string> ElementShellComposite::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& data : data_) {
        writer.clear();
        writer.writeInt(data.eid);
        writer.writeInt(data.pid);
        writer.writeInt(data.n1);
        writer.writeInt(data.n2);
        writer.writeInt(data.n3);
        writer.writeInt(data.n4);
        writer.writeInt(data.icomp);
        writer.writeInt(data.setefg);
        result.push_back(writer.getLine());
    }

    return result;
}

void ElementShellComposite::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

// ============================================================================
// ElementDirectMatrixInput
// ============================================================================

bool ElementDirectMatrixInput::parse(const std::vector<std::string>& lines,
                                      util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    // Skip comments/keyword lines
    while (lineIdx < lines.size() &&
           (util::CardParser::isCommentLine(lines[lineIdx]) ||
            util::CardParser::isKeywordLine(lines[lineIdx]))) {
        ++lineIdx;
    }

    if (lineIdx >= lines.size()) return true;

    // Card 1: ID, TYPE, NROW, NCOL
    parser.setLine(lines[lineIdx]);
    data_.id = static_cast<int>(parser.getInt64At(0).value_or(0));
    data_.type = static_cast<int>(parser.getInt64At(intW).value_or(0));
    data_.nrow = static_cast<int>(parser.getInt64At(2 * intW).value_or(0));
    data_.ncol = static_cast<int>(parser.getInt64At(3 * intW).value_or(0));
    ++lineIdx;

    // Read matrix values
    data_.matrix.clear();
    while (lineIdx < lines.size()) {
        if (util::CardParser::isCommentLine(lines[lineIdx]) ||
            util::CardParser::isKeywordLine(lines[lineIdx])) {
            ++lineIdx;
            continue;
        }

        parser.setLine(lines[lineIdx]);
        for (size_t pos = 0; pos + realW <= lines[lineIdx].size(); pos += realW) {
            auto val = parser.getDoubleAt(pos, realW);
            if (val) {
                data_.matrix.push_back(*val);
            }
        }
        ++lineIdx;
    }

    return true;
}

std::vector<std::string> ElementDirectMatrixInput::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.clear();
    writer.writeInt(data_.id);
    writer.writeInt(data_.type);
    writer.writeInt(data_.nrow);
    writer.writeInt(data_.ncol);
    result.push_back(writer.getLine());

    // Write matrix values (8 per line for standard format)
    const size_t valuesPerLine = (format == util::CardParser::Format::Large) ? 4 : 8;
    for (size_t i = 0; i < data_.matrix.size(); i += valuesPerLine) {
        writer.clear();
        for (size_t j = 0; j < valuesPerLine && (i + j) < data_.matrix.size(); ++j) {
            writer.writeDouble(data_.matrix[i + j]);
        }
        result.push_back(writer.getLine());
    }

    return result;
}

void ElementDirectMatrixInput::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

// ============================================================================
// ElementInterpolationShell
// ============================================================================

bool ElementInterpolationShell::parse(const std::vector<std::string>& lines,
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

        auto eid = parser.getInt64At(pos);
        if (!eid) continue;

        Data data;
        data.eid = *eid;
        pos += intW;
        data.pid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data.n1 = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data.n2 = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data.n3 = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data.n4 = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data.iflag = static_cast<int>(parser.getInt64At(pos).value_or(0));

        data_.push_back(data);
    }

    return true;
}

std::vector<std::string> ElementInterpolationShell::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& data : data_) {
        writer.clear();
        writer.writeInt(data.eid);
        writer.writeInt(data.pid);
        writer.writeInt(data.n1);
        writer.writeInt(data.n2);
        writer.writeInt(data.n3);
        writer.writeInt(data.n4);
        writer.writeInt(data.iflag);
        result.push_back(writer.getLine());
    }

    return result;
}

void ElementInterpolationShell::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

// ============================================================================
// ElementTrim
// ============================================================================

bool ElementTrim::parse(const std::vector<std::string>& lines,
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

        auto eid = parser.getInt64At(pos);
        if (!eid) continue;

        Data data;
        data.eid = *eid;
        pos += intW;
        data.n1 = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.n2 = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.n3 = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.pid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.tbirth = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data.tdeath = parser.getDoubleAt(pos, realW).value_or(1e28);

        data_.push_back(data);
    }

    return true;
}

std::vector<std::string> ElementTrim::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& data : data_) {
        writer.clear();
        writer.writeInt(data.eid);
        writer.writeInt(data.n1);
        writer.writeInt(data.n2);
        writer.writeInt(data.n3);
        writer.writeInt(data.pid);
        writer.writeDouble(data.tbirth);
        writer.writeDouble(data.tdeath);
        result.push_back(writer.getLine());
    }

    return result;
}

void ElementTrim::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

// ============================================================================
// ElementMassNodeSet
// ============================================================================

bool ElementMassNodeSet::parse(const std::vector<std::string>& lines,
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

        Data data;
        data.nsid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.mass = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data.pid = static_cast<int>(parser.getInt64At(pos).value_or(0));

        if (data.nsid != 0) {
            data_.push_back(data);
        }
    }

    return true;
}

std::vector<std::string> ElementMassNodeSet::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& data : data_) {
        writer.clear();
        writer.writeInt(data.nsid);
        writer.writeDouble(data.mass);
        writer.writeInt(data.pid);
        result.push_back(writer.getLine());
    }

    return result;
}

void ElementMassNodeSet::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

// ============================================================================
// ElementSeatbeltSlipring
// ============================================================================

bool ElementSeatbeltSlipring::parse(const std::vector<std::string>& lines,
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

        auto id = parser.getInt64At(pos);
        if (!id) continue;

        Data data;
        data.id = *id;
        pos += intW;
        data.n1 = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data.n2 = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data.n3 = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data.sbid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.friction = parser.getDoubleAt(pos, realW).value_or(0.0);

        data_.push_back(data);
    }

    return true;
}

std::vector<std::string> ElementSeatbeltSlipring::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& data : data_) {
        writer.clear();
        writer.writeInt(data.id);
        writer.writeInt(data.n1);
        writer.writeInt(data.n2);
        writer.writeInt(data.n3);
        writer.writeInt(data.sbid);
        writer.writeDouble(data.friction);
        result.push_back(writer.getLine());
    }

    return result;
}

void ElementSeatbeltSlipring::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

// ============================================================================
// ElementShellSourceSink
// ============================================================================

bool ElementShellSourceSink::parse(const std::vector<std::string>& lines,
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

        auto eid = parser.getInt64At(pos);
        if (!eid) continue;

        Data data;
        data.eid = *eid;
        pos += intW;
        data.pid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data.n1 = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data.n2 = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data.n3 = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data.n4 = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data.source = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.sink = static_cast<int>(parser.getInt64At(pos).value_or(0));

        data_.push_back(data);
    }

    return true;
}

std::vector<std::string> ElementShellSourceSink::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& data : data_) {
        writer.clear();
        writer.writeInt(data.eid);
        writer.writeInt(data.pid);
        writer.writeInt(data.n1);
        writer.writeInt(data.n2);
        writer.writeInt(data.n3);
        writer.writeInt(data.n4);
        writer.writeInt(data.source);
        writer.writeInt(data.sink);
        result.push_back(writer.getLine());
    }

    return result;
}

void ElementShellSourceSink::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

// Register keywords
REGISTER_KEYWORD(ElementShell, "*ELEMENT_SHELL")
REGISTER_KEYWORD(ElementSolid, "*ELEMENT_SOLID")
REGISTER_KEYWORD(ElementBeam, "*ELEMENT_BEAM")
REGISTER_KEYWORD(ElementDiscrete, "*ELEMENT_DISCRETE")
REGISTER_KEYWORD(ElementSeatbelt, "*ELEMENT_SEATBELT")
REGISTER_KEYWORD(ElementMass, "*ELEMENT_MASS")
REGISTER_KEYWORD(ElementInertia, "*ELEMENT_INERTIA")
REGISTER_KEYWORD(ElementTshell, "*ELEMENT_TSHELL")
REGISTER_KEYWORD(ElementShellThickness, "*ELEMENT_SHELL_THICKNESS")
REGISTER_KEYWORD(ElementBeamOrientation, "*ELEMENT_BEAM_ORIENTATION")
REGISTER_KEYWORD(ElementMassPartSet, "*ELEMENT_MASS_PART_SET")
REGISTER_KEYWORD(ElementSeatbeltAccelerometer, "*ELEMENT_SEATBELT_ACCELEROMETER")
REGISTER_KEYWORD(ElementSeatbeltPretensioner, "*ELEMENT_SEATBELT_PRETENSIONER")
REGISTER_KEYWORD(ElementSeatbeltRetractor, "*ELEMENT_SEATBELT_RETRACTOR")
REGISTER_KEYWORD(ElementSeatbeltSensor, "*ELEMENT_SEATBELT_SENSOR")
REGISTER_KEYWORD(ElementPlotel, "*ELEMENT_PLOTEL")
REGISTER_KEYWORD(ElementBearing, "*ELEMENT_BEARING")
REGISTER_KEYWORD(ElementLancing, "*ELEMENT_LANCING")
REGISTER_KEYWORD(ElementGeneralizedShell, "*ELEMENT_GENERALIZED_SHELL")
REGISTER_KEYWORD(ElementSolidOrtho, "*ELEMENT_SOLID_ORTHO")
REGISTER_KEYWORD(ElementBeamPulley, "*ELEMENT_BEAM_PULLEY")
REGISTER_KEYWORD(ElementShellComposite, "*ELEMENT_SHELL_COMPOSITE")
REGISTER_KEYWORD(ElementDirectMatrixInput, "*ELEMENT_DIRECT_MATRIX_INPUT")
REGISTER_KEYWORD(ElementInterpolationShell, "*ELEMENT_INTERPOLATION_SHELL")
REGISTER_KEYWORD(ElementTrim, "*ELEMENT_TRIM")
REGISTER_KEYWORD(ElementMassNodeSet, "*ELEMENT_MASS_NODE_SET")
REGISTER_KEYWORD(ElementSeatbeltSlipring, "*ELEMENT_SEATBELT_SLIPRING")
REGISTER_KEYWORD(ElementShellSourceSink, "*ELEMENT_SHELL_SOURCE_SINK")

} // namespace koo::dyna
