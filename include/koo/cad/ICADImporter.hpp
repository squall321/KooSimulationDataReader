#pragma once

#include <koo/Export.hpp>
#include <koo/cad/Geometry.hpp>
#include <koo/cad/CADTypes.hpp>
#include <koo/common/Vec3.hpp>
#include <string>
#include <memory>
#include <vector>

namespace koo::cad {

/**
 * @brief Abstract interface for CAD file import
 *
 * This interface abstracts CAD kernel operations, allowing different
 * backends (OpenCASCADE, ACIS, Parasolid, etc.) to be used interchangeably.
 *
 * Implementations are responsible for:
 * - Loading CAD files (STEP, IGES, etc.)
 * - Extracting topology (solids, faces, edges, vertices)
 * - Providing geometry queries (bounding box, volume, etc.)
 * - Converting native CAD data to library-neutral Geometry representation
 *
 * Usage:
 *   std::unique_ptr<ICADImporter> importer = std::make_unique<OccadImporter>();
 *   if (importer->importFile("model.step")) {
 *       auto geometry = importer->getGeometry();
 *       std::cout << "Solids: " << geometry->getNumSolids() << "\n";
 *   }
 */
class KOO_API ICADImporter {
public:
    /**
     * @brief Virtual destructor
     */
    virtual ~ICADImporter() = default;

    // ========================================================================
    // File Import
    // ========================================================================

    /**
     * @brief Import CAD file
     * @param filepath Path to CAD file (STEP, IGES, etc.)
     * @return true if import successful, false otherwise
     *
     * Loads the CAD file and extracts all topology entities.
     * After successful import, use getGeometry() to access the data.
     */
    virtual bool importFile(const std::string& filepath) = 0;

    /**
     * @brief Get last error message
     * @return Error message from last failed operation
     */
    virtual std::string getLastError() const = 0;

    // ========================================================================
    // Geometry Access
    // ========================================================================

    /**
     * @brief Get geometry representation
     * @return Shared pointer to Geometry object
     *
     * Returns the geometry loaded by importFile().
     * Returns nullptr if no geometry is loaded.
     */
    virtual std::shared_ptr<Geometry> getGeometry() const = 0;

    /**
     * @brief Check if geometry is loaded
     * @return true if geometry exists
     */
    virtual bool hasGeometry() const = 0;

    // ========================================================================
    // Topology Queries
    // ========================================================================

    /**
     * @brief Get all solid entities
     * @return Vector of solids
     */
    virtual std::vector<GeometryEntity> getSolids() const = 0;

    /**
     * @brief Get all face entities
     * @return Vector of faces
     */
    virtual std::vector<GeometryEntity> getFaces() const = 0;

    /**
     * @brief Get all edge entities
     * @return Vector of edges
     */
    virtual std::vector<GeometryEntity> getEdges() const = 0;

    /**
     * @brief Get all vertex entities
     * @return Vector of vertices
     */
    virtual std::vector<GeometryEntity> getVertices() const = 0;

    /**
     * @brief Get number of solids
     * @return Solid count
     */
    virtual int getNumSolids() const = 0;

    /**
     * @brief Get number of faces
     * @return Face count
     */
    virtual int getNumFaces() const = 0;

    /**
     * @brief Get number of edges
     * @return Edge count
     */
    virtual int getNumEdges() const = 0;

    /**
     * @brief Get number of vertices
     * @return Vertex count
     */
    virtual int getNumVertices() const = 0;

    // ========================================================================
    // Geometric Queries
    // ========================================================================

    /**
     * @brief Get global bounding box
     * @return Bounding box containing all geometry
     */
    virtual common::BoundingBox getBoundingBox() const = 0;

    /**
     * @brief Compute total volume
     * @return Sum of all solid volumes
     */
    virtual double getTotalVolume() const = 0;

    /**
     * @brief Compute total surface area
     * @return Sum of all face areas
     */
    virtual double getTotalSurfaceArea() const = 0;

    // ========================================================================
    // Cleanup
    // ========================================================================

    /**
     * @brief Clear all loaded geometry
     */
    virtual void clear() = 0;
};

/**
 * @brief Factory function to create CAD importer
 * @param backend Backend name ("opencascade", etc.)
 * @return Unique pointer to importer, or nullptr if backend unavailable
 *
 * Usage:
 *   auto importer = createCADImporter("opencascade");
 *   if (!importer) {
 *       // OpenCASCADE not available
 *   }
 */
KOO_API std::unique_ptr<ICADImporter> createCADImporter(const std::string& backend = "opencascade");

} // namespace koo::cad
