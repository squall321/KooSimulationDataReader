#include <koo/dyna/managers/NodeManager.hpp>
#include <koo/dyna/Element.hpp>
#include <koo/dyna/Node.hpp>
#include <algorithm>
#include <cmath>
#include <limits>

namespace koo::dyna::managers {

NodeManager::NodeManager(Model& model)
    : model_(model)
    , indexBuilt_(false)
{
}

void NodeManager::buildIndex() {
    // Clear existing indices
    nodeToElements_.clear();

    // Get all element information
    auto allElements = getAllElementInfo();

    // Build node → elements mapping
    for (const auto& elem : allElements) {
        for (NodeId nid : elem.nodeIds) {
            if (nid != 0) {  // Skip invalid node IDs
                nodeToElements_[nid].push_back(elem.id);
            }
        }
    }

    indexBuilt_ = true;
}

void NodeManager::clearIndex() {
    nodeToElements_.clear();
    indexBuilt_ = false;
}

const NodeData* NodeManager::getNode(NodeId nid) const {
    return model_.findNode(nid);
}

std::vector<NodeId> NodeManager::getAllNodeIds() const {
    std::vector<NodeId> result;

    const Node* nodeKeyword = model_.getNodes();
    if (!nodeKeyword) {
        return result;
    }

    const auto& nodes = nodeKeyword->getNodes();
    result.reserve(nodes.size());
    for (const auto& node : nodes) {
        result.push_back(node.id);
    }

    return result;
}

bool NodeManager::hasNode(NodeId nid) const {
    return getNode(nid) != nullptr;
}

size_t NodeManager::getNodeCount() const {
    const Node* nodeKeyword = model_.getNodes();
    return nodeKeyword ? nodeKeyword->getNodeCount() : 0;
}

std::array<double, 3> NodeManager::getCoordinates(NodeId nid) const {
    const NodeData* node = getNode(nid);
    if (node) {
        return {node->position.x, node->position.y, node->position.z};
    }
    return {0.0, 0.0, 0.0};
}

Vec3 NodeManager::getPosition(NodeId nid) const {
    const NodeData* node = getNode(nid);
    return node ? node->position : Vec3{0.0, 0.0, 0.0};
}

bool NodeManager::setCoordinates(NodeId nid, double x, double y, double z) {
    NodeData* node = model_.findNode(nid);
    if (!node) {
        return false;
    }

    node->position.x = x;
    node->position.y = y;
    node->position.z = z;
    return true;
}

bool NodeManager::setPosition(NodeId nid, const Vec3& pos) {
    NodeData* node = model_.findNode(nid);
    if (!node) {
        return false;
    }

    node->position = pos;
    return true;
}

std::vector<ElementId> NodeManager::getConnectedElements(NodeId nid) const {
    auto it = nodeToElements_.find(nid);
    if (it != nodeToElements_.end()) {
        return it->second;
    }
    return {};
}

size_t NodeManager::getConnectedElementCount(NodeId nid) const {
    auto it = nodeToElements_.find(nid);
    return it != nodeToElements_.end() ? it->second.size() : 0;
}

bool NodeManager::isBoundaryNode(NodeId nid) const {
    // Simplified boundary check:
    // A node is on the boundary if it has fewer connected elements
    // than would be expected for an interior node.
    // This is a heuristic and not perfect, but works for most cases.

    auto connectedElems = getConnectedElements(nid);
    if (connectedElems.empty()) {
        return true;  // Isolated node is technically a boundary
    }

    // For a more accurate check, we'd need to analyze element connectivity
    // and find free faces. This is a simplified version.
    // In practice, SetManager's external surface detection is more accurate.

    return false;  // Placeholder - proper implementation would check for free faces
}

std::vector<NodeId> NodeManager::findNodesNear(const Vec3& point, double radius) const {
    std::vector<NodeId> result;

    const Node* nodeKeyword = model_.getNodes();
    if (!nodeKeyword) {
        return result;
    }

    const double radiusSq = radius * radius;

    for (const auto& node : nodeKeyword->getNodes()) {
        Vec3 diff = node.position - point;
        double distSq = diff.lengthSquared();

        if (distSq <= radiusSq) {
            result.push_back(node.id);
        }
    }

    return result;
}

NodeId NodeManager::findClosestNode(const Vec3& point) const {
    const Node* nodeKeyword = model_.getNodes();
    if (!nodeKeyword) {
        return 0;
    }

    const auto& nodes = nodeKeyword->getNodes();
    if (nodes.empty()) {
        return 0;
    }

    NodeId closestId = 0;
    double minDistSq = std::numeric_limits<double>::max();

    for (const auto& node : nodes) {
        Vec3 diff = node.position - point;
        double distSq = diff.lengthSquared();

        if (distSq < minDistSq) {
            minDistSq = distSq;
            closestId = node.id;
        }
    }

    return closestId;
}

double NodeManager::computeDistance(NodeId nid1, NodeId nid2) const {
    const NodeData* node1 = getNode(nid1);
    const NodeData* node2 = getNode(nid2);

    if (!node1 || !node2) {
        return -1.0;
    }

    Vec3 diff = node2->position - node1->position;
    return diff.length();
}

std::vector<Vec3> NodeManager::getCoordinates(const std::vector<NodeId>& nodeIds) const {
    std::vector<Vec3> result;
    result.reserve(nodeIds.size());

    for (NodeId nid : nodeIds) {
        result.push_back(getPosition(nid));
    }

    return result;
}

void NodeManager::transformNodes(const std::vector<NodeId>& nodeIds, const Matrix4x4& matrix) {
    for (NodeId nid : nodeIds) {
        NodeData* node = model_.findNode(nid);
        if (node) {
            node->position = matrix * node->position;
        }
    }
}

std::vector<NodeManager::ElementInfo> NodeManager::getAllElementInfo() const {
    std::vector<ElementInfo> result;

    // Helper lambda to extract element info
    auto extractInfo = [&result](const auto* elemKeyword) {
        if (!elemKeyword) return;

        const auto& elements = elemKeyword->getElements();
        for (const auto& elem : elements) {
            ElementInfo info;
            info.id = elem.id;
            info.nodeIds = elem.nodeIds;
            result.push_back(info);
        }
    };

    // Collect shell elements
    extractInfo(model_.getShellElements());

    // Collect solid elements
    extractInfo(model_.getSolidElements());

    // Collect beam elements
    auto beamKeywords = model_.getKeywordsOfType<ElementBeam>();
    for (auto* beamKw : beamKeywords) {
        const auto& elements = beamKw->getElements();
        for (const auto& elem : elements) {
            ElementInfo info;
            info.id = elem.id;
            info.nodeIds = elem.nodeIds;
            // Add orientation node if present
            if (elem.n3 != 0) {
                info.nodeIds.push_back(elem.n3);
            }
            result.push_back(info);
        }
    }

    // Collect discrete elements
    auto discreteKeywords = model_.getKeywordsOfType<ElementDiscrete>();
    for (auto* discreteKw : discreteKeywords) {
        const auto& elements = discreteKw->getElements();
        for (const auto& elem : elements) {
            ElementInfo info;
            info.id = elem.id;
            info.nodeIds = elem.nodeIds;
            result.push_back(info);
        }
    }

    // Collect seatbelt elements
    auto seatbeltKeywords = model_.getKeywordsOfType<ElementSeatbelt>();
    for (auto* seatbeltKw : seatbeltKeywords) {
        const auto& elements = seatbeltKw->getElements();
        for (const auto& elem : elements) {
            ElementInfo info;
            info.id = elem.id;
            info.nodeIds = elem.nodeIds;
            result.push_back(info);
        }
    }

    return result;
}

} // namespace koo::dyna::managers
