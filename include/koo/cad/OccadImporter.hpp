#pragma once

#include <koo/Export.hpp>
#include <koo/cad/ICADImporter.hpp>
#include <koo/cad/Geometry.hpp>
#include <koo/cad/CADTypes.hpp>
#include <memory>
#include <string>

namespace koo::cad {

/**
 * @brief OpenCASCADE implementation of ICADImporter
 *
 * Provides CAD file import using OpenCASCADE Technology (OCCT):
 * - STEP file import (AP203, AP214, AP242)
 * - IGES file import
 * - Topology extraction (solids, faces, edges, vertices)
 * - Geometric queries (volume, area, bounding box)
 *
 * OpenCASCADE handles are wrapped in opaque pointers to avoid
 * exposing OpenCASCADE headers to library users.
 *
 * Usage:
 *   OccadImporter importer;
 *   if (importer.importFile("model.step")) {
 *       auto geometry = importer.getGeometry();
 *       std::cout << "Solids: " << geometry->getNumSolids() << "\n";
 *   }
 */
class KOO_API OccadImporter : public ICADImporter {
public:
    /**
     * @brief Constructor
     */
    OccadImporter();

    /**
     * @brief Destructor
     */
    ~OccadImporter() override;

    // Prevent copying (contains OpenCASCADE handles)
    OccadImporter(const OccadImporter&) = delete;
    OccadImporter& operator=(const OccadImporter&) = delete;

    // Allow moving
    OccadImporter(OccadImporter&&) noexcept;
    OccadImporter& operator=(OccadImporter&&) noexcept;

    // ========================================================================
    // ICADImporter Interface Implementation
    // ========================================================================

    bool importFile(const std::string& filepath) override;
    std::string getLastError() const override;

    std::shared_ptr<Geometry> getGeometry() const override;
    bool hasGeometry() const override;

    std::vector<GeometryEntity> getSolids() const override;
    std::vector<GeometryEntity> getFaces() const override;
    std::vector<GeometryEntity> getEdges() const override;
    std::vector<GeometryEntity> getVertices() const override;

    int getNumSolids() const override;
    int getNumFaces() const override;
    int getNumEdges() const override;
    int getNumVertices() const override;

    common::BoundingBox getBoundingBox() const override;
    double getTotalVolume() const override;
    double getTotalSurfaceArea() const override;

    void clear() override;

    // ========================================================================
    // OpenCASCADE-Specific Methods
    // ========================================================================

    /**
     * @brief Get native OpenCASCADE compound shape
     * @return Opaque pointer to TopoDS_Compound
     *
     * Advanced users can cast this to TopoDS_Compound* if they include
     * OpenCASCADE headers. Not recommended for normal use.
     */
    void* getNativeShape() const;

    /**
     * @brief Set import tolerance
     * @param tolerance Tolerance for shape healing (default: 1e-6)
     */
    void setImportTolerance(double tolerance);

    /**
     * @brief Get import tolerance
     * @return Current tolerance
     */
    double getImportTolerance() const;

    /**
     * @brief Enable/disable shape healing
     * @param enable Enable healing (default: true)
     *
     * Shape healing fixes common CAD import issues:
     * - Small edges/faces
     * - Self-intersections
     * - Invalid topology
     */
    void setShapeHealing(bool enable);

    /**
     * @brief Check if shape healing is enabled
     * @return true if enabled
     */
    bool isShapeHealingEnabled() const;

private:
    // Pimpl idiom to hide OpenCASCADE types
    class Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace koo::cad
