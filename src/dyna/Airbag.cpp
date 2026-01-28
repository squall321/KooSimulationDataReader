#include <koo/dyna/Airbag.hpp>
#include <koo/dyna/ModelVisitor.hpp>
#include <koo/dyna/KeywordFactory.hpp>
#include <koo/util/StringUtils.hpp>

namespace koo::dyna {

// ============================================================================
// AirbagSimplePressureVolume
// ============================================================================

bool AirbagSimplePressureVolume::parse(const std::vector<std::string>& lines,
                                        util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: SID, SIDTYP, RBID, VSCA, PSCA, VINI, MWD, SPSF
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.sid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.sidtyp = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.rbid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.vsca = parser.getDoubleAt(pos, realW).value_or(1.0);
    pos += realW;
    data_.psca = parser.getDoubleAt(pos, realW).value_or(1.0);
    pos += realW;
    data_.vini = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.mwd = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.spsf = parser.getDoubleAt(pos, realW).value_or(0.0);
    ++lineIdx;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 2: CN, BETA, LCID, LCIDDR
    parser.setLine(lines[lineIdx]);
    pos = 0;

    data_.cn = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.beta = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.lciddr = static_cast<int>(parser.getInt64At(pos).value_or(0));

    return true;
}

std::vector<std::string> AirbagSimplePressureVolume::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    // Card 1
    writer.writeInt(data_.sid);
    writer.writeInt(data_.sidtyp);
    writer.writeInt(data_.rbid);
    writer.writeDouble(data_.vsca);
    writer.writeDouble(data_.psca);
    writer.writeDouble(data_.vini);
    writer.writeDouble(data_.mwd);
    writer.writeDouble(data_.spsf);
    result.push_back(writer.getLine());

    // Card 2
    writer.clear();
    writer.writeInt(data_.cn);
    writer.writeInt(data_.beta);
    writer.writeInt(data_.lcid);
    writer.writeInt(data_.lciddr);
    result.push_back(writer.getLine());

    return result;
}

void AirbagSimplePressureVolume::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(AirbagSimplePressureVolume, "*AIRBAG_SIMPLE_PRESSURE_VOLUME")

// ============================================================================
// AirbagHybrid
// ============================================================================

bool AirbagHybrid::parse(const std::vector<std::string>& lines,
                          util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: SID, SIDTYP, RBID, VSCA, PSCA, VINI, MWD, SPSF
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.sid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.sidtyp = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.rbid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.vsca = parser.getDoubleAt(pos, realW).value_or(1.0);
    pos += realW;
    data_.psca = parser.getDoubleAt(pos, realW).value_or(1.0);
    pos += realW;
    data_.vini = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.mwd = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.spsf = parser.getDoubleAt(pos, realW).value_or(0.0);
    ++lineIdx;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 2: ATMOST, ATMOSP, GC, CC, CCI
    parser.setLine(lines[lineIdx]);
    pos = 0;

    data_.atmost = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.atmosp = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.gc = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.cc = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.cci = static_cast<int>(parser.getInt64At(pos).value_or(0));

    return true;
}

std::vector<std::string> AirbagHybrid::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    // Card 1
    writer.writeInt(data_.sid);
    writer.writeInt(data_.sidtyp);
    writer.writeInt(data_.rbid);
    writer.writeDouble(data_.vsca);
    writer.writeDouble(data_.psca);
    writer.writeDouble(data_.vini);
    writer.writeDouble(data_.mwd);
    writer.writeDouble(data_.spsf);
    result.push_back(writer.getLine());

    // Card 2
    writer.clear();
    writer.writeDouble(data_.atmost);
    writer.writeDouble(data_.atmosp);
    writer.writeDouble(data_.gc);
    writer.writeInt(data_.cc);
    writer.writeInt(data_.cci);
    result.push_back(writer.getLine());

    return result;
}

void AirbagHybrid::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(AirbagHybrid, "*AIRBAG_HYBRID")

// ============================================================================
// AirbagParticle
// ============================================================================

bool AirbagParticle::parse(const std::vector<std::string>& lines,
                            util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: SID, SIDTYP, RBID, VSCA, PSCA, VINI, MWD, SPSF
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.sid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.sidtyp = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.rbid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.vsca = parser.getDoubleAt(pos, realW).value_or(1.0);
    pos += realW;
    data_.psca = parser.getDoubleAt(pos, realW).value_or(1.0);
    pos += realW;
    data_.vini = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.mwd = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.spsf = parser.getDoubleAt(pos, realW).value_or(0.0);
    ++lineIdx;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 2: NP, UNIT, VISCA, TATM, PATM
    parser.setLine(lines[lineIdx]);
    pos = 0;

    data_.np = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.unit = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.visca = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.tatm = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.patm = parser.getDoubleAt(pos, realW).value_or(0.0);

    return true;
}

std::vector<std::string> AirbagParticle::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    // Card 1
    writer.writeInt(data_.sid);
    writer.writeInt(data_.sidtyp);
    writer.writeInt(data_.rbid);
    writer.writeDouble(data_.vsca);
    writer.writeDouble(data_.psca);
    writer.writeDouble(data_.vini);
    writer.writeDouble(data_.mwd);
    writer.writeDouble(data_.spsf);
    result.push_back(writer.getLine());

    // Card 2
    writer.clear();
    writer.writeInt(data_.np);
    writer.writeInt(data_.unit);
    writer.writeDouble(data_.visca);
    writer.writeDouble(data_.tatm);
    writer.writeDouble(data_.patm);
    result.push_back(writer.getLine());

    return result;
}

void AirbagParticle::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(AirbagParticle, "*AIRBAG_PARTICLE")

// ============================================================================
// AirbagReferenceGeometry
// ============================================================================

bool AirbagReferenceGeometry::parse(const std::vector<std::string>& lines,
                                     util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: ID, BIRTH, TBIRTH, TDEATH
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.id = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.birth = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.tbirth = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.tdeath = parser.getDoubleAt(pos, realW).value_or(0.0);

    return true;
}

std::vector<std::string> AirbagReferenceGeometry::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.id);
    writer.writeInt(data_.birth);
    writer.writeDouble(data_.tbirth);
    writer.writeDouble(data_.tdeath);
    result.push_back(writer.getLine());

    return result;
}

void AirbagReferenceGeometry::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(AirbagReferenceGeometry, "*AIRBAG_REFERENCE_GEOMETRY")

// ============================================================================
// AirbagSimpleAirbagModel
// ============================================================================

bool AirbagSimpleAirbagModel::parse(const std::vector<std::string>& lines,
                                     util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: SID, SIDTYP, RBID, VSCA, PSCA, VINI, MWD, SPSF
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.sid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.sidtyp = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.rbid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.vsca = parser.getDoubleAt(pos, realW).value_or(1.0);
    pos += realW;
    data_.psca = parser.getDoubleAt(pos, realW).value_or(1.0);
    pos += realW;
    data_.vini = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.mwd = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.spsf = parser.getDoubleAt(pos, realW).value_or(0.0);
    ++lineIdx;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 2: CV, CP, T, LCID, MU, PE, RO
    parser.setLine(lines[lineIdx]);
    pos = 0;

    data_.cv = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.cp = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.t = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.mu = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.pe = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.ro = parser.getDoubleAt(pos, realW).value_or(0.0);

    return true;
}

std::vector<std::string> AirbagSimpleAirbagModel::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    // Card 1
    writer.writeInt(data_.sid);
    writer.writeInt(data_.sidtyp);
    writer.writeInt(data_.rbid);
    writer.writeDouble(data_.vsca);
    writer.writeDouble(data_.psca);
    writer.writeDouble(data_.vini);
    writer.writeDouble(data_.mwd);
    writer.writeDouble(data_.spsf);
    result.push_back(writer.getLine());

    // Card 2
    writer.clear();
    writer.writeDouble(data_.cv);
    writer.writeDouble(data_.cp);
    writer.writeDouble(data_.t);
    writer.writeInt(data_.lcid);
    writer.writeDouble(data_.mu);
    writer.writeDouble(data_.pe);
    writer.writeDouble(data_.ro);
    result.push_back(writer.getLine());

    return result;
}

void AirbagSimpleAirbagModel::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(AirbagSimpleAirbagModel, "*AIRBAG_SIMPLE_AIRBAG_MODEL")

// ============================================================================
// AirbagWangNefske
// ============================================================================

bool AirbagWangNefske::parse(const std::vector<std::string>& lines,
                              util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: SID, SIDTYP, RBID, VSCA, PSCA, VINI, MWD, SPSF
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.sid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.sidtyp = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.rbid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.vsca = parser.getDoubleAt(pos, realW).value_or(1.0);
    pos += realW;
    data_.psca = parser.getDoubleAt(pos, realW).value_or(1.0);
    pos += realW;
    data_.vini = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.mwd = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.spsf = parser.getDoubleAt(pos, realW).value_or(0.0);
    ++lineIdx;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 2: CN, BETA, LCID, LCIDDR
    parser.setLine(lines[lineIdx]);
    pos = 0;

    data_.cn = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.beta = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.lciddr = static_cast<int>(parser.getInt64At(pos).value_or(0));
    ++lineIdx;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 3: CV, CP, T
    parser.setLine(lines[lineIdx]);
    pos = 0;

    data_.cv = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.cp = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.t = parser.getDoubleAt(pos, realW).value_or(0.0);

    return true;
}

std::vector<std::string> AirbagWangNefske::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    // Card 1
    writer.writeInt(data_.sid);
    writer.writeInt(data_.sidtyp);
    writer.writeInt(data_.rbid);
    writer.writeDouble(data_.vsca);
    writer.writeDouble(data_.psca);
    writer.writeDouble(data_.vini);
    writer.writeDouble(data_.mwd);
    writer.writeDouble(data_.spsf);
    result.push_back(writer.getLine());

    // Card 2
    writer.clear();
    writer.writeDouble(data_.cn);
    writer.writeDouble(data_.beta);
    writer.writeInt(data_.lcid);
    writer.writeInt(data_.lciddr);
    result.push_back(writer.getLine());

    // Card 3
    writer.clear();
    writer.writeDouble(data_.cv);
    writer.writeDouble(data_.cp);
    writer.writeDouble(data_.t);
    result.push_back(writer.getLine());

    return result;
}

void AirbagWangNefske::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(AirbagWangNefske, "*AIRBAG_WANG_NEFSKE")

// ============================================================================
// AirbagLinearFluid
// ============================================================================

bool AirbagLinearFluid::parse(const std::vector<std::string>& lines,
                               util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: SID, SIDTYP, RBID, VSCA, PSCA, VINI, BULK, RO
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.sid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.sidtyp = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.rbid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.vsca = parser.getDoubleAt(pos, realW).value_or(1.0);
    pos += realW;
    data_.psca = parser.getDoubleAt(pos, realW).value_or(1.0);
    pos += realW;
    data_.vini = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.bulk = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.ro = parser.getDoubleAt(pos, realW).value_or(0.0);
    ++lineIdx;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 2: PE, LCID
    parser.setLine(lines[lineIdx]);
    pos = 0;

    data_.pe = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));

    return true;
}

std::vector<std::string> AirbagLinearFluid::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    // Card 1
    writer.writeInt(data_.sid);
    writer.writeInt(data_.sidtyp);
    writer.writeInt(data_.rbid);
    writer.writeDouble(data_.vsca);
    writer.writeDouble(data_.psca);
    writer.writeDouble(data_.vini);
    writer.writeDouble(data_.bulk);
    writer.writeDouble(data_.ro);
    result.push_back(writer.getLine());

    // Card 2
    writer.clear();
    writer.writeDouble(data_.pe);
    writer.writeInt(data_.lcid);
    result.push_back(writer.getLine());

    return result;
}

void AirbagLinearFluid::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(AirbagLinearFluid, "*AIRBAG_LINEAR_FLUID")

// ============================================================================
// AirbagAdiabaticGasModel
// ============================================================================

bool AirbagAdiabaticGasModel::parse(const std::vector<std::string>& lines,
                                     util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: SID, SIDTYP, RBID, VSCA, PSCA, VINI, MWD, SPSF
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.sid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.sidtyp = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.rbid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.vsca = parser.getDoubleAt(pos, realW).value_or(1.0);
    pos += realW;
    data_.psca = parser.getDoubleAt(pos, realW).value_or(1.0);
    pos += realW;
    data_.vini = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.mwd = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.spsf = parser.getDoubleAt(pos, realW).value_or(0.0);
    ++lineIdx;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 2: GAMMA, PE, RO
    parser.setLine(lines[lineIdx]);
    pos = 0;

    data_.gamma = parser.getDoubleAt(pos, realW).value_or(1.4);
    pos += realW;
    data_.pe = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.ro = parser.getDoubleAt(pos, realW).value_or(0.0);

    return true;
}

std::vector<std::string> AirbagAdiabaticGasModel::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    // Card 1
    writer.writeInt(data_.sid);
    writer.writeInt(data_.sidtyp);
    writer.writeInt(data_.rbid);
    writer.writeDouble(data_.vsca);
    writer.writeDouble(data_.psca);
    writer.writeDouble(data_.vini);
    writer.writeDouble(data_.mwd);
    writer.writeDouble(data_.spsf);
    result.push_back(writer.getLine());

    // Card 2
    writer.clear();
    writer.writeDouble(data_.gamma);
    writer.writeDouble(data_.pe);
    writer.writeDouble(data_.ro);
    result.push_back(writer.getLine());

    return result;
}

void AirbagAdiabaticGasModel::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(AirbagAdiabaticGasModel, "*AIRBAG_ADIABATIC_GAS_MODEL")

// ============================================================================
// AirbagInteraction
// ============================================================================

bool AirbagInteraction::parse(const std::vector<std::string>& lines,
                               util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: MASTER, SLAVE, AREA, LCID, SHAPE, MU
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.master = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.slave = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.area = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.shape = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.mu = parser.getDoubleAt(pos, realW).value_or(0.0);

    return true;
}

std::vector<std::string> AirbagInteraction::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.master);
    writer.writeInt(data_.slave);
    writer.writeDouble(data_.area);
    writer.writeInt(data_.lcid);
    writer.writeInt(data_.shape);
    writer.writeDouble(data_.mu);
    result.push_back(writer.getLine());

    return result;
}

void AirbagInteraction::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(AirbagInteraction, "*AIRBAG_INTERACTION")

// ============================================================================
// AirbagLoadCurve
// ============================================================================

bool AirbagLoadCurve::parse(const std::vector<std::string>& lines,
                             util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: SID, SIDTYP, RBID, VSCA, PSCA, VINI, MWD, SPSF
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.sid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.sidtyp = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.rbid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.vsca = parser.getDoubleAt(pos, realW).value_or(1.0);
    pos += realW;
    data_.psca = parser.getDoubleAt(pos, realW).value_or(1.0);
    pos += realW;
    data_.vini = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.mwd = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.spsf = parser.getDoubleAt(pos, realW).value_or(0.0);
    ++lineIdx;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 2: CN, BETA, LCID, LCIDDR, LCIDT, LCIDR, GCFLAG, GCID
    parser.setLine(lines[lineIdx]);
    pos = 0;

    data_.cn = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.beta = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.lciddr = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.lcidt = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.lcidr = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.gcflag = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.gcid = static_cast<int>(parser.getInt64At(pos).value_or(0));

    return true;
}

std::vector<std::string> AirbagLoadCurve::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    // Card 1
    writer.writeInt(data_.sid);
    writer.writeInt(data_.sidtyp);
    writer.writeInt(data_.rbid);
    writer.writeDouble(data_.vsca);
    writer.writeDouble(data_.psca);
    writer.writeDouble(data_.vini);
    writer.writeDouble(data_.mwd);
    writer.writeDouble(data_.spsf);
    result.push_back(writer.getLine());

    // Card 2
    writer.clear();
    writer.writeInt(data_.cn);
    writer.writeInt(data_.beta);
    writer.writeInt(data_.lcid);
    writer.writeInt(data_.lciddr);
    writer.writeDouble(data_.lcidt);
    writer.writeDouble(data_.lcidr);
    writer.writeInt(data_.gcflag);
    writer.writeInt(data_.gcid);
    result.push_back(writer.getLine());

    return result;
}

void AirbagLoadCurve::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(AirbagLoadCurve, "*AIRBAG_LOAD_CURVE")

} // namespace koo::dyna
