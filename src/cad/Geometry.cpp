#include <koo/cad/Geometry.hpp>

namespace koo::cad {

Geometry::Geometry()
    : nativeGeometry_(nullptr)
{
}

Geometry::~Geometry() {
    // Note: nativeGeometry_ cleanup is handled by CAD importer
    // We don't own the native geometry directly
}

void Geometry::clear() {
    solids_.clear();
    faces_.clear();
    edges_.clear();
    vertices_.clear();
    globalBounds_ = common::BoundingBox();
    nativeGeometry_ = nullptr;
}

} // namespace koo::cad
