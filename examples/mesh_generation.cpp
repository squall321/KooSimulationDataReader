// Example: Generate mesh from CAD geometry using Gmsh
// Demonstrates complete CAD → Mesh workflow

#include <koo/cad/ICADImporter.hpp>
#include <koo/mesh/IMeshGenerator.hpp>
#include <koo/mesh/MeshParameters.hpp>
#include <koo/mesh/MeshQuality.hpp>
#include <iostream>
#include <iomanip>

using namespace koo::cad;
using namespace koo::mesh;

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <file.step|file.iges>\n";
        return 1;
    }

    std::string filepath = argv[1];

    std::cout << "========================================\n";
    std::cout << "Mesh Generation Example\n";
    std::cout << "========================================\n\n";

    // ========================================================================
    // Step 1: Import CAD Geometry
    // ========================================================================
    std::cout << "Step 1: Import CAD Geometry\n";
    std::cout << "----------------------------\n";

    auto importer = createCADImporter("opencascade");
    if (!importer) {
        std::cerr << "ERROR: OpenCASCADE not available\n";
        return 1;
    }

    std::cout << "Importing: " << filepath << "\n";

    if (!importer->importFile(filepath)) {
        std::cerr << "ERROR: " << importer->getLastError() << "\n";
        return 1;
    }

    std::cout << "Import successful!\n";
    std::cout << "Solids: " << importer->getNumSolids() << "\n";
    std::cout << "Faces:  " << importer->getNumFaces() << "\n\n";

    auto geometry = importer->getGeometry();

    // ========================================================================
    // Step 2: Create Mesh Generator
    // ========================================================================
    std::cout << "Step 2: Create Mesh Generator\n";
    std::cout << "------------------------------\n";

    auto mesher = createMeshGenerator("gmsh");
    if (!mesher) {
        std::cerr << "ERROR: Gmsh not available\n";
        return 1;
    }

    std::cout << "Mesh generator created (Gmsh)\n\n";

    // ========================================================================
    // Step 3: Configure Mesh Parameters
    // ========================================================================
    std::cout << "Step 3: Configure Mesh Parameters\n";
    std::cout << "----------------------------------\n";

    // Use medium preset as starting point
    MeshParameters params = MeshParameters::medium(5.0);

    std::cout << "Mesh parameters:\n";
    std::cout << "  Global element size: " << params.globalElementSize << " mm\n";
    std::cout << "  Min element size:    " << params.minElementSize << " mm\n";
    std::cout << "  Max element size:    " << params.maxElementSize << " mm\n";
    std::cout << "  Mesh type:           Volume\n";
    std::cout << "  Element order:       Linear\n";
    std::cout << "  Quality threshold:   " << params.minElementQuality << "\n";
    std::cout << "  Optimization passes: " << params.numOptimizationPasses << "\n\n";

    // ========================================================================
    // Step 4: Set Geometry
    // ========================================================================
    std::cout << "Step 4: Set Geometry\n";
    std::cout << "--------------------\n";

    if (!mesher->setGeometry(geometry)) {
        std::cerr << "ERROR: Failed to set geometry\n";
        return 1;
    }

    std::cout << "Geometry set successfully\n\n";

    // ========================================================================
    // Step 5: Generate Mesh
    // ========================================================================
    std::cout << "Step 5: Generate Mesh\n";
    std::cout << "---------------------\n";

    std::cout << "Generating mesh... (this may take a while)\n";

    if (!mesher->generateVolumeMesh(params)) {
        std::cerr << "ERROR: Mesh generation failed\n";
        std::cerr << "       " << mesher->getLastError() << "\n";
        return 1;
    }

    std::cout << "Mesh generation successful!\n\n";

    // ========================================================================
    // Step 6: Get Mesh Statistics
    // ========================================================================
    std::cout << "Step 6: Mesh Statistics\n";
    std::cout << "-----------------------\n";

    auto meshData = mesher->getMeshData();
    if (!meshData) {
        std::cerr << "ERROR: No mesh data\n";
        return 1;
    }

    std::cout << "Nodes:    " << meshData->getNumNodes() << "\n";
    std::cout << "Elements: " << meshData->getNumElements() << "\n\n";

    std::cout << meshData->getStatistics() << "\n";

    // ========================================================================
    // Step 7: Compute Mesh Quality
    // ========================================================================
    std::cout << "Step 7: Mesh Quality\n";
    std::cout << "--------------------\n";

    MeshQuality quality = mesher->computeQuality();

    std::cout << std::fixed << std::setprecision(3);
    std::cout << "Quality Metrics:\n";
    std::cout << "  Aspect Ratio:\n";
    std::cout << "    Min: " << quality.minAspectRatio << "\n";
    std::cout << "    Max: " << quality.maxAspectRatio << "\n";
    std::cout << "    Avg: " << quality.avgAspectRatio << "\n\n";

    std::cout << "  Jacobian:\n";
    std::cout << "    Min: " << quality.minJacobian << "\n";
    std::cout << "    Max: " << quality.maxJacobian << "\n";
    std::cout << "    Avg: " << quality.avgJacobian << "\n\n";

    std::cout << "  Problem Elements:\n";
    std::cout << "    Degenerate: " << quality.numDegenerateElements << "\n";
    std::cout << "    Inverted:   " << quality.numInvertedElements << "\n";
    std::cout << "    Poor:       " << quality.numPoorQualityElements << "\n\n";

    std::cout << "  Overall Grade: " << quality.getGrade() << "\n\n";

    // ========================================================================
    // Step 8: Quality Check
    // ========================================================================
    std::cout << "Step 8: Quality Check\n";
    std::cout << "---------------------\n";

    if (quality.isAcceptable()) {
        std::cout << "✓ Mesh quality is ACCEPTABLE\n\n";
    } else {
        std::cout << "✗ Mesh quality is NOT ACCEPTABLE\n";
        std::cout << "  Consider:\n";
        std::cout << "  - Reducing element size\n";
        std::cout << "  - Increasing optimization passes\n";
        std::cout << "  - Checking CAD geometry quality\n\n";
    }

    // ========================================================================
    // Step 9: Mesh Optimization (Optional)
    // ========================================================================
    if (quality.getGrade() < 'B') {
        std::cout << "Step 9: Mesh Optimization\n";
        std::cout << "-------------------------\n";

        std::cout << "Quality grade " << quality.getGrade() << " - applying optimization\n";

        if (mesher->optimizeMesh(5)) {
            std::cout << "Optimization successful!\n";

            MeshQuality newQuality = mesher->computeQuality();
            std::cout << "New grade: " << newQuality.getGrade() << "\n\n";
        } else {
            std::cout << "Optimization failed\n\n";
        }
    }

    // ========================================================================
    // Step 10: Save Mesh (Optional)
    // ========================================================================
    std::cout << "Step 10: Save Mesh (Optional)\n";
    std::cout << "-----------------------------\n";

    std::string outputFile = "output_mesh.msh";
    if (mesher->saveMeshToFile(outputFile)) {
        std::cout << "Mesh saved to: " << outputFile << "\n";
        std::cout << "(Can be viewed in Gmsh GUI)\n\n";
    }

    // ========================================================================
    // Examples: Different Mesh Presets
    // ========================================================================
    std::cout << "========================================\n";
    std::cout << "Examples: Different Mesh Presets\n";
    std::cout << "========================================\n\n";

    std::cout << "Coarse mesh (fast):\n";
    auto coarse = MeshParameters::coarse(10.0);
    std::cout << "  Element size: " << coarse.globalElementSize << " mm\n";
    std::cout << "  Quality threshold: " << coarse.minElementQuality << "\n\n";

    std::cout << "Fine mesh (high quality):\n";
    auto fine = MeshParameters::fine(1.0);
    std::cout << "  Element size: " << fine.globalElementSize << " mm\n";
    std::cout << "  Quality threshold: " << fine.minElementQuality << "\n";
    std::cout << "  Optimization passes: " << fine.numOptimizationPasses << "\n\n";

    std::cout << "Quadratic elements:\n";
    auto quadratic = MeshParameters::quadratic(5.0);
    std::cout << "  Element order: " << (quadratic.elementOrder == ElementOrder::Quadratic ? "Quadratic" : "Linear") << "\n";
    std::cout << "  High-order optimization: " << (quadratic.optimizeHighOrder ? "enabled" : "disabled") << "\n\n";

    std::cout << "Surface mesh only:\n";
    auto surface = MeshParameters::surfaceOnly(5.0);
    std::cout << "  Mesh type: Surface\n\n";

    // ========================================================================
    // Summary
    // ========================================================================
    std::cout << "========================================\n";
    std::cout << "Mesh Generation Complete!\n";
    std::cout << "========================================\n\n";

    std::cout << "Summary:\n";
    std::cout << "  Input:    " << filepath << "\n";
    std::cout << "  Solids:   " << importer->getNumSolids() << "\n";
    std::cout << "  Nodes:    " << meshData->getNumNodes() << "\n";
    std::cout << "  Elements: " << meshData->getNumElements() << "\n";
    std::cout << "  Quality:  Grade " << quality.getGrade() << "\n";
    std::cout << "  Output:   " << outputFile << "\n";

    return 0;
}
