#pragma once

#include <koo/Export.hpp>
#include <koo/ecad/Types.hpp>
#include <koo/ecad/Layer.hpp>
#include <koo/ecad/EdaData.hpp>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

namespace koo::ecad {

/**
 * @brief Step (design unit) in ODB++
 *
 * A step represents a single design unit such as:
 * - pcb: Single PCB design
 * - panel: Panel containing multiple PCBs
 * - coupon: Test coupon
 * - array: Array of units
 *
 * Located at: steps/<step_name>/
 * Contains:
 * - stephdr: Step header information
 * - profile: Board outline
 * - layers/: Layer data
 * - eda/: EDA data (netlists, components)
 * - attrlist: Step attributes
 */
class KOO_API Step {
public:
    Step() = default;
    explicit Step(const std::string& name);
    ~Step() = default;

    /// Step name
    const std::string& getName() const { return name_; }
    void setName(const std::string& name) { name_ = name; }

    /// Step type
    StepType getType() const { return type_; }
    void setType(StepType type) { type_ = type; }

    // ========== Layers ==========

    /// Get layer by name
    Layer* getLayer(const std::string& name);
    const Layer* getLayer(const std::string& name) const;

    /// Add layer
    void addLayer(std::unique_ptr<Layer> layer);

    /// Get all layer names
    std::vector<std::string> getLayerNames() const;

    /// Get layer count
    size_t getLayerCount() const { return layers_.size(); }

    /// Get all layers
    const std::unordered_map<std::string, std::unique_ptr<Layer>>& getLayers() const {
        return layers_;
    }

    /// Get layers by type
    std::vector<Layer*> getLayersByType(LayerType type);

    // ========== EDA Data ==========

    /// Get EDA data (netlists, components)
    EdaData& getEdaData() { return edaData_; }
    const EdaData& getEdaData() const { return edaData_; }

    /// Check if EDA data is loaded
    bool hasEdaData() const { return hasEdaData_; }
    void setHasEdaData(bool has) { hasEdaData_ = has; }

    // ========== Profile (Board Outline) ==========

    /// Get profile contours
    const std::vector<Contour>& getProfile() const { return profile_; }
    std::vector<Contour>& getProfile() { return profile_; }

    /// Add profile contour
    void addProfileContour(const Contour& contour);

    /// Get board bounding box
    BoundingBox2D getBoundingBox() const;

    // ========== Step Header (stephdr) ==========

    /// Datum point (origin)
    Point2D getDatum() const { return datum_; }
    void setDatum(const Point2D& d) { datum_ = d; }
    void setDatum(double x, double y) { datum_ = {x, y}; }

    /// X/Y datum flag
    char getXDatum() const { return xDatum_; }
    void setXDatum(char d) { xDatum_ = d; }
    char getYDatum() const { return yDatum_; }
    void setYDatum(char d) { yDatum_ = d; }

    /// Affect holes flag
    bool getAffectHoles() const { return affectHoles_; }
    void setAffectHoles(bool a) { affectHoles_ = a; }

    /// Active area
    BoundingBox2D getActiveArea() const { return activeArea_; }
    void setActiveArea(const BoundingBox2D& area) { activeArea_ = area; }

    // ========== Step Repeat (for panels) ==========

    /// Get step repeats (sub-step placements)
    const std::vector<StepRepeat>& getStepRepeats() const { return stepRepeats_; }

    /// Add step repeat
    void addStepRepeat(const StepRepeat& repeat);

    /// Get total instance count
    size_t getTotalInstanceCount() const;

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

    // ========== Zones ==========

    /// Get zones
    const std::vector<Zone>& getZones() const { return zones_; }
    std::vector<Zone>& getZones() { return zones_; }

    /// Add zone
    void addZone(const Zone& zone) { zones_.push_back(zone); }

    // ========== Dimensions ==========

    /// Get dimensions
    const std::vector<Dimension>& getDimensions() const { return dimensions_; }
    std::vector<Dimension>& getDimensions() { return dimensions_; }

    /// Add dimension
    void addDimension(const Dimension& dim) { dimensions_.push_back(dim); }

    // ========== Utility ==========

    /// Get copper layers in stack order
    std::vector<CopperLayer*> getCopperLayers();

    /// Get drill layers
    std::vector<DrillLayer*> getDrillLayers();

    /// Check if this is a panel step
    bool isPanel() const { return type_ == StepType::Panel && !stepRepeats_.empty(); }

    /// Get unique net names across all layers
    std::vector<std::string> getAllNetNames() const;

private:
    std::string name_;
    StepType type_ = StepType::Pcb;

    // Layers
    std::unordered_map<std::string, std::unique_ptr<Layer>> layers_;

    // EDA data
    EdaData edaData_;
    bool hasEdaData_ = false;

    // Profile
    std::vector<Contour> profile_;

    // Step header info
    Point2D datum_{0.0, 0.0};
    char xDatum_ = 'L';  // L=Left, R=Right, C=Center
    char yDatum_ = 'B';  // B=Bottom, T=Top, C=Center
    bool affectHoles_ = true;
    BoundingBox2D activeArea_;

    // Step repeats (for panels)
    std::vector<StepRepeat> stepRepeats_;

    // Attributes
    AttributeList attributes_;

    // Zones
    std::vector<Zone> zones_;

    // Dimensions
    std::vector<Dimension> dimensions_;
};

} // namespace koo::ecad
