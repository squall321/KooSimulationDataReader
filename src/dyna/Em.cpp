#include <koo/dyna/Em.hpp>
#include <koo/dyna/ModelVisitor.hpp>
#include <koo/dyna/KeywordFactory.hpp>
#include <koo/util/StringUtils.hpp>

namespace koo::dyna {

// ============================================================================
// EmControl
// ============================================================================

bool EmControl::parse(const std::vector<std::string>& lines,
                       util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: ETEFRMW, MESSION, EMDT, EMDT_MIN, EMDT_MAX, NCYCLEM
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.etefrmw = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.mession = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.emdt = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.emdt_min = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.emdt_max = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.ncyclem = static_cast<int>(parser.getInt64At(pos).value_or(1));

    return true;
}

std::vector<std::string> EmControl::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.etefrmw);
    writer.writeInt(data_.mession);
    writer.writeDouble(data_.emdt);
    writer.writeDouble(data_.emdt_min);
    writer.writeDouble(data_.emdt_max);
    writer.writeInt(data_.ncyclem);
    result.push_back(writer.getLine());

    return result;
}

void EmControl::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(EmControl, "*EM_CONTROL")

// ============================================================================
// EmMat001
// ============================================================================

bool EmMat001::parse(const std::vector<std::string>& lines,
                      util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: MID, SIGMA, EOSMU, EOSEP
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.mid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.sigma = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.eosmu = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.eosep = parser.getDoubleAt(pos, realW).value_or(0.0);

    return true;
}

std::vector<std::string> EmMat001::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.mid);
    writer.writeDouble(data_.sigma);
    writer.writeDouble(data_.eosmu);
    writer.writeDouble(data_.eosep);
    result.push_back(writer.getLine());

    return result;
}

void EmMat001::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(EmMat001, "*EM_MAT_001")

// ============================================================================
// EmCircuit
// ============================================================================

bool EmCircuit::parse(const std::vector<std::string>& lines,
                       util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: CIRCID, CIRCTYP, LCID, R, L, C, V0, I0
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.circid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.circtyp = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.r = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.l = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.c = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.v0 = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.i0 = parser.getDoubleAt(pos, realW).value_or(0.0);

    return true;
}

std::vector<std::string> EmCircuit::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.circid);
    writer.writeInt(data_.circtyp);
    writer.writeInt(data_.lcid);
    writer.writeDouble(data_.r);
    writer.writeDouble(data_.l);
    writer.writeDouble(data_.c);
    writer.writeDouble(data_.v0);
    writer.writeDouble(data_.i0);
    result.push_back(writer.getLine());

    return result;
}

void EmCircuit::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(EmCircuit, "*EM_CIRCUIT")

// ============================================================================
// EmBoundary
// ============================================================================

bool EmBoundary::parse(const std::vector<std::string>& lines,
                        util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: SSID, BTYPE, VAL, LCID
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.ssid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.btype = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.val = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));

    return true;
}

std::vector<std::string> EmBoundary::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.ssid);
    writer.writeInt(data_.btype);
    writer.writeDouble(data_.val);
    writer.writeInt(data_.lcid);
    result.push_back(writer.getLine());

    return result;
}

void EmBoundary::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(EmBoundary, "*EM_BOUNDARY")

// ============================================================================
// EmSolverFem
// ============================================================================

bool EmSolverFem::parse(const std::vector<std::string>& lines,
                         util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: MESSION, RELTOL, TOL, MAXITER, PCOND
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.mession = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.reltol = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.tol = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.maxiter = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.pcond = static_cast<int>(parser.getInt64At(pos).value_or(0));

    return true;
}

std::vector<std::string> EmSolverFem::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.mession);
    writer.writeInt(data_.reltol);
    writer.writeDouble(data_.tol);
    writer.writeInt(data_.maxiter);
    writer.writeInt(data_.pcond);
    result.push_back(writer.getLine());

    return result;
}

void EmSolverFem::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(EmSolverFem, "*EM_SOLVER_FEM")

// ============================================================================
// EmOutput
// ============================================================================

bool EmOutput::parse(const std::vector<std::string>& lines,
                      util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: MESSION, MAGFLD, EFLD, JFLD, JOULE
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.mession = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.magfld = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.efld = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.jfld = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.joule = static_cast<int>(parser.getInt64At(pos).value_or(0));

    return true;
}

std::vector<std::string> EmOutput::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.mession);
    writer.writeInt(data_.magfld);
    writer.writeInt(data_.efld);
    writer.writeInt(data_.jfld);
    writer.writeInt(data_.joule);
    result.push_back(writer.getLine());

    return result;
}

void EmOutput::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(EmOutput, "*EM_OUTPUT")

// ============================================================================
// EmIsopotential
// ============================================================================

bool EmIsopotential::parse(const std::vector<std::string>& lines,
                            util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: SETID, SETTYPE, VAL, LCID
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.setid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.settype = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.val = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));

    return true;
}

std::vector<std::string> EmIsopotential::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.setid);
    writer.writeInt(data_.settype);
    writer.writeDouble(data_.val);
    writer.writeInt(data_.lcid);
    result.push_back(writer.getLine());

    return result;
}

void EmIsopotential::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(EmIsopotential, "*EM_ISOPOTENTIAL")

// ============================================================================
// EmContact
// ============================================================================

bool EmContact::parse(const std::vector<std::string>& lines,
                       util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: CONTID, SSID1, SSID2, CONTTYPE, PERM
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.contid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.ssid1 = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.ssid2 = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.conttype = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.perm = parser.getDoubleAt(pos, realW).value_or(0.0);

    return true;
}

std::vector<std::string> EmContact::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.contid);
    writer.writeInt(data_.ssid1);
    writer.writeInt(data_.ssid2);
    writer.writeInt(data_.conttype);
    writer.writeDouble(data_.perm);
    result.push_back(writer.getLine());

    return result;
}

void EmContact::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(EmContact, "*EM_CONTACT")

// ============================================================================
// EmMat002
// ============================================================================

bool EmMat002::parse(const std::vector<std::string>& lines,
                      util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: MID, EOSMU, EOSEP, SIGMA
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.mid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.eosmu = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.eosep = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.sigma = parser.getDoubleAt(pos, realW).value_or(0.0);

    return true;
}

std::vector<std::string> EmMat002::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.mid);
    writer.writeDouble(data_.eosmu);
    writer.writeDouble(data_.eosep);
    writer.writeDouble(data_.sigma);
    result.push_back(writer.getLine());

    return result;
}

void EmMat002::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(EmMat002, "*EM_MAT_002")

// ============================================================================
// EmControlCoupling
// ============================================================================

bool EmControlCoupling::parse(const std::vector<std::string>& lines,
                               util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: COUPTYPE, MECHSOL, THERMSOL, TSTART, TEND
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.couptype = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.mechsol = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.thermsol = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.tstart = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.tend = parser.getDoubleAt(pos, realW).value_or(0.0);

    return true;
}

std::vector<std::string> EmControlCoupling::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.couptype);
    writer.writeInt(data_.mechsol);
    writer.writeInt(data_.thermsol);
    writer.writeDouble(data_.tstart);
    writer.writeDouble(data_.tend);
    result.push_back(writer.getLine());

    return result;
}

void EmControlCoupling::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(EmControlCoupling, "*EM_CONTROL_COUPLING")

// ============================================================================
// EmRandlesBatmac
// ============================================================================

bool EmRandlesBatmac::parse(const std::vector<std::string>& lines,
                             util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: BATMACID, RDLID, FUNCTYPE
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.batmacid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.rdlid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.functype = static_cast<int>(parser.getInt64At(pos).value_or(0));
    ++lineIdx;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 2: R0, R1, C1, R2, C2
    parser.setLine(lines[lineIdx]);
    pos = 0;

    data_.r0 = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.r1 = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.c1 = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.r2 = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.c2 = parser.getDoubleAt(pos, realW).value_or(0.0);
    ++lineIdx;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 3: SOC_LCID, TEMP_LCID
    parser.setLine(lines[lineIdx]);
    pos = 0;

    data_.soc_lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.temp_lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));

    return true;
}

std::vector<std::string> EmRandlesBatmac::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    // Card 1
    writer.writeInt(data_.batmacid);
    writer.writeInt(data_.rdlid);
    writer.writeInt(data_.functype);
    result.push_back(writer.getLine());

    // Card 2
    writer.clear();
    writer.writeDouble(data_.r0);
    writer.writeDouble(data_.r1);
    writer.writeDouble(data_.c1);
    writer.writeDouble(data_.r2);
    writer.writeDouble(data_.c2);
    result.push_back(writer.getLine());

    // Card 3
    writer.clear();
    writer.writeInt(data_.soc_lcid);
    writer.writeInt(data_.temp_lcid);
    result.push_back(writer.getLine());

    return result;
}

void EmRandlesBatmac::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(EmRandlesBatmac, "*EM_RANDLES_BATMAC")

} // namespace koo::dyna
