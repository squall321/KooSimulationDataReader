#include <gtest/gtest.h>
#include <koo/dyna/KeywordFileReader.hpp>
#include <koo/dyna/KeywordFactory.hpp>

using namespace koo::dyna;

TEST(KeywordFileReaderTest, ReadFromString) {
    std::string content = R"(
*KEYWORD
*TITLE
Simple Model
*NODE
         1       0.0       0.0       0.0
         2       1.0       0.0       0.0
         3       1.0       1.0       0.0
         4       0.0       1.0       0.0
*ELEMENT_SHELL
         1         1         1         2         3         4
*PART
Shell Part
         1         1         1
*SECTION_SHELL
         1         2       1.0         2
       1.5       1.5       1.5       1.5
*MAT_ELASTIC
         1   7.85-9    2.1+5       0.3
*END
)";

    KeywordFileReader reader;
    Model model = reader.readFromString(content);

    EXPECT_FALSE(reader.hasErrors());
    EXPECT_EQ(model.getTitle(), "Simple Model");
    EXPECT_EQ(model.getNodeCount(), 4);
    EXPECT_EQ(model.getShellElementCount(), 1);
    EXPECT_EQ(model.getPartCount(), 1);
    EXPECT_EQ(model.getSections().size(), 1);
    EXPECT_EQ(model.getMaterials().size(), 1);
}

TEST(KeywordFileReaderTest, ParseNodes) {
    std::string content = R"(
*NODE
         1       0.0       0.0       0.0
         2       1.0       2.0       3.0
)";

    KeywordFileReader reader;
    Model model = reader.readFromString(content);

    EXPECT_EQ(model.getNodeCount(), 2);

    auto* node = model.findNode(2);
    ASSERT_NE(node, nullptr);
    EXPECT_DOUBLE_EQ(node->position.x, 1.0);
    EXPECT_DOUBLE_EQ(node->position.y, 2.0);
    EXPECT_DOUBLE_EQ(node->position.z, 3.0);
}

TEST(KeywordFileReaderTest, ParseShellElements) {
    std::string content = R"(
*ELEMENT_SHELL
         1         1         1         2         3         4
         2         2         5         6         7         8
)";

    KeywordFileReader reader;
    Model model = reader.readFromString(content);

    EXPECT_EQ(model.getShellElementCount(), 2);

    auto* shells = model.getShellElements();
    ASSERT_NE(shells, nullptr);

    auto* elem = shells->getElement(2);
    ASSERT_NE(elem, nullptr);
    EXPECT_EQ(elem->pid, 2);
}

TEST(KeywordFileReaderTest, ParseMaterial) {
    std::string content = R"(
*MAT_ELASTIC
         1   2700.0   70000.0      0.33
)";

    KeywordFileReader reader;
    Model model = reader.readFromString(content);

    auto* mat = model.findMaterial(1);
    ASSERT_NE(mat, nullptr);
    EXPECT_EQ(mat->getMaterialType(), MaterialType::Elastic);

    auto* elastic = dynamic_cast<MatElastic*>(mat);
    ASSERT_NE(elastic, nullptr);
    EXPECT_DOUBLE_EQ(elastic->getData().ro, 2700.0);
    EXPECT_DOUBLE_EQ(elastic->getData().e, 70000.0);
    EXPECT_DOUBLE_EQ(elastic->getData().pr, 0.33);
}

TEST(KeywordFileReaderTest, ParseComments) {
    std::string content = R"(
$ This is a comment
*NODE
$ Another comment
         1       0.0       0.0       0.0
$---+----1----+----2----+----3
         2       1.0       0.0       0.0
)";

    KeywordFileReader reader;
    Model model = reader.readFromString(content);

    EXPECT_EQ(model.getNodeCount(), 2);
}

TEST(KeywordFileReaderTest, UnknownKeyword) {
    std::string content = R"(
*UNKNOWN_KEYWORD
some data
more data
*NODE
         1       0.0       0.0       0.0
)";

    KeywordFileReader reader;
    Model model = reader.readFromString(content);

    // Unknown keyword should be stored as GenericKeyword
    EXPECT_EQ(model.getKeywords().size(), 2);
    EXPECT_EQ(model.getNodeCount(), 1);
}

TEST(KeywordFileReaderTest, LargeFormatKeyword) {
    // LONG=S format: nid(I10), x(E20), y(E20), z(E20), tc(I10), rc(I10)
    // Total: 10 + 20 + 20 + 20 + 10 + 10 = 90 chars per line
    std::string content = R"(*KEYWORD LONG=S
*NODE+
         1                 0.0                 0.0                 0.0         0         0
         2               100.0                 0.0                 0.0         0         0
)";

    KeywordFileReader reader;
    Model model = reader.readFromString(content);

    EXPECT_EQ(model.getNodeCount(), 2);

    // Verify values were parsed correctly
    auto* node1 = model.findNode(1);
    ASSERT_NE(node1, nullptr);
    EXPECT_DOUBLE_EQ(node1->position.x, 0.0);

    auto* node2 = model.findNode(2);
    ASSERT_NE(node2, nullptr);
    EXPECT_DOUBLE_EQ(node2->position.x, 100.0);
}

TEST(KeywordFileReaderTest, KeywordWithTitle) {
    std::string content = R"(
*TITLE
My Test Model
*NODE
         1       0.0       0.0       0.0
)";

    KeywordFileReader reader;
    Model model = reader.readFromString(content);

    EXPECT_EQ(model.getTitle(), "My Test Model");
}

TEST(KeywordFileReaderTest, EndKeyword) {
    std::string content = R"(
*NODE
         1       0.0       0.0       0.0
*END
*NODE
         2       1.0       0.0       0.0
)";

    KeywordFileReader reader;
    Model model = reader.readFromString(content);

    // Only first node should be read (before *END)
    EXPECT_EQ(model.getNodeCount(), 1);
}

TEST(KeywordFactoryTest, RegisteredKeywords) {
    auto& factory = KeywordFactory::instance();

    EXPECT_TRUE(factory.isRegistered("*NODE"));
    EXPECT_TRUE(factory.isRegistered("*ELEMENT_SHELL"));
    EXPECT_TRUE(factory.isRegistered("*ELEMENT_SOLID"));
    EXPECT_TRUE(factory.isRegistered("*PART"));
    EXPECT_TRUE(factory.isRegistered("*MAT_ELASTIC"));
    EXPECT_TRUE(factory.isRegistered("*MAT_RIGID"));
    EXPECT_TRUE(factory.isRegistered("*SECTION_SHELL"));
    EXPECT_TRUE(factory.isRegistered("*SECTION_SOLID"));

    // Aliases
    EXPECT_TRUE(factory.isRegistered("*MAT_001"));
    EXPECT_TRUE(factory.isRegistered("*MAT_020"));
}

TEST(KeywordFactoryTest, CreateKeyword) {
    auto& factory = KeywordFactory::instance();

    auto node = factory.create("*NODE");
    ASSERT_NE(node, nullptr);
    EXPECT_EQ(node->getKeywordName(), "*NODE");

    auto shell = factory.create("*ELEMENT_SHELL");
    ASSERT_NE(shell, nullptr);
    EXPECT_EQ(shell->getKeywordName(), "*ELEMENT_SHELL");
}

TEST(KeywordFactoryTest, CreateUnknownKeyword) {
    auto& factory = KeywordFactory::instance();

    auto unknown = factory.create("*UNKNOWN_XYZ");
    ASSERT_NE(unknown, nullptr);

    // Unknown keywords become GenericKeyword
    auto* generic = dynamic_cast<GenericKeyword*>(unknown.get());
    ASSERT_NE(generic, nullptr);
}
