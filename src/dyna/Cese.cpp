#include <koo/dyna/Cese.hpp>
#include <koo/dyna/ModelVisitor.hpp>
#include <koo/dyna/KeywordFactory.hpp>
#include <koo/util/StringUtils.hpp>

namespace koo::dyna {

// ============================================================================
// CeseControlTimestep
// ============================================================================

bool CeseControlTimestep::parse(const std::vector<std::string>& lines,
                                 util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: IDDT, CFL, DT_INIT, DT_MIN, DT_MAX
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.iddt = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.cfl = parser.getDoubleAt(pos, realW).value_or(0.9);
    pos += realW;
    data_.dt_init = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.dt_min = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.dt_max = parser.getDoubleAt(pos, realW).value_or(0.0);

    return true;
}

std::vector<std::string> CeseControlTimestep::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.iddt);
    writer.writeDouble(data_.cfl);
    writer.writeDouble(data_.dt_init);
    writer.writeDouble(data_.dt_min);
    writer.writeDouble(data_.dt_max);
    result.push_back(writer.getLine());

    return result;
}

void CeseControlTimestep::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(CeseControlTimestep, "*CESE_CONTROL_TIMESTEP")

// ============================================================================
// CeseControlSolver
// ============================================================================

bool CeseControlSolver::parse(const std::vector<std::string>& lines,
                               util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: IDGAS, GAMMA, PREF, RHOREF, TREF
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.idgas = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.gamma = parser.getDoubleAt(pos, realW).value_or(1.4);
    pos += realW;
    data_.pref = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.rhoref = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.tref = parser.getDoubleAt(pos, realW).value_or(0.0);

    return true;
}

std::vector<std::string> CeseControlSolver::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.idgas);
    writer.writeDouble(data_.gamma);
    writer.writeDouble(data_.pref);
    writer.writeDouble(data_.rhoref);
    writer.writeDouble(data_.tref);
    result.push_back(writer.getLine());

    return result;
}

void CeseControlSolver::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(CeseControlSolver, "*CESE_CONTROL_SOLVER")

// ============================================================================
// CeseBoundaryPrescribed
// ============================================================================

bool CeseBoundaryPrescribed::parse(const std::vector<std::string>& lines,
                                    util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: SSID, IDCOMP, VAL, LCID
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.ssid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.idcomp = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.val = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));

    return true;
}

std::vector<std::string> CeseBoundaryPrescribed::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.ssid);
    writer.writeInt(data_.idcomp);
    writer.writeDouble(data_.val);
    writer.writeInt(data_.lcid);
    result.push_back(writer.getLine());

    return result;
}

void CeseBoundaryPrescribed::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(CeseBoundaryPrescribed, "*CESE_BOUNDARY_PRESCRIBED")

// ============================================================================
// CeseInitial
// ============================================================================

bool CeseInitial::parse(const std::vector<std::string>& lines,
                         util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: PID, U, V, W, RHO, P, T
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.pid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.u = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.v = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.w = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.rho = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.p = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.t = parser.getDoubleAt(pos, realW).value_or(0.0);

    return true;
}

std::vector<std::string> CeseInitial::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.pid);
    writer.writeDouble(data_.u);
    writer.writeDouble(data_.v);
    writer.writeDouble(data_.w);
    writer.writeDouble(data_.rho);
    writer.writeDouble(data_.p);
    writer.writeDouble(data_.t);
    result.push_back(writer.getLine());

    return result;
}

void CeseInitial::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(CeseInitial, "*CESE_INITIAL")

// ============================================================================
// CeseBoundaryReflective
// ============================================================================

bool CeseBoundaryReflective::parse(const std::vector<std::string>& lines,
                                    util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: SSID, RTYPE
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.ssid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.rtype = static_cast<int>(parser.getInt64At(pos).value_or(0));

    return true;
}

std::vector<std::string> CeseBoundaryReflective::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.ssid);
    writer.writeInt(data_.rtype);
    result.push_back(writer.getLine());

    return result;
}

void CeseBoundaryReflective::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(CeseBoundaryReflective, "*CESE_BOUNDARY_REFLECTIVE")

// ============================================================================
// CeseBoundaryNonReflective
// ============================================================================

bool CeseBoundaryNonReflective::parse(const std::vector<std::string>& lines,
                                       util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: SSID, PBACK, LCID
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.ssid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.pback = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));

    return true;
}

std::vector<std::string> CeseBoundaryNonReflective::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.ssid);
    writer.writeDouble(data_.pback);
    writer.writeInt(data_.lcid);
    result.push_back(writer.getLine());

    return result;
}

void CeseBoundaryNonReflective::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(CeseBoundaryNonReflective, "*CESE_BOUNDARY_NON_REFLECTIVE")

// ============================================================================
// CesePart
// ============================================================================

bool CesePart::parse(const std::vector<std::string>& lines,
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

std::vector<std::string> CesePart::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.pid);
    writer.writeInt(data_.mid);
    writer.writeInt(data_.eosid);
    result.push_back(writer.getLine());

    return result;
}

void CesePart::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(CesePart, "*CESE_PART")

// ============================================================================
// CeseControlOutput
// ============================================================================

bool CeseControlOutput::parse(const std::vector<std::string>& lines,
                               util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: DENSITY, VELOCITY, PRESSURE, TEMPERATURE, MACH
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.density = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.velocity = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.pressure = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.temperature = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.mach = static_cast<int>(parser.getInt64At(pos).value_or(0));

    return true;
}

std::vector<std::string> CeseControlOutput::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.density);
    writer.writeInt(data_.velocity);
    writer.writeInt(data_.pressure);
    writer.writeInt(data_.temperature);
    writer.writeInt(data_.mach);
    result.push_back(writer.getLine());

    return result;
}

void CeseControlOutput::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(CeseControlOutput, "*CESE_CONTROL_OUTPUT")

// ============================================================================
// CeseControlMesh
// ============================================================================

bool CeseControlMesh::parse(const std::vector<std::string>& lines,
                             util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: IAUTO, MSHSIZ, NELEM, ADAPT, REFINE
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.iauto = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.mshsiz = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.nelem = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.adapt = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.refine = parser.getDoubleAt(pos, realW).value_or(0.0);

    return true;
}

std::vector<std::string> CeseControlMesh::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.iauto);
    writer.writeDouble(data_.mshsiz);
    writer.writeInt(data_.nelem);
    writer.writeInt(data_.adapt);
    writer.writeDouble(data_.refine);
    result.push_back(writer.getLine());

    return result;
}

void CeseControlMesh::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(CeseControlMesh, "*CESE_CONTROL_MESH")

// ============================================================================
// CeseMat
// ============================================================================

bool CeseMat::parse(const std::vector<std::string>& lines,
                     util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: MID, RHO, GAMMA, CV, MU, KAPPA
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.mid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.rho = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.gamma = parser.getDoubleAt(pos, realW).value_or(1.4);
    pos += realW;
    data_.cv = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.mu = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.kappa = parser.getDoubleAt(pos, realW).value_or(0.0);

    return true;
}

std::vector<std::string> CeseMat::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.mid);
    writer.writeDouble(data_.rho);
    writer.writeDouble(data_.gamma);
    writer.writeDouble(data_.cv);
    writer.writeDouble(data_.mu);
    writer.writeDouble(data_.kappa);
    result.push_back(writer.getLine());

    return result;
}

void CeseMat::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(CeseMat, "*CESE_MAT")

} // namespace koo::dyna
