# Structured Mesh Generation Guide

This guide explains how to generate structured hexahedral meshes from CAD files for LS-DYNA simulations.

## Overview

Structured meshes use hexahedral (brick) elements which provide:
- **Better accuracy** for linear problems compared to tetrahedral meshes
- **Lower computational cost** (fewer elements needed for same accuracy)
- **Improved convergence** in implicit solvers
- **Better stress gradients** in structural analysis

The library provides three approaches:
1. **Transfinite meshing**: Structured mesh with uniform divisions
2. **Extrusion-based meshing**: Layer-by-layer hex generation
3. **Hex-dominant meshing**: Automatic hex/tet mixing for complex geometries

## Quick Start

### 1. Structured (Transfinite) Mesh

```cpp
#include <koo/dyna/Model.hpp>
#include <koo/dyna/managers/ModelManager.hpp>
#include <koo/dyna/managers/GeometryManager.hpp>
#include <koo/mesh/MeshParameters.hpp>

using namespace koo;

// Create model and managers
dyna::Model model;
dyna::managers::ModelManager modelMgr(model);
dyna::managers::GeometryManager geomMgr(model, modelMgr);

// Define structured mesh parameters
mesh::MeshParameters params = mesh::MeshParameters::structured(
    5.0,   // Element size
    10     // Divisions per edge
);

// Import and mesh
dyna::PartId partId = geomMgr.importAndMeshCAD(
    "box.step",
    params,
    1, 1,
    "StructuredPart"
);
```

### 2. Extrusion-Based Mesh

```cpp
// Define extrusion parameters
mesh::MeshParameters params = mesh::MeshParameters::extruded(
    1.0,   // Layer thickness
    10     // Number of layers
);

// Import and mesh
dyna::PartId partId = geomMgr.importAndMeshCAD(
    "plate.step",
    params,
    1, 1
);
```

### 3. Hex-Dominant Mesh (Automatic)

```cpp
// Let the mesher automatically choose hex/tet elements
mesh::MeshParameters params = mesh::MeshParameters::hexDominant(5.0);

dyna::PartId partId = geomMgr.importAndMeshCAD(
    "complex_part.step",
    params,
    1, 1
);
```

## Mesh Parameters Reference

### Structured Mesh Preset

**Signature:**
```cpp
static MeshParameters structured(double size = 5.0, int divisions = 10);
```

**Parameters:**
- `size`: Global element size (approximate)
- `divisions`: Number of divisions per edge for transfinite mesh

**Features:**
- Uses transfinite meshing algorithm
- Generates pure hexahedral elements (Hex8)
- Suitable for simple geometries (boxes, cylinders, etc.)
- Best accuracy and performance

**Example:**
```cpp
// Fine structured mesh with 20 divisions per edge
auto params = mesh::MeshParameters::structured(2.0, 20);
```

### Extrusion Mesh Preset

**Signature:**
```cpp
static MeshParameters extruded(double layerThickness = 1.0, int numLayers = 10);
```

**Parameters:**
- `layerThickness`: Thickness of each layer
- `numLayers`: Number of layers to extrude

**Features:**
- Automatically detects extrudable surfaces
- Generates hexahedral elements through extrusion
- Suitable for prismatic geometries
- Controlled layer thickness

**Example:**
```cpp
// Extrude 15 layers of 0.5mm each
auto params = mesh::MeshParameters::extruded(0.5, 15);
```

### Hex-Dominant Preset

**Signature:**
```cpp
static MeshParameters hexDominant(double size = 5.0);
```

**Parameters:**
- `size`: Global element size

**Features:**
- Automatically mixes hexahedral and tetrahedral elements
- Uses recombination algorithm to maximize hex elements
- Suitable for complex geometries
- Fallback for geometries that cannot be fully structured

**Example:**
```cpp
// Hex-dominant mesh with 3mm elements
auto params = mesh::MeshParameters::hexDominant(3.0);
```

## Advanced Configuration

### Manual Parameter Setup

```cpp
mesh::MeshParameters params;

// Basic sizing
params.globalElementSize = 5.0;
params.minElementSize = 1.0;
params.maxElementSize = 10.0;

// Structured mesh options
params.useStructuredMesh = true;
params.useTransfinite = true;
params.transfiniteDivisions = 15;

// Element type control
params.allowHexes = true;
params.allowQuads = true;
params.recombineAll = true;

// Extrusion options
params.autoDetectExtrusion = true;
params.extrusionLayers = 10;
params.extrusionLayerThickness = 1.0;

// Quality control
params.numOptimizationPasses = 1;  // Less optimization for structured
params.elementOrder = mesh::ElementOrder::Linear;
```

## Complete Workflow Example

```cpp
#include <koo/dyna/Model.hpp>
#include <koo/dyna/managers/ModelManager.hpp>
#include <koo/dyna/managers/GeometryManager.hpp>
#include <koo/dyna/KeywordFileWriter.hpp>
#include <koo/mesh/MeshParameters.hpp>
#include <iostream>

int main() {
    // 1. Create model and managers
    koo::dyna::Model model;
    koo::dyna::managers::ModelManager modelMgr(model);
    koo::dyna::managers::GeometryManager geomMgr(model, modelMgr);

    // 2. Define structured mesh parameters
    auto params = koo::mesh::MeshParameters::structured(5.0, 10);

    // 3. Import CAD and generate structured mesh
    auto partId = geomMgr.importAndMeshCAD(
        "box.step",
        params,
        1,  // Material ID
        1,  // Section ID
        "StructuredBox"
    );

    // 4. Check results
    if (partId < 0) {
        std::cerr << "ERROR: " << geomMgr.getLastError() << std::endl;
        return 1;
    }

    std::cout << "Created part ID: " << partId << std::endl;
    std::cout << "Nodes: " << model.getNodeCount() << std::endl;
    std::cout << "Solid elements: " << model.getSolidElementCount() << std::endl;

    // 5. Analyze element types
    const auto* solids = model.getSolidElements();
    int hexCount = 0, tetCount = 0;

    for (const auto& elem : solids->getElements()) {
        if (elem.nodeIds.size() == 8) hexCount++;
        else if (elem.nodeIds.size() == 4) tetCount++;
    }

    std::cout << "Hex elements: " << hexCount << std::endl;
    std::cout << "Tet elements: " << tetCount << std::endl;
    std::cout << "Hex ratio: " << (100.0 * hexCount / (hexCount + tetCount)) << "%" << std::endl;

    // 6. Write LS-DYNA keyword file
    koo::dyna::KeywordFileWriter writer;
    writer.write(model, "structured_mesh.k");

    return 0;
}
```

## Mesh Quality Comparison

### Tetrahedral vs Hexahedral

| Aspect | Tetrahedral (Tet4) | Hexahedral (Hex8) |
|--------|-------------------|-------------------|
| Accuracy | Moderate | High |
| Elements needed | More (5-6x) | Fewer |
| Computational cost | Higher | Lower |
| Memory usage | Higher | Lower |
| Mesh generation | Automatic, robust | Requires structure |
| Complex geometries | Easy | Difficult |

### When to Use Each Type

**Use Structured/Hex Mesh When:**
- Geometry is simple (boxes, cylinders, plates)
- High accuracy is required
- Computational efficiency is important
- Geometry can be decomposed into structured blocks
- Extrusion is possible (2D → 3D)

**Use Tetrahedral Mesh When:**
- Geometry is complex (organic shapes, assemblies)
- Quick meshing is priority
- Geometry has many small features
- Automated workflow is required

**Use Hex-Dominant When:**
- Compromise between accuracy and automation
- Some structured regions, some complex regions
- Want maximum hex coverage with automatic fallback

## Element Size Guidelines

### For Structural Analysis

```cpp
// Coarse mesh (initial analysis)
auto params = mesh::MeshParameters::structured(10.0, 5);

// Medium mesh (standard analysis)
auto params = mesh::MeshParameters::structured(5.0, 10);

// Fine mesh (detailed analysis)
auto params = mesh::MeshParameters::structured(2.0, 20);

// Very fine mesh (high-precision)
auto params = mesh::MeshParameters::structured(1.0, 40);
```

### For Impact/Crash Analysis

```cpp
// Impact zone: Fine mesh
auto impactParams = mesh::MeshParameters::structured(2.0, 15);

// Far field: Coarse mesh
auto farFieldParams = mesh::MeshParameters::structured(10.0, 5);
```

## Supported Geometries

### Best Results (Pure Hex)

- **Boxes**: Rectangular solids
- **Cylinders**: Circular extrusions
- **Plates**: Thin structures with extrusion
- **Simple assemblies**: Composed of above

### Good Results (Hex-Dominant)

- **Fillets**: Rounded edges
- **Chamfers**: Beveled edges
- **Simple curves**: Moderate curvature
- **Symmetric parts**: Repetitive structures

### Limited Results (Mixed)

- **Organic shapes**: Complex curvature
- **Small features**: Holes, slots
- **Thin shells**: Complex surfaces
- **Irregular assemblies**: Non-aligned parts

## Command Line Tools

### Build Example

```bash
cd build
cmake .. -DWITH_OPENCASCADE=ON -DWITH_GMSH=ON
make -j4
```

### Run Structured Meshing

```bash
# Structured (transfinite) mesh
./bin/structured_mesh_generation box.step output.k 1

# Extrusion-based mesh
./bin/structured_mesh_generation plate.step output.k 2

# Hex-dominant (automatic)
./bin/structured_mesh_generation part.step output.k 3
```

## Troubleshooting

### "Transfinite mesh setup warning"

**Cause**: Geometry is too complex for structured meshing.

**Solutions**:
1. Simplify geometry (remove small features)
2. Use hex-dominant preset instead
3. Decompose geometry into simpler parts
4. Check for surface defects (gaps, overlaps)

### "Extrusion attempt failed"

**Cause**: Geometry is not extrudable.

**Solutions**:
1. Verify geometry has clear extrusion direction
2. Check base surface is planar or developable
3. Use structured or hex-dominant instead
4. Manually define extrusion path

### Low hex ratio (< 50%)

**Cause**: Geometry features prevent hexahedral meshing.

**Solutions**:
1. Increase `transfiniteDivisions` parameter
2. Use smaller element size
3. Remove fillets and chamfers (if acceptable)
4. Accept mixed mesh (hex-dominant is still beneficial)

### Out of memory

**Cause**: Too many elements for available RAM.

**Solutions**:
1. Increase element size
2. Reduce `transfiniteDivisions`
3. Reduce `extrusionLayers`
4. Process geometry in parts

### Poor element quality

**Cause**: High aspect ratio or distorted elements.

**Solutions**:
1. Adjust `extrusionLayerThickness` for uniform aspect ratio
2. Use `numOptimizationPasses` (1-3 for structured)
3. Increase `transfiniteDivisions` for smoother transitions
4. Check geometry quality (use shape healing)

## Performance Tips

### 1. Element Count Estimation

```
Box dimensions: L × W × H
Divisions per edge: N
Element count: N³ for structured mesh

Example: 100mm × 100mm × 100mm box, N=10
Elements: 10³ = 1000 Hex8 elements
```

### 2. Optimization Settings

```cpp
// Structured mesh: Minimal optimization
params.numOptimizationPasses = 1;

// Hex-dominant: Moderate optimization
params.numOptimizationPasses = 3;

// Complex mixed mesh: Heavy optimization
params.numOptimizationPasses = 5;
```

### 3. Memory Usage

```
Approximate memory per element:
- Hex8:  ~500 bytes (8 nodes × ~60 bytes)
- Tet4:  ~250 bytes (4 nodes × ~60 bytes)

1 million Hex8 elements ≈ 500 MB
```

## Next Steps

- See [examples/structured_mesh_generation.cpp](../examples/structured_mesh_generation.cpp)
- See [BATCH_MESHING_GUIDE.md](BATCH_MESHING_GUIDE.md) for batch processing
- Read [Gmsh documentation](https://gmsh.info/doc/texinfo/gmsh.html) for advanced options

## References

- **Gmsh Transfinite Meshing**: [Gmsh Structured Mesh Tutorial](https://gmsh.info/doc/texinfo/gmsh.html#Structured-grids)
- **LS-DYNA Element Types**: LS-DYNA User Manual, Section 27
- **Hex Meshing Theory**: "The Whisker Weaving Algorithm" (Tautges et al., 1996)
