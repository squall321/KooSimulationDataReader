#include <koo/ecad/Layer.hpp>
#include <algorithm>

namespace koo::ecad {

// ============================================================================
// Layer
// ============================================================================

Layer::Layer(const std::string& name) : name_(name) {}

void Layer::addFeature(std::unique_ptr<Feature> feature) {
    if (feature) {
        features_.push_back(std::move(feature));
    }
}

void Layer::removeFeature(size_t index) {
    if (index < features_.size()) {
        features_.erase(features_.begin() + static_cast<std::ptrdiff_t>(index));
    }
}

void Layer::clearFeatures() {
    features_.clear();
}

Feature* Layer::getFeature(size_t index) {
    return (index < features_.size()) ? features_[index].get() : nullptr;
}

const Feature* Layer::getFeature(size_t index) const {
    return (index < features_.size()) ? features_[index].get() : nullptr;
}

std::vector<Feature*> Layer::getFeaturesByNet(const std::string& netName) {
    std::vector<Feature*> result;
    for (const auto& f : features_) {
        if (f->getNetName() == netName) {
            result.push_back(f.get());
        }
    }
    return result;
}

std::vector<Feature*> Layer::getFeaturesInArea(const BoundingBox2D& area) {
    std::vector<Feature*> result;
    for (const auto& f : features_) {
        BoundingBox2D box = f->getBoundingBox();
        // Check if bounding boxes overlap
        if (box.min.x <= area.max.x && box.max.x >= area.min.x &&
            box.min.y <= area.max.y && box.max.y >= area.min.y) {
            result.push_back(f.get());
        }
    }
    return result;
}

BoundingBox2D Layer::getBoundingBox() const {
    BoundingBox2D box;
    for (const auto& f : features_) {
        box.expand(f->getBoundingBox());
    }
    return box;
}

void Layer::addProfileContour(const Contour& contour) {
    profile_.push_back(contour);
}

void Layer::addSymbolName(const std::string& name) {
    symbolNames_.push_back(name);
}

std::string Layer::getSymbolName(int index) const {
    if (index >= 0 && static_cast<size_t>(index) < symbolNames_.size()) {
        return symbolNames_[static_cast<size_t>(index)];
    }
    return "";
}

int Layer::getSymbolIndex(const std::string& name) const {
    for (size_t i = 0; i < symbolNames_.size(); ++i) {
        if (symbolNames_[i] == name) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

// ============================================================================
// CopperLayer
// ============================================================================

std::vector<LineFeature*> CopperLayer::getTracesOnNet(const std::string& netName) {
    std::vector<LineFeature*> result;
    for (const auto& f : features_) {
        if (f->getType() == FeatureType::Line && f->getNetName() == netName) {
            result.push_back(static_cast<LineFeature*>(f.get()));
        }
    }
    return result;
}

std::vector<PadFeature*> CopperLayer::getPadsOnNet(const std::string& netName) {
    std::vector<PadFeature*> result;
    for (const auto& f : features_) {
        if (f->getType() == FeatureType::Pad && f->getNetName() == netName) {
            result.push_back(static_cast<PadFeature*>(f.get()));
        }
    }
    return result;
}

// ============================================================================
// DrillLayer
// ============================================================================

void DrillLayer::addDrillTool(const DrillTool& tool) {
    drillTools_.push_back(tool);
}

std::unordered_map<double, int> DrillLayer::getDrillSizeHistogram() const {
    std::unordered_map<double, int> histogram;

    // Count from features
    for (const auto& f : features_) {
        if (f->getType() == FeatureType::Pad) {
            // For drill layers, pad symbol typically indicates drill size
            // This is a simplified implementation - actual size comes from symbol
            histogram[1.0]++;  // Placeholder
        }
    }

    // Also use tool definitions
    for (const auto& tool : drillTools_) {
        histogram[tool.diameter] = tool.drillCount;
    }

    return histogram;
}

// ============================================================================
// LayerMatrix
// ============================================================================

void LayerMatrix::addLayer(const LayerDefinition& def) {
    layers_.push_back(def);
}

const LayerDefinition* LayerMatrix::getLayerDefinition(const std::string& name) const {
    for (const auto& layer : layers_) {
        if (layer.name == name) {
            return &layer;
        }
    }
    return nullptr;
}

std::vector<std::string> LayerMatrix::getLayerNames() const {
    std::vector<std::string> names;
    names.reserve(layers_.size());
    for (const auto& layer : layers_) {
        names.push_back(layer.name);
    }
    return names;
}

void LayerMatrix::addStep(const StepDefinition& step) {
    steps_.push_back(step);
}

std::vector<std::string> LayerMatrix::getStepNames() const {
    std::vector<std::string> names;
    names.reserve(steps_.size());
    for (const auto& step : steps_) {
        names.push_back(step.name);
    }
    return names;
}

} // namespace koo::ecad
