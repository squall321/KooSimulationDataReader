#pragma once

#include <koo/Export.hpp>
#include <koo/ecad/Types.hpp>
#include <koo/ecad/Feature.hpp>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

namespace koo::ecad {

// Forward declarations
class Component;
class Net;
class Package;

// ============================================================================
// Pin
// ============================================================================

/**
 * @brief Pin definition within a package or component
 *
 * ODB++ PIN record: PIN <name> <type> <x> <y> <f_l> <e_l> <m_l>
 */
struct KOO_API Pin {
    std::string name;           ///< Pin name (1, 2, A1, VCC, etc.)
    std::string netName;        ///< Connected net name
    double x = 0.0, y = 0.0;    ///< Position (relative to component origin)
    PinType type = PinType::Smd;
    int featureLayerIndex = -1; ///< Feature layer index
    int electricalLayerIndex = -1; ///< Electrical layer index
    int mechanicalLayerIndex = -1; ///< Mechanical layer index

    // Extended pin information
    double rotation = 0.0;
    bool mirror = false;
    std::string padstackName;   ///< Associated padstack

    AttributeList attributes;
};

/**
 * @brief Pin reference (component + pin)
 */
struct PinRef {
    std::string refDes;         ///< Component reference designator
    std::string pinName;        ///< Pin name within component

    bool operator==(const PinRef& other) const {
        return refDes == other.refDes && pinName == other.pinName;
    }
};

// ============================================================================
// Package (Footprint)
// ============================================================================

/**
 * @brief Package/Footprint definition
 *
 * ODB++ PKG record: PKG <name> <pitch> <xmin> <ymin> <xmax> <ymax>
 */
class KOO_API Package {
public:
    Package() = default;
    explicit Package(const std::string& name);

    /// Package name
    const std::string& getName() const { return name_; }
    void setName(const std::string& name) { name_ = name; }

    /// Pin pitch
    double getPitch() const { return pitch_; }
    void setPitch(double p) { pitch_ = p; }

    /// Bounding box
    BoundingBox2D getBoundingBox() const { return boundingBox_; }
    void setBoundingBox(const BoundingBox2D& box) { boundingBox_ = box; }

    /// Pins
    const std::vector<Pin>& getPins() const { return pins_; }
    void addPin(const Pin& pin);
    const Pin* getPin(const std::string& name) const;
    size_t getPinCount() const { return pins_.size(); }

    /// Outline contours
    const std::vector<Contour>& getOutlines() const { return outlines_; }
    void addOutline(const Contour& outline);

    /// Attributes
    const AttributeList& getAttributes() const { return attributes_; }
    void setAttribute(const std::string& key, const std::string& value) {
        attributes_[key] = value;
    }

private:
    std::string name_;
    double pitch_ = 0.0;
    BoundingBox2D boundingBox_;
    std::vector<Pin> pins_;
    std::vector<Contour> outlines_;
    AttributeList attributes_;
};

// ============================================================================
// Component
// ============================================================================

/**
 * @brief Component (placed part)
 *
 * ODB++ CMP record: CMP <refdes> <pkg_num> <x> <y> <rot> <mir> <comp_name>
 */
class KOO_API Component {
public:
    Component() = default;
    explicit Component(const std::string& refDes);

    /// Reference designator (U1, R1, C1, etc.)
    const std::string& getRefDes() const { return refDes_; }
    void setRefDes(const std::string& rd) { refDes_ = rd; }

    /// Part number / Value
    const std::string& getPartNumber() const { return partNumber_; }
    void setPartNumber(const std::string& pn) { partNumber_ = pn; }

    /// Package name
    const std::string& getPackageName() const { return packageName_; }
    void setPackageName(const std::string& pkg) { packageName_ = pkg; }
    int getPackageIndex() const { return packageIndex_; }
    void setPackageIndex(int idx) { packageIndex_ = idx; }

    /// Position
    Point2D getPosition() const { return {x_, y_}; }
    void setPosition(double x, double y) { x_ = x; y_ = y; }

    /// Rotation (degrees)
    double getRotation() const { return rotation_; }
    void setRotation(double rot) { rotation_ = rot; }

    /// Mirror
    bool isMirrored() const { return mirror_; }
    void setMirrored(bool m) { mirror_ = m; }

    /// Mounting side
    MountSide getSide() const { return side_; }
    void setSide(MountSide s) { side_ = s; }

    /// Component name (from CMP record)
    const std::string& getComponentName() const { return componentName_; }
    void setComponentName(const std::string& name) { componentName_ = name; }

    /// Pins (component-level pin information)
    const std::vector<Pin>& getPins() const { return pins_; }
    void addPin(const Pin& pin);
    const Pin* getPin(const std::string& name) const;
    size_t getPinCount() const { return pins_.size(); }

    /// BOM properties
    const std::string& getValue() const { return value_; }
    void setValue(const std::string& v) { value_ = v; }

    const std::string& getDescription() const { return description_; }
    void setDescription(const std::string& desc) { description_ = desc; }

    const std::string& getManufacturer() const { return manufacturer_; }
    void setManufacturer(const std::string& mfr) { manufacturer_ = mfr; }

    const std::string& getManufacturerPartNumber() const { return mfrPartNumber_; }
    void setManufacturerPartNumber(const std::string& mpn) { mfrPartNumber_ = mpn; }

    /// Attributes
    const AttributeList& getAttributes() const { return attributes_; }
    void setAttribute(const std::string& key, const std::string& value) {
        attributes_[key] = value;
    }
    std::string getAttribute(const std::string& key) const {
        auto it = attributes_.find(key);
        return (it != attributes_.end()) ? it->second : "";
    }

    /// Toeprint reference (TOP record)
    int getToeprintTop() const { return toeprintTop_; }
    void setToeprintTop(int t) { toeprintTop_ = t; }
    int getToeprintBottom() const { return toeprintBottom_; }
    void setToeprintBottom(int t) { toeprintBottom_ = t; }

private:
    std::string refDes_;
    std::string partNumber_;
    std::string packageName_;
    int packageIndex_ = -1;
    double x_ = 0.0, y_ = 0.0;
    double rotation_ = 0.0;
    bool mirror_ = false;
    MountSide side_ = MountSide::Top;
    std::string componentName_;

    std::vector<Pin> pins_;

    // BOM properties
    std::string value_;
    std::string description_;
    std::string manufacturer_;
    std::string mfrPartNumber_;

    int toeprintTop_ = -1;
    int toeprintBottom_ = -1;

    AttributeList attributes_;
};

// ============================================================================
// Net
// ============================================================================

/**
 * @brief Electrical net (connection)
 *
 * ODB++ NET record: NET <net_name>
 */
class KOO_API Net {
public:
    Net() = default;
    explicit Net(const std::string& name);

    /// Net name
    const std::string& getName() const { return name_; }
    void setName(const std::string& name) { name_ = name; }

    /// Net number (internal)
    int getNetNumber() const { return netNumber_; }
    void setNetNumber(int num) { netNumber_ = num; }

    /// Connected pins
    const std::vector<PinRef>& getPins() const { return pins_; }
    void addPin(const PinRef& pin);
    void addPin(const std::string& refDes, const std::string& pinName);
    size_t getPinCount() const { return pins_.size(); }

    /// Net class
    NetClass getNetClass() const { return netClass_; }
    void setNetClass(NetClass nc) { netClass_ = nc; }

    /// Check net type
    bool isPower() const;
    bool isGround() const;
    bool isSignal() const { return netClass_ == NetClass::Signal; }

    /// Attributes
    const AttributeList& getAttributes() const { return attributes_; }
    void setAttribute(const std::string& key, const std::string& value) {
        attributes_[key] = value;
    }

    /// Subnet information
    struct Subnet {
        enum class Type { Via, Trace, Plane, Toeprint };
        Type type = Type::Trace;
        std::vector<int> featureIds;
    };
    const std::vector<Subnet>& getSubnets() const { return subnets_; }
    void addSubnet(const Subnet& subnet);

private:
    std::string name_;
    int netNumber_ = -1;
    std::vector<PinRef> pins_;
    NetClass netClass_ = NetClass::Signal;
    AttributeList attributes_;
    std::vector<Subnet> subnets_;
};

// ============================================================================
// EdaData - Container for EDA information
// ============================================================================

/**
 * @brief EDA data container
 *
 * Located at: steps/<step_name>/eda/data
 *
 * Contains:
 * - Netlists
 * - Components
 * - Packages
 * - Feature-to-net mappings
 */
class KOO_API EdaData {
public:
    EdaData() = default;

    // ========== Components ==========

    /// Get component by reference designator
    Component* getComponent(const std::string& refDes);
    const Component* getComponent(const std::string& refDes) const;

    /// Add component
    void addComponent(std::unique_ptr<Component> comp);

    /// Get all component reference designators
    std::vector<std::string> getComponentRefDes() const;

    /// Get component count
    size_t getComponentCount() const { return components_.size(); }

    /// Get all components
    const std::unordered_map<std::string, std::unique_ptr<Component>>& getComponents() const {
        return components_;
    }

    // ========== Nets ==========

    /// Get net by name
    Net* getNet(const std::string& name);
    const Net* getNet(const std::string& name) const;

    /// Add net
    void addNet(std::unique_ptr<Net> net);

    /// Get all net names
    std::vector<std::string> getNetNames() const;

    /// Get net count
    size_t getNetCount() const { return nets_.size(); }

    /// Get all nets
    const std::unordered_map<std::string, std::unique_ptr<Net>>& getNets() const {
        return nets_;
    }

    // ========== Packages ==========

    /// Get package by name
    Package* getPackage(const std::string& name);
    const Package* getPackage(const std::string& name) const;

    /// Get package by index
    Package* getPackage(int index);
    const Package* getPackage(int index) const;

    /// Add package
    void addPackage(std::unique_ptr<Package> pkg);

    /// Get all package names
    std::vector<std::string> getPackageNames() const;

    /// Get package count
    size_t getPackageCount() const { return packages_.size(); }

    // ========== Statistics ==========

    /// Total pin count
    size_t getTotalPinCount() const;

    /// Get components on side
    std::vector<const Component*> getComponentsOnSide(MountSide side) const;

    // ========== Layer Names ==========

    /// Layer name mapping (for EDA data references)
    const std::vector<std::string>& getLayerNames() const { return layerNames_; }
    void setLayerNames(const std::vector<std::string>& names) { layerNames_ = names; }
    void addLayerName(const std::string& name);

    // ========== Attributes ==========

    /// Net attribute lookup table
    const std::vector<std::string>& getNetAttributeNames() const { return netAttributeNames_; }
    void setNetAttributeNames(const std::vector<std::string>& names) { netAttributeNames_ = names; }

    /// Net attribute text strings
    const std::vector<std::string>& getNetAttributeStrings() const { return netAttributeStrings_; }
    void setNetAttributeStrings(const std::vector<std::string>& strings) { netAttributeStrings_ = strings; }

    // ========== BOM (Bill of Materials) ==========

    /// Add BOM item
    void addBomItem(const BomItem& item);

    /// Get all BOM items
    const std::vector<BomItem>& getBomItems() const { return bomItems_; }

    /// Get BOM item by reference designator
    const BomItem* getBomItem(const std::string& refDes) const;

    /// Get BOM item count
    size_t getBomItemCount() const { return bomItems_.size(); }

    // ========== Subnets ==========

    /// Add subnet to current net
    void addSubnet(const std::string& netName, const Subnet& subnet);

    /// Get subnets for a net
    std::vector<Subnet> getSubnets(const std::string& netName) const;

    // ========== Feature Groups (FGR) ==========

    /// Feature group record
    struct FeatureGroup {
        std::string type;           ///< e.g., "via_in_pad", "embedded_comp"
        std::vector<FeatureId> features;
        AttributeList attributes;
    };

    /// Add feature group
    void addFeatureGroup(const FeatureGroup& group);

    /// Get feature groups
    const std::vector<FeatureGroup>& getFeatureGroups() const { return featureGroups_; }

    // ========== Feature ID Maps ==========

    /// Feature ID record (FID) - maps feature to net/subnet
    struct FeatureIdRecord {
        FeatureId featureId;
        int netNum = -1;
        int subnetNum = -1;
    };

    /// Add feature ID record
    void addFeatureIdRecord(const FeatureIdRecord& record);

    /// Get net/subnet for a feature
    std::pair<int, int> getFeatureNetSubnet(const FeatureId& fid) const;

private:
    std::unordered_map<std::string, std::unique_ptr<Component>> components_;
    std::unordered_map<std::string, std::unique_ptr<Net>> nets_;
    std::vector<std::unique_ptr<Package>> packages_;
    std::unordered_map<std::string, size_t> packageNameToIndex_;

    std::vector<std::string> layerNames_;
    std::vector<std::string> netAttributeNames_;
    std::vector<std::string> netAttributeStrings_;

    // BOM data
    std::vector<BomItem> bomItems_;
    std::unordered_map<std::string, size_t> bomRefDesIndex_;

    // Subnets (stored per net)
    std::unordered_map<std::string, std::vector<Subnet>> netSubnets_;

    // Feature groups
    std::vector<FeatureGroup> featureGroups_;

    // Feature ID records
    std::vector<FeatureIdRecord> featureIdRecords_;
};

} // namespace koo::ecad
