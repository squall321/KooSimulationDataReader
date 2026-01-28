#pragma once

#include <koo/Export.hpp>
#include <koo/ecad/Types.hpp>
#include <koo/ecad/Feature.hpp>
#include <memory>
#include <string>
#include <vector>

namespace koo::ecad {

/**
 * @brief Layer definition in ODB++ matrix
 *
 * Contains metadata about a layer as defined in the matrix file.
 */
struct LayerDefinition {
    std::string name;
    LayerType type = LayerType::Unknown;
    LayerContext context = LayerContext::Board;
    Polarity polarity = Polarity::Positive;
    Side side = Side::None;
    int row = 0;                    ///< Row number in matrix
    int startName = 0;              ///< Start drill name (for drill layers)
    int endName = 0;                ///< End drill name (for drill layers)
    double thickness = 0.0;         ///< Layer thickness (for copper)
    std::string oldName;            ///< Original name before renaming
};

/**
 * @brief Layer containing graphic features
 *
 * Located at: steps/<step_name>/layers/<layer_name>/
 * Files:
 *   - features (or features.z) - graphic data
 *   - attrlist - layer attributes
 *   - profile - layer outline (optional)
 *   - components - component data (for component layers)
 *   - tools - drill tools (for drill layers)
 */
class KOO_API Layer {
public:
    Layer() = default;
    explicit Layer(const std::string& name);
    virtual ~Layer() = default;

    /// Layer name
    const std::string& getName() const { return name_; }
    void setName(const std::string& name) { name_ = name; }

    /// Layer type
    LayerType getType() const { return type_; }
    void setType(LayerType type) { type_ = type; }

    /// Layer context
    LayerContext getContext() const { return context_; }
    void setContext(LayerContext ctx) { context_ = ctx; }

    /// Polarity
    Polarity getPolarity() const { return polarity_; }
    void setPolarity(Polarity p) { polarity_ = p; }

    /// Side
    Side getSide() const { return side_; }
    void setSide(Side s) { side_ = s; }

    /// Row number in matrix
    int getRow() const { return row_; }
    void setRow(int r) { row_ = r; }

    // ========== Features ==========

    /// Get all features
    const std::vector<std::unique_ptr<Feature>>& getFeatures() const { return features_; }
    std::vector<std::unique_ptr<Feature>>& getFeatures() { return features_; }

    /// Get feature count
    size_t getFeatureCount() const { return features_.size(); }

    /// Add feature
    void addFeature(std::unique_ptr<Feature> feature);

    /// Remove feature at index
    void removeFeature(size_t index);

    /// Clear all features
    void clearFeatures();

    /// Get feature by index
    Feature* getFeature(size_t index);
    const Feature* getFeature(size_t index) const;

    /// Find features by net name
    std::vector<Feature*> getFeaturesByNet(const std::string& netName);

    /// Find features in area
    std::vector<Feature*> getFeaturesInArea(const BoundingBox2D& area);

    // ========== Bounding Box ==========

    /// Get layer bounding box
    BoundingBox2D getBoundingBox() const;

    // ========== Attributes ==========

    /// Attributes
    const AttributeList& getAttributes() const { return attributes_; }
    void setAttribute(const std::string& key, const std::string& value) {
        attributes_[key] = value;
    }
    std::string getAttribute(const std::string& key) const {
        auto it = attributes_.find(key);
        return (it != attributes_.end()) ? it->second : "";
    }

    // ========== Units ==========

    /// Units (INCH or MM)
    const std::string& getUnits() const { return units_; }
    void setUnits(const std::string& u) { units_ = u; }

    // ========== Profile ==========

    /// Layer profile (outline)
    const std::vector<Contour>& getProfile() const { return profile_; }
    std::vector<Contour>& getProfile() { return profile_; }
    void addProfileContour(const Contour& contour);

    // ========== Symbol Names ==========

    /// Symbol name table (index -> name mapping)
    const std::vector<std::string>& getSymbolNames() const { return symbolNames_; }
    void setSymbolNames(const std::vector<std::string>& names) { symbolNames_ = names; }
    void addSymbolName(const std::string& name);
    std::string getSymbolName(int index) const;
    int getSymbolIndex(const std::string& name) const;

protected:
    std::string name_;
    LayerType type_ = LayerType::Unknown;
    LayerContext context_ = LayerContext::Board;
    Polarity polarity_ = Polarity::Positive;
    Side side_ = Side::None;
    int row_ = 0;
    std::string units_ = "MM";

    std::vector<std::unique_ptr<Feature>> features_;
    AttributeList attributes_;
    std::vector<Contour> profile_;
    std::vector<std::string> symbolNames_;
};

// ============================================================================
// Specialized Layer Types
// ============================================================================

/**
 * @brief Copper layer (signal or plane)
 */
class KOO_API CopperLayer : public Layer {
public:
    CopperLayer() { type_ = LayerType::Signal; }
    explicit CopperLayer(const std::string& name) : Layer(name) {
        type_ = LayerType::Signal;
    }

    /// Layer number (1 = top, 2 = inner1, etc.)
    int getLayerNumber() const { return layerNumber_; }
    void setLayerNumber(int num) { layerNumber_ = num; }

    /// Copper thickness (oz or um)
    double getThickness() const { return thickness_; }
    void setThickness(double t) { thickness_ = t; }

    /// Get traces (line features) on specific net
    std::vector<LineFeature*> getTracesOnNet(const std::string& netName);

    /// Get pads on specific net
    std::vector<PadFeature*> getPadsOnNet(const std::string& netName);

private:
    int layerNumber_ = 1;
    double thickness_ = 0.0;
};

/**
 * @brief Drill layer
 */
class KOO_API DrillLayer : public Layer {
public:
    DrillLayer() { type_ = LayerType::Drill; }
    explicit DrillLayer(const std::string& name) : Layer(name) {
        type_ = LayerType::Drill;
    }

    /// Drill type
    DrillType getDrillType() const { return drillType_; }
    void setDrillType(DrillType t) { drillType_ = t; }

    /// Start layer (1-based)
    int getStartLayer() const { return startLayer_; }
    void setStartLayer(int l) { startLayer_ = l; }

    /// End layer (1-based)
    int getEndLayer() const { return endLayer_; }
    void setEndLayer(int l) { endLayer_ = l; }

    /// Drill tool definition
    struct DrillTool {
        int number = 0;             ///< Tool number
        double diameter = 0.0;      ///< Drill diameter
        DrillType type = DrillType::Plated;
        int drillCount = 0;         ///< Number of hits
    };

    /// Get drill tools
    const std::vector<DrillTool>& getDrillTools() const { return drillTools_; }
    void addDrillTool(const DrillTool& tool);

    /// Get drill size histogram (diameter -> count)
    std::unordered_map<double, int> getDrillSizeHistogram() const;

private:
    DrillType drillType_ = DrillType::Plated;
    int startLayer_ = 1;
    int endLayer_ = 1;
    std::vector<DrillTool> drillTools_;
};

/**
 * @brief Solder mask layer
 */
class KOO_API SolderMaskLayer : public Layer {
public:
    SolderMaskLayer() { type_ = LayerType::SolderMask; }
    explicit SolderMaskLayer(const std::string& name) : Layer(name) {
        type_ = LayerType::SolderMask;
    }
};

/**
 * @brief Silk screen layer
 */
class KOO_API SilkscreenLayer : public Layer {
public:
    SilkscreenLayer() { type_ = LayerType::SilkScreen; }
    explicit SilkscreenLayer(const std::string& name) : Layer(name) {
        type_ = LayerType::SilkScreen;
    }
};

/**
 * @brief Solder paste layer
 */
class KOO_API SolderPasteLayer : public Layer {
public:
    SolderPasteLayer() { type_ = LayerType::SolderPaste; }
    explicit SolderPasteLayer(const std::string& name) : Layer(name) {
        type_ = LayerType::SolderPaste;
    }
};

/**
 * @brief Document layer
 */
class KOO_API DocumentLayer : public Layer {
public:
    DocumentLayer() {
        type_ = LayerType::Document;
        context_ = LayerContext::Document;
    }
    explicit DocumentLayer(const std::string& name) : Layer(name) {
        type_ = LayerType::Document;
        context_ = LayerContext::Document;
    }
};

// ============================================================================
// Layer Matrix
// ============================================================================

/**
 * @brief Layer matrix defining all layers and their order
 *
 * ODB++ matrix file defines:
 * - All steps in the job
 * - All layers and their attributes (type, context, polarity, etc.)
 * - Layer stack order
 */
class KOO_API LayerMatrix {
public:
    LayerMatrix() = default;

    /// Add layer definition
    void addLayer(const LayerDefinition& def);

    /// Get layer definition by name
    const LayerDefinition* getLayerDefinition(const std::string& name) const;

    /// Get all layer definitions
    const std::vector<LayerDefinition>& getLayerDefinitions() const { return layers_; }

    /// Get layer names in order
    std::vector<std::string> getLayerNames() const;

    /// Get layer count
    size_t getLayerCount() const { return layers_.size(); }

    /// Step definition
    struct StepDefinition {
        std::string name;
        int col = 0;
    };

    /// Add step definition
    void addStep(const StepDefinition& step);

    /// Get step definitions
    const std::vector<StepDefinition>& getStepDefinitions() const { return steps_; }

    /// Get step names
    std::vector<std::string> getStepNames() const;

private:
    std::vector<LayerDefinition> layers_;
    std::vector<StepDefinition> steps_;
};

} // namespace koo::ecad
