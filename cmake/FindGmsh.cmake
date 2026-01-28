# FindGmsh.cmake
# --------------
# Find Gmsh mesh generator library
#
# This module defines:
#  Gmsh_FOUND - system has Gmsh
#  Gmsh_INCLUDE_DIRS - include directories for Gmsh
#  Gmsh_LIBRARIES - libraries for Gmsh
#  Gmsh_VERSION - version string
#
# Imported targets:
#  Gmsh::Gmsh - Gmsh library

# Find Gmsh installation directory
find_path(Gmsh_DIR
    NAMES GmshConfig.cmake gmsh-config.cmake
    PATHS
        ENV GMSH_DIR
        /usr/local/share/gmsh
        /usr/share/gmsh
        /opt/gmsh
        "C:/Program Files/Gmsh"
    PATH_SUFFIXES
        cmake
        lib/cmake/gmsh
)

# If found, use the config file
if(Gmsh_DIR)
    include(${Gmsh_DIR}/GmshConfig.cmake OPTIONAL)
endif()

# Manual search if config not found
if(NOT Gmsh_FOUND)
    # Find include directory
    find_path(Gmsh_INCLUDE_DIR
        NAMES gmsh.h
        PATHS
            ENV GMSH_DIR
            /usr/local/include
            /usr/include
            /opt/gmsh/include
            "C:/Program Files/Gmsh/include"
        PATH_SUFFIXES include gmsh
    )

    # Find library
    find_library(Gmsh_LIBRARY
        NAMES gmsh
        PATHS
            ENV GMSH_DIR
            /usr/local/lib
            /usr/lib
            /opt/gmsh/lib
            "C:/Program Files/Gmsh/lib"
        PATH_SUFFIXES lib
    )

    # Set variables
    set(Gmsh_INCLUDE_DIRS ${Gmsh_INCLUDE_DIR})
    set(Gmsh_LIBRARIES ${Gmsh_LIBRARY})

    # Try to extract version
    if(EXISTS "${Gmsh_INCLUDE_DIR}/gmsh.h")
        file(STRINGS "${Gmsh_INCLUDE_DIR}/gmsh.h"
             _version_major REGEX "^#define GMSH_API_VERSION_MAJOR")
        file(STRINGS "${Gmsh_INCLUDE_DIR}/gmsh.h"
             _version_minor REGEX "^#define GMSH_API_VERSION_MINOR")

        if(_version_major AND _version_minor)
            string(REGEX REPLACE ".*GMSH_API_VERSION_MAJOR ([0-9]+).*" "\\1"
                   _major "${_version_major}")
            string(REGEX REPLACE ".*GMSH_API_VERSION_MINOR ([0-9]+).*" "\\1"
                   _minor "${_version_minor}")
            set(Gmsh_VERSION "${_major}.${_minor}")
        endif()
    endif()
endif()

# Handle standard find_package arguments
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Gmsh
    REQUIRED_VARS Gmsh_LIBRARIES Gmsh_INCLUDE_DIRS
    VERSION_VAR Gmsh_VERSION
)

# Create imported target
if(Gmsh_FOUND AND NOT TARGET Gmsh::Gmsh)
    add_library(Gmsh::Gmsh UNKNOWN IMPORTED)
    set_target_properties(Gmsh::Gmsh PROPERTIES
        IMPORTED_LOCATION "${Gmsh_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${Gmsh_INCLUDE_DIRS}"
    )
endif()

mark_as_advanced(
    Gmsh_DIR
    Gmsh_INCLUDE_DIR
    Gmsh_LIBRARY
)
