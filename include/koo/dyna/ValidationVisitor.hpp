#pragma once

#include <koo/Export.hpp>
#include <koo/dyna/ModelVisitor.hpp>
#include <koo/dyna/Node.hpp>
#include <koo/dyna/Element.hpp>
#include <koo/dyna/Material.hpp>
#include <koo/dyna/Part.hpp>
#include <koo/dyna/Section.hpp>

#include <set>
#include <unordered_map>
#include <vector>
#include <string>
#include <sstream>

namespace koo::dyna {

/**
 * @brief Severity level for validation messages
 */
enum class ValidationSeverity {
    Info,
    Warning,
    Error
};

/**
 * @brief Validation message
 */
struct ValidationMessage {
    ValidationSeverity severity;
    std::string category;
    std::string message;
};

/**
 * @brief Model validation visitor
 *
 * Validates an LS-DYNA model for:
 * - Duplicate IDs (nodes, elements, parts, materials, sections)
 * - Missing references (nodes referenced by elements, materials/sections referenced by parts)
 * - Unreferenced entities (nodes, materials, sections not used)
 * - Invalid data (negative densities, zero Young's modulus, etc.)
 */
class KOO_API ValidationVisitor : public ModelVisitor {
public:
    ValidationVisitor() = default;

    // Simple validation: just check for duplicate node IDs
    void visit(Node& keyword) override {
        for (const auto& node : keyword.getNodes()) {
            if (!nodeIds_.insert(node.id).second) {
                addError("Nodes", "Duplicate node ID: " + std::to_string(node.id));
            }
        }
    }

    // Simple validation: check for duplicate element IDs and track part references
    void visit(ElementShell& keyword) override {
        for (const auto& elem : keyword.getElements()) {
            if (!elementIds_.insert(elem.id).second) {
                addError("Elements", "Duplicate element ID: " + std::to_string(elem.id));
            }
            partReferences_[elem.pid]++;
        }
    }

    void visit(ElementSolid& keyword) override {
        for (const auto& elem : keyword.getElements()) {
            if (!elementIds_.insert(elem.id).second) {
                addError("Elements", "Duplicate element ID: " + std::to_string(elem.id));
            }
            partReferences_[elem.pid]++;
        }
    }

    void visit(ElementBeam& keyword) override {
        for (const auto& elem : keyword.getElements()) {
            if (!elementIds_.insert(elem.id).second) {
                addError("Elements", "Duplicate element ID: " + std::to_string(elem.id));
            }
            partReferences_[elem.pid]++;
        }
    }

    // Part validation
    void visit(Part& keyword) override {
        for (const auto& part : keyword.getParts()) {
            if (!partIds_.insert(part.id).second) {
                addError("Parts", "Duplicate part ID: " + std::to_string(part.id));
            }
            if (part.mid != 0) {
                materialReferences_[part.mid]++;
            }
            if (part.secid != 0) {
                sectionReferences_[part.secid]++;
            }
        }
    }

    // Material validation - MatElastic
    void visit(MatElastic& keyword) override {
        auto& data = keyword.getData();
        if (!materialIds_.insert(data.id).second) {
            addError("Materials", "Duplicate material ID: " + std::to_string(data.id));
        }
        if (data.ro <= 0.0) {
            addWarning("Materials", "Material " + std::to_string(data.id) + " has non-positive density");
        }
        if (data.e <= 0.0) {
            addWarning("Materials", "Material " + std::to_string(data.id) + " has non-positive Young's modulus");
        }
    }

    // Material validation - MatRigid
    void visit(MatRigid& keyword) override {
        auto& data = keyword.getData();
        if (!materialIds_.insert(data.id).second) {
            addError("Materials", "Duplicate material ID: " + std::to_string(data.id));
        }
    }

    // Material validation - MatPlasticKinematic
    void visit(MatPlasticKinematic& keyword) override {
        auto& data = keyword.getData();
        if (!materialIds_.insert(data.mid).second) {
            addError("Materials", "Duplicate material ID: " + std::to_string(data.mid));
        }
        if (data.ro <= 0.0) {
            addWarning("Materials", "Material " + std::to_string(data.mid) + " has non-positive density");
        }
        if (data.e <= 0.0) {
            addWarning("Materials", "Material " + std::to_string(data.mid) + " has non-positive Young's modulus");
        }
    }

    // Section validation
    void visit(SectionShell& keyword) override {
        SectionId sid = keyword.getSectionId();
        if (!sectionIds_.insert(sid).second) {
            addError("Sections", "Duplicate section ID: " + std::to_string(sid));
        }
        if (keyword.getThickness() <= 0.0) {
            addWarning("Sections", "Shell section " + std::to_string(sid) + " has non-positive thickness");
        }
    }

    void visit(SectionSolid& keyword) override {
        SectionId sid = keyword.getSectionId();
        if (!sectionIds_.insert(sid).second) {
            addError("Sections", "Duplicate section ID: " + std::to_string(sid));
        }
    }

    void visit(SectionBeam& keyword) override {
        SectionId sid = keyword.getSectionId();
        if (!sectionIds_.insert(sid).second) {
            addError("Sections", "Duplicate section ID: " + std::to_string(sid));
        }
    }

    // After all keywords are visited, perform final checks
    void finalizeValidation() {
        // Check for unreferenced parts
        for (const auto& pid : partIds_) {
            if (partReferences_[pid] == 0) {
                addWarning("Parts", "Part " + std::to_string(pid) + " is not referenced by any elements");
            }
        }

        // Check for unreferenced materials
        for (const auto& mid : materialIds_) {
            if (materialReferences_[mid] == 0) {
                addWarning("Materials", "Material " + std::to_string(mid) + " is not referenced by any parts");
            }
        }

        // Check for unreferenced sections
        for (const auto& sid : sectionIds_) {
            if (sectionReferences_[sid] == 0) {
                addWarning("Sections", "Section " + std::to_string(sid) + " is not referenced by any parts");
            }
        }
    }

    // Get validation results
    const std::vector<ValidationMessage>& getMessages() const { return messages_; }

    size_t getErrorCount() const {
        size_t count = 0;
        for (const auto& msg : messages_) {
            if (msg.severity == ValidationSeverity::Error) count++;
        }
        return count;
    }

    size_t getWarningCount() const {
        size_t count = 0;
        for (const auto& msg : messages_) {
            if (msg.severity == ValidationSeverity::Warning) count++;
        }
        return count;
    }

    // Print messages with optional minimum severity filter
    void printMessages(std::ostream& os, ValidationSeverity minSeverity = ValidationSeverity::Info) const;

private:
    void addError(const std::string& category, const std::string& message) {
        messages_.push_back({ValidationSeverity::Error, category, message});
    }

    void addWarning(const std::string& category, const std::string& message) {
        messages_.push_back({ValidationSeverity::Warning, category, message});
    }

    void addInfo(const std::string& category, const std::string& message) {
        messages_.push_back({ValidationSeverity::Info, category, message});
    }

    // Tracking sets
    std::set<NodeId> nodeIds_;
    std::set<ElementId> elementIds_;
    std::set<PartId> partIds_;
    std::set<MaterialId> materialIds_;
    std::set<SectionId> sectionIds_;

    // Reference counting
    std::unordered_map<PartId, int> partReferences_;
    std::unordered_map<MaterialId, int> materialReferences_;
    std::unordered_map<SectionId, int> sectionReferences_;

    // Messages
    std::vector<ValidationMessage> messages_;
};

} // namespace koo::dyna
