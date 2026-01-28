# CAD Module CMakeLists.txt
# Phase 5a: OpenCASCADE Integration

# CAD module sources
# Note: Many classes are header-only with inline implementations
set(KOO_CAD_SOURCES
    Geometry.cpp
    ICADImporter.cpp
)

# OpenCASCADE implementation (conditional)
if(WITH_OPENCASCADE AND OpenCASCADE_FOUND)
    list(APPEND KOO_CAD_SOURCES
        OccadImporter.cpp
    )
endif()

# Create CAD library target
# Use INTERFACE library if no sources (header-only mode)
if(KOO_CAD_SOURCES)
    add_library(koo_cad STATIC ${KOO_CAD_SOURCES})

    # Link OpenCASCADE if available
    if(WITH_OPENCASCADE AND OpenCASCADE_FOUND)
        set(OPENCASCADE_LIBS
            OpenCASCADE::TKernel
            OpenCASCADE::TKMath
            OpenCASCADE::TKBRep
            OpenCASCADE::TKGeomBase
            OpenCASCADE::TKGeomAlgo
            OpenCASCADE::TKTopAlgo
            OpenCASCADE::TKPrim
            OpenCASCADE::TKBO
            OpenCASCADE::TKShHealing
            OpenCASCADE::TKXSBase
        )

        # Add STEP/IGES support (use new 7.9+ names if available, otherwise old names)
        if(TARGET OpenCASCADE::TKDESTEP)
            list(APPEND OPENCASCADE_LIBS OpenCASCADE::TKDESTEP)
            if(TARGET OpenCASCADE::TKDEIGES)
                list(APPEND OPENCASCADE_LIBS OpenCASCADE::TKDEIGES)
            endif()
        else()
            list(APPEND OPENCASCADE_LIBS
                OpenCASCADE::TKSTEP
                OpenCASCADE::TKSTEPBase
                OpenCASCADE::TKSTEPAttr
                OpenCASCADE::TKSTEP209
                OpenCASCADE::TKIGES
            )
        endif()

        target_link_libraries(koo_cad PUBLIC ${OPENCASCADE_LIBS})
        message(STATUS "CAD module: OpenCASCADE support enabled")
    else()
        message(STATUS "CAD module: Header-only (no OpenCASCADE)")
    endif()

    set_project_warnings(koo_cad)
    set_target_properties(koo_cad PROPERTIES OUTPUT_NAME koo_cad)
else()
    # Header-only mode
    add_library(koo_cad INTERFACE)
    message(STATUS "CAD module: INTERFACE library (header-only mode)")
endif()
