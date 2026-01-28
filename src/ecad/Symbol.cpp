#include <koo/ecad/Symbol.hpp>
#include <regex>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <cmath>

namespace koo::ecad {

// ============================================================================
// Symbol
// ============================================================================

Symbol::Symbol(const std::string& name) : name_(name) {
    // Try to determine if this is a standard symbol
    auto parsed = parseStandardSymbol(name);
    if (parsed) {
        type_ = parsed->type_;
        isStandard_ = true;
        primaryDim_ = parsed->primaryDim_;
        secondaryDim_ = parsed->secondaryDim_;
        tertiaryDim_ = parsed->tertiaryDim_;
        quaternaryDim_ = parsed->quaternaryDim_;
        quinaryDim_ = parsed->quinaryDim_;
        senaryDim_ = parsed->senaryDim_;
        spokeCount_ = parsed->spokeCount_;
        spokeGap_ = parsed->spokeGap_;
        angle_ = parsed->angle_;
        cornerRadius_ = parsed->cornerRadius_;
        corners_ = parsed->corners_;
        unit_ = parsed->unit_;
        boundingBox_ = parsed->boundingBox_;
    }
}

void Symbol::addFeature(std::unique_ptr<Feature> feature) {
    features_.push_back(std::move(feature));
}

void Symbol::clearFeatures() {
    features_.clear();
}

std::unique_ptr<Symbol> Symbol::clone() const {
    auto copy = std::make_unique<Symbol>(name_);
    copy->type_ = type_;
    copy->isStandard_ = isStandard_;
    copy->boundingBox_ = boundingBox_;
    copy->attributes_ = attributes_;
    copy->primaryDim_ = primaryDim_;
    copy->secondaryDim_ = secondaryDim_;
    copy->tertiaryDim_ = tertiaryDim_;
    copy->quaternaryDim_ = quaternaryDim_;
    copy->quinaryDim_ = quinaryDim_;
    copy->senaryDim_ = senaryDim_;
    copy->spokeCount_ = spokeCount_;
    copy->spokeGap_ = spokeGap_;
    copy->angle_ = angle_;
    copy->cornerRadius_ = cornerRadius_;
    copy->corners_ = corners_;
    copy->unit_ = unit_;

    for (const auto& f : features_) {
        copy->features_.push_back(f->clone());
    }

    return copy;
}

bool Symbol::isStandardSymbolName(const std::string& name) {
    return parseStandardSymbol(name) != nullptr;
}

// Helper function to parse numeric value (supporting decimal)
static double parseNum(const std::string& s) {
    return std::stod(s);
}

// Helper to compute bounding box
static BoundingBox2D makeBBox(double w, double h) {
    return BoundingBox2D({-w / 2.0, -h / 2.0}, {w / 2.0, h / 2.0});
}

static BoundingBox2D makeBBoxFromRadius(double r) {
    return BoundingBox2D({-r, -r}, {r, r});
}

std::unique_ptr<Symbol> Symbol::parseStandardSymbol(const std::string& name) {
    if (name.empty()) return nullptr;

    std::smatch match;

    // ========================================================================
    // Basic shapes
    // ========================================================================

    // Round: r<size>
    static const std::regex roundRegex(R"(^r(\d+\.?\d*)$)", std::regex::icase);
    if (std::regex_match(name, match, roundRegex)) {
        double size = parseNum(match[1].str());
        return std::make_unique<RoundSymbol>(size);
    }

    // Square: s<size>
    static const std::regex squareRegex(R"(^s(\d+\.?\d*)$)", std::regex::icase);
    if (std::regex_match(name, match, squareRegex)) {
        double size = parseNum(match[1].str());
        return std::make_unique<SquareSymbol>(size);
    }

    // Rounded rectangle: rect<w>x<h>xr<rad>[x<corners>]
    static const std::regex roundedRectRegex(R"(^rect(\d+\.?\d*)x(\d+\.?\d*)xr(\d+\.?\d*)(?:x(\d+))?$)", std::regex::icase);
    if (std::regex_match(name, match, roundedRectRegex)) {
        double width = parseNum(match[1].str());
        double height = parseNum(match[2].str());
        double radius = parseNum(match[3].str());
        uint8_t corners = 0xFF;
        if (match[4].matched) {
            corners = static_cast<uint8_t>(std::stoi(match[4].str()));
        }
        return std::make_unique<RoundedRectangleSymbol>(width, height, radius, corners);
    }

    // Chamfered rectangle: rect<w>x<h>xc<chamfer>[x<corners>]
    static const std::regex chamferedRectRegex(R"(^rect(\d+\.?\d*)x(\d+\.?\d*)xc(\d+\.?\d*)(?:x(\d+))?$)", std::regex::icase);
    if (std::regex_match(name, match, chamferedRectRegex)) {
        double width = parseNum(match[1].str());
        double height = parseNum(match[2].str());
        double chamfer = parseNum(match[3].str());
        uint8_t corners = 0xFF;
        if (match[4].matched) {
            corners = static_cast<uint8_t>(std::stoi(match[4].str()));
        }
        return std::make_unique<ChamferedRectangleSymbol>(width, height, chamfer, corners);
    }

    // Rectangle: rect<width>x<height>
    static const std::regex rectRegex(R"(^rect(\d+\.?\d*)x(\d+\.?\d*)$)", std::regex::icase);
    if (std::regex_match(name, match, rectRegex)) {
        double width = parseNum(match[1].str());
        double height = parseNum(match[2].str());
        return std::make_unique<RectangleSymbol>(width, height);
    }

    // Half oval: oval_h<w>x<h>
    static const std::regex halfOvalRegex(R"(^oval_h(\d+\.?\d*)x(\d+\.?\d*)$)", std::regex::icase);
    if (std::regex_match(name, match, halfOvalRegex)) {
        double width = parseNum(match[1].str());
        double height = parseNum(match[2].str());
        return std::make_unique<HalfOvalSymbol>(width, height);
    }

    // Oblong/Oval: oval<width>x<height>
    static const std::regex ovalRegex(R"(^oval(\d+\.?\d*)x(\d+\.?\d*)$)", std::regex::icase);
    if (std::regex_match(name, match, ovalRegex)) {
        double width = parseNum(match[1].str());
        double height = parseNum(match[2].str());
        return std::make_unique<OblongSymbol>(width, height);
    }

    // Diamond: di<w>x<h> (two params) or di<s> (single param, square diamond)
    static const std::regex diamond2Regex(R"(^di(\d+\.?\d*)x(\d+\.?\d*)$)", std::regex::icase);
    if (std::regex_match(name, match, diamond2Regex)) {
        double width = parseNum(match[1].str());
        double height = parseNum(match[2].str());
        return std::make_unique<DiamondSymbol>(width, height);
    }

    static const std::regex diamond1Regex(R"(^di(\d+\.?\d*)$)", std::regex::icase);
    if (std::regex_match(name, match, diamond1Regex)) {
        double size = parseNum(match[1].str());
        return std::make_unique<DiamondSymbol>(size, size);
    }

    // Octagon: oct<w>x<h>x<corner> or oct<s>x<corner>
    static const std::regex oct3Regex(R"(^oct(\d+\.?\d*)x(\d+\.?\d*)x(\d+\.?\d*)$)", std::regex::icase);
    if (std::regex_match(name, match, oct3Regex)) {
        double width = parseNum(match[1].str());
        double height = parseNum(match[2].str());
        double corner = parseNum(match[3].str());
        return std::make_unique<OctagonSymbol>(width, height, corner);
    }

    static const std::regex oct2Regex(R"(^oct(\d+\.?\d*)x(\d+\.?\d*)$)", std::regex::icase);
    if (std::regex_match(name, match, oct2Regex)) {
        double size = parseNum(match[1].str());
        double corner = parseNum(match[2].str());
        return std::make_unique<OctagonSymbol>(size, size, corner);
    }

    // Triangle: tri<base>x<h>
    static const std::regex triRegex(R"(^tri(\d+\.?\d*)x(\d+\.?\d*)$)", std::regex::icase);
    if (std::regex_match(name, match, triRegex)) {
        double base = parseNum(match[1].str());
        double height = parseNum(match[2].str());
        return std::make_unique<TriangleSymbol>(base, height);
    }

    // ========================================================================
    // Hexagons
    // ========================================================================

    // Horizontal hexagon: hex_l<w>x<h>x<r>
    static const std::regex hexLRegex(R"(^hex_l(\d+\.?\d*)x(\d+\.?\d*)x(\d+\.?\d*)$)", std::regex::icase);
    if (std::regex_match(name, match, hexLRegex)) {
        double width = parseNum(match[1].str());
        double height = parseNum(match[2].str());
        double corner = parseNum(match[3].str());
        return std::make_unique<HexagonSymbol>(width, height, corner, true);
    }

    // Vertical hexagon: hex_s<w>x<h>x<r>
    static const std::regex hexSRegex(R"(^hex_s(\d+\.?\d*)x(\d+\.?\d*)x(\d+\.?\d*)$)", std::regex::icase);
    if (std::regex_match(name, match, hexSRegex)) {
        double width = parseNum(match[1].str());
        double height = parseNum(match[2].str());
        double corner = parseNum(match[3].str());
        return std::make_unique<HexagonSymbol>(width, height, corner, false);
    }

    // ========================================================================
    // Butterflies
    // ========================================================================

    // Round butterfly: bfr<d>
    static const std::regex bfrRegex(R"(^bfr(\d+\.?\d*)$)", std::regex::icase);
    if (std::regex_match(name, match, bfrRegex)) {
        double size = parseNum(match[1].str());
        return std::make_unique<ButterflySymbol>(size, true);
    }

    // Square butterfly: bfs<s>
    static const std::regex bfsRegex(R"(^bfs(\d+\.?\d*)$)", std::regex::icase);
    if (std::regex_match(name, match, bfsRegex)) {
        double size = parseNum(match[1].str());
        return std::make_unique<ButterflySymbol>(size, false);
    }

    // ========================================================================
    // Donuts
    // ========================================================================

    // Round donut: donut_r<outer>x<inner>
    static const std::regex donutRRegex(R"(^donut_r(\d+\.?\d*)x(\d+\.?\d*)$)", std::regex::icase);
    if (std::regex_match(name, match, donutRRegex)) {
        double outer = parseNum(match[1].str());
        double inner = parseNum(match[2].str());
        return std::make_unique<RoundDonutSymbol>(outer, inner);
    }

    // Square donut: donut_s<outer>x<inner>
    static const std::regex donutSRegex(R"(^donut_s(\d+\.?\d*)x(\d+\.?\d*)$)", std::regex::icase);
    if (std::regex_match(name, match, donutSRegex)) {
        double outer = parseNum(match[1].str());
        double inner = parseNum(match[2].str());
        return std::make_unique<SquareDonutSymbol>(outer, inner);
    }

    // Square-round donut: donut_sr<outer>x<inner>
    static const std::regex donutSRRegex(R"(^donut_sr(\d+\.?\d*)x(\d+\.?\d*)$)", std::regex::icase);
    if (std::regex_match(name, match, donutSRRegex)) {
        double outer = parseNum(match[1].str());
        double inner = parseNum(match[2].str());
        auto sym = std::make_unique<Symbol>(name);
        sym->type_ = SymbolType::SquareRoundDonut;
        sym->isStandard_ = true;
        sym->primaryDim_ = outer;
        sym->secondaryDim_ = inner;
        sym->boundingBox_ = makeBBoxFromRadius(outer / 2.0);
        return sym;
    }

    // Rectangle donut: donut_rc<ow>x<oh>x<lw>[xr<rad>]
    static const std::regex donutRCRadRegex(R"(^donut_rc(\d+\.?\d*)x(\d+\.?\d*)x(\d+\.?\d*)xr(\d+\.?\d*)$)", std::regex::icase);
    if (std::regex_match(name, match, donutRCRadRegex)) {
        double ow = parseNum(match[1].str());
        double oh = parseNum(match[2].str());
        double lw = parseNum(match[3].str());
        double rad = parseNum(match[4].str());
        auto sym = std::make_unique<RectangleDonutSymbol>(ow, oh, lw);
        sym->setType(SymbolType::RoundedRectDonut);
        sym->setCornerRadius(rad);
        return sym;
    }

    static const std::regex donutRCRegex(R"(^donut_rc(\d+\.?\d*)x(\d+\.?\d*)x(\d+\.?\d*)$)", std::regex::icase);
    if (std::regex_match(name, match, donutRCRegex)) {
        double ow = parseNum(match[1].str());
        double oh = parseNum(match[2].str());
        double lw = parseNum(match[3].str());
        return std::make_unique<RectangleDonutSymbol>(ow, oh, lw);
    }

    // Oval donut: donut_o<ow>x<oh>x<lw>
    static const std::regex donutORegex(R"(^donut_o(\d+\.?\d*)x(\d+\.?\d*)x(\d+\.?\d*)$)", std::regex::icase);
    if (std::regex_match(name, match, donutORegex)) {
        double ow = parseNum(match[1].str());
        double oh = parseNum(match[2].str());
        double lw = parseNum(match[3].str());
        return std::make_unique<OvalDonutSymbol>(ow, oh, lw);
    }

    // ========================================================================
    // Thermals
    // ========================================================================

    // Round thermal rounded: thr<od>x<id>x<angle>x<spokes>x<gap>
    static const std::regex thrRegex(R"(^thr(\d+\.?\d*)x(\d+\.?\d*)x(\d+\.?\d*)x(\d+)x(\d+\.?\d*)$)", std::regex::icase);
    if (std::regex_match(name, match, thrRegex)) {
        double od = parseNum(match[1].str());
        double id = parseNum(match[2].str());
        double angle = parseNum(match[3].str());
        int spokes = std::stoi(match[4].str());
        double gap = parseNum(match[5].str());
        return std::make_unique<ThermalSymbol>(od, id, angle, spokes, gap, true);
    }

    // Round thermal squared: ths<od>x<id>x<angle>x<spokes>x<gap>
    static const std::regex thsRegex(R"(^ths(\d+\.?\d*)x(\d+\.?\d*)x(\d+\.?\d*)x(\d+)x(\d+\.?\d*)$)", std::regex::icase);
    if (std::regex_match(name, match, thsRegex)) {
        double od = parseNum(match[1].str());
        double id = parseNum(match[2].str());
        double angle = parseNum(match[3].str());
        int spokes = std::stoi(match[4].str());
        double gap = parseNum(match[5].str());
        return std::make_unique<ThermalSymbol>(od, id, angle, spokes, gap, false);
    }

    // Legacy thermal: th<od>x<id>x<spokes>x<gap> (4 params, no angle)
    static const std::regex thLegacyRegex(R"(^th[r]?(\d+\.?\d*)x(\d+\.?\d*)x(\d+)x(\d+\.?\d*)$)", std::regex::icase);
    if (std::regex_match(name, match, thLegacyRegex)) {
        double od = parseNum(match[1].str());
        double id = parseNum(match[2].str());
        int spokes = std::stoi(match[3].str());
        double gap = parseNum(match[4].str());
        return std::make_unique<ThermalSymbol>(od, id, 0.0, spokes, gap, true);
    }

    // Square thermal: s_ths<os>x<is>x<angle>x<spokes>x<gap>
    static const std::regex sThsRegex(R"(^s_ths(\d+\.?\d*)x(\d+\.?\d*)x(\d+\.?\d*)x(\d+)x(\d+\.?\d*)$)", std::regex::icase);
    if (std::regex_match(name, match, sThsRegex)) {
        double os = parseNum(match[1].str());
        double is = parseNum(match[2].str());
        double angle = parseNum(match[3].str());
        int spokes = std::stoi(match[4].str());
        double gap = parseNum(match[5].str());
        return std::make_unique<SquareThermalSymbol>(os, is, angle, spokes, gap, false);
    }

    // Square thermal open corners: s_tho<os>x<is>x<angle>x<spokes>x<gap>
    static const std::regex sThoRegex(R"(^s_tho(\d+\.?\d*)x(\d+\.?\d*)x(\d+\.?\d*)x(\d+)x(\d+\.?\d*)$)", std::regex::icase);
    if (std::regex_match(name, match, sThoRegex)) {
        double os = parseNum(match[1].str());
        double is = parseNum(match[2].str());
        double angle = parseNum(match[3].str());
        int spokes = std::stoi(match[4].str());
        double gap = parseNum(match[5].str());
        return std::make_unique<SquareThermalSymbol>(os, is, angle, spokes, gap, true);
    }

    // Line thermal: s_thr<os>x<is>x<angle>x<spokes>x<gap>
    static const std::regex sThrRegex(R"(^s_thr(\d+\.?\d*)x(\d+\.?\d*)x(\d+\.?\d*)x(\d+)x(\d+\.?\d*)$)", std::regex::icase);
    if (std::regex_match(name, match, sThrRegex)) {
        double os = parseNum(match[1].str());
        double is = parseNum(match[2].str());
        double angle = parseNum(match[3].str());
        int spokes = std::stoi(match[4].str());
        double gap = parseNum(match[5].str());
        auto sym = std::make_unique<SquareThermalSymbol>(os, is, angle, spokes, gap, false);
        sym->setType(SymbolType::LineThermal);
        return sym;
    }

    // Square-round thermal: sr_ths<os>x<id>x<angle>x<spokes>x<gap>
    static const std::regex srThsRegex(R"(^sr_ths(\d+\.?\d*)x(\d+\.?\d*)x(\d+\.?\d*)x(\d+)x(\d+\.?\d*)$)", std::regex::icase);
    if (std::regex_match(name, match, srThsRegex)) {
        double os = parseNum(match[1].str());
        double id = parseNum(match[2].str());
        double angle = parseNum(match[3].str());
        int spokes = std::stoi(match[4].str());
        double gap = parseNum(match[5].str());
        auto sym = std::make_unique<SquareThermalSymbol>(os, id, angle, spokes, gap, false);
        sym->setType(SymbolType::SquareRoundThermal);
        return sym;
    }

    // Rectangular thermal: rc_ths<w>x<h>x<angle>x<spokes>x<gap>x<air_gap>
    static const std::regex rcThsRegex(R"(^rc_ths(\d+\.?\d*)x(\d+\.?\d*)x(\d+\.?\d*)x(\d+)x(\d+\.?\d*)x(\d+\.?\d*)$)", std::regex::icase);
    if (std::regex_match(name, match, rcThsRegex)) {
        double w = parseNum(match[1].str());
        double h = parseNum(match[2].str());
        double angle = parseNum(match[3].str());
        int spokes = std::stoi(match[4].str());
        double gap = parseNum(match[5].str());
        double airGap = parseNum(match[6].str());
        return std::make_unique<RectangularThermalSymbol>(w, h, angle, spokes, gap, airGap, false);
    }

    // Rectangular thermal open corners: rc_tho<w>x<h>x<angle>x<spokes>x<gap>x<air_gap>
    static const std::regex rcThoRegex(R"(^rc_tho(\d+\.?\d*)x(\d+\.?\d*)x(\d+\.?\d*)x(\d+)x(\d+\.?\d*)x(\d+\.?\d*)$)", std::regex::icase);
    if (std::regex_match(name, match, rcThoRegex)) {
        double w = parseNum(match[1].str());
        double h = parseNum(match[2].str());
        double angle = parseNum(match[3].str());
        int spokes = std::stoi(match[4].str());
        double gap = parseNum(match[5].str());
        double airGap = parseNum(match[6].str());
        return std::make_unique<RectangularThermalSymbol>(w, h, angle, spokes, gap, airGap, true);
    }

    // Oval thermal: o_ths<ow>x<oh>x<angle>x<spokes>x<gap>x<lw>
    static const std::regex oThsRegex(R"(^o_ths(\d+\.?\d*)x(\d+\.?\d*)x(\d+\.?\d*)x(\d+)x(\d+\.?\d*)x(\d+\.?\d*)$)", std::regex::icase);
    if (std::regex_match(name, match, oThsRegex)) {
        double ow = parseNum(match[1].str());
        double oh = parseNum(match[2].str());
        double angle = parseNum(match[3].str());
        int spokes = std::stoi(match[4].str());
        double gap = parseNum(match[5].str());
        double lw = parseNum(match[6].str());
        return std::make_unique<OvalThermalSymbol>(ow, oh, angle, spokes, gap, lw);
    }

    // Oblong thermal: oblong_ths<ow>x<oh>x<angle>x<spokes>x<gap>x<lw>
    static const std::regex oblongThsRegex(R"(^oblong_ths(\d+\.?\d*)x(\d+\.?\d*)x(\d+\.?\d*)x(\d+)x(\d+\.?\d*)x(\d+\.?\d*)$)", std::regex::icase);
    if (std::regex_match(name, match, oblongThsRegex)) {
        double ow = parseNum(match[1].str());
        double oh = parseNum(match[2].str());
        double angle = parseNum(match[3].str());
        int spokes = std::stoi(match[4].str());
        double gap = parseNum(match[5].str());
        double lw = parseNum(match[6].str());
        auto sym = std::make_unique<OvalThermalSymbol>(ow, oh, angle, spokes, gap, lw);
        sym->setType(SymbolType::OblongThermal);
        return sym;
    }

    // ========================================================================
    // Solder stencil symbols (home plates)
    // ========================================================================

    // Home plate: hplate<w>x<h>x<c>
    static const std::regex hplateRegex(R"(^hplate(\d+\.?\d*)x(\d+\.?\d*)x(\d+\.?\d*)$)", std::regex::icase);
    if (std::regex_match(name, match, hplateRegex)) {
        double w = parseNum(match[1].str());
        double h = parseNum(match[2].str());
        double c = parseNum(match[3].str());
        return std::make_unique<HomePlateSymbol>(SymbolType::HomePlate, w, h, c);
    }

    // Inverted home plate: rhplate<w>x<h>x<c>
    static const std::regex rhplateRegex(R"(^rhplate(\d+\.?\d*)x(\d+\.?\d*)x(\d+\.?\d*)$)", std::regex::icase);
    if (std::regex_match(name, match, rhplateRegex)) {
        double w = parseNum(match[1].str());
        double h = parseNum(match[2].str());
        double c = parseNum(match[3].str());
        return std::make_unique<HomePlateSymbol>(SymbolType::InvertedHomePlate, w, h, c);
    }

    // Flat home plate: fhplate<w>x<h>x<vc>x<hc>
    static const std::regex fhplateRegex(R"(^fhplate(\d+\.?\d*)x(\d+\.?\d*)x(\d+\.?\d*)x(\d+\.?\d*)$)", std::regex::icase);
    if (std::regex_match(name, match, fhplateRegex)) {
        double w = parseNum(match[1].str());
        double h = parseNum(match[2].str());
        double vc = parseNum(match[3].str());
        double hc = parseNum(match[4].str());
        auto sym = std::make_unique<HomePlateSymbol>(SymbolType::FlatHomePlate, w, h, vc);
        sym->setQuaternaryDimension(hc);
        return sym;
    }

    // Radiused inverted home plate: radhplate<w>x<h>x<ms>
    static const std::regex radhplateRegex(R"(^radhplate(\d+\.?\d*)x(\d+\.?\d*)x(\d+\.?\d*)$)", std::regex::icase);
    if (std::regex_match(name, match, radhplateRegex)) {
        double w = parseNum(match[1].str());
        double h = parseNum(match[2].str());
        double ms = parseNum(match[3].str());
        return std::make_unique<HomePlateSymbol>(SymbolType::RadiusedInvHomePlate, w, h, ms);
    }

    // Radiused home plate: radplate<w>x<h>x<ms>
    static const std::regex radplateRegex(R"(^radplate(\d+\.?\d*)x(\d+\.?\d*)x(\d+\.?\d*)$)", std::regex::icase);
    if (std::regex_match(name, match, radplateRegex)) {
        double w = parseNum(match[1].str());
        double h = parseNum(match[2].str());
        double ms = parseNum(match[3].str());
        return std::make_unique<HomePlateSymbol>(SymbolType::RadiusedHomePlate, w, h, ms);
    }

    // Cross: cross<od>x<id>x<angle>
    static const std::regex crossRegex(R"(^cross(\d+\.?\d*)x(\d+\.?\d*)x(\d+\.?\d*)$)", std::regex::icase);
    if (std::regex_match(name, match, crossRegex)) {
        double od = parseNum(match[1].str());
        double id = parseNum(match[2].str());
        double angle = parseNum(match[3].str());
        auto sym = std::make_unique<Symbol>(name);
        sym->type_ = SymbolType::Cross;
        sym->isStandard_ = true;
        sym->primaryDim_ = od;
        sym->secondaryDim_ = id;
        sym->angle_ = angle;
        sym->boundingBox_ = makeBBoxFromRadius(od / 2.0);
        return sym;
    }

    // Dog bone: dogbone<w>x<h>x<lw>x<id>x<od>x<s>
    static const std::regex dogboneRegex(R"(^dogbone(\d+\.?\d*)x(\d+\.?\d*)x(\d+\.?\d*)x(\d+\.?\d*)x(\d+\.?\d*)x(\d+\.?\d*)$)", std::regex::icase);
    if (std::regex_match(name, match, dogboneRegex)) {
        double w = parseNum(match[1].str());
        double h = parseNum(match[2].str());
        double lw = parseNum(match[3].str());
        double id = parseNum(match[4].str());
        double od = parseNum(match[5].str());
        double s = parseNum(match[6].str());
        auto sym = std::make_unique<Symbol>(name);
        sym->type_ = SymbolType::DogBone;
        sym->isStandard_ = true;
        sym->primaryDim_ = w;
        sym->secondaryDim_ = h;
        sym->tertiaryDim_ = lw;
        sym->quaternaryDim_ = id;
        sym->quinaryDim_ = od;
        sym->senaryDim_ = s;
        sym->boundingBox_ = makeBBox(w, h);
        return sym;
    }

    // D-Pack: dpack<w>x<h>x<a>
    static const std::regex dpackRegex(R"(^dpack(\d+\.?\d*)x(\d+\.?\d*)x(\d+\.?\d*)$)", std::regex::icase);
    if (std::regex_match(name, match, dpackRegex)) {
        double w = parseNum(match[1].str());
        double h = parseNum(match[2].str());
        double a = parseNum(match[3].str());
        auto sym = std::make_unique<Symbol>(name);
        sym->type_ = SymbolType::DPack;
        sym->isStandard_ = true;
        sym->primaryDim_ = w;
        sym->secondaryDim_ = h;
        sym->tertiaryDim_ = a;
        sym->boundingBox_ = makeBBox(w, h);
        return sym;
    }

    // ========================================================================
    // Special symbols
    // ========================================================================

    // Target: target<d>x<iw>x<ow>x<ig>x<og>
    static const std::regex targetRegex(R"(^target(\d+\.?\d*)x(\d+\.?\d*)x(\d+\.?\d*)x(\d+\.?\d*)x(\d+\.?\d*)$)", std::regex::icase);
    if (std::regex_match(name, match, targetRegex)) {
        double d = parseNum(match[1].str());
        double iw = parseNum(match[2].str());
        double ow = parseNum(match[3].str());
        double ig = parseNum(match[4].str());
        double og = parseNum(match[5].str());
        return std::make_unique<TargetSymbol>(d, iw, ow, ig, og);
    }

    // Null: null<n>
    static const std::regex nullRegex(R"(^null(\d+)$)", std::regex::icase);
    if (std::regex_match(name, match, nullRegex)) {
        int n = std::stoi(match[1].str());
        return std::make_unique<NullSymbol>(n);
    }

    // Moire: moire<d>x<rw>x<rg>x<rc>x<lw>x<ll>x<la>
    static const std::regex moireRegex(R"(^moire(\d+\.?\d*)x(\d+\.?\d*)x(\d+\.?\d*)x(\d+)x(\d+\.?\d*)x(\d+\.?\d*)x(\d+\.?\d*)$)", std::regex::icase);
    if (std::regex_match(name, match, moireRegex)) {
        auto sym = std::make_unique<Symbol>(name);
        sym->type_ = SymbolType::Moire;
        sym->isStandard_ = true;
        sym->primaryDim_ = parseNum(match[1].str());      // diameter
        sym->secondaryDim_ = parseNum(match[2].str());    // ring width
        sym->tertiaryDim_ = parseNum(match[3].str());     // ring gap
        sym->spokeCount_ = std::stoi(match[4].str());     // ring count
        sym->quaternaryDim_ = parseNum(match[5].str());   // line width
        sym->quinaryDim_ = parseNum(match[6].str());      // line length
        sym->senaryDim_ = parseNum(match[7].str());       // line angle
        sym->boundingBox_ = makeBBoxFromRadius(sym->primaryDim_ / 2.0);
        return sym;
    }

    // Hole: hole<d>x<p>x<tp>x<tm>
    static const std::regex holeRegex(R"(^hole(\d+\.?\d*)x([pn])x(\d+)x([yn])$)", std::regex::icase);
    if (std::regex_match(name, match, holeRegex)) {
        auto sym = std::make_unique<Symbol>(name);
        sym->type_ = SymbolType::Hole;
        sym->isStandard_ = true;
        sym->primaryDim_ = parseNum(match[1].str());   // diameter
        // p/n = plated/non-plated
        sym->corners_ = (match[2].str()[0] == 'p' || match[2].str()[0] == 'P') ? 1 : 0;
        sym->spokeCount_ = std::stoi(match[3].str()); // tool type
        // y/n = toleranced
        sym->secondaryDim_ = (match[4].str()[0] == 'y' || match[4].str()[0] == 'Y') ? 1.0 : 0.0;
        sym->boundingBox_ = makeBBoxFromRadius(sym->primaryDim_ / 2.0);
        return sym;
    }

    return nullptr;
}

// ============================================================================
// RoundSymbol
// ============================================================================

RoundSymbol::RoundSymbol(double diameter) : Symbol("r" + std::to_string(static_cast<int>(diameter)), NoParseTag{}) {
    type_ = SymbolType::Round;
    isStandard_ = true;
    primaryDim_ = diameter;
    double half = diameter / 2.0;
    boundingBox_ = BoundingBox2D({-half, -half}, {half, half});
}

// ============================================================================
// SquareSymbol
// ============================================================================

SquareSymbol::SquareSymbol(double size) : Symbol("s" + std::to_string(static_cast<int>(size)), NoParseTag{}) {
    type_ = SymbolType::Square;
    isStandard_ = true;
    primaryDim_ = size;
    double half = size / 2.0;
    boundingBox_ = BoundingBox2D({-half, -half}, {half, half});
}

// ============================================================================
// RectangleSymbol
// ============================================================================

RectangleSymbol::RectangleSymbol(double width, double height)
    : Symbol("rect" + std::to_string(static_cast<int>(width)) + "x" +
             std::to_string(static_cast<int>(height)), NoParseTag{}) {
    type_ = SymbolType::Rectangle;
    isStandard_ = true;
    primaryDim_ = width;
    secondaryDim_ = height;
    boundingBox_ = makeBBox(width, height);
}

// ============================================================================
// RoundedRectangleSymbol
// ============================================================================

RoundedRectangleSymbol::RoundedRectangleSymbol(double width, double height, double radius, uint8_t corners)
    : Symbol("rect" + std::to_string(static_cast<int>(width)) + "x" +
             std::to_string(static_cast<int>(height)) + "xr" +
             std::to_string(static_cast<int>(radius)), NoParseTag{}) {
    type_ = SymbolType::RoundedRectangle;
    isStandard_ = true;
    primaryDim_ = width;
    secondaryDim_ = height;
    cornerRadius_ = radius;
    corners_ = corners;
    boundingBox_ = makeBBox(width, height);
}

// ============================================================================
// ChamferedRectangleSymbol
// ============================================================================

ChamferedRectangleSymbol::ChamferedRectangleSymbol(double width, double height, double chamfer, uint8_t corners)
    : Symbol("rect" + std::to_string(static_cast<int>(width)) + "x" +
             std::to_string(static_cast<int>(height)) + "xc" +
             std::to_string(static_cast<int>(chamfer)), NoParseTag{}) {
    type_ = SymbolType::ChamferedRectangle;
    isStandard_ = true;
    primaryDim_ = width;
    secondaryDim_ = height;
    cornerRadius_ = chamfer;
    corners_ = corners;
    boundingBox_ = makeBBox(width, height);
}

// ============================================================================
// OblongSymbol
// ============================================================================

OblongSymbol::OblongSymbol(double width, double height)
    : Symbol("oval" + std::to_string(static_cast<int>(width)) + "x" +
             std::to_string(static_cast<int>(height)), NoParseTag{}) {
    type_ = SymbolType::Oblong;
    isStandard_ = true;
    primaryDim_ = width;
    secondaryDim_ = height;
    boundingBox_ = makeBBox(width, height);
}

// ============================================================================
// DiamondSymbol
// ============================================================================

DiamondSymbol::DiamondSymbol(double width, double height)
    : Symbol("di" + std::to_string(static_cast<int>(width)) + "x" +
             std::to_string(static_cast<int>(height)), NoParseTag{}) {
    type_ = SymbolType::Diamond;
    isStandard_ = true;
    primaryDim_ = width;
    secondaryDim_ = height;
    boundingBox_ = makeBBox(width, height);
}

// ============================================================================
// OctagonSymbol
// ============================================================================

OctagonSymbol::OctagonSymbol(double width, double height, double corner)
    : Symbol("oct" + std::to_string(static_cast<int>(width)) + "x" +
             std::to_string(static_cast<int>(height)) + "x" +
             std::to_string(static_cast<int>(corner)), NoParseTag{}) {
    type_ = SymbolType::Octagon;
    isStandard_ = true;
    primaryDim_ = width;
    secondaryDim_ = height;
    tertiaryDim_ = corner;
    boundingBox_ = makeBBox(width, height);
}

// ============================================================================
// TriangleSymbol
// ============================================================================

TriangleSymbol::TriangleSymbol(double base, double height)
    : Symbol("tri" + std::to_string(static_cast<int>(base)) + "x" +
             std::to_string(static_cast<int>(height)), NoParseTag{}) {
    type_ = SymbolType::Triangle;
    isStandard_ = true;
    primaryDim_ = base;
    secondaryDim_ = height;
    boundingBox_ = makeBBox(base, height);
}

// ============================================================================
// HalfOvalSymbol
// ============================================================================

HalfOvalSymbol::HalfOvalSymbol(double width, double height)
    : Symbol("oval_h" + std::to_string(static_cast<int>(width)) + "x" +
             std::to_string(static_cast<int>(height)), NoParseTag{}) {
    type_ = SymbolType::HalfOval;
    isStandard_ = true;
    primaryDim_ = width;
    secondaryDim_ = height;
    boundingBox_ = makeBBox(width, height);
}

// ============================================================================
// HexagonSymbol
// ============================================================================

HexagonSymbol::HexagonSymbol(double width, double height, double corner, bool horizontal)
    : Symbol((horizontal ? "hex_l" : "hex_s") +
             std::to_string(static_cast<int>(width)) + "x" +
             std::to_string(static_cast<int>(height)) + "x" +
             std::to_string(static_cast<int>(corner)), NoParseTag{}) {
    type_ = horizontal ? SymbolType::HorizontalHexagon : SymbolType::VerticalHexagon;
    isStandard_ = true;
    primaryDim_ = width;
    secondaryDim_ = height;
    tertiaryDim_ = corner;
    boundingBox_ = makeBBox(width, height);
}

// ============================================================================
// ButterflySymbol
// ============================================================================

ButterflySymbol::ButterflySymbol(double size, bool round)
    : Symbol((round ? "bfr" : "bfs") + std::to_string(static_cast<int>(size)), NoParseTag{}) {
    type_ = round ? SymbolType::Butterfly : SymbolType::SquareButterfly;
    isStandard_ = true;
    primaryDim_ = size;
    double half = size / 2.0;
    boundingBox_ = BoundingBox2D({-half, -half}, {half, half});
}

// ============================================================================
// RoundDonutSymbol
// ============================================================================

RoundDonutSymbol::RoundDonutSymbol(double outerDiameter, double innerDiameter)
    : Symbol("donut_r" + std::to_string(static_cast<int>(outerDiameter)) + "x" +
             std::to_string(static_cast<int>(innerDiameter)), NoParseTag{}) {
    type_ = SymbolType::RoundDonut;
    isStandard_ = true;
    primaryDim_ = outerDiameter;
    secondaryDim_ = innerDiameter;
    double half = outerDiameter / 2.0;
    boundingBox_ = BoundingBox2D({-half, -half}, {half, half});
}

// ============================================================================
// SquareDonutSymbol
// ============================================================================

SquareDonutSymbol::SquareDonutSymbol(double outer, double inner)
    : Symbol("donut_s" + std::to_string(static_cast<int>(outer)) + "x" +
             std::to_string(static_cast<int>(inner)), NoParseTag{}) {
    type_ = SymbolType::SquareDonut;
    isStandard_ = true;
    primaryDim_ = outer;
    secondaryDim_ = inner;
    double half = outer / 2.0;
    boundingBox_ = BoundingBox2D({-half, -half}, {half, half});
}

// ============================================================================
// RectangleDonutSymbol
// ============================================================================

RectangleDonutSymbol::RectangleDonutSymbol(double outerWidth, double outerHeight, double lineWidth)
    : Symbol("donut_rc" + std::to_string(static_cast<int>(outerWidth)) + "x" +
             std::to_string(static_cast<int>(outerHeight)) + "x" +
             std::to_string(static_cast<int>(lineWidth)), NoParseTag{}) {
    type_ = SymbolType::RectangleDonut;
    isStandard_ = true;
    primaryDim_ = outerWidth;
    secondaryDim_ = outerHeight;
    tertiaryDim_ = lineWidth;
    boundingBox_ = makeBBox(outerWidth, outerHeight);
}

// ============================================================================
// OvalDonutSymbol
// ============================================================================

OvalDonutSymbol::OvalDonutSymbol(double outerWidth, double outerHeight, double lineWidth)
    : Symbol("donut_o" + std::to_string(static_cast<int>(outerWidth)) + "x" +
             std::to_string(static_cast<int>(outerHeight)) + "x" +
             std::to_string(static_cast<int>(lineWidth)), NoParseTag{}) {
    type_ = SymbolType::OvalDonut;
    isStandard_ = true;
    primaryDim_ = outerWidth;
    secondaryDim_ = outerHeight;
    tertiaryDim_ = lineWidth;
    boundingBox_ = makeBBox(outerWidth, outerHeight);
}

// ============================================================================
// ThermalSymbol
// ============================================================================

ThermalSymbol::ThermalSymbol(double outerDiameter, double innerDiameter,
                             double angle, int spokes, double gap, bool rounded)
    : Symbol((rounded ? "thr" : "ths") +
             std::to_string(static_cast<int>(outerDiameter)) + "x" +
             std::to_string(static_cast<int>(innerDiameter)) + "x" +
             std::to_string(static_cast<int>(angle)) + "x" +
             std::to_string(spokes) + "x" +
             std::to_string(static_cast<int>(gap)), NoParseTag{}) {
    type_ = rounded ? SymbolType::RoundThermalRounded : SymbolType::RoundThermalSquared;
    isStandard_ = true;
    primaryDim_ = outerDiameter;
    secondaryDim_ = innerDiameter;
    angle_ = angle;
    spokeCount_ = spokes;
    spokeGap_ = gap;
    double half = outerDiameter / 2.0;
    boundingBox_ = BoundingBox2D({-half, -half}, {half, half});
}

// ============================================================================
// SquareThermalSymbol
// ============================================================================

SquareThermalSymbol::SquareThermalSymbol(double outerSize, double innerSize,
                                         double angle, int spokes, double gap,
                                         bool openCorners)
    : Symbol((openCorners ? "s_tho" : "s_ths") +
             std::to_string(static_cast<int>(outerSize)) + "x" +
             std::to_string(static_cast<int>(innerSize)) + "x" +
             std::to_string(static_cast<int>(angle)) + "x" +
             std::to_string(spokes) + "x" +
             std::to_string(static_cast<int>(gap)), NoParseTag{}) {
    type_ = openCorners ? SymbolType::SquareThermalOpenCorner : SymbolType::SquareThermal;
    isStandard_ = true;
    primaryDim_ = outerSize;
    secondaryDim_ = innerSize;
    angle_ = angle;
    spokeCount_ = spokes;
    spokeGap_ = gap;
    double half = outerSize / 2.0;
    boundingBox_ = BoundingBox2D({-half, -half}, {half, half});
}

// ============================================================================
// RectangularThermalSymbol
// ============================================================================

RectangularThermalSymbol::RectangularThermalSymbol(double width, double height,
                                                   double angle, int spokes, double gap, double airGap,
                                                   bool openCorners)
    : Symbol((openCorners ? "rc_tho" : "rc_ths") +
             std::to_string(static_cast<int>(width)) + "x" +
             std::to_string(static_cast<int>(height)) + "x" +
             std::to_string(static_cast<int>(angle)) + "x" +
             std::to_string(spokes) + "x" +
             std::to_string(static_cast<int>(gap)) + "x" +
             std::to_string(static_cast<int>(airGap)), NoParseTag{}) {
    type_ = openCorners ? SymbolType::RectThermalOpenCorner : SymbolType::RectangularThermal;
    isStandard_ = true;
    primaryDim_ = width;
    secondaryDim_ = height;
    angle_ = angle;
    spokeCount_ = spokes;
    spokeGap_ = gap;
    senaryDim_ = airGap;
    boundingBox_ = makeBBox(width, height);
}

// ============================================================================
// OvalThermalSymbol
// ============================================================================

OvalThermalSymbol::OvalThermalSymbol(double outerWidth, double outerHeight,
                                     double angle, int spokes, double gap, double lineWidth)
    : Symbol("o_ths" +
             std::to_string(static_cast<int>(outerWidth)) + "x" +
             std::to_string(static_cast<int>(outerHeight)) + "x" +
             std::to_string(static_cast<int>(angle)) + "x" +
             std::to_string(spokes) + "x" +
             std::to_string(static_cast<int>(gap)) + "x" +
             std::to_string(static_cast<int>(lineWidth)), NoParseTag{}) {
    type_ = SymbolType::OvalThermal;
    isStandard_ = true;
    primaryDim_ = outerWidth;
    secondaryDim_ = outerHeight;
    angle_ = angle;
    spokeCount_ = spokes;
    spokeGap_ = gap;
    tertiaryDim_ = lineWidth;
    boundingBox_ = makeBBox(outerWidth, outerHeight);
}

// ============================================================================
// HomePlateSymbol
// ============================================================================

HomePlateSymbol::HomePlateSymbol(SymbolType plateType, double width, double height, double cut,
                                 double radiusAcute, double radiusObtuse)
    : Symbol("hplate" + std::to_string(static_cast<int>(width)) + "x" +
             std::to_string(static_cast<int>(height)) + "x" +
             std::to_string(static_cast<int>(cut)), NoParseTag{}) {
    type_ = plateType;
    isStandard_ = true;
    primaryDim_ = width;
    secondaryDim_ = height;
    tertiaryDim_ = cut;
    quaternaryDim_ = radiusAcute;
    quinaryDim_ = radiusObtuse;
    boundingBox_ = makeBBox(width, height);
}

// ============================================================================
// TargetSymbol
// ============================================================================

TargetSymbol::TargetSymbol(double diameter, double innerWidth, double outerWidth,
                           double innerGap, double outerGap)
    : Symbol("target" +
             std::to_string(static_cast<int>(diameter)) + "x" +
             std::to_string(static_cast<int>(innerWidth)) + "x" +
             std::to_string(static_cast<int>(outerWidth)) + "x" +
             std::to_string(static_cast<int>(innerGap)) + "x" +
             std::to_string(static_cast<int>(outerGap)), NoParseTag{}) {
    type_ = SymbolType::Target;
    isStandard_ = true;
    primaryDim_ = diameter;
    secondaryDim_ = innerWidth;
    tertiaryDim_ = outerWidth;
    quaternaryDim_ = innerGap;
    quinaryDim_ = outerGap;
    double half = diameter / 2.0;
    boundingBox_ = BoundingBox2D({-half, -half}, {half, half});
}

// ============================================================================
// NullSymbol
// ============================================================================

NullSymbol::NullSymbol(int index)
    : Symbol("null" + std::to_string(index), NoParseTag{}) {
    type_ = SymbolType::Null;
    isStandard_ = true;
    primaryDim_ = static_cast<double>(index);
    // Null symbol has no size
    boundingBox_ = BoundingBox2D({0, 0}, {0, 0});
}

// ============================================================================
// SymbolLibrary
// ============================================================================

Symbol* SymbolLibrary::getSymbol(const std::string& name) {
    auto it = symbols_.find(name);
    if (it != symbols_.end()) {
        return it->second.get();
    }

    // Try to create standard symbol
    auto standard = createStandardSymbol(name);
    if (standard) {
        Symbol* ptr = standard.get();
        symbols_[name] = std::move(standard);
        return ptr;
    }

    return nullptr;
}

const Symbol* SymbolLibrary::getSymbol(const std::string& name) const {
    auto it = symbols_.find(name);
    if (it != symbols_.end()) {
        return it->second.get();
    }
    return nullptr;
}

Symbol* SymbolLibrary::getOrCreateSymbol(const std::string& name) {
    return getSymbol(name);
}

void SymbolLibrary::addSymbol(std::unique_ptr<Symbol> symbol) {
    if (symbol) {
        symbols_[symbol->getName()] = std::move(symbol);
    }
}

bool SymbolLibrary::hasSymbol(const std::string& name) const {
    return symbols_.find(name) != symbols_.end();
}

std::vector<std::string> SymbolLibrary::getSymbolNames() const {
    std::vector<std::string> names;
    names.reserve(symbols_.size());
    for (const auto& pair : symbols_) {
        names.push_back(pair.first);
    }
    return names;
}

void SymbolLibrary::clear() {
    symbols_.clear();
}

bool SymbolLibrary::loadUserSymbol(const std::string& /*symbolPath*/) {
    // TODO: Implement loading user-defined symbol from features file
    return false;
}

std::unique_ptr<Symbol> SymbolLibrary::createStandardSymbol(const std::string& name) const {
    return Symbol::parseStandardSymbol(name);
}

} // namespace koo::ecad
