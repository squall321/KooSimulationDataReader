// Example: Complete CAD → Mesh → LS-DYNA Workflow
// Demonstrates GeometryManager integration with ModelManager

#include <koo/dyna/Model.hpp>
#include <koo/dyna/KeywordFileWriter.hpp>
#include <koo/dyna/managers/ModelManager.hpp>
#include <koo/dyna/managers/GeometryManager.hpp>
#include <koo/mesh/MeshParameters.hpp>
#include <iostream>
#include <iomanip>

using namespace koo::dyna;
using namespace koo::dyna::managers;
using namespace koo::mesh;

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <file.step|file.iges> [output.k]\n";
        return 1;
    }

    std::string inputFile = argv[1];
    std::string outputFile = (argc >= 3) ? argv[2] : "output.k";

    std::cout << "========================================\n";
    std::cout << "Complete CAD → LS-DYNA Workflow\n";
    std::cout << "========================================\n\n";

    // ========================================================================
    // Step 1: Create Model and Managers
    // ========================================================================
    std::cout << "Step 1: Initialize\n";
    std::cout << "------------------\n";

    Model model;
    ModelManager mgr(model);
    GeometryManager geomMgr(model, mgr);

    std::cout << "✓ Model created\n";
    std::cout << "✓ ModelManager created\n";
    std::cout << "✓ GeometryManager created\n\n";

    // ========================================================================
    // Example 1: Simple Workflow (One Call)
    // ========================================================================
    std::cout << "========================================\n";
    std::cout << "Example 1: Simple Workflow (One Call)\n";
    std::cout << "========================================\n\n";

    std::cout << "Input file: " << inputFile << "\n\n";

    // ONE CALL to do everything!
    MeshParameters params = MeshParameters::medium(5.0);

    std::cout << "Importing and meshing CAD file...\n";
    std::cout << "  Element size: " << params.globalElementSize << " mm\n";
    std::cout << "  Quality threshold: " << params.minElementQuality << "\n";
    std::cout << "  Optimization passes: " << params.numOptimizationPasses << "\n\n";

    PartId partId = geomMgr.importAndMeshCAD(
        inputFile,
        params,
        1,  // Material ID
        1,  // Section ID
        "ImportedPart"
    );

    if (partId <= 0) {
        std::cerr << "ERROR: Import and mesh failed\n";
        std::cerr << "       " << geomMgr.getLastError() << "\n";
        return 1;
    }

    std::cout << "✓ CAD imported and meshed successfully!\n";
    std::cout << "  Part ID: " << partId << "\n\n";

    // ========================================================================
    // Step 2: Model Summary
    // ========================================================================
    std::cout << "Step 2: Model Summary\n";
    std::cout << "---------------------\n\n";

    mgr.printModelSummary();
    std::cout << "\n";

    // ========================================================================
    // Step 3: Apply Simulation Setup (Using ModelManager)
    // ========================================================================
    std::cout << "Step 3: Apply Simulation Setup\n";
    std::cout << "-------------------------------\n\n";

    std::cout << "Applying loads and boundary conditions...\n";

    // Apply pressure load
    mgr.loads().applyPressureToPart(partId, 10.0);  // 10 MPa
    std::cout << "✓ Applied 10 MPa pressure to part\n";

    // Apply gravity
    mgr.loads().applyGravity(9.81);
    std::cout << "✓ Applied gravity (9.81 m/s²)\n";

    // Create self-contact
    mgr.contacts().createSelfContact(partId);
    std::cout << "✓ Created self-contact\n\n";

    // ========================================================================
    // Step 4: Write LS-DYNA File
    // ========================================================================
    std::cout << "Step 4: Write LS-DYNA File\n";
    std::cout << "---------------------------\n\n";

    KeywordFileWriter writer;
    if (writer.write(model, outputFile)) {
        std::cout << "✓ LS-DYNA file written: " << outputFile << "\n\n";
    } else {
        std::cerr << "ERROR: Failed to write LS-DYNA file\n";
        return 1;
    }

    // ========================================================================
    // Example 2: Step-by-Step Workflow (More Control)
    // ========================================================================
    std::cout << "========================================\n";
    std::cout << "Example 2: Step-by-Step Workflow\n";
    std::cout << "========================================\n\n";

    // Clear previous data
    model.clear();
    geomMgr.clear();

    // Step 2a: Load CAD
    std::cout << "Step 2a: Load CAD\n";
    std::cout << "-----------------\n";

    if (!geomMgr.loadCAD(inputFile)) {
        std::cerr << "ERROR: CAD load failed\n";
        return 1;
    }

    std::cout << "✓ CAD loaded\n";
    std::cout << "  Solids: " << geomMgr.getNumSolids() << "\n";
    std::cout << "  Faces: " << geomMgr.getNumFaces() << "\n";

    auto bbox = geomMgr.getBoundingBox();
    std::cout << "  Bounding box:\n";
    std::cout << "    Min: (" << bbox.min.x << ", " << bbox.min.y << ", " << bbox.min.z << ")\n";
    std::cout << "    Max: (" << bbox.max.x << ", " << bbox.max.y << ", " << bbox.max.z << ")\n";
    std::cout << "  Volume: " << geomMgr.getTotalVolume() << " mm³\n\n";

    // Step 2b: Generate Mesh
    std::cout << "Step 2b: Generate Mesh\n";
    std::cout << "----------------------\n";

    // Try coarse mesh first
    MeshParameters coarseParams = MeshParameters::coarse(10.0);
    std::cout << "Generating coarse mesh (element size: " << coarseParams.globalElementSize << " mm)...\n";

    if (!geomMgr.generateMesh(coarseParams)) {
        std::cerr << "ERROR: Mesh generation failed\n";
        return 1;
    }

    std::cout << "✓ Mesh generated\n";
    std::cout << "  Nodes: " << geomMgr.getNodeCount() << "\n";
    std::cout << "  Elements: " << geomMgr.getElementCount() << "\n\n";

    // Step 2c: Check Quality
    std::cout << "Step 2c: Check Mesh Quality\n";
    std::cout << "----------------------------\n";

    MeshQuality quality = geomMgr.getMeshQuality();
    std::cout << "Mesh quality:\n";
    std::cout << "  Grade: " << quality.getGrade() << "\n";
    std::cout << "  Aspect ratio: " << quality.minAspectRatio << " - " << quality.maxAspectRatio << "\n";
    std::cout << "  Degenerate elements: " << quality.numDegenerateElements << "\n";
    std::cout << "  Inverted elements: " << quality.numInvertedElements << "\n\n";

    // Step 2d: Refine if Needed
    if (quality.getGrade() < 'B') {
        std::cout << "Step 2d: Refine Mesh (Quality < B)\n";
        std::cout << "-----------------------------------\n";

        MeshParameters fineParams = MeshParameters::fine(5.0);
        std::cout << "Regenerating with finer mesh...\n";

        if (geomMgr.generateMesh(fineParams)) {
            quality = geomMgr.getMeshQuality();
            std::cout << "✓ Mesh refined\n";
            std::cout << "  New grade: " << quality.getGrade() << "\n";
            std::cout << "  New elements: " << geomMgr.getElementCount() << "\n\n";
        }
    }

    // Step 2e: Create Part
    std::cout << "Step 2e: Create LS-DYNA Part\n";
    std::cout << "----------------------------\n";

    PartId partId2 = geomMgr.createPartFromMesh(1, 1, "StepByStepPart");
    if (partId2 > 0) {
        std::cout << "✓ Part created: " << partId2 << "\n\n";
    }

    // ========================================================================
    // Example 3: Assembly Import
    // ========================================================================
    std::cout << "========================================\n";
    std::cout << "Example 3: Assembly Import\n";
    std::cout << "========================================\n\n";

    // Clear for fresh start
    model.clear();
    geomMgr.clear();

    // Define material/section mapping for each solid
    std::map<int, std::pair<int, int>> solidMapping = {
        {1, {1, 1}},  // Solid 1: Material 1, Section 1
        {2, {2, 2}},  // Solid 2: Material 2, Section 2
        {3, {3, 3}}   // Solid 3: Material 3, Section 3
    };

    std::cout << "Importing assembly with " << solidMapping.size() << " solid mappings...\n\n";

    MeshParameters assemblyParams = MeshParameters::medium(5.0);
    std::vector<PartId> partIds = geomMgr.importAndMeshAssembly(
        inputFile,
        solidMapping,
        assemblyParams
    );

    if (partIds.empty()) {
        std::cout << "Note: Assembly import may fail if input has single solid\n";
        std::cout << "      (This is expected for simple parts)\n\n";
    } else {
        std::cout << "✓ Assembly imported: " << partIds.size() << " parts created\n";
        for (size_t i = 0; i < partIds.size(); ++i) {
            std::cout << "  Part " << (i+1) << ": ID = " << partIds[i] << "\n";
        }
        std::cout << "\n";

        // Create contacts between all parts
        std::cout << "Creating pairwise contacts...\n";
        int contactCount = mgr.createAllPairwiseContacts(partIds);
        std::cout << "✓ Created " << contactCount << " contacts\n\n";
    }

    // ========================================================================
    // Example 4: Crash Simulation Setup
    // ========================================================================
    std::cout << "========================================\n";
    std::cout << "Example 4: Crash Simulation Setup\n";
    std::cout << "========================================\n\n";

    // Clear for fresh start
    model.clear();
    geomMgr.clear();

    // Import vehicle part
    std::cout << "Importing vehicle geometry...\n";
    MeshParameters crashParams = MeshParameters::medium(5.0);
    PartId vehiclePid = geomMgr.importAndMeshCAD(
        inputFile,
        crashParams,
        1, 1,
        "Vehicle"
    );

    if (vehiclePid > 0) {
        std::cout << "✓ Vehicle imported: Part " << vehiclePid << "\n";

        // For demo, use same part as barrier
        PartId barrierPid = vehiclePid;

        // Setup crash simulation (one call!)
        std::cout << "\nSetting up crash simulation...\n";
        int crashContacts = mgr.setupCrashSimulation(vehiclePid, barrierPid);

        std::cout << "✓ Crash simulation configured\n";
        std::cout << "  Contacts created: " << crashContacts << "\n";
        std::cout << "  Gravity: enabled (9.81 m/s²)\n\n";
    }

    // ========================================================================
    // Example 5: Pressure Vessel Setup
    // ========================================================================
    std::cout << "========================================\n";
    std::cout << "Example 5: Pressure Vessel Setup\n";
    std::cout << "========================================\n\n";

    // Clear for fresh start
    model.clear();
    geomMgr.clear();

    // Import vessel
    std::cout << "Importing vessel geometry...\n";
    MeshParameters vesselParams = MeshParameters::fine(2.0);
    PartId vesselPid = geomMgr.importAndMeshCAD(
        inputFile,
        vesselParams,
        1, 1,
        "PressureVessel"
    );

    if (vesselPid > 0) {
        std::cout << "✓ Vessel imported: Part " << vesselPid << "\n";

        // Setup pressure vessel (one call!)
        std::cout << "\nSetting up pressure vessel...\n";
        mgr.setupPressureVessel(
            vesselPid,
            10.0,   // 10 MPa internal pressure
            true,   // Enable gravity
            9.81,   // Gravity magnitude
            true,   // Use pulse load
            0.5,    // Rise time
            2.0,    // Hold time
            0.5     // Fall time
        );

        std::cout << "✓ Pressure vessel configured\n";
        std::cout << "  Internal pressure: 10 MPa (pulse)\n";
        std::cout << "  Gravity: enabled\n\n";
    }

    // ========================================================================
    // Final Summary
    // ========================================================================
    std::cout << "========================================\n";
    std::cout << "Final Model Summary\n";
    std::cout << "========================================\n\n";

    mgr.printModelSummary();

    std::cout << "\n========================================\n";
    std::cout << "Workflow Complete!\n";
    std::cout << "========================================\n\n";

    std::cout << "Summary:\n";
    std::cout << "  Input:  " << inputFile << "\n";
    std::cout << "  Output: " << outputFile << "\n";
    std::cout << "  Parts:  " << mgr.getAllPartIds().size() << "\n";
    std::cout << "  Nodes:  " << mgr.getNodeCount() << "\n";
    std::cout << "  Elems:  " << mgr.getElementCount() << "\n";
    std::cout << "  Loads:  " << mgr.getLoadCount() << "\n";
    std::cout << "  Contacts: " << mgr.getContactCount() << "\n";

    return 0;
}
