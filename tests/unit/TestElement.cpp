#include <gtest/gtest.h>
#include <koo/dyna/Element.hpp>

using namespace koo::dyna;

TEST(ElementShellTest, AddAndGetElement) {
    ElementShell shells;

    shells.addElement(1, 1, 1, 2, 3, 4);
    shells.addElement(2, 1, 2, 3, 4, 5);

    EXPECT_EQ(shells.getElementCount(), 2);
    EXPECT_TRUE(shells.hasElement(1));
    EXPECT_TRUE(shells.hasElement(2));
    EXPECT_FALSE(shells.hasElement(3));

    auto* elem = shells.getElement(1);
    ASSERT_NE(elem, nullptr);
    EXPECT_EQ(elem->id, 1);
    EXPECT_EQ(elem->pid, 1);
    ASSERT_EQ(elem->nodeIds.size(), 4);
    EXPECT_EQ(elem->nodeIds[0], 1);
    EXPECT_EQ(elem->nodeIds[1], 2);
    EXPECT_EQ(elem->nodeIds[2], 3);
    EXPECT_EQ(elem->nodeIds[3], 4);
}

TEST(ElementShellTest, RemoveElement) {
    ElementShell shells;

    shells.addElement(1, 1, 1, 2, 3, 4);
    shells.addElement(2, 1, 2, 3, 4, 5);

    shells.removeElement(1);

    EXPECT_EQ(shells.getElementCount(), 1);
    EXPECT_FALSE(shells.hasElement(1));
    EXPECT_TRUE(shells.hasElement(2));
}

TEST(ElementShellTest, Parse) {
    ElementShell shells;

    std::vector<std::string> lines = {
        "         1         1         1         2         3         4",
        "         2         1         2         3         4         5"
    };

    EXPECT_TRUE(shells.parse(lines));
    EXPECT_EQ(shells.getElementCount(), 2);

    auto* elem = shells.getElement(2);
    ASSERT_NE(elem, nullptr);
    EXPECT_EQ(elem->nodeIds[0], 2);
}

TEST(ElementShellTest, Write) {
    ElementShell shells;

    shells.addElement(1, 1, 1, 2, 3, 4);
    shells.addElement(2, 2, 5, 6, 7, 8);

    auto lines = shells.write();
    EXPECT_EQ(lines.size(), 2);

    ElementShell parsed;
    EXPECT_TRUE(parsed.parse(lines));
    EXPECT_EQ(parsed.getElementCount(), 2);

    auto* elem = parsed.getElement(2);
    ASSERT_NE(elem, nullptr);
    EXPECT_EQ(elem->pid, 2);
}

TEST(ElementShellTest, Clone) {
    ElementShell shells;
    shells.addElement(1, 1, 1, 2, 3, 4);

    auto cloned = shells.clone();
    auto* shellClone = dynamic_cast<ElementShell*>(cloned.get());

    ASSERT_NE(shellClone, nullptr);
    EXPECT_EQ(shellClone->getElementCount(), 1);
}

TEST(ElementSolidTest, AddAndGetElement) {
    ElementSolid solids;

    solids.addElement(1, 1, 1, 2, 3, 4, 5, 6, 7, 8);

    EXPECT_EQ(solids.getElementCount(), 1);
    EXPECT_TRUE(solids.hasElement(1));

    auto* elem = solids.getElement(1);
    ASSERT_NE(elem, nullptr);
    EXPECT_EQ(elem->id, 1);
    EXPECT_EQ(elem->pid, 1);
    ASSERT_EQ(elem->nodeIds.size(), 8);
    EXPECT_EQ(elem->nodeIds[0], 1);
    EXPECT_EQ(elem->nodeIds[7], 8);
}

TEST(ElementSolidTest, Parse) {
    ElementSolid solids;

    std::vector<std::string> lines = {
        "         1         1         1         2         3         4         5         6         7         8"
    };

    EXPECT_TRUE(solids.parse(lines));
    EXPECT_EQ(solids.getElementCount(), 1);

    auto* elem = solids.getElement(1);
    ASSERT_NE(elem, nullptr);
    EXPECT_EQ(elem->nodeIds.size(), 8);
}

TEST(ElementSolidTest, Write) {
    ElementSolid solids;

    solids.addElement(1, 1, 1, 2, 3, 4, 5, 6, 7, 8);

    auto lines = solids.write();
    EXPECT_EQ(lines.size(), 1);

    ElementSolid parsed;
    EXPECT_TRUE(parsed.parse(lines));
    EXPECT_EQ(parsed.getElementCount(), 1);

    auto* elem = parsed.getElement(1);
    ASSERT_NE(elem, nullptr);
    EXPECT_EQ(elem->pid, 1);
    EXPECT_EQ(elem->nodeIds.size(), 8);
    EXPECT_EQ(elem->nodeIds[0], 1);
    EXPECT_EQ(elem->nodeIds[7], 8);
}

TEST(ElementSolidTest, Clone) {
    ElementSolid solids;
    solids.addElement(1, 1, 1, 2, 3, 4, 5, 6, 7, 8);

    auto cloned = solids.clone();
    auto* solidClone = dynamic_cast<ElementSolid*>(cloned.get());

    ASSERT_NE(solidClone, nullptr);
    EXPECT_EQ(solidClone->getElementCount(), 1);
}
