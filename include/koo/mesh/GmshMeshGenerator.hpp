#pragma once

#include <koo/Export.hpp>
#include <koo/mesh/IMeshGenerator.hpp>
#include <koo/mesh/MeshData.hpp>
#include <koo/mesh/MeshParameters.hpp>
#include <koo/mesh/MeshQuality.hpp>
#include <koo/cad/Geometry.hpp>
#include <memory>
#include <string>

namespace koo::mesh {

/**
 * @brief Gmsh implementation of IMeshGenerator
 *
 * Provides mesh generation using Gmsh API:
 * - Surface meshing (triangulation, quadrangulation)
 * - Volume meshing (tetrahedralization, hexahedralization)
 * - Mesh quality control
 * - Mesh optimization
 * - Mesh refinement
 *
 * Gmsh context is wrapped in opaque pointers to avoid
 * exposing Gmsh headers to library users.
 *
 * Usage:
 *   GmshMeshGenerator mesher;
 *   mesher.setGeometry(geometry);
 *
 *   MeshParameters params = MeshParameters::medium(5.0);
 *   if (mesher.generateVolumeMesh(params)) {
 *       auto meshData = mesher.getMeshData();
 *       auto quality = mesher.computeQuality();
 *   }
 */
class KOO_API GmshMeshGenerator : public IMeshGenerator {
public:
    /**
     * @brief Constructor
     */
    GmshMeshGenerator();

    /**
     * @brief Destructor
     */
    ~GmshMeshGenerator() override;

    // Prevent copying (contains Gmsh context)
    GmshMeshGenerator(const GmshMeshGenerator&) = delete;
    GmshMeshGenerator& operator=(const GmshMeshGenerator&) = delete;

    // Allow moving
    GmshMeshGenerator(GmshMeshGenerator&&) noexcept;
    GmshMeshGenerator& operator=(GmshMeshGenerator&&) noexcept;

    // ========================================================================
    // IMeshGenerator Interface Implementation
    // ========================================================================

    bool setGeometry(std::shared_ptr<cad::Geometry> geometry) override;
    bool hasGeometry() const override;

    bool generateSurfaceMesh(const MeshParameters& params) override;
    bool generateVolumeMesh(const MeshParameters& params) override;
    bool generateMesh(const MeshParameters& params) override;
    std::string getLastError() const override;

    std::shared_ptr<MeshData> getMeshData() const override;
    bool hasMesh() const override;

    MeshQuality computeQuality() const override;
    std::string getStatistics() const override;

    bool refineMesh(double targetSize) override;
    bool refineMeshInRegion(
        const common::Vec3& center,
        double radius,
        double targetSize
    ) override;

    bool optimizeMesh(int numPasses = 3) override;

    void clear() override;

    // ========================================================================
    // Gmsh-Specific Methods
    // ========================================================================

    /**
     * @brief Set number of CPU cores for parallel meshing
     * @param numCores Number of cores (0 = automatic)
     */
    void setNumCores(int numCores);

    /**
     * @brief Get number of CPU cores
     */
    int getNumCores() const;

    /**
     * @brief Enable/disable mesh smoothing
     * @param enable Enable smoothing
     */
    void setSmoothing(bool enable);

    /**
     * @brief Check if smoothing is enabled
     */
    bool isSmoothingEnabled() const;

    /**
     * @brief Set mesh algorithm
     * @param algorithm Mesh algorithm
     */
    void setMeshAlgorithm(MeshAlgorithm algorithm);

    /**
     * @brief Get mesh algorithm
     */
    MeshAlgorithm getMeshAlgorithm() const;

    /**
     * @brief Enable/disable verbose output
     * @param enable Enable verbose output
     */
    void setVerbose(bool enable);

    /**
     * @brief Check if verbose output is enabled
     */
    bool isVerbose() const;

    /**
     * @brief Save mesh to file (for debugging)
     * @param filepath Output file path (.msh, .vtk, etc.)
     * @return true if successful
     */
    bool saveMeshToFile(const std::string& filepath);

private:
    // Pimpl idiom to hide Gmsh types
    class Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace koo::mesh
