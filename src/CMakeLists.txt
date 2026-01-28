# Build options for individual modules
option(BUILD_DYNA_MODULE "Build LS-DYNA module" ON)
option(BUILD_ECAD_MODULE "Build ECAD/ODB++ module" ON)

# Source files - LS-DYNA module
set(KOO_DYNA_SOURCES
    dyna/Keyword.cpp
    dyna/Node.cpp
    dyna/Element.cpp
    dyna/Part.cpp
    dyna/Material.cpp
    dyna/Section.cpp
    dyna/Control.cpp
    dyna/Database.cpp
    dyna/Boundary.cpp
    dyna/Load.cpp
    dyna/Define.cpp
    dyna/Initial.cpp
    dyna/Include.cpp
    dyna/Set.cpp
    dyna/Contact.cpp
    dyna/Constrained.cpp
    dyna/Rigidwall.cpp
    dyna/Damping.cpp
    dyna/Airbag.cpp
    dyna/Hourglass.cpp
    dyna/Frequency.cpp
    dyna/Ale.cpp
    dyna/Eos.cpp
    dyna/MatAdd.cpp
    dyna/Em.cpp
    dyna/Icfd.cpp
    dyna/Sensor.cpp
    dyna/Integration.cpp
    dyna/Cese.cpp
    dyna/DeformableToRigid.cpp
    dyna/Parameter.cpp
    dyna/Chemistry.cpp
    dyna/Interface.cpp
    dyna/Sph.cpp
    dyna/Dualcese.cpp
    dyna/Thermal.cpp
    dyna/Implicit.cpp
    dyna/Perturbation.cpp
    dyna/Stochastic.cpp
    dyna/Model.cpp
    dyna/KeywordFileReader.cpp
    dyna/KeywordFileWriter.cpp
    dyna/KeywordFactory.cpp
    dyna/StatisticsVisitor.cpp
    dyna/ValidationVisitor.cpp
    dyna/managers/PartManager.cpp
    dyna/managers/ElementManager.cpp
    dyna/managers/NodeManager.cpp
    dyna/managers/SetManager.cpp
    dyna/managers/ContactManager.cpp
    dyna/managers/LoadManager.cpp
    dyna/managers/ModelManager.cpp
    dyna/managers/GeometryManager.cpp
)

# Source files - ECAD module (ODB++ support)
set(KOO_ECAD_SOURCES
    ecad/Feature.cpp
    ecad/Symbol.cpp
    ecad/Layer.cpp
    ecad/EdaData.cpp
    ecad/Step.cpp
    ecad/OdbJob.cpp
    ecad/OdbReader.cpp
    ecad/OdbWriter.cpp
)

# Source files - Utilities (shared by both modules)
set(KOO_UTIL_SOURCES
    util/CardParser.cpp
    util/StringUtils.cpp
)

# Find threading library
find_package(Threads REQUIRED)

# Find ZLIB for ODB++ compressed file support
find_package(ZLIB REQUIRED)

# ============================================================================
# Phase 5: CAD and Mesh modules (optional dependencies)
# ============================================================================
option(BUILD_CAD_MODULE "Build CAD module" ON)
option(BUILD_MESH_MODULE "Build Mesh module" ON)

# Build CAD module if enabled
if(BUILD_CAD_MODULE)
    add_subdirectory(cad)
endif()

# Build Mesh module if enabled (depends on CAD)
if(BUILD_MESH_MODULE AND BUILD_CAD_MODULE)
    add_subdirectory(mesh)
endif()

# ============================================================================
# ECAD-only library (for independent testing)
# ============================================================================
if(BUILD_ECAD_MODULE)
    add_library(koo_ecad STATIC ${KOO_ECAD_SOURCES} util/StringUtils.cpp)
    target_compile_definitions(koo_ecad PUBLIC KOO_SIM_STATIC)
    target_link_libraries(koo_ecad PRIVATE Threads::Threads ZLIB::ZLIB)
    set_project_warnings(koo_ecad)
    set_target_properties(koo_ecad PROPERTIES OUTPUT_NAME koo_ecad)
endif()

# ============================================================================
# DYNA-only library (for independent testing)
# ============================================================================
if(BUILD_DYNA_MODULE)
    add_library(koo_dyna STATIC ${KOO_DYNA_SOURCES} ${KOO_UTIL_SOURCES})
    target_compile_definitions(koo_dyna PUBLIC KOO_SIM_STATIC)
    target_link_libraries(koo_dyna PRIVATE Threads::Threads)

    # Link CAD/Mesh modules if available (for GeometryManager)
    if(BUILD_CAD_MODULE)
        target_link_libraries(koo_dyna PUBLIC koo_cad)
    endif()
    if(BUILD_MESH_MODULE)
        target_link_libraries(koo_dyna PUBLIC koo_mesh)
    endif()

    set_project_warnings(koo_dyna)
    set_target_properties(koo_dyna PROPERTIES OUTPUT_NAME koo_dyna)
endif()

# ============================================================================
# Combined library (full distribution)
# ============================================================================

# Build combined sources based on enabled modules
set(KOO_SIM_SOURCES ${KOO_UTIL_SOURCES})
if(BUILD_DYNA_MODULE)
    list(APPEND KOO_SIM_SOURCES ${KOO_DYNA_SOURCES})
endif()
if(BUILD_ECAD_MODULE)
    list(APPEND KOO_SIM_SOURCES ${KOO_ECAD_SOURCES})
endif()

# Shared library
if(BUILD_SHARED_LIBS)
    add_library(koo_sim SHARED ${KOO_SIM_SOURCES})
    target_compile_definitions(koo_sim PRIVATE KOO_SIM_EXPORTS)
    target_link_libraries(koo_sim PRIVATE Threads::Threads ZLIB::ZLIB)

    # Link CAD/Mesh modules if available
    if(BUILD_CAD_MODULE)
        target_link_libraries(koo_sim PUBLIC koo_cad)
    endif()
    if(BUILD_MESH_MODULE)
        target_link_libraries(koo_sim PUBLIC koo_mesh)
    endif()

    set_project_warnings(koo_sim)

    set_target_properties(koo_sim PROPERTIES
        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}
        OUTPUT_NAME koo_sim
    )

    # Install shared library
    install(TARGETS koo_sim
        EXPORT KooSimTargets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
endif()

# Static library
if(BUILD_STATIC_LIBS)
    add_library(koo_sim_static STATIC ${KOO_SIM_SOURCES})
    target_compile_definitions(koo_sim_static PUBLIC KOO_SIM_STATIC)
    target_link_libraries(koo_sim_static PRIVATE Threads::Threads ZLIB::ZLIB)

    # Link CAD/Mesh modules if available
    if(BUILD_CAD_MODULE)
        target_link_libraries(koo_sim_static PUBLIC koo_cad)
    endif()
    if(BUILD_MESH_MODULE)
        target_link_libraries(koo_sim_static PUBLIC koo_mesh)
    endif()

    set_project_warnings(koo_sim_static)

    set_target_properties(koo_sim_static PROPERTIES
        OUTPUT_NAME koo_sim_static
    )

    # Install static library
    install(TARGETS koo_sim_static
        EXPORT KooSimTargets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
endif()

# Install CAD/Mesh modules separately (once)
if(BUILD_CAD_MODULE)
    install(TARGETS koo_cad
        EXPORT KooSimTargets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
endif()

if(BUILD_MESH_MODULE)
    install(TARGETS koo_mesh
        EXPORT KooSimTargets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
endif()

# Export targets
install(EXPORT KooSimTargets
    FILE KooSimTargets.cmake
    NAMESPACE KooSim::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/KooSim
)
