#include <koo/dyna/Section.hpp>
#include <koo/dyna/ModelVisitor.hpp>
#include <koo/dyna/KeywordFactory.hpp>
#include <koo/util/StringUtils.hpp>

namespace koo::dyna {

// SectionShell implementation
bool SectionShell::parse(const std::vector<std::string>& lines,
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
            // Card 1: SECID(I), ELFORM(I), SHRF(E), NIP(I), PROPT(E), QR/IRID(I), ICOMP(I), SETYP(E)
            size_t pos = 0;
            secid_ = parser.getInt64At(pos).value_or(0);
            pos += intW;
            elform_ = static_cast<int>(parser.getInt64At(pos).value_or(2));
            pos += intW;
            shrf_ = parser.getDoubleAt(pos, realW).value_or(1.0);
            pos += realW;
            nip_ = static_cast<int>(parser.getInt64At(pos).value_or(2));
            pos += intW;
            propt_ = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            qr_irid_ = static_cast<int>(parser.getInt64At(pos).value_or(0));
            pos += intW;
            icomp_ = static_cast<int>(parser.getInt64At(pos).value_or(0));
            pos += intW;
            setyp_ = parser.getDoubleAt(pos, realW).value_or(1.0);
        } else if (cardNum == 1) {
            // Card 2: T1(E), T2(E), T3(E), T4(E), NLOC(E)
            size_t pos = 0;
            t1_ = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            t2_ = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            t3_ = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            t4_ = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            nloc_ = parser.getDoubleAt(pos, realW).value_or(0.0);
        }

        cardNum++;
        if (cardNum >= 2) break;
    }

    return true;
}

std::vector<std::string> SectionShell::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    // Card 1
    writer.writeInt(secid_);
    writer.writeInt(elform_);
    writer.writeDouble(shrf_);
    writer.writeInt(nip_);
    writer.writeDouble(propt_);
    writer.writeInt(qr_irid_);
    writer.writeInt(icomp_);
    writer.writeDouble(setyp_);
    result.push_back(writer.getLine());

    // Card 2
    writer.clear();
    writer.writeDouble(t1_);
    writer.writeDouble(t2_);
    writer.writeDouble(t3_);
    writer.writeDouble(t4_);
    writer.writeDouble(nloc_);
    result.push_back(writer.getLine());

    return result;
}

void SectionShell::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

// SectionSolid implementation
bool SectionSolid::parse(const std::vector<std::string>& lines,
                         util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();  // Always 10

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);

        // Card 1: SECID(I), ELFORM(I), AET(I) - all integers
        size_t pos = 0;
        secid_ = parser.getInt64At(pos).value_or(0);
        pos += intW;
        elform_ = static_cast<int>(parser.getInt64At(pos).value_or(1));
        pos += intW;
        aet_ = static_cast<int>(parser.getInt64At(pos).value_or(0));

        break; // Only one card for basic SECTION_SOLID
    }

    return true;
}

std::vector<std::string> SectionSolid::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(secid_);
    writer.writeInt(elform_);
    writer.writeInt(aet_);

    result.push_back(writer.getLine());
    return result;
}

void SectionSolid::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

// ============================================================================
// SectionBeam
// ============================================================================

bool SectionBeam::parse(const std::vector<std::string>& lines,
                        util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    // Skip comments
    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: SECID, ELFORM, SHRF, QR/IRID, CST, SCOOR
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    secid_ = parser.getInt64At(pos).value_or(0);
    pos += intW;
    elform_ = static_cast<int>(parser.getInt64At(pos).value_or(1));
    pos += intW;
    shrf_ = parser.getDoubleAt(pos, realW).value_or(1.0);
    pos += realW;
    qr_irid_ = static_cast<int>(parser.getInt64At(pos).value_or(2));
    pos += intW;
    cst_ = static_cast<int>(parser.getInt64At(pos).value_or(1));
    pos += intW;
    scoor_ = parser.getDoubleAt(pos, realW).value_or(0.0);
    ++lineIdx;

    // Skip comments
    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 2 depends on CST (cross section type)
    parser.setLine(lines[lineIdx]);
    pos = 0;

    // For CST=1 (tubular): TS1, TS2, TT1, TT2, NSLOC, NTLOC
    cs_.ts1 = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    cs_.ts2 = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    cs_.tt1 = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    cs_.tt2 = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    cs_.nsloc = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    cs_.ntloc = parser.getDoubleAt(pos, realW).value_or(0.0);

    return true;
}

std::vector<std::string> SectionBeam::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    // Card 1
    writer.writeInt(secid_);
    writer.writeInt(elform_);
    writer.writeDouble(shrf_);
    writer.writeInt(qr_irid_);
    writer.writeInt(cst_);
    writer.writeDouble(scoor_);
    result.push_back(writer.getLine());

    // Card 2 (for CST=1)
    writer.clear();
    writer.writeDouble(cs_.ts1);
    writer.writeDouble(cs_.ts2);
    writer.writeDouble(cs_.tt1);
    writer.writeDouble(cs_.tt2);
    writer.writeDouble(cs_.nsloc);
    writer.writeDouble(cs_.ntloc);
    result.push_back(writer.getLine());

    return result;
}

void SectionBeam::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

// ============================================================================
// SectionDiscrete
// ============================================================================

bool SectionDiscrete::parse(const std::vector<std::string>& lines,
                            util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: SECID, DRO, KD, V0, CL, FD, CDL, TDL
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    secid_ = parser.getInt64At(pos).value_or(0);
    pos += intW;
    dro_ = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    kd_ = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    v0_ = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    cl_ = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    fd_ = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    cdl_ = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    tdl_ = static_cast<int>(parser.getInt64At(pos).value_or(0));

    return true;
}

std::vector<std::string> SectionDiscrete::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(secid_);
    writer.writeInt(dro_);
    writer.writeInt(kd_);
    writer.writeInt(v0_);
    writer.writeDouble(cl_);
    writer.writeDouble(fd_);
    writer.writeInt(cdl_);
    writer.writeInt(tdl_);
    result.push_back(writer.getLine());

    return result;
}

void SectionDiscrete::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

// ============================================================================
// SectionSeatbelt
// ============================================================================

bool SectionSeatbelt::parse(const std::vector<std::string>& lines,
                            util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: SECID, AREA, THICK
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    secid_ = parser.getInt64At(pos).value_or(0);
    pos += parser.getIntWidth();
    area_ = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    thick_ = parser.getDoubleAt(pos, realW).value_or(0.0);

    return true;
}

std::vector<std::string> SectionSeatbelt::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(secid_);
    writer.writeDouble(area_);
    writer.writeDouble(thick_);
    result.push_back(writer.getLine());

    return result;
}

void SectionSeatbelt::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

// ============================================================================
// SectionTshell
// ============================================================================

bool SectionTshell::parse(const std::vector<std::string>& lines,
                          util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: SECID, ELFORM, SHRF, NIP, PROPT, QR/IRID, ICOMP, TSHEAR
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    secid_ = parser.getInt64At(pos).value_or(0);
    pos += intW;
    elform_ = static_cast<int>(parser.getInt64At(pos).value_or(1));
    pos += intW;
    shrf_ = static_cast<int>(parser.getInt64At(pos).value_or(1));
    pos += intW;
    nip_ = static_cast<int>(parser.getInt64At(pos).value_or(2));
    pos += intW;
    propt_ = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    qr_irid_ = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    icomp_ = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    tshear_ = static_cast<int>(parser.getInt64At(pos).value_or(0));

    return true;
}

std::vector<std::string> SectionTshell::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(secid_);
    writer.writeInt(elform_);
    writer.writeInt(shrf_);
    writer.writeInt(nip_);
    writer.writeInt(propt_);
    writer.writeInt(qr_irid_);
    writer.writeInt(icomp_);
    writer.writeInt(tshear_);
    result.push_back(writer.getLine());

    return result;
}

void SectionTshell::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

// ============================================================================
// SectionAle2d
// ============================================================================

bool SectionAle2d::parse(const std::vector<std::string>& lines,
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

        // Card 1: SECID, ATEFLAG, ELFORM, THICK, NIP, NXDOF, ISAMP
        size_t pos = 0;
        data_.secid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data_.ateflag = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.elform = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.thick = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.nip = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.nxdof = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.isamp = static_cast<int>(parser.getInt64At(pos).value_or(0));

        break;
    }

    return true;
}

std::vector<std::string> SectionAle2d::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.secid);
    writer.writeInt(data_.ateflag);
    writer.writeInt(data_.elform);
    writer.writeDouble(data_.thick);
    writer.writeInt(data_.nip);
    writer.writeInt(data_.nxdof);
    writer.writeInt(data_.isamp);
    result.push_back(writer.getLine());

    return result;
}

void SectionAle2d::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

// ============================================================================
// SectionAle1d
// ============================================================================

bool SectionAle1d::parse(const std::vector<std::string>& lines,
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

        // Card 1: SECID, ELFORM, AREA
        size_t pos = 0;
        data_.secid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data_.elform = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.area = parser.getDoubleAt(pos, realW).value_or(0.0);

        break;
    }

    return true;
}

std::vector<std::string> SectionAle1d::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.secid);
    writer.writeInt(data_.elform);
    writer.writeDouble(data_.area);
    result.push_back(writer.getLine());

    return result;
}

void SectionAle1d::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

// ============================================================================
// SectionShellTitle
// ============================================================================

bool SectionShellTitle::parse(const std::vector<std::string>& lines,
                               util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    // First non-comment line is the title
    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    title_ = util::StringUtils::trim(lines[lineIdx]);
    ++lineIdx;

    int cardNum = 0;
    while (lineIdx < lines.size()) {
        if (util::CardParser::isCommentLine(lines[lineIdx])) {
            ++lineIdx;
            continue;
        }

        parser.setLine(lines[lineIdx]);

        if (cardNum == 0) {
            size_t pos = 0;
            secid_ = parser.getInt64At(pos).value_or(0);
            pos += intW;
            elform_ = static_cast<int>(parser.getInt64At(pos).value_or(2));
            pos += intW;
            shrf_ = parser.getDoubleAt(pos, realW).value_or(1.0);
            pos += realW;
            nip_ = static_cast<int>(parser.getInt64At(pos).value_or(2));
            pos += intW;
            propt_ = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            qr_irid_ = static_cast<int>(parser.getInt64At(pos).value_or(0));
            pos += intW;
            icomp_ = static_cast<int>(parser.getInt64At(pos).value_or(0));
        } else if (cardNum == 1) {
            size_t pos = 0;
            t1_ = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            t2_ = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            t3_ = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            t4_ = parser.getDoubleAt(pos, realW).value_or(0.0);
        }

        ++cardNum;
        ++lineIdx;
        if (cardNum >= 2) break;
    }

    return true;
}

std::vector<std::string> SectionShellTitle::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    result.push_back(title_);

    writer.writeInt(secid_);
    writer.writeInt(elform_);
    writer.writeDouble(shrf_);
    writer.writeInt(nip_);
    writer.writeDouble(propt_);
    writer.writeInt(qr_irid_);
    writer.writeInt(icomp_);
    result.push_back(writer.getLine());

    writer.clear();
    writer.writeDouble(t1_);
    writer.writeDouble(t2_);
    writer.writeDouble(t3_);
    writer.writeDouble(t4_);
    result.push_back(writer.getLine());

    return result;
}

void SectionShellTitle::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

// ============================================================================
// SectionSolidTitle
// ============================================================================

bool SectionSolidTitle::parse(const std::vector<std::string>& lines,
                               util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    size_t lineIdx = 0;

    // First non-comment line is the title
    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    title_ = util::StringUtils::trim(lines[lineIdx]);
    ++lineIdx;

    while (lineIdx < lines.size()) {
        if (util::CardParser::isCommentLine(lines[lineIdx])) {
            ++lineIdx;
            continue;
        }

        parser.setLine(lines[lineIdx]);
        size_t pos = 0;

        secid_ = parser.getInt64At(pos).value_or(0);
        pos += intW;
        elform_ = static_cast<int>(parser.getInt64At(pos).value_or(1));
        pos += intW;
        aet_ = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> SectionSolidTitle::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    result.push_back(title_);

    writer.writeInt(secid_);
    writer.writeInt(elform_);
    writer.writeInt(aet_);
    result.push_back(writer.getLine());

    return result;
}

void SectionSolidTitle::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

// ============================================================================
// SectionBeamTitle
// ============================================================================

bool SectionBeamTitle::parse(const std::vector<std::string>& lines,
                              util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    // First non-comment line is the title
    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    title_ = util::StringUtils::trim(lines[lineIdx]);
    ++lineIdx;

    while (lineIdx < lines.size()) {
        if (util::CardParser::isCommentLine(lines[lineIdx])) {
            ++lineIdx;
            continue;
        }

        parser.setLine(lines[lineIdx]);
        size_t pos = 0;

        secid_ = parser.getInt64At(pos).value_or(0);
        pos += intW;
        elform_ = static_cast<int>(parser.getInt64At(pos).value_or(1));
        pos += intW;
        shrf_ = parser.getDoubleAt(pos, realW).value_or(1.0);
        pos += realW;
        qr_irid_ = static_cast<int>(parser.getInt64At(pos).value_or(2));
        pos += intW;
        cst_ = static_cast<int>(parser.getInt64At(pos).value_or(1));
        pos += intW;
        scoor_ = parser.getDoubleAt(pos, realW).value_or(0.0);
        break;
    }

    return true;
}

std::vector<std::string> SectionBeamTitle::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    result.push_back(title_);

    writer.writeInt(secid_);
    writer.writeInt(elform_);
    writer.writeDouble(shrf_);
    writer.writeInt(qr_irid_);
    writer.writeInt(cst_);
    writer.writeDouble(scoor_);
    result.push_back(writer.getLine());

    return result;
}

void SectionBeamTitle::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

// ============================================================================
// SectionPointSource
// ============================================================================

bool SectionPointSource::parse(const std::vector<std::string>& lines,
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

        // Card 1: SECID, ELFORM, MASS, MOMX, MOMY, MOMZ, ENERGY
        size_t pos = 0;
        data_.secid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data_.elform = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.mass = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.momx = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.momy = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.momz = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.energy = parser.getDoubleAt(pos, realW).value_or(0.0);

        break;
    }

    return true;
}

std::vector<std::string> SectionPointSource::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.secid);
    writer.writeInt(data_.elform);
    writer.writeDouble(data_.mass);
    writer.writeDouble(data_.momx);
    writer.writeDouble(data_.momy);
    writer.writeDouble(data_.momz);
    writer.writeDouble(data_.energy);
    result.push_back(writer.getLine());

    return result;
}

void SectionPointSource::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

// ============================================================================
// SectionPointSourceMixture
// ============================================================================

bool SectionPointSourceMixture::parse(const std::vector<std::string>& lines,
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

        // Card 1: SECID, ELFORM, NGAS, MMASS, GAMMA
        size_t pos = 0;
        data_.secid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data_.elform = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.ngas = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.mmass = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.gamma = parser.getDoubleAt(pos, realW).value_or(1.4);

        break;
    }

    return true;
}

std::vector<std::string> SectionPointSourceMixture::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.secid);
    writer.writeInt(data_.elform);
    writer.writeInt(data_.ngas);
    writer.writeDouble(data_.mmass);
    writer.writeDouble(data_.gamma);
    result.push_back(writer.getLine());

    return result;
}

void SectionPointSourceMixture::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

// ============================================================================
// SectionShellEfg
// ============================================================================

bool SectionShellEfg::parse(const std::vector<std::string>& lines,
                             util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    int lineIdx = 0;
    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);

        if (lineIdx == 0) {
            // Card 1: SECID, ELFORM, SHRF, NIP, PROPT, QR/IRID, ICOMP
            size_t pos = 0;
            data_.secid = parser.getInt64At(pos).value_or(0);
            pos += intW;
            data_.elform = static_cast<int>(parser.getInt64At(pos).value_or(41));
            pos += intW;
            data_.shrf = parser.getDoubleAt(pos, realW).value_or(1.0);
            pos += realW;
            data_.nip = static_cast<int>(parser.getInt64At(pos).value_or(2));
            pos += intW;
            data_.propt = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.qr_irid = static_cast<int>(parser.getInt64At(pos).value_or(0));
            pos += intW;
            data_.icomp = static_cast<int>(parser.getInt64At(pos).value_or(0));
        } else if (lineIdx == 1) {
            // Card 2: T1, T2, T3, T4, NLOC
            size_t pos = 0;
            data_.t1 = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.t2 = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.t3 = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.t4 = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.nloc = parser.getDoubleAt(pos, realW).value_or(0.0);
        } else if (lineIdx == 2) {
            // Card 3: DX, DY, ISPLINE, IDILA, IEBT, IDIM
            size_t pos = 0;
            data_.dx = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.dy = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.ispline = static_cast<int>(parser.getInt64At(pos).value_or(0));
            pos += intW;
            data_.idila = static_cast<int>(parser.getInt64At(pos).value_or(0));
            pos += intW;
            data_.iebt = static_cast<int>(parser.getInt64At(pos).value_or(0));
            pos += intW;
            data_.idim = static_cast<int>(parser.getInt64At(pos).value_or(0));
            break;
        }

        lineIdx++;
    }

    return true;
}

std::vector<std::string> SectionShellEfg::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    // Card 1
    writer.writeInt(data_.secid);
    writer.writeInt(data_.elform);
    writer.writeDouble(data_.shrf);
    writer.writeInt(data_.nip);
    writer.writeDouble(data_.propt);
    writer.writeInt(data_.qr_irid);
    writer.writeInt(data_.icomp);
    result.push_back(writer.getLine());

    // Card 2
    writer.clear();
    writer.writeDouble(data_.t1);
    writer.writeDouble(data_.t2);
    writer.writeDouble(data_.t3);
    writer.writeDouble(data_.t4);
    writer.writeDouble(data_.nloc);
    result.push_back(writer.getLine());

    // Card 3
    writer.clear();
    writer.writeDouble(data_.dx);
    writer.writeDouble(data_.dy);
    writer.writeInt(data_.ispline);
    writer.writeInt(data_.idila);
    writer.writeInt(data_.iebt);
    writer.writeInt(data_.idim);
    result.push_back(writer.getLine());

    return result;
}

void SectionShellEfg::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

// ============================================================================
// SectionSolidEfg
// ============================================================================

bool SectionSolidEfg::parse(const std::vector<std::string>& lines,
                             util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    int lineIdx = 0;
    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);

        if (lineIdx == 0) {
            // Card 1: SECID, ELFORM, AET
            size_t pos = 0;
            data_.secid = parser.getInt64At(pos).value_or(0);
            pos += intW;
            data_.elform = static_cast<int>(parser.getInt64At(pos).value_or(41));
            pos += intW;
            data_.aet = static_cast<int>(parser.getInt64At(pos).value_or(0));
        } else if (lineIdx == 1) {
            // Card 2: DX, DY, DZ, ISPLINE, IDILA, IEBT, IDIM, NIP
            size_t pos = 0;
            data_.dx = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.dy = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.dz = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.ispline = static_cast<int>(parser.getInt64At(pos).value_or(0));
            pos += intW;
            data_.idila = static_cast<int>(parser.getInt64At(pos).value_or(0));
            pos += intW;
            data_.iebt = static_cast<int>(parser.getInt64At(pos).value_or(0));
            pos += intW;
            data_.idim = static_cast<int>(parser.getInt64At(pos).value_or(0));
            pos += intW;
            data_.nip = static_cast<int>(parser.getInt64At(pos).value_or(0));
            break;
        }

        lineIdx++;
    }

    return true;
}

std::vector<std::string> SectionSolidEfg::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    // Card 1
    writer.writeInt(data_.secid);
    writer.writeInt(data_.elform);
    writer.writeInt(data_.aet);
    result.push_back(writer.getLine());

    // Card 2
    writer.clear();
    writer.writeDouble(data_.dx);
    writer.writeDouble(data_.dy);
    writer.writeDouble(data_.dz);
    writer.writeInt(data_.ispline);
    writer.writeInt(data_.idila);
    writer.writeInt(data_.iebt);
    writer.writeInt(data_.idim);
    writer.writeInt(data_.nip);
    result.push_back(writer.getLine());

    return result;
}

void SectionSolidEfg::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

// ============================================================================
// SectionShellAle
// ============================================================================

bool SectionShellAle::parse(const std::vector<std::string>& lines,
                             util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    int lineIdx = 0;
    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);

        if (lineIdx == 0) {
            // Card 1: SECID, ELFORM, SHRF, NIP, PROPT, QR/IRID, AFAC
            size_t pos = 0;
            data_.secid = parser.getInt64At(pos).value_or(0);
            pos += intW;
            data_.elform = static_cast<int>(parser.getInt64At(pos).value_or(5));
            pos += intW;
            data_.shrf = parser.getDoubleAt(pos, realW).value_or(1.0);
            pos += realW;
            data_.nip = static_cast<int>(parser.getInt64At(pos).value_or(2));
            pos += intW;
            data_.propt = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.qr_irid = static_cast<int>(parser.getInt64At(pos).value_or(0));
            pos += intW;
            data_.afac = static_cast<int>(parser.getInt64At(pos).value_or(0));
        } else if (lineIdx == 1) {
            // Card 2: T1, T2, T3, T4
            size_t pos = 0;
            data_.t1 = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.t2 = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.t3 = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data_.t4 = parser.getDoubleAt(pos, realW).value_or(0.0);
            break;
        }

        lineIdx++;
    }

    return true;
}

std::vector<std::string> SectionShellAle::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    // Card 1
    writer.writeInt(data_.secid);
    writer.writeInt(data_.elform);
    writer.writeDouble(data_.shrf);
    writer.writeInt(data_.nip);
    writer.writeDouble(data_.propt);
    writer.writeInt(data_.qr_irid);
    writer.writeInt(data_.afac);
    result.push_back(writer.getLine());

    // Card 2
    writer.clear();
    writer.writeDouble(data_.t1);
    writer.writeDouble(data_.t2);
    writer.writeDouble(data_.t3);
    writer.writeDouble(data_.t4);
    result.push_back(writer.getLine());

    return result;
}

void SectionShellAle::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

// ============================================================================
// SectionSolidAle
// ============================================================================

bool SectionSolidAle::parse(const std::vector<std::string>& lines,
                             util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);

        // Card 1: SECID, ELFORM, AET, AFAC, BFAC, CFAC, DFAC
        size_t pos = 0;
        data_.secid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data_.elform = static_cast<int>(parser.getInt64At(pos).value_or(11));
        pos += intW;
        data_.aet = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.afac = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.bfac = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.cfac = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.dfac = static_cast<int>(parser.getInt64At(pos).value_or(0));

        break;
    }

    return true;
}

std::vector<std::string> SectionSolidAle::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.secid);
    writer.writeInt(data_.elform);
    writer.writeInt(data_.aet);
    writer.writeInt(data_.afac);
    writer.writeInt(data_.bfac);
    writer.writeInt(data_.cfac);
    writer.writeInt(data_.dfac);
    result.push_back(writer.getLine());

    return result;
}

void SectionSolidAle::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

// ============================================================================
// SectionTshellTitle
// ============================================================================

bool SectionTshellTitle::parse(const std::vector<std::string>& lines,
                                util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    size_t lineIdx = 0;

    // First non-comment line is the title
    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    title_ = util::StringUtils::trim(lines[lineIdx]);
    ++lineIdx;

    while (lineIdx < lines.size()) {
        if (util::CardParser::isCommentLine(lines[lineIdx])) {
            ++lineIdx;
            continue;
        }

        parser.setLine(lines[lineIdx]);
        size_t pos = 0;

        secid_ = parser.getInt64At(pos).value_or(0);
        pos += intW;
        elform_ = static_cast<int>(parser.getInt64At(pos).value_or(1));
        pos += intW;
        shrf_ = static_cast<int>(parser.getInt64At(pos).value_or(1));
        pos += intW;
        nip_ = static_cast<int>(parser.getInt64At(pos).value_or(2));
        pos += intW;
        propt_ = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        qr_irid_ = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        icomp_ = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        tshear_ = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> SectionTshellTitle::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    result.push_back(title_);

    writer.writeInt(secid_);
    writer.writeInt(elform_);
    writer.writeInt(shrf_);
    writer.writeInt(nip_);
    writer.writeInt(propt_);
    writer.writeInt(qr_irid_);
    writer.writeInt(icomp_);
    writer.writeInt(tshear_);
    result.push_back(writer.getLine());

    return result;
}

void SectionTshellTitle::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

// ============================================================================
// SectionDiscreteTitle
// ============================================================================

bool SectionDiscreteTitle::parse(const std::vector<std::string>& lines,
                                  util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    // First non-comment line is the title
    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    title_ = util::StringUtils::trim(lines[lineIdx]);
    ++lineIdx;

    while (lineIdx < lines.size()) {
        if (util::CardParser::isCommentLine(lines[lineIdx])) {
            ++lineIdx;
            continue;
        }

        parser.setLine(lines[lineIdx]);
        size_t pos = 0;

        secid_ = parser.getInt64At(pos).value_or(0);
        pos += intW;
        dro_ = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        kd_ = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        v0_ = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        cl_ = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        fd_ = parser.getDoubleAt(pos, realW).value_or(0.0);
        break;
    }

    return true;
}

std::vector<std::string> SectionDiscreteTitle::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    result.push_back(title_);

    writer.writeInt(secid_);
    writer.writeInt(dro_);
    writer.writeInt(kd_);
    writer.writeInt(v0_);
    writer.writeDouble(cl_);
    writer.writeDouble(fd_);
    result.push_back(writer.getLine());

    return result;
}

void SectionDiscreteTitle::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

// Register keywords
REGISTER_KEYWORD(SectionShell, "*SECTION_SHELL")
REGISTER_KEYWORD(SectionSolid, "*SECTION_SOLID")
REGISTER_KEYWORD(SectionBeam, "*SECTION_BEAM")
REGISTER_KEYWORD(SectionDiscrete, "*SECTION_DISCRETE")
REGISTER_KEYWORD(SectionSeatbelt, "*SECTION_SEATBELT")
REGISTER_KEYWORD(SectionTshell, "*SECTION_TSHELL")
REGISTER_KEYWORD(SectionAle2d, "*SECTION_ALE2D")
REGISTER_KEYWORD(SectionAle1d, "*SECTION_ALE1D")
REGISTER_KEYWORD(SectionShellTitle, "*SECTION_SHELL_TITLE")
REGISTER_KEYWORD(SectionSolidTitle, "*SECTION_SOLID_TITLE")
REGISTER_KEYWORD(SectionBeamTitle, "*SECTION_BEAM_TITLE")
REGISTER_KEYWORD(SectionPointSource, "*SECTION_POINT_SOURCE")
REGISTER_KEYWORD(SectionPointSourceMixture, "*SECTION_POINT_SOURCE_MIXTURE")
REGISTER_KEYWORD(SectionShellEfg, "*SECTION_SHELL_EFG")
REGISTER_KEYWORD(SectionSolidEfg, "*SECTION_SOLID_EFG")
REGISTER_KEYWORD(SectionShellAle, "*SECTION_SHELL_ALE")
REGISTER_KEYWORD(SectionSolidAle, "*SECTION_SOLID_ALE")
REGISTER_KEYWORD(SectionTshellTitle, "*SECTION_TSHELL_TITLE")
REGISTER_KEYWORD(SectionDiscreteTitle, "*SECTION_DISCRETE_TITLE")

} // namespace koo::dyna
