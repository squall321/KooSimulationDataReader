#include <gtest/gtest.h>
#include <koo/dyna/Section.hpp>

using namespace koo::dyna;

TEST(SectionShellTest, ParseAndWrite) {
    SectionShell sec;

    std::vector<std::string> lines = {
        "         1         2       1.0         5       0.0         0         0       1.0",
        "       1.5       1.5       1.5       1.5       0.0"
    };

    EXPECT_TRUE(sec.parse(lines));

    EXPECT_EQ(sec.getSectionId(), 1);
    EXPECT_EQ(sec.getElform(), 2);
    EXPECT_EQ(sec.getNip(), 5);
    EXPECT_DOUBLE_EQ(sec.getThickness(), 1.5);

    auto written = sec.write();
    EXPECT_EQ(written.size(), 2);

    SectionShell parsed;
    EXPECT_TRUE(parsed.parse(written));
    EXPECT_EQ(parsed.getSectionId(), 1);
}

TEST(SectionShellTest, SetThickness) {
    SectionShell sec;
    sec.setSectionId(1);
    sec.setThickness(2.0);

    EXPECT_DOUBLE_EQ(sec.getThickness(), 2.0);
}

TEST(SectionShellTest, Clone) {
    SectionShell sec;
    sec.setSectionId(5);
    sec.setThickness(1.0);

    auto cloned = sec.clone();
    auto* secClone = dynamic_cast<SectionShell*>(cloned.get());

    ASSERT_NE(secClone, nullptr);
    EXPECT_EQ(secClone->getSectionId(), 5);
    EXPECT_DOUBLE_EQ(secClone->getThickness(), 1.0);
}

TEST(SectionShellTest, SectionType) {
    SectionShell sec;
    EXPECT_EQ(sec.getSectionType(), SectionType::Shell);
    EXPECT_EQ(sec.getKeywordName(), "*SECTION_SHELL");
}

TEST(SectionSolidTest, ParseAndWrite) {
    SectionSolid sec;

    std::vector<std::string> lines = {
        "         1         1         0"
    };

    EXPECT_TRUE(sec.parse(lines));

    EXPECT_EQ(sec.getSectionId(), 1);
    EXPECT_EQ(sec.getElform(), 1);

    auto written = sec.write();
    EXPECT_EQ(written.size(), 1);

    SectionSolid parsed;
    EXPECT_TRUE(parsed.parse(written));
    EXPECT_EQ(parsed.getSectionId(), 1);
}

TEST(SectionSolidTest, Clone) {
    SectionSolid sec;
    sec.setSectionId(2);
    sec.setElform(2);

    auto cloned = sec.clone();
    auto* secClone = dynamic_cast<SectionSolid*>(cloned.get());

    ASSERT_NE(secClone, nullptr);
    EXPECT_EQ(secClone->getSectionId(), 2);
    EXPECT_EQ(secClone->getElform(), 2);
}

TEST(SectionSolidTest, SectionType) {
    SectionSolid sec;
    EXPECT_EQ(sec.getSectionType(), SectionType::Solid);
    EXPECT_EQ(sec.getKeywordName(), "*SECTION_SOLID");
}
