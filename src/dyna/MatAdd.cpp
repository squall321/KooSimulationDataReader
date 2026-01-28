#include <koo/dyna/MatAdd.hpp>
#include <koo/dyna/ModelVisitor.hpp>
#include <koo/dyna/KeywordFactory.hpp>
#include <koo/util/StringUtils.hpp>

namespace koo::dyna {

// ============================================================================
// MatAddErosion
// ============================================================================

bool MatAddErosion::parse(const std::vector<std::string>& lines,
                           util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: MID, EXCL, MXPRES, MNEPS, EFFEPS, VOLEPS, NUMFIP, NCS
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.mid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.excl = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.mxpres = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.mneps = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.effeps = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.voleps = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.numfip = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.ncs = parser.getDoubleAt(pos, realW).value_or(1.0);

    ++lineIdx;
    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 2: MNPRES, SIGP1, SIGVM, MXEPS, EPSSH, SIGTH, IMPULSE, FATEFR
    parser.setLine(lines[lineIdx]);
    pos = 0;

    data_.mnpres = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.sigp1 = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.sigvm = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.mxeps = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.epssh = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.sigth = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.impulse = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.fatefr = parser.getDoubleAt(pos, realW).value_or(0.0);

    return true;
}

std::vector<std::string> MatAddErosion::write(util::CardParser::Format format) const {
    std::vector<std::string> result;

    // Card 1
    {
        util::CardWriter writer(format);
        writer.writeInt(data_.mid);
        writer.writeDouble(data_.excl);
        writer.writeDouble(data_.mxpres);
        writer.writeDouble(data_.mneps);
        writer.writeDouble(data_.effeps);
        writer.writeDouble(data_.voleps);
        writer.writeDouble(data_.numfip);
        writer.writeDouble(data_.ncs);
        result.push_back(writer.getLine());
    }

    // Card 2
    {
        util::CardWriter writer(format);
        writer.writeDouble(data_.mnpres);
        writer.writeDouble(data_.sigp1);
        writer.writeDouble(data_.sigvm);
        writer.writeDouble(data_.mxeps);
        writer.writeDouble(data_.epssh);
        writer.writeDouble(data_.sigth);
        writer.writeDouble(data_.impulse);
        writer.writeDouble(data_.fatefr);
        result.push_back(writer.getLine());
    }

    return result;
}

void MatAddErosion::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(MatAddErosion, "*MAT_ADD_EROSION")

// ============================================================================
// MatAddFatigue
// ============================================================================

bool MatAddFatigue::parse(const std::vector<std::string>& lines,
                           util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: MID, LCID, BETA, SE, NE, SF, NF, NATEFR
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.mid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.beta = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.se = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.ne = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.sf = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.nf = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.natefr = static_cast<int>(parser.getInt64At(pos).value_or(0));

    return true;
}

std::vector<std::string> MatAddFatigue::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.mid);
    writer.writeInt(data_.lcid);
    writer.writeDouble(data_.beta);
    writer.writeDouble(data_.se);
    writer.writeDouble(data_.ne);
    writer.writeDouble(data_.sf);
    writer.writeDouble(data_.nf);
    writer.writeInt(data_.natefr);
    result.push_back(writer.getLine());

    return result;
}

void MatAddFatigue::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(MatAddFatigue, "*MAT_ADD_FATIGUE")

// ============================================================================
// MatAddThermalExpansion
// ============================================================================

bool MatAddThermalExpansion::parse(const std::vector<std::string>& lines,
                                    util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: MID, LCID, ALPHA, TREF
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.mid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.alpha = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.tref = parser.getDoubleAt(pos, realW).value_or(0.0);

    return true;
}

std::vector<std::string> MatAddThermalExpansion::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.mid);
    writer.writeInt(data_.lcid);
    writer.writeDouble(data_.alpha);
    writer.writeDouble(data_.tref);
    result.push_back(writer.getLine());

    return result;
}

void MatAddThermalExpansion::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(MatAddThermalExpansion, "*MAT_ADD_THERMAL_EXPANSION")

// ============================================================================
// MatAddDamageGissmo
// ============================================================================

bool MatAddDamageGissmo::parse(const std::vector<std::string>& lines,
                                util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: MID, NUMFIP, DMGEXP, DCRIT, FADEXP, LCDMG, LCFAD, L_IN
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.mid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.numfip = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.dmgexp = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.dcrit = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.fadexp = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.lcdmg = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.lcfad = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.l_in = parser.getDoubleAt(pos, realW).value_or(0.0);

    ++lineIdx;
    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 2: LCREG, ECRIT
    parser.setLine(lines[lineIdx]);
    pos = 0;

    data_.lcreg = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.ecrit = parser.getDoubleAt(pos, realW).value_or(0.0);

    return true;
}

std::vector<std::string> MatAddDamageGissmo::write(util::CardParser::Format format) const {
    std::vector<std::string> result;

    // Card 1
    {
        util::CardWriter writer(format);
        writer.writeInt(data_.mid);
        writer.writeInt(data_.numfip);
        writer.writeDouble(data_.dmgexp);
        writer.writeDouble(data_.dcrit);
        writer.writeDouble(data_.fadexp);
        writer.writeInt(data_.lcdmg);
        writer.writeInt(data_.lcfad);
        writer.writeDouble(data_.l_in);
        result.push_back(writer.getLine());
    }

    // Card 2
    {
        util::CardWriter writer(format);
        writer.writeInt(data_.lcreg);
        writer.writeDouble(data_.ecrit);
        result.push_back(writer.getLine());
    }

    return result;
}

void MatAddDamageGissmo::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(MatAddDamageGissmo, "*MAT_ADD_DAMAGE_GISSMO")

// ============================================================================
// MatAddCohesive
// ============================================================================

bool MatAddCohesive::parse(const std::vector<std::string>& lines,
                            util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: MID, T, GIC, GIIC, XMU, T_MULT
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.mid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.t = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.gic = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.giic = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.xmu = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.t_mult = parser.getDoubleAt(pos, realW).value_or(1.0);

    return true;
}

std::vector<std::string> MatAddCohesive::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.mid);
    writer.writeDouble(data_.t);
    writer.writeDouble(data_.gic);
    writer.writeDouble(data_.giic);
    writer.writeDouble(data_.xmu);
    writer.writeDouble(data_.t_mult);
    result.push_back(writer.getLine());

    return result;
}

void MatAddCohesive::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(MatAddCohesive, "*MAT_ADD_COHESIVE")

// ============================================================================
// MatAddPermeability
// ============================================================================

bool MatAddPermeability::parse(const std::vector<std::string>& lines,
                                util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: MID, PERM, LCPERM, VOID0, ALPHA
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.mid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.perm = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.lcperm = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.void0 = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.alpha = parser.getDoubleAt(pos, realW).value_or(0.0);

    return true;
}

std::vector<std::string> MatAddPermeability::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.mid);
    writer.writeDouble(data_.perm);
    writer.writeInt(data_.lcperm);
    writer.writeDouble(data_.void0);
    writer.writeDouble(data_.alpha);
    result.push_back(writer.getLine());

    return result;
}

void MatAddPermeability::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(MatAddPermeability, "*MAT_ADD_PERMEABILITY")

// ============================================================================
// MatAddPoreAir
// ============================================================================

bool MatAddPoreAir::parse(const std::vector<std::string>& lines,
                           util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: MID, BULK, PORE_PR, LCBULK
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.mid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.bulk = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.pore_pr = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.lcbulk = static_cast<int>(parser.getInt64At(pos).value_or(0));

    return true;
}

std::vector<std::string> MatAddPoreAir::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.mid);
    writer.writeDouble(data_.bulk);
    writer.writeDouble(data_.pore_pr);
    writer.writeInt(data_.lcbulk);
    result.push_back(writer.getLine());

    return result;
}

void MatAddPoreAir::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(MatAddPoreAir, "*MAT_ADD_PORE_AIR")

// ============================================================================
// MatAddInelasticity
// ============================================================================

bool MatAddInelasticity::parse(const std::vector<std::string>& lines,
                                util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: MID, ITYPE, H, C, P, LCSS
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.mid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.itype = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.h = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.c = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.p = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.lcss = static_cast<int>(parser.getInt64At(pos).value_or(0));

    return true;
}

std::vector<std::string> MatAddInelasticity::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.mid);
    writer.writeInt(data_.itype);
    writer.writeDouble(data_.h);
    writer.writeDouble(data_.c);
    writer.writeDouble(data_.p);
    writer.writeInt(data_.lcss);
    result.push_back(writer.getLine());

    return result;
}

void MatAddInelasticity::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(MatAddInelasticity, "*MAT_ADD_INELASTICITY")

} // namespace koo::dyna
