#pragma once

#include <koo/Export.hpp>
#include <koo/dyna/Model.hpp>
#include <koo/util/Types.hpp>
#include <vector>
#include <unordered_map>
#include <array>
#include <memory>

namespace koo::dyna::managers {

/**
 * @brief Manager for node-related operations
 *
 * Provides efficient access to node data and relationships:
 * - Quick lookups of nodes by ID
 * - Access to node coordinates
 * - Finding elements connected to a node
 * - Spatial queries
 *
 * Usage:
 *   Model model = reader.read("model.k");
 *   NodeManager mgr(model);
 *   mgr.buildIndex();
 *
 *   auto coords = mgr.getCoordinates(100);
 *   auto connectedElems = mgr.getConnectedElements(100);
 */
class KOO_API NodeManager {
public:
    /**
     * @brief Construct a NodeManager for the given model
     * @param model The model to manage (must outlive this manager)
     */
    explicit NodeManager(Model& model);

    /**
     * @brief Destructor
     */
    ~NodeManager() = default;

    // Prevent copying
    NodeManager(const NodeManager&) = delete;
    NodeManager& operator=(const NodeManager&) = delete;

    // Allow moving
    NodeManager(NodeManager&&) noexcept = default;
    NodeManager& operator=(NodeManager&&) noexcept = default;

    // ========================================================================
    // Index Management
    // ========================================================================

    /**
     * @brief Build internal indices for fast lookup
     *
     * This method scans all elements in the model and builds:
     * - Node ID → connected Element IDs mapping
     *
     * Must be called before using connectivity queries.
     * Call again after modifying the model.
     */
    void buildIndex();

    /**
     * @brief Check if indices have been built
     */
    bool isIndexBuilt() const { return indexBuilt_; }

    /**
     * @brief Clear all cached indices
     */
    void clearIndex();

    // ========================================================================
    // Node Queries
    // ========================================================================

    /**
     * @brief Get node data by ID
     * @param nid Node ID
     * @return Pointer to node data, or nullptr if not found
     */
    const NodeData* getNode(NodeId nid) const;

    /**
     * @brief Get all node IDs in the model
     * @return Vector of all node IDs
     */
    std::vector<NodeId> getAllNodeIds() const;

    /**
     * @brief Check if a node exists
     * @param nid Node ID
     */
    bool hasNode(NodeId nid) const;

    /**
     * @brief Get the number of nodes in the model
     */
    size_t getNodeCount() const;

    // ========================================================================
    // Coordinate Queries
    // ========================================================================

    /**
     * @brief Get coordinates of a node
     * @param nid Node ID
     * @return Array [x, y, z], or [0, 0, 0] if node not found
     */
    std::array<double, 3> getCoordinates(NodeId nid) const;

    /**
     * @brief Get position as Vec3
     * @param nid Node ID
     * @return Position vector, or Vec3{0,0,0} if not found
     */
    Vec3 getPosition(NodeId nid) const;

    /**
     * @brief Set coordinates of a node
     * @param nid Node ID
     * @param x X coordinate
     * @param y Y coordinate
     * @param z Z coordinate
     * @return true if successful, false if node not found
     */
    bool setCoordinates(NodeId nid, double x, double y, double z);

    /**
     * @brief Set position of a node
     * @param nid Node ID
     * @param pos Position vector
     * @return true if successful, false if node not found
     */
    bool setPosition(NodeId nid, const Vec3& pos);

    // ========================================================================
    // Connectivity Queries
    // ========================================================================

    /**
     * @brief Get all elements connected to a node
     * @param nid Node ID
     * @return Vector of element IDs
     *
     * Returns empty vector if node doesn't exist or has no connected elements.
     * Requires buildIndex() to have been called.
     */
    std::vector<ElementId> getConnectedElements(NodeId nid) const;

    /**
     * @brief Get the number of elements connected to a node
     * @param nid Node ID
     * @return Number of connected elements
     */
    size_t getConnectedElementCount(NodeId nid) const;

    /**
     * @brief Check if a node is on the boundary (has free faces)
     * @param nid Node ID
     * @return true if node is on the model boundary
     *
     * A node is considered on the boundary if it belongs to any
     * external surface segment. This is a simplified check.
     */
    bool isBoundaryNode(NodeId nid) const;

    // ========================================================================
    // Spatial Queries
    // ========================================================================

    /**
     * @brief Find nodes within a distance of a point
     * @param point Query point
     * @param radius Search radius
     * @return Vector of node IDs within the radius
     *
     * Simple brute-force search. For large models, consider
     * using a spatial data structure (not yet implemented).
     */
    std::vector<NodeId> findNodesNear(const Vec3& point, double radius) const;

    /**
     * @brief Find the closest node to a point
     * @param point Query point
     * @return Node ID of closest node, or 0 if no nodes exist
     */
    NodeId findClosestNode(const Vec3& point) const;

    /**
     * @brief Compute distance between two nodes
     * @param nid1 First node ID
     * @param nid2 Second node ID
     * @return Distance, or -1.0 if either node not found
     */
    double computeDistance(NodeId nid1, NodeId nid2) const;

    // ========================================================================
    // Batch Operations
    // ========================================================================

    /**
     * @brief Get coordinates for multiple nodes
     * @param nodeIds Vector of node IDs
     * @return Vector of Vec3 positions (same order as input)
     *
     * Returns Vec3{0,0,0} for nodes that don't exist.
     */
    std::vector<Vec3> getCoordinates(const std::vector<NodeId>& nodeIds) const;

    /**
     * @brief Transform nodes by a matrix
     * @param nodeIds Vector of node IDs to transform
     * @param matrix 4x4 transformation matrix
     *
     * Modifies the underlying node coordinates in the model.
     */
    void transformNodes(const std::vector<NodeId>& nodeIds, const Matrix4x4& matrix);

private:
    // Reference to the model we're managing
    Model& model_;

    // Index: NodeId → vector of connected ElementIds
    mutable std::unordered_map<NodeId, std::vector<ElementId>> nodeToElements_;

    // Flag indicating if indices have been built
    mutable bool indexBuilt_ = false;

    // Helper: Get all elements from the model
    struct ElementInfo {
        ElementId id;
        std::vector<NodeId> nodeIds;
    };
    std::vector<ElementInfo> getAllElementInfo() const;
};

} // namespace koo::dyna::managers
