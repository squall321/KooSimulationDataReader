#pragma once

#include <koo/Export.hpp>
#include <koo/dyna/ModelVisitor.hpp>
#include <koo/dyna/Node.hpp>
#include <koo/dyna/Element.hpp>
#include <koo/dyna/Part.hpp>
#include <koo/dyna/Material.hpp>
#include <koo/dyna/Section.hpp>
#include <unordered_map>
#include <string>
#include <iosfwd>

namespace koo::dyna {

/**
 * @brief Collects statistics from a model
 *
 * Traverses a model and collects comprehensive statistics about nodes,
 * elements, materials, parts, and other entities.
 *
 * Example usage:
 * @code
 * StatisticsVisitor stats;
 * model.accept(stats);
 *
 * std::cout << "Total nodes: " << stats.getTotalNodeCount() << "\n";
 * std::cout << "Total elements: " << stats.getTotalElementCount() << "\n";
 * std::cout << "Materials: " << stats.getMaterialCount() << "\n";
 *
 * stats.printSummary(std::cout);
 * @endcode
 */
class KOO_API StatisticsVisitor : public ModelVisitor {
public:
    StatisticsVisitor() = default;

    // Node statistics
    void visit(Node& keyword) override {
        nodeCount_ += keyword.getNodes().size();
    }

    // Element statistics
    void visit(ElementShell& keyword) override {
        shellElementCount_ += keyword.getElements().size();
    }

    void visit(ElementSolid& keyword) override {
        solidElementCount_ += keyword.getElements().size();
    }

    void visit(ElementBeam& keyword) override {
        beamElementCount_ += keyword.getElements().size();
    }

    void visit(ElementDiscrete& keyword) override {
        discreteElementCount_ += keyword.getElements().size();
    }

    // Material statistics - track all material types
    void visit(MatElastic& keyword) override {
        materialCount_++;
        materialTypes_["Elastic"]++;
    }

    void visit(MatRigid& keyword) override {
        materialCount_++;
        materialTypes_["Rigid"]++;
    }

    void visit(MatPlasticKinematic& keyword) override {
        materialCount_++;
        materialTypes_["Plastic Kinematic"]++;
    }

    void visit(MatJohnsonCook& keyword) override {
        materialCount_++;
        materialTypes_["Johnson-Cook"]++;
    }

    // Part statistics
    void visit(Part& keyword) override {
        partCount_ += keyword.getParts().size();
    }

    // Section statistics
    void visit(SectionShell& keyword) override {
        sectionCount_++;
        sectionTypes_["Shell"]++;
    }

    void visit(SectionSolid& keyword) override {
        sectionCount_++;
        sectionTypes_["Solid"]++;
    }

    void visit(SectionBeam& keyword) override {
        sectionCount_++;
        sectionTypes_["Beam"]++;
    }

    // Accessors
    size_t getTotalNodeCount() const { return nodeCount_; }
    size_t getTotalElementCount() const {
        return shellElementCount_ + solidElementCount_ + beamElementCount_ + discreteElementCount_;
    }
    size_t getShellElementCount() const { return shellElementCount_; }
    size_t getSolidElementCount() const { return solidElementCount_; }
    size_t getBeamElementCount() const { return beamElementCount_; }
    size_t getDiscreteElementCount() const { return discreteElementCount_; }
    size_t getMaterialCount() const { return materialCount_; }
    size_t getPartCount() const { return partCount_; }
    size_t getSectionCount() const { return sectionCount_; }

    const std::unordered_map<std::string, size_t>& getMaterialTypes() const { return materialTypes_; }
    const std::unordered_map<std::string, size_t>& getSectionTypes() const { return sectionTypes_; }

    // Print formatted summary
    void printSummary(std::ostream& os) const;

private:
    // Counters
    size_t nodeCount_ = 0;
    size_t shellElementCount_ = 0;
    size_t solidElementCount_ = 0;
    size_t beamElementCount_ = 0;
    size_t discreteElementCount_ = 0;
    size_t materialCount_ = 0;
    size_t partCount_ = 0;
    size_t sectionCount_ = 0;

    // Type breakdowns
    std::unordered_map<std::string, size_t> materialTypes_;
    std::unordered_map<std::string, size_t> sectionTypes_;
};

} // namespace koo::dyna
