#pragma once

#include <koo/Export.hpp>
#include <koo/common/Vec3.hpp>
#include <vector>
#include <string>

namespace koo::cad {

/**
 * @brief Geometry entity types
 */
enum class EntityType {
    Solid,      ///< 3D solid body
    Face,       ///< 2D surface
    Edge,       ///< 1D curve
    Vertex      ///< 0D point
};

/**
 * @brief Convert entity type to string
 */
inline std::string toString(EntityType type) {
    switch (type) {
        case EntityType::Solid:  return "Solid";
        case EntityType::Face:   return "Face";
        case EntityType::Edge:   return "Edge";
        case EntityType::Vertex: return "Vertex";
        default:                  return "Unknown";
    }
}

/**
 * @brief Geometry entity representing a topological object
 *
 * Wraps a native CAD kernel shape (e.g., OpenCASCADE TopoDS_Shape)
 * with a library-neutral interface.
 */
struct KOO_API GeometryEntity {
    EntityType type;           ///< Entity type
    int id;                    ///< Unique ID within type
    common::BoundingBox bounds;     ///< Axis-aligned bounding box
    void* nativeHandle;        ///< Opaque pointer to native shape (e.g., TopoDS_Shape*)

    GeometryEntity()
        : type(EntityType::Vertex)
        , id(-1)
        , bounds()
        , nativeHandle(nullptr)
    {}

    GeometryEntity(EntityType t, int entityId, const common::BoundingBox& b, void* handle = nullptr)
        : type(t)
        , id(entityId)
        , bounds(b)
        , nativeHandle(handle)
    {}
};

} // namespace koo::cad
