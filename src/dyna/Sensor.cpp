#include <koo/dyna/Sensor.hpp>
#include <koo/dyna/ModelVisitor.hpp>
#include <koo/dyna/KeywordFactory.hpp>
#include <koo/util/StringUtils.hpp>

namespace koo::dyna {

// ============================================================================
// SensorDefineNode
// ============================================================================

bool SensorDefineNode::parse(const std::vector<std::string>& lines,
                              util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: SENSID, NID, CTYPE, DIR
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.sensid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.nid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.ctype = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.dir = static_cast<int>(parser.getInt64At(pos).value_or(0));

    return true;
}

std::vector<std::string> SensorDefineNode::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.sensid);
    writer.writeInt(data_.nid);
    writer.writeInt(data_.ctype);
    writer.writeInt(data_.dir);
    result.push_back(writer.getLine());

    return result;
}

void SensorDefineNode::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(SensorDefineNode, "*SENSOR_DEFINE_NODE")

// ============================================================================
// SensorDefineElement
// ============================================================================

bool SensorDefineElement::parse(const std::vector<std::string>& lines,
                                 util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: SENSID, EID, CTYPE, COMP
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.sensid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.eid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.ctype = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.comp = static_cast<int>(parser.getInt64At(pos).value_or(0));

    return true;
}

std::vector<std::string> SensorDefineElement::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.sensid);
    writer.writeInt(data_.eid);
    writer.writeInt(data_.ctype);
    writer.writeInt(data_.comp);
    result.push_back(writer.getLine());

    return result;
}

void SensorDefineElement::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(SensorDefineElement, "*SENSOR_DEFINE_ELEMENT")

// ============================================================================
// SensorSwitch
// ============================================================================

bool SensorSwitch::parse(const std::vector<std::string>& lines,
                          util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: SWID, SENSID, TYPEID, VALUE, OUTPUT
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.swid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.sensid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.typeid_ = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.value = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.output = static_cast<int>(parser.getInt64At(pos).value_or(0));

    return true;
}

std::vector<std::string> SensorSwitch::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.swid);
    writer.writeInt(data_.sensid);
    writer.writeInt(data_.typeid_);
    writer.writeDouble(data_.value);
    writer.writeInt(data_.output);
    result.push_back(writer.getLine());

    return result;
}

void SensorSwitch::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(SensorSwitch, "*SENSOR_SWITCH")

// ============================================================================
// SensorControl
// ============================================================================

bool SensorControl::parse(const std::vector<std::string>& lines,
                           util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: CNTLID, SWID, BIRTH, BIRTHTIME, DEATH, DEATHTIME
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.cntlid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.swid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.birth = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.birthtime = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.death = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.deathtime = parser.getDoubleAt(pos, realW).value_or(0.0);

    return true;
}

std::vector<std::string> SensorControl::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.cntlid);
    writer.writeInt(data_.swid);
    writer.writeInt(data_.birth);
    writer.writeDouble(data_.birthtime);
    writer.writeInt(data_.death);
    writer.writeDouble(data_.deathtime);
    result.push_back(writer.getLine());

    return result;
}

void SensorControl::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(SensorControl, "*SENSOR_CONTROL")

// ============================================================================
// SensorDefineForce
// ============================================================================

bool SensorDefineForce::parse(const std::vector<std::string>& lines,
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

    data_.sensid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.ftype = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.id1 = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.id2 = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.cid = static_cast<int>(parser.getInt64At(pos).value_or(0));

    return true;
}

std::vector<std::string> SensorDefineForce::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.sensid);
    writer.writeInt(data_.ftype);
    writer.writeInt(data_.id1);
    writer.writeInt(data_.id2);
    writer.writeInt(data_.cid);
    result.push_back(writer.getLine());

    return result;
}

void SensorDefineForce::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(SensorDefineForce, "*SENSOR_DEFINE_FORCE")

// ============================================================================
// SensorDefineMisc
// ============================================================================

bool SensorDefineMisc::parse(const std::vector<std::string>& lines,
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

    data_.sensid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.mtype = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.id = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.vid = static_cast<int>(parser.getInt64At(pos).value_or(0));

    return true;
}

std::vector<std::string> SensorDefineMisc::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.sensid);
    writer.writeInt(data_.mtype);
    writer.writeInt(data_.id);
    writer.writeInt(data_.vid);
    result.push_back(writer.getLine());

    return result;
}

void SensorDefineMisc::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(SensorDefineMisc, "*SENSOR_DEFINE_MISC")

// ============================================================================
// SensorDefineCalcMath
// ============================================================================

bool SensorDefineCalcMath::parse(const std::vector<std::string>& lines,
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

    data_.sensid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.mathop = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.sens1 = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.sens2 = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.const1 = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.const2 = parser.getDoubleAt(pos, realW).value_or(0.0);

    return true;
}

std::vector<std::string> SensorDefineCalcMath::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.sensid);
    writer.writeInt(data_.mathop);
    writer.writeInt(data_.sens1);
    writer.writeInt(data_.sens2);
    writer.writeDouble(data_.const1);
    writer.writeDouble(data_.const2);
    result.push_back(writer.getLine());

    return result;
}

void SensorDefineCalcMath::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(SensorDefineCalcMath, "*SENSOR_DEFINE_CALC_MATH")

// ============================================================================
// SensorSwitchCalcLogic
// ============================================================================

bool SensorSwitchCalcLogic::parse(const std::vector<std::string>& lines,
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

    data_.swid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.logicop = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.sw1 = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.sw2 = static_cast<int>(parser.getInt64At(pos).value_or(0));

    return true;
}

std::vector<std::string> SensorSwitchCalcLogic::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.swid);
    writer.writeInt(data_.logicop);
    writer.writeInt(data_.sw1);
    writer.writeInt(data_.sw2);
    result.push_back(writer.getLine());

    return result;
}

void SensorSwitchCalcLogic::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(SensorSwitchCalcLogic, "*SENSOR_SWITCH_CALC_LOGIC")

// ============================================================================
// SensorDefineFunction
// ============================================================================

bool SensorDefineFunction::parse(const std::vector<std::string>& lines,
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

    data_.sensid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.funcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.arg1 = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.arg2 = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.arg3 = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.arg4 = static_cast<int>(parser.getInt64At(pos).value_or(0));

    return true;
}

std::vector<std::string> SensorDefineFunction::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.sensid);
    writer.writeInt(data_.funcid);
    writer.writeInt(data_.arg1);
    writer.writeInt(data_.arg2);
    writer.writeInt(data_.arg3);
    writer.writeInt(data_.arg4);
    result.push_back(writer.getLine());

    return result;
}

void SensorDefineFunction::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(SensorDefineFunction, "*SENSOR_DEFINE_FUNCTION")

// ============================================================================
// SensorDefineCoordinate
// ============================================================================

bool SensorDefineCoordinate::parse(const std::vector<std::string>& lines,
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

    data_.sensid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.nid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.axis = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.cid = static_cast<int>(parser.getInt64At(pos).value_or(0));

    return true;
}

std::vector<std::string> SensorDefineCoordinate::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.sensid);
    writer.writeInt(data_.nid);
    writer.writeInt(data_.axis);
    writer.writeInt(data_.cid);
    result.push_back(writer.getLine());

    return result;
}

void SensorDefineCoordinate::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(SensorDefineCoordinate, "*SENSOR_DEFINE_COORDINATE")

// ============================================================================
// SensorSwitchShellToSolid
// ============================================================================

bool SensorSwitchShellToSolid::parse(const std::vector<std::string>& lines,
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

    data_.swid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.pid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.method = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.thick = parser.getDoubleAt(pos, realW).value_or(0.0);

    return true;
}

std::vector<std::string> SensorSwitchShellToSolid::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.swid);
    writer.writeInt(data_.pid);
    writer.writeInt(data_.method);
    writer.writeDouble(data_.thick);
    result.push_back(writer.getLine());

    return result;
}

void SensorSwitchShellToSolid::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(SensorSwitchShellToSolid, "*SENSOR_SWITCH_SHELL_TO_SOLID")

} // namespace koo::dyna
