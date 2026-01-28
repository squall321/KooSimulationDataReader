#include <koo/dyna/Dualcese.hpp>
#include <koo/dyna/ModelVisitor.hpp>
#include <koo/dyna/KeywordFactory.hpp>
#include <koo/util/StringUtils.hpp>

namespace koo::dyna {

// ============================================================================
// DualceseControlTimestep
// ============================================================================

bool DualceseControlTimestep::parse(const std::vector<std::string>& lines,
                                     util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: IDDT, CFL, DTINIT, DTMIN, DTMAX
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.iddt = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.cfl = parser.getDoubleAt(pos, realW).value_or(0.9);
    pos += realW;
    data_.dtinit = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.dtmin = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.dtmax = parser.getDoubleAt(pos, realW).value_or(0.0);

    return true;
}

std::vector<std::string> DualceseControlTimestep::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.iddt);
    writer.writeDouble(data_.cfl);
    writer.writeDouble(data_.dtinit);
    writer.writeDouble(data_.dtmin);
    writer.writeDouble(data_.dtmax);
    result.push_back(writer.getLine());

    return result;
}

void DualceseControlTimestep::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DualceseControlTimestep, "*DUALCESE_CONTROL_TIMESTEP")

// ============================================================================
// DualceseControlGeneral
// ============================================================================

bool DualceseControlGeneral::parse(const std::vector<std::string>& lines,
                                    util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: IDMETH, IMESH, IBC, ICHEM, ITURB
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.idmeth = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.imesh = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.ibc = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.ichem = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.iturb = static_cast<int>(parser.getInt64At(pos).value_or(0));

    return true;
}

std::vector<std::string> DualceseControlGeneral::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.idmeth);
    writer.writeInt(data_.imesh);
    writer.writeInt(data_.ibc);
    writer.writeInt(data_.ichem);
    writer.writeInt(data_.iturb);
    result.push_back(writer.getLine());

    return result;
}

void DualceseControlGeneral::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DualceseControlGeneral, "*DUALCESE_CONTROL_GENERAL")

// ============================================================================
// DualceseBoundaryPrescribedVel
// ============================================================================

bool DualceseBoundaryPrescribedVel::parse(const std::vector<std::string>& lines,
                                           util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: SSID, DOF, VEL, LCID
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.ssid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.dof = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.vel = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));

    return true;
}

std::vector<std::string> DualceseBoundaryPrescribedVel::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.ssid);
    writer.writeInt(data_.dof);
    writer.writeDouble(data_.vel);
    writer.writeInt(data_.lcid);
    result.push_back(writer.getLine());

    return result;
}

void DualceseBoundaryPrescribedVel::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DualceseBoundaryPrescribedVel, "*DUALCESE_BOUNDARY_PRESCRIBED_VEL")

// ============================================================================
// DualceseBoundaryPrescribedPre
// ============================================================================

bool DualceseBoundaryPrescribedPre::parse(const std::vector<std::string>& lines,
                                           util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: SSID, PRES, LCID
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.ssid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.pres = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));

    return true;
}

std::vector<std::string> DualceseBoundaryPrescribedPre::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.ssid);
    writer.writeDouble(data_.pres);
    writer.writeInt(data_.lcid);
    result.push_back(writer.getLine());

    return result;
}

void DualceseBoundaryPrescribedPre::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DualceseBoundaryPrescribedPre, "*DUALCESE_BOUNDARY_PRESCRIBED_PRE")

// ============================================================================
// DualceseInitial
// ============================================================================

bool DualceseInitial::parse(const std::vector<std::string>& lines,
                             util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: PID, RHO, U, V, W, P
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.pid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.rho = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.u = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.v = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.w = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.p = parser.getDoubleAt(pos, realW).value_or(0.0);

    return true;
}

std::vector<std::string> DualceseInitial::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.pid);
    writer.writeDouble(data_.rho);
    writer.writeDouble(data_.u);
    writer.writeDouble(data_.v);
    writer.writeDouble(data_.w);
    writer.writeDouble(data_.p);
    result.push_back(writer.getLine());

    return result;
}

void DualceseInitial::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DualceseInitial, "*DUALCESE_INITIAL")

// ============================================================================
// DualcesePart
// ============================================================================

bool DualcesePart::parse(const std::vector<std::string>& lines,
                          util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: PID, MID, EOSID
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.pid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.mid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.eosid = static_cast<int>(parser.getInt64At(pos).value_or(0));

    return true;
}

std::vector<std::string> DualcesePart::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.pid);
    writer.writeInt(data_.mid);
    writer.writeInt(data_.eosid);
    result.push_back(writer.getLine());

    return result;
}

void DualcesePart::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DualcesePart, "*DUALCESE_PART")

// ============================================================================
// DualceseControlSolver
// ============================================================================

bool DualceseControlSolver::parse(const std::vector<std::string>& lines,
                                   util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: ISOLVER, GAMMA, PREF, RHOREF
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.isolver = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.gamma = parser.getDoubleAt(pos, realW).value_or(1.4);
    pos += realW;
    data_.pref = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.rhoref = parser.getDoubleAt(pos, realW).value_or(0.0);

    return true;
}

std::vector<std::string> DualceseControlSolver::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.isolver);
    writer.writeDouble(data_.gamma);
    writer.writeDouble(data_.pref);
    writer.writeDouble(data_.rhoref);
    result.push_back(writer.getLine());

    return result;
}

void DualceseControlSolver::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DualceseControlSolver, "*DUALCESE_CONTROL_SOLVER")

// ============================================================================
// DualceseBoundaryReflective
// ============================================================================

bool DualceseBoundaryReflective::parse(const std::vector<std::string>& lines,
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

    data_.ssid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.rtype = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.coef = parser.getDoubleAt(pos, realW).value_or(1.0);

    return true;
}

std::vector<std::string> DualceseBoundaryReflective::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.ssid);
    writer.writeInt(data_.rtype);
    writer.writeDouble(data_.coef);
    result.push_back(writer.getLine());

    return result;
}

void DualceseBoundaryReflective::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DualceseBoundaryReflective, "*DUALCESE_BOUNDARY_REFLECTIVE")

// ============================================================================
// DualceseMat
// ============================================================================

bool DualceseMat::parse(const std::vector<std::string>& lines,
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

    data_.mid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.rho = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.mu = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.pr = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.cv = parser.getDoubleAt(pos, realW).value_or(0.0);

    return true;
}

std::vector<std::string> DualceseMat::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.mid);
    writer.writeDouble(data_.rho);
    writer.writeDouble(data_.mu);
    writer.writeDouble(data_.pr);
    writer.writeDouble(data_.cv);
    result.push_back(writer.getLine());

    return result;
}

void DualceseMat::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DualceseMat, "*DUALCESE_MAT")

} // namespace koo::dyna
