#pragma once

#include <koo/Export.hpp>
#include <koo/dyna/Model.hpp>
#include <koo/dyna/managers/ModelManager.hpp>
#include <koo/mesh/MeshParameters.hpp>
#include <koo/mesh/MeshQuality.hpp>
#include <koo/common/Vec3.hpp>
#include <string>
#include <vector>
#include <map>
#include <memory>

// Forward declarations
namespace koo::cad { class ICADImporter; class Geometry; }
namespace koo::mesh { class IMeshGenerator; class MeshData; }

namespace koo::dyna::managers {

/**
 * @brief High-level manager for CAD → Mesh → LS-DYNA workflows
 *
 * GeometryManager orchestrates the complete workflow:
 * 1. Load CAD file (STEP/IGES) using OpenCASCADE
 * 2. Generate mesh using Gmsh
 * 3. Create LS-DYNA Model (parts, elements, nodes)
 * 4. Integrate with ModelManager for simulation setup
 *
 * This manager provides workflow automation for common scenarios:
 * - Single part import and meshing
 * - Assembly import with multiple solids
 * - Mesh quality control
 * - Material/section assignment
 *
 * Usage:
 *   Model model;
 *   ModelManager mgr(model);
 *   GeometryManager geomMgr(model, mgr);
 *
 *   // Import CAD and generate mesh (one call!)
 *   mesh::MeshParameters params = mesh::MeshParameters::medium(5.0);
 *   PartId partId = geomMgr.importAndMeshCAD("part.step", params, matId, sectId);
 *
 *   // Use ModelManager for simulation setup
 *   mgr.loads().applyPressureToPart(partId, 10.0);
 *   mgr.contacts().createSelfContact(partId);
 */
class KOO_API GeometryManager {
public:
    /**
     * @brief Construct GeometryManager
     * @param model Reference to Model
     * @param modelManager Reference to ModelManager for integration
     *
     * Note: model and modelManager must outlive this GeometryManager.
     */
    explicit GeometryManager(Model& model, ModelManager& modelManager);

    /**
     * @brief Destructor
     */
    ~GeometryManager();

    // Prevent copying
    GeometryManager(const GeometryManager&) = delete;
    GeometryManager& operator=(const GeometryManager&) = delete;

    // Allow moving
    GeometryManager(GeometryManager&&) noexcept;
    GeometryManager& operator=(GeometryManager&&) noexcept;

    // ========================================================================
    // High-Level Workflows
    // ========================================================================

    /**
     * @brief Import CAD file and generate mesh (complete workflow)
     * @param filepath Path to CAD file (STEP, IGES, etc.)
     * @param meshParams Mesh generation parameters
     * @param materialId Material ID to assign
     * @param sectionId Section ID to assign
     * @param partTitle Part title (optional)
     * @return Part ID if successful, -1 on failure
     *
     * Complete workflow in one call:
     * 1. Import CAD file
     * 2. Generate mesh
     * 3. Create LS-DYNA part with elements/nodes
     * 4. Assign material and section
     *
     * Example:
     *   auto params = mesh::MeshParameters::medium(5.0);
     *   PartId pid = geomMgr.importAndMeshCAD("part.step", params, 1, 1);
     */
    PartId importAndMeshCAD(
        const std::string& filepath,
        const mesh::MeshParameters& meshParams,
        int materialId,
        int sectionId,
        const std::string& partTitle = ""
    );

    /**
     * @brief Import assembly with multiple solids
     * @param filepath Path to CAD file
     * @param solidToMatSection Map: solid ID → (material ID, section ID)
     * @param meshParams Mesh generation parameters
     * @return Vector of part IDs (one per solid), empty on failure
     *
     * Imports an assembly and creates separate parts for each solid.
     * Each solid gets its own material and section.
     *
     * Example:
     *   std::map<int, std::pair<int, int>> mapping = {
     *       {1, {1, 1}},  // Solid 1: Material 1, Section 1
     *       {2, {2, 2}},  // Solid 2: Material 2, Section 2
     *   };
     *   auto partIds = geomMgr.importAndMeshAssembly("asm.step", mapping, params);
     */
    std::vector<PartId> importAndMeshAssembly(
        const std::string& filepath,
        const std::map<int, std::pair<int, int>>& solidToMatSection,
        const mesh::MeshParameters& meshParams
    );

    /**
     * @brief Batch import all STEP/IGES files from a directory
     * @param directoryPath Path to directory containing CAD files
     * @param meshParams Mesh generation parameters
     * @param materialId Material ID to assign to all parts
     * @param sectionId Section ID to assign to all parts
     * @param filePattern File extension filter (e.g., ".step", ".stp", ".iges")
     *                    Default: empty (imports all .step, .stp, .iges, .igs files)
     * @return Vector of part IDs (one per file), empty on failure
     *
     * Scans directory for CAD files matching the pattern and imports them.
     * Each file becomes a separate part with the same material/section.
     * Part titles are derived from filenames.
     *
     * Example:
     *   auto params = mesh::MeshParameters::medium(5.0);
     *   auto partIds = geomMgr.importDirectoryAndMesh("./cad_models", params, 1, 1);
     *   // Imports all .step/.stp/.iges/.igs files from ./cad_models
     *
     *   auto stepOnly = geomMgr.importDirectoryAndMesh("./parts", params, 1, 1, ".step");
     *   // Imports only .step files
     */
    std::vector<PartId> importDirectoryAndMesh(
        const std::string& directoryPath,
        const mesh::MeshParameters& meshParams,
        int materialId,
        int sectionId,
        const std::string& filePattern = ""
    );

    /**
     * @brief Batch import with per-file material/section mapping
     * @param fileToMatSection Map: filepath → (material ID, section ID)
     * @param meshParams Mesh generation parameters
     * @return Vector of part IDs (one per file), empty on failure
     *
     * Imports multiple CAD files with individual material/section assignments.
     *
     * Example:
     *   std::map<std::string, std::pair<int, int>> mapping = {
     *       {"part1.step", {1, 1}},  // Steel part
     *       {"part2.step", {2, 2}},  // Aluminum part
     *   };
     *   auto partIds = geomMgr.importBatchAndMesh(mapping, params);
     */
    std::vector<PartId> importBatchAndMesh(
        const std::map<std::string, std::pair<int, int>>& fileToMatSection,
        const mesh::MeshParameters& meshParams
    );

    // ========================================================================
    // Step-by-Step Workflow (Low-Level Control)
    // ========================================================================

    /**
     * @brief Load CAD file
     * @param filepath Path to CAD file
     * @return true if successful
     *
     * Loads CAD file using OpenCASCADE.
     * After loading, use generateMesh() to create mesh.
     */
    bool loadCAD(const std::string& filepath);

    /**
     * @brief Generate mesh from loaded geometry
     * @param params Mesh parameters
     * @return true if successful
     *
     * Requires geometry to be loaded first via loadCAD().
     */
    bool generateMesh(const mesh::MeshParameters& params);

    /**
     * @brief Create LS-DYNA part from generated mesh
     * @param materialId Material ID
     * @param sectionId Section ID
     * @param partTitle Part title (optional)
     * @return Part ID if successful, -1 on failure
     *
     * Requires mesh to be generated first via generateMesh().
     * Adds nodes, elements, and part to the model.
     */
    PartId createPartFromMesh(
        int materialId,
        int sectionId,
        const std::string& partTitle = ""
    );

    // ========================================================================
    // Geometry Queries
    // ========================================================================

    /**
     * @brief Get number of solids in loaded geometry
     * @return Solid count (0 if no geometry loaded)
     */
    int getNumSolids() const;

    /**
     * @brief Get number of faces in loaded geometry
     * @return Face count (0 if no geometry loaded)
     */
    int getNumFaces() const;

    /**
     * @brief Get bounding box of loaded geometry
     * @return Bounding box
     */
    common::BoundingBox getBoundingBox() const;

    /**
     * @brief Get total volume of all solids
     * @return Total volume
     */
    double getTotalVolume() const;

    /**
     * @brief Check if geometry is loaded
     * @return true if geometry exists
     */
    bool hasGeometry() const;

    // ========================================================================
    // Mesh Queries
    // ========================================================================

    /**
     * @brief Get number of nodes in generated mesh
     * @return Node count (0 if no mesh)
     */
    size_t getNodeCount() const;

    /**
     * @brief Get number of elements in generated mesh
     * @return Element count (0 if no mesh)
     */
    size_t getElementCount() const;

    /**
     * @brief Compute mesh quality metrics
     * @return Quality metrics
     */
    mesh::MeshQuality getMeshQuality() const;

    /**
     * @brief Check if mesh is generated
     * @return true if mesh exists
     */
    bool hasMesh() const;

    // ========================================================================
    // Error Handling
    // ========================================================================

    /**
     * @brief Get last error message
     * @return Error message from last failed operation
     */
    std::string getLastError() const;

    // ========================================================================
    // Cleanup
    // ========================================================================

    /**
     * @brief Clear all loaded geometry and mesh data
     */
    void clear();

private:
    // Implementation details hidden
    class Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace koo::dyna::managers
