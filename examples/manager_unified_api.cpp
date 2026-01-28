// Example demonstrating the unified ModelManager API
// This corresponds to Phase 4: ModelManager Integration

#include <koo/dyna/KeywordFileReader.hpp>
#include <koo/dyna/managers/ModelManager.hpp>
#include <iostream>
#include <iomanip>

using namespace koo::dyna;
using namespace koo::dyna::managers;

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input.k>\n";
        return 1;
    }

    // ========================================================================
    // Load model and create unified manager
    // ========================================================================
    std::cout << "Loading model from: " << argv[1] << "\n\n";

    KeywordFileReader reader;
    Model model = reader.read(argv[1]);

    // BEFORE (Phase 1-3): Create and initialize 6 separate managers
    // PartManager partMgr(model);
    // ElementManager elemMgr(model);
    // NodeManager nodeMgr(model);
    // SetManager setMgr(model, partMgr, elemMgr);
    // ContactManager contactMgr(model, setMgr);
    // LoadManager loadMgr(model, setMgr);
    // partMgr.buildIndex();
    // elemMgr.buildIndex();
    // nodeMgr.buildIndex();

    // AFTER (Phase 4): Single unified manager
    ModelManager mgr(model);  // Automatically creates all managers and builds indices!

    // ========================================================================
    // Example 1: Model Summary
    // ========================================================================
    std::cout << "========================================\n";
    std::cout << "Example 1: Model Summary\n";
    std::cout << "========================================\n\n";

    mgr.printModelSummary();
    std::cout << "\n";

    // ========================================================================
    // Example 2: Simple Contact Creation
    // ========================================================================
    std::cout << "========================================\n";
    std::cout << "Example 2: Simple Contact Creation\n";
    std::cout << "========================================\n\n";

    auto allPartIds = mgr.getAllPartIds();
    if (allPartIds.size() >= 2) {
        PartId part1 = allPartIds[0];
        PartId part2 = allPartIds[1];

        std::cout << "Creating contact between Part " << part1
                  << " and Part " << part2 << "...\n";

        // Simple API: just access contacts() and create
        mgr.contacts().createPartBasedContact(part1, part2);

        std::cout << "Contact created successfully!\n\n";
    }

    // ========================================================================
    // Example 3: Simple Load Application
    // ========================================================================
    std::cout << "========================================\n";
    std::cout << "Example 3: Load Application\n";
    std::cout << "========================================\n\n";

    if (!allPartIds.empty()) {
        PartId targetPart = allPartIds[0];

        std::cout << "Applying 1.0 MPa pressure to Part " << targetPart << "...\n";

        // Simple API: just access loads() and apply
        mgr.loads().applyPressureToPart(targetPart, 1.0);

        std::cout << "Pressure load applied!\n\n";
    }

    // ========================================================================
    // Example 4: Gravity Application
    // ========================================================================
    std::cout << "========================================\n";
    std::cout << "Example 4: Gravity\n";
    std::cout << "========================================\n\n";

    std::cout << "Applying gravity (9.81 m/s^2)...\n";
    mgr.loads().applyGravity(9.81);
    std::cout << "Gravity applied!\n\n";

    // ========================================================================
    // Example 5: Access Individual Managers
    // ========================================================================
    std::cout << "========================================\n";
    std::cout << "Example 5: Individual Manager Access\n";
    std::cout << "========================================\n\n";

    // You can still access individual managers when needed
    std::cout << "Accessing individual managers:\n";

    // Part manager
    auto partStats = mgr.parts().getStatistics(allPartIds[0]);
    std::cout << "Part " << allPartIds[0] << " statistics:\n";
    std::cout << "  Elements: " << partStats.elementCount << "\n";
    std::cout << "  Nodes: " << partStats.nodeCount << "\n";

    // Element manager
    auto shells = mgr.elements().getShellElements();
    std::cout << "\nTotal shell elements: " << shells.size() << "\n";

    // Node manager
    std::cout << "Total nodes: " << mgr.nodes().getNodeCount() << "\n\n";

    // ========================================================================
    // Example 6: Workflow Automation - Crash Simulation
    // ========================================================================
    std::cout << "========================================\n";
    std::cout << "Example 6: Crash Simulation Workflow\n";
    std::cout << "========================================\n\n";

    if (allPartIds.size() >= 2) {
        std::vector<PartId> vehicleParts = {allPartIds[0]};
        std::vector<PartId> barrierParts = {allPartIds[1]};

        std::cout << "Setting up crash simulation...\n";
        std::cout << "  Vehicle parts: " << vehicleParts.size() << "\n";
        std::cout << "  Barrier parts: " << barrierParts.size() << "\n";

        // ONE CALL does everything:
        // - Creates vehicle-barrier contact
        // - Creates self-contact for vehicle
        // - Creates self-contact for barrier
        // - Applies gravity
        int contactCount = mgr.setupCrashSimulation(
            vehicleParts,
            barrierParts,
            true,   // Enable gravity
            9.81    // Gravity magnitude
        );

        std::cout << "Crash simulation setup complete!\n";
        std::cout << "  Contacts created: " << contactCount << "\n\n";
    }

    // ========================================================================
    // Example 7: Workflow Automation - Forming Simulation
    // ========================================================================
    std::cout << "========================================\n";
    std::cout << "Example 7: Forming Simulation Workflow\n";
    std::cout << "========================================\n\n";

    if (allPartIds.size() >= 4) {
        PartId blankPid = allPartIds[0];
        PartId diePid = allPartIds[1];
        PartId punchPid = allPartIds[2];
        PartId blankHolderPid = allPartIds[3];

        std::cout << "Setting up forming simulation...\n";
        std::cout << "  Blank: Part " << blankPid << "\n";
        std::cout << "  Die: Part " << diePid << "\n";
        std::cout << "  Punch: Part " << punchPid << "\n";
        std::cout << "  Blank holder: Part " << blankHolderPid << "\n";

        // ONE CALL does everything:
        // - Creates forming contacts (blank-die, blank-punch, blank-holder)
        // - Applies blank holder pressure
        int contactCount = mgr.setupFormingSimulation(
            blankPid,
            diePid,
            punchPid,
            blankHolderPid,
            5.0  // 5 MPa blank holder force
        );

        std::cout << "Forming simulation setup complete!\n";
        std::cout << "  Contacts created: " << contactCount << "\n\n";
    }

    // ========================================================================
    // Example 8: Workflow Automation - Pressure Vessel
    // ========================================================================
    std::cout << "========================================\n";
    std::cout << "Example 8: Pressure Vessel Workflow\n";
    std::cout << "========================================\n\n";

    if (!allPartIds.empty()) {
        PartId vesselPid = allPartIds[0];

        std::cout << "Setting up pressure vessel simulation...\n";
        std::cout << "  Vessel: Part " << vesselPid << "\n";

        // ONE CALL does everything:
        // - Creates pressure load with pulse curve
        // - Applies gravity
        mgr.setupPressureVessel(
            vesselPid,
            10.0,   // 10 MPa internal pressure
            true,   // Enable gravity
            9.81,   // Gravity magnitude
            true,   // Use pulse load
            0.5,    // Rise time (s)
            2.0,    // Hold time (s)
            0.5     // Fall time (s)
        );

        std::cout << "Pressure vessel setup complete!\n";
        std::cout << "  Internal pressure: 10 MPa (pulse load)\n";
        std::cout << "  Gravity: enabled\n\n";
    }

    // ========================================================================
    // Example 9: Multi-Part Contact Creation
    // ========================================================================
    std::cout << "========================================\n";
    std::cout << "Example 9: Multi-Part Contact\n";
    std::cout << "========================================\n\n";

    if (allPartIds.size() >= 3) {
        std::vector<PartId> parts = {allPartIds[0], allPartIds[1], allPartIds[2]};

        std::cout << "Creating all pairwise contacts for " << parts.size() << " parts...\n";

        // ONE CALL creates all pairwise contacts
        int contactCount = mgr.createAllPairwiseContacts(parts);

        std::cout << "Pairwise contacts created: " << contactCount << "\n";
        std::cout << "  (Expected: " << (parts.size() * (parts.size() - 1) / 2) << ")\n\n";
    }

    // ========================================================================
    // Example 10: Self-Contact for All Parts
    // ========================================================================
    std::cout << "========================================\n";
    std::cout << "Example 10: Self-Contact for All\n";
    std::cout << "========================================\n\n";

    if (allPartIds.size() >= 2) {
        std::vector<PartId> parts = {allPartIds[0], allPartIds[1]};

        std::cout << "Creating self-contact for " << parts.size() << " parts...\n";

        // ONE CALL creates self-contact for each part
        int contactCount = mgr.createAllSelfContacts(parts);

        std::cout << "Self-contacts created: " << contactCount << "\n\n";
    }

    // ========================================================================
    // Example 11: Tied Connections
    // ========================================================================
    std::cout << "========================================\n";
    std::cout << "Example 11: Tied Connections\n";
    std::cout << "========================================\n\n";

    if (allPartIds.size() >= 4) {
        std::vector<std::pair<PartId, PartId>> pairs = {
            {allPartIds[0], allPartIds[1]},
            {allPartIds[2], allPartIds[3]}
        };

        std::cout << "Creating tied connections for " << pairs.size() << " part pairs...\n";

        // ONE CALL creates all tied contacts
        int contactCount = mgr.createTiedConnections(pairs);

        std::cout << "Tied connections created: " << contactCount << "\n\n";
    }

    // ========================================================================
    // Summary
    // ========================================================================
    std::cout << "========================================\n";
    std::cout << "Final Model State\n";
    std::cout << "========================================\n\n";

    mgr.printModelSummary();

    std::cout << "========================================\n";
    std::cout << "ModelManager demonstration complete!\n";
    std::cout << "========================================\n";

    return 0;
}
