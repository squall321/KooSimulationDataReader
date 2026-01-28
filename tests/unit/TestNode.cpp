#include <gtest/gtest.h>
#include <koo/dyna/Node.hpp>

using namespace koo::dyna;
using namespace koo;

TEST(NodeTest, AddAndGetNode) {
    Node nodes;

    nodes.addNode(1, 0.0, 0.0, 0.0);
    nodes.addNode(2, 1.0, 0.0, 0.0);
    nodes.addNode(3, 1.0, 1.0, 0.0);

    EXPECT_EQ(nodes.getNodeCount(), 3);
    EXPECT_TRUE(nodes.hasNode(1));
    EXPECT_TRUE(nodes.hasNode(2));
    EXPECT_TRUE(nodes.hasNode(3));
    EXPECT_FALSE(nodes.hasNode(4));

    auto* node = nodes.getNode(2);
    ASSERT_NE(node, nullptr);
    EXPECT_EQ(node->id, 2);
    EXPECT_DOUBLE_EQ(node->position.x, 1.0);
    EXPECT_DOUBLE_EQ(node->position.y, 0.0);
    EXPECT_DOUBLE_EQ(node->position.z, 0.0);
}

TEST(NodeTest, UpdateNode) {
    Node nodes;

    nodes.addNode(1, 0.0, 0.0, 0.0);
    nodes.addNode(1, 1.0, 2.0, 3.0);  // Update existing

    EXPECT_EQ(nodes.getNodeCount(), 1);

    auto* node = nodes.getNode(1);
    ASSERT_NE(node, nullptr);
    EXPECT_DOUBLE_EQ(node->position.x, 1.0);
    EXPECT_DOUBLE_EQ(node->position.y, 2.0);
    EXPECT_DOUBLE_EQ(node->position.z, 3.0);
}

TEST(NodeTest, RemoveNode) {
    Node nodes;

    nodes.addNode(1, 0.0, 0.0, 0.0);
    nodes.addNode(2, 1.0, 0.0, 0.0);
    nodes.addNode(3, 2.0, 0.0, 0.0);

    nodes.removeNode(2);

    EXPECT_EQ(nodes.getNodeCount(), 2);
    EXPECT_TRUE(nodes.hasNode(1));
    EXPECT_FALSE(nodes.hasNode(2));
    EXPECT_TRUE(nodes.hasNode(3));
}

TEST(NodeTest, Clear) {
    Node nodes;

    nodes.addNode(1, 0.0, 0.0, 0.0);
    nodes.addNode(2, 1.0, 0.0, 0.0);

    nodes.clear();

    EXPECT_EQ(nodes.getNodeCount(), 0);
    EXPECT_FALSE(nodes.hasNode(1));
}

TEST(NodeTest, Parse) {
    Node nodes;

    std::vector<std::string> lines = {
        "         1       0.0       0.0       0.0         0         0",
        "         2       1.0       0.0       0.0         0         0",
        "         3       1.0       1.0       0.0         0         0",
        "         4       0.0       1.0       0.0         0         0"
    };

    EXPECT_TRUE(nodes.parse(lines));
    EXPECT_EQ(nodes.getNodeCount(), 4);

    auto* node = nodes.getNode(3);
    ASSERT_NE(node, nullptr);
    EXPECT_DOUBLE_EQ(node->position.x, 1.0);
    EXPECT_DOUBLE_EQ(node->position.y, 1.0);
    EXPECT_DOUBLE_EQ(node->position.z, 0.0);
}

TEST(NodeTest, Write) {
    Node nodes;

    nodes.addNode(1, 0.0, 0.0, 0.0);
    nodes.addNode(2, 1.5, 2.5, 3.5);

    auto lines = nodes.write();
    EXPECT_EQ(lines.size(), 2);

    // Parse written output
    Node parsed;
    EXPECT_TRUE(parsed.parse(lines));
    EXPECT_EQ(parsed.getNodeCount(), 2);

    auto* node = parsed.getNode(2);
    ASSERT_NE(node, nullptr);
    EXPECT_DOUBLE_EQ(node->position.x, 1.5);
    EXPECT_DOUBLE_EQ(node->position.y, 2.5);
    EXPECT_DOUBLE_EQ(node->position.z, 3.5);
}

TEST(NodeTest, BoundingBox) {
    Node nodes;

    nodes.addNode(1, -1.0, -2.0, -3.0);
    nodes.addNode(2, 4.0, 5.0, 6.0);
    nodes.addNode(3, 0.0, 0.0, 0.0);

    auto bbox = nodes.getBoundingBox();

    EXPECT_DOUBLE_EQ(bbox.min.x, -1.0);
    EXPECT_DOUBLE_EQ(bbox.min.y, -2.0);
    EXPECT_DOUBLE_EQ(bbox.min.z, -3.0);
    EXPECT_DOUBLE_EQ(bbox.max.x, 4.0);
    EXPECT_DOUBLE_EQ(bbox.max.y, 5.0);
    EXPECT_DOUBLE_EQ(bbox.max.z, 6.0);
}

TEST(NodeTest, Transform) {
    Node nodes;

    nodes.addNode(1, 1.0, 2.0, 3.0);

    // Translation matrix
    Matrix4x4 translate = Matrix4x4::translation(10.0, 20.0, 30.0);
    nodes.transform(translate);

    auto* node = nodes.getNode(1);
    ASSERT_NE(node, nullptr);
    EXPECT_DOUBLE_EQ(node->position.x, 11.0);
    EXPECT_DOUBLE_EQ(node->position.y, 22.0);
    EXPECT_DOUBLE_EQ(node->position.z, 33.0);
}

TEST(NodeTest, Clone) {
    Node nodes;
    nodes.addNode(1, 1.0, 2.0, 3.0);
    nodes.addNode(2, 4.0, 5.0, 6.0);

    auto cloned = nodes.clone();
    auto* nodeClone = dynamic_cast<Node*>(cloned.get());

    ASSERT_NE(nodeClone, nullptr);
    EXPECT_EQ(nodeClone->getNodeCount(), 2);

    // Modify original
    nodes.addNode(3, 7.0, 8.0, 9.0);

    // Clone should not be affected
    EXPECT_EQ(nodeClone->getNodeCount(), 2);
}
