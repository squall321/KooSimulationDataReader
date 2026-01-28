#include <koo/mesh/IMeshGenerator.hpp>

#ifdef KOO_HAS_GMSH
#include <koo/mesh/GmshMeshGenerator.hpp>
#endif

namespace koo::mesh {

// Factory function - returns appropriate mesh generator based on backend
std::unique_ptr<IMeshGenerator> createMeshGenerator(const std::string& backend) {
    if (backend == "gmsh") {
#ifdef KOO_HAS_GMSH
        return std::make_unique<GmshMeshGenerator>();
#else
        return nullptr;  // Gmsh not available
#endif
    }

    return nullptr;  // Unknown backend
}

} // namespace koo::mesh
