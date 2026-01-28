// Unit tests for MeshQuality
// Phase 5b: Mesh Integration

#include <koo/mesh/MeshQuality.hpp>
#include <gtest/gtest.h>

using namespace koo::mesh;

TEST(MeshQualityTest, DefaultConstruction) {
    MeshQuality quality;

    // Default values should be reasonable
    EXPECT_EQ(quality.minAspectRatio, 0.0);
    EXPECT_EQ(quality.maxAspectRatio, 0.0);
    EXPECT_EQ(quality.avgAspectRatio, 0.0);
    EXPECT_EQ(quality.numDegenerateElements, 0);
    EXPECT_EQ(quality.numInvertedElements, 0);
    EXPECT_EQ(quality.numPoorQualityElements, 0);
}

TEST(MeshQualityTest, IsAcceptableGoodMesh) {
    MeshQuality quality;
    quality.minAspectRatio = 0.5;
    quality.maxAspectRatio = 0.95;
    quality.avgAspectRatio = 0.75;
    quality.minJacobian = 0.1;
    quality.maxJacobian = 1.0;
    quality.avgJacobian = 0.7;
    quality.numDegenerateElements = 0;
    quality.numInvertedElements = 0;
    quality.numPoorQualityElements = 0;

    EXPECT_TRUE(quality.isAcceptable());
}

TEST(MeshQualityTest, IsAcceptablePoorAspectRatio) {
    MeshQuality quality;
    quality.minAspectRatio = 0.05;  // Too low
    quality.maxAspectRatio = 0.95;
    quality.minJacobian = 0.1;
    quality.numDegenerateElements = 0;
    quality.numInvertedElements = 0;

    EXPECT_FALSE(quality.isAcceptable());
}

TEST(MeshQualityTest, IsAcceptablePoorJacobian) {
    MeshQuality quality;
    quality.minAspectRatio = 0.5;
    quality.maxAspectRatio = 0.95;
    quality.minJacobian = 0.005;  // Too low
    quality.numDegenerateElements = 0;
    quality.numInvertedElements = 0;

    EXPECT_FALSE(quality.isAcceptable());
}

TEST(MeshQualityTest, IsAcceptableWithInvertedElements) {
    MeshQuality quality;
    quality.minAspectRatio = 0.5;
    quality.minJacobian = 0.1;
    quality.numInvertedElements = 5;  // Has inverted elements

    EXPECT_FALSE(quality.isAcceptable());
}

TEST(MeshQualityTest, GetGradeExcellent) {
    MeshQuality quality;
    quality.minAspectRatio = 0.8;
    quality.avgAspectRatio = 0.9;
    quality.minJacobian = 0.5;
    quality.numDegenerateElements = 0;
    quality.numInvertedElements = 0;
    quality.numPoorQualityElements = 0;

    EXPECT_EQ(quality.getGrade(), 'A');
}

TEST(MeshQualityTest, GetGradeGood) {
    MeshQuality quality;
    quality.minAspectRatio = 0.5;
    quality.avgAspectRatio = 0.7;
    quality.minJacobian = 0.2;
    quality.numDegenerateElements = 0;
    quality.numInvertedElements = 0;
    quality.numPoorQualityElements = 1;

    EXPECT_EQ(quality.getGrade(), 'B');
}

TEST(MeshQualityTest, GetGradeAcceptable) {
    MeshQuality quality;
    quality.minAspectRatio = 0.3;
    quality.avgAspectRatio = 0.5;
    quality.minJacobian = 0.1;
    quality.numDegenerateElements = 0;
    quality.numInvertedElements = 0;
    quality.numPoorQualityElements = 5;

    EXPECT_EQ(quality.getGrade(), 'C');
}

TEST(MeshQualityTest, GetGradePoor) {
    MeshQuality quality;
    quality.minAspectRatio = 0.15;
    quality.avgAspectRatio = 0.3;
    quality.minJacobian = 0.05;
    quality.numDegenerateElements = 2;
    quality.numInvertedElements = 0;

    EXPECT_EQ(quality.getGrade(), 'D');
}

TEST(MeshQualityTest, GetGradeFailing) {
    MeshQuality quality;
    quality.minAspectRatio = 0.05;
    quality.avgAspectRatio = 0.2;
    quality.minJacobian = 0.02;
    quality.numInvertedElements = 5;

    EXPECT_EQ(quality.getGrade(), 'F');
}

TEST(MeshQualityTest, GetSummary) {
    MeshQuality quality;
    quality.minAspectRatio = 0.5;
    quality.maxAspectRatio = 0.95;
    quality.avgAspectRatio = 0.75;
    quality.minJacobian = 0.1;
    quality.maxJacobian = 1.0;
    quality.avgJacobian = 0.7;

    std::string summary = quality.getSummary();

    EXPECT_FALSE(summary.empty());
    EXPECT_NE(summary.find("Aspect Ratio"), std::string::npos);
    EXPECT_NE(summary.find("Jacobian"), std::string::npos);
}

TEST(MeshQualityTest, CustomThresholds) {
    MeshQuality quality;
    quality.minAspectRatio = 0.15;
    quality.minJacobian = 0.05;

    // Default thresholds
    EXPECT_FALSE(quality.isAcceptable());

    // Custom lower thresholds
    EXPECT_TRUE(quality.isAcceptable(0.1, 0.01));
}
