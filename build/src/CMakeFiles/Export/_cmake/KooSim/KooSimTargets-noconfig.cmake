#----------------------------------------------------------------
# Generated CMake target import file.
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "KooSim::koo_sim" for configuration ""
set_property(TARGET KooSim::koo_sim APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(KooSim::koo_sim PROPERTIES
  IMPORTED_LOCATION_NOCONFIG "${_IMPORT_PREFIX}/lib/libkoo_sim.so.0.1.0"
  IMPORTED_SONAME_NOCONFIG "libkoo_sim.so.0"
  )

list(APPEND _IMPORT_CHECK_TARGETS KooSim::koo_sim )
list(APPEND _IMPORT_CHECK_FILES_FOR_KooSim::koo_sim "${_IMPORT_PREFIX}/lib/libkoo_sim.so.0.1.0" )

# Import target "KooSim::koo_sim_static" for configuration ""
set_property(TARGET KooSim::koo_sim_static APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(KooSim::koo_sim_static PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_NOCONFIG "CXX"
  IMPORTED_LOCATION_NOCONFIG "${_IMPORT_PREFIX}/lib/libkoo_sim_static.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS KooSim::koo_sim_static )
list(APPEND _IMPORT_CHECK_FILES_FOR_KooSim::koo_sim_static "${_IMPORT_PREFIX}/lib/libkoo_sim_static.a" )

# Import target "KooSim::koo_cad" for configuration ""
set_property(TARGET KooSim::koo_cad APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(KooSim::koo_cad PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_NOCONFIG "CXX"
  IMPORTED_LOCATION_NOCONFIG "${_IMPORT_PREFIX}/lib/libkoo_cad.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS KooSim::koo_cad )
list(APPEND _IMPORT_CHECK_FILES_FOR_KooSim::koo_cad "${_IMPORT_PREFIX}/lib/libkoo_cad.a" )

# Import target "KooSim::koo_mesh" for configuration ""
set_property(TARGET KooSim::koo_mesh APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(KooSim::koo_mesh PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_NOCONFIG "CXX"
  IMPORTED_LOCATION_NOCONFIG "${_IMPORT_PREFIX}/lib/libkoo_mesh.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS KooSim::koo_mesh )
list(APPEND _IMPORT_CHECK_FILES_FOR_KooSim::koo_mesh "${_IMPORT_PREFIX}/lib/libkoo_mesh.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
