#include <koo/dyna/Frequency.hpp>
#include <koo/dyna/ModelVisitor.hpp>
#include <koo/dyna/KeywordFactory.hpp>
#include <koo/util/StringUtils.hpp>

namespace koo::dyna {

// ============================================================================
// FrequencyDomainAcousticFem
// ============================================================================

bool FrequencyDomainAcousticFem::parse(const std::vector<std::string>& lines,
                                        util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: NFREQ, FMIN, FMAX, MDMIN, MDMAX, DAMPF, LCDAM, LCTYP
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.nfreq = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.fmin = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.fmax = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.mdmin = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.mdmax = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.dampf = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.lcdam = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.lctyp = static_cast<int>(parser.getInt64At(pos).value_or(0));

    return true;
}

std::vector<std::string> FrequencyDomainAcousticFem::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.nfreq);
    writer.writeDouble(data_.fmin);
    writer.writeDouble(data_.fmax);
    writer.writeInt(data_.mdmin);
    writer.writeInt(data_.mdmax);
    writer.writeDouble(data_.dampf);
    writer.writeInt(data_.lcdam);
    writer.writeInt(data_.lctyp);
    result.push_back(writer.getLine());

    return result;
}

void FrequencyDomainAcousticFem::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(FrequencyDomainAcousticFem, "*FREQUENCY_DOMAIN_ACOUSTIC_FEM")

// ============================================================================
// FrequencyDomainFrf
// ============================================================================

bool FrequencyDomainFrf::parse(const std::vector<std::string>& lines,
                                util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: N1, N2, DESSION, RESSION, DAMPF, MDMIN, MDMAX
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.n1 = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.n2 = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.dession = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.ression = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.dampf = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.mdmin = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.mdmax = static_cast<int>(parser.getInt64At(pos).value_or(0));
    ++lineIdx;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 2: FNMIN, FNMAX, FMIN, FMAX, NFREQ, LCDAM
    parser.setLine(lines[lineIdx]);
    pos = 0;

    data_.fnmin = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.fnmax = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.fmin = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.fmax = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.nfreq = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.lcdam = static_cast<int>(parser.getInt64At(pos).value_or(0));

    return true;
}

std::vector<std::string> FrequencyDomainFrf::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    // Card 1
    writer.writeInt(data_.n1);
    writer.writeInt(data_.n2);
    writer.writeInt(data_.dession);
    writer.writeInt(data_.ression);
    writer.writeDouble(data_.dampf);
    writer.writeInt(data_.mdmin);
    writer.writeInt(data_.mdmax);
    result.push_back(writer.getLine());

    // Card 2
    writer.clear();
    writer.writeInt(data_.fnmin);
    writer.writeInt(data_.fnmax);
    writer.writeDouble(data_.fmin);
    writer.writeDouble(data_.fmax);
    writer.writeInt(data_.nfreq);
    writer.writeInt(data_.lcdam);
    result.push_back(writer.getLine());

    return result;
}

void FrequencyDomainFrf::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(FrequencyDomainFrf, "*FREQUENCY_DOMAIN_FRF")

// ============================================================================
// FrequencyDomainRandomVibration
// ============================================================================

bool FrequencyDomainRandomVibration::parse(const std::vector<std::string>& lines,
                                            util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: LDPSD, VESSION, RESPID, MDMIN, MDMAX, DAMPF, LCDAM
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.ldpsd = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.vession = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.respid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.mdmin = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.mdmax = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.dampf = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.lcdam = static_cast<int>(parser.getInt64At(pos).value_or(0));
    ++lineIdx;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 2: FMIN, FMAX, NFREQ
    parser.setLine(lines[lineIdx]);
    pos = 0;

    data_.fmin = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.fmax = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.nfreq = static_cast<int>(parser.getInt64At(pos).value_or(0));

    return true;
}

std::vector<std::string> FrequencyDomainRandomVibration::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    // Card 1
    writer.writeInt(data_.ldpsd);
    writer.writeInt(data_.vession);
    writer.writeInt(data_.respid);
    writer.writeInt(data_.mdmin);
    writer.writeInt(data_.mdmax);
    writer.writeDouble(data_.dampf);
    writer.writeInt(data_.lcdam);
    result.push_back(writer.getLine());

    // Card 2
    writer.clear();
    writer.writeDouble(data_.fmin);
    writer.writeDouble(data_.fmax);
    writer.writeInt(data_.nfreq);
    result.push_back(writer.getLine());

    return result;
}

void FrequencyDomainRandomVibration::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(FrequencyDomainRandomVibration, "*FREQUENCY_DOMAIN_RANDOM_VIBRATION")

// ============================================================================
// FrequencyDomainSsd
// ============================================================================

bool FrequencyDomainSsd::parse(const std::vector<std::string>& lines,
                                util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: MDMIN, MDMAX, DAMPF, LCDAM, LCTYP, FMIN, FMAX, NFREQ
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.mdmin = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.mdmax = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.dampf = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.lcdam = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.lctyp = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.fmin = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.fmax = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.nfreq = static_cast<int>(parser.getInt64At(pos).value_or(0));

    return true;
}

std::vector<std::string> FrequencyDomainSsd::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.mdmin);
    writer.writeInt(data_.mdmax);
    writer.writeDouble(data_.dampf);
    writer.writeInt(data_.lcdam);
    writer.writeInt(data_.lctyp);
    writer.writeDouble(data_.fmin);
    writer.writeDouble(data_.fmax);
    writer.writeInt(data_.nfreq);
    result.push_back(writer.getLine());

    return result;
}

void FrequencyDomainSsd::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(FrequencyDomainSsd, "*FREQUENCY_DOMAIN_SSD")

// ============================================================================
// FrequencyDomainAcousticBem
// ============================================================================

bool FrequencyDomainAcousticBem::parse(const std::vector<std::string>& lines,
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

    data_.nfreq = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.fmin = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.fmax = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.ssid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.rho = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.bulk = parser.getDoubleAt(pos, realW).value_or(0.0);

    return true;
}

std::vector<std::string> FrequencyDomainAcousticBem::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.nfreq);
    writer.writeDouble(data_.fmin);
    writer.writeDouble(data_.fmax);
    writer.writeInt(data_.ssid);
    writer.writeDouble(data_.rho);
    writer.writeDouble(data_.bulk);
    result.push_back(writer.getLine());

    return result;
}

void FrequencyDomainAcousticBem::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(FrequencyDomainAcousticBem, "*FREQUENCY_DOMAIN_ACOUSTIC_BEM")

// ============================================================================
// FrequencyDomainMode
// ============================================================================

bool FrequencyDomainMode::parse(const std::vector<std::string>& lines,
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

    data_.mdmin = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.mdmax = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.fnmin = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.fnmax = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.fmin = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.fmax = parser.getDoubleAt(pos, realW).value_or(0.0);

    return true;
}

std::vector<std::string> FrequencyDomainMode::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.mdmin);
    writer.writeInt(data_.mdmax);
    writer.writeInt(data_.fnmin);
    writer.writeInt(data_.fnmax);
    writer.writeDouble(data_.fmin);
    writer.writeDouble(data_.fmax);
    result.push_back(writer.getLine());

    return result;
}

void FrequencyDomainMode::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(FrequencyDomainMode, "*FREQUENCY_DOMAIN_MODE")

// ============================================================================
// FrequencyDomainPath
// ============================================================================

bool FrequencyDomainPath::parse(const std::vector<std::string>& lines,
                                 util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.id = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.n1 = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.n2 = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.npts = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.type = static_cast<int>(parser.getInt64At(pos).value_or(0));

    return true;
}

std::vector<std::string> FrequencyDomainPath::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.id);
    writer.writeInt(data_.n1);
    writer.writeInt(data_.n2);
    writer.writeInt(data_.npts);
    writer.writeInt(data_.type);
    result.push_back(writer.getLine());

    return result;
}

void FrequencyDomainPath::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(FrequencyDomainPath, "*FREQUENCY_DOMAIN_PATH")

// ============================================================================
// FrequencyDomainResponseSpectrum
// ============================================================================

bool FrequencyDomainResponseSpectrum::parse(const std::vector<std::string>& lines,
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

    data_.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.mdmin = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.mdmax = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.dampf = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.dession = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.comb = static_cast<int>(parser.getInt64At(pos).value_or(0));

    return true;
}

std::vector<std::string> FrequencyDomainResponseSpectrum::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.lcid);
    writer.writeInt(data_.mdmin);
    writer.writeInt(data_.mdmax);
    writer.writeDouble(data_.dampf);
    writer.writeInt(data_.dession);
    writer.writeInt(data_.comb);
    result.push_back(writer.getLine());

    return result;
}

void FrequencyDomainResponseSpectrum::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(FrequencyDomainResponseSpectrum, "*FREQUENCY_DOMAIN_RESPONSE_SPECTRUM")

// ============================================================================
// FrequencyDomainSsdDirect
// ============================================================================

bool FrequencyDomainSsdDirect::parse(const std::vector<std::string>& lines,
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

    data_.fmin = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.fmax = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.nfreq = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.dampf = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.lcdam = static_cast<int>(parser.getInt64At(pos).value_or(0));

    return true;
}

std::vector<std::string> FrequencyDomainSsdDirect::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeDouble(data_.fmin);
    writer.writeDouble(data_.fmax);
    writer.writeInt(data_.nfreq);
    writer.writeDouble(data_.dampf);
    writer.writeInt(data_.lcdam);
    result.push_back(writer.getLine());

    return result;
}

void FrequencyDomainSsdDirect::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(FrequencyDomainSsdDirect, "*FREQUENCY_DOMAIN_SSD_DIRECT")

// ============================================================================
// FrequencyDomainVibrationFatigue
// ============================================================================

bool FrequencyDomainVibrationFatigue::parse(const std::vector<std::string>& lines,
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

    data_.ldpsd = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.respid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.mdmin = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.mdmax = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.dampf = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.sth = parser.getDoubleAt(pos, realW).value_or(0.0);

    return true;
}

std::vector<std::string> FrequencyDomainVibrationFatigue::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.ldpsd);
    writer.writeInt(data_.respid);
    writer.writeInt(data_.mdmin);
    writer.writeInt(data_.mdmax);
    writer.writeDouble(data_.dampf);
    writer.writeDouble(data_.sth);
    result.push_back(writer.getLine());

    return result;
}

void FrequencyDomainVibrationFatigue::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(FrequencyDomainVibrationFatigue, "*FREQUENCY_DOMAIN_VIBRATION_FATIGUE")

} // namespace koo::dyna
