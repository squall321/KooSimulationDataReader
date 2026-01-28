#include <koo/dyna/Control.hpp>
#include <koo/dyna/ModelVisitor.hpp>
#include <koo/dyna/KeywordFactory.hpp>
#include <koo/util/StringUtils.hpp>

namespace koo::dyna {

// ============================================================================
// ControlTermination
// ============================================================================

bool ControlTermination::parse(const std::vector<std::string>& lines,
                                util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t realW = parser.getRealWidth();

    // Card 1: ENDTIM, ENDCYC, DTMIN, ENDENG, ENDMAS, NOSOL
    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;
        data_.endtim = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.endcyc = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.dtmin = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.endeng = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.endmas = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.nosol = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> ControlTermination::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeDouble(data_.endtim);
    writer.writeDouble(data_.endcyc);
    writer.writeDouble(data_.dtmin);
    writer.writeDouble(data_.endeng);
    writer.writeDouble(data_.endmas);
    writer.writeInt(data_.nosol);

    result.push_back(writer.getLine());
    return result;
}

void ControlTermination::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ControlTermination, "*CONTROL_TERMINATION")

// ============================================================================
// ControlTimestep
// ============================================================================

bool ControlTimestep::parse(const std::vector<std::string>& lines,
                             util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t realW = parser.getRealWidth();
    const size_t intW = parser.getIntWidth();

    // Card 1: DTINIT, TSSFAC, ISDO, TSLIMT, DT2MS, LCTM, ERODE, MS1ST
    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;
        data_.dtinit = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.tssfac = parser.getDoubleAt(pos, realW).value_or(0.9);
        pos += realW;
        data_.isdo = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.tslimt = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.dt2ms = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.lctm = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.erode = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.ms1st = parser.getDoubleAt(pos, realW).value_or(0.0);
        break;
    }

    return true;
}

std::vector<std::string> ControlTimestep::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeDouble(data_.dtinit);
    writer.writeDouble(data_.tssfac);
    writer.writeInt(data_.isdo);
    writer.writeDouble(data_.tslimt);
    writer.writeDouble(data_.dt2ms);
    writer.writeInt(data_.lctm);
    writer.writeInt(data_.erode);
    writer.writeDouble(data_.ms1st);

    result.push_back(writer.getLine());
    return result;
}

void ControlTimestep::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ControlTimestep, "*CONTROL_TIMESTEP")

// ============================================================================
// ControlEnergy
// ============================================================================

bool ControlEnergy::parse(const std::vector<std::string>& lines,
                           util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    // Card 1: HGEN, RWEN, SLNTEN, RYLEN, IRGEN, MESSION, RAYESSION, FESSION
    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;
        data_.hgen = static_cast<int>(parser.getInt64At(pos).value_or(2));
        pos += intW;
        data_.rwen = static_cast<int>(parser.getInt64At(pos).value_or(2));
        pos += intW;
        data_.slnten = static_cast<int>(parser.getInt64At(pos).value_or(2));
        pos += intW;
        data_.rylen = static_cast<int>(parser.getInt64At(pos).value_or(2));
        pos += intW;
        data_.irgen = static_cast<int>(parser.getInt64At(pos).value_or(2));
        pos += intW;
        data_.mession = static_cast<int>(parser.getInt64At(pos).value_or(2));
        pos += intW;
        data_.rayession = static_cast<int>(parser.getInt64At(pos).value_or(2));
        pos += intW;
        data_.fession = static_cast<int>(parser.getInt64At(pos).value_or(2));
        break;
    }

    return true;
}

std::vector<std::string> ControlEnergy::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.hgen);
    writer.writeInt(data_.rwen);
    writer.writeInt(data_.slnten);
    writer.writeInt(data_.rylen);
    writer.writeInt(data_.irgen);
    writer.writeInt(data_.mession);
    writer.writeInt(data_.rayession);
    writer.writeInt(data_.fession);

    result.push_back(writer.getLine());
    return result;
}

void ControlEnergy::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ControlEnergy, "*CONTROL_ENERGY")

// ============================================================================
// ControlOutput
// ============================================================================

bool ControlOutput::parse(const std::vector<std::string>& lines,
                           util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;
        data_.npopt = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.netefr = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.nrefup = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.iaccop = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.optefr = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.ipnint = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.iketefr = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.iftefr = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> ControlOutput::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.npopt);
    writer.writeInt(data_.netefr);
    writer.writeInt(data_.nrefup);
    writer.writeInt(data_.iaccop);
    writer.writeInt(data_.optefr);
    writer.writeInt(data_.ipnint);
    writer.writeInt(data_.iketefr);
    writer.writeInt(data_.iftefr);

    result.push_back(writer.getLine());
    return result;
}

void ControlOutput::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ControlOutput, "*CONTROL_OUTPUT")

// ============================================================================
// ControlContact
// ============================================================================

bool ControlContact::parse(const std::vector<std::string>& lines,
                            util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t realW = parser.getRealWidth();
    const size_t intW = parser.getIntWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;
        data_.slsfac = parser.getDoubleAt(pos, realW).value_or(0.1);
        pos += realW;
        data_.rwpnal = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.islchk = static_cast<int>(parser.getInt64At(pos).value_or(1));
        pos += intW;
        data_.shlthk = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.penopt = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.thkchg = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.otefr = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.enmass = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> ControlContact::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeDouble(data_.slsfac);
    writer.writeDouble(data_.rwpnal);
    writer.writeInt(data_.islchk);
    writer.writeInt(data_.shlthk);
    writer.writeInt(data_.penopt);
    writer.writeInt(data_.thkchg);
    writer.writeInt(data_.otefr);
    writer.writeInt(data_.enmass);

    result.push_back(writer.getLine());
    return result;
}

void ControlContact::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ControlContact, "*CONTROL_CONTACT")

// ============================================================================
// ControlHourglass
// ============================================================================

bool ControlHourglass::parse(const std::vector<std::string>& lines,
                              util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t realW = parser.getRealWidth();
    const size_t intW = parser.getIntWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;
        data_.ihq = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.qh = parser.getDoubleAt(pos, realW).value_or(0.1);
        break;
    }

    return true;
}

std::vector<std::string> ControlHourglass::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.ihq);
    writer.writeDouble(data_.qh);

    result.push_back(writer.getLine());
    return result;
}

void ControlHourglass::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ControlHourglass, "*CONTROL_HOURGLASS")

// ============================================================================
// ControlBulkViscosity
// ============================================================================

bool ControlBulkViscosity::parse(const std::vector<std::string>& lines,
                                  util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t realW = parser.getRealWidth();
    const size_t intW = parser.getIntWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;
        data_.q1 = parser.getDoubleAt(pos, realW).value_or(1.5);
        pos += realW;
        data_.q2 = parser.getDoubleAt(pos, realW).value_or(0.06);
        pos += realW;
        data_.type = static_cast<int>(parser.getInt64At(pos).value_or(1));
        pos += intW;
        data_.btype = parser.getDoubleAt(pos, realW).value_or(0.0);
        break;
    }

    return true;
}

std::vector<std::string> ControlBulkViscosity::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeDouble(data_.q1);
    writer.writeDouble(data_.q2);
    writer.writeInt(data_.type);
    writer.writeDouble(data_.btype);

    result.push_back(writer.getLine());
    return result;
}

void ControlBulkViscosity::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ControlBulkViscosity, "*CONTROL_BULK_VISCOSITY")

// ============================================================================
// ControlShell
// ============================================================================

bool ControlShell::parse(const std::vector<std::string>& lines,
                          util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t realW = parser.getRealWidth();
    const size_t intW = parser.getIntWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;
        data_.wrpang = parser.getDoubleAt(pos, realW).value_or(20.0);
        pos += realW;
        data_.esort = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.irnxx = static_cast<int>(parser.getInt64At(pos).value_or(-1));
        pos += intW;
        data_.istupd = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.theory = parser.getDoubleAt(pos, realW).value_or(2.0);
        pos += realW;
        data_.bwc = parser.getDoubleAt(pos, realW).value_or(1.0);
        pos += realW;
        data_.miter = static_cast<int>(parser.getInt64At(pos).value_or(1));
        pos += intW;
        data_.proj = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> ControlShell::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeDouble(data_.wrpang);
    writer.writeInt(data_.esort);
    writer.writeInt(data_.irnxx);
    writer.writeInt(data_.istupd);
    writer.writeDouble(data_.theory);
    writer.writeDouble(data_.bwc);
    writer.writeInt(data_.miter);
    writer.writeInt(data_.proj);

    result.push_back(writer.getLine());
    return result;
}

void ControlShell::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ControlShell, "*CONTROL_SHELL")

// ============================================================================
// ControlSolid
// ============================================================================

bool ControlSolid::parse(const std::vector<std::string>& lines,
                          util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;
        data_.esort = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.fmatrx = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.niptets = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.swlocl = static_cast<int>(parser.getInt64At(pos).value_or(1));
        pos += intW;
        data_.pession = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.niptso = static_cast<int>(parser.getInt64At(pos).value_or(8));
        pos += intW;
        data_.dt2msso = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.lmc = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> ControlSolid::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.esort);
    writer.writeInt(data_.fmatrx);
    writer.writeInt(data_.niptets);
    writer.writeInt(data_.swlocl);
    writer.writeInt(data_.pession);
    writer.writeInt(data_.niptso);
    writer.writeInt(data_.dt2msso);
    writer.writeInt(data_.lmc);

    result.push_back(writer.getLine());
    return result;
}

void ControlSolid::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ControlSolid, "*CONTROL_SOLID")

// ============================================================================
// ControlAccuracy
// ============================================================================

bool ControlAccuracy::parse(const std::vector<std::string>& lines,
                             util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;
        data_.osu = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.inn = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.pidosu = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.iatefr = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> ControlAccuracy::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.osu);
    writer.writeInt(data_.inn);
    writer.writeInt(data_.pidosu);
    writer.writeInt(data_.iatefr);

    result.push_back(writer.getLine());
    return result;
}

void ControlAccuracy::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ControlAccuracy, "*CONTROL_ACCURACY")

// ============================================================================
// ControlRigid
// ============================================================================

bool ControlRigid::parse(const std::vector<std::string>& lines,
                          util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;
        data_.lmf = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.jntf = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.orthmd = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.partm = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.sparse = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.metalf = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> ControlRigid::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.lmf);
    writer.writeInt(data_.jntf);
    writer.writeInt(data_.orthmd);
    writer.writeInt(data_.partm);
    writer.writeInt(data_.sparse);
    writer.writeInt(data_.metalf);

    result.push_back(writer.getLine());
    return result;
}

void ControlRigid::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ControlRigid, "*CONTROL_RIGID")

// ============================================================================
// ControlCpu
// ============================================================================

bool ControlCpu::parse(const std::vector<std::string>& lines,
                        util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        data_.cputime = parser.getDoubleAt(0, realW).value_or(0.0);
        break;
    }

    return true;
}

std::vector<std::string> ControlCpu::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeDouble(data_.cputime);

    result.push_back(writer.getLine());
    return result;
}

void ControlCpu::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ControlCpu, "*CONTROL_CPU")

// ============================================================================
// ControlParallel
// ============================================================================

bool ControlParallel::parse(const std::vector<std::string>& lines,
                             util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;
        data_.ncpu = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.numrhs = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.const1 = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.const2 = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> ControlParallel::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.ncpu);
    writer.writeInt(data_.numrhs);
    writer.writeInt(data_.const1);
    writer.writeInt(data_.const2);

    result.push_back(writer.getLine());
    return result;
}

void ControlParallel::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ControlParallel, "*CONTROL_PARALLEL")

// ============================================================================
// ControlDynamicRelaxation
// ============================================================================

bool ControlDynamicRelaxation::parse(const std::vector<std::string>& lines,
                                      util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;
        data_.nrcyck = static_cast<int>(parser.getInt64At(pos).value_or(250));
        pos += intW;
        data_.drterm = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.drfctr = parser.getDoubleAt(pos, realW).value_or(0.995);
        pos += realW;
        data_.drrel = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.itefr = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.dtefr = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.tssfdr = parser.getDoubleAt(pos, realW).value_or(1.0);
        break;
    }

    return true;
}

std::vector<std::string> ControlDynamicRelaxation::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.nrcyck);
    writer.writeDouble(data_.drterm);
    writer.writeDouble(data_.drfctr);
    writer.writeDouble(data_.drrel);
    writer.writeInt(data_.itefr);
    writer.writeInt(data_.dtefr);
    writer.writeDouble(data_.tssfdr);

    result.push_back(writer.getLine());
    return result;
}

void ControlDynamicRelaxation::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ControlDynamicRelaxation, "*CONTROL_DYNAMIC_RELAXATION")

// ============================================================================
// ControlMppDecompositionDistributeAleElements
// ============================================================================

bool ControlMppDecompositionDistributeAleElements::parse(const std::vector<std::string>& lines,
                                                          util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        data_.option = static_cast<int>(parser.getInt64At(0).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> ControlMppDecompositionDistributeAleElements::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.option);

    result.push_back(writer.getLine());
    return result;
}

void ControlMppDecompositionDistributeAleElements::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ControlMppDecompositionDistributeAleElements, "*CONTROL_MPP_DECOMPOSITION_DISTRIBUTE_ALE_ELEMENTS")

// Note: ControlImplicitGeneral, ControlImplicitAuto, ControlImplicitSolution
// are already implemented in Implicit.cpp

// ============================================================================
// ControlAle
// ============================================================================

bool ControlAle::parse(const std::vector<std::string>& lines,
                        util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;
        data_.dtefr = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.mtefr = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.smtefr = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.start = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.end = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.dttefr = parser.getDoubleAt(pos, realW).value_or(0.0);
        break;
    }

    return true;
}

std::vector<std::string> ControlAle::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.dtefr);
    writer.writeInt(data_.mtefr);
    writer.writeInt(data_.smtefr);
    writer.writeInt(data_.start);
    writer.writeInt(data_.end);
    writer.writeDouble(data_.dttefr);

    result.push_back(writer.getLine());
    return result;
}

void ControlAle::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ControlAle, "*CONTROL_ALE")

// Note: ControlSph is defined in Sph.cpp
// Note: ControlThermalSolver, ControlThermalTimestep are defined in Thermal.cpp

// ============================================================================
// ControlImplicitForming
// ============================================================================

bool ControlImplicitForming::parse(const std::vector<std::string>& lines,
                                    util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;
        data_.tefr = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.nsrs = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.psid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.nrcf = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.ilcnt = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.ncpf = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.dtefr = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.rfgap = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> ControlImplicitForming::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.tefr);
    writer.writeInt(data_.nsrs);
    writer.writeInt(data_.psid);
    writer.writeInt(data_.nrcf);
    writer.writeInt(data_.ilcnt);
    writer.writeInt(data_.ncpf);
    writer.writeDouble(data_.dtefr);
    writer.writeInt(data_.rfgap);

    result.push_back(writer.getLine());
    return result;
}

void ControlImplicitForming::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ControlImplicitForming, "*CONTROL_IMPLICIT_FORMING")

// ============================================================================
// ControlFormingBestfit
// ============================================================================

bool ControlFormingBestfit::parse(const std::vector<std::string>& lines,
                                   util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;
        data_.pid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.thick = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.spcrpt = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.extend = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.sf = parser.getDoubleAt(pos, realW).value_or(1.0);
        pos += realW;
        data_.irefn = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.local = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.orient = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> ControlFormingBestfit::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.pid);
    writer.writeInt(data_.thick);
    writer.writeInt(data_.spcrpt);
    writer.writeInt(data_.extend);
    writer.writeDouble(data_.sf);
    writer.writeInt(data_.irefn);
    writer.writeInt(data_.local);
    writer.writeInt(data_.orient);

    result.push_back(writer.getLine());
    return result;
}

void ControlFormingBestfit::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ControlFormingBestfit, "*CONTROL_FORMING_BESTFIT")

// ============================================================================
// ControlFormingOnestep
// ============================================================================

bool ControlFormingOnestep::parse(const std::vector<std::string>& lines,
                                   util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;
        data_.blkid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.blktyp = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.dieid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.pid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.ftype = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.option = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.nstep = static_cast<int>(parser.getInt64At(pos).value_or(1));
        pos += intW;
        data_.tol = parser.getDoubleAt(pos, realW).value_or(0.001);
        break;
    }

    return true;
}

std::vector<std::string> ControlFormingOnestep::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.blkid);
    writer.writeInt(data_.blktyp);
    writer.writeInt(data_.dieid);
    writer.writeInt(data_.pid);
    writer.writeInt(data_.ftype);
    writer.writeInt(data_.option);
    writer.writeInt(data_.nstep);
    writer.writeDouble(data_.tol);

    result.push_back(writer.getLine());
    return result;
}

void ControlFormingOnestep::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ControlFormingOnestep, "*CONTROL_FORMING_ONESTEP")

// ============================================================================
// ControlAdaptive
// ============================================================================

bool ControlAdaptive::parse(const std::vector<std::string>& lines,
                             util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;
        data_.adpfreq = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.adptol = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.adpopt = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.maxlvl = static_cast<int>(parser.getInt64At(pos).value_or(3));
        pos += intW;
        data_.tbirth = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.tdeath = parser.getDoubleAt(pos, realW).value_or(1e28);
        pos += realW;
        data_.lcadp = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.ioflag = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> ControlAdaptive::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.adpfreq);
    writer.writeDouble(data_.adptol);
    writer.writeInt(data_.adpopt);
    writer.writeInt(data_.maxlvl);
    writer.writeDouble(data_.tbirth);
    writer.writeDouble(data_.tdeath);
    writer.writeInt(data_.lcadp);
    writer.writeInt(data_.ioflag);

    result.push_back(writer.getLine());
    return result;
}

void ControlAdaptive::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ControlAdaptive, "*CONTROL_ADAPTIVE")

// ============================================================================
// ControlRemeshing
// ============================================================================

bool ControlRemeshing::parse(const std::vector<std::string>& lines,
                              util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;
        data_.rmshtyp = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.rmshint = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.rmshtol = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.rmshopt = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.rmshmax = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.sizscl = parser.getDoubleAt(pos, realW).value_or(1.0);
        pos += realW;
        data_.nsmth = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.rmshfq = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> ControlRemeshing::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.rmshtyp);
    writer.writeInt(data_.rmshint);
    writer.writeDouble(data_.rmshtol);
    writer.writeInt(data_.rmshopt);
    writer.writeInt(data_.rmshmax);
    writer.writeDouble(data_.sizscl);
    writer.writeInt(data_.nsmth);
    writer.writeInt(data_.rmshfq);

    result.push_back(writer.getLine());
    return result;
}

void ControlRemeshing::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ControlRemeshing, "*CONTROL_REMESHING")

// ============================================================================
// ControlSpotweldBeam
// ============================================================================

bool ControlSpotweldBeam::parse(const std::vector<std::string>& lines,
                                 util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;
        data_.nfail = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.tbirth = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.tdeath = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.sclmr = parser.getDoubleAt(pos, realW).value_or(1.0);
        break;
    }

    return true;
}

std::vector<std::string> ControlSpotweldBeam::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.nfail);
    writer.writeInt(data_.tbirth);
    writer.writeInt(data_.tdeath);
    writer.writeDouble(data_.sclmr);

    result.push_back(writer.getLine());
    return result;
}

void ControlSpotweldBeam::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ControlSpotweldBeam, "*CONTROL_SPOTWELD_BEAM")

// Note: ControlImplicitGeneral, ControlImplicitAuto, ControlImplicitSolution,
// ControlImplicitSolver, ControlImplicitEigenvalue, ControlImplicitBuckle,
// ControlImplicitDynamics are implemented in Implicit.cpp

// ============================================================================
// ControlBeam
// ============================================================================

bool ControlBeam::parse(const std::vector<std::string>& lines,
                         util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;
        data_.elform = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.shtefr = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.warpflag = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.betefr = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> ControlBeam::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.elform);
    writer.writeInt(data_.shtefr);
    writer.writeInt(data_.warpflag);
    writer.writeInt(data_.betefr);

    result.push_back(writer.getLine());
    return result;
}

void ControlBeam::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ControlBeam, "*CONTROL_BEAM")

// ============================================================================
// ControlSubcycle
// ============================================================================

bool ControlSubcycle::parse(const std::vector<std::string>& lines,
                             util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;
        data_.sctefr = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.mtefr = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.ltefr = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.dtefr = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> ControlSubcycle::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.sctefr);
    writer.writeInt(data_.mtefr);
    writer.writeInt(data_.ltefr);
    writer.writeInt(data_.dtefr);

    result.push_back(writer.getLine());
    return result;
}

void ControlSubcycle::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ControlSubcycle, "*CONTROL_SUBCYCLE")

} // namespace koo::dyna
