#include <gtest/gtest.h>
#include <koo/dyna/Model.hpp>
#include <koo/dyna/StatisticsVisitor.hpp>
#include <koo/dyna/ValidationVisitor.hpp>
#include <koo/dyna/Node.hpp>
#include <koo/dyna/Element.hpp>
#include <koo/dyna/Part.hpp>
#include <koo/dyna/Material.hpp>
#include <koo/dyna/Section.hpp>
#include <sstream>

using namespace koo::dyna;

class ModelVisitorTest : public ::testing::Test {
protected:
    void SetUp() override {
        model = std::make_unique<Model>();
    }

    std::unique_ptr<Model> model;
};

TEST_F(ModelVisitorTest, StatisticsVisitor_CountsNodes) {
    // Create nodes
    auto nodes = std::make_unique<Node>();
    nodes->getNodes().push_back({1, 0.0, 0.0, 0.0});
    nodes->getNodes().push_back({2, 1.0, 0.0, 0.0});
    nodes->getNodes().push_back({3, 1.0, 1.0, 0.0});
    nodes->getNodes().push_back({4, 0.0, 1.0, 0.0});
    model->addKeyword(std::move(nodes));

    // Collect statistics
    StatisticsVisitor stats;
    model->accept(stats);

    EXPECT_EQ(stats.getTotalNodeCount(), 4);
}

TEST_F(ModelVisitorTest, StatisticsVisitor_CountsElements) {
    // Create shell elements
    auto shells = std::make_unique<ElementShell>();
    shells->getElements().push_back({1, 1, 1, 2, 3, 4});
    shells->getElements().push_back({2, 1, 5, 6, 7, 8});
    model->addKeyword(std::move(shells));

    // Create solid elements
    auto solids = std::make_unique<ElementSolid>();
    solids->getElements().push_back({3, 1, 1, 2, 3, 4, 5, 6, 7, 8});
    model->addKeyword(std::move(solids));

    // Collect statistics
    StatisticsVisitor stats;
    model->accept(stats);

    EXPECT_EQ(stats.getShellElementCount(), 2);
    EXPECT_EQ(stats.getSolidElementCount(), 1);
    EXPECT_EQ(stats.getTotalElementCount(), 3);
}

TEST_F(ModelVisitorTest, StatisticsVisitor_CountsMaterials) {
    // Create materials
    auto mat1 = std::make_unique<MatElastic>();
    mat1->getData().id = 1;
    mat1->getData().ro = 7850.0;
    mat1->getData().e = 2.1e11;
    mat1->getData().pr = 0.3;
    model->addKeyword(std::move(mat1));

    auto mat2 = std::make_unique<MatRigid>();
    mat2->getData().id = 2;
    model->addKeyword(std::move(mat2));

    auto mat3 = std::make_unique<MatPlasticKinematic>();
    mat3->getData().mid = 3;
    model->addKeyword(std::move(mat3));

    // Collect statistics
    StatisticsVisitor stats;
    model->accept(stats);

    EXPECT_EQ(stats.getMaterialCount(), 3);

    auto& matTypes = stats.getMaterialTypes();
    EXPECT_EQ(matTypes.at("Elastic"), 1);
    EXPECT_EQ(matTypes.at("Rigid"), 1);
    EXPECT_EQ(matTypes.at("Plastic Kinematic"), 1);
}

TEST_F(ModelVisitorTest, StatisticsVisitor_CountsParts) {
    // Create parts
    auto parts = std::make_unique<Part>();
    parts->getParts().push_back({1, 1, 1, "Part 1"});
    parts->getParts().push_back({2, 2, 2, "Part 2"});
    parts->getParts().push_back({3, 3, 3, "Part 3"});
    model->addKeyword(std::move(parts));

    // Collect statistics
    StatisticsVisitor stats;
    model->accept(stats);

    EXPECT_EQ(stats.getPartCount(), 3);
}

TEST_F(ModelVisitorTest, StatisticsVisitor_PrintsSummary) {
    // Create a complete model
    auto nodes = std::make_unique<Node>();
    nodes->getNodes().push_back({1, 0.0, 0.0, 0.0});
    nodes->getNodes().push_back({2, 1.0, 0.0, 0.0});
    model->addKeyword(std::move(nodes));

    auto shells = std::make_unique<ElementShell>();
    shells->getElements().push_back({1, 1, 1, 2, 3, 4});
    model->addKeyword(std::move(shells));

    auto mat = std::make_unique<MatElastic>();
    mat->getData().id = 1;
    model->addKeyword(std::move(mat));

    // Collect and print statistics
    StatisticsVisitor stats;
    model->accept(stats);

    std::ostringstream oss;
    stats.printSummary(oss);

    std::string output = oss.str();
    EXPECT_TRUE(output.find("Total Nodes:") != std::string::npos);
    EXPECT_TRUE(output.find("Total Elements:") != std::string::npos);
    EXPECT_TRUE(output.find("Total Materials:") != std::string::npos);
}

TEST_F(ModelVisitorTest, ValidationVisitor_DetectsDuplicateNodeIds) {
    // Create nodes with duplicate ID
    auto nodes = std::make_unique<Node>();
    nodes->getNodes().push_back({1, 0.0, 0.0, 0.0});
    nodes->getNodes().push_back({1, 1.0, 0.0, 0.0}); // Duplicate ID!
    nodes->getNodes().push_back({2, 2.0, 0.0, 0.0});
    model->addKeyword(std::move(nodes));

    // Validate
    ValidationVisitor validator;
    model->accept(validator);
    validator.finalizeValidation();

    EXPECT_TRUE(validator.getErrorCount() > 0);
    EXPECT_GE(validator.getErrorCount(), 1);

    // Check that error message mentions duplicate
    const auto& messages = validator.getMessages();
    bool foundDuplicateError = false;
    for (const auto& msg : messages) {
        if (msg.severity == ValidationSeverity::Error &&
            msg.message.find("Duplicate node ID") != std::string::npos) {
            foundDuplicateError = true;
            break;
        }
    }
    EXPECT_TRUE(foundDuplicateError);
}

TEST_F(ModelVisitorTest, ValidationVisitor_DetectsDuplicateElementIds) {
    // Create elements with duplicate ID
    auto shells = std::make_unique<ElementShell>();
    shells->getElements().push_back({1, 1, 1, 2, 3, 4});
    shells->getElements().push_back({1, 1, 5, 6, 7, 8}); // Duplicate ID!
    model->addKeyword(std::move(shells));

    // Validate
    ValidationVisitor validator;
    model->accept(validator);
    validator.finalizeValidation();

    EXPECT_TRUE(validator.getErrorCount() > 0);

    // Check for duplicate element error
    const auto& messages = validator.getMessages();
    bool foundDuplicateElement = false;
    for (const auto& msg : messages) {
        if (msg.severity == ValidationSeverity::Error &&
            msg.category == "Elements" &&
            msg.message.find("Duplicate element ID") != std::string::npos) {
            foundDuplicateElement = true;
            break;
        }
    }
    EXPECT_TRUE(foundDuplicateElement);
}

TEST_F(ModelVisitorTest, ValidationVisitor_DetectsUnusedMaterial) {
    // Create a material that's never used
    auto mat = std::make_unique<MatElastic>();
    mat->getData().id = 1;
    model->addKeyword(std::move(mat));

    // Validate
    ValidationVisitor validator;
    model->accept(validator);
    validator.finalizeValidation();

    EXPECT_TRUE(validator.getMessages().size() > 0);

    // Check for unused material info
    const auto& messages = validator.getMessages();
    bool foundUnusedMaterial = false;
    for (const auto& msg : messages) {
        if (msg.severity == ValidationSeverity::Warning &&
            msg.message.find("not referenced") != std::string::npos) {
            foundUnusedMaterial = true;
            break;
        }
    }
    EXPECT_TRUE(foundUnusedMaterial);
}

TEST_F(ModelVisitorTest, ValidationVisitor_PassesValidModel) {
    // Create a valid model
    auto nodes = std::make_unique<Node>();
    nodes->getNodes().push_back({1, 0.0, 0.0, 0.0});
    nodes->getNodes().push_back({2, 1.0, 0.0, 0.0});
    nodes->getNodes().push_back({3, 1.0, 1.0, 0.0});
    nodes->getNodes().push_back({4, 0.0, 1.0, 0.0});
    model->addKeyword(std::move(nodes));

    auto mat = std::make_unique<MatElastic>();
    mat->getData().id = 1;
    mat->getData().ro = 7850.0;
    mat->getData().e = 2.1e11;
    model->addKeyword(std::move(mat));

    auto part = std::make_unique<Part>();
    part->getParts().push_back({1, 1, 1, "Test Part"});
    model->addKeyword(std::move(part));

    auto shells = std::make_unique<ElementShell>();
    shells->getElements().push_back({1, 1, 1, 2, 3, 4});
    model->addKeyword(std::move(shells));

    // Validate
    ValidationVisitor validator;
    model->accept(validator);
    validator.finalizeValidation();

    EXPECT_FALSE(validator.getErrorCount() > 0);
}

TEST_F(ModelVisitorTest, ValidationVisitor_PrintsMessages) {
    // Create model with some issues
    auto nodes = std::make_unique<Node>();
    nodes->getNodes().push_back({1, 0.0, 0.0, 0.0});
    nodes->getNodes().push_back({1, 1.0, 0.0, 0.0}); // Duplicate!
    model->addKeyword(std::move(nodes));

    ValidationVisitor validator;
    model->accept(validator);
    validator.finalizeValidation();

    std::ostringstream oss;
    validator.printMessages(oss);

    std::string output = oss.str();
    EXPECT_TRUE(output.find("[ERROR]") != std::string::npos);
    EXPECT_TRUE(output.find("Validation Summary:") != std::string::npos);
}

TEST_F(ModelVisitorTest, ValidationVisitor_MultipleValidations) {
    // First model with duplicate nodes
    auto nodes = std::make_unique<Node>();
    nodes->getNodes().push_back({1, 0.0, 0.0, 0.0});
    nodes->getNodes().push_back({1, 1.0, 0.0, 0.0}); // Duplicate
    model->addKeyword(std::move(nodes));

    ValidationVisitor validator;
    model->accept(validator);
    validator.finalizeValidation();

    EXPECT_GT(validator.getMessages().size(), 0);
    EXPECT_GT(validator.getErrorCount(), 0);

    // Create a new validator for a different model
    ValidationVisitor newValidator;
    Model newModel;
    auto goodNodes = std::make_unique<Node>();
    goodNodes->getNodes().push_back({1, 0.0, 0.0, 0.0});
    newModel.addKeyword(std::move(goodNodes));

    newModel.accept(newValidator);
    newValidator.finalizeValidation();

    EXPECT_EQ(newValidator.getErrorCount(), 0);
    EXPECT_EQ(newValidator.getWarningCount(), 0);
}

TEST_F(ModelVisitorTest, StatisticsVisitor_CanReset) {
    auto nodes = std::make_unique<Node>();
    nodes->getNodes().push_back({1, 0.0, 0.0, 0.0});
    model->addKeyword(std::move(nodes));

    StatisticsVisitor stats;
    model->accept(stats);

    EXPECT_GT(stats.getTotalNodeCount(), 0);

    // Test that creating a new visitor starts with zero counts
    StatisticsVisitor newStats;
    EXPECT_EQ(newStats.getTotalNodeCount(), 0);
    EXPECT_EQ(newStats.getTotalElementCount(), 0);
    EXPECT_EQ(newStats.getMaterialCount(), 0);
}
