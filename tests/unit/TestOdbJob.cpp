#include <gtest/gtest.h>
#include <koo/ecad/OdbJob.hpp>
#include <koo/ecad/Step.hpp>

using namespace koo::ecad;

// ============================================================================
// OdbJob Basic Tests
// ============================================================================

TEST(OdbJobTest, CreateWithName) {
    OdbJob job("test_pcb");
    EXPECT_EQ(job.getName(), "test_pcb");
}

TEST(OdbJobTest, SetName) {
    OdbJob job;
    job.setName("my_board");
    EXPECT_EQ(job.getName(), "my_board");
}

TEST(OdbJobTest, SetSourcePath) {
    OdbJob job;
    job.setSourcePath("/path/to/odb");
    EXPECT_EQ(job.getSourcePath(), "/path/to/odb");
}

// ============================================================================
// JobInfo Tests
// ============================================================================

TEST(OdbJobTest, GetSetInfo) {
    OdbJob job;

    JobInfo info;
    info.name = "TestBoard";
    info.creationDate = "2024-01-15";
    info.modificationDate = "2024-01-20";
    info.saveApp = "TestCAD";
    info.saveUser = "user";
    info.version = OdbVersion::V8_1;
    info.units = "MM";

    job.setInfo(info);

    const auto& retrieved = job.getInfo();
    EXPECT_EQ(retrieved.name, "TestBoard");
    EXPECT_EQ(retrieved.version, OdbVersion::V8_1);
    EXPECT_EQ(retrieved.units, "MM");
}

// ============================================================================
// Steps Tests
// ============================================================================

TEST(OdbJobTest, CreateStep) {
    OdbJob job;

    Step& step = job.createStep("pcb");
    step.setAttribute("source", "/odb/steps/pcb");

    EXPECT_EQ(job.getStepCount(), 1);

    Step* found = job.getStep("pcb");
    ASSERT_NE(found, nullptr);
    EXPECT_EQ(found->getName(), "pcb");
}

TEST(OdbJobTest, AddStep) {
    OdbJob job;

    auto step = std::make_unique<Step>("panel");
    step->setAttribute("source", "/odb/steps/panel");
    job.addStep(std::move(step));

    EXPECT_EQ(job.getStepCount(), 1);
    EXPECT_NE(job.getStep("panel"), nullptr);
}

TEST(OdbJobTest, RemoveStep) {
    OdbJob job;

    job.createStep("step1");
    job.createStep("step2");

    EXPECT_EQ(job.getStepCount(), 2);

    job.removeStep("step1");

    EXPECT_EQ(job.getStepCount(), 1);
    EXPECT_EQ(job.getStep("step1"), nullptr);
    EXPECT_NE(job.getStep("step2"), nullptr);
}

TEST(OdbJobTest, GetStepNames) {
    OdbJob job;

    job.createStep("pcb");
    job.createStep("panel");
    job.createStep("array");

    auto names = job.getStepNames();
    EXPECT_EQ(names.size(), 3);

    // All names should be present
    EXPECT_NE(std::find(names.begin(), names.end(), "pcb"), names.end());
    EXPECT_NE(std::find(names.begin(), names.end(), "panel"), names.end());
    EXPECT_NE(std::find(names.begin(), names.end(), "array"), names.end());
}

TEST(OdbJobTest, GetPrimaryStep) {
    OdbJob job;

    // No steps - should return nullptr
    EXPECT_EQ(job.getPrimaryStep(), nullptr);

    job.createStep("step1");
    job.createStep("pcb");  // "pcb" is often the primary step
    job.createStep("step2");

    // Primary step should be found (implementation dependent)
    Step* primary = job.getPrimaryStep();
    ASSERT_NE(primary, nullptr);
}

// ============================================================================
// Symbol Library Tests
// ============================================================================

TEST(OdbJobTest, AddSymbol) {
    OdbJob job;

    auto symbol = std::make_unique<Symbol>("custom_pad");
    job.addSymbol(std::move(symbol));

    Symbol* found = job.getSymbol("custom_pad");
    ASSERT_NE(found, nullptr);
    EXPECT_EQ(found->getName(), "custom_pad");
}

TEST(OdbJobTest, GetSymbolNames) {
    OdbJob job;

    job.addSymbol(std::make_unique<Symbol>("sym1"));
    job.addSymbol(std::make_unique<Symbol>("sym2"));

    auto names = job.getSymbolNames();
    EXPECT_EQ(names.size(), 2);
}

// ============================================================================
// Attributes Tests
// ============================================================================

TEST(OdbJobTest, SetAndGetAttribute) {
    OdbJob job;

    job.setAttribute("customer", "ACME Corp");
    job.setAttribute("revision", "A");

    EXPECT_EQ(job.getAttribute("customer"), "ACME Corp");
    EXPECT_EQ(job.getAttribute("revision"), "A");
    EXPECT_EQ(job.getAttribute("nonexistent"), "");
}

// ============================================================================
// Stackup Tests
// ============================================================================

TEST(OdbJobTest, AddStackupLayer) {
    OdbJob job;

    StackupLayer layer1;
    layer1.name = "top";
    layer1.materialType = StackupMaterialType::Copper;
    layer1.thickness = 0.035;
    layer1.material = "copper";
    job.addStackupLayer(layer1);

    StackupLayer layer2;
    layer2.name = "dielectric1";
    layer2.materialType = StackupMaterialType::Prepreg;
    layer2.thickness = 0.1;
    layer2.material = "FR4";
    job.addStackupLayer(layer2);

    EXPECT_EQ(job.getStackup().size(), 2);
    EXPECT_EQ(job.getStackup()[0].name, "top");
    EXPECT_EQ(job.getStackup()[1].materialType, StackupMaterialType::Prepreg);
}

TEST(OdbJobTest, GetTotalThickness) {
    OdbJob job;

    StackupLayer layer1;
    layer1.thickness = 0.035;
    job.addStackupLayer(layer1);

    StackupLayer layer2;
    layer2.thickness = 0.1;
    job.addStackupLayer(layer2);

    StackupLayer layer3;
    layer3.thickness = 0.035;
    job.addStackupLayer(layer3);

    EXPECT_DOUBLE_EQ(job.getTotalThickness(), 0.17);
}

// ============================================================================
// Impedance Tests
// ============================================================================

TEST(OdbJobTest, AddImpedanceConstraint) {
    OdbJob job;

    ImpedanceConstraint constraint;
    constraint.id = "diff_100";
    constraint.type = "differential";
    constraint.impedance = 100.0;
    constraint.tolerance = 10.0;
    constraint.layer = "top";
    constraint.traceWidth = 0.1;
    constraint.spacing = 0.15;
    job.addImpedanceConstraint(constraint);

    EXPECT_EQ(job.getImpedanceConstraints().size(), 1);
    EXPECT_EQ(job.getImpedanceConstraints()[0].id, "diff_100");
    EXPECT_DOUBLE_EQ(job.getImpedanceConstraints()[0].impedance, 100.0);
}

// ============================================================================
// Intentional Shorts Tests
// ============================================================================

TEST(OdbJobTest, AddIntentionalShort) {
    OdbJob job;

    IntentionalShort shortDef;
    shortDef.netUids.push_back(1);
    shortDef.netUids.push_back(2);
    shortDef.id = 100;
    job.addIntentionalShort(shortDef);

    EXPECT_EQ(job.getIntentionalShorts().size(), 1);
    EXPECT_EQ(job.getIntentionalShorts()[0].netUids.size(), 2);
    EXPECT_EQ(job.getIntentionalShorts()[0].id, 100);
}

// ============================================================================
// Drill Tools Tests
// ============================================================================

TEST(OdbJobTest, AddDrillTool) {
    OdbJob job;

    DrillTool tool;
    tool.num = 1;
    tool.diameter = 0.3;
    tool.type = DrillToolType::Plated;
    job.addDrillTool(tool);

    EXPECT_EQ(job.getDrillTools().size(), 1);
    EXPECT_EQ(job.getDrillTools()[0].num, 1);
    EXPECT_DOUBLE_EQ(job.getDrillTools()[0].diameter, 0.3);
}

// ============================================================================
// Metadata Tests
// ============================================================================

TEST(OdbJobTest, SetMetadata) {
    OdbJob job;

    Metadata meta;
    meta.creationDate = "2024-01-20";
    meta.modificationDate = "2024-01-21";
    meta.source = "TestCAD";
    meta.version = "8.1";
    job.setMetadata(meta);

    EXPECT_EQ(job.getMetadata().creationDate, "2024-01-20");
    EXPECT_EQ(job.getMetadata().source, "TestCAD");
}

// ============================================================================
// Component Variants Tests
// ============================================================================

TEST(OdbJobTest, AddVariant) {
    OdbJob job;

    ComponentVariant variant;
    variant.name = "no_debug";
    variant.description = "Production without debug components";

    ComponentVariantEntry entry1;
    entry1.refDes = "J1";
    entry1.state = ComponentVariantState::Disabled;
    variant.components.push_back(entry1);

    ComponentVariantEntry entry2;
    entry2.refDes = "R99";
    entry2.state = ComponentVariantState::Replaced;
    entry2.alternatePartNumber = "RC0603-0R";
    variant.components.push_back(entry2);

    job.addVariant(variant);

    EXPECT_EQ(job.getVariantCount(), 1);

    const ComponentVariant* found = job.getVariant("no_debug");
    ASSERT_NE(found, nullptr);
    EXPECT_EQ(found->components.size(), 2);
    EXPECT_EQ(found->components[0].state, ComponentVariantState::Disabled);
}

// ============================================================================
// Embedded Components Tests
// ============================================================================

TEST(OdbJobTest, AddEmbeddedComponent) {
    OdbJob job;

    EmbeddedComponent comp;
    comp.name = "ER1";
    comp.type = EmbeddedComponentType::Resistor;
    comp.value = 100.0;
    comp.layerName = "inner2";
    comp.x = 10.5;
    comp.y = 20.3;
    job.addEmbeddedComponent(comp);

    EXPECT_EQ(job.getEmbeddedComponentCount(), 1);
    EXPECT_EQ(job.getEmbeddedComponents()[0].name, "ER1");
    EXPECT_EQ(job.getEmbeddedComponents()[0].type, EmbeddedComponentType::Resistor);
}

TEST(OdbJobTest, GetEmbeddedComponentsByType) {
    OdbJob job;

    EmbeddedComponent res;
    res.name = "ER1";
    res.type = EmbeddedComponentType::Resistor;
    job.addEmbeddedComponent(res);

    EmbeddedComponent cap;
    cap.name = "EC1";
    cap.type = EmbeddedComponentType::Capacitor;
    job.addEmbeddedComponent(cap);

    EmbeddedComponent res2;
    res2.name = "ER2";
    res2.type = EmbeddedComponentType::Resistor;
    job.addEmbeddedComponent(res2);

    auto resistors = job.getEmbeddedComponentsByType(EmbeddedComponentType::Resistor);
    EXPECT_EQ(resistors.size(), 2);

    auto capacitors = job.getEmbeddedComponentsByType(EmbeddedComponentType::Capacitor);
    EXPECT_EQ(capacitors.size(), 1);

    auto inductors = job.getEmbeddedComponentsByType(EmbeddedComponentType::Inductor);
    EXPECT_EQ(inductors.size(), 0);
}

// ============================================================================
// Build-up Info Tests
// ============================================================================

TEST(OdbJobTest, SetBuildupInfo) {
    OdbJob job;

    BuildupInfo buildup;
    buildup.name = "8-layer-hdi";
    buildup.totalLayers = 8;
    buildup.totalThickness = 1.6;

    ViaSpan span1;
    span1.name = "through";
    span1.type = ViaSpanType::Through;
    span1.startLayer = 1;
    span1.endLayer = 8;
    span1.startLayerName = "top";
    span1.endLayerName = "bottom";
    buildup.viaSpans.push_back(span1);

    ViaSpan span2;
    span2.name = "micro_1";
    span2.type = ViaSpanType::Microvia;
    span2.startLayer = 1;
    span2.endLayer = 2;
    span2.startLayerName = "top";
    span2.endLayerName = "inner1";
    buildup.viaSpans.push_back(span2);

    job.setBuildupInfo(buildup);

    const auto& info = job.getBuildupInfo();
    EXPECT_EQ(info.totalLayers, 8);
    EXPECT_EQ(info.name, "8-layer-hdi");
    EXPECT_EQ(info.viaSpans.size(), 2);
    EXPECT_EQ(info.viaSpans[1].type, ViaSpanType::Microvia);
}

// ============================================================================
// VPL Tests
// ============================================================================

TEST(OdbJobTest, AddVendorPart) {
    OdbJob job;

    VendorPart part;
    part.partNumber = "RC0603FR-0710KL";
    part.vendorName = "Yageo";
    part.description = "10K 1% 0603 resistor";
    part.datasheet = "https://example.com/datasheet.pdf";
    job.addVendorPart(part);

    EXPECT_EQ(job.getVendorPartCount(), 1);
    EXPECT_EQ(job.getVendorParts()[0].vendorName, "Yageo");
}

// ============================================================================
// Customer Info Tests
// ============================================================================

TEST(OdbJobTest, SetCustomerInfo) {
    OdbJob job;

    CustomerInfo info;
    info.name = "ACME Electronics";
    info.company = "ACME Corp";
    info.projectName = "Widget v2";
    info.projectNumber = "WDG-001";
    info.revision = "B";
    job.setCustomerInfo(info);

    EXPECT_EQ(job.getCustomerInfo().name, "ACME Electronics");
    EXPECT_EQ(job.getCustomerInfo().revision, "B");
}

// ============================================================================
// Clear Tests
// ============================================================================

TEST(OdbJobTest, Clear) {
    OdbJob job("test");
    job.createStep("pcb");
    job.setAttribute("key", "value");
    job.addSymbol(std::make_unique<Symbol>("sym"));

    StackupLayer layer;
    layer.thickness = 0.035;
    job.addStackupLayer(layer);

    job.clear();

    EXPECT_EQ(job.getStepCount(), 0);
    EXPECT_TRUE(job.getAttributes().empty());
    EXPECT_TRUE(job.getSymbolNames().empty());
    EXPECT_TRUE(job.getStackup().empty());
}

// ============================================================================
// Move Semantics Tests
// ============================================================================

TEST(OdbJobTest, MoveConstruction) {
    OdbJob job1("original");
    job1.createStep("pcb");
    job1.setAttribute("key", "value");

    OdbJob job2 = std::move(job1);

    EXPECT_EQ(job2.getName(), "original");
    EXPECT_EQ(job2.getStepCount(), 1);
    EXPECT_EQ(job2.getAttribute("key"), "value");
}

TEST(OdbJobTest, MoveAssignment) {
    OdbJob job1("first");
    job1.createStep("step1");

    OdbJob job2("second");
    job2 = std::move(job1);

    EXPECT_EQ(job2.getName(), "first");
    EXPECT_EQ(job2.getStepCount(), 1);
}
