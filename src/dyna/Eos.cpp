#include <koo/dyna/Eos.hpp>
#include <koo/dyna/ModelVisitor.hpp>
#include <koo/dyna/KeywordFactory.hpp>
#include <koo/util/StringUtils.hpp>

namespace koo::dyna {

// ============================================================================
// EosLinearPolynomial
// ============================================================================

bool EosLinearPolynomial::parse(const std::vector<std::string>& lines,
                                 util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: EOSID, C0, C1, C2, C3, C4, C5, C6
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.eosid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.c0 = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.c1 = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.c2 = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.c3 = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.c4 = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.c5 = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.c6 = parser.getDoubleAt(pos, realW).value_or(0.0);

    ++lineIdx;
    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 2: E0, V0
    parser.setLine(lines[lineIdx]);
    pos = 0;

    data_.e0 = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.v0 = parser.getDoubleAt(pos, realW).value_or(1.0);

    return true;
}

std::vector<std::string> EosLinearPolynomial::write(util::CardParser::Format format) const {
    std::vector<std::string> result;

    // Card 1
    {
        util::CardWriter writer(format);
        writer.writeInt(data_.eosid);
        writer.writeDouble(data_.c0);
        writer.writeDouble(data_.c1);
        writer.writeDouble(data_.c2);
        writer.writeDouble(data_.c3);
        writer.writeDouble(data_.c4);
        writer.writeDouble(data_.c5);
        writer.writeDouble(data_.c6);
        result.push_back(writer.getLine());
    }

    // Card 2
    {
        util::CardWriter writer(format);
        writer.writeDouble(data_.e0);
        writer.writeDouble(data_.v0);
        result.push_back(writer.getLine());
    }

    return result;
}

void EosLinearPolynomial::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(EosLinearPolynomial, "*EOS_LINEAR_POLYNOMIAL")

// ============================================================================
// EosGruneisen
// ============================================================================

bool EosGruneisen::parse(const std::vector<std::string>& lines,
                          util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: EOSID, C, S1, S2, S3, GAMAO, A, E0
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.eosid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.c = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.s1 = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.s2 = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.s3 = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.gamao = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.a = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.e0 = parser.getDoubleAt(pos, realW).value_or(0.0);

    ++lineIdx;
    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 2: V0
    parser.setLine(lines[lineIdx]);
    pos = 0;

    data_.v0 = parser.getDoubleAt(pos, realW).value_or(1.0);

    return true;
}

std::vector<std::string> EosGruneisen::write(util::CardParser::Format format) const {
    std::vector<std::string> result;

    // Card 1
    {
        util::CardWriter writer(format);
        writer.writeInt(data_.eosid);
        writer.writeDouble(data_.c);
        writer.writeDouble(data_.s1);
        writer.writeDouble(data_.s2);
        writer.writeDouble(data_.s3);
        writer.writeDouble(data_.gamao);
        writer.writeDouble(data_.a);
        writer.writeDouble(data_.e0);
        result.push_back(writer.getLine());
    }

    // Card 2
    {
        util::CardWriter writer(format);
        writer.writeDouble(data_.v0);
        result.push_back(writer.getLine());
    }

    return result;
}

void EosGruneisen::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(EosGruneisen, "*EOS_GRUNEISEN")

// ============================================================================
// EosJwl
// ============================================================================

bool EosJwl::parse(const std::vector<std::string>& lines,
                    util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: EOSID, A, B, R1, R2, OMEG, E0, V0
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.eosid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.a = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.b = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.r1 = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.r2 = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.omeg = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.e0 = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.v0 = parser.getDoubleAt(pos, realW).value_or(1.0);

    return true;
}

std::vector<std::string> EosJwl::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.eosid);
    writer.writeDouble(data_.a);
    writer.writeDouble(data_.b);
    writer.writeDouble(data_.r1);
    writer.writeDouble(data_.r2);
    writer.writeDouble(data_.omeg);
    writer.writeDouble(data_.e0);
    writer.writeDouble(data_.v0);
    result.push_back(writer.getLine());

    return result;
}

void EosJwl::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(EosJwl, "*EOS_JWL")

// ============================================================================
// EosIdealGas
// ============================================================================

bool EosIdealGas::parse(const std::vector<std::string>& lines,
                         util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: EOSID, CV, CP, T0, V0
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.eosid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.cv = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.cp = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.t0 = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.v0 = parser.getDoubleAt(pos, realW).value_or(1.0);

    return true;
}

std::vector<std::string> EosIdealGas::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.eosid);
    writer.writeDouble(data_.cv);
    writer.writeDouble(data_.cp);
    writer.writeDouble(data_.t0);
    writer.writeDouble(data_.v0);
    result.push_back(writer.getLine());

    return result;
}

void EosIdealGas::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(EosIdealGas, "*EOS_IDEAL_GAS")

// ============================================================================
// EosTabulatedCompaction
// ============================================================================

bool EosTabulatedCompaction::parse(const std::vector<std::string>& lines,
                                    util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: EOSID, GAMA, E0, V0, LCID, LCID2
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.eosid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.gama = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.e0 = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.v0 = parser.getDoubleAt(pos, realW).value_or(1.0);
    pos += realW;
    data_.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.lcid2 = static_cast<int>(parser.getInt64At(pos).value_or(0));

    return true;
}

std::vector<std::string> EosTabulatedCompaction::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.eosid);
    writer.writeDouble(data_.gama);
    writer.writeDouble(data_.e0);
    writer.writeDouble(data_.v0);
    writer.writeInt(data_.lcid);
    writer.writeInt(data_.lcid2);
    result.push_back(writer.getLine());

    return result;
}

void EosTabulatedCompaction::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(EosTabulatedCompaction, "*EOS_TABULATED_COMPACTION")

// ============================================================================
// EosIgnitionGrowth
// ============================================================================

bool EosIgnitionGrowth::parse(const std::vector<std::string>& lines,
                               util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: EOSID, A, B, R1, R2, OMEG, E0, V0
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.eosid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.a = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.b = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.r1 = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.r2 = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.omeg = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.e0 = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.v0 = parser.getDoubleAt(pos, realW).value_or(1.0);

    return true;
}

std::vector<std::string> EosIgnitionGrowth::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.eosid);
    writer.writeDouble(data_.a);
    writer.writeDouble(data_.b);
    writer.writeDouble(data_.r1);
    writer.writeDouble(data_.r2);
    writer.writeDouble(data_.omeg);
    writer.writeDouble(data_.e0);
    writer.writeDouble(data_.v0);
    result.push_back(writer.getLine());

    return result;
}

void EosIgnitionGrowth::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(EosIgnitionGrowth, "*EOS_IGNITION_GROWTH")

// ============================================================================
// EosMurnaghan
// ============================================================================

bool EosMurnaghan::parse(const std::vector<std::string>& lines,
                          util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: EOSID, K, N, REF
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.eosid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.k = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.n = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.ref = parser.getDoubleAt(pos, realW).value_or(0.0);

    return true;
}

std::vector<std::string> EosMurnaghan::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.eosid);
    writer.writeDouble(data_.k);
    writer.writeDouble(data_.n);
    writer.writeDouble(data_.ref);
    result.push_back(writer.getLine());

    return result;
}

void EosMurnaghan::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(EosMurnaghan, "*EOS_MURNAGHAN")

// ============================================================================
// EosTillotson
// ============================================================================

bool EosTillotson::parse(const std::vector<std::string>& lines,
                          util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: EOSID, A, B, ALPHA, BETA, E0, ES, ESS, RHO0
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.eosid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.a = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.b = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.alpha = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.beta = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.e0 = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.es = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.ess = parser.getDoubleAt(pos, realW).value_or(0.0);

    ++lineIdx;
    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 2: RHO0
    parser.setLine(lines[lineIdx]);
    pos = 0;

    data_.rho0 = parser.getDoubleAt(pos, realW).value_or(0.0);

    return true;
}

std::vector<std::string> EosTillotson::write(util::CardParser::Format format) const {
    std::vector<std::string> result;

    // Card 1
    {
        util::CardWriter writer(format);
        writer.writeInt(data_.eosid);
        writer.writeDouble(data_.a);
        writer.writeDouble(data_.b);
        writer.writeDouble(data_.alpha);
        writer.writeDouble(data_.beta);
        writer.writeDouble(data_.e0);
        writer.writeDouble(data_.es);
        writer.writeDouble(data_.ess);
        result.push_back(writer.getLine());
    }

    // Card 2
    {
        util::CardWriter writer(format);
        writer.writeDouble(data_.rho0);
        result.push_back(writer.getLine());
    }

    return result;
}

void EosTillotson::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(EosTillotson, "*EOS_TILLOTSON")

// ============================================================================
// EosTabulated
// ============================================================================

bool EosTabulated::parse(const std::vector<std::string>& lines,
                          util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;
        data_.eosid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.gama = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.e0 = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.v0 = parser.getDoubleAt(pos, realW).value_or(1.0);
        pos += realW;
        data_.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> EosTabulated::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.eosid);
    writer.writeDouble(data_.gama);
    writer.writeDouble(data_.e0);
    writer.writeDouble(data_.v0);
    writer.writeInt(data_.lcid);
    result.push_back(writer.getLine());

    return result;
}

void EosTabulated::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(EosTabulated, "*EOS_TABULATED")

// ============================================================================
// EosStiffGas
// ============================================================================

bool EosStiffGas::parse(const std::vector<std::string>& lines,
                         util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;
        data_.eosid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.gamma = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.e0 = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.v0 = parser.getDoubleAt(pos, realW).value_or(1.0);
        pos += realW;
        data_.pref = parser.getDoubleAt(pos, realW).value_or(0.0);
        break;
    }

    return true;
}

std::vector<std::string> EosStiffGas::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.eosid);
    writer.writeDouble(data_.gamma);
    writer.writeDouble(data_.e0);
    writer.writeDouble(data_.v0);
    writer.writeDouble(data_.pref);
    result.push_back(writer.getLine());

    return result;
}

void EosStiffGas::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(EosStiffGas, "*EOS_STIFF_GAS")

// ============================================================================
// EosSack
// ============================================================================

bool EosSack::parse(const std::vector<std::string>& lines,
                     util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;
        data_.eosid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.e0 = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.v0 = parser.getDoubleAt(pos, realW).value_or(1.0);
        pos += realW;
        data_.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> EosSack::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.eosid);
    writer.writeDouble(data_.e0);
    writer.writeDouble(data_.v0);
    writer.writeInt(data_.lcid);
    result.push_back(writer.getLine());

    return result;
}

void EosSack::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(EosSack, "*EOS_SACK")

// ============================================================================
// EosRatioOfPolynomials
// ============================================================================

bool EosRatioOfPolynomials::parse(const std::vector<std::string>& lines,
                                   util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;
        data_.eosid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.e0 = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.v0 = parser.getDoubleAt(pos, realW).value_or(1.0);
        pos += realW;
        data_.a0 = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.a1 = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.a2 = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.b0 = parser.getDoubleAt(pos, realW).value_or(1.0);
        pos += realW;
        data_.b1 = parser.getDoubleAt(pos, realW).value_or(0.0);
        break;
    }

    return true;
}

std::vector<std::string> EosRatioOfPolynomials::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.eosid);
    writer.writeDouble(data_.e0);
    writer.writeDouble(data_.v0);
    writer.writeDouble(data_.a0);
    writer.writeDouble(data_.a1);
    writer.writeDouble(data_.a2);
    writer.writeDouble(data_.b0);
    writer.writeDouble(data_.b1);
    result.push_back(writer.getLine());

    return result;
}

void EosRatioOfPolynomials::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(EosRatioOfPolynomials, "*EOS_RATIO_OF_POLYNOMIALS")

// ============================================================================
// EosOsborne
// ============================================================================

bool EosOsborne::parse(const std::vector<std::string>& lines,
                        util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;
        data_.eosid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.a1 = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.a2 = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.a3 = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.b1 = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.b2 = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.e0 = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.v0 = parser.getDoubleAt(pos, realW).value_or(1.0);
        break;
    }

    return true;
}

std::vector<std::string> EosOsborne::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.eosid);
    writer.writeDouble(data_.a1);
    writer.writeDouble(data_.a2);
    writer.writeDouble(data_.a3);
    writer.writeDouble(data_.b1);
    writer.writeDouble(data_.b2);
    writer.writeDouble(data_.e0);
    writer.writeDouble(data_.v0);
    result.push_back(writer.getLine());

    return result;
}

void EosOsborne::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(EosOsborne, "*EOS_OSBORNE")

// ============================================================================
// EosPropellantDeflagration
// ============================================================================

bool EosPropellantDeflagration::parse(const std::vector<std::string>& lines,
                                       util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;
        data_.eosid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.k = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.n = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.e0 = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.v0 = parser.getDoubleAt(pos, realW).value_or(1.0);
        pos += realW;
        data_.rho0 = parser.getDoubleAt(pos, realW).value_or(0.0);
        break;
    }

    return true;
}

std::vector<std::string> EosPropellantDeflagration::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.eosid);
    writer.writeDouble(data_.k);
    writer.writeDouble(data_.n);
    writer.writeDouble(data_.e0);
    writer.writeDouble(data_.v0);
    writer.writeDouble(data_.rho0);
    result.push_back(writer.getLine());

    return result;
}

void EosPropellantDeflagration::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(EosPropellantDeflagration, "*EOS_PROPELLANT_DEFLAGRATION")

// ============================================================================
// EosSesame
// ============================================================================

bool EosSesame::parse(const std::vector<std::string>& lines,
                       util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;
        data_.eosid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.matid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.e0 = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.v0 = parser.getDoubleAt(pos, realW).value_or(1.0);
        break;
    }

    return true;
}

std::vector<std::string> EosSesame::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.eosid);
    writer.writeInt(data_.matid);
    writer.writeDouble(data_.e0);
    writer.writeDouble(data_.v0);
    result.push_back(writer.getLine());

    return result;
}

void EosSesame::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(EosSesame, "*EOS_SESAME")

// ============================================================================
// EosGasket
// ============================================================================

bool EosGasket::parse(const std::vector<std::string>& lines,
                       util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;
        data_.eosid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.lcidl = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.lcidu = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.e0 = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.v0 = parser.getDoubleAt(pos, realW).value_or(1.0);
        break;
    }

    return true;
}

std::vector<std::string> EosGasket::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.eosid);
    writer.writeInt(data_.lcidl);
    writer.writeInt(data_.lcidu);
    writer.writeDouble(data_.e0);
    writer.writeDouble(data_.v0);
    result.push_back(writer.getLine());

    return result;
}

void EosGasket::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(EosGasket, "*EOS_GASKET")

// ============================================================================
// EosUserDefined
// ============================================================================

bool EosUserDefined::parse(const std::vector<std::string>& lines,
                            util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;
        data_.eosid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.nhv = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.p1 = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.p2 = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.p3 = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.p4 = parser.getDoubleAt(pos, realW).value_or(0.0);
        break;
    }

    return true;
}

std::vector<std::string> EosUserDefined::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.eosid);
    writer.writeInt(data_.nhv);
    writer.writeDouble(data_.p1);
    writer.writeDouble(data_.p2);
    writer.writeDouble(data_.p3);
    writer.writeDouble(data_.p4);
    result.push_back(writer.getLine());

    return result;
}

void EosUserDefined::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(EosUserDefined, "*EOS_USER_DEFINED")

// ============================================================================
// EosPowderBurn
// ============================================================================

bool EosPowderBurn::parse(const std::vector<std::string>& lines,
                           util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;
        data_.eosid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.a = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.b = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.rgas = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.e0 = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.v0 = parser.getDoubleAt(pos, realW).value_or(1.0);
        break;
    }

    return true;
}

std::vector<std::string> EosPowderBurn::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.eosid);
    writer.writeDouble(data_.a);
    writer.writeDouble(data_.b);
    writer.writeDouble(data_.rgas);
    writer.writeDouble(data_.e0);
    writer.writeDouble(data_.v0);
    result.push_back(writer.getLine());

    return result;
}

void EosPowderBurn::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(EosPowderBurn, "*EOS_POWDER_BURN")

// ============================================================================
// EosLinearPolynomialWithEnergyLeak
// ============================================================================

bool EosLinearPolynomialWithEnergyLeak::parse(const std::vector<std::string>& lines,
                                               util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;
        data_.eosid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.c0 = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.c1 = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.c2 = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.c3 = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.c4 = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.c5 = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.eleak = parser.getDoubleAt(pos, realW).value_or(0.0);
        break;
    }

    return true;
}

std::vector<std::string> EosLinearPolynomialWithEnergyLeak::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.eosid);
    writer.writeDouble(data_.c0);
    writer.writeDouble(data_.c1);
    writer.writeDouble(data_.c2);
    writer.writeDouble(data_.c3);
    writer.writeDouble(data_.c4);
    writer.writeDouble(data_.c5);
    writer.writeDouble(data_.eleak);
    result.push_back(writer.getLine());

    return result;
}

void EosLinearPolynomialWithEnergyLeak::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(EosLinearPolynomialWithEnergyLeak, "*EOS_LINEAR_POLYNOMIAL_WITH_ENERGY_LEAK")

// ============================================================================
// EosJwlb
// ============================================================================

bool EosJwlb::parse(const std::vector<std::string>& lines,
                     util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;
        data_.eosid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.a = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.b = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.r1 = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.r2 = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.omeg = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.e0 = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.v0 = parser.getDoubleAt(pos, realW).value_or(1.0);
        break;
    }

    return true;
}

std::vector<std::string> EosJwlb::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.eosid);
    writer.writeDouble(data_.a);
    writer.writeDouble(data_.b);
    writer.writeDouble(data_.r1);
    writer.writeDouble(data_.r2);
    writer.writeDouble(data_.omeg);
    writer.writeDouble(data_.e0);
    writer.writeDouble(data_.v0);
    result.push_back(writer.getLine());

    return result;
}

void EosJwlb::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(EosJwlb, "*EOS_JWLB")

} // namespace koo::dyna
