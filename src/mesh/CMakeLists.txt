# Mesh Module CMakeLists.txt
# Phase 5b: Gmsh Integration

# Mesh module sources
# Note: Many classes are header-only with inline implementations
set(KOO_MESH_SOURCES
    IMeshGenerator.cpp
)

# Gmsh implementation (conditional)
if(WITH_GMSH AND Gmsh_FOUND)
    list(APPEND KOO_MESH_SOURCES
        GmshMeshGenerator.cpp
    )
endif()

# Create Mesh library target
if(KOO_MESH_SOURCES)
    add_library(koo_mesh STATIC ${KOO_MESH_SOURCES})
    target_compile_definitions(koo_mesh PUBLIC KOO_SIM_STATIC)

    # Link dependencies
    target_link_libraries(koo_mesh PUBLIC koo_cad)

    # Link Gmsh if available
    if(WITH_GMSH AND Gmsh_FOUND)
        target_link_libraries(koo_mesh PUBLIC Gmsh::Gmsh)
        message(STATUS "Mesh module: Gmsh support enabled")
    else()
        message(STATUS "Mesh module: Header-only (no Gmsh)")
    endif()

    set_project_warnings(koo_mesh)
    set_target_properties(koo_mesh PROPERTIES OUTPUT_NAME koo_mesh)
else()
    # Header-only mode
    add_library(koo_mesh INTERFACE)
    target_link_libraries(koo_mesh INTERFACE koo_cad)
    message(STATUS "Mesh module: INTERFACE library (header-only mode)")
endif()
