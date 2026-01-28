# Platform-specific settings

if(WIN32)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    add_definitions(-DNOMINMAX)
    add_definitions(-DWIN32_LEAN_AND_MEAN)
endif()

if(MSVC)
    # UTF-8 source and execution charset
    add_compile_options(/utf-8)
    # Parallel compilation
    add_compile_options(/MP)
else()
    # Position independent code for shared libraries
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

# macOS specific
if(APPLE)
    set(CMAKE_MACOSX_RPATH ON)
    set(CMAKE_INSTALL_RPATH "@executable_path/../lib")
endif()

# Linux specific
if(UNIX AND NOT APPLE)
    set(CMAKE_INSTALL_RPATH "$ORIGIN/../lib")
endif()
