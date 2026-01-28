#include <gtest/gtest.h>
#include <koo/ecad/OdbReader.hpp>
#include <koo/ecad/OdbWriter.hpp>
#include <filesystem>
#include <fstream>

using namespace koo::ecad;

// Test helper to create test ODB++ data
class OdbReaderTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create temp directory for tests
        tempDir_ = std::filesystem::temp_directory_path() / "odb_reader_test";
        std::filesystem::create_directories(tempDir_);
    }

    void TearDown() override {
        // Clean up temp directory
        if (std::filesystem::exists(tempDir_)) {
            std::filesystem::remove_all(tempDir_);
        }
    }

    // Helper to create a simple ODB++ structure for testing
    void createSimpleOdbStructure(const std::filesystem::path& odbPath) {
        OdbWriter writer;
        OdbJob job("test_job");

        // Add step
        Step& step = job.createStep("pcb");
        step.setDatum(0.0, 0.0);

        // Add simple profile
        Contour profile(0.0, 0.0, PolygonType::Island);
        profile.addLineSegment(10.0, 0.0);
        profile.addLineSegment(10.0, 10.0);
        profile.addLineSegment(0.0, 10.0);
        profile.addLineSegment(0.0, 0.0);
        step.addProfileContour(profile);

        // Add layer
        auto layer = std::make_unique<CopperLayer>("top");
        step.addLayer(std::move(layer));

        OdbWriter::Options options;
        options.compressFeatures = false;
        writer.write(job, odbPath, options);
    }

    std::filesystem::path tempDir_;
};

// ============================================================================
// Basic Reading Tests
// ============================================================================

TEST_F(OdbReaderTest, ReadSimpleJob) {
    auto odbPath = tempDir_ / "simple_job";
    createSimpleOdbStructure(odbPath);

    OdbReader reader;
    OdbJob job = reader.read(odbPath);

    EXPECT_FALSE(reader.hasError()) << "Error: " << reader.getLastError();
    // Job name comes from directory name, not the OdbJob constructor
    EXPECT_EQ("simple_job", job.getName());
    EXPECT_EQ(1u, job.getStepCount());
}

TEST_F(OdbReaderTest, ReadMatrix) {
    auto odbPath = tempDir_ / "matrix_test";
    createSimpleOdbStructure(odbPath);

    OdbReader reader;
    LayerMatrix matrix = reader.readMatrix(odbPath);

    EXPECT_FALSE(reader.hasError());
    // Matrix should have at least the step we created
}

TEST_F(OdbReaderTest, ListSteps) {
    auto odbPath = tempDir_ / "list_steps";
    createSimpleOdbStructure(odbPath);

    OdbReader reader;
    auto steps = reader.listSteps(odbPath);

    EXPECT_FALSE(reader.hasError());
    EXPECT_EQ(1u, steps.size());
    EXPECT_EQ("pcb", steps[0]);
}

TEST_F(OdbReaderTest, ReadSingleStep) {
    auto odbPath = tempDir_ / "single_step";
    createSimpleOdbStructure(odbPath);

    OdbReader reader;
    auto step = reader.readStep(odbPath, "pcb");

    EXPECT_FALSE(reader.hasError());
    ASSERT_NE(step, nullptr);
    EXPECT_EQ("pcb", step->getName());
}

TEST_F(OdbReaderTest, ReadSingleLayer) {
    auto odbPath = tempDir_ / "single_layer";
    createSimpleOdbStructure(odbPath);

    OdbReader reader;
    auto layer = reader.readLayer(odbPath, "pcb", "top");

    EXPECT_FALSE(reader.hasError());
    ASSERT_NE(layer, nullptr);
    EXPECT_EQ("top", layer->getName());
}

// ============================================================================
// Reading Options Tests
// ============================================================================

TEST_F(OdbReaderTest, ReadWithOptions_NoFeatures) {
    auto odbPath = tempDir_ / "no_features";
    createSimpleOdbStructure(odbPath);

    OdbReader reader;
    OdbReader::Options options;
    options.loadFeatures = false;
    OdbJob job = reader.read(odbPath, options);

    EXPECT_FALSE(reader.hasError());
    EXPECT_EQ(1u, job.getStepCount());
}

TEST_F(OdbReaderTest, ReadWithOptions_NoEdaData) {
    auto odbPath = tempDir_ / "no_eda";
    createSimpleOdbStructure(odbPath);

    OdbReader reader;
    OdbReader::Options options;
    options.loadEdaData = false;
    OdbJob job = reader.read(odbPath, options);

    EXPECT_FALSE(reader.hasError());
}

TEST_F(OdbReaderTest, ReadWithOptions_NoSymbols) {
    auto odbPath = tempDir_ / "no_symbols";
    createSimpleOdbStructure(odbPath);

    OdbReader reader;
    OdbReader::Options options;
    options.loadSymbols = false;
    OdbJob job = reader.read(odbPath, options);

    EXPECT_FALSE(reader.hasError());
}

TEST_F(OdbReaderTest, ReadWithOptions_StepFilter) {
    auto odbPath = tempDir_ / "step_filter";

    // Create job with multiple steps
    OdbWriter writer;
    OdbJob originalJob("multi_step");
    originalJob.createStep("step1");
    originalJob.createStep("step2");
    originalJob.createStep("step3");
    writer.write(originalJob, odbPath);

    // Read only step2
    OdbReader reader;
    OdbReader::Options options;
    options.stepFilter = {"step2"};
    OdbJob job = reader.read(odbPath, options);

    EXPECT_FALSE(reader.hasError());
    EXPECT_EQ(1u, job.getStepCount());
    EXPECT_NE(job.getStep("step2"), nullptr);
}

// ============================================================================
// Compressed Files Tests
// ============================================================================

TEST_F(OdbReaderTest, ReadCompressedFeatures) {
    auto odbPath = tempDir_ / "compressed";

    // Create job with compressed features
    OdbWriter writer;
    OdbJob originalJob("compressed_job");
    Step& step = originalJob.createStep("pcb");
    auto layer = std::make_unique<CopperLayer>("top");

    // Add features
    auto pad = std::make_unique<PadFeature>();
    pad->setPosition(5.0, 5.0);
    pad->setSymbolName("r50");
    layer->addFeature(std::move(pad));

    step.addLayer(std::move(layer));

    OdbWriter::Options writeOptions;
    writeOptions.compressFeatures = true;
    writer.write(originalJob, odbPath, writeOptions);

    // Read with decompression
    OdbReader reader;
    OdbReader::Options readOptions;
    readOptions.loadFeatures = true;
    readOptions.decompressFeatures = true;
    OdbJob job = reader.read(odbPath, readOptions);

    EXPECT_FALSE(reader.hasError());

    const Step* loadedStep = job.getStep("pcb");
    ASSERT_NE(loadedStep, nullptr);

    const Layer* loadedLayer = loadedStep->getLayer("top");
    ASSERT_NE(loadedLayer, nullptr);
    EXPECT_EQ(1u, loadedLayer->getFeatureCount());
}

TEST_F(OdbReaderTest, ReadUncompressedFeatures) {
    auto odbPath = tempDir_ / "uncompressed";

    // Create job with uncompressed features
    OdbWriter writer;
    OdbJob originalJob("uncompressed_job");
    Step& step = originalJob.createStep("pcb");
    auto layer = std::make_unique<CopperLayer>("top");

    auto pad = std::make_unique<PadFeature>();
    pad->setPosition(5.0, 5.0);
    pad->setSymbolName("r50");
    layer->addFeature(std::move(pad));

    step.addLayer(std::move(layer));

    OdbWriter::Options writeOptions;
    writeOptions.compressFeatures = false;
    writer.write(originalJob, odbPath, writeOptions);

    // Read
    OdbReader reader;
    OdbReader::Options readOptions;
    readOptions.loadFeatures = true;
    OdbJob job = reader.read(odbPath, readOptions);

    EXPECT_FALSE(reader.hasError());

    const Step* loadedStep = job.getStep("pcb");
    ASSERT_NE(loadedStep, nullptr);

    const Layer* loadedLayer = loadedStep->getLayer("top");
    ASSERT_NE(loadedLayer, nullptr);
    EXPECT_EQ(1u, loadedLayer->getFeatureCount());
}

// ============================================================================
// Error Handling Tests
// ============================================================================

TEST_F(OdbReaderTest, ErrorHandling_FileNotFound) {
    auto odbPath = tempDir_ / "nonexistent";

    OdbReader reader;
    // OdbReader throws exception for non-existent paths
    EXPECT_THROW(reader.read(odbPath), std::runtime_error);
}

TEST_F(OdbReaderTest, ErrorHandling_InvalidStep) {
    auto odbPath = tempDir_ / "invalid_step";
    createSimpleOdbStructure(odbPath);

    OdbReader reader;
    auto step = reader.readStep(odbPath, "nonexistent_step");

    EXPECT_TRUE(reader.hasError());
    EXPECT_EQ(step, nullptr);
}

TEST_F(OdbReaderTest, ErrorHandling_InvalidLayer) {
    auto odbPath = tempDir_ / "invalid_layer";
    createSimpleOdbStructure(odbPath);

    OdbReader reader;
    auto layer = reader.readLayer(odbPath, "pcb", "nonexistent_layer");

    EXPECT_TRUE(reader.hasError());
    EXPECT_EQ(layer, nullptr);
}

TEST_F(OdbReaderTest, ErrorHandling_CorruptedMatrix) {
    auto odbPath = tempDir_ / "corrupted";
    createSimpleOdbStructure(odbPath);

    // Corrupt the matrix file
    auto matrixPath = odbPath / "matrix" / "matrix";
    std::ofstream corruptFile(matrixPath);
    corruptFile << "CORRUPTED DATA !!@#$%";
    corruptFile.close();

    OdbReader reader;
    OdbJob job = reader.read(odbPath);

    // Should still succeed but with warnings or partial data
    // The reader should be resilient to corrupted data
}

// ============================================================================
// Data Verification Tests
// ============================================================================

TEST_F(OdbReaderTest, ReadEdaData) {
    auto odbPath = tempDir_ / "eda_test";

    // Create job with EDA data
    OdbWriter writer;
    OdbJob originalJob("eda_test");
    Step& step = originalJob.createStep("pcb");
    EdaData& eda = step.getEdaData();

    auto pkg = std::make_unique<Package>("PKG1");
    eda.addPackage(std::move(pkg));

    auto comp = std::make_unique<Component>("U1");
    comp->setPackageName("PKG1");
    comp->setPackageIndex(0);  // Set package index
    eda.addComponent(std::move(comp));

    OdbWriter::Options writeOptions;
    writeOptions.writeEdaData = true;
    writeOptions.compressFeatures = false;  // Don't compress for easier debugging
    writer.write(originalJob, odbPath, writeOptions);

    // Read
    OdbReader reader;
    OdbReader::Options readOptions;
    readOptions.loadEdaData = true;
    OdbJob job = reader.read(odbPath, readOptions);

    EXPECT_FALSE(reader.hasError());

    const Step* loadedStep = job.getStep("pcb");
    ASSERT_NE(loadedStep, nullptr);

    const EdaData& loadedEda = loadedStep->getEdaData();
    EXPECT_EQ(1u, loadedEda.getPackageCount());
    EXPECT_EQ(1u, loadedEda.getComponentCount());
}

TEST_F(OdbReaderTest, ReadStackup) {
    auto odbPath = tempDir_ / "stackup_test";

    // Create job with stackup
    OdbWriter writer;
    OdbJob originalJob("stackup_job");

    StackupLayer layer;
    layer.name = "top";
    layer.materialType = StackupMaterialType::Copper;
    layer.thickness = 0.035;
    originalJob.addStackupLayer(layer);

    OdbWriter::Options writeOptions;
    writeOptions.writeStackup = true;
    writer.write(originalJob, odbPath, writeOptions);

    // Read
    OdbReader reader;
    OdbJob job = reader.read(odbPath);

    EXPECT_FALSE(reader.hasError());
    EXPECT_EQ(1u, job.getStackup().size());
}

TEST_F(OdbReaderTest, ReadSymbols) {
    auto odbPath = tempDir_ / "symbols_test";

    // Create job with user symbols
    OdbWriter writer;
    OdbJob originalJob("symbols_job");

    auto symbol = std::make_unique<Symbol>("custom_pad");
    symbol->setType(SymbolType::User);

    auto line = std::make_unique<LineFeature>();
    line->setStart(0.0, 0.0);
    line->setEnd(5.0, 5.0);
    symbol->addFeature(std::move(line));

    originalJob.addSymbol(std::move(symbol));

    OdbWriter::Options writeOptions;
    writeOptions.writeSymbols = true;
    writeOptions.compressFeatures = false;
    writer.write(originalJob, odbPath, writeOptions);

    // Read
    OdbReader reader;
    OdbReader::Options readOptions;
    readOptions.loadSymbols = true;
    OdbJob job = reader.read(odbPath, readOptions);

    EXPECT_FALSE(reader.hasError());
    EXPECT_NE(job.getSymbol("custom_pad"), nullptr);
}

// ============================================================================
// Progress Callback Test
// ============================================================================

TEST_F(OdbReaderTest, ProgressCallback) {
    auto odbPath = tempDir_ / "progress_test";
    createSimpleOdbStructure(odbPath);

    std::vector<std::pair<std::string, double>> progressReports;

    OdbReader reader;
    reader.setProgressCallback([&](const std::string& message, double progress) {
        progressReports.push_back({message, progress});
    });

    OdbJob job = reader.read(odbPath);

    EXPECT_FALSE(reader.hasError());

    // Should have received progress reports
    EXPECT_FALSE(progressReports.empty());

    // Progress should be between 0.0 and 1.0
    for (const auto& report : progressReports) {
        EXPECT_GE(report.second, 0.0);
        EXPECT_LE(report.second, 1.0);
    }
}
