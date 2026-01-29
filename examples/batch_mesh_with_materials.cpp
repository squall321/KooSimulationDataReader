/**
 * @file batch_mesh_with_materials.cpp
 * @brief Example: Batch meshing with different materials per file
 *
 * This example demonstrates how to assign different materials/sections
 * to different CAD files in a batch import.
 */

#include <koo/dyna/Model.hpp>
#include <koo/dyna/managers/ModelManager.hpp>
#include <koo/dyna/managers/GeometryManager.hpp>
#include <koo/dyna/KeywordFileWriter.hpp>
#include <koo/mesh/MeshParameters.hpp>
#include <iostream>
#include <map>

using namespace koo;

int main(int argc, char** argv) {
    std::cout << "=== Batch Mesh Generation with Material Mapping ===" << std::endl;
    std::cout << std::endl;

    // Create model and managers
    dyna::Model model;
    dyna::managers::ModelManager modelMgr(model);
    dyna::managers::GeometryManager geomMgr(model, modelMgr);

    // Define mesh parameters
    mesh::MeshParameters meshParams = mesh::MeshParameters::fine(2.0);

    std::cout << "Mesh Parameters:" << std::endl;
    std::cout << "  Element size: 2.0 (fine)" << std::endl;
    std::cout << "  Element order: Linear" << std::endl;
    std::cout << std::endl;

    // Define file-to-material mapping
    // Map structure: filepath → (materialId, sectionId)
    std::map<std::string, std::pair<int, int>> fileMapping = {
        {"part_steel.step",     {1, 1}},  // Steel: Material 1, Section 1
        {"part_aluminum.step",  {2, 2}},  // Aluminum: Material 2, Section 2
        {"part_rubber.step",    {3, 3}},  // Rubber: Material 3, Section 3
    };

    std::cout << "File-to-Material Mapping:" << std::endl;
    for (const auto& entry : fileMapping) {
        std::cout << "  " << entry.first
                  << " → Material " << entry.second.first
                  << ", Section " << entry.second.second << std::endl;
    }
    std::cout << std::endl;

    // Batch import with mapping
    std::cout << "Importing CAD files..." << std::endl;

    auto partIds = geomMgr.importBatchAndMesh(
        fileMapping,
        meshParams
    );

    if (partIds.empty()) {
        std::cerr << "ERROR: No parts created!" << std::endl;
        std::cerr << "Last error: " << geomMgr.getLastError() << std::endl;
        return 1;
    }

    std::cout << "\nSuccessfully created " << partIds.size() << " parts" << std::endl;

    // Print model statistics
    std::cout << "\nModel Statistics:" << std::endl;
    std::cout << "  Total nodes:    " << model.getNodeCount() << std::endl;
    std::cout << "  Shell elements: " << model.getShellElementCount() << std::endl;
    std::cout << "  Solid elements: " << model.getSolidElementCount() << std::endl;
    std::cout << "  Parts:          " << partIds.size() << std::endl;
    std::cout << std::endl;

    // Write LS-DYNA keyword file
    std::string outputFile = "multi_material_mesh.k";
    std::cout << "Writing LS-DYNA keyword file: " << outputFile << std::endl;

    dyna::KeywordFileWriter writer;
    if (!writer.write(model, outputFile)) {
        std::cerr << "ERROR: Failed to write keyword file!" << std::endl;
        return 1;
    }

    std::cout << "SUCCESS: Multi-material mesh generation complete!" << std::endl;
    std::cout << "\nNote: You need to define *MAT and *SECTION keywords separately" << std::endl;
    std::cout << "      for each material/section ID used." << std::endl;

    return 0;
}
