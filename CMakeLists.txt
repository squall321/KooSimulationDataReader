cmake_minimum_required(VERSION 3.16)

project(KooSimulationDataReader
    VERSION 0.1.0
    DESCRIPTION "LS-DYNA keyword file processing library"
    LANGUAGES CXX
)

# C++17 required
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build options
option(BUILD_SHARED_LIBS "Build shared library" ON)
option(BUILD_STATIC_LIBS "Build static library" ON)
option(BUILD_CLI "Build CLI executable" ON)
option(BUILD_TESTS "Build tests" ON)
option(BUILD_EXAMPLES "Build examples" OFF)
option(BUILD_PYTHON "Build Python bindings" OFF)
option(WITH_OPENCASCADE "Enable OpenCASCADE CAD support" OFF)
option(WITH_GMSH "Enable GMSH meshing support" OFF)
option(BUILD_INTEGRATION_TESTS "Build integration tests (requires test data)" OFF)

# Output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Platform-specific settings
include(cmake/Platform.cmake)
include(cmake/CompilerWarnings.cmake)

# Add cmake module path
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# Optional dependencies for Phase 5 (CAD Integration)
if(WITH_OPENCASCADE)
    find_package(OpenCASCADE)
    if(OpenCASCADE_FOUND)
        message(STATUS "OpenCASCADE found - CAD import enabled")
        add_compile_definitions(KOO_HAS_OPENCASCADE)
    else()
        message(WARNING "OpenCASCADE not found - CAD import will use stub implementation")
    endif()
endif()

if(WITH_GMSH)
    find_package(Gmsh)
    if(Gmsh_FOUND)
        message(STATUS "Gmsh found - mesh generation enabled")
        add_compile_definitions(KOO_HAS_GMSH)
    else()
        message(STATUS "Gmsh not found locally - attempting to download Gmsh SDK...")
        include(FetchContent)

        # Determine platform-specific Gmsh SDK URL
        if(UNIX AND NOT APPLE)
            # Linux x64
            set(GMSH_URL "https://gmsh.info/bin/Linux/gmsh-4.13.1-Linux64-sdk.tgz")
            set(GMSH_HASH "")
        elseif(APPLE)
            # macOS
            set(GMSH_URL "https://gmsh.info/bin/MacOSX/gmsh-4.13.1-MacOSX-sdk.tgz")
            set(GMSH_HASH "")
        elseif(WIN32)
            # Windows x64
            set(GMSH_URL "https://gmsh.info/bin/Windows/gmsh-4.13.1-Windows64-sdk.zip")
            set(GMSH_HASH "")
        else()
            message(WARNING "Unsupported platform for automatic Gmsh download")
        endif()

        if(GMSH_URL)
            FetchContent_Declare(
                gmsh_sdk
                URL ${GMSH_URL}
            )

            FetchContent_MakeAvailable(gmsh_sdk)

            # Set Gmsh variables manually from downloaded SDK
            set(Gmsh_INCLUDE_DIRS "${gmsh_sdk_SOURCE_DIR}/include")
            set(Gmsh_LIBRARY "${gmsh_sdk_SOURCE_DIR}/lib/libgmsh.so")

            if(WIN32)
                set(Gmsh_LIBRARY "${gmsh_sdk_SOURCE_DIR}/lib/gmsh.lib")
            elseif(APPLE)
                set(Gmsh_LIBRARY "${gmsh_sdk_SOURCE_DIR}/lib/libgmsh.dylib")
            endif()

            # Create imported target manually
            if(EXISTS "${Gmsh_LIBRARY}")
                add_library(Gmsh::Gmsh UNKNOWN IMPORTED)
                set_target_properties(Gmsh::Gmsh PROPERTIES
                    IMPORTED_LOCATION "${Gmsh_LIBRARY}"
                    INTERFACE_INCLUDE_DIRECTORIES "${Gmsh_INCLUDE_DIRS}"
                )
                set(Gmsh_FOUND TRUE)
                add_compile_definitions(KOO_HAS_GMSH)
                message(STATUS "Gmsh SDK downloaded successfully - mesh generation enabled")
            else()
                message(WARNING "Gmsh SDK download failed - mesh generation will use stub implementation")
            endif()
        endif()
    endif()
endif()

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Source library
add_subdirectory(src)

# CLI executable
if(BUILD_CLI)
    add_subdirectory(cli)
endif()

# Tests
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Examples
if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# Install configuration
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# Install headers
install(DIRECTORY include/koo
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.hpp"
)

# Generate package config
configure_package_config_file(
    ${CMAKE_SOURCE_DIR}/cmake/KooSimConfig.cmake.in
    ${CMAKE_BINARY_DIR}/KooSimConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/KooSim
)

write_basic_package_version_file(
    ${CMAKE_BINARY_DIR}/KooSimConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES
    ${CMAKE_BINARY_DIR}/KooSimConfig.cmake
    ${CMAKE_BINARY_DIR}/KooSimConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/KooSim
)
