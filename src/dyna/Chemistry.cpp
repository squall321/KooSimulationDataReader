#include <koo/dyna/Chemistry.hpp>
#include <koo/dyna/ModelVisitor.hpp>
#include <koo/dyna/KeywordFactory.hpp>
#include <koo/util/StringUtils.hpp>

namespace koo::dyna {

// ============================================================================
// ChemistryControlInflator
// ============================================================================

bool ChemistryControlInflator::parse(const std::vector<std::string>& lines,
                                      util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: SID, INFID, ATEFR, LCID, LCIDT
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.sid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.infid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.atefr = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.lcidt = static_cast<int>(parser.getInt64At(pos).value_or(0));

    return true;
}

std::vector<std::string> ChemistryControlInflator::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.sid);
    writer.writeInt(data_.infid);
    writer.writeDouble(data_.atefr);
    writer.writeInt(data_.lcid);
    writer.writeInt(data_.lcidt);
    result.push_back(writer.getLine());

    return result;
}

void ChemistryControlInflator::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ChemistryControlInflator, "*CHEMISTRY_CONTROL_INFLATOR")

// ============================================================================
// ChemistryComposition
// ============================================================================

bool ChemistryComposition::parse(const std::vector<std::string>& lines,
                                  util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: COMPID, NSPEC
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.compid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.nspec = static_cast<int>(parser.getInt64At(pos).value_or(0));

    ++lineIdx;

    // Species cards
    while (lineIdx < lines.size()) {
        while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
            ++lineIdx;
        }
        if (lineIdx >= lines.size()) break;

        parser.setLine(lines[lineIdx]);
        pos = 0;

        Species spec;
        spec.specid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        spec.mfrac = parser.getDoubleAt(pos, realW).value_or(0.0);

        species_.push_back(spec);
        ++lineIdx;
    }

    return true;
}

std::vector<std::string> ChemistryComposition::write(util::CardParser::Format format) const {
    std::vector<std::string> result;

    // Card 1
    {
        util::CardWriter writer(format);
        writer.writeInt(data_.compid);
        writer.writeInt(data_.nspec);
        result.push_back(writer.getLine());
    }

    // Species cards
    for (const auto& spec : species_) {
        util::CardWriter writer(format);
        writer.writeInt(spec.specid);
        writer.writeDouble(spec.mfrac);
        result.push_back(writer.getLine());
    }

    return result;
}

void ChemistryComposition::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ChemistryComposition, "*CHEMISTRY_COMPOSITION")

// ============================================================================
// ChemistryInflatorProperties
// ============================================================================

bool ChemistryInflatorProperties::parse(const std::vector<std::string>& lines,
                                         util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: INFID, CHMASS, CHTEMP, CHPRES, COMPID
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.infid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.chmass = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.chtemp = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.chpres = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.compid = static_cast<int>(parser.getInt64At(pos).value_or(0));

    return true;
}

std::vector<std::string> ChemistryInflatorProperties::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.infid);
    writer.writeDouble(data_.chmass);
    writer.writeDouble(data_.chtemp);
    writer.writeDouble(data_.chpres);
    writer.writeInt(data_.compid);
    result.push_back(writer.getLine());

    return result;
}

void ChemistryInflatorProperties::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ChemistryInflatorProperties, "*CHEMISTRY_INFLATOR_PROPERTIES")

// ============================================================================
// ChemistryControlZnd
// ============================================================================

bool ChemistryControlZnd::parse(const std::vector<std::string>& lines,
                                 util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: ZNDID, DCJVEL, PCJ, RHOCJ, LCID
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.zndid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.dcjvel = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.pcj = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.rhocj = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));

    return true;
}

std::vector<std::string> ChemistryControlZnd::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.zndid);
    writer.writeDouble(data_.dcjvel);
    writer.writeDouble(data_.pcj);
    writer.writeDouble(data_.rhocj);
    writer.writeInt(data_.lcid);
    result.push_back(writer.getLine());

    return result;
}

void ChemistryControlZnd::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ChemistryControlZnd, "*CHEMISTRY_CONTROL_ZND")

// ============================================================================
// ChemistryModel
// ============================================================================

bool ChemistryModel::parse(const std::vector<std::string>& lines,
                            util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: MODID, MTYPE, ATEMP, ACOEF, ECOEF
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.modid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.mtype = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.atemp = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.acoef = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.ecoef = parser.getDoubleAt(pos, realW).value_or(0.0);

    return true;
}

std::vector<std::string> ChemistryModel::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.modid);
    writer.writeInt(data_.mtype);
    writer.writeDouble(data_.atemp);
    writer.writeDouble(data_.acoef);
    writer.writeDouble(data_.ecoef);
    result.push_back(writer.getLine());

    return result;
}

void ChemistryModel::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ChemistryModel, "*CHEMISTRY_MODEL")

// ============================================================================
// ChemistrySpecies
// ============================================================================

bool ChemistrySpecies::parse(const std::vector<std::string>& lines,
                              util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: SPECID, MOLWT, HFORM, CP
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.specid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.molwt = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.hform = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.cp = parser.getDoubleAt(pos, realW).value_or(0.0);

    return true;
}

std::vector<std::string> ChemistrySpecies::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.specid);
    writer.writeDouble(data_.molwt);
    writer.writeDouble(data_.hform);
    writer.writeDouble(data_.cp);
    result.push_back(writer.getLine());

    return result;
}

void ChemistrySpecies::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ChemistrySpecies, "*CHEMISTRY_SPECIES")

// ============================================================================
// ChemistryReaction
// ============================================================================

bool ChemistryReaction::parse(const std::vector<std::string>& lines,
                               util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: RXNID, MODID, ARRHENIUS_A, ARRHENIUS_B, ARRHENIUS_E
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.rxnid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.modid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.arrhenius_a = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.arrhenius_b = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.arrhenius_e = parser.getDoubleAt(pos, realW).value_or(0.0);

    return true;
}

std::vector<std::string> ChemistryReaction::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.rxnid);
    writer.writeInt(data_.modid);
    writer.writeDouble(data_.arrhenius_a);
    writer.writeDouble(data_.arrhenius_b);
    writer.writeDouble(data_.arrhenius_e);
    result.push_back(writer.getLine());

    return result;
}

void ChemistryReaction::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ChemistryReaction, "*CHEMISTRY_REACTION")

// ============================================================================
// ChemistryControl
// ============================================================================

bool ChemistryControl::parse(const std::vector<std::string>& lines,
                              util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: CTRLID, METHOD, DTMIN, DTMAX, MAXITER, TOL
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.ctrlid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.method = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.dtmin = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.dtmax = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.maxiter = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.tol = parser.getDoubleAt(pos, realW).value_or(0.0);

    return true;
}

std::vector<std::string> ChemistryControl::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.ctrlid);
    writer.writeInt(data_.method);
    writer.writeDouble(data_.dtmin);
    writer.writeDouble(data_.dtmax);
    writer.writeInt(data_.maxiter);
    writer.writeDouble(data_.tol);
    result.push_back(writer.getLine());

    return result;
}

void ChemistryControl::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ChemistryControl, "*CHEMISTRY_CONTROL")

// ============================================================================
// ChemistryDetonation
// ============================================================================

bool ChemistryDetonation::parse(const std::vector<std::string>& lines,
                                 util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: DETID, XDET, YDET, ZDET, TDET, DVEL
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.detid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.xdet = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.ydet = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.zdet = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.tdet = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.dvel = parser.getDoubleAt(pos, realW).value_or(0.0);

    return true;
}

std::vector<std::string> ChemistryDetonation::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.detid);
    writer.writeDouble(data_.xdet);
    writer.writeDouble(data_.ydet);
    writer.writeDouble(data_.zdet);
    writer.writeDouble(data_.tdet);
    writer.writeDouble(data_.dvel);
    result.push_back(writer.getLine());

    return result;
}

void ChemistryDetonation::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ChemistryDetonation, "*CHEMISTRY_DETONATION")

} // namespace koo::dyna
