/**
 * @file structured_mesh_generation.cpp
 * @brief Example: Structured hexahedral mesh generation
 *
 * This example demonstrates how to:
 * 1. Generate structured (hexahedral) meshes
 * 2. Use extrusion-based meshing for appropriate geometries
 * 3. Control element size and layer count
 * 4. Automatic K-file output
 */

#include <koo/dyna/Model.hpp>
#include <koo/dyna/managers/ModelManager.hpp>
#include <koo/dyna/managers/GeometryManager.hpp>
#include <koo/dyna/KeywordFileWriter.hpp>
#include <koo/mesh/MeshParameters.hpp>
#include <iostream>

using namespace koo;

int main(int argc, char** argv) {
    std::cout << "=== Structured Hexahedral Mesh Generation ===" << std::endl;
    std::cout << std::endl;

    // Check command line arguments
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <step_file> [output_file.k] [mesh_type]" << std::endl;
        std::cout << std::endl;
        std::cout << "Mesh Types:" << std::endl;
        std::cout << "  1 - Structured (transfinite) mesh" << std::endl;
        std::cout << "  2 - Extrusion-based mesh" << std::endl;
        std::cout << "  3 - Hex-dominant mesh (default)" << std::endl;
        std::cout << std::endl;
        std::cout << "Examples:" << std::endl;
        std::cout << "  " << argv[0] << " box.step output.k 1    # Structured mesh" << std::endl;
        std::cout << "  " << argv[0] << " plate.step output.k 2  # Extruded mesh" << std::endl;
        std::cout << "  " << argv[0] << " part.step              # Hex-dominant (auto)" << std::endl;
        return 1;
    }

    std::string cadFile = argv[1];
    std::string outputFile = (argc >= 3) ? argv[2] : "structured_mesh.k";
    int meshType = (argc >= 4) ? std::stoi(argv[3]) : 3;

    std::cout << "Input CAD file: " << cadFile << std::endl;
    std::cout << "Output file:    " << outputFile << std::endl;
    std::cout << std::endl;

    // Create model and managers
    dyna::Model model;
    dyna::managers::ModelManager modelMgr(model);
    dyna::managers::GeometryManager geomMgr(model, modelMgr);

    // Define mesh parameters based on type
    mesh::MeshParameters meshParams;

    switch (meshType) {
        case 1: {
            // Structured (transfinite) mesh
            std::cout << "Mesh Type: Structured (transfinite)" << std::endl;
            std::cout << "  Element type: Hexahedra (Hex8)" << std::endl;
            std::cout << "  Divisions: 10 per edge" << std::endl;
            std::cout << std::endl;

            meshParams = mesh::MeshParameters::structured(5.0, 10);
            break;
        }

        case 2: {
            // Extrusion-based mesh
            std::cout << "Mesh Type: Extrusion-based" << std::endl;
            std::cout << "  Element type: Hexahedra (Hex8)" << std::endl;
            std::cout << "  Layer thickness: 1.0" << std::endl;
            std::cout << "  Number of layers: 10" << std::endl;
            std::cout << std::endl;

            meshParams = mesh::MeshParameters::extruded(1.0, 10);
            break;
        }

        case 3:
        default: {
            // Hex-dominant (fallback for complex geometries)
            std::cout << "Mesh Type: Hex-dominant (automatic)" << std::endl;
            std::cout << "  Element type: Mixed (Hex8 + Tet4)" << std::endl;
            std::cout << "  Element size: 5.0" << std::endl;
            std::cout << std::endl;

            meshParams = mesh::MeshParameters::hexDominant(5.0);
            break;
        }
    }

    // Import CAD and generate mesh
    std::cout << "Importing CAD file..." << std::endl;

    auto partId = geomMgr.importAndMeshCAD(
        cadFile,
        meshParams,
        1,  // Material ID
        1,  // Section ID
        "StructuredPart"
    );

    if (partId < 0) {
        std::cerr << "ERROR: Failed to import and mesh CAD file!" << std::endl;
        std::cerr << "Error: " << geomMgr.getLastError() << std::endl;
        return 1;
    }

    std::cout << "SUCCESS: Part created (ID " << partId << ")" << std::endl;
    std::cout << std::endl;

    // Print model statistics
    std::cout << "Model Statistics:" << std::endl;
    std::cout << "  Total nodes:    " << model.getNodeCount() << std::endl;
    std::cout << "  Shell elements: " << model.getShellElementCount() << std::endl;
    std::cout << "  Solid elements: " << model.getSolidElementCount() << std::endl;

    // Count element types
    const auto* solids = model.getSolidElements();
    if (solids) {
        int hexCount = 0, tetCount = 0, prismCount = 0, pyramidCount = 0;

        for (const auto& elem : solids->getElements()) {
            // Element type detection based on node count
            size_t nodeCount = elem.nodeIds.size();
            if (nodeCount == 8) hexCount++;
            else if (nodeCount == 4) tetCount++;
            else if (nodeCount == 6) prismCount++;
            else if (nodeCount == 5) pyramidCount++;
        }

        std::cout << std::endl;
        std::cout << "Element Type Breakdown:" << std::endl;
        std::cout << "  Hexahedra (Hex8):  " << hexCount << std::endl;
        std::cout << "  Tetrahedra (Tet4): " << tetCount << std::endl;
        std::cout << "  Prisms (Prism6):   " << prismCount << std::endl;
        std::cout << "  Pyramids (Pyr5):   " << pyramidCount << std::endl;

        if (hexCount > 0) {
            double hexRatio = 100.0 * hexCount / (hexCount + tetCount + prismCount + pyramidCount);
            std::cout << "  Hex ratio:         " << hexRatio << "%" << std::endl;
        }
    }

    std::cout << std::endl;

    // Write LS-DYNA keyword file
    std::cout << "Writing LS-DYNA keyword file: " << outputFile << std::endl;

    dyna::KeywordFileWriter writer;
    if (!writer.write(model, outputFile)) {
        std::cerr << "ERROR: Failed to write keyword file!" << std::endl;
        return 1;
    }

    std::cout << "SUCCESS: Structured mesh generation complete!" << std::endl;
    std::cout << std::endl;
    std::cout << "Next steps:" << std::endl;
    std::cout << "  1. Open '" << outputFile << "' in LS-PrePost to visualize" << std::endl;
    std::cout << "  2. Add *MAT and *SECTION keywords for material properties" << std::endl;
    std::cout << "  3. Define boundary conditions and loads" << std::endl;
    std::cout << std::endl;
    std::cout << "Note: Structured meshes provide better accuracy for linear problems" << std::endl;
    std::cout << "      and reduce computational cost compared to tetrahedral meshes." << std::endl;

    return 0;
}
