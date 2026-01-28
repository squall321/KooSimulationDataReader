#pragma once

#include <koo/Export.hpp>
#include <koo/dyna/Model.hpp>
#include <koo/dyna/Load.hpp>
#include <koo/dyna/managers/SetManager.hpp>
#include <koo/util/Types.hpp>
#include <koo/util/Math.hpp>
#include <vector>
#include <string>
#include <memory>

namespace koo::dyna::managers {

// Forward declarations
class PartManager;
class ElementManager;

/**
 * @brief Manager for load-related operations
 *
 * Provides automated load application for LS-DYNA simulations:
 * - Pressure loads on parts
 * - Concentrated forces on nodes
 * - Body forces (gravity, acceleration)
 * - Load curve management
 *
 * Key features:
 * - Part-based load application
 * - Automatic surface selection using SetManager
 * - Load curve creation and management
 * - Common load types (pressure, force, gravity, acceleration)
 *
 * Usage:
 *   Model model = reader.read("model.k");
 *   PartManager partMgr(model);
 *   ElementManager elemMgr(model);
 *   SetManager setMgr(model, partMgr, elemMgr);
 *   LoadManager loadMgr(model, setMgr);
 *
 *   // Apply pressure to part surface
 *   loadMgr.applyPressureToPart(1, 1.0);  // 1 MPa pressure on part 1
 *
 *   // Apply gravity
 *   loadMgr.applyGravity(9.81);  // 9.81 m/s^2 in -Z direction
 */
class KOO_API LoadManager {
public:
    /**
     * @brief Load type enumeration
     */
    enum class LoadType {
        Pressure,                   ///< Pressure load (normal to surface)
        Force,                      ///< Concentrated force
        Gravity,                    ///< Gravitational acceleration
        Acceleration                ///< General acceleration
    };

    /**
     * @brief Direction for body loads
     */
    enum class Direction {
        X,                          ///< X direction
        Y,                          ///< Y direction
        Z,                          ///< Z direction
        NegativeX,                  ///< -X direction
        NegativeY,                  ///< -Y direction
        NegativeZ                   ///< -Z direction (gravity)
    };

    /**
     * @brief Load parameters
     */
    struct LoadParameters {
        double magnitude = 0.0;     ///< Load magnitude
        int lcid = 0;               ///< Load curve ID (0 = constant)
        double scaleFactor = 1.0;   ///< Scale factor applied to magnitude
        double activationTime = 0.0;///< Time when load becomes active
        Vec3 direction = {0, 0, 0}; ///< Direction vector (for vector loads)

        /**
         * @brief Create constant load (no load curve)
         * @param mag Magnitude
         * @return Load parameters
         */
        static LoadParameters constant(double mag);

        /**
         * @brief Create time-varying load (uses load curve)
         * @param lcid Load curve ID
         * @param sf Scale factor
         * @return Load parameters
         */
        static LoadParameters timeVarying(int lcid, double sf = 1.0);
    };

    /**
     * @brief Construct a LoadManager
     * @param model The model to manage (must outlive this manager)
     * @param setMgr Reference to SetManager (for segment set creation)
     */
    LoadManager(Model& model, SetManager& setMgr);

    /**
     * @brief Destructor
     */
    ~LoadManager() = default;

    // Prevent copying
    LoadManager(const LoadManager&) = delete;
    LoadManager& operator=(const LoadManager&) = delete;

    // Allow moving
    LoadManager(LoadManager&&) noexcept = default;
    LoadManager& operator=(LoadManager&&) noexcept = default;

    // ========================================================================
    // High-Level Load Creation
    // ========================================================================

    /**
     * @brief Apply pressure load to part surface
     * @param pid Part ID
     * @param pressure Pressure magnitude (positive = into surface, negative = out of surface)
     * @param params Load parameters (optional, uses constant load if not specified)
     * @return Load keyword created
     *
     * This is the MOST COMMON load application workflow.
     *
     * Algorithm:
     * 1. Extract external surface from part (using SetManager)
     * 2. Create segment set from external surface
     * 3. Create *LOAD_SEGMENT_SET keyword
     * 4. Apply pressure magnitude and load curve
     *
     * Example:
     *   // Apply 1.0 MPa constant pressure to part 1
     *   loadMgr.applyPressureToPart(1, 1.0);
     *
     *   // Apply time-varying pressure using load curve 1
     *   loadMgr.applyPressureToPart(1, 1.0, LoadParameters::timeVarying(1));
     */
    int applyPressureToPart(
        PartId pid,
        double pressure,
        const LoadParameters& params = LoadParameters::constant(1.0)
    );

    /**
     * @brief Apply concentrated force to nodes
     * @param nodeIds List of node IDs
     * @param force Force vector (x, y, z components)
     * @param params Load parameters (optional)
     * @return Number of load keywords created
     *
     * Creates *LOAD_NODE_POINT for each node.
     * Force is distributed equally across all nodes.
     */
    int applyForceToNodes(
        const std::vector<NodeId>& nodeIds,
        const Vec3& force,
        const LoadParameters& params = LoadParameters::constant(1.0)
    );

    /**
     * @brief Apply concentrated force to a node set
     * @param nodeSetId Node set ID
     * @param force Force vector (x, y, z components)
     * @param params Load parameters (optional)
     * @return Load keyword created
     *
     * Creates *LOAD_NODE_SET.
     * Force is applied to all nodes in the set.
     */
    int applyForceToNodeSet(
        int nodeSetId,
        const Vec3& force,
        const LoadParameters& params = LoadParameters::constant(1.0)
    );

    /**
     * @brief Apply gravity in -Z direction
     * @param magnitude Gravitational acceleration (e.g., 9.81 m/s^2)
     * @param params Load parameters (optional)
     * @return Load keyword created
     *
     * Creates *LOAD_BODY_Z with negative magnitude.
     * Applies to all parts in the model.
     *
     * Example:
     *   loadMgr.applyGravity(9.81);  // Standard Earth gravity
     */
    int applyGravity(
        double magnitude,
        const LoadParameters& params = LoadParameters::constant(1.0)
    );

    /**
     * @brief Apply body acceleration
     * @param direction Direction enum (X, Y, Z, NegativeX, NegativeY, NegativeZ)
     * @param magnitude Acceleration magnitude
     * @param params Load parameters (optional)
     * @return Load keyword created
     *
     * Creates appropriate *LOAD_BODY_X/Y/Z keyword.
     */
    int applyAcceleration(
        Direction direction,
        double magnitude,
        const LoadParameters& params = LoadParameters::constant(1.0)
    );

    /**
     * @brief Apply pressure to segment set (low-level)
     * @param segmentSetId Segment set ID
     * @param pressure Pressure magnitude
     * @param params Load parameters
     * @return Load keyword created
     *
     * Low-level method. Use applyPressureToPart() for most cases.
     */
    int applyPressureToSegmentSet(
        int segmentSetId,
        double pressure,
        const LoadParameters& params
    );

    // ========================================================================
    // Load Curve Management
    // ========================================================================

    /**
     * @brief Create a load curve
     * @param times Time values
     * @param values Load values at each time
     * @param title Load curve title (optional)
     * @return Load curve ID
     *
     * Creates *DEFINE_CURVE keyword.
     */
    int createLoadCurve(
        const std::vector<double>& times,
        const std::vector<double>& values,
        const std::string& title = ""
    );

    /**
     * @brief Create a step load curve (0 → 1 at specified time)
     * @param stepTime Time when load reaches full magnitude
     * @param rampTime Ramp duration (0 = instant step)
     * @param title Load curve title (optional)
     * @return Load curve ID
     */
    int createStepLoadCurve(
        double stepTime,
        double rampTime = 0.0,
        const std::string& title = ""
    );

    /**
     * @brief Create a ramp load curve (linear 0 → 1)
     * @param startTime Ramp start time
     * @param endTime Ramp end time
     * @param title Load curve title (optional)
     * @return Load curve ID
     */
    int createRampLoadCurve(
        double startTime,
        double endTime,
        const std::string& title = ""
    );

    /**
     * @brief Create a pulse load curve (rise → hold → fall)
     * @param riseTime Rise duration
     * @param holdTime Hold duration
     * @param fallTime Fall duration
     * @param title Load curve title (optional)
     * @return Load curve ID
     */
    int createPulseLoadCurve(
        double riseTime,
        double holdTime,
        double fallTime,
        const std::string& title = ""
    );

    // ========================================================================
    // Load Query
    // ========================================================================

    /**
     * @brief Get all load keywords in the model
     * @return Vector of load keyword pointers
     */
    std::vector<const LoadKeyword*> getAllLoads() const;

    /**
     * @brief Count total loads in model
     * @return Number of load keywords
     */
    size_t getLoadCount() const;

    /**
     * @brief Get next available load curve ID
     * @return Next load curve ID
     */
    int getNextLoadCurveId();

private:
    // Reference to the model and set manager
    Model& model_;
    SetManager& setManager_;

    // Load curve ID counter
    int nextLoadCurveId_ = 1;

    /**
     * @brief Scan existing load curves to avoid ID conflicts
     */
    void scanExistingLoadCurves();

    /**
     * @brief Convert direction enum to axis keyword and sign
     * @param dir Direction enum
     * @param outSign Output sign (-1 or +1)
     * @return Axis character ('X', 'Y', or 'Z')
     */
    char directionToAxis(Direction dir, double& outSign) const;
};

} // namespace koo::dyna::managers
