#include <gtest/gtest.h>
#include <koo/dyna/KeywordFileReader.hpp>
#include <koo/dyna/KeywordFileWriter.hpp>
#include <koo/util/CardParser.hpp>

using namespace koo::dyna;
using namespace koo::util;

TEST(KeywordFileWriterTest, WriteToString) {
    Model model;
    model.setTitle("Test Model");

    auto& nodes = model.getOrCreateNodes();
    nodes.addNode(1, 0.0, 0.0, 0.0);
    nodes.addNode(2, 1.0, 0.0, 0.0);

    KeywordFileWriter writer;
    std::string output = writer.writeToString(model);

    EXPECT_FALSE(output.empty());
    EXPECT_NE(output.find("*KEYWORD"), std::string::npos);
    EXPECT_NE(output.find("*TITLE"), std::string::npos);
    EXPECT_NE(output.find("Test Model"), std::string::npos);
    EXPECT_NE(output.find("*NODE"), std::string::npos);
    EXPECT_NE(output.find("*END"), std::string::npos);
}

TEST(KeywordFileWriterTest, RoundTrip) {
    // Create original model
    Model original;
    original.setTitle("Round Trip Test");

    auto& nodes = original.getOrCreateNodes();
    nodes.addNode(1, 0.0, 0.0, 0.0);
    nodes.addNode(2, 1.0, 0.0, 0.0);
    nodes.addNode(3, 1.0, 1.0, 0.0);
    nodes.addNode(4, 0.0, 1.0, 0.0);

    auto& shells = original.getOrCreateShellElements();
    shells.addElement(1, 1, 1, 2, 3, 4);

    auto& parts = original.getOrCreateParts();
    parts.addPart(1, 1, 1, "Shell Part");

    // Write to string
    KeywordFileWriter writer;
    std::string output = writer.writeToString(original);

    // Read back
    KeywordFileReader reader;
    Model parsed = reader.readFromString(output);

    // Verify
    EXPECT_FALSE(reader.hasErrors());
    EXPECT_EQ(parsed.getTitle(), "Round Trip Test");
    EXPECT_EQ(parsed.getNodeCount(), 4);
    EXPECT_EQ(parsed.getShellElementCount(), 1);
    EXPECT_EQ(parsed.getPartCount(), 1);

    // Check node values
    auto* node = parsed.findNode(3);
    ASSERT_NE(node, nullptr);
    EXPECT_DOUBLE_EQ(node->position.x, 1.0);
    EXPECT_DOUBLE_EQ(node->position.y, 1.0);
    EXPECT_DOUBLE_EQ(node->position.z, 0.0);
}

TEST(KeywordFileWriterTest, WriterOptions) {
    Model model;
    model.setTitle("Options Test");

    auto& nodes = model.getOrCreateNodes();
    nodes.addNode(1, 0.0, 0.0, 0.0);

    // Test with different options
    WriterOptions options;
    options.writeKeywordDirective = false;
    options.writeEnd = false;

    KeywordFileWriter writer(options);
    std::string output = writer.writeToString(model);

    EXPECT_EQ(output.find("*KEYWORD"), std::string::npos);
    EXPECT_EQ(output.find("*END"), std::string::npos);
    EXPECT_NE(output.find("*TITLE"), std::string::npos);
}

TEST(KeywordFileWriterTest, LargeFormat) {
    Model model;

    auto& nodes = model.getOrCreateNodes();
    nodes.addNode(1, 0.0, 0.0, 0.0);

    WriterOptions options;
    options.format = CardParser::Format::Large;

    KeywordFileWriter writer(options);
    std::string output = writer.writeToString(model);

    EXPECT_NE(output.find("LONG=S"), std::string::npos);
    EXPECT_NE(output.find("*NODE+"), std::string::npos);
}

TEST(KeywordFileWriterTest, NoTitle) {
    Model model;

    auto& nodes = model.getOrCreateNodes();
    nodes.addNode(1, 0.0, 0.0, 0.0);

    KeywordFileWriter writer;
    std::string output = writer.writeToString(model);

    // Should not have *TITLE if title is empty
    EXPECT_EQ(output.find("*TITLE"), std::string::npos);
}

TEST(KeywordFileWriterTest, MultipleKeywords) {
    Model model;
    model.setTitle("Multi Keyword Test");

    auto& nodes = model.getOrCreateNodes();
    nodes.addNode(1, 0.0, 0.0, 0.0);

    auto& shells = model.getOrCreateShellElements();
    shells.addElement(1, 1, 1, 2, 3, 4);

    auto mat = std::make_unique<MatElastic>();
    mat->setMaterialId(1);
    mat->getData().e = 210000.0;
    model.addKeyword(std::move(mat));

    KeywordFileWriter writer;
    std::string output = writer.writeToString(model);

    EXPECT_NE(output.find("*NODE"), std::string::npos);
    EXPECT_NE(output.find("*ELEMENT_SHELL"), std::string::npos);
    EXPECT_NE(output.find("*MAT_ELASTIC"), std::string::npos);
}
