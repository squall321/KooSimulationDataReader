#include <koo/dyna/Integration.hpp>
#include <koo/dyna/ModelVisitor.hpp>
#include <koo/dyna/KeywordFactory.hpp>
#include <koo/util/StringUtils.hpp>

namespace koo::dyna {

// ============================================================================
// IntegrationShell
// ============================================================================

bool IntegrationShell::parse(const std::vector<std::string>& lines,
                              util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: IRID, NIP, S, PID, WF
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.irid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.nip = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.s = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.pid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.wf = parser.getDoubleAt(pos, realW).value_or(0.0);

    return true;
}

std::vector<std::string> IntegrationShell::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.irid);
    writer.writeInt(data_.nip);
    writer.writeDouble(data_.s);
    writer.writeInt(data_.pid);
    writer.writeDouble(data_.wf);
    result.push_back(writer.getLine());

    return result;
}

void IntegrationShell::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(IntegrationShell, "*INTEGRATION_SHELL")

// ============================================================================
// IntegrationBeam
// ============================================================================

bool IntegrationBeam::parse(const std::vector<std::string>& lines,
                             util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: IRID, NIP, RA, ICST, TS1, TS2
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.irid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.nip = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.ra = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.icst = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.ts1 = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.ts2 = parser.getDoubleAt(pos, realW).value_or(0.0);

    return true;
}

std::vector<std::string> IntegrationBeam::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.irid);
    writer.writeInt(data_.nip);
    writer.writeInt(data_.ra);
    writer.writeInt(data_.icst);
    writer.writeDouble(data_.ts1);
    writer.writeDouble(data_.ts2);
    result.push_back(writer.getLine());

    return result;
}

void IntegrationBeam::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(IntegrationBeam, "*INTEGRATION_BEAM")

// ============================================================================
// IntegrationSolid
// ============================================================================

bool IntegrationSolid::parse(const std::vector<std::string>& lines,
                              util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: IRID, NIP, NXDOF, NHSV
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.irid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.nip = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.nxdof = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.nhsv = static_cast<int>(parser.getInt64At(pos).value_or(0));

    return true;
}

std::vector<std::string> IntegrationSolid::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.irid);
    writer.writeInt(data_.nip);
    writer.writeInt(data_.nxdof);
    writer.writeInt(data_.nhsv);
    result.push_back(writer.getLine());

    return result;
}

void IntegrationSolid::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(IntegrationSolid, "*INTEGRATION_SOLID")

// ============================================================================
// IntegrationPointTransform
// ============================================================================

bool IntegrationPointTransform::parse(const std::vector<std::string>& lines,
                                       util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: IRID, ITYPE, XI1, XI2, XI3, WGT
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.irid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.itype = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.xi1 = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.xi2 = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.xi3 = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.wgt = parser.getDoubleAt(pos, realW).value_or(0.0);

    return true;
}

std::vector<std::string> IntegrationPointTransform::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.irid);
    writer.writeInt(data_.itype);
    writer.writeDouble(data_.xi1);
    writer.writeDouble(data_.xi2);
    writer.writeDouble(data_.xi3);
    writer.writeDouble(data_.wgt);
    result.push_back(writer.getLine());

    return result;
}

void IntegrationPointTransform::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(IntegrationPointTransform, "*INTEGRATION_POINT_TRANSFORM")

// ============================================================================
// IntegrationShellThickness
// ============================================================================

bool IntegrationShellThickness::parse(const std::vector<std::string>& lines,
                                       util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: IRID, NIP, T1, WF1, T2, WF2
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.irid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.nip = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.t1 = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.wf1 = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.t2 = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.wf2 = parser.getDoubleAt(pos, realW).value_or(0.0);

    return true;
}

std::vector<std::string> IntegrationShellThickness::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.irid);
    writer.writeInt(data_.nip);
    writer.writeDouble(data_.t1);
    writer.writeDouble(data_.wf1);
    writer.writeDouble(data_.t2);
    writer.writeDouble(data_.wf2);
    result.push_back(writer.getLine());

    return result;
}

void IntegrationShellThickness::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(IntegrationShellThickness, "*INTEGRATION_SHELL_THICKNESS")

// ============================================================================
// IntegrationTshell
// ============================================================================

bool IntegrationTshell::parse(const std::vector<std::string>& lines,
                               util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: IRID, NIP, NT, NI
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.irid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.nip = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.nt = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.ni = static_cast<int>(parser.getInt64At(pos).value_or(0));

    return true;
}

std::vector<std::string> IntegrationTshell::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.irid);
    writer.writeInt(data_.nip);
    writer.writeInt(data_.nt);
    writer.writeInt(data_.ni);
    result.push_back(writer.getLine());

    return result;
}

void IntegrationTshell::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(IntegrationTshell, "*INTEGRATION_TSHELL")

// ============================================================================
// IntegrationPoint
// ============================================================================

bool IntegrationPoint::parse(const std::vector<std::string>& lines,
                              util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: IRID, PTID, XI, ETA, ZETA, WGT
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.irid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.ptid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.xi = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.eta = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.zeta = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.wgt = parser.getDoubleAt(pos, realW).value_or(1.0);

    return true;
}

std::vector<std::string> IntegrationPoint::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.irid);
    writer.writeInt(data_.ptid);
    writer.writeDouble(data_.xi);
    writer.writeDouble(data_.eta);
    writer.writeDouble(data_.zeta);
    writer.writeDouble(data_.wgt);
    result.push_back(writer.getLine());

    return result;
}

void IntegrationPoint::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(IntegrationPoint, "*INTEGRATION_POINT")

} // namespace koo::dyna
