#include <koo/dyna/managers/SetManager.hpp>
#include <koo/dyna/managers/PartManager.hpp>
#include <koo/dyna/managers/ElementManager.hpp>
#include <koo/dyna/Set.hpp>
#include <algorithm>
#include <unordered_map>
#include <sstream>

namespace koo::dyna::managers {

SetManager::SetManager(Model& model, PartManager& partMgr, ElementManager& elemMgr)
    : model_(model)
    , partManager_(partMgr)
    , elementManager_(elemMgr)
{
    scanExistingSets();
}

void SetManager::scanExistingSets() {
    // Scan node sets
    auto nodeSets = model_.getKeywordsOfType<SetNodeList>();
    for (const auto* kw : nodeSets) {
        int sid = kw->getSetId();
        if (sid >= nextNodeSetId_) {
            nextNodeSetId_ = sid + 1;
        }
    }

    auto nodeSetsTitled = model_.getKeywordsOfType<SetNodeListTitle>();
    for (const auto* kw : nodeSetsTitled) {
        int sid = kw->getSetId();
        if (sid >= nextNodeSetId_) {
            nextNodeSetId_ = sid + 1;
        }
    }

    // Scan segment sets
    auto segmentSets = model_.getKeywordsOfType<SetSegment>();
    for (const auto* kw : segmentSets) {
        int sid = kw->getSetId();
        if (sid >= nextSegmentSetId_) {
            nextSegmentSetId_ = sid + 1;
        }
    }

    auto segmentSetsTitled = model_.getKeywordsOfType<SetSegmentTitle>();
    for (const auto* kw : segmentSetsTitled) {
        int sid = kw->getSetId();
        if (sid >= nextSegmentSetId_) {
            nextSegmentSetId_ = sid + 1;
        }
    }

    // Scan part sets
    auto partSets = model_.getKeywordsOfType<SetPartList>();
    for (const auto* kw : partSets) {
        int sid = kw->getSetId();
        if (sid >= nextPartSetId_) {
            nextPartSetId_ = sid + 1;
        }
    }

    auto partSetsTitled = model_.getKeywordsOfType<SetPartListTitle>();
    for (const auto* kw : partSetsTitled) {
        int sid = kw->getSetId();
        if (sid >= nextPartSetId_) {
            nextPartSetId_ = sid + 1;
        }
    }

    // Scan shell sets
    auto shellSets = model_.getKeywordsOfType<SetShellList>();
    for (const auto* kw : shellSets) {
        int sid = kw->getSetId();
        if (sid >= nextShellSetId_) {
            nextShellSetId_ = sid + 1;
        }
    }

    auto shellSetsTitled = model_.getKeywordsOfType<SetShellListTitle>();
    for (const auto* kw : shellSetsTitled) {
        int sid = kw->getSetId();
        if (sid >= nextShellSetId_) {
            nextShellSetId_ = sid + 1;
        }
    }

    // Scan solid sets
    auto solidSets = model_.getKeywordsOfType<SetSolidList>();
    for (const auto* kw : solidSets) {
        int sid = kw->getSetId();
        if (sid >= nextSolidSetId_) {
            nextSolidSetId_ = sid + 1;
        }
    }

    auto solidSetsTitled = model_.getKeywordsOfType<SetSolidListTitle>();
    for (const auto* kw : solidSetsTitled) {
        int sid = kw->getSetId();
        if (sid >= nextSolidSetId_) {
            nextSolidSetId_ = sid + 1;
        }
    }
}

int SetManager::getNextAvailableSetId(const std::string& type) {
    if (type == "node") {
        return nextNodeSetId_++;
    } else if (type == "segment") {
        return nextSegmentSetId_++;
    } else if (type == "part") {
        return nextPartSetId_++;
    } else if (type == "shell") {
        return nextShellSetId_++;
    } else if (type == "solid") {
        return nextSolidSetId_++;
    }
    return 0;  // Invalid type
}

// ============================================================================
// Node Set Operations
// ============================================================================

int SetManager::createNodeSet(const std::vector<NodeId>& nodes,
                               const std::string& title) {
    int setId = getNextAvailableSetId("node");

    auto keyword = std::make_unique<SetNodeListTitle>();
    keyword->setSetId(setId);
    keyword->setTitle(title);

    auto& nodeList = keyword->getNodes();
    nodeList = nodes;

    model_.addKeyword(std::move(keyword));
    return setId;
}

int SetManager::createNodeSetFromPart(PartId pid, const std::string& title) {
    auto nodes = partManager_.getNodes(pid);

    std::string finalTitle = title;
    if (finalTitle.empty()) {
        std::ostringstream oss;
        oss << "Part" << pid << "_Nodes";
        finalTitle = oss.str();
    }

    return createNodeSet(nodes, finalTitle);
}

std::vector<NodeId> SetManager::getNodeSet(int setId) const {
    // Check SetNodeListTitle
    auto keywordsTitled = model_.getKeywordsOfType<SetNodeListTitle>();
    for (const auto* kw : keywordsTitled) {
        if (kw->getSetId() == setId) {
            return kw->getNodes();
        }
    }

    // Check SetNodeList
    auto keywords = model_.getKeywordsOfType<SetNodeList>();
    for (const auto* kw : keywords) {
        if (kw->getSetId() == setId) {
            return kw->getNodes();
        }
    }

    return {};
}

std::vector<int> SetManager::getAllNodeSetIds() const {
    std::vector<int> result;

    auto keywordsTitled = model_.getKeywordsOfType<SetNodeListTitle>();
    for (const auto* kw : keywordsTitled) {
        result.push_back(kw->getSetId());
    }

    auto keywords = model_.getKeywordsOfType<SetNodeList>();
    for (const auto* kw : keywords) {
        result.push_back(kw->getSetId());
    }

    return result;
}

// ============================================================================
// Segment Set Operations
// ============================================================================

int SetManager::createSegmentSet(const std::vector<SegmentData>& segments,
                                  const std::string& title) {
    int setId = getNextAvailableSetId("segment");

    auto keyword = std::make_unique<SetSegmentTitle>();
    keyword->setSetId(setId);
    keyword->setTitle(title);

    auto& segList = keyword->getSegments();
    for (const auto& seg : segments) {
        SetSegment::Segment s;
        s.n1 = seg.n1;
        s.n2 = seg.n2;
        s.n3 = seg.n3;
        s.n4 = seg.n4;
        segList.push_back(s);
    }

    model_.addKeyword(std::move(keyword));
    return setId;
}

int SetManager::createSegmentSetFromPartSurface(PartId pid, const std::string& title) {
    // Get all elements in the part
    auto elementIds = partManager_.getElements(pid);

    // Extract all segments from these elements
    std::vector<SegmentData> allSegments;
    for (ElementId eid : elementIds) {
        auto elemSegments = elementManager_.getSegments(eid);
        for (const auto& seg : elemSegments) {
            allSegments.push_back(convertSegment(seg));
        }
    }

    // Filter for external surface (segments appearing only once)
    auto externalSegments = filterExternalSegments(allSegments);

    // Create title
    std::string finalTitle = title;
    if (finalTitle.empty()) {
        std::ostringstream oss;
        oss << "Part" << pid << "_Surface";
        finalTitle = oss.str();
    }

    return createSegmentSet(externalSegments, finalTitle);
}

int SetManager::createSegmentSetFromPartElements(PartId pid, const std::string& title) {
    // Get all elements in the part
    auto elementIds = partManager_.getElements(pid);

    // Extract ALL segments (no filtering)
    std::vector<SegmentData> allSegments;
    for (ElementId eid : elementIds) {
        auto elemSegments = elementManager_.getSegments(eid);
        for (const auto& seg : elemSegments) {
            allSegments.push_back(convertSegment(seg));
        }
    }

    // Create title
    std::string finalTitle = title;
    if (finalTitle.empty()) {
        std::ostringstream oss;
        oss << "Part" << pid << "_AllSegments";
        finalTitle = oss.str();
    }

    return createSegmentSet(allSegments, finalTitle);
}

std::vector<SetManager::SegmentData> SetManager::getSegmentSet(int setId) const {
    std::vector<SegmentData> result;

    // Check SetSegmentTitle
    auto keywordsTitled = model_.getKeywordsOfType<SetSegmentTitle>();
    for (const auto* kw : keywordsTitled) {
        if (kw->getSetId() == setId) {
            for (const auto& seg : kw->getSegments()) {
                result.emplace_back(seg.n1, seg.n2, seg.n3, seg.n4);
            }
            return result;
        }
    }

    // Check SetSegment
    auto keywords = model_.getKeywordsOfType<SetSegment>();
    for (const auto* kw : keywords) {
        if (kw->getSetId() == setId) {
            for (const auto& seg : kw->getSegments()) {
                result.emplace_back(seg.n1, seg.n2, seg.n3, seg.n4);
            }
            return result;
        }
    }

    return result;
}

std::vector<int> SetManager::getAllSegmentSetIds() const {
    std::vector<int> result;

    auto keywordsTitled = model_.getKeywordsOfType<SetSegmentTitle>();
    for (const auto* kw : keywordsTitled) {
        result.push_back(kw->getSetId());
    }

    auto keywords = model_.getKeywordsOfType<SetSegment>();
    for (const auto* kw : keywords) {
        result.push_back(kw->getSetId());
    }

    return result;
}

// ============================================================================
// Part Set Operations
// ============================================================================

int SetManager::createPartSet(const std::vector<PartId>& parts,
                               const std::string& title) {
    int setId = getNextAvailableSetId("part");

    auto keyword = std::make_unique<SetPartListTitle>();
    keyword->setSetId(setId);
    keyword->setTitle(title);

    auto& partList = keyword->getParts();
    partList = parts;

    model_.addKeyword(std::move(keyword));
    return setId;
}

std::vector<PartId> SetManager::getPartSet(int setId) const {
    // Check SetPartListTitle
    auto keywordsTitled = model_.getKeywordsOfType<SetPartListTitle>();
    for (const auto* kw : keywordsTitled) {
        if (kw->getSetId() == setId) {
            return kw->getParts();
        }
    }

    // Check SetPartList
    auto keywords = model_.getKeywordsOfType<SetPartList>();
    for (const auto* kw : keywords) {
        if (kw->getSetId() == setId) {
            return kw->getParts();
        }
    }

    return {};
}

std::vector<int> SetManager::getAllPartSetIds() const {
    std::vector<int> result;

    auto keywordsTitled = model_.getKeywordsOfType<SetPartListTitle>();
    for (const auto* kw : keywordsTitled) {
        result.push_back(kw->getSetId());
    }

    auto keywords = model_.getKeywordsOfType<SetPartList>();
    for (const auto* kw : keywords) {
        result.push_back(kw->getSetId());
    }

    return result;
}

// ============================================================================
// Shell/Solid Set Operations
// ============================================================================

int SetManager::createShellSet(const std::vector<ElementId>& elements,
                                const std::string& title) {
    int setId = getNextAvailableSetId("shell");

    auto keyword = std::make_unique<SetShellListTitle>();
    keyword->setSetId(setId);
    keyword->setTitle(title);

    auto& elemList = keyword->getElements();
    elemList = elements;

    model_.addKeyword(std::move(keyword));
    return setId;
}

int SetManager::createSolidSet(const std::vector<ElementId>& elements,
                                const std::string& title) {
    int setId = getNextAvailableSetId("solid");

    auto keyword = std::make_unique<SetSolidListTitle>();
    keyword->setSetId(setId);
    keyword->setTitle(title);

    auto& elemList = keyword->getElements();
    elemList = elements;

    model_.addKeyword(std::move(keyword));
    return setId;
}

std::vector<ElementId> SetManager::getShellSet(int setId) const {
    // Check SetShellListTitle
    auto keywordsTitled = model_.getKeywordsOfType<SetShellListTitle>();
    for (const auto* kw : keywordsTitled) {
        if (kw->getSetId() == setId) {
            return kw->getElements();
        }
    }

    // Check SetShellList
    auto keywords = model_.getKeywordsOfType<SetShellList>();
    for (const auto* kw : keywords) {
        if (kw->getSetId() == setId) {
            return kw->getElements();
        }
    }

    return {};
}

std::vector<ElementId> SetManager::getSolidSet(int setId) const {
    // Check SetSolidListTitle
    auto keywordsTitled = model_.getKeywordsOfType<SetSolidListTitle>();
    for (const auto* kw : keywordsTitled) {
        if (kw->getSetId() == setId) {
            return kw->getElements();
        }
    }

    // Check SetSolidList
    auto keywords = model_.getKeywordsOfType<SetSolidList>();
    for (const auto* kw : keywords) {
        if (kw->getSetId() == setId) {
            return kw->getElements();
        }
    }

    return {};
}

// ============================================================================
// Helper Methods
// ============================================================================

std::size_t SetManager::SegmentHash::operator()(const SegmentData& seg) const {
    // Hash based on sorted node IDs for consistent hashing
    std::vector<NodeId> nodes = {seg.n1, seg.n2, seg.n3};
    if (seg.n4 != 0) {
        nodes.push_back(seg.n4);
    }
    std::sort(nodes.begin(), nodes.end());

    std::size_t hash = 0;
    for (NodeId nid : nodes) {
        hash ^= std::hash<NodeId>{}(nid) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
    }
    return hash;
}

SetManager::SegmentData SetManager::normalizeSegment(const SegmentData& seg) const {
    // Normalize by sorting node IDs
    // This ensures segments with different orientations are treated as equal
    std::vector<NodeId> nodes = {seg.n1, seg.n2, seg.n3};
    if (seg.n4 != 0) {
        nodes.push_back(seg.n4);
    }

    std::sort(nodes.begin(), nodes.end());

    SegmentData normalized;
    normalized.n1 = nodes[0];
    normalized.n2 = nodes[1];
    normalized.n3 = nodes[2];
    normalized.n4 = nodes.size() > 3 ? nodes[3] : 0;

    return normalized;
}

std::vector<SetManager::SegmentData> SetManager::filterExternalSegments(
    const std::vector<SegmentData>& allSegments) const
{
    // Count occurrences of each normalized segment
    std::unordered_map<SegmentData, int, SegmentHash> segmentCount;

    for (const auto& seg : allSegments) {
        auto normalized = normalizeSegment(seg);
        segmentCount[normalized]++;
    }

    // Keep only segments that appear exactly once (external surface)
    std::vector<SegmentData> externalSegments;
    std::unordered_map<SegmentData, bool, SegmentHash> added;

    for (const auto& seg : allSegments) {
        auto normalized = normalizeSegment(seg);

        // If this segment appears only once AND we haven't added it yet
        if (segmentCount[normalized] == 1 && !added[normalized]) {
            externalSegments.push_back(seg);  // Keep original orientation
            added[normalized] = true;
        }
    }

    return externalSegments;
}

SetManager::SegmentData SetManager::convertSegment(
    const ElementManager::Segment& seg) const
{
    SegmentData result;

    if (seg.nodeIds.size() >= 3) {
        result.n1 = seg.nodeIds[0];
        result.n2 = seg.nodeIds[1];
        result.n3 = seg.nodeIds[2];
        result.n4 = seg.nodeIds.size() >= 4 ? seg.nodeIds[3] : 0;
    }

    return result;
}

} // namespace koo::dyna::managers
