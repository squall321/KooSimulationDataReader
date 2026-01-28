#pragma once

#include <koo/Export.hpp>
#include <koo/dyna/Model.hpp>
#include <koo/util/Types.hpp>
#include <vector>
#include <unordered_map>
#include <string>
#include <memory>

namespace koo::dyna::managers {

/**
 * @brief Manager for part-related operations
 *
 * Provides efficient access to part data and relationships:
 * - Quick lookups of parts by ID
 * - Access to elements belonging to a part
 * - Access to nodes used by a part
 * - Part statistics and metadata
 *
 * Usage:
 *   Model model = reader.read("model.k");
 *   PartManager mgr(model);
 *   mgr.buildIndex();
 *
 *   auto elements = mgr.getElements(1);  // Get all elements in part 1
 *   auto stats = mgr.getStatistics(1);   // Get part statistics
 */
class KOO_API PartManager {
public:
    /**
     * @brief Construct a PartManager for the given model
     * @param model The model to manage (must outlive this manager)
     */
    explicit PartManager(Model& model);

    /**
     * @brief Destructor
     */
    ~PartManager() = default;

    // Prevent copying (managers reference a model)
    PartManager(const PartManager&) = delete;
    PartManager& operator=(const PartManager&) = delete;

    // Allow moving
    PartManager(PartManager&&) noexcept = default;
    PartManager& operator=(PartManager&&) noexcept = default;

    // ========================================================================
    // Index Management
    // ========================================================================

    /**
     * @brief Build internal indices for fast lookup
     *
     * This method scans all elements in the model and builds:
     * - Part ID → Element IDs mapping
     * - Part ID → Node IDs mapping (derived from elements)
     *
     * Must be called before using query methods. Call again after
     * modifying the model to rebuild indices.
     */
    void buildIndex();

    /**
     * @brief Check if indices have been built
     * @return true if buildIndex() has been called
     */
    bool isIndexBuilt() const { return indexBuilt_; }

    /**
     * @brief Clear all cached indices
     *
     * Call this before modifying the model structure, then call
     * buildIndex() again afterward.
     */
    void clearIndex();

    // ========================================================================
    // Part Queries
    // ========================================================================

    /**
     * @brief Get part data by ID
     * @param pid Part ID
     * @return Pointer to part data, or nullptr if not found
     */
    const PartData* getPart(PartId pid) const;

    /**
     * @brief Get all part IDs in the model
     * @return Vector of all part IDs
     *
     * Note: Requires buildIndex() to have been called
     */
    std::vector<PartId> getAllPartIds() const;

    /**
     * @brief Check if a part exists
     * @param pid Part ID
     * @return true if part exists
     */
    bool hasPart(PartId pid) const;

    /**
     * @brief Get the number of parts in the model
     * @return Number of parts
     */
    size_t getPartCount() const;

    // ========================================================================
    // Element Queries
    // ========================================================================

    /**
     * @brief Get all elements belonging to a part
     * @param pid Part ID
     * @return Vector of element IDs
     *
     * Returns empty vector if part doesn't exist or has no elements.
     * Requires buildIndex() to have been called.
     */
    std::vector<ElementId> getElements(PartId pid) const;

    /**
     * @brief Get the number of elements in a part
     * @param pid Part ID
     * @return Number of elements
     */
    size_t getElementCount(PartId pid) const;

    // ========================================================================
    // Node Queries
    // ========================================================================

    /**
     * @brief Get all unique nodes used by a part's elements
     * @param pid Part ID
     * @return Vector of unique node IDs (sorted)
     *
     * Returns empty vector if part doesn't exist or has no elements.
     * Requires buildIndex() to have been called.
     */
    std::vector<NodeId> getNodes(PartId pid) const;

    /**
     * @brief Get the number of unique nodes in a part
     * @param pid Part ID
     * @return Number of unique nodes
     */
    size_t getNodeCount(PartId pid) const;

    // ========================================================================
    // Part Properties
    // ========================================================================

    /**
     * @brief Part statistics structure
     */
    struct Statistics {
        PartId partId = 0;
        std::string title;
        size_t nodeCount = 0;
        size_t elementCount = 0;
        MaterialId materialId = 0;
        SectionId sectionId = 0;
        BoundingBox boundingBox;
    };

    /**
     * @brief Get comprehensive statistics for a part
     * @param pid Part ID
     * @return Statistics structure
     *
     * Returns default-constructed Statistics if part doesn't exist.
     * Computing bounding box requires accessing node coordinates.
     * Requires buildIndex() to have been called.
     */
    Statistics getStatistics(PartId pid) const;

    /**
     * @brief Set the material ID for a part
     * @param pid Part ID
     * @param mid Material ID
     * @return true if successful, false if part not found
     *
     * Note: Modifies the underlying Part keyword in the model
     */
    bool setMaterial(PartId pid, MaterialId mid);

    /**
     * @brief Set the section ID for a part
     * @param pid Part ID
     * @param sid Section ID
     * @return true if successful, false if part not found
     *
     * Note: Modifies the underlying Part keyword in the model
     */
    bool setSection(PartId pid, SectionId sid);

    /**
     * @brief Get the title of a part
     * @param pid Part ID
     * @return Part title, or empty string if not found
     */
    std::string getTitle(PartId pid) const;

    // ========================================================================
    // Advanced Queries
    // ========================================================================

    /**
     * @brief Get bounding box for a part
     * @param pid Part ID
     * @return Bounding box encompassing all nodes in the part
     *
     * Returns invalid bounding box if part doesn't exist or has no nodes.
     */
    BoundingBox getBoundingBox(PartId pid) const;

private:
    // Reference to the model we're managing
    Model& model_;

    // Index: PartId → vector of ElementIds
    mutable std::unordered_map<PartId, std::vector<ElementId>> partToElements_;

    // Index: PartId → vector of unique NodeIds (sorted)
    mutable std::unordered_map<PartId, std::vector<NodeId>> partToNodes_;

    // Flag indicating if indices have been built
    mutable bool indexBuilt_ = false;

    // Helper: Get element type and node IDs from any element
    struct ElementInfo {
        ElementId id;
        PartId pid;
        std::vector<NodeId> nodeIds;
        ElementType type;
    };

    std::vector<ElementInfo> getAllElementInfo() const;
};

} // namespace koo::dyna::managers
