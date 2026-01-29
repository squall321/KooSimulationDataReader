# Examples CMakeLists.txt

# Helper function to add example executable
function(add_koo_example EXAMPLE_NAME SOURCE_FILE)
    add_executable(${EXAMPLE_NAME} ${SOURCE_FILE})

    # Link to main library
    if(BUILD_SHARED_LIBS)
        target_link_libraries(${EXAMPLE_NAME} PRIVATE koo_sim)
    else()
        target_link_libraries(${EXAMPLE_NAME} PRIVATE koo_sim_static)
    endif()

    # Set output directory
    set_target_properties(${EXAMPLE_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/examples
    )
endfunction()

# ============================================================================
# Manager Examples (Phase 1-4)
# ============================================================================
add_koo_example(manager_basic_usage manager_basic_usage.cpp)
add_koo_example(manager_unified_api manager_unified_api.cpp)

# ============================================================================
# Phase 5 Examples (CAD Integration)
# ============================================================================

# Phase 5a: CAD Import Example
if(BUILD_CAD_MODULE)
    add_koo_example(cad_import_step cad_import_step.cpp)

    if(WITH_OPENCASCADE AND OpenCASCADE_FOUND)
        message(STATUS "Example: cad_import_step (with OpenCASCADE support)")
    else()
        message(STATUS "Example: cad_import_step (stub implementation)")
    endif()
endif()

# Phase 5b: Mesh Generation Example
if(BUILD_CAD_MODULE AND BUILD_MESH_MODULE)
    add_koo_example(mesh_generation mesh_generation.cpp)

    if(WITH_OPENCASCADE AND OpenCASCADE_FOUND AND WITH_GMSH AND Gmsh_FOUND)
        message(STATUS "Example: mesh_generation (with OpenCASCADE + Gmsh support)")
    else()
        message(STATUS "Example: mesh_generation (stub implementation)")
    endif()
endif()

# Phase 5c: Complete Geometry Workflow Example
if(BUILD_CAD_MODULE AND BUILD_MESH_MODULE)
    add_koo_example(geometry_workflow geometry_workflow.cpp)

    if(WITH_OPENCASCADE AND OpenCASCADE_FOUND AND WITH_GMSH AND Gmsh_FOUND)
        message(STATUS "Example: geometry_workflow (full CAD → Mesh → LS-DYNA pipeline)")
    else()
        message(STATUS "Example: geometry_workflow (stub implementation)")
    endif()
endif()

# Phase 5e: Batch Mesh Generation Examples
if(BUILD_CAD_MODULE AND BUILD_MESH_MODULE)
    add_koo_example(batch_mesh_generation batch_mesh_generation.cpp)
    add_koo_example(batch_mesh_with_materials batch_mesh_with_materials.cpp)

    if(WITH_OPENCASCADE AND OpenCASCADE_FOUND AND WITH_GMSH AND Gmsh_FOUND)
        message(STATUS "Example: batch_mesh_generation (directory-based meshing)")
        message(STATUS "Example: batch_mesh_with_materials (per-file material mapping)")
    else()
        message(STATUS "Example: batch meshing (stub implementation)")
    endif()
endif()

# Phase 5e: Structured Mesh Generation Example
if(BUILD_CAD_MODULE AND BUILD_MESH_MODULE)
    add_koo_example(structured_mesh_generation structured_mesh_generation.cpp)

    if(WITH_OPENCASCADE AND OpenCASCADE_FOUND AND WITH_GMSH AND Gmsh_FOUND)
        message(STATUS "Example: structured_mesh_generation (hexahedral/extrusion meshing)")
    else()
        message(STATUS "Example: structured_mesh_generation (stub implementation)")
    endif()
endif()

# ============================================================================
# ECAD Examples (if ECAD module is built)
# ============================================================================
if(BUILD_ECAD_MODULE)
    # ECAD examples will be added later
    # add_subdirectory(ECAD)
endif()
