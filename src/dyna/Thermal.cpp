#include <koo/dyna/Thermal.hpp>
#include <koo/dyna/ModelVisitor.hpp>
#include <koo/dyna/KeywordFactory.hpp>
#include <koo/util/StringUtils.hpp>

namespace koo::dyna {

// ============================================================================
// ControlThermalSolver
// ============================================================================

bool ControlThermalSolver::parse(const std::vector<std::string>& lines,
                                  util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: ATEFR, SOLVER, CG_TOL, CG_MAX, FWORK, ETEFR, MSGLV
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.atefr = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.solver = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.cg_tol = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.cg_max = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.fwork = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.etefr = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.msglv = static_cast<int>(parser.getInt64At(pos).value_or(0));

    return true;
}

std::vector<std::string> ControlThermalSolver::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.atefr);
    writer.writeInt(data_.solver);
    writer.writeInt(data_.cg_tol);
    writer.writeInt(data_.cg_max);
    writer.writeDouble(data_.fwork);
    writer.writeDouble(data_.etefr);
    writer.writeInt(data_.msglv);
    result.push_back(writer.getLine());

    return result;
}

void ControlThermalSolver::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ControlThermalSolver, "*CONTROL_THERMAL_SOLVER")

// ============================================================================
// ControlThermalTimestep
// ============================================================================

bool ControlThermalTimestep::parse(const std::vector<std::string>& lines,
                                    util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: TS, TIP, ITS, TMIN, TMAX, DTEMP, TSCP
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.ts = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.tip = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.its = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.tmin = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.tmax = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.dtemp = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.tscp = static_cast<int>(parser.getInt64At(pos).value_or(0));

    return true;
}

std::vector<std::string> ControlThermalTimestep::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeDouble(data_.ts);
    writer.writeDouble(data_.tip);
    writer.writeInt(data_.its);
    writer.writeDouble(data_.tmin);
    writer.writeDouble(data_.tmax);
    writer.writeDouble(data_.dtemp);
    writer.writeInt(data_.tscp);
    result.push_back(writer.getLine());

    return result;
}

void ControlThermalTimestep::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ControlThermalTimestep, "*CONTROL_THERMAL_TIMESTEP")

// ============================================================================
// ControlThermalNonlinear
// ============================================================================

bool ControlThermalNonlinear::parse(const std::vector<std::string>& lines,
                                     util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: REFMAX, TOL, DTEFR, TEFR
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.refmax = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.tol = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.dtefr = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.tefr = static_cast<int>(parser.getInt64At(pos).value_or(0));

    return true;
}

std::vector<std::string> ControlThermalNonlinear::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.refmax);
    writer.writeDouble(data_.tol);
    writer.writeInt(data_.dtefr);
    writer.writeInt(data_.tefr);
    result.push_back(writer.getLine());

    return result;
}

void ControlThermalNonlinear::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ControlThermalNonlinear, "*CONTROL_THERMAL_NONLINEAR")

// ============================================================================
// BoundaryTemperature
// ============================================================================

bool BoundaryTemperature::parse(const std::vector<std::string>& lines,
                                 util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (size_t lineIdx = 0; lineIdx < lines.size(); ++lineIdx) {
        if (util::CardParser::isCommentLine(lines[lineIdx])) {
            continue;
        }

        parser.setLine(lines[lineIdx]);
        size_t pos = 0;

        Entry entry;
        entry.nid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.temp = parser.getDoubleAt(pos, realW).value_or(0.0);

        if (entry.nid != 0) {
            entries_.push_back(entry);
        }
    }

    return true;
}

std::vector<std::string> BoundaryTemperature::write(util::CardParser::Format format) const {
    std::vector<std::string> result;

    for (const auto& entry : entries_) {
        util::CardWriter writer(format);
        writer.writeInt(entry.nid);
        writer.writeInt(entry.lcid);
        writer.writeDouble(entry.temp);
        result.push_back(writer.getLine());
    }

    return result;
}

void BoundaryTemperature::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(BoundaryTemperature, "*BOUNDARY_TEMPERATURE")

// ============================================================================
// BoundaryFlux
// ============================================================================

bool BoundaryFlux::parse(const std::vector<std::string>& lines,
                          util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: SSID, LCID, FLUX
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.ssid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.flux = parser.getDoubleAt(pos, realW).value_or(0.0);

    return true;
}

std::vector<std::string> BoundaryFlux::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.ssid);
    writer.writeInt(data_.lcid);
    writer.writeDouble(data_.flux);
    result.push_back(writer.getLine());

    return result;
}

void BoundaryFlux::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(BoundaryFlux, "*BOUNDARY_FLUX")

// ============================================================================
// BoundaryConvection
// ============================================================================

bool BoundaryConvection::parse(const std::vector<std::string>& lines,
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
    data_.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.lchc = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.hlc = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.tinf = parser.getDoubleAt(pos, realW).value_or(0.0);

    return true;
}

std::vector<std::string> BoundaryConvection::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.ssid);
    writer.writeInt(data_.lcid);
    writer.writeInt(data_.lchc);
    writer.writeDouble(data_.hlc);
    writer.writeDouble(data_.tinf);
    result.push_back(writer.getLine());

    return result;
}

void BoundaryConvection::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(BoundaryConvection, "*BOUNDARY_CONVECTION")

// ============================================================================
// BoundaryRadiation
// ============================================================================

bool BoundaryRadiation::parse(const std::vector<std::string>& lines,
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
    data_.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.emiss = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.tinf = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.mult = parser.getDoubleAt(pos, realW).value_or(1.0);

    return true;
}

std::vector<std::string> BoundaryRadiation::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.ssid);
    writer.writeInt(data_.lcid);
    writer.writeDouble(data_.emiss);
    writer.writeDouble(data_.tinf);
    writer.writeDouble(data_.mult);
    result.push_back(writer.getLine());

    return result;
}

void BoundaryRadiation::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(BoundaryRadiation, "*BOUNDARY_RADIATION")

// ============================================================================
// InitialTemperature
// ============================================================================

bool InitialTemperature::parse(const std::vector<std::string>& lines,
                                util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (size_t lineIdx = 0; lineIdx < lines.size(); ++lineIdx) {
        if (util::CardParser::isCommentLine(lines[lineIdx])) {
            continue;
        }

        parser.setLine(lines[lineIdx]);
        size_t pos = 0;

        Entry entry;
        entry.nid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.temp = parser.getDoubleAt(pos, realW).value_or(0.0);

        if (entry.nid != 0) {
            entries_.push_back(entry);
        }
    }

    return true;
}

std::vector<std::string> InitialTemperature::write(util::CardParser::Format format) const {
    std::vector<std::string> result;

    for (const auto& entry : entries_) {
        util::CardWriter writer(format);
        writer.writeInt(entry.nid);
        writer.writeDouble(entry.temp);
        result.push_back(writer.getLine());
    }

    return result;
}

void InitialTemperature::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(InitialTemperature, "*INITIAL_TEMPERATURE")

// ============================================================================
// InitialTemperatureSet
// ============================================================================

bool InitialTemperatureSet::parse(const std::vector<std::string>& lines,
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

    data_.nsid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.temp = parser.getDoubleAt(pos, realW).value_or(0.0);

    return true;
}

std::vector<std::string> InitialTemperatureSet::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.nsid);
    writer.writeDouble(data_.temp);
    result.push_back(writer.getLine());

    return result;
}

void InitialTemperatureSet::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(InitialTemperatureSet, "*INITIAL_TEMPERATURE_SET")

// ============================================================================
// MatThermalIsotropic
// ============================================================================

bool MatThermalIsotropic::parse(const std::vector<std::string>& lines,
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

    data_.tmid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.tro = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.tgrlc = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.tgmult = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.hc = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.tc = parser.getDoubleAt(pos, realW).value_or(0.0);

    return true;
}

std::vector<std::string> MatThermalIsotropic::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.tmid);
    writer.writeDouble(data_.tro);
    writer.writeDouble(data_.tgrlc);
    writer.writeDouble(data_.tgmult);
    writer.writeDouble(data_.hc);
    writer.writeDouble(data_.tc);
    result.push_back(writer.getLine());

    return result;
}

void MatThermalIsotropic::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(MatThermalIsotropic, "*MAT_THERMAL_ISOTROPIC")

} // namespace koo::dyna
