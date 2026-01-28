#include <gtest/gtest.h>
#include <koo/ecad/Feature.hpp>

using namespace koo::ecad;

// ============================================================================
// LineFeature Tests
// ============================================================================

TEST(LineFeatureTest, CreateAndGetType) {
    LineFeature line;
    EXPECT_EQ(line.getType(), FeatureType::Line);
}

TEST(LineFeatureTest, SetStartEnd) {
    LineFeature line;
    line.setStart(0.0, 0.0);
    line.setEnd(10.0, 5.0);

    auto start = line.getStart();
    auto end = line.getEnd();

    EXPECT_DOUBLE_EQ(start.x, 0.0);
    EXPECT_DOUBLE_EQ(start.y, 0.0);
    EXPECT_DOUBLE_EQ(end.x, 10.0);
    EXPECT_DOUBLE_EQ(end.y, 5.0);
}

TEST(LineFeatureTest, SetSymbol) {
    LineFeature line;
    line.setSymbolName("r100");
    line.setSymbolIndex(5);

    EXPECT_EQ(line.getSymbolName(), "r100");
    EXPECT_EQ(line.getSymbolIndex(), 5);
}

TEST(LineFeatureTest, Polarity) {
    LineFeature line;
    EXPECT_EQ(line.getPolarity(), Polarity::Positive);  // default

    line.setPolarity(Polarity::Negative);
    EXPECT_EQ(line.getPolarity(), Polarity::Negative);
}

TEST(LineFeatureTest, GetBoundingBox) {
    LineFeature line;
    line.setStart(5.0, 10.0);
    line.setEnd(15.0, 20.0);

    auto bbox = line.getBoundingBox();
    EXPECT_DOUBLE_EQ(bbox.min.x, 5.0);
    EXPECT_DOUBLE_EQ(bbox.min.y, 10.0);
    EXPECT_DOUBLE_EQ(bbox.max.x, 15.0);
    EXPECT_DOUBLE_EQ(bbox.max.y, 20.0);
}

TEST(LineFeatureTest, Clone) {
    LineFeature line;
    line.setStart(1.0, 2.0);
    line.setEnd(3.0, 4.0);
    line.setSymbolName("round");
    line.setPolarity(Polarity::Negative);

    auto cloned = line.clone();
    auto* lineClone = dynamic_cast<LineFeature*>(cloned.get());

    ASSERT_NE(lineClone, nullptr);
    EXPECT_EQ(lineClone->getType(), FeatureType::Line);
    EXPECT_EQ(lineClone->getStart().x, 1.0);
    EXPECT_EQ(lineClone->getEnd().x, 3.0);
    EXPECT_EQ(lineClone->getSymbolName(), "round");
    EXPECT_EQ(lineClone->getPolarity(), Polarity::Negative);
}

// ============================================================================
// PadFeature Tests
// ============================================================================

TEST(PadFeatureTest, CreateAndGetType) {
    PadFeature pad;
    EXPECT_EQ(pad.getType(), FeatureType::Pad);
}

TEST(PadFeatureTest, SetPosition) {
    PadFeature pad;
    pad.setPosition(25.4, 12.7);

    auto pos = pad.getPosition();
    EXPECT_DOUBLE_EQ(pos.x, 25.4);
    EXPECT_DOUBLE_EQ(pos.y, 12.7);
}

TEST(PadFeatureTest, SetRotation) {
    PadFeature pad;
    pad.setRotation(45.0);
    EXPECT_DOUBLE_EQ(pad.getRotation(), 45.0);
}

TEST(PadFeatureTest, SetMirror) {
    PadFeature pad;
    EXPECT_FALSE(pad.isMirrored());

    pad.setMirrored(true);
    EXPECT_TRUE(pad.isMirrored());
}

TEST(PadFeatureTest, SetResizeFactor) {
    PadFeature pad;
    pad.setResizeFactor(0.1);
    pad.setHasResize(true);
    EXPECT_DOUBLE_EQ(pad.getResizeFactor(), 0.1);
    EXPECT_TRUE(pad.hasResize());
}

TEST(PadFeatureTest, Clone) {
    PadFeature pad;
    pad.setPosition(10.0, 20.0);
    pad.setRotation(90.0);
    pad.setSymbolName("rect50x100");

    auto cloned = pad.clone();
    auto* padClone = dynamic_cast<PadFeature*>(cloned.get());

    ASSERT_NE(padClone, nullptr);
    EXPECT_EQ(padClone->getType(), FeatureType::Pad);
    EXPECT_DOUBLE_EQ(padClone->getPosition().x, 10.0);
    EXPECT_DOUBLE_EQ(padClone->getRotation(), 90.0);
}

// ============================================================================
// ArcFeature Tests
// ============================================================================

TEST(ArcFeatureTest, CreateAndGetType) {
    ArcFeature arc;
    EXPECT_EQ(arc.getType(), FeatureType::Arc);
}

TEST(ArcFeatureTest, SetStartEndCenter) {
    ArcFeature arc;
    arc.setStart(0.0, 0.0);
    arc.setEnd(10.0, 0.0);
    arc.setCenter(5.0, 0.0);

    EXPECT_DOUBLE_EQ(arc.getStart().x, 0.0);
    EXPECT_DOUBLE_EQ(arc.getEnd().x, 10.0);
    EXPECT_DOUBLE_EQ(arc.getCenter().x, 5.0);
}

TEST(ArcFeatureTest, SetClockwise) {
    ArcFeature arc;
    EXPECT_FALSE(arc.isClockwise());  // default CCW

    arc.setClockwise(true);
    EXPECT_TRUE(arc.isClockwise());
}

TEST(ArcFeatureTest, Clone) {
    ArcFeature arc;
    arc.setStart(0.0, 5.0);
    arc.setEnd(5.0, 0.0);
    arc.setCenter(0.0, 0.0);
    arc.setClockwise(true);

    auto cloned = arc.clone();
    auto* arcClone = dynamic_cast<ArcFeature*>(cloned.get());

    ASSERT_NE(arcClone, nullptr);
    EXPECT_TRUE(arcClone->isClockwise());
    EXPECT_DOUBLE_EQ(arcClone->getCenter().x, 0.0);
}

// ============================================================================
// TextFeature Tests
// ============================================================================

TEST(TextFeatureTest, CreateAndGetType) {
    TextFeature text;
    EXPECT_EQ(text.getType(), FeatureType::Text);
}

TEST(TextFeatureTest, SetText) {
    TextFeature text;
    text.setText("U1");
    EXPECT_EQ(text.getText(), "U1");
}

TEST(TextFeatureTest, SetPosition) {
    TextFeature text;
    text.setPosition(15.0, 25.0);

    auto pos = text.getPosition();
    EXPECT_DOUBLE_EQ(pos.x, 15.0);
    EXPECT_DOUBLE_EQ(pos.y, 25.0);
}

TEST(TextFeatureTest, SetFont) {
    TextFeature text;
    text.setFont("standard");
    EXPECT_EQ(text.getFont(), "standard");
}

TEST(TextFeatureTest, SetCharacterSize) {
    TextFeature text;
    text.setSize(1.27, 2.54);

    EXPECT_DOUBLE_EQ(text.getXSize(), 1.27);
    EXPECT_DOUBLE_EQ(text.getYSize(), 2.54);
}

TEST(TextFeatureTest, SetWidthFactor) {
    TextFeature text;
    text.setWidthFactor(0.8);
    EXPECT_DOUBLE_EQ(text.getWidthFactor(), 0.8);
}

TEST(TextFeatureTest, SetRotation) {
    TextFeature text;
    text.setRotation(270.0);
    EXPECT_DOUBLE_EQ(text.getRotation(), 270.0);
}

TEST(TextFeatureTest, SetMirror) {
    TextFeature text;
    text.setMirrored(true);
    EXPECT_TRUE(text.isMirrored());
}

TEST(TextFeatureTest, SetVersion) {
    TextFeature text;
    text.setVersion(2);
    EXPECT_EQ(text.getVersion(), 2);
}

TEST(TextFeatureTest, Clone) {
    TextFeature text;
    text.setText("REF");
    text.setPosition(10.0, 20.0);
    text.setRotation(90.0);

    auto cloned = text.clone();
    auto* textClone = dynamic_cast<TextFeature*>(cloned.get());

    ASSERT_NE(textClone, nullptr);
    EXPECT_EQ(textClone->getText(), "REF");
    EXPECT_DOUBLE_EQ(textClone->getRotation(), 90.0);
}

// ============================================================================
// Contour Tests
// ============================================================================

TEST(ContourTest, DefaultValues) {
    Contour contour;
    EXPECT_EQ(contour.getPolygonType(), PolygonType::Island);
    EXPECT_TRUE(contour.getSegments().empty());
}

TEST(ContourTest, SetStart) {
    Contour contour;
    contour.setStart(5.0, 10.0);

    auto start = contour.getStart();
    EXPECT_DOUBLE_EQ(start.x, 5.0);
    EXPECT_DOUBLE_EQ(start.y, 10.0);
}

TEST(ContourTest, SetPolygonType) {
    Contour contour;
    contour.setPolygonType(PolygonType::Hole);
    EXPECT_EQ(contour.getPolygonType(), PolygonType::Hole);
}

TEST(ContourTest, AddLineSegment) {
    Contour contour;
    contour.setStart(0, 0);
    contour.addLineSegment(10, 0);
    contour.addLineSegment(10, 10);
    contour.addLineSegment(0, 10);

    EXPECT_EQ(contour.getSegments().size(), 3);
    EXPECT_EQ(contour.getSegments()[0].type, ContourSegmentType::Line);
}

TEST(ContourTest, AddArcSegment) {
    Contour contour;
    contour.setStart(0, 0);
    contour.addArcSegment(10, 0, 5, 0, true);

    EXPECT_EQ(contour.getSegments().size(), 1);
    EXPECT_EQ(contour.getSegments()[0].type, ContourSegmentType::Arc);
    EXPECT_TRUE(contour.getSegments()[0].clockwise);
}

// ============================================================================
// SurfaceFeature Tests
// ============================================================================

TEST(SurfaceFeatureTest, CreateAndGetType) {
    SurfaceFeature surface;
    EXPECT_EQ(surface.getType(), FeatureType::Surface);
}

TEST(SurfaceFeatureTest, AddContour) {
    SurfaceFeature surface;

    Contour outer(0.0, 0.0, PolygonType::Island);
    outer.addLineSegment(10, 0);
    outer.addLineSegment(10, 10);
    outer.addLineSegment(0, 10);
    surface.addContour(outer);

    Contour hole(2.0, 2.0, PolygonType::Hole);
    hole.addLineSegment(8, 2);
    hole.addLineSegment(8, 8);
    hole.addLineSegment(2, 8);
    surface.addContour(hole);

    EXPECT_EQ(surface.getContours().size(), 2);
    EXPECT_EQ(surface.getContours()[0].getPolygonType(), PolygonType::Island);
    EXPECT_EQ(surface.getContours()[1].getPolygonType(), PolygonType::Hole);
}

TEST(SurfaceFeatureTest, Clone) {
    SurfaceFeature surface;

    Contour contour(0.0, 0.0, PolygonType::Island);
    contour.addLineSegment(5, 0);
    contour.addLineSegment(5, 5);
    contour.addLineSegment(0, 5);
    surface.addContour(contour);
    surface.setPolarity(Polarity::Negative);

    auto cloned = surface.clone();
    auto* surfClone = dynamic_cast<SurfaceFeature*>(cloned.get());

    ASSERT_NE(surfClone, nullptr);
    EXPECT_EQ(surfClone->getContours().size(), 1);
    EXPECT_EQ(surfClone->getPolarity(), Polarity::Negative);
}

// ============================================================================
// Feature Attributes Tests
// ============================================================================

TEST(FeatureTest, SetAndGetAttributes) {
    PadFeature pad;

    pad.setAttribute("net", "VCC");
    pad.setAttribute("test_point", "yes");

    EXPECT_EQ(pad.getAttribute("net"), "VCC");
    EXPECT_EQ(pad.getAttribute("test_point"), "yes");
    EXPECT_EQ(pad.getAttribute("nonexistent"), "");
}

TEST(FeatureTest, GetAttributeList) {
    ArcFeature arc;

    arc.setAttribute("key1", "value1");
    arc.setAttribute("key2", "value2");

    const auto& attrs = arc.getAttributes();
    EXPECT_EQ(attrs.size(), 2);
    EXPECT_EQ(attrs.at("key1"), "value1");
}

// ============================================================================
// Feature Dcode Tests
// ============================================================================

TEST(FeatureTest, SetAndGetDcode) {
    PadFeature pad;
    pad.setDcode(10);
    EXPECT_EQ(pad.getDcode(), 10);
}

// ============================================================================
// Feature Id Tests
// ============================================================================

TEST(FeatureTest, SetAndGetId) {
    LineFeature line;
    line.setId("feature_42");
    EXPECT_EQ(line.getId(), "feature_42");
}

// ============================================================================
// Feature NetName Tests
// ============================================================================

TEST(FeatureTest, SetAndGetNetName) {
    PadFeature pad;
    pad.setNetName("VCC");
    EXPECT_EQ(pad.getNetName(), "VCC");
}
