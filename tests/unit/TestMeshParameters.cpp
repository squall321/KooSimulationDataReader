// Unit tests for MeshParameters
// Phase 5b: Mesh Integration

#include <koo/mesh/MeshParameters.hpp>
#include <gtest/gtest.h>

using namespace koo::mesh;

TEST(MeshParametersTest, DefaultConstruction) {
    MeshParameters params;

    EXPECT_GT(params.globalElementSize, 0.0);
    EXPECT_GT(params.minElementSize, 0.0);
    EXPECT_GT(params.maxElementSize, 0.0);
    EXPECT_GE(params.numOptimizationPasses, 0);
}

TEST(MeshParametersTest, CoarsePreset) {
    MeshParameters params = MeshParameters::coarse(10.0);

    EXPECT_EQ(params.globalElementSize, 10.0);
    EXPECT_EQ(params.meshType, MeshType::Volume);
    EXPECT_EQ(params.elementOrder, ElementOrder::Linear);
    EXPECT_GT(params.minElementQuality, 0.0);
    EXPECT_EQ(params.numOptimizationPasses, 1);
}

TEST(MeshParametersTest, MediumPreset) {
    MeshParameters params = MeshParameters::medium(5.0);

    EXPECT_EQ(params.globalElementSize, 5.0);
    EXPECT_EQ(params.meshType, MeshType::Volume);
    EXPECT_EQ(params.elementOrder, ElementOrder::Linear);
    EXPECT_GT(params.numOptimizationPasses, 1);
}

TEST(MeshParametersTest, FinePreset) {
    MeshParameters params = MeshParameters::fine(1.0);

    EXPECT_EQ(params.globalElementSize, 1.0);
    EXPECT_EQ(params.meshType, MeshType::Volume);
    EXPECT_EQ(params.elementOrder, ElementOrder::Linear);
    EXPECT_GT(params.numOptimizationPasses, 2);
    EXPECT_GT(params.minElementQuality, 0.0);
}

TEST(MeshParametersTest, QuadraticPreset) {
    MeshParameters params = MeshParameters::quadratic(5.0);

    EXPECT_EQ(params.globalElementSize, 5.0);
    EXPECT_EQ(params.elementOrder, ElementOrder::Quadratic);
    EXPECT_TRUE(params.optimizeHighOrder);
}

TEST(MeshParametersTest, SurfaceOnlyPreset) {
    MeshParameters params = MeshParameters::surfaceOnly(5.0);

    EXPECT_EQ(params.globalElementSize, 5.0);
    EXPECT_EQ(params.meshType, MeshType::Surface);
    EXPECT_EQ(params.elementOrder, ElementOrder::Linear);
}

TEST(MeshParametersTest, CustomParameters) {
    MeshParameters params;
    params.globalElementSize = 3.5;
    params.minElementSize = 0.5;
    params.maxElementSize = 10.0;
    params.meshType = MeshType::Volume;
    params.elementOrder = ElementOrder::Quadratic;
    params.algorithm = MeshAlgorithm::Frontal;
    params.numOptimizationPasses = 5;
    params.minElementQuality = 0.3;

    EXPECT_EQ(params.globalElementSize, 3.5);
    EXPECT_EQ(params.minElementSize, 0.5);
    EXPECT_EQ(params.maxElementSize, 10.0);
    EXPECT_EQ(params.meshType, MeshType::Volume);
    EXPECT_EQ(params.elementOrder, ElementOrder::Quadratic);
    EXPECT_EQ(params.algorithm, MeshAlgorithm::Frontal);
    EXPECT_EQ(params.numOptimizationPasses, 5);
    EXPECT_EQ(params.minElementQuality, 0.3);
}

TEST(MeshParametersTest, SizeRelations) {
    MeshParameters params;
    params.globalElementSize = 5.0;
    params.minElementSize = 1.0;
    params.maxElementSize = 20.0;

    // Verify size relationships
    EXPECT_LT(params.minElementSize, params.globalElementSize);
    EXPECT_GT(params.maxElementSize, params.globalElementSize);
    EXPECT_LT(params.minElementSize, params.maxElementSize);
}
