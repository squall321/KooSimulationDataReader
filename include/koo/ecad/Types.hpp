#pragma once

#include <koo/Export.hpp>
#include <string>
#include <vector>
#include <unordered_map>
#include <cstdint>

namespace koo::ecad {

// ============================================================================
// Basic Types
// ============================================================================

/// 2D Point
struct Point2D {
    double x = 0.0;
    double y = 0.0;

    Point2D() = default;
    Point2D(double x_, double y_) : x(x_), y(y_) {}

    Point2D operator+(const Point2D& other) const {
        return {x + other.x, y + other.y};
    }

    Point2D operator-(const Point2D& other) const {
        return {x - other.x, y - other.y};
    }

    Point2D operator*(double scalar) const {
        return {x * scalar, y * scalar};
    }

    bool operator==(const Point2D& other) const {
        return x == other.x && y == other.y;
    }
};

/// 2D Bounding Box
struct BoundingBox2D {
    Point2D min{1e30, 1e30};
    Point2D max{-1e30, -1e30};

    BoundingBox2D() = default;
    BoundingBox2D(const Point2D& min_, const Point2D& max_) : min(min_), max(max_) {}

    void expand(const Point2D& point) {
        if (point.x < min.x) min.x = point.x;
        if (point.y < min.y) min.y = point.y;
        if (point.x > max.x) max.x = point.x;
        if (point.y > max.y) max.y = point.y;
    }

    void expand(const BoundingBox2D& other) {
        expand(other.min);
        expand(other.max);
    }

    bool contains(const Point2D& point) const {
        return point.x >= min.x && point.x <= max.x &&
               point.y >= min.y && point.y <= max.y;
    }

    Point2D center() const {
        return {(min.x + max.x) * 0.5, (min.y + max.y) * 0.5};
    }

    double width() const { return max.x - min.x; }
    double height() const { return max.y - min.y; }

    bool isValid() const {
        return min.x <= max.x && min.y <= max.y;
    }
};

// ============================================================================
// Enumerations
// ============================================================================

/// Feature polarity
enum class Polarity {
    Positive,   ///< Add material
    Negative    ///< Remove material (void)
};

/// Feature types in ODB++
enum class FeatureType {
    Line,       ///< Line segment
    Pad,        ///< Pad (aperture flash)
    Arc,        ///< Circular arc
    Surface,    ///< Filled polygon
    Text,       ///< Text annotation
    Barcode     ///< Barcode
};

/// Layer types
enum class LayerType {
    Signal,         ///< Signal/routing layer (copper)
    PowerGround,    ///< Power/ground plane
    Mixed,          ///< Mixed signal/plane
    SolderMask,     ///< Solder mask
    SilkScreen,     ///< Silk screen
    SolderPaste,    ///< Solder paste
    Drill,          ///< Drill layer
    Rout,           ///< Routing/outline
    Document,       ///< Documentation
    Component,      ///< Component layer
    Mask,           ///< Generic mask
    ConductPaste,   ///< Conductive paste
    Unknown
};

/// Layer context
enum class LayerContext {
    Board,      ///< Part of the actual board
    Misc,       ///< Miscellaneous
    Document    ///< Documentation only
};

/// Layer side
enum class Side {
    Top,
    Bottom,
    Inner,
    Both,
    None
};

/// Drill type
enum class DrillType {
    Plated,         ///< Plated through hole
    NonPlated,      ///< Non-plated hole
    Via,            ///< Via
    LaserTop,       ///< Laser drill from top
    LaserBottom,    ///< Laser drill from bottom
    BackDrill       ///< Back drill
};

/// Step type
enum class StepType {
    Pcb,        ///< Single PCB
    Panel,      ///< Panel with multiple PCBs
    Array,      ///< Array
    Coupon,     ///< Test coupon
    Template,   ///< Template
    Other
};

/// Component mounting side
enum class MountSide {
    Top,
    Bottom
};

/// Pin type
enum class PinType {
    ThroughHole,
    Smd,
    Bga,
    Fiducial,
    Other
};

/// Net class (for design rules)
enum class NetClass {
    Signal,
    Power,
    Ground,
    Clock,
    Differential,
    Other
};

/// ODB++ version
enum class OdbVersion {
    V7_0,
    V8_0,
    V8_1,
    Unknown
};

/// Symbol type - All ODB++ standard symbols (Appendix A)
enum class SymbolType {
    // Basic shapes
    Round,              ///< Round aperture (r<d>)
    Square,             ///< Square aperture (s<s>)
    Rectangle,          ///< Rectangle (rect<w>x<h>)
    RoundedRectangle,   ///< Rounded rectangle (rect<w>x<h>xr<rad>)
    ChamferedRectangle, ///< Chamfered rectangle (rect<w>x<h>xc<rad>)
    Oblong,             ///< Oblong/oval (oval<w>x<h>)
    Diamond,            ///< Diamond (di<w>x<h>)
    Octagon,            ///< Octagon (oct<w>x<h>x<r>)
    Triangle,           ///< Triangle (tri<base>x<h>)
    HalfOval,           ///< Half oval (oval_h<w>x<h>)

    // Hexagons
    HorizontalHexagon,  ///< Horizontal hexagon (hex_l<w>x<h>x<r>)
    VerticalHexagon,    ///< Vertical hexagon (hex_s<w>x<h>x<r>)

    // Butterflies
    Butterfly,          ///< Butterfly (bfr<d>)
    SquareButterfly,    ///< Square butterfly (bfs<s>)

    // Donuts
    RoundDonut,         ///< Round donut (donut_r<od>x<id>)
    SquareDonut,        ///< Square donut (donut_s<od>x<id>)
    SquareRoundDonut,   ///< Square/round donut (donut_sr<od>x<id>)
    RoundedSquareDonut, ///< Rounded square donut (donut_s<od>x<id>xr<rad>)
    RectangleDonut,     ///< Rectangle donut (donut_rc<ow>x<oh>x<lw>)
    RoundedRectDonut,   ///< Rounded rectangle donut (donut_rc<ow>x<oh>x<lw>xr<rad>)
    OvalDonut,          ///< Oval donut (donut_o<ow>x<oh>x<lw>)

    // Thermals
    RoundThermalRounded,    ///< Round thermal rounded (thr<od>x<id>x<angle>x<spokes>x<gap>)
    RoundThermalSquared,    ///< Round thermal squared (ths<od>x<id>x<angle>x<spokes>x<gap>)
    SquareThermal,          ///< Square thermal (s_ths<os>x<is>x<angle>x<spokes>x<gap>)
    SquareThermalOpenCorner,///< Square thermal open corners (s_tho<od>x<id>x<angle>x<spokes>x<gap>)
    LineThermal,            ///< Line thermal (s_thr<os>x<is>x<angle>x<spokes>x<gap>)
    SquareRoundThermal,     ///< Square-round thermal (sr_ths<os>x<id>x<angle>x<spokes>x<gap>)
    RectangularThermal,     ///< Rectangular thermal (rc_ths<w>x<h>x<angle>x<spokes>x<gap>x<air_gap>)
    RectThermalOpenCorner,  ///< Rectangular thermal open corners (rc_tho)
    RoundedSquareThermal,   ///< Rounded square thermal (s_ths with xr<rad>)
    RoundedRectThermal,     ///< Rounded rectangle thermal (rc_ths with xr<rad>)
    OvalThermal,            ///< Oval thermal (o_ths<ow>x<oh>x<angle>x<spokes>x<gap>x<lw>)
    OblongThermal,          ///< Oblong thermal (oblong_ths)

    // Solder stencil symbols
    HomePlate,              ///< Home plate (hplate<w>x<h>x<c>)
    InvertedHomePlate,      ///< Inverted home plate (rhplate<w>x<h>x<c>)
    FlatHomePlate,          ///< Flat home plate (fhplate<w>x<h>x<vc>x<hc>)
    RadiusedInvHomePlate,   ///< Radiused inverted home plate (radhplate)
    RadiusedHomePlate,      ///< Radiused home plate (radplate)
    Cross,                  ///< Cross symbol
    DogBone,                ///< Dog bone symbol
    DPack,                  ///< D-pack symbol

    // Special symbols
    Target,                 ///< Target symbol
    Null,                   ///< Null/empty symbol (null<n>)
    Moire,                  ///< Moire pattern
    Hole,                   ///< Hole symbol (hole<d>x<p>x<tp>x<tm>)

    // Legacy/compatibility
    Thermal,                ///< Generic thermal (legacy)

    User                    ///< User-defined symbol
};

// ============================================================================
// Attribute handling
// ============================================================================

/// Attribute list (key-value pairs)
using AttributeList = std::unordered_map<std::string, std::string>;

// ============================================================================
// Job information
// ============================================================================

/// ODB++ job metadata
struct JobInfo {
    std::string name;
    std::string creationDate;
    std::string modificationDate;
    std::string saveApp;
    std::string saveUser;
    OdbVersion version = OdbVersion::Unknown;
    std::string units = "MM";   ///< "MM" or "INCH"
};

// ============================================================================
// Step repeat (for panel definition)
// ============================================================================

/// Step repeat structure (placement of sub-steps)
struct StepRepeat {
    std::string stepName;       ///< Name of the repeated step
    double x = 0.0;             ///< X offset
    double y = 0.0;             ///< Y offset
    double dx = 0.0;            ///< X spacing for array
    double dy = 0.0;            ///< Y spacing for array
    int nx = 1;                 ///< Number in X direction
    int ny = 1;                 ///< Number in Y direction
    double angle = 0.0;         ///< Rotation angle (degrees)
    bool mirror = false;        ///< Mirror flag
};

// ============================================================================
// Component Variants
// ============================================================================

/// Component variant state
enum class ComponentVariantState {
    Enabled,    ///< Component is populated
    Disabled,   ///< Component not populated (DNP)
    Replaced    ///< Component replaced with alternate
};

/// Component variant entry
struct ComponentVariantEntry {
    std::string refDes;                     ///< Component reference designator
    ComponentVariantState state = ComponentVariantState::Enabled;
    std::string alternatePartNumber;        ///< Alternate part number if replaced
    std::string alternatePackage;           ///< Alternate package if changed
    AttributeList attributes;
};

/// Component variant (design variant/configuration)
struct ComponentVariant {
    std::string name;                       ///< Variant name
    std::string description;                ///< Variant description
    std::vector<ComponentVariantEntry> components;
    AttributeList attributes;
};

// ============================================================================
// Embedded Components
// ============================================================================

/// Embedded component type
enum class EmbeddedComponentType {
    Resistor,       ///< EMBEDDED_R - Embedded resistor
    Capacitor,      ///< EMBEDDED_C - Embedded capacitor
    Inductor,       ///< EMBEDDED_L - Embedded inductor
    Unknown
};

/// Embedded component definition
struct EmbeddedComponent {
    std::string name;                               ///< Component name/identifier
    EmbeddedComponentType type = EmbeddedComponentType::Unknown;
    double value = 0.0;                             ///< Value (ohms, farads, henries)
    std::string valueUnit;                          ///< Value unit (ohm, pF, nH, etc.)
    double tolerance = 0.0;                         ///< Tolerance percentage
    double x = 0.0, y = 0.0;                        ///< Position
    double width = 0.0, height = 0.0;               ///< Size
    std::string layerName;                          ///< Layer where embedded
    int startLayer = -1;                            ///< Start layer number
    int endLayer = -1;                              ///< End layer number
    std::string material;                           ///< Material name/type
    double thickness = 0.0;                         ///< Thickness
    AttributeList attributes;
};

// ============================================================================
// Build-up Information
// ============================================================================

/// Via span type
enum class ViaSpanType {
    Through,        ///< Through-hole via (all layers)
    Blind,          ///< Blind via (outer to inner)
    Buried,         ///< Buried via (inner to inner)
    Microvia,       ///< Microvia (single layer span)
    Unknown
};

/// Via span definition
struct ViaSpan {
    std::string name;                       ///< Span name
    ViaSpanType type = ViaSpanType::Unknown;
    int startLayer = -1;                    ///< Start layer number
    int endLayer = -1;                      ///< End layer number
    std::string startLayerName;             ///< Start layer name
    std::string endLayerName;               ///< End layer name
    double minDiameter = 0.0;               ///< Minimum via diameter
    double maxDiameter = 0.0;               ///< Maximum via diameter
    std::string drillMethod;                ///< Drill method (mechanical, laser, etc.)
    AttributeList attributes;
};

/// Build-up layer (for HDI/sequential lamination)
struct BuildupLayer {
    std::string name;                       ///< Layer name
    int sequence = 0;                       ///< Build-up sequence number
    std::string type;                       ///< Layer type (core, prepreg, copper)
    double thickness = 0.0;                 ///< Thickness
    std::string material;                   ///< Material name
    bool isCore = false;                    ///< Is this a core layer?
    int copperLayerTop = -1;                ///< Top copper layer
    int copperLayerBottom = -1;             ///< Bottom copper layer
    AttributeList attributes;
};

/// Build-up information container
struct BuildupInfo {
    std::string name;                           ///< Build-up name
    int totalLayers = 0;                        ///< Total number of copper layers
    double totalThickness = 0.0;                ///< Total board thickness
    std::vector<ViaSpan> viaSpans;              ///< Via span definitions
    std::vector<BuildupLayer> layers;           ///< Build-up layers
    AttributeList attributes;
};

// ============================================================================
// VPL (Vendor Part List)
// ============================================================================

/// Vendor part entry
struct VendorPart {
    std::string partNumber;                 ///< Internal part number
    std::string vendorName;                 ///< Vendor/manufacturer name
    std::string vendorPartNumber;           ///< Vendor's part number
    std::string description;                ///< Part description
    std::string category;                   ///< Part category
    double unitPrice = 0.0;                 ///< Unit price
    std::string currency;                   ///< Currency code
    int leadTime = 0;                       ///< Lead time in days
    int minOrderQty = 0;                    ///< Minimum order quantity
    std::string datasheet;                  ///< Datasheet URL/path
    AttributeList attributes;
};

// ============================================================================
// Customer Information
// ============================================================================

/// Customer information
struct CustomerInfo {
    std::string name;                       ///< Customer name
    std::string company;                    ///< Company name
    std::string projectName;                ///< Project name
    std::string projectNumber;              ///< Project/order number
    std::string revision;                   ///< Design revision
    std::string contact;                    ///< Contact person
    std::string email;                      ///< Email address
    std::string phone;                      ///< Phone number
    std::string address;                    ///< Address
    std::string notes;                      ///< Additional notes
    AttributeList attributes;
};

// ============================================================================
// Unit conversion
// ============================================================================

/// Unit for ODB++ coordinates (typically mils or mm)
constexpr double MIL_TO_MM = 0.0254;
constexpr double MM_TO_MIL = 1.0 / MIL_TO_MM;
constexpr double INCH_TO_MM = 25.4;
constexpr double MM_TO_INCH = 1.0 / INCH_TO_MM;
constexpr double MICRON_TO_MM = 0.001;
constexpr double MM_TO_MICRON = 1000.0;

// ============================================================================
// Drill Tool Types
// ============================================================================

/// Drill tool type
enum class DrillToolType {
    Plated,     ///< PLATED
    NonPlated,  ///< NON_PLATED
    Via         ///< VIA
};

/// Drill tool type2 (sub-type)
enum class DrillToolType2 {
    Standard,   ///< STANDARD
    PressFit,   ///< PRESS_FIT
    Photo,      ///< PHOTO (for via)
    Laser       ///< LASER (for via)
};

/// Drill tool definition
struct DrillTool {
    int num = 0;                                ///< Tool number
    DrillToolType type = DrillToolType::Plated; ///< Tool type
    DrillToolType2 type2 = DrillToolType2::Standard; ///< Sub-type
    double minTol = 0.0;                        ///< Minimum tolerance
    double maxTol = 0.0;                        ///< Maximum tolerance
    std::string bit;                            ///< Drill bit string
    double finishSize = -1.0;                   ///< Finished hole size
    double drillSize = -1.0;                    ///< Drill tool size
    int drillCount = 0;                         ///< Number of holes drilled
    double diameter = 0.0;                      ///< Calculated diameter
};

// ============================================================================
// Attribute Types
// ============================================================================

/// Attribute value type
enum class AttributeType {
    Boolean,    ///< True/false
    Integer,    ///< Integer number
    Float,      ///< Floating point number
    Option,     ///< Enumerated option
    Text        ///< Text string
};

/// Attribute class (category)
enum class AttributeClass {
    Product,        ///< Product attributes (sysattr)
    DFM,            ///< DFM analysis attributes (sysattr.dfm)
    Fabrication,    ///< Fabrication process (sysattr.fab)
    Assembly,       ///< Assembly process (sysattr.assy)
    Test,           ///< Test process (sysattr.test)
    Generic,        ///< Generic process (sysattr.gen)
    User            ///< User-defined (userattr)
};

/// Attribute definition
struct AttributeDefinition {
    std::string name;
    AttributeType type = AttributeType::Text;
    AttributeClass attrClass = AttributeClass::User;
    std::string description;
    std::string unitType;                       ///< DISTANCE, SIZE, TEMPERATURE, etc.
    std::string units;                          ///< MM, INCH, etc.
    std::vector<std::string> options;           ///< For OPTION type
    double minValue = 0.0;                      ///< For numeric types
    double maxValue = 0.0;                      ///< For numeric types
    std::string defaultValue;
};

// ============================================================================
// Subnet Types (EDA Data)
// ============================================================================

/// Subnet type
enum class SubnetType {
    Toeprint,   ///< TOP - Connection of component pin to board
    Via,        ///< VIA - Connectivity padstack between layers
    Trace,      ///< TRC - Collection of lines/arcs
    Plane       ///< PLN - Surface used for connectivity
};

/// Plane fill type
enum class PlaneFillType {
    Solid,      ///< S - Solid fill
    Outline     ///< O - Outline only
};

/// Plane cutout type
enum class PlaneCutoutType {
    Circle,     ///< C - Circle cutout
    Rectangle,  ///< R - Rectangle cutout
    Octagon,    ///< O - Octagon cutout
    Exact       ///< E - Exact shape
};

/// Feature ID reference (for EDA linking)
struct FeatureId {
    char type = 'C';    ///< C=Copper, L=Laminate, H=Hole
    int layerNum = 0;   ///< Layer index
    int featureNum = 0; ///< Feature index in layer
};

/// Subnet definition
struct Subnet {
    SubnetType type = SubnetType::Trace;
    // For Toeprint
    char side = 'T';            ///< T=Top, B=Bottom
    int componentNum = 0;       ///< Component index
    int toeprintNum = 0;        ///< Toeprint index
    // For Plane
    PlaneFillType fillType = PlaneFillType::Solid;
    PlaneCutoutType cutoutType = PlaneCutoutType::Exact;
    double fillSize = 0.0;
    // Feature references
    std::vector<FeatureId> features;
};

// ============================================================================
// BOM (Bill of Materials) Types
// ============================================================================

/// BOM item
struct BomItem {
    std::string refDes;             ///< Reference designator
    std::string partNumber;         ///< Part number
    std::string manufacturer;       ///< Manufacturer
    std::string description;        ///< Description
    int quantity = 1;               ///< Quantity
    std::vector<std::string> descriptions; ///< Multiple descriptions (DESC1, DESC2, ...)
    AttributeList attributes;       ///< Additional attributes
};

// ============================================================================
// Netlist Types
// ============================================================================

/// Netlist record type
enum class NetlistRecordType {
    Net,            ///< N - Net definition
    Point,          ///< P - Net point
    Staggered,      ///< ST - Staggered point
    Optimize,       ///< H - Optimization record
    SubNet,         ///< SN - Sub-net definition
    Via,            ///< V - Via definition
    Trace,          ///< T - Trace
    LoadedNet       ///< LD - Loaded net
};

/// Netlist point
struct NetlistPoint {
    std::string netName;
    double x = 0.0;
    double y = 0.0;
    double radius = 0.0;        ///< Radius for round connection
    char side = 'T';            ///< T=Top, B=Bottom, I=Inner
    bool staggered = false;     ///< Staggered via
    int epoint = 0;             ///< End point type
    int exp = 0;                ///< Expansion
};

// ============================================================================
// Component Toeprint Types
// ============================================================================

/// Toeprint (pad on component footprint)
struct Toeprint {
    int pinNum = 0;             ///< Pin number
    double x = 0.0;             ///< X location
    double y = 0.0;             ///< Y location
    double rotation = 0.0;      ///< Rotation angle
    bool mirror = false;        ///< Mirror flag
    std::string netName;        ///< Connected net
    int netNum = -1;            ///< Net index
    int subnetNum = -1;         ///< Subnet index
    std::string name;           ///< Pin name
};

// ============================================================================
// HDI (High Density Interconnect) Types
// ============================================================================

/// HDI package type
enum class HdiPackageType {
    Die,        ///< DIE or SRC - Source component
    Bga,        ///< BGA or DST - Destination component
    Source,
    Destination
};

/// HDI package
struct HdiPackage {
    std::string name;
    std::string refDes;
    HdiPackageType type = HdiPackageType::Die;
};

/// HDI net point
struct HdiNetPoint {
    std::string net;            ///< Net name
    double x = 0.0;
    double y = 0.0;
    std::string symbol;
    std::string die;            ///< Source component name
    std::string bump;           ///< Bump name
    std::string destComp;       ///< Destination component
    std::string destPin;        ///< Destination pin
    std::string gateComp;       ///< Gate component
    std::string gatePin;        ///< Gate pin
};

// ============================================================================
// Zone Types (Board regions)
// ============================================================================

/// Zone definition
struct Zone {
    std::string name;
    std::vector<Point2D> boundary;  ///< Zone boundary polygon
    int startLayer = 0;             ///< Start layer index
    int endLayer = 0;               ///< End layer index
    AttributeList attributes;
};

// ============================================================================
// Dimension Types
// ============================================================================

/// Dimension type
enum class DimensionType {
    Linear,
    Radial,
    Angular,
    Leader,
    Note
};

/// Dimension definition
struct Dimension {
    DimensionType type = DimensionType::Linear;
    Point2D start;
    Point2D end;
    Point2D textPosition;
    std::string text;
    double value = 0.0;
    std::string units;
    AttributeList attributes;
};

// ============================================================================
// Stackup Types (Layer stack)
// ============================================================================

/// Material type in stackup
enum class StackupMaterialType {
    Copper,
    Core,
    Prepreg,
    SolderMask,
    Coverlay,
    Adhesive,
    Stiffener,
    Other
};

/// Stackup layer
struct StackupLayer {
    std::string name;
    StackupMaterialType materialType = StackupMaterialType::Other;
    double thickness = 0.0;         ///< Layer thickness
    double dielectricConstant = 0.0;///< Er value
    double lossTangent = 0.0;       ///< Loss tangent
    std::string material;           ///< Material name
    int layerIndex = -1;            ///< Corresponding matrix layer
    AttributeList properties;
};

// ============================================================================
// Impedance Types
// ============================================================================

/// Impedance constraint
struct ImpedanceConstraint {
    std::string id;
    std::string type;               ///< single, differential, etc.
    double impedance = 0.0;         ///< Target impedance (ohms)
    double tolerance = 0.0;         ///< Tolerance percentage
    std::string layer;              ///< Layer name
    double traceWidth = 0.0;        ///< Trace width
    double spacing = 0.0;           ///< Spacing (for differential)
};

// ============================================================================
// Short Net Types
// ============================================================================

/// Intentional short definition
struct IntentionalShort {
    std::vector<uint32_t> netUids;  ///< UIDs of shorted nets
    std::vector<FeatureId> features;///< Features causing the short
    uint32_t id = 0;                ///< Unique ID
};

// ============================================================================
// Metadata Types
// ============================================================================

/// ODB++ metadata entry
struct MetadataEntry {
    std::string key;
    std::string value;
    std::string type;               ///< Optional type (string, integer, float)
    std::string unit;               ///< Optional unit (mm, inch, etc.)
};

/// ODB++ metadata container
struct Metadata {
    std::string version;                    ///< ODB++ version
    std::string source;                     ///< Source application
    std::string creationDate;               ///< Creation timestamp
    std::string modificationDate;           ///< Last modification timestamp
    std::vector<MetadataEntry> entries;     ///< All metadata entries
    AttributeList attributes;               ///< Quick lookup map
};

} // namespace koo::ecad
