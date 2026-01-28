#include <koo/ecad/Step.hpp>
#include <algorithm>
#include <set>

namespace koo::ecad {

Step::Step(const std::string& name) : name_(name) {}

Layer* Step::getLayer(const std::string& name) {
    auto it = layers_.find(name);
    return (it != layers_.end()) ? it->second.get() : nullptr;
}

const Layer* Step::getLayer(const std::string& name) const {
    auto it = layers_.find(name);
    return (it != layers_.end()) ? it->second.get() : nullptr;
}

void Step::addLayer(std::unique_ptr<Layer> layer) {
    if (layer) {
        layers_[layer->getName()] = std::move(layer);
    }
}

std::vector<std::string> Step::getLayerNames() const {
    std::vector<std::string> names;
    names.reserve(layers_.size());
    for (const auto& pair : layers_) {
        names.push_back(pair.first);
    }
    return names;
}

std::vector<Layer*> Step::getLayersByType(LayerType type) {
    std::vector<Layer*> result;
    for (auto& pair : layers_) {
        if (pair.second->getType() == type) {
            result.push_back(pair.second.get());
        }
    }
    return result;
}

void Step::addProfileContour(const Contour& contour) {
    profile_.push_back(contour);
}

BoundingBox2D Step::getBoundingBox() const {
    BoundingBox2D box;

    // First try profile
    for (const auto& contour : profile_) {
        box.expand(contour.getBoundingBox());
    }

    // If no profile, use layers
    if (!box.isValid()) {
        for (const auto& pair : layers_) {
            box.expand(pair.second->getBoundingBox());
        }
    }

    return box;
}

void Step::addStepRepeat(const StepRepeat& repeat) {
    stepRepeats_.push_back(repeat);
}

size_t Step::getTotalInstanceCount() const {
    size_t count = 0;
    for (const auto& sr : stepRepeats_) {
        count += static_cast<size_t>(sr.nx) * static_cast<size_t>(sr.ny);
    }
    return count > 0 ? count : 1;  // At least 1 (this step itself)
}

std::vector<CopperLayer*> Step::getCopperLayers() {
    std::vector<CopperLayer*> result;
    for (auto& pair : layers_) {
        if (pair.second->getType() == LayerType::Signal ||
            pair.second->getType() == LayerType::PowerGround ||
            pair.second->getType() == LayerType::Mixed) {
            // Try to cast to CopperLayer
            auto* copper = dynamic_cast<CopperLayer*>(pair.second.get());
            if (copper) {
                result.push_back(copper);
            }
        }
    }
    // Sort by layer number
    std::sort(result.begin(), result.end(), [](CopperLayer* a, CopperLayer* b) {
        return a->getLayerNumber() < b->getLayerNumber();
    });
    return result;
}

std::vector<DrillLayer*> Step::getDrillLayers() {
    std::vector<DrillLayer*> result;
    for (auto& pair : layers_) {
        if (pair.second->getType() == LayerType::Drill) {
            auto* drill = dynamic_cast<DrillLayer*>(pair.second.get());
            if (drill) {
                result.push_back(drill);
            }
        }
    }
    return result;
}

std::vector<std::string> Step::getAllNetNames() const {
    std::set<std::string> netNames;

    // From EDA data
    for (const auto& name : edaData_.getNetNames()) {
        netNames.insert(name);
    }

    // From features
    for (const auto& pair : layers_) {
        for (const auto& feature : pair.second->getFeatures()) {
            if (!feature->getNetName().empty()) {
                netNames.insert(feature->getNetName());
            }
        }
    }

    return std::vector<std::string>(netNames.begin(), netNames.end());
}

} // namespace koo::ecad
