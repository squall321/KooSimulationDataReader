// Example demonstrating basic usage of the manager system
// This corresponds to Phase 1: Basic Managers (PartManager, ElementManager, NodeManager)

#include <koo/dyna/KeywordFileReader.hpp>
#include <koo/dyna/managers/PartManager.hpp>
#include <koo/dyna/managers/ElementManager.hpp>
#include <koo/dyna/managers/NodeManager.hpp>
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
    // Load model
    // ========================================================================
    std::cout << "Loading model from: " << argv[1] << "\n\n";

    KeywordFileReader reader;
    Model model = reader.read(argv[1]);

    // ========================================================================
    // Create managers and build indices
    // ========================================================================
    std::cout << "Building manager indices...\n";

    PartManager partMgr(model);
    ElementManager elemMgr(model);
    NodeManager nodeMgr(model);

    partMgr.buildIndex();
    elemMgr.buildIndex();
    nodeMgr.buildIndex();

    std::cout << "Indices built successfully!\n\n";

    // ========================================================================
    // Example 1: List all parts with statistics
    // ========================================================================
    std::cout << "========================================\n";
    std::cout << "Example 1: Part Statistics\n";
    std::cout << "========================================\n\n";

    auto allPartIds = partMgr.getAllPartIds();
    std::cout << "Total parts: " << allPartIds.size() << "\n\n";

    std::cout << std::left << std::setw(8) << "PID"
              << std::setw(12) << "Elements"
              << std::setw(12) << "Nodes"
              << std::setw(10) << "Material"
              << std::setw(10) << "Section"
              << "Title\n";
    std::cout << std::string(80, '-') << "\n";

    for (PartId pid : allPartIds) {
        auto stats = partMgr.getStatistics(pid);

        std::cout << std::left << std::setw(8) << pid
                  << std::setw(12) << stats.elementCount
                  << std::setw(12) << stats.nodeCount
                  << std::setw(10) << stats.materialId
                  << std::setw(10) << stats.sectionId
                  << stats.title << "\n";
    }
    std::cout << "\n";

    // ========================================================================
    // Example 2: Element type distribution
    // ========================================================================
    std::cout << "========================================\n";
    std::cout << "Example 2: Element Type Distribution\n";
    std::cout << "========================================\n\n";

    auto shells = elemMgr.getShellElements();
    auto solids = elemMgr.getSolidElements();
    auto beams = elemMgr.getBeamElements();
    auto discretes = elemMgr.getDiscreteElements();

    std::cout << "Shell elements:    " << shells.size() << "\n";
    std::cout << "Solid elements:    " << solids.size() << "\n";
    std::cout << "Beam elements:     " << beams.size() << "\n";
    std::cout << "Discrete elements: " << discretes.size() << "\n";
    std::cout << "Total elements:    " << elemMgr.getElementCount() << "\n\n";

    // ========================================================================
    // Example 3: Analyze a specific part
    // ========================================================================
    if (!allPartIds.empty()) {
        PartId pid = allPartIds[0];  // Analyze first part

        std::cout << "========================================\n";
        std::cout << "Example 3: Detailed Part Analysis (PID=" << pid << ")\n";
        std::cout << "========================================\n\n";

        // Get elements in this part
        auto elements = partMgr.getElements(pid);
        std::cout << "Elements in part " << pid << ": " << elements.size() << "\n";

        // Analyze element types
        size_t shellCount = 0, solidCount = 0;
        for (ElementId eid : elements) {
            ElementType type = elemMgr.getElementType(eid);
            if (type == ElementType::Shell) shellCount++;
            else if (type == ElementType::Solid) solidCount++;
        }

        std::cout << "  Shell: " << shellCount << "\n";
        std::cout << "  Solid: " << solidCount << "\n\n";

        // Get nodes in this part
        auto nodes = partMgr.getNodes(pid);
        std::cout << "Nodes in part " << pid << ": " << nodes.size() << "\n";

        // Compute bounding box
        auto bbox = partMgr.getBoundingBox(pid);
        if (bbox.isValid()) {
            std::cout << "Bounding box:\n";
            std::cout << "  Min: (" << bbox.min.x << ", " << bbox.min.y << ", " << bbox.min.z << ")\n";
            std::cout << "  Max: (" << bbox.max.x << ", " << bbox.max.y << ", " << bbox.max.z << ")\n";
            Vec3 size = bbox.size();
            std::cout << "  Size: (" << size.x << ", " << size.y << ", " << size.z << ")\n";
        }
        std::cout << "\n";
    }

    // ========================================================================
    // Example 4: Node connectivity analysis
    // ========================================================================
    auto allNodeIds = nodeMgr.getAllNodeIds();
    if (!allNodeIds.empty()) {
        std::cout << "========================================\n";
        std::cout << "Example 4: Node Connectivity Analysis\n";
        std::cout << "========================================\n\n";

        std::cout << "Total nodes: " << allNodeIds.size() << "\n\n";

        // Analyze first few nodes
        size_t sampleCount = std::min(static_cast<size_t>(5), allNodeIds.size());

        std::cout << std::left << std::setw(12) << "Node ID"
                  << std::setw(15) << "Connected Elems"
                  << "Position\n";
        std::cout << std::string(80, '-') << "\n";

        for (size_t i = 0; i < sampleCount; ++i) {
            NodeId nid = allNodeIds[i];
            auto connectedElems = nodeMgr.getConnectedElements(nid);
            auto pos = nodeMgr.getPosition(nid);

            std::cout << std::left << std::setw(12) << nid
                      << std::setw(15) << connectedElems.size()
                      << "(" << pos.x << ", " << pos.y << ", " << pos.z << ")\n";
        }
        std::cout << "\n";
    }

    // ========================================================================
    // Example 5: Birth/Death time analysis
    // ========================================================================
    std::cout << "========================================\n";
    std::cout << "Example 5: Element Birth/Death Times\n";
    std::cout << "========================================\n\n";

    size_t birthCount = 0, deathCount = 0;
    for (ElementId eid : elemMgr.getAllElementIds()) {
        if (elemMgr.getBirthTime(eid)) birthCount++;
        if (elemMgr.getDeathTime(eid)) deathCount++;
    }

    std::cout << "Elements with birth time: " << birthCount << "\n";
    std::cout << "Elements with death time: " << deathCount << "\n";

    // Check element status at specific time
    if (birthCount > 0 || deathCount > 0) {
        double checkTime = 5.0;
        size_t aliveCount = 0;
        for (ElementId eid : elemMgr.getAllElementIds()) {
            if (elemMgr.isAliveAt(eid, checkTime)) {
                aliveCount++;
            }
        }
        std::cout << "Elements alive at t=" << checkTime << ": "
                  << aliveCount << " / " << elemMgr.getElementCount() << "\n";
    }
    std::cout << "\n";

    // ========================================================================
    // Example 6: Segment extraction for contact surfaces
    // ========================================================================
    if (!shells.empty()) {
        std::cout << "========================================\n";
        std::cout << "Example 6: Segment Extraction\n";
        std::cout << "========================================\n\n";

        // Extract segments from first shell element
        ElementId sampleEid = shells[0];
        auto segments = elemMgr.getSegments(sampleEid);

        std::cout << "Element " << sampleEid << " has " << segments.size() << " segment(s)\n";
        for (const auto& seg : segments) {
            std::cout << "  Face " << seg.faceIndex << ": "
                      << seg.nodeIds.size() << " nodes [";
            for (size_t i = 0; i < seg.nodeIds.size(); ++i) {
                if (i > 0) std::cout << ", ";
                std::cout << seg.nodeIds[i];
            }
            std::cout << "]\n";
        }
        std::cout << "\n";

        // Count total segments in model
        size_t totalSegments = 0;
        for (ElementId eid : shells) {
            totalSegments += elemMgr.getSegments(eid).size();
        }
        for (ElementId eid : solids) {
            totalSegments += elemMgr.getSegments(eid).size();
        }
        std::cout << "Total segments in model: " << totalSegments << "\n";
        std::cout << "(These can be used for contact definitions)\n\n";
    }

    std::cout << "========================================\n";
    std::cout << "Manager usage examples completed!\n";
    std::cout << "========================================\n";

    return 0;
}
