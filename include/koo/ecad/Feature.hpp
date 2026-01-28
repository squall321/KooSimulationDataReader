#pragma once

#include <koo/Export.hpp>
#include <koo/ecad/Types.hpp>
#include <memory>
#include <string>
#include <vector>

namespace koo::ecad {

/**
 * @brief Base class for all ODB++ features
 *
 * Features are the basic graphic elements in ODB++ layers.
 * Types: Line, Pad, Arc, Surface, Text, Barcode
 *
 * ODB++ Feature format:
 * L <xs> <ys> <xe> <ye> <sym_num> <polarity> <dcode>;<attrs>;ID=<id>
 * P <x> <y> <apt_def> <polarity> <dcode> <orient_def>;<attrs>;ID=<id>
 * A <xs> <ys> <xe> <ye> <xc> <yc> <sym_num> <polarity> <dcode> <cw>;<attrs>;ID=<id>
 * S <polarity> <dcode>;<attrs>;ID=<id> followed by polygon data
 */
class KOO_API Feature {
public:
    virtual ~Feature() = default;

    /// Get feature type
    virtual FeatureType getType() const = 0;

    /// Get bounding box
    virtual BoundingBox2D getBoundingBox() const = 0;

    /// Clone this feature (Prototype pattern)
    virtual std::unique_ptr<Feature> clone() const = 0;

    /// Get unique identifier
    const std::string& getId() const { return id_; }
    void setId(const std::string& id) { id_ = id; }

    /// Get/set polarity
    Polarity getPolarity() const { return polarity_; }
    void setPolarity(Polarity p) { polarity_ = p; }

    /// Get/set dcode (Gerber D-code or Excellon tool number)
    int getDcode() const { return dcode_; }
    void setDcode(int dcode) { dcode_ = dcode; }

    /// Get/set net name
    const std::string& getNetName() const { return netName_; }
    void setNetName(const std::string& net) { netName_ = net; }

    /// Attributes
    const AttributeList& getAttributes() const { return attributes_; }
    void setAttribute(const std::string& key, const std::string& value) {
        attributes_[key] = value;
    }
    std::string getAttribute(const std::string& key) const {
        auto it = attributes_.find(key);
        return (it != attributes_.end()) ? it->second : "";
    }

protected:
    std::string id_;
    Polarity polarity_ = Polarity::Positive;
    int dcode_ = 0;
    std::string netName_;
    AttributeList attributes_;
};

// ============================================================================
// LineFeature
// ============================================================================

/**
 * @brief Line feature (trace/track)
 *
 * ODB++ format: L <xs> <ys> <xe> <ye> <sym_num> <polarity> <dcode>
 */
class KOO_API LineFeature : public Feature {
public:
    LineFeature() = default;
    LineFeature(double xs, double ys, double xe, double ye,
                const std::string& symbolName);

    FeatureType getType() const override { return FeatureType::Line; }
    BoundingBox2D getBoundingBox() const override;
    std::unique_ptr<Feature> clone() const override;

    /// Start point
    Point2D getStart() const { return {xs_, ys_}; }
    void setStart(double x, double y) { xs_ = x; ys_ = y; }

    /// End point
    Point2D getEnd() const { return {xe_, ye_}; }
    void setEnd(double x, double y) { xe_ = x; ye_ = y; }

    /// Symbol name (e.g., "r10" for 10mil round)
    const std::string& getSymbolName() const { return symbolName_; }
    void setSymbolName(const std::string& name) { symbolName_ = name; }

    /// Symbol index (for parsing)
    int getSymbolIndex() const { return symbolIndex_; }
    void setSymbolIndex(int idx) { symbolIndex_ = idx; }

    /// Line length
    double getLength() const;

private:
    double xs_ = 0.0, ys_ = 0.0;
    double xe_ = 0.0, ye_ = 0.0;
    std::string symbolName_;
    int symbolIndex_ = 0;
};

// ============================================================================
// PadFeature
// ============================================================================

/**
 * @brief Pad feature (aperture flash)
 *
 * ODB++ format: P <x> <y> <apt_def> <polarity> <dcode> <orient_def>
 *
 * orient_def: 0-7 for legacy 90-degree increments, 8<angle> or 9<angle> for arbitrary rotation
 */
class KOO_API PadFeature : public Feature {
public:
    PadFeature() = default;
    PadFeature(double x, double y, const std::string& symbolName,
               double rotation = 0.0, bool mirror = false);

    FeatureType getType() const override { return FeatureType::Pad; }
    BoundingBox2D getBoundingBox() const override;
    std::unique_ptr<Feature> clone() const override;

    /// Position
    Point2D getPosition() const { return {x_, y_}; }
    void setPosition(double x, double y) { x_ = x; y_ = y; }

    /// Symbol name
    const std::string& getSymbolName() const { return symbolName_; }
    void setSymbolName(const std::string& name) { symbolName_ = name; }

    /// Symbol index
    int getSymbolIndex() const { return symbolIndex_; }
    void setSymbolIndex(int idx) { symbolIndex_ = idx; }

    /// Rotation (degrees)
    double getRotation() const { return rotation_; }
    void setRotation(double angle) { rotation_ = angle; }

    /// Mirror flag
    bool isMirrored() const { return mirror_; }
    void setMirrored(bool m) { mirror_ = m; }

    /// Resize factor (optional, used when apt_def is -1)
    double getResizeFactor() const { return resizeFactor_; }
    void setResizeFactor(double factor) { resizeFactor_ = factor; }
    bool hasResize() const { return hasResize_; }
    void setHasResize(bool r) { hasResize_ = r; }

private:
    double x_ = 0.0, y_ = 0.0;
    std::string symbolName_;
    int symbolIndex_ = 0;
    double rotation_ = 0.0;
    bool mirror_ = false;
    double resizeFactor_ = 0.0;
    bool hasResize_ = false;
};

// ============================================================================
// ArcFeature
// ============================================================================

/**
 * @brief Arc feature
 *
 * ODB++ format: A <xs> <ys> <xe> <ye> <xc> <yc> <sym_num> <polarity> <dcode> <cw>
 */
class KOO_API ArcFeature : public Feature {
public:
    ArcFeature() = default;
    ArcFeature(double xs, double ys, double xe, double ye,
               double xc, double yc, const std::string& symbolName,
               bool clockwise = false);

    FeatureType getType() const override { return FeatureType::Arc; }
    BoundingBox2D getBoundingBox() const override;
    std::unique_ptr<Feature> clone() const override;

    /// Start point
    Point2D getStart() const { return {xs_, ys_}; }
    void setStart(double x, double y) { xs_ = x; ys_ = y; }

    /// End point
    Point2D getEnd() const { return {xe_, ye_}; }
    void setEnd(double x, double y) { xe_ = x; ye_ = y; }

    /// Center point
    Point2D getCenter() const { return {xc_, yc_}; }
    void setCenter(double x, double y) { xc_ = x; yc_ = y; }

    /// Symbol name
    const std::string& getSymbolName() const { return symbolName_; }
    void setSymbolName(const std::string& name) { symbolName_ = name; }

    /// Symbol index
    int getSymbolIndex() const { return symbolIndex_; }
    void setSymbolIndex(int idx) { symbolIndex_ = idx; }

    /// Clockwise direction
    bool isClockwise() const { return clockwise_; }
    void setClockwise(bool cw) { clockwise_ = cw; }

    /// Calculate radius
    double getRadius() const;

    /// Calculate arc length
    double getArcLength() const;

    /// Calculate sweep angle (radians)
    double getSweepAngle() const;

private:
    double xs_ = 0.0, ys_ = 0.0;
    double xe_ = 0.0, ye_ = 0.0;
    double xc_ = 0.0, yc_ = 0.0;
    std::string symbolName_;
    int symbolIndex_ = 0;
    bool clockwise_ = false;
};

// ============================================================================
// Contour (for Surface features)
// ============================================================================

/// Contour segment type
enum class ContourSegmentType {
    Line,   ///< OS - line segment
    Arc     ///< OC - arc segment
};

/// Single segment in a contour
struct ContourSegment {
    ContourSegmentType type = ContourSegmentType::Line;
    double x = 0.0, y = 0.0;            ///< End point (for line) or end point (for arc)
    double xc = 0.0, yc = 0.0;          ///< Center point (for arc only)
    bool clockwise = false;              ///< Arc direction (for arc only)
};

/// Polygon type
enum class PolygonType {
    Island,     ///< I - solid area
    Hole        ///< H - hole/void
};

/**
 * @brief Closed contour (polygon boundary)
 *
 * ODB++ format:
 * OB <xbs> <ybs> <poly_type>
 * OS <x> <y>
 * OC <xe> <ye> <xc> <yc> <cw>
 * OE
 */
class KOO_API Contour {
public:
    Contour() = default;
    Contour(double startX, double startY, PolygonType type = PolygonType::Island);

    /// Start point
    Point2D getStart() const { return {startX_, startY_}; }
    void setStart(double x, double y) { startX_ = x; startY_ = y; }

    /// Polygon type
    PolygonType getPolygonType() const { return polygonType_; }
    void setPolygonType(PolygonType t) { polygonType_ = t; }

    /// Add line segment
    void addLineSegment(double x, double y);

    /// Add arc segment
    void addArcSegment(double xe, double ye, double xc, double yc, bool clockwise);

    /// Get segments
    const std::vector<ContourSegment>& getSegments() const { return segments_; }

    /// Calculate area (positive for CCW, negative for CW)
    double getArea() const;

    /// Get bounding box
    BoundingBox2D getBoundingBox() const;

    /// Check if point is inside
    bool contains(double x, double y) const;

private:
    double startX_ = 0.0, startY_ = 0.0;
    PolygonType polygonType_ = PolygonType::Island;
    std::vector<ContourSegment> segments_;
};

// ============================================================================
// SurfaceFeature
// ============================================================================

/**
 * @brief Surface feature (filled polygon)
 *
 * ODB++ format:
 * S <polarity> <dcode>;<attrs>;ID=<id>
 * OB <xbs> <ybs> <poly_type>
 * OS/OC segments...
 * OE
 * SE (end of surface)
 */
class KOO_API SurfaceFeature : public Feature {
public:
    SurfaceFeature() = default;

    FeatureType getType() const override { return FeatureType::Surface; }
    BoundingBox2D getBoundingBox() const override;
    std::unique_ptr<Feature> clone() const override;

    /// Add contour
    void addContour(const Contour& contour);
    void addContour(Contour&& contour);

    /// Get contours
    const std::vector<Contour>& getContours() const { return contours_; }
    std::vector<Contour>& getContours() { return contours_; }

    /// Get outer boundary (first island contour)
    const Contour* getOuterBoundary() const;

    /// Get holes
    std::vector<const Contour*> getHoles() const;

    /// Calculate total area
    double getArea() const;

    /// Check if point is inside surface
    bool contains(double x, double y) const;

private:
    std::vector<Contour> contours_;
};

// ============================================================================
// TextFeature
// ============================================================================

/**
 * @brief Text feature
 *
 * ODB++ format:
 * T <x> <y> <font> <polarity> <orient_def> <xsize> <ysize> <width_factor> <text> <version>
 */
class KOO_API TextFeature : public Feature {
public:
    TextFeature() = default;
    TextFeature(double x, double y, const std::string& text,
                const std::string& font, double height);

    FeatureType getType() const override { return FeatureType::Text; }
    BoundingBox2D getBoundingBox() const override;
    std::unique_ptr<Feature> clone() const override;

    /// Position
    Point2D getPosition() const { return {x_, y_}; }
    void setPosition(double x, double y) { x_ = x; y_ = y; }

    /// Text content
    const std::string& getText() const { return text_; }
    void setText(const std::string& t) { text_ = t; }

    /// Font name
    const std::string& getFont() const { return font_; }
    void setFont(const std::string& f) { font_ = f; }

    /// Character size
    double getXSize() const { return xSize_; }
    double getYSize() const { return ySize_; }
    void setSize(double xSize, double ySize) { xSize_ = xSize; ySize_ = ySize; }

    /// Width factor
    double getWidthFactor() const { return widthFactor_; }
    void setWidthFactor(double wf) { widthFactor_ = wf; }

    /// Rotation (degrees)
    double getRotation() const { return rotation_; }
    void setRotation(double angle) { rotation_ = angle; }

    /// Mirror flag
    bool isMirrored() const { return mirror_; }
    void setMirrored(bool m) { mirror_ = m; }

    /// Version (placement mode)
    int getVersion() const { return version_; }
    void setVersion(int v) { version_ = v; }

private:
    double x_ = 0.0, y_ = 0.0;
    std::string text_;
    std::string font_ = "standard";
    double xSize_ = 1.0, ySize_ = 1.0;
    double widthFactor_ = 1.0;
    double rotation_ = 0.0;
    bool mirror_ = false;
    int version_ = 0;
};

// ============================================================================
// BarcodeFeature
// ============================================================================

/**
 * @brief Barcode feature
 *
 * ODB++ format:
 * B <x> <y> <barcode> <font> <polarity> <orient_def> E <w> <h> <fasc> <cs> <bg> <astr> <astr_pos> <text>
 */
class KOO_API BarcodeFeature : public Feature {
public:
    BarcodeFeature() = default;

    FeatureType getType() const override { return FeatureType::Barcode; }
    BoundingBox2D getBoundingBox() const override;
    std::unique_ptr<Feature> clone() const override;

    /// Position
    Point2D getPosition() const { return {x_, y_}; }
    void setPosition(double x, double y) { x_ = x; y_ = y; }

    /// Barcode type (e.g., "UPC39")
    const std::string& getBarcodeType() const { return barcodeType_; }
    void setBarcodeType(const std::string& type) { barcodeType_ = type; }

    /// Font name
    const std::string& getFont() const { return font_; }
    void setFont(const std::string& f) { font_ = f; }

    /// Rotation (degrees)
    double getRotation() const { return rotation_; }
    void setRotation(double angle) { rotation_ = angle; }

    /// Mirror flag
    bool isMirrored() const { return mirror_; }
    void setMirrored(bool m) { mirror_ = m; }

    /// Element width
    double getElementWidth() const { return elementWidth_; }
    void setElementWidth(double w) { elementWidth_ = w; }

    /// Barcode height
    double getHeight() const { return height_; }
    void setHeight(double h) { height_ = h; }

    /// Full ASCII mode
    bool isFullAscii() const { return fullAscii_; }
    void setFullAscii(bool fa) { fullAscii_ = fa; }

    /// Checksum enabled
    bool hasChecksum() const { return checksum_; }
    void setChecksum(bool cs) { checksum_ = cs; }

    /// Inverted background
    bool hasInvertedBackground() const { return invertedBackground_; }
    void setInvertedBackground(bool inv) { invertedBackground_ = inv; }

    /// Additional string
    bool hasAdditionalString() const { return hasAdditionalString_; }
    void setHasAdditionalString(bool has) { hasAdditionalString_ = has; }

    /// String position (true = top, false = bottom)
    bool isStringOnTop() const { return stringOnTop_; }
    void setStringOnTop(bool top) { stringOnTop_ = top; }

    /// Text content
    const std::string& getText() const { return text_; }
    void setText(const std::string& t) { text_ = t; }

private:
    double x_ = 0.0, y_ = 0.0;
    std::string barcodeType_ = "UPC39";
    std::string font_ = "standard";
    double rotation_ = 0.0;
    bool mirror_ = false;
    double elementWidth_ = 0.008;
    double height_ = 0.2;
    bool fullAscii_ = false;
    bool checksum_ = false;
    bool invertedBackground_ = false;
    bool hasAdditionalString_ = true;
    bool stringOnTop_ = true;
    std::string text_;
};

} // namespace koo::ecad
