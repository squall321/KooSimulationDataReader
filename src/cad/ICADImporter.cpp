#include <koo/cad/ICADImporter.hpp>

#ifdef KOO_HAS_OPENCASCADE
#include <koo/cad/OccadImporter.hpp>
#endif

namespace koo::cad {

// Factory function - returns appropriate CAD importer based on backend
std::unique_ptr<ICADImporter> createCADImporter(const std::string& backend) {
    if (backend == "opencascade") {
#ifdef KOO_HAS_OPENCASCADE
        return std::make_unique<OccadImporter>();
#else
        return nullptr;  // OpenCASCADE not available
#endif
    }

    return nullptr;  // Unknown backend
}

} // namespace koo::cad
