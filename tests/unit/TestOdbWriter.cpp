#include <gtest/gtest.h>
#include <koo/ecad/OdbWriter.hpp>
#include <koo/ecad/OdbReader.hpp>
#include <filesystem>
#include <fstream>

using namespace koo::ecad;

// Test helper to create a temporary directory
class OdbWriterTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create temp directory for tests
        tempDir_ = std::filesystem::temp_directory_path() / "odb_writer_test";
        std::filesystem::create_directories(tempDir_);
    }

    void TearDown() override {
        // Clean up temp directory
        if (std::filesystem::exists(tempDir_)) {
            std::filesystem::remove_all(tempDir_);
        }
    }

    std::filesystem::path tempDir_;
};

// ============================================================================
// Basic Write Tests
// ============================================================================

TEST_F(OdbWriterTest, WriteEmptyJob) {
    OdbWriter writer;
    OdbJob job("test_job");

    auto odbPath = tempDir_ / "empty_job";

    ASSERT_TRUE(writer.write(job, odbPath));
    EXPECT_FALSE(writer.hasError());

    // Verify directory structure
    EXPECT_TRUE(std::filesystem::exists(odbPath / "matrix"));
    EXPECT_TRUE(std::filesystem::exists(odbPath / "steps"));
    EXPECT_TRUE(std::filesystem::exists(odbPath / "symbols"));
    EXPECT_TRUE(std::filesystem::exists(odbPath / "misc"));
}

TEST_F(OdbWriterTest, WriteJobWithInfo) {
    OdbWriter writer;
    OdbJob job("pcb_board");

    JobInfo info;
    info.name = "PCB Board";
    info.units = "MM";
    info.creationDate = "2024-01-15";
    info.modificationDate = "2024-01-20";
    info.saveApp = "TestApp";
    info.saveUser = "user";
    info.version = OdbVersion::V8_1;
    job.setInfo(info);

    auto odbPath = tempDir_ / "job_with_info";

    ASSERT_TRUE(writer.write(job, odbPath));

    // Verify info file exists
    EXPECT_TRUE(std::filesystem::exists(odbPath / "misc" / "info"));
}

TEST_F(OdbWriterTest, WriteJobOverwriteProtection) {
    OdbWriter writer;
    OdbJob job("test_job");

    auto odbPath = tempDir_ / "overwrite_test";

    // First write should succeed
    ASSERT_TRUE(writer.write(job, odbPath));

    // Second write without overwrite option should fail
    OdbWriter::Options options;
    options.overwrite = false;
    EXPECT_FALSE(writer.write(job, odbPath, options));
    EXPECT_TRUE(writer.hasError());

    // Third write with overwrite option should succeed
    options.overwrite = true;
    EXPECT_TRUE(writer.write(job, odbPath, options));
}

// ============================================================================
// Matrix Writing Tests
// ============================================================================

TEST_F(OdbWriterTest, WriteMatrix) {
    OdbWriter writer;
    OdbJob job("matrix_test");

    // Add step to matrix
    LayerMatrix::StepDefinition stepDef;
    stepDef.col = 1;
    stepDef.name = "pcb";
    job.getMatrix().addStep(stepDef);

    // Add layers to matrix
    LayerDefinition layer1;
    layer1.row = 1;
    layer1.name = "top";
    layer1.type = LayerType::Signal;
    layer1.context = LayerContext::Board;
    layer1.polarity = Polarity::Positive;
    job.getMatrix().addLayer(layer1);

    LayerDefinition layer2;
    layer2.row = 2;
    layer2.name = "gnd";
    layer2.type = LayerType::PowerGround;
    layer2.context = LayerContext::Board;
    layer2.polarity = Polarity::Positive;
    job.getMatrix().addLayer(layer2);

    auto odbPath = tempDir_ / "matrix_test";
    ASSERT_TRUE(writer.write(job, odbPath));

    // Verify matrix file exists and contains expected content
    auto matrixPath = odbPath / "matrix" / "matrix";
    EXPECT_TRUE(std::filesystem::exists(matrixPath));

    std::ifstream file(matrixPath);
    std::string content((std::istreambuf_iterator<char>(file)),
                         std::istreambuf_iterator<char>());

    EXPECT_NE(content.find("STEP {"), std::string::npos);
    EXPECT_NE(content.find("NAME=pcb"), std::string::npos);
    EXPECT_NE(content.find("LAYER {"), std::string::npos);
    EXPECT_NE(content.find("NAME=top"), std::string::npos);
    EXPECT_NE(content.find("TYPE=SIGNAL"), std::string::npos);
}

// ============================================================================
// Step Writing Tests
// ============================================================================

TEST_F(OdbWriterTest, WriteStep) {
    OdbWriter writer;
    OdbJob job("step_test");

    Step& step = job.createStep("pcb");

    // Set step header properties
    step.setDatum(100.0, 100.0);
    step.setXDatum('L');
    step.setYDatum('B');
    step.setAffectHoles(true);

    // Add profile
    Contour profile(0.0, 0.0, PolygonType::Island);
    profile.addLineSegment(100.0, 0.0);
    profile.addLineSegment(100.0, 100.0);
    profile.addLineSegment(0.0, 100.0);
    profile.addLineSegment(0.0, 0.0);
    step.addProfileContour(profile);

    auto odbPath = tempDir_ / "step_test";
    ASSERT_TRUE(writer.write(job, odbPath));

    // Verify step directory structure
    EXPECT_TRUE(std::filesystem::exists(odbPath / "steps" / "pcb"));
    EXPECT_TRUE(std::filesystem::exists(odbPath / "steps" / "pcb" / "stephdr"));
    EXPECT_TRUE(std::filesystem::exists(odbPath / "steps" / "pcb" / "profile"));
    EXPECT_TRUE(std::filesystem::exists(odbPath / "steps" / "pcb" / "layers"));
}

TEST_F(OdbWriterTest, WriteStepAndRepeat) {
    OdbWriter writer;
    OdbJob job("sr_test");

    Step& step = job.createStep("panel");

    StepRepeat sr;
    sr.stepName = "pcb";
    sr.x = 10.0;
    sr.y = 10.0;
    sr.dx = 50.0;
    sr.dy = 0.0;
    sr.nx = 2;
    sr.ny = 1;
    sr.angle = 0.0;
    sr.mirror = false;
    step.addStepRepeat(sr);

    auto odbPath = tempDir_ / "sr_test";
    ASSERT_TRUE(writer.write(job, odbPath));

    // Verify stephdr contains step and repeat
    auto stephdrPath = odbPath / "steps" / "panel" / "stephdr";
    std::ifstream file(stephdrPath);
    std::string content((std::istreambuf_iterator<char>(file)),
                         std::istreambuf_iterator<char>());

    EXPECT_NE(content.find("STEP-AND-REPEAT {"), std::string::npos);
    EXPECT_NE(content.find("NAME=pcb"), std::string::npos);
}

// ============================================================================
// Layer Writing Tests
// ============================================================================

TEST_F(OdbWriterTest, WriteLayerWithFeatures) {
    OdbWriter writer;
    OdbJob job("layer_test");

    Step& step = job.createStep("pcb");
    auto topLayer = std::make_unique<CopperLayer>("top");
    topLayer->setType(LayerType::Signal);
    Layer& layer = *topLayer;
    step.addLayer(std::move(topLayer));

    // Add line feature
    auto line = std::make_unique<LineFeature>();
    line->setStart(0.0, 0.0);
    line->setEnd(10.0, 10.0);
    line->setSymbolName("r100");
    line->setPolarity(Polarity::Positive);
    layer.addFeature(std::move(line));

    // Add pad feature
    auto pad = std::make_unique<PadFeature>();
    pad->setPosition(5.0, 5.0);
    pad->setSymbolName("s50");
    pad->setPolarity(Polarity::Positive);
    layer.addFeature(std::move(pad));

    OdbWriter::Options options;
    options.compressFeatures = false;  // Don't compress for easy verification

    auto odbPath = tempDir_ / "layer_test";
    ASSERT_TRUE(writer.write(job, odbPath, options));

    // Verify features file exists
    auto featuresPath = odbPath / "steps" / "pcb" / "layers" / "top" / "features";
    EXPECT_TRUE(std::filesystem::exists(featuresPath));

    std::ifstream file(featuresPath);
    std::string content((std::istreambuf_iterator<char>(file)),
                         std::istreambuf_iterator<char>());

    // Should contain symbol list and features
    EXPECT_NE(content.find("$"), std::string::npos);  // Symbol count marker
    EXPECT_NE(content.find("L "), std::string::npos);  // Line feature
    EXPECT_NE(content.find("P "), std::string::npos);  // Pad feature
}

TEST_F(OdbWriterTest, WriteCompressedFeatures) {
    OdbWriter writer;
    OdbJob job("compress_test");

    Step& step = job.createStep("pcb");
    auto topLayer = std::make_unique<CopperLayer>("top");
    Layer& layer = *topLayer;
    step.addLayer(std::move(topLayer));

    auto line = std::make_unique<LineFeature>();
    line->setStart(0.0, 0.0);
    line->setEnd(10.0, 10.0);
    line->setSymbolName("r100");
    layer.addFeature(std::move(line));

    OdbWriter::Options options;
    options.compressFeatures = true;

    auto odbPath = tempDir_ / "compress_test";
    ASSERT_TRUE(writer.write(job, odbPath, options));

    // Verify compressed features file exists
    auto compressedPath = odbPath / "steps" / "pcb" / "layers" / "top" / "features.z";
    EXPECT_TRUE(std::filesystem::exists(compressedPath));
}

// ============================================================================
// EDA Data Writing Tests
// ============================================================================

TEST_F(OdbWriterTest, WriteEdaData) {
    OdbWriter writer;
    OdbJob job("eda_test");

    Step& step = job.createStep("pcb");
    EdaData& eda = step.getEdaData();

    // Add package
    auto pkg = std::make_unique<Package>("SOIC8");
    pkg->setPitch(1.27);

    Pin pin1;
    pin1.name = "1";
    pin1.x = -1.27;
    pin1.y = 0.0;
    pin1.type = PinType::Smd;
    pkg->addPin(pin1);

    eda.addPackage(std::move(pkg));

    // Add component
    auto comp = std::make_unique<Component>("U1");
    comp->setPackageName("SOIC8");
    comp->setPosition(10.0, 20.0);
    comp->setRotation(0.0);
    comp->setSide(MountSide::Top);
    comp->setPartNumber("LM358");
    eda.addComponent(std::move(comp));

    // Add net
    auto net = std::make_unique<Net>("VCC");
    net->setNetNumber(1);
    net->setNetClass(NetClass::Power);
    net->addPin("U1", "8");
    eda.addNet(std::move(net));

    OdbWriter::Options options;
    options.compressFeatures = false;

    auto odbPath = tempDir_ / "eda_test";
    ASSERT_TRUE(writer.write(job, odbPath, options));

    // Verify EDA data file
    auto edaPath = odbPath / "steps" / "pcb" / "eda" / "data";
    EXPECT_TRUE(std::filesystem::exists(edaPath));

    std::ifstream file(edaPath);
    std::string content((std::istreambuf_iterator<char>(file)),
                         std::istreambuf_iterator<char>());

    EXPECT_NE(content.find("PKG SOIC8"), std::string::npos);
    EXPECT_NE(content.find("CMP 0"), std::string::npos);  // Changed from "CMP U1" to match new format
    EXPECT_NE(content.find("U1"), std::string::npos);     // Check component refdes separately
    EXPECT_NE(content.find("NET VCC"), std::string::npos);
}

// ============================================================================
// Stackup Writing Tests
// ============================================================================

TEST_F(OdbWriterTest, WriteStackup) {
    OdbWriter writer;
    OdbJob job("stackup_test");

    StackupLayer layer1;
    layer1.name = "top";
    layer1.materialType = StackupMaterialType::Copper;
    layer1.thickness = 0.035;
    layer1.material = "copper";
    job.addStackupLayer(layer1);

    StackupLayer layer2;
    layer2.name = "core";
    layer2.materialType = StackupMaterialType::Core;
    layer2.thickness = 0.8;
    layer2.material = "FR4";
    layer2.dielectricConstant = 4.5;
    job.addStackupLayer(layer2);

    StackupLayer layer3;
    layer3.name = "bottom";
    layer3.materialType = StackupMaterialType::Copper;
    layer3.thickness = 0.035;
    job.addStackupLayer(layer3);

    auto odbPath = tempDir_ / "stackup_test";
    ASSERT_TRUE(writer.write(job, odbPath));

    auto stackupPath = odbPath / "misc" / "stackup";
    EXPECT_TRUE(std::filesystem::exists(stackupPath));

    std::ifstream file(stackupPath);
    std::string content((std::istreambuf_iterator<char>(file)),
                         std::istreambuf_iterator<char>());

    EXPECT_NE(content.find("LAYER 1"), std::string::npos);
    EXPECT_NE(content.find("NAME=top"), std::string::npos);
    EXPECT_NE(content.find("TYPE=COPPER"), std::string::npos);
}

// ============================================================================
// Symbol Writing Tests
// ============================================================================

TEST_F(OdbWriterTest, WriteUserSymbol) {
    OdbWriter writer;
    OdbJob job("symbol_test");

    auto symbol = std::make_unique<Symbol>("custom_pad");
    symbol->setType(SymbolType::User);

    auto line = std::make_unique<LineFeature>();
    line->setStart(-5.0, -5.0);
    line->setEnd(5.0, 5.0);
    symbol->addFeature(std::move(line));

    job.addSymbol(std::move(symbol));

    OdbWriter::Options options;
    options.compressFeatures = false;

    auto odbPath = tempDir_ / "symbol_test";
    ASSERT_TRUE(writer.write(job, odbPath, options));

    // Note: Standard symbols are not written, only user symbols
    auto symbolPath = odbPath / "symbols" / "custom_pad";
    EXPECT_TRUE(std::filesystem::exists(symbolPath));
    EXPECT_TRUE(std::filesystem::exists(symbolPath / "features"));
}

// ============================================================================
// Misc Data Writing Tests
// ============================================================================

TEST_F(OdbWriterTest, WriteImpedance) {
    OdbWriter writer;
    OdbJob job("impedance_test");

    ImpedanceConstraint constraint;
    constraint.id = "diff_100";
    constraint.type = "differential";
    constraint.impedance = 100.0;
    constraint.tolerance = 10.0;
    constraint.layer = "top";
    constraint.traceWidth = 0.1;
    constraint.spacing = 0.15;
    job.addImpedanceConstraint(constraint);

    auto odbPath = tempDir_ / "impedance_test";
    ASSERT_TRUE(writer.write(job, odbPath));

    auto impedancePath = odbPath / "misc" / "impedance";
    EXPECT_TRUE(std::filesystem::exists(impedancePath));
}

TEST_F(OdbWriterTest, WriteDrillTools) {
    OdbWriter writer;
    OdbJob job("tools_test");

    DrillTool tool1;
    tool1.num = 1;
    tool1.diameter = 0.3;
    tool1.type = DrillToolType::Plated;
    tool1.bit = "round";
    job.addDrillTool(tool1);

    DrillTool tool2;
    tool2.num = 2;
    tool2.diameter = 0.5;
    tool2.type = DrillToolType::NonPlated;
    job.addDrillTool(tool2);

    auto odbPath = tempDir_ / "tools_test";
    ASSERT_TRUE(writer.write(job, odbPath));

    auto toolsPath = odbPath / "misc" / "tools";
    EXPECT_TRUE(std::filesystem::exists(toolsPath));

    std::ifstream file(toolsPath);
    std::string content((std::istreambuf_iterator<char>(file)),
                         std::istreambuf_iterator<char>());

    EXPECT_NE(content.find("TOOL 1"), std::string::npos);
    EXPECT_NE(content.find("DIAMETER=0.3"), std::string::npos);
}

TEST_F(OdbWriterTest, WriteVariants) {
    OdbWriter writer;
    OdbJob job("variant_test");

    ComponentVariant variant;
    variant.name = "production";
    variant.description = "Production variant";

    ComponentVariantEntry entry;
    entry.refDes = "J1";
    entry.state = ComponentVariantState::Disabled;
    variant.components.push_back(entry);

    job.addVariant(variant);

    auto odbPath = tempDir_ / "variant_test";
    ASSERT_TRUE(writer.write(job, odbPath));

    auto variantsPath = odbPath / "misc" / "variants";
    EXPECT_TRUE(std::filesystem::exists(variantsPath));
}

TEST_F(OdbWriterTest, WriteEmbeddedComponents) {
    OdbWriter writer;
    OdbJob job("embedded_test");

    EmbeddedComponent comp;
    comp.name = "ER1";
    comp.type = EmbeddedComponentType::Resistor;
    comp.value = 100.0;
    comp.layerName = "inner2";
    comp.x = 10.0;
    comp.y = 20.0;
    job.addEmbeddedComponent(comp);

    auto odbPath = tempDir_ / "embedded_test";
    ASSERT_TRUE(writer.write(job, odbPath));

    auto embeddedPath = odbPath / "misc" / "embedded";
    EXPECT_TRUE(std::filesystem::exists(embeddedPath));
}

TEST_F(OdbWriterTest, WriteBuildupInfo) {
    OdbWriter writer;
    OdbJob job("buildup_test");

    BuildupInfo buildup;
    buildup.name = "4-layer";
    buildup.totalLayers = 4;
    buildup.totalThickness = 1.6;

    ViaSpan span;
    span.name = "through";
    span.type = ViaSpanType::Through;
    span.startLayer = 1;
    span.endLayer = 4;
    span.startLayerName = "top";
    span.endLayerName = "bottom";
    buildup.viaSpans.push_back(span);

    job.setBuildupInfo(buildup);

    auto odbPath = tempDir_ / "buildup_test";
    ASSERT_TRUE(writer.write(job, odbPath));

    auto buildupPath = odbPath / "misc" / "buildup";
    EXPECT_TRUE(std::filesystem::exists(buildupPath));
}

TEST_F(OdbWriterTest, WriteVendorParts) {
    OdbWriter writer;
    OdbJob job("vpl_test");

    VendorPart part;
    part.partNumber = "RC0603FR-0710KL";
    part.vendorName = "Yageo";
    part.description = "10K 1% 0603";
    part.datasheet = "http://example.com/datasheet.pdf";
    job.addVendorPart(part);

    auto odbPath = tempDir_ / "vpl_test";
    ASSERT_TRUE(writer.write(job, odbPath));

    auto vplPath = odbPath / "misc" / "vpl";
    EXPECT_TRUE(std::filesystem::exists(vplPath));
}

TEST_F(OdbWriterTest, WriteCustomerInfo) {
    OdbWriter writer;
    OdbJob job("customer_test");

    CustomerInfo info;
    info.name = "ACME Electronics";
    info.company = "ACME Corp";
    info.projectName = "Widget v2";
    info.projectNumber = "WDG-001";
    info.revision = "B";
    job.setCustomerInfo(info);

    auto odbPath = tempDir_ / "customer_test";
    ASSERT_TRUE(writer.write(job, odbPath));

    auto customerPath = odbPath / "misc" / "customer";
    EXPECT_TRUE(std::filesystem::exists(customerPath));

    std::ifstream file(customerPath);
    std::string content((std::istreambuf_iterator<char>(file)),
                         std::istreambuf_iterator<char>());

    EXPECT_NE(content.find("NAME=ACME Electronics"), std::string::npos);
    EXPECT_NE(content.find("COMPANY=ACME Corp"), std::string::npos);
}

// ============================================================================
// Partial Write Tests
// ============================================================================

TEST_F(OdbWriterTest, WriteMatrixOnly) {
    OdbWriter writer;
    OdbJob job("matrix_only");

    LayerDefinition layer;
    layer.row = 1;
    layer.name = "top";
    layer.type = LayerType::Signal;
    job.getMatrix().addLayer(layer);

    auto odbPath = tempDir_ / "matrix_only";
    ASSERT_TRUE(writer.writeMatrix(job, odbPath));

    EXPECT_TRUE(std::filesystem::exists(odbPath / "matrix" / "matrix"));
}

TEST_F(OdbWriterTest, WriteStepOnly) {
    OdbWriter writer;

    Step step("single_step");
    step.setDatum(0.0, 0.0);
    step.setXDatum('L');
    step.setYDatum('B');

    auto stepPath = tempDir_ / "single_step";
    ASSERT_TRUE(writer.writeStep(step, stepPath));

    EXPECT_TRUE(std::filesystem::exists(stepPath / "stephdr"));
    EXPECT_TRUE(std::filesystem::exists(stepPath / "layers"));
}

TEST_F(OdbWriterTest, WriteLayerOnly) {
    OdbWriter writer;

    CopperLayer layer("single_layer");
    layer.setType(LayerType::Signal);

    auto line = std::make_unique<LineFeature>();
    line->setStart(0.0, 0.0);
    line->setEnd(10.0, 10.0);
    line->setSymbolName("r100");
    layer.addFeature(std::move(line));

    auto layerPath = tempDir_ / "single_layer";
    ASSERT_TRUE(writer.writeLayer(layer, layerPath));

    // Either compressed or uncompressed features should exist
    bool hasFeatures = std::filesystem::exists(layerPath / "features") ||
                       std::filesystem::exists(layerPath / "features.z");
    EXPECT_TRUE(hasFeatures);
}

// ============================================================================
// Progress Callback Test
// ============================================================================

TEST_F(OdbWriterTest, ProgressCallback) {
    OdbWriter writer;
    OdbJob job("progress_test");

    job.createStep("pcb");

    std::vector<std::pair<std::string, double>> progressReports;

    writer.setProgressCallback([&](const std::string& message, double progress) {
        progressReports.push_back({message, progress});
    });

    auto odbPath = tempDir_ / "progress_test";
    ASSERT_TRUE(writer.write(job, odbPath));

    // Should have received progress reports
    EXPECT_FALSE(progressReports.empty());

    // First report should be at 0.0, last at 1.0
    EXPECT_DOUBLE_EQ(progressReports.front().second, 0.0);
    EXPECT_DOUBLE_EQ(progressReports.back().second, 1.0);
}

// ============================================================================
// Surface Feature Writing Test
// ============================================================================

TEST_F(OdbWriterTest, WriteSurfaceFeature) {
    OdbWriter writer;
    OdbJob job("surface_test");

    Step& step = job.createStep("pcb");
    auto topLayer = std::make_unique<CopperLayer>("top");
    Layer& layer = *topLayer;
    step.addLayer(std::move(topLayer));

    auto surface = std::make_unique<SurfaceFeature>();
    surface->setPolarity(Polarity::Positive);

    Contour contour(0.0, 0.0, PolygonType::Island);
    contour.addLineSegment(10.0, 0.0);
    contour.addLineSegment(10.0, 10.0);
    contour.addLineSegment(0.0, 10.0);
    contour.addLineSegment(0.0, 0.0);
    surface->addContour(contour);

    layer.addFeature(std::move(surface));

    OdbWriter::Options options;
    options.compressFeatures = false;

    auto odbPath = tempDir_ / "surface_test";
    ASSERT_TRUE(writer.write(job, odbPath, options));

    auto featuresPath = odbPath / "steps" / "pcb" / "layers" / "top" / "features";
    std::ifstream file(featuresPath);
    std::string content((std::istreambuf_iterator<char>(file)),
                         std::istreambuf_iterator<char>());

    EXPECT_NE(content.find("S P"), std::string::npos);  // Surface with positive polarity
    EXPECT_NE(content.find("OB"), std::string::npos);   // Contour begin
    EXPECT_NE(content.find("OS"), std::string::npos);   // Line segment
    EXPECT_NE(content.find("OE"), std::string::npos);   // Contour end
    EXPECT_NE(content.find("SE"), std::string::npos);   // Surface end
}

// ============================================================================
// Arc Feature Writing Test
// ============================================================================

TEST_F(OdbWriterTest, WriteArcFeature) {
    OdbWriter writer;
    OdbJob job("arc_test");

    Step& step = job.createStep("pcb");
    auto topLayer = std::make_unique<CopperLayer>("top");
    Layer& layer = *topLayer;
    step.addLayer(std::move(topLayer));

    auto arc = std::make_unique<ArcFeature>();
    arc->setStart(0.0, 0.0);
    arc->setEnd(10.0, 0.0);
    arc->setCenter(5.0, 0.0);
    arc->setClockwise(true);
    arc->setSymbolName("r50");
    arc->setPolarity(Polarity::Positive);
    layer.addFeature(std::move(arc));

    OdbWriter::Options options;
    options.compressFeatures = false;

    auto odbPath = tempDir_ / "arc_test";
    ASSERT_TRUE(writer.write(job, odbPath, options));

    auto featuresPath = odbPath / "steps" / "pcb" / "layers" / "top" / "features";
    std::ifstream file(featuresPath);
    std::string content((std::istreambuf_iterator<char>(file)),
                         std::istreambuf_iterator<char>());

    EXPECT_NE(content.find("A "), std::string::npos);  // Arc feature
}

// ============================================================================
// Text Feature Writing Test
// ============================================================================

TEST_F(OdbWriterTest, WriteTextFeature) {
    OdbWriter writer;
    OdbJob job("text_test");

    Step& step = job.createStep("pcb");
    auto sstLayer = std::make_unique<CopperLayer>("sst");
    sstLayer->setType(LayerType::SilkScreen);
    Layer& layer = *sstLayer;
    step.addLayer(std::move(sstLayer));

    auto text = std::make_unique<TextFeature>();
    text->setPosition(10.0, 20.0);
    text->setText("U1");
    text->setFont("standard");
    text->setPolarity(Polarity::Positive);
    text->setRotation(0.0);
    text->setSize(1.0, 1.0);
    layer.addFeature(std::move(text));

    OdbWriter::Options options;
    options.compressFeatures = false;

    auto odbPath = tempDir_ / "text_test";
    ASSERT_TRUE(writer.write(job, odbPath, options));

    auto featuresPath = odbPath / "steps" / "pcb" / "layers" / "sst" / "features";
    std::ifstream file(featuresPath);
    std::string content((std::istreambuf_iterator<char>(file)),
                         std::istreambuf_iterator<char>());

    EXPECT_NE(content.find("T "), std::string::npos);    // Text feature
    EXPECT_NE(content.find("'U1'"), std::string::npos);  // Text content
}

// ============================================================================
// Round-Trip Tests (Write -> Read -> Verify)
// ============================================================================

TEST_F(OdbWriterTest, RoundTrip_EmptyJob) {
    // 1. Create original job
    OdbJob originalJob("empty_job");

    auto odbPath = tempDir_ / "empty_job";  // Use job name as directory name

    // 2. Write
    OdbWriter writer;
    ASSERT_TRUE(writer.write(originalJob, odbPath));

    // 3. Read
    OdbReader reader;
    OdbJob loadedJob = reader.read(odbPath);
    ASSERT_FALSE(reader.hasError()) << "Read error: " << reader.getLastError();

    // 4. Verify
    EXPECT_EQ(originalJob.getName(), loadedJob.getName());
    EXPECT_EQ(originalJob.getStepCount(), loadedJob.getStepCount());
}

TEST_F(OdbWriterTest, RoundTrip_JobWithMatrix) {
    // 1. Create job with matrix
    OdbJob originalJob("matrix_job");

    // Add step to matrix
    LayerMatrix::StepDefinition stepDef;
    stepDef.col = 1;
    stepDef.name = "pcb";
    originalJob.getMatrix().addStep(stepDef);

    // Add layers to matrix
    LayerDefinition layer1;
    layer1.row = 1;
    layer1.name = "top";
    layer1.type = LayerType::Signal;
    layer1.context = LayerContext::Board;
    layer1.polarity = Polarity::Positive;
    originalJob.getMatrix().addLayer(layer1);

    LayerDefinition layer2;
    layer2.row = 2;
    layer2.name = "bottom";
    layer2.type = LayerType::Signal;
    layer2.context = LayerContext::Board;
    layer2.polarity = Polarity::Positive;
    originalJob.getMatrix().addLayer(layer2);

    auto odbPath = tempDir_ / "matrix_job";

    // 2. Write
    OdbWriter writer;
    ASSERT_TRUE(writer.write(originalJob, odbPath));

    // 3. Read
    OdbReader reader;
    OdbJob loadedJob = reader.read(odbPath);
    ASSERT_FALSE(reader.hasError());

    // 4. Verify
    EXPECT_EQ(originalJob.getName(), loadedJob.getName());
    EXPECT_EQ(originalJob.getLayerNames().size(), loadedJob.getLayerNames().size());

    auto originalLayers = originalJob.getLayerNames();
    auto loadedLayers = loadedJob.getLayerNames();
    EXPECT_EQ(originalLayers.size(), loadedLayers.size());
}

TEST_F(OdbWriterTest, RoundTrip_JobWithStep) {
    // 1. Create job with step
    OdbJob originalJob("step_job");

    Step& originalStep = originalJob.createStep("pcb");
    originalStep.setDatum(100.0, 100.0);
    originalStep.setXDatum('L');
    originalStep.setYDatum('B');

    // Add profile
    Contour profile(0.0, 0.0, PolygonType::Island);
    profile.addLineSegment(100.0, 0.0);
    profile.addLineSegment(100.0, 100.0);
    profile.addLineSegment(0.0, 100.0);
    profile.addLineSegment(0.0, 0.0);
    originalStep.addProfileContour(profile);

    auto odbPath = tempDir_ / "step_job";

    // 2. Write
    OdbWriter writer;
    ASSERT_TRUE(writer.write(originalJob, odbPath));

    // 3. Read
    OdbReader reader;
    OdbJob loadedJob = reader.read(odbPath);
    ASSERT_FALSE(reader.hasError());

    // 4. Verify
    EXPECT_EQ(originalJob.getStepCount(), loadedJob.getStepCount());

    const Step* loadedStep = loadedJob.getStep("pcb");
    ASSERT_NE(loadedStep, nullptr);

    EXPECT_EQ(originalStep.getName(), loadedStep->getName());
    EXPECT_DOUBLE_EQ(originalStep.getDatum().x, loadedStep->getDatum().x);
    EXPECT_DOUBLE_EQ(originalStep.getDatum().y, loadedStep->getDatum().y);
    EXPECT_EQ(originalStep.getProfile().size(), loadedStep->getProfile().size());
}

TEST_F(OdbWriterTest, RoundTrip_StepWithLayers) {
    // 1. Create job with step and layers
    OdbJob originalJob("layers_job");

    Step& step = originalJob.createStep("pcb");

    auto topLayer = std::make_unique<CopperLayer>("top");
    topLayer->setType(LayerType::Signal);
    step.addLayer(std::move(topLayer));

    auto bottomLayer = std::make_unique<CopperLayer>("bottom");
    bottomLayer->setType(LayerType::Signal);
    step.addLayer(std::move(bottomLayer));

    auto odbPath = tempDir_ / "layers_job";

    // 2. Write
    OdbWriter writer;
    OdbWriter::Options options;
    options.compressFeatures = false;
    ASSERT_TRUE(writer.write(originalJob, odbPath, options));

    // 3. Read
    OdbReader reader;
    OdbJob loadedJob = reader.read(odbPath);
    ASSERT_FALSE(reader.hasError());

    // 4. Verify
    const Step* loadedStep = loadedJob.getStep("pcb");
    ASSERT_NE(loadedStep, nullptr);

    EXPECT_EQ(step.getLayerCount(), loadedStep->getLayerCount());
    EXPECT_NE(loadedStep->getLayer("top"), nullptr);
    EXPECT_NE(loadedStep->getLayer("bottom"), nullptr);
}

TEST_F(OdbWriterTest, RoundTrip_LayersWithFeatures) {
    // 1. Create job with features
    OdbJob originalJob("features_job");

    Step& step = originalJob.createStep("pcb");
    auto topLayer = std::make_unique<CopperLayer>("top");
    Layer& layer = *topLayer;
    step.addLayer(std::move(topLayer));

    // Add line feature
    auto line = std::make_unique<LineFeature>();
    line->setStart(0.0, 0.0);
    line->setEnd(10.0, 10.0);
    line->setSymbolName("r100");
    line->setPolarity(Polarity::Positive);
    layer.addFeature(std::move(line));

    // Add pad feature
    auto pad = std::make_unique<PadFeature>();
    pad->setPosition(5.0, 5.0);
    pad->setSymbolName("s50");
    pad->setPolarity(Polarity::Positive);
    layer.addFeature(std::move(pad));

    auto odbPath = tempDir_ / "features_job";

    // 2. Write (uncompressed for easier debugging)
    OdbWriter writer;
    OdbWriter::Options options;
    options.compressFeatures = false;
    ASSERT_TRUE(writer.write(originalJob, odbPath, options));

    // 3. Read
    OdbReader reader;
    OdbReader::Options readOptions;
    readOptions.loadFeatures = true;
    readOptions.decompressFeatures = true;
    OdbJob loadedJob = reader.read(odbPath, readOptions);
    ASSERT_FALSE(reader.hasError()) << "Read error: " << reader.getLastError();

    // 4. Verify
    const Step* loadedStep = loadedJob.getStep("pcb");
    ASSERT_NE(loadedStep, nullptr);

    const Layer* loadedLayer = loadedStep->getLayer("top");
    ASSERT_NE(loadedLayer, nullptr);

    EXPECT_EQ(2u, loadedLayer->getFeatureCount());
}

TEST_F(OdbWriterTest, RoundTrip_AllFeatureTypes) {
    // 1. Create job with all feature types
    OdbJob originalJob("all_features_job");

    Step& step = originalJob.createStep("pcb");
    auto topLayer = std::make_unique<CopperLayer>("top");
    Layer& layer = *topLayer;
    step.addLayer(std::move(topLayer));

    // Line
    auto line = std::make_unique<LineFeature>();
    line->setStart(0.0, 0.0);
    line->setEnd(10.0, 0.0);
    line->setSymbolName("r50");
    layer.addFeature(std::move(line));

    // Pad
    auto pad = std::make_unique<PadFeature>();
    pad->setPosition(20.0, 0.0);
    pad->setSymbolName("r100");
    layer.addFeature(std::move(pad));

    // Arc
    auto arc = std::make_unique<ArcFeature>();
    arc->setStart(30.0, 0.0);
    arc->setEnd(40.0, 0.0);
    arc->setCenter(35.0, 5.0);
    arc->setClockwise(true);
    arc->setSymbolName("r50");
    layer.addFeature(std::move(arc));

    // Surface
    auto surface = std::make_unique<SurfaceFeature>();
    Contour contour(50.0, 0.0, PolygonType::Island);
    contour.addLineSegment(60.0, 0.0);
    contour.addLineSegment(60.0, 10.0);
    contour.addLineSegment(50.0, 10.0);
    contour.addLineSegment(50.0, 0.0);
    surface->addContour(contour);
    layer.addFeature(std::move(surface));

    auto odbPath = tempDir_ / "all_features_job";

    // 2. Write
    OdbWriter writer;
    OdbWriter::Options options;
    options.compressFeatures = false;
    ASSERT_TRUE(writer.write(originalJob, odbPath, options));

    // 3. Read
    OdbReader reader;
    OdbReader::Options readOptions;
    readOptions.loadFeatures = true;
    OdbJob loadedJob = reader.read(odbPath, readOptions);
    ASSERT_FALSE(reader.hasError());

    // 4. Verify
    const Step* loadedStep = loadedJob.getStep("pcb");
    ASSERT_NE(loadedStep, nullptr);

    const Layer* loadedLayer = loadedStep->getLayer("top");
    ASSERT_NE(loadedLayer, nullptr);

    EXPECT_EQ(4u, loadedLayer->getFeatureCount());
}

TEST_F(OdbWriterTest, RoundTrip_CompressedFiles) {
    // 1. Create job
    OdbJob originalJob("compressed_job");

    Step& step = originalJob.createStep("pcb");
    auto topLayer = std::make_unique<CopperLayer>("top");
    Layer& layer = *topLayer;
    step.addLayer(std::move(topLayer));

    // Add multiple features to make compression worthwhile
    for (int i = 0; i < 100; ++i) {
        auto pad = std::make_unique<PadFeature>();
        pad->setPosition(i * 2.0, i * 2.0);
        pad->setSymbolName("r50");
        layer.addFeature(std::move(pad));
    }

    auto odbPath = tempDir_ / "compressed_job";

    // 2. Write with compression
    OdbWriter writer;
    OdbWriter::Options options;
    options.compressFeatures = true;
    ASSERT_TRUE(writer.write(originalJob, odbPath, options));

    // 3. Verify compressed file exists
    auto compressedPath = odbPath / "steps" / "pcb" / "layers" / "top" / "features.z";
    EXPECT_TRUE(std::filesystem::exists(compressedPath));

    // 4. Read with decompression
    OdbReader reader;
    OdbReader::Options readOptions;
    readOptions.loadFeatures = true;
    readOptions.decompressFeatures = true;
    OdbJob loadedJob = reader.read(odbPath, readOptions);
    ASSERT_FALSE(reader.hasError());

    // 5. Verify
    const Step* loadedStep = loadedJob.getStep("pcb");
    ASSERT_NE(loadedStep, nullptr);

    const Layer* loadedLayer = loadedStep->getLayer("top");
    ASSERT_NE(loadedLayer, nullptr);

    EXPECT_EQ(100u, loadedLayer->getFeatureCount());
}

TEST_F(OdbWriterTest, RoundTrip_EdaData) {
    // 1. Create job with EDA data
    OdbJob originalJob("eda_job");

    Step& step = originalJob.createStep("pcb");
    EdaData& eda = step.getEdaData();

    // Add package
    auto pkg = std::make_unique<Package>("SOIC8");
    pkg->setPitch(1.27);

    Pin pin1;
    pin1.name = "1";
    pin1.x = 0.0;
    pin1.y = 0.0;
    pin1.type = PinType::Smd;
    pkg->addPin(pin1);

    eda.addPackage(std::move(pkg));

    // Add component
    auto comp = std::make_unique<Component>("U1");
    comp->setPackageName("SOIC8");
    comp->setPosition(10.0, 20.0);
    comp->setRotation(0.0);
    comp->setSide(MountSide::Top);
    eda.addComponent(std::move(comp));

    // Add net
    auto net = std::make_unique<Net>("VCC");
    net->setNetNumber(1);
    net->addPin("U1", "1");
    eda.addNet(std::move(net));

    auto odbPath = tempDir_ / "eda_job";

    // 2. Write
    OdbWriter writer;
    OdbWriter::Options options;
    options.compressFeatures = false;
    options.writeEdaData = true;
    ASSERT_TRUE(writer.write(originalJob, odbPath, options));

    // 3. Read
    OdbReader reader;
    OdbReader::Options readOptions;
    readOptions.loadEdaData = true;
    OdbJob loadedJob = reader.read(odbPath, readOptions);
    ASSERT_FALSE(reader.hasError());

    // 4. Verify
    const Step* loadedStep = loadedJob.getStep("pcb");
    ASSERT_NE(loadedStep, nullptr);

    const EdaData& loadedEda = loadedStep->getEdaData();
    EXPECT_EQ(1u, loadedEda.getPackageCount());
    EXPECT_EQ(1u, loadedEda.getComponentCount());
    EXPECT_EQ(1u, loadedEda.getNetCount());

    EXPECT_NE(loadedEda.getPackage("SOIC8"), nullptr);
    EXPECT_NE(loadedEda.getComponent("U1"), nullptr);
    EXPECT_NE(loadedEda.getNet("VCC"), nullptr);
}

TEST_F(OdbWriterTest, RoundTrip_StackupAndImpedance) {
    // 1. Create job with stackup
    OdbJob originalJob("stackup_job");

    StackupLayer layer1;
    layer1.name = "top";
    layer1.materialType = StackupMaterialType::Copper;
    layer1.thickness = 0.035;
    originalJob.addStackupLayer(layer1);

    StackupLayer layer2;
    layer2.name = "core";
    layer2.materialType = StackupMaterialType::Core;
    layer2.thickness = 0.8;
    layer2.dielectricConstant = 4.5;
    originalJob.addStackupLayer(layer2);

    ImpedanceConstraint constraint;
    constraint.id = "diff_100";
    constraint.type = "differential";
    constraint.impedance = 100.0;
    constraint.tolerance = 10.0;
    originalJob.addImpedanceConstraint(constraint);

    auto odbPath = tempDir_ / "stackup_job";

    // 2. Write
    OdbWriter writer;
    OdbWriter::Options options;
    options.writeStackup = true;
    options.writeImpedance = true;
    ASSERT_TRUE(writer.write(originalJob, odbPath, options));

    // 3. Read
    OdbReader reader;
    OdbJob loadedJob = reader.read(odbPath);
    ASSERT_FALSE(reader.hasError());

    // 4. Verify
    EXPECT_EQ(2u, loadedJob.getStackup().size());
    EXPECT_EQ(1u, loadedJob.getImpedanceConstraints().size());
}

TEST_F(OdbWriterTest, RoundTrip_Metadata) {
    // 1. Create job with metadata
    OdbJob originalJob("metadata_job");

    originalJob.setAttribute("custom_attr1", "value1");
    originalJob.setAttribute("custom_attr2", "value2");

    auto odbPath = tempDir_ / "metadata_job";

    // 2. Write
    OdbWriter writer;
    ASSERT_TRUE(writer.write(originalJob, odbPath));

    // 3. Read
    OdbReader reader;
    OdbJob loadedJob = reader.read(odbPath);
    ASSERT_FALSE(reader.hasError());

    // 4. Verify
    EXPECT_EQ(originalJob.getAttribute("custom_attr1"),
              loadedJob.getAttribute("custom_attr1"));
    EXPECT_EQ(originalJob.getAttribute("custom_attr2"),
              loadedJob.getAttribute("custom_attr2"));
}

