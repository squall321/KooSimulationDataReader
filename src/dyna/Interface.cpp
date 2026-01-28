#include <koo/dyna/Interface.hpp>
#include <koo/dyna/ModelVisitor.hpp>
#include <koo/dyna/KeywordFactory.hpp>
#include <koo/util/StringUtils.hpp>

namespace koo::dyna {

// ============================================================================
// InterfaceLinkingDiscrete
// ============================================================================

bool InterfaceLinkingDiscrete::parse(const std::vector<std::string>& lines,
                                      util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: IFID, SLAVSET, MASTSET, CTYPE, STIFF, DAMP
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.ifid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.slavset = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.mastset = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.ctype = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.stiff = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.damp = parser.getDoubleAt(pos, realW).value_or(0.0);

    return true;
}

std::vector<std::string> InterfaceLinkingDiscrete::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.ifid);
    writer.writeInt(data_.slavset);
    writer.writeInt(data_.mastset);
    writer.writeInt(data_.ctype);
    writer.writeDouble(data_.stiff);
    writer.writeDouble(data_.damp);
    result.push_back(writer.getLine());

    return result;
}

void InterfaceLinkingDiscrete::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(InterfaceLinkingDiscrete, "*INTERFACE_LINKING_DISCRETE")

// ============================================================================
// InterfaceSpringbackSeamless
// ============================================================================

bool InterfaceSpringbackSeamless::parse(const std::vector<std::string>& lines,
                                         util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: PSID, PTYPE, FTYPE, NSHV
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.psid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.ptype = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.ftype = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.nshv = static_cast<int>(parser.getInt64At(pos).value_or(0));

    return true;
}

std::vector<std::string> InterfaceSpringbackSeamless::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.psid);
    writer.writeInt(data_.ptype);
    writer.writeInt(data_.ftype);
    writer.writeInt(data_.nshv);
    result.push_back(writer.getLine());

    return result;
}

void InterfaceSpringbackSeamless::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(InterfaceSpringbackSeamless, "*INTERFACE_SPRINGBACK_SEAMLESS")

// ============================================================================
// InterfaceCompensationNew
// ============================================================================

bool InterfaceCompensationNew::parse(const std::vector<std::string>& lines,
                                      util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: PSID, METHOD, SCALE, NITER, TOL
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.psid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.method = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.scale = parser.getDoubleAt(pos, realW).value_or(1.0);
    pos += realW;
    data_.niter = static_cast<int>(parser.getInt64At(pos).value_or(1));
    pos += intW;
    data_.tol = parser.getDoubleAt(pos, realW).value_or(0.0);

    return true;
}

std::vector<std::string> InterfaceCompensationNew::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.psid);
    writer.writeInt(data_.method);
    writer.writeDouble(data_.scale);
    writer.writeInt(data_.niter);
    writer.writeDouble(data_.tol);
    result.push_back(writer.getLine());

    return result;
}

void InterfaceCompensationNew::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(InterfaceCompensationNew, "*INTERFACE_COMPENSATION_NEW")

// ============================================================================
// InterfaceBlanksize
// ============================================================================

bool InterfaceBlanksize::parse(const std::vector<std::string>& lines,
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

    data_.id = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.psid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.method = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.scale = parser.getDoubleAt(pos, realW).value_or(1.0);
    pos += realW;
    data_.niter = static_cast<int>(parser.getInt64At(pos).value_or(1));

    return true;
}

std::vector<std::string> InterfaceBlanksize::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.id);
    writer.writeInt(data_.psid);
    writer.writeInt(data_.method);
    writer.writeDouble(data_.scale);
    writer.writeInt(data_.niter);
    result.push_back(writer.getLine());

    return result;
}

void InterfaceBlanksize::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(InterfaceBlanksize, "*INTERFACE_BLANKSIZE")

// ============================================================================
// InterfaceComponent
// ============================================================================

bool InterfaceComponent::parse(const std::vector<std::string>& lines,
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

    data_.id = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.compid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.type = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.pid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.thick = parser.getDoubleAt(pos, realW).value_or(0.0);

    return true;
}

std::vector<std::string> InterfaceComponent::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.id);
    writer.writeInt(data_.compid);
    writer.writeInt(data_.type);
    writer.writeInt(data_.pid);
    writer.writeDouble(data_.thick);
    result.push_back(writer.getLine());

    return result;
}

void InterfaceComponent::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(InterfaceComponent, "*INTERFACE_COMPONENT")

// ============================================================================
// InterfaceSpringbackLsdyna
// ============================================================================

bool InterfaceSpringbackLsdyna::parse(const std::vector<std::string>& lines,
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

    data_.psid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.ptype = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.thickflag = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.strsflag = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.histflag = static_cast<int>(parser.getInt64At(pos).value_or(0));

    return true;
}

std::vector<std::string> InterfaceSpringbackLsdyna::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.psid);
    writer.writeInt(data_.ptype);
    writer.writeInt(data_.thickflag);
    writer.writeInt(data_.strsflag);
    writer.writeInt(data_.histflag);
    result.push_back(writer.getLine());

    return result;
}

void InterfaceSpringbackLsdyna::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(InterfaceSpringbackLsdyna, "*INTERFACE_SPRINGBACK_LSDYNA")

// ============================================================================
// InterfaceSsi
// ============================================================================

bool InterfaceSsi::parse(const std::vector<std::string>& lines,
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

    data_.id = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.ssid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.msid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.fric = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.fs = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.fd = parser.getDoubleAt(pos, realW).value_or(0.0);

    return true;
}

std::vector<std::string> InterfaceSsi::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.id);
    writer.writeInt(data_.ssid);
    writer.writeInt(data_.msid);
    writer.writeInt(data_.fric);
    writer.writeDouble(data_.fs);
    writer.writeDouble(data_.fd);
    result.push_back(writer.getLine());

    return result;
}

void InterfaceSsi::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(InterfaceSsi, "*INTERFACE_SSI")

// ============================================================================
// InterfaceLinking
// ============================================================================

bool InterfaceLinking::parse(const std::vector<std::string>& lines,
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

    data_.id = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.slave = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.master = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.stype = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.mtype = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.pen = parser.getDoubleAt(pos, realW).value_or(0.0);

    return true;
}

std::vector<std::string> InterfaceLinking::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.id);
    writer.writeInt(data_.slave);
    writer.writeInt(data_.master);
    writer.writeInt(data_.stype);
    writer.writeInt(data_.mtype);
    writer.writeDouble(data_.pen);
    result.push_back(writer.getLine());

    return result;
}

void InterfaceLinking::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(InterfaceLinking, "*INTERFACE_LINKING")

// ============================================================================
// InterfaceFrictionalSlideline
// ============================================================================

bool InterfaceFrictionalSlideline::parse(const std::vector<std::string>& lines,
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

    data_.id = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.ssid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.msid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.fs = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.fd = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.dc = parser.getDoubleAt(pos, realW).value_or(0.0);

    return true;
}

std::vector<std::string> InterfaceFrictionalSlideline::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.id);
    writer.writeInt(data_.ssid);
    writer.writeInt(data_.msid);
    writer.writeDouble(data_.fs);
    writer.writeDouble(data_.fd);
    writer.writeDouble(data_.dc);
    result.push_back(writer.getLine());

    return result;
}

void InterfaceFrictionalSlideline::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(InterfaceFrictionalSlideline, "*INTERFACE_FRICTIONAL_SLIDELINE")

} // namespace koo::dyna
