#pragma once

#include <koo/Export.hpp>
#include <koo/dyna/Model.hpp>
#include <koo/dyna/Set.hpp>
#include <koo/dyna/managers/ElementManager.hpp>
#include <koo/util/Types.hpp>
#include <vector>
#include <unordered_map>
#include <string>
#include <memory>

namespace koo::dyna::managers {

// Forward declarations
class PartManager;

/**
 * @brief Manager for set-related operations
 *
 * Provides creation and management of LS-DYNA sets:
 * - Node sets
 * - Segment sets (for contact and boundary conditions)
 * - Part sets
 * - Element sets (shell, solid)
 *
 * Key features:
 * - Automatic set ID allocation
 * - External surface extraction (for contact)
 * - Part-based set creation
 * - Set querying and modification
 *
 * Usage:
 *   Model model = reader.read("model.k");
 *   SetManager setMgr(model, partMgr, elemMgr);
 *
 *   // Create segment set from part's external surface
 *   int setId = setMgr.createSegmentSetFromPartSurface(1, "Part1_Surface");
 *
 *   // Create node set from part
 *   int nodeSetId = setMgr.createNodeSetFromPart(1, "Part1_Nodes");
 */
class KOO_API SetManager {
public:
    /**
     * @brief Construct a SetManager
     * @param model The model to manage (must outlive this manager)
     * @param partMgr Reference to PartManager (for part-based operations)
     * @param elemMgr Reference to ElementManager (for segment extraction)
     *
     * Note: PartManager and ElementManager must have their indices built
     * before using SetManager operations that depend on them.
     */
    SetManager(Model& model, PartManager& partMgr, ElementManager& elemMgr);

    /**
     * @brief Destructor
     */
    ~SetManager() = default;

    // Prevent copying
    SetManager(const SetManager&) = delete;
    SetManager& operator=(const SetManager&) = delete;

    // Allow moving
    SetManager(SetManager&&) noexcept = default;
    SetManager& operator=(SetManager&&) noexcept = default;

    // ========================================================================
    // Node Set Operations
    // ========================================================================

    /**
     * @brief Create a node set from a list of node IDs
     * @param nodes Vector of node IDs
     * @param title Optional title for the set
     * @return Created set ID
     *
     * Creates a *SET_NODE_LIST_TITLE keyword with the given nodes.
     * Automatically assigns next available set ID.
     */
    int createNodeSet(const std::vector<NodeId>& nodes,
                      const std::string& title = "");

    /**
     * @brief Create a node set from all nodes in a part
     * @param pid Part ID
     * @param title Optional title (defaults to "Part{pid}_Nodes")
     * @return Created set ID
     *
     * Uses PartManager to get all nodes in the part.
     */
    int createNodeSetFromPart(PartId pid, const std::string& title = "");

    /**
     * @brief Get all node IDs in a node set
     * @param setId Set ID
     * @return Vector of node IDs, or empty if set not found
     */
    std::vector<NodeId> getNodeSet(int setId) const;

    /**
     * @brief Get all node set IDs in the model
     * @return Vector of set IDs
     */
    std::vector<int> getAllNodeSetIds() const;

    // ========================================================================
    // Segment Set Operations
    // ========================================================================

    /**
     * @brief Segment data structure (matches LS-DYNA format)
     */
    struct SegmentData {
        NodeId n1 = 0;
        NodeId n2 = 0;
        NodeId n3 = 0;
        NodeId n4 = 0;  ///< 0 for triangular segments

        SegmentData() = default;
        SegmentData(NodeId n1_, NodeId n2_, NodeId n3_, NodeId n4_ = 0)
            : n1(n1_), n2(n2_), n3(n3_), n4(n4_) {}

        bool operator==(const SegmentData& other) const {
            return n1 == other.n1 && n2 == other.n2 &&
                   n3 == other.n3 && n4 == other.n4;
        }
    };

    /**
     * @brief Create a segment set from a list of segments
     * @param segments Vector of segment data
     * @param title Optional title for the set
     * @return Created set ID
     *
     * Creates a *SET_SEGMENT_TITLE keyword.
     */
    int createSegmentSet(const std::vector<SegmentData>& segments,
                         const std::string& title = "");

    /**
     * @brief Create a segment set from part's external surface
     * @param pid Part ID
     * @param title Optional title (defaults to "Part{pid}_Surface")
     * @return Created set ID
     *
     * This is the CRITICAL function for contact/BC automation.
     *
     * Algorithm:
     * 1. Get all elements in the part
     * 2. Extract all segments from these elements
     * 3. Filter for external segments (appear in only 1 element)
     * 4. Create set from external segments
     *
     * External surface = segments shared by 0 elements outside the part
     */
    int createSegmentSetFromPartSurface(PartId pid, const std::string& title = "");

    /**
     * @brief Create a segment set from all elements in a part
     * @param pid Part ID
     * @param title Optional title
     * @return Created set ID
     *
     * Unlike createSegmentSetFromPartSurface, this includes ALL
     * segments (internal + external). Useful for debugging.
     */
    int createSegmentSetFromPartElements(PartId pid, const std::string& title = "");

    /**
     * @brief Get all segments in a segment set
     * @param setId Set ID
     * @return Vector of segments, or empty if set not found
     */
    std::vector<SegmentData> getSegmentSet(int setId) const;

    /**
     * @brief Get all segment set IDs in the model
     * @return Vector of set IDs
     */
    std::vector<int> getAllSegmentSetIds() const;

    // ========================================================================
    // Part Set Operations
    // ========================================================================

    /**
     * @brief Create a part set from a list of part IDs
     * @param parts Vector of part IDs
     * @param title Optional title for the set
     * @return Created set ID
     *
     * Creates a *SET_PART_LIST_TITLE keyword.
     */
    int createPartSet(const std::vector<PartId>& parts,
                      const std::string& title = "");

    /**
     * @brief Get all part IDs in a part set
     * @param setId Set ID
     * @return Vector of part IDs, or empty if set not found
     */
    std::vector<PartId> getPartSet(int setId) const;

    /**
     * @brief Get all part set IDs in the model
     * @return Vector of set IDs
     */
    std::vector<int> getAllPartSetIds() const;

    // ========================================================================
    // Element Set Operations (Shell/Solid)
    // ========================================================================

    /**
     * @brief Create a shell element set
     * @param elements Vector of shell element IDs
     * @param title Optional title
     * @return Created set ID
     */
    int createShellSet(const std::vector<ElementId>& elements,
                       const std::string& title = "");

    /**
     * @brief Create a solid element set
     * @param elements Vector of solid element IDs
     * @param title Optional title
     * @return Created set ID
     */
    int createSolidSet(const std::vector<ElementId>& elements,
                       const std::string& title = "");

    /**
     * @brief Get all shell element IDs in a shell set
     * @param setId Set ID
     * @return Vector of element IDs
     */
    std::vector<ElementId> getShellSet(int setId) const;

    /**
     * @brief Get all solid element IDs in a solid set
     * @param setId Set ID
     * @return Vector of element IDs
     */
    std::vector<ElementId> getSolidSet(int setId) const;

    // ========================================================================
    // Set ID Management
    // ========================================================================

    /**
     * @brief Get next available set ID for a given type
     * @param type Set type ("node", "segment", "part", "shell", "solid")
     * @return Next available set ID
     *
     * Scans existing sets to avoid ID conflicts.
     */
    int getNextAvailableSetId(const std::string& type);

    /**
     * @brief Scan all existing sets and update ID counters
     *
     * Call this after loading a model to ensure set IDs don't conflict.
     */
    void scanExistingSets();

private:
    // Reference to the model and other managers
    Model& model_;
    PartManager& partManager_;
    ElementManager& elementManager_;

    // Set ID counters (incremented for each new set)
    int nextNodeSetId_ = 1;
    int nextSegmentSetId_ = 1;
    int nextPartSetId_ = 1;
    int nextShellSetId_ = 1;
    int nextSolidSetId_ = 1;

    // Helper: Extract external surface segments
    struct SegmentHash {
        std::size_t operator()(const SegmentData& seg) const;
    };

    /**
     * @brief Filter segments to get only external ones
     * @param allSegments All segments from all elements
     * @return Only segments that appear exactly once (external surface)
     *
     * Algorithm:
     * 1. Count occurrences of each segment (using normalized form)
     * 2. Return only segments with count == 1
     *
     * Normalized form: Sort node IDs to handle different orientations
     * Example: (1,2,3,4) and (4,3,2,1) are the same segment
     */
    std::vector<SegmentData> filterExternalSegments(
        const std::vector<SegmentData>& allSegments) const;

    /**
     * @brief Normalize segment (sort nodes for consistent comparison)
     * @param seg Input segment
     * @return Normalized segment with sorted node IDs
     */
    SegmentData normalizeSegment(const SegmentData& seg) const;

    /**
     * @brief Convert ElementManager::Segment to SegmentData
     * @param seg ElementManager segment
     * @return SetManager segment data
     */
    SegmentData convertSegment(const ElementManager::Segment& seg) const;
};

} // namespace koo::dyna::managers
