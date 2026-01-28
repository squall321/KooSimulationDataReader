#pragma once

#include <koo/Export.hpp>
#include <koo/ecad/Types.hpp>
#include <koo/ecad/Step.hpp>
#include <koo/ecad/Layer.hpp>
#include <koo/ecad/Symbol.hpp>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <filesystem>

namespace koo::ecad {

/**
 * @brief ODB++ Job - Top-level container
 *
 * Represents a complete ODB++ job/database containing:
 * - Matrix: Layer definitions and step list
 * - Steps: Design units with layer data
 * - Symbols: User-defined symbol library
 * - Fonts: Font definitions
 * - Misc: Global attributes and metadata
 *
 * Directory structure:
 * odb_job/
 * ├── matrix/matrix           - Layer matrix
 * ├── steps/<step_name>/      - Step data
 * ├── symbols/<symbol_name>/  - User symbols
 * ├── fonts/                  - Fonts
 * ├── misc/                   - Metadata
 * └── input/                  - Original input files (optional)
 */
class KOO_API OdbJob {
public:
    OdbJob() = default;
    explicit OdbJob(const std::string& name);
    ~OdbJob() = default;

    // Prevent copying (large data structure)
    OdbJob(const OdbJob&) = delete;
    OdbJob& operator=(const OdbJob&) = delete;

    // Allow moving
    OdbJob(OdbJob&&) = default;
    OdbJob& operator=(OdbJob&&) = default;

    // ========== Job Info ==========

    /// Job name
    const std::string& getName() const { return name_; }
    void setName(const std::string& name) { name_ = name; }

    /// Job information (metadata)
    const JobInfo& getInfo() const { return info_; }
    JobInfo& getInfo() { return info_; }
    void setInfo(const JobInfo& info) { info_ = info; }

    /// Source path (where loaded from)
    const std::filesystem::path& getSourcePath() const { return sourcePath_; }
    void setSourcePath(const std::filesystem::path& path) { sourcePath_ = path; }

    // ========== Matrix ==========

    /// Get layer matrix
    const LayerMatrix& getMatrix() const { return matrix_; }
    LayerMatrix& getMatrix() { return matrix_; }

    // ========== Steps ==========

    /// Get step by name
    Step* getStep(const std::string& name);
    const Step* getStep(const std::string& name) const;

    /// Add step
    void addStep(std::unique_ptr<Step> step);

    /// Create and add new step
    Step& createStep(const std::string& name);

    /// Remove step
    void removeStep(const std::string& name);

    /// Get all step names
    std::vector<std::string> getStepNames() const;

    /// Get step count
    size_t getStepCount() const { return steps_.size(); }

    /// Get all steps
    const std::unordered_map<std::string, std::unique_ptr<Step>>& getSteps() const {
        return steps_;
    }

    /// Get primary step (usually "pcb" or first step)
    Step* getPrimaryStep();
    const Step* getPrimaryStep() const;

    // ========== Symbols ==========

    /// Get symbol library
    SymbolLibrary& getSymbolLibrary() { return symbolLibrary_; }
    const SymbolLibrary& getSymbolLibrary() const { return symbolLibrary_; }

    /// Convenience: get symbol by name
    Symbol* getSymbol(const std::string& name);
    const Symbol* getSymbol(const std::string& name) const;

    /// Add user-defined symbol
    void addSymbol(std::unique_ptr<Symbol> symbol);

    /// Get symbol names
    std::vector<std::string> getSymbolNames() const;

    // ========== Global Attributes ==========

    /// Global attributes (from misc/attrlist)
    const AttributeList& getAttributes() const { return attributes_; }
    void setAttribute(const std::string& key, const std::string& value) {
        attributes_[key] = value;
    }
    std::string getAttribute(const std::string& key) const {
        auto it = attributes_.find(key);
        return (it != attributes_.end()) ? it->second : "";
    }

    // ========== Utility ==========

    /// Get all layer names (from matrix)
    std::vector<std::string> getLayerNames() const;

    /// Get all net names across all steps
    std::vector<std::string> getAllNetNames() const;

    /// Get total component count
    size_t getTotalComponentCount() const;

    /// Get total feature count
    size_t getTotalFeatureCount() const;

    /// Clear all data
    void clear();

    // ========== Stackup ==========

    /// Get stackup layers
    const std::vector<StackupLayer>& getStackup() const { return stackup_; }
    std::vector<StackupLayer>& getStackup() { return stackup_; }

    /// Add stackup layer
    void addStackupLayer(const StackupLayer& layer) { stackup_.push_back(layer); }

    /// Get total board thickness
    double getTotalThickness() const {
        double thickness = 0.0;
        for (const auto& layer : stackup_) {
            thickness += layer.thickness;
        }
        return thickness;
    }

    // ========== Impedance ==========

    /// Get impedance constraints
    const std::vector<ImpedanceConstraint>& getImpedanceConstraints() const { return impedanceConstraints_; }
    std::vector<ImpedanceConstraint>& getImpedanceConstraints() { return impedanceConstraints_; }

    /// Add impedance constraint
    void addImpedanceConstraint(const ImpedanceConstraint& constraint) {
        impedanceConstraints_.push_back(constraint);
    }

    // ========== Intentional Shorts ==========

    /// Get intentional shorts
    const std::vector<IntentionalShort>& getIntentionalShorts() const { return intentionalShorts_; }

    /// Add intentional short
    void addIntentionalShort(const IntentionalShort& shortDef) {
        intentionalShorts_.push_back(shortDef);
    }

    // ========== Drill Tools ==========

    /// Get drill tools
    const std::vector<DrillTool>& getDrillTools() const { return drillTools_; }
    std::vector<DrillTool>& getDrillTools() { return drillTools_; }

    /// Add drill tool
    void addDrillTool(const DrillTool& tool) { drillTools_.push_back(tool); }

    // ========== Metadata ==========

    /// Get metadata
    const Metadata& getMetadata() const { return metadata_; }
    Metadata& getMetadata() { return metadata_; }

    /// Set metadata
    void setMetadata(const Metadata& meta) { metadata_ = meta; }

    // ========== Component Variants ==========

    /// Get component variants
    const std::vector<ComponentVariant>& getVariants() const { return variants_; }
    std::vector<ComponentVariant>& getVariants() { return variants_; }

    /// Add component variant
    void addVariant(const ComponentVariant& variant) { variants_.push_back(variant); }

    /// Get variant by name
    const ComponentVariant* getVariant(const std::string& name) const {
        for (const auto& v : variants_) {
            if (v.name == name) return &v;
        }
        return nullptr;
    }

    /// Get variant count
    size_t getVariantCount() const { return variants_.size(); }

    // ========== Embedded Components ==========

    /// Get embedded components
    const std::vector<EmbeddedComponent>& getEmbeddedComponents() const { return embeddedComponents_; }
    std::vector<EmbeddedComponent>& getEmbeddedComponents() { return embeddedComponents_; }

    /// Add embedded component
    void addEmbeddedComponent(const EmbeddedComponent& comp) { embeddedComponents_.push_back(comp); }

    /// Get embedded components by type
    std::vector<const EmbeddedComponent*> getEmbeddedComponentsByType(EmbeddedComponentType type) const {
        std::vector<const EmbeddedComponent*> result;
        for (const auto& comp : embeddedComponents_) {
            if (comp.type == type) result.push_back(&comp);
        }
        return result;
    }

    /// Get embedded component count
    size_t getEmbeddedComponentCount() const { return embeddedComponents_.size(); }

    // ========== Build-up Information ==========

    /// Get build-up info
    const BuildupInfo& getBuildupInfo() const { return buildupInfo_; }
    BuildupInfo& getBuildupInfo() { return buildupInfo_; }

    /// Set build-up info
    void setBuildupInfo(const BuildupInfo& info) { buildupInfo_ = info; }

    // ========== VPL (Vendor Part List) ==========

    /// Get vendor parts
    const std::vector<VendorPart>& getVendorParts() const { return vendorParts_; }
    std::vector<VendorPart>& getVendorParts() { return vendorParts_; }

    /// Add vendor part
    void addVendorPart(const VendorPart& part) { vendorParts_.push_back(part); }

    /// Get vendor part count
    size_t getVendorPartCount() const { return vendorParts_.size(); }

    // ========== Customer Information ==========

    /// Get customer info
    const CustomerInfo& getCustomerInfo() const { return customerInfo_; }
    CustomerInfo& getCustomerInfo() { return customerInfo_; }

    /// Set customer info
    void setCustomerInfo(const CustomerInfo& info) { customerInfo_ = info; }

    // ========== Validation ==========

    /// Validate job structure
    struct ValidationResult {
        bool valid = true;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
    };

    ValidationResult validate() const;

private:
    std::string name_;
    JobInfo info_;
    std::filesystem::path sourcePath_;

    LayerMatrix matrix_;
    std::unordered_map<std::string, std::unique_ptr<Step>> steps_;
    SymbolLibrary symbolLibrary_;
    AttributeList attributes_;

    std::vector<StackupLayer> stackup_;
    std::vector<ImpedanceConstraint> impedanceConstraints_;
    std::vector<IntentionalShort> intentionalShorts_;
    std::vector<DrillTool> drillTools_;
    Metadata metadata_;
    std::vector<ComponentVariant> variants_;
    std::vector<EmbeddedComponent> embeddedComponents_;
    BuildupInfo buildupInfo_;
    std::vector<VendorPart> vendorParts_;
    CustomerInfo customerInfo_;
};

} // namespace koo::ecad
