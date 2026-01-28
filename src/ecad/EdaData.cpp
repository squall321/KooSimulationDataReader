#include <koo/ecad/EdaData.hpp>
#include <algorithm>
#include <cctype>

namespace koo::ecad {

// ============================================================================
// Package
// ============================================================================

Package::Package(const std::string& name) : name_(name) {}

void Package::addPin(const Pin& pin) {
    pins_.push_back(pin);
}

const Pin* Package::getPin(const std::string& name) const {
    for (const auto& pin : pins_) {
        if (pin.name == name) {
            return &pin;
        }
    }
    return nullptr;
}

void Package::addOutline(const Contour& outline) {
    outlines_.push_back(outline);
}

// ============================================================================
// Component
// ============================================================================

Component::Component(const std::string& refDes) : refDes_(refDes) {}

void Component::addPin(const Pin& pin) {
    pins_.push_back(pin);
}

const Pin* Component::getPin(const std::string& name) const {
    for (const auto& pin : pins_) {
        if (pin.name == name) {
            return &pin;
        }
    }
    return nullptr;
}

// ============================================================================
// Net
// ============================================================================

Net::Net(const std::string& name) : name_(name) {
    // Auto-detect net class from name
    std::string upperName = name;
    std::transform(upperName.begin(), upperName.end(), upperName.begin(),
                   [](unsigned char c) { return std::toupper(c); });

    if (upperName.find("VCC") != std::string::npos ||
        upperName.find("VDD") != std::string::npos ||
        upperName.find("+5V") != std::string::npos ||
        upperName.find("+3V") != std::string::npos ||
        upperName.find("+12V") != std::string::npos ||
        upperName.find("PWR") != std::string::npos ||
        upperName.find("POWER") != std::string::npos) {
        netClass_ = NetClass::Power;
    } else if (upperName.find("GND") != std::string::npos ||
               upperName.find("VSS") != std::string::npos ||
               upperName.find("GROUND") != std::string::npos ||
               upperName.find("DGND") != std::string::npos ||
               upperName.find("AGND") != std::string::npos) {
        netClass_ = NetClass::Ground;
    } else if (upperName.find("CLK") != std::string::npos ||
               upperName.find("CLOCK") != std::string::npos) {
        netClass_ = NetClass::Clock;
    }
}

void Net::addPin(const PinRef& pin) {
    pins_.push_back(pin);
}

void Net::addPin(const std::string& refDes, const std::string& pinName) {
    PinRef ref;
    ref.refDes = refDes;
    ref.pinName = pinName;
    pins_.push_back(ref);
}

bool Net::isPower() const {
    return netClass_ == NetClass::Power;
}

bool Net::isGround() const {
    return netClass_ == NetClass::Ground;
}

void Net::addSubnet(const Subnet& subnet) {
    subnets_.push_back(subnet);
}

// ============================================================================
// EdaData
// ============================================================================

Component* EdaData::getComponent(const std::string& refDes) {
    auto it = components_.find(refDes);
    return (it != components_.end()) ? it->second.get() : nullptr;
}

const Component* EdaData::getComponent(const std::string& refDes) const {
    auto it = components_.find(refDes);
    return (it != components_.end()) ? it->second.get() : nullptr;
}

void EdaData::addComponent(std::unique_ptr<Component> comp) {
    if (comp) {
        components_[comp->getRefDes()] = std::move(comp);
    }
}

std::vector<std::string> EdaData::getComponentRefDes() const {
    std::vector<std::string> refDes;
    refDes.reserve(components_.size());
    for (const auto& pair : components_) {
        refDes.push_back(pair.first);
    }
    return refDes;
}

Net* EdaData::getNet(const std::string& name) {
    auto it = nets_.find(name);
    return (it != nets_.end()) ? it->second.get() : nullptr;
}

const Net* EdaData::getNet(const std::string& name) const {
    auto it = nets_.find(name);
    return (it != nets_.end()) ? it->second.get() : nullptr;
}

void EdaData::addNet(std::unique_ptr<Net> net) {
    if (net) {
        nets_[net->getName()] = std::move(net);
    }
}

std::vector<std::string> EdaData::getNetNames() const {
    std::vector<std::string> names;
    names.reserve(nets_.size());
    for (const auto& pair : nets_) {
        names.push_back(pair.first);
    }
    return names;
}

Package* EdaData::getPackage(const std::string& name) {
    auto it = packageNameToIndex_.find(name);
    if (it != packageNameToIndex_.end() && it->second < packages_.size()) {
        return packages_[it->second].get();
    }
    return nullptr;
}

const Package* EdaData::getPackage(const std::string& name) const {
    auto it = packageNameToIndex_.find(name);
    if (it != packageNameToIndex_.end() && it->second < packages_.size()) {
        return packages_[it->second].get();
    }
    return nullptr;
}

Package* EdaData::getPackage(int index) {
    if (index >= 0 && static_cast<size_t>(index) < packages_.size()) {
        return packages_[static_cast<size_t>(index)].get();
    }
    return nullptr;
}

const Package* EdaData::getPackage(int index) const {
    if (index >= 0 && static_cast<size_t>(index) < packages_.size()) {
        return packages_[static_cast<size_t>(index)].get();
    }
    return nullptr;
}

void EdaData::addPackage(std::unique_ptr<Package> pkg) {
    if (pkg) {
        packageNameToIndex_[pkg->getName()] = packages_.size();
        packages_.push_back(std::move(pkg));
    }
}

std::vector<std::string> EdaData::getPackageNames() const {
    std::vector<std::string> names;
    names.reserve(packages_.size());
    for (const auto& pkg : packages_) {
        names.push_back(pkg->getName());
    }
    return names;
}

size_t EdaData::getTotalPinCount() const {
    size_t count = 0;
    for (const auto& pair : components_) {
        count += pair.second->getPinCount();
    }
    return count;
}

std::vector<const Component*> EdaData::getComponentsOnSide(MountSide side) const {
    std::vector<const Component*> result;
    for (const auto& pair : components_) {
        if (pair.second->getSide() == side) {
            result.push_back(pair.second.get());
        }
    }
    return result;
}

void EdaData::addLayerName(const std::string& name) {
    layerNames_.push_back(name);
}

// ============================================================================
// BOM
// ============================================================================

void EdaData::addBomItem(const BomItem& item) {
    bomRefDesIndex_[item.refDes] = bomItems_.size();
    bomItems_.push_back(item);
}

const BomItem* EdaData::getBomItem(const std::string& refDes) const {
    auto it = bomRefDesIndex_.find(refDes);
    if (it != bomRefDesIndex_.end() && it->second < bomItems_.size()) {
        return &bomItems_[it->second];
    }
    return nullptr;
}

// ============================================================================
// Subnets
// ============================================================================

void EdaData::addSubnet(const std::string& netName, const Subnet& subnet) {
    netSubnets_[netName].push_back(subnet);
}

std::vector<Subnet> EdaData::getSubnets(const std::string& netName) const {
    auto it = netSubnets_.find(netName);
    if (it != netSubnets_.end()) {
        return it->second;
    }
    return {};
}

// ============================================================================
// Feature Groups
// ============================================================================

void EdaData::addFeatureGroup(const FeatureGroup& group) {
    featureGroups_.push_back(group);
}

// ============================================================================
// Feature ID Records
// ============================================================================

void EdaData::addFeatureIdRecord(const FeatureIdRecord& record) {
    featureIdRecords_.push_back(record);
}

std::pair<int, int> EdaData::getFeatureNetSubnet(const FeatureId& fid) const {
    for (const auto& record : featureIdRecords_) {
        if (record.featureId.type == fid.type &&
            record.featureId.layerNum == fid.layerNum &&
            record.featureId.featureNum == fid.featureNum) {
            return {record.netNum, record.subnetNum};
        }
    }
    return {-1, -1};
}

} // namespace koo::ecad
