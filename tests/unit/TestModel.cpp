#include <gtest/gtest.h>
#include <koo/dyna/Model.hpp>

using namespace koo::dyna;
using namespace koo;

TEST(ModelTest, EmptyModel) {
    Model model;

    EXPECT_TRUE(model.getTitle().empty());
    EXPECT_EQ(model.getNodeCount(), 0);
    EXPECT_EQ(model.getShellElementCount(), 0);
    EXPECT_EQ(model.getSolidElementCount(), 0);
    EXPECT_EQ(model.getPartCount(), 0);
}

TEST(ModelTest, SetTitle) {
    Model model;
    model.setTitle("Test Model");

    EXPECT_EQ(model.getTitle(), "Test Model");
}

TEST(ModelTest, AddKeyword) {
    Model model;

    auto nodes = std::make_unique<Node>();
    nodes->addNode(1, 0.0, 0.0, 0.0);
    nodes->addNode(2, 1.0, 0.0, 0.0);

    model.addKeyword(std::move(nodes));

    EXPECT_EQ(model.getKeywords().size(), 1);
    EXPECT_EQ(model.getNodeCount(), 2);
}

TEST(ModelTest, GetOrCreateNodes) {
    Model model;

    auto& nodes = model.getOrCreateNodes();
    nodes.addNode(1, 0.0, 0.0, 0.0);

    EXPECT_EQ(model.getNodeCount(), 1);

    // Second call should return same object
    auto& nodes2 = model.getOrCreateNodes();
    EXPECT_EQ(&nodes, &nodes2);
}

TEST(ModelTest, GetOrCreateElements) {
    Model model;

    auto& shells = model.getOrCreateShellElements();
    shells.addElement(1, 1, 1, 2, 3, 4);

    EXPECT_EQ(model.getShellElementCount(), 1);

    auto& solids = model.getOrCreateSolidElements();
    solids.addElement(1, 1, 1, 2, 3, 4, 5, 6, 7, 8);

    EXPECT_EQ(model.getSolidElementCount(), 1);
    EXPECT_EQ(model.getTotalElementCount(), 2);
}

TEST(ModelTest, FindNode) {
    Model model;

    auto& nodes = model.getOrCreateNodes();
    nodes.addNode(1, 1.0, 2.0, 3.0);

    auto* found = model.findNode(1);
    ASSERT_NE(found, nullptr);
    EXPECT_DOUBLE_EQ(found->position.x, 1.0);

    auto* notFound = model.findNode(999);
    EXPECT_EQ(notFound, nullptr);
}

TEST(ModelTest, FindPart) {
    Model model;

    auto& parts = model.getOrCreateParts();
    parts.addPart(1, 1, 1, "Part 1");

    auto* found = model.findPart(1);
    ASSERT_NE(found, nullptr);
    EXPECT_EQ(found->title, "Part 1");

    auto* notFound = model.findPart(999);
    EXPECT_EQ(notFound, nullptr);
}

TEST(ModelTest, Materials) {
    Model model;

    auto mat1 = std::make_unique<MatElastic>();
    mat1->setMaterialId(1);

    auto mat2 = std::make_unique<MatRigid>();
    mat2->setMaterialId(2);

    model.addKeyword(std::move(mat1));
    model.addKeyword(std::move(mat2));

    auto materials = model.getMaterials();
    EXPECT_EQ(materials.size(), 2);

    auto* found = model.findMaterial(1);
    ASSERT_NE(found, nullptr);
    EXPECT_EQ(found->getMaterialType(), MaterialType::Elastic);
}

TEST(ModelTest, Sections) {
    Model model;

    auto sec = std::make_unique<SectionShell>();
    sec->setSectionId(1);
    sec->setThickness(1.5);

    model.addKeyword(std::move(sec));

    auto sections = model.getSections();
    EXPECT_EQ(sections.size(), 1);

    auto* found = model.findSection(1);
    ASSERT_NE(found, nullptr);
    EXPECT_EQ(found->getSectionType(), SectionType::Shell);
}

TEST(ModelTest, BoundingBox) {
    Model model;

    auto& nodes = model.getOrCreateNodes();
    nodes.addNode(1, -1.0, -2.0, -3.0);
    nodes.addNode(2, 4.0, 5.0, 6.0);

    auto bbox = model.getBoundingBox();
    EXPECT_TRUE(bbox.isValid());
    EXPECT_DOUBLE_EQ(bbox.min.x, -1.0);
    EXPECT_DOUBLE_EQ(bbox.max.x, 4.0);
}

TEST(ModelTest, Clone) {
    Model model;
    model.setTitle("Original");

    auto& nodes = model.getOrCreateNodes();
    nodes.addNode(1, 1.0, 2.0, 3.0);

    auto cloned = model.clone();

    EXPECT_EQ(cloned->getTitle(), "Original");
    EXPECT_EQ(cloned->getNodeCount(), 1);

    // Modify original
    model.setTitle("Modified");
    nodes.addNode(2, 4.0, 5.0, 6.0);

    // Clone should not be affected
    EXPECT_EQ(cloned->getTitle(), "Original");
    EXPECT_EQ(cloned->getNodeCount(), 1);
}

TEST(ModelTest, CopyConstructor) {
    Model model;
    model.setTitle("Original");

    auto& nodes = model.getOrCreateNodes();
    nodes.addNode(1, 1.0, 2.0, 3.0);

    Model copy(model);

    EXPECT_EQ(copy.getTitle(), "Original");
    EXPECT_EQ(copy.getNodeCount(), 1);

    // Modify original
    nodes.addNode(2, 4.0, 5.0, 6.0);

    // Copy should not be affected
    EXPECT_EQ(copy.getNodeCount(), 1);
}

TEST(ModelTest, Clear) {
    Model model;
    model.setTitle("Test");

    auto& nodes = model.getOrCreateNodes();
    nodes.addNode(1, 0.0, 0.0, 0.0);

    model.clear();

    EXPECT_TRUE(model.getTitle().empty());
    EXPECT_EQ(model.getKeywords().size(), 0);
    EXPECT_EQ(model.getNodeCount(), 0);
}
