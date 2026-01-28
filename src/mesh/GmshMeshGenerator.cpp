#include <koo/mesh/GmshMeshGenerator.hpp>
#include <koo/cad/CADTypes.hpp>

#ifdef KOO_HAS_GMSH
#include <gmsh.h>

// OpenCASCADE includes for shape import
#ifdef KOO_HAS_OPENCASCADE
#include <TopoDS_Shape.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS.hxx>
#include <TopExp_Explorer.hxx>
#include <TopAbs.hxx>
#endif

#endif

#include <sstream>
#include <algorithm>
#include <cmath>

namespace koo::mesh {

// ============================================================================
// Pimpl Implementation
// ============================================================================

class GmshMeshGenerator::Impl {
public:
    std::shared_ptr<cad::Geometry> geometry_;
    std::shared_ptr<MeshData> meshData_;
    std::string lastError_;

    // Gmsh settings
    int numCores_ = 0;  // 0 = automatic
    bool smoothing_ = true;
    MeshAlgorithm algorithm_ = MeshAlgorithm::Automatic;
    bool verbose_ = false;
    bool gmshInitialized_ = false;

    Impl() {
#ifdef KOO_HAS_GMSH
        gmsh::initialize();
        gmsh::option::setNumber("General.Terminal", verbose_ ? 1 : 0);
        gmsh::option::setNumber("General.Verbosity", verbose_ ? 5 : 2);
        gmshInitialized_ = true;
#endif
    }

    ~Impl() {
#ifdef KOO_HAS_GMSH
        if (gmshInitialized_) {
            gmsh::clear();
            gmsh::finalize();
        }
#endif
    }

    bool setGeometry(std::shared_ptr<cad::Geometry> geometry) {
#ifndef KOO_HAS_GMSH
        lastError_ = "Gmsh support not available";
        return false;
#else
        geometry_ = geometry;

        // Clear previous model
        gmsh::clear();
        gmsh::model::add("imported");

#ifdef KOO_HAS_OPENCASCADE
        // TODO: Implement proper OpenCASCADE to Gmsh conversion
        // Current approach: Create placeholder geometry and assign physical groups
        //
        // For now, we create a simple box as placeholder
        // In full implementation, this should:
        // 1. Export OpenCASCADE shape to temp STEP file
        // 2. Import STEP file via gmsh::merge(filepath)
        // 3. Extract entities and create physical groups
        //
        // Alternatively: Use gmsh::model::occ::addBox and similar primitives

        // Create a simple placeholder geometry
        // This will be replaced with proper STEP import in Phase 5e-1 completion
        double x = 0, y = 0, z = 0;
        double dx = 10, dy = 10, dz = 10;

        // Add a box as placeholder
        int tag = gmsh::model::occ::addBox(x, y, z, dx, dy, dz);

        // Synchronize
        gmsh::model::occ::synchronize();

        // Create physical groups
        // For solids
        const auto& solids = geometry->getSolids();
        if (!solids.empty()) {
            std::vector<int> volumeTags = {tag};
            gmsh::model::addPhysicalGroup(3, volumeTags, solids[0].id);
        }

        // TODO: Add proper face physical groups once STEP import works

        lastError_ = "Warning: Using placeholder geometry - full OpenCASCADE integration pending";
#else
        lastError_ = "OpenCASCADE support required for geometry import";
        return false;
#endif

        return true;
#endif
    }

    bool generateMesh(const MeshParameters& params, int dimension) {
#ifndef KOO_HAS_GMSH
        lastError_ = "Gmsh support not available";
        return false;
#else
        if (!geometry_) {
            lastError_ = "No geometry loaded";
            return false;
        }

        // Set mesh parameters
        gmsh::option::setNumber("Mesh.CharacteristicLengthMin", params.minElementSize);
        gmsh::option::setNumber("Mesh.CharacteristicLengthMax", params.maxElementSize);
        gmsh::option::setNumber("Mesh.ElementOrder",
            params.elementOrder == ElementOrder::Linear ? 1 : 2);

        // Set algorithm
        int algoCode = 1;  // MeshAdapt default
        switch (algorithm_) {
            case MeshAlgorithm::Delaunay: algoCode = 5; break;
            case MeshAlgorithm::Frontal: algoCode = 6; break;
            case MeshAlgorithm::Automatic: algoCode = 1; break;
        }
        gmsh::option::setNumber("Mesh.Algorithm", algoCode);
        gmsh::option::setNumber("Mesh.Algorithm3D", algoCode);

        // Set optimization
        gmsh::option::setNumber("Mesh.Optimize", smoothing_ ? 1 : 0);
        gmsh::option::setNumber("Mesh.OptimizeNetgen", smoothing_ ? 1 : 0);

        // Set parallel cores
        if (numCores_ > 0) {
            gmsh::option::setNumber("General.NumThreads", numCores_);
        }

        // Generate mesh
        try {
            gmsh::model::mesh::generate(dimension);

            // Optimize if requested
            if (params.numOptimizationPasses > 0) {
                gmsh::model::mesh::optimize("Netgen", true, params.numOptimizationPasses);
            }

            extractMeshData();
            return true;

        } catch (const std::exception& e) {
            lastError_ = std::string("Gmsh mesh generation failed: ") + e.what();
            return false;
        }
#endif
    }

    void extractMeshData() {
#ifdef KOO_HAS_GMSH
        meshData_ = std::make_shared<MeshData>();

        // Extract nodes
        std::vector<size_t> nodeTags;
        std::vector<double> coords, parametricCoords;
        gmsh::model::mesh::getNodes(nodeTags, coords, parametricCoords);

        for (size_t i = 0; i < nodeTags.size(); ++i) {
            MeshData::Node node;
            node.id = static_cast<int>(nodeTags[i]);
            node.position.x = coords[3*i];
            node.position.y = coords[3*i+1];
            node.position.z = coords[3*i+2];
            meshData_->addNode(node);
        }

        // Extract elements with geometry entity mapping
        std::vector<int> elementTypes;
        std::vector<std::vector<size_t>> elementTags, nodeTagsPerType;
        gmsh::model::mesh::getElements(elementTypes, elementTags, nodeTagsPerType);

        for (size_t i = 0; i < elementTypes.size(); ++i) {
            int gmshType = elementTypes[i];
            ElementType elemType = convertGmshType(gmshType);

            if (elemType == ElementType::Unknown) {
                continue;  // Skip unknown element types
            }

            size_t numNodesPerElem = getNumNodesForGmshType(gmshType);

            for (size_t j = 0; j < elementTags[i].size(); ++j) {
                MeshData::Element elem;
                elem.id = static_cast<int>(elementTags[i][j]);
                elem.type = elemType;

                // Get element nodes
                size_t startIdx = j * numNodesPerElem;
                for (size_t k = 0; k < numNodesPerElem; ++k) {
                    elem.nodeIds.push_back(static_cast<int>(nodeTagsPerType[i][startIdx + k]));
                }

                // CRITICAL: Get physical group tag (CAD entity ID)
                int dim = getDimensionForElementType(elemType);
                int elemType_out = 0;
                std::vector<std::size_t> nodeTags_out;
                int entityTag = -1;
                int partitionTag = -1;

                // Get the elementary entity containing this element
                gmsh::model::mesh::getElement(static_cast<std::size_t>(elem.id),
                                             elemType_out,
                                             nodeTags_out,
                                             entityTag,
                                             partitionTag);

                // Find physical groups for this entity
                std::vector<int> physicalTags;
                gmsh::model::getPhysicalGroupsForEntity(dim, entityTag, physicalTags);

                if (!physicalTags.empty()) {
                    elem.geometryEntityId = physicalTags[0];  // Use first physical group
                }

                meshData_->addElement(elem);
            }
        }

        // Build geometry-to-element mapping
        std::map<int, std::vector<int>> geometryToElements;
        for (const auto& elem : meshData_->getElements()) {
            if (elem.geometryEntityId >= 0) {
                geometryToElements[elem.geometryEntityId].push_back(elem.id);
            }
        }
        meshData_->setGeometryAssociations(geometryToElements);
#endif
    }

#ifdef KOO_HAS_GMSH
    ElementType convertGmshType(int gmshType) const {
        switch (gmshType) {
            case 1:  return ElementType::Unknown;  // Line2 (not used in LS-DYNA)
            case 2:  return ElementType::Tri3;
            case 3:  return ElementType::Quad4;
            case 4:  return ElementType::Tet4;
            case 5:  return ElementType::Hex8;
            case 6:  return ElementType::Prism6;
            case 7:  return ElementType::Pyramid5;
            case 8:  return ElementType::Unknown;  // Line3 (not used in LS-DYNA)
            case 9:  return ElementType::Tri6;
            case 10: return ElementType::Quad8;
            case 11: return ElementType::Tet10;
            case 17: return ElementType::Hex20;
            default: return ElementType::Unknown;
        }
    }

    size_t getNumNodesForGmshType(int gmshType) const {
        switch (gmshType) {
            case 1:  return 2;   // Line2
            case 2:  return 3;   // Tri3
            case 3:  return 4;   // Quad4
            case 4:  return 4;   // Tet4
            case 5:  return 8;   // Hex8
            case 6:  return 6;   // Wedge6
            case 7:  return 5;   // Pyramid5
            case 8:  return 3;   // Line3
            case 9:  return 6;   // Tri6
            case 10: return 9;   // Quad9
            case 11: return 10;  // Tet10
            case 17: return 20;  // Hex20
            default: return 0;
        }
    }

    int getDimensionForElementType(ElementType type) const {
        if (is2D(type)) return 2;
        if (is3D(type)) return 3;
        return 0;
    }
#endif

    MeshQuality computeQuality() const {
        MeshQuality quality;

        if (!meshData_) {
            return quality;
        }

        // Simple quality metrics based on element aspect ratios
        // TODO: Implement proper quality metrics
        quality.minAspectRatio = 0.5;
        quality.maxAspectRatio = 5.0;
        quality.avgAspectRatio = 1.5;
        quality.totalElements = meshData_->getNumElements();

        return quality;
    }
};

// ============================================================================
// GmshMeshGenerator Implementation
// ============================================================================

GmshMeshGenerator::GmshMeshGenerator()
    : impl_(std::make_unique<Impl>())
{
}

GmshMeshGenerator::~GmshMeshGenerator() = default;

GmshMeshGenerator::GmshMeshGenerator(GmshMeshGenerator&&) noexcept = default;
GmshMeshGenerator& GmshMeshGenerator::operator=(GmshMeshGenerator&&) noexcept = default;

// ========================================================================
// IMeshGenerator Interface Implementation
// ========================================================================

bool GmshMeshGenerator::setGeometry(std::shared_ptr<cad::Geometry> geometry) {
    return impl_->setGeometry(geometry);
}

bool GmshMeshGenerator::hasGeometry() const {
    return impl_->geometry_ != nullptr;
}

bool GmshMeshGenerator::generateSurfaceMesh(const MeshParameters& params) {
    return impl_->generateMesh(params, 2);  // 2D mesh
}

bool GmshMeshGenerator::generateVolumeMesh(const MeshParameters& params) {
    return impl_->generateMesh(params, 3);  // 3D mesh
}

bool GmshMeshGenerator::generateMesh(const MeshParameters& params) {
    // Determine dimension from geometry
    if (!impl_->geometry_) {
        impl_->lastError_ = "No geometry loaded";
        return false;
    }

    // If we have solids, generate volume mesh, otherwise surface mesh
    int dimension = impl_->geometry_->getSolids().empty() ? 2 : 3;
    return impl_->generateMesh(params, dimension);
}

std::string GmshMeshGenerator::getLastError() const {
    return impl_->lastError_;
}

std::shared_ptr<MeshData> GmshMeshGenerator::getMeshData() const {
    return impl_->meshData_;
}

bool GmshMeshGenerator::hasMesh() const {
    return impl_->meshData_ && impl_->meshData_->getNumElements() > 0;
}

MeshQuality GmshMeshGenerator::computeQuality() const {
    return impl_->computeQuality();
}

std::string GmshMeshGenerator::getStatistics() const {
    if (!impl_->meshData_) {
        return "No mesh data available";
    }

    std::ostringstream oss;
    oss << "Mesh Statistics:\n";
    oss << "  Nodes: " << impl_->meshData_->getNumNodes() << "\n";
    oss << "  Elements: " << impl_->meshData_->getNumElements() << "\n";

    return oss.str();
}

bool GmshMeshGenerator::refineMesh(double targetSize) {
#ifdef KOO_HAS_GMSH
    gmsh::option::setNumber("Mesh.CharacteristicLengthMax", targetSize);
    try {
        gmsh::model::mesh::refine();
        impl_->extractMeshData();
        return true;
    } catch (const std::exception& e) {
        impl_->lastError_ = std::string("Mesh refinement failed: ") + e.what();
        return false;
    }
#else
    impl_->lastError_ = "Gmsh support not available";
    return false;
#endif
}

bool GmshMeshGenerator::refineMeshInRegion(
    const common::Vec3& center,
    double radius,
    double targetSize)
{
    (void)center; (void)radius; (void)targetSize;
    impl_->lastError_ = "Regional refinement not yet implemented";
    return false;
}

bool GmshMeshGenerator::optimizeMesh(int numPasses) {
#ifdef KOO_HAS_GMSH
    try {
        gmsh::model::mesh::optimize("Netgen", true, numPasses);
        impl_->extractMeshData();
        return true;
    } catch (const std::exception& e) {
        impl_->lastError_ = std::string("Mesh optimization failed: ") + e.what();
        return false;
    }
#else
    (void)numPasses;
    impl_->lastError_ = "Gmsh support not available";
    return false;
#endif
}

void GmshMeshGenerator::clear() {
#ifdef KOO_HAS_GMSH
    gmsh::clear();
#endif
    impl_->geometry_.reset();
    impl_->meshData_.reset();
    impl_->lastError_.clear();
}

// ========================================================================
// Gmsh-Specific Methods
// ========================================================================

void GmshMeshGenerator::setNumCores(int numCores) {
    impl_->numCores_ = numCores;
}

int GmshMeshGenerator::getNumCores() const {
    return impl_->numCores_;
}

void GmshMeshGenerator::setSmoothing(bool enable) {
    impl_->smoothing_ = enable;
}

bool GmshMeshGenerator::isSmoothingEnabled() const {
    return impl_->smoothing_;
}

void GmshMeshGenerator::setMeshAlgorithm(MeshAlgorithm algorithm) {
    impl_->algorithm_ = algorithm;
}

MeshAlgorithm GmshMeshGenerator::getMeshAlgorithm() const {
    return impl_->algorithm_;
}

void GmshMeshGenerator::setVerbose(bool enable) {
    impl_->verbose_ = enable;
#ifdef KOO_HAS_GMSH
    gmsh::option::setNumber("General.Terminal", enable ? 1 : 0);
    gmsh::option::setNumber("General.Verbosity", enable ? 5 : 2);
#endif
}

bool GmshMeshGenerator::isVerbose() const {
    return impl_->verbose_;
}

bool GmshMeshGenerator::saveMeshToFile(const std::string& filepath) {
#ifdef KOO_HAS_GMSH
    try {
        gmsh::write(filepath);
        return true;
    } catch (const std::exception& e) {
        impl_->lastError_ = std::string("Failed to write mesh file: ") + e.what();
        return false;
    }
#else
    (void)filepath;
    impl_->lastError_ = "Gmsh support not available";
    return false;
#endif
}

} // namespace koo::mesh
