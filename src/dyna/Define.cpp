#include <koo/dyna/Define.hpp>
#include <koo/dyna/ModelVisitor.hpp>
#include <koo/dyna/KeywordFactory.hpp>
#include <koo/util/StringUtils.hpp>
#include <algorithm>
#include <cmath>
#include <sstream>

namespace koo::dyna {

// ============================================================================
// DefineCurve
// ============================================================================

bool DefineCurve::parse(const std::vector<std::string>& lines,
                         util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    points_.clear();
    size_t i = 0;

    // Skip comment lines to find header
    while (i < lines.size() && util::CardParser::isCommentLine(lines[i])) {
        ++i;
    }
    if (i >= lines.size()) return true;

    // Parse header line
    // LCID, SIDR, SFA, SFO, OFFA, OFFO, DATTYP, LCINT
    parser.setLine(lines[i]);
    size_t pos = 0;

    header_.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    header_.sidr = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    header_.sfa = parser.getDoubleAt(pos, realW).value_or(1.0);
    pos += realW;
    header_.sfo = parser.getDoubleAt(pos, realW).value_or(1.0);
    pos += realW;
    header_.offa = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    header_.offo = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    header_.dattyp = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    header_.lcint = static_cast<int>(parser.getInt64At(pos).value_or(0));
    ++i;

    // Parse point data
    for (; i < lines.size(); ++i) {
        if (util::CardParser::isCommentLine(lines[i])) continue;

        parser.setLine(lines[i]);
        pos = 0;

        double a = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        double o = parser.getDoubleAt(pos, realW).value_or(0.0);

        points_.emplace_back(a, o);
    }

    return true;
}

std::vector<std::string> DefineCurve::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    // Header line
    writer.writeInt(header_.lcid);
    writer.writeInt(header_.sidr);
    writer.writeDouble(header_.sfa);
    writer.writeDouble(header_.sfo);
    writer.writeDouble(header_.offa);
    writer.writeDouble(header_.offo);
    writer.writeInt(header_.dattyp);
    writer.writeInt(header_.lcint);
    result.push_back(writer.getLine());

    // Point data
    for (const auto& pt : points_) {
        writer.clear();
        writer.writeDouble(pt.first);
        writer.writeDouble(pt.second);
        result.push_back(writer.getLine());
    }

    return result;
}

void DefineCurve::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

double DefineCurve::interpolate(double x) const {
    if (points_.empty()) return 0.0;
    if (points_.size() == 1) return points_[0].second;

    // Apply scale and offset to input
    double scaledX = (x - header_.offa) / header_.sfa;

    // Find bounding points
    if (scaledX <= points_.front().first) {
        return points_.front().second * header_.sfo + header_.offo;
    }
    if (scaledX >= points_.back().first) {
        return points_.back().second * header_.sfo + header_.offo;
    }

    // Linear interpolation
    for (size_t i = 1; i < points_.size(); ++i) {
        if (scaledX <= points_[i].first) {
            double x0 = points_[i - 1].first;
            double x1 = points_[i].first;
            double y0 = points_[i - 1].second;
            double y1 = points_[i].second;

            double t = (scaledX - x0) / (x1 - x0);
            double y = y0 + t * (y1 - y0);

            return y * header_.sfo + header_.offo;
        }
    }

    return points_.back().second * header_.sfo + header_.offo;
}

REGISTER_KEYWORD(DefineCurve, "*DEFINE_CURVE")

// ============================================================================
// DefineCurveTitle
// ============================================================================

bool DefineCurveTitle::parse(const std::vector<std::string>& lines,
                              util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    points_.clear();
    size_t i = 0;

    // Skip comment lines to find title
    while (i < lines.size() && util::CardParser::isCommentLine(lines[i])) {
        ++i;
    }
    if (i >= lines.size()) return true;

    // First line is title
    title_ = util::StringUtils::trim(lines[i]);
    ++i;

    // Skip comment lines to find header
    while (i < lines.size() && util::CardParser::isCommentLine(lines[i])) {
        ++i;
    }
    if (i >= lines.size()) return true;

    // Parse header line
    parser.setLine(lines[i]);
    size_t pos = 0;

    header_.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    header_.sidr = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    header_.sfa = parser.getDoubleAt(pos, realW).value_or(1.0);
    pos += realW;
    header_.sfo = parser.getDoubleAt(pos, realW).value_or(1.0);
    pos += realW;
    header_.offa = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    header_.offo = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    header_.dattyp = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    header_.lcint = static_cast<int>(parser.getInt64At(pos).value_or(0));
    ++i;

    // Parse point data
    for (; i < lines.size(); ++i) {
        if (util::CardParser::isCommentLine(lines[i])) continue;

        parser.setLine(lines[i]);
        pos = 0;

        double a = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        double o = parser.getDoubleAt(pos, realW).value_or(0.0);

        points_.emplace_back(a, o);
    }

    return true;
}

std::vector<std::string> DefineCurveTitle::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    // Title line
    result.push_back(title_);

    // Header line
    writer.writeInt(header_.lcid);
    writer.writeInt(header_.sidr);
    writer.writeDouble(header_.sfa);
    writer.writeDouble(header_.sfo);
    writer.writeDouble(header_.offa);
    writer.writeDouble(header_.offo);
    writer.writeInt(header_.dattyp);
    writer.writeInt(header_.lcint);
    result.push_back(writer.getLine());

    // Point data
    for (const auto& pt : points_) {
        writer.clear();
        writer.writeDouble(pt.first);
        writer.writeDouble(pt.second);
        result.push_back(writer.getLine());
    }

    return result;
}

void DefineCurveTitle::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DefineCurveTitle, "*DEFINE_CURVE_TITLE")

// ============================================================================
// DefineCoordinateNodes
// ============================================================================

bool DefineCoordinateNodes::parse(const std::vector<std::string>& lines,
                                   util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;

        data_.cid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.n1 = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data_.n2 = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data_.n3 = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data_.flag = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> DefineCoordinateNodes::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.cid);
    writer.writeInt(data_.n1);
    writer.writeInt(data_.n2);
    writer.writeInt(data_.n3);
    writer.writeInt(data_.flag);

    result.push_back(writer.getLine());
    return result;
}

void DefineCoordinateNodes::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DefineCoordinateNodes, "*DEFINE_COORDINATE_NODES")

// ============================================================================
// DefineCoordinateVector
// ============================================================================

bool DefineCoordinateVector::parse(const std::vector<std::string>& lines,
                                    util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    // Skip comments
    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: CID, X0, Y0, Z0
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.cid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.x0 = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.y0 = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.z0 = parser.getDoubleAt(pos, realW).value_or(0.0);
    ++lineIdx;

    // Skip comments
    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 2: XX, XY, XZ, YX, YY, YZ
    parser.setLine(lines[lineIdx]);
    pos = 0;

    data_.xx = parser.getDoubleAt(pos, realW).value_or(1.0);
    pos += realW;
    data_.xy = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.xz = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.yx = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.yy = parser.getDoubleAt(pos, realW).value_or(1.0);
    pos += realW;
    data_.yz = parser.getDoubleAt(pos, realW).value_or(0.0);

    return true;
}

std::vector<std::string> DefineCoordinateVector::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    // Card 1
    writer.writeInt(data_.cid);
    writer.writeDouble(data_.x0);
    writer.writeDouble(data_.y0);
    writer.writeDouble(data_.z0);
    result.push_back(writer.getLine());

    // Card 2
    writer.clear();
    writer.writeDouble(data_.xx);
    writer.writeDouble(data_.xy);
    writer.writeDouble(data_.xz);
    writer.writeDouble(data_.yx);
    writer.writeDouble(data_.yy);
    writer.writeDouble(data_.yz);
    result.push_back(writer.getLine());

    return result;
}

void DefineCoordinateVector::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DefineCoordinateVector, "*DEFINE_COORDINATE_VECTOR")

// ============================================================================
// DefineCoordinateSystem
// ============================================================================

bool DefineCoordinateSystem::parse(const std::vector<std::string>& lines,
                                    util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    // Skip comments
    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: CID, XO, YO, ZO, XL, YL, ZL
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.cid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.xo = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.yo = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.zo = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.xl = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.yl = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.zl = parser.getDoubleAt(pos, realW).value_or(0.0);
    ++lineIdx;

    // Skip comments
    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 2: XP, YP, ZP
    parser.setLine(lines[lineIdx]);
    pos = 0;

    data_.xp = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.yp = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.zp = parser.getDoubleAt(pos, realW).value_or(0.0);

    return true;
}

std::vector<std::string> DefineCoordinateSystem::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    // Card 1
    writer.writeInt(data_.cid);
    writer.writeDouble(data_.xo);
    writer.writeDouble(data_.yo);
    writer.writeDouble(data_.zo);
    writer.writeDouble(data_.xl);
    writer.writeDouble(data_.yl);
    writer.writeDouble(data_.zl);
    result.push_back(writer.getLine());

    // Card 2
    writer.clear();
    writer.writeDouble(data_.xp);
    writer.writeDouble(data_.yp);
    writer.writeDouble(data_.zp);
    result.push_back(writer.getLine());

    return result;
}

void DefineCoordinateSystem::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DefineCoordinateSystem, "*DEFINE_COORDINATE_SYSTEM")

// ============================================================================
// DefineBox
// ============================================================================

bool DefineBox::parse(const std::vector<std::string>& lines,
                       util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    // Skip comments
    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: BOXID, XMIN, XMAX, YMIN, YMAX, ZMIN, ZMAX
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.boxid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.xmin = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.xmax = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.ymin = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.ymax = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.zmin = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.zmax = parser.getDoubleAt(pos, realW).value_or(0.0);

    return true;
}

std::vector<std::string> DefineBox::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.boxid);
    writer.writeDouble(data_.xmin);
    writer.writeDouble(data_.xmax);
    writer.writeDouble(data_.ymin);
    writer.writeDouble(data_.ymax);
    writer.writeDouble(data_.zmin);
    writer.writeDouble(data_.zmax);

    result.push_back(writer.getLine());
    return result;
}

void DefineBox::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DefineBox, "*DEFINE_BOX")

// ============================================================================
// DefineTransformation
// ============================================================================

bool DefineTransformation::parse(const std::vector<std::string>& lines,
                                  util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: TRANID, OPTION, A1, A2, A3, A4, A5, A6, A7
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.tranid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.option = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.a1 = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.a2 = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.a3 = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.a4 = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.a5 = parser.getDoubleAt(pos, realW).value_or(0.0);

    ++lineIdx;
    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx < lines.size()) {
        parser.setLine(lines[lineIdx]);
        data_.a6 = parser.getDoubleAt(0, realW).value_or(0.0);
        data_.a7 = parser.getDoubleAt(realW, realW).value_or(0.0);
    }

    return true;
}

std::vector<std::string> DefineTransformation::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.tranid);
    writer.writeInt(data_.option);
    writer.writeDouble(data_.a1);
    writer.writeDouble(data_.a2);
    writer.writeDouble(data_.a3);
    writer.writeDouble(data_.a4);
    writer.writeDouble(data_.a5);
    result.push_back(writer.getLine());

    writer.clear();
    writer.writeDouble(data_.a6);
    writer.writeDouble(data_.a7);
    result.push_back(writer.getLine());

    return result;
}

void DefineTransformation::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DefineTransformation, "*DEFINE_TRANSFORMATION")

// ============================================================================
// DefineVector
// ============================================================================

bool DefineVector::parse(const std::vector<std::string>& lines,
                          util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: VID, XT, YT, ZT, XH, YH, ZH, CID
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.vid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.xt = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.yt = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.zt = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.xh = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.yh = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.zh = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.cid = static_cast<int>(parser.getInt64At(pos).value_or(0));

    return true;
}

std::vector<std::string> DefineVector::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.vid);
    writer.writeDouble(data_.xt);
    writer.writeDouble(data_.yt);
    writer.writeDouble(data_.zt);
    writer.writeDouble(data_.xh);
    writer.writeDouble(data_.yh);
    writer.writeDouble(data_.zh);
    writer.writeInt(data_.cid);

    result.push_back(writer.getLine());
    return result;
}

void DefineVector::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DefineVector, "*DEFINE_VECTOR")

// ============================================================================
// DefineTable
// ============================================================================

bool DefineTable::parse(const std::vector<std::string>& lines,
                         util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t realW = parser.getRealWidth();

    entries_.clear();
    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: TBID
    parser.setLine(lines[lineIdx]);
    tbid_ = static_cast<int>(parser.getInt64At(0).value_or(0));
    ++lineIdx;

    // Parse table entries (VALUE, LCID pairs)
    for (; lineIdx < lines.size(); ++lineIdx) {
        if (util::CardParser::isCommentLine(lines[lineIdx])) continue;

        parser.setLine(lines[lineIdx]);
        Entry entry;
        entry.value = parser.getDoubleAt(0, realW).value_or(0.0);
        entry.lcid = static_cast<int>(parser.getInt64At(realW).value_or(0));
        entries_.push_back(entry);
    }

    return true;
}

std::vector<std::string> DefineTable::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    // Header
    writer.writeInt(tbid_);
    result.push_back(writer.getLine());

    // Entries
    for (const auto& entry : entries_) {
        writer.clear();
        writer.writeDouble(entry.value);
        writer.writeInt(entry.lcid);
        result.push_back(writer.getLine());
    }

    return result;
}

void DefineTable::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DefineTable, "*DEFINE_TABLE")

// ============================================================================
// DefineFriction
// ============================================================================

bool DefineFriction::parse(const std::vector<std::string>& lines,
                            util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: FID, FS, FD, DC, VC, FV
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.fid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.fs = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.fd = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.dc = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.vc = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.fv = parser.getDoubleAt(pos, realW).value_or(0.0);

    return true;
}

std::vector<std::string> DefineFriction::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.fid);
    writer.writeDouble(data_.fs);
    writer.writeDouble(data_.fd);
    writer.writeDouble(data_.dc);
    writer.writeDouble(data_.vc);
    writer.writeDouble(data_.fv);

    result.push_back(writer.getLine());
    return result;
}

void DefineFriction::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DefineFriction, "*DEFINE_FRICTION")

// ============================================================================
// DefineSdOrientation
// ============================================================================

bool DefineSdOrientation::parse(const std::vector<std::string>& lines,
                                 util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: VID, IOP, XT, YT, ZT, XH, YH, ZH
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.vid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.iop = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.xt = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.yt = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.zt = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.xh = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.yh = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.zh = parser.getDoubleAt(pos, realW).value_or(0.0);

    return true;
}

std::vector<std::string> DefineSdOrientation::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.vid);
    writer.writeInt(data_.iop);
    writer.writeDouble(data_.xt);
    writer.writeDouble(data_.yt);
    writer.writeDouble(data_.zt);
    writer.writeDouble(data_.xh);
    writer.writeDouble(data_.yh);
    writer.writeDouble(data_.zh);

    result.push_back(writer.getLine());
    return result;
}

void DefineSdOrientation::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DefineSdOrientation, "*DEFINE_SD_ORIENTATION")

// ============================================================================
// DefineElementDeath
// ============================================================================

bool DefineElementDeath::parse(const std::vector<std::string>& lines,
                                util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    data_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        Data entry;
        size_t pos = 0;

        entry.sid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.time = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        entry.boxid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.cycle = static_cast<int>(parser.getInt64At(pos).value_or(0));

        if (entry.sid != 0) {
            data_.push_back(entry);
        }
    }

    return true;
}

std::vector<std::string> DefineElementDeath::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& entry : data_) {
        writer.clear();
        writer.writeInt(entry.sid);
        writer.writeDouble(entry.time);
        writer.writeInt(entry.boxid);
        writer.writeInt(entry.cycle);
        result.push_back(writer.getLine());
    }

    return result;
}

void DefineElementDeath::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DefineElementDeath, "*DEFINE_ELEMENT_DEATH")

// ============================================================================
// DefineContactVolume
// ============================================================================

bool DefineContactVolume::parse(const std::vector<std::string>& lines,
                                 util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    data_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        Data entry;
        size_t pos = 0;

        entry.cvid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.boxid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.pid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.type = static_cast<int>(parser.getInt64At(pos).value_or(0));

        if (entry.cvid != 0) {
            data_.push_back(entry);
        }
    }

    return true;
}

std::vector<std::string> DefineContactVolume::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& entry : data_) {
        writer.clear();
        writer.writeInt(entry.cvid);
        writer.writeInt(entry.boxid);
        writer.writeInt(entry.pid);
        writer.writeInt(entry.type);
        result.push_back(writer.getLine());
    }

    return result;
}

void DefineContactVolume::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DefineContactVolume, "*DEFINE_CONTACT_VOLUME")

// ============================================================================
// DefineCurveFunction
// ============================================================================

bool DefineCurveFunction::parse(const std::vector<std::string>& lines,
                                 util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: Header
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.sidr = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.sfa = parser.getDoubleAt(pos, realW).value_or(1.0);
    pos += realW;
    data_.sfo = parser.getDoubleAt(pos, realW).value_or(1.0);
    pos += realW;
    data_.offa = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.offo = parser.getDoubleAt(pos, realW).value_or(0.0);
    ++lineIdx;

    // Card 2: Function string
    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx < lines.size()) {
        function_ = util::StringUtils::trim(lines[lineIdx]);
    }

    return true;
}

std::vector<std::string> DefineCurveFunction::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.lcid);
    writer.writeInt(data_.sidr);
    writer.writeDouble(data_.sfa);
    writer.writeDouble(data_.sfo);
    writer.writeDouble(data_.offa);
    writer.writeDouble(data_.offo);
    result.push_back(writer.getLine());

    if (!function_.empty()) {
        result.push_back(function_);
    }

    return result;
}

void DefineCurveFunction::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DefineCurveFunction, "*DEFINE_CURVE_FUNCTION")

// ============================================================================
// DefineConnectionProperties
// ============================================================================

bool DefineConnectionProperties::parse(const std::vector<std::string>& lines,
                                        util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    data_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        Data entry;
        size_t pos = 0;

        entry.cpid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.type = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.dfail = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        entry.efail = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        entry.nfail = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        entry.sfail = parser.getDoubleAt(pos, realW).value_or(0.0);

        if (entry.cpid != 0) {
            data_.push_back(entry);
        }
    }

    return true;
}

std::vector<std::string> DefineConnectionProperties::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& entry : data_) {
        writer.clear();
        writer.writeInt(entry.cpid);
        writer.writeInt(entry.type);
        writer.writeDouble(entry.dfail);
        writer.writeDouble(entry.efail);
        writer.writeDouble(entry.nfail);
        writer.writeDouble(entry.sfail);
        result.push_back(writer.getLine());
    }

    return result;
}

void DefineConnectionProperties::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DefineConnectionProperties, "*DEFINE_CONNECTION_PROPERTIES")

// ============================================================================
// DefineDeMeshSurface
// ============================================================================

bool DefineDeMeshSurface::parse(const std::vector<std::string>& lines,
                                 util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    data_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        Data entry;
        size_t pos = 0;

        entry.sid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.pid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.type = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.size = parser.getDoubleAt(pos, realW).value_or(0.0);

        if (entry.sid != 0) {
            data_.push_back(entry);
        }
    }

    return true;
}

std::vector<std::string> DefineDeMeshSurface::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& entry : data_) {
        writer.clear();
        writer.writeInt(entry.sid);
        writer.writeInt(entry.pid);
        writer.writeInt(entry.type);
        writer.writeDouble(entry.size);
        result.push_back(writer.getLine());
    }

    return result;
}

void DefineDeMeshSurface::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DefineDeMeshSurface, "*DEFINE_DE_MESH_SURFACE")

// ============================================================================
// DefineCpmVent
// ============================================================================

bool DefineCpmVent::parse(const std::vector<std::string>& lines,
                           util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.ventid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.ssid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.acoef = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.bcoef = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.ccoef = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));

    return true;
}

std::vector<std::string> DefineCpmVent::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.ventid);
    writer.writeInt(data_.ssid);
    writer.writeDouble(data_.acoef);
    writer.writeDouble(data_.bcoef);
    writer.writeDouble(data_.ccoef);
    writer.writeInt(data_.lcid);
    result.push_back(writer.getLine());

    return result;
}

void DefineCpmVent::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DefineCpmVent, "*DEFINE_CPM_VENT")

// ============================================================================
// DefineFunctionTabulated
// ============================================================================

bool DefineFunctionTabulated::parse(const std::vector<std::string>& lines,
                                     util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.funcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.nvar = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.mvar = static_cast<int>(parser.getInt64At(pos).value_or(0));

    return true;
}

std::vector<std::string> DefineFunctionTabulated::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.funcid);
    writer.writeInt(data_.nvar);
    writer.writeInt(data_.lcid);
    writer.writeInt(data_.mvar);
    result.push_back(writer.getLine());

    return result;
}

void DefineFunctionTabulated::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DefineFunctionTabulated, "*DEFINE_FUNCTION_TABULATED")

// ============================================================================
// DefineCurveSmooth
// ============================================================================

bool DefineCurveSmooth::parse(const std::vector<std::string>& lines,
                               util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    bool headerParsed = false;

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);

        if (!headerParsed) {
            // Card 1: LCID, SIDR, SFA, SFO, OFFA, OFFO, DATTYP, NPTS
            size_t pos = 0;
            header_.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
            pos += intW;
            header_.sidr = static_cast<int>(parser.getInt64At(pos).value_or(0));
            pos += intW;
            header_.sfa = parser.getDoubleAt(pos, realW).value_or(1.0);
            pos += realW;
            header_.sfo = parser.getDoubleAt(pos, realW).value_or(1.0);
            pos += realW;
            header_.offa = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            header_.offo = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            header_.dattyp = static_cast<int>(parser.getInt64At(pos).value_or(0));
            pos += intW;
            header_.npts = static_cast<int>(parser.getInt64At(pos).value_or(0));
            headerParsed = true;
        } else {
            // Data points: A, O
            double a = parser.getDoubleAt(0, realW).value_or(0.0);
            double o = parser.getDoubleAt(realW, realW).value_or(0.0);
            points_.emplace_back(a, o);
        }
    }

    return true;
}

std::vector<std::string> DefineCurveSmooth::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    // Header card
    writer.writeInt(header_.lcid);
    writer.writeInt(header_.sidr);
    writer.writeDouble(header_.sfa);
    writer.writeDouble(header_.sfo);
    writer.writeDouble(header_.offa);
    writer.writeDouble(header_.offo);
    writer.writeInt(header_.dattyp);
    writer.writeInt(header_.npts);
    result.push_back(writer.getLine());

    // Data points
    for (const auto& pt : points_) {
        writer.clear();
        writer.writeDouble(pt.first);
        writer.writeDouble(pt.second);
        result.push_back(writer.getLine());
    }

    return result;
}

void DefineCurveSmooth::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DefineCurveSmooth, "*DEFINE_CURVE_SMOOTH")

// ============================================================================
// DefineHexSpotweldAssembly
// ============================================================================

bool DefineHexSpotweldAssembly::parse(const std::vector<std::string>& lines,
                                       util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size()) {
        while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
            ++lineIdx;
        }
        if (lineIdx >= lines.size()) break;

        // Header card: ASSEMBLYID, NQUAD, FUNC, FILTER
        Assembly assembly;
        parser.setLine(lines[lineIdx]);
        size_t pos = 0;

        assembly.assemblyId = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        assembly.nquad = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        assembly.func = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        assembly.filter = parser.getDoubleAt(pos, realW).value_or(0.0);
        ++lineIdx;

        if (assembly.assemblyId == 0) break;

        // Read quad IDs (8 per card)
        int quadsRead = 0;
        while (quadsRead < assembly.nquad && lineIdx < lines.size()) {
            while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
                ++lineIdx;
            }
            if (lineIdx >= lines.size()) break;

            parser.setLine(lines[lineIdx]);
            pos = 0;

            for (int i = 0; i < 8 && quadsRead < assembly.nquad; ++i) {
                int qid = static_cast<int>(parser.getInt64At(pos).value_or(0));
                pos += intW;
                if (qid != 0) {
                    assembly.quadIds.push_back(qid);
                    ++quadsRead;
                }
            }
            ++lineIdx;
        }

        assemblies_.push_back(assembly);
    }

    return true;
}

std::vector<std::string> DefineHexSpotweldAssembly::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& assembly : assemblies_) {
        // Header card
        writer.clear();
        writer.writeInt(assembly.assemblyId);
        writer.writeInt(assembly.nquad);
        writer.writeInt(assembly.func);
        writer.writeDouble(assembly.filter);
        result.push_back(writer.getLine());

        // Quad ID cards (8 per line)
        size_t quadCount = assembly.quadIds.size();
        for (size_t i = 0; i < quadCount; i += 8) {
            writer.clear();
            for (size_t j = 0; j < 8 && (i + j) < quadCount; ++j) {
                writer.writeInt(assembly.quadIds[i + j]);
            }
            result.push_back(writer.getLine());
        }
    }

    return result;
}

void DefineHexSpotweldAssembly::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DefineHexSpotweldAssembly, "*DEFINE_HEX_SPOTWELD_ASSEMBLY")

// ============================================================================
// DefineFunction
// ============================================================================

bool DefineFunction::parse(const std::vector<std::string>& lines,
                            util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    bool firstLine = true;
    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        if (firstLine) {
            parser.setLine(line);
            data_.id = static_cast<int>(parser.getInt64At(0).value_or(0));
            firstLine = false;
        } else {
            // Expression lines
            if (!data_.expression.empty()) {
                data_.expression += "\n";
            }
            data_.expression += line;
        }
    }

    return true;
}

std::vector<std::string> DefineFunction::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.id);
    result.push_back(writer.getLine());

    // Split expression by newlines
    std::istringstream iss(data_.expression);
    std::string line;
    while (std::getline(iss, line)) {
        result.push_back(line);
    }

    return result;
}

void DefineFunction::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DefineFunction, "*DEFINE_FUNCTION")

// ============================================================================
// DefineMaterialHistories
// ============================================================================

bool DefineMaterialHistories::parse(const std::vector<std::string>& lines,
                                     util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    bool firstLine = true;
    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);

        if (firstLine) {
            data_.mid = static_cast<int>(parser.getInt64At(0).value_or(0));
            firstLine = false;
        } else {
            size_t pos = 0;
            while (pos + intW <= line.size()) {
                auto val = parser.getInt64At(pos);
                if (val && *val != 0) {
                    data_.hisv.push_back(static_cast<int>(*val));
                }
                pos += intW;
            }
        }
    }

    return true;
}

std::vector<std::string> DefineMaterialHistories::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.mid);
    result.push_back(writer.getLine());

    writer.clear();
    for (size_t i = 0; i < data_.hisv.size(); ++i) {
        writer.writeInt(data_.hisv[i]);
        if ((i + 1) % 8 == 0) {
            result.push_back(writer.getLine());
            writer.clear();
        }
    }

    if (!writer.getLine().empty()) {
        result.push_back(writer.getLine());
    }

    return result;
}

void DefineMaterialHistories::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DefineMaterialHistories, "*DEFINE_MATERIAL_HISTORIES")

// ============================================================================
// DefineAdaptiveSolidToSph
// ============================================================================

bool DefineAdaptiveSolidToSph::parse(const std::vector<std::string>& lines,
                                      util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;

        data_.pid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.fail = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.iopt = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.sphcell = parser.getDoubleAt(pos, realW).value_or(0.0);
        break;
    }

    return true;
}

std::vector<std::string> DefineAdaptiveSolidToSph::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.pid);
    writer.writeDouble(data_.fail);
    writer.writeInt(data_.iopt);
    writer.writeDouble(data_.sphcell);
    result.push_back(writer.getLine());

    return result;
}

void DefineAdaptiveSolidToSph::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DefineAdaptiveSolidToSph, "*DEFINE_ADAPTIVE_SOLID_TO_SPH")

// ============================================================================
// DefineTable2D
// ============================================================================

bool DefineTable2D::parse(const std::vector<std::string>& lines,
                           util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    bool firstLine = true;
    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);

        if (firstLine) {
            data_.id = static_cast<int>(parser.getInt64At(0).value_or(0));
            firstLine = false;
        } else {
            size_t pos = 0;
            double value = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            int curveId = static_cast<int>(parser.getInt64At(pos).value_or(0));
            if (curveId != 0) {
                data_.curves.emplace_back(value, curveId);
            }
        }
    }

    return true;
}

std::vector<std::string> DefineTable2D::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.id);
    result.push_back(writer.getLine());

    for (const auto& curve : data_.curves) {
        writer.clear();
        writer.writeDouble(curve.first);
        writer.writeInt(curve.second);
        result.push_back(writer.getLine());
    }

    return result;
}

void DefineTable2D::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DefineTable2D, "*DEFINE_TABLE_2D")

// ============================================================================
// DefineTable3D
// ============================================================================

bool DefineTable3D::parse(const std::vector<std::string>& lines,
                           util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    bool firstLine = true;
    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);

        if (firstLine) {
            data_.id = static_cast<int>(parser.getInt64At(0).value_or(0));
            firstLine = false;
        } else {
            size_t pos = 0;
            double value = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            int tableId = static_cast<int>(parser.getInt64At(pos).value_or(0));
            if (tableId != 0) {
                data_.tables.emplace_back(value, tableId);
            }
        }
    }

    return true;
}

std::vector<std::string> DefineTable3D::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.id);
    result.push_back(writer.getLine());

    for (const auto& table : data_.tables) {
        writer.clear();
        writer.writeDouble(table.first);
        writer.writeInt(table.second);
        result.push_back(writer.getLine());
    }

    return result;
}

void DefineTable3D::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DefineTable3D, "*DEFINE_TABLE_3D")

// ============================================================================
// DefineDeMeshBeam
// ============================================================================

bool DefineDeMeshBeam::parse(const std::vector<std::string>& lines,
                              util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;

        data_.sid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.pid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.rad = parser.getDoubleAt(pos, realW).value_or(0.0);
        break;
    }

    return true;
}

std::vector<std::string> DefineDeMeshBeam::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.sid);
    writer.writeInt(data_.pid);
    writer.writeDouble(data_.rad);
    result.push_back(writer.getLine());

    return result;
}

void DefineDeMeshBeam::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DefineDeMeshBeam, "*DEFINE_DE_MESH_BEAM")

// ============================================================================
// DefineSpotweldRuptureParameter
// ============================================================================

bool DefineSpotweldRuptureParameter::parse(const std::vector<std::string>& lines,
                                            util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;

        data_.nrr = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.tfail = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.rfail = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.nfail = parser.getDoubleAt(pos, realW).value_or(0.0);
        break;
    }

    return true;
}

std::vector<std::string> DefineSpotweldRuptureParameter::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.nrr);
    writer.writeDouble(data_.tfail);
    writer.writeDouble(data_.rfail);
    writer.writeDouble(data_.nfail);
    result.push_back(writer.getLine());

    return result;
}

void DefineSpotweldRuptureParameter::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DefineSpotweldRuptureParameter, "*DEFINE_SPOTWELD_RUPTURE_PARAMETER")

// ============================================================================
// DefineFormingBlankmesh
// ============================================================================

bool DefineFormingBlankmesh::parse(const std::vector<std::string>& lines,
                                    util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;

        data_.nsid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.size = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.itype = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> DefineFormingBlankmesh::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.nsid);
    writer.writeDouble(data_.size);
    writer.writeInt(data_.itype);
    result.push_back(writer.getLine());

    return result;
}

void DefineFormingBlankmesh::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DefineFormingBlankmesh, "*DEFINE_FORMING_BLANKMESH")

// ============================================================================
// DefinePressureTube
// ============================================================================

bool DefinePressureTube::parse(const std::vector<std::string>& lines,
                                util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;

        data_.id = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.lc = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> DefinePressureTube::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.id);
    writer.writeDouble(data_.lc);
    writer.writeInt(data_.lcid);
    result.push_back(writer.getLine());

    return result;
}

void DefinePressureTube::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DefinePressureTube, "*DEFINE_PRESSURE_TUBE")

// ============================================================================
// DefineSpotweldMultiscale
// ============================================================================

bool DefineSpotweldMultiscale::parse(const std::vector<std::string>& lines,
                                      util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;

        data_.id = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.iopt = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.tfail = parser.getDoubleAt(pos, realW).value_or(0.0);
        break;
    }

    return true;
}

std::vector<std::string> DefineSpotweldMultiscale::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.id);
    writer.writeInt(data_.iopt);
    writer.writeDouble(data_.tfail);
    result.push_back(writer.getLine());

    return result;
}

void DefineSpotweldMultiscale::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DefineSpotweldMultiscale, "*DEFINE_SPOTWELD_MULTISCALE")

// ============================================================================
// DefinePblastAirburst
// ============================================================================

bool DefinePblastAirburst::parse(const std::vector<std::string>& lines,
                                  util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;

        data_.bid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.xb = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.yb = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.zb = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.tob = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.wgt = parser.getDoubleAt(pos, realW).value_or(0.0);
        break;
    }

    return true;
}

std::vector<std::string> DefinePblastAirburst::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.bid);
    writer.writeDouble(data_.xb);
    writer.writeDouble(data_.yb);
    writer.writeDouble(data_.zb);
    writer.writeDouble(data_.tob);
    writer.writeDouble(data_.wgt);
    result.push_back(writer.getLine());

    return result;
}

void DefinePblastAirburst::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DefinePblastAirburst, "*DEFINE_PBLAST_AIRBURST")

// ============================================================================
// DefinePblastSurface
// ============================================================================

bool DefinePblastSurface::parse(const std::vector<std::string>& lines,
                                 util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;

        data_.bid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.xb = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.yb = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.zb = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.tob = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.wgt = parser.getDoubleAt(pos, realW).value_or(0.0);
        break;
    }

    return true;
}

std::vector<std::string> DefinePblastSurface::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.bid);
    writer.writeDouble(data_.xb);
    writer.writeDouble(data_.yb);
    writer.writeDouble(data_.zb);
    writer.writeDouble(data_.tob);
    writer.writeDouble(data_.wgt);
    result.push_back(writer.getLine());

    return result;
}

void DefinePblastSurface::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DefinePblastSurface, "*DEFINE_PBLAST_SURFACE")

// ============================================================================
// DefineFilter
// ============================================================================

bool DefineFilter::parse(const std::vector<std::string>& lines,
                          util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;
        data_.fid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.ftype = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.freq = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.nterm = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> DefineFilter::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.fid);
    writer.writeInt(data_.ftype);
    writer.writeDouble(data_.freq);
    writer.writeInt(data_.nterm);
    result.push_back(writer.getLine());

    return result;
}

void DefineFilter::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DefineFilter, "*DEFINE_FILTER")

// ============================================================================
// DefineDeathTimes
// ============================================================================

bool DefineDeathTimes::parse(const std::vector<std::string>& lines,
                              util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    data_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;

        auto eid = parser.getInt64At(pos);
        if (!eid) continue;

        Data entry;
        entry.eid = static_cast<int>(*eid);
        pos += intW;
        entry.timed = parser.getDoubleAt(pos, realW).value_or(0.0);

        data_.push_back(entry);
    }

    return true;
}

std::vector<std::string> DefineDeathTimes::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& entry : data_) {
        writer.clear();
        writer.writeInt(entry.eid);
        writer.writeDouble(entry.timed);
        result.push_back(writer.getLine());
    }

    return result;
}

void DefineDeathTimes::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DefineDeathTimes, "*DEFINE_DEATH_TIMES")

// ============================================================================
// DefineBirthTimes
// ============================================================================

bool DefineBirthTimes::parse(const std::vector<std::string>& lines,
                              util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    data_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;

        auto eid = parser.getInt64At(pos);
        if (!eid) continue;

        Data entry;
        entry.eid = static_cast<int>(*eid);
        pos += intW;
        entry.timeb = parser.getDoubleAt(pos, realW).value_or(0.0);

        data_.push_back(entry);
    }

    return true;
}

std::vector<std::string> DefineBirthTimes::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& entry : data_) {
        writer.clear();
        writer.writeInt(entry.eid);
        writer.writeDouble(entry.timeb);
        result.push_back(writer.getLine());
    }

    return result;
}

void DefineBirthTimes::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DefineBirthTimes, "*DEFINE_BIRTH_TIMES")

// ============================================================================
// DefineGroundMotion
// ============================================================================

bool DefineGroundMotion::parse(const std::vector<std::string>& lines,
                                util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;
        data_.gmid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.sf = parser.getDoubleAt(pos, realW).value_or(1.0);
        pos += realW;
        data_.idir = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> DefineGroundMotion::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.gmid);
    writer.writeInt(data_.lcid);
    writer.writeDouble(data_.sf);
    writer.writeInt(data_.idir);
    result.push_back(writer.getLine());

    return result;
}

void DefineGroundMotion::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DefineGroundMotion, "*DEFINE_GROUND_MOTION")

// ============================================================================
// DefineLoadBlastSegment
// ============================================================================

bool DefineLoadBlastSegment::parse(const std::vector<std::string>& lines,
                                    util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;
        data_.bid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.ssid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.sf = parser.getDoubleAt(pos, realW).value_or(1.0);
        break;
    }

    return true;
}

std::vector<std::string> DefineLoadBlastSegment::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.bid);
    writer.writeInt(data_.ssid);
    writer.writeDouble(data_.sf);
    result.push_back(writer.getLine());

    return result;
}

void DefineLoadBlastSegment::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DefineLoadBlastSegment, "*DEFINE_LOAD_BLAST_SEGMENT")

// ============================================================================
// DefinePorousChemistry
// ============================================================================

bool DefinePorousChemistry::parse(const std::vector<std::string>& lines,
                                   util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;
        data_.pcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.nspec = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.por = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.tort = parser.getDoubleAt(pos, realW).value_or(1.0);
        break;
    }

    return true;
}

std::vector<std::string> DefinePorousChemistry::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.pcid);
    writer.writeInt(data_.nspec);
    writer.writeDouble(data_.por);
    writer.writeDouble(data_.tort);
    result.push_back(writer.getLine());

    return result;
}

void DefinePorousChemistry::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DefinePorousChemistry, "*DEFINE_POROUS_CHEMISTRY")

// ============================================================================
// DefineStochasticVariation
// ============================================================================

bool DefineStochasticVariation::parse(const std::vector<std::string>& lines,
                                       util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;
        data_.svid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.distrib = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.mean = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.stddev = parser.getDoubleAt(pos, realW).value_or(0.0);
        break;
    }

    return true;
}

std::vector<std::string> DefineStochasticVariation::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.svid);
    writer.writeInt(data_.distrib);
    writer.writeDouble(data_.mean);
    writer.writeDouble(data_.stddev);
    result.push_back(writer.getLine());

    return result;
}

void DefineStochasticVariation::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DefineStochasticVariation, "*DEFINE_STOCHASTIC_VARIATION")

// ============================================================================
// DefineHazProperties
// ============================================================================

bool DefineHazProperties::parse(const std::vector<std::string>& lines,
                                 util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;
        data_.hazid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.width = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.sf = parser.getDoubleAt(pos, realW).value_or(1.0);
        break;
    }

    return true;
}

std::vector<std::string> DefineHazProperties::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.hazid);
    writer.writeDouble(data_.width);
    writer.writeDouble(data_.sf);
    result.push_back(writer.getLine());

    return result;
}

void DefineHazProperties::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DefineHazProperties, "*DEFINE_HAZ_PROPERTIES")

// ============================================================================
// DefineCarpetPlot
// ============================================================================

bool DefineCarpetPlot::parse(const std::vector<std::string>& lines,
                              util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;
        data_.cpid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.np1 = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.np2 = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.min1 = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.max1 = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.min2 = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.max2 = parser.getDoubleAt(pos, realW).value_or(0.0);
        break;
    }

    return true;
}

std::vector<std::string> DefineCarpetPlot::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.cpid);
    writer.writeInt(data_.np1);
    writer.writeInt(data_.np2);
    writer.writeDouble(data_.min1);
    writer.writeDouble(data_.max1);
    writer.writeDouble(data_.min2);
    writer.writeDouble(data_.max2);
    result.push_back(writer.getLine());

    return result;
}

void DefineCarpetPlot::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DefineCarpetPlot, "*DEFINE_CARPET_PLOT")

// ============================================================================
// DefineTrimSeed
// ============================================================================

bool DefineTrimSeed::parse(const std::vector<std::string>& lines,
                            util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;
        data_.tsid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.x = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.y = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.z = parser.getDoubleAt(pos, realW).value_or(0.0);
        break;
    }

    return true;
}

std::vector<std::string> DefineTrimSeed::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.tsid);
    writer.writeDouble(data_.x);
    writer.writeDouble(data_.y);
    writer.writeDouble(data_.z);
    result.push_back(writer.getLine());

    return result;
}

void DefineTrimSeed::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DefineTrimSeed, "*DEFINE_TRIM_SEED")

// ============================================================================
// DefineRegion
// ============================================================================

bool DefineRegion::parse(const std::vector<std::string>& lines,
                          util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;
        data_.rid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.xmin = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.xmax = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.ymin = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.ymax = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.zmin = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.zmax = parser.getDoubleAt(pos, realW).value_or(0.0);
        break;
    }

    return true;
}

std::vector<std::string> DefineRegion::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.rid);
    writer.writeDouble(data_.xmin);
    writer.writeDouble(data_.xmax);
    writer.writeDouble(data_.ymin);
    writer.writeDouble(data_.ymax);
    writer.writeDouble(data_.zmin);
    writer.writeDouble(data_.zmax);
    result.push_back(writer.getLine());

    return result;
}

void DefineRegion::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DefineRegion, "*DEFINE_REGION")

// ============================================================================
// DefineCurveCompensated
// ============================================================================

bool DefineCurveCompensated::parse(const std::vector<std::string>& lines,
                                    util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;
        data_.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.sidr = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.sfa = parser.getDoubleAt(pos, realW).value_or(1.0);
        pos += realW;
        data_.sfo = parser.getDoubleAt(pos, realW).value_or(1.0);
        pos += realW;
        data_.cfac = parser.getDoubleAt(pos, realW).value_or(1.0);
        break;
    }

    return true;
}

std::vector<std::string> DefineCurveCompensated::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.lcid);
    writer.writeInt(data_.sidr);
    writer.writeDouble(data_.sfa);
    writer.writeDouble(data_.sfo);
    writer.writeDouble(data_.cfac);
    result.push_back(writer.getLine());

    return result;
}

void DefineCurveCompensated::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DefineCurveCompensated, "*DEFINE_CURVE_COMPENSATED")

// ============================================================================
// DefineCurveEntity
// ============================================================================

bool DefineCurveEntity::parse(const std::vector<std::string>& lines,
                               util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;
        data_.ceid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.ctype = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.np = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> DefineCurveEntity::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.ceid);
    writer.writeInt(data_.ctype);
    writer.writeInt(data_.np);
    result.push_back(writer.getLine());

    return result;
}

void DefineCurveEntity::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DefineCurveEntity, "*DEFINE_CURVE_ENTITY")

// ============================================================================
// DefineCurveDuplicate
// ============================================================================

bool DefineCurveDuplicate::parse(const std::vector<std::string>& lines,
                                  util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;
        data_.newlcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.oldlcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.sfa = parser.getDoubleAt(pos, realW).value_or(1.0);
        pos += realW;
        data_.sfo = parser.getDoubleAt(pos, realW).value_or(1.0);
        break;
    }

    return true;
}

std::vector<std::string> DefineCurveDuplicate::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.newlcid);
    writer.writeInt(data_.oldlcid);
    writer.writeDouble(data_.sfa);
    writer.writeDouble(data_.sfo);
    result.push_back(writer.getLine());

    return result;
}

void DefineCurveDuplicate::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DefineCurveDuplicate, "*DEFINE_CURVE_DUPLICATE")

// ============================================================================
// DefineCurveTrim2d
// ============================================================================

bool DefineCurveTrim2d::parse(const std::vector<std::string>& lines,
                               util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;
        data_.tcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.type = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.np = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> DefineCurveTrim2d::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.tcid);
    writer.writeInt(data_.type);
    writer.writeInt(data_.np);
    result.push_back(writer.getLine());

    return result;
}

void DefineCurveTrim2d::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DefineCurveTrim2d, "*DEFINE_CURVE_TRIM_2D")

// ============================================================================
// DefineCurveTrim3d
// ============================================================================

bool DefineCurveTrim3d::parse(const std::vector<std::string>& lines,
                               util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;
        data_.tcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.type = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.np = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> DefineCurveTrim3d::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.tcid);
    writer.writeInt(data_.type);
    writer.writeInt(data_.np);
    result.push_back(writer.getLine());

    return result;
}

void DefineCurveTrim3d::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DefineCurveTrim3d, "*DEFINE_CURVE_TRIM_3D")

} // namespace koo::dyna
