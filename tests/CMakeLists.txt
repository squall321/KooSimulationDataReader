# Tests

include(FetchContent)

# Fetch Google Test
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

# ============================================================================
# DYNA module tests (requires DYNA module)
# ============================================================================
if(BUILD_DYNA_MODULE)
    add_executable(koo_dyna_tests
        unit/TestStringUtils.cpp
        unit/TestCardParser.cpp
        unit/TestNode.cpp
        unit/TestElement.cpp
        unit/TestPart.cpp
        unit/TestMaterial.cpp
        unit/TestSection.cpp
        unit/TestModel.cpp
        unit/TestKeywordFileReader.cpp
        unit/TestKeywordFileWriter.cpp
        unit/TestModelVisitor.cpp
    )

    target_link_libraries(koo_dyna_tests PRIVATE
        koo_dyna
        GTest::gtest_main
    )

    target_include_directories(koo_dyna_tests PRIVATE
        ${CMAKE_SOURCE_DIR}/include
    )

    include(GoogleTest)
    gtest_discover_tests(koo_dyna_tests)
endif()

# ============================================================================
# ECAD module tests (requires ECAD module)
# ============================================================================
if(BUILD_ECAD_MODULE)
    add_executable(koo_ecad_tests
        unit/TestFeature.cpp
        unit/TestSymbol.cpp
        unit/TestLayer.cpp
        unit/TestEdaData.cpp
        unit/TestOdbJob.cpp
        unit/TestOdbWriter.cpp
        unit/TestOdbReader.cpp
    )

    target_link_libraries(koo_ecad_tests PRIVATE
        koo_ecad
        GTest::gtest_main
    )

    target_include_directories(koo_ecad_tests PRIVATE
        ${CMAKE_SOURCE_DIR}/include
    )

    include(GoogleTest)
    gtest_discover_tests(koo_ecad_tests)
endif()

# ============================================================================
# Phase 5: CAD module tests (requires CAD module)
# ============================================================================
if(BUILD_CAD_MODULE)
    add_executable(koo_cad_tests
        unit/TestGeometry.cpp
    )

    target_link_libraries(koo_cad_tests PRIVATE
        koo_cad
        GTest::gtest_main
    )

    target_include_directories(koo_cad_tests PRIVATE
        ${CMAKE_SOURCE_DIR}/include
    )

    include(GoogleTest)
    gtest_discover_tests(koo_cad_tests)
endif()

# ============================================================================
# Phase 5: Mesh module tests (requires Mesh module)
# ============================================================================
if(BUILD_MESH_MODULE)
    add_executable(koo_mesh_tests
        unit/TestMeshParameters.cpp
        unit/TestMeshQuality.cpp
    )

    target_link_libraries(koo_mesh_tests PRIVATE
        koo_mesh
        GTest::gtest_main
    )

    target_include_directories(koo_mesh_tests PRIVATE
        ${CMAKE_SOURCE_DIR}/include
    )

    include(GoogleTest)
    gtest_discover_tests(koo_mesh_tests)
endif()

# ============================================================================
# Full test suite (both modules)
# ============================================================================
if(BUILD_DYNA_MODULE AND BUILD_ECAD_MODULE)
    set(KOO_SIM_TEST_SOURCES
        unit/TestStringUtils.cpp
        unit/TestCardParser.cpp
        unit/TestNode.cpp
        unit/TestElement.cpp
        unit/TestPart.cpp
        unit/TestMaterial.cpp
        unit/TestSection.cpp
        unit/TestModel.cpp
        unit/TestKeywordFileReader.cpp
        unit/TestKeywordFileWriter.cpp
        unit/TestFeature.cpp
        unit/TestSymbol.cpp
        unit/TestLayer.cpp
        unit/TestEdaData.cpp
        unit/TestOdbJob.cpp
        unit/TestOdbWriter.cpp
        unit/TestOdbReader.cpp
    )

    add_executable(koo_sim_tests ${KOO_SIM_TEST_SOURCES})

    target_link_libraries(koo_sim_tests PRIVATE
        $<IF:$<BOOL:${BUILD_SHARED_LIBS}>,koo_sim,koo_sim_static>
        GTest::gtest_main
    )

    target_include_directories(koo_sim_tests PRIVATE
        ${CMAKE_SOURCE_DIR}/include
    )

    include(GoogleTest)
    gtest_discover_tests(koo_sim_tests)
endif()

# Copy test data
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/test_data
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# ============================================================================
# Integration tests (ECAD module)
# ============================================================================
if(BUILD_ECAD_MODULE AND BUILD_INTEGRATION_TESTS)
    add_executable(koo_ecad_integration_tests
        integration/TestOdbReaderIntegration.cpp
    )

    target_link_libraries(koo_ecad_integration_tests PRIVATE
        koo_ecad
        GTest::gtest_main
    )

    target_include_directories(koo_ecad_integration_tests PRIVATE
        ${CMAKE_SOURCE_DIR}/include
    )

    include(GoogleTest)
    gtest_discover_tests(koo_ecad_integration_tests)
endif()
