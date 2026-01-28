# FindOpenCASCADE.cmake
# --------------------
# Find OpenCASCADE Technology (OCCT)
#
# This module defines:
#  OpenCASCADE_FOUND - system has OpenCASCADE
#  OpenCASCADE_INCLUDE_DIRS - include directories for OpenCASCADE
#  OpenCASCADE_LIBRARIES - libraries for OpenCASCADE
#  OpenCASCADE_VERSION - version string
#
# Imported targets:
#  OpenCASCADE::TKernel - Core kernel
#  OpenCASCADE::TKBRep - Boundary representation
#  OpenCASCADE::TKSTEP - STEP import/export
#  OpenCASCADE::TKIGES - IGES import/export

# Find OpenCASCADE installation directory
find_path(OpenCASCADE_DIR
    NAMES OpenCASCADEConfig.cmake opencascade.cmake
    PATHS
        ENV CASROOT
        ENV OCCT_DIR
        /usr/local/share/opencascade
        /usr/share/opencascade
        /opt/opencascade
        "C:/OpenCASCADE"
        "C:/Program Files/OpenCASCADE"
    PATH_SUFFIXES
        cmake
        lib/cmake/opencascade
)

# If found, use the config file
if(OpenCASCADE_DIR)
    include(${OpenCASCADE_DIR}/OpenCASCADEConfig.cmake OPTIONAL)
endif()

# Manual search if config not found
if(NOT OpenCASCADE_FOUND)
    # Find include directory
    find_path(OpenCASCADE_INCLUDE_DIR
        NAMES Standard.hxx
        PATHS
            ENV CASROOT
            /usr/local/include/opencascade
            /usr/include/opencascade
            /opt/opencascade/include
            "C:/OpenCASCADE/include"
        PATH_SUFFIXES inc include
    )

    # Find library directory (prefer 7.7.0 for consistency with TKSTEP)
    find_library(OpenCASCADE_TKernel_LIBRARY
        NAMES TKernel
        HINTS
            /usr/local/opencascade-7.7.0/lib
        PATHS
            ENV CASROOT
            /home/koopark/opt/occt-7.9.0/lib
            /usr/local/lib
            /usr/lib
            /opt/opencascade/lib
            "C:/OpenCASCADE/lib"
        PATH_SUFFIXES lib
    )

    # Define required libraries (core)
    set(_OPENCASCADE_REQUIRED_LIBS
        TKernel TKMath TKBRep TKG2d TKG3d TKGeomBase TKGeomAlgo
        TKTopAlgo TKPrim TKBO TKShHealing TKMesh TKXSBase
    )

    # Find all required libraries
    set(OpenCASCADE_LIBRARIES "")
    foreach(_lib ${_OPENCASCADE_REQUIRED_LIBS})
        find_library(OpenCASCADE_${_lib}_LIBRARY
            NAMES ${_lib}
            PATHS
                ENV CASROOT
                /home/koopark/opt/occt-7.9.0/lib
                /usr/local/opencascade-7.7.0/lib
                /usr/local/lib
                /usr/lib
                /opt/opencascade/lib
                "C:/OpenCASCADE/lib"
            PATH_SUFFIXES lib
        )
        if(OpenCASCADE_${_lib}_LIBRARY)
            list(APPEND OpenCASCADE_LIBRARIES ${OpenCASCADE_${_lib}_LIBRARY})
        endif()
    endforeach()

    # Find STEP/IGES libraries (try new 7.9+ names first, then fall back to old names)
    find_library(OpenCASCADE_TKDESTEP_LIBRARY NAMES TKDESTEP
        PATHS ENV CASROOT /home/koopark/opt/occt-7.9.0/lib /usr/local/opencascade-7.7.0/lib
              /usr/local/lib /usr/lib /opt/opencascade/lib "C:/OpenCASCADE/lib"
        PATH_SUFFIXES lib)

    find_library(OpenCASCADE_TKDEIGES_LIBRARY NAMES TKDEIGES
        PATHS ENV CASROOT /home/koopark/opt/occt-7.9.0/lib /usr/local/opencascade-7.7.0/lib
              /usr/local/lib /usr/lib /opt/opencascade/lib "C:/OpenCASCADE/lib"
        PATH_SUFFIXES lib)

    if(OpenCASCADE_TKDESTEP_LIBRARY)
        # Use new 7.9+ libraries
        list(APPEND OpenCASCADE_LIBRARIES ${OpenCASCADE_TKDESTEP_LIBRARY})
        if(OpenCASCADE_TKDEIGES_LIBRARY)
            list(APPEND OpenCASCADE_LIBRARIES ${OpenCASCADE_TKDEIGES_LIBRARY})
        endif()
    else()
        # Fall back to old library names
        foreach(_lib TKSTEP TKSTEPBase TKSTEPAttr TKSTEP209 TKIGES)
            find_library(OpenCASCADE_${_lib}_LIBRARY NAMES ${_lib}
                PATHS ENV CASROOT /home/koopark/opt/occt-7.9.0/lib /usr/local/opencascade-7.7.0/lib
                      /usr/local/lib /usr/lib /opt/opencascade/lib "C:/OpenCASCADE/lib"
                PATH_SUFFIXES lib)
            if(OpenCASCADE_${_lib}_LIBRARY)
                list(APPEND OpenCASCADE_LIBRARIES ${OpenCASCADE_${_lib}_LIBRARY})
            endif()
        endforeach()
    endif()

    # Set variables
    set(OpenCASCADE_INCLUDE_DIRS ${OpenCASCADE_INCLUDE_DIR})

    # Try to extract version
    if(EXISTS "${OpenCASCADE_INCLUDE_DIR}/Standard_Version.hxx")
        file(STRINGS "${OpenCASCADE_INCLUDE_DIR}/Standard_Version.hxx"
             _version_line REGEX "^#define OCC_VERSION_COMPLETE")
        if(_version_line)
            string(REGEX REPLACE ".*\"([0-9.]+)\".*" "\\1"
                   OpenCASCADE_VERSION "${_version_line}")
        endif()
    endif()
endif()

# Handle standard find_package arguments
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(OpenCASCADE
    REQUIRED_VARS OpenCASCADE_LIBRARIES OpenCASCADE_INCLUDE_DIRS
    VERSION_VAR OpenCASCADE_VERSION
)

# Create imported targets
if(OpenCASCADE_FOUND AND NOT TARGET OpenCASCADE::TKernel)
    # TKernel (core)
    add_library(OpenCASCADE::TKernel UNKNOWN IMPORTED)
    set_target_properties(OpenCASCADE::TKernel PROPERTIES
        IMPORTED_LOCATION "${OpenCASCADE_TKernel_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${OpenCASCADE_INCLUDE_DIRS}"
    )

    # TKBRep (boundary representation)
    if(OpenCASCADE_TKBRep_LIBRARY)
        add_library(OpenCASCADE::TKBRep UNKNOWN IMPORTED)
        set_target_properties(OpenCASCADE::TKBRep PROPERTIES
            IMPORTED_LOCATION "${OpenCASCADE_TKBRep_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${OpenCASCADE_INCLUDE_DIRS}"
        )
    endif()

    # TKSTEP (STEP I/O)
    if(OpenCASCADE_TKSTEP_LIBRARY)
        add_library(OpenCASCADE::TKSTEP UNKNOWN IMPORTED)
        set_target_properties(OpenCASCADE::TKSTEP PROPERTIES
            IMPORTED_LOCATION "${OpenCASCADE_TKSTEP_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${OpenCASCADE_INCLUDE_DIRS}"
        )
    endif()

    # TKIGES (IGES I/O)
    if(OpenCASCADE_TKIGES_LIBRARY)
        add_library(OpenCASCADE::TKIGES UNKNOWN IMPORTED)
        set_target_properties(OpenCASCADE::TKIGES PROPERTIES
            IMPORTED_LOCATION "${OpenCASCADE_TKIGES_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${OpenCASCADE_INCLUDE_DIRS}"
        )
    endif()

    # TKMath (mathematics)
    if(OpenCASCADE_TKMath_LIBRARY)
        add_library(OpenCASCADE::TKMath UNKNOWN IMPORTED)
        set_target_properties(OpenCASCADE::TKMath PROPERTIES
            IMPORTED_LOCATION "${OpenCASCADE_TKMath_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${OpenCASCADE_INCLUDE_DIRS}"
        )
    endif()

    # TKGeomBase (geometry base)
    if(OpenCASCADE_TKGeomBase_LIBRARY)
        add_library(OpenCASCADE::TKGeomBase UNKNOWN IMPORTED)
        set_target_properties(OpenCASCADE::TKGeomBase PROPERTIES
            IMPORTED_LOCATION "${OpenCASCADE_TKGeomBase_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${OpenCASCADE_INCLUDE_DIRS}"
        )
    endif()

    # TKGeomAlgo (geometry algorithms)
    if(OpenCASCADE_TKGeomAlgo_LIBRARY)
        add_library(OpenCASCADE::TKGeomAlgo UNKNOWN IMPORTED)
        set_target_properties(OpenCASCADE::TKGeomAlgo PROPERTIES
            IMPORTED_LOCATION "${OpenCASCADE_TKGeomAlgo_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${OpenCASCADE_INCLUDE_DIRS}"
        )
    endif()

    # TKTopAlgo (topology algorithms)
    if(OpenCASCADE_TKTopAlgo_LIBRARY)
        add_library(OpenCASCADE::TKTopAlgo UNKNOWN IMPORTED)
        set_target_properties(OpenCASCADE::TKTopAlgo PROPERTIES
            IMPORTED_LOCATION "${OpenCASCADE_TKTopAlgo_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${OpenCASCADE_INCLUDE_DIRS}"
        )
    endif()

    # TKPrim (primitives)
    if(OpenCASCADE_TKPrim_LIBRARY)
        add_library(OpenCASCADE::TKPrim UNKNOWN IMPORTED)
        set_target_properties(OpenCASCADE::TKPrim PROPERTIES
            IMPORTED_LOCATION "${OpenCASCADE_TKPrim_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${OpenCASCADE_INCLUDE_DIRS}"
        )
    endif()

    # TKBO (boolean operations)
    if(OpenCASCADE_TKBO_LIBRARY)
        add_library(OpenCASCADE::TKBO UNKNOWN IMPORTED)
        set_target_properties(OpenCASCADE::TKBO PROPERTIES
            IMPORTED_LOCATION "${OpenCASCADE_TKBO_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${OpenCASCADE_INCLUDE_DIRS}"
        )
    endif()

    # TKShHealing (shape healing)
    if(OpenCASCADE_TKShHealing_LIBRARY)
        add_library(OpenCASCADE::TKShHealing UNKNOWN IMPORTED)
        set_target_properties(OpenCASCADE::TKShHealing PROPERTIES
            IMPORTED_LOCATION "${OpenCASCADE_TKShHealing_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${OpenCASCADE_INCLUDE_DIRS}"
        )
    endif()

    # TKXSBase (exchange base)
    if(OpenCASCADE_TKXSBase_LIBRARY)
        add_library(OpenCASCADE::TKXSBase UNKNOWN IMPORTED)
        set_target_properties(OpenCASCADE::TKXSBase PROPERTIES
            IMPORTED_LOCATION "${OpenCASCADE_TKXSBase_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${OpenCASCADE_INCLUDE_DIRS}"
        )
    endif()

    # TKSTEPBase (STEP base)
    if(OpenCASCADE_TKSTEPBase_LIBRARY)
        add_library(OpenCASCADE::TKSTEPBase UNKNOWN IMPORTED)
        set_target_properties(OpenCASCADE::TKSTEPBase PROPERTIES
            IMPORTED_LOCATION "${OpenCASCADE_TKSTEPBase_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${OpenCASCADE_INCLUDE_DIRS}"
        )
    endif()

    # TKSTEPAttr (STEP attributes)
    if(OpenCASCADE_TKSTEPAttr_LIBRARY)
        add_library(OpenCASCADE::TKSTEPAttr UNKNOWN IMPORTED)
        set_target_properties(OpenCASCADE::TKSTEPAttr PROPERTIES
            IMPORTED_LOCATION "${OpenCASCADE_TKSTEPAttr_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${OpenCASCADE_INCLUDE_DIRS}"
        )
    endif()

    # TKSTEP209 (STEP AP209)
    if(OpenCASCADE_TKSTEP209_LIBRARY)
        add_library(OpenCASCADE::TKSTEP209 UNKNOWN IMPORTED)
        set_target_properties(OpenCASCADE::TKSTEP209 PROPERTIES
            IMPORTED_LOCATION "${OpenCASCADE_TKSTEP209_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${OpenCASCADE_INCLUDE_DIRS}"
        )
    endif()

    # TKDESTEP (STEP in 7.9+)
    if(OpenCASCADE_TKDESTEP_LIBRARY)
        add_library(OpenCASCADE::TKDESTEP UNKNOWN IMPORTED)
        set_target_properties(OpenCASCADE::TKDESTEP PROPERTIES
            IMPORTED_LOCATION "${OpenCASCADE_TKDESTEP_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${OpenCASCADE_INCLUDE_DIRS}"
        )
    endif()

    # TKDEIGES (IGES in 7.9+)
    if(OpenCASCADE_TKDEIGES_LIBRARY)
        add_library(OpenCASCADE::TKDEIGES UNKNOWN IMPORTED)
        set_target_properties(OpenCASCADE::TKDEIGES PROPERTIES
            IMPORTED_LOCATION "${OpenCASCADE_TKDEIGES_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${OpenCASCADE_INCLUDE_DIRS}"
        )
    endif()
endif()

mark_as_advanced(
    OpenCASCADE_DIR
    OpenCASCADE_INCLUDE_DIR
    OpenCASCADE_TKernel_LIBRARY
)
