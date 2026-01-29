# Mesh Generation Overview

This document provides an overview of the mesh generation capabilities in the KooSimulationDataReader library.

## Features

The library provides comprehensive CAD to LS-DYNA mesh generation capabilities:

### 1. **Batch Processing** ([BATCH_MESHING_GUIDE.md](BATCH_MESHING_GUIDE.md))

Process multiple CAD files automatically:
- **Directory Import**: Import all STEP/IGES files from a folder
- **Per-File Material Mapping**: Assign different materials to different files
- **Automatic Part Creation**: Each file becomes a separate LS-DYNA part
- **Error Handling**: Continues on failure, reports problematic files

**Example:**
```cpp
auto partIds = geomMgr.importDirectoryAndMesh(
    "./cad_models",
    mesh::MeshParameters::medium(5.0),
    1, 1  // Material ID, Section ID
);
```

### 2. **Structured Meshing** ([STRUCTURED_MESH_GUIDE.md](STRUCTURED_MESH_GUIDE.md))

Generate hexahedral (brick) elements for better accuracy:

#### Transfinite Mesh (Pure Hexahedral)
```cpp
auto params = mesh::MeshParameters::structured(
    5.0,   // Element size
    10     // Divisions per edge
);
```
- Best for simple geometries (boxes, cylinders)
- Highest accuracy and lowest computational cost
- Uniform structured mesh

#### Extrusion-Based Mesh
```cpp
auto params = mesh::MeshParameters::extruded(
    1.0,   // Layer thickness
    10     // Number of layers
);
```
- Automatic detection of extrudable surfaces
- Layer-by-layer hexahedral element generation
- Ideal for prismatic geometries (plates, shells)

#### Hex-Dominant Mesh
```cpp
auto params = mesh::MeshParameters::hexDominant(5.0);
```
- Automatic hex/tet element mixing
- Maximizes hexahedral coverage
- Works with complex geometries

### 3. **Unstructured Meshing**

Traditional tetrahedral mesh generation:
```cpp
auto params = mesh::MeshParameters::medium(5.0);
// or
auto params = mesh::MeshParameters::fine(2.0);
```
- Fully automatic
- Handles any geometry complexity
- Quality presets: coarse, medium, fine

## Mesh Quality Comparison

| Element Type | Accuracy | Speed | Memory | Use Case |
|--------------|----------|-------|--------|----------|
| **Hex8** (Structured) | Excellent | Fast | Low | Simple geometries |
| **Hex8** (Extrusion) | Excellent | Fast | Low | Prismatic parts |
| **Hex8** (Dominant) | Very Good | Medium | Medium | Complex parts |
| **Tet4** (Unstructured) | Good | Slower | Higher | Any geometry |

**Element Count Ratio**: Tetrahedral meshes typically require 5-6x more elements than hexahedral meshes for similar accuracy.

## Supported File Formats

- **STEP**: `.step`, `.stp`, `.STEP`, `.STP`
- **IGES**: `.iges`, `.igs`, `.IGES`, `.IGS`

## Complete Workflow

```cpp
#include <koo/dyna/Model.hpp>
#include <koo/dyna/managers/ModelManager.hpp>
#include <koo/dyna/managers/GeometryManager.hpp>
#include <koo/dyna/KeywordFileWriter.hpp>
#include <koo/mesh/MeshParameters.hpp>

using namespace koo;

int main() {
    // 1. Create model and managers
    dyna::Model model;
    dyna::managers::ModelManager modelMgr(model);
    dyna::managers::GeometryManager geomMgr(model, modelMgr);

    // 2. Choose meshing strategy
    mesh::MeshParameters params;

    // Option A: Structured mesh (best accuracy)
    params = mesh::MeshParameters::structured(5.0, 10);

    // Option B: Extrusion-based (for extrudable parts)
    params = mesh::MeshParameters::extruded(1.0, 10);

    // Option C: Hex-dominant (automatic, robust)
    params = mesh::MeshParameters::hexDominant(5.0);

    // Option D: Tetrahedral (fully automatic)
    params = mesh::MeshParameters::medium(5.0);

    // 3. Import and mesh (single file)
    auto partId = geomMgr.importAndMeshCAD(
        "part.step",
        params,
        1,  // Material ID
        1   // Section ID
    );

    // OR: Batch process directory
    auto partIds = geomMgr.importDirectoryAndMesh(
        "./cad_models",
        params,
        1, 1
    );

    // 4. Write LS-DYNA keyword file
    dyna::KeywordFileWriter writer;
    writer.write(model, "output.k");

    return 0;
}
```

## Examples

### Command-Line Tools

```bash
# Build examples
cd build
cmake .. -DWITH_OPENCASCADE=ON -DWITH_GMSH=ON -DBUILD_EXAMPLES=ON
make -j4

# Single file mesh generation
./bin/examples/mesh_generation part.step 5.0

# Batch processing
./bin/examples/batch_mesh_generation ./cad_models output.k

# Batch with material mapping
./bin/examples/batch_mesh_with_materials

# Structured mesh generation
./bin/examples/structured_mesh_generation box.step output.k 1  # Transfinite
./bin/examples/structured_mesh_generation plate.step output.k 2  # Extrusion
./bin/examples/structured_mesh_generation part.step output.k 3  # Hex-dominant
```

## Decision Guide

### When to Use Structured Mesh

**Use structured/transfinite mesh when:**
- Geometry is simple (boxes, cylinders)
- Very high accuracy is required
- Computational efficiency is critical
- Geometry has regular topology

**Use extrusion-based mesh when:**
- Geometry is extrudable (2D profile → 3D solid)
- Consistent layer thickness is important
- Prismatic parts (plates, shells, beams)

**Use hex-dominant mesh when:**
- Geometry has complex features
- Want best accuracy with automation
- Mix of simple and complex regions
- Automatic hex coverage is acceptable

### When to Use Unstructured Mesh

**Use tetrahedral mesh when:**
- Geometry is very complex
- Quick meshing is priority
- Automatic workflow is essential
- Many small features present

## Mesh Parameters Reference

### Element Sizing

```cpp
params.globalElementSize = 5.0;    // Target element size
params.minElementSize = 1.0;       // Minimum allowed size
params.maxElementSize = 10.0;      // Maximum allowed size
```

### Structured Options

```cpp
params.useStructuredMesh = true;
params.useTransfinite = true;
params.transfiniteDivisions = 10;  // Divisions per edge

params.autoDetectExtrusion = true;
params.extrusionLayers = 10;
params.extrusionLayerThickness = 1.0;
```

### Quality Control

```cpp
params.numOptimizationPasses = 3;  // Optimization iterations
params.optimizeNetgen = true;      // Use Netgen optimizer
params.minElementQuality = 0.3;    // Quality threshold (0-1)
```

### Element Type Control

```cpp
params.allowQuads = true;          // Allow quad surface elements
params.allowHexes = true;          // Allow hex volume elements
params.recombineAll = true;        // Recombine triangles → quads
params.elementOrder = mesh::ElementOrder::Linear;  // or Quadratic
```

## Performance Guidelines

### Memory Usage Estimation

```
Hexahedral (Hex8):  ~500 bytes per element
Tetrahedral (Tet4): ~250 bytes per element

Example:
- 1M Hex8 elements  ≈ 500 MB
- 1M Tet4 elements  ≈ 250 MB
```

### Element Count Estimation

```
Box dimensions: L × W × H
Transfinite divisions: N
Element count: N³

Example: 100mm × 100mm × 100mm, N=10
Result: 1,000 Hex8 elements
```

### Optimization Settings

```cpp
// Fast meshing (prototyping)
params.numOptimizationPasses = 0;

// Balanced (typical)
params.numOptimizationPasses = 3;

// High quality (final analysis)
params.numOptimizationPasses = 5;
```

## Troubleshooting

### Common Issues

1. **"No CAD files found in directory"**
   - Check directory path
   - Verify file extensions (.step, .stp, .iges, .igs)

2. **"Transfinite mesh setup warning"**
   - Geometry too complex for structured mesh
   - Use hex-dominant preset instead
   - Simplify geometry (remove small features)

3. **"Extrusion attempt failed"**
   - Geometry not extrudable
   - Try structured or hex-dominant instead
   - Check base surface planarity

4. **Low hex ratio (< 50%)**
   - Increase transfiniteDivisions
   - Use smaller element size
   - Accept mixed mesh (still beneficial)

5. **Out of memory**
   - Increase element size
   - Reduce transfiniteDivisions
   - Process in batches

## Advanced Topics

### Entity Traceability

The mesh generation pipeline preserves CAD entity IDs for boundary conditions:

```
STEP File (Face ID: 5)
    ↓
OpenCASCADE (GeometryEntity ID: 5)
    ↓
Gmsh Physical Group (tag: 5)
    ↓
MeshData (geometryEntityId: 5)
    ↓
LS-DYNA SetSegment (for Face 5)
```

This enables:
- Boundary condition assignment to specific CAD faces
- Tied contact between CAD surfaces
- Load application to CAD edges/vertices

### Custom Algorithms

```cpp
// Set specific meshing algorithm
geomMgr.setMeshAlgorithm(mesh::MeshAlgorithm::Delaunay);
// or
geomMgr.setMeshAlgorithm(mesh::MeshAlgorithm::Frontal);
```

### Parallel Processing

```cpp
// Use multiple CPU cores for meshing
geomMgr.setNumCores(4);
```

## Dependencies

- **OpenCASCADE 7.7.0+**: CAD file import
- **Gmsh 4.x**: Mesh generation
- **CMake 3.12+**: Build system
- **C++17**: Language standard

## Build Configuration

```bash
# Full features
cmake .. -DWITH_OPENCASCADE=ON -DWITH_GMSH=ON -DBUILD_EXAMPLES=ON

# Gmsh auto-download
# If Gmsh not found locally, CMake will automatically download Gmsh SDK

# OpenCASCADE version detection
# Automatically detects and uses TKDESTEP (7.9+) or TKSTEP (7.7)
```

## References

- [BATCH_MESHING_GUIDE.md](BATCH_MESHING_GUIDE.md) - Batch processing details
- [STRUCTURED_MESH_GUIDE.md](STRUCTURED_MESH_GUIDE.md) - Structured meshing reference
- [Phase 5e Implementation Plan](../plan/phase5e_implementation.md) - Technical design
- [Gmsh Documentation](https://gmsh.info/doc/texinfo/gmsh.html) - Gmsh API reference
- [LS-DYNA User Manual](https://www.dynasupport.com/) - LS-DYNA element types

## Examples Source Code

All examples are located in `examples/`:
- `batch_mesh_generation.cpp` - Directory-based batch processing
- `batch_mesh_with_materials.cpp` - Per-file material mapping
- `structured_mesh_generation.cpp` - Structured/hexahedral meshing
- `mesh_generation.cpp` - Single file mesh generation
- `geometry_workflow.cpp` - Complete CAD → LS-DYNA pipeline

## Next Steps

1. **Try the examples**: Build and run example programs
2. **Read the guides**: Review batch and structured mesh guides
3. **Prepare CAD files**: Organize STEP/IGES files in directories
4. **Generate meshes**: Run batch meshing on your CAD library
5. **Visualize results**: Open .k files in LS-PrePost
6. **Define physics**: Add materials, boundary conditions, loads
7. **Run simulations**: Execute LS-DYNA analysis

## Support

For issues or questions:
- Check [TROUBLESHOOTING.md](TROUBLESHOOTING.md)
- Review example source code
- Consult API documentation in header files
- Report issues on GitHub
