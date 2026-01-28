/**
 * @file TestOdbReaderIntegration.cpp
 * @brief Integration tests for OdbReader with real ODB++ files
 */

#include <gtest/gtest.h>
#include <koo/ecad/OdbReader.hpp>
#include <koo/ecad/OdbJob.hpp>
#include <filesystem>
#include <iostream>
#include <iomanip>

using namespace koo::ecad;
namespace fs = std::filesystem;

/**
 * @brief Test fixture for ODB++ file integration tests
 */
class OdbReaderIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Find the examples/ECAD directory
        fs::path currentPath = fs::current_path();

        // Try different relative paths
        std::vector<fs::path> searchPaths = {
            currentPath / "examples/ECAD",
            currentPath / "../examples/ECAD",
            currentPath / "../../examples/ECAD",
            currentPath / "../../../examples/ECAD",
        };

        for (const auto& p : searchPaths) {
            if (fs::exists(p)) {
                examplesPath_ = fs::canonical(p);
                break;
            }
        }
    }

    fs::path examplesPath_;
};

/**
 * @brief Test reading a complete ODB++ job from zip file
 */
TEST_F(OdbReaderIntegrationTest, ReadP3EurRev03) {
    // Skip test if examples directory not found
    if (examplesPath_.empty()) {
        GTEST_SKIP() << "Examples directory not found";
    }

    fs::path odbZipPath = examplesPath_ / "P3_EUR_REV03.zip";
    if (!fs::exists(odbZipPath)) {
        GTEST_SKIP() << "P3_EUR_REV03.zip not found at " << odbZipPath;
    }

    // Extract zip to temp directory
    fs::path tempDir = fs::temp_directory_path() / "odb_test_p3";
    if (fs::exists(tempDir)) {
        fs::remove_all(tempDir);
    }
    fs::create_directories(tempDir);

    // Unzip
    std::string unzipCmd = "unzip -q \"" + odbZipPath.string() + "\" -d \"" + tempDir.string() + "\"";
    int result = std::system(unzipCmd.c_str());
    ASSERT_EQ(result, 0) << "Failed to unzip ODB++ file";

    // Find the extracted ODB++ directory
    fs::path odbPath = tempDir;

    // Read the ODB++ job
    OdbReader reader;

    // Set up progress callback for visibility
    std::vector<std::string> progressMessages;
    reader.setProgressCallback([&](const std::string& msg, double progress) {
        progressMessages.push_back(msg);
        std::cout << "[" << std::fixed << std::setprecision(1) << (progress * 100) << "%] " << msg << std::endl;
    });

    OdbJob job = reader.read(odbPath);

    // Check for errors
    if (reader.hasError()) {
        std::cout << "Reader error: " << reader.getLastError() << std::endl;
    }

    // ========== Print Summary ==========
    std::cout << "\n========== ODB++ Job Summary ==========" << std::endl;
    std::cout << "Job Name: " << job.getName() << std::endl;

    // Matrix info
    const auto& matrix = job.getMatrix();
    std::cout << "\n--- Matrix ---" << std::endl;
    std::cout << "Step definitions: " << matrix.getStepDefinitions().size() << std::endl;
    std::cout << "Layer definitions: " << matrix.getLayerDefinitions().size() << std::endl;

    for (const auto& stepDef : matrix.getStepDefinitions()) {
        std::cout << "  Step: " << stepDef.name << " (col=" << stepDef.col << ")" << std::endl;
    }

    for (const auto& layerDef : matrix.getLayerDefinitions()) {
        std::cout << "  Layer: " << layerDef.name
                  << " (type=" << static_cast<int>(layerDef.type)
                  << ", context=" << static_cast<int>(layerDef.context)
                  << ", row=" << layerDef.row << ")" << std::endl;
    }

    // Steps
    std::cout << "\n--- Steps ---" << std::endl;
    auto stepNames = job.getStepNames();
    std::cout << "Total steps: " << stepNames.size() << std::endl;

    for (const auto& stepName : stepNames) {
        const Step* step = job.getStep(stepName);
        if (!step) continue;

        std::cout << "\nStep: " << stepName << std::endl;
        std::cout << "  Layers: " << step->getLayerCount() << std::endl;

        // Profile
        const auto& profile = step->getProfile();
        std::cout << "  Profile contours: " << profile.size() << std::endl;

        // Layers in step
        auto layerNames = step->getLayerNames();
        size_t totalFeatures = 0;
        for (const auto& layerName : layerNames) {
            const Layer* layer = step->getLayer(layerName);
            if (layer) {
                totalFeatures += layer->getFeatureCount();
            }
        }
        std::cout << "  Total features: " << totalFeatures << std::endl;

        // EDA Data
        if (step->hasEdaData()) {
            const auto& eda = step->getEdaData();
            std::cout << "\n  --- EDA Data ---" << std::endl;
            std::cout << "  Packages: " << eda.getPackageCount() << std::endl;
            std::cout << "  Components: " << eda.getComponentCount() << std::endl;
            std::cout << "  Nets: " << eda.getNetCount() << std::endl;
            std::cout << "  Total pins: " << eda.getTotalPinCount() << std::endl;
            std::cout << "  BOM items: " << eda.getBomItemCount() << std::endl;

            // Sample packages
            auto pkgNames = eda.getPackageNames();
            std::cout << "\n  Sample packages (first 5):" << std::endl;
            int count = 0;
            for (const auto& name : pkgNames) {
                if (count++ >= 5) break;
                const Package* pkg = eda.getPackage(name);
                if (pkg) {
                    std::cout << "    - " << name << " (pins: " << pkg->getPinCount() << ")" << std::endl;
                }
            }

            // Sample components
            auto refDesigns = eda.getComponentRefDes();
            std::cout << "\n  Sample components (first 5):" << std::endl;
            count = 0;
            for (const auto& rd : refDesigns) {
                if (count++ >= 5) break;
                const Component* comp = eda.getComponent(rd);
                if (comp) {
                    std::cout << "    - " << rd
                              << " (pkg: " << comp->getPackageName()
                              << ", pos: " << comp->getPosition().x << "," << comp->getPosition().y
                              << ", side: " << (comp->getSide() == MountSide::Top ? "Top" : "Bottom")
                              << ")" << std::endl;
                }
            }

            // Sample nets
            auto netNames = eda.getNetNames();
            std::cout << "\n  Sample nets (first 5):" << std::endl;
            count = 0;
            for (const auto& name : netNames) {
                if (count++ >= 5) break;
                const Net* net = eda.getNet(name);
                if (net) {
                    std::cout << "    - " << name << " (pins: " << net->getPinCount() << ")" << std::endl;
                }
            }
        }
    }

    // Symbols
    std::cout << "\n--- Symbols ---" << std::endl;
    auto symbolNames = job.getSymbolNames();
    std::cout << "Total symbols: " << symbolNames.size() << std::endl;
    int symCount = 0;
    for (const auto& name : symbolNames) {
        if (symCount++ >= 10) {
            std::cout << "  ... and " << (symbolNames.size() - 10) << " more" << std::endl;
            break;
        }
        const Symbol* sym = job.getSymbol(name);
        if (sym) {
            std::cout << "  - " << name << " (features: " << sym->getFeatures().size() << ")" << std::endl;
        }
    }

    // Metadata
    std::cout << "\n--- Metadata ---" << std::endl;
    const auto& meta = job.getMetadata();
    std::cout << "ODB++ Version: " << meta.version << std::endl;
    std::cout << "Source: " << meta.source << std::endl;
    std::cout << "Creation date: " << meta.creationDate << std::endl;
    std::cout << "Modification date: " << meta.modificationDate << std::endl;

    // Cleanup
    fs::remove_all(tempDir);

    // ========== Assertions ==========
    // These are basic sanity checks that should pass for any valid ODB++ file

    EXPECT_FALSE(job.getName().empty()) << "Job should have a name";
    EXPECT_GT(matrix.getLayerDefinitions().size(), 0) << "Should have layer definitions";
    EXPECT_GT(stepNames.size(), 0) << "Should have at least one step";

    // Check that we loaded the step
    for (const auto& stepName : stepNames) {
        const Step* step = job.getStep(stepName);
        ASSERT_NE(step, nullptr) << "Step '" << stepName << "' should be loaded";
        EXPECT_GT(step->getLayerCount(), 0) << "Step should have layers";
    }

    std::cout << "\n========== Test Complete ==========" << std::endl;
}

/**
 * @brief Test reading individual components of an ODB++ file
 */
TEST_F(OdbReaderIntegrationTest, ReadMatrixOnly) {
    if (examplesPath_.empty()) {
        GTEST_SKIP() << "Examples directory not found";
    }

    fs::path odbZipPath = examplesPath_ / "P3_EUR_REV03.zip";
    if (!fs::exists(odbZipPath)) {
        GTEST_SKIP() << "P3_EUR_REV03.zip not found";
    }

    // Extract zip to temp directory
    fs::path tempDir = fs::temp_directory_path() / "odb_test_matrix";
    if (fs::exists(tempDir)) {
        fs::remove_all(tempDir);
    }
    fs::create_directories(tempDir);

    std::string unzipCmd = "unzip -q \"" + odbZipPath.string() + "\" -d \"" + tempDir.string() + "\"";
    int result = std::system(unzipCmd.c_str());
    ASSERT_EQ(result, 0) << "Failed to unzip ODB++ file";

    OdbReader reader;
    LayerMatrix matrix = reader.readMatrix(tempDir);

    EXPECT_GT(matrix.getLayerDefinitions().size(), 0);
    EXPECT_GT(matrix.getStepDefinitions().size(), 0);

    // Print layer matrix
    std::cout << "\nLayer Matrix:" << std::endl;
    for (const auto& layerDef : matrix.getLayerDefinitions()) {
        std::cout << "  " << layerDef.name
                  << " [row " << layerDef.row << "]"
                  << " type=" << static_cast<int>(layerDef.type)
                  << " polarity=" << (layerDef.polarity == Polarity::Positive ? "pos" : "neg")
                  << std::endl;
    }

    // Cleanup
    fs::remove_all(tempDir);
}

/**
 * @brief Test listing steps in ODB++ file
 */
TEST_F(OdbReaderIntegrationTest, ListSteps) {
    if (examplesPath_.empty()) {
        GTEST_SKIP() << "Examples directory not found";
    }

    fs::path odbZipPath = examplesPath_ / "P3_EUR_REV03.zip";
    if (!fs::exists(odbZipPath)) {
        GTEST_SKIP() << "P3_EUR_REV03.zip not found";
    }

    // Extract zip to temp directory
    fs::path tempDir = fs::temp_directory_path() / "odb_test_steps";
    if (fs::exists(tempDir)) {
        fs::remove_all(tempDir);
    }
    fs::create_directories(tempDir);

    std::string unzipCmd = "unzip -q \"" + odbZipPath.string() + "\" -d \"" + tempDir.string() + "\"";
    int result = std::system(unzipCmd.c_str());
    ASSERT_EQ(result, 0) << "Failed to unzip ODB++ file";

    OdbReader reader;
    auto steps = reader.listSteps(tempDir);

    EXPECT_GT(steps.size(), 0);

    std::cout << "\nSteps in ODB++:" << std::endl;
    for (const auto& step : steps) {
        std::cout << "  - " << step << std::endl;
    }

    // Cleanup
    fs::remove_all(tempDir);
}

/**
 * @brief Test reading with selective options
 */
TEST_F(OdbReaderIntegrationTest, ReadWithOptions) {
    if (examplesPath_.empty()) {
        GTEST_SKIP() << "Examples directory not found";
    }

    fs::path odbZipPath = examplesPath_ / "P3_EUR_REV03.zip";
    if (!fs::exists(odbZipPath)) {
        GTEST_SKIP() << "P3_EUR_REV03.zip not found";
    }

    // Extract zip to temp directory
    fs::path tempDir = fs::temp_directory_path() / "odb_test_options";
    if (fs::exists(tempDir)) {
        fs::remove_all(tempDir);
    }
    fs::create_directories(tempDir);

    std::string unzipCmd = "unzip -q \"" + odbZipPath.string() + "\" -d \"" + tempDir.string() + "\"";
    int result = std::system(unzipCmd.c_str());
    ASSERT_EQ(result, 0) << "Failed to unzip ODB++ file";

    // Read without features (metadata only)
    OdbReader::Options options;
    options.loadFeatures = false;
    options.loadEdaData = true;
    options.loadSymbols = false;

    OdbReader reader;
    OdbJob job = reader.read(tempDir, options);

    // Should still have basic structure
    EXPECT_GT(job.getStepNames().size(), 0);
    EXPECT_EQ(job.getSymbolNames().size(), 0); // Symbols not loaded

    // EDA data should be loaded
    for (const auto& stepName : job.getStepNames()) {
        const Step* step = job.getStep(stepName);
        if (step && step->hasEdaData()) {
            EXPECT_GT(step->getEdaData().getComponentCount(), 0);
        }
    }

    // Cleanup
    fs::remove_all(tempDir);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
