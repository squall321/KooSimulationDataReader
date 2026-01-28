#include <koo/dyna/Implicit.hpp>
#include <koo/dyna/ModelVisitor.hpp>
#include <koo/dyna/KeywordFactory.hpp>
#include <koo/util/StringUtils.hpp>

namespace koo::dyna {

// ============================================================================
// ControlImplicitGeneral
// ============================================================================

bool ControlImplicitGeneral::parse(const std::vector<std::string>& lines,
                                    util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: IMFLAG, DT0, IMFORM, NSBS, IGS, CNSTN, FORM, ZERO_V
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.imflag = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.dt0 = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.imform = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.nsbs = static_cast<int>(parser.getInt64At(pos).value_or(1));
    pos += intW;
    data_.igs = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.cnstn = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.form = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.zero_v = static_cast<int>(parser.getInt64At(pos).value_or(0));

    return true;
}

std::vector<std::string> ControlImplicitGeneral::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.imflag);
    writer.writeDouble(data_.dt0);
    writer.writeInt(data_.imform);
    writer.writeInt(data_.nsbs);
    writer.writeInt(data_.igs);
    writer.writeInt(data_.cnstn);
    writer.writeInt(data_.form);
    writer.writeInt(data_.zero_v);
    result.push_back(writer.getLine());

    return result;
}

void ControlImplicitGeneral::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ControlImplicitGeneral, "*CONTROL_IMPLICIT_GENERAL")

// ============================================================================
// ControlImplicitAuto
// ============================================================================

bool ControlImplicitAuto::parse(const std::vector<std::string>& lines,
                                 util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: IATEFR, TEFR, DTMIN, DTMAX, DTEXP, KFAIL, KCYCLE
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.iatefr = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.tefr = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.dtmin = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.dtmax = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.dtexp = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.kfail = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.kcycle = static_cast<int>(parser.getInt64At(pos).value_or(0));

    return true;
}

std::vector<std::string> ControlImplicitAuto::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.iatefr);
    writer.writeInt(data_.tefr);
    writer.writeDouble(data_.dtmin);
    writer.writeDouble(data_.dtmax);
    writer.writeDouble(data_.dtexp);
    writer.writeDouble(data_.kfail);
    writer.writeInt(data_.kcycle);
    result.push_back(writer.getLine());

    return result;
}

void ControlImplicitAuto::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ControlImplicitAuto, "*CONTROL_IMPLICIT_AUTO")

// ============================================================================
// ControlImplicitSolution
// ============================================================================

bool ControlImplicitSolution::parse(const std::vector<std::string>& lines,
                                     util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: NSOLVR, ILIMIT, MAXREF, DTEFR, ETEFR, RTEFR, LSTOL, ABSTOL
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.nsolvr = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.ilimit = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.maxref = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.dtefr = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.etefr = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.rtefr = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.lstol = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.abstol = parser.getDoubleAt(pos, realW).value_or(0.0);

    return true;
}

std::vector<std::string> ControlImplicitSolution::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.nsolvr);
    writer.writeInt(data_.ilimit);
    writer.writeDouble(data_.maxref);
    writer.writeDouble(data_.dtefr);
    writer.writeDouble(data_.etefr);
    writer.writeDouble(data_.rtefr);
    writer.writeDouble(data_.lstol);
    writer.writeDouble(data_.abstol);
    result.push_back(writer.getLine());

    return result;
}

void ControlImplicitSolution::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ControlImplicitSolution, "*CONTROL_IMPLICIT_SOLUTION")

// ============================================================================
// ControlImplicitSolver
// ============================================================================

bool ControlImplicitSolver::parse(const std::vector<std::string>& lines,
                                   util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: LSOLVR, LPRINT, NEGEV, ORDER, DTEFR, MSGLV
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.lsolvr = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.lprint = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.negev = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.order = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.dtefr = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.msglv = static_cast<int>(parser.getInt64At(pos).value_or(0));

    return true;
}

std::vector<std::string> ControlImplicitSolver::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.lsolvr);
    writer.writeInt(data_.lprint);
    writer.writeInt(data_.negev);
    writer.writeInt(data_.order);
    writer.writeDouble(data_.dtefr);
    writer.writeInt(data_.msglv);
    result.push_back(writer.getLine());

    return result;
}

void ControlImplicitSolver::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ControlImplicitSolver, "*CONTROL_IMPLICIT_SOLVER")

// ============================================================================
// ControlImplicitDynamics
// ============================================================================

bool ControlImplicitDynamics::parse(const std::vector<std::string>& lines,
                                     util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: IMASS, GAMMA, BETA, TDYBIR, TDYDTH, TDYBURT, TDYDTHT
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.imass = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.gamma = parser.getDoubleAt(pos, realW).value_or(0.5);
    pos += realW;
    data_.beta = parser.getDoubleAt(pos, realW).value_or(0.25);
    pos += realW;
    data_.tdybir = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.tdydth = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.tdyburt = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.tdydtht = parser.getDoubleAt(pos, realW).value_or(0.0);

    return true;
}

std::vector<std::string> ControlImplicitDynamics::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.imass);
    writer.writeDouble(data_.gamma);
    writer.writeDouble(data_.beta);
    writer.writeInt(data_.tdybir);
    writer.writeInt(data_.tdydth);
    writer.writeDouble(data_.tdyburt);
    writer.writeDouble(data_.tdydtht);
    result.push_back(writer.getLine());

    return result;
}

void ControlImplicitDynamics::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ControlImplicitDynamics, "*CONTROL_IMPLICIT_DYNAMICS")

// ============================================================================
// ControlImplicitEigenvalue
// ============================================================================

bool ControlImplicitEigenvalue::parse(const std::vector<std::string>& lines,
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

    data_.neig = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.center = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.lflag = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.lftend = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.shfscl = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.mtefr = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.mastset = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.mshflag = static_cast<int>(parser.getInt64At(pos).value_or(0));

    return true;
}

std::vector<std::string> ControlImplicitEigenvalue::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.neig);
    writer.writeInt(data_.center);
    writer.writeInt(data_.lflag);
    writer.writeInt(data_.lftend);
    writer.writeDouble(data_.shfscl);
    writer.writeInt(data_.mtefr);
    writer.writeInt(data_.mastset);
    writer.writeInt(data_.mshflag);
    result.push_back(writer.getLine());

    return result;
}

void ControlImplicitEigenvalue::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ControlImplicitEigenvalue, "*CONTROL_IMPLICIT_EIGENVALUE")

// ============================================================================
// ControlImplicitBuckle
// ============================================================================

bool ControlImplicitBuckle::parse(const std::vector<std::string>& lines,
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

    data_.nmode = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.tefr = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.eitefr = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.mshflag = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.sfact = parser.getDoubleAt(pos, realW).value_or(1.0);

    return true;
}

std::vector<std::string> ControlImplicitBuckle::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.nmode);
    writer.writeInt(data_.tefr);
    writer.writeDouble(data_.eitefr);
    writer.writeInt(data_.mshflag);
    writer.writeDouble(data_.sfact);
    result.push_back(writer.getLine());

    return result;
}

void ControlImplicitBuckle::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ControlImplicitBuckle, "*CONTROL_IMPLICIT_BUCKLE")

// ============================================================================
// ControlImplicitStabilization
// ============================================================================

bool ControlImplicitStabilization::parse(const std::vector<std::string>& lines,
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

    data_.ias = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.scale = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.tstart = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.tend = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.form = static_cast<int>(parser.getInt64At(pos).value_or(0));

    return true;
}

std::vector<std::string> ControlImplicitStabilization::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.ias);
    writer.writeDouble(data_.scale);
    writer.writeDouble(data_.tstart);
    writer.writeDouble(data_.tend);
    writer.writeInt(data_.form);
    result.push_back(writer.getLine());

    return result;
}

void ControlImplicitStabilization::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ControlImplicitStabilization, "*CONTROL_IMPLICIT_STABILIZATION")

// ============================================================================
// ControlImplicitModalDynamic
// ============================================================================

bool ControlImplicitModalDynamic::parse(const std::vector<std::string>& lines,
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

    data_.neig = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.mdmin = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.mdmax = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.dampf = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.lcdam = static_cast<int>(parser.getInt64At(pos).value_or(0));

    return true;
}

std::vector<std::string> ControlImplicitModalDynamic::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.neig);
    writer.writeInt(data_.mdmin);
    writer.writeInt(data_.mdmax);
    writer.writeDouble(data_.dampf);
    writer.writeInt(data_.lcdam);
    result.push_back(writer.getLine());

    return result;
}

void ControlImplicitModalDynamic::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ControlImplicitModalDynamic, "*CONTROL_IMPLICIT_MODAL_DYNAMIC")

} // namespace koo::dyna
