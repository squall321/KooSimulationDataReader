# Batch Mesh Generation Guide

This guide explains how to generate LS-DYNA meshes from multiple CAD files (STEP/IGES) in batch mode.

## Overview

The `GeometryManager` class provides three methods for batch processing:

1. **`importDirectoryAndMesh()`**: Import all CAD files from a directory
2. **`importBatchAndMesh()`**: Import specific files with per-file material mapping
3. **`importAndMeshCAD()`**: Import single file (manual loop for batch)

## Quick Start

### 1. Import All Files from Directory

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

// Define mesh parameters
mesh::MeshParameters params = mesh::MeshParameters::medium(5.0);

// Import all STEP/IGES files from directory
std::vector<dyna::PartId> partIds = geomMgr.importDirectoryAndMesh(
    "./cad_models",  // Directory path
    params,          // Mesh parameters
    1,               // Material ID (applied to all parts)
    1                // Section ID (applied to all parts)
);
```

### 2. Import with Material Mapping

```cpp
// Define per-file material/section mapping
std::map<std::string, std::pair<int, int>> mapping = {
    {"part_steel.step",    {1, 1}},  // Material 1, Section 1
    {"part_aluminum.step", {2, 2}},  // Material 2, Section 2
    {"part_rubber.step",   {3, 3}},  // Material 3, Section 3
};

// Import with mapping
std::vector<dyna::PartId> partIds = geomMgr.importBatchAndMesh(
    mapping,
    params
);
```

## API Reference

### `importDirectoryAndMesh()`

Import all CAD files from a directory with the same material/section.

**Signature:**
```cpp
std::vector<PartId> importDirectoryAndMesh(
    const std::string& directoryPath,
    const mesh::MeshParameters& meshParams,
    int materialId,
    int sectionId,
    const std::string& filePattern = ""
);
```

**Parameters:**
- `directoryPath`: Path to directory containing CAD files
- `meshParams`: Mesh generation parameters (element size, quality, etc.)
- `materialId`: Material ID to assign to all parts
- `sectionId`: Section ID to assign to all parts
- `filePattern`: (Optional) File extension filter (e.g., ".step")
  - Default: empty (imports all .step, .stp, .iges, .igs files)

**Returns:**
- Vector of part IDs (one per successfully imported file)
- Empty vector on failure (check `getLastError()`)

**Example:**
```cpp
// Import only .step files
auto partIds = geomMgr.importDirectoryAndMesh(
    "./parts",
    params,
    1, 1,
    ".step"  // Only import .step files
);
```

### `importBatchAndMesh()`

Import multiple files with individual material/section assignments.

**Signature:**
```cpp
std::vector<PartId> importBatchAndMesh(
    const std::map<std::string, std::pair<int, int>>& fileToMatSection,
    const mesh::MeshParameters& meshParams
);
```

**Parameters:**
- `fileToMatSection`: Map of filepath → (materialId, sectionId)
- `meshParams`: Mesh generation parameters

**Returns:**
- Vector of part IDs (one per file)
- Empty vector on failure

**Example:**
```cpp
std::map<std::string, std::pair<int, int>> mapping = {
    {"./parts/part1.step", {1, 1}},
    {"./parts/part2.step", {2, 2}},
};

auto partIds = geomMgr.importBatchAndMesh(mapping, params);
```

## Mesh Parameters

### Predefined Quality Presets

```cpp
// Coarse mesh (fast, lower quality)
auto params = mesh::MeshParameters::coarse(10.0);  // element size: 10.0

// Medium mesh (balanced)
auto params = mesh::MeshParameters::medium(5.0);   // element size: 5.0

// Fine mesh (slow, high quality)
auto params = mesh::MeshParameters::fine(2.0);     // element size: 2.0
```

### Custom Parameters

```cpp
mesh::MeshParameters params;
params.minElementSize = 1.0;
params.maxElementSize = 10.0;
params.elementOrder = mesh::ElementOrder::Linear;  // or Quadratic
params.numOptimizationPasses = 3;
params.optimizeNetgen = true;
params.recombineAll = false;
```

## Supported File Formats

- **STEP**: `.step`, `.stp`, `.STEP`, `.STP`
- **IGES**: `.iges`, `.igs`, `.IGES`, `.IGS`

## Complete Workflow Example

```cpp
#include <koo/dyna/Model.hpp>
#include <koo/dyna/managers/ModelManager.hpp>
#include <koo/dyna/managers/GeometryManager.hpp>
#include <koo/dyna/KeywordFileWriter.hpp>
#include <koo/mesh/MeshParameters.hpp>

int main() {
    // 1. Create model and managers
    koo::dyna::Model model;
    koo::dyna::managers::ModelManager modelMgr(model);
    koo::dyna::managers::GeometryManager geomMgr(model, modelMgr);

    // 2. Set mesh parameters
    auto params = koo::mesh::MeshParameters::medium(5.0);

    // 3. Batch import CAD files
    auto partIds = geomMgr.importDirectoryAndMesh(
        "./cad_models",
        params,
        1,  // Material ID
        1   // Section ID
    );

    // 4. Check results
    if (partIds.empty()) {
        std::cerr << "ERROR: " << geomMgr.getLastError() << std::endl;
        return 1;
    }

    std::cout << "Created " << partIds.size() << " parts" << std::endl;
    std::cout << "Total nodes: " << model.getNodeCount() << std::endl;
    std::cout << "Total elements: "
              << (model.getShellElementCount() + model.getSolidElementCount())
              << std::endl;

    // 5. Write LS-DYNA keyword file
    koo::dyna::KeywordFileWriter writer;
    writer.write(model, "output.k");

    return 0;
}
```

## Error Handling

```cpp
auto partIds = geomMgr.importDirectoryAndMesh("./cad", params, 1, 1);

if (partIds.empty()) {
    std::cerr << "Batch import failed!" << std::endl;
    std::cerr << "Error: " << geomMgr.getLastError() << std::endl;
    // Last error contains details about which file failed
}

// Partial success is possible - some files may succeed while others fail
if (partIds.size() < expectedCount) {
    std::cerr << "Warning: Only " << partIds.size()
              << " of " << expectedCount << " files imported" << std::endl;
    std::cerr << "Last error: " << geomMgr.getLastError() << std::endl;
}
```

## Performance Tips

1. **Element Size**: Larger elements = faster meshing
   - Coarse (10.0): Very fast, suitable for prototyping
   - Medium (5.0): Good balance for most cases
   - Fine (2.0): High quality, but slow for large models

2. **Optimization**: Disable for faster meshing
   ```cpp
   params.numOptimizationPasses = 0;  // Skip optimization
   ```

3. **Parallel Processing**: Import files in parallel (future feature)

4. **Memory**: Large assemblies may require significant RAM
   - Monitor memory usage with large batches
   - Consider processing in smaller batches if needed

## Directory Structure Example

```
project/
├── cad_models/
│   ├── part1.step
│   ├── part2.step
│   ├── part3.stp
│   └── assembly.iges
├── output/
│   └── mesh_output.k
└── my_meshing_app.cpp
```

## Command Line Tools

### Build Examples

```bash
cd build
cmake .. -DWITH_OPENCASCADE=ON -DWITH_GMSH=ON
make -j4
```

### Run Batch Meshing

```bash
# Run batch mesh generation example
./bin/batch_mesh_generation ./cad_models output.k

# Run multi-material example
./bin/batch_mesh_with_materials
```

## Troubleshooting

### "No CAD files found in directory"
- Check directory path is correct
- Ensure files have supported extensions (.step, .stp, .iges, .igs)
- Verify file permissions

### "Failed to import: filename.step"
- Check if file is corrupted
- Verify OpenCASCADE is properly installed
- Try opening file in CAD software first

### "Mesh generation failed"
- Try increasing element size
- Check geometry for defects (gaps, overlaps)
- Enable shape healing: `cadImporter->setShapeHealing(true)`

### "Out of memory"
- Reduce number of files per batch
- Increase element size
- Process files sequentially instead of loading all at once

## Next Steps

- See [examples/batch_mesh_generation.cpp](../examples/batch_mesh_generation.cpp)
- See [examples/batch_mesh_with_materials.cpp](../examples/batch_mesh_with_materials.cpp)
- Read [Phase 5e Implementation Plan](../plan/phase5e_implementation.md)
