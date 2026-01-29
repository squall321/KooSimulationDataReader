/**
 * @file batch_mesh_generation.cpp
 * @brief Example: Batch mesh generation from directory of STEP files
 *
 * This example demonstrates how to:
 * 1. Load all STEP files from a directory
 * 2. Generate meshes for each file
 * 3. Create LS-DYNA parts automatically
 * 4. Write output to keyword file
 */

#include <koo/dyna/Model.hpp>
#include <koo/dyna/managers/ModelManager.hpp>
#include <koo/dyna/managers/GeometryManager.hpp>
#include <koo/dyna/KeywordFileWriter.hpp>
#include <koo/mesh/MeshParameters.hpp>
#include <iostream>

using namespace koo;

int main(int argc, char** argv) {
    // Check command line arguments
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <directory_path> [output_file.k]\n";
        std::cout << "\nExample:\n";
        std::cout << "  " << argv[0] << " ./cad_models output.k\n";
        std::cout << "\nThis will:\n";
        std::cout << "  1. Scan ./cad_models for .step/.stp/.iges/.igs files\n";
        std::cout << "  2. Generate mesh for each file (element size: 5.0)\n";
        std::cout << "  3. Create LS-DYNA parts with Material 1, Section 1\n";
        std::cout << "  4. Write to output.k (default: mesh_output.k)\n";
        return 1;
    }

    std::string directoryPath = argv[1];
    std::string outputFile = (argc >= 3) ? argv[2] : "mesh_output.k";

    std::cout << "=== Batch Mesh Generation ===" << std::endl;
    std::cout << "Directory: " << directoryPath << std::endl;
    std::cout << "Output:    " << outputFile << std::endl;
    std::cout << std::endl;

    // Create model and managers
    dyna::Model model;
    dyna::managers::ModelManager modelMgr(model);
    dyna::managers::GeometryManager geomMgr(model, modelMgr);

    // Set mesh parameters (medium quality, element size: 5.0)
    mesh::MeshParameters meshParams = mesh::MeshParameters::medium(5.0);

    std::cout << "Mesh Parameters:" << std::endl;
    std::cout << "  Element size: 5.0" << std::endl;
    std::cout << "  Quality:      Medium" << std::endl;
    std::cout << "  Element order: Linear" << std::endl;
    std::cout << std::endl;

    // Import all STEP/IGES files from directory
    std::cout << "Scanning directory for CAD files..." << std::endl;

    int materialId = 1;
    int sectionId = 1;

    auto partIds = geomMgr.importDirectoryAndMesh(
        directoryPath,
        meshParams,
        materialId,
        sectionId
    );

    if (partIds.empty()) {
        std::cerr << "ERROR: No parts created!" << std::endl;
        std::cerr << "Last error: " << geomMgr.getLastError() << std::endl;
        return 1;
    }

    std::cout << "\nSuccessfully created " << partIds.size() << " parts:" << std::endl;

    // Print summary for each part
    for (size_t i = 0; i < partIds.size(); ++i) {
        auto partId = partIds[i];
        std::cout << "  Part " << (i+1) << " (ID " << partId << ")" << std::endl;
    }

    // Print model statistics
    std::cout << "\nModel Statistics:" << std::endl;
    std::cout << "  Total nodes:    " << model.getNodeCount() << std::endl;
    std::cout << "  Shell elements: " << model.getShellElementCount() << std::endl;
    std::cout << "  Solid elements: " << model.getSolidElementCount() << std::endl;
    std::cout << "  Parts:          " << partIds.size() << std::endl;
    std::cout << std::endl;

    // Write LS-DYNA keyword file
    std::cout << "Writing LS-DYNA keyword file: " << outputFile << std::endl;

    dyna::KeywordFileWriter writer;
    if (!writer.write(model, outputFile)) {
        std::cerr << "ERROR: Failed to write keyword file!" << std::endl;
        return 1;
    }

    std::cout << "SUCCESS: Mesh generation complete!" << std::endl;
    std::cout << "\nYou can now open '" << outputFile << "' in LS-PrePost to visualize the mesh." << std::endl;

    return 0;
}
