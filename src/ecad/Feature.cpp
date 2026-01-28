#include <koo/ecad/Feature.hpp>
#include <cmath>
#include <algorithm>

namespace koo::ecad {

// ============================================================================
// LineFeature
// ============================================================================

LineFeature::LineFeature(double xs, double ys, double xe, double ye,
                         const std::string& symbolName)
    : xs_(xs), ys_(ys), xe_(xe), ye_(ye), symbolName_(symbolName) {}

BoundingBox2D LineFeature::getBoundingBox() const {
    BoundingBox2D box;
    box.expand({xs_, ys_});
    box.expand({xe_, ye_});
    return box;
}

std::unique_ptr<Feature> LineFeature::clone() const {
    return std::make_unique<LineFeature>(*this);
}

double LineFeature::getLength() const {
    double dx = xe_ - xs_;
    double dy = ye_ - ys_;
    return std::sqrt(dx * dx + dy * dy);
}

// ============================================================================
// PadFeature
// ============================================================================

PadFeature::PadFeature(double x, double y, const std::string& symbolName,
                       double rotation, bool mirror)
    : x_(x), y_(y), symbolName_(symbolName), rotation_(rotation), mirror_(mirror) {}

BoundingBox2D PadFeature::getBoundingBox() const {
    // For now, return a point bounding box
    // Proper implementation would need symbol dimensions
    BoundingBox2D box;
    box.expand({x_, y_});
    return box;
}

std::unique_ptr<Feature> PadFeature::clone() const {
    return std::make_unique<PadFeature>(*this);
}

// ============================================================================
// ArcFeature
// ============================================================================

ArcFeature::ArcFeature(double xs, double ys, double xe, double ye,
                       double xc, double yc, const std::string& symbolName,
                       bool clockwise)
    : xs_(xs), ys_(ys), xe_(xe), ye_(ye), xc_(xc), yc_(yc),
      symbolName_(symbolName), clockwise_(clockwise) {}

BoundingBox2D ArcFeature::getBoundingBox() const {
    BoundingBox2D box;
    box.expand({xs_, ys_});
    box.expand({xe_, ye_});

    // Also check for extrema at 0, 90, 180, 270 degrees
    double radius = getRadius();
    double startAngle = std::atan2(ys_ - yc_, xs_ - xc_);
    double endAngle = std::atan2(ye_ - yc_, xe_ - xc_);

    // Normalize angles
    if (startAngle < 0) startAngle += 2.0 * M_PI;
    if (endAngle < 0) endAngle += 2.0 * M_PI;

    // Check cardinal directions
    auto checkAngle = [&](double angle) {
        bool inArc = false;
        if (clockwise_) {
            inArc = (startAngle >= angle && angle >= endAngle) ||
                    (startAngle < endAngle && (angle <= startAngle || angle >= endAngle));
        } else {
            inArc = (startAngle <= angle && angle <= endAngle) ||
                    (startAngle > endAngle && (angle >= startAngle || angle <= endAngle));
        }
        if (inArc) {
            box.expand({xc_ + radius * std::cos(angle), yc_ + radius * std::sin(angle)});
        }
    };

    checkAngle(0.0);
    checkAngle(M_PI / 2.0);
    checkAngle(M_PI);
    checkAngle(3.0 * M_PI / 2.0);

    return box;
}

std::unique_ptr<Feature> ArcFeature::clone() const {
    return std::make_unique<ArcFeature>(*this);
}

double ArcFeature::getRadius() const {
    double dx = xs_ - xc_;
    double dy = ys_ - yc_;
    return std::sqrt(dx * dx + dy * dy);
}

double ArcFeature::getSweepAngle() const {
    double startAngle = std::atan2(ys_ - yc_, xs_ - xc_);
    double endAngle = std::atan2(ye_ - yc_, xe_ - xc_);

    double sweep = endAngle - startAngle;

    if (clockwise_) {
        if (sweep > 0) sweep -= 2.0 * M_PI;
    } else {
        if (sweep < 0) sweep += 2.0 * M_PI;
    }

    return std::abs(sweep);
}

double ArcFeature::getArcLength() const {
    return getRadius() * getSweepAngle();
}

// ============================================================================
// Contour
// ============================================================================

Contour::Contour(double startX, double startY, PolygonType type)
    : startX_(startX), startY_(startY), polygonType_(type) {}

void Contour::addLineSegment(double x, double y) {
    ContourSegment seg;
    seg.type = ContourSegmentType::Line;
    seg.x = x;
    seg.y = y;
    segments_.push_back(seg);
}

void Contour::addArcSegment(double xe, double ye, double xc, double yc, bool clockwise) {
    ContourSegment seg;
    seg.type = ContourSegmentType::Arc;
    seg.x = xe;
    seg.y = ye;
    seg.xc = xc;
    seg.yc = yc;
    seg.clockwise = clockwise;
    segments_.push_back(seg);
}

double Contour::getArea() const {
    if (segments_.empty()) return 0.0;

    double area = 0.0;
    double prevX = startX_;
    double prevY = startY_;

    for (const auto& seg : segments_) {
        if (seg.type == ContourSegmentType::Line) {
            // Shoelace formula
            area += (prevX * seg.y - seg.x * prevY);
            prevX = seg.x;
            prevY = seg.y;
        } else {
            // Arc segment - approximate with line for now
            // TODO: proper arc area calculation
            area += (prevX * seg.y - seg.x * prevY);
            prevX = seg.x;
            prevY = seg.y;
        }
    }

    // Close the polygon
    area += (prevX * startY_ - startX_ * prevY);

    return area / 2.0;
}

BoundingBox2D Contour::getBoundingBox() const {
    BoundingBox2D box;
    box.expand({startX_, startY_});

    for (const auto& seg : segments_) {
        box.expand({seg.x, seg.y});
        if (seg.type == ContourSegmentType::Arc) {
            // For arcs, also check the center point and cardinal directions
            // (simplified - should check if cardinal points are within arc)
            double radius = std::sqrt((seg.x - seg.xc) * (seg.x - seg.xc) +
                                      (seg.y - seg.yc) * (seg.y - seg.yc));
            box.expand({seg.xc - radius, seg.yc - radius});
            box.expand({seg.xc + radius, seg.yc + radius});
        }
    }

    return box;
}

bool Contour::contains(double x, double y) const {
    // Ray casting algorithm (simplified for lines only)
    if (segments_.empty()) return false;

    int crossings = 0;
    double prevX = startX_;
    double prevY = startY_;

    for (const auto& seg : segments_) {
        double x1 = prevX, y1 = prevY;
        double x2 = seg.x, y2 = seg.y;

        if (((y1 <= y && y < y2) || (y2 <= y && y < y1)) &&
            (x < (x2 - x1) * (y - y1) / (y2 - y1) + x1)) {
            crossings++;
        }

        prevX = seg.x;
        prevY = seg.y;
    }

    // Close to start
    double x1 = prevX, y1 = prevY;
    double x2 = startX_, y2 = startY_;
    if (((y1 <= y && y < y2) || (y2 <= y && y < y1)) &&
        (x < (x2 - x1) * (y - y1) / (y2 - y1) + x1)) {
        crossings++;
    }

    return (crossings % 2) == 1;
}

// ============================================================================
// SurfaceFeature
// ============================================================================

BoundingBox2D SurfaceFeature::getBoundingBox() const {
    BoundingBox2D box;
    for (const auto& contour : contours_) {
        box.expand(contour.getBoundingBox());
    }
    return box;
}

std::unique_ptr<Feature> SurfaceFeature::clone() const {
    return std::make_unique<SurfaceFeature>(*this);
}

void SurfaceFeature::addContour(const Contour& contour) {
    contours_.push_back(contour);
}

void SurfaceFeature::addContour(Contour&& contour) {
    contours_.push_back(std::move(contour));
}

const Contour* SurfaceFeature::getOuterBoundary() const {
    for (const auto& contour : contours_) {
        if (contour.getPolygonType() == PolygonType::Island) {
            return &contour;
        }
    }
    return nullptr;
}

std::vector<const Contour*> SurfaceFeature::getHoles() const {
    std::vector<const Contour*> holes;
    for (const auto& contour : contours_) {
        if (contour.getPolygonType() == PolygonType::Hole) {
            holes.push_back(&contour);
        }
    }
    return holes;
}

double SurfaceFeature::getArea() const {
    double area = 0.0;
    for (const auto& contour : contours_) {
        double contourArea = std::abs(contour.getArea());
        if (contour.getPolygonType() == PolygonType::Hole) {
            area -= contourArea;
        } else {
            area += contourArea;
        }
    }
    return area;
}

bool SurfaceFeature::contains(double x, double y) const {
    // First check if inside any island
    bool insideIsland = false;
    for (const auto& contour : contours_) {
        if (contour.getPolygonType() == PolygonType::Island && contour.contains(x, y)) {
            insideIsland = true;
            break;
        }
    }

    if (!insideIsland) return false;

    // Check if inside any hole
    for (const auto& contour : contours_) {
        if (contour.getPolygonType() == PolygonType::Hole && contour.contains(x, y)) {
            return false;
        }
    }

    return true;
}

// ============================================================================
// TextFeature
// ============================================================================

TextFeature::TextFeature(double x, double y, const std::string& text,
                         const std::string& font, double height)
    : x_(x), y_(y), text_(text), font_(font), xSize_(height), ySize_(height) {}

BoundingBox2D TextFeature::getBoundingBox() const {
    // Approximate bounding box based on text length and size
    double width = static_cast<double>(text_.length()) * xSize_;
    double height = ySize_;

    BoundingBox2D box;
    box.expand({x_, y_});
    box.expand({x_ + width, y_ + height});
    return box;
}

std::unique_ptr<Feature> TextFeature::clone() const {
    return std::make_unique<TextFeature>(*this);
}

// ============================================================================
// BarcodeFeature
// ============================================================================

BoundingBox2D BarcodeFeature::getBoundingBox() const {
    // Approximate bounding box
    double width = static_cast<double>(text_.length()) * elementWidth_ * 10;  // rough estimate
    BoundingBox2D box;
    box.expand({x_, y_});
    box.expand({x_ + width, y_ + height_});
    return box;
}

std::unique_ptr<Feature> BarcodeFeature::clone() const {
    return std::make_unique<BarcodeFeature>(*this);
}

} // namespace koo::ecad
