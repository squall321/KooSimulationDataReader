#include <koo/dyna/Node.hpp>
#include <koo/dyna/ModelVisitor.hpp>
#include <koo/dyna/KeywordFactory.hpp>

namespace koo::dyna {

bool Node::parse(const std::vector<std::string>& lines,
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

        // Node card layout: nid(I), x(E), y(E), z(E), tc(I), rc(I)
        // Position calculation depends on field widths
        size_t pos = 0;

        auto id = parser.getInt64At(pos);
        if (!id) continue;
        pos += intW;

        NodeData node;
        node.id = *id;
        node.position.x = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        node.position.y = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        node.position.z = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        node.tc = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        node.rc = static_cast<int>(parser.getInt64At(pos).value_or(0));

        addNode(node);
    }

    return true;
}

std::vector<std::string> Node::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& node : nodes_) {
        writer.clear();
        writer.writeInt(node.id);
        writer.writeDouble(node.position.x);
        writer.writeDouble(node.position.y);
        writer.writeDouble(node.position.z);
        writer.writeInt(node.tc);
        writer.writeInt(node.rc);
        result.push_back(writer.getLine());
    }

    return result;
}

void Node::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

void Node::addNode(const NodeData& node) {
    auto it = idIndex_.find(node.id);
    if (it != idIndex_.end()) {
        // Update existing node
        nodes_[it->second] = node;
    } else {
        // Add new node
        idIndex_[node.id] = nodes_.size();
        nodes_.push_back(node);
    }
}

void Node::addNode(NodeId id, double x, double y, double z) {
    addNode(NodeData(id, x, y, z));
}

void Node::addNode(NodeId id, const Vec3& position) {
    addNode(NodeData(id, position));
}

bool Node::hasNode(NodeId id) const {
    return idIndex_.find(id) != idIndex_.end();
}

NodeData* Node::getNode(NodeId id) {
    auto it = idIndex_.find(id);
    if (it != idIndex_.end()) {
        return &nodes_[it->second];
    }
    return nullptr;
}

const NodeData* Node::getNode(NodeId id) const {
    auto it = idIndex_.find(id);
    if (it != idIndex_.end()) {
        return &nodes_[it->second];
    }
    return nullptr;
}

void Node::removeNode(NodeId id) {
    auto it = idIndex_.find(id);
    if (it != idIndex_.end()) {
        size_t index = it->second;
        nodes_.erase(nodes_.begin() + static_cast<std::ptrdiff_t>(index));
        rebuildIndex();
    }
}

void Node::clear() {
    nodes_.clear();
    idIndex_.clear();
}

NodeData* Node::findNode(NodeId id) {
    return getNode(id);
}

const NodeData* Node::findNode(NodeId id) const {
    return getNode(id);
}

BoundingBox Node::getBoundingBox() const {
    BoundingBox bbox;
    for (const auto& node : nodes_) {
        bbox.expand(node.position);
    }
    return bbox;
}

void Node::transform(const Matrix4x4& matrix) {
    for (auto& node : nodes_) {
        node.position = matrix * node.position;
    }
}

void Node::rebuildIndex() {
    idIndex_.clear();
    for (size_t i = 0; i < nodes_.size(); ++i) {
        idIndex_[nodes_[i].id] = i;
    }
}

// ============================================================================
// NodeTransform
// ============================================================================

bool NodeTransform::parse(const std::vector<std::string>& lines,
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
        data_.nsid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.cid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> NodeTransform::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.nsid);
    writer.writeInt(data_.cid);
    result.push_back(writer.getLine());

    return result;
}

void NodeTransform::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

// ============================================================================
// NodeMerge
// ============================================================================

bool NodeMerge::parse(const std::vector<std::string>& lines,
                      util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);
        size_t pos = 0;
        data_.tol = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.nsid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> NodeMerge::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeDouble(data_.tol);
    writer.writeInt(data_.nsid);
    result.push_back(writer.getLine());

    return result;
}

void NodeMerge::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

// ============================================================================
// NodeScalar
// ============================================================================

bool NodeScalar::parse(const std::vector<std::string>& lines,
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

        auto nid = parser.getInt64At(pos);
        if (!nid) continue;

        NodeScalarData node;
        node.nid = *nid;
        pos += intW;
        node.value = parser.getDoubleAt(pos, realW).value_or(0.0);
        addNode(node);
    }

    return true;
}

std::vector<std::string> NodeScalar::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& node : nodes_) {
        writer.clear();
        writer.writeInt(node.nid);
        writer.writeDouble(node.value);
        result.push_back(writer.getLine());
    }

    return result;
}

void NodeScalar::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

void NodeScalar::addNode(const NodeScalarData& node) {
    nodes_.push_back(node);
}

// ============================================================================
// NodeRigidBody
// ============================================================================

bool NodeRigidBody::parse(const std::vector<std::string>& lines,
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

        auto pid = parser.getInt64At(pos);
        if (!pid) continue;

        Data entry;
        entry.pid = *pid;
        pos += intW;
        entry.nid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        entry.pnode = static_cast<int>(parser.getInt64At(pos).value_or(0));
        addEntry(entry);
    }

    return true;
}

std::vector<std::string> NodeRigidBody::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& entry : entries_) {
        writer.clear();
        writer.writeInt(entry.pid);
        writer.writeInt(entry.nid);
        writer.writeInt(entry.pnode);
        result.push_back(writer.getLine());
    }

    return result;
}

void NodeRigidBody::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

void NodeRigidBody::addEntry(const Data& entry) {
    entries_.push_back(entry);
}

// ============================================================================
// NodeThickness
// ============================================================================

bool NodeThickness::parse(const std::vector<std::string>& lines,
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

        auto nid = parser.getInt64At(pos);
        if (!nid) continue;

        NodeThicknessData node;
        node.nid = *nid;
        pos += intW;
        node.thick = parser.getDoubleAt(pos, realW).value_or(0.0);
        addNode(node);
    }

    return true;
}

std::vector<std::string> NodeThickness::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& node : nodes_) {
        writer.clear();
        writer.writeInt(node.nid);
        writer.writeDouble(node.thick);
        result.push_back(writer.getLine());
    }

    return result;
}

void NodeThickness::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

void NodeThickness::addNode(const NodeThicknessData& node) {
    nodes_.push_back(node);
}

// ============================================================================
// NodeToTarget
// ============================================================================

bool NodeToTarget::parse(const std::vector<std::string>& lines,
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

        auto nid = parser.getInt64At(pos);
        if (!nid) continue;

        Data entry;
        entry.nid = *nid;
        pos += intW;
        entry.segid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        addEntry(entry);
    }

    return true;
}

std::vector<std::string> NodeToTarget::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& entry : entries_) {
        writer.clear();
        writer.writeInt(entry.nid);
        writer.writeInt(entry.segid);
        result.push_back(writer.getLine());
    }

    return result;
}

void NodeToTarget::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

void NodeToTarget::addEntry(const Data& entry) {
    entries_.push_back(entry);
}

// ============================================================================
// NodeSpotWeld
// ============================================================================

bool NodeSpotWeld::parse(const std::vector<std::string>& lines,
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

        auto nid = parser.getInt64At(pos);
        if (!nid) continue;

        Data entry;
        entry.nid = *nid;
        pos += intW;
        entry.nsid1 = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.nsid2 = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.dn = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        entry.dt = parser.getDoubleAt(pos, realW).value_or(0.0);
        addEntry(entry);
    }

    return true;
}

std::vector<std::string> NodeSpotWeld::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& entry : entries_) {
        writer.clear();
        writer.writeInt(entry.nid);
        writer.writeInt(entry.nsid1);
        writer.writeInt(entry.nsid2);
        writer.writeDouble(entry.dn);
        writer.writeDouble(entry.dt);
        result.push_back(writer.getLine());
    }

    return result;
}

void NodeSpotWeld::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

void NodeSpotWeld::addEntry(const Data& entry) {
    entries_.push_back(entry);
}

// ============================================================================
// NodeMergeSet
// ============================================================================

bool NodeMergeSet::parse(const std::vector<std::string>& lines,
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
        data_.nsid1 = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.nsid2 = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.tol = parser.getDoubleAt(pos, realW).value_or(0.0);
        break;
    }

    return true;
}

std::vector<std::string> NodeMergeSet::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.nsid1);
    writer.writeInt(data_.nsid2);
    writer.writeDouble(data_.tol);
    result.push_back(writer.getLine());

    return result;
}

void NodeMergeSet::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

// ============================================================================
// NodeMergeTolerance
// ============================================================================

bool NodeMergeTolerance::parse(const std::vector<std::string>& lines,
                               util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);
        data_.tol = parser.getDoubleAt(0, realW).value_or(0.0);
        break;
    }

    return true;
}

std::vector<std::string> NodeMergeTolerance::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeDouble(data_.tol);
    result.push_back(writer.getLine());

    return result;
}

void NodeMergeTolerance::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

// ============================================================================
// NodeReference
// ============================================================================

bool NodeReference::parse(const std::vector<std::string>& lines,
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

        auto nid = parser.getInt64At(pos);
        if (!nid) continue;

        Data entry;
        entry.nid = *nid;
        pos += intW;
        entry.cid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        addEntry(entry);
    }

    return true;
}

std::vector<std::string> NodeReference::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& entry : entries_) {
        writer.clear();
        writer.writeInt(entry.nid);
        writer.writeInt(entry.cid);
        result.push_back(writer.getLine());
    }

    return result;
}

void NodeReference::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

void NodeReference::addEntry(const Data& entry) {
    entries_.push_back(entry);
}

// ============================================================================
// NodeRigidSurface
// ============================================================================

bool NodeRigidSurface::parse(const std::vector<std::string>& lines,
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

        auto nid = parser.getInt64At(pos);
        if (!nid) continue;

        Data entry;
        entry.nid = *nid;
        pos += intW;
        entry.surfid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        addEntry(entry);
    }

    return true;
}

std::vector<std::string> NodeRigidSurface::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& entry : entries_) {
        writer.clear();
        writer.writeInt(entry.nid);
        writer.writeInt(entry.surfid);
        result.push_back(writer.getLine());
    }

    return result;
}

void NodeRigidSurface::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

void NodeRigidSurface::addEntry(const Data& entry) {
    entries_.push_back(entry);
}

// ============================================================================
// NodeScalarValue
// ============================================================================

bool NodeScalarValue::parse(const std::vector<std::string>& lines,
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

        auto nid = parser.getInt64At(pos);
        if (!nid) continue;

        Data entry;
        entry.nid = *nid;
        pos += intW;
        entry.value = parser.getDoubleAt(pos, realW).value_or(0.0);
        addEntry(entry);
    }

    return true;
}

std::vector<std::string> NodeScalarValue::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& entry : entries_) {
        writer.clear();
        writer.writeInt(entry.nid);
        writer.writeDouble(entry.value);
        result.push_back(writer.getLine());
    }

    return result;
}

void NodeScalarValue::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

void NodeScalarValue::addEntry(const Data& entry) {
    entries_.push_back(entry);
}

// ============================================================================
// NodeThicknessSet
// ============================================================================

bool NodeThicknessSet::parse(const std::vector<std::string>& lines,
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

        auto nsid = parser.getInt64At(pos);
        if (!nsid) continue;

        Data entry;
        entry.nsid = static_cast<int>(*nsid);
        pos += intW;
        entry.thick = parser.getDoubleAt(pos, realW).value_or(0.0);
        addEntry(entry);
    }

    return true;
}

std::vector<std::string> NodeThicknessSet::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& entry : entries_) {
        writer.clear();
        writer.writeInt(entry.nsid);
        writer.writeDouble(entry.thick);
        result.push_back(writer.getLine());
    }

    return result;
}

void NodeThicknessSet::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

void NodeThicknessSet::addEntry(const Data& entry) {
    entries_.push_back(entry);
}

// ============================================================================
// NodeThicknessSetGenerate
// ============================================================================

bool NodeThicknessSetGenerate::parse(const std::vector<std::string>& lines,
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
        data_.nsid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.thick1 = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.thick2 = parser.getDoubleAt(pos, realW).value_or(0.0);
        break;
    }

    return true;
}

std::vector<std::string> NodeThicknessSetGenerate::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.nsid);
    writer.writeDouble(data_.thick1);
    writer.writeDouble(data_.thick2);
    result.push_back(writer.getLine());

    return result;
}

void NodeThicknessSetGenerate::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

// ============================================================================
// NodeToTargetVector
// ============================================================================

bool NodeToTargetVector::parse(const std::vector<std::string>& lines,
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

        auto nid = parser.getInt64At(pos);
        if (!nid) continue;

        Data entry;
        entry.nid = *nid;
        pos += intW;
        entry.segid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.vx = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        entry.vy = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        entry.vz = parser.getDoubleAt(pos, realW).value_or(0.0);
        addEntry(entry);
    }

    return true;
}

std::vector<std::string> NodeToTargetVector::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& entry : entries_) {
        writer.clear();
        writer.writeInt(entry.nid);
        writer.writeInt(entry.segid);
        writer.writeDouble(entry.vx);
        writer.writeDouble(entry.vy);
        writer.writeDouble(entry.vz);
        result.push_back(writer.getLine());
    }

    return result;
}

void NodeToTargetVector::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

void NodeToTargetVector::addEntry(const Data& entry) {
    entries_.push_back(entry);
}

// Register keywords
REGISTER_KEYWORD(Node, "*NODE")
REGISTER_KEYWORD(NodeTransform, "*NODE_TRANSFORM")
REGISTER_KEYWORD(NodeMerge, "*NODE_MERGE")
REGISTER_KEYWORD(NodeScalar, "*NODE_SCALAR")
REGISTER_KEYWORD(NodeRigidBody, "*NODE_RIGID_BODY")
REGISTER_KEYWORD(NodeThickness, "*NODE_THICKNESS")
REGISTER_KEYWORD(NodeToTarget, "*NODE_TO_TARGET")
REGISTER_KEYWORD(NodeSpotWeld, "*NODE_SPOT_WELD")
REGISTER_KEYWORD(NodeMergeSet, "*NODE_MERGE_SET")
REGISTER_KEYWORD(NodeMergeTolerance, "*NODE_MERGE_TOLERANCE")
REGISTER_KEYWORD(NodeReference, "*NODE_REFERENCE")
REGISTER_KEYWORD(NodeRigidSurface, "*NODE_RIGID_SURFACE")
REGISTER_KEYWORD(NodeScalarValue, "*NODE_SCALAR_VALUE")
REGISTER_KEYWORD(NodeThicknessSet, "*NODE_THICKNESS_SET")
REGISTER_KEYWORD(NodeThicknessSetGenerate, "*NODE_THICKNESS_SET_GENERATE")
REGISTER_KEYWORD(NodeToTargetVector, "*NODE_TO_TARGET_VECTOR")

} // namespace koo::dyna
