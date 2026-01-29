#pragma once

#include <koo/Export.hpp>
#include <koo/mesh/MeshTypes.hpp>

namespace koo::mesh {

/**
 * @brief Mesh generation parameters
 *
 * Controls mesh density, algorithm selection, element type, and quality.
 * Provides presets for common scenarios.
 */
struct KOO_API MeshParameters {
    // ========================================================================
    // Element Sizing
    // ========================================================================

    double globalElementSize = 1.0;    ///< Global element size
    double minElementSize = 0.1;       ///< Minimum element size
    double maxElementSize = 10.0;      ///< Maximum element size

    // ========================================================================
    // Algorithm Selection
    // ========================================================================

    MeshAlgorithm algorithm = MeshAlgorithm::Automatic;  ///< Mesh algorithm

    // ========================================================================
    // Mesh Type
    // ========================================================================

    MeshType meshType = MeshType::Both;  ///< Surface, volume, or both

    // ========================================================================
    // Element Order
    // ========================================================================

    ElementOrder elementOrder = ElementOrder::Linear;  ///< Linear or quadratic

    // ========================================================================
    // Quality Control
    // ========================================================================

    double minElementQuality = 0.1;    ///< Minimum element quality (0-1)
    int numOptimizationPasses = 3;     ///< Number of optimization passes
    bool optimizeNetgen = true;        ///< Use Netgen optimizer (if available)
    bool optimizeHighOrder = false;    ///< Optimize high-order nodes

    // ========================================================================
    // Advanced Options
    // ========================================================================

    bool allowQuads = false;           ///< Allow quad elements (mixed mesh)
    bool allowHexes = false;           ///< Allow hex elements (mixed mesh)
    bool recombineAll = false;         ///< Recombine triangles into quads
    double smoothingSteps = 1;         ///< Number of smoothing iterations

    // ========================================================================
    // Structured/Extrusion Options
    // ========================================================================

    bool useStructuredMesh = false;    ///< Use structured (transfinite) mesh
    bool autoDetectExtrusion = true;   ///< Auto-detect extrudable geometries
    int extrusionLayers = 10;          ///< Number of layers for extrusion
    double extrusionLayerThickness = 1.0;  ///< Thickness per extrusion layer
    bool useTransfinite = false;       ///< Use transfinite meshing (structured)
    int transfiniteDivisions = 10;     ///< Divisions for transfinite mesh

    // ========================================================================
    // Presets
    // ========================================================================

    /**
     * @brief Coarse mesh preset
     * @param size Global element size
     * @return Parameters for coarse mesh
     */
    static MeshParameters coarse(double size = 10.0) {
        MeshParameters params;
        params.globalElementSize = size;
        params.minElementSize = size * 0.1;
        params.maxElementSize = size * 2.0;
        params.minElementQuality = 0.2;
        params.numOptimizationPasses = 1;
        return params;
    }

    /**
     * @brief Medium mesh preset (balanced)
     * @param size Global element size
     * @return Parameters for medium mesh
     */
    static MeshParameters medium(double size = 5.0) {
        MeshParameters params;
        params.globalElementSize = size;
        params.minElementSize = size * 0.1;
        params.maxElementSize = size * 1.5;
        params.minElementQuality = 0.3;
        params.numOptimizationPasses = 3;
        return params;
    }

    /**
     * @brief Fine mesh preset (high quality)
     * @param size Global element size
     * @return Parameters for fine mesh
     */
    static MeshParameters fine(double size = 1.0) {
        MeshParameters params;
        params.globalElementSize = size;
        params.minElementSize = size * 0.05;
        params.maxElementSize = size * 1.2;
        params.minElementQuality = 0.4;
        params.numOptimizationPasses = 5;
        params.optimizeHighOrder = true;
        return params;
    }

    /**
     * @brief Surface mesh only
     * @param size Global element size
     * @return Parameters for surface mesh
     */
    static MeshParameters surfaceOnly(double size = 5.0) {
        MeshParameters params = medium(size);
        params.meshType = MeshType::Surface;
        return params;
    }

    /**
     * @brief Volume mesh only
     * @param size Global element size
     * @return Parameters for volume mesh
     */
    static MeshParameters volumeOnly(double size = 5.0) {
        MeshParameters params = medium(size);
        params.meshType = MeshType::Volume;
        return params;
    }

    /**
     * @brief Quadratic elements
     * @param size Global element size
     * @return Parameters for quadratic mesh
     */
    static MeshParameters quadratic(double size = 5.0) {
        MeshParameters params = medium(size);
        params.elementOrder = ElementOrder::Quadratic;
        params.optimizeHighOrder = true;
        return params;
    }

    /**
     * @brief Hex-dominant mesh (if possible)
     * @param size Global element size
     * @return Parameters for hex-dominant mesh
     */
    static MeshParameters hexDominant(double size = 5.0) {
        MeshParameters params = medium(size);
        params.allowQuads = true;
        params.allowHexes = true;
        params.recombineAll = true;
        return params;
    }

    /**
     * @brief Structured mesh preset (hexahedral)
     * @param size Global element size
     * @param divisions Number of divisions for transfinite mesh
     * @return Parameters for structured mesh
     */
    static MeshParameters structured(double size = 5.0, int divisions = 10) {
        MeshParameters params = medium(size);
        params.useStructuredMesh = true;
        params.allowQuads = true;
        params.allowHexes = true;
        params.recombineAll = true;
        params.useTransfinite = true;
        params.transfiniteDivisions = divisions;
        params.numOptimizationPasses = 1;  // Less optimization needed for structured
        return params;
    }

    /**
     * @brief Extrusion-based mesh preset
     * @param layerThickness Thickness per layer
     * @param numLayers Number of layers
     * @return Parameters for extrusion mesh
     */
    static MeshParameters extruded(double layerThickness = 1.0, int numLayers = 10) {
        MeshParameters params = medium(layerThickness * numLayers / 10.0);
        params.autoDetectExtrusion = true;
        params.extrusionLayers = numLayers;
        params.extrusionLayerThickness = layerThickness;
        params.allowQuads = true;
        params.allowHexes = true;
        params.recombineAll = true;
        params.numOptimizationPasses = 1;
        return params;
    }
};

} // namespace koo::mesh
