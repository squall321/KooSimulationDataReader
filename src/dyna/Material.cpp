#include <koo/dyna/Material.hpp>
#include <koo/dyna/ModelVisitor.hpp>
#include <koo/dyna/KeywordFactory.hpp>

namespace koo::dyna {

// MatElastic implementation
bool MatElastic::parse(const std::vector<std::string>& lines,
                       util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();    // Always 10
    const size_t realW = parser.getRealWidth();  // 10 (Standard) or 20 (Large)

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);

        // Card 1: MID(I), RO(E), E(E), PR(E), DA(E), DB(E), K(E)
        size_t pos = 0;
        data_.id = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data_.ro = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.e = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.pr = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.da = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.db = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.k = parser.getDoubleAt(pos, realW).value_or(0.0);

        break; // Only one card for MAT_ELASTIC
    }

    return true;
}

std::vector<std::string> MatElastic::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.id);
    writer.writeDouble(data_.ro);
    writer.writeDouble(data_.e);
    writer.writeDouble(data_.pr);
    writer.writeDouble(data_.da);
    writer.writeDouble(data_.db);
    writer.writeDouble(data_.k);

    result.push_back(writer.getLine());
    return result;
}

void MatElastic::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

// MatRigid implementation
bool MatRigid::parse(const std::vector<std::string>& lines,
                     util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();    // Always 10
    const size_t realW = parser.getRealWidth();  // 10 (Standard) or 20 (Large)

    size_t cardNum = 0;
    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);

        if (cardNum == 0) {
            // Card 1: MID(I), RO(E), E(E), PR(E)
            size_t pos = 0;
            data_.id = parser.getInt64At(pos).value_or(0);
            pos += intW;
            data_.ro = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.e = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.pr = parser.getDoubleAt(pos, realW).value_or(0.0);
        } else if (cardNum == 1) {
            // Card 2: CMO(I), CON1(I), CON2(I)
            size_t pos = 0;
            data_.cmo = static_cast<int>(parser.getInt64At(pos).value_or(0));
            pos += intW;
            data_.con1 = static_cast<int>(parser.getInt64At(pos).value_or(0));
            pos += intW;
            data_.con2 = static_cast<int>(parser.getInt64At(pos).value_or(0));
        }

        cardNum++;
        if (cardNum >= 2) break;
    }

    return true;
}

std::vector<std::string> MatRigid::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    // Card 1
    writer.writeInt(data_.id);
    writer.writeDouble(data_.ro);
    writer.writeDouble(data_.e);
    writer.writeDouble(data_.pr);
    result.push_back(writer.getLine());

    // Card 2
    writer.clear();
    writer.writeInt(data_.cmo);
    writer.writeInt(data_.con1);
    writer.writeInt(data_.con2);
    result.push_back(writer.getLine());

    return result;
}

void MatRigid::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

// ============================================================================
// MatPlasticKinematic
// ============================================================================

bool MatPlasticKinematic::parse(const std::vector<std::string>& lines,
                                 util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t cardNum = 0;
    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);

        if (cardNum == 0) {
            // Card 1: MID, RO, E, PR, SIGY, ETAN, BETA
            size_t pos = 0;
            data_.mid = parser.getInt64At(pos).value_or(0);
            pos += intW;
            data_.ro = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.e = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.pr = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.sigy = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.etan = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.beta = parser.getDoubleAt(pos, realW).value_or(0.0);
        } else if (cardNum == 1) {
            // Card 2: SRC, SRP, FS, VP
            size_t pos = 0;
            data_.src = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.srp = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.fs = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.vp = static_cast<int>(parser.getInt64At(pos).value_or(0));
        }

        cardNum++;
        if (cardNum >= 2) break;
    }

    return true;
}

std::vector<std::string> MatPlasticKinematic::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    // Card 1
    writer.writeInt(data_.mid);
    writer.writeDouble(data_.ro);
    writer.writeDouble(data_.e);
    writer.writeDouble(data_.pr);
    writer.writeDouble(data_.sigy);
    writer.writeDouble(data_.etan);
    writer.writeDouble(data_.beta);
    result.push_back(writer.getLine());

    // Card 2
    writer.clear();
    writer.writeDouble(data_.src);
    writer.writeDouble(data_.srp);
    writer.writeDouble(data_.fs);
    writer.writeInt(data_.vp);
    result.push_back(writer.getLine());

    return result;
}

void MatPlasticKinematic::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

// ============================================================================
// MatPiecewiseLinearPlasticity
// ============================================================================

bool MatPiecewiseLinearPlasticity::parse(const std::vector<std::string>& lines,
                                          util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t cardNum = 0;
    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);

        if (cardNum == 0) {
            // Card 1: MID, RO, E, PR, SIGY, ETAN, FAIL, TDEL
            size_t pos = 0;
            data_.mid = parser.getInt64At(pos).value_or(0);
            pos += intW;
            data_.ro = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.e = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.pr = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.sigy = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.etan = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.fail = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.tdel = parser.getDoubleAt(pos, realW).value_or(0.0);
        } else if (cardNum == 1) {
            // Card 2: C, P, LCSS, LCSR, VP
            size_t pos = 0;
            data_.c = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.p = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.lcss = static_cast<int>(parser.getInt64At(pos).value_or(0));
            pos += intW;
            data_.lcsr = static_cast<int>(parser.getInt64At(pos).value_or(0));
            pos += intW;
            data_.vp = parser.getDoubleAt(pos, realW).value_or(0.0);
        }

        cardNum++;
        if (cardNum >= 2) break;
    }

    return true;
}

std::vector<std::string> MatPiecewiseLinearPlasticity::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    // Card 1
    writer.writeInt(data_.mid);
    writer.writeDouble(data_.ro);
    writer.writeDouble(data_.e);
    writer.writeDouble(data_.pr);
    writer.writeDouble(data_.sigy);
    writer.writeDouble(data_.etan);
    writer.writeDouble(data_.fail);
    writer.writeDouble(data_.tdel);
    result.push_back(writer.getLine());

    // Card 2
    writer.clear();
    writer.writeDouble(data_.c);
    writer.writeDouble(data_.p);
    writer.writeInt(data_.lcss);
    writer.writeInt(data_.lcsr);
    writer.writeDouble(data_.vp);
    result.push_back(writer.getLine());

    return result;
}

void MatPiecewiseLinearPlasticity::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

// ============================================================================
// MatJohnsonCook
// ============================================================================

bool MatJohnsonCook::parse(const std::vector<std::string>& lines,
                            util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t cardNum = 0;
    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);

        if (cardNum == 0) {
            // Card 1: MID, RO, G, E, PR, DTF, VP, RATEOP
            size_t pos = 0;
            data_.mid = parser.getInt64At(pos).value_or(0);
            pos += intW;
            data_.ro = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.g = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.e = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.pr = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.dtf = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.vp = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.rateop = parser.getDoubleAt(pos, realW).value_or(0.0);
        } else if (cardNum == 1) {
            // Card 2: A, B, N, C, M, TM, TR, EPSO
            size_t pos = 0;
            data_.a = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.b = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.n = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.c = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.m = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.tm = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.tr = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.epso = parser.getDoubleAt(pos, realW).value_or(0.0);
        } else if (cardNum == 2) {
            // Card 3: CP, PC, SPALL, IT, D1, D2, D3, D4
            size_t pos = 0;
            data_.cp = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.pc = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.spall = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.it = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.d1 = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.d2 = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.d3 = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.d4 = parser.getDoubleAt(pos, realW).value_or(0.0);
        } else if (cardNum == 3) {
            // Card 4: D5
            data_.d5 = parser.getDoubleAt(0, realW).value_or(0.0);
        }

        cardNum++;
        if (cardNum >= 4) break;
    }

    return true;
}

std::vector<std::string> MatJohnsonCook::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    // Card 1
    writer.writeInt(data_.mid);
    writer.writeDouble(data_.ro);
    writer.writeDouble(data_.g);
    writer.writeDouble(data_.e);
    writer.writeDouble(data_.pr);
    writer.writeDouble(data_.dtf);
    writer.writeDouble(data_.vp);
    writer.writeDouble(data_.rateop);
    result.push_back(writer.getLine());

    // Card 2
    writer.clear();
    writer.writeDouble(data_.a);
    writer.writeDouble(data_.b);
    writer.writeDouble(data_.n);
    writer.writeDouble(data_.c);
    writer.writeDouble(data_.m);
    writer.writeDouble(data_.tm);
    writer.writeDouble(data_.tr);
    writer.writeDouble(data_.epso);
    result.push_back(writer.getLine());

    // Card 3
    writer.clear();
    writer.writeDouble(data_.cp);
    writer.writeDouble(data_.pc);
    writer.writeDouble(data_.spall);
    writer.writeDouble(data_.it);
    writer.writeDouble(data_.d1);
    writer.writeDouble(data_.d2);
    writer.writeDouble(data_.d3);
    writer.writeDouble(data_.d4);
    result.push_back(writer.getLine());

    // Card 4
    writer.clear();
    writer.writeDouble(data_.d5);
    result.push_back(writer.getLine());

    return result;
}

void MatJohnsonCook::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

// ============================================================================
// MatNull
// ============================================================================

bool MatNull::parse(const std::vector<std::string>& lines,
                     util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);

        // Card 1: MID, RO, PC, MU, TEROD, CEROD, YM, PR
        size_t pos = 0;
        data_.mid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data_.ro = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.pc = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.mu = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.terod = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.cerod = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.ym = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.prr = parser.getDoubleAt(pos, realW).value_or(0.0);

        break;
    }

    return true;
}

std::vector<std::string> MatNull::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.mid);
    writer.writeDouble(data_.ro);
    writer.writeDouble(data_.pc);
    writer.writeDouble(data_.mu);
    writer.writeDouble(data_.terod);
    writer.writeDouble(data_.cerod);
    writer.writeDouble(data_.ym);
    writer.writeDouble(data_.prr);

    result.push_back(writer.getLine());
    return result;
}

void MatNull::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

// ============================================================================
// MatViscoelastic
// ============================================================================

bool MatViscoelastic::parse(const std::vector<std::string>& lines,
                             util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);

        // Card 1: MID, RO, BULK, G0, GI, BETA
        size_t pos = 0;
        data_.mid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data_.ro = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.bulk = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.g0 = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.gi = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.beta = parser.getDoubleAt(pos, realW).value_or(0.0);

        break;
    }

    return true;
}

std::vector<std::string> MatViscoelastic::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.mid);
    writer.writeDouble(data_.ro);
    writer.writeDouble(data_.bulk);
    writer.writeDouble(data_.g0);
    writer.writeDouble(data_.gi);
    writer.writeDouble(data_.beta);

    result.push_back(writer.getLine());
    return result;
}

void MatViscoelastic::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

// ============================================================================
// MatPowerLawPlasticity
// ============================================================================

bool MatPowerLawPlasticity::parse(const std::vector<std::string>& lines,
                                   util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t cardNum = 0;
    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);

        if (cardNum == 0) {
            // Card 1: MID, RO, E, PR, K, N, SRC, SRP
            size_t pos = 0;
            data_.mid = parser.getInt64At(pos).value_or(0);
            pos += intW;
            data_.ro = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.e = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.pr = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.k = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.n = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.src = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.srp = parser.getDoubleAt(pos, realW).value_or(0.0);
        } else if (cardNum == 1) {
            // Card 2: SIGY, VP
            size_t pos = 0;
            data_.sigy = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.vp = parser.getDoubleAt(pos, realW).value_or(0.0);
        }

        cardNum++;
        if (cardNum >= 2) break;
    }

    return true;
}

std::vector<std::string> MatPowerLawPlasticity::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    // Card 1
    writer.writeInt(data_.mid);
    writer.writeDouble(data_.ro);
    writer.writeDouble(data_.e);
    writer.writeDouble(data_.pr);
    writer.writeDouble(data_.k);
    writer.writeDouble(data_.n);
    writer.writeDouble(data_.src);
    writer.writeDouble(data_.srp);
    result.push_back(writer.getLine());

    // Card 2
    writer.clear();
    writer.writeDouble(data_.sigy);
    writer.writeDouble(data_.vp);
    result.push_back(writer.getLine());

    return result;
}

void MatPowerLawPlasticity::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

// ============================================================================
// MatHoneycomb
// ============================================================================

bool MatHoneycomb::parse(const std::vector<std::string>& lines,
                          util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t cardNum = 0;
    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);

        if (cardNum == 0) {
            // Card 1: MID, RO, E, PR, SIGY, VF, MU, BULK
            size_t pos = 0;
            data_.mid = parser.getInt64At(pos).value_or(0);
            pos += intW;
            data_.ro = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.e = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.pr = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.sigy = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.vf = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.mu = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.bulk = parser.getDoubleAt(pos, realW).value_or(0.0);
        } else if (cardNum == 1) {
            // Card 2: LCA, LCB, LCC, LCS, LCAB, LCBC, LCCA, LCSR
            size_t pos = 0;
            data_.lca = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.lcb = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.lcc = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.lcs = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.lcab = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.lcbc = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.lcca = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.lcsr = parser.getDoubleAt(pos, realW).value_or(0.0);
        } else if (cardNum == 2) {
            // Card 3: EAAU, EBBU, ECCU, GABU, GBCU, GCAU, APTS, MACF
            size_t pos = 0;
            data_.eaau = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.ebbu = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.eccu = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.gabu = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.gbcu = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.gcau = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.apts = static_cast<int>(parser.getInt64At(pos).value_or(0));
            pos += intW;
            data_.macf = static_cast<int>(parser.getInt64At(pos).value_or(0));
        }

        cardNum++;
        if (cardNum >= 3) break;
    }

    return true;
}

std::vector<std::string> MatHoneycomb::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    // Card 1
    writer.writeInt(data_.mid);
    writer.writeDouble(data_.ro);
    writer.writeDouble(data_.e);
    writer.writeDouble(data_.pr);
    writer.writeDouble(data_.sigy);
    writer.writeDouble(data_.vf);
    writer.writeDouble(data_.mu);
    writer.writeDouble(data_.bulk);
    result.push_back(writer.getLine());

    // Card 2
    writer.clear();
    writer.writeDouble(data_.lca);
    writer.writeDouble(data_.lcb);
    writer.writeDouble(data_.lcc);
    writer.writeDouble(data_.lcs);
    writer.writeDouble(data_.lcab);
    writer.writeDouble(data_.lcbc);
    writer.writeDouble(data_.lcca);
    writer.writeDouble(data_.lcsr);
    result.push_back(writer.getLine());

    // Card 3
    writer.clear();
    writer.writeDouble(data_.eaau);
    writer.writeDouble(data_.ebbu);
    writer.writeDouble(data_.eccu);
    writer.writeDouble(data_.gabu);
    writer.writeDouble(data_.gbcu);
    writer.writeDouble(data_.gcau);
    writer.writeInt(data_.apts);
    writer.writeInt(data_.macf);
    result.push_back(writer.getLine());

    return result;
}

void MatHoneycomb::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

// ============================================================================
// MatModifiedPiecewiseLinearPlasticity
// ============================================================================

bool MatModifiedPiecewiseLinearPlasticity::parse(const std::vector<std::string>& lines,
                                                  util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t cardNum = 0;
    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);

        if (cardNum == 0) {
            // Card 1: MID, RO, E, PR, SIGY, ETAN, FAIL, TDEL
            size_t pos = 0;
            data_.mid = parser.getInt64At(pos).value_or(0);
            pos += intW;
            data_.ro = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.e = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.pr = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.sigy = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.etan = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.fail = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.tdel = parser.getDoubleAt(pos, realW).value_or(0.0);
        } else if (cardNum == 1) {
            // Card 2: C, P, LCSS, LCSR, VP, EPSTHIN, EPSMAJ, NUMINT
            size_t pos = 0;
            data_.c = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.p = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.lcss = static_cast<int>(parser.getInt64At(pos).value_or(0));
            pos += intW;
            data_.lcsr = static_cast<int>(parser.getInt64At(pos).value_or(0));
            pos += intW;
            data_.vp = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.epsthin = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.epsmaj = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.numint = parser.getDoubleAt(pos, realW).value_or(0.0);
        } else if (cardNum == 2) {
            // Card 3: EPSC, EPSR, EPSAB, LCAB
            size_t pos = 0;
            data_.epsc = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.epsr = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.epsab = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.lcab = parser.getDoubleAt(pos, realW).value_or(0.0);
        }

        cardNum++;
        if (cardNum >= 3) break;
    }

    return true;
}

std::vector<std::string> MatModifiedPiecewiseLinearPlasticity::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    // Card 1
    writer.writeInt(data_.mid);
    writer.writeDouble(data_.ro);
    writer.writeDouble(data_.e);
    writer.writeDouble(data_.pr);
    writer.writeDouble(data_.sigy);
    writer.writeDouble(data_.etan);
    writer.writeDouble(data_.fail);
    writer.writeDouble(data_.tdel);
    result.push_back(writer.getLine());

    // Card 2
    writer.clear();
    writer.writeDouble(data_.c);
    writer.writeDouble(data_.p);
    writer.writeInt(data_.lcss);
    writer.writeInt(data_.lcsr);
    writer.writeDouble(data_.vp);
    writer.writeDouble(data_.epsthin);
    writer.writeDouble(data_.epsmaj);
    writer.writeDouble(data_.numint);
    result.push_back(writer.getLine());

    // Card 3
    writer.clear();
    writer.writeDouble(data_.epsc);
    writer.writeDouble(data_.epsr);
    writer.writeDouble(data_.epsab);
    writer.writeDouble(data_.lcab);
    result.push_back(writer.getLine());

    return result;
}

void MatModifiedPiecewiseLinearPlasticity::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

// ============================================================================
// MatCrushableFoam
// ============================================================================

bool MatCrushableFoam::parse(const std::vector<std::string>& lines,
                              util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);

        // Card 1: MID, RO, E, PR, LCID, TSC, DAMP, NCYCLE, MAXEPS
        size_t pos = 0;
        data_.mid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data_.ro = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.e = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.pr = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.lcid = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.tsc = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.damp = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.ncycle = parser.getDoubleAt(pos, realW).value_or(0.0);

        break;
    }

    return true;
}

std::vector<std::string> MatCrushableFoam::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.mid);
    writer.writeDouble(data_.ro);
    writer.writeDouble(data_.e);
    writer.writeDouble(data_.pr);
    writer.writeDouble(data_.lcid);
    writer.writeDouble(data_.tsc);
    writer.writeDouble(data_.damp);
    writer.writeDouble(data_.ncycle);

    result.push_back(writer.getLine());
    return result;
}

void MatCrushableFoam::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

// ============================================================================
// MatSpotWeld
// ============================================================================

bool MatSpotWeld::parse(const std::vector<std::string>& lines,
                         util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t cardNum = 0;
    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);

        if (cardNum == 0) {
            // Card 1: MID, RO, E, PR, SIGY, EH, DT, TFAIL
            size_t pos = 0;
            data_.mid = parser.getInt64At(pos).value_or(0);
            pos += intW;
            data_.ro = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.e = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.pr = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.sigy = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.eh = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.dt = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.tfail = static_cast<int>(parser.getInt64At(pos).value_or(0));
        } else if (cardNum == 1) {
            // Card 2: EFAIL, NF, RS, DTF, TF
            size_t pos = 0;
            data_.efail = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.nf = static_cast<int>(parser.getInt64At(pos).value_or(0));
            pos += intW;
            data_.rs = static_cast<int>(parser.getInt64At(pos).value_or(0));
            pos += intW;
            data_.dtf = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.tf = parser.getDoubleAt(pos, realW).value_or(0.0);
        }

        cardNum++;
        if (cardNum >= 2) break;
    }

    return true;
}

std::vector<std::string> MatSpotWeld::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    // Card 1
    writer.writeInt(data_.mid);
    writer.writeDouble(data_.ro);
    writer.writeDouble(data_.e);
    writer.writeDouble(data_.pr);
    writer.writeDouble(data_.sigy);
    writer.writeDouble(data_.eh);
    writer.writeDouble(data_.dt);
    writer.writeInt(data_.tfail);
    result.push_back(writer.getLine());

    // Card 2
    writer.clear();
    writer.writeDouble(data_.efail);
    writer.writeInt(data_.nf);
    writer.writeInt(data_.rs);
    writer.writeDouble(data_.dtf);
    writer.writeDouble(data_.tf);
    result.push_back(writer.getLine());

    return result;
}

void MatSpotWeld::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

// ============================================================================
// MatOgdenRubber
// ============================================================================

bool MatOgdenRubber::parse(const std::vector<std::string>& lines,
                            util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t cardNum = 0;
    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);

        if (cardNum == 0) {
            // Card 1: MID, RO, PR, N, NV, G, SIGF, REF
            size_t pos = 0;
            data_.mid = parser.getInt64At(pos).value_or(0);
            pos += intW;
            data_.ro = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.pr = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.n = static_cast<int>(parser.getInt64At(pos).value_or(0));
            pos += intW;
            data_.nv = static_cast<int>(parser.getInt64At(pos).value_or(0));
            pos += intW;
            data_.g = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.sigf = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.ref = parser.getDoubleAt(pos, realW).value_or(0.0);
        } else if (cardNum == 1) {
            // Card 2: MU1, MU2, MU3, ALPHA1, ALPHA2, ALPHA3
            size_t pos = 0;
            data_.mu1 = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.mu2 = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.mu3 = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.alpha1 = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.alpha2 = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.alpha3 = parser.getDoubleAt(pos, realW).value_or(0.0);
        }

        cardNum++;
        if (cardNum >= 2) break;
    }

    return true;
}

std::vector<std::string> MatOgdenRubber::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    // Card 1
    writer.writeInt(data_.mid);
    writer.writeDouble(data_.ro);
    writer.writeDouble(data_.pr);
    writer.writeInt(data_.n);
    writer.writeInt(data_.nv);
    writer.writeDouble(data_.g);
    writer.writeDouble(data_.sigf);
    writer.writeDouble(data_.ref);
    result.push_back(writer.getLine());

    // Card 2
    writer.clear();
    writer.writeDouble(data_.mu1);
    writer.writeDouble(data_.mu2);
    writer.writeDouble(data_.mu3);
    writer.writeDouble(data_.alpha1);
    writer.writeDouble(data_.alpha2);
    writer.writeDouble(data_.alpha3);
    result.push_back(writer.getLine());

    return result;
}

void MatOgdenRubber::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

// ============================================================================
// MatFabric
// ============================================================================

bool MatFabric::parse(const std::vector<std::string>& lines,
                       util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t cardNum = 0;
    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);

        if (cardNum == 0) {
            // Card 1: MID, RO, EA, EB, EC, PRBA, PRCA, PRCB
            size_t pos = 0;
            data_.mid = parser.getInt64At(pos).value_or(0);
            pos += intW;
            data_.ro = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.ea = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.eb = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.ec = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.prba = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.prca = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.prcb = parser.getDoubleAt(pos, realW).value_or(0.0);
        } else if (cardNum == 1) {
            // Card 2: GAB, GBC, GCA, CSE, EL, PRL, LRATIO, DTEFAIL
            size_t pos = 0;
            data_.gab = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.gbc = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.gca = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.cse = static_cast<int>(parser.getInt64At(pos).value_or(0));
            pos += intW;
            data_.el = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.prl = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.lratio = static_cast<int>(parser.getInt64At(pos).value_or(0));
            pos += intW;
            data_.dtefail = parser.getDoubleAt(pos, realW).value_or(0.0);
        } else if (cardNum == 2) {
            // Card 3: APTS
            data_.apts = static_cast<int>(parser.getInt64At(0).value_or(0));
        }

        cardNum++;
        if (cardNum >= 3) break;
    }

    return true;
}

std::vector<std::string> MatFabric::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    // Card 1
    writer.writeInt(data_.mid);
    writer.writeDouble(data_.ro);
    writer.writeDouble(data_.ea);
    writer.writeDouble(data_.eb);
    writer.writeDouble(data_.ec);
    writer.writeDouble(data_.prba);
    writer.writeDouble(data_.prca);
    writer.writeDouble(data_.prcb);
    result.push_back(writer.getLine());

    // Card 2
    writer.clear();
    writer.writeDouble(data_.gab);
    writer.writeDouble(data_.gbc);
    writer.writeDouble(data_.gca);
    writer.writeInt(data_.cse);
    writer.writeDouble(data_.el);
    writer.writeDouble(data_.prl);
    writer.writeInt(data_.lratio);
    writer.writeDouble(data_.dtefail);
    result.push_back(writer.getLine());

    // Card 3
    writer.clear();
    writer.writeInt(data_.apts);
    result.push_back(writer.getLine());

    return result;
}

void MatFabric::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

// ============================================================================
// MatMooneyRivlinRubber
// ============================================================================

bool MatMooneyRivlinRubber::parse(const std::vector<std::string>& lines,
                                   util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: MID, RO, PR, A, B, REF, SGL, SW, ST
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.mid = static_cast<MaterialId>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.ro = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.pr = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.a = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.b = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.ref = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.sgl = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.sw = parser.getDoubleAt(pos, realW).value_or(0.0);

    ++lineIdx;
    if (lineIdx < lines.size() && !util::CardParser::isCommentLine(lines[lineIdx])) {
        parser.setLine(lines[lineIdx]);
        data_.st = parser.getDoubleAt(0, realW).value_or(0.0);
    }

    return true;
}

std::vector<std::string> MatMooneyRivlinRubber::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    // Card 1
    writer.writeInt(data_.mid);
    writer.writeDouble(data_.ro);
    writer.writeDouble(data_.pr);
    writer.writeDouble(data_.a);
    writer.writeDouble(data_.b);
    writer.writeInt(data_.ref);
    writer.writeDouble(data_.sgl);
    writer.writeDouble(data_.sw);
    result.push_back(writer.getLine());

    // Card 2
    writer.clear();
    writer.writeDouble(data_.st);
    result.push_back(writer.getLine());

    return result;
}

void MatMooneyRivlinRubber::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

// ============================================================================
// MatLowDensityFoam
// ============================================================================

bool MatLowDensityFoam::parse(const std::vector<std::string>& lines,
                               util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: MID, RO, E, LCID, TC, HU, BETA, DAMP
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.mid = static_cast<MaterialId>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.ro = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.e = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.tc = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.hu = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.beta = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.damp = parser.getDoubleAt(pos, realW).value_or(0.0);

    ++lineIdx;
    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 2: SHAPE, FAIL, KCON, REF
    parser.setLine(lines[lineIdx]);
    pos = 0;
    data_.shape = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.fail = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.kcon = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.ref = parser.getDoubleAt(pos, realW).value_or(0.0);

    return true;
}

std::vector<std::string> MatLowDensityFoam::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    // Card 1
    writer.writeInt(data_.mid);
    writer.writeDouble(data_.ro);
    writer.writeDouble(data_.e);
    writer.writeInt(data_.lcid);
    writer.writeDouble(data_.tc);
    writer.writeDouble(data_.hu);
    writer.writeDouble(data_.beta);
    writer.writeDouble(data_.damp);
    result.push_back(writer.getLine());

    // Card 2
    writer.clear();
    writer.writeDouble(data_.shape);
    writer.writeDouble(data_.fail);
    writer.writeInt(data_.kcon);
    writer.writeDouble(data_.ref);
    result.push_back(writer.getLine());

    return result;
}

void MatLowDensityFoam::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

// ============================================================================
// MatOrthotropicElastic
// ============================================================================

bool MatOrthotropicElastic::parse(const std::vector<std::string>& lines,
                                   util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t cardNum = 0;
    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);

        if (cardNum == 0) {
            // Card 1: MID, RO, EA, EB, (EC), PRBA, (PRCA), (PRCB)
            size_t pos = 0;
            data_.mid = parser.getInt64At(pos).value_or(0);
            pos += intW;
            data_.ro = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.ea = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.eb = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.ec = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.prba = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.prca = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.prcb = parser.getDoubleAt(pos, realW).value_or(0.0);
        } else if (cardNum == 1) {
            // Card 2: GAB, GBC, GCA, AOPT
            size_t pos = 0;
            data_.gab = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.gbc = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.gca = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.aopt = static_cast<int>(parser.getInt64At(pos).value_or(0));
        } else if (cardNum == 2) {
            // Card 3: XP, YP, ZP, A1, A2, A3
            size_t pos = 0;
            data_.xp = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.yp = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.zp = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.a1 = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.a2 = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.a3 = parser.getDoubleAt(pos, realW).value_or(0.0);
        } else if (cardNum == 3) {
            // Card 4: D1, D2, D3
            size_t pos = 0;
            data_.d1 = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.d2 = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.d3 = parser.getDoubleAt(pos, realW).value_or(0.0);
        }

        cardNum++;
        if (cardNum >= 4) break;
    }

    return true;
}

std::vector<std::string> MatOrthotropicElastic::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    // Card 1
    writer.writeInt(data_.mid);
    writer.writeDouble(data_.ro);
    writer.writeDouble(data_.ea);
    writer.writeDouble(data_.eb);
    writer.writeDouble(data_.ec);
    writer.writeDouble(data_.prba);
    writer.writeDouble(data_.prca);
    writer.writeDouble(data_.prcb);
    result.push_back(writer.getLine());

    // Card 2
    writer.clear();
    writer.writeDouble(data_.gab);
    writer.writeDouble(data_.gbc);
    writer.writeDouble(data_.gca);
    writer.writeInt(data_.aopt);
    result.push_back(writer.getLine());

    // Card 3 (if AOPT requires it)
    writer.clear();
    writer.writeDouble(data_.xp);
    writer.writeDouble(data_.yp);
    writer.writeDouble(data_.zp);
    writer.writeDouble(data_.a1);
    writer.writeDouble(data_.a2);
    writer.writeDouble(data_.a3);
    result.push_back(writer.getLine());

    // Card 4
    writer.clear();
    writer.writeDouble(data_.d1);
    writer.writeDouble(data_.d2);
    writer.writeDouble(data_.d3);
    result.push_back(writer.getLine());

    return result;
}

void MatOrthotropicElastic::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

// ============================================================================
// MatEnhancedCompositeDamage
// ============================================================================

bool MatEnhancedCompositeDamage::parse(const std::vector<std::string>& lines,
                                        util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t cardNum = 0;
    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);

        if (cardNum == 0) {
            // Card 1: MID, RO, EA, EB, (EC), PRBA, (PRCA), (PRCB)
            size_t pos = 0;
            data_.mid = parser.getInt64At(pos).value_or(0);
            pos += intW;
            data_.ro = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.ea = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.eb = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.ec = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.prba = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.prca = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.prcb = parser.getDoubleAt(pos, realW).value_or(0.0);
        } else if (cardNum == 1) {
            // Card 2: GAB, GBC, GCA, (KF), AOPT
            size_t pos = 0;
            data_.gab = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.gbc = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.gca = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.kf = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.aopt = static_cast<int>(parser.getInt64At(pos).value_or(0));
        } else if (cardNum == 2) {
            // Card 3: XC, XT, YC, YT, SC, CRIT, BETA, PEL
            size_t pos = 0;
            data_.xc = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.xt = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.yc = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.yt = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.sc = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.crit = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.beta = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.pel = parser.getDoubleAt(pos, realW).value_or(0.0);
        } else if (cardNum == 3) {
            // Card 4: EPSF, EPSR, TSMD, SOFT
            size_t pos = 0;
            data_.epsf = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.epsr = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.tsmd = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.soft = parser.getDoubleAt(pos, realW).value_or(0.0);
        } else if (cardNum == 4) {
            // Card 5 (AOPT dependent): A1, A2, A3, D1, D2, D3
            size_t pos = 0;
            data_.a1 = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.a2 = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.a3 = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.d1 = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.d2 = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.d3 = parser.getDoubleAt(pos, realW).value_or(0.0);
        } else if (cardNum == 5) {
            // Card 6: DFAILM, DFAILS, DFAILT, DFAILC, EFS, ALPH, SOFT2, TFAIL
            size_t pos = 0;
            data_.dfailm = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.dfails = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.dfailt = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.dfailc = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.efs = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.alph = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.soft2 = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.tfail = static_cast<int>(parser.getInt64At(pos).value_or(0));
        } else if (cardNum == 6) {
            // Card 7: FBRT, YCFAC, SLIMT1, SLIMC1, SLIMT2, SLIMC2, SLIMS, NCYRED
            size_t pos = 0;
            data_.fbrt = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.ycfac = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.slimt1 = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.slimc1 = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.slimt2 = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.slimc2 = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.slims = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.ncyred = parser.getDoubleAt(pos, realW).value_or(0.0);
        }

        cardNum++;
        if (cardNum >= 7) break;
    }

    return true;
}

std::vector<std::string> MatEnhancedCompositeDamage::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    // Card 1
    writer.writeInt(data_.mid);
    writer.writeDouble(data_.ro);
    writer.writeDouble(data_.ea);
    writer.writeDouble(data_.eb);
    writer.writeDouble(data_.ec);
    writer.writeDouble(data_.prba);
    writer.writeDouble(data_.prca);
    writer.writeDouble(data_.prcb);
    result.push_back(writer.getLine());

    // Card 2
    writer.clear();
    writer.writeDouble(data_.gab);
    writer.writeDouble(data_.gbc);
    writer.writeDouble(data_.gca);
    writer.writeDouble(data_.kf);
    writer.writeInt(data_.aopt);
    result.push_back(writer.getLine());

    // Card 3
    writer.clear();
    writer.writeDouble(data_.xc);
    writer.writeDouble(data_.xt);
    writer.writeDouble(data_.yc);
    writer.writeDouble(data_.yt);
    writer.writeDouble(data_.sc);
    writer.writeDouble(data_.crit);
    writer.writeDouble(data_.beta);
    writer.writeDouble(data_.pel);
    result.push_back(writer.getLine());

    // Card 4
    writer.clear();
    writer.writeDouble(data_.epsf);
    writer.writeDouble(data_.epsr);
    writer.writeDouble(data_.tsmd);
    writer.writeDouble(data_.soft);
    result.push_back(writer.getLine());

    // Card 5
    writer.clear();
    writer.writeDouble(data_.a1);
    writer.writeDouble(data_.a2);
    writer.writeDouble(data_.a3);
    writer.writeDouble(data_.d1);
    writer.writeDouble(data_.d2);
    writer.writeDouble(data_.d3);
    result.push_back(writer.getLine());

    // Card 6
    writer.clear();
    writer.writeDouble(data_.dfailm);
    writer.writeDouble(data_.dfails);
    writer.writeDouble(data_.dfailt);
    writer.writeDouble(data_.dfailc);
    writer.writeDouble(data_.efs);
    writer.writeDouble(data_.alph);
    writer.writeDouble(data_.soft2);
    writer.writeInt(data_.tfail);
    result.push_back(writer.getLine());

    // Card 7
    writer.clear();
    writer.writeDouble(data_.fbrt);
    writer.writeDouble(data_.ycfac);
    writer.writeDouble(data_.slimt1);
    writer.writeDouble(data_.slimc1);
    writer.writeDouble(data_.slimt2);
    writer.writeDouble(data_.slimc2);
    writer.writeDouble(data_.slims);
    writer.writeDouble(data_.ncyred);
    result.push_back(writer.getLine());

    return result;
}

void MatEnhancedCompositeDamage::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

// ============================================================================
// MatLaminatedCompositeFabric
// ============================================================================

bool MatLaminatedCompositeFabric::parse(const std::vector<std::string>& lines,
                                         util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t cardNum = 0;
    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);

        if (cardNum == 0) {
            // Card 1: MID, RO, EA, EB, (EC), PRBA, (PRCA), (PRCB)
            size_t pos = 0;
            data_.mid = parser.getInt64At(pos).value_or(0);
            pos += intW;
            data_.ro = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.ea = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.eb = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.ec = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.prba = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.prca = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.prcb = parser.getDoubleAt(pos, realW).value_or(0.0);
        } else if (cardNum == 1) {
            // Card 2: GAB, GBC, GCA, AOPT, TSIZE, ERODS, SOFT, FS
            size_t pos = 0;
            data_.gab = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.gbc = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.gca = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.aopt = static_cast<int>(parser.getInt64At(pos).value_or(0));
            pos += intW;
            data_.tsize = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.erods = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.soft = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.fs = static_cast<int>(parser.getInt64At(pos).value_or(0));
        } else if (cardNum == 2) {
            // Card 3 (AOPT dependent): XP, YP, ZP, A1, A2, A3
            size_t pos = 0;
            data_.xp = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.yp = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.zp = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.a1 = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.a2 = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.a3 = parser.getDoubleAt(pos, realW).value_or(0.0);
        } else if (cardNum == 3) {
            // Card 4: D1, D2, D3
            size_t pos = 0;
            data_.d1 = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.d2 = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.d3 = parser.getDoubleAt(pos, realW).value_or(0.0);
        } else if (cardNum == 4) {
            // Card 5: XT, XC, YT, YC, SC
            size_t pos = 0;
            data_.xt = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.xc = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.yt = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.yc = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.sc = parser.getDoubleAt(pos, realW).value_or(0.0);
        } else if (cardNum == 5) {
            // Card 6: E11C, E11T, E22C, E22T, GMS
            size_t pos = 0;
            data_.e11c = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.e11t = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.e22c = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.e22t = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.gms = parser.getDoubleAt(pos, realW).value_or(0.0);
        } else if (cardNum == 6) {
            // Card 7: SLIMT1, SLIMC1, SLIMT2, SLIMC2, SLIMS, LCXC, LCXT, LCYC
            size_t pos = 0;
            data_.slimt1 = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.slimc1 = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.slimt2 = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.slimc2 = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.slims = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.lcxc = static_cast<int>(parser.getInt64At(pos).value_or(0));
            pos += intW;
            data_.lcxt = static_cast<int>(parser.getInt64At(pos).value_or(0));
            pos += intW;
            data_.lcyc = static_cast<int>(parser.getInt64At(pos).value_or(0));
        } else if (cardNum == 7) {
            // Card 8: LCYT, LCSC, DT
            size_t pos = 0;
            data_.lcyt = static_cast<int>(parser.getInt64At(pos).value_or(0));
            pos += intW;
            data_.lcsc = static_cast<int>(parser.getInt64At(pos).value_or(0));
            pos += intW;
            data_.dt = parser.getDoubleAt(pos, realW).value_or(0.0);
        }

        cardNum++;
        if (cardNum >= 8) break;
    }

    return true;
}

std::vector<std::string> MatLaminatedCompositeFabric::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    // Card 1
    writer.writeInt(data_.mid);
    writer.writeDouble(data_.ro);
    writer.writeDouble(data_.ea);
    writer.writeDouble(data_.eb);
    writer.writeDouble(data_.ec);
    writer.writeDouble(data_.prba);
    writer.writeDouble(data_.prca);
    writer.writeDouble(data_.prcb);
    result.push_back(writer.getLine());

    // Card 2
    writer.clear();
    writer.writeDouble(data_.gab);
    writer.writeDouble(data_.gbc);
    writer.writeDouble(data_.gca);
    writer.writeInt(data_.aopt);
    writer.writeDouble(data_.tsize);
    writer.writeDouble(data_.erods);
    writer.writeDouble(data_.soft);
    writer.writeInt(data_.fs);
    result.push_back(writer.getLine());

    // Card 3
    writer.clear();
    writer.writeDouble(data_.xp);
    writer.writeDouble(data_.yp);
    writer.writeDouble(data_.zp);
    writer.writeDouble(data_.a1);
    writer.writeDouble(data_.a2);
    writer.writeDouble(data_.a3);
    result.push_back(writer.getLine());

    // Card 4
    writer.clear();
    writer.writeDouble(data_.d1);
    writer.writeDouble(data_.d2);
    writer.writeDouble(data_.d3);
    result.push_back(writer.getLine());

    // Card 5
    writer.clear();
    writer.writeDouble(data_.xt);
    writer.writeDouble(data_.xc);
    writer.writeDouble(data_.yt);
    writer.writeDouble(data_.yc);
    writer.writeDouble(data_.sc);
    result.push_back(writer.getLine());

    // Card 6
    writer.clear();
    writer.writeDouble(data_.e11c);
    writer.writeDouble(data_.e11t);
    writer.writeDouble(data_.e22c);
    writer.writeDouble(data_.e22t);
    writer.writeDouble(data_.gms);
    result.push_back(writer.getLine());

    // Card 7
    writer.clear();
    writer.writeDouble(data_.slimt1);
    writer.writeDouble(data_.slimc1);
    writer.writeDouble(data_.slimt2);
    writer.writeDouble(data_.slimc2);
    writer.writeDouble(data_.slims);
    writer.writeInt(data_.lcxc);
    writer.writeInt(data_.lcxt);
    writer.writeInt(data_.lcyc);
    result.push_back(writer.getLine());

    // Card 8
    writer.clear();
    writer.writeInt(data_.lcyt);
    writer.writeInt(data_.lcsc);
    writer.writeDouble(data_.dt);
    result.push_back(writer.getLine());

    return result;
}

void MatLaminatedCompositeFabric::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

// ============================================================================
// MatElasticPlasticThermal
// ============================================================================

bool MatElasticPlasticThermal::parse(const std::vector<std::string>& lines,
                                      util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t cardNum = 0;
    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);

        if (cardNum == 0) {
            // Card 1: MID, RO, E, PR, SIGY, ETAN, ALPHA
            size_t pos = 0;
            data_.mid = parser.getInt64At(pos).value_or(0);
            pos += intW;
            data_.ro = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.e = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.pr = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.sigy = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.etan = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.alpha = parser.getDoubleAt(pos, realW).value_or(0.0);
        } else if (cardNum == 1) {
            // Card 2: LCSS, LCTH, TH0
            size_t pos = 0;
            data_.lcss = static_cast<int>(parser.getInt64At(pos).value_or(0));
            pos += intW;
            data_.lcth = static_cast<int>(parser.getInt64At(pos).value_or(0));
            pos += intW;
            data_.th0 = parser.getDoubleAt(pos, realW).value_or(0.0);
        }

        cardNum++;
        if (cardNum >= 2) break;
    }

    return true;
}

std::vector<std::string> MatElasticPlasticThermal::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    // Card 1
    writer.writeInt(data_.mid);
    writer.writeDouble(data_.ro);
    writer.writeDouble(data_.e);
    writer.writeDouble(data_.pr);
    writer.writeDouble(data_.sigy);
    writer.writeDouble(data_.etan);
    writer.writeDouble(data_.alpha);
    result.push_back(writer.getLine());

    // Card 2
    writer.clear();
    writer.writeInt(data_.lcss);
    writer.writeInt(data_.lcth);
    writer.writeDouble(data_.th0);
    result.push_back(writer.getLine());

    return result;
}

void MatElasticPlasticThermal::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

// ============================================================================
// MatSoilAndFoam
// ============================================================================

bool MatSoilAndFoam::parse(const std::vector<std::string>& lines,
                            util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t cardNum = 0;
    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);

        if (cardNum == 0) {
            // Card 1: MID, RO, G, KU, A0, A1, A2, PC
            size_t pos = 0;
            data_.mid = parser.getInt64At(pos).value_or(0);
            pos += intW;
            data_.ro = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.g = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.ku = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.a0 = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.a1 = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.a2 = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.pc = parser.getDoubleAt(pos, realW).value_or(0.0);
        } else if (cardNum == 1) {
            // Card 2: VCR, REF, LCID
            size_t pos = 0;
            data_.vcr = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.ref = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        } else {
            // Optional volumetric strain vs pressure data
            size_t pos = 0;
            double eps = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            double p = parser.getDoubleAt(pos, realW).value_or(0.0);
            if (eps != 0.0 || p != 0.0) {
                data_.eps_p.push_back({eps, p});
            }
        }

        cardNum++;
    }

    return true;
}

std::vector<std::string> MatSoilAndFoam::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    // Card 1
    writer.writeInt(data_.mid);
    writer.writeDouble(data_.ro);
    writer.writeDouble(data_.g);
    writer.writeDouble(data_.ku);
    writer.writeDouble(data_.a0);
    writer.writeDouble(data_.a1);
    writer.writeDouble(data_.a2);
    writer.writeDouble(data_.pc);
    result.push_back(writer.getLine());

    // Card 2
    writer.clear();
    writer.writeDouble(data_.vcr);
    writer.writeDouble(data_.ref);
    writer.writeInt(data_.lcid);
    result.push_back(writer.getLine());

    // Optional eps-p pairs
    for (const auto& pair : data_.eps_p) {
        writer.clear();
        writer.writeDouble(pair.first);
        writer.writeDouble(pair.second);
        result.push_back(writer.getLine());
    }

    return result;
}

void MatSoilAndFoam::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

// ============================================================================
// MatElasticPlasticHydro
// ============================================================================

bool MatElasticPlasticHydro::parse(const std::vector<std::string>& lines,
                                    util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);

        // Card 1: MID, RO, G, SIGY, EH, PC, FS, EOSID
        size_t pos = 0;
        data_.mid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data_.ro = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.g = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.sigy = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.eh = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.pc = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.fs = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.eosid = static_cast<int>(parser.getInt64At(pos).value_or(0));

        break;
    }

    return true;
}

std::vector<std::string> MatElasticPlasticHydro::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.mid);
    writer.writeDouble(data_.ro);
    writer.writeDouble(data_.g);
    writer.writeDouble(data_.sigy);
    writer.writeDouble(data_.eh);
    writer.writeDouble(data_.pc);
    writer.writeDouble(data_.fs);
    writer.writeInt(data_.eosid);
    result.push_back(writer.getLine());

    return result;
}

void MatElasticPlasticHydro::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

// ============================================================================
// MatCompositeDamage
// ============================================================================

bool MatCompositeDamage::parse(const std::vector<std::string>& lines,
                                util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t cardNum = 0;
    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);

        if (cardNum == 0) {
            // Card 1: MID, RO, EA, EB, (EC), PRBA, (PRCA), (PRCB)
            size_t pos = 0;
            data_.mid = parser.getInt64At(pos).value_or(0);
            pos += intW;
            data_.ro = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.ea = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.eb = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.ec = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.prba = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.prca = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.prcb = parser.getDoubleAt(pos, realW).value_or(0.0);
        } else if (cardNum == 1) {
            // Card 2: GAB, GBC, GCA, AOPT, SC
            size_t pos = 0;
            data_.gab = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.gbc = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.gca = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.aopt = static_cast<int>(parser.getInt64At(pos).value_or(0));
            pos += intW;
            data_.sc = parser.getDoubleAt(pos, realW).value_or(0.0);
        } else if (cardNum == 2) {
            // Card 3: XT, YT, YC, ALPH, SN, SYZ, SZX, BETA
            size_t pos = 0;
            data_.xt = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.yt = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.yc = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.alph = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.sn = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.syz = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.szx = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.beta = parser.getDoubleAt(pos, realW).value_or(0.0);
        } else if (cardNum == 3) {
            // Card 4 (AOPT dependent): A1, A2, A3
            size_t pos = 0;
            data_.a1 = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.a2 = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.a3 = parser.getDoubleAt(pos, realW).value_or(0.0);
        } else if (cardNum == 4) {
            // Card 5 (AOPT dependent): D1, D2, D3
            size_t pos = 0;
            data_.d1 = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.d2 = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.d3 = parser.getDoubleAt(pos, realW).value_or(0.0);
        }

        cardNum++;
        if (cardNum >= 5) break;
    }

    return true;
}

std::vector<std::string> MatCompositeDamage::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    // Card 1
    writer.writeInt(data_.mid);
    writer.writeDouble(data_.ro);
    writer.writeDouble(data_.ea);
    writer.writeDouble(data_.eb);
    writer.writeDouble(data_.ec);
    writer.writeDouble(data_.prba);
    writer.writeDouble(data_.prca);
    writer.writeDouble(data_.prcb);
    result.push_back(writer.getLine());

    // Card 2
    writer.clear();
    writer.writeDouble(data_.gab);
    writer.writeDouble(data_.gbc);
    writer.writeDouble(data_.gca);
    writer.writeInt(data_.aopt);
    writer.writeDouble(data_.sc);
    result.push_back(writer.getLine());

    // Card 3
    writer.clear();
    writer.writeDouble(data_.xt);
    writer.writeDouble(data_.yt);
    writer.writeDouble(data_.yc);
    writer.writeDouble(data_.alph);
    writer.writeDouble(data_.sn);
    writer.writeDouble(data_.syz);
    writer.writeDouble(data_.szx);
    writer.writeDouble(data_.beta);
    result.push_back(writer.getLine());

    // Card 4
    writer.clear();
    writer.writeDouble(data_.a1);
    writer.writeDouble(data_.a2);
    writer.writeDouble(data_.a3);
    result.push_back(writer.getLine());

    // Card 5
    writer.clear();
    writer.writeDouble(data_.d1);
    writer.writeDouble(data_.d2);
    writer.writeDouble(data_.d3);
    result.push_back(writer.getLine());

    return result;
}

void MatCompositeDamage::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

// Register keywords
REGISTER_KEYWORD(MatElastic, "*MAT_ELASTIC")
REGISTER_KEYWORD(MatElastic, "*MAT_001")
REGISTER_KEYWORD(MatRigid, "*MAT_RIGID")
REGISTER_KEYWORD(MatRigid, "*MAT_020")
REGISTER_KEYWORD(MatPlasticKinematic, "*MAT_PLASTIC_KINEMATIC")
REGISTER_KEYWORD(MatPlasticKinematic, "*MAT_003")
REGISTER_KEYWORD(MatPiecewiseLinearPlasticity, "*MAT_PIECEWISE_LINEAR_PLASTICITY")
REGISTER_KEYWORD(MatPiecewiseLinearPlasticity, "*MAT_024")
REGISTER_KEYWORD(MatJohnsonCook, "*MAT_JOHNSON_COOK")
REGISTER_KEYWORD(MatJohnsonCook, "*MAT_015")
REGISTER_KEYWORD(MatNull, "*MAT_NULL")
REGISTER_KEYWORD(MatNull, "*MAT_009")
REGISTER_KEYWORD(MatViscoelastic, "*MAT_VISCOELASTIC")
REGISTER_KEYWORD(MatViscoelastic, "*MAT_006")
REGISTER_KEYWORD(MatPowerLawPlasticity, "*MAT_POWER_LAW_PLASTICITY")
REGISTER_KEYWORD(MatPowerLawPlasticity, "*MAT_018")
REGISTER_KEYWORD(MatHoneycomb, "*MAT_HONEYCOMB")
REGISTER_KEYWORD(MatHoneycomb, "*MAT_026")
REGISTER_KEYWORD(MatModifiedPiecewiseLinearPlasticity, "*MAT_MODIFIED_PIECEWISE_LINEAR_PLASTICITY")
REGISTER_KEYWORD(MatModifiedPiecewiseLinearPlasticity, "*MAT_123")
REGISTER_KEYWORD(MatCrushableFoam, "*MAT_CRUSHABLE_FOAM")
REGISTER_KEYWORD(MatCrushableFoam, "*MAT_063")
REGISTER_KEYWORD(MatSpotWeld, "*MAT_SPOTWELD")
REGISTER_KEYWORD(MatSpotWeld, "*MAT_100")
REGISTER_KEYWORD(MatOgdenRubber, "*MAT_OGDEN_RUBBER")
REGISTER_KEYWORD(MatOgdenRubber, "*MAT_077")
REGISTER_KEYWORD(MatFabric, "*MAT_FABRIC")
REGISTER_KEYWORD(MatFabric, "*MAT_034")
REGISTER_KEYWORD(MatMooneyRivlinRubber, "*MAT_MOONEY-RIVLIN_RUBBER")
REGISTER_KEYWORD(MatMooneyRivlinRubber, "*MAT_027")
REGISTER_KEYWORD(MatLowDensityFoam, "*MAT_LOW_DENSITY_FOAM")
REGISTER_KEYWORD(MatLowDensityFoam, "*MAT_057")
REGISTER_KEYWORD(MatOrthotropicElastic, "*MAT_ORTHOTROPIC_ELASTIC")
REGISTER_KEYWORD(MatOrthotropicElastic, "*MAT_002")
REGISTER_KEYWORD(MatEnhancedCompositeDamage, "*MAT_ENHANCED_COMPOSITE_DAMAGE")
REGISTER_KEYWORD(MatEnhancedCompositeDamage, "*MAT_054")
REGISTER_KEYWORD(MatEnhancedCompositeDamage, "*MAT_055")
REGISTER_KEYWORD(MatLaminatedCompositeFabric, "*MAT_LAMINATED_COMPOSITE_FABRIC")
REGISTER_KEYWORD(MatLaminatedCompositeFabric, "*MAT_058")
REGISTER_KEYWORD(MatElasticPlasticThermal, "*MAT_ELASTIC_PLASTIC_THERMAL")
REGISTER_KEYWORD(MatElasticPlasticThermal, "*MAT_004")
REGISTER_KEYWORD(MatSoilAndFoam, "*MAT_SOIL_AND_FOAM")
REGISTER_KEYWORD(MatSoilAndFoam, "*MAT_005")
REGISTER_KEYWORD(MatElasticPlasticHydro, "*MAT_ELASTIC_PLASTIC_HYDRO")
REGISTER_KEYWORD(MatElasticPlasticHydro, "*MAT_010")
REGISTER_KEYWORD(MatCompositeDamage, "*MAT_COMPOSITE_DAMAGE")
REGISTER_KEYWORD(MatCompositeDamage, "*MAT_022")

// ============================================================================
// MatGeologicCapModel
// ============================================================================

bool MatGeologicCapModel::parse(const std::vector<std::string>& lines,
                                 util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t cardNum = 0;
    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);

        if (cardNum == 0) {
            // Card 1: MID, RO, BULK, G, ALPHA, THETA, GAMMA, BETA
            size_t pos = 0;
            data_.mid = parser.getInt64At(pos).value_or(0);
            pos += intW;
            data_.ro = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.bulk = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.g = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.alpha = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.theta = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.gamma = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.beta = parser.getDoubleAt(pos, realW).value_or(0.0);
        } else if (cardNum == 1) {
            // Card 2: R, D, W, X0, TCUT, CONV1, CONV2
            size_t pos = 0;
            data_.r = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.d = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.w = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.x0 = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.tcut = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.conv1 = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.conv2 = parser.getDoubleAt(pos, realW).value_or(0.0);
        }

        cardNum++;
        if (cardNum >= 2) break;
    }

    return true;
}

std::vector<std::string> MatGeologicCapModel::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    // Card 1
    writer.writeInt(data_.mid);
    writer.writeDouble(data_.ro);
    writer.writeDouble(data_.bulk);
    writer.writeDouble(data_.g);
    writer.writeDouble(data_.alpha);
    writer.writeDouble(data_.theta);
    writer.writeDouble(data_.gamma);
    writer.writeDouble(data_.beta);
    result.push_back(writer.getLine());

    // Card 2
    writer.clear();
    writer.writeDouble(data_.r);
    writer.writeDouble(data_.d);
    writer.writeDouble(data_.w);
    writer.writeDouble(data_.x0);
    writer.writeDouble(data_.tcut);
    writer.writeDouble(data_.conv1);
    writer.writeDouble(data_.conv2);
    result.push_back(writer.getLine());

    return result;
}

void MatGeologicCapModel::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

// ============================================================================
// MatPlasticityWithDamage
// ============================================================================

bool MatPlasticityWithDamage::parse(const std::vector<std::string>& lines,
                                     util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t cardNum = 0;
    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);

        if (cardNum == 0) {
            // Card 1: MID, RO, E, PR, SIGY, ETAN, FAIL, TDEL
            size_t pos = 0;
            data_.mid = parser.getInt64At(pos).value_or(0);
            pos += intW;
            data_.ro = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.e = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.pr = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.sigy = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.etan = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.fail = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.tdel = parser.getDoubleAt(pos, realW).value_or(0.0);
        } else if (cardNum == 1) {
            // Card 2: C, P, LCSS, LCSR, VP, EPPF, DTEFAIL, NUMINT
            size_t pos = 0;
            data_.c = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.p = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.lcss = static_cast<int>(parser.getInt64At(pos).value_or(0));
            pos += intW;
            data_.lcsr = static_cast<int>(parser.getInt64At(pos).value_or(0));
            pos += intW;
            data_.vp = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.eppf = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.dtefail = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.numint = static_cast<int>(parser.getInt64At(pos).value_or(0));
        } else if (cardNum == 2) {
            // Card 3: DC, FLAG
            size_t pos = 0;
            data_.dc = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.flag = parser.getDoubleAt(pos, realW).value_or(0.0);
        }

        cardNum++;
        if (cardNum >= 3) break;
    }

    return true;
}

std::vector<std::string> MatPlasticityWithDamage::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    // Card 1
    writer.writeInt(data_.mid);
    writer.writeDouble(data_.ro);
    writer.writeDouble(data_.e);
    writer.writeDouble(data_.pr);
    writer.writeDouble(data_.sigy);
    writer.writeDouble(data_.etan);
    writer.writeDouble(data_.fail);
    writer.writeDouble(data_.tdel);
    result.push_back(writer.getLine());

    // Card 2
    writer.clear();
    writer.writeDouble(data_.c);
    writer.writeDouble(data_.p);
    writer.writeInt(data_.lcss);
    writer.writeInt(data_.lcsr);
    writer.writeDouble(data_.vp);
    writer.writeDouble(data_.eppf);
    writer.writeDouble(data_.dtefail);
    writer.writeInt(data_.numint);
    result.push_back(writer.getLine());

    // Card 3
    writer.clear();
    writer.writeDouble(data_.dc);
    writer.writeDouble(data_.flag);
    result.push_back(writer.getLine());

    return result;
}

void MatPlasticityWithDamage::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

// ============================================================================
// MatSimplifiedJohnsonCook
// ============================================================================

bool MatSimplifiedJohnsonCook::parse(const std::vector<std::string>& lines,
                                      util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t cardNum = 0;
    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);

        if (cardNum == 0) {
            // Card 1: MID, RO, E, PR, VP
            size_t pos = 0;
            data_.mid = parser.getInt64At(pos).value_or(0);
            pos += intW;
            data_.ro = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.e = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.pr = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.vp = parser.getDoubleAt(pos, realW).value_or(0.0);
        } else if (cardNum == 1) {
            // Card 2: A, B, N, C, PSFAIL, SIGMAX, SIGSAT, EPSO
            size_t pos = 0;
            data_.a = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.b = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.n = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.c = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.psfail = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.sigmax = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.sigsat = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.epso = parser.getDoubleAt(pos, realW).value_or(1.0);
        }

        cardNum++;
        if (cardNum >= 2) break;
    }

    return true;
}

std::vector<std::string> MatSimplifiedJohnsonCook::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    // Card 1
    writer.writeInt(data_.mid);
    writer.writeDouble(data_.ro);
    writer.writeDouble(data_.e);
    writer.writeDouble(data_.pr);
    writer.writeDouble(data_.vp);
    result.push_back(writer.getLine());

    // Card 2
    writer.clear();
    writer.writeDouble(data_.a);
    writer.writeDouble(data_.b);
    writer.writeDouble(data_.n);
    writer.writeDouble(data_.c);
    writer.writeDouble(data_.psfail);
    writer.writeDouble(data_.sigmax);
    writer.writeDouble(data_.sigsat);
    writer.writeDouble(data_.epso);
    result.push_back(writer.getLine());

    return result;
}

void MatSimplifiedJohnsonCook::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

// ============================================================================
// MatSamp1
// ============================================================================

bool MatSamp1::parse(const std::vector<std::string>& lines,
                      util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t cardNum = 0;
    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);

        if (cardNum == 0) {
            // Card 1: MID, RO, E, PR
            size_t pos = 0;
            data_.mid = parser.getInt64At(pos).value_or(0);
            pos += intW;
            data_.ro = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.e = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.pr = parser.getDoubleAt(pos, realW).value_or(0.0);
        } else if (cardNum == 1) {
            // Card 2: LCID_T, LCID_C, LCID_S, LCID_B, BETA_T, BETA_C, BETA_S, BETA_B
            size_t pos = 0;
            data_.lcid_t = static_cast<int>(parser.getInt64At(pos).value_or(0));
            pos += intW;
            data_.lcid_c = static_cast<int>(parser.getInt64At(pos).value_or(0));
            pos += intW;
            data_.lcid_s = static_cast<int>(parser.getInt64At(pos).value_or(0));
            pos += intW;
            data_.lcid_b = static_cast<int>(parser.getInt64At(pos).value_or(0));
            pos += intW;
            data_.beta_t = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.beta_c = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.beta_s = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.beta_b = parser.getDoubleAt(pos, realW).value_or(0.0);
        } else if (cardNum == 2) {
            // Card 3: NA, LCID_R, FI, MACF, APTS, HTA, SOFT, IHIS
            size_t pos = 0;
            data_.na = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.lcid_r = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.fi = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.macf = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.apts = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.hta = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.soft = parser.getDoubleAt(pos, realW).value_or(1.0);
            pos += realW;
            data_.ihis = parser.getDoubleAt(pos, realW).value_or(0.0);
        } else if (cardNum == 3) {
            // Card 4: IVOL, FTYPE, FPAR1, FPAR2, FPAR3, FPAR4, EPPFR, NUMFI
            size_t pos = 0;
            data_.ivol = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.ftype = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.fpar1 = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.fpar2 = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.fpar3 = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.fpar4 = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.eppfr = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.numfi = parser.getDoubleAt(pos, realW).value_or(0.0);
        }

        cardNum++;
        if (cardNum >= 4) break;
    }

    return true;
}

std::vector<std::string> MatSamp1::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    // Card 1
    writer.writeInt(data_.mid);
    writer.writeDouble(data_.ro);
    writer.writeDouble(data_.e);
    writer.writeDouble(data_.pr);
    result.push_back(writer.getLine());

    // Card 2
    writer.clear();
    writer.writeInt(data_.lcid_t);
    writer.writeInt(data_.lcid_c);
    writer.writeInt(data_.lcid_s);
    writer.writeInt(data_.lcid_b);
    writer.writeDouble(data_.beta_t);
    writer.writeDouble(data_.beta_c);
    writer.writeDouble(data_.beta_s);
    writer.writeDouble(data_.beta_b);
    result.push_back(writer.getLine());

    // Card 3
    writer.clear();
    writer.writeDouble(data_.na);
    writer.writeDouble(data_.lcid_r);
    writer.writeDouble(data_.fi);
    writer.writeDouble(data_.macf);
    writer.writeDouble(data_.apts);
    writer.writeDouble(data_.hta);
    writer.writeDouble(data_.soft);
    writer.writeDouble(data_.ihis);
    result.push_back(writer.getLine());

    // Card 4
    writer.clear();
    writer.writeDouble(data_.ivol);
    writer.writeDouble(data_.ftype);
    writer.writeDouble(data_.fpar1);
    writer.writeDouble(data_.fpar2);
    writer.writeDouble(data_.fpar3);
    writer.writeDouble(data_.fpar4);
    writer.writeDouble(data_.eppfr);
    writer.writeDouble(data_.numfi);
    result.push_back(writer.getLine());

    return result;
}

void MatSamp1::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(MatGeologicCapModel, "*MAT_GEOLOGIC_CAP_MODEL")
REGISTER_KEYWORD(MatGeologicCapModel, "*MAT_025")
REGISTER_KEYWORD(MatPlasticityWithDamage, "*MAT_PLASTICITY_WITH_DAMAGE")
REGISTER_KEYWORD(MatPlasticityWithDamage, "*MAT_081")
REGISTER_KEYWORD(MatSimplifiedJohnsonCook, "*MAT_SIMPLIFIED_JOHNSON_COOK")
REGISTER_KEYWORD(MatSimplifiedJohnsonCook, "*MAT_098")
REGISTER_KEYWORD(MatSamp1, "*MAT_SAMP-1")
REGISTER_KEYWORD(MatSamp1, "*MAT_187")

// ============================================================================
// MatOrthoElasticPlastic
// ============================================================================

bool MatOrthoElasticPlastic::parse(const std::vector<std::string>& lines,
                                    util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    int cardNum = 0;
    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;

        if (cardNum == 0) {
            // Card 1: MID, RO, EA, EB, EC, PRBA, PRCA, PRCB
            data_.mid = parser.getInt64At(pos).value_or(0);
            pos += intW;
            data_.ro = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.ea = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.eb = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.ec = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.prba = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.prca = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.prcb = parser.getDoubleAt(pos, realW).value_or(0.0);
        } else if (cardNum == 1) {
            // Card 2: GAB, GBC, GCA, SIGY, ETAN, IOPT, MACF
            data_.gab = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.gbc = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.gca = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.sigy = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.etan = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.iopt = static_cast<int>(parser.getInt64At(pos).value_or(0));
            pos += intW;
            data_.macf = static_cast<int>(parser.getInt64At(pos).value_or(1));
        } else if (cardNum == 2) {
            // Card 3: R00, R45, R90, LCID, BETA
            data_.r00 = parser.getDoubleAt(pos, realW).value_or(1.0);
            pos += realW;
            data_.r45 = parser.getDoubleAt(pos, realW).value_or(1.0);
            pos += realW;
            data_.r90 = parser.getDoubleAt(pos, realW).value_or(1.0);
            pos += realW;
            data_.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
            pos += intW;
            data_.beta = parser.getDoubleAt(pos, realW).value_or(0.0);
        }

        cardNum++;
        if (cardNum >= 3) break;
    }

    return true;
}

std::vector<std::string> MatOrthoElasticPlastic::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    // Card 1
    writer.writeInt(data_.mid);
    writer.writeDouble(data_.ro);
    writer.writeDouble(data_.ea);
    writer.writeDouble(data_.eb);
    writer.writeDouble(data_.ec);
    writer.writeDouble(data_.prba);
    writer.writeDouble(data_.prca);
    writer.writeDouble(data_.prcb);
    result.push_back(writer.getLine());

    // Card 2
    writer.clear();
    writer.writeDouble(data_.gab);
    writer.writeDouble(data_.gbc);
    writer.writeDouble(data_.gca);
    writer.writeDouble(data_.sigy);
    writer.writeDouble(data_.etan);
    writer.writeInt(data_.iopt);
    writer.writeInt(data_.macf);
    result.push_back(writer.getLine());

    // Card 3
    writer.clear();
    writer.writeDouble(data_.r00);
    writer.writeDouble(data_.r45);
    writer.writeDouble(data_.r90);
    writer.writeInt(data_.lcid);
    writer.writeDouble(data_.beta);
    result.push_back(writer.getLine());

    return result;
}

void MatOrthoElasticPlastic::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(MatOrthoElasticPlastic, "*MAT_ORTHOTROPIC_ELASTIC_PLASTIC")
REGISTER_KEYWORD(MatOrthoElasticPlastic, "*MAT_108")

// ============================================================================
// MatHighExplosiveBurn (*MAT_008)
// ============================================================================

bool MatHighExplosiveBurn::parse(const std::vector<std::string>& lines,
                                  util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;

        data_.mid = static_cast<MaterialId>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.ro = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.d = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.pcj = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.beta = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.k = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.g = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.sigy = parser.getDoubleAt(pos, realW).value_or(0.0);
        break;
    }

    return true;
}

std::vector<std::string> MatHighExplosiveBurn::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.mid);
    writer.writeDouble(data_.ro);
    writer.writeDouble(data_.d);
    writer.writeDouble(data_.pcj);
    writer.writeDouble(data_.beta);
    writer.writeDouble(data_.k);
    writer.writeDouble(data_.g);
    writer.writeDouble(data_.sigy);
    result.push_back(writer.getLine());

    return result;
}

void MatHighExplosiveBurn::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(MatHighExplosiveBurn, "*MAT_HIGH_EXPLOSIVE_BURN")
REGISTER_KEYWORD(MatHighExplosiveBurn, "*MAT_008")

// ============================================================================
// MatBlatzKoRubber (*MAT_007)
// ============================================================================

bool MatBlatzKoRubber::parse(const std::vector<std::string>& lines,
                              util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;

        data_.mid = static_cast<MaterialId>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.ro = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.g = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.ref = parser.getDoubleAt(pos, realW).value_or(0.0);
        break;
    }

    return true;
}

std::vector<std::string> MatBlatzKoRubber::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.mid);
    writer.writeDouble(data_.ro);
    writer.writeDouble(data_.g);
    writer.writeDouble(data_.ref);
    result.push_back(writer.getLine());

    return result;
}

void MatBlatzKoRubber::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(MatBlatzKoRubber, "*MAT_BLATZ_KO_RUBBER")
REGISTER_KEYWORD(MatBlatzKoRubber, "*MAT_007")

// ============================================================================
// MatSteinberg (*MAT_011)
// ============================================================================

bool MatSteinberg::parse(const std::vector<std::string>& lines,
                          util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;

        data_.mid = static_cast<MaterialId>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.ro = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.g = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.y0 = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.ymax = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.b = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.n = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.h = parser.getDoubleAt(pos, realW).value_or(0.0);
        break;
    }

    return true;
}

std::vector<std::string> MatSteinberg::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.mid);
    writer.writeDouble(data_.ro);
    writer.writeDouble(data_.g);
    writer.writeDouble(data_.y0);
    writer.writeDouble(data_.ymax);
    writer.writeDouble(data_.b);
    writer.writeDouble(data_.n);
    writer.writeDouble(data_.h);
    result.push_back(writer.getLine());

    return result;
}

void MatSteinberg::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(MatSteinberg, "*MAT_STEINBERG")
REGISTER_KEYWORD(MatSteinberg, "*MAT_011")

// ============================================================================
// MatIsotropicElasticFailure (*MAT_013)
// ============================================================================

bool MatIsotropicElasticFailure::parse(const std::vector<std::string>& lines,
                                        util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;

        data_.mid = static_cast<MaterialId>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.ro = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.e = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.pr = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.sigy = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.etan = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.bulk = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.psfail = parser.getDoubleAt(pos, realW).value_or(0.0);
        break;
    }

    return true;
}

std::vector<std::string> MatIsotropicElasticFailure::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.mid);
    writer.writeDouble(data_.ro);
    writer.writeDouble(data_.e);
    writer.writeDouble(data_.pr);
    writer.writeDouble(data_.sigy);
    writer.writeDouble(data_.etan);
    writer.writeDouble(data_.bulk);
    writer.writeDouble(data_.psfail);
    result.push_back(writer.getLine());

    return result;
}

void MatIsotropicElasticFailure::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(MatIsotropicElasticFailure, "*MAT_ISOTROPIC_ELASTIC_FAILURE")
REGISTER_KEYWORD(MatIsotropicElasticFailure, "*MAT_013")

// ============================================================================
// MatIsotropicElasticPlastic (*MAT_012)
// ============================================================================

bool MatIsotropicElasticPlastic::parse(const std::vector<std::string>& lines,
                                        util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;

        data_.mid = static_cast<MaterialId>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.ro = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.e = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.pr = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.sigy = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.etan = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.bulk = parser.getDoubleAt(pos, realW).value_or(0.0);
        break;
    }

    return true;
}

std::vector<std::string> MatIsotropicElasticPlastic::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.mid);
    writer.writeDouble(data_.ro);
    writer.writeDouble(data_.e);
    writer.writeDouble(data_.pr);
    writer.writeDouble(data_.sigy);
    writer.writeDouble(data_.etan);
    writer.writeDouble(data_.bulk);
    result.push_back(writer.getLine());

    return result;
}

void MatIsotropicElasticPlastic::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(MatIsotropicElasticPlastic, "*MAT_ISOTROPIC_ELASTIC_PLASTIC")
REGISTER_KEYWORD(MatIsotropicElasticPlastic, "*MAT_012")

// ============================================================================
// MatSoilAndFoamFailure (*MAT_014)
// ============================================================================

bool MatSoilAndFoamFailure::parse(const std::vector<std::string>& lines,
                                   util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;

        data_.mid = static_cast<MaterialId>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.ro = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.g = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.bulk = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.a0 = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.a1 = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.a2 = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.pc = parser.getDoubleAt(pos, realW).value_or(0.0);
        break;
    }

    return true;
}

std::vector<std::string> MatSoilAndFoamFailure::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.mid);
    writer.writeDouble(data_.ro);
    writer.writeDouble(data_.g);
    writer.writeDouble(data_.bulk);
    writer.writeDouble(data_.a0);
    writer.writeDouble(data_.a1);
    writer.writeDouble(data_.a2);
    writer.writeDouble(data_.pc);
    result.push_back(writer.getLine());

    return result;
}

void MatSoilAndFoamFailure::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(MatSoilAndFoamFailure, "*MAT_SOIL_AND_FOAM_FAILURE")
REGISTER_KEYWORD(MatSoilAndFoamFailure, "*MAT_014")

// ============================================================================
// MatPseudoTensor (*MAT_016)
// ============================================================================

bool MatPseudoTensor::parse(const std::vector<std::string>& lines,
                            util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;

        data_.mid = static_cast<MaterialId>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.ro = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.g = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.pr = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.sigy = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.etan = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.lcp = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.lcr = parser.getDoubleAt(pos, realW).value_or(0.0);
        break;
    }

    return true;
}

std::vector<std::string> MatPseudoTensor::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.mid);
    writer.writeDouble(data_.ro);
    writer.writeDouble(data_.g);
    writer.writeDouble(data_.pr);
    writer.writeDouble(data_.sigy);
    writer.writeDouble(data_.etan);
    writer.writeDouble(data_.lcp);
    writer.writeDouble(data_.lcr);
    result.push_back(writer.getLine());

    return result;
}

void MatPseudoTensor::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(MatPseudoTensor, "*MAT_PSEUDO_TENSOR")
REGISTER_KEYWORD(MatPseudoTensor, "*MAT_016")

// ============================================================================
// MatOrientedCrack (*MAT_017)
// ============================================================================

bool MatOrientedCrack::parse(const std::vector<std::string>& lines,
                              util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;

        data_.mid = static_cast<MaterialId>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.ro = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.e = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.pr = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.sigy = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.etan = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.fs = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.soft = parser.getDoubleAt(pos, realW).value_or(0.0);
        break;
    }

    return true;
}

std::vector<std::string> MatOrientedCrack::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.mid);
    writer.writeDouble(data_.ro);
    writer.writeDouble(data_.e);
    writer.writeDouble(data_.pr);
    writer.writeDouble(data_.sigy);
    writer.writeDouble(data_.etan);
    writer.writeDouble(data_.fs);
    writer.writeDouble(data_.soft);
    result.push_back(writer.getLine());

    return result;
}

void MatOrientedCrack::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(MatOrientedCrack, "*MAT_ORIENTED_CRACK")
REGISTER_KEYWORD(MatOrientedCrack, "*MAT_017")

// ============================================================================
// MatStrainRateDependentPlasticity (*MAT_019)
// ============================================================================

bool MatStrainRateDependentPlasticity::parse(const std::vector<std::string>& lines,
                                              util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;

        data_.mid = static_cast<MaterialId>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.ro = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.e = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.pr = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.sigy = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.etan = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.c = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.p = parser.getDoubleAt(pos, realW).value_or(0.0);
        break;
    }

    return true;
}

std::vector<std::string> MatStrainRateDependentPlasticity::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.mid);
    writer.writeDouble(data_.ro);
    writer.writeDouble(data_.e);
    writer.writeDouble(data_.pr);
    writer.writeDouble(data_.sigy);
    writer.writeDouble(data_.etan);
    writer.writeDouble(data_.c);
    writer.writeDouble(data_.p);
    result.push_back(writer.getLine());

    return result;
}

void MatStrainRateDependentPlasticity::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(MatStrainRateDependentPlasticity, "*MAT_STRAIN_RATE_DEPENDENT_PLASTICITY")
REGISTER_KEYWORD(MatStrainRateDependentPlasticity, "*MAT_019")

// ============================================================================
// MatThermalOrthotropic (*MAT_021)
// ============================================================================

bool MatThermalOrthotropic::parse(const std::vector<std::string>& lines,
                                   util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;

        data_.mid = static_cast<MaterialId>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.ro = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.ea = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.eb = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.ec = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.prba = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.prca = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.prcb = parser.getDoubleAt(pos, realW).value_or(0.0);
        break;
    }

    return true;
}

std::vector<std::string> MatThermalOrthotropic::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.mid);
    writer.writeDouble(data_.ro);
    writer.writeDouble(data_.ea);
    writer.writeDouble(data_.eb);
    writer.writeDouble(data_.ec);
    writer.writeDouble(data_.prba);
    writer.writeDouble(data_.prca);
    writer.writeDouble(data_.prcb);
    result.push_back(writer.getLine());

    return result;
}

void MatThermalOrthotropic::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(MatThermalOrthotropic, "*MAT_THERMAL_ORTHOTROPIC")
REGISTER_KEYWORD(MatThermalOrthotropic, "*MAT_021")

// ============================================================================
// MatTempDependentOrthotropic (*MAT_023)
// ============================================================================

bool MatTempDependentOrthotropic::parse(const std::vector<std::string>& lines,
                                         util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;

        data_.mid = static_cast<MaterialId>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.ro = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.ea = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.eb = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.ec = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.prba = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.prca = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.prcb = parser.getDoubleAt(pos, realW).value_or(0.0);
        break;
    }

    return true;
}

std::vector<std::string> MatTempDependentOrthotropic::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.mid);
    writer.writeDouble(data_.ro);
    writer.writeDouble(data_.ea);
    writer.writeDouble(data_.eb);
    writer.writeDouble(data_.ec);
    writer.writeDouble(data_.prba);
    writer.writeDouble(data_.prca);
    writer.writeDouble(data_.prcb);
    result.push_back(writer.getLine());

    return result;
}

void MatTempDependentOrthotropic::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(MatTempDependentOrthotropic, "*MAT_TEMPERATURE_DEPENDENT_ORTHOTROPIC")
REGISTER_KEYWORD(MatTempDependentOrthotropic, "*MAT_023")

// ============================================================================
// MatResultantPlasticity (*MAT_028)
// ============================================================================

bool MatResultantPlasticity::parse(const std::vector<std::string>& lines,
                                    util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t cardNum = 0;
    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);

        if (cardNum == 0) {
            size_t pos = 0;
            data_.mid = parser.getInt64At(pos).value_or(0);
            pos += intW;
            data_.ro = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.e = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.pr = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.sigy = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.etan = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.fail = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.tdel = parser.getDoubleAt(pos, realW).value_or(0.0);
        } else if (cardNum == 1) {
            size_t pos = 0;
            data_.c = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.p = parser.getDoubleAt(pos, realW).value_or(0.0);
        }

        cardNum++;
        if (cardNum >= 2) break;
    }

    return true;
}

std::vector<std::string> MatResultantPlasticity::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.mid);
    writer.writeDouble(data_.ro);
    writer.writeDouble(data_.e);
    writer.writeDouble(data_.pr);
    writer.writeDouble(data_.sigy);
    writer.writeDouble(data_.etan);
    writer.writeDouble(data_.fail);
    writer.writeDouble(data_.tdel);
    result.push_back(writer.getLine());

    writer.clear();
    writer.writeDouble(data_.c);
    writer.writeDouble(data_.p);
    result.push_back(writer.getLine());

    return result;
}

void MatResultantPlasticity::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(MatResultantPlasticity, "*MAT_RESULTANT_PLASTICITY")
REGISTER_KEYWORD(MatResultantPlasticity, "*MAT_028")

// ============================================================================
// MatForceLimited (*MAT_029)
// ============================================================================

bool MatForceLimited::parse(const std::vector<std::string>& lines,
                             util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);

        size_t pos = 0;
        data_.mid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data_.ro = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.e = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.pr = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.df = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.apts = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.failm = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));

        break;
    }

    return true;
}

std::vector<std::string> MatForceLimited::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.mid);
    writer.writeDouble(data_.ro);
    writer.writeDouble(data_.e);
    writer.writeDouble(data_.pr);
    writer.writeDouble(data_.df);
    writer.writeDouble(data_.apts);
    writer.writeDouble(data_.failm);
    writer.writeInt(data_.lcid);

    result.push_back(writer.getLine());
    return result;
}

void MatForceLimited::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(MatForceLimited, "*MAT_FORCE_LIMITED")
REGISTER_KEYWORD(MatForceLimited, "*MAT_029")

// ============================================================================
// MatShapeMemory (*MAT_030)
// ============================================================================

bool MatShapeMemory::parse(const std::vector<std::string>& lines,
                            util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t cardNum = 0;
    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);

        if (cardNum == 0) {
            size_t pos = 0;
            data_.mid = parser.getInt64At(pos).value_or(0);
            pos += intW;
            data_.ro = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.e = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.pr = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.em = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.sigasm = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.sigafm = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.sigsma = parser.getDoubleAt(pos, realW).value_or(0.0);
        } else if (cardNum == 1) {
            size_t pos = 0;
            data_.sigfma = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.eps_l = parser.getDoubleAt(pos, realW).value_or(0.0);
        }

        cardNum++;
        if (cardNum >= 2) break;
    }

    return true;
}

std::vector<std::string> MatShapeMemory::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.mid);
    writer.writeDouble(data_.ro);
    writer.writeDouble(data_.e);
    writer.writeDouble(data_.pr);
    writer.writeDouble(data_.em);
    writer.writeDouble(data_.sigasm);
    writer.writeDouble(data_.sigafm);
    writer.writeDouble(data_.sigsma);
    result.push_back(writer.getLine());

    writer.clear();
    writer.writeDouble(data_.sigfma);
    writer.writeDouble(data_.eps_l);
    result.push_back(writer.getLine());

    return result;
}

void MatShapeMemory::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(MatShapeMemory, "*MAT_SHAPE_MEMORY")
REGISTER_KEYWORD(MatShapeMemory, "*MAT_030")

// ============================================================================
// MatFrazerNashRubber (*MAT_031)
// ============================================================================

bool MatFrazerNashRubber::parse(const std::vector<std::string>& lines,
                                 util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);

        size_t pos = 0;
        data_.mid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data_.ro = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.pr = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.n = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.g = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.sigf = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.ref = parser.getDoubleAt(pos, realW).value_or(0.0);

        break;
    }

    return true;
}

std::vector<std::string> MatFrazerNashRubber::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.mid);
    writer.writeDouble(data_.ro);
    writer.writeDouble(data_.pr);
    writer.writeDouble(data_.n);
    writer.writeDouble(data_.g);
    writer.writeDouble(data_.sigf);
    writer.writeDouble(data_.ref);

    result.push_back(writer.getLine());
    return result;
}

void MatFrazerNashRubber::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(MatFrazerNashRubber, "*MAT_FRAZER_NASH_RUBBER")
REGISTER_KEYWORD(MatFrazerNashRubber, "*MAT_031")

// ============================================================================
// MatLaminatedGlass (*MAT_032)
// ============================================================================

bool MatLaminatedGlass::parse(const std::vector<std::string>& lines,
                               util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);

        size_t pos = 0;
        data_.mid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data_.ro = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.e = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.pr = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.sigy = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.etan = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.fail = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.tdel = parser.getDoubleAt(pos, realW).value_or(0.0);

        break;
    }

    return true;
}

std::vector<std::string> MatLaminatedGlass::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.mid);
    writer.writeDouble(data_.ro);
    writer.writeDouble(data_.e);
    writer.writeDouble(data_.pr);
    writer.writeDouble(data_.sigy);
    writer.writeDouble(data_.etan);
    writer.writeDouble(data_.fail);
    writer.writeDouble(data_.tdel);

    result.push_back(writer.getLine());
    return result;
}

void MatLaminatedGlass::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(MatLaminatedGlass, "*MAT_LAMINATED_GLASS")
REGISTER_KEYWORD(MatLaminatedGlass, "*MAT_032")

// ============================================================================
// MatBarlatAnisotropicPlasticity (*MAT_033)
// ============================================================================

bool MatBarlatAnisotropicPlasticity::parse(const std::vector<std::string>& lines,
                                            util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t cardNum = 0;
    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);

        if (cardNum == 0) {
            size_t pos = 0;
            data_.mid = parser.getInt64At(pos).value_or(0);
            pos += intW;
            data_.ro = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.e = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.pr = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.sigy = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
            pos += intW;
            data_.m = parser.getDoubleAt(pos, realW).value_or(0.0);
        } else if (cardNum == 1) {
            size_t pos = 0;
            data_.r00 = parser.getDoubleAt(pos, realW).value_or(1.0);
            pos += realW;
            data_.r45 = parser.getDoubleAt(pos, realW).value_or(1.0);
            pos += realW;
            data_.r90 = parser.getDoubleAt(pos, realW).value_or(1.0);
            pos += realW;
            data_.cb = parser.getDoubleAt(pos, realW).value_or(0.0);
        }

        cardNum++;
        if (cardNum >= 2) break;
    }

    return true;
}

std::vector<std::string> MatBarlatAnisotropicPlasticity::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.mid);
    writer.writeDouble(data_.ro);
    writer.writeDouble(data_.e);
    writer.writeDouble(data_.pr);
    writer.writeDouble(data_.sigy);
    writer.writeInt(data_.lcid);
    writer.writeDouble(data_.m);
    result.push_back(writer.getLine());

    writer.clear();
    writer.writeDouble(data_.r00);
    writer.writeDouble(data_.r45);
    writer.writeDouble(data_.r90);
    writer.writeDouble(data_.cb);
    result.push_back(writer.getLine());

    return result;
}

void MatBarlatAnisotropicPlasticity::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(MatBarlatAnisotropicPlasticity, "*MAT_BARLAT_ANISOTROPIC_PLASTICITY")
REGISTER_KEYWORD(MatBarlatAnisotropicPlasticity, "*MAT_033")

// ============================================================================
// MatSpringElastic (*MAT_S01)
// ============================================================================

bool MatSpringElastic::parse(const std::vector<std::string>& lines,
                              util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);

        size_t pos = 0;
        data_.mid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data_.k = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.f0 = parser.getDoubleAt(pos, realW).value_or(0.0);

        break;
    }

    return true;
}

std::vector<std::string> MatSpringElastic::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.mid);
    writer.writeDouble(data_.k);
    writer.writeDouble(data_.f0);

    result.push_back(writer.getLine());
    return result;
}

void MatSpringElastic::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(MatSpringElastic, "*MAT_SPRING_ELASTIC")
REGISTER_KEYWORD(MatSpringElastic, "*MAT_S01")

// ============================================================================
// MatDamperViscous (*MAT_S02)
// ============================================================================

bool MatDamperViscous::parse(const std::vector<std::string>& lines,
                              util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);

        size_t pos = 0;
        data_.mid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data_.dc = parser.getDoubleAt(pos, realW).value_or(0.0);

        break;
    }

    return true;
}

std::vector<std::string> MatDamperViscous::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.mid);
    writer.writeDouble(data_.dc);

    result.push_back(writer.getLine());
    return result;
}

void MatDamperViscous::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(MatDamperViscous, "*MAT_DAMPER_VISCOUS")
REGISTER_KEYWORD(MatDamperViscous, "*MAT_S02")

// ============================================================================
// MatSpringNonlinearElastic (*MAT_S03)
// ============================================================================

bool MatSpringNonlinearElastic::parse(const std::vector<std::string>& lines,
                                       util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);

        size_t pos = 0;
        data_.mid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data_.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.stiff = parser.getDoubleAt(pos, realW).value_or(0.0);

        break;
    }

    return true;
}

std::vector<std::string> MatSpringNonlinearElastic::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.mid);
    writer.writeInt(data_.lcid);
    writer.writeDouble(data_.stiff);

    result.push_back(writer.getLine());
    return result;
}

void MatSpringNonlinearElastic::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(MatSpringNonlinearElastic, "*MAT_SPRING_NONLINEAR_ELASTIC")
REGISTER_KEYWORD(MatSpringNonlinearElastic, "*MAT_S03")

// ============================================================================
// MatSpringElastoplastic (*MAT_S04)
// ============================================================================

bool MatSpringElastoplastic::parse(const std::vector<std::string>& lines,
                                    util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);

        size_t pos = 0;
        data_.mid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data_.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.k = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.fy = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.beta = parser.getDoubleAt(pos, realW).value_or(0.0);

        break;
    }

    return true;
}

std::vector<std::string> MatSpringElastoplastic::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.mid);
    writer.writeInt(data_.lcid);
    writer.writeDouble(data_.k);
    writer.writeDouble(data_.fy);
    writer.writeDouble(data_.beta);

    result.push_back(writer.getLine());
    return result;
}

void MatSpringElastoplastic::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(MatSpringElastoplastic, "*MAT_SPRING_ELASTOPLASTIC")
REGISTER_KEYWORD(MatSpringElastoplastic, "*MAT_S04")

// ============================================================================
// MatSpringGeneralNonlinear (*MAT_S05)
// ============================================================================

bool MatSpringGeneralNonlinear::parse(const std::vector<std::string>& lines,
                                       util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);

        size_t pos = 0;
        data_.mid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data_.lcidl = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.lcidu = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.k = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.beta = parser.getDoubleAt(pos, realW).value_or(0.0);

        break;
    }

    return true;
}

std::vector<std::string> MatSpringGeneralNonlinear::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.mid);
    writer.writeInt(data_.lcidl);
    writer.writeInt(data_.lcidu);
    writer.writeDouble(data_.k);
    writer.writeDouble(data_.beta);

    result.push_back(writer.getLine());
    return result;
}

void MatSpringGeneralNonlinear::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(MatSpringGeneralNonlinear, "*MAT_SPRING_GENERAL_NONLINEAR")
REGISTER_KEYWORD(MatSpringGeneralNonlinear, "*MAT_S05")

// ============================================================================
// MatSpringMaxwell (*MAT_S06)
// ============================================================================

bool MatSpringMaxwell::parse(const std::vector<std::string>& lines,
                              util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);

        size_t pos = 0;
        data_.mid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data_.k = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.c = parser.getDoubleAt(pos, realW).value_or(0.0);

        break;
    }

    return true;
}

std::vector<std::string> MatSpringMaxwell::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.mid);
    writer.writeDouble(data_.k);
    writer.writeDouble(data_.c);

    result.push_back(writer.getLine());
    return result;
}

void MatSpringMaxwell::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(MatSpringMaxwell, "*MAT_SPRING_MAXWELL")
REGISTER_KEYWORD(MatSpringMaxwell, "*MAT_S06")

// ============================================================================
// MatCableDiscreteBeam (*MAT_071)
// ============================================================================

bool MatCableDiscreteBeam::parse(const std::vector<std::string>& lines,
                                  util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);

        size_t pos = 0;
        data_.mid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data_.ro = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.e = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.area = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.fo = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.lmin = parser.getDoubleAt(pos, realW).value_or(0.0);

        break;
    }

    return true;
}

std::vector<std::string> MatCableDiscreteBeam::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.mid);
    writer.writeDouble(data_.ro);
    writer.writeDouble(data_.e);
    writer.writeDouble(data_.area);
    writer.writeDouble(data_.fo);
    writer.writeDouble(data_.lmin);

    result.push_back(writer.getLine());
    return result;
}

void MatCableDiscreteBeam::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(MatCableDiscreteBeam, "*MAT_CABLE_DISCRETE_BEAM")
REGISTER_KEYWORD(MatCableDiscreteBeam, "*MAT_071")

// ============================================================================
// MatElasticViscoplasticThermal (*MAT_106)
// ============================================================================

bool MatElasticViscoplasticThermal::parse(const std::vector<std::string>& lines,
                                           util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);

        size_t pos = 0;
        data_.mid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data_.ro = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.e = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.pr = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.sigy = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.alpha = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.lcss = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.qt1 = parser.getDoubleAt(pos, realW).value_or(0.0);

        break;
    }

    return true;
}

std::vector<std::string> MatElasticViscoplasticThermal::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.mid);
    writer.writeDouble(data_.ro);
    writer.writeDouble(data_.e);
    writer.writeDouble(data_.pr);
    writer.writeDouble(data_.sigy);
    writer.writeDouble(data_.alpha);
    writer.writeDouble(data_.lcss);
    writer.writeDouble(data_.qt1);

    result.push_back(writer.getLine());
    return result;
}

void MatElasticViscoplasticThermal::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(MatElasticViscoplasticThermal, "*MAT_ELASTIC_VISCOPLASTIC_THERMAL")
REGISTER_KEYWORD(MatElasticViscoplasticThermal, "*MAT_106")

// ============================================================================
// MatUserDefined (*MAT_041-050)
// ============================================================================

bool MatUserDefined::parse(const std::vector<std::string>& lines,
                            util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t cardNum = 0;
    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);

        if (cardNum == 0) {
            size_t pos = 0;
            data_.mid = parser.getInt64At(pos).value_or(0);
            pos += intW;
            data_.ro = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.mt = static_cast<int>(parser.getInt64At(pos).value_or(0));
            pos += intW;
            data_.lmc = static_cast<int>(parser.getInt64At(pos).value_or(0));
            pos += intW;
            data_.nhv = static_cast<int>(parser.getInt64At(pos).value_or(0));
            pos += intW;
            data_.iortho = static_cast<int>(parser.getInt64At(pos).value_or(0));
            pos += intW;
            data_.ibulk = static_cast<int>(parser.getInt64At(pos).value_or(0));
            pos += intW;
            data_.ig = static_cast<int>(parser.getInt64At(pos).value_or(0));
        } else {
            // Read material constants
            size_t pos = 0;
            for (int i = 0; i < 8 && static_cast<int>(data_.p.size()) < data_.lmc; i++) {
                data_.p.push_back(parser.getDoubleAt(pos, realW).value_or(0.0));
                pos += realW;
            }
        }

        cardNum++;
    }

    return true;
}

std::vector<std::string> MatUserDefined::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.mid);
    writer.writeDouble(data_.ro);
    writer.writeInt(data_.mt);
    writer.writeInt(data_.lmc);
    writer.writeInt(data_.nhv);
    writer.writeInt(data_.iortho);
    writer.writeInt(data_.ibulk);
    writer.writeInt(data_.ig);
    result.push_back(writer.getLine());

    // Write material constants
    size_t idx = 0;
    while (idx < data_.p.size()) {
        writer.clear();
        for (int i = 0; i < 8 && idx < data_.p.size(); i++, idx++) {
            writer.writeDouble(data_.p[idx]);
        }
        result.push_back(writer.getLine());
    }

    return result;
}

void MatUserDefined::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(MatUserDefined, "*MAT_USER_DEFINED_MATERIAL_MODELS")
REGISTER_KEYWORD(MatUserDefined, "*MAT_041")
REGISTER_KEYWORD(MatUserDefined, "*MAT_042")
REGISTER_KEYWORD(MatUserDefined, "*MAT_043")
REGISTER_KEYWORD(MatUserDefined, "*MAT_044")
REGISTER_KEYWORD(MatUserDefined, "*MAT_045")
REGISTER_KEYWORD(MatUserDefined, "*MAT_046")
REGISTER_KEYWORD(MatUserDefined, "*MAT_047")
REGISTER_KEYWORD(MatUserDefined, "*MAT_048")
REGISTER_KEYWORD(MatUserDefined, "*MAT_049")
REGISTER_KEYWORD(MatUserDefined, "*MAT_050")

// ============================================================================
// MatFuChangFoam (*MAT_083)
// ============================================================================

bool MatFuChangFoam::parse(const std::vector<std::string>& lines,
                            util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t cardNum = 0;
    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);

        if (cardNum == 0) {
            size_t pos = 0;
            data_.mid = parser.getInt64At(pos).value_or(0);
            pos += intW;
            data_.ro = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.e = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.kcon = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
            pos += intW;
            data_.tc = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.hu = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.beta = parser.getDoubleAt(pos, realW).value_or(0.0);
        } else if (cardNum == 1) {
            size_t pos = 0;
            data_.damp = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.shape = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.fail = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.bvflag = static_cast<int>(parser.getInt64At(pos).value_or(0));
            pos += intW;
            data_.ed = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.beta1 = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.kflag = static_cast<int>(parser.getInt64At(pos).value_or(0));
            pos += intW;
            data_.ref = parser.getDoubleAt(pos, realW).value_or(0.0);
        }

        cardNum++;
        if (cardNum >= 2) break;
    }

    return true;
}

std::vector<std::string> MatFuChangFoam::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.mid);
    writer.writeDouble(data_.ro);
    writer.writeDouble(data_.e);
    writer.writeDouble(data_.kcon);
    writer.writeInt(data_.lcid);
    writer.writeDouble(data_.tc);
    writer.writeDouble(data_.hu);
    writer.writeDouble(data_.beta);
    result.push_back(writer.getLine());

    writer.clear();
    writer.writeDouble(data_.damp);
    writer.writeDouble(data_.shape);
    writer.writeDouble(data_.fail);
    writer.writeInt(data_.bvflag);
    writer.writeDouble(data_.ed);
    writer.writeDouble(data_.beta1);
    writer.writeInt(data_.kflag);
    writer.writeDouble(data_.ref);
    result.push_back(writer.getLine());

    return result;
}

void MatFuChangFoam::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(MatFuChangFoam, "*MAT_FU_CHANG_FOAM")
REGISTER_KEYWORD(MatFuChangFoam, "*MAT_083")

// ============================================================================
// MatWinfrithConcrete (*MAT_084 / *MAT_085)
// ============================================================================

bool MatWinfrithConcrete::parse(const std::vector<std::string>& lines,
                                 util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t cardNum = 0;
    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);

        if (cardNum == 0) {
            size_t pos = 0;
            data_.mid = parser.getInt64At(pos).value_or(0);
            pos += intW;
            data_.ro = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.tm = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.pr = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.ucs = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.uts = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.fe = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.apts = parser.getDoubleAt(pos, realW).value_or(0.0);
        } else if (cardNum == 1) {
            size_t pos = 0;
            data_.strfl = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.e = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.ys = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.erate = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.conm = static_cast<int>(parser.getInt64At(pos).value_or(0));
        }

        cardNum++;
        if (cardNum >= 2) break;
    }

    return true;
}

std::vector<std::string> MatWinfrithConcrete::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.mid);
    writer.writeDouble(data_.ro);
    writer.writeDouble(data_.tm);
    writer.writeDouble(data_.pr);
    writer.writeDouble(data_.ucs);
    writer.writeDouble(data_.uts);
    writer.writeDouble(data_.fe);
    writer.writeDouble(data_.apts);
    result.push_back(writer.getLine());

    writer.clear();
    writer.writeDouble(data_.strfl);
    writer.writeDouble(data_.e);
    writer.writeDouble(data_.ys);
    writer.writeDouble(data_.erate);
    writer.writeInt(data_.conm);
    result.push_back(writer.getLine());

    return result;
}

void MatWinfrithConcrete::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(MatWinfrithConcrete, "*MAT_WINFRITH_CONCRETE")
REGISTER_KEYWORD(MatWinfrithConcrete, "*MAT_084")
REGISTER_KEYWORD(MatWinfrithConcrete, "*MAT_085")

// ============================================================================
// MatConcreteDamageRel3 (*MAT_072R3)
// ============================================================================

bool MatConcreteDamageRel3::parse(const std::vector<std::string>& lines,
                                   util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t cardNum = 0;
    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);

        if (cardNum == 0) {
            size_t pos = 0;
            data_.mid = parser.getInt64At(pos).value_or(0);
            pos += intW;
            data_.ro = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.pr = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.ft = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.a0 = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.a1 = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.a2 = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.b1 = parser.getDoubleAt(pos, realW).value_or(0.0);
        } else if (cardNum == 1) {
            size_t pos = 0;
            data_.omega = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.a1f = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.srate = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.eta = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.fc = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.loctype = static_cast<int>(parser.getInt64At(pos).value_or(0));
            pos += intW;
            data_.wlz = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.lcrate = parser.getDoubleAt(pos, realW).value_or(0.0);
        }

        cardNum++;
        if (cardNum >= 2) break;
    }

    return true;
}

std::vector<std::string> MatConcreteDamageRel3::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.mid);
    writer.writeDouble(data_.ro);
    writer.writeDouble(data_.pr);
    writer.writeDouble(data_.ft);
    writer.writeDouble(data_.a0);
    writer.writeDouble(data_.a1);
    writer.writeDouble(data_.a2);
    writer.writeDouble(data_.b1);
    result.push_back(writer.getLine());

    writer.clear();
    writer.writeDouble(data_.omega);
    writer.writeDouble(data_.a1f);
    writer.writeDouble(data_.srate);
    writer.writeDouble(data_.eta);
    writer.writeDouble(data_.fc);
    writer.writeInt(data_.loctype);
    writer.writeDouble(data_.wlz);
    writer.writeDouble(data_.lcrate);
    result.push_back(writer.getLine());

    return result;
}

void MatConcreteDamageRel3::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(MatConcreteDamageRel3, "*MAT_CONCRETE_DAMAGE_REL3")
REGISTER_KEYWORD(MatConcreteDamageRel3, "*MAT_072R3")

// ============================================================================
// MatCscmConcrete (*MAT_159)
// ============================================================================

bool MatCscmConcrete::parse(const std::vector<std::string>& lines,
                             util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t cardNum = 0;
    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);

        if (cardNum == 0) {
            size_t pos = 0;
            data_.mid = parser.getInt64At(pos).value_or(0);
            pos += intW;
            data_.ro = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.nplot = static_cast<int>(parser.getInt64At(pos).value_or(1));
            pos += intW;
            data_.incre = static_cast<int>(parser.getInt64At(pos).value_or(0));
            pos += intW;
            data_.irate = static_cast<int>(parser.getInt64At(pos).value_or(0));
            pos += intW;
            data_.erode = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.recov = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.iretrc = static_cast<int>(parser.getInt64At(pos).value_or(0));
        } else if (cardNum == 1) {
            size_t pos = 0;
            data_.pred = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.fc = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.datefac = parser.getDoubleAt(pos, realW).value_or(1.0);
            pos += realW;
            data_.units = static_cast<int>(parser.getInt64At(pos).value_or(0));
        }

        cardNum++;
        if (cardNum >= 2) break;
    }

    return true;
}

std::vector<std::string> MatCscmConcrete::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.mid);
    writer.writeDouble(data_.ro);
    writer.writeInt(data_.nplot);
    writer.writeInt(data_.incre);
    writer.writeInt(data_.irate);
    writer.writeDouble(data_.erode);
    writer.writeDouble(data_.recov);
    writer.writeInt(data_.iretrc);
    result.push_back(writer.getLine());

    writer.clear();
    writer.writeDouble(data_.pred);
    writer.writeDouble(data_.fc);
    writer.writeDouble(data_.datefac);
    writer.writeInt(data_.units);
    result.push_back(writer.getLine());

    return result;
}

void MatCscmConcrete::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(MatCscmConcrete, "*MAT_CSCM_CONCRETE")
REGISTER_KEYWORD(MatCscmConcrete, "*MAT_CSCM")
REGISTER_KEYWORD(MatCscmConcrete, "*MAT_159")

// ============================================================================
// MatPlasticGreenNaghdi (*MAT_035)
// ============================================================================

bool MatPlasticGreenNaghdi::parse(const std::vector<std::string>& lines,
                                   util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t cardNum = 0;
    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);

        if (cardNum == 0) {
            size_t pos = 0;
            data_.mid = parser.getInt64At(pos).value_or(0);
            pos += intW;
            data_.ro = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.e = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.pr = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.sigy = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.etan = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.bulk = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.lcss = static_cast<int>(parser.getInt64At(pos).value_or(0));
        } else if (cardNum == 1) {
            size_t pos = 0;
            data_.q1 = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.c1 = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.q2 = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.c2 = parser.getDoubleAt(pos, realW).value_or(0.0);
        }

        cardNum++;
        if (cardNum >= 2) break;
    }

    return true;
}

std::vector<std::string> MatPlasticGreenNaghdi::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.mid);
    writer.writeDouble(data_.ro);
    writer.writeDouble(data_.e);
    writer.writeDouble(data_.pr);
    writer.writeDouble(data_.sigy);
    writer.writeDouble(data_.etan);
    writer.writeDouble(data_.bulk);
    writer.writeInt(data_.lcss);
    result.push_back(writer.getLine());

    writer.clear();
    writer.writeDouble(data_.q1);
    writer.writeDouble(data_.c1);
    writer.writeDouble(data_.q2);
    writer.writeDouble(data_.c2);
    result.push_back(writer.getLine());

    return result;
}

void MatPlasticGreenNaghdi::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(MatPlasticGreenNaghdi, "*MAT_PLASTIC_GREEN_NAGHDI")
REGISTER_KEYWORD(MatPlasticGreenNaghdi, "*MAT_035")

// ============================================================================
// Mat3ParameterBarlat (*MAT_036)
// ============================================================================

bool Mat3ParameterBarlat::parse(const std::vector<std::string>& lines,
                                 util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t cardNum = 0;
    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);

        if (cardNum == 0) {
            size_t pos = 0;
            data_.mid = parser.getInt64At(pos).value_or(0);
            pos += intW;
            data_.ro = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.e = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.pr = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.k = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.m = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.r00 = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.r45 = parser.getDoubleAt(pos, realW).value_or(0.0);
        } else if (cardNum == 1) {
            size_t pos = 0;
            data_.r90 = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.lcss = static_cast<int>(parser.getInt64At(pos).value_or(0));
            pos += intW;
            data_.aopt = parser.getDoubleAt(pos, realW).value_or(0.0);
        }

        cardNum++;
        if (cardNum >= 2) break;
    }

    return true;
}

std::vector<std::string> Mat3ParameterBarlat::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.mid);
    writer.writeDouble(data_.ro);
    writer.writeDouble(data_.e);
    writer.writeDouble(data_.pr);
    writer.writeDouble(data_.k);
    writer.writeDouble(data_.m);
    writer.writeDouble(data_.r00);
    writer.writeDouble(data_.r45);
    result.push_back(writer.getLine());

    writer.clear();
    writer.writeDouble(data_.r90);
    writer.writeInt(data_.lcss);
    writer.writeDouble(data_.aopt);
    result.push_back(writer.getLine());

    return result;
}

void Mat3ParameterBarlat::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(Mat3ParameterBarlat, "*MAT_3-PARAMETER_BARLAT")
REGISTER_KEYWORD(Mat3ParameterBarlat, "*MAT_036")

// ============================================================================
// MatTransverselyAnisotropicElasticPlastic (*MAT_037)
// ============================================================================

bool MatTransverselyAnisotropicElasticPlastic::parse(const std::vector<std::string>& lines,
                                                      util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);

        size_t pos = 0;
        data_.mid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data_.ro = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.e = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.pr = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.sigy = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.etan = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.r = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.hlcid = static_cast<int>(parser.getInt64At(pos).value_or(0));

        break;
    }

    return true;
}

std::vector<std::string> MatTransverselyAnisotropicElasticPlastic::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.mid);
    writer.writeDouble(data_.ro);
    writer.writeDouble(data_.e);
    writer.writeDouble(data_.pr);
    writer.writeDouble(data_.sigy);
    writer.writeDouble(data_.etan);
    writer.writeDouble(data_.r);
    writer.writeInt(data_.hlcid);
    result.push_back(writer.getLine());

    return result;
}

void MatTransverselyAnisotropicElasticPlastic::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(MatTransverselyAnisotropicElasticPlastic, "*MAT_TRANSVERSELY_ANISOTROPIC_ELASTIC_PLASTIC")
REGISTER_KEYWORD(MatTransverselyAnisotropicElasticPlastic, "*MAT_037")

// ============================================================================
// MatBlatzKoFoam (*MAT_038)
// ============================================================================

bool MatBlatzKoFoam::parse(const std::vector<std::string>& lines,
                            util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);

        size_t pos = 0;
        data_.mid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data_.ro = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.g = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.ref = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.dtefac = parser.getDoubleAt(pos, realW).value_or(0.0);

        break;
    }

    return true;
}

std::vector<std::string> MatBlatzKoFoam::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.mid);
    writer.writeDouble(data_.ro);
    writer.writeDouble(data_.g);
    writer.writeDouble(data_.ref);
    writer.writeDouble(data_.dtefac);
    result.push_back(writer.getLine());

    return result;
}

void MatBlatzKoFoam::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(MatBlatzKoFoam, "*MAT_BLATZ-KO_FOAM")
REGISTER_KEYWORD(MatBlatzKoFoam, "*MAT_038")

// ============================================================================
// MatFldTransverselyAnisotropic (*MAT_039)
// ============================================================================

bool MatFldTransverselyAnisotropic::parse(const std::vector<std::string>& lines,
                                           util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);

        size_t pos = 0;
        data_.mid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data_.ro = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.e = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.pr = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.sigy = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.etan = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.r = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.hlcid = static_cast<int>(parser.getInt64At(pos).value_or(0));

        break;
    }

    return true;
}

std::vector<std::string> MatFldTransverselyAnisotropic::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.mid);
    writer.writeDouble(data_.ro);
    writer.writeDouble(data_.e);
    writer.writeDouble(data_.pr);
    writer.writeDouble(data_.sigy);
    writer.writeDouble(data_.etan);
    writer.writeDouble(data_.r);
    writer.writeInt(data_.hlcid);
    result.push_back(writer.getLine());

    return result;
}

void MatFldTransverselyAnisotropic::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(MatFldTransverselyAnisotropic, "*MAT_FLD_TRANSVERSELY_ANISOTROPIC")
REGISTER_KEYWORD(MatFldTransverselyAnisotropic, "*MAT_039")

// ============================================================================
// MatNonlinearOrthotropic (*MAT_040)
// ============================================================================

bool MatNonlinearOrthotropic::parse(const std::vector<std::string>& lines,
                                     util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t cardNum = 0;
    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);

        if (cardNum == 0) {
            size_t pos = 0;
            data_.mid = parser.getInt64At(pos).value_or(0);
            pos += intW;
            data_.ro = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.ea = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.eb = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.ec = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.prba = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.prca = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.prcb = parser.getDoubleAt(pos, realW).value_or(0.0);
        } else if (cardNum == 1) {
            size_t pos = 0;
            data_.gab = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.gbc = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.gca = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.aopt = parser.getDoubleAt(pos, realW).value_or(0.0);
        }

        cardNum++;
        if (cardNum >= 2) break;
    }

    return true;
}

std::vector<std::string> MatNonlinearOrthotropic::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.mid);
    writer.writeDouble(data_.ro);
    writer.writeDouble(data_.ea);
    writer.writeDouble(data_.eb);
    writer.writeDouble(data_.ec);
    writer.writeDouble(data_.prba);
    writer.writeDouble(data_.prca);
    writer.writeDouble(data_.prcb);
    result.push_back(writer.getLine());

    writer.clear();
    writer.writeDouble(data_.gab);
    writer.writeDouble(data_.gbc);
    writer.writeDouble(data_.gca);
    writer.writeDouble(data_.aopt);
    result.push_back(writer.getLine());

    return result;
}

void MatNonlinearOrthotropic::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(MatNonlinearOrthotropic, "*MAT_NONLINEAR_ORTHOTROPIC")
REGISTER_KEYWORD(MatNonlinearOrthotropic, "*MAT_040")

// ============================================================================
// MatBamman (*MAT_051)
// ============================================================================

bool MatBamman::parse(const std::vector<std::string>& lines,
                       util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t cardNum = 0;
    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);

        if (cardNum == 0) {
            size_t pos = 0;
            data_.mid = parser.getInt64At(pos).value_or(0);
            pos += intW;
            data_.ro = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.e = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.pr = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.t = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.hc = parser.getDoubleAt(pos, realW).value_or(0.0);
        } else if (cardNum == 1) {
            size_t pos = 0;
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
        }

        cardNum++;
        if (cardNum >= 2) break;
    }

    return true;
}

std::vector<std::string> MatBamman::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.mid);
    writer.writeDouble(data_.ro);
    writer.writeDouble(data_.e);
    writer.writeDouble(data_.pr);
    writer.writeDouble(data_.t);
    writer.writeDouble(data_.hc);
    result.push_back(writer.getLine());

    writer.clear();
    writer.writeDouble(data_.c1);
    writer.writeDouble(data_.c2);
    writer.writeDouble(data_.c3);
    writer.writeDouble(data_.c4);
    writer.writeDouble(data_.c5);
    writer.writeDouble(data_.c6);
    result.push_back(writer.getLine());

    return result;
}

void MatBamman::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(MatBamman, "*MAT_BAMMAN")
REGISTER_KEYWORD(MatBamman, "*MAT_051")

// ============================================================================
// MatBammanDamage (*MAT_052)
// ============================================================================

bool MatBammanDamage::parse(const std::vector<std::string>& lines,
                             util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t cardNum = 0;
    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);

        if (cardNum == 0) {
            size_t pos = 0;
            data_.mid = parser.getInt64At(pos).value_or(0);
            pos += intW;
            data_.ro = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.e = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.pr = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.t = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.hc = parser.getDoubleAt(pos, realW).value_or(0.0);
        } else if (cardNum == 1) {
            size_t pos = 0;
            data_.d1 = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.d2 = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.d3 = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.phic = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.spall = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.fail = parser.getDoubleAt(pos, realW).value_or(0.0);
        }

        cardNum++;
        if (cardNum >= 2) break;
    }

    return true;
}

std::vector<std::string> MatBammanDamage::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.mid);
    writer.writeDouble(data_.ro);
    writer.writeDouble(data_.e);
    writer.writeDouble(data_.pr);
    writer.writeDouble(data_.t);
    writer.writeDouble(data_.hc);
    result.push_back(writer.getLine());

    writer.clear();
    writer.writeDouble(data_.d1);
    writer.writeDouble(data_.d2);
    writer.writeDouble(data_.d3);
    writer.writeDouble(data_.phic);
    writer.writeDouble(data_.spall);
    writer.writeDouble(data_.fail);
    result.push_back(writer.getLine());

    return result;
}

void MatBammanDamage::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(MatBammanDamage, "*MAT_BAMMAN_DAMAGE")
REGISTER_KEYWORD(MatBammanDamage, "*MAT_052")

// ============================================================================
// MatClosedCellFoam (*MAT_053)
// ============================================================================

bool MatClosedCellFoam::parse(const std::vector<std::string>& lines,
                               util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t cardNum = 0;
    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);

        if (cardNum == 0) {
            size_t pos = 0;
            data_.mid = parser.getInt64At(pos).value_or(0);
            pos += intW;
            data_.ro = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.e = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.lcid = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.tc = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.hu = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.beta = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.damp = parser.getDoubleAt(pos, realW).value_or(0.0);
        } else if (cardNum == 1) {
            size_t pos = 0;
            data_.shape = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.fail = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.bvflag = parser.getDoubleAt(pos, realW).value_or(0.0);
        }

        cardNum++;
        if (cardNum >= 2) break;
    }

    return true;
}

std::vector<std::string> MatClosedCellFoam::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.mid);
    writer.writeDouble(data_.ro);
    writer.writeDouble(data_.e);
    writer.writeDouble(data_.lcid);
    writer.writeDouble(data_.tc);
    writer.writeDouble(data_.hu);
    writer.writeDouble(data_.beta);
    writer.writeDouble(data_.damp);
    result.push_back(writer.getLine());

    writer.clear();
    writer.writeDouble(data_.shape);
    writer.writeDouble(data_.fail);
    writer.writeDouble(data_.bvflag);
    result.push_back(writer.getLine());

    return result;
}

void MatClosedCellFoam::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(MatClosedCellFoam, "*MAT_CLOSED_CELL_FOAM")
REGISTER_KEYWORD(MatClosedCellFoam, "*MAT_053")

// ============================================================================
// MatRateSensitiveCompositeFabric (*MAT_058)
// ============================================================================

bool MatRateSensitiveCompositeFabric::parse(const std::vector<std::string>& lines,
                                             util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);

        size_t pos = 0;
        data_.mid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data_.ro = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.ea = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.eb = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.prba = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.gab = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.gbc = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.gca = parser.getDoubleAt(pos, realW).value_or(0.0);

        break;
    }

    return true;
}

std::vector<std::string> MatRateSensitiveCompositeFabric::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.mid);
    writer.writeDouble(data_.ro);
    writer.writeDouble(data_.ea);
    writer.writeDouble(data_.eb);
    writer.writeDouble(data_.prba);
    writer.writeDouble(data_.gab);
    writer.writeDouble(data_.gbc);
    writer.writeDouble(data_.gca);
    result.push_back(writer.getLine());

    return result;
}

void MatRateSensitiveCompositeFabric::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(MatRateSensitiveCompositeFabric, "*MAT_RATE_SENSITIVE_COMPOSITE_FABRIC")
REGISTER_KEYWORD(MatRateSensitiveCompositeFabric, "*MAT_058")

// ============================================================================
// MatCompositeFailureSolidModel (*MAT_059)
// ============================================================================

bool MatCompositeFailureSolidModel::parse(const std::vector<std::string>& lines,
                                           util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t cardNum = 0;
    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);

        if (cardNum == 0) {
            size_t pos = 0;
            data_.mid = parser.getInt64At(pos).value_or(0);
            pos += intW;
            data_.ro = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.ea = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.eb = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.ec = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.prba = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.prca = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.prcb = parser.getDoubleAt(pos, realW).value_or(0.0);
        } else if (cardNum == 1) {
            size_t pos = 0;
            data_.gab = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.gbc = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.gca = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.aopt = parser.getDoubleAt(pos, realW).value_or(0.0);
        }

        cardNum++;
        if (cardNum >= 2) break;
    }

    return true;
}

std::vector<std::string> MatCompositeFailureSolidModel::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.mid);
    writer.writeDouble(data_.ro);
    writer.writeDouble(data_.ea);
    writer.writeDouble(data_.eb);
    writer.writeDouble(data_.ec);
    writer.writeDouble(data_.prba);
    writer.writeDouble(data_.prca);
    writer.writeDouble(data_.prcb);
    result.push_back(writer.getLine());

    writer.clear();
    writer.writeDouble(data_.gab);
    writer.writeDouble(data_.gbc);
    writer.writeDouble(data_.gca);
    writer.writeDouble(data_.aopt);
    result.push_back(writer.getLine());

    return result;
}

void MatCompositeFailureSolidModel::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(MatCompositeFailureSolidModel, "*MAT_COMPOSITE_FAILURE_SOLID_MODEL")
REGISTER_KEYWORD(MatCompositeFailureSolidModel, "*MAT_059")

// ============================================================================
// MatElasticWithViscosity (*MAT_060)
// ============================================================================

bool MatElasticWithViscosity::parse(const std::vector<std::string>& lines,
                                     util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);

        size_t pos = 0;
        data_.mid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data_.ro = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.k = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.g0 = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.gi = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.beta = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.ref = parser.getDoubleAt(pos, realW).value_or(0.0);

        break;
    }

    return true;
}

std::vector<std::string> MatElasticWithViscosity::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.mid);
    writer.writeDouble(data_.ro);
    writer.writeDouble(data_.k);
    writer.writeDouble(data_.g0);
    writer.writeDouble(data_.gi);
    writer.writeDouble(data_.beta);
    writer.writeDouble(data_.ref);
    result.push_back(writer.getLine());

    return result;
}

void MatElasticWithViscosity::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(MatElasticWithViscosity, "*MAT_ELASTIC_WITH_VISCOSITY")
REGISTER_KEYWORD(MatElasticWithViscosity, "*MAT_060")

// ============================================================================
// MatKelvinMaxwellViscoelastic (*MAT_061)
// ============================================================================

bool MatKelvinMaxwellViscoelastic::parse(const std::vector<std::string>& lines,
                                          util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);

        size_t pos = 0;
        data_.mid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data_.ro = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.bulk = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.g0 = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.gi = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.beta = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.dc = parser.getDoubleAt(pos, realW).value_or(0.0);

        break;
    }

    return true;
}

std::vector<std::string> MatKelvinMaxwellViscoelastic::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.mid);
    writer.writeDouble(data_.ro);
    writer.writeDouble(data_.bulk);
    writer.writeDouble(data_.g0);
    writer.writeDouble(data_.gi);
    writer.writeDouble(data_.beta);
    writer.writeDouble(data_.dc);
    result.push_back(writer.getLine());

    return result;
}

void MatKelvinMaxwellViscoelastic::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(MatKelvinMaxwellViscoelastic, "*MAT_KELVIN-MAXWELL_VISCOELASTIC")
REGISTER_KEYWORD(MatKelvinMaxwellViscoelastic, "*MAT_061")

// ============================================================================
// MatViscousFoam (*MAT_062)
// ============================================================================

bool MatViscousFoam::parse(const std::vector<std::string>& lines,
                            util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);

        size_t pos = 0;
        data_.mid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data_.ro = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.e = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.tc = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.hu = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.beta = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.damp = parser.getDoubleAt(pos, realW).value_or(0.0);

        break;
    }

    return true;
}

std::vector<std::string> MatViscousFoam::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.mid);
    writer.writeDouble(data_.ro);
    writer.writeDouble(data_.e);
    writer.writeInt(data_.lcid);
    writer.writeDouble(data_.tc);
    writer.writeDouble(data_.hu);
    writer.writeDouble(data_.beta);
    writer.writeDouble(data_.damp);
    result.push_back(writer.getLine());

    return result;
}

void MatViscousFoam::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(MatViscousFoam, "*MAT_VISCOUS_FOAM")
REGISTER_KEYWORD(MatViscousFoam, "*MAT_062")

// ============================================================================
// MatViscoelasticThermal (*MAT_064)
// ============================================================================

bool MatViscoelasticThermal::parse(const std::vector<std::string>& lines,
                                    util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);

        size_t pos = 0;
        data_.mid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data_.ro = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.bulk = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.g0 = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.gi = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.beta = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.lcte = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.tref = parser.getDoubleAt(pos, realW).value_or(0.0);

        break;
    }

    return true;
}

std::vector<std::string> MatViscoelasticThermal::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.mid);
    writer.writeDouble(data_.ro);
    writer.writeDouble(data_.bulk);
    writer.writeDouble(data_.g0);
    writer.writeDouble(data_.gi);
    writer.writeDouble(data_.beta);
    writer.writeInt(data_.lcte);
    writer.writeDouble(data_.tref);
    result.push_back(writer.getLine());

    return result;
}

void MatViscoelasticThermal::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(MatViscoelasticThermal, "*MAT_VISCOELASTIC_THERMAL")
REGISTER_KEYWORD(MatViscoelasticThermal, "*MAT_064")

// ============================================================================
// MatBilkhuDuboisFoam (*MAT_075)
// ============================================================================

bool MatBilkhuDuboisFoam::parse(const std::vector<std::string>& lines,
                                 util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);

        size_t pos = 0;
        data_.mid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data_.ro = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.e = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.pr = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.tc = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.damp = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.lcidr = parser.getDoubleAt(pos, realW).value_or(0.0);

        break;
    }

    return true;
}

std::vector<std::string> MatBilkhuDuboisFoam::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.mid);
    writer.writeDouble(data_.ro);
    writer.writeDouble(data_.e);
    writer.writeDouble(data_.pr);
    writer.writeInt(data_.lcid);
    writer.writeDouble(data_.tc);
    writer.writeDouble(data_.damp);
    writer.writeDouble(data_.lcidr);
    result.push_back(writer.getLine());

    return result;
}

void MatBilkhuDuboisFoam::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(MatBilkhuDuboisFoam, "*MAT_BILKHU_DUBOIS_FOAM")
REGISTER_KEYWORD(MatBilkhuDuboisFoam, "*MAT_075")

// ============================================================================
// MatGeneralViscoelastic (*MAT_076)
// ============================================================================

bool MatGeneralViscoelastic::parse(const std::vector<std::string>& lines,
                                    util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);

        size_t pos = 0;
        data_.mid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data_.ro = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.bulk = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.lcg = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.lck = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.nt = parser.getDoubleAt(pos, realW).value_or(0.0);

        break;
    }

    return true;
}

std::vector<std::string> MatGeneralViscoelastic::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.mid);
    writer.writeDouble(data_.ro);
    writer.writeDouble(data_.bulk);
    writer.writeInt(data_.lcg);
    writer.writeInt(data_.lck);
    writer.writeDouble(data_.nt);
    result.push_back(writer.getLine());

    return result;
}

void MatGeneralViscoelastic::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(MatGeneralViscoelastic, "*MAT_GENERAL_VISCOELASTIC")
REGISTER_KEYWORD(MatGeneralViscoelastic, "*MAT_076")

// ============================================================================
// MatPlasticityWithDamageOrtho (*MAT_082)
// ============================================================================

bool MatPlasticityWithDamageOrtho::parse(const std::vector<std::string>& lines,
                                          util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);

        size_t pos = 0;
        data_.mid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data_.ro = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.ea = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.eb = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.ec = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.prba = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.prca = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.prcb = parser.getDoubleAt(pos, realW).value_or(0.0);

        break;
    }

    return true;
}

std::vector<std::string> MatPlasticityWithDamageOrtho::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.mid);
    writer.writeDouble(data_.ro);
    writer.writeDouble(data_.ea);
    writer.writeDouble(data_.eb);
    writer.writeDouble(data_.ec);
    writer.writeDouble(data_.prba);
    writer.writeDouble(data_.prca);
    writer.writeDouble(data_.prcb);
    result.push_back(writer.getLine());

    return result;
}

void MatPlasticityWithDamageOrtho::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(MatPlasticityWithDamageOrtho, "*MAT_PLASTICITY_WITH_DAMAGE_ORTHO")
REGISTER_KEYWORD(MatPlasticityWithDamageOrtho, "*MAT_082")

// ============================================================================
// MatPiecewiseLinearPlasticityStochastic (*MAT_089)
// ============================================================================

bool MatPiecewiseLinearPlasticityStochastic::parse(const std::vector<std::string>& lines,
                                                    util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t cardNum = 0;
    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);

        if (cardNum == 0) {
            size_t pos = 0;
            data_.mid = parser.getInt64At(pos).value_or(0);
            pos += intW;
            data_.ro = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.e = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.pr = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.sigy = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.etan = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.fail = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.tdel = parser.getDoubleAt(pos, realW).value_or(0.0);
        } else if (cardNum == 1) {
            size_t pos = 0;
            data_.c = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.p = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.lcss = static_cast<int>(parser.getInt64At(pos).value_or(0));
        }

        cardNum++;
        if (cardNum >= 2) break;
    }

    return true;
}

std::vector<std::string> MatPiecewiseLinearPlasticityStochastic::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.mid);
    writer.writeDouble(data_.ro);
    writer.writeDouble(data_.e);
    writer.writeDouble(data_.pr);
    writer.writeDouble(data_.sigy);
    writer.writeDouble(data_.etan);
    writer.writeDouble(data_.fail);
    writer.writeDouble(data_.tdel);
    result.push_back(writer.getLine());

    writer.clear();
    writer.writeDouble(data_.c);
    writer.writeDouble(data_.p);
    writer.writeInt(data_.lcss);
    result.push_back(writer.getLine());

    return result;
}

void MatPiecewiseLinearPlasticityStochastic::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(MatPiecewiseLinearPlasticityStochastic, "*MAT_PIECEWISE_LINEAR_PLASTICITY_STOCHASTIC")
REGISTER_KEYWORD(MatPiecewiseLinearPlasticityStochastic, "*MAT_089")

// ============================================================================
// MatAcoustic (*MAT_090)
// ============================================================================

bool MatAcoustic::parse(const std::vector<std::string>& lines,
                         util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);

        size_t pos = 0;
        data_.mid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data_.ro = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.c = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.beta = parser.getDoubleAt(pos, realW).value_or(0.0);

        break;
    }

    return true;
}

std::vector<std::string> MatAcoustic::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.mid);
    writer.writeDouble(data_.ro);
    writer.writeDouble(data_.c);
    writer.writeDouble(data_.beta);
    result.push_back(writer.getLine());

    return result;
}

void MatAcoustic::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(MatAcoustic, "*MAT_ACOUSTIC")
REGISTER_KEYWORD(MatAcoustic, "*MAT_090")

// ============================================================================
// MatSoftTissue (*MAT_091)
// ============================================================================

bool MatSoftTissue::parse(const std::vector<std::string>& lines,
                           util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);

        size_t pos = 0;
        data_.mid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data_.ro = parser.getDoubleAt(pos, realW).value_or(0.0);
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
        data_.xk = parser.getDoubleAt(pos, realW).value_or(0.0);

        break;
    }

    return true;
}

std::vector<std::string> MatSoftTissue::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.mid);
    writer.writeDouble(data_.ro);
    writer.writeDouble(data_.c1);
    writer.writeDouble(data_.c2);
    writer.writeDouble(data_.c3);
    writer.writeDouble(data_.c4);
    writer.writeDouble(data_.c5);
    writer.writeDouble(data_.xk);
    result.push_back(writer.getLine());

    return result;
}

void MatSoftTissue::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(MatSoftTissue, "*MAT_SOFT_TISSUE")
REGISTER_KEYWORD(MatSoftTissue, "*MAT_091")

// ============================================================================
// MatArrudaBoyce (*MAT_127)
// ============================================================================

bool MatArrudaBoyce::parse(const std::vector<std::string>& lines,
                            util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);

        size_t pos = 0;
        data_.mid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data_.ro = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.k = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.mu = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.n = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.ref = parser.getDoubleAt(pos, realW).value_or(0.0);

        break;
    }

    return true;
}

std::vector<std::string> MatArrudaBoyce::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.mid);
    writer.writeDouble(data_.ro);
    writer.writeDouble(data_.k);
    writer.writeDouble(data_.mu);
    writer.writeDouble(data_.n);
    writer.writeDouble(data_.ref);
    result.push_back(writer.getLine());

    return result;
}

void MatArrudaBoyce::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(MatArrudaBoyce, "*MAT_ARRUDA_BOYCE_RUBBER")
REGISTER_KEYWORD(MatArrudaBoyce, "*MAT_127")

// ============================================================================
// MatSimplifiedRubber (*MAT_181)
// ============================================================================

bool MatSimplifiedRubber::parse(const std::vector<std::string>& lines,
                                 util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);

        size_t pos = 0;
        data_.mid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data_.ro = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.km = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.mu = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.g = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.sigf = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.ref = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.prten = parser.getDoubleAt(pos, realW).value_or(0.0);

        break;
    }

    return true;
}

std::vector<std::string> MatSimplifiedRubber::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.mid);
    writer.writeDouble(data_.ro);
    writer.writeDouble(data_.km);
    writer.writeDouble(data_.mu);
    writer.writeDouble(data_.g);
    writer.writeInt(data_.sigf);
    writer.writeDouble(data_.ref);
    writer.writeDouble(data_.prten);
    result.push_back(writer.getLine());

    return result;
}

void MatSimplifiedRubber::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(MatSimplifiedRubber, "*MAT_SIMPLIFIED_RUBBER")
REGISTER_KEYWORD(MatSimplifiedRubber, "*MAT_SIMPLIFIED_RUBBER_WITH_DAMAGE")
REGISTER_KEYWORD(MatSimplifiedRubber, "*MAT_181")

// ============================================================================
// MatArupAdhesive (*MAT_169)
// ============================================================================

bool MatArupAdhesive::parse(const std::vector<std::string>& lines,
                             util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);
        size_t pos = 0;

        data_.mid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data_.ro = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.e = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.pr = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.etan = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.sigy = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.fail = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.gic = parser.getDoubleAt(pos, realW).value_or(0.0);

        break;
    }

    return true;
}

std::vector<std::string> MatArupAdhesive::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.mid);
    writer.writeDouble(data_.ro);
    writer.writeDouble(data_.e);
    writer.writeDouble(data_.pr);
    writer.writeDouble(data_.etan);
    writer.writeDouble(data_.sigy);
    writer.writeDouble(data_.fail);
    writer.writeDouble(data_.gic);
    result.push_back(writer.getLine());

    return result;
}

void MatArupAdhesive::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(MatArupAdhesive, "*MAT_ARUP_ADHESIVE")
REGISTER_KEYWORD(MatArupAdhesive, "*MAT_169")

// ============================================================================
// MatCohesiveGeneral (*MAT_184)
// ============================================================================

bool MatCohesiveGeneral::parse(const std::vector<std::string>& lines,
                                util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);
        size_t pos = 0;

        data_.mid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data_.ro = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.roflg = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.intfail = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.ten = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.she = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.psr = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.gic = parser.getDoubleAt(pos, realW).value_or(0.0);

        break;
    }

    return true;
}

std::vector<std::string> MatCohesiveGeneral::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.mid);
    writer.writeDouble(data_.ro);
    writer.writeInt(data_.roflg);
    writer.writeInt(data_.intfail);
    writer.writeDouble(data_.ten);
    writer.writeDouble(data_.she);
    writer.writeDouble(data_.psr);
    writer.writeDouble(data_.gic);
    result.push_back(writer.getLine());

    return result;
}

void MatCohesiveGeneral::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(MatCohesiveGeneral, "*MAT_COHESIVE_GENERAL")
REGISTER_KEYWORD(MatCohesiveGeneral, "*MAT_184")

// ============================================================================
// MatCohesiveElastic (*MAT_185)
// ============================================================================

bool MatCohesiveElastic::parse(const std::vector<std::string>& lines,
                                util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);
        size_t pos = 0;

        data_.mid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data_.ro = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.en = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.et = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.gic = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.giic = parser.getDoubleAt(pos, realW).value_or(0.0);

        break;
    }

    return true;
}

std::vector<std::string> MatCohesiveElastic::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.mid);
    writer.writeDouble(data_.ro);
    writer.writeDouble(data_.en);
    writer.writeDouble(data_.et);
    writer.writeDouble(data_.gic);
    writer.writeDouble(data_.giic);
    result.push_back(writer.getLine());

    return result;
}

void MatCohesiveElastic::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(MatCohesiveElastic, "*MAT_COHESIVE_ELASTIC")
REGISTER_KEYWORD(MatCohesiveElastic, "*MAT_185")

// ============================================================================
// MatTabulatedJohnsonCook (*MAT_224)
// ============================================================================

bool MatTabulatedJohnsonCook::parse(const std::vector<std::string>& lines,
                                     util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);
        size_t pos = 0;

        data_.mid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data_.ro = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.e = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.pr = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.cp = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.tr = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.beta = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.numint = static_cast<int>(parser.getInt64At(pos).value_or(0));

        break;
    }

    return true;
}

std::vector<std::string> MatTabulatedJohnsonCook::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.mid);
    writer.writeDouble(data_.ro);
    writer.writeDouble(data_.e);
    writer.writeDouble(data_.pr);
    writer.writeDouble(data_.cp);
    writer.writeDouble(data_.tr);
    writer.writeDouble(data_.beta);
    writer.writeInt(data_.numint);
    result.push_back(writer.getLine());

    return result;
}

void MatTabulatedJohnsonCook::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(MatTabulatedJohnsonCook, "*MAT_TABULATED_JOHNSON_COOK")
REGISTER_KEYWORD(MatTabulatedJohnsonCook, "*MAT_224")

// ============================================================================
// MatAnisotropicViscoplastic (*MAT_103)
// ============================================================================

bool MatAnisotropicViscoplastic::parse(const std::vector<std::string>& lines,
                                        util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);
        size_t pos = 0;

        data_.mid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data_.ro = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.e = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.pr = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.sigy = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.r00 = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.r45 = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.r90 = parser.getDoubleAt(pos, realW).value_or(0.0);

        break;
    }

    return true;
}

std::vector<std::string> MatAnisotropicViscoplastic::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.mid);
    writer.writeDouble(data_.ro);
    writer.writeDouble(data_.e);
    writer.writeDouble(data_.pr);
    writer.writeDouble(data_.sigy);
    writer.writeDouble(data_.r00);
    writer.writeDouble(data_.r45);
    writer.writeDouble(data_.r90);
    result.push_back(writer.getLine());

    return result;
}

void MatAnisotropicViscoplastic::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(MatAnisotropicViscoplastic, "*MAT_ANISOTROPIC_VISCOPLASTIC")
REGISTER_KEYWORD(MatAnisotropicViscoplastic, "*MAT_103")

// ============================================================================
// MatDamage3 (*MAT_153)
// ============================================================================

bool MatDamage3::parse(const std::vector<std::string>& lines,
                        util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);
        size_t pos = 0;

        data_.mid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data_.ro = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.e = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.pr = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.lcss = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.d1 = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.d2 = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.d3 = parser.getDoubleAt(pos, realW).value_or(0.0);

        break;
    }

    return true;
}

std::vector<std::string> MatDamage3::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.mid);
    writer.writeDouble(data_.ro);
    writer.writeDouble(data_.e);
    writer.writeDouble(data_.pr);
    writer.writeInt(data_.lcss);
    writer.writeDouble(data_.d1);
    writer.writeDouble(data_.d2);
    writer.writeDouble(data_.d3);
    result.push_back(writer.getLine());

    return result;
}

void MatDamage3::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(MatDamage3, "*MAT_DAMAGE_3")
REGISTER_KEYWORD(MatDamage3, "*MAT_153")

// ============================================================================
// MatSeismicIsolator (*MAT_197)
// ============================================================================

bool MatSeismicIsolator::parse(const std::vector<std::string>& lines,
                                util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);
        size_t pos = 0;

        data_.mid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data_.ro = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.ky = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.kd = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.fy = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.alpha = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.mu = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.td = parser.getDoubleAt(pos, realW).value_or(0.0);

        break;
    }

    return true;
}

std::vector<std::string> MatSeismicIsolator::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.mid);
    writer.writeDouble(data_.ro);
    writer.writeDouble(data_.ky);
    writer.writeDouble(data_.kd);
    writer.writeDouble(data_.fy);
    writer.writeDouble(data_.alpha);
    writer.writeDouble(data_.mu);
    writer.writeDouble(data_.td);
    result.push_back(writer.getLine());

    return result;
}

void MatSeismicIsolator::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(MatSeismicIsolator, "*MAT_SEISMIC_ISOLATOR")
REGISTER_KEYWORD(MatSeismicIsolator, "*MAT_197")

// ============================================================================
// MatSpringInelastic
// ============================================================================

bool MatSpringInelastic::parse(const std::vector<std::string>& lines,
                                util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);
        size_t pos = 0;

        data_.mid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data_.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.lcu = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.beta = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.ty = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.cy = parser.getDoubleAt(pos, realW).value_or(0.0);

        break;
    }

    return true;
}

std::vector<std::string> MatSpringInelastic::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.mid);
    writer.writeInt(data_.lcid);
    writer.writeInt(data_.lcu);
    writer.writeDouble(data_.beta);
    writer.writeDouble(data_.ty);
    writer.writeDouble(data_.cy);
    result.push_back(writer.getLine());

    return result;
}

void MatSpringInelastic::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(MatSpringInelastic, "*MAT_SPRING_INELASTIC")

// ============================================================================
// MatDamperNonlinearViscous
// ============================================================================

bool MatDamperNonlinearViscous::parse(const std::vector<std::string>& lines,
                                       util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);
        size_t pos = 0;

        data_.mid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data_.lcdr = static_cast<int>(parser.getInt64At(pos).value_or(0));

        break;
    }

    return true;
}

std::vector<std::string> MatDamperNonlinearViscous::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.mid);
    writer.writeInt(data_.lcdr);
    result.push_back(writer.getLine());

    return result;
}

void MatDamperNonlinearViscous::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(MatDamperNonlinearViscous, "*MAT_DAMPER_NONLINEAR_VISCOUS")

// ============================================================================
// MatHystereticBeam
// ============================================================================

bool MatHystereticBeam::parse(const std::vector<std::string>& lines,
                               util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);
        size_t pos = 0;

        data_.mid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data_.ro = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.e = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.pr = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.lcpms = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.lcpma = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.lcnms = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.lcnma = static_cast<int>(parser.getInt64At(pos).value_or(0));

        break;
    }

    return true;
}

std::vector<std::string> MatHystereticBeam::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.mid);
    writer.writeDouble(data_.ro);
    writer.writeDouble(data_.e);
    writer.writeDouble(data_.pr);
    writer.writeInt(data_.lcpms);
    writer.writeInt(data_.lcpma);
    writer.writeInt(data_.lcnms);
    writer.writeInt(data_.lcnma);
    result.push_back(writer.getLine());

    return result;
}

void MatHystereticBeam::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(MatHystereticBeam, "*MAT_HYSTERETIC_BEAM")

} // namespace koo::dyna
