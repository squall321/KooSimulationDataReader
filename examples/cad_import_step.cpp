// Example: Import STEP file using OpenCASCADE
// Demonstrates CAD file import with OccadImporter

#include <koo/cad/ICADImporter.hpp>
#include <koo/cad/OccadImporter.hpp>
#include <iostream>
#include <iomanip>

using namespace koo::cad;

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <file.step|file.iges>\n";
        return 1;
    }

    std::string filepath = argv[1];

    std::cout << "========================================\n";
    std::cout << "CAD Import Example\n";
    std::cout << "========================================\n\n";

    // ========================================================================
    // Method 1: Direct instantiation
    // ========================================================================
    std::cout << "Method 1: Direct instantiation (OccadImporter)\n";
    std::cout << "-----------------------------------------------\n";

    OccadImporter importer;

    // Optional: Configure import settings
    importer.setImportTolerance(1e-6);
    importer.setShapeHealing(true);

    std::cout << "Import tolerance: " << importer.getImportTolerance() << "\n";
    std::cout << "Shape healing:    " << (importer.isShapeHealingEnabled() ? "enabled" : "disabled") << "\n\n";

    // Import file
    std::cout << "Importing: " << filepath << "\n";

    if (!importer.importFile(filepath)) {
        std::cerr << "ERROR: " << importer.getLastError() << "\n";
        return 1;
    }

    std::cout << "Import successful!\n\n";

    // ========================================================================
    // Topology Information
    // ========================================================================
    std::cout << "========================================\n";
    std::cout << "Topology Information\n";
    std::cout << "========================================\n\n";

    std::cout << "Solids:   " << importer.getNumSolids() << "\n";
    std::cout << "Faces:    " << importer.getNumFaces() << "\n";
    std::cout << "Edges:    " << importer.getNumEdges() << "\n";
    std::cout << "Vertices: " << importer.getNumVertices() << "\n\n";

    // ========================================================================
    // Geometric Queries
    // ========================================================================
    std::cout << "========================================\n";
    std::cout << "Geometric Queries\n";
    std::cout << "========================================\n\n";

    auto bbox = importer.getBoundingBox();
    std::cout << std::fixed << std::setprecision(3);
    std::cout << "Bounding Box:\n";
    std::cout << "  Min: (" << bbox.min.x << ", " << bbox.min.y << ", " << bbox.min.z << ")\n";
    std::cout << "  Max: (" << bbox.max.x << ", " << bbox.max.y << ", " << bbox.max.z << ")\n";
    std::cout << "  Size: (" << bbox.getWidth() << " x " << bbox.getHeight() << " x " << bbox.getDepth() << ")\n\n";

    double volume = importer.getTotalVolume();
    double area = importer.getTotalSurfaceArea();

    std::cout << "Total Volume: " << volume << "\n";
    std::cout << "Surface Area: " << area << "\n\n";

    // ========================================================================
    // Geometry Access
    // ========================================================================
    std::cout << "========================================\n";
    std::cout << "Geometry Entities\n";
    std::cout << "========================================\n\n";

    auto geometry = importer.getGeometry();
    if (geometry) {
        std::cout << "Geometry object created successfully\n";
        std::cout << "Solids in geometry: " << geometry->getNumSolids() << "\n";
        std::cout << "Faces in geometry:  " << geometry->getNumFaces() << "\n\n";

        // Print first few solids
        const auto& solids = geometry->getSolids();
        int count = std::min(5, static_cast<int>(solids.size()));
        std::cout << "First " << count << " solids:\n";
        for (int i = 0; i < count; ++i) {
            const auto& solid = solids[i];
            std::cout << "  Solid " << solid.id << ":\n";
            std::cout << "    Bounding box: ("
                      << solid.bounds.min.x << ", "
                      << solid.bounds.min.y << ", "
                      << solid.bounds.min.z << ") to ("
                      << solid.bounds.max.x << ", "
                      << solid.bounds.max.y << ", "
                      << solid.bounds.max.z << ")\n";
        }
        std::cout << "\n";
    }

    // ========================================================================
    // Method 2: Factory function
    // ========================================================================
    std::cout << "========================================\n";
    std::cout << "Method 2: Factory function\n";
    std::cout << "========================================\n\n";

    auto importer2 = createCADImporter("opencascade");
    if (!importer2) {
        std::cerr << "OpenCASCADE backend not available\n";
        return 1;
    }

    std::cout << "Created CAD importer via factory\n";

    if (importer2->importFile(filepath)) {
        std::cout << "Import successful via factory!\n";
        std::cout << "Solids: " << importer2->getNumSolids() << "\n\n";
    }

    // ========================================================================
    // Cleanup
    // ========================================================================
    std::cout << "========================================\n";
    std::cout << "Cleanup\n";
    std::cout << "========================================\n\n";

    importer.clear();
    std::cout << "Geometry cleared\n";
    std::cout << "Has geometry after clear: " << (importer.hasGeometry() ? "yes" : "no") << "\n\n";

    std::cout << "========================================\n";
    std::cout << "CAD import example complete!\n";
    std::cout << "========================================\n";

    return 0;
}
