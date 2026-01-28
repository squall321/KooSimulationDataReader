#pragma once

#include <koo/Export.hpp>
#include <koo/dyna/Model.hpp>
#include <koo/dyna/Element.hpp>
#include <koo/util/Types.hpp>
#include <vector>
#include <unordered_map>
#include <optional>
#include <memory>

namespace koo::dyna::managers {

/**
 * @brief Manager for element-related operations
 *
 * Provides efficient access to element data and operations:
 * - Quick lookups of elements by ID
 * - Filtering elements by type
 * - Access to element connectivity (nodes)
 * - Element birth/death time queries
 * - Segment extraction for contact/BC
 *
 * Usage:
 *   Model model = reader.read("model.k");
 *   ElementManager mgr(model);
 *   mgr.buildIndex();
 *
 *   auto solidElems = mgr.getSolidElements();
 *   bool alive = mgr.isAliveAt(elemId, 5.0);  // Check if alive at t=5.0
 */
class KOO_API ElementManager {
public:
    /**
     * @brief Construct an ElementManager for the given model
     * @param model The model to manage (must outlive this manager)
     */
    explicit ElementManager(Model& model);

    /**
     * @brief Destructor
     */
    ~ElementManager() = default;

    // Prevent copying
    ElementManager(const ElementManager&) = delete;
    ElementManager& operator=(const ElementManager&) = delete;

    // Allow moving
    ElementManager(ElementManager&&) noexcept = default;
    ElementManager& operator=(ElementManager&&) noexcept = default;

    // ========================================================================
    // Index Management
    // ========================================================================

    /**
     * @brief Build internal indices for fast lookup
     *
     * This method scans all elements and builds:
     * - Element ID → Element data pointer mapping
     * - Element ID → Part ID mapping
     * - Element ID → Birth/Death time mapping
     * - Type-based element lists
     *
     * Must be called before using query methods.
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
    // Element Queries
    // ========================================================================

    /**
     * @brief Get element by ID (type-erased)
     * @param eid Element ID
     * @return Pointer to ElementData, or nullptr if not found
     *
     * Note: Returns base ElementData pointer. Use getElementType()
     * to determine actual type, then cast if needed.
     */
    const ElementData* getElement(ElementId eid) const;

    /**
     * @brief Get all element IDs in the model
     * @return Vector of all element IDs
     */
    std::vector<ElementId> getAllElementIds() const;

    /**
     * @brief Check if an element exists
     * @param eid Element ID
     */
    bool hasElement(ElementId eid) const;

    /**
     * @brief Get total number of elements
     */
    size_t getElementCount() const;

    // ========================================================================
    // Type-Based Queries
    // ========================================================================

    /**
     * @brief Get the type of an element
     * @param eid Element ID
     * @return Element type, or ElementType::Unknown if not found
     */
    ElementType getElementType(ElementId eid) const;

    /**
     * @brief Get all elements of a specific type
     * @param type Element type
     * @return Vector of element IDs
     */
    std::vector<ElementId> getElementsByType(ElementType type) const;

    /**
     * @brief Get all shell element IDs
     */
    std::vector<ElementId> getShellElements() const;

    /**
     * @brief Get all solid element IDs
     */
    std::vector<ElementId> getSolidElements() const;

    /**
     * @brief Get all beam element IDs
     */
    std::vector<ElementId> getBeamElements() const;

    /**
     * @brief Get all discrete element IDs
     */
    std::vector<ElementId> getDiscreteElements() const;

    // ========================================================================
    // Part Queries
    // ========================================================================

    /**
     * @brief Get the part ID for an element
     * @param eid Element ID
     * @return Part ID, or 0 if element not found
     */
    PartId getPartId(ElementId eid) const;

    // ========================================================================
    // Connectivity Queries
    // ========================================================================

    /**
     * @brief Get node IDs for an element
     * @param eid Element ID
     * @return Vector of node IDs (empty if element not found)
     */
    std::vector<NodeId> getNodes(ElementId eid) const;

    /**
     * @brief Get number of nodes in an element
     * @param eid Element ID
     * @return Number of nodes, or 0 if element not found
     */
    size_t getNodeCount(ElementId eid) const;

    // ========================================================================
    // Segment Extraction (for Contact/BC)
    // ========================================================================

    /**
     * @brief Segment data structure
     *
     * A segment represents a face of an element:
     * - For shell: the shell face itself (3 or 4 nodes)
     * - For solid: one of 6 faces (4 nodes for hex, 3 or 4 for wedge/tet)
     */
    struct Segment {
        std::vector<NodeId> nodeIds;  ///< 3 or 4 nodes defining the segment
        ElementId sourceElement;       ///< Element this segment came from
        int faceIndex;                 ///< Face index (0 for shell, 0-5 for solid)

        Segment() : sourceElement(0), faceIndex(0) {}
        Segment(std::vector<NodeId> nodes, ElementId elem, int face)
            : nodeIds(std::move(nodes)), sourceElement(elem), faceIndex(face) {}
    };

    /**
     * @brief Get all segments (faces) for an element
     * @param eid Element ID
     * @return Vector of segments
     *
     * - Shell: returns 1 segment (the shell face)
     * - Solid (hex): returns 6 segments (all faces)
     * - Solid (wedge): returns 5 segments
     * - Solid (tet): returns 4 segments
     * - Beam/Discrete: returns empty vector
     */
    std::vector<Segment> getSegments(ElementId eid) const;

    /**
     * @brief Get all surface segments in the model
     * @return Vector of all segments from all elements
     *
     * This does NOT filter for external surfaces - it returns
     * all possible segments. Use SetManager to extract only
     * external (non-shared) segments.
     */
    std::vector<Segment> getAllSegments() const;

    // ========================================================================
    // Time-Based Queries (Birth/Death)
    // ========================================================================

    /**
     * @brief Get birth time for an element
     * @param eid Element ID
     * @return Birth time, or std::nullopt if no birth time defined
     *
     * Returns the time at which this element becomes active.
     * Elements without a defined birth time are active from t=0.
     */
    std::optional<double> getBirthTime(ElementId eid) const;

    /**
     * @brief Get death time for an element
     * @param eid Element ID
     * @return Death time, or std::nullopt if no death time defined
     *
     * Returns the time at which this element is deactivated.
     * Elements without a defined death time remain active.
     */
    std::optional<double> getDeathTime(ElementId eid) const;

    /**
     * @brief Check if element is alive at a given time
     * @param eid Element ID
     * @param time Simulation time
     * @return true if element is active at the given time
     *
     * An element is alive if:
     * - time >= birthTime (or no birth time defined)
     * - time < deathTime (or no death time defined)
     */
    bool isAliveAt(ElementId eid, double time) const;

private:
    // Reference to the model we're managing
    Model& model_;

    // Index: ElementId → ElementData pointer
    mutable std::unordered_map<ElementId, const ElementData*> elementIndex_;

    // Index: ElementId → PartId
    mutable std::unordered_map<ElementId, PartId> elementToPart_;

    // Index: ElementId → ElementType
    mutable std::unordered_map<ElementId, ElementType> elementType_;

    // Index: ElementType → vector of ElementIds
    mutable std::unordered_map<ElementType, std::vector<ElementId>> typeToElements_;

    // Time indices
    mutable std::unordered_map<ElementId, double> birthTimes_;
    mutable std::unordered_map<ElementId, double> deathTimes_;

    // Flag indicating if indices have been built
    mutable bool indexBuilt_ = false;

    // Helper methods
    void buildBirthDeathIndex();
    std::vector<Segment> extractShellSegments(const ShellElementData& elem) const;
    std::vector<Segment> extractSolidSegments(const SolidElementData& elem) const;
};

} // namespace koo::dyna::managers
