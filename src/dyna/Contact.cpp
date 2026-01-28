#include <koo/dyna/Contact.hpp>
#include <koo/dyna/ModelVisitor.hpp>
#include <koo/dyna/KeywordFactory.hpp>
#include <koo/util/StringUtils.hpp>

namespace koo::dyna {

namespace {

// Helper to parse contact Card1
void parseCard1(ContactAutomaticSingleSurface::Card1& card, util::CardParser& parser, size_t intW) {
    size_t pos = 0;
    card.ssid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    card.msid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    card.sstyp = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    card.mstyp = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    card.sboxid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    card.mboxid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    card.spr = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    card.mpr = static_cast<int>(parser.getInt64At(pos).value_or(0));
}

// Helper to parse contact Card2
void parseCard2(ContactAutomaticSingleSurface::Card2& card, util::CardParser& parser,
                size_t intW, size_t realW) {
    size_t pos = 0;
    card.fs = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    card.fd = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    card.dc = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    card.vc = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    card.vdc = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    card.penchk = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    card.bt = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    card.dt = parser.getDoubleAt(pos, realW).value_or(1e20);
}

// Helper to parse contact Card3
void parseCard3(ContactAutomaticSingleSurface::Card3& card, util::CardParser& parser, size_t realW) {
    size_t pos = 0;
    card.sfs = parser.getDoubleAt(pos, realW).value_or(1.0);
    pos += realW;
    card.sfm = parser.getDoubleAt(pos, realW).value_or(1.0);
    pos += realW;
    card.sst = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    card.mst = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    card.sfst = parser.getDoubleAt(pos, realW).value_or(1.0);
    pos += realW;
    card.sfmt = parser.getDoubleAt(pos, realW).value_or(1.0);
    pos += realW;
    card.fsf = parser.getDoubleAt(pos, realW).value_or(1.0);
    pos += realW;
    card.vsf = parser.getDoubleAt(pos, realW).value_or(1.0);
}

// Helper to write contact Card1
void writeCard1(const ContactAutomaticSingleSurface::Card1& card, util::CardWriter& writer) {
    writer.writeInt(card.ssid);
    writer.writeInt(card.msid);
    writer.writeInt(card.sstyp);
    writer.writeInt(card.mstyp);
    writer.writeInt(card.sboxid);
    writer.writeInt(card.mboxid);
    writer.writeInt(card.spr);
    writer.writeInt(card.mpr);
}

// Helper to write contact Card2
void writeCard2(const ContactAutomaticSingleSurface::Card2& card, util::CardWriter& writer) {
    writer.writeDouble(card.fs);
    writer.writeDouble(card.fd);
    writer.writeDouble(card.dc);
    writer.writeDouble(card.vc);
    writer.writeDouble(card.vdc);
    writer.writeInt(card.penchk);
    writer.writeDouble(card.bt);
    writer.writeDouble(card.dt);
}

// Helper to write contact Card3
void writeCard3(const ContactAutomaticSingleSurface::Card3& card, util::CardWriter& writer) {
    writer.writeDouble(card.sfs);
    writer.writeDouble(card.sfm);
    writer.writeDouble(card.sst);
    writer.writeDouble(card.mst);
    writer.writeDouble(card.sfst);
    writer.writeDouble(card.sfmt);
    writer.writeDouble(card.fsf);
    writer.writeDouble(card.vsf);
}

} // anonymous namespace

// ============================================================================
// ContactAutomaticSingleSurface
// ============================================================================

bool ContactAutomaticSingleSurface::parse(const std::vector<std::string>& lines,
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

    // Card 1
    parser.setLine(lines[lineIdx]);
    parseCard1(card1_, parser, intW);
    ++lineIdx;

    // Skip comments
    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 2
    parser.setLine(lines[lineIdx]);
    parseCard2(card2_, parser, intW, realW);
    ++lineIdx;

    // Skip comments
    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 3
    parser.setLine(lines[lineIdx]);
    parseCard3(card3_, parser, realW);

    return true;
}

std::vector<std::string> ContactAutomaticSingleSurface::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writeCard1(card1_, writer);
    result.push_back(writer.getLine());

    writer.clear();
    writeCard2(card2_, writer);
    result.push_back(writer.getLine());

    writer.clear();
    writeCard3(card3_, writer);
    result.push_back(writer.getLine());

    return result;
}

void ContactAutomaticSingleSurface::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ContactAutomaticSingleSurface, "*CONTACT_AUTOMATIC_SINGLE_SURFACE")

// ============================================================================
// ContactAutomaticSurfaceToSurface
// ============================================================================

bool ContactAutomaticSurfaceToSurface::parse(const std::vector<std::string>& lines,
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
    parseCard1(card1_, parser, intW);
    ++lineIdx;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    parser.setLine(lines[lineIdx]);
    parseCard2(card2_, parser, intW, realW);
    ++lineIdx;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    parser.setLine(lines[lineIdx]);
    parseCard3(card3_, parser, realW);

    return true;
}

std::vector<std::string> ContactAutomaticSurfaceToSurface::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writeCard1(card1_, writer);
    result.push_back(writer.getLine());

    writer.clear();
    writeCard2(card2_, writer);
    result.push_back(writer.getLine());

    writer.clear();
    writeCard3(card3_, writer);
    result.push_back(writer.getLine());

    return result;
}

void ContactAutomaticSurfaceToSurface::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ContactAutomaticSurfaceToSurface, "*CONTACT_AUTOMATIC_SURFACE_TO_SURFACE")

// ============================================================================
// ContactAutomaticNodesToSurface
// ============================================================================

bool ContactAutomaticNodesToSurface::parse(const std::vector<std::string>& lines,
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
    parseCard1(card1_, parser, intW);
    ++lineIdx;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    parser.setLine(lines[lineIdx]);
    parseCard2(card2_, parser, intW, realW);
    ++lineIdx;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    parser.setLine(lines[lineIdx]);
    parseCard3(card3_, parser, realW);

    return true;
}

std::vector<std::string> ContactAutomaticNodesToSurface::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writeCard1(card1_, writer);
    result.push_back(writer.getLine());

    writer.clear();
    writeCard2(card2_, writer);
    result.push_back(writer.getLine());

    writer.clear();
    writeCard3(card3_, writer);
    result.push_back(writer.getLine());

    return result;
}

void ContactAutomaticNodesToSurface::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ContactAutomaticNodesToSurface, "*CONTACT_AUTOMATIC_NODES_TO_SURFACE")

// ============================================================================
// ContactAutomaticGeneral
// ============================================================================

bool ContactAutomaticGeneral::parse(const std::vector<std::string>& lines,
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
    parseCard1(card1_, parser, intW);
    ++lineIdx;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    parser.setLine(lines[lineIdx]);
    parseCard2(card2_, parser, intW, realW);
    ++lineIdx;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    parser.setLine(lines[lineIdx]);
    parseCard3(card3_, parser, realW);

    return true;
}

std::vector<std::string> ContactAutomaticGeneral::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writeCard1(card1_, writer);
    result.push_back(writer.getLine());

    writer.clear();
    writeCard2(card2_, writer);
    result.push_back(writer.getLine());

    writer.clear();
    writeCard3(card3_, writer);
    result.push_back(writer.getLine());

    return result;
}

void ContactAutomaticGeneral::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ContactAutomaticGeneral, "*CONTACT_AUTOMATIC_GENERAL")

// ============================================================================
// ContactSurfaceToSurface
// ============================================================================

bool ContactSurfaceToSurface::parse(const std::vector<std::string>& lines,
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
    parseCard1(card1_, parser, intW);
    ++lineIdx;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    parser.setLine(lines[lineIdx]);
    parseCard2(card2_, parser, intW, realW);
    ++lineIdx;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    parser.setLine(lines[lineIdx]);
    parseCard3(card3_, parser, realW);

    return true;
}

std::vector<std::string> ContactSurfaceToSurface::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writeCard1(card1_, writer);
    result.push_back(writer.getLine());

    writer.clear();
    writeCard2(card2_, writer);
    result.push_back(writer.getLine());

    writer.clear();
    writeCard3(card3_, writer);
    result.push_back(writer.getLine());

    return result;
}

void ContactSurfaceToSurface::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ContactSurfaceToSurface, "*CONTACT_SURFACE_TO_SURFACE")

// ============================================================================
// ContactNodesToSurface
// ============================================================================

bool ContactNodesToSurface::parse(const std::vector<std::string>& lines,
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
    parseCard1(card1_, parser, intW);
    ++lineIdx;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    parser.setLine(lines[lineIdx]);
    parseCard2(card2_, parser, intW, realW);
    ++lineIdx;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    parser.setLine(lines[lineIdx]);
    parseCard3(card3_, parser, realW);

    return true;
}

std::vector<std::string> ContactNodesToSurface::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writeCard1(card1_, writer);
    result.push_back(writer.getLine());

    writer.clear();
    writeCard2(card2_, writer);
    result.push_back(writer.getLine());

    writer.clear();
    writeCard3(card3_, writer);
    result.push_back(writer.getLine());

    return result;
}

void ContactNodesToSurface::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ContactNodesToSurface, "*CONTACT_NODES_TO_SURFACE")

// ============================================================================
// ContactTiedNodesToSurface
// ============================================================================

bool ContactTiedNodesToSurface::parse(const std::vector<std::string>& lines,
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
    parseCard1(card1_, parser, intW);
    ++lineIdx;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    parser.setLine(lines[lineIdx]);
    parseCard2(card2_, parser, intW, realW);

    return true;
}

std::vector<std::string> ContactTiedNodesToSurface::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writeCard1(card1_, writer);
    result.push_back(writer.getLine());

    writer.clear();
    writeCard2(card2_, writer);
    result.push_back(writer.getLine());

    return result;
}

void ContactTiedNodesToSurface::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ContactTiedNodesToSurface, "*CONTACT_TIED_NODES_TO_SURFACE")

// ============================================================================
// ContactTiedSurfaceToSurface
// ============================================================================

bool ContactTiedSurfaceToSurface::parse(const std::vector<std::string>& lines,
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
    parseCard1(card1_, parser, intW);
    ++lineIdx;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    parser.setLine(lines[lineIdx]);
    parseCard2(card2_, parser, intW, realW);

    return true;
}

std::vector<std::string> ContactTiedSurfaceToSurface::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writeCard1(card1_, writer);
    result.push_back(writer.getLine());

    writer.clear();
    writeCard2(card2_, writer);
    result.push_back(writer.getLine());

    return result;
}

void ContactTiedSurfaceToSurface::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ContactTiedSurfaceToSurface, "*CONTACT_TIED_SURFACE_TO_SURFACE")

// ============================================================================
// ContactSpotweld
// ============================================================================

bool ContactSpotweld::parse(const std::vector<std::string>& lines,
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
    parseCard1(card1_, parser, intW);
    ++lineIdx;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    parser.setLine(lines[lineIdx]);
    parseCard2(card2_, parser, intW, realW);

    return true;
}

std::vector<std::string> ContactSpotweld::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writeCard1(card1_, writer);
    result.push_back(writer.getLine());

    writer.clear();
    writeCard2(card2_, writer);
    result.push_back(writer.getLine());

    return result;
}

void ContactSpotweld::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ContactSpotweld, "*CONTACT_SPOTWELD")

// ============================================================================
// ContactRigidBodyOneWay
// ============================================================================

bool ContactRigidBodyOneWay::parse(const std::vector<std::string>& lines,
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
    parseCard1(card1_, parser, intW);
    ++lineIdx;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    parser.setLine(lines[lineIdx]);
    parseCard2(card2_, parser, intW, realW);

    return true;
}

std::vector<std::string> ContactRigidBodyOneWay::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writeCard1(card1_, writer);
    result.push_back(writer.getLine());

    writer.clear();
    writeCard2(card2_, writer);
    result.push_back(writer.getLine());

    return result;
}

void ContactRigidBodyOneWay::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ContactRigidBodyOneWay, "*CONTACT_RIGID_BODY_ONE_WAY")

// ============================================================================
// ContactErodingSingleSurface
// ============================================================================

bool ContactErodingSingleSurface::parse(const std::vector<std::string>& lines,
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
    parseCard1(card1_, parser, intW);
    ++lineIdx;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    parser.setLine(lines[lineIdx]);
    parseCard2(card2_, parser, intW, realW);
    ++lineIdx;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    parser.setLine(lines[lineIdx]);
    parseCard3(card3_, parser, realW);

    return true;
}

std::vector<std::string> ContactErodingSingleSurface::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writeCard1(card1_, writer);
    result.push_back(writer.getLine());

    writer.clear();
    writeCard2(card2_, writer);
    result.push_back(writer.getLine());

    writer.clear();
    writeCard3(card3_, writer);
    result.push_back(writer.getLine());

    return result;
}

void ContactErodingSingleSurface::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ContactErodingSingleSurface, "*CONTACT_ERODING_SINGLE_SURFACE")

// ============================================================================
// ContactErodingSurfaceToSurface
// ============================================================================

bool ContactErodingSurfaceToSurface::parse(const std::vector<std::string>& lines,
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
    parseCard1(card1_, parser, intW);
    ++lineIdx;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    parser.setLine(lines[lineIdx]);
    parseCard2(card2_, parser, intW, realW);
    ++lineIdx;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    parser.setLine(lines[lineIdx]);
    parseCard3(card3_, parser, realW);

    return true;
}

std::vector<std::string> ContactErodingSurfaceToSurface::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writeCard1(card1_, writer);
    result.push_back(writer.getLine());

    writer.clear();
    writeCard2(card2_, writer);
    result.push_back(writer.getLine());

    writer.clear();
    writeCard3(card3_, writer);
    result.push_back(writer.getLine());

    return result;
}

void ContactErodingSurfaceToSurface::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ContactErodingSurfaceToSurface, "*CONTACT_ERODING_SURFACE_TO_SURFACE")

// ============================================================================
// ContactErodingNodesToSurface
// ============================================================================

bool ContactErodingNodesToSurface::parse(const std::vector<std::string>& lines,
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
    parseCard1(card1_, parser, intW);
    ++lineIdx;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    parser.setLine(lines[lineIdx]);
    parseCard2(card2_, parser, intW, realW);
    ++lineIdx;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    parser.setLine(lines[lineIdx]);
    parseCard3(card3_, parser, realW);

    return true;
}

std::vector<std::string> ContactErodingNodesToSurface::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writeCard1(card1_, writer);
    result.push_back(writer.getLine());

    writer.clear();
    writeCard2(card2_, writer);
    result.push_back(writer.getLine());

    writer.clear();
    writeCard3(card3_, writer);
    result.push_back(writer.getLine());

    return result;
}

void ContactErodingNodesToSurface::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ContactErodingNodesToSurface, "*CONTACT_ERODING_NODES_TO_SURFACE")

// ============================================================================
// ContactFormingOneWaySurfaceToSurface
// ============================================================================

bool ContactFormingOneWaySurfaceToSurface::parse(const std::vector<std::string>& lines,
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
    parseCard1(card1_, parser, intW);
    ++lineIdx;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    parser.setLine(lines[lineIdx]);
    parseCard2(card2_, parser, intW, realW);
    ++lineIdx;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    parser.setLine(lines[lineIdx]);
    parseCard3(card3_, parser, realW);

    return true;
}

std::vector<std::string> ContactFormingOneWaySurfaceToSurface::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writeCard1(card1_, writer);
    result.push_back(writer.getLine());

    writer.clear();
    writeCard2(card2_, writer);
    result.push_back(writer.getLine());

    writer.clear();
    writeCard3(card3_, writer);
    result.push_back(writer.getLine());

    return result;
}

void ContactFormingOneWaySurfaceToSurface::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ContactFormingOneWaySurfaceToSurface, "*CONTACT_FORMING_ONE_WAY_SURFACE_TO_SURFACE")

// ============================================================================
// ContactInterior
// ============================================================================

bool ContactInterior::parse(const std::vector<std::string>& lines,
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
    data_.ssid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.stype = static_cast<int>(parser.getInt64At(pos).value_or(0));

    return true;
}

std::vector<std::string> ContactInterior::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.ssid);
    writer.writeInt(data_.stype);
    result.push_back(writer.getLine());

    return result;
}

void ContactInterior::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ContactInterior, "*CONTACT_INTERIOR")

// ============================================================================
// ContactAutomaticSingleSurfaceMortar
// ============================================================================

bool ContactAutomaticSingleSurfaceMortar::parse(const std::vector<std::string>& lines,
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
    parseCard1(card1_, parser, intW);
    ++lineIdx;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    parser.setLine(lines[lineIdx]);
    parseCard2(card2_, parser, intW, realW);
    ++lineIdx;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    parser.setLine(lines[lineIdx]);
    parseCard3(card3_, parser, realW);

    return true;
}

std::vector<std::string> ContactAutomaticSingleSurfaceMortar::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writeCard1(card1_, writer);
    result.push_back(writer.getLine());

    writer.clear();
    writeCard2(card2_, writer);
    result.push_back(writer.getLine());

    writer.clear();
    writeCard3(card3_, writer);
    result.push_back(writer.getLine());

    return result;
}

void ContactAutomaticSingleSurfaceMortar::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ContactAutomaticSingleSurfaceMortar, "*CONTACT_AUTOMATIC_SINGLE_SURFACE_MORTAR")

// ============================================================================
// ContactAutomaticSurfaceToSurfaceMortar
// ============================================================================

bool ContactAutomaticSurfaceToSurfaceMortar::parse(const std::vector<std::string>& lines,
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
    parseCard1(card1_, parser, intW);
    ++lineIdx;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    parser.setLine(lines[lineIdx]);
    parseCard2(card2_, parser, intW, realW);
    ++lineIdx;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    parser.setLine(lines[lineIdx]);
    parseCard3(card3_, parser, realW);

    return true;
}

std::vector<std::string> ContactAutomaticSurfaceToSurfaceMortar::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writeCard1(card1_, writer);
    result.push_back(writer.getLine());

    writer.clear();
    writeCard2(card2_, writer);
    result.push_back(writer.getLine());

    writer.clear();
    writeCard3(card3_, writer);
    result.push_back(writer.getLine());

    return result;
}

void ContactAutomaticSurfaceToSurfaceMortar::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ContactAutomaticSurfaceToSurfaceMortar, "*CONTACT_AUTOMATIC_SURFACE_TO_SURFACE_MORTAR")

// ============================================================================
// ContactTiedShellEdgeToSurface
// ============================================================================

bool ContactTiedShellEdgeToSurface::parse(const std::vector<std::string>& lines,
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
    parseCard1(card1_, parser, intW);
    ++lineIdx;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    parser.setLine(lines[lineIdx]);
    parseCard2(card2_, parser, intW, realW);

    return true;
}

std::vector<std::string> ContactTiedShellEdgeToSurface::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writeCard1(card1_, writer);
    result.push_back(writer.getLine());

    writer.clear();
    writeCard2(card2_, writer);
    result.push_back(writer.getLine());

    return result;
}

void ContactTiedShellEdgeToSurface::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ContactTiedShellEdgeToSurface, "*CONTACT_TIED_SHELL_EDGE_TO_SURFACE")

// ============================================================================
// ContactDrawbead
// ============================================================================

bool ContactDrawbead::parse(const std::vector<std::string>& lines,
                             util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    data_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;

        Data entry;
        entry.ssid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.msid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.lcidrf = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        entry.nid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.dbdth = parser.getDoubleAt(pos, realW).value_or(0.0);

        data_.push_back(entry);
    }

    return true;
}

std::vector<std::string> ContactDrawbead::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& entry : data_) {
        writer.clear();
        writer.writeInt(entry.ssid);
        writer.writeInt(entry.msid);
        writer.writeDouble(entry.lcidrf);
        writer.writeInt(entry.nid);
        writer.writeDouble(entry.dbdth);
        result.push_back(writer.getLine());
    }

    return result;
}

void ContactDrawbead::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ContactDrawbead, "*CONTACT_DRAWBEAD")

// ============================================================================
// ContactForceTransducerPenalty
// ============================================================================

bool ContactForceTransducerPenalty::parse(const std::vector<std::string>& lines,
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
    parseCard1(card1_, parser, intW);
    ++lineIdx;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    parser.setLine(lines[lineIdx]);
    parseCard2(card2_, parser, intW, realW);

    return true;
}

std::vector<std::string> ContactForceTransducerPenalty::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writeCard1(card1_, writer);
    result.push_back(writer.getLine());

    writer.clear();
    writeCard2(card2_, writer);
    result.push_back(writer.getLine());

    return result;
}

void ContactForceTransducerPenalty::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ContactForceTransducerPenalty, "*CONTACT_FORCE_TRANSDUCER_PENALTY")

// ============================================================================
// Contact2dAutomaticSingleSurface
// ============================================================================

bool Contact2dAutomaticSingleSurface::parse(const std::vector<std::string>& lines,
                                             util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: SSID, MSID, SSTYP, MSTYP
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;
    data_.ssid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.msid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.sstyp = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.mstyp = static_cast<int>(parser.getInt64At(pos).value_or(0));
    ++lineIdx;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 2: FS, FD, DC, SOFT
    parser.setLine(lines[lineIdx]);
    pos = 0;
    data_.fs = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.fd = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.dc = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.soft = parser.getDoubleAt(pos, realW).value_or(0.0);

    return true;
}

std::vector<std::string> Contact2dAutomaticSingleSurface::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    // Card 1
    writer.writeInt(data_.ssid);
    writer.writeInt(data_.msid);
    writer.writeInt(data_.sstyp);
    writer.writeInt(data_.mstyp);
    result.push_back(writer.getLine());

    // Card 2
    writer.clear();
    writer.writeDouble(data_.fs);
    writer.writeDouble(data_.fd);
    writer.writeDouble(data_.dc);
    writer.writeDouble(data_.soft);
    result.push_back(writer.getLine());

    return result;
}

void Contact2dAutomaticSingleSurface::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(Contact2dAutomaticSingleSurface, "*CONTACT_2D_AUTOMATIC_SINGLE_SURFACE")

// ============================================================================
// ContactGebodSegment
// ============================================================================

bool ContactGebodSegment::parse(const std::vector<std::string>& lines,
                                 util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: PID, DID, SEGTYP, SF, OPT
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;
    data_.pid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.did = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.segtyp = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.sf = parser.getDoubleAt(pos, realW).value_or(1.0);
    pos += realW;
    data_.opt = static_cast<int>(parser.getInt64At(pos).value_or(0));

    return true;
}

std::vector<std::string> ContactGebodSegment::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.pid);
    writer.writeInt(data_.did);
    writer.writeInt(data_.segtyp);
    writer.writeDouble(data_.sf);
    writer.writeInt(data_.opt);
    result.push_back(writer.getLine());

    return result;
}

void ContactGebodSegment::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ContactGebodSegment, "*CONTACT_GEBOD_SEGMENT")

// ============================================================================
// ContactAutomaticSurfaceToSurfaceTiebreak
// ============================================================================

bool ContactAutomaticSurfaceToSurfaceTiebreak::parse(const std::vector<std::string>& lines,
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
            parseCard1(card1_, parser, intW);
        } else if (cardNum == 1) {
            parseCard2(card2_, parser, intW, realW);
        } else if (cardNum == 2) {
            parseCard3(card3_, parser, realW);
        } else if (cardNum == 3) {
            // Tiebreak card: OPTION, NFLS, SFLS, PARAM, ERATEN, ERATES, CT2CN, CN
            size_t pos = 0;
            tiebreak_.option = static_cast<int>(parser.getInt64At(pos).value_or(0));
            pos += intW;
            tiebreak_.nfls = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            tiebreak_.sfls = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            tiebreak_.param = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            tiebreak_.eraten = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            tiebreak_.erates = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            tiebreak_.ct2cn = parser.getDoubleAt(pos, realW).value_or(1.0);
            pos += realW;
            tiebreak_.cn = parser.getDoubleAt(pos, realW).value_or(0.0);
        }

        cardNum++;
        if (cardNum >= 4) break;
    }

    return true;
}

std::vector<std::string> ContactAutomaticSurfaceToSurfaceTiebreak::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    // Card 1
    writeCard1(card1_, writer);
    result.push_back(writer.getLine());

    // Card 2
    writer.clear();
    writeCard2(card2_, writer);
    result.push_back(writer.getLine());

    // Card 3
    writer.clear();
    writeCard3(card3_, writer);
    result.push_back(writer.getLine());

    // Tiebreak card
    writer.clear();
    writer.writeInt(tiebreak_.option);
    writer.writeDouble(tiebreak_.nfls);
    writer.writeDouble(tiebreak_.sfls);
    writer.writeDouble(tiebreak_.param);
    writer.writeDouble(tiebreak_.eraten);
    writer.writeDouble(tiebreak_.erates);
    writer.writeDouble(tiebreak_.ct2cn);
    writer.writeDouble(tiebreak_.cn);
    result.push_back(writer.getLine());

    return result;
}

void ContactAutomaticSurfaceToSurfaceTiebreak::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ContactAutomaticSurfaceToSurfaceTiebreak, "*CONTACT_AUTOMATIC_SURFACE_TO_SURFACE_TIEBREAK")

// ============================================================================
// ContactAutomaticSingleSurfaceId
// ============================================================================

bool ContactAutomaticSingleSurfaceId::parse(const std::vector<std::string>& lines,
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
            // ID Card: CID, HEADING
            size_t pos = 0;
            idCard_.cid = static_cast<int>(parser.getInt64At(pos).value_or(0));
            pos += intW;
            // Rest of line is heading
            if (line.length() > pos) {
                idCard_.heading = line.substr(pos);
                // Trim trailing spaces
                size_t end = idCard_.heading.find_last_not_of(" \t\r\n");
                if (end != std::string::npos) {
                    idCard_.heading = idCard_.heading.substr(0, end + 1);
                }
            }
        } else if (cardNum == 1) {
            parseCard1(card1_, parser, intW);
        } else if (cardNum == 2) {
            parseCard2(card2_, parser, intW, realW);
        } else if (cardNum == 3) {
            parseCard3(card3_, parser, realW);
        }

        cardNum++;
        if (cardNum >= 4) break;
    }

    return true;
}

std::vector<std::string> ContactAutomaticSingleSurfaceId::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    // ID Card
    writer.writeInt(idCard_.cid);
    std::string idLine = writer.getLine();
    // Append heading if present
    if (!idCard_.heading.empty()) {
        idLine += idCard_.heading;
    }
    result.push_back(idLine);

    // Card 1
    writer.clear();
    writeCard1(card1_, writer);
    result.push_back(writer.getLine());

    // Card 2
    writer.clear();
    writeCard2(card2_, writer);
    result.push_back(writer.getLine());

    // Card 3
    writer.clear();
    writeCard3(card3_, writer);
    result.push_back(writer.getLine());

    return result;
}

void ContactAutomaticSingleSurfaceId::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ContactAutomaticSingleSurfaceId, "*CONTACT_AUTOMATIC_SINGLE_SURFACE_ID")

// ============================================================================
// ContactEntity
// ============================================================================

bool ContactEntity::parse(const std::vector<std::string>& lines,
                          util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: SLAVE, MASTER, SSTYP, MSTYP, SBOXID, MBOXID
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.slave = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.master = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.sstyp = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.mstyp = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.sboxid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.mboxid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    ++lineIdx;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 2: FS, FD, DC, VC, PENCHK, BT, DT
    parser.setLine(lines[lineIdx]);
    pos = 0;

    data_.fs = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.fd = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.dc = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.vc = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.penchk = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.bt = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.dt = parser.getDoubleAt(pos, realW).value_or(1.0e20);

    return true;
}

std::vector<std::string> ContactEntity::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    // Card 1
    writer.writeInt(data_.slave);
    writer.writeInt(data_.master);
    writer.writeInt(data_.sstyp);
    writer.writeInt(data_.mstyp);
    writer.writeInt(data_.sboxid);
    writer.writeInt(data_.mboxid);
    result.push_back(writer.getLine());

    // Card 2
    writer.clear();
    writer.writeDouble(data_.fs);
    writer.writeDouble(data_.fd);
    writer.writeDouble(data_.dc);
    writer.writeDouble(data_.vc);
    writer.writeDouble(data_.penchk);
    writer.writeDouble(data_.bt);
    writer.writeDouble(data_.dt);
    result.push_back(writer.getLine());

    return result;
}

void ContactEntity::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ContactEntity, "*CONTACT_ENTITY")

// ============================================================================
// ContactAutomaticSurfaceToSurfaceId
// ============================================================================

bool ContactAutomaticSurfaceToSurfaceId::parse(const std::vector<std::string>& lines,
                                                util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    int cardNum = 0;
    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);

        if (cardNum == 0) {
            // ID Card: CID, HEADING
            size_t pos = 0;
            cid_ = static_cast<int>(parser.getInt64At(pos).value_or(0));
            pos += intW;
            if (line.length() > pos) {
                heading_ = line.substr(pos);
                size_t end = heading_.find_last_not_of(" \t\r\n");
                if (end != std::string::npos) {
                    heading_ = heading_.substr(0, end + 1);
                }
            }
        } else if (cardNum == 1) {
            parseCard1(card1_, parser, intW);
        } else if (cardNum == 2) {
            parseCard2(card2_, parser, intW, realW);
        } else if (cardNum == 3) {
            parseCard3(card3_, parser, realW);
        }

        cardNum++;
        if (cardNum >= 4) break;
    }

    return true;
}

std::vector<std::string> ContactAutomaticSurfaceToSurfaceId::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    // ID Card
    writer.writeInt(cid_);
    std::string idLine = writer.getLine();
    if (!heading_.empty()) {
        idLine += heading_;
    }
    result.push_back(idLine);

    // Card 1
    writer.clear();
    writeCard1(card1_, writer);
    result.push_back(writer.getLine());

    // Card 2
    writer.clear();
    writeCard2(card2_, writer);
    result.push_back(writer.getLine());

    // Card 3
    writer.clear();
    writeCard3(card3_, writer);
    result.push_back(writer.getLine());

    return result;
}

void ContactAutomaticSurfaceToSurfaceId::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ContactAutomaticSurfaceToSurfaceId, "*CONTACT_AUTOMATIC_SURFACE_TO_SURFACE_ID")

// ============================================================================
// ContactFormingSurfaceToSurface
// ============================================================================

bool ContactFormingSurfaceToSurface::parse(const std::vector<std::string>& lines,
                                            util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    int cardNum = 0;
    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);

        if (cardNum == 0) {
            parseCard1(card1_, parser, intW);
        } else if (cardNum == 1) {
            parseCard2(card2_, parser, intW, realW);
        } else if (cardNum == 2) {
            parseCard3(card3_, parser, realW);
        }

        cardNum++;
        if (cardNum >= 3) break;
    }

    return true;
}

std::vector<std::string> ContactFormingSurfaceToSurface::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writeCard1(card1_, writer);
    result.push_back(writer.getLine());

    writer.clear();
    writeCard2(card2_, writer);
    result.push_back(writer.getLine());

    writer.clear();
    writeCard3(card3_, writer);
    result.push_back(writer.getLine());

    return result;
}

void ContactFormingSurfaceToSurface::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ContactFormingSurfaceToSurface, "*CONTACT_FORMING_SURFACE_TO_SURFACE")

// ============================================================================
// ContactSingleSurface
// ============================================================================

bool ContactSingleSurface::parse(const std::vector<std::string>& lines,
                                  util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    int cardNum = 0;
    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);

        if (cardNum == 0) {
            parseCard1(card1_, parser, intW);
        } else if (cardNum == 1) {
            parseCard2(card2_, parser, intW, realW);
        } else if (cardNum == 2) {
            parseCard3(card3_, parser, realW);
        }

        cardNum++;
        if (cardNum >= 3) break;
    }

    return true;
}

std::vector<std::string> ContactSingleSurface::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writeCard1(card1_, writer);
    result.push_back(writer.getLine());

    writer.clear();
    writeCard2(card2_, writer);
    result.push_back(writer.getLine());

    writer.clear();
    writeCard3(card3_, writer);
    result.push_back(writer.getLine());

    return result;
}

void ContactSingleSurface::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ContactSingleSurface, "*CONTACT_SINGLE_SURFACE")

// ============================================================================
// ContactAutomaticBeamsToSurface
// ============================================================================

bool ContactAutomaticBeamsToSurface::parse(const std::vector<std::string>& lines,
                                            util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    int cardNum = 0;
    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);

        if (cardNum == 0) {
            parseCard1(card1_, parser, intW);
        } else if (cardNum == 1) {
            parseCard2(card2_, parser, intW, realW);
        } else if (cardNum == 2) {
            parseCard3(card3_, parser, realW);
        }

        cardNum++;
        if (cardNum >= 3) break;
    }

    return true;
}

std::vector<std::string> ContactAutomaticBeamsToSurface::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writeCard1(card1_, writer);
    result.push_back(writer.getLine());

    writer.clear();
    writeCard2(card2_, writer);
    result.push_back(writer.getLine());

    writer.clear();
    writeCard3(card3_, writer);
    result.push_back(writer.getLine());

    return result;
}

void ContactAutomaticBeamsToSurface::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ContactAutomaticBeamsToSurface, "*CONTACT_AUTOMATIC_BEAMS_TO_SURFACE")

// ============================================================================
// ContactTiedShellEdgeToSolid
// ============================================================================

bool ContactTiedShellEdgeToSolid::parse(const std::vector<std::string>& lines,
                                         util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    int cardNum = 0;
    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);

        if (cardNum == 0) {
            parseCard1(card1_, parser, intW);
        } else if (cardNum == 1) {
            parseCard2(card2_, parser, intW, realW);
        }

        cardNum++;
        if (cardNum >= 2) break;
    }

    return true;
}

std::vector<std::string> ContactTiedShellEdgeToSolid::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writeCard1(card1_, writer);
    result.push_back(writer.getLine());

    writer.clear();
    writeCard2(card2_, writer);
    result.push_back(writer.getLine());

    return result;
}

void ContactTiedShellEdgeToSolid::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ContactTiedShellEdgeToSolid, "*CONTACT_TIED_SHELL_EDGE_TO_SOLID")

// ============================================================================
// ContactRigidBodyTwoWay
// ============================================================================

bool ContactRigidBodyTwoWay::parse(const std::vector<std::string>& lines,
                                    util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    int cardNum = 0;
    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);

        if (cardNum == 0) {
            parseCard1(card1_, parser, intW);
        } else if (cardNum == 1) {
            parseCard2(card2_, parser, intW, realW);
        }

        cardNum++;
        if (cardNum >= 2) break;
    }

    return true;
}

std::vector<std::string> ContactRigidBodyTwoWay::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writeCard1(card1_, writer);
    result.push_back(writer.getLine());

    writer.clear();
    writeCard2(card2_, writer);
    result.push_back(writer.getLine());

    return result;
}

void ContactRigidBodyTwoWay::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ContactRigidBodyTwoWay, "*CONTACT_RIGID_BODY_TWO_WAY")

// ============================================================================
// ContactAutomaticNodesToSurfaceId
// ============================================================================

bool ContactAutomaticNodesToSurfaceId::parse(const std::vector<std::string>& lines,
                                              util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    int cardNum = 0;
    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);

        if (cardNum == 0) {
            size_t pos = 0;
            idCard_.cid = static_cast<int>(parser.getInt64At(pos).value_or(0));
            pos += intW;
            if (line.size() > pos) {
                idCard_.heading = line.substr(pos);
                // Trim trailing whitespace
                while (!idCard_.heading.empty() && std::isspace(idCard_.heading.back())) {
                    idCard_.heading.pop_back();
                }
            }
        } else if (cardNum == 1) {
            parseCard1(card1_, parser, intW);
        } else if (cardNum == 2) {
            parseCard2(card2_, parser, intW, realW);
        } else if (cardNum == 3) {
            parseCard3(card3_, parser, realW);
        }

        cardNum++;
        if (cardNum >= 4) break;
    }

    return true;
}

std::vector<std::string> ContactAutomaticNodesToSurfaceId::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(idCard_.cid);
    writer.writeString(idCard_.heading);
    result.push_back(writer.getLine());

    writer.clear();
    writeCard1(card1_, writer);
    result.push_back(writer.getLine());

    writer.clear();
    writeCard2(card2_, writer);
    result.push_back(writer.getLine());

    writer.clear();
    writeCard3(card3_, writer);
    result.push_back(writer.getLine());

    return result;
}

void ContactAutomaticNodesToSurfaceId::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ContactAutomaticNodesToSurfaceId, "*CONTACT_AUTOMATIC_NODES_TO_SURFACE_ID")

// ============================================================================
// ContactAirbagSingleSurface
// ============================================================================

bool ContactAirbagSingleSurface::parse(const std::vector<std::string>& lines,
                                        util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    int cardNum = 0;
    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);

        if (cardNum == 0) {
            parseCard1(card1_, parser, intW);
        } else if (cardNum == 1) {
            parseCard2(card2_, parser, intW, realW);
        }

        cardNum++;
        if (cardNum >= 2) break;
    }

    return true;
}

std::vector<std::string> ContactAirbagSingleSurface::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writeCard1(card1_, writer);
    result.push_back(writer.getLine());

    writer.clear();
    writeCard2(card2_, writer);
    result.push_back(writer.getLine());

    return result;
}

void ContactAirbagSingleSurface::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ContactAirbagSingleSurface, "*CONTACT_AIRBAG_SINGLE_SURFACE")

// ============================================================================
// ContactGuidedCable
// ============================================================================

bool ContactGuidedCable::parse(const std::vector<std::string>& lines,
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

        data_.ssid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.msid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.sstyp = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.mstyp = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.fs = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.fd = parser.getDoubleAt(pos, realW).value_or(0.0);

        break;
    }

    return true;
}

std::vector<std::string> ContactGuidedCable::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.ssid);
    writer.writeInt(data_.msid);
    writer.writeInt(data_.sstyp);
    writer.writeInt(data_.mstyp);
    writer.writeDouble(data_.fs);
    writer.writeDouble(data_.fd);
    result.push_back(writer.getLine());

    return result;
}

void ContactGuidedCable::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ContactGuidedCable, "*CONTACT_GUIDED_CABLE")

// ============================================================================
// ContactTiebreakNodesToSurface
// ============================================================================

bool ContactTiebreakNodesToSurface::parse(const std::vector<std::string>& lines,
                                           util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    int cardNum = 0;
    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);

        if (cardNum == 0) {
            parseCard1(card1_, parser, intW);
        } else if (cardNum == 1) {
            parseCard2(card2_, parser, intW, realW);
        } else if (cardNum == 2) {
            size_t pos = 0;
            tiebreakCard_.nfls = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            tiebreakCard_.sfls = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            tiebreakCard_.eraten = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            tiebreakCard_.erates = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            tiebreakCard_.ct2cn = parser.getDoubleAt(pos, realW).value_or(1.0);
            pos += realW;
            tiebreakCard_.option = static_cast<int>(parser.getInt64At(pos).value_or(0));
        }

        cardNum++;
        if (cardNum >= 3) break;
    }

    return true;
}

std::vector<std::string> ContactTiebreakNodesToSurface::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writeCard1(card1_, writer);
    result.push_back(writer.getLine());

    writer.clear();
    writeCard2(card2_, writer);
    result.push_back(writer.getLine());

    writer.clear();
    writer.writeDouble(tiebreakCard_.nfls);
    writer.writeDouble(tiebreakCard_.sfls);
    writer.writeDouble(tiebreakCard_.eraten);
    writer.writeDouble(tiebreakCard_.erates);
    writer.writeDouble(tiebreakCard_.ct2cn);
    writer.writeInt(tiebreakCard_.option);
    result.push_back(writer.getLine());

    return result;
}

void ContactTiebreakNodesToSurface::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ContactTiebreakNodesToSurface, "*CONTACT_TIEBREAK_NODES_TO_SURFACE")

// ============================================================================
// ContactTiebreakSurfaceToSurface
// ============================================================================

bool ContactTiebreakSurfaceToSurface::parse(const std::vector<std::string>& lines,
                                             util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    int cardNum = 0;
    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);

        if (cardNum == 0) {
            parseCard1(card1_, parser, intW);
        } else if (cardNum == 1) {
            parseCard2(card2_, parser, intW, realW);
        } else if (cardNum == 2) {
            size_t pos = 0;
            tiebreakCard_.nfls = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            tiebreakCard_.sfls = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            tiebreakCard_.eraten = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            tiebreakCard_.erates = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            tiebreakCard_.ct2cn = parser.getDoubleAt(pos, realW).value_or(1.0);
            pos += realW;
            tiebreakCard_.option = static_cast<int>(parser.getInt64At(pos).value_or(0));
        }

        cardNum++;
        if (cardNum >= 3) break;
    }

    return true;
}

std::vector<std::string> ContactTiebreakSurfaceToSurface::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writeCard1(card1_, writer);
    result.push_back(writer.getLine());

    writer.clear();
    writeCard2(card2_, writer);
    result.push_back(writer.getLine());

    writer.clear();
    writer.writeDouble(tiebreakCard_.nfls);
    writer.writeDouble(tiebreakCard_.sfls);
    writer.writeDouble(tiebreakCard_.eraten);
    writer.writeDouble(tiebreakCard_.erates);
    writer.writeDouble(tiebreakCard_.ct2cn);
    writer.writeInt(tiebreakCard_.option);
    result.push_back(writer.getLine());

    return result;
}

void ContactTiebreakSurfaceToSurface::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ContactTiebreakSurfaceToSurface, "*CONTACT_TIEBREAK_SURFACE_TO_SURFACE")

// ============================================================================
// ContactSlidingOnlyPenalty
// ============================================================================

bool ContactSlidingOnlyPenalty::parse(const std::vector<std::string>& lines,
                                       util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    int cardNum = 0;
    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);

        if (cardNum == 0) {
            parseCard1(card1_, parser, intW);
        } else if (cardNum == 1) {
            parseCard2(card2_, parser, intW, realW);
        }

        cardNum++;
        if (cardNum >= 2) break;
    }

    return true;
}

std::vector<std::string> ContactSlidingOnlyPenalty::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writeCard1(card1_, writer);
    result.push_back(writer.getLine());

    writer.clear();
    writeCard2(card2_, writer);
    result.push_back(writer.getLine());

    return result;
}

void ContactSlidingOnlyPenalty::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ContactSlidingOnlyPenalty, "*CONTACT_SLIDING_ONLY_PENALTY")

// ============================================================================
// ContactOption
// ============================================================================

bool ContactOption::parse(const std::vector<std::string>& lines,
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

        data_.cid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.option = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.value = parser.getDoubleAt(pos, realW).value_or(0.0);

        break;
    }

    return true;
}

std::vector<std::string> ContactOption::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.cid);
    writer.writeInt(data_.option);
    writer.writeDouble(data_.value);
    result.push_back(writer.getLine());

    return result;
}

void ContactOption::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ContactOption, "*CONTACT_OPTION")

// ============================================================================
// ContactAddWear
// ============================================================================

bool ContactAddWear::parse(const std::vector<std::string>& lines,
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

        data_.cid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.k1 = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.k2 = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));

        break;
    }

    return true;
}

std::vector<std::string> ContactAddWear::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.cid);
    writer.writeDouble(data_.k1);
    writer.writeDouble(data_.k2);
    writer.writeInt(data_.lcid);
    result.push_back(writer.getLine());

    return result;
}

void ContactAddWear::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ContactAddWear, "*CONTACT_ADD_WEAR")

// ============================================================================
// Contact2DAutomaticSurfaceToSurface
// ============================================================================

bool Contact2DAutomaticSurfaceToSurface::parse(const std::vector<std::string>& lines,
                                                util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    int cardNum = 0;
    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);

        if (cardNum == 0) {
            parseCard1(card1_, parser, intW);
        } else if (cardNum == 1) {
            parseCard2(card2_, parser, intW, realW);
        }

        cardNum++;
        if (cardNum >= 2) break;
    }

    return true;
}

std::vector<std::string> Contact2DAutomaticSurfaceToSurface::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writeCard1(card1_, writer);
    result.push_back(writer.getLine());

    writer.clear();
    writeCard2(card2_, writer);
    result.push_back(writer.getLine());

    return result;
}

void Contact2DAutomaticSurfaceToSurface::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(Contact2DAutomaticSurfaceToSurface, "*CONTACT_2D_AUTOMATIC_SURFACE_TO_SURFACE")

// ============================================================================
// Contact2DNodesToSurface
// ============================================================================

bool Contact2DNodesToSurface::parse(const std::vector<std::string>& lines,
                                     util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    int cardNum = 0;
    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);

        if (cardNum == 0) {
            parseCard1(card1_, parser, intW);
        } else if (cardNum == 1) {
            parseCard2(card2_, parser, intW, realW);
        }

        cardNum++;
        if (cardNum >= 2) break;
    }

    return true;
}

std::vector<std::string> Contact2DNodesToSurface::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writeCard1(card1_, writer);
    result.push_back(writer.getLine());

    writer.clear();
    writeCard2(card2_, writer);
    result.push_back(writer.getLine());

    return result;
}

void Contact2DNodesToSurface::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(Contact2DNodesToSurface, "*CONTACT_2D_NODES_TO_SURFACE")

} // namespace koo::dyna
