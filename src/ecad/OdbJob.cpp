#include <koo/ecad/OdbJob.hpp>
#include <algorithm>
#include <set>

namespace koo::ecad {

OdbJob::OdbJob(const std::string& name) : name_(name) {}

Step* OdbJob::getStep(const std::string& name) {
    auto it = steps_.find(name);
    return (it != steps_.end()) ? it->second.get() : nullptr;
}

const Step* OdbJob::getStep(const std::string& name) const {
    auto it = steps_.find(name);
    return (it != steps_.end()) ? it->second.get() : nullptr;
}

void OdbJob::addStep(std::unique_ptr<Step> step) {
    if (step) {
        steps_[step->getName()] = std::move(step);
    }
}

Step& OdbJob::createStep(const std::string& name) {
    auto step = std::make_unique<Step>(name);
    Step* ptr = step.get();
    steps_[name] = std::move(step);
    return *ptr;
}

void OdbJob::removeStep(const std::string& name) {
    steps_.erase(name);
}

std::vector<std::string> OdbJob::getStepNames() const {
    std::vector<std::string> names;
    names.reserve(steps_.size());
    for (const auto& pair : steps_) {
        names.push_back(pair.first);
    }
    return names;
}

Step* OdbJob::getPrimaryStep() {
    // Try "pcb" first
    auto* step = getStep("pcb");
    if (step) return step;

    // Try any step starting with "pcb"
    for (auto& pair : steps_) {
        if (pair.first.find("pcb") == 0) {
            return pair.second.get();
        }
    }

    // Return first step
    if (!steps_.empty()) {
        return steps_.begin()->second.get();
    }

    return nullptr;
}

const Step* OdbJob::getPrimaryStep() const {
    return const_cast<OdbJob*>(this)->getPrimaryStep();
}

Symbol* OdbJob::getSymbol(const std::string& name) {
    return symbolLibrary_.getSymbol(name);
}

const Symbol* OdbJob::getSymbol(const std::string& name) const {
    return symbolLibrary_.getSymbol(name);
}

void OdbJob::addSymbol(std::unique_ptr<Symbol> symbol) {
    symbolLibrary_.addSymbol(std::move(symbol));
}

std::vector<std::string> OdbJob::getSymbolNames() const {
    return symbolLibrary_.getSymbolNames();
}

std::vector<std::string> OdbJob::getLayerNames() const {
    return matrix_.getLayerNames();
}

std::vector<std::string> OdbJob::getAllNetNames() const {
    std::set<std::string> allNets;

    for (const auto& pair : steps_) {
        auto stepNets = pair.second->getAllNetNames();
        allNets.insert(stepNets.begin(), stepNets.end());
    }

    return std::vector<std::string>(allNets.begin(), allNets.end());
}

size_t OdbJob::getTotalComponentCount() const {
    size_t count = 0;
    for (const auto& pair : steps_) {
        count += pair.second->getEdaData().getComponentCount();
    }
    return count;
}

size_t OdbJob::getTotalFeatureCount() const {
    size_t count = 0;
    for (const auto& stepPair : steps_) {
        for (const auto& layerPair : stepPair.second->getLayers()) {
            count += layerPair.second->getFeatureCount();
        }
    }
    return count;
}

void OdbJob::clear() {
    name_.clear();
    info_ = JobInfo();
    sourcePath_.clear();
    matrix_ = LayerMatrix();
    steps_.clear();
    symbolLibrary_.clear();
    attributes_.clear();
    stackup_.clear();
    impedanceConstraints_.clear();
    intentionalShorts_.clear();
    drillTools_.clear();
    metadata_ = Metadata();
    variants_.clear();
    embeddedComponents_.clear();
    buildupInfo_ = BuildupInfo();
    vendorParts_.clear();
    customerInfo_ = CustomerInfo();
}

OdbJob::ValidationResult OdbJob::validate() const {
    ValidationResult result;

    // Check for job name
    if (name_.empty()) {
        result.warnings.push_back("Job name is empty");
    }

    // Check for at least one step
    if (steps_.empty()) {
        result.warnings.push_back("No steps defined in job");
    }

    // Check matrix consistency
    auto matrixSteps = matrix_.getStepNames();
    for (const auto& stepName : matrixSteps) {
        if (steps_.find(stepName) == steps_.end()) {
            result.warnings.push_back("Step '" + stepName + "' in matrix but not loaded");
        }
    }

    // Check each step
    for (const auto& pair : steps_) {
        const Step* step = pair.second.get();

        // Check for layers
        if (step->getLayerCount() == 0) {
            result.warnings.push_back("Step '" + step->getName() + "' has no layers");
        }

        // Check for profile
        if (step->getProfile().empty()) {
            result.warnings.push_back("Step '" + step->getName() + "' has no profile");
        }

        // Validate step repeats for panels
        if (step->getType() == StepType::Panel) {
            for (const auto& sr : step->getStepRepeats()) {
                if (steps_.find(sr.stepName) == steps_.end()) {
                    result.errors.push_back("Step repeat references non-existent step: " + sr.stepName);
                    result.valid = false;
                }
            }
        }
    }

    return result;
}

} // namespace koo::ecad
