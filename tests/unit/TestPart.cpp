#include <gtest/gtest.h>
#include <koo/dyna/Part.hpp>

using namespace koo::dyna;

TEST(PartTest, AddAndGetPart) {
    Part parts;

    parts.addPart(1, 1, 1, "Part 1");
    parts.addPart(2, 2, 2, "Part 2");

    EXPECT_EQ(parts.getPartCount(), 2);
    EXPECT_TRUE(parts.hasPart(1));
    EXPECT_TRUE(parts.hasPart(2));
    EXPECT_FALSE(parts.hasPart(3));

    auto* part = parts.getPart(1);
    ASSERT_NE(part, nullptr);
    EXPECT_EQ(part->id, 1);
    EXPECT_EQ(part->secid, 1);
    EXPECT_EQ(part->mid, 1);
    EXPECT_EQ(part->title, "Part 1");
}

TEST(PartTest, RemovePart) {
    Part parts;

    parts.addPart(1, 1, 1, "Part 1");
    parts.addPart(2, 2, 2, "Part 2");

    parts.removePart(1);

    EXPECT_EQ(parts.getPartCount(), 1);
    EXPECT_FALSE(parts.hasPart(1));
    EXPECT_TRUE(parts.hasPart(2));
}

TEST(PartTest, Parse) {
    Part parts;

    std::vector<std::string> lines = {
        "Shell Part",
        "         1         1         1         0         0         0         0         0"
    };

    EXPECT_TRUE(parts.parse(lines));
    EXPECT_EQ(parts.getPartCount(), 1);

    auto* part = parts.getPart(1);
    ASSERT_NE(part, nullptr);
    EXPECT_EQ(part->title, "Shell Part");
    EXPECT_EQ(part->secid, 1);
    EXPECT_EQ(part->mid, 1);
}

TEST(PartTest, Write) {
    Part parts;

    parts.addPart(1, 1, 1, "Test Part");

    auto lines = parts.write();
    EXPECT_EQ(lines.size(), 2);  // Title + data

    Part parsed;
    EXPECT_TRUE(parsed.parse(lines));
    EXPECT_EQ(parsed.getPartCount(), 1);

    auto* part = parsed.getPart(1);
    ASSERT_NE(part, nullptr);
    EXPECT_EQ(part->title, "Test Part");
}

TEST(PartTest, Clone) {
    Part parts;
    parts.addPart(1, 1, 1, "Part 1");

    auto cloned = parts.clone();
    auto* partClone = dynamic_cast<Part*>(cloned.get());

    ASSERT_NE(partClone, nullptr);
    EXPECT_EQ(partClone->getPartCount(), 1);

    parts.addPart(2, 2, 2, "Part 2");
    EXPECT_EQ(partClone->getPartCount(), 1);
}
