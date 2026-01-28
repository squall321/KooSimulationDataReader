#pragma once

#include <koo/Export.hpp>
#include <koo/dyna/Model.hpp>
#include <koo/dyna/managers/PartManager.hpp>
#include <koo/dyna/managers/ElementManager.hpp>
#include <koo/dyna/managers/NodeManager.hpp>
#include <koo/dyna/managers/SetManager.hpp>
#include <koo/dyna/managers/ContactManager.hpp>
#include <koo/dyna/managers/LoadManager.hpp>
#include <memory>
#include <string>

namespace koo::dyna::managers {

/**
 * @brief Unified high-level manager for LS-DYNA models
 *
 * ModelManager provides a single entry point for all manager functionality:
 * - Automatic initialization of all sub-managers
 * - Unified API for common operations
 * - Workflow automation for typical simulation scenarios
 * - Coordinated operations across multiple managers
 *
 * Key features:
 * - One-stop access to all manager functionality
 * - Automatic index building
 * - Workflow templates (crash, forming, pressure vessel, etc.)
 * - Simplified API for common tasks
 *
 * Benefits over using individual managers:
 * - No need to manually create and initialize 6 managers
 * - No need to remember initialization order
 * - Workflow methods encode best practices
 * - Cleaner, more maintainable code
 *
 * Usage:
 *   Model model = reader.read("model.k");
 *   ModelManager mgr(model);
 *
 *   // Simple access to all managers
 *   mgr.parts().getStatistics(1);
 *   mgr.contacts().createPartBasedContact(1, 2);
 *   mgr.loads().applyPressureToPart(1, 1.0);
 *
 *   // Or use workflow automation
 *   mgr.setupCrashSimulation(vehicleParts, barrierParts);
 */
class KOO_API ModelManager {
public:
    /**
     * @brief Construct a ModelManager
     * @param model The model to manage (must outlive this manager)
     * @param autoBuildIndices If true, automatically build all indices (default: true)
     *
     * Creates all sub-managers and optionally builds their indices.
     */
    explicit ModelManager(Model& model, bool autoBuildIndices = true);

    /**
     * @brief Destructor
     */
    ~ModelManager() = default;

    // Prevent copying
    ModelManager(const ModelManager&) = delete;
    ModelManager& operator=(const ModelManager&) = delete;

    // Allow moving
    ModelManager(ModelManager&&) noexcept = default;
    ModelManager& operator=(ModelManager&&) noexcept = default;

    // ========================================================================
    // Manager Access
    // ========================================================================

    /**
     * @brief Get PartManager
     * @return Reference to PartManager
     */
    PartManager& parts() { return *partManager_; }
    const PartManager& parts() const { return *partManager_; }

    /**
     * @brief Get ElementManager
     * @return Reference to ElementManager
     */
    ElementManager& elements() { return *elementManager_; }
    const ElementManager& elements() const { return *elementManager_; }

    /**
     * @brief Get NodeManager
     * @return Reference to NodeManager
     */
    NodeManager& nodes() { return *nodeManager_; }
    const NodeManager& nodes() const { return *nodeManager_; }

    /**
     * @brief Get SetManager
     * @return Reference to SetManager
     */
    SetManager& sets() { return *setManager_; }
    const SetManager& sets() const { return *setManager_; }

    /**
     * @brief Get ContactManager
     * @return Reference to ContactManager
     */
    ContactManager& contacts() { return *contactManager_; }
    const ContactManager& contacts() const { return *contactManager_; }

    /**
     * @brief Get LoadManager
     * @return Reference to LoadManager
     */
    LoadManager& loads() { return *loadManager_; }
    const LoadManager& loads() const { return *loadManager_; }

    /**
     * @brief Get underlying Model
     * @return Reference to Model
     */
    Model& model() { return model_; }
    const Model& model() const { return model_; }

    // ========================================================================
    // Index Management
    // ========================================================================

    /**
     * @brief Build all manager indices
     *
     * Builds indices for PartManager, ElementManager, and NodeManager.
     * Call this if you created ModelManager with autoBuildIndices=false.
     */
    void buildIndices();

    /**
     * @brief Rebuild all manager indices
     *
     * Clears and rebuilds all indices. Use after modifying the model.
     */
    void rebuildIndices();

    /**
     * @brief Check if indices are built
     * @return true if all indices are built
     */
    bool hasIndices() const;

    // ========================================================================
    // Workflow Automation - Crash Simulations
    // ========================================================================

    /**
     * @brief Setup crash simulation (vehicle to barrier)
     * @param vehicleParts List of vehicle part IDs
     * @param barrierParts List of barrier part IDs
     * @param enableGravity Enable gravity (default: true)
     * @param gravityMagnitude Gravity magnitude in m/s^2 (default: 9.81)
     * @return Number of contacts created
     *
     * Automated workflow:
     * 1. Create vehicle-to-barrier contact
     * 2. Create self-contact for vehicle
     * 3. Create self-contact for barrier
     * 4. Apply gravity (if enabled)
     *
     * Example:
     *   mgr.setupCrashSimulation({1, 2, 3}, {10}, true, 9.81);
     */
    int setupCrashSimulation(
        const std::vector<PartId>& vehicleParts,
        const std::vector<PartId>& barrierParts,
        bool enableGravity = true,
        double gravityMagnitude = 9.81
    );

    /**
     * @brief Setup crash simulation with single vehicle part
     * @param vehiclePid Vehicle part ID
     * @param barrierPid Barrier part ID
     * @param enableGravity Enable gravity
     * @param gravityMagnitude Gravity magnitude
     * @return Number of contacts created
     */
    int setupCrashSimulation(
        PartId vehiclePid,
        PartId barrierPid,
        bool enableGravity = true,
        double gravityMagnitude = 9.81
    );

    // ========================================================================
    // Workflow Automation - Forming Simulations
    // ========================================================================

    /**
     * @brief Setup metal forming simulation
     * @param blankPid Blank (sheet metal) part ID
     * @param diePid Die part ID
     * @param punchPid Punch part ID
     * @param blankHolderPid Blank holder part ID
     * @param blankHolderForce Blank holder force (MPa)
     * @return Number of contacts created
     *
     * Automated workflow:
     * 1. Create forming contact: blank to die
     * 2. Create forming contact: blank to punch
     * 3. Create forming contact: blank to blank holder
     * 4. Apply pressure to blank holder
     *
     * Example:
     *   mgr.setupFormingSimulation(1, 2, 3, 4, 5.0);
     */
    int setupFormingSimulation(
        PartId blankPid,
        PartId diePid,
        PartId punchPid,
        PartId blankHolderPid,
        double blankHolderForce
    );

    // ========================================================================
    // Workflow Automation - Pressure Vessel
    // ========================================================================

    /**
     * @brief Setup pressure vessel simulation
     * @param vesselPid Vessel part ID
     * @param internalPressure Internal pressure (MPa)
     * @param enableGravity Enable gravity (default: true)
     * @param gravityMagnitude Gravity magnitude (default: 9.81)
     * @param usePulseLoad Use pulse load curve (default: false)
     * @param pulseRiseTime Rise time for pulse (default: 0.5 s)
     * @param pulseHoldTime Hold time for pulse (default: 2.0 s)
     * @param pulseFallTime Fall time for pulse (default: 0.5 s)
     * @return 1 if successful
     *
     * Automated workflow:
     * 1. Apply internal pressure to vessel
     * 2. Apply gravity (if enabled)
     * 3. Optionally use pulse load curve
     *
     * Example:
     *   // Constant pressure
     *   mgr.setupPressureVessel(1, 10.0);
     *
     *   // Pulse load
     *   mgr.setupPressureVessel(1, 10.0, true, 9.81, true, 0.5, 2.0, 0.5);
     */
    int setupPressureVessel(
        PartId vesselPid,
        double internalPressure,
        bool enableGravity = true,
        double gravityMagnitude = 9.81,
        bool usePulseLoad = false,
        double pulseRiseTime = 0.5,
        double pulseHoldTime = 2.0,
        double pulseFallTime = 0.5
    );

    // ========================================================================
    // Workflow Automation - Multi-Part Contact
    // ========================================================================

    /**
     * @brief Create all pairwise contacts between parts
     * @param partIds List of part IDs
     * @param params Contact parameters (default: default parameters)
     * @return Number of contacts created
     *
     * Creates contact between every pair of parts.
     * WARNING: Creates N*(N-1)/2 contacts. Use with caution for large N.
     *
     * Example:
     *   std::vector<PartId> parts = {1, 2, 3, 4};
     *   mgr.createAllPairwiseContacts(parts);  // Creates 6 contacts
     */
    int createAllPairwiseContacts(
        const std::vector<PartId>& partIds,
        const ContactManager::ContactParameters& params = ContactManager::ContactParameters::getDefault()
    );

    /**
     * @brief Create self-contact for all parts
     * @param partIds List of part IDs
     * @param params Contact parameters (default: default parameters)
     * @return Number of contacts created
     *
     * Creates self-contact for each part individually.
     *
     * Example:
     *   mgr.createAllSelfContacts({1, 2, 3});  // 3 self-contacts
     */
    int createAllSelfContacts(
        const std::vector<PartId>& partIds,
        const ContactManager::ContactParameters& params = ContactManager::ContactParameters::getDefault()
    );

    // ========================================================================
    // Workflow Automation - Tied Connections
    // ========================================================================

    /**
     * @brief Create tied connections between part pairs
     * @param partPairs List of (slave, master) part ID pairs
     * @param params Contact parameters (default: tied parameters)
     * @return Number of tied contacts created
     *
     * Creates tied contact for each part pair.
     * Useful for spotweld representations or mesh connections.
     *
     * Example:
     *   std::vector<std::pair<PartId, PartId>> pairs = {{1, 2}, {3, 4}};
     *   mgr.createTiedConnections(pairs);
     */
    int createTiedConnections(
        const std::vector<std::pair<PartId, PartId>>& partPairs,
        const ContactManager::ContactParameters& params = ContactManager::ContactParameters::getTied()
    );

    // ========================================================================
    // High-Level Query Operations
    // ========================================================================

    /**
     * @brief Get model statistics summary
     * @return Statistics string
     *
     * Returns summary of:
     * - Total parts, elements, nodes
     * - Element type distribution
     * - Contact count
     * - Load count
     */
    std::string getModelSummary() const;

    /**
     * @brief Print model summary to console
     */
    void printModelSummary() const;

    /**
     * @brief Get all part IDs in model
     * @return Vector of part IDs
     */
    std::vector<PartId> getAllPartIds() const;

    /**
     * @brief Get total element count
     * @return Number of elements
     */
    size_t getElementCount() const;

    /**
     * @brief Get total node count
     * @return Number of nodes
     */
    size_t getNodeCount() const;

    /**
     * @brief Get total contact count
     * @return Number of contacts
     */
    size_t getContactCount() const;

    /**
     * @brief Get total load count
     * @return Number of loads
     */
    size_t getLoadCount() const;

private:
    // Reference to the model
    Model& model_;

    // Sub-managers (owned)
    std::unique_ptr<PartManager> partManager_;
    std::unique_ptr<ElementManager> elementManager_;
    std::unique_ptr<NodeManager> nodeManager_;
    std::unique_ptr<SetManager> setManager_;
    std::unique_ptr<ContactManager> contactManager_;
    std::unique_ptr<LoadManager> loadManager_;

    // Index state
    bool indicesBuilt_ = false;
};

} // namespace koo::dyna::managers
