#pragma once

#include <koo/Export.hpp>
#include <koo/common/Vec3.hpp>
#include <vector>
#include <string>

namespace koo::mesh {

/**
 * @brief Element type enumeration
 */
enum class ElementType {
    // 2D elements
    Tri3,       ///< 3-node triangle
    Tri6,       ///< 6-node triangle (quadratic)
    Quad4,      ///< 4-node quadrilateral
    Quad8,      ///< 8-node quadrilateral (quadratic)

    // 3D elements
    Tet4,       ///< 4-node tetrahedron
    Tet10,      ///< 10-node tetrahedron (quadratic)
    Hex8,       ///< 8-node hexahedron
    Hex20,      ///< 20-node hexahedron (quadratic)
    Pyramid5,   ///< 5-node pyramid
    Prism6,     ///< 6-node prism/wedge

    Unknown     ///< Unknown element type
};

/**
 * @brief Convert element type to string
 */
inline std::string toString(ElementType type) {
    switch (type) {
        case ElementType::Tri3:     return "Tri3";
        case ElementType::Tri6:     return "Tri6";
        case ElementType::Quad4:    return "Quad4";
        case ElementType::Quad8:    return "Quad8";
        case ElementType::Tet4:     return "Tet4";
        case ElementType::Tet10:    return "Tet10";
        case ElementType::Hex8:     return "Hex8";
        case ElementType::Hex20:    return "Hex20";
        case ElementType::Pyramid5: return "Pyramid5";
        case ElementType::Prism6:   return "Prism6";
        default:                    return "Unknown";
    }
}

/**
 * @brief Get number of nodes for element type
 */
inline int getNumNodes(ElementType type) {
    switch (type) {
        case ElementType::Tri3:     return 3;
        case ElementType::Tri6:     return 6;
        case ElementType::Quad4:    return 4;
        case ElementType::Quad8:    return 8;
        case ElementType::Tet4:     return 4;
        case ElementType::Tet10:    return 10;
        case ElementType::Hex8:     return 8;
        case ElementType::Hex20:    return 20;
        case ElementType::Pyramid5: return 5;
        case ElementType::Prism6:   return 6;
        default:                    return 0;
    }
}

/**
 * @brief Check if element type is 2D
 */
inline bool is2D(ElementType type) {
    return type == ElementType::Tri3 || type == ElementType::Tri6 ||
           type == ElementType::Quad4 || type == ElementType::Quad8;
}

/**
 * @brief Check if element type is 3D
 */
inline bool is3D(ElementType type) {
    return type == ElementType::Tet4 || type == ElementType::Tet10 ||
           type == ElementType::Hex8 || type == ElementType::Hex20 ||
           type == ElementType::Pyramid5 || type == ElementType::Prism6;
}

/**
 * @brief Mesh algorithm types
 */
enum class MeshAlgorithm {
    Automatic,              ///< Let mesher choose
    Delaunay,               ///< Delaunay triangulation
    Frontal,                ///< Advancing front
    PackingParallelograms,  ///< Packing of parallelograms (Gmsh)
    MeshAdapt               ///< Adaptive meshing
};

/**
 * @brief Mesh type
 */
enum class MeshType {
    Surface,    ///< Surface mesh only (2D elements)
    Volume,     ///< Volume mesh only (3D elements)
    Both        ///< Both surface and volume meshes
};

/**
 * @brief Element order
 */
enum class ElementOrder {
    Linear,     ///< Linear elements (e.g., Tet4, Hex8)
    Quadratic   ///< Quadratic elements (e.g., Tet10, Hex20)
};

} // namespace koo::mesh
