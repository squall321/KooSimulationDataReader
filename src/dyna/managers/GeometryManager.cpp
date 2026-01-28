#include <koo/dyna/managers/GeometryManager.hpp>
#include <koo/cad/ICADImporter.hpp>
#include <koo/mesh/IMeshGenerator.hpp>
#include <koo/mesh/MeshData.hpp>

namespace koo::dyna::managers {

// ============================================================================
// Pimpl Implementation
// ============================================================================

class GeometryManager::Impl {
public:
    Model& model_;
    ModelManager& modelManager_;
    std::unique_ptr<cad::ICADImporter> cadImporter_;
    std::unique_ptr<mesh::IMeshGenerator> meshGenerator_;
    std::string lastError_;

    explicit Impl(Model& model, ModelManager& modelManager)
        : model_(model)
        , modelManager_(modelManager)
    {
        // Create CAD importer
        cadImporter_ = cad::createCADImporter("opencascade");
        if (!cadImporter_) {
            lastError_ = "OpenCASCADE backend not available - CAD import will not work";
        }

        // Create mesh generator
        meshGenerator_ = mesh::createMeshGenerator("gmsh");
        if (!meshGenerator_) {
            lastError_ = "Gmsh backend not available - mesh generation will not work";
        }
    }

    bool checkBackends() const {
        return cadImporter_ && meshGenerator_;
    }
};

// ============================================================================
// GeometryManager Implementation
// ============================================================================

GeometryManager::GeometryManager(Model& model, ModelManager& modelManager)
    : impl_(std::make_unique<Impl>(model, modelManager))
{
}

GeometryManager::~GeometryManager() = default;

GeometryManager::GeometryManager(GeometryManager&&) noexcept = default;
GeometryManager& GeometryManager::operator=(GeometryManager&&) noexcept = default;

// ========================================================================
// High-Level Workflows
// ========================================================================

PartId GeometryManager::importAndMeshCAD(
    const std::string& filepath,
    const mesh::MeshParameters& meshParams,
    int materialId,
    int sectionId,
    const std::string& partTitle)
{
    if (!impl_->checkBackends()) {
        impl_->lastError_ = "CAD/Mesh backends not available";
        return -1;
    }

    // Step 1: Load CAD file
    if (!loadCAD(filepath)) {
        return -1;
    }

    // Step 2: Generate mesh
    if (!generateMesh(meshParams)) {
        return -1;
    }

    // Step 3: Create LS-DYNA part from mesh
    PartId partId = createPartFromMesh(materialId, sectionId, partTitle);

    return partId;
}

std::vector<PartId> GeometryManager::importAndMeshAssembly(
    const std::string& filepath,
    const std::map<int, std::pair<int, int>>& solidToMatSection,
    const mesh::MeshParameters& meshParams)
{
    (void)filepath; (void)solidToMatSection; (void)meshParams;

    std::vector<PartId> partIds;
    impl_->lastError_ = "importAndMeshAssembly: Not yet fully implemented";
    return partIds;
}

// ========================================================================
// Step-by-Step Workflow
// ========================================================================

bool GeometryManager::loadCAD(const std::string& filepath) {
    if (!impl_->cadImporter_) {
        impl_->lastError_ = "CAD importer not available";
        return false;
    }

    if (!impl_->cadImporter_->importFile(filepath)) {
        impl_->lastError_ = impl_->cadImporter_->getLastError();
        return false;
    }

    return true;
}

bool GeometryManager::generateMesh(const mesh::MeshParameters& params) {
    if (!impl_->meshGenerator_) {
        impl_->lastError_ = "Mesh generator not available";
        return false;
    }

    if (!impl_->cadImporter_ || !impl_->cadImporter_->hasGeometry()) {
        impl_->lastError_ = "No CAD geometry loaded";
        return false;
    }

    // Set geometry
    auto geometry = impl_->cadImporter_->getGeometry();
    if (!impl_->meshGenerator_->setGeometry(geometry)) {
        impl_->lastError_ = "Failed to set geometry";
        return false;
    }

    // Generate mesh
    if (!impl_->meshGenerator_->generateMesh(params)) {
        impl_->lastError_ = impl_->meshGenerator_->getLastError();
        return false;
    }

    return true;
}

PartId GeometryManager::createPartFromMesh(
    int materialId,
    int sectionId,
    const std::string& partTitle)
{
    if (!impl_->meshGenerator_ || !impl_->meshGenerator_->hasMesh()) {
        impl_->lastError_ = "No mesh data available";
        return -1;
    }

    auto meshData = impl_->meshGenerator_->getMeshData();
    if (!meshData) {
        impl_->lastError_ = "Failed to retrieve mesh data";
        return -1;
    }

    // Step 1: Allocate Node IDs
    NodeId nextNodeId = 1;
    const Node* existingNodes = impl_->model_.getNodes();
    if (existingNodes) {
        for (const auto& node : existingNodes->getNodes()) {
            if (node.id >= nextNodeId) {
                nextNodeId = node.id + 1;
            }
        }
    }

    // Step 2: Add nodes to model
    Node& nodes = impl_->model_.getOrCreateNodes();
    std::unordered_map<int, NodeId> meshNodeToLSDynaNode;

    for (const auto& meshNode : meshData->getNodes()) {
        NodeData nd;
        nd.id = nextNodeId;
        nd.position.x = meshNode.position.x;
        nd.position.y = meshNode.position.y;
        nd.position.z = meshNode.position.z;
        nodes.addNode(nd);

        meshNodeToLSDynaNode[meshNode.id] = nextNodeId;
        nextNodeId++;
    }

    // Step 3: Allocate Part ID
    PartId partId = 1;
    const Part* existingParts = impl_->model_.getParts();
    if (existingParts) {
        for (const auto& part : existingParts->getParts()) {
            if (part.id >= partId) {
                partId = part.id + 1;
            }
        }
    }

    // Step 4: Create part
    Part& parts = impl_->model_.getOrCreateParts();
    parts.addPart(partId, sectionId, materialId, partTitle);

    // Step 5: Allocate Element IDs
    ElementId nextElemId = 1;
    const ElementShell* existingShells = impl_->model_.getShellElements();
    if (existingShells) {
        for (const auto& elem : existingShells->getElements()) {
            if (elem.id >= nextElemId) {
                nextElemId = elem.id + 1;
            }
        }
    }
    const ElementSolid* existingSolids = impl_->model_.getSolidElements();
    if (existingSolids) {
        for (const auto& elem : existingSolids->getElements()) {
            if (elem.id >= nextElemId) {
                nextElemId = elem.id + 1;
            }
        }
    }

    // Step 6: Add elements
    bool hasShells = false, hasSolids = false;
    for (const auto& meshElem : meshData->getElements()) {
        if (mesh::is2D(meshElem.type)) {
            hasShells = true;
        } else if (mesh::is3D(meshElem.type)) {
            hasSolids = true;
        }
    }

    ElementShell* shells = hasShells ? &impl_->model_.getOrCreateShellElements() : nullptr;
    ElementSolid* solids = hasSolids ? &impl_->model_.getOrCreateSolidElements() : nullptr;

    for (const auto& meshElem : meshData->getElements()) {
        std::vector<NodeId> dynaNodes;
        for (int meshNodeId : meshElem.nodeIds) {
            auto it = meshNodeToLSDynaNode.find(meshNodeId);
            if (it != meshNodeToLSDynaNode.end()) {
                dynaNodes.push_back(it->second);
            }
        }

        if (mesh::is2D(meshElem.type) && shells) {
            // Add shell element
            if (meshElem.type == mesh::ElementType::Tri3) {
                // Triangle: duplicate last node for quad format
                if (dynaNodes.size() >= 3) {
                    shells->addElement(nextElemId, partId, dynaNodes[0], dynaNodes[1], dynaNodes[2], dynaNodes[2]);
                }
            } else if (meshElem.type == mesh::ElementType::Quad4) {
                if (dynaNodes.size() >= 4) {
                    shells->addElement(nextElemId, partId, dynaNodes[0], dynaNodes[1], dynaNodes[2], dynaNodes[3]);
                }
            } else if (meshElem.type == mesh::ElementType::Tri6) {
                // Tri6: use corner nodes only (1st 3 nodes)
                if (dynaNodes.size() >= 6) {
                    shells->addElement(nextElemId, partId, dynaNodes[0], dynaNodes[1], dynaNodes[2], dynaNodes[2]);
                }
            } else if (meshElem.type == mesh::ElementType::Quad8) {
                // Quad8: use corner nodes only (1st 4 nodes)
                if (dynaNodes.size() >= 8) {
                    shells->addElement(nextElemId, partId, dynaNodes[0], dynaNodes[1], dynaNodes[2], dynaNodes[3]);
                }
            }

            nextElemId++;

        } else if (mesh::is3D(meshElem.type) && solids) {
            // Add solid element
            if (meshElem.type == mesh::ElementType::Tet4) {
                // Tet4: pad with zeros for hex format
                if (dynaNodes.size() >= 4) {
                    solids->addElement(nextElemId, partId,
                        dynaNodes[0], dynaNodes[1], dynaNodes[2], dynaNodes[3],
                        dynaNodes[3], dynaNodes[3], dynaNodes[3], dynaNodes[3]);
                }
            } else if (meshElem.type == mesh::ElementType::Hex8) {
                if (dynaNodes.size() >= 8) {
                    solids->addElement(nextElemId, partId,
                        dynaNodes[0], dynaNodes[1], dynaNodes[2], dynaNodes[3],
                        dynaNodes[4], dynaNodes[5], dynaNodes[6], dynaNodes[7]);
                }
            } else if (meshElem.type == mesh::ElementType::Prism6) {
                // Prism6 (wedge): pad top face
                if (dynaNodes.size() >= 6) {
                    solids->addElement(nextElemId, partId,
                        dynaNodes[0], dynaNodes[1], dynaNodes[2], dynaNodes[2],
                        dynaNodes[3], dynaNodes[4], dynaNodes[5], dynaNodes[5]);
                }
            } else if (meshElem.type == mesh::ElementType::Pyramid5) {
                // Pyramid5: pad apex
                if (dynaNodes.size() >= 5) {
                    solids->addElement(nextElemId, partId,
                        dynaNodes[0], dynaNodes[1], dynaNodes[2], dynaNodes[3],
                        dynaNodes[4], dynaNodes[4], dynaNodes[4], dynaNodes[4]);
                }
            } else if (meshElem.type == mesh::ElementType::Tet10) {
                // Tet10: use corner nodes only (1st 4 nodes)
                if (dynaNodes.size() >= 10) {
                    solids->addElement(nextElemId, partId,
                        dynaNodes[0], dynaNodes[1], dynaNodes[2], dynaNodes[3],
                        dynaNodes[3], dynaNodes[3], dynaNodes[3], dynaNodes[3]);
                }
            } else if (meshElem.type == mesh::ElementType::Hex20) {
                // Hex20: use corner nodes only (1st 8 nodes)
                if (dynaNodes.size() >= 20) {
                    solids->addElement(nextElemId, partId,
                        dynaNodes[0], dynaNodes[1], dynaNodes[2], dynaNodes[3],
                        dynaNodes[4], dynaNodes[5], dynaNodes[6], dynaNodes[7]);
                }
            }

            nextElemId++;
        }
    }

    // Step 7: Rebuild ModelManager indices
    impl_->modelManager_.rebuildIndices();

    return partId;
}

// ========================================================================
// Geometry Queries
// ========================================================================

int GeometryManager::getNumSolids() const {
    if (!impl_->cadImporter_ || !impl_->cadImporter_->hasGeometry()) {
        return 0;
    }
    return impl_->cadImporter_->getNumSolids();
}

int GeometryManager::getNumFaces() const {
    if (!impl_->cadImporter_ || !impl_->cadImporter_->hasGeometry()) {
        return 0;
    }
    return impl_->cadImporter_->getNumFaces();
}

common::BoundingBox GeometryManager::getBoundingBox() const {
    if (!impl_->cadImporter_ || !impl_->cadImporter_->hasGeometry()) {
        return common::BoundingBox();
    }
    return impl_->cadImporter_->getBoundingBox();
}

double GeometryManager::getTotalVolume() const {
    if (!impl_->cadImporter_ || !impl_->cadImporter_->hasGeometry()) {
        return 0.0;
    }
    return impl_->cadImporter_->getTotalVolume();
}

bool GeometryManager::hasGeometry() const {
    return impl_->cadImporter_ && impl_->cadImporter_->hasGeometry();
}

// ========================================================================
// Mesh Queries
// ========================================================================

size_t GeometryManager::getNodeCount() const {
    if (!impl_->meshGenerator_ || !impl_->meshGenerator_->hasMesh()) {
        return 0;
    }
    auto meshData = impl_->meshGenerator_->getMeshData();
    return meshData ? meshData->getNumNodes() : 0;
}

size_t GeometryManager::getElementCount() const {
    if (!impl_->meshGenerator_ || !impl_->meshGenerator_->hasMesh()) {
        return 0;
    }
    auto meshData = impl_->meshGenerator_->getMeshData();
    return meshData ? meshData->getNumElements() : 0;
}

mesh::MeshQuality GeometryManager::getMeshQuality() const {
    if (!impl_->meshGenerator_ || !impl_->meshGenerator_->hasMesh()) {
        return mesh::MeshQuality();
    }
    return impl_->meshGenerator_->computeQuality();
}

bool GeometryManager::hasMesh() const {
    return impl_->meshGenerator_ && impl_->meshGenerator_->hasMesh();
}

// ========================================================================
// Error Handling
// ========================================================================

std::string GeometryManager::getLastError() const {
    return impl_->lastError_;
}

// ========================================================================
// Cleanup
// ========================================================================

void GeometryManager::clear() {
    if (impl_->cadImporter_) {
        impl_->cadImporter_->clear();
    }
    if (impl_->meshGenerator_) {
        impl_->meshGenerator_->clear();
    }
    impl_->lastError_.clear();
}

} // namespace koo::dyna::managers
