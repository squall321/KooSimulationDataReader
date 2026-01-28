#pragma once

#include <koo/Export.hpp>
#include <koo/dyna/Model.hpp>
#include <koo/dyna/Contact.hpp>
#include <koo/dyna/managers/SetManager.hpp>
#include <koo/util/Types.hpp>
#include <vector>
#include <string>
#include <memory>

namespace koo::dyna::managers {

// Forward declarations
class PartManager;
class ElementManager;

/**
 * @brief Manager for contact-related operations
 *
 * Provides automated contact generation for LS-DYNA simulations:
 * - Part-based contact creation
 * - Automatic surface extraction
 * - Contact parameter management
 * - Common contact types (automatic, tied, eroding)
 *
 * Key features:
 * - High-level workflows for common contact scenarios
 * - Automatic segment set creation using SetManager
 * - Contact parameter presets (default, aggressive, soft, tied)
 * - Self-contact and multi-part contact
 *
 * Usage:
 *   Model model = reader.read("model.k");
 *   PartManager partMgr(model);
 *   ElementManager elemMgr(model);
 *   NodeManager nodeMgr(model);
 *   SetManager setMgr(model, partMgr, elemMgr);
 *   ContactManager contactMgr(model, setMgr);
 *
 *   // Create automatic surface-to-surface contact between two parts
 *   contactMgr.createPartBasedContact(1, 2);
 *
 *   // Create self-contact for a single part
 *   contactMgr.createSelfContact(1);
 *
 *   // Create tied contact (for mesh connections)
 *   contactMgr.createTiedContact(1, 2);
 */
class KOO_API ContactManager {
public:
    /**
     * @brief Contact type enumeration
     */
    enum class ContactType {
        AutomaticSingleSurface,      ///< Single surface contact (self-contact)
        AutomaticSurfaceToSurface,   ///< Surface-to-surface contact
        AutomaticNodesToSurface,     ///< Nodes to surface contact
        TiedSurfaceToSurface,        ///< Tied contact (no sliding)
        TiedNodesToSurface,          ///< Tied nodes to surface
        ErodingSingleSurface,        ///< Eroding single surface (for element deletion)
        ErodingSurfaceToSurface,     ///< Eroding surface-to-surface
        FormingOneWay                ///< Forming one-way (for stamping)
    };

    /**
     * @brief Contact parameters
     */
    struct ContactParameters {
        double fs = 0.0;            ///< Static friction coefficient
        double fd = 0.0;            ///< Dynamic friction coefficient
        double dc = 0.0;            ///< Exponential decay coefficient
        double vc = 0.0;            ///< Viscous friction coefficient
        double vdc = 0.0;           ///< Viscous damping coefficient
        double sfs = 1.0;           ///< Scale factor for slave penalty
        double sfm = 1.0;           ///< Scale factor for master penalty
        double sst = 0.0;           ///< Slave shell thickness for contact
        double mst = 0.0;           ///< Master shell thickness for contact
        double sfst = 1.0;          ///< Scale factor for slave thickness
        double sfmt = 1.0;          ///< Scale factor for master thickness
        double bt = 0.0;            ///< Birth time
        double dt = 1e20;           ///< Death time
        int sstyp = 0;              ///< Slave surface type (0=segments, 2=part sets, 3=part IDs)
        int mstyp = 0;              ///< Master surface type

        /**
         * @brief Default contact parameters (moderate)
         */
        static ContactParameters getDefault();

        /**
         * @brief Aggressive contact parameters (tight contact)
         */
        static ContactParameters getAggressive();

        /**
         * @brief Soft contact parameters (soft materials)
         */
        static ContactParameters getSoft();

        /**
         * @brief Tied contact parameters (bonded surfaces)
         */
        static ContactParameters getTied();

        /**
         * @brief Forming contact parameters (stamping simulations)
         */
        static ContactParameters getForming();
    };

    /**
     * @brief Construct a ContactManager
     * @param model The model to manage (must outlive this manager)
     * @param setMgr Reference to SetManager (for segment set creation)
     *
     * Note: SetManager must be initialized before using ContactManager.
     */
    ContactManager(Model& model, SetManager& setMgr);

    /**
     * @brief Destructor
     */
    ~ContactManager() = default;

    // Prevent copying
    ContactManager(const ContactManager&) = delete;
    ContactManager& operator=(const ContactManager&) = delete;

    // Allow moving
    ContactManager(ContactManager&&) noexcept = default;
    ContactManager& operator=(ContactManager&&) noexcept = default;

    // ========================================================================
    // High-Level Contact Creation
    // ========================================================================

    /**
     * @brief Create automatic surface-to-surface contact between two parts
     * @param slavePid Slave part ID
     * @param masterPid Master part ID
     * @param params Contact parameters (optional, uses defaults if not specified)
     * @param title Contact title (optional, auto-generated if empty)
     * @return Contact ID (for tracking/modification)
     *
     * This is the MOST COMMON contact creation workflow.
     *
     * Algorithm:
     * 1. Extract external surface from slave part (using SetManager)
     * 2. Extract external surface from master part
     * 3. Create *CONTACT_AUTOMATIC_SURFACE_TO_SURFACE keyword
     * 4. Set friction and penalty parameters
     *
     * Example:
     *   // Create contact between body (part 1) and barrier (part 2)
     *   contactMgr.createPartBasedContact(1, 2);
     */
    int createPartBasedContact(
        PartId slavePid,
        PartId masterPid,
        const ContactParameters& params = ContactParameters::getDefault(),
        const std::string& title = ""
    );

    /**
     * @brief Create self-contact for a single part
     * @param pid Part ID
     * @param params Contact parameters (optional)
     * @param title Contact title (optional)
     * @return Contact ID
     *
     * Creates *CONTACT_AUTOMATIC_SINGLE_SURFACE.
     * Useful for parts that may fold or contact themselves (e.g., airbags, cloth).
     */
    int createSelfContact(
        PartId pid,
        const ContactParameters& params = ContactParameters::getDefault(),
        const std::string& title = ""
    );

    /**
     * @brief Create tied contact between two parts (bonded, no sliding)
     * @param slavePid Slave part ID
     * @param masterPid Master part ID
     * @param params Contact parameters (optional, uses tied defaults)
     * @param title Contact title (optional)
     * @return Contact ID
     *
     * Creates *CONTACT_TIED_SURFACE_TO_SURFACE.
     * Used for connecting meshes (e.g., spotweld representations, mesh transitions).
     */
    int createTiedContact(
        PartId slavePid,
        PartId masterPid,
        const ContactParameters& params = ContactParameters::getTied(),
        const std::string& title = ""
    );

    /**
     * @brief Create eroding contact for parts with element deletion
     * @param slavePid Slave part ID
     * @param masterPid Master part ID
     * @param params Contact parameters (optional)
     * @param title Contact title (optional)
     * @return Contact ID
     *
     * Creates *CONTACT_ERODING_SURFACE_TO_SURFACE.
     * Used when elements are deleted (e.g., tearing, failure, penetration).
     */
    int createErodingContact(
        PartId slavePid,
        PartId masterPid,
        const ContactParameters& params = ContactParameters::getDefault(),
        const std::string& title = ""
    );

    /**
     * @brief Create forming contact (one-way, for stamping)
     * @param slavePid Slave part ID (sheet metal)
     * @param masterPid Master part ID (tool/die)
     * @param params Contact parameters (optional, uses forming defaults)
     * @param title Contact title (optional)
     * @return Contact ID
     *
     * Creates *CONTACT_FORMING_ONE_WAY_SURFACE_TO_SURFACE.
     * Optimized for metal forming simulations.
     */
    int createFormingContact(
        PartId slavePid,
        PartId masterPid,
        const ContactParameters& params = ContactParameters::getForming(),
        const std::string& title = ""
    );

    /**
     * @brief Create nodes-to-surface contact
     * @param slaveNodeSetId Slave node set ID
     * @param masterSegmentSetId Master segment set ID
     * @param params Contact parameters (optional)
     * @param title Contact title (optional)
     * @return Contact ID
     *
     * Creates *CONTACT_AUTOMATIC_NODES_TO_SURFACE.
     * Used when you want only slave nodes to be checked (more efficient).
     */
    int createNodesToSurfaceContact(
        int slaveNodeSetId,
        int masterSegmentSetId,
        const ContactParameters& params = ContactParameters::getDefault(),
        const std::string& title = ""
    );

    /**
     * @brief Create contact between segment sets (low-level)
     * @param slaveSetId Slave segment set ID
     * @param masterSetId Master segment set ID
     * @param type Contact type
     * @param params Contact parameters
     * @param title Contact title (optional)
     * @return Contact ID
     *
     * Low-level method for creating contact with existing segment sets.
     * Use part-based methods for most cases.
     */
    int createContact(
        int slaveSetId,
        int masterSetId,
        ContactType type,
        const ContactParameters& params,
        const std::string& title = ""
    );

    // ========================================================================
    // Contact Query and Modification
    // ========================================================================

    /**
     * @brief Get all contact keywords in the model
     * @return Vector of contact keyword pointers
     */
    std::vector<const ContactKeyword*> getAllContacts() const;

    /**
     * @brief Count total contacts in model
     * @return Number of contact keywords
     */
    size_t getContactCount() const;

    /**
     * @brief Get contact by ID (for ID-based contacts)
     * @param contactId Contact ID
     * @return Contact keyword pointer, or nullptr if not found
     */
    const ContactKeyword* getContact(int contactId) const;

    /**
     * @brief Remove a contact keyword from the model
     * @param contactId Contact ID
     * @return true if removed, false if not found
     */
    bool removeContact(int contactId);

    // ========================================================================
    // Batch Operations
    // ========================================================================

    /**
     * @brief Create self-contact for all parts in the model
     * @param params Contact parameters
     * @return Number of contacts created
     *
     * Useful for general simulations where all parts might interact.
     */
    int createAllSelfContacts(const ContactParameters& params = ContactParameters::getDefault());

    /**
     * @brief Create contact between all pairs of parts
     * @param params Contact parameters
     * @return Number of contacts created
     *
     * WARNING: Creates N*(N-1)/2 contacts for N parts. Use with caution.
     * Better to use selective contact creation for large models.
     */
    int createAllToAllContacts(const ContactParameters& params = ContactParameters::getDefault());

    /**
     * @brief Create contact between a list of parts
     * @param partIds List of part IDs
     * @param params Contact parameters
     * @return Number of contacts created
     *
     * Creates pairwise contacts between all parts in the list.
     */
    int createMultiPartContact(
        const std::vector<PartId>& partIds,
        const ContactParameters& params = ContactParameters::getDefault()
    );

private:
    // Reference to the model and set manager
    Model& model_;
    SetManager& setManager_;

    // Contact ID counter (for ID-based contacts)
    int nextContactId_ = 1;

    /**
     * @brief Get next available contact ID
     * @return Next contact ID
     */
    int getNextContactId();

    /**
     * @brief Apply parameters to contact Card1
     * @param card Card1 reference
     * @param slaveSetId Slave set ID
     * @param masterSetId Master set ID
     * @param params Contact parameters
     */
    void applyCard1Parameters(
        ContactAutomaticSingleSurface::Card1& card,
        int slaveSetId,
        int masterSetId,
        const ContactParameters& params
    );

    /**
     * @brief Apply parameters to contact Card2
     * @param card Card2 reference
     * @param params Contact parameters
     */
    void applyCard2Parameters(
        ContactAutomaticSingleSurface::Card2& card,
        const ContactParameters& params
    );

    /**
     * @brief Apply parameters to contact Card3
     * @param card Card3 reference
     * @param params Contact parameters
     */
    void applyCard3Parameters(
        ContactAutomaticSingleSurface::Card3& card,
        const ContactParameters& params
    );
};

} // namespace koo::dyna::managers
