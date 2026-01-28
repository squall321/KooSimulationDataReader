#pragma once

#include <koo/Export.hpp>
#include <koo/dyna/Keyword.hpp>
#include <koo/util/Types.hpp>
#include <array>
#include <vector>
#include <unordered_map>

namespace koo::dyna {

/**
 * @brief Element type enumeration
 */
enum class ElementType {
    Unknown,
    Shell,      // 3 or 4 node shell
    Solid,      // 4, 6, or 8 node solid
    Beam,       // 2 node beam
    Discrete,   // Discrete element
    Seatbelt,   // Seatbelt element
    Mass,       // Mass element
    Inertia     // Inertia element
};

/**
 * @brief Base element data
 */
struct KOO_API ElementData {
    ElementId id = 0;
    PartId pid = 0;
    std::vector<NodeId> nodeIds;
    ElementType type = ElementType::Unknown;

    ElementData() = default;
    ElementData(ElementId id_, PartId pid_, std::initializer_list<NodeId> nodes)
        : id(id_), pid(pid_), nodeIds(nodes) {}
};

/**
 * @brief Shell element data
 */
struct KOO_API ShellElementData : public ElementData {
    double thickness = 0.0;  // Optional thickness override
    double beta = 0.0;       // Material angle

    ShellElementData() { type = ElementType::Shell; }
    ShellElementData(ElementId id_, PartId pid_, NodeId n1, NodeId n2, NodeId n3, NodeId n4 = 0);
};

/**
 * @brief Solid element data
 */
struct KOO_API SolidElementData : public ElementData {
    SolidElementData() { type = ElementType::Solid; }
    SolidElementData(ElementId id_, PartId pid_,
                     NodeId n1, NodeId n2, NodeId n3, NodeId n4,
                     NodeId n5, NodeId n6, NodeId n7, NodeId n8);
};

/**
 * @brief Beam element data
 */
struct KOO_API BeamElementData : public ElementData {
    NodeId n3 = 0;        // Third node (orientation)
    int rt1 = 0;          // Release condition at node 1
    int rr1 = 0;          // Release condition at node 1 (rotation)
    int rt2 = 0;          // Release condition at node 2
    int rr2 = 0;          // Release condition at node 2 (rotation)
    int local = 2;        // Coordinate system

    BeamElementData() { type = ElementType::Beam; }
    BeamElementData(ElementId id_, PartId pid_, NodeId n1, NodeId n2, NodeId n3_ = 0)
        : n3(n3_) {
        type = ElementType::Beam;
        id = id_;
        pid = pid_;
        nodeIds = {n1, n2};
    }
};

/**
 * @brief Discrete element data
 */
struct KOO_API DiscreteElementData : public ElementData {
    int vid = 0;          // Orientation vector ID
    double s = 1.0;       // Scale factor for force
    int pf = 0;           // Print flag
    double offset = 0.0;  // Optional offset

    DiscreteElementData() { type = ElementType::Discrete; }
    DiscreteElementData(ElementId id_, PartId pid_, NodeId n1, NodeId n2 = 0) {
        type = ElementType::Discrete;
        id = id_;
        pid = pid_;
        nodeIds = n2 != 0 ? std::vector<NodeId>{n1, n2} : std::vector<NodeId>{n1};
    }
};

/**
 * @brief Seatbelt element data
 */
struct KOO_API SeatbeltElementData : public ElementData {
    int sbrid = 0;        // Seatbelt retractor ID
    double length = 0.0;  // Element length
    double lmin = 0.0;    // Minimum element length

    SeatbeltElementData() { type = ElementType::Seatbelt; }
    SeatbeltElementData(ElementId id_, PartId pid_, NodeId n1, NodeId n2) {
        type = ElementType::Seatbelt;
        id = id_;
        pid = pid_;
        nodeIds = {n1, n2};
    }
};

/**
 * @brief Mass element data
 */
struct KOO_API MassElementData : public ElementData {
    double mass = 0.0;    // Lumped mass value

    MassElementData() { type = ElementType::Mass; }
    MassElementData(ElementId id_, NodeId nid, double mass_)
        : mass(mass_) {
        type = ElementType::Mass;
        id = id_;
        pid = 0;
        nodeIds = {nid};
    }
};

/**
 * @brief Base class for element collections
 */
class KOO_API ElementBase : public Keyword {
public:
    virtual ElementType getElementType() const = 0;
    virtual size_t getElementCount() const = 0;
    virtual void clear() = 0;
};

/**
 * @brief *ELEMENT_SHELL keyword
 */
class KOO_API ElementShell : public CloneableKeyword<ElementShell, ElementBase> {
public:
    ElementShell() = default;

    std::string getKeywordName() const override { return "*ELEMENT_SHELL"; }
    ElementType getElementType() const override { return ElementType::Shell; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    // Element access
    void addElement(const ShellElementData& elem);
    void addElement(ElementId id, PartId pid, NodeId n1, NodeId n2, NodeId n3, NodeId n4 = 0);

    bool hasElement(ElementId id) const;
    ShellElementData* getElement(ElementId id);
    const ShellElementData* getElement(ElementId id) const;

    void removeElement(ElementId id);
    void clear() override;

    // Iteration
    const std::vector<ShellElementData>& getElements() const { return elements_; }
    std::vector<ShellElementData>& getElements() { return elements_; }
    size_t getElementCount() const override { return elements_.size(); }

private:
    void rebuildIndex();

    std::vector<ShellElementData> elements_;
    std::unordered_map<ElementId, size_t> idIndex_;
};

/**
 * @brief *ELEMENT_SOLID keyword
 */
class KOO_API ElementSolid : public CloneableKeyword<ElementSolid, ElementBase> {
public:
    ElementSolid() = default;

    std::string getKeywordName() const override { return "*ELEMENT_SOLID"; }
    ElementType getElementType() const override { return ElementType::Solid; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    // Element access
    void addElement(const SolidElementData& elem);
    void addElement(ElementId id, PartId pid,
                    NodeId n1, NodeId n2, NodeId n3, NodeId n4,
                    NodeId n5, NodeId n6, NodeId n7, NodeId n8);

    bool hasElement(ElementId id) const;
    SolidElementData* getElement(ElementId id);
    const SolidElementData* getElement(ElementId id) const;

    void removeElement(ElementId id);
    void clear() override;

    // Iteration
    const std::vector<SolidElementData>& getElements() const { return elements_; }
    std::vector<SolidElementData>& getElements() { return elements_; }
    size_t getElementCount() const override { return elements_.size(); }

private:
    void rebuildIndex();

    std::vector<SolidElementData> elements_;
    std::unordered_map<ElementId, size_t> idIndex_;
};

/**
 * @brief *ELEMENT_BEAM keyword
 */
class KOO_API ElementBeam : public CloneableKeyword<ElementBeam, ElementBase> {
public:
    ElementBeam() = default;

    std::string getKeywordName() const override { return "*ELEMENT_BEAM"; }
    ElementType getElementType() const override { return ElementType::Beam; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    void addElement(const BeamElementData& elem);
    void clear() override { elements_.clear(); idIndex_.clear(); }

    const std::vector<BeamElementData>& getElements() const { return elements_; }
    std::vector<BeamElementData>& getElements() { return elements_; }
    size_t getElementCount() const override { return elements_.size(); }

private:
    void rebuildIndex();
    std::vector<BeamElementData> elements_;
    std::unordered_map<ElementId, size_t> idIndex_;
};

/**
 * @brief *ELEMENT_DISCRETE keyword
 */
class KOO_API ElementDiscrete : public CloneableKeyword<ElementDiscrete, ElementBase> {
public:
    ElementDiscrete() = default;

    std::string getKeywordName() const override { return "*ELEMENT_DISCRETE"; }
    ElementType getElementType() const override { return ElementType::Discrete; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    void addElement(const DiscreteElementData& elem);
    void clear() override { elements_.clear(); idIndex_.clear(); }

    const std::vector<DiscreteElementData>& getElements() const { return elements_; }
    std::vector<DiscreteElementData>& getElements() { return elements_; }
    size_t getElementCount() const override { return elements_.size(); }

private:
    void rebuildIndex();
    std::vector<DiscreteElementData> elements_;
    std::unordered_map<ElementId, size_t> idIndex_;
};

/**
 * @brief *ELEMENT_SEATBELT keyword
 */
class KOO_API ElementSeatbelt : public CloneableKeyword<ElementSeatbelt, ElementBase> {
public:
    ElementSeatbelt() = default;

    std::string getKeywordName() const override { return "*ELEMENT_SEATBELT"; }
    ElementType getElementType() const override { return ElementType::Seatbelt; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    void addElement(const SeatbeltElementData& elem);
    void clear() override { elements_.clear(); idIndex_.clear(); }

    const std::vector<SeatbeltElementData>& getElements() const { return elements_; }
    std::vector<SeatbeltElementData>& getElements() { return elements_; }
    size_t getElementCount() const override { return elements_.size(); }

private:
    void rebuildIndex();
    std::vector<SeatbeltElementData> elements_;
    std::unordered_map<ElementId, size_t> idIndex_;
};

/**
 * @brief *ELEMENT_MASS keyword
 */
class KOO_API ElementMass : public CloneableKeyword<ElementMass, ElementBase> {
public:
    ElementMass() = default;

    std::string getKeywordName() const override { return "*ELEMENT_MASS"; }
    ElementType getElementType() const override { return ElementType::Mass; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    void addElement(const MassElementData& elem);
    void clear() override { elements_.clear(); idIndex_.clear(); }

    const std::vector<MassElementData>& getElements() const { return elements_; }
    std::vector<MassElementData>& getElements() { return elements_; }
    size_t getElementCount() const override { return elements_.size(); }

private:
    void rebuildIndex();
    std::vector<MassElementData> elements_;
    std::unordered_map<ElementId, size_t> idIndex_;
};

/**
 * @brief Inertia element data
 */
struct KOO_API InertiaElementData : public ElementData {
    double ixx = 0.0;     // Moment of inertia about X
    double ixy = 0.0;     // Product of inertia XY
    double ixz = 0.0;     // Product of inertia XZ
    double iyy = 0.0;     // Moment of inertia about Y
    double iyz = 0.0;     // Product of inertia YZ
    double izz = 0.0;     // Moment of inertia about Z
    double mass = 0.0;    // Mass

    InertiaElementData() { type = ElementType::Inertia; }
};

/**
 * @brief *ELEMENT_INERTIA keyword
 */
class KOO_API ElementInertia : public CloneableKeyword<ElementInertia, ElementBase> {
public:
    ElementInertia() = default;

    std::string getKeywordName() const override { return "*ELEMENT_INERTIA"; }
    ElementType getElementType() const override { return ElementType::Inertia; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    void addElement(const InertiaElementData& elem);
    void clear() override { elements_.clear(); idIndex_.clear(); }

    const std::vector<InertiaElementData>& getElements() const { return elements_; }
    std::vector<InertiaElementData>& getElements() { return elements_; }
    size_t getElementCount() const override { return elements_.size(); }

private:
    void rebuildIndex();
    std::vector<InertiaElementData> elements_;
    std::unordered_map<ElementId, size_t> idIndex_;
};

/**
 * @brief Thick shell (TSHELL) element data
 */
struct KOO_API TshellElementData : public ElementData {
    double thick1 = 0.0;  // Thickness at node 1
    double thick2 = 0.0;  // Thickness at node 2
    double thick3 = 0.0;  // Thickness at node 3
    double thick4 = 0.0;  // Thickness at node 4

    TshellElementData() { type = ElementType::Shell; }
};

/**
 * @brief *ELEMENT_TSHELL keyword
 */
class KOO_API ElementTshell : public CloneableKeyword<ElementTshell, ElementBase> {
public:
    ElementTshell() = default;

    std::string getKeywordName() const override { return "*ELEMENT_TSHELL"; }
    ElementType getElementType() const override { return ElementType::Shell; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    void addElement(const TshellElementData& elem);
    void clear() override { elements_.clear(); idIndex_.clear(); }

    const std::vector<TshellElementData>& getElements() const { return elements_; }
    std::vector<TshellElementData>& getElements() { return elements_; }
    size_t getElementCount() const override { return elements_.size(); }

private:
    void rebuildIndex();
    std::vector<TshellElementData> elements_;
    std::unordered_map<ElementId, size_t> idIndex_;
};

/**
 * @brief Shell thickness data
 */
struct KOO_API ShellThicknessData {
    ElementId eid = 0;    // Element ID
    double thick1 = 0.0;  // Thickness at node 1
    double thick2 = 0.0;  // Thickness at node 2
    double thick3 = 0.0;  // Thickness at node 3
    double thick4 = 0.0;  // Thickness at node 4
};

/**
 * @brief *ELEMENT_SHELL_THICKNESS keyword
 */
class KOO_API ElementShellThickness : public CloneableKeyword<ElementShellThickness, Keyword> {
public:
    ElementShellThickness() = default;

    std::string getKeywordName() const override { return "*ELEMENT_SHELL_THICKNESS"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    std::vector<ShellThicknessData>& getThicknessData() { return data_; }
    const std::vector<ShellThicknessData>& getThicknessData() const { return data_; }

private:
    std::vector<ShellThicknessData> data_;
};

/**
 * @brief Beam orientation data
 */
struct KOO_API BeamOrientationData {
    ElementId eid = 0;    // Element ID
    double ux = 0.0;      // Local x-axis x component
    double uy = 0.0;      // Local x-axis y component
    double uz = 0.0;      // Local x-axis z component
    double vx = 0.0;      // Local y-axis x component
    double vy = 0.0;      // Local y-axis y component
    double vz = 0.0;      // Local y-axis z component
};

/**
 * @brief *ELEMENT_BEAM_ORIENTATION keyword
 */
class KOO_API ElementBeamOrientation : public CloneableKeyword<ElementBeamOrientation, Keyword> {
public:
    ElementBeamOrientation() = default;

    std::string getKeywordName() const override { return "*ELEMENT_BEAM_ORIENTATION"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    std::vector<BeamOrientationData>& getOrientationData() { return data_; }
    const std::vector<BeamOrientationData>& getOrientationData() const { return data_; }

private:
    std::vector<BeamOrientationData> data_;
};

/**
 * @brief *ELEMENT_MASS_PART_SET keyword - assigns mass to part set
 */
class KOO_API ElementMassPartSet : public CloneableKeyword<ElementMassPartSet, Keyword> {
public:
    struct Data {
        int psid = 0;           // Part set ID
        double mass = 0.0;      // Total mass to distribute
    };

    ElementMassPartSet() = default;

    std::string getKeywordName() const override { return "*ELEMENT_MASS_PART_SET"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    std::vector<Data>& getData() { return data_; }
    const std::vector<Data>& getData() const { return data_; }

private:
    std::vector<Data> data_;
};

/**
 * @brief Seatbelt accelerometer element data
 */
struct KOO_API SeatbeltAccelerometerData {
    int64_t id = 0;           // Element ID
    int64_t nid = 0;          // Node ID for accelerometer
    int sbacid = 0;           // Seatbelt accelerometer ID
};

/**
 * @brief *ELEMENT_SEATBELT_ACCELEROMETER keyword
 */
class KOO_API ElementSeatbeltAccelerometer : public CloneableKeyword<ElementSeatbeltAccelerometer, Keyword> {
public:
    ElementSeatbeltAccelerometer() = default;

    std::string getKeywordName() const override { return "*ELEMENT_SEATBELT_ACCELEROMETER"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    std::vector<SeatbeltAccelerometerData>& getData() { return data_; }
    const std::vector<SeatbeltAccelerometerData>& getData() const { return data_; }

private:
    std::vector<SeatbeltAccelerometerData> data_;
};

/**
 * @brief Seatbelt pretensioner element data
 */
struct KOO_API SeatbeltPretensionerData {
    int64_t id = 0;           // Element ID
    int64_t sbprid = 0;       // Seatbelt pretensioner ID
    int64_t sbsid = 0;        // Seatbelt sensor ID
    double time = 0.0;        // Time delay
    double pull = 0.0;        // Pull-in length
    double llcid = 0;         // Load curve ID for locking
    double ulcid = 0;         // Load curve ID for unlocking
};

/**
 * @brief *ELEMENT_SEATBELT_PRETENSIONER keyword
 */
class KOO_API ElementSeatbeltPretensioner : public CloneableKeyword<ElementSeatbeltPretensioner, Keyword> {
public:
    ElementSeatbeltPretensioner() = default;

    std::string getKeywordName() const override { return "*ELEMENT_SEATBELT_PRETENSIONER"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    std::vector<SeatbeltPretensionerData>& getData() { return data_; }
    const std::vector<SeatbeltPretensionerData>& getData() const { return data_; }

private:
    std::vector<SeatbeltPretensionerData> data_;
};

/**
 * @brief Seatbelt retractor element data
 */
struct KOO_API SeatbeltRetractorData {
    int64_t id = 0;           // Element ID
    int64_t sbrid = 0;        // Seatbelt retractor ID
    int64_t nid = 0;          // Node ID
    int64_t sbsid = 0;        // Seatbelt sensor ID
    double tdel = 0.0;        // Time delay
    double pull = 0.0;        // Pull-out length
    int llcid = 0;            // Load curve ID
    double lmin = 0.0;        // Minimum belt length
};

/**
 * @brief *ELEMENT_SEATBELT_RETRACTOR keyword
 */
class KOO_API ElementSeatbeltRetractor : public CloneableKeyword<ElementSeatbeltRetractor, Keyword> {
public:
    ElementSeatbeltRetractor() = default;

    std::string getKeywordName() const override { return "*ELEMENT_SEATBELT_RETRACTOR"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    std::vector<SeatbeltRetractorData>& getData() { return data_; }
    const std::vector<SeatbeltRetractorData>& getData() const { return data_; }

private:
    std::vector<SeatbeltRetractorData> data_;
};

/**
 * @brief Seatbelt sensor element data
 */
struct KOO_API SeatbeltSensorData {
    int64_t id = 0;           // Element ID
    int64_t sbsid = 0;        // Seatbelt sensor ID
    int sbt = 0;              // Sensor type
    int64_t sbsid1 = 0;       // First sensor ID (for type 2)
    int64_t sbsid2 = 0;       // Second sensor ID (for type 2)
    double accel = 0.0;       // Acceleration threshold
    double tdel = 0.0;        // Time delay
};

/**
 * @brief *ELEMENT_SEATBELT_SENSOR keyword
 */
class KOO_API ElementSeatbeltSensor : public CloneableKeyword<ElementSeatbeltSensor, Keyword> {
public:
    ElementSeatbeltSensor() = default;

    std::string getKeywordName() const override { return "*ELEMENT_SEATBELT_SENSOR"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    std::vector<SeatbeltSensorData>& getData() { return data_; }
    const std::vector<SeatbeltSensorData>& getData() const { return data_; }

private:
    std::vector<SeatbeltSensorData> data_;
};

/**
 * @brief Plotel element data for visualization
 */
struct KOO_API PlotelElementData {
    ElementId id = 0;     // Element ID
    NodeId n1 = 0;        // First node
    NodeId n2 = 0;        // Second node
};

/**
 * @brief *ELEMENT_PLOTEL keyword - plot element for visualization
 */
class KOO_API ElementPlotel : public CloneableKeyword<ElementPlotel, Keyword> {
public:
    ElementPlotel() = default;

    std::string getKeywordName() const override { return "*ELEMENT_PLOTEL"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    std::vector<PlotelElementData>& getData() { return data_; }
    const std::vector<PlotelElementData>& getData() const { return data_; }

private:
    std::vector<PlotelElementData> data_;
};

/**
 * @brief Bearing element data
 */
struct KOO_API BearingElementData {
    ElementId id = 0;     // Element ID
    PartId pid = 0;       // Part ID
    NodeId n1 = 0;        // First node (inner ring)
    NodeId n2 = 0;        // Second node (outer ring)
    int vid = 0;          // Orientation vector ID
};

/**
 * @brief *ELEMENT_BEARING keyword - bearing element
 */
class KOO_API ElementBearing : public CloneableKeyword<ElementBearing, Keyword> {
public:
    ElementBearing() = default;

    std::string getKeywordName() const override { return "*ELEMENT_BEARING"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    std::vector<BearingElementData>& getData() { return data_; }
    const std::vector<BearingElementData>& getData() const { return data_; }

private:
    std::vector<BearingElementData> data_;
};

/**
 * @brief Lancing element data
 */
struct KOO_API LancingElementData {
    ElementId eid = 0;        // Element ID
    int n1 = 0;               // First node
    int n2 = 0;               // Second node
    int n3 = 0;               // Third node (optional)
    int n4 = 0;               // Fourth node (optional)
    int pid = 0;              // Part ID
    double tbirth = 0.0;      // Birth time
    double tdeath = 1e28;     // Death time
};

/**
 * @brief *ELEMENT_LANCING keyword - defines lancing (cutting) elements
 *
 * Used in sheet metal forming to define trimming/lancing operations.
 */
class KOO_API ElementLancing : public CloneableKeyword<ElementLancing, Keyword> {
public:
    ElementLancing() = default;

    std::string getKeywordName() const override { return "*ELEMENT_LANCING"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    std::vector<LancingElementData>& getData() { return data_; }
    const std::vector<LancingElementData>& getData() const { return data_; }

private:
    std::vector<LancingElementData> data_;
};

/**
 * @brief Generalized shell element data
 */
struct KOO_API GeneralizedShellElementData {
    ElementId eid = 0;        // Element ID
    PartId pid = 0;           // Part ID
    NodeId n1 = 0;            // Node 1
    NodeId n2 = 0;            // Node 2
    NodeId n3 = 0;            // Node 3
    NodeId n4 = 0;            // Node 4 (optional)
    NodeId n5 = 0;            // Node 5 (optional)
    NodeId n6 = 0;            // Node 6 (optional)
    NodeId n7 = 0;            // Node 7 (optional)
    NodeId n8 = 0;            // Node 8 (optional)
};

/**
 * @brief *ELEMENT_GENERALIZED_SHELL keyword
 *
 * Defines generalized shell elements with higher-order interpolation.
 */
class KOO_API ElementGeneralizedShell : public CloneableKeyword<ElementGeneralizedShell, Keyword> {
public:
    ElementGeneralizedShell() = default;

    std::string getKeywordName() const override { return "*ELEMENT_GENERALIZED_SHELL"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    std::vector<GeneralizedShellElementData>& getData() { return data_; }
    const std::vector<GeneralizedShellElementData>& getData() const { return data_; }

private:
    std::vector<GeneralizedShellElementData> data_;
};

/**
 * @brief *ELEMENT_SOLID_ORTHO
 *
 * Solid element with orthotropic material orientation.
 */
class KOO_API ElementSolidOrtho : public CloneableKeyword<ElementSolidOrtho, Keyword> {
public:
    struct Data {
        ElementId eid = 0;
        PartId pid = 0;
        NodeId n1 = 0, n2 = 0, n3 = 0, n4 = 0;
        NodeId n5 = 0, n6 = 0, n7 = 0, n8 = 0;
        double a1 = 0.0, a2 = 0.0, a3 = 0.0;  // a-direction vector
        double d1 = 0.0, d2 = 0.0, d3 = 0.0;  // d-direction vector
    };

    ElementSolidOrtho() = default;

    std::string getKeywordName() const override { return "*ELEMENT_SOLID_ORTHO"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    std::vector<Data>& getData() { return data_; }
    const std::vector<Data>& getData() const { return data_; }

private:
    std::vector<Data> data_;
};

/**
 * @brief *ELEMENT_BEAM_PULLEY
 *
 * Beam element for pulley systems.
 */
class KOO_API ElementBeamPulley : public CloneableKeyword<ElementBeamPulley, Keyword> {
public:
    struct Data {
        ElementId eid = 0;
        PartId pid = 0;
        NodeId n1 = 0;          // Node 1
        NodeId n2 = 0;          // Node 2
        NodeId n3 = 0;          // Pulley node
        double radius = 0.0;    // Pulley radius
    };

    ElementBeamPulley() = default;

    std::string getKeywordName() const override { return "*ELEMENT_BEAM_PULLEY"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    std::vector<Data>& getData() { return data_; }
    const std::vector<Data>& getData() const { return data_; }

private:
    std::vector<Data> data_;
};

/**
 * @brief *ELEMENT_SHELL_COMPOSITE
 *
 * Composite shell element with layered structure.
 */
class KOO_API ElementShellComposite : public CloneableKeyword<ElementShellComposite, Keyword> {
public:
    struct Data {
        ElementId eid = 0;
        PartId pid = 0;
        NodeId n1 = 0, n2 = 0, n3 = 0, n4 = 0;
        int icomp = 0;          // Composite flag
        int setefg = 0;         // Set EFG
    };

    ElementShellComposite() = default;

    std::string getKeywordName() const override { return "*ELEMENT_SHELL_COMPOSITE"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    std::vector<Data>& getData() { return data_; }
    const std::vector<Data>& getData() const { return data_; }

private:
    std::vector<Data> data_;
};

/**
 * @brief *ELEMENT_DIRECT_MATRIX_INPUT
 *
 * Direct matrix input element.
 */
class KOO_API ElementDirectMatrixInput : public CloneableKeyword<ElementDirectMatrixInput, Keyword> {
public:
    struct Data {
        int id = 0;             // Matrix ID
        int type = 0;           // Matrix type (stiffness, mass, damping)
        int nrow = 0;           // Number of rows
        int ncol = 0;           // Number of columns
        std::vector<double> matrix;  // Matrix values
    };

    ElementDirectMatrixInput() = default;

    std::string getKeywordName() const override { return "*ELEMENT_DIRECT_MATRIX_INPUT"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    Data& getData() { return data_; }
    const Data& getData() const { return data_; }

private:
    Data data_;
};

/**
 * @brief *ELEMENT_INTERPOLATION_SHELL
 *
 * Interpolation shell element.
 */
class KOO_API ElementInterpolationShell : public CloneableKeyword<ElementInterpolationShell, Keyword> {
public:
    struct Data {
        ElementId eid = 0;
        PartId pid = 0;
        NodeId n1 = 0, n2 = 0, n3 = 0, n4 = 0;
        int iflag = 0;          // Interpolation flag
    };

    ElementInterpolationShell() = default;

    std::string getKeywordName() const override { return "*ELEMENT_INTERPOLATION_SHELL"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    std::vector<Data>& getData() { return data_; }
    const std::vector<Data>& getData() const { return data_; }

private:
    std::vector<Data> data_;
};

/**
 * @brief *ELEMENT_TRIM
 *
 * Trim element for forming analysis.
 */
class KOO_API ElementTrim : public CloneableKeyword<ElementTrim, Keyword> {
public:
    struct Data {
        ElementId eid = 0;
        int n1 = 0, n2 = 0, n3 = 0;
        int pid = 0;
        double tbirth = 0.0;
        double tdeath = 1e28;
    };

    ElementTrim() = default;

    std::string getKeywordName() const override { return "*ELEMENT_TRIM"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    std::vector<Data>& getData() { return data_; }
    const std::vector<Data>& getData() const { return data_; }

private:
    std::vector<Data> data_;
};

/**
 * @brief *ELEMENT_MASS_NODE_SET
 *
 * Mass element on node set.
 */
class KOO_API ElementMassNodeSet : public CloneableKeyword<ElementMassNodeSet, Keyword> {
public:
    struct Data {
        int nsid = 0;           // Node set ID
        double mass = 0.0;      // Total mass to distribute
        int pid = 0;            // Part ID
    };

    ElementMassNodeSet() = default;

    std::string getKeywordName() const override { return "*ELEMENT_MASS_NODE_SET"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    std::vector<Data>& getData() { return data_; }
    const std::vector<Data>& getData() const { return data_; }

private:
    std::vector<Data> data_;
};

/**
 * @brief *ELEMENT_SEATBELT_SLIPRING
 *
 * Seatbelt slipring element.
 */
class KOO_API ElementSeatbeltSlipring : public CloneableKeyword<ElementSeatbeltSlipring, Keyword> {
public:
    struct Data {
        ElementId id = 0;
        NodeId n1 = 0;          // Node 1
        NodeId n2 = 0;          // Node 2
        NodeId n3 = 0;          // Slipring node
        int sbid = 0;           // Seatbelt ID
        double friction = 0.0;  // Friction coefficient
    };

    ElementSeatbeltSlipring() = default;

    std::string getKeywordName() const override { return "*ELEMENT_SEATBELT_SLIPRING"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    std::vector<Data>& getData() { return data_; }
    const std::vector<Data>& getData() const { return data_; }

private:
    std::vector<Data> data_;
};

/**
 * @brief *ELEMENT_SHELL_SOURCE_SINK
 *
 * Shell element with source/sink for mass addition.
 */
class KOO_API ElementShellSourceSink : public CloneableKeyword<ElementShellSourceSink, Keyword> {
public:
    struct Data {
        ElementId eid = 0;
        PartId pid = 0;
        NodeId n1 = 0, n2 = 0, n3 = 0, n4 = 0;
        int source = 0;         // Source element ID
        int sink = 0;           // Sink element ID
    };

    ElementShellSourceSink() = default;

    std::string getKeywordName() const override { return "*ELEMENT_SHELL_SOURCE_SINK"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    std::vector<Data>& getData() { return data_; }
    const std::vector<Data>& getData() const { return data_; }

private:
    std::vector<Data> data_;
};

} // namespace koo::dyna
