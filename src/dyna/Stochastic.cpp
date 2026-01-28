#include <koo/dyna/Stochastic.hpp>
#include <koo/dyna/ModelVisitor.hpp>
#include <koo/dyna/KeywordFactory.hpp>
#include <koo/util/StringUtils.hpp>

namespace koo::dyna {

// ============================================================================
// StochasticStructureField
// ============================================================================

bool StochasticStructureField::parse(const std::vector<std::string>& lines,
                                      util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: FIELDID, FTYPE, MEAN, STDDEV, DISTTYPE, CORLEN
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.fieldid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.ftype = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.mean = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.stddev = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.disttype = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.corlen = parser.getDoubleAt(pos, realW).value_or(0.0);

    return true;
}

std::vector<std::string> StochasticStructureField::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.fieldid);
    writer.writeInt(data_.ftype);
    writer.writeDouble(data_.mean);
    writer.writeDouble(data_.stddev);
    writer.writeInt(data_.disttype);
    writer.writeDouble(data_.corlen);
    result.push_back(writer.getLine());

    return result;
}

void StochasticStructureField::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(StochasticStructureField, "*STOCHASTIC_STRUCTURE_FIELD")

// ============================================================================
// StochasticRandomization
// ============================================================================

bool StochasticRandomization::parse(const std::vector<std::string>& lines,
                                     util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: SEED, NSAMPLES, METHOD, OUTPUT
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.seed = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.nsamples = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.method = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.output = static_cast<int>(parser.getInt64At(pos).value_or(0));

    return true;
}

std::vector<std::string> StochasticRandomization::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.seed);
    writer.writeInt(data_.nsamples);
    writer.writeInt(data_.method);
    writer.writeInt(data_.output);
    result.push_back(writer.getLine());

    return result;
}

void StochasticRandomization::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(StochasticRandomization, "*STOCHASTIC_RANDOMIZATION")

// ============================================================================
// StochasticVariable
// ============================================================================

bool StochasticVariable::parse(const std::vector<std::string>& lines,
                                util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: VARID, MEAN, STDDEV, DISTTYPE, PARAM1, PARAM2
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.varid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.mean = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.stddev = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.disttype = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.param1 = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.param2 = parser.getDoubleAt(pos, realW).value_or(0.0);

    return true;
}

std::vector<std::string> StochasticVariable::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.varid);
    writer.writeDouble(data_.mean);
    writer.writeDouble(data_.stddev);
    writer.writeInt(data_.disttype);
    writer.writeDouble(data_.param1);
    writer.writeDouble(data_.param2);
    result.push_back(writer.getLine());

    return result;
}

void StochasticVariable::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(StochasticVariable, "*STOCHASTIC_VARIABLE")

// ============================================================================
// StochasticCorrelatedField
// ============================================================================

bool StochasticCorrelatedField::parse(const std::vector<std::string>& lines,
                                       util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: FIELDID, CORTYPE, LX, LY, LZ, NKL
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.fieldid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.cortype = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.lx = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.ly = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.lz = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.nkl = static_cast<int>(parser.getInt64At(pos).value_or(0));

    return true;
}

std::vector<std::string> StochasticCorrelatedField::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.fieldid);
    writer.writeInt(data_.cortype);
    writer.writeDouble(data_.lx);
    writer.writeDouble(data_.ly);
    writer.writeDouble(data_.lz);
    writer.writeInt(data_.nkl);
    result.push_back(writer.getLine());

    return result;
}

void StochasticCorrelatedField::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(StochasticCorrelatedField, "*STOCHASTIC_CORRELATED_FIELD")

// ============================================================================
// StochasticMaterialProperty
// ============================================================================

bool StochasticMaterialProperty::parse(const std::vector<std::string>& lines,
                                        util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: MID, PROPID, VARID, SCALE
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.mid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.propid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.varid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.scale = parser.getDoubleAt(pos, realW).value_or(1.0);

    return true;
}

std::vector<std::string> StochasticMaterialProperty::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.mid);
    writer.writeInt(data_.propid);
    writer.writeInt(data_.varid);
    writer.writeDouble(data_.scale);
    result.push_back(writer.getLine());

    return result;
}

void StochasticMaterialProperty::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(StochasticMaterialProperty, "*STOCHASTIC_MATERIAL_PROPERTY")

// ============================================================================
// StochasticPart
// ============================================================================

bool StochasticPart::parse(const std::vector<std::string>& lines,
                            util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: PID, FIELDID, MAPTYPE, TOL
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.pid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.fieldid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.maptype = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.tol = parser.getDoubleAt(pos, realW).value_or(0.0);

    return true;
}

std::vector<std::string> StochasticPart::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.pid);
    writer.writeInt(data_.fieldid);
    writer.writeInt(data_.maptype);
    writer.writeDouble(data_.tol);
    result.push_back(writer.getLine());

    return result;
}

void StochasticPart::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(StochasticPart, "*STOCHASTIC_PART")

// ============================================================================
// StochasticSample
// ============================================================================

bool StochasticSample::parse(const std::vector<std::string>& lines,
                              util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: SAMPLEID, METHOD, NSAMPLES, SEED, OUTPUT
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.sampleid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.method = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.nsamples = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.seed = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.output = static_cast<int>(parser.getInt64At(pos).value_or(0));

    return true;
}

std::vector<std::string> StochasticSample::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.sampleid);
    writer.writeInt(data_.method);
    writer.writeInt(data_.nsamples);
    writer.writeInt(data_.seed);
    writer.writeInt(data_.output);
    result.push_back(writer.getLine());

    return result;
}

void StochasticSample::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(StochasticSample, "*STOCHASTIC_SAMPLE")

// ============================================================================
// StochasticOutput
// ============================================================================

bool StochasticOutput::parse(const std::vector<std::string>& lines,
                              util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: OUTTYPE, HISTBIN, CONF, STATS, CORR
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.outtype = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.histbin = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.conf = parser.getDoubleAt(pos, realW).value_or(0.95);
    pos += realW;
    data_.stats = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.corr = static_cast<int>(parser.getInt64At(pos).value_or(0));

    return true;
}

std::vector<std::string> StochasticOutput::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.outtype);
    writer.writeInt(data_.histbin);
    writer.writeDouble(data_.conf);
    writer.writeInt(data_.stats);
    writer.writeInt(data_.corr);
    result.push_back(writer.getLine());

    return result;
}

void StochasticOutput::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(StochasticOutput, "*STOCHASTIC_OUTPUT")

// ============================================================================
// StochasticMonteCarloRun
// ============================================================================

bool StochasticMonteCarloRun::parse(const std::vector<std::string>& lines,
                                     util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: RUNID, NRUNS, SEED, PARALLEL, RESTART, OUTFREQ
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.runid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.nruns = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.seed = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.parallel = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.restart = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.outfreq = static_cast<int>(parser.getInt64At(pos).value_or(0));

    return true;
}

std::vector<std::string> StochasticMonteCarloRun::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.runid);
    writer.writeInt(data_.nruns);
    writer.writeInt(data_.seed);
    writer.writeInt(data_.parallel);
    writer.writeInt(data_.restart);
    writer.writeInt(data_.outfreq);
    result.push_back(writer.getLine());

    return result;
}

void StochasticMonteCarloRun::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(StochasticMonteCarloRun, "*STOCHASTIC_MONTE_CARLO_RUN")

} // namespace koo::dyna
