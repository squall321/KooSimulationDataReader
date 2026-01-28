#include <koo/dyna/Load.hpp>
#include <koo/dyna/ModelVisitor.hpp>
#include <koo/dyna/KeywordFactory.hpp>
#include <koo/util/StringUtils.hpp>

namespace koo::dyna {

// ============================================================================
// LoadNodePoint
// ============================================================================

bool LoadNodePoint::parse(const std::vector<std::string>& lines,
                           util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    loads_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        NodeLoadData load;
        size_t pos = 0;

        load.nid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        load.dof = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        load.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        load.sf = parser.getDoubleAt(pos, realW).value_or(1.0);
        pos += realW;
        load.cid = static_cast<int>(parser.getInt64At(pos).value_or(0));

        if (load.nid != 0) {
            loads_.push_back(load);
        }
    }

    return true;
}

std::vector<std::string> LoadNodePoint::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& load : loads_) {
        writer.clear();
        writer.writeInt(load.nid);
        writer.writeInt(load.dof);
        writer.writeInt(load.lcid);
        writer.writeDouble(load.sf);
        writer.writeInt(load.cid);
        result.push_back(writer.getLine());
    }

    return result;
}

void LoadNodePoint::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(LoadNodePoint, "*LOAD_NODE_POINT")

// ============================================================================
// LoadNodeSet
// ============================================================================

bool LoadNodeSet::parse(const std::vector<std::string>& lines,
                         util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    loads_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        Data load;
        size_t pos = 0;

        load.nsid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        load.dof = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        load.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        load.sf = parser.getDoubleAt(pos, realW).value_or(1.0);
        pos += realW;
        load.cid = static_cast<int>(parser.getInt64At(pos).value_or(0));

        if (load.nsid != 0) {
            loads_.push_back(load);
        }
    }

    return true;
}

std::vector<std::string> LoadNodeSet::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& load : loads_) {
        writer.clear();
        writer.writeInt(load.nsid);
        writer.writeInt(load.dof);
        writer.writeInt(load.lcid);
        writer.writeDouble(load.sf);
        writer.writeInt(load.cid);
        result.push_back(writer.getLine());
    }

    return result;
}

void LoadNodeSet::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(LoadNodeSet, "*LOAD_NODE_SET")

// ============================================================================
// LoadBodyZ
// ============================================================================

bool LoadBodyZ::parse(const std::vector<std::string>& lines,
                       util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;

        data_.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.sf = parser.getDoubleAt(pos, realW).value_or(1.0);
        pos += realW;
        data_.lciddr = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.xc = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.yc = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.zc = parser.getDoubleAt(pos, realW).value_or(0.0);
        break;
    }

    return true;
}

std::vector<std::string> LoadBodyZ::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.lcid);
    writer.writeDouble(data_.sf);
    writer.writeInt(data_.lciddr);
    writer.writeDouble(data_.xc);
    writer.writeDouble(data_.yc);
    writer.writeDouble(data_.zc);

    result.push_back(writer.getLine());
    return result;
}

void LoadBodyZ::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(LoadBodyZ, "*LOAD_BODY_Z")

// ============================================================================
// LoadBodyX
// ============================================================================

bool LoadBodyX::parse(const std::vector<std::string>& lines,
                       util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;

        data_.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.sf = parser.getDoubleAt(pos, realW).value_or(1.0);
        pos += realW;
        data_.lciddr = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.xc = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.yc = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.zc = parser.getDoubleAt(pos, realW).value_or(0.0);
        break;
    }

    return true;
}

std::vector<std::string> LoadBodyX::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.lcid);
    writer.writeDouble(data_.sf);
    writer.writeInt(data_.lciddr);
    writer.writeDouble(data_.xc);
    writer.writeDouble(data_.yc);
    writer.writeDouble(data_.zc);

    result.push_back(writer.getLine());
    return result;
}

void LoadBodyX::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(LoadBodyX, "*LOAD_BODY_X")

// ============================================================================
// LoadBodyY
// ============================================================================

bool LoadBodyY::parse(const std::vector<std::string>& lines,
                       util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;

        data_.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.sf = parser.getDoubleAt(pos, realW).value_or(1.0);
        pos += realW;
        data_.lciddr = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.xc = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.yc = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.zc = parser.getDoubleAt(pos, realW).value_or(0.0);
        break;
    }

    return true;
}

std::vector<std::string> LoadBodyY::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.lcid);
    writer.writeDouble(data_.sf);
    writer.writeInt(data_.lciddr);
    writer.writeDouble(data_.xc);
    writer.writeDouble(data_.yc);
    writer.writeDouble(data_.zc);

    result.push_back(writer.getLine());
    return result;
}

void LoadBodyY::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(LoadBodyY, "*LOAD_BODY_Y")

// ============================================================================
// LoadSegment
// ============================================================================

bool LoadSegment::parse(const std::vector<std::string>& lines,
                         util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    loads_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        SegmentLoadData load;
        size_t pos = 0;

        load.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        load.sf = parser.getDoubleAt(pos, realW).value_or(1.0);
        pos += realW;
        load.at = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        load.n1 = parser.getInt64At(pos).value_or(0);
        pos += intW;
        load.n2 = parser.getInt64At(pos).value_or(0);
        pos += intW;
        load.n3 = parser.getInt64At(pos).value_or(0);
        pos += intW;
        load.n4 = parser.getInt64At(pos).value_or(0);

        if (load.lcid != 0 || load.n1 != 0) {
            loads_.push_back(load);
        }
    }

    return true;
}

std::vector<std::string> LoadSegment::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& load : loads_) {
        writer.clear();
        writer.writeInt(load.lcid);
        writer.writeDouble(load.sf);
        writer.writeDouble(load.at);
        writer.writeInt(load.n1);
        writer.writeInt(load.n2);
        writer.writeInt(load.n3);
        writer.writeInt(load.n4);
        result.push_back(writer.getLine());
    }

    return result;
}

void LoadSegment::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(LoadSegment, "*LOAD_SEGMENT")

// ============================================================================
// LoadSegmentSet
// ============================================================================

bool LoadSegmentSet::parse(const std::vector<std::string>& lines,
                            util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    loads_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        Data load;
        size_t pos = 0;

        load.ssid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        load.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        load.sf = parser.getDoubleAt(pos, realW).value_or(1.0);
        pos += realW;
        load.at = parser.getDoubleAt(pos, realW).value_or(0.0);

        if (load.ssid != 0) {
            loads_.push_back(load);
        }
    }

    return true;
}

std::vector<std::string> LoadSegmentSet::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& load : loads_) {
        writer.clear();
        writer.writeInt(load.ssid);
        writer.writeInt(load.lcid);
        writer.writeDouble(load.sf);
        writer.writeDouble(load.at);
        result.push_back(writer.getLine());
    }

    return result;
}

void LoadSegmentSet::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(LoadSegmentSet, "*LOAD_SEGMENT_SET")

// ============================================================================
// LoadShellSet
// ============================================================================

bool LoadShellSet::parse(const std::vector<std::string>& lines,
                          util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    loads_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        Data load;
        size_t pos = 0;

        load.esid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        load.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        load.sf = parser.getDoubleAt(pos, realW).value_or(1.0);
        pos += realW;
        load.at = parser.getDoubleAt(pos, realW).value_or(0.0);

        if (load.esid != 0) {
            loads_.push_back(load);
        }
    }

    return true;
}

std::vector<std::string> LoadShellSet::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& load : loads_) {
        writer.clear();
        writer.writeInt(load.esid);
        writer.writeInt(load.lcid);
        writer.writeDouble(load.sf);
        writer.writeDouble(load.at);
        result.push_back(writer.getLine());
    }

    return result;
}

void LoadShellSet::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(LoadShellSet, "*LOAD_SHELL_SET")

// ============================================================================
// LoadRigidBody
// ============================================================================

bool LoadRigidBody::parse(const std::vector<std::string>& lines,
                           util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    loads_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        Data load;
        size_t pos = 0;

        load.pid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        load.dof = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        load.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        load.sf = parser.getDoubleAt(pos, realW).value_or(1.0);
        pos += realW;
        load.cid = static_cast<int>(parser.getInt64At(pos).value_or(0));

        if (load.pid != 0) {
            loads_.push_back(load);
        }
    }

    return true;
}

std::vector<std::string> LoadRigidBody::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& load : loads_) {
        writer.clear();
        writer.writeInt(load.pid);
        writer.writeInt(load.dof);
        writer.writeInt(load.lcid);
        writer.writeDouble(load.sf);
        writer.writeInt(load.cid);
        result.push_back(writer.getLine());
    }

    return result;
}

void LoadRigidBody::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(LoadRigidBody, "*LOAD_RIGID_BODY")

// ============================================================================
// LoadThermalVariable
// ============================================================================

bool LoadThermalVariable::parse(const std::vector<std::string>& lines,
                                 util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;

        data_.nsid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.sf = parser.getDoubleAt(pos, realW).value_or(1.0);
        pos += realW;
        data_.death = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> LoadThermalVariable::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.nsid);
    writer.writeInt(data_.lcid);
    writer.writeDouble(data_.sf);
    writer.writeInt(data_.death);

    result.push_back(writer.getLine());
    return result;
}

void LoadThermalVariable::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(LoadThermalVariable, "*LOAD_THERMAL_VARIABLE")

// ============================================================================
// LoadMotionNode
// ============================================================================

bool LoadMotionNode::parse(const std::vector<std::string>& lines,
                            util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    loads_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        Data load;
        size_t pos = 0;

        load.nid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        load.dof = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        load.vad = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        load.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        load.sf = parser.getDoubleAt(pos, realW).value_or(1.0);
        pos += realW;
        load.vid = static_cast<int>(parser.getInt64At(pos).value_or(0));

        if (load.nid != 0) {
            loads_.push_back(load);
        }
    }

    return true;
}

std::vector<std::string> LoadMotionNode::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& load : loads_) {
        writer.clear();
        writer.writeInt(load.nid);
        writer.writeInt(load.dof);
        writer.writeInt(load.vad);
        writer.writeInt(load.lcid);
        writer.writeDouble(load.sf);
        writer.writeInt(load.vid);
        result.push_back(writer.getLine());
    }

    return result;
}

void LoadMotionNode::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(LoadMotionNode, "*LOAD_MOTION_NODE")

// ============================================================================
// LoadBeamSet
// ============================================================================

bool LoadBeamSet::parse(const std::vector<std::string>& lines,
                         util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    loads_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        Data load;
        size_t pos = 0;

        load.bsid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        load.dir = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        load.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        load.sf = parser.getDoubleAt(pos, realW).value_or(1.0);
        pos += realW;
        load.cid = static_cast<int>(parser.getInt64At(pos).value_or(0));

        if (load.bsid != 0) {
            loads_.push_back(load);
        }
    }

    return true;
}

std::vector<std::string> LoadBeamSet::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& load : loads_) {
        writer.clear();
        writer.writeInt(load.bsid);
        writer.writeInt(load.dir);
        writer.writeInt(load.lcid);
        writer.writeDouble(load.sf);
        writer.writeInt(load.cid);
        result.push_back(writer.getLine());
    }

    return result;
}

void LoadBeamSet::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(LoadBeamSet, "*LOAD_BEAM_SET")

// ============================================================================
// LoadBodyParts
// ============================================================================

bool LoadBodyParts::parse(const std::vector<std::string>& lines,
                           util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;

        data_.psid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.lcidx = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.lcidy = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.lcidz = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.sfx = parser.getDoubleAt(pos, realW).value_or(1.0);
        pos += realW;
        data_.sfy = parser.getDoubleAt(pos, realW).value_or(1.0);
        pos += realW;
        data_.sfz = parser.getDoubleAt(pos, realW).value_or(1.0);
        break;
    }

    return true;
}

std::vector<std::string> LoadBodyParts::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.psid);
    writer.writeInt(data_.lcidx);
    writer.writeInt(data_.lcidy);
    writer.writeInt(data_.lcidz);
    writer.writeDouble(data_.sfx);
    writer.writeDouble(data_.sfy);
    writer.writeDouble(data_.sfz);

    result.push_back(writer.getLine());
    return result;
}

void LoadBodyParts::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(LoadBodyParts, "*LOAD_BODY_PARTS")

// ============================================================================
// LoadThermalConstant
// ============================================================================

bool LoadThermalConstant::parse(const std::vector<std::string>& lines,
                                 util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;

        data_.nsid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.temp = parser.getDoubleAt(pos, realW).value_or(0.0);
        break;
    }

    return true;
}

std::vector<std::string> LoadThermalConstant::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.nsid);
    writer.writeDouble(data_.temp);

    result.push_back(writer.getLine());
    return result;
}

void LoadThermalConstant::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(LoadThermalConstant, "*LOAD_THERMAL_CONSTANT")

// ============================================================================
// LoadGravityPart
// ============================================================================

bool LoadGravityPart::parse(const std::vector<std::string>& lines,
                             util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    data_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        Data entry;
        size_t pos = 0;

        entry.pid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.n1 = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.n2 = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.sf = parser.getDoubleAt(pos, realW).value_or(1.0);
        pos += realW;
        entry.cid = static_cast<int>(parser.getInt64At(pos).value_or(0));

        if (entry.pid != 0) {
            data_.push_back(entry);
        }
    }

    return true;
}

std::vector<std::string> LoadGravityPart::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& entry : data_) {
        writer.clear();
        writer.writeInt(entry.pid);
        writer.writeInt(entry.n1);
        writer.writeInt(entry.n2);
        writer.writeInt(entry.lcid);
        writer.writeDouble(entry.sf);
        writer.writeInt(entry.cid);
        result.push_back(writer.getLine());
    }

    return result;
}

void LoadGravityPart::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(LoadGravityPart, "*LOAD_GRAVITY_PART")

// ============================================================================
// LoadDensityDepth
// ============================================================================

bool LoadDensityDepth::parse(const std::vector<std::string>& lines,
                              util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    data_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        Data entry;
        size_t pos = 0;

        entry.pid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.d = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        entry.rho = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        entry.c = parser.getDoubleAt(pos, realW).value_or(0.0);

        if (entry.pid != 0) {
            data_.push_back(entry);
        }
    }

    return true;
}

std::vector<std::string> LoadDensityDepth::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& entry : data_) {
        writer.clear();
        writer.writeInt(entry.pid);
        writer.writeDouble(entry.d);
        writer.writeDouble(entry.rho);
        writer.writeDouble(entry.c);
        result.push_back(writer.getLine());
    }

    return result;
}

void LoadDensityDepth::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(LoadDensityDepth, "*LOAD_DENSITY_DEPTH")

// ============================================================================
// LoadSeismicSsi
// ============================================================================

bool LoadSeismicSsi::parse(const std::vector<std::string>& lines,
                            util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;

        data_.ssid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.gmset = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.sf = static_cast<int>(parser.getInt64At(pos).value_or(1));
        pos += intW;
        data_.btype = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> LoadSeismicSsi::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.ssid);
    writer.writeInt(data_.gmset);
    writer.writeInt(data_.sf);
    writer.writeInt(data_.btype);

    result.push_back(writer.getLine());
    return result;
}

void LoadSeismicSsi::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(LoadSeismicSsi, "*LOAD_SEISMIC_SSI")

// ============================================================================
// LoadBlastEnhanced
// ============================================================================

bool LoadBlastEnhanced::parse(const std::vector<std::string>& lines,
                               util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;

        data_.bid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.m = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.xbo = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.ybo = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.zbo = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.tbo = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.unit = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.blast = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> LoadBlastEnhanced::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.bid);
    writer.writeDouble(data_.m);
    writer.writeDouble(data_.xbo);
    writer.writeDouble(data_.ybo);
    writer.writeDouble(data_.zbo);
    writer.writeDouble(data_.tbo);
    writer.writeInt(data_.unit);
    writer.writeInt(data_.blast);

    result.push_back(writer.getLine());
    return result;
}

void LoadBlastEnhanced::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(LoadBlastEnhanced, "*LOAD_BLAST_ENHANCED")

// ============================================================================
// LoadSpcForce
// ============================================================================

bool LoadSpcForce::parse(const std::vector<std::string>& lines,
                          util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    data_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        Data entry;
        size_t pos = 0;

        entry.nsid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.dof = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.sf = parser.getDoubleAt(pos, realW).value_or(1.0);
        pos += realW;
        entry.cid = static_cast<int>(parser.getInt64At(pos).value_or(0));

        if (entry.nsid != 0) {
            data_.push_back(entry);
        }
    }

    return true;
}

std::vector<std::string> LoadSpcForce::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& entry : data_) {
        writer.clear();
        writer.writeInt(entry.nsid);
        writer.writeInt(entry.dof);
        writer.writeInt(entry.lcid);
        writer.writeDouble(entry.sf);
        writer.writeInt(entry.cid);
        result.push_back(writer.getLine());
    }

    return result;
}

void LoadSpcForce::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(LoadSpcForce, "*LOAD_SPCFORCE")

// ============================================================================
// LoadSurfaceStress
// ============================================================================

bool LoadSurfaceStress::parse(const std::vector<std::string>& lines,
                               util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    data_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        Data entry;
        size_t pos = 0;

        entry.ssid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.lcidxx = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.lcidyy = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.lcidzz = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.lcidxy = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.lcidyz = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.lcidzx = static_cast<int>(parser.getInt64At(pos).value_or(0));

        if (entry.ssid != 0) {
            data_.push_back(entry);
        }
    }

    return true;
}

std::vector<std::string> LoadSurfaceStress::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& entry : data_) {
        writer.clear();
        writer.writeInt(entry.ssid);
        writer.writeInt(entry.lcidxx);
        writer.writeInt(entry.lcidyy);
        writer.writeInt(entry.lcidzz);
        writer.writeInt(entry.lcidxy);
        writer.writeInt(entry.lcidyz);
        writer.writeInt(entry.lcidzx);
        result.push_back(writer.getLine());
    }

    return result;
}

void LoadSurfaceStress::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(LoadSurfaceStress, "*LOAD_SURFACE_STRESS")

// ============================================================================
// LoadMovingPressure
// ============================================================================

bool LoadMovingPressure::parse(const std::vector<std::string>& lines,
                                util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;

        data_.pid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.sf = parser.getDoubleAt(pos, realW).value_or(1.0);
        pos += realW;
        data_.dirl = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.vel = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.lcidv = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> LoadMovingPressure::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.pid);
    writer.writeInt(data_.lcid);
    writer.writeDouble(data_.sf);
    writer.writeInt(data_.dirl);
    writer.writeDouble(data_.vel);
    writer.writeInt(data_.lcidv);

    result.push_back(writer.getLine());
    return result;
}

void LoadMovingPressure::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(LoadMovingPressure, "*LOAD_MOVING_PRESSURE")

// ============================================================================
// LoadBrode
// ============================================================================

bool LoadBrode::parse(const std::vector<std::string>& lines,
                       util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;

        data_.ssid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.xb = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.yb = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.zb = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.tb = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.p0 = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.e0 = parser.getDoubleAt(pos, realW).value_or(0.0);
        break;
    }

    return true;
}

std::vector<std::string> LoadBrode::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.ssid);
    writer.writeDouble(data_.xb);
    writer.writeDouble(data_.yb);
    writer.writeDouble(data_.zb);
    writer.writeDouble(data_.tb);
    writer.writeDouble(data_.p0);
    writer.writeDouble(data_.e0);

    result.push_back(writer.getLine());
    return result;
}

void LoadBrode::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(LoadBrode, "*LOAD_BRODE")

// ============================================================================
// LoadThermalBinout
// ============================================================================

bool LoadThermalBinout::parse(const std::vector<std::string>& lines,
                               util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    int lineIdx = 0;
    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);

        if (lineIdx == 0) {
            size_t pos = 0;
            data_.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
            pos += intW;
            data_.sf = parser.getDoubleAt(pos, realW).value_or(1.0);
            pos += realW;
            data_.psid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        } else if (lineIdx == 1) {
            data_.filename = util::StringUtils::trim(line);
            break;
        }
        lineIdx++;
    }

    return true;
}

std::vector<std::string> LoadThermalBinout::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.lcid);
    writer.writeDouble(data_.sf);
    writer.writeInt(data_.psid);
    result.push_back(writer.getLine());

    if (!data_.filename.empty()) {
        result.push_back(data_.filename);
    }

    return result;
}

void LoadThermalBinout::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(LoadThermalBinout, "*LOAD_THERMAL_BINOUT")

// ============================================================================
// LoadErodingPartSet
// ============================================================================

bool LoadErodingPartSet::parse(const std::vector<std::string>& lines,
                                util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;

        data_.psid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.sf = parser.getDoubleAt(pos, realW).value_or(1.0);
        pos += realW;
        data_.dof = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.cid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> LoadErodingPartSet::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.psid);
    writer.writeInt(data_.lcid);
    writer.writeDouble(data_.sf);
    writer.writeInt(data_.dof);
    writer.writeInt(data_.cid);

    result.push_back(writer.getLine());
    return result;
}

void LoadErodingPartSet::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(LoadErodingPartSet, "*LOAD_ERODING_PART_SET")

// ============================================================================
// LoadHeatGenerationSet
// ============================================================================

bool LoadHeatGenerationSet::parse(const std::vector<std::string>& lines,
                                   util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;

        data_.psid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.sf = parser.getDoubleAt(pos, realW).value_or(1.0);
        pos += realW;
        data_.birth = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.death = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> LoadHeatGenerationSet::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.psid);
    writer.writeInt(data_.lcid);
    writer.writeDouble(data_.sf);
    writer.writeInt(data_.birth);
    writer.writeInt(data_.death);

    result.push_back(writer.getLine());
    return result;
}

void LoadHeatGenerationSet::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(LoadHeatGenerationSet, "*LOAD_HEAT_GENERATION_SET")

// ============================================================================
// LoadMask
// ============================================================================

bool LoadMask::parse(const std::vector<std::string>& lines,
                      util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;

        data_.id = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.type = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.ssid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.box = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.value = parser.getDoubleAt(pos, realW).value_or(0.0);
        break;
    }

    return true;
}

std::vector<std::string> LoadMask::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.id);
    writer.writeInt(data_.type);
    writer.writeInt(data_.ssid);
    writer.writeInt(data_.box);
    writer.writeDouble(data_.value);

    result.push_back(writer.getLine());
    return result;
}

void LoadMask::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(LoadMask, "*LOAD_MASK")

// ============================================================================
// LoadRemovePart
// ============================================================================

bool LoadRemovePart::parse(const std::vector<std::string>& lines,
                            util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    data_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        Data entry;
        size_t pos = 0;

        entry.pid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.time = parser.getDoubleAt(pos, realW).value_or(0.0);

        if (entry.pid != 0) {
            data_.push_back(entry);
        }
    }

    return true;
}

std::vector<std::string> LoadRemovePart::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& entry : data_) {
        writer.clear();
        writer.writeInt(entry.pid);
        writer.writeDouble(entry.time);
        result.push_back(writer.getLine());
    }

    return result;
}

void LoadRemovePart::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(LoadRemovePart, "*LOAD_REMOVE_PART")

// ============================================================================
// LoadSuperplasticForming
// ============================================================================

bool LoadSuperplasticForming::parse(const std::vector<std::string>& lines,
                                     util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;

        data_.psid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.maxp = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.rate = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.dtefac = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> LoadSuperplasticForming::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.psid);
    writer.writeInt(data_.lcid);
    writer.writeDouble(data_.maxp);
    writer.writeDouble(data_.rate);
    writer.writeInt(data_.dtefac);

    result.push_back(writer.getLine());
    return result;
}

void LoadSuperplasticForming::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(LoadSuperplasticForming, "*LOAD_SUPERPLASTIC_FORMING")

// ============================================================================
// LoadSteadyStateRolling
// ============================================================================

bool LoadSteadyStateRolling::parse(const std::vector<std::string>& lines,
                                    util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;

        data_.psid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.radius = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.omega = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.vel = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.cid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> LoadSteadyStateRolling::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.psid);
    writer.writeDouble(data_.radius);
    writer.writeDouble(data_.omega);
    writer.writeDouble(data_.vel);
    writer.writeInt(data_.lcid);
    writer.writeInt(data_.cid);

    result.push_back(writer.getLine());
    return result;
}

void LoadSteadyStateRolling::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(LoadSteadyStateRolling, "*LOAD_STEADY_STATE_ROLLING")

// ============================================================================
// LoadSegmentNonuniform
// ============================================================================

bool LoadSegmentNonuniform::parse(const std::vector<std::string>& lines,
                                   util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    data_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        Data entry;
        size_t pos = 0;

        entry.ssid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.sf = parser.getDoubleAt(pos, realW).value_or(1.0);
        pos += realW;
        entry.at = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.lcidn1 = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.lcidn2 = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.lcidn3 = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.lcidn4 = static_cast<int>(parser.getInt64At(pos).value_or(0));

        if (entry.ssid != 0) {
            data_.push_back(entry);
        }
    }

    return true;
}

std::vector<std::string> LoadSegmentNonuniform::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& entry : data_) {
        writer.clear();
        writer.writeInt(entry.ssid);
        writer.writeInt(entry.lcid);
        writer.writeDouble(entry.sf);
        writer.writeInt(entry.at);
        writer.writeInt(entry.lcidn1);
        writer.writeInt(entry.lcidn2);
        writer.writeInt(entry.lcidn3);
        writer.writeInt(entry.lcidn4);
        result.push_back(writer.getLine());
    }

    return result;
}

void LoadSegmentNonuniform::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(LoadSegmentNonuniform, "*LOAD_SEGMENT_NONUNIFORM")

// ============================================================================
// LoadAleConvection
// ============================================================================

bool LoadAleConvection::parse(const std::vector<std::string>& lines,
                               util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;

        data_.ssid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.sf = parser.getDoubleAt(pos, realW).value_or(1.0);
        pos += realW;
        data_.tamb = parser.getDoubleAt(pos, realW).value_or(0.0);
        break;
    }

    return true;
}

std::vector<std::string> LoadAleConvection::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.ssid);
    writer.writeInt(data_.lcid);
    writer.writeDouble(data_.sf);
    writer.writeDouble(data_.tamb);

    result.push_back(writer.getLine());
    return result;
}

void LoadAleConvection::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(LoadAleConvection, "*LOAD_ALE_CONVECTION")

// ============================================================================
// LoadNegativeVolume
// ============================================================================

bool LoadNegativeVolume::parse(const std::vector<std::string>& lines,
                                util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;

        data_.psid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.option = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.scale = parser.getDoubleAt(pos, realW).value_or(1.0);
        break;
    }

    return true;
}

std::vector<std::string> LoadNegativeVolume::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.psid);
    writer.writeInt(data_.option);
    writer.writeDouble(data_.scale);

    result.push_back(writer.getLine());
    return result;
}

void LoadNegativeVolume::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(LoadNegativeVolume, "*LOAD_NEGATIVE_VOLUME")

// ============================================================================
// LoadAcousticSource
// ============================================================================

bool LoadAcousticSource::parse(const std::vector<std::string>& lines,
                                util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    data_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        Data entry;
        size_t pos = 0;

        entry.sid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.nid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        entry.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.sf = parser.getDoubleAt(pos, realW).value_or(1.0);
        pos += realW;
        entry.type = static_cast<int>(parser.getInt64At(pos).value_or(0));

        if (entry.sid != 0 || entry.nid != 0) {
            data_.push_back(entry);
        }
    }

    return true;
}

std::vector<std::string> LoadAcousticSource::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& entry : data_) {
        writer.clear();
        writer.writeInt(entry.sid);
        writer.writeInt(entry.nid);
        writer.writeInt(entry.lcid);
        writer.writeDouble(entry.sf);
        writer.writeInt(entry.type);
        result.push_back(writer.getLine());
    }

    return result;
}

void LoadAcousticSource::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(LoadAcousticSource, "*LOAD_ACOUSTIC_SOURCE")

// ============================================================================
// LoadBeam
// ============================================================================

bool LoadBeam::parse(const std::vector<std::string>& lines,
                      util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    loads_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        Data load;
        size_t pos = 0;

        load.eid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        load.dir = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        load.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        load.sf = parser.getDoubleAt(pos, realW).value_or(1.0);
        pos += realW;
        load.cid = static_cast<int>(parser.getInt64At(pos).value_or(0));

        if (load.eid != 0) {
            loads_.push_back(load);
        }
    }

    return true;
}

std::vector<std::string> LoadBeam::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& load : loads_) {
        writer.clear();
        writer.writeInt(load.eid);
        writer.writeInt(load.dir);
        writer.writeInt(load.lcid);
        writer.writeDouble(load.sf);
        writer.writeInt(load.cid);
        result.push_back(writer.getLine());
    }

    return result;
}

void LoadBeam::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(LoadBeam, "*LOAD_BEAM")

// ============================================================================
// LoadBlast
// ============================================================================

bool LoadBlast::parse(const std::vector<std::string>& lines,
                       util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;

        data_.bid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.m = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.xbo = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.ybo = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.zbo = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.tbo = parser.getDoubleAt(pos, realW).value_or(0.0);
        break;
    }

    return true;
}

std::vector<std::string> LoadBlast::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.bid);
    writer.writeDouble(data_.m);
    writer.writeDouble(data_.xbo);
    writer.writeDouble(data_.ybo);
    writer.writeDouble(data_.zbo);
    writer.writeDouble(data_.tbo);

    result.push_back(writer.getLine());
    return result;
}

void LoadBlast::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(LoadBlast, "*LOAD_BLAST")

// ============================================================================
// LoadBlastSegmentSet
// ============================================================================

bool LoadBlastSegmentSet::parse(const std::vector<std::string>& lines,
                                 util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    data_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        Data entry;
        size_t pos = 0;

        entry.bid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.ssid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.sf = parser.getDoubleAt(pos, realW).value_or(1.0);

        if (entry.bid != 0 || entry.ssid != 0) {
            data_.push_back(entry);
        }
    }

    return true;
}

std::vector<std::string> LoadBlastSegmentSet::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& entry : data_) {
        writer.clear();
        writer.writeInt(entry.bid);
        writer.writeInt(entry.ssid);
        writer.writeDouble(entry.sf);
        result.push_back(writer.getLine());
    }

    return result;
}

void LoadBlastSegmentSet::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(LoadBlastSegmentSet, "*LOAD_BLAST_SEGMENT_SET")

// ============================================================================
// LoadBodyGeneralized
// ============================================================================

bool LoadBodyGeneralized::parse(const std::vector<std::string>& lines,
                                 util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;

        data_.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.sf = parser.getDoubleAt(pos, realW).value_or(1.0);
        pos += realW;
        data_.cid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.ax = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.ay = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.az = parser.getDoubleAt(pos, realW).value_or(0.0);
        break;
    }

    return true;
}

std::vector<std::string> LoadBodyGeneralized::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.lcid);
    writer.writeDouble(data_.sf);
    writer.writeInt(data_.cid);
    writer.writeDouble(data_.ax);
    writer.writeDouble(data_.ay);
    writer.writeDouble(data_.az);

    result.push_back(writer.getLine());
    return result;
}

void LoadBodyGeneralized::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(LoadBodyGeneralized, "*LOAD_BODY_GENERALIZED")

// ============================================================================
// LoadBodyPorous
// ============================================================================

bool LoadBodyPorous::parse(const std::vector<std::string>& lines,
                            util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;

        data_.psid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.lcidx = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.lcidy = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.lcidz = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.sfx = parser.getDoubleAt(pos, realW).value_or(1.0);
        pos += realW;
        data_.sfy = parser.getDoubleAt(pos, realW).value_or(1.0);
        pos += realW;
        data_.sfz = parser.getDoubleAt(pos, realW).value_or(1.0);
        break;
    }

    return true;
}

std::vector<std::string> LoadBodyPorous::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.psid);
    writer.writeInt(data_.lcidx);
    writer.writeInt(data_.lcidy);
    writer.writeInt(data_.lcidz);
    writer.writeDouble(data_.sfx);
    writer.writeDouble(data_.sfy);
    writer.writeDouble(data_.sfz);

    result.push_back(writer.getLine());
    return result;
}

void LoadBodyPorous::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(LoadBodyPorous, "*LOAD_BODY_POROUS")

// ============================================================================
// LoadExpansionPressure
// ============================================================================

bool LoadExpansionPressure::parse(const std::vector<std::string>& lines,
                                   util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;

        data_.psid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.sf = parser.getDoubleAt(pos, realW).value_or(1.0);
        pos += realW;
        data_.opt = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> LoadExpansionPressure::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.psid);
    writer.writeInt(data_.lcid);
    writer.writeDouble(data_.sf);
    writer.writeInt(data_.opt);

    result.push_back(writer.getLine());
    return result;
}

void LoadExpansionPressure::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(LoadExpansionPressure, "*LOAD_EXPANSION_PRESSURE")

// ============================================================================
// LoadHeatController
// ============================================================================

bool LoadHeatController::parse(const std::vector<std::string>& lines,
                                util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    data_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        Data entry;
        size_t pos = 0;

        entry.hcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.nsid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.sf = parser.getDoubleAt(pos, realW).value_or(1.0);
        pos += realW;
        entry.target = parser.getDoubleAt(pos, realW).value_or(0.0);

        if (entry.hcid != 0 || entry.nsid != 0) {
            data_.push_back(entry);
        }
    }

    return true;
}

std::vector<std::string> LoadHeatController::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& entry : data_) {
        writer.clear();
        writer.writeInt(entry.hcid);
        writer.writeInt(entry.nsid);
        writer.writeInt(entry.lcid);
        writer.writeDouble(entry.sf);
        writer.writeDouble(entry.target);
        result.push_back(writer.getLine());
    }

    return result;
}

void LoadHeatController::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(LoadHeatController, "*LOAD_HEAT_CONTROLLER")

// ============================================================================
// LoadHeatExothermicReaction
// ============================================================================

bool LoadHeatExothermicReaction::parse(const std::vector<std::string>& lines,
                                        util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;

        data_.psid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.sf = parser.getDoubleAt(pos, realW).value_or(1.0);
        pos += realW;
        data_.q0 = parser.getDoubleAt(pos, realW).value_or(0.0);
        break;
    }

    return true;
}

std::vector<std::string> LoadHeatExothermicReaction::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.psid);
    writer.writeInt(data_.lcid);
    writer.writeDouble(data_.sf);
    writer.writeDouble(data_.q0);

    result.push_back(writer.getLine());
    return result;
}

void LoadHeatExothermicReaction::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(LoadHeatExothermicReaction, "*LOAD_HEAT_EXOTHERMIC_REACTION")

// ============================================================================
// LoadHeatGeneration
// ============================================================================

bool LoadHeatGeneration::parse(const std::vector<std::string>& lines,
                                util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    loads_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        Data load;
        size_t pos = 0;

        load.eid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        load.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        load.sf = parser.getDoubleAt(pos, realW).value_or(1.0);

        if (load.eid != 0) {
            loads_.push_back(load);
        }
    }

    return true;
}

std::vector<std::string> LoadHeatGeneration::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& load : loads_) {
        writer.clear();
        writer.writeInt(load.eid);
        writer.writeInt(load.lcid);
        writer.writeDouble(load.sf);
        result.push_back(writer.getLine());
    }

    return result;
}

void LoadHeatGeneration::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(LoadHeatGeneration, "*LOAD_HEAT_GENERATION")

// ============================================================================
// LoadNode
// ============================================================================

bool LoadNode::parse(const std::vector<std::string>& lines,
                      util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    loads_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        Data load;
        size_t pos = 0;

        load.nid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        load.dof = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        load.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        load.sf = parser.getDoubleAt(pos, realW).value_or(1.0);
        pos += realW;
        load.cid = static_cast<int>(parser.getInt64At(pos).value_or(0));

        if (load.nid != 0) {
            loads_.push_back(load);
        }
    }

    return true;
}

std::vector<std::string> LoadNode::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& load : loads_) {
        writer.clear();
        writer.writeInt(load.nid);
        writer.writeInt(load.dof);
        writer.writeInt(load.lcid);
        writer.writeDouble(load.sf);
        writer.writeInt(load.cid);
        result.push_back(writer.getLine());
    }

    return result;
}

void LoadNode::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(LoadNode, "*LOAD_NODE")

// ============================================================================
// LoadPze
// ============================================================================

bool LoadPze::parse(const std::vector<std::string>& lines,
                     util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    data_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        Data entry;
        size_t pos = 0;

        entry.nsid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.sf = parser.getDoubleAt(pos, realW).value_or(1.0);
        pos += realW;
        entry.opt = static_cast<int>(parser.getInt64At(pos).value_or(0));

        if (entry.nsid != 0) {
            data_.push_back(entry);
        }
    }

    return true;
}

std::vector<std::string> LoadPze::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& entry : data_) {
        writer.clear();
        writer.writeInt(entry.nsid);
        writer.writeInt(entry.lcid);
        writer.writeDouble(entry.sf);
        writer.writeInt(entry.opt);
        result.push_back(writer.getLine());
    }

    return result;
}

void LoadPze::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(LoadPze, "*LOAD_PZE")

// ============================================================================
// LoadSegmentFile
// ============================================================================

bool LoadSegmentFile::parse(const std::vector<std::string>& lines,
                             util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    int lineIdx = 0;
    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);

        if (lineIdx == 0) {
            size_t pos = 0;
            data_.ssid = static_cast<int>(parser.getInt64At(pos).value_or(0));
            pos += intW;
            data_.sf = parser.getDoubleAt(pos, realW).value_or(1.0);
        } else if (lineIdx == 1) {
            data_.filename = util::StringUtils::trim(line);
            break;
        }
        lineIdx++;
    }

    return true;
}

std::vector<std::string> LoadSegmentFile::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.ssid);
    writer.writeDouble(data_.sf);
    result.push_back(writer.getLine());

    if (!data_.filename.empty()) {
        result.push_back(data_.filename);
    }

    return result;
}

void LoadSegmentFile::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(LoadSegmentFile, "*LOAD_SEGMENT_FILE")

// ============================================================================
// LoadSegmentSetAngle
// ============================================================================

bool LoadSegmentSetAngle::parse(const std::vector<std::string>& lines,
                                 util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    loads_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        Data load;
        size_t pos = 0;

        load.ssid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        load.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        load.sf = parser.getDoubleAt(pos, realW).value_or(1.0);
        pos += realW;
        load.angle = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        load.lcang = static_cast<int>(parser.getInt64At(pos).value_or(0));

        if (load.ssid != 0) {
            loads_.push_back(load);
        }
    }

    return true;
}

std::vector<std::string> LoadSegmentSetAngle::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& load : loads_) {
        writer.clear();
        writer.writeInt(load.ssid);
        writer.writeInt(load.lcid);
        writer.writeDouble(load.sf);
        writer.writeInt(load.angle);
        writer.writeInt(load.lcang);
        result.push_back(writer.getLine());
    }

    return result;
}

void LoadSegmentSetAngle::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(LoadSegmentSetAngle, "*LOAD_SEGMENT_SET_ANGLE")

// ============================================================================
// LoadSeismicSsiAux
// ============================================================================

bool LoadSeismicSsiAux::parse(const std::vector<std::string>& lines,
                               util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;

        data_.ssid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.gmset = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.type = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> LoadSeismicSsiAux::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.ssid);
    writer.writeInt(data_.gmset);
    writer.writeInt(data_.type);

    result.push_back(writer.getLine());
    return result;
}

void LoadSeismicSsiAux::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(LoadSeismicSsiAux, "*LOAD_SEISMIC_SSI_AUX")

// ============================================================================
// LoadShellElement
// ============================================================================

bool LoadShellElement::parse(const std::vector<std::string>& lines,
                              util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    loads_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        Data load;
        size_t pos = 0;

        load.eid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        load.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        load.sf = parser.getDoubleAt(pos, realW).value_or(1.0);
        pos += realW;
        load.at = static_cast<int>(parser.getInt64At(pos).value_or(0));

        if (load.eid != 0) {
            loads_.push_back(load);
        }
    }

    return true;
}

std::vector<std::string> LoadShellElement::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& load : loads_) {
        writer.clear();
        writer.writeInt(load.eid);
        writer.writeInt(load.lcid);
        writer.writeDouble(load.sf);
        writer.writeInt(load.at);
        result.push_back(writer.getLine());
    }

    return result;
}

void LoadShellElement::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(LoadShellElement, "*LOAD_SHELL_ELEMENT")

// ============================================================================
// LoadStiffenPart
// ============================================================================

bool LoadStiffenPart::parse(const std::vector<std::string>& lines,
                             util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    data_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        Data entry;
        size_t pos = 0;

        entry.psid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.sf = parser.getDoubleAt(pos, realW).value_or(1.0);

        if (entry.psid != 0) {
            data_.push_back(entry);
        }
    }

    return true;
}

std::vector<std::string> LoadStiffenPart::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& entry : data_) {
        writer.clear();
        writer.writeInt(entry.psid);
        writer.writeInt(entry.lcid);
        writer.writeDouble(entry.sf);
        result.push_back(writer.getLine());
    }

    return result;
}

void LoadStiffenPart::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(LoadStiffenPart, "*LOAD_STIFFEN_PART")

// ============================================================================
// LoadThermalD3plot
// ============================================================================

bool LoadThermalD3plot::parse(const std::vector<std::string>& lines,
                               util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    int lineIdx = 0;
    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);

        if (lineIdx == 0) {
            size_t pos = 0;
            data_.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
            pos += intW;
            data_.sf = parser.getDoubleAt(pos, realW).value_or(1.0);
            pos += realW;
            data_.psid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        } else if (lineIdx == 1) {
            data_.filename = util::StringUtils::trim(line);
            break;
        }
        lineIdx++;
    }

    return true;
}

std::vector<std::string> LoadThermalD3plot::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.lcid);
    writer.writeDouble(data_.sf);
    writer.writeInt(data_.psid);
    result.push_back(writer.getLine());

    if (!data_.filename.empty()) {
        result.push_back(data_.filename);
    }

    return result;
}

void LoadThermalD3plot::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(LoadThermalD3plot, "*LOAD_THERMAL_D3PLOT")

// ============================================================================
// LoadThermalLoadCurve
// ============================================================================

bool LoadThermalLoadCurve::parse(const std::vector<std::string>& lines,
                                  util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    data_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        Data entry;
        size_t pos = 0;

        entry.nsid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.sf = parser.getDoubleAt(pos, realW).value_or(1.0);

        if (entry.nsid != 0) {
            data_.push_back(entry);
        }
    }

    return true;
}

std::vector<std::string> LoadThermalLoadCurve::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& entry : data_) {
        writer.clear();
        writer.writeInt(entry.nsid);
        writer.writeInt(entry.lcid);
        writer.writeDouble(entry.sf);
        result.push_back(writer.getLine());
    }

    return result;
}

void LoadThermalLoadCurve::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(LoadThermalLoadCurve, "*LOAD_THERMAL_LOAD_CURVE")

// ============================================================================
// LoadThermalRsw
// ============================================================================

bool LoadThermalRsw::parse(const std::vector<std::string>& lines,
                            util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    data_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        Data entry;
        size_t pos = 0;

        entry.nsid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.sf = parser.getDoubleAt(pos, realW).value_or(1.0);
        pos += realW;
        entry.opt = static_cast<int>(parser.getInt64At(pos).value_or(0));

        if (entry.nsid != 0) {
            data_.push_back(entry);
        }
    }

    return true;
}

std::vector<std::string> LoadThermalRsw::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& entry : data_) {
        writer.clear();
        writer.writeInt(entry.nsid);
        writer.writeInt(entry.lcid);
        writer.writeDouble(entry.sf);
        writer.writeInt(entry.opt);
        result.push_back(writer.getLine());
    }

    return result;
}

void LoadThermalRsw::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(LoadThermalRsw, "*LOAD_THERMAL_RSW")

// ============================================================================
// LoadThermalTopaz
// ============================================================================

bool LoadThermalTopaz::parse(const std::vector<std::string>& lines,
                              util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    int lineIdx = 0;
    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);

        if (lineIdx == 0) {
            size_t pos = 0;
            data_.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
            pos += intW;
            data_.sf = parser.getDoubleAt(pos, realW).value_or(1.0);
        } else if (lineIdx == 1) {
            data_.filename = util::StringUtils::trim(line);
            break;
        }
        lineIdx++;
    }

    return true;
}

std::vector<std::string> LoadThermalTopaz::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.lcid);
    writer.writeDouble(data_.sf);
    result.push_back(writer.getLine());

    if (!data_.filename.empty()) {
        result.push_back(data_.filename);
    }

    return result;
}

void LoadThermalTopaz::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(LoadThermalTopaz, "*LOAD_THERMAL_TOPAZ")

// ============================================================================
// LoadThermalVariableBeam
// ============================================================================

bool LoadThermalVariableBeam::parse(const std::vector<std::string>& lines,
                                     util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    data_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        Data entry;
        size_t pos = 0;

        entry.bsid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.sf = parser.getDoubleAt(pos, realW).value_or(1.0);

        if (entry.bsid != 0) {
            data_.push_back(entry);
        }
    }

    return true;
}

std::vector<std::string> LoadThermalVariableBeam::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& entry : data_) {
        writer.clear();
        writer.writeInt(entry.bsid);
        writer.writeInt(entry.lcid);
        writer.writeDouble(entry.sf);
        result.push_back(writer.getLine());
    }

    return result;
}

void LoadThermalVariableBeam::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(LoadThermalVariableBeam, "*LOAD_THERMAL_VARIABLE_BEAM")

// ============================================================================
// LoadThermalVariableNode
// ============================================================================

bool LoadThermalVariableNode::parse(const std::vector<std::string>& lines,
                                     util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    data_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        Data entry;
        size_t pos = 0;

        entry.nid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        entry.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.sf = parser.getDoubleAt(pos, realW).value_or(1.0);

        if (entry.nid != 0) {
            data_.push_back(entry);
        }
    }

    return true;
}

std::vector<std::string> LoadThermalVariableNode::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& entry : data_) {
        writer.clear();
        writer.writeInt(entry.nid);
        writer.writeInt(entry.lcid);
        writer.writeDouble(entry.sf);
        result.push_back(writer.getLine());
    }

    return result;
}

void LoadThermalVariableNode::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(LoadThermalVariableNode, "*LOAD_THERMAL_VARIABLE_NODE")

// ============================================================================
// LoadThermalVariableShell
// ============================================================================

bool LoadThermalVariableShell::parse(const std::vector<std::string>& lines,
                                      util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    data_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        Data entry;
        size_t pos = 0;

        entry.esid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.sf = parser.getDoubleAt(pos, realW).value_or(1.0);

        if (entry.esid != 0) {
            data_.push_back(entry);
        }
    }

    return true;
}

std::vector<std::string> LoadThermalVariableShell::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& entry : data_) {
        writer.clear();
        writer.writeInt(entry.esid);
        writer.writeInt(entry.lcid);
        writer.writeDouble(entry.sf);
        result.push_back(writer.getLine());
    }

    return result;
}

void LoadThermalVariableShell::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(LoadThermalVariableShell, "*LOAD_THERMAL_VARIABLE_SHELL")

// ============================================================================
// LoadVolumeLoss
// ============================================================================

bool LoadVolumeLoss::parse(const std::vector<std::string>& lines,
                            util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;

        data_.psid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.sf = parser.getDoubleAt(pos, realW).value_or(1.0);
        pos += realW;
        data_.opt = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> LoadVolumeLoss::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.psid);
    writer.writeInt(data_.lcid);
    writer.writeDouble(data_.sf);
    writer.writeInt(data_.opt);

    result.push_back(writer.getLine());
    return result;
}

void LoadVolumeLoss::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(LoadVolumeLoss, "*LOAD_VOLUME_LOSS")

// ============================================================================
// LoadBodyVector
// ============================================================================

bool LoadBodyVector::parse(const std::vector<std::string>& lines,
                            util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;

        data_.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.sf = parser.getDoubleAt(pos, realW).value_or(1.0);
        pos += realW;
        data_.vid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.vx = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.vy = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.vz = parser.getDoubleAt(pos, realW).value_or(0.0);
        break;
    }

    return true;
}

std::vector<std::string> LoadBodyVector::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.lcid);
    writer.writeDouble(data_.sf);
    writer.writeInt(data_.vid);
    writer.writeDouble(data_.vx);
    writer.writeDouble(data_.vy);
    writer.writeDouble(data_.vz);

    result.push_back(writer.getLine());
    return result;
}

void LoadBodyVector::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(LoadBodyVector, "*LOAD_BODY_VECTOR")

// ============================================================================
// LoadSegmentId
// ============================================================================

bool LoadSegmentId::parse(const std::vector<std::string>& lines,
                           util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    loads_.clear();

    int lineIdx = 0;
    Data currentLoad;
    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);

        if (lineIdx == 0) {
            // ID and heading card
            size_t pos = 0;
            currentLoad.id = static_cast<int>(parser.getInt64At(pos).value_or(0));
            pos += intW;
            currentLoad.heading = util::StringUtils::trim(line.substr(std::min(pos, line.size())));
            lineIdx = 1;
        } else {
            // Data card
            size_t pos = 0;
            currentLoad.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
            pos += intW;
            currentLoad.sf = parser.getDoubleAt(pos, realW).value_or(1.0);
            pos += realW;
            currentLoad.at = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            currentLoad.n1 = parser.getInt64At(pos).value_or(0);
            pos += intW;
            currentLoad.n2 = parser.getInt64At(pos).value_or(0);
            pos += intW;
            currentLoad.n3 = parser.getInt64At(pos).value_or(0);
            pos += intW;
            currentLoad.n4 = parser.getInt64At(pos).value_or(0);

            if (currentLoad.id != 0) {
                loads_.push_back(currentLoad);
            }
            currentLoad = Data();
            lineIdx = 0;
        }
    }

    return true;
}

std::vector<std::string> LoadSegmentId::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& load : loads_) {
        writer.clear();
        writer.writeInt(load.id);
        if (!load.heading.empty()) {
            result.push_back(writer.getLine() + load.heading);
        } else {
            result.push_back(writer.getLine());
        }

        writer.clear();
        writer.writeInt(load.lcid);
        writer.writeDouble(load.sf);
        writer.writeDouble(load.at);
        writer.writeInt(load.n1);
        writer.writeInt(load.n2);
        writer.writeInt(load.n3);
        writer.writeInt(load.n4);
        result.push_back(writer.getLine());
    }

    return result;
}

void LoadSegmentId::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(LoadSegmentId, "*LOAD_SEGMENT_ID")

// ============================================================================
// LoadThermalConstantNode
// ============================================================================

bool LoadThermalConstantNode::parse(const std::vector<std::string>& lines,
                                     util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    data_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        Data entry;
        size_t pos = 0;

        entry.nid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        entry.temp = parser.getDoubleAt(pos, realW).value_or(0.0);

        if (entry.nid != 0) {
            data_.push_back(entry);
        }
    }

    return true;
}

std::vector<std::string> LoadThermalConstantNode::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& entry : data_) {
        writer.clear();
        writer.writeInt(entry.nid);
        writer.writeDouble(entry.temp);
        result.push_back(writer.getLine());
    }

    return result;
}

void LoadThermalConstantNode::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(LoadThermalConstantNode, "*LOAD_THERMAL_CONSTANT_NODE")

// ============================================================================
// LoadThermalElement
// ============================================================================

bool LoadThermalElement::parse(const std::vector<std::string>& lines,
                                util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    data_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        Data entry;
        size_t pos = 0;

        entry.eid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        entry.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.sf = parser.getDoubleAt(pos, realW).value_or(1.0);

        if (entry.eid != 0) {
            data_.push_back(entry);
        }
    }

    return true;
}

std::vector<std::string> LoadThermalElement::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& entry : data_) {
        writer.clear();
        writer.writeInt(entry.eid);
        writer.writeInt(entry.lcid);
        writer.writeDouble(entry.sf);
        result.push_back(writer.getLine());
    }

    return result;
}

void LoadThermalElement::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(LoadThermalElement, "*LOAD_THERMAL_ELEMENT")

// ============================================================================
// LoadThermalVariableElement
// ============================================================================

bool LoadThermalVariableElement::parse(const std::vector<std::string>& lines,
                                        util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    data_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        Data entry;
        size_t pos = 0;

        entry.eid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        entry.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.sf = parser.getDoubleAt(pos, realW).value_or(1.0);

        if (entry.eid != 0) {
            data_.push_back(entry);
        }
    }

    return true;
}

std::vector<std::string> LoadThermalVariableElement::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& entry : data_) {
        writer.clear();
        writer.writeInt(entry.eid);
        writer.writeInt(entry.lcid);
        writer.writeDouble(entry.sf);
        result.push_back(writer.getLine());
    }

    return result;
}

void LoadThermalVariableElement::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(LoadThermalVariableElement, "*LOAD_THERMAL_VARIABLE_ELEMENT")


// ============================================================================
// LoadDensity
// ============================================================================

bool LoadDensity::parse(const std::vector<std::string>& lines, util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;
        parser.setLine(line);
        size_t pos = 0;

        data_.psid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.sf = parser.getDoubleAt(pos, realW).value_or(1.0);
        break;
    }

    return true;
}

std::vector<std::string> LoadDensity::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.psid);
    writer.writeInt(data_.lcid);
    writer.writeDouble(data_.sf);
    result.push_back(writer.getLine());

    return result;
}

void LoadDensity::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(LoadDensity, "*LOAD_DENSITY")

// ============================================================================
// LoadGravity
// ============================================================================

bool LoadGravity::parse(const std::vector<std::string>& lines, util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;
        parser.setLine(line);
        size_t pos = 0;

        data_.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.sf = parser.getDoubleAt(pos, realW).value_or(1.0);
        pos += realW;
        data_.gx = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.gy = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.gz = parser.getDoubleAt(pos, realW).value_or(0.0);
        break;
    }

    return true;
}

std::vector<std::string> LoadGravity::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.lcid);
    writer.writeDouble(data_.sf);
    writer.writeDouble(data_.gx);
    writer.writeDouble(data_.gy);
    writer.writeDouble(data_.gz);
    result.push_back(writer.getLine());

    return result;
}

void LoadGravity::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(LoadGravity, "*LOAD_GRAVITY")

// ============================================================================
// LoadSsa
// ============================================================================

bool LoadSsa::parse(const std::vector<std::string>& lines, util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;
        parser.setLine(line);
        size_t pos = 0;

        data_.psid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.lcidx = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.lcidy = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.lcidz = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.sfx = parser.getDoubleAt(pos, realW).value_or(1.0);
        pos += realW;
        data_.sfy = parser.getDoubleAt(pos, realW).value_or(1.0);
        pos += realW;
        data_.sfz = parser.getDoubleAt(pos, realW).value_or(1.0);
        break;
    }

    return true;
}

std::vector<std::string> LoadSsa::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.psid);
    writer.writeInt(data_.lcidx);
    writer.writeInt(data_.lcidy);
    writer.writeInt(data_.lcidz);
    writer.writeDouble(data_.sfx);
    writer.writeDouble(data_.sfy);
    writer.writeDouble(data_.sfz);
    result.push_back(writer.getLine());

    return result;
}

void LoadSsa::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(LoadSsa, "*LOAD_SSA")

// ============================================================================
// LoadSsaGravity
// ============================================================================

bool LoadSsaGravity::parse(const std::vector<std::string>& lines, util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;
        parser.setLine(line);
        size_t pos = 0;

        data_.psid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.sf = parser.getDoubleAt(pos, realW).value_or(1.0);
        pos += realW;
        data_.gx = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.gy = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.gz = parser.getDoubleAt(pos, realW).value_or(0.0);
        break;
    }

    return true;
}

std::vector<std::string> LoadSsaGravity::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.psid);
    writer.writeInt(data_.lcid);
    writer.writeDouble(data_.sf);
    writer.writeDouble(data_.gx);
    writer.writeDouble(data_.gy);
    writer.writeDouble(data_.gz);
    result.push_back(writer.getLine());

    return result;
}

void LoadSsaGravity::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(LoadSsaGravity, "*LOAD_SSA_GRAVITY")

// ============================================================================
// LoadSeismic
// ============================================================================

bool LoadSeismic::parse(const std::vector<std::string>& lines, util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;
        parser.setLine(line);
        size_t pos = 0;

        data_.nsid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.gmset = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.dof = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.sf = static_cast<int>(parser.getInt64At(pos).value_or(1));
        break;
    }

    return true;
}

std::vector<std::string> LoadSeismic::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.nsid);
    writer.writeInt(data_.gmset);
    writer.writeInt(data_.dof);
    writer.writeInt(data_.sf);
    result.push_back(writer.getLine());

    return result;
}

void LoadSeismic::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(LoadSeismic, "*LOAD_SEISMIC")

// ============================================================================
// LoadBodyRx
// ============================================================================

bool LoadBodyRx::parse(const std::vector<std::string>& lines, util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;
        parser.setLine(line);
        size_t pos = 0;

        data_.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.sf = parser.getDoubleAt(pos, realW).value_or(1.0);
        pos += realW;
        data_.xc = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.yc = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.zc = parser.getDoubleAt(pos, realW).value_or(0.0);
        break;
    }

    return true;
}

std::vector<std::string> LoadBodyRx::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.lcid);
    writer.writeDouble(data_.sf);
    writer.writeDouble(data_.xc);
    writer.writeDouble(data_.yc);
    writer.writeDouble(data_.zc);
    result.push_back(writer.getLine());

    return result;
}

void LoadBodyRx::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(LoadBodyRx, "*LOAD_BODY_RX")

// ============================================================================
// LoadBodyRy
// ============================================================================

bool LoadBodyRy::parse(const std::vector<std::string>& lines, util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;
        parser.setLine(line);
        size_t pos = 0;

        data_.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.sf = parser.getDoubleAt(pos, realW).value_or(1.0);
        pos += realW;
        data_.xc = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.yc = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.zc = parser.getDoubleAt(pos, realW).value_or(0.0);
        break;
    }

    return true;
}

std::vector<std::string> LoadBodyRy::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.lcid);
    writer.writeDouble(data_.sf);
    writer.writeDouble(data_.xc);
    writer.writeDouble(data_.yc);
    writer.writeDouble(data_.zc);
    result.push_back(writer.getLine());

    return result;
}

void LoadBodyRy::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(LoadBodyRy, "*LOAD_BODY_RY")

// ============================================================================
// LoadBodyRz
// ============================================================================

bool LoadBodyRz::parse(const std::vector<std::string>& lines, util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;
        parser.setLine(line);
        size_t pos = 0;

        data_.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.sf = parser.getDoubleAt(pos, realW).value_or(1.0);
        pos += realW;
        data_.xc = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.yc = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.zc = parser.getDoubleAt(pos, realW).value_or(0.0);
        break;
    }

    return true;
}

std::vector<std::string> LoadBodyRz::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.lcid);
    writer.writeDouble(data_.sf);
    writer.writeDouble(data_.xc);
    writer.writeDouble(data_.yc);
    writer.writeDouble(data_.zc);
    result.push_back(writer.getLine());

    return result;
}

void LoadBodyRz::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(LoadBodyRz, "*LOAD_BODY_RZ")

// ============================================================================
// LoadThermalVariableSolid
// ============================================================================

bool LoadThermalVariableSolid::parse(const std::vector<std::string>& lines, util::CardParser::Format format) {
    data_.clear();

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;
        parser.setLine(line);
        Data entry;
        size_t pos = 0;

        entry.esid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.sf = parser.getDoubleAt(pos, realW).value_or(1.0);

        if (entry.esid != 0) {
            data_.push_back(entry);
        }
    }

    return true;
}

std::vector<std::string> LoadThermalVariableSolid::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& entry : data_) {
        writer.clear();
        writer.writeInt(entry.esid);
        writer.writeInt(entry.lcid);
        writer.writeDouble(entry.sf);
        result.push_back(writer.getLine());
    }

    return result;
}

void LoadThermalVariableSolid::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(LoadThermalVariableSolid, "*LOAD_THERMAL_VARIABLE_SOLID")

// ============================================================================
// LoadThermalVariableTshell
// ============================================================================

bool LoadThermalVariableTshell::parse(const std::vector<std::string>& lines, util::CardParser::Format format) {
    data_.clear();

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;
        parser.setLine(line);
        Data entry;
        size_t pos = 0;

        entry.esid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.sf = parser.getDoubleAt(pos, realW).value_or(1.0);

        if (entry.esid != 0) {
            data_.push_back(entry);
        }
    }

    return true;
}

std::vector<std::string> LoadThermalVariableTshell::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& entry : data_) {
        writer.clear();
        writer.writeInt(entry.esid);
        writer.writeInt(entry.lcid);
        writer.writeDouble(entry.sf);
        result.push_back(writer.getLine());
    }

    return result;
}

void LoadThermalVariableTshell::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(LoadThermalVariableTshell, "*LOAD_THERMAL_VARIABLE_TSHELL")

// ============================================================================
// LoadRigidBodyInertia
// ============================================================================

bool LoadRigidBodyInertia::parse(const std::vector<std::string>& lines, util::CardParser::Format format) {
    data_.clear();

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;
        parser.setLine(line);
        Data entry;
        size_t pos = 0;

        entry.pid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        entry.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.sf = parser.getDoubleAt(pos, realW).value_or(1.0);
        pos += realW;
        entry.ax = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        entry.ay = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        entry.az = parser.getDoubleAt(pos, realW).value_or(0.0);

        if (entry.pid != 0) {
            data_.push_back(entry);
        }
    }

    return true;
}

std::vector<std::string> LoadRigidBodyInertia::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& entry : data_) {
        writer.clear();
        writer.writeInt(entry.pid);
        writer.writeInt(entry.lcid);
        writer.writeDouble(entry.sf);
        writer.writeDouble(entry.ax);
        writer.writeDouble(entry.ay);
        writer.writeDouble(entry.az);
        result.push_back(writer.getLine());
    }

    return result;
}

void LoadRigidBodyInertia::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(LoadRigidBodyInertia, "*LOAD_RIGID_BODY_INERTIA")

// ============================================================================
// LoadSegmentData
// ============================================================================

bool LoadSegmentData::parse(const std::vector<std::string>& lines, util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    bool firstLine = true;
    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;
        if (firstLine) {
            parser.setLine(line);
            size_t pos = 0;
            data_.ssid = static_cast<int>(parser.getInt64At(pos).value_or(0));
            pos += intW;
            data_.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
            pos += intW;
            data_.sf = parser.getDoubleAt(pos, realW).value_or(1.0);
            firstLine = false;
        } else {
            data_.filename = line;
            while (!data_.filename.empty() && (data_.filename.back() == ' ' || data_.filename.back() == '\r')) {
                data_.filename.pop_back();
            }
            break;
        }
    }

    return true;
}

std::vector<std::string> LoadSegmentData::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.ssid);
    writer.writeInt(data_.lcid);
    writer.writeDouble(data_.sf);
    result.push_back(writer.getLine());

    if (!data_.filename.empty()) {
        result.push_back(data_.filename);
    }

    return result;
}

void LoadSegmentData::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(LoadSegmentData, "*LOAD_SEGMENT_DATA")

// ============================================================================
// LoadBodyPart
// ============================================================================

bool LoadBodyPart::parse(const std::vector<std::string>& lines, util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;
        parser.setLine(line);
        size_t pos = 0;

        data_.pid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data_.lcidx = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.lcidy = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.lcidz = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.sfx = parser.getDoubleAt(pos, realW).value_or(1.0);
        pos += realW;
        data_.sfy = parser.getDoubleAt(pos, realW).value_or(1.0);
        pos += realW;
        data_.sfz = parser.getDoubleAt(pos, realW).value_or(1.0);
        break;
    }

    return true;
}

std::vector<std::string> LoadBodyPart::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.pid);
    writer.writeInt(data_.lcidx);
    writer.writeInt(data_.lcidy);
    writer.writeInt(data_.lcidz);
    writer.writeDouble(data_.sfx);
    writer.writeDouble(data_.sfy);
    writer.writeDouble(data_.sfz);
    result.push_back(writer.getLine());

    return result;
}

void LoadBodyPart::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(LoadBodyPart, "*LOAD_BODY_PART")

// ============================================================================
// LoadBodyPartSet
// ============================================================================

bool LoadBodyPartSet::parse(const std::vector<std::string>& lines, util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;
        parser.setLine(line);
        size_t pos = 0;

        data_.psid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.lcidx = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.lcidy = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.lcidz = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.sfx = parser.getDoubleAt(pos, realW).value_or(1.0);
        pos += realW;
        data_.sfy = parser.getDoubleAt(pos, realW).value_or(1.0);
        pos += realW;
        data_.sfz = parser.getDoubleAt(pos, realW).value_or(1.0);
        break;
    }

    return true;
}

std::vector<std::string> LoadBodyPartSet::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.psid);
    writer.writeInt(data_.lcidx);
    writer.writeInt(data_.lcidy);
    writer.writeInt(data_.lcidz);
    writer.writeDouble(data_.sfx);
    writer.writeDouble(data_.sfy);
    writer.writeDouble(data_.sfz);
    result.push_back(writer.getLine());

    return result;
}

void LoadBodyPartSet::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(LoadBodyPartSet, "*LOAD_BODY_PART_SET")

// ============================================================================
// LoadFluidPressure
// ============================================================================

bool LoadFluidPressure::parse(const std::vector<std::string>& lines, util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;
        parser.setLine(line);
        size_t pos = 0;

        data_.ssid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.sf = parser.getDoubleAt(pos, realW).value_or(1.0);
        pos += realW;
        data_.pref = parser.getDoubleAt(pos, realW).value_or(0.0);
        break;
    }

    return true;
}

std::vector<std::string> LoadFluidPressure::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.ssid);
    writer.writeInt(data_.lcid);
    writer.writeDouble(data_.sf);
    writer.writeDouble(data_.pref);
    result.push_back(writer.getLine());

    return result;
}

void LoadFluidPressure::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(LoadFluidPressure, "*LOAD_FLUID_PRESSURE")

// ============================================================================
// LoadInteriorPressure
// ============================================================================

bool LoadInteriorPressure::parse(const std::vector<std::string>& lines, util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;
        parser.setLine(line);
        size_t pos = 0;

        data_.psid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.sf = parser.getDoubleAt(pos, realW).value_or(1.0);
        break;
    }

    return true;
}

std::vector<std::string> LoadInteriorPressure::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.psid);
    writer.writeInt(data_.lcid);
    writer.writeDouble(data_.sf);
    result.push_back(writer.getLine());

    return result;
}

void LoadInteriorPressure::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(LoadInteriorPressure, "*LOAD_INTERIOR_PRESSURE")

// ============================================================================
// LoadTyrePress
// ============================================================================

bool LoadTyrePress::parse(const std::vector<std::string>& lines, util::CardParser::Format format) {
    data_.clear();

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;
        parser.setLine(line);
        Data entry;
        size_t pos = 0;

        entry.pid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.sf = parser.getDoubleAt(pos, realW).value_or(1.0);
        pos += realW;
        entry.pressure = parser.getDoubleAt(pos, realW).value_or(0.0);

        if (entry.pid != 0) {
            data_.push_back(entry);
        }
    }

    return true;
}

std::vector<std::string> LoadTyrePress::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& entry : data_) {
        writer.clear();
        writer.writeInt(entry.pid);
        writer.writeInt(entry.lcid);
        writer.writeDouble(entry.sf);
        writer.writeDouble(entry.pressure);
        result.push_back(writer.getLine());
    }

    return result;
}

void LoadTyrePress::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(LoadTyrePress, "*LOAD_TYREPRESS")

// ============================================================================
// LoadAirbagPressure
// ============================================================================

bool LoadAirbagPressure::parse(const std::vector<std::string>& lines, util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;
        parser.setLine(line);
        size_t pos = 0;

        data_.psid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.sf = parser.getDoubleAt(pos, realW).value_or(1.0);
        pos += realW;
        data_.type = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> LoadAirbagPressure::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.psid);
    writer.writeInt(data_.lcid);
    writer.writeDouble(data_.sf);
    writer.writeInt(data_.type);
    result.push_back(writer.getLine());

    return result;
}

void LoadAirbagPressure::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(LoadAirbagPressure, "*LOAD_AIRBAG_PRESSURE")

// ============================================================================
// LoadLanczos
// ============================================================================

bool LoadLanczos::parse(const std::vector<std::string>& lines, util::CardParser::Format format) {
    data_.clear();

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;
        parser.setLine(line);
        Data entry;
        size_t pos = 0;

        entry.nsid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.dof = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.sf = parser.getDoubleAt(pos, realW).value_or(1.0);

        if (entry.nsid != 0) {
            data_.push_back(entry);
        }
    }

    return true;
}

std::vector<std::string> LoadLanczos::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& entry : data_) {
        writer.clear();
        writer.writeInt(entry.nsid);
        writer.writeInt(entry.dof);
        writer.writeInt(entry.lcid);
        writer.writeDouble(entry.sf);
        result.push_back(writer.getLine());
    }

    return result;
}

void LoadLanczos::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(LoadLanczos, "*LOAD_LANCZOS")

// ============================================================================
// LoadPressurePenetration
// ============================================================================

bool LoadPressurePenetration::parse(const std::vector<std::string>& lines, util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;
        parser.setLine(line);
        size_t pos = 0;

        data_.ssid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.sf = parser.getDoubleAt(pos, realW).value_or(1.0);
        pos += realW;
        data_.pmin = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.pmax = parser.getDoubleAt(pos, realW).value_or(0.0);
        break;
    }

    return true;
}

std::vector<std::string> LoadPressurePenetration::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.ssid);
    writer.writeInt(data_.lcid);
    writer.writeDouble(data_.sf);
    writer.writeDouble(data_.pmin);
    writer.writeDouble(data_.pmax);
    result.push_back(writer.getLine());

    return result;
}

void LoadPressurePenetration::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(LoadPressurePenetration, "*LOAD_PRESSURE_PENETRATION")

// ============================================================================
// LoadRail
// ============================================================================

bool LoadRail::parse(const std::vector<std::string>& lines, util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;
        parser.setLine(line);
        size_t pos = 0;

        data_.nsid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.sf = parser.getDoubleAt(pos, realW).value_or(1.0);
        pos += realW;
        data_.type = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> LoadRail::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.nsid);
    writer.writeInt(data_.lcid);
    writer.writeDouble(data_.sf);
    writer.writeInt(data_.type);
    result.push_back(writer.getLine());

    return result;
}

void LoadRail::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(LoadRail, "*LOAD_RAIL")

// ============================================================================
// LoadTrackTurn
// ============================================================================

bool LoadTrackTurn::parse(const std::vector<std::string>& lines, util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;
        parser.setLine(line);
        size_t pos = 0;

        data_.pid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.sf = parser.getDoubleAt(pos, realW).value_or(1.0);
        pos += realW;
        data_.radius = parser.getDoubleAt(pos, realW).value_or(0.0);
        break;
    }

    return true;
}

std::vector<std::string> LoadTrackTurn::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.pid);
    writer.writeInt(data_.lcid);
    writer.writeDouble(data_.sf);
    writer.writeDouble(data_.radius);
    result.push_back(writer.getLine());

    return result;
}

void LoadTrackTurn::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(LoadTrackTurn, "*LOAD_TRACK_TURN")

// ============================================================================
// LoadWheelPatch
// ============================================================================

bool LoadWheelPatch::parse(const std::vector<std::string>& lines, util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;
        parser.setLine(line);
        size_t pos = 0;

        data_.pid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.sf = parser.getDoubleAt(pos, realW).value_or(1.0);
        pos += realW;
        data_.xc = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.yc = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.zc = parser.getDoubleAt(pos, realW).value_or(0.0);
        break;
    }

    return true;
}

std::vector<std::string> LoadWheelPatch::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.pid);
    writer.writeInt(data_.lcid);
    writer.writeDouble(data_.sf);
    writer.writeDouble(data_.xc);
    writer.writeDouble(data_.yc);
    writer.writeDouble(data_.zc);
    result.push_back(writer.getLine());

    return result;
}

void LoadWheelPatch::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(LoadWheelPatch, "*LOAD_WHEEL_PATCH")

// ============================================================================
// LoadWave
// ============================================================================

bool LoadWave::parse(const std::vector<std::string>& lines, util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;
        parser.setLine(line);
        size_t pos = 0;

        data_.ssid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.sf = parser.getDoubleAt(pos, realW).value_or(1.0);
        pos += realW;
        data_.wtype = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.amp = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.freq = parser.getDoubleAt(pos, realW).value_or(0.0);
        break;
    }

    return true;
}

std::vector<std::string> LoadWave::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.ssid);
    writer.writeInt(data_.lcid);
    writer.writeDouble(data_.sf);
    writer.writeInt(data_.wtype);
    writer.writeDouble(data_.amp);
    writer.writeDouble(data_.freq);
    result.push_back(writer.getLine());

    return result;
}

void LoadWave::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(LoadWave, "*LOAD_WAVE")

// ============================================================================
// LoadPendulum
// ============================================================================

bool LoadPendulum::parse(const std::vector<std::string>& lines, util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;
        parser.setLine(line);
        size_t pos = 0;

        data_.pid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data_.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.sf = parser.getDoubleAt(pos, realW).value_or(1.0);
        pos += realW;
        data_.length = parser.getDoubleAt(pos, realW).value_or(0.0);
        break;
    }

    return true;
}

std::vector<std::string> LoadPendulum::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.pid);
    writer.writeInt(data_.lcid);
    writer.writeDouble(data_.sf);
    writer.writeDouble(data_.length);
    result.push_back(writer.getLine());

    return result;
}

void LoadPendulum::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(LoadPendulum, "*LOAD_PENDULUM")

// ============================================================================
// LoadThermalTopaz3d
// ============================================================================

bool LoadThermalTopaz3d::parse(const std::vector<std::string>& lines, util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    bool firstLine = true;
    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;
        if (firstLine) {
            parser.setLine(line);
            size_t pos = 0;
            data_.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
            pos += intW;
            data_.sf = parser.getDoubleAt(pos, realW).value_or(1.0);
            firstLine = false;
        } else {
            data_.filename = line;
            while (!data_.filename.empty() && (data_.filename.back() == ' ' || data_.filename.back() == '\r')) {
                data_.filename.pop_back();
            }
            break;
        }
    }

    return true;
}

std::vector<std::string> LoadThermalTopaz3d::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.lcid);
    writer.writeDouble(data_.sf);
    result.push_back(writer.getLine());

    if (!data_.filename.empty()) {
        result.push_back(data_.filename);
    }

    return result;
}

void LoadThermalTopaz3d::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(LoadThermalTopaz3d, "*LOAD_THERMAL_TOPAZ3D")

// ============================================================================
// LoadThermalBody
// ============================================================================

bool LoadThermalBody::parse(const std::vector<std::string>& lines, util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;
        parser.setLine(line);
        size_t pos = 0;

        data_.psid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.sf = parser.getDoubleAt(pos, realW).value_or(1.0);
        break;
    }

    return true;
}

std::vector<std::string> LoadThermalBody::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.psid);
    writer.writeInt(data_.lcid);
    writer.writeDouble(data_.sf);
    result.push_back(writer.getLine());

    return result;
}

void LoadThermalBody::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(LoadThermalBody, "*LOAD_THERMAL_BODY")

// ============================================================================
// LoadSoftElement
// ============================================================================

bool LoadSoftElement::parse(const std::vector<std::string>& lines, util::CardParser::Format format) {
    data_.clear();

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;
        parser.setLine(line);
        Data entry;
        size_t pos = 0;

        entry.eid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        entry.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.sf = parser.getDoubleAt(pos, realW).value_or(1.0);

        if (entry.eid != 0) {
            data_.push_back(entry);
        }
    }

    return true;
}

std::vector<std::string> LoadSoftElement::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& entry : data_) {
        writer.clear();
        writer.writeInt(entry.eid);
        writer.writeInt(entry.lcid);
        writer.writeDouble(entry.sf);
        result.push_back(writer.getLine());
    }

    return result;
}

void LoadSoftElement::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(LoadSoftElement, "*LOAD_SOFT_ELEMENT")

// ============================================================================
// LoadSurfaceStressSegment
// ============================================================================

bool LoadSurfaceStressSegment::parse(const std::vector<std::string>& lines, util::CardParser::Format format) {
    data_.clear();

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;
        parser.setLine(line);
        Data entry;
        size_t pos = 0;

        entry.ssid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.sf = parser.getDoubleAt(pos, realW).value_or(1.0);
        pos += realW;
        entry.sigmaxx = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        entry.sigmayy = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        entry.sigmaxy = parser.getDoubleAt(pos, realW).value_or(0.0);

        if (entry.ssid != 0) {
            data_.push_back(entry);
        }
    }

    return true;
}

std::vector<std::string> LoadSurfaceStressSegment::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& entry : data_) {
        writer.clear();
        writer.writeInt(entry.ssid);
        writer.writeInt(entry.lcid);
        writer.writeDouble(entry.sf);
        writer.writeDouble(entry.sigmaxx);
        writer.writeDouble(entry.sigmayy);
        writer.writeDouble(entry.sigmaxy);
        result.push_back(writer.getLine());
    }

    return result;
}

void LoadSurfaceStressSegment::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(LoadSurfaceStressSegment, "*LOAD_SURFACE_STRESS_SEGMENT")

// ============================================================================
// LoadMotionNodeSet
// ============================================================================

bool LoadMotionNodeSet::parse(const std::vector<std::string>& lines, util::CardParser::Format format) {
    data_.clear();

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;
        parser.setLine(line);
        Data entry;
        size_t pos = 0;

        entry.nsid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.dof = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.vad = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.sf = parser.getDoubleAt(pos, realW).value_or(1.0);
        pos += realW;
        entry.vid = static_cast<int>(parser.getInt64At(pos).value_or(0));

        if (entry.nsid != 0) {
            data_.push_back(entry);
        }
    }

    return true;
}

std::vector<std::string> LoadMotionNodeSet::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& entry : data_) {
        writer.clear();
        writer.writeInt(entry.nsid);
        writer.writeInt(entry.dof);
        writer.writeInt(entry.vad);
        writer.writeInt(entry.lcid);
        writer.writeDouble(entry.sf);
        writer.writeInt(entry.vid);
        result.push_back(writer.getLine());
    }

    return result;
}

void LoadMotionNodeSet::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(LoadMotionNodeSet, "*LOAD_MOTION_NODE_SET")

// ============================================================================
// LoadRadiation
// ============================================================================

bool LoadRadiation::parse(const std::vector<std::string>& lines, util::CardParser::Format format) {
    data_.clear();

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;
        parser.setLine(line);
        Data entry;
        size_t pos = 0;

        entry.ssid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.sf = parser.getDoubleAt(pos, realW).value_or(1.0);
        pos += realW;
        entry.emiss = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        entry.tamb = parser.getDoubleAt(pos, realW).value_or(0.0);

        if (entry.ssid != 0) {
            data_.push_back(entry);
        }
    }

    return true;
}

std::vector<std::string> LoadRadiation::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& entry : data_) {
        writer.clear();
        writer.writeInt(entry.ssid);
        writer.writeInt(entry.lcid);
        writer.writeDouble(entry.sf);
        writer.writeDouble(entry.emiss);
        writer.writeDouble(entry.tamb);
        result.push_back(writer.getLine());
    }

    return result;
}

void LoadRadiation::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(LoadRadiation, "*LOAD_RADIATION")

// ============================================================================
// LoadConvection
// ============================================================================

bool LoadConvection::parse(const std::vector<std::string>& lines, util::CardParser::Format format) {
    data_.clear();

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;
        parser.setLine(line);
        Data entry;
        size_t pos = 0;

        entry.ssid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.sf = parser.getDoubleAt(pos, realW).value_or(1.0);
        pos += realW;
        entry.h = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        entry.tamb = parser.getDoubleAt(pos, realW).value_or(0.0);

        if (entry.ssid != 0) {
            data_.push_back(entry);
        }
    }

    return true;
}

std::vector<std::string> LoadConvection::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& entry : data_) {
        writer.clear();
        writer.writeInt(entry.ssid);
        writer.writeInt(entry.lcid);
        writer.writeDouble(entry.sf);
        writer.writeDouble(entry.h);
        writer.writeDouble(entry.tamb);
        result.push_back(writer.getLine());
    }

    return result;
}

void LoadConvection::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(LoadConvection, "*LOAD_CONVECTION")

// ============================================================================
// LoadHeatFlux
// ============================================================================

bool LoadHeatFlux::parse(const std::vector<std::string>& lines, util::CardParser::Format format) {
    data_.clear();

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;
        parser.setLine(line);
        Data entry;
        size_t pos = 0;

        entry.ssid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.sf = parser.getDoubleAt(pos, realW).value_or(1.0);
        pos += realW;
        entry.flux = parser.getDoubleAt(pos, realW).value_or(0.0);

        if (entry.ssid != 0) {
            data_.push_back(entry);
        }
    }

    return true;
}

std::vector<std::string> LoadHeatFlux::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& entry : data_) {
        writer.clear();
        writer.writeInt(entry.ssid);
        writer.writeInt(entry.lcid);
        writer.writeDouble(entry.sf);
        writer.writeDouble(entry.flux);
        result.push_back(writer.getLine());
    }

    return result;
}

void LoadHeatFlux::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(LoadHeatFlux, "*LOAD_HEAT_FLUX")

// ============================================================================
// LoadSpc
// ============================================================================

bool LoadSpc::parse(const std::vector<std::string>& lines, util::CardParser::Format format) {
    data_.clear();

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;
        parser.setLine(line);
        Data entry;
        size_t pos = 0;

        entry.nid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        entry.dof = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.sf = parser.getDoubleAt(pos, realW).value_or(1.0);

        if (entry.nid != 0) {
            data_.push_back(entry);
        }
    }

    return true;
}

std::vector<std::string> LoadSpc::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& entry : data_) {
        writer.clear();
        writer.writeInt(entry.nid);
        writer.writeInt(entry.dof);
        writer.writeInt(entry.lcid);
        writer.writeDouble(entry.sf);
        result.push_back(writer.getLine());
    }

    return result;
}

void LoadSpc::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(LoadSpc, "*LOAD_SPC")

// ============================================================================
// LoadSpcSet
// ============================================================================

bool LoadSpcSet::parse(const std::vector<std::string>& lines, util::CardParser::Format format) {
    data_.clear();

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;
        parser.setLine(line);
        Data entry;
        size_t pos = 0;

        entry.nsid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.dof = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.sf = parser.getDoubleAt(pos, realW).value_or(1.0);

        if (entry.nsid != 0) {
            data_.push_back(entry);
        }
    }

    return true;
}

std::vector<std::string> LoadSpcSet::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& entry : data_) {
        writer.clear();
        writer.writeInt(entry.nsid);
        writer.writeInt(entry.dof);
        writer.writeInt(entry.lcid);
        writer.writeDouble(entry.sf);
        result.push_back(writer.getLine());
    }

    return result;
}

void LoadSpcSet::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(LoadSpcSet, "*LOAD_SPC_SET")

// ============================================================================
// LoadSegmentPressure
// ============================================================================

bool LoadSegmentPressure::parse(const std::vector<std::string>& lines, util::CardParser::Format format) {
    data_.clear();

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;
        parser.setLine(line);
        Data entry;
        size_t pos = 0;

        entry.ssid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.sf = parser.getDoubleAt(pos, realW).value_or(1.0);
        pos += realW;
        entry.p0 = parser.getDoubleAt(pos, realW).value_or(0.0);

        if (entry.ssid != 0) {
            data_.push_back(entry);
        }
    }

    return true;
}

std::vector<std::string> LoadSegmentPressure::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& entry : data_) {
        writer.clear();
        writer.writeInt(entry.ssid);
        writer.writeInt(entry.lcid);
        writer.writeDouble(entry.sf);
        writer.writeDouble(entry.p0);
        result.push_back(writer.getLine());
    }

    return result;
}

void LoadSegmentPressure::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(LoadSegmentPressure, "*LOAD_SEGMENT_PRESSURE")

// ============================================================================
// LoadAirmix
// ============================================================================

bool LoadAirmix::parse(const std::vector<std::string>& lines, util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;
        parser.setLine(line);
        size_t pos = 0;

        data_.psid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.sf = parser.getDoubleAt(pos, realW).value_or(1.0);
        break;
    }

    return true;
}

std::vector<std::string> LoadAirmix::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.psid);
    writer.writeInt(data_.lcid);
    writer.writeDouble(data_.sf);
    result.push_back(writer.getLine());

    return result;
}

void LoadAirmix::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(LoadAirmix, "*LOAD_AIRMIX")

} // namespace koo::dyna

