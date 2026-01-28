// Unit tests for Geometry class
// Phase 5a: CAD Integration

#include <koo/cad/Geometry.hpp>
#include <koo/cad/CADTypes.hpp>
#include <gtest/gtest.h>

using namespace koo::cad;
using namespace koo::common;

class GeometryTest : public ::testing::Test {
protected:
    void SetUp() override {
        geometry = std::make_shared<Geometry>();
    }

    std::shared_ptr<Geometry> geometry;
};

TEST_F(GeometryTest, DefaultConstruction) {
    EXPECT_EQ(geometry->getNumSolids(), 0);
    EXPECT_EQ(geometry->getNumFaces(), 0);
    EXPECT_EQ(geometry->getNumEdges(), 0);
    EXPECT_EQ(geometry->getNumVertices(), 0);
}

TEST_F(GeometryTest, AddSolid) {
    GeometryEntity solid;
    solid.type = EntityType::Solid;
    solid.id = 1;
    solid.bounds.min = Vec3(0, 0, 0);
    solid.bounds.max = Vec3(10, 10, 10);
    solid.nativeHandle = nullptr;

    geometry->addSolid(solid);

    EXPECT_EQ(geometry->getNumSolids(), 1);
    EXPECT_EQ(geometry->getSolids().size(), 1);
    EXPECT_EQ(geometry->getSolids()[0].id, 1);
    EXPECT_EQ(geometry->getSolids()[0].type, EntityType::Solid);
}

TEST_F(GeometryTest, AddMultipleSolids) {
    for (int i = 1; i <= 5; ++i) {
        GeometryEntity solid;
        solid.type = EntityType::Solid;
        solid.id = i;
        solid.bounds.min = Vec3(0, 0, 0);
        solid.bounds.max = Vec3(i * 10, i * 10, i * 10);
        geometry->addSolid(solid);
    }

    EXPECT_EQ(geometry->getNumSolids(), 5);
}

TEST_F(GeometryTest, AddFace) {
    GeometryEntity face;
    face.type = EntityType::Face;
    face.id = 1;
    face.bounds.min = Vec3(0, 0, 0);
    face.bounds.max = Vec3(10, 10, 0);

    geometry->addFace(face);

    EXPECT_EQ(geometry->getNumFaces(), 1);
    EXPECT_EQ(geometry->getFaces()[0].type, EntityType::Face);
}

TEST_F(GeometryTest, BoundingBox) {
    // Add solids with known bounds
    GeometryEntity solid1;
    solid1.type = EntityType::Solid;
    solid1.id = 1;
    solid1.bounds.min = Vec3(0, 0, 0);
    solid1.bounds.max = Vec3(10, 10, 10);
    geometry->addSolid(solid1);

    GeometryEntity solid2;
    solid2.type = EntityType::Solid;
    solid2.id = 2;
    solid2.bounds.min = Vec3(5, 5, 5);
    solid2.bounds.max = Vec3(20, 20, 20);
    geometry->addSolid(solid2);

    // Bounding box should encompass all solids
    auto bbox = geometry->getBoundingBox();
    EXPECT_EQ(bbox.min.x, 0);
    EXPECT_EQ(bbox.min.y, 0);
    EXPECT_EQ(bbox.min.z, 0);
    EXPECT_EQ(bbox.max.x, 20);
    EXPECT_EQ(bbox.max.y, 20);
    EXPECT_EQ(bbox.max.z, 20);
}

TEST_F(GeometryTest, Clear) {
    // Add some data
    GeometryEntity solid;
    solid.type = EntityType::Solid;
    solid.id = 1;
    geometry->addSolid(solid);

    GeometryEntity face;
    face.type = EntityType::Face;
    face.id = 1;
    geometry->addFace(face);

    EXPECT_GT(geometry->getNumSolids(), 0);
    EXPECT_GT(geometry->getNumFaces(), 0);

    // Clear
    geometry->clear();

    EXPECT_EQ(geometry->getNumSolids(), 0);
    EXPECT_EQ(geometry->getNumFaces(), 0);
    EXPECT_EQ(geometry->getNumEdges(), 0);
    EXPECT_EQ(geometry->getNumVertices(), 0);
}

TEST_F(GeometryTest, NativeGeometry) {
    // Should start as null
    EXPECT_EQ(geometry->getNativeGeometry(), nullptr);

    // Set some arbitrary pointer (just for testing)
    int dummy = 42;
    geometry->setNativeGeometry(&dummy);

    EXPECT_NE(geometry->getNativeGeometry(), nullptr);
}
