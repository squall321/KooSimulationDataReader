#include <koo/dyna/managers/ElementManager.hpp>
#include <koo/dyna/Element.hpp>
#include <koo/dyna/Define.hpp>
#include <algorithm>

namespace koo::dyna::managers {

ElementManager::ElementManager(Model& model)
    : model_(model)
    , indexBuilt_(false)
{
}

void ElementManager::buildIndex() {
    // Clear existing indices
    elementIndex_.clear();
    elementToPart_.clear();
    elementType_.clear();
    typeToElements_.clear();
    birthTimes_.clear();
    deathTimes_.clear();

    // Helper lambda to index elements
    auto indexElements = [this](const auto* keyword, ElementType type) {
        if (!keyword) return;

        const auto& elements = keyword->getElements();
        for (const auto& elem : elements) {
            elementIndex_[elem.id] = &elem;
            elementToPart_[elem.id] = elem.pid;
            elementType_[elem.id] = type;
            typeToElements_[type].push_back(elem.id);
        }
    };

    // Index shell elements
    indexElements(model_.getShellElements(), ElementType::Shell);

    // Index solid elements
    indexElements(model_.getSolidElements(), ElementType::Solid);

    // Index beam elements
    auto beamKeywords = model_.getKeywordsOfType<ElementBeam>();
    for (auto* keyword : beamKeywords) {
        const auto& elements = keyword->getElements();
        for (const auto& elem : elements) {
            elementIndex_[elem.id] = &elem;
            elementToPart_[elem.id] = elem.pid;
            elementType_[elem.id] = ElementType::Beam;
            typeToElements_[ElementType::Beam].push_back(elem.id);
        }
    }

    // Index discrete elements
    auto discreteKeywords = model_.getKeywordsOfType<ElementDiscrete>();
    for (auto* keyword : discreteKeywords) {
        const auto& elements = keyword->getElements();
        for (const auto& elem : elements) {
            elementIndex_[elem.id] = &elem;
            elementToPart_[elem.id] = elem.pid;
            elementType_[elem.id] = ElementType::Discrete;
            typeToElements_[ElementType::Discrete].push_back(elem.id);
        }
    }

    // Index seatbelt elements
    auto seatbeltKeywords = model_.getKeywordsOfType<ElementSeatbelt>();
    for (auto* keyword : seatbeltKeywords) {
        const auto& elements = keyword->getElements();
        for (const auto& elem : elements) {
            elementIndex_[elem.id] = &elem;
            elementToPart_[elem.id] = elem.pid;
            elementType_[elem.id] = ElementType::Seatbelt;
            typeToElements_[ElementType::Seatbelt].push_back(elem.id);
        }
    }

    // Build birth/death time index
    buildBirthDeathIndex();

    indexBuilt_ = true;
}

void ElementManager::buildBirthDeathIndex() {
    // Index birth times from DEFINE_BIRTH_TIMES
    auto birthKeywords = model_.getKeywordsOfType<DefineBirthTimes>();
    for (auto* keyword : birthKeywords) {
        const auto& data = keyword->getData();
        for (const auto& entry : data) {
            birthTimes_[entry.eid] = entry.timeb;
        }
    }

    // Index death times from DEFINE_DEATH_TIMES
    auto deathKeywords = model_.getKeywordsOfType<DefineDeathTimes>();
    for (auto* keyword : deathKeywords) {
        const auto& data = keyword->getData();
        for (const auto& entry : data) {
            deathTimes_[entry.eid] = entry.timed;
        }
    }
}

void ElementManager::clearIndex() {
    elementIndex_.clear();
    elementToPart_.clear();
    elementType_.clear();
    typeToElements_.clear();
    birthTimes_.clear();
    deathTimes_.clear();
    indexBuilt_ = false;
}

const ElementData* ElementManager::getElement(ElementId eid) const {
    auto it = elementIndex_.find(eid);
    return it != elementIndex_.end() ? it->second : nullptr;
}

std::vector<ElementId> ElementManager::getAllElementIds() const {
    std::vector<ElementId> result;
    result.reserve(elementIndex_.size());
    for (const auto& [eid, _] : elementIndex_) {
        result.push_back(eid);
    }
    return result;
}

bool ElementManager::hasElement(ElementId eid) const {
    return elementIndex_.find(eid) != elementIndex_.end();
}

size_t ElementManager::getElementCount() const {
    return elementIndex_.size();
}

ElementType ElementManager::getElementType(ElementId eid) const {
    auto it = elementType_.find(eid);
    return it != elementType_.end() ? it->second : ElementType::Unknown;
}

std::vector<ElementId> ElementManager::getElementsByType(ElementType type) const {
    auto it = typeToElements_.find(type);
    return it != typeToElements_.end() ? it->second : std::vector<ElementId>{};
}

std::vector<ElementId> ElementManager::getShellElements() const {
    return getElementsByType(ElementType::Shell);
}

std::vector<ElementId> ElementManager::getSolidElements() const {
    return getElementsByType(ElementType::Solid);
}

std::vector<ElementId> ElementManager::getBeamElements() const {
    return getElementsByType(ElementType::Beam);
}

std::vector<ElementId> ElementManager::getDiscreteElements() const {
    return getElementsByType(ElementType::Discrete);
}

PartId ElementManager::getPartId(ElementId eid) const {
    auto it = elementToPart_.find(eid);
    return it != elementToPart_.end() ? it->second : 0;
}

std::vector<NodeId> ElementManager::getNodes(ElementId eid) const {
    const ElementData* elem = getElement(eid);
    return elem ? elem->nodeIds : std::vector<NodeId>{};
}

size_t ElementManager::getNodeCount(ElementId eid) const {
    const ElementData* elem = getElement(eid);
    return elem ? elem->nodeIds.size() : 0;
}

std::vector<ElementManager::Segment> ElementManager::getSegments(ElementId eid) const {
    ElementType type = getElementType(eid);

    if (type == ElementType::Shell) {
        // Get shell element
        auto* shellKeyword = model_.getShellElements();
        if (shellKeyword) {
            const ShellElementData* elem = shellKeyword->getElement(eid);
            if (elem) {
                return extractShellSegments(*elem);
            }
        }
    }
    else if (type == ElementType::Solid) {
        // Get solid element
        auto* solidKeyword = model_.getSolidElements();
        if (solidKeyword) {
            const SolidElementData* elem = solidKeyword->getElement(eid);
            if (elem) {
                return extractSolidSegments(*elem);
            }
        }
    }

    // Beam, discrete, etc. don't have segments
    return {};
}

std::vector<ElementManager::Segment> ElementManager::getAllSegments() const {
    std::vector<Segment> allSegments;

    // Extract from all shell elements
    auto* shellKeyword = model_.getShellElements();
    if (shellKeyword) {
        for (const auto& elem : shellKeyword->getElements()) {
            auto segments = extractShellSegments(elem);
            allSegments.insert(allSegments.end(), segments.begin(), segments.end());
        }
    }

    // Extract from all solid elements
    auto* solidKeyword = model_.getSolidElements();
    if (solidKeyword) {
        for (const auto& elem : solidKeyword->getElements()) {
            auto segments = extractSolidSegments(elem);
            allSegments.insert(allSegments.end(), segments.begin(), segments.end());
        }
    }

    return allSegments;
}

std::optional<double> ElementManager::getBirthTime(ElementId eid) const {
    auto it = birthTimes_.find(eid);
    if (it != birthTimes_.end()) {
        return it->second;
    }
    return std::nullopt;
}

std::optional<double> ElementManager::getDeathTime(ElementId eid) const {
    auto it = deathTimes_.find(eid);
    if (it != deathTimes_.end()) {
        return it->second;
    }
    return std::nullopt;
}

bool ElementManager::isAliveAt(ElementId eid, double time) const {
    // Check birth time
    auto birth = getBirthTime(eid);
    if (birth && time < *birth) {
        return false;  // Not yet born
    }

    // Check death time
    auto death = getDeathTime(eid);
    if (death && time >= *death) {
        return false;  // Already dead
    }

    return true;  // Alive
}

std::vector<ElementManager::Segment> ElementManager::extractShellSegments(
    const ShellElementData& elem) const
{
    std::vector<Segment> segments;

    // Shell has one segment (the shell face itself)
    std::vector<NodeId> nodes;
    for (NodeId nid : elem.nodeIds) {
        if (nid != 0) {  // Skip zero node IDs (tri3 has only 3 nodes)
            nodes.push_back(nid);
        }
    }

    if (!nodes.empty()) {
        segments.emplace_back(nodes, elem.id, 0);
    }

    return segments;
}

std::vector<ElementManager::Segment> ElementManager::extractSolidSegments(
    const SolidElementData& elem) const
{
    std::vector<Segment> segments;

    const auto& nodes = elem.nodeIds;
    const size_t nNodes = nodes.size();

    if (nNodes == 8) {
        // Hexahedron (8 nodes) - 6 quad faces
        // LS-DYNA node numbering for hex8
        // Face indices follow LS-DYNA convention:
        // Face 0: nodes 1-2-3-4 (bottom)
        // Face 1: nodes 5-6-7-8 (top)
        // Face 2: nodes 1-2-6-5 (front)
        // Face 3: nodes 2-3-7-6 (right)
        // Face 4: nodes 3-4-8-7 (back)
        // Face 5: nodes 4-1-5-8 (left)

        std::vector<std::vector<int>> faceIndices = {
            {0, 1, 2, 3},  // Bottom
            {4, 5, 6, 7},  // Top
            {0, 1, 5, 4},  // Front
            {1, 2, 6, 5},  // Right
            {2, 3, 7, 6},  // Back
            {3, 0, 4, 7}   // Left
        };

        for (size_t faceIdx = 0; faceIdx < faceIndices.size(); ++faceIdx) {
            std::vector<NodeId> faceNodes;
            for (int nodeIdx : faceIndices[faceIdx]) {
                if (static_cast<size_t>(nodeIdx) < nodes.size() && nodes[nodeIdx] != 0) {
                    faceNodes.push_back(nodes[nodeIdx]);
                }
            }
            if (faceNodes.size() >= 3) {
                segments.emplace_back(faceNodes, elem.id, static_cast<int>(faceIdx));
            }
        }
    }
    else if (nNodes == 6) {
        // Wedge/Pentahedron (6 nodes) - 5 faces (2 tri + 3 quad)
        // Faces:
        // Face 0: nodes 1-2-3 (tri bottom)
        // Face 1: nodes 4-5-6 (tri top)
        // Face 2: nodes 1-2-5-4 (quad)
        // Face 3: nodes 2-3-6-5 (quad)
        // Face 4: nodes 3-1-4-6 (quad)

        std::vector<std::vector<int>> faceIndices = {
            {0, 1, 2},     // Tri bottom
            {3, 4, 5},     // Tri top
            {0, 1, 4, 3},  // Quad 1
            {1, 2, 5, 4},  // Quad 2
            {2, 0, 3, 5}   // Quad 3
        };

        for (size_t faceIdx = 0; faceIdx < faceIndices.size(); ++faceIdx) {
            std::vector<NodeId> faceNodes;
            for (int nodeIdx : faceIndices[faceIdx]) {
                if (static_cast<size_t>(nodeIdx) < nodes.size() && nodes[nodeIdx] != 0) {
                    faceNodes.push_back(nodes[nodeIdx]);
                }
            }
            if (faceNodes.size() >= 3) {
                segments.emplace_back(faceNodes, elem.id, static_cast<int>(faceIdx));
            }
        }
    }
    else if (nNodes == 4) {
        // Tetrahedron (4 nodes) - 4 tri faces
        // Face 0: nodes 1-2-3
        // Face 1: nodes 1-4-2
        // Face 2: nodes 2-4-3
        // Face 3: nodes 3-4-1

        std::vector<std::vector<int>> faceIndices = {
            {0, 1, 2},
            {0, 3, 1},
            {1, 3, 2},
            {2, 3, 0}
        };

        for (size_t faceIdx = 0; faceIdx < faceIndices.size(); ++faceIdx) {
            std::vector<NodeId> faceNodes;
            for (int nodeIdx : faceIndices[faceIdx]) {
                if (static_cast<size_t>(nodeIdx) < nodes.size() && nodes[nodeIdx] != 0) {
                    faceNodes.push_back(nodes[nodeIdx]);
                }
            }
            if (faceNodes.size() == 3) {
                segments.emplace_back(faceNodes, elem.id, static_cast<int>(faceIdx));
            }
        }
    }

    return segments;
}

} // namespace koo::dyna::managers
