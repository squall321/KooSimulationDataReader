#include <gtest/gtest.h>
#include <koo/util/StringUtils.hpp>

using namespace koo::util;

TEST(StringUtilsTest, Trim) {
    EXPECT_EQ(StringUtils::trim("  hello  "), "hello");
    EXPECT_EQ(StringUtils::trim("hello"), "hello");
    EXPECT_EQ(StringUtils::trim("  "), "");
    EXPECT_EQ(StringUtils::trim(""), "");
    EXPECT_EQ(StringUtils::trim("\t\nhello\r\n"), "hello");
}

TEST(StringUtilsTest, TrimLeft) {
    EXPECT_EQ(StringUtils::trimLeft("  hello  "), "hello  ");
    EXPECT_EQ(StringUtils::trimLeft("hello"), "hello");
}

TEST(StringUtilsTest, TrimRight) {
    EXPECT_EQ(StringUtils::trimRight("  hello  "), "  hello");
    EXPECT_EQ(StringUtils::trimRight("hello"), "hello");
}

TEST(StringUtilsTest, ToUpper) {
    EXPECT_EQ(StringUtils::toUpper("hello"), "HELLO");
    EXPECT_EQ(StringUtils::toUpper("Hello World"), "HELLO WORLD");
    EXPECT_EQ(StringUtils::toUpper("*MAT_ELASTIC"), "*MAT_ELASTIC");
}

TEST(StringUtilsTest, ToLower) {
    EXPECT_EQ(StringUtils::toLower("HELLO"), "hello");
    EXPECT_EQ(StringUtils::toLower("Hello World"), "hello world");
}

TEST(StringUtilsTest, SplitChar) {
    auto result = StringUtils::split("a,b,c", ',');
    ASSERT_EQ(result.size(), 3);
    EXPECT_EQ(result[0], "a");
    EXPECT_EQ(result[1], "b");
    EXPECT_EQ(result[2], "c");

    result = StringUtils::split("hello", ',');
    ASSERT_EQ(result.size(), 1);
    EXPECT_EQ(result[0], "hello");
}

TEST(StringUtilsTest, ParseInt) {
    EXPECT_EQ(StringUtils::parseInt("123"), 123);
    EXPECT_EQ(StringUtils::parseInt("-456"), -456);
    EXPECT_EQ(StringUtils::parseInt("  789  "), 789);
    EXPECT_EQ(StringUtils::parseInt(""), std::nullopt);
    EXPECT_EQ(StringUtils::parseInt("abc"), std::nullopt);
}

TEST(StringUtilsTest, ParseInt64) {
    EXPECT_EQ(StringUtils::parseInt64("123456789012"), 123456789012LL);
    EXPECT_EQ(StringUtils::parseInt64("-123456789012"), -123456789012LL);
}

TEST(StringUtilsTest, ParseDouble) {
    EXPECT_DOUBLE_EQ(*StringUtils::parseDouble("3.14"), 3.14);
    EXPECT_DOUBLE_EQ(*StringUtils::parseDouble("-2.5"), -2.5);
    EXPECT_DOUBLE_EQ(*StringUtils::parseDouble("1.0e-5"), 1.0e-5);
    EXPECT_DOUBLE_EQ(*StringUtils::parseDouble("1.0E+5"), 1.0e+5);

    // LS-DYNA format (without 'e')
    EXPECT_DOUBLE_EQ(*StringUtils::parseDouble("1.0-5"), 1.0e-5);
    EXPECT_DOUBLE_EQ(*StringUtils::parseDouble("2.5+3"), 2.5e+3);

    // Fortran-style 'd' exponent
    EXPECT_DOUBLE_EQ(*StringUtils::parseDouble("1.0d-5"), 1.0e-5);

    EXPECT_EQ(StringUtils::parseDouble(""), std::nullopt);
    EXPECT_EQ(StringUtils::parseDouble("   "), std::nullopt);
}

TEST(StringUtilsTest, FormatInt) {
    EXPECT_EQ(StringUtils::formatInt(123, 10), "       123");
    EXPECT_EQ(StringUtils::formatInt(-45, 10), "       -45");
    EXPECT_EQ(StringUtils::formatInt(1, 5), "    1");
}

TEST(StringUtilsTest, StartsWith) {
    EXPECT_TRUE(StringUtils::startsWith("hello world", "hello"));
    EXPECT_TRUE(StringUtils::startsWith("hello", "hello"));
    EXPECT_FALSE(StringUtils::startsWith("hello", "world"));
    EXPECT_FALSE(StringUtils::startsWith("hi", "hello"));
}

TEST(StringUtilsTest, EndsWith) {
    EXPECT_TRUE(StringUtils::endsWith("hello world", "world"));
    EXPECT_TRUE(StringUtils::endsWith("hello", "hello"));
    EXPECT_FALSE(StringUtils::endsWith("hello", "world"));
}

TEST(StringUtilsTest, Contains) {
    EXPECT_TRUE(StringUtils::contains("hello world", "lo wo"));
    EXPECT_TRUE(StringUtils::contains("hello", "hello"));
    EXPECT_FALSE(StringUtils::contains("hello", "xyz"));
}

TEST(StringUtilsTest, Replace) {
    EXPECT_EQ(StringUtils::replace("hello world", "world", "there"), "hello there");
    EXPECT_EQ(StringUtils::replace("hello world", "xyz", "abc"), "hello world");
}

TEST(StringUtilsTest, ReplaceAll) {
    EXPECT_EQ(StringUtils::replaceAll("a-b-c", "-", "_"), "a_b_c");
    EXPECT_EQ(StringUtils::replaceAll("aaa", "a", "bb"), "bbbbbb");
}
