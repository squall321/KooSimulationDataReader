#include <koo/dyna/managers/PartManager.hpp>
#include <koo/dyna/Element.hpp>
#include <koo/dyna/Node.hpp>
#include <koo/dyna/Part.hpp>
#include <algorithm>
#include <unordered_set>

namespace koo::dyna::managers {

PartManager::PartManager(Model& model)
    : model_(model)
    , indexBuilt_(false)
{
}

void PartManager::buildIndex() {
    // Clear existing indices
    partToElements_.clear();
    partToNodes_.clear();

    // Get all element information
    auto allElements = getAllElementInfo();

    // Build part → elements mapping
    for (const auto& elem : allElements) {
        partToElements_[elem.pid].push_back(elem.id);
    }

    // Build part → nodes mapping (unique nodes per part)
    for (const auto& [pid, elementIds] : partToElements_) {
        std::unordered_set<NodeId> uniqueNodes;

        // Collect all nodes from all elements in this part
        for (const auto& elem : allElements) {
            if (elem.pid == pid) {
                for (NodeId nid : elem.nodeIds) {
                    if (nid != 0) {  // Skip invalid node IDs
                        uniqueNodes.insert(nid);
                    }
                }
            }
        }

        // Convert to sorted vector
        std::vector<NodeId> nodeVec(uniqueNodes.begin(), uniqueNodes.end());
        std::sort(nodeVec.begin(), nodeVec.end());
        partToNodes_[pid] = std::move(nodeVec);
    }

    indexBuilt_ = true;
}

void PartManager::clearIndex() {
    partToElements_.clear();
    partToNodes_.clear();
    indexBuilt_ = false;
}

const PartData* PartManager::getPart(PartId pid) const {
    return model_.findPart(pid);
}

std::vector<PartId> PartManager::getAllPartIds() const {
    std::vector<PartId> result;

    // Get Part keyword
    auto* partKeyword = model_.getParts();
    if (!partKeyword) {
        return result;
    }

    // Extract all part IDs
    const auto& parts = partKeyword->getParts();
    result.reserve(parts.size());
    for (const auto& part : parts) {
        result.push_back(part.id);
    }

    return result;
}

bool PartManager::hasPart(PartId pid) const {
    return getPart(pid) != nullptr;
}

size_t PartManager::getPartCount() const {
    auto* partKeyword = model_.getParts();
    return partKeyword ? partKeyword->getPartCount() : 0;
}

std::vector<ElementId> PartManager::getElements(PartId pid) const {
    auto it = partToElements_.find(pid);
    if (it != partToElements_.end()) {
        return it->second;
    }
    return {};
}

size_t PartManager::getElementCount(PartId pid) const {
    auto it = partToElements_.find(pid);
    return it != partToElements_.end() ? it->second.size() : 0;
}

std::vector<NodeId> PartManager::getNodes(PartId pid) const {
    auto it = partToNodes_.find(pid);
    if (it != partToNodes_.end()) {
        return it->second;
    }
    return {};
}

size_t PartManager::getNodeCount(PartId pid) const {
    auto it = partToNodes_.find(pid);
    return it != partToNodes_.end() ? it->second.size() : 0;
}

PartManager::Statistics PartManager::getStatistics(PartId pid) const {
    Statistics stats;
    stats.partId = pid;

    // Get part data
    const PartData* part = getPart(pid);
    if (!part) {
        return stats;  // Return empty stats
    }

    stats.title = part->title;
    stats.materialId = part->mid;
    stats.sectionId = part->secid;

    // Get element and node counts
    stats.elementCount = getElementCount(pid);
    stats.nodeCount = getNodeCount(pid);

    // Compute bounding box
    stats.boundingBox = getBoundingBox(pid);

    return stats;
}

bool PartManager::setMaterial(PartId pid, MaterialId mid) {
    PartData* part = model_.findPart(pid);
    if (!part) {
        return false;
    }

    part->mid = mid;
    return true;
}

bool PartManager::setSection(PartId pid, SectionId sid) {
    PartData* part = model_.findPart(pid);
    if (!part) {
        return false;
    }

    part->secid = sid;
    return true;
}

std::string PartManager::getTitle(PartId pid) const {
    const PartData* part = getPart(pid);
    return part ? part->title : "";
}

BoundingBox PartManager::getBoundingBox(PartId pid) const {
    BoundingBox bbox;

    // Get nodes for this part
    auto nodeIds = getNodes(pid);
    if (nodeIds.empty()) {
        return bbox;  // Return invalid bbox
    }

    // Get Node keyword
    const Node* nodeKeyword = model_.getNodes();
    if (!nodeKeyword) {
        return bbox;
    }

    // Expand bounding box with each node's coordinates
    for (NodeId nid : nodeIds) {
        const NodeData* node = nodeKeyword->getNode(nid);
        if (node) {
            bbox.expand(node->position);
        }
    }

    return bbox;
}

std::vector<PartManager::ElementInfo> PartManager::getAllElementInfo() const {
    std::vector<ElementInfo> result;

    // Helper lambda to extract element info
    auto extractInfo = [&result](const auto* elemKeyword, ElementType type) {
        if (!elemKeyword) return;

        const auto& elements = elemKeyword->getElements();
        for (const auto& elem : elements) {
            ElementInfo info;
            info.id = elem.id;
            info.pid = elem.pid;
            info.nodeIds = elem.nodeIds;
            info.type = type;
            result.push_back(info);
        }
    };

    // Collect shell elements
    extractInfo(model_.getShellElements(), ElementType::Shell);

    // Collect solid elements
    extractInfo(model_.getSolidElements(), ElementType::Solid);

    // Collect beam elements
    auto beamKeywords = model_.getKeywordsOfType<ElementBeam>();
    for (auto* beamKw : beamKeywords) {
        const auto& elements = beamKw->getElements();
        for (const auto& elem : elements) {
            ElementInfo info;
            info.id = elem.id;
            info.pid = elem.pid;
            info.nodeIds = elem.nodeIds;
            // Add orientation node if present
            if (elem.n3 != 0) {
                info.nodeIds.push_back(elem.n3);
            }
            info.type = ElementType::Beam;
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
            info.pid = elem.pid;
            info.nodeIds = elem.nodeIds;
            info.type = ElementType::Discrete;
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
            info.pid = elem.pid;
            info.nodeIds = elem.nodeIds;
            info.type = ElementType::Seatbelt;
            result.push_back(info);
        }
    }

    // Note: Mass elements don't have part IDs in LS-DYNA
    // (they use PID=0 or no PID), so we skip them for part-based queries

    return result;
}

} // namespace koo::dyna::managers
