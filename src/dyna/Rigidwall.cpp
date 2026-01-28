#include <koo/dyna/Rigidwall.hpp>
#include <koo/dyna/ModelVisitor.hpp>
#include <koo/dyna/KeywordFactory.hpp>
#include <koo/util/StringUtils.hpp>

namespace koo::dyna {

// ============================================================================
// RigidwallPlanar
// ============================================================================

bool RigidwallPlanar::parse(const std::vector<std::string>& lines,
                             util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    // Skip comments
    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: NSID, NSIDEX, BOXID
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.nsid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.nsidex = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.boxid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    ++lineIdx;

    // Skip comments
    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 2: XT, YT, ZT, XH, YH, ZH, FRIC, WVEL
    parser.setLine(lines[lineIdx]);
    pos = 0;

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
    data_.fric = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.wvel = parser.getDoubleAt(pos, realW).value_or(0.0);

    return true;
}

std::vector<std::string> RigidwallPlanar::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    // Card 1
    writer.writeInt(data_.nsid);
    writer.writeInt(data_.nsidex);
    writer.writeInt(data_.boxid);
    result.push_back(writer.getLine());

    // Card 2
    writer.clear();
    writer.writeDouble(data_.xt);
    writer.writeDouble(data_.yt);
    writer.writeDouble(data_.zt);
    writer.writeDouble(data_.xh);
    writer.writeDouble(data_.yh);
    writer.writeDouble(data_.zh);
    writer.writeDouble(data_.fric);
    writer.writeDouble(data_.wvel);
    result.push_back(writer.getLine());

    return result;
}

void RigidwallPlanar::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(RigidwallPlanar, "*RIGIDWALL_PLANAR")

// ============================================================================
// RigidwallGeometricFlat
// ============================================================================

bool RigidwallGeometricFlat::parse(const std::vector<std::string>& lines,
                                    util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: ID, NSID, NSIDEX, BOXID, BIRTH, DEATH
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.id = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.nsid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.nsidex = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.boxid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.birth = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.death = parser.getDoubleAt(pos, realW).value_or(1e20);
    ++lineIdx;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 2: XT, YT, ZT, XH, YH, ZH, FRIC
    parser.setLine(lines[lineIdx]);
    pos = 0;

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
    data_.fric = parser.getDoubleAt(pos, realW).value_or(0.0);
    ++lineIdx;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 3: LENL, LENM (optional)
    parser.setLine(lines[lineIdx]);
    pos = 0;

    data_.lenl = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.lenm = parser.getDoubleAt(pos, realW).value_or(0.0);

    return true;
}

std::vector<std::string> RigidwallGeometricFlat::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    // Card 1
    writer.writeInt(data_.id);
    writer.writeInt(data_.nsid);
    writer.writeInt(data_.nsidex);
    writer.writeInt(data_.boxid);
    writer.writeDouble(data_.birth);
    writer.writeDouble(data_.death);
    result.push_back(writer.getLine());

    // Card 2
    writer.clear();
    writer.writeDouble(data_.xt);
    writer.writeDouble(data_.yt);
    writer.writeDouble(data_.zt);
    writer.writeDouble(data_.xh);
    writer.writeDouble(data_.yh);
    writer.writeDouble(data_.zh);
    writer.writeDouble(data_.fric);
    result.push_back(writer.getLine());

    // Card 3
    writer.clear();
    writer.writeDouble(data_.lenl);
    writer.writeDouble(data_.lenm);
    result.push_back(writer.getLine());

    return result;
}

void RigidwallGeometricFlat::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(RigidwallGeometricFlat, "*RIGIDWALL_GEOMETRIC_FLAT")

// ============================================================================
// RigidwallGeometricCylinder
// ============================================================================

bool RigidwallGeometricCylinder::parse(const std::vector<std::string>& lines,
                                        util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.id = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.nsid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.nsidex = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.boxid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.birth = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.death = parser.getDoubleAt(pos, realW).value_or(1e20);
    ++lineIdx;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 2
    parser.setLine(lines[lineIdx]);
    pos = 0;

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
    data_.fric = parser.getDoubleAt(pos, realW).value_or(0.0);
    ++lineIdx;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 3
    parser.setLine(lines[lineIdx]);
    pos = 0;

    data_.radius = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.length = parser.getDoubleAt(pos, realW).value_or(0.0);

    return true;
}

std::vector<std::string> RigidwallGeometricCylinder::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    // Card 1
    writer.writeInt(data_.id);
    writer.writeInt(data_.nsid);
    writer.writeInt(data_.nsidex);
    writer.writeInt(data_.boxid);
    writer.writeDouble(data_.birth);
    writer.writeDouble(data_.death);
    result.push_back(writer.getLine());

    // Card 2
    writer.clear();
    writer.writeDouble(data_.xt);
    writer.writeDouble(data_.yt);
    writer.writeDouble(data_.zt);
    writer.writeDouble(data_.xh);
    writer.writeDouble(data_.yh);
    writer.writeDouble(data_.zh);
    writer.writeDouble(data_.fric);
    result.push_back(writer.getLine());

    // Card 3
    writer.clear();
    writer.writeDouble(data_.radius);
    writer.writeDouble(data_.length);
    result.push_back(writer.getLine());

    return result;
}

void RigidwallGeometricCylinder::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(RigidwallGeometricCylinder, "*RIGIDWALL_GEOMETRIC_CYLINDER")

// ============================================================================
// RigidwallGeometricSphere
// ============================================================================

bool RigidwallGeometricSphere::parse(const std::vector<std::string>& lines,
                                      util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.id = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.nsid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.nsidex = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.boxid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.birth = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.death = parser.getDoubleAt(pos, realW).value_or(1e20);
    ++lineIdx;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 2
    parser.setLine(lines[lineIdx]);
    pos = 0;

    data_.xc = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.yc = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.zc = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.radius = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.fric = parser.getDoubleAt(pos, realW).value_or(0.0);

    return true;
}

std::vector<std::string> RigidwallGeometricSphere::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    // Card 1
    writer.writeInt(data_.id);
    writer.writeInt(data_.nsid);
    writer.writeInt(data_.nsidex);
    writer.writeInt(data_.boxid);
    writer.writeDouble(data_.birth);
    writer.writeDouble(data_.death);
    result.push_back(writer.getLine());

    // Card 2
    writer.clear();
    writer.writeDouble(data_.xc);
    writer.writeDouble(data_.yc);
    writer.writeDouble(data_.zc);
    writer.writeDouble(data_.radius);
    writer.writeDouble(data_.fric);
    result.push_back(writer.getLine());

    return result;
}

void RigidwallGeometricSphere::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(RigidwallGeometricSphere, "*RIGIDWALL_GEOMETRIC_SPHERE")

// ============================================================================
// RigidwallPlanarMoving
// ============================================================================

bool RigidwallPlanarMoving::parse(const std::vector<std::string>& lines,
                                   util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: NSID, NSIDEX, BOXID
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.nsid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.nsidex = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.boxid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    ++lineIdx;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 2: XT, YT, ZT, XH, YH, ZH, FRIC
    parser.setLine(lines[lineIdx]);
    pos = 0;

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
    data_.fric = parser.getDoubleAt(pos, realW).value_or(0.0);
    ++lineIdx;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 3: VX, VY, VZ
    parser.setLine(lines[lineIdx]);
    pos = 0;

    data_.vx = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.vy = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.vz = parser.getDoubleAt(pos, realW).value_or(0.0);

    return true;
}

std::vector<std::string> RigidwallPlanarMoving::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    // Card 1
    writer.writeInt(data_.nsid);
    writer.writeInt(data_.nsidex);
    writer.writeInt(data_.boxid);
    result.push_back(writer.getLine());

    // Card 2
    writer.clear();
    writer.writeDouble(data_.xt);
    writer.writeDouble(data_.yt);
    writer.writeDouble(data_.zt);
    writer.writeDouble(data_.xh);
    writer.writeDouble(data_.yh);
    writer.writeDouble(data_.zh);
    writer.writeDouble(data_.fric);
    result.push_back(writer.getLine());

    // Card 3
    writer.clear();
    writer.writeDouble(data_.vx);
    writer.writeDouble(data_.vy);
    writer.writeDouble(data_.vz);
    result.push_back(writer.getLine());

    return result;
}

void RigidwallPlanarMoving::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(RigidwallPlanarMoving, "*RIGIDWALL_PLANAR_MOVING")

// ============================================================================
// RigidwallPlanarFinite
// ============================================================================

bool RigidwallPlanarFinite::parse(const std::vector<std::string>& lines,
                                   util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.nsid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.nsidex = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.boxid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    ++lineIdx;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 2
    parser.setLine(lines[lineIdx]);
    pos = 0;

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
    data_.fric = parser.getDoubleAt(pos, realW).value_or(0.0);
    ++lineIdx;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 3
    parser.setLine(lines[lineIdx]);
    pos = 0;

    data_.lenl = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.lenm = parser.getDoubleAt(pos, realW).value_or(0.0);

    return true;
}

std::vector<std::string> RigidwallPlanarFinite::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    // Card 1
    writer.writeInt(data_.nsid);
    writer.writeInt(data_.nsidex);
    writer.writeInt(data_.boxid);
    result.push_back(writer.getLine());

    // Card 2
    writer.clear();
    writer.writeDouble(data_.xt);
    writer.writeDouble(data_.yt);
    writer.writeDouble(data_.zt);
    writer.writeDouble(data_.xh);
    writer.writeDouble(data_.yh);
    writer.writeDouble(data_.zh);
    writer.writeDouble(data_.fric);
    result.push_back(writer.getLine());

    // Card 3
    writer.clear();
    writer.writeDouble(data_.lenl);
    writer.writeDouble(data_.lenm);
    result.push_back(writer.getLine());

    return result;
}

void RigidwallPlanarFinite::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(RigidwallPlanarFinite, "*RIGIDWALL_PLANAR_FINITE")

// ============================================================================
// RigidwallPlanarForces
// ============================================================================

bool RigidwallPlanarForces::parse(const std::vector<std::string>& lines,
                                   util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.nsid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.nsidex = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.boxid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    ++lineIdx;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 2
    parser.setLine(lines[lineIdx]);
    pos = 0;

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
    data_.fric = parser.getDoubleAt(pos, realW).value_or(0.0);

    return true;
}

std::vector<std::string> RigidwallPlanarForces::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    // Card 1
    writer.writeInt(data_.nsid);
    writer.writeInt(data_.nsidex);
    writer.writeInt(data_.boxid);
    result.push_back(writer.getLine());

    // Card 2
    writer.clear();
    writer.writeDouble(data_.xt);
    writer.writeDouble(data_.yt);
    writer.writeDouble(data_.zt);
    writer.writeDouble(data_.xh);
    writer.writeDouble(data_.yh);
    writer.writeDouble(data_.zh);
    writer.writeDouble(data_.fric);
    result.push_back(writer.getLine());

    return result;
}

void RigidwallPlanarForces::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(RigidwallPlanarForces, "*RIGIDWALL_PLANAR_FORCES")

// ============================================================================
// RigidwallGeometricCone
// ============================================================================

bool RigidwallGeometricCone::parse(const std::vector<std::string>& lines,
                                    util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.id = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.nsid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.nsidex = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.boxid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.birth = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.death = parser.getDoubleAt(pos, realW).value_or(1e20);
    ++lineIdx;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 2
    parser.setLine(lines[lineIdx]);
    pos = 0;

    data_.xc = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.yc = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.zc = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.xh = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.yh = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.zh = parser.getDoubleAt(pos, realW).value_or(0.0);
    ++lineIdx;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 3
    parser.setLine(lines[lineIdx]);
    pos = 0;

    data_.angle = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.fric = parser.getDoubleAt(pos, realW).value_or(0.0);

    return true;
}

std::vector<std::string> RigidwallGeometricCone::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    // Card 1
    writer.writeInt(data_.id);
    writer.writeInt(data_.nsid);
    writer.writeInt(data_.nsidex);
    writer.writeInt(data_.boxid);
    writer.writeDouble(data_.birth);
    writer.writeDouble(data_.death);
    result.push_back(writer.getLine());

    // Card 2
    writer.clear();
    writer.writeDouble(data_.xc);
    writer.writeDouble(data_.yc);
    writer.writeDouble(data_.zc);
    writer.writeDouble(data_.xh);
    writer.writeDouble(data_.yh);
    writer.writeDouble(data_.zh);
    result.push_back(writer.getLine());

    // Card 3
    writer.clear();
    writer.writeDouble(data_.angle);
    writer.writeDouble(data_.fric);
    result.push_back(writer.getLine());

    return result;
}

void RigidwallGeometricCone::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(RigidwallGeometricCone, "*RIGIDWALL_GEOMETRIC_CONE")

// ============================================================================
// RigidwallPlanarOrtho
// ============================================================================

bool RigidwallPlanarOrtho::parse(const std::vector<std::string>& lines,
                                  util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.nsid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.nsidex = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.boxid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    ++lineIdx;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 2
    parser.setLine(lines[lineIdx]);
    pos = 0;

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
    data_.fricl = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.fricm = parser.getDoubleAt(pos, realW).value_or(0.0);

    return true;
}

std::vector<std::string> RigidwallPlanarOrtho::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    // Card 1
    writer.writeInt(data_.nsid);
    writer.writeInt(data_.nsidex);
    writer.writeInt(data_.boxid);
    result.push_back(writer.getLine());

    // Card 2
    writer.clear();
    writer.writeDouble(data_.xt);
    writer.writeDouble(data_.yt);
    writer.writeDouble(data_.zt);
    writer.writeDouble(data_.xh);
    writer.writeDouble(data_.yh);
    writer.writeDouble(data_.zh);
    writer.writeDouble(data_.fricl);
    writer.writeDouble(data_.fricm);
    result.push_back(writer.getLine());

    return result;
}

void RigidwallPlanarOrtho::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(RigidwallPlanarOrtho, "*RIGIDWALL_PLANAR_ORTHO")

// ============================================================================
// RigidwallGeometricPrism
// ============================================================================

bool RigidwallGeometricPrism::parse(const std::vector<std::string>& lines,
                                     util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.id = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.nsid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.nsidex = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.boxid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.birth = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.death = parser.getDoubleAt(pos, realW).value_or(1e20);
    ++lineIdx;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 2
    parser.setLine(lines[lineIdx]);
    pos = 0;

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
    data_.fric = parser.getDoubleAt(pos, realW).value_or(0.0);
    ++lineIdx;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 3
    parser.setLine(lines[lineIdx]);
    pos = 0;

    data_.lenl = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.lenm = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.height = parser.getDoubleAt(pos, realW).value_or(0.0);

    return true;
}

std::vector<std::string> RigidwallGeometricPrism::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    // Card 1
    writer.writeInt(data_.id);
    writer.writeInt(data_.nsid);
    writer.writeInt(data_.nsidex);
    writer.writeInt(data_.boxid);
    writer.writeDouble(data_.birth);
    writer.writeDouble(data_.death);
    result.push_back(writer.getLine());

    // Card 2
    writer.clear();
    writer.writeDouble(data_.xt);
    writer.writeDouble(data_.yt);
    writer.writeDouble(data_.zt);
    writer.writeDouble(data_.xh);
    writer.writeDouble(data_.yh);
    writer.writeDouble(data_.zh);
    writer.writeDouble(data_.fric);
    result.push_back(writer.getLine());

    // Card 3
    writer.clear();
    writer.writeDouble(data_.lenl);
    writer.writeDouble(data_.lenm);
    writer.writeDouble(data_.height);
    result.push_back(writer.getLine());

    return result;
}

void RigidwallGeometricPrism::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(RigidwallGeometricPrism, "*RIGIDWALL_GEOMETRIC_PRISM")

// ============================================================================
// RigidwallGeometricTorus
// ============================================================================

bool RigidwallGeometricTorus::parse(const std::vector<std::string>& lines,
                                     util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.id = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.nsid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.nsidex = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.boxid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.birth = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.death = parser.getDoubleAt(pos, realW).value_or(1e20);
    ++lineIdx;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 2
    parser.setLine(lines[lineIdx]);
    pos = 0;

    data_.xc = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.yc = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.zc = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.rMajor = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.rMinor = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.fric = parser.getDoubleAt(pos, realW).value_or(0.0);

    return true;
}

std::vector<std::string> RigidwallGeometricTorus::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    // Card 1
    writer.writeInt(data_.id);
    writer.writeInt(data_.nsid);
    writer.writeInt(data_.nsidex);
    writer.writeInt(data_.boxid);
    writer.writeDouble(data_.birth);
    writer.writeDouble(data_.death);
    result.push_back(writer.getLine());

    // Card 2
    writer.clear();
    writer.writeDouble(data_.xc);
    writer.writeDouble(data_.yc);
    writer.writeDouble(data_.zc);
    writer.writeDouble(data_.rMajor);
    writer.writeDouble(data_.rMinor);
    writer.writeDouble(data_.fric);
    result.push_back(writer.getLine());

    return result;
}

void RigidwallGeometricTorus::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(RigidwallGeometricTorus, "*RIGIDWALL_GEOMETRIC_TORUS")

// ============================================================================
// RigidwallPlanarId
// ============================================================================

bool RigidwallPlanarId::parse(const std::vector<std::string>& lines,
                               util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: ID, TITLE
    parser.setLine(lines[lineIdx]);
    data_.id = static_cast<int>(parser.getInt64At(0).value_or(0));
    if (lines[lineIdx].size() > intW) {
        data_.title = util::StringUtils::trim(lines[lineIdx].substr(intW));
    }
    ++lineIdx;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 2: NSID, NSIDEX, BOXID, BIRTH, DEATH
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.nsid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.nsidex = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.boxid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.birth = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.death = parser.getDoubleAt(pos, realW).value_or(1e20);
    ++lineIdx;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 3: XT, YT, ZT, XH, YH, ZH, FRIC, WVEL
    parser.setLine(lines[lineIdx]);
    pos = 0;

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
    data_.fric = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.wvel = parser.getDoubleAt(pos, realW).value_or(0.0);

    return true;
}

std::vector<std::string> RigidwallPlanarId::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    // Card 1
    writer.writeInt(data_.id);
    std::string line1 = writer.getLine();
    if (!data_.title.empty()) {
        line1 += data_.title;
    }
    result.push_back(line1);

    // Card 2
    writer.clear();
    writer.writeInt(data_.nsid);
    writer.writeInt(data_.nsidex);
    writer.writeInt(data_.boxid);
    writer.writeDouble(data_.birth);
    writer.writeDouble(data_.death);
    result.push_back(writer.getLine());

    // Card 3
    writer.clear();
    writer.writeDouble(data_.xt);
    writer.writeDouble(data_.yt);
    writer.writeDouble(data_.zt);
    writer.writeDouble(data_.xh);
    writer.writeDouble(data_.yh);
    writer.writeDouble(data_.zh);
    writer.writeDouble(data_.fric);
    writer.writeDouble(data_.wvel);
    result.push_back(writer.getLine());

    return result;
}

void RigidwallPlanarId::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(RigidwallPlanarId, "*RIGIDWALL_PLANAR_ID")

// ============================================================================
// RigidwallPlanarMovingForces
// ============================================================================

bool RigidwallPlanarMovingForces::parse(const std::vector<std::string>& lines,
                                         util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.nsid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.nsidex = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.boxid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    ++lineIdx;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 2
    parser.setLine(lines[lineIdx]);
    pos = 0;

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
    data_.fric = parser.getDoubleAt(pos, realW).value_or(0.0);
    ++lineIdx;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 3
    parser.setLine(lines[lineIdx]);
    pos = 0;

    data_.vx = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.vy = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.vz = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));

    return true;
}

std::vector<std::string> RigidwallPlanarMovingForces::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    // Card 1
    writer.writeInt(data_.nsid);
    writer.writeInt(data_.nsidex);
    writer.writeInt(data_.boxid);
    result.push_back(writer.getLine());

    // Card 2
    writer.clear();
    writer.writeDouble(data_.xt);
    writer.writeDouble(data_.yt);
    writer.writeDouble(data_.zt);
    writer.writeDouble(data_.xh);
    writer.writeDouble(data_.yh);
    writer.writeDouble(data_.zh);
    writer.writeDouble(data_.fric);
    result.push_back(writer.getLine());

    // Card 3
    writer.clear();
    writer.writeDouble(data_.vx);
    writer.writeDouble(data_.vy);
    writer.writeDouble(data_.vz);
    writer.writeInt(data_.lcid);
    result.push_back(writer.getLine());

    return result;
}

void RigidwallPlanarMovingForces::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(RigidwallPlanarMovingForces, "*RIGIDWALL_PLANAR_MOVING_FORCES")

// ============================================================================
// RigidwallGeometricCurved
// ============================================================================

bool RigidwallGeometricCurved::parse(const std::vector<std::string>& lines,
                                      util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.id = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.nsid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.nsidex = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.boxid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.birth = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.death = parser.getDoubleAt(pos, realW).value_or(1e20);
    ++lineIdx;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 2
    parser.setLine(lines[lineIdx]);
    pos = 0;

    data_.curveid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.fric = parser.getDoubleAt(pos, realW).value_or(0.0);

    return true;
}

std::vector<std::string> RigidwallGeometricCurved::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    // Card 1
    writer.writeInt(data_.id);
    writer.writeInt(data_.nsid);
    writer.writeInt(data_.nsidex);
    writer.writeInt(data_.boxid);
    writer.writeDouble(data_.birth);
    writer.writeDouble(data_.death);
    result.push_back(writer.getLine());

    // Card 2
    writer.clear();
    writer.writeInt(data_.curveid);
    writer.writeDouble(data_.fric);
    result.push_back(writer.getLine());

    return result;
}

void RigidwallGeometricCurved::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(RigidwallGeometricCurved, "*RIGIDWALL_GEOMETRIC_CURVED")

// ============================================================================
// RigidwallPlanarFiniteId
// ============================================================================

bool RigidwallPlanarFiniteId::parse(const std::vector<std::string>& lines,
                                     util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: ID, TITLE
    parser.setLine(lines[lineIdx]);
    data_.id = static_cast<int>(parser.getInt64At(0).value_or(0));
    if (lines[lineIdx].size() > intW) {
        data_.title = util::StringUtils::trim(lines[lineIdx].substr(intW));
    }
    ++lineIdx;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 2
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.nsid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.nsidex = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.boxid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.birth = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.death = parser.getDoubleAt(pos, realW).value_or(1e20);
    ++lineIdx;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 3
    parser.setLine(lines[lineIdx]);
    pos = 0;

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
    data_.fric = parser.getDoubleAt(pos, realW).value_or(0.0);
    ++lineIdx;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 4
    parser.setLine(lines[lineIdx]);
    pos = 0;

    data_.lenl = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.lenm = parser.getDoubleAt(pos, realW).value_or(0.0);

    return true;
}

std::vector<std::string> RigidwallPlanarFiniteId::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    // Card 1
    writer.writeInt(data_.id);
    std::string line1 = writer.getLine();
    if (!data_.title.empty()) {
        line1 += data_.title;
    }
    result.push_back(line1);

    // Card 2
    writer.clear();
    writer.writeInt(data_.nsid);
    writer.writeInt(data_.nsidex);
    writer.writeInt(data_.boxid);
    writer.writeDouble(data_.birth);
    writer.writeDouble(data_.death);
    result.push_back(writer.getLine());

    // Card 3
    writer.clear();
    writer.writeDouble(data_.xt);
    writer.writeDouble(data_.yt);
    writer.writeDouble(data_.zt);
    writer.writeDouble(data_.xh);
    writer.writeDouble(data_.yh);
    writer.writeDouble(data_.zh);
    writer.writeDouble(data_.fric);
    result.push_back(writer.getLine());

    // Card 4
    writer.clear();
    writer.writeDouble(data_.lenl);
    writer.writeDouble(data_.lenm);
    result.push_back(writer.getLine());

    return result;
}

void RigidwallPlanarFiniteId::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(RigidwallPlanarFiniteId, "*RIGIDWALL_PLANAR_FINITE_ID")

// ============================================================================
// RigidwallGeometricMotion
// ============================================================================

bool RigidwallGeometricMotion::parse(const std::vector<std::string>& lines,
                                      util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.id = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.lcidvx = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.lcidvy = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.lcidvz = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.lcidax = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.lciday = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.lcidaz = static_cast<int>(parser.getInt64At(pos).value_or(0));
    ++lineIdx;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 2
    parser.setLine(lines[lineIdx]);
    pos = 0;

    data_.sfvx = parser.getDoubleAt(pos, realW).value_or(1.0);
    pos += realW;
    data_.sfvy = parser.getDoubleAt(pos, realW).value_or(1.0);
    pos += realW;
    data_.sfvz = parser.getDoubleAt(pos, realW).value_or(1.0);

    return true;
}

std::vector<std::string> RigidwallGeometricMotion::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    // Card 1
    writer.writeInt(data_.id);
    writer.writeInt(data_.lcidvx);
    writer.writeInt(data_.lcidvy);
    writer.writeInt(data_.lcidvz);
    writer.writeInt(data_.lcidax);
    writer.writeInt(data_.lciday);
    writer.writeInt(data_.lcidaz);
    result.push_back(writer.getLine());

    // Card 2
    writer.clear();
    writer.writeDouble(data_.sfvx);
    writer.writeDouble(data_.sfvy);
    writer.writeDouble(data_.sfvz);
    result.push_back(writer.getLine());

    return result;
}

void RigidwallGeometricMotion::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(RigidwallGeometricMotion, "*RIGIDWALL_GEOMETRIC_MOTION")

// ============================================================================
// RigidwallPlanarMovingFinite
// ============================================================================

bool RigidwallPlanarMovingFinite::parse(const std::vector<std::string>& lines,
                                         util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.nsid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.nsidex = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.boxid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    ++lineIdx;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 2
    parser.setLine(lines[lineIdx]);
    pos = 0;

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
    data_.fric = parser.getDoubleAt(pos, realW).value_or(0.0);
    ++lineIdx;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 3
    parser.setLine(lines[lineIdx]);
    pos = 0;

    data_.lenl = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.lenm = parser.getDoubleAt(pos, realW).value_or(0.0);
    ++lineIdx;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 4
    parser.setLine(lines[lineIdx]);
    pos = 0;

    data_.vx = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.vy = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.vz = parser.getDoubleAt(pos, realW).value_or(0.0);

    return true;
}

std::vector<std::string> RigidwallPlanarMovingFinite::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    // Card 1
    writer.writeInt(data_.nsid);
    writer.writeInt(data_.nsidex);
    writer.writeInt(data_.boxid);
    result.push_back(writer.getLine());

    // Card 2
    writer.clear();
    writer.writeDouble(data_.xt);
    writer.writeDouble(data_.yt);
    writer.writeDouble(data_.zt);
    writer.writeDouble(data_.xh);
    writer.writeDouble(data_.yh);
    writer.writeDouble(data_.zh);
    writer.writeDouble(data_.fric);
    result.push_back(writer.getLine());

    // Card 3
    writer.clear();
    writer.writeDouble(data_.lenl);
    writer.writeDouble(data_.lenm);
    result.push_back(writer.getLine());

    // Card 4
    writer.clear();
    writer.writeDouble(data_.vx);
    writer.writeDouble(data_.vy);
    writer.writeDouble(data_.vz);
    result.push_back(writer.getLine());

    return result;
}

void RigidwallPlanarMovingFinite::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(RigidwallPlanarMovingFinite, "*RIGIDWALL_PLANAR_MOVING_FINITE")

} // namespace koo::dyna
