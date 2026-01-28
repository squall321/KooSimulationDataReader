#pragma once

#include <koo/Export.hpp>
#include <koo/cad/CADTypes.hpp>
#include <koo/common/Vec3.hpp>
#include <vector>
#include <memory>

namespace koo::cad {

/**
 * @brief Geometry representation containing all topological entities
 *
 * Represents a complete CAD model with hierarchical topology:
 * - Solids: 3D bodies
 * - Faces: 2D surfaces bounding solids
 * - Edges: 1D curves bounding faces
 * - Vertices: 0D points bounding edges
 *
 * This is a library-neutral representation that wraps native CAD kernel data.
 */
class KOO_API Geometry {
public:
    /**
     * @brief Default constructor
     */
    Geometry();

    /**
     * @brief Destructor
     */
    ~Geometry();

    // Prevent copying (contains opaque pointers)
    Geometry(const Geometry&) = delete;
    Geometry& operator=(const Geometry&) = delete;

    // Allow moving
    Geometry(Geometry&&) noexcept = default;
    Geometry& operator=(Geometry&&) noexcept = default;

    // ========================================================================
    // Topology Access
    // ========================================================================

    /**
     * @brief Get all solids
     * @return Vector of solid entities
     */
    const std::vector<GeometryEntity>& getSolids() const { return solids_; }

    /**
     * @brief Get all faces
     * @return Vector of face entities
     */
    const std::vector<GeometryEntity>& getFaces() const { return faces_; }

    /**
     * @brief Get all edges
     * @return Vector of edge entities
     */
    const std::vector<GeometryEntity>& getEdges() const { return edges_; }

    /**
     * @brief Get all vertices
     * @return Vector of vertex entities
     */
    const std::vector<GeometryEntity>& getVertices() const { return vertices_; }

    /**
     * @brief Get number of solids
     */
    size_t getNumSolids() const { return solids_.size(); }

    /**
     * @brief Get number of faces
     */
    size_t getNumFaces() const { return faces_.size(); }

    /**
     * @brief Get number of edges
     */
    size_t getNumEdges() const { return edges_.size(); }

    /**
     * @brief Get number of vertices
     */
    size_t getNumVertices() const { return vertices_.size(); }

    // ========================================================================
    // Queries
    // ========================================================================

    /**
     * @brief Get global bounding box
     * @return Bounding box containing all geometry
     */
    const common::BoundingBox& getBoundingBox() const { return globalBounds_; }

    /**
     * @brief Check if geometry is empty
     * @return true if no entities exist
     */
    bool isEmpty() const {
        return solids_.empty() && faces_.empty() && edges_.empty() && vertices_.empty();
    }

    // ========================================================================
    // Internal Access (for importer implementations)
    // ========================================================================

    /**
     * @brief Add a solid entity
     */
    void addSolid(const GeometryEntity& solid) { solids_.push_back(solid); }

    /**
     * @brief Add a face entity
     */
    void addFace(const GeometryEntity& face) { faces_.push_back(face); }

    /**
     * @brief Add an edge entity
     */
    void addEdge(const GeometryEntity& edge) { edges_.push_back(edge); }

    /**
     * @brief Add a vertex entity
     */
    void addVertex(const GeometryEntity& vertex) { vertices_.push_back(vertex); }

    /**
     * @brief Set global bounding box
     */
    void setBoundingBox(const common::BoundingBox& bounds) { globalBounds_ = bounds; }

    /**
     * @brief Set native geometry handle
     * @param handle Opaque pointer to native CAD kernel data
     */
    void setNativeGeometry(void* handle) { nativeGeometry_ = handle; }

    /**
     * @brief Get native geometry handle
     * @return Opaque pointer to native CAD kernel data
     */
    void* getNativeGeometry() const { return nativeGeometry_; }

    /**
     * @brief Clear all entities
     */
    void clear();

private:
    std::vector<GeometryEntity> solids_;   ///< All solid entities
    std::vector<GeometryEntity> faces_;    ///< All face entities
    std::vector<GeometryEntity> edges_;    ///< All edge entities
    std::vector<GeometryEntity> vertices_; ///< All vertex entities

    common::BoundingBox globalBounds_;     ///< Global bounding box
    void* nativeGeometry_;                 ///< Native CAD kernel data (e.g., TopoDS_Compound*)
};

} // namespace koo::cad
