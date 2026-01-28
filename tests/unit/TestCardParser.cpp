#include <gtest/gtest.h>
#include <koo/util/CardParser.hpp>

using namespace koo::util;

TEST(CardParserTest, StandardFormat) {
    CardParser parser(CardParser::Format::Standard);

    // Standard format: 10 columns per field
    parser.setLine("       123     1.500     2.500     3.500");

    EXPECT_EQ(parser.getInt(0), 123);
    EXPECT_DOUBLE_EQ(*parser.getDouble(1), 1.5);
    EXPECT_DOUBLE_EQ(*parser.getDouble(2), 2.5);
    EXPECT_DOUBLE_EQ(*parser.getDouble(3), 3.5);
}

TEST(CardParserTest, LargeFormat) {
    CardParser parser(CardParser::Format::Large);

    // LONG=S format: integers 10 cols, reals 20 cols
    // Test position-based access for mixed int/real fields
    // Layout: int(10) + real(20) = "       123          1.500000000"
    std::string line = "       123          1.500000000";
    parser.setLine(line);

    // Position-based access
    EXPECT_EQ(parser.getInt64At(0), 123);
    EXPECT_DOUBLE_EQ(parser.getDoubleAt(10, 20).value_or(0.0), 1.5);

    // Verify widths
    EXPECT_EQ(parser.getIntWidth(), 10);
    EXPECT_EQ(parser.getRealWidth(), 20);
}

TEST(CardParserTest, DefaultValues) {
    CardParser parser;
    parser.setLine("       123");

    EXPECT_EQ(parser.getIntOr(0, 0), 123);
    EXPECT_EQ(parser.getIntOr(1, 999), 999);
    EXPECT_DOUBLE_EQ(parser.getDoubleOr(1, 0.5), 0.5);
}

TEST(CardParserTest, IsKeywordLine) {
    EXPECT_TRUE(CardParser::isKeywordLine("*NODE"));
    EXPECT_TRUE(CardParser::isKeywordLine("*ELEMENT_SHELL"));
    EXPECT_TRUE(CardParser::isKeywordLine("  *MAT_ELASTIC"));
    EXPECT_FALSE(CardParser::isKeywordLine("$ comment"));
    EXPECT_FALSE(CardParser::isKeywordLine("123 456"));
}

TEST(CardParserTest, ExtractKeyword) {
    EXPECT_EQ(CardParser::extractKeyword("*NODE"), "*NODE");
    EXPECT_EQ(CardParser::extractKeyword("*ELEMENT_SHELL"), "*ELEMENT_SHELL");
    EXPECT_EQ(CardParser::extractKeyword("*mat_elastic"), "*MAT_ELASTIC");
    EXPECT_EQ(CardParser::extractKeyword("*KEYWORD LONG=S"), "*KEYWORD");
    EXPECT_EQ(CardParser::extractKeyword("*TITLE$comment"), "*TITLE");
}

TEST(CardParserTest, IsCommentLine) {
    EXPECT_TRUE(CardParser::isCommentLine("$ This is a comment"));
    EXPECT_TRUE(CardParser::isCommentLine("  $comment"));
    EXPECT_TRUE(CardParser::isCommentLine(""));
    EXPECT_TRUE(CardParser::isCommentLine("   "));
    EXPECT_FALSE(CardParser::isCommentLine("*NODE"));
    EXPECT_FALSE(CardParser::isCommentLine("123 456"));
}

TEST(CardParserTest, IsLargeFormat) {
    EXPECT_TRUE(CardParser::isLargeFormat("*NODE+"));
    EXPECT_TRUE(CardParser::isLargeFormat("*ELEMENT_SHELL+"));
    EXPECT_FALSE(CardParser::isLargeFormat("*NODE"));
    EXPECT_FALSE(CardParser::isLargeFormat("*ELEMENT_SHELL"));
}

TEST(CardParserTest, IsFieldEmpty) {
    CardParser parser;
    parser.setLine("       123          ");

    EXPECT_FALSE(parser.isFieldEmpty(0));
    EXPECT_TRUE(parser.isFieldEmpty(1));
}

TEST(CardWriterTest, WriteInt) {
    CardWriter writer;
    writer.writeInt(123);
    writer.writeInt(456);

    std::string line = writer.getLine();
    EXPECT_EQ(line.length(), 20);

    CardParser parser;
    parser.setLine(line);
    EXPECT_EQ(parser.getInt(0), 123);
    EXPECT_EQ(parser.getInt(1), 456);
}

TEST(CardWriterTest, WriteDouble) {
    CardWriter writer;
    writer.writeDouble(1.5);
    writer.writeDouble(-2.5);

    std::string line = writer.getLine();

    CardParser parser;
    parser.setLine(line);
    EXPECT_DOUBLE_EQ(*parser.getDouble(0), 1.5);
    EXPECT_DOUBLE_EQ(*parser.getDouble(1), -2.5);
}

TEST(CardWriterTest, NewLine) {
    CardWriter writer;
    writer.writeInt(1);
    writer.writeInt(2);
    writer.newLine();
    writer.writeInt(3);

    auto lines = writer.getLines();
    ASSERT_EQ(lines.size(), 2);

    CardParser parser;
    parser.setLine(lines[0]);
    EXPECT_EQ(parser.getInt(0), 1);
    EXPECT_EQ(parser.getInt(1), 2);

    parser.setLine(lines[1]);
    EXPECT_EQ(parser.getInt(0), 3);
}

TEST(CardWriterTest, LargeFormat) {
    CardWriter writer(CardParser::Format::Large);
    writer.writeInt(123);      // 10 chars (integers always 10)
    writer.writeDouble(1.5);   // 20 chars (reals 20 in Large format)

    std::string line = writer.getLine();
    // int(10) + real(20) = 30 chars
    EXPECT_EQ(line.length(), 30);

    // Verify we can read back using position-based access
    CardParser parser(CardParser::Format::Large);
    parser.setLine(line);
    EXPECT_EQ(parser.getInt64At(0), 123);
    EXPECT_DOUBLE_EQ(parser.getDoubleAt(10, 20).value_or(0.0), 1.5);
}
