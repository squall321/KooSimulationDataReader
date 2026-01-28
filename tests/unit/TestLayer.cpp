#include <gtest/gtest.h>
#include <koo/ecad/Layer.hpp>
#include <koo/ecad/Feature.hpp>

using namespace koo::ecad;

// ============================================================================
// Layer Basic Tests
// ============================================================================

TEST(LayerTest, CreateWithName) {
    Layer layer("top");
    EXPECT_EQ(layer.getName(), "top");
}

TEST(LayerTest, SetName) {
    Layer layer;
    layer.setName("signal_1");
    EXPECT_EQ(layer.getName(), "signal_1");
}

TEST(LayerTest, SetType) {
    Layer layer("top");
    layer.setType(LayerType::Signal);
    EXPECT_EQ(layer.getType(), LayerType::Signal);

    layer.setType(LayerType::PowerGround);
    EXPECT_EQ(layer.getType(), LayerType::PowerGround);
}

TEST(LayerTest, SetContext) {
    Layer layer("board_outline");
    layer.setContext(LayerContext::Board);
    EXPECT_EQ(layer.getContext(), LayerContext::Board);
}

TEST(LayerTest, SetPolarity) {
    Layer layer("solder_mask");
    layer.setPolarity(Polarity::Negative);
    EXPECT_EQ(layer.getPolarity(), Polarity::Negative);
}

TEST(LayerTest, SetSide) {
    Layer layer("silk_top");
    layer.setSide(Side::Top);
    EXPECT_EQ(layer.getSide(), Side::Top);

    Layer layer2("silk_bot");
    layer2.setSide(Side::Bottom);
    EXPECT_EQ(layer2.getSide(), Side::Bottom);
}

TEST(LayerTest, SetRow) {
    Layer layer("inner1");
    layer.setRow(3);
    EXPECT_EQ(layer.getRow(), 3);
}

// ============================================================================
// Layer Features Tests
// ============================================================================

TEST(LayerTest, AddLineFeature) {
    Layer layer("top");

    auto line = std::make_unique<LineFeature>();
    line->setStart(0, 0);
    line->setEnd(10, 10);
    line->setSymbolName("r100");

    layer.addFeature(std::move(line));

    EXPECT_EQ(layer.getFeatureCount(), 1);
}

TEST(LayerTest, AddMultipleFeatures) {
    Layer layer("signal");

    layer.addFeature(std::make_unique<LineFeature>());
    layer.addFeature(std::make_unique<PadFeature>());
    layer.addFeature(std::make_unique<ArcFeature>());

    EXPECT_EQ(layer.getFeatureCount(), 3);
}

TEST(LayerTest, GetFeatureByIndex) {
    Layer layer("top");

    auto pad = std::make_unique<PadFeature>();
    pad->setPosition(5.0, 10.0);
    layer.addFeature(std::move(pad));

    const Feature* feature = layer.getFeature(0);
    ASSERT_NE(feature, nullptr);
    EXPECT_EQ(feature->getType(), FeatureType::Pad);

    // Out of bounds
    EXPECT_EQ(layer.getFeature(99), nullptr);
}

TEST(LayerTest, ClearFeatures) {
    Layer layer("test");

    layer.addFeature(std::make_unique<LineFeature>());
    layer.addFeature(std::make_unique<PadFeature>());

    EXPECT_EQ(layer.getFeatureCount(), 2);

    layer.clearFeatures();
    EXPECT_EQ(layer.getFeatureCount(), 0);
}

// ============================================================================
// Layer Attributes Tests
// ============================================================================

TEST(LayerTest, SetAndGetAttribute) {
    Layer layer("gnd_plane");

    layer.setAttribute("net", "GND");
    layer.setAttribute("fill", "solid");

    EXPECT_EQ(layer.getAttribute("net"), "GND");
    EXPECT_EQ(layer.getAttribute("fill"), "solid");
    EXPECT_EQ(layer.getAttribute("nonexistent"), "");
}

TEST(LayerTest, GetAttributes) {
    Layer layer("test");

    layer.setAttribute("key1", "value1");
    layer.setAttribute("key2", "value2");

    const auto& attrs = layer.getAttributes();
    EXPECT_EQ(attrs.size(), 2);
}

// ============================================================================
// Layer Symbol Names Tests
// ============================================================================

TEST(LayerTest, AddSymbolName) {
    Layer layer("top");

    layer.addSymbolName("round100");
    layer.addSymbolName("rect50x100");
    layer.addSymbolName("oval80x120");

    EXPECT_EQ(layer.getSymbolNames().size(), 3);
    EXPECT_EQ(layer.getSymbolNames()[0], "round100");
    EXPECT_EQ(layer.getSymbolNames()[2], "oval80x120");
}

TEST(LayerTest, SetSymbolNames) {
    Layer layer("signal");

    std::vector<std::string> names = {"sym1", "sym2", "sym3"};
    layer.setSymbolNames(names);

    EXPECT_EQ(layer.getSymbolNames().size(), 3);
    EXPECT_EQ(layer.getSymbolNames()[1], "sym2");
}

// ============================================================================
// LayerDefinition Tests
// ============================================================================

TEST(LayerDefinitionTest, DefaultValues) {
    LayerDefinition def;

    EXPECT_TRUE(def.name.empty());
    EXPECT_EQ(def.type, LayerType::Unknown);
    EXPECT_EQ(def.context, LayerContext::Board);
    EXPECT_EQ(def.polarity, Polarity::Positive);
    EXPECT_EQ(def.side, Side::None);
    EXPECT_EQ(def.row, 0);
}

TEST(LayerDefinitionTest, SetValues) {
    LayerDefinition def;

    def.name = "signal_1";
    def.type = LayerType::Signal;
    def.context = LayerContext::Board;
    def.polarity = Polarity::Positive;
    def.side = Side::Inner;
    def.row = 2;
    def.startName = 0;
    def.endName = 0;

    EXPECT_EQ(def.name, "signal_1");
    EXPECT_EQ(def.type, LayerType::Signal);
    EXPECT_EQ(def.row, 2);
}

// ============================================================================
// LayerMatrix Tests
// ============================================================================

TEST(LayerMatrixTest, AddLayer) {
    LayerMatrix matrix;

    LayerDefinition def1;
    def1.name = "top";
    def1.type = LayerType::Signal;
    def1.row = 1;
    matrix.addLayer(def1);

    LayerDefinition def2;
    def2.name = "bottom";
    def2.type = LayerType::Signal;
    def2.row = 2;
    matrix.addLayer(def2);

    EXPECT_EQ(matrix.getLayerCount(), 2);
}

TEST(LayerMatrixTest, GetLayerDefinitionByName) {
    LayerMatrix matrix;

    LayerDefinition def;
    def.name = "gnd_plane";
    def.type = LayerType::PowerGround;
    matrix.addLayer(def);

    const LayerDefinition* found = matrix.getLayerDefinition("gnd_plane");
    ASSERT_NE(found, nullptr);
    EXPECT_EQ(found->type, LayerType::PowerGround);

    EXPECT_EQ(matrix.getLayerDefinition("nonexistent"), nullptr);
}

TEST(LayerMatrixTest, GetLayerNames) {
    LayerMatrix matrix;

    LayerDefinition def1;
    def1.name = "top";
    matrix.addLayer(def1);

    LayerDefinition def2;
    def2.name = "bottom";
    matrix.addLayer(def2);

    auto names = matrix.getLayerNames();
    EXPECT_EQ(names.size(), 2);
}

TEST(LayerMatrixTest, AddStep) {
    LayerMatrix matrix;

    LayerMatrix::StepDefinition step1;
    step1.name = "pcb";
    step1.col = 1;
    matrix.addStep(step1);

    LayerMatrix::StepDefinition step2;
    step2.name = "panel";
    step2.col = 2;
    matrix.addStep(step2);

    EXPECT_EQ(matrix.getStepDefinitions().size(), 2);
}

TEST(LayerMatrixTest, GetStepNames) {
    LayerMatrix matrix;

    LayerMatrix::StepDefinition step1;
    step1.name = "pcb";
    matrix.addStep(step1);

    LayerMatrix::StepDefinition step2;
    step2.name = "panel";
    matrix.addStep(step2);

    auto names = matrix.getStepNames();
    EXPECT_EQ(names.size(), 2);
}

// ============================================================================
// CopperLayer Tests
// ============================================================================

TEST(CopperLayerTest, DefaultType) {
    CopperLayer layer;
    EXPECT_EQ(layer.getType(), LayerType::Signal);
}

TEST(CopperLayerTest, SetLayerNumber) {
    CopperLayer layer("top");
    layer.setLayerNumber(1);
    EXPECT_EQ(layer.getLayerNumber(), 1);
}

TEST(CopperLayerTest, SetThickness) {
    CopperLayer layer("top");
    layer.setThickness(35.0);  // 35um = 1oz copper
    EXPECT_DOUBLE_EQ(layer.getThickness(), 35.0);
}

// ============================================================================
// DrillLayer Tests
// ============================================================================

TEST(DrillLayerTest, DefaultType) {
    DrillLayer layer;
    EXPECT_EQ(layer.getType(), LayerType::Drill);
}

TEST(DrillLayerTest, SetDrillType) {
    DrillLayer layer("pth");
    layer.setDrillType(DrillType::Plated);
    EXPECT_EQ(layer.getDrillType(), DrillType::Plated);
}

TEST(DrillLayerTest, SetStartEndLayers) {
    DrillLayer layer("via");
    layer.setStartLayer(1);
    layer.setEndLayer(4);

    EXPECT_EQ(layer.getStartLayer(), 1);
    EXPECT_EQ(layer.getEndLayer(), 4);
}

TEST(DrillLayerTest, AddDrillTool) {
    DrillLayer layer("drill");

    DrillLayer::DrillTool tool;
    tool.number = 1;
    tool.diameter = 0.3;
    tool.type = DrillType::Plated;
    tool.drillCount = 100;

    layer.addDrillTool(tool);

    EXPECT_EQ(layer.getDrillTools().size(), 1);
    EXPECT_EQ(layer.getDrillTools()[0].number, 1);
    EXPECT_DOUBLE_EQ(layer.getDrillTools()[0].diameter, 0.3);
}

// ============================================================================
// SolderMaskLayer Tests
// ============================================================================

TEST(SolderMaskLayerTest, DefaultType) {
    SolderMaskLayer layer;
    EXPECT_EQ(layer.getType(), LayerType::SolderMask);
}

// ============================================================================
// SilkscreenLayer Tests
// ============================================================================

TEST(SilkscreenLayerTest, DefaultType) {
    SilkscreenLayer layer;
    EXPECT_EQ(layer.getType(), LayerType::SilkScreen);
}

// ============================================================================
// SolderPasteLayer Tests
// ============================================================================

TEST(SolderPasteLayerTest, DefaultType) {
    SolderPasteLayer layer;
    EXPECT_EQ(layer.getType(), LayerType::SolderPaste);
}

// ============================================================================
// DocumentLayer Tests
// ============================================================================

TEST(DocumentLayerTest, DefaultType) {
    DocumentLayer layer;
    EXPECT_EQ(layer.getType(), LayerType::Document);
    EXPECT_EQ(layer.getContext(), LayerContext::Document);
}
