#include <koo/dyna/Icfd.hpp>
#include <koo/dyna/ModelVisitor.hpp>
#include <koo/dyna/KeywordFactory.hpp>
#include <koo/util/StringUtils.hpp>

namespace koo::dyna {

// ============================================================================
// IcfdControlTime
// ============================================================================

bool IcfdControlTime::parse(const std::vector<std::string>& lines,
                             util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: TTM, DT, CFL, LCID
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.ttm = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.dt = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.cfl = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));

    return true;
}

std::vector<std::string> IcfdControlTime::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeDouble(data_.ttm);
    writer.writeDouble(data_.dt);
    writer.writeDouble(data_.cfl);
    writer.writeInt(data_.lcid);
    result.push_back(writer.getLine());

    return result;
}

void IcfdControlTime::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(IcfdControlTime, "*ICFD_CONTROL_TIME")

// ============================================================================
// IcfdControlGeneral
// ============================================================================

bool IcfdControlGeneral::parse(const std::vector<std::string>& lines,
                                util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: APTS, NPTM, DTOL, CTOL, METH
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.apts = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.nptm = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.dtol = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.ctol = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.meth = static_cast<int>(parser.getInt64At(pos).value_or(0));

    return true;
}

std::vector<std::string> IcfdControlGeneral::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.apts);
    writer.writeInt(data_.nptm);
    writer.writeDouble(data_.dtol);
    writer.writeDouble(data_.ctol);
    writer.writeInt(data_.meth);
    result.push_back(writer.getLine());

    return result;
}

void IcfdControlGeneral::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(IcfdControlGeneral, "*ICFD_CONTROL_GENERAL")

// ============================================================================
// IcfdPart
// ============================================================================

bool IcfdPart::parse(const std::vector<std::string>& lines,
                      util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: PID, SECID, MID
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.pid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.secid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.mid = static_cast<int>(parser.getInt64At(pos).value_or(0));

    return true;
}

std::vector<std::string> IcfdPart::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.pid);
    writer.writeInt(data_.secid);
    writer.writeInt(data_.mid);
    result.push_back(writer.getLine());

    return result;
}

void IcfdPart::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(IcfdPart, "*ICFD_PART")

// ============================================================================
// IcfdMat
// ============================================================================

bool IcfdMat::parse(const std::vector<std::string>& lines,
                     util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: MID, RHO, MU, HCF, TCF
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.mid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.rho = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.mu = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.hcf = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.tcf = parser.getDoubleAt(pos, realW).value_or(0.0);

    return true;
}

std::vector<std::string> IcfdMat::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.mid);
    writer.writeDouble(data_.rho);
    writer.writeDouble(data_.mu);
    writer.writeDouble(data_.hcf);
    writer.writeDouble(data_.tcf);
    result.push_back(writer.getLine());

    return result;
}

void IcfdMat::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(IcfdMat, "*ICFD_MAT")

// ============================================================================
// IcfdBoundaryPrescribedVel
// ============================================================================

bool IcfdBoundaryPrescribedVel::parse(const std::vector<std::string>& lines,
                                       util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: PID, DOF, VAD, LCID
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.pid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.dof = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.vad = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));

    return true;
}

std::vector<std::string> IcfdBoundaryPrescribedVel::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.pid);
    writer.writeInt(data_.dof);
    writer.writeDouble(data_.vad);
    writer.writeInt(data_.lcid);
    result.push_back(writer.getLine());

    return result;
}

void IcfdBoundaryPrescribedVel::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(IcfdBoundaryPrescribedVel, "*ICFD_BOUNDARY_PRESCRIBED_VEL")

// ============================================================================
// IcfdBoundaryPrescribedPre
// ============================================================================

bool IcfdBoundaryPrescribedPre::parse(const std::vector<std::string>& lines,
                                       util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: PID, PVAL, LCID, DEATH
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.pid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.pval = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.death = static_cast<int>(parser.getInt64At(pos).value_or(0));

    return true;
}

std::vector<std::string> IcfdBoundaryPrescribedPre::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.pid);
    writer.writeDouble(data_.pval);
    writer.writeInt(data_.lcid);
    writer.writeInt(data_.death);
    result.push_back(writer.getLine());

    return result;
}

void IcfdBoundaryPrescribedPre::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(IcfdBoundaryPrescribedPre, "*ICFD_BOUNDARY_PRESCRIBED_PRE")

// ============================================================================
// IcfdControlOutput
// ============================================================================

bool IcfdControlOutput::parse(const std::vector<std::string>& lines,
                               util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: MESSION, VELOCITY, PRESSURE, TEMPOUT, DRAG
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.mession = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.velocity = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.pressure = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.tempout = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.drag = static_cast<int>(parser.getInt64At(pos).value_or(0));

    return true;
}

std::vector<std::string> IcfdControlOutput::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.mession);
    writer.writeInt(data_.velocity);
    writer.writeInt(data_.pressure);
    writer.writeInt(data_.tempout);
    writer.writeInt(data_.drag);
    result.push_back(writer.getLine());

    return result;
}

void IcfdControlOutput::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(IcfdControlOutput, "*ICFD_CONTROL_OUTPUT")

// ============================================================================
// IcfdControlMesh
// ============================================================================

bool IcfdControlMesh::parse(const std::vector<std::string>& lines,
                             util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: MSHTYP, MSHSIZ, NELEM, ADAPT
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.mshtyp = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.mshsiz = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.nelem = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.adapt = static_cast<int>(parser.getInt64At(pos).value_or(0));

    return true;
}

std::vector<std::string> IcfdControlMesh::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.mshtyp);
    writer.writeDouble(data_.mshsiz);
    writer.writeInt(data_.nelem);
    writer.writeInt(data_.adapt);
    result.push_back(writer.getLine());

    return result;
}

void IcfdControlMesh::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(IcfdControlMesh, "*ICFD_CONTROL_MESH")

// ============================================================================
// IcfdControlTurbulence
// ============================================================================

bool IcfdControlTurbulence::parse(const std::vector<std::string>& lines,
                                   util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: TMOD, CMU, TKE, EPS
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.tmod = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.cmu = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.tke = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.eps = parser.getDoubleAt(pos, realW).value_or(0.0);

    return true;
}

std::vector<std::string> IcfdControlTurbulence::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.tmod);
    writer.writeDouble(data_.cmu);
    writer.writeDouble(data_.tke);
    writer.writeDouble(data_.eps);
    result.push_back(writer.getLine());

    return result;
}

void IcfdControlTurbulence::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(IcfdControlTurbulence, "*ICFD_CONTROL_TURBULENCE")

// ============================================================================
// IcfdControlFsi
// ============================================================================

bool IcfdControlFsi::parse(const std::vector<std::string>& lines,
                            util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: OWC, BT, DT, PESSION, FRCFRQ
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.owc = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.bt = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.dt = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.pession = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.frcfrq = parser.getDoubleAt(pos, realW).value_or(0.0);

    return true;
}

std::vector<std::string> IcfdControlFsi::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.owc);
    writer.writeInt(data_.bt);
    writer.writeInt(data_.dt);
    writer.writeInt(data_.pession);
    writer.writeDouble(data_.frcfrq);
    result.push_back(writer.getLine());

    return result;
}

void IcfdControlFsi::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(IcfdControlFsi, "*ICFD_CONTROL_FSI")

// ============================================================================
// IcfdInitial
// ============================================================================

bool IcfdInitial::parse(const std::vector<std::string>& lines,
                         util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: PID, VX, VY, VZ, PRE, TEMP
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.pid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.vx = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.vy = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.vz = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.pre = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.temp = parser.getDoubleAt(pos, realW).value_or(0.0);

    return true;
}

std::vector<std::string> IcfdInitial::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.pid);
    writer.writeDouble(data_.vx);
    writer.writeDouble(data_.vy);
    writer.writeDouble(data_.vz);
    writer.writeDouble(data_.pre);
    writer.writeDouble(data_.temp);
    result.push_back(writer.getLine());

    return result;
}

void IcfdInitial::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(IcfdInitial, "*ICFD_INITIAL")

// ============================================================================
// IcfdDefineMesh
// ============================================================================

bool IcfdDefineMesh::parse(const std::vector<std::string>& lines,
                            util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: PID, MTYPE, ESIZE, NELEM, BL, NBL, BLR, REMESH
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.pid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.mtype = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.esize = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.nelem = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.bl = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.nbl = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.blr = parser.getDoubleAt(pos, realW).value_or(1.2);
    pos += realW;
    data_.remesh = static_cast<int>(parser.getInt64At(pos).value_or(0));

    return true;
}

std::vector<std::string> IcfdDefineMesh::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.pid);
    writer.writeInt(data_.mtype);
    writer.writeDouble(data_.esize);
    writer.writeInt(data_.nelem);
    writer.writeDouble(data_.bl);
    writer.writeInt(data_.nbl);
    writer.writeDouble(data_.blr);
    writer.writeInt(data_.remesh);
    result.push_back(writer.getLine());

    return result;
}

void IcfdDefineMesh::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(IcfdDefineMesh, "*ICFD_DEFINE_MESH")

} // namespace koo::dyna
