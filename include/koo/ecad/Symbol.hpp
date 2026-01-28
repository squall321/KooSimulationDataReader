#pragma once

#include <koo/Export.hpp>
#include <koo/ecad/Types.hpp>
#include <koo/ecad/Feature.hpp>
#include <memory>
#include <string>
#include <vector>
#include <optional>
#include <cstdint>

namespace koo::ecad {

/**
 * @brief Symbol definition for ODB++
 *
 * Symbols define the shapes used for pads, lines, and arcs.
 * There are two types:
 * 1. Standard symbols - parametric shapes (r10 = 10mil round, s50 = 50mil square)
 * 2. User-defined symbols - custom shapes defined with features
 *
 * ODB++ Standard Symbol naming conventions (Appendix A):
 *
 * Basic shapes:
 * - r<d>                   : Round (circle) with diameter d
 * - s<s>                   : Square with side s
 * - rect<w>x<h>            : Rectangle width w, height h
 * - rect<w>x<h>xr<rad>     : Rounded rectangle
 * - rect<w>x<h>xc<rad>     : Chamfered rectangle
 * - oval<w>x<h>            : Oblong/oval
 * - di<w>x<h>              : Diamond
 * - oct<w>x<h>x<r>         : Octagon
 * - tri<base>x<h>          : Triangle
 * - oval_h<w>x<h>          : Half oval
 *
 * Hexagons:
 * - hex_l<w>x<h>x<r>       : Horizontal hexagon
 * - hex_s<w>x<h>x<r>       : Vertical hexagon
 *
 * Butterflies:
 * - bfr<d>                 : Round butterfly
 * - bfs<s>                 : Square butterfly
 *
 * Donuts:
 * - donut_r<od>x<id>       : Round donut
 * - donut_s<od>x<id>       : Square donut
 * - donut_sr<od>x<id>      : Square/round donut
 * - donut_rc<ow>x<oh>x<lw> : Rectangle donut
 * - donut_o<ow>x<oh>x<lw>  : Oval donut
 *
 * Thermals:
 * - thr<od>x<id>x<angle>x<spokes>x<gap>   : Round thermal (rounded)
 * - ths<od>x<id>x<angle>x<spokes>x<gap>   : Round thermal (squared)
 * - s_ths<os>x<is>x<angle>x<spokes>x<gap> : Square thermal
 * - s_tho<od>x<id>x<angle>x<spokes>x<gap> : Square thermal (open corners)
 * - s_thr<os>x<is>x<angle>x<spokes>x<gap> : Line thermal
 * - sr_ths<os>x<id>x<angle>x<spokes>x<gap>: Square-round thermal
 * - rc_ths<w>x<h>x<angle>x<spokes>x<gap>x<air_gap> : Rectangular thermal
 * - rc_tho<w>x<h>x<angle>x<spokes>x<gap>x<air_gap> : Rect thermal (open corners)
 * - o_ths<ow>x<oh>x<angle>x<spokes>x<gap>x<lw>     : Oval thermal
 *
 * Solder stencil symbols:
 * - hplate<w>x<h>x<c>              : Home plate
 * - rhplate<w>x<h>x<c>             : Inverted home plate
 * - fhplate<w>x<h>x<vc>x<hc>       : Flat home plate
 * - radhplate<w>x<h>x<ms>          : Radiused inverted home plate
 * - radplate<w>x<h>x<ms>           : Radiused home plate
 *
 * Special:
 * - null<n>                : Null symbol
 * - target<d>x<iw>x<ow>x<ig>x<og>  : Target symbol
 * - hole<d>x<p>x<tp>x<tm>  : Hole symbol
 */
class KOO_API Symbol {
public:
    Symbol() = default;
    explicit Symbol(const std::string& name);
    virtual ~Symbol() = default;

    /// Get symbol name
    const std::string& getName() const { return name_; }
    void setName(const std::string& name) { name_ = name; }

    /// Get symbol type
    SymbolType getType() const { return type_; }
    void setType(SymbolType type) { type_ = type; }

    /// Check if this is a standard (parametric) symbol
    bool isStandard() const { return isStandard_; }

    /// Get bounding box
    BoundingBox2D getBoundingBox() const { return boundingBox_; }
    void setBoundingBox(const BoundingBox2D& box) { boundingBox_ = box; }

    /// Attributes
    const AttributeList& getAttributes() const { return attributes_; }
    void setAttribute(const std::string& key, const std::string& value) {
        attributes_[key] = value;
    }

    /// For user-defined symbols: feature list
    const std::vector<std::unique_ptr<Feature>>& getFeatures() const { return features_; }
    void addFeature(std::unique_ptr<Feature> feature);
    void clearFeatures();

    /// Clone the symbol
    std::unique_ptr<Symbol> clone() const;

    /// Parse standard symbol name and extract parameters
    /// Returns nullptr if not a valid standard symbol
    static std::unique_ptr<Symbol> parseStandardSymbol(const std::string& name);

    /// Check if a symbol name is a standard symbol
    static bool isStandardSymbolName(const std::string& name);

    // ========================================================================
    // Dimension accessors (used by various symbol types)
    // ========================================================================

    /// Primary dimension (diameter, width, outer size, etc.)
    double getPrimaryDimension() const { return primaryDim_; }
    void setPrimaryDimension(double dim) { primaryDim_ = dim; }

    /// Secondary dimension (height, inner diameter, etc.)
    double getSecondaryDimension() const { return secondaryDim_; }
    void setSecondaryDimension(double dim) { secondaryDim_ = dim; }

    /// Third dimension (line width, corner size, etc.)
    double getTertiaryDimension() const { return tertiaryDim_; }
    void setTertiaryDimension(double dim) { tertiaryDim_ = dim; }

    /// Fourth dimension (for complex symbols)
    double getQuaternaryDimension() const { return quaternaryDim_; }
    void setQuaternaryDimension(double dim) { quaternaryDim_ = dim; }

    /// Fifth dimension (for complex symbols)
    double getQuinaryDimension() const { return quinaryDim_; }
    void setQuinaryDimension(double dim) { quinaryDim_ = dim; }

    /// Sixth dimension (for complex symbols)
    double getSenaryDimension() const { return senaryDim_; }
    void setSenaryDimension(double dim) { senaryDim_ = dim; }

    /// For thermal symbols: spoke count
    int getSpokeCount() const { return spokeCount_; }
    void setSpokeCount(int count) { spokeCount_ = count; }

    /// For thermal symbols: spoke gap
    double getSpokeGap() const { return spokeGap_; }
    void setSpokeGap(double gap) { spokeGap_ = gap; }

    /// For thermal symbols: angle
    double getAngle() const { return angle_; }
    void setAngle(double angle) { angle_ = angle; }

    /// Corner radius (for rounded rectangles/thermals)
    double getCornerRadius() const { return cornerRadius_; }
    void setCornerRadius(double radius) { cornerRadius_ = radius; }

    /// Corner specification (bitmask: 1=corner1, 2=corner2, 4=corner3, 8=corner4)
    uint8_t getCorners() const { return corners_; }
    void setCorners(uint8_t corners) { corners_ = corners; }

    /// Unit of the symbol dimensions ('I' for imperial/mils, 'M' for metric/microns)
    char getUnit() const { return unit_; }
    void setUnit(char unit) { unit_ = unit; }

    // Helper methods for common dimension names
    double getWidth() const { return primaryDim_; }
    double getHeight() const { return secondaryDim_; }
    double getOuterDiameter() const { return primaryDim_; }
    double getInnerDiameter() const { return secondaryDim_; }
    double getLineWidth() const { return tertiaryDim_; }
    double getAirGap() const { return senaryDim_; }

protected:
    /// Tag for protected constructor that sets name without parsing (for derived classes)
    struct NoParseTag {};
    Symbol(const std::string& name, NoParseTag) : name_(name) {}
    std::string name_;
    SymbolType type_ = SymbolType::User;
    bool isStandard_ = false;
    BoundingBox2D boundingBox_;
    AttributeList attributes_;

    // For user-defined symbols
    std::vector<std::unique_ptr<Feature>> features_;

    // Dimensions (interpretation depends on symbol type)
    double primaryDim_ = 0.0;       // diameter, width, outer size
    double secondaryDim_ = 0.0;     // height, inner diameter
    double tertiaryDim_ = 0.0;      // line width, corner size
    double quaternaryDim_ = 0.0;    // for complex symbols
    double quinaryDim_ = 0.0;       // for complex symbols
    double senaryDim_ = 0.0;        // for complex symbols (air gap, etc.)

    // For thermal symbols
    int spokeCount_ = 4;
    double spokeGap_ = 0.0;
    double angle_ = 0.0;

    // For rounded/chamfered shapes
    double cornerRadius_ = 0.0;
    uint8_t corners_ = 0xFF;        // All corners by default

    // Unit specification
    char unit_ = 0;                 // 0=default, 'I'=imperial, 'M'=metric
};

// ============================================================================
// Standard Symbol Helper Classes
// ============================================================================

/**
 * @brief Round symbol (r<diameter>)
 */
class KOO_API RoundSymbol : public Symbol {
public:
    explicit RoundSymbol(double diameter);
    double getDiameter() const { return primaryDim_; }
};

/**
 * @brief Square symbol (s<size>)
 */
class KOO_API SquareSymbol : public Symbol {
public:
    explicit SquareSymbol(double size);
    double getSize() const { return primaryDim_; }
};

/**
 * @brief Rectangle symbol (rect<width>x<height>)
 */
class KOO_API RectangleSymbol : public Symbol {
public:
    RectangleSymbol(double width, double height);
    double getWidth() const { return primaryDim_; }
    double getHeight() const { return secondaryDim_; }
};

/**
 * @brief Rounded rectangle symbol (rect<w>x<h>xr<rad>[x<corners>])
 */
class KOO_API RoundedRectangleSymbol : public Symbol {
public:
    RoundedRectangleSymbol(double width, double height, double radius, uint8_t corners = 0xFF);
    double getWidth() const { return primaryDim_; }
    double getHeight() const { return secondaryDim_; }
    double getRadius() const { return cornerRadius_; }
};

/**
 * @brief Chamfered rectangle symbol (rect<w>x<h>xc<chamfer>[x<corners>])
 */
class KOO_API ChamferedRectangleSymbol : public Symbol {
public:
    ChamferedRectangleSymbol(double width, double height, double chamfer, uint8_t corners = 0xFF);
    double getWidth() const { return primaryDim_; }
    double getHeight() const { return secondaryDim_; }
    double getChamfer() const { return cornerRadius_; }
};

/**
 * @brief Oblong/Oval symbol (oval<width>x<height>)
 */
class KOO_API OblongSymbol : public Symbol {
public:
    OblongSymbol(double width, double height);
    double getWidth() const { return primaryDim_; }
    double getHeight() const { return secondaryDim_; }
};

/**
 * @brief Diamond symbol (di<width>x<height>)
 */
class KOO_API DiamondSymbol : public Symbol {
public:
    DiamondSymbol(double width, double height);
    double getWidth() const { return primaryDim_; }
    double getHeight() const { return secondaryDim_; }
};

/**
 * @brief Octagon symbol (oct<width>x<height>x<corner>)
 */
class KOO_API OctagonSymbol : public Symbol {
public:
    OctagonSymbol(double width, double height, double corner);
    double getWidth() const { return primaryDim_; }
    double getHeight() const { return secondaryDim_; }
    double getCornerSize() const { return tertiaryDim_; }
};

/**
 * @brief Triangle symbol (tri<base>x<height>)
 */
class KOO_API TriangleSymbol : public Symbol {
public:
    TriangleSymbol(double base, double height);
    double getBase() const { return primaryDim_; }
    double getHeight() const { return secondaryDim_; }
};

/**
 * @brief Half oval symbol (oval_h<width>x<height>)
 */
class KOO_API HalfOvalSymbol : public Symbol {
public:
    HalfOvalSymbol(double width, double height);
    double getWidth() const { return primaryDim_; }
    double getHeight() const { return secondaryDim_; }
};

/**
 * @brief Hexagon symbol (hex_l or hex_s<w>x<h>x<r>)
 */
class KOO_API HexagonSymbol : public Symbol {
public:
    HexagonSymbol(double width, double height, double corner, bool horizontal);
    double getWidth() const { return primaryDim_; }
    double getHeight() const { return secondaryDim_; }
    double getCornerSize() const { return tertiaryDim_; }
    bool isHorizontal() const { return type_ == SymbolType::HorizontalHexagon; }
};

/**
 * @brief Butterfly symbol (bfr<d> or bfs<s>)
 */
class KOO_API ButterflySymbol : public Symbol {
public:
    ButterflySymbol(double size, bool round);
    double getSize() const { return primaryDim_; }
    bool isRound() const { return type_ == SymbolType::Butterfly; }
};

/**
 * @brief Round donut symbol (donut_r<outer>x<inner>)
 */
class KOO_API RoundDonutSymbol : public Symbol {
public:
    RoundDonutSymbol(double outerDiameter, double innerDiameter);
    double getOuterDiameter() const { return primaryDim_; }
    double getInnerDiameter() const { return secondaryDim_; }
};

/**
 * @brief Square donut symbol (donut_s<outer>x<inner>)
 */
class KOO_API SquareDonutSymbol : public Symbol {
public:
    SquareDonutSymbol(double outer, double inner);
    double getOuterSize() const { return primaryDim_; }
    double getInnerSize() const { return secondaryDim_; }
};

/**
 * @brief Rectangle donut symbol (donut_rc<ow>x<oh>x<lw>)
 */
class KOO_API RectangleDonutSymbol : public Symbol {
public:
    RectangleDonutSymbol(double outerWidth, double outerHeight, double lineWidth);
    double getOuterWidth() const { return primaryDim_; }
    double getOuterHeight() const { return secondaryDim_; }
    double getLineWidth() const { return tertiaryDim_; }
};

/**
 * @brief Oval donut symbol (donut_o<ow>x<oh>x<lw>)
 */
class KOO_API OvalDonutSymbol : public Symbol {
public:
    OvalDonutSymbol(double outerWidth, double outerHeight, double lineWidth);
    double getOuterWidth() const { return primaryDim_; }
    double getOuterHeight() const { return secondaryDim_; }
    double getLineWidth() const { return tertiaryDim_; }
};

/**
 * @brief Thermal relief symbol (various types)
 */
class KOO_API ThermalSymbol : public Symbol {
public:
    /// Round thermal (thr or ths)
    ThermalSymbol(double outerDiameter, double innerDiameter,
                  double angle, int spokes, double gap, bool rounded = true);

    double getOuterDiameter() const { return primaryDim_; }
    double getInnerDiameter() const { return secondaryDim_; }
    int getSpokes() const { return spokeCount_; }
    double getGap() const { return spokeGap_; }
};

/**
 * @brief Square thermal symbol (s_ths, s_tho, s_thr)
 */
class KOO_API SquareThermalSymbol : public Symbol {
public:
    SquareThermalSymbol(double outerSize, double innerSize,
                        double angle, int spokes, double gap,
                        bool openCorners = false);
    double getOuterSize() const { return primaryDim_; }
    double getInnerSize() const { return secondaryDim_; }
};

/**
 * @brief Rectangular thermal symbol (rc_ths, rc_tho)
 */
class KOO_API RectangularThermalSymbol : public Symbol {
public:
    RectangularThermalSymbol(double width, double height,
                             double angle, int spokes, double gap, double airGap,
                             bool openCorners = false);
    double getWidth() const { return primaryDim_; }
    double getHeight() const { return secondaryDim_; }
    double getAirGap() const { return senaryDim_; }
};

/**
 * @brief Oval thermal symbol (o_ths)
 */
class KOO_API OvalThermalSymbol : public Symbol {
public:
    OvalThermalSymbol(double outerWidth, double outerHeight,
                      double angle, int spokes, double gap, double lineWidth);
    double getOuterWidth() const { return primaryDim_; }
    double getOuterHeight() const { return secondaryDim_; }
    double getLineWidth() const { return tertiaryDim_; }
};

/**
 * @brief Home plate symbol (hplate, rhplate, fhplate, etc.)
 */
class KOO_API HomePlateSymbol : public Symbol {
public:
    HomePlateSymbol(SymbolType plateType, double width, double height, double cut,
                    double radiusAcute = 0.0, double radiusObtuse = 0.0);
    double getWidth() const { return primaryDim_; }
    double getHeight() const { return secondaryDim_; }
    double getCut() const { return tertiaryDim_; }
    double getRadiusAcute() const { return quaternaryDim_; }
    double getRadiusObtuse() const { return quinaryDim_; }
};

/**
 * @brief Target symbol
 */
class KOO_API TargetSymbol : public Symbol {
public:
    TargetSymbol(double diameter, double innerWidth, double outerWidth,
                 double innerGap, double outerGap);
    double getDiameter() const { return primaryDim_; }
    double getInnerWidth() const { return secondaryDim_; }
    double getOuterWidth() const { return tertiaryDim_; }
    double getInnerGap() const { return quaternaryDim_; }
    double getOuterGap() const { return quinaryDim_; }
};

/**
 * @brief Null symbol (null<n>)
 */
class KOO_API NullSymbol : public Symbol {
public:
    explicit NullSymbol(int index);
    int getIndex() const { return static_cast<int>(primaryDim_); }
};

// ============================================================================
// Symbol Library
// ============================================================================

/**
 * @brief Symbol library manager
 *
 * Manages both standard and user-defined symbols for an ODB++ job.
 */
class KOO_API SymbolLibrary {
public:
    SymbolLibrary() = default;

    /// Get symbol by name (creates standard symbol if not found)
    Symbol* getSymbol(const std::string& name);
    const Symbol* getSymbol(const std::string& name) const;

    /// Get or create symbol by name
    /// For standard symbols, creates on demand
    /// For user-defined symbols, returns nullptr if not found
    Symbol* getOrCreateSymbol(const std::string& name);

    /// Add user-defined symbol
    void addSymbol(std::unique_ptr<Symbol> symbol);

    /// Check if symbol exists
    bool hasSymbol(const std::string& name) const;

    /// Get all symbol names
    std::vector<std::string> getSymbolNames() const;

    /// Get symbol count
    size_t getSymbolCount() const { return symbols_.size(); }

    /// Clear all symbols
    void clear();

    /// Load user-defined symbol from features file
    bool loadUserSymbol(const std::string& symbolPath);

private:
    std::unordered_map<std::string, std::unique_ptr<Symbol>> symbols_;

    /// Try to create a standard symbol from name
    std::unique_ptr<Symbol> createStandardSymbol(const std::string& name) const;
};

} // namespace koo::ecad
