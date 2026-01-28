#pragma once

#include <koo/Export.hpp>
#include <koo/mesh/MeshData.hpp>
#include <koo/mesh/MeshParameters.hpp>
#include <koo/mesh/MeshQuality.hpp>
#include <koo/cad/Geometry.hpp>
#include <string>
#include <memory>

namespace koo::mesh {

/**
 * @brief Abstract interface for mesh generation
 *
 * This interface abstracts mesh generation operations, allowing different
 * backends (Gmsh, Netgen, TetGen, etc.) to be used interchangeably.
 *
 * Implementations are responsible for:
 * - Surface meshing (triangulation/quadrangulation)
 * - Volume meshing (tetrahedralization/hexahedralization)
 * - Mesh quality control
 * - Mesh optimization
 * - Converting mesh to library-neutral MeshData representation
 *
 * Usage:
 *   std::unique_ptr<IMeshGenerator> mesher = std::make_unique<GmshMeshGenerator>();
 *   mesher->setGeometry(geometry);
 *   if (mesher->generateVolumeMesh(params)) {
 *       auto meshData = mesher->getMeshData();
 *       auto quality = mesher->computeQuality();
 *   }
 */
class KOO_API IMeshGenerator {
public:
    /**
     * @brief Virtual destructor
     */
    virtual ~IMeshGenerator() = default;

    // ========================================================================
    // Geometry Setup
    // ========================================================================

    /**
     * @brief Set geometry to mesh
     * @param geometry Geometry representation from CAD importer
     * @return true if geometry is valid for meshing
     *
     * The geometry must be set before calling any mesh generation methods.
     */
    virtual bool setGeometry(std::shared_ptr<cad::Geometry> geometry) = 0;

    /**
     * @brief Check if geometry is loaded
     * @return true if geometry exists
     */
    virtual bool hasGeometry() const = 0;

    // ========================================================================
    // Mesh Generation
    // ========================================================================

    /**
     * @brief Generate surface mesh
     * @param params Mesh parameters
     * @return true if successful
     *
     * Creates 2D mesh (triangles/quads) on all geometry surfaces.
     */
    virtual bool generateSurfaceMesh(const MeshParameters& params) = 0;

    /**
     * @brief Generate volume mesh
     * @param params Mesh parameters
     * @return true if successful
     *
     * Creates 3D mesh (tetrahedra/hexahedra) for all geometry solids.
     * Automatically generates surface mesh first if not already done.
     */
    virtual bool generateVolumeMesh(const MeshParameters& params) = 0;

    /**
     * @brief Generate mesh (surface and/or volume based on params)
     * @param params Mesh parameters
     * @return true if successful
     *
     * Convenience method that calls generateSurfaceMesh and/or
     * generateVolumeMesh based on params.meshType.
     */
    virtual bool generateMesh(const MeshParameters& params) = 0;

    /**
     * @brief Get last error message
     * @return Error message from last failed operation
     */
    virtual std::string getLastError() const = 0;

    // ========================================================================
    // Mesh Data Access
    // ========================================================================

    /**
     * @brief Get generated mesh data
     * @return Shared pointer to MeshData
     *
     * Returns nullptr if no mesh has been generated.
     */
    virtual std::shared_ptr<MeshData> getMeshData() const = 0;

    /**
     * @brief Check if mesh exists
     * @return true if mesh has been generated
     */
    virtual bool hasMesh() const = 0;

    // ========================================================================
    // Mesh Quality
    // ========================================================================

    /**
     * @brief Compute mesh quality metrics
     * @return Quality metrics
     *
     * Computes aspect ratio, Jacobian, skewness, etc.
     * Requires mesh to be generated first.
     */
    virtual MeshQuality computeQuality() const = 0;

    /**
     * @brief Get mesh statistics
     * @return Statistics string (node count, element count, etc.)
     */
    virtual std::string getStatistics() const = 0;

    // ========================================================================
    // Mesh Refinement
    // ========================================================================

    /**
     * @brief Refine mesh globally
     * @param targetSize Target element size
     * @return true if successful
     *
     * Refines entire mesh to target element size.
     */
    virtual bool refineMesh(double targetSize) = 0;

    /**
     * @brief Refine mesh in a region
     * @param center Center of refinement region
     * @param radius Radius of refinement region
     * @param targetSize Target element size in region
     * @return true if successful
     */
    virtual bool refineMeshInRegion(
        const common::Vec3& center,
        double radius,
        double targetSize
    ) = 0;

    // ========================================================================
    // Mesh Optimization
    // ========================================================================

    /**
     * @brief Optimize mesh quality
     * @param numPasses Number of optimization passes
     * @return true if successful
     *
     * Applies smoothing, edge swapping, etc. to improve element quality.
     */
    virtual bool optimizeMesh(int numPasses = 3) = 0;

    // ========================================================================
    // Cleanup
    // ========================================================================

    /**
     * @brief Clear all mesh data
     */
    virtual void clear() = 0;
};

/**
 * @brief Factory function to create mesh generator
 * @param backend Backend name ("gmsh", "netgen", "tetgen", etc.)
 * @return Unique pointer to mesh generator, or nullptr if backend unavailable
 *
 * Usage:
 *   auto mesher = createMeshGenerator("gmsh");
 *   if (!mesher) {
 *       // Gmsh not available
 *   }
 */
KOO_API std::unique_ptr<IMeshGenerator> createMeshGenerator(const std::string& backend = "gmsh");

} // namespace koo::mesh
