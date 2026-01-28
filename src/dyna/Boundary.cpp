#include <koo/dyna/Boundary.hpp>
#include <koo/dyna/ModelVisitor.hpp>
#include <koo/dyna/KeywordFactory.hpp>
#include <koo/util/StringUtils.hpp>

namespace koo::dyna {

// ============================================================================
// BoundarySpcNode
// ============================================================================

void BoundarySpcNode::addConstraint(NodeId nid, int dofx, int dofy, int dofz,
                                     int dofrx, int dofry, int dofrz) {
    SpcData spc;
    spc.nid = nid;
    spc.dofx = dofx;
    spc.dofy = dofy;
    spc.dofz = dofz;
    spc.dofrx = dofrx;
    spc.dofry = dofry;
    spc.dofrz = dofrz;
    constraints_.push_back(spc);
}

bool BoundarySpcNode::parse(const std::vector<std::string>& lines,
                             util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    constraints_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        SpcData spc;
        size_t pos = 0;

        // NID, CID, DOFX, DOFY, DOFZ, DOFRX, DOFRY, DOFRZ
        spc.nid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        spc.cid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        spc.dofx = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        spc.dofy = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        spc.dofz = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        spc.dofrx = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        spc.dofry = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        spc.dofrz = static_cast<int>(parser.getInt64At(pos).value_or(0));

        if (spc.nid != 0) {
            constraints_.push_back(spc);
        }
    }

    return true;
}

std::vector<std::string> BoundarySpcNode::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& spc : constraints_) {
        writer.clear();
        writer.writeInt(spc.nid);
        writer.writeInt(spc.cid);
        writer.writeInt(spc.dofx);
        writer.writeInt(spc.dofy);
        writer.writeInt(spc.dofz);
        writer.writeInt(spc.dofrx);
        writer.writeInt(spc.dofry);
        writer.writeInt(spc.dofrz);
        result.push_back(writer.getLine());
    }

    return result;
}

void BoundarySpcNode::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(BoundarySpcNode, "*BOUNDARY_SPC_NODE")

// ============================================================================
// BoundarySpcSet
// ============================================================================

bool BoundarySpcSet::parse(const std::vector<std::string>& lines,
                            util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    constraints_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        Data data;
        size_t pos = 0;

        // NSID, CID, DOFX, DOFY, DOFZ, DOFRX, DOFRY, DOFRZ
        data.nsid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.cid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.dofx = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.dofy = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.dofz = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.dofrx = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.dofry = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.dofrz = static_cast<int>(parser.getInt64At(pos).value_or(0));

        if (data.nsid != 0) {
            constraints_.push_back(data);
        }
    }

    return true;
}

std::vector<std::string> BoundarySpcSet::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& data : constraints_) {
        writer.clear();
        writer.writeInt(data.nsid);
        writer.writeInt(data.cid);
        writer.writeInt(data.dofx);
        writer.writeInt(data.dofy);
        writer.writeInt(data.dofz);
        writer.writeInt(data.dofrx);
        writer.writeInt(data.dofry);
        writer.writeInt(data.dofrz);
        result.push_back(writer.getLine());
    }

    return result;
}

void BoundarySpcSet::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(BoundarySpcSet, "*BOUNDARY_SPC_SET")

// ============================================================================
// BoundaryPrescribedMotionNode
// ============================================================================

bool BoundaryPrescribedMotionNode::parse(const std::vector<std::string>& lines,
                                           util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    motions_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        PrescribedMotionData motion;
        size_t pos = 0;

        // NID, DOF, VAD, LCID, SF, VID, DEATH, BIRTH
        motion.id = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        motion.dof = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        motion.vad = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        motion.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        motion.sf = parser.getDoubleAt(pos, realW).value_or(1.0);
        pos += realW;
        motion.vid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        motion.death = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        motion.birth = static_cast<int>(parser.getInt64At(pos).value_or(0));

        if (motion.id != 0) {
            motions_.push_back(motion);
        }
    }

    return true;
}

std::vector<std::string> BoundaryPrescribedMotionNode::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& motion : motions_) {
        writer.clear();
        writer.writeInt(motion.id);
        writer.writeInt(motion.dof);
        writer.writeInt(motion.vad);
        writer.writeInt(motion.lcid);
        writer.writeDouble(motion.sf);
        writer.writeInt(motion.vid);
        writer.writeInt(motion.death);
        writer.writeInt(motion.birth);
        result.push_back(writer.getLine());
    }

    return result;
}

void BoundaryPrescribedMotionNode::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(BoundaryPrescribedMotionNode, "*BOUNDARY_PRESCRIBED_MOTION_NODE")

// ============================================================================
// BoundaryPrescribedMotionSet
// ============================================================================

bool BoundaryPrescribedMotionSet::parse(const std::vector<std::string>& lines,
                                          util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    motions_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        PrescribedMotionData motion;
        size_t pos = 0;

        // NSID, DOF, VAD, LCID, SF, VID, DEATH, BIRTH
        motion.id = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        motion.dof = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        motion.vad = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        motion.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        motion.sf = parser.getDoubleAt(pos, realW).value_or(1.0);
        pos += realW;
        motion.vid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        motion.death = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        motion.birth = static_cast<int>(parser.getInt64At(pos).value_or(0));

        if (motion.id != 0) {
            motions_.push_back(motion);
        }
    }

    return true;
}

std::vector<std::string> BoundaryPrescribedMotionSet::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& motion : motions_) {
        writer.clear();
        writer.writeInt(motion.id);
        writer.writeInt(motion.dof);
        writer.writeInt(motion.vad);
        writer.writeInt(motion.lcid);
        writer.writeDouble(motion.sf);
        writer.writeInt(motion.vid);
        writer.writeInt(motion.death);
        writer.writeInt(motion.birth);
        result.push_back(writer.getLine());
    }

    return result;
}

void BoundaryPrescribedMotionSet::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(BoundaryPrescribedMotionSet, "*BOUNDARY_PRESCRIBED_MOTION_SET")

// ============================================================================
// BoundaryPrescribedMotionRigid
// ============================================================================

bool BoundaryPrescribedMotionRigid::parse(const std::vector<std::string>& lines,
                                            util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    motions_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        PrescribedMotionData motion;
        size_t pos = 0;

        // PID, DOF, VAD, LCID, SF, VID, DEATH, BIRTH
        motion.id = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        motion.dof = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        motion.vad = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        motion.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        motion.sf = parser.getDoubleAt(pos, realW).value_or(1.0);
        pos += realW;
        motion.vid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        motion.death = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        motion.birth = static_cast<int>(parser.getInt64At(pos).value_or(0));

        if (motion.id != 0) {
            motions_.push_back(motion);
        }
    }

    return true;
}

std::vector<std::string> BoundaryPrescribedMotionRigid::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& motion : motions_) {
        writer.clear();
        writer.writeInt(motion.id);
        writer.writeInt(motion.dof);
        writer.writeInt(motion.vad);
        writer.writeInt(motion.lcid);
        writer.writeDouble(motion.sf);
        writer.writeInt(motion.vid);
        writer.writeInt(motion.death);
        writer.writeInt(motion.birth);
        result.push_back(writer.getLine());
    }

    return result;
}

void BoundaryPrescribedMotionRigid::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(BoundaryPrescribedMotionRigid, "*BOUNDARY_PRESCRIBED_MOTION_RIGID")

// ============================================================================
// BoundaryThermalNode
// ============================================================================

bool BoundaryThermalNode::parse(const std::vector<std::string>& lines,
                                  util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    constraints_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        Data data;
        size_t pos = 0;

        data.nid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.sf = parser.getDoubleAt(pos, realW).value_or(1.0);

        if (data.nid != 0) {
            constraints_.push_back(data);
        }
    }

    return true;
}

std::vector<std::string> BoundaryThermalNode::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& data : constraints_) {
        writer.clear();
        writer.writeInt(data.nid);
        writer.writeInt(data.lcid);
        writer.writeDouble(data.sf);
        result.push_back(writer.getLine());
    }

    return result;
}

void BoundaryThermalNode::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(BoundaryThermalNode, "*BOUNDARY_THERMAL_NODE")

// ============================================================================
// BoundaryThermalSet
// ============================================================================

bool BoundaryThermalSet::parse(const std::vector<std::string>& lines,
                                 util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    constraints_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        Data data;
        size_t pos = 0;

        data.nsid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.sf = parser.getDoubleAt(pos, realW).value_or(1.0);

        if (data.nsid != 0) {
            constraints_.push_back(data);
        }
    }

    return true;
}

std::vector<std::string> BoundaryThermalSet::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& data : constraints_) {
        writer.clear();
        writer.writeInt(data.nsid);
        writer.writeInt(data.lcid);
        writer.writeDouble(data.sf);
        result.push_back(writer.getLine());
    }

    return result;
}

void BoundaryThermalSet::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(BoundaryThermalSet, "*BOUNDARY_THERMAL_SET")

// ============================================================================
// BoundaryConvectionSet
// ============================================================================

bool BoundaryConvectionSet::parse(const std::vector<std::string>& lines,
                                    util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    conditions_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        Data data;
        size_t pos = 0;

        data.ssid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.pseteflag = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.hlcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.hmult = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.tlcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.tmult = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.loc = parser.getDoubleAt(pos, realW).value_or(0.0);

        if (data.ssid != 0) {
            conditions_.push_back(data);
        }
    }

    return true;
}

std::vector<std::string> BoundaryConvectionSet::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& data : conditions_) {
        writer.clear();
        writer.writeInt(data.ssid);
        writer.writeInt(data.pseteflag);
        writer.writeInt(data.hlcid);
        writer.writeInt(data.hmult);
        writer.writeInt(data.tlcid);
        writer.writeInt(data.tmult);
        writer.writeDouble(data.loc);
        result.push_back(writer.getLine());
    }

    return result;
}

void BoundaryConvectionSet::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(BoundaryConvectionSet, "*BOUNDARY_CONVECTION_SET")

// ============================================================================
// BoundaryRadiationSet
// ============================================================================

bool BoundaryRadiationSet::parse(const std::vector<std::string>& lines,
                                   util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    conditions_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        Data data;
        size_t pos = 0;

        data.ssid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.pseteflag = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.type = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.elcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.tlcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.loc = parser.getDoubleAt(pos, realW).value_or(0.0);

        if (data.ssid != 0) {
            conditions_.push_back(data);
        }
    }

    return true;
}

std::vector<std::string> BoundaryRadiationSet::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& data : conditions_) {
        writer.clear();
        writer.writeInt(data.ssid);
        writer.writeInt(data.pseteflag);
        writer.writeInt(data.type);
        writer.writeInt(data.elcid);
        writer.writeInt(data.tlcid);
        writer.writeDouble(data.loc);
        result.push_back(writer.getLine());
    }

    return result;
}

void BoundaryRadiationSet::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(BoundaryRadiationSet, "*BOUNDARY_RADIATION_SET")

// ============================================================================
// BoundaryFluxSet
// ============================================================================

bool BoundaryFluxSet::parse(const std::vector<std::string>& lines,
                              util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    conditions_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        Data data;
        size_t pos = 0;

        data.ssid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.pseteflag = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.sf = parser.getDoubleAt(pos, realW).value_or(1.0);
        pos += realW;
        data.loc = parser.getDoubleAt(pos, realW).value_or(0.0);

        if (data.ssid != 0) {
            conditions_.push_back(data);
        }
    }

    return true;
}

std::vector<std::string> BoundaryFluxSet::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& data : conditions_) {
        writer.clear();
        writer.writeInt(data.ssid);
        writer.writeInt(data.pseteflag);
        writer.writeInt(data.lcid);
        writer.writeDouble(data.sf);
        writer.writeDouble(data.loc);
        result.push_back(writer.getLine());
    }

    return result;
}

void BoundaryFluxSet::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(BoundaryFluxSet, "*BOUNDARY_FLUX_SET")

// ============================================================================
// BoundaryNonReflecting
// ============================================================================

bool BoundaryNonReflecting::parse(const std::vector<std::string>& lines,
                                    util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    conditions_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        Data data;
        size_t pos = 0;

        data.ssid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.ad = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.as = static_cast<int>(parser.getInt64At(pos).value_or(0));

        if (data.ssid != 0) {
            conditions_.push_back(data);
        }
    }

    return true;
}

std::vector<std::string> BoundaryNonReflecting::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& data : conditions_) {
        writer.clear();
        writer.writeInt(data.ssid);
        writer.writeInt(data.ad);
        writer.writeInt(data.as);
        result.push_back(writer.getLine());
    }

    return result;
}

void BoundaryNonReflecting::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(BoundaryNonReflecting, "*BOUNDARY_NON_REFLECTING")

// ============================================================================
// BoundarySpcSetBirthDeath
// ============================================================================

bool BoundarySpcSetBirthDeath::parse(const std::vector<std::string>& lines,
                                       util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    constraints_.clear();

    size_t lineIdx = 0;
    while (lineIdx < lines.size()) {
        if (util::CardParser::isCommentLine(lines[lineIdx])) {
            ++lineIdx;
            continue;
        }

        Data data;

        // Card 1: NSID, CID, DOFX, DOFY, DOFZ, DOFRX, DOFRY, DOFRZ
        parser.setLine(lines[lineIdx]);
        size_t pos = 0;

        data.nsid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.cid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.dofx = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.dofy = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.dofz = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.dofrx = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.dofry = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.dofrz = static_cast<int>(parser.getInt64At(pos).value_or(0));
        ++lineIdx;

        // Card 2: BIRTH, DEATH
        if (lineIdx < lines.size() && !util::CardParser::isCommentLine(lines[lineIdx])) {
            parser.setLine(lines[lineIdx]);
            pos = 0;
            data.birth = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data.death = parser.getDoubleAt(pos, realW).value_or(1.0e28);
            ++lineIdx;
        }

        if (data.nsid != 0) {
            constraints_.push_back(data);
        }
    }

    return true;
}

std::vector<std::string> BoundarySpcSetBirthDeath::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& data : constraints_) {
        writer.clear();
        writer.writeInt(data.nsid);
        writer.writeInt(data.cid);
        writer.writeInt(data.dofx);
        writer.writeInt(data.dofy);
        writer.writeInt(data.dofz);
        writer.writeInt(data.dofrx);
        writer.writeInt(data.dofry);
        writer.writeInt(data.dofrz);
        result.push_back(writer.getLine());

        writer.clear();
        writer.writeDouble(data.birth);
        writer.writeDouble(data.death);
        result.push_back(writer.getLine());
    }

    return result;
}

void BoundarySpcSetBirthDeath::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(BoundarySpcSetBirthDeath, "*BOUNDARY_SPC_SET_BIRTH_DEATH")

// ============================================================================
// BoundaryPrescribedMotionSetBox
// ============================================================================

bool BoundaryPrescribedMotionSetBox::parse(const std::vector<std::string>& lines,
                                             util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    motions_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        Data data;
        size_t pos = 0;

        data.nsid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.dof = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.vad = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.sf = parser.getDoubleAt(pos, realW).value_or(1.0);
        pos += realW;
        data.vid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.death = parser.getDoubleAt(pos, realW).value_or(1e28);
        pos += realW;
        data.birth = parser.getDoubleAt(pos, realW).value_or(0.0);

        if (data.nsid != 0) {
            motions_.push_back(data);
        }
    }

    return true;
}

std::vector<std::string> BoundaryPrescribedMotionSetBox::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& data : motions_) {
        writer.clear();
        writer.writeInt(data.nsid);
        writer.writeInt(data.dof);
        writer.writeInt(data.vad);
        writer.writeInt(data.lcid);
        writer.writeDouble(data.sf);
        writer.writeInt(data.vid);
        writer.writeDouble(data.death);
        writer.writeDouble(data.birth);
        result.push_back(writer.getLine());
    }

    return result;
}

void BoundaryPrescribedMotionSetBox::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(BoundaryPrescribedMotionSetBox, "*BOUNDARY_PRESCRIBED_MOTION_SET_BOX")

// ============================================================================
// BoundaryCyclic
// ============================================================================

bool BoundaryCyclic::parse(const std::vector<std::string>& lines,
                            util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;

        data_.nsid1 = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.nsid2 = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.nid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.cx = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.cy = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.cz = parser.getDoubleAt(pos, realW).value_or(0.0);

        break;
    }

    return true;
}

std::vector<std::string> BoundaryCyclic::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.nsid1);
    writer.writeInt(data_.nsid2);
    writer.writeInt(data_.nid);
    writer.writeDouble(data_.cx);
    writer.writeDouble(data_.cy);
    writer.writeDouble(data_.cz);
    result.push_back(writer.getLine());

    return result;
}

void BoundaryCyclic::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(BoundaryCyclic, "*BOUNDARY_CYCLIC")

// ============================================================================
// BoundarySlidingPlane
// ============================================================================

bool BoundarySlidingPlane::parse(const std::vector<std::string>& lines,
                                   util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    conditions_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        Data data;
        size_t pos = 0;

        data.nsid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.vector = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.xt = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data.yt = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data.zt = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data.xh = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data.yh = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data.zh = parser.getDoubleAt(pos, realW).value_or(1.0);

        if (data.nsid != 0) {
            conditions_.push_back(data);
        }
    }

    return true;
}

std::vector<std::string> BoundarySlidingPlane::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& data : conditions_) {
        writer.clear();
        writer.writeInt(data.nsid);
        writer.writeInt(data.vector);
        writer.writeDouble(data.xt);
        writer.writeDouble(data.yt);
        writer.writeDouble(data.zt);
        writer.writeDouble(data.xh);
        writer.writeDouble(data.yh);
        writer.writeDouble(data.zh);
        result.push_back(writer.getLine());
    }

    return result;
}

void BoundarySlidingPlane::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(BoundarySlidingPlane, "*BOUNDARY_SLIDING_PLANE")

// ============================================================================
// BoundaryAmbientEos
// ============================================================================

bool BoundaryAmbientEos::parse(const std::vector<std::string>& lines,
                                 util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;

        data_.ssid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.eos = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.r0 = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.p0 = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.t0 = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.e0 = parser.getDoubleAt(pos, realW).value_or(0.0);

        break;
    }

    return true;
}

std::vector<std::string> BoundaryAmbientEos::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.ssid);
    writer.writeInt(data_.eos);
    writer.writeDouble(data_.r0);
    writer.writeDouble(data_.p0);
    writer.writeDouble(data_.t0);
    writer.writeDouble(data_.e0);
    result.push_back(writer.getLine());

    return result;
}

void BoundaryAmbientEos::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(BoundaryAmbientEos, "*BOUNDARY_AMBIENT_EOS")

// ============================================================================
// BoundaryFluxTrajectory
// ============================================================================

bool BoundaryFluxTrajectory::parse(const std::vector<std::string>& lines,
                                     util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    conditions_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        Data data;
        size_t pos = 0;

        data.pid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.ssid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.sf = parser.getDoubleAt(pos, realW).value_or(1.0);
        pos += realW;
        data.loc = parser.getDoubleAt(pos, realW).value_or(0.0);

        if (data.pid != 0 || data.ssid != 0) {
            conditions_.push_back(data);
        }
    }

    return true;
}

std::vector<std::string> BoundaryFluxTrajectory::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& data : conditions_) {
        writer.clear();
        writer.writeInt(data.pid);
        writer.writeInt(data.ssid);
        writer.writeInt(data.lcid);
        writer.writeDouble(data.sf);
        writer.writeDouble(data.loc);
        result.push_back(writer.getLine());
    }

    return result;
}

void BoundaryFluxTrajectory::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(BoundaryFluxTrajectory, "*BOUNDARY_FLUX_TRAJECTORY")

// ============================================================================
// BoundaryPap
// ============================================================================

bool BoundaryPap::parse(const std::vector<std::string>& lines,
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
        data_.death = parser.getDoubleAt(pos, realW).value_or(1e28);
        pos += realW;
        data_.birth = parser.getDoubleAt(pos, realW).value_or(0.0);
        break;
    }

    return true;
}

std::vector<std::string> BoundaryPap::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.ssid);
    writer.writeInt(data_.lcid);
    writer.writeDouble(data_.sf);
    writer.writeDouble(data_.death);
    writer.writeDouble(data_.birth);
    result.push_back(writer.getLine());

    return result;
}

void BoundaryPap::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(BoundaryPap, "*BOUNDARY_PAP")

// ============================================================================
// BoundaryAcousticImpedance
// ============================================================================

bool BoundaryAcousticImpedance::parse(const std::vector<std::string>& lines,
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
        data_.imp = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.sf = parser.getDoubleAt(pos, realW).value_or(1.0);
        break;
    }

    return true;
}

std::vector<std::string> BoundaryAcousticImpedance::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.ssid);
    writer.writeDouble(data_.imp);
    writer.writeInt(data_.lcid);
    writer.writeDouble(data_.sf);
    result.push_back(writer.getLine());

    return result;
}

void BoundaryAcousticImpedance::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(BoundaryAcousticImpedance, "*BOUNDARY_ACOUSTIC_IMPEDANCE")

// ============================================================================
// BoundaryPrescribedAccelerometer
// ============================================================================

bool BoundaryPrescribedAccelerometer::parse(const std::vector<std::string>& lines,
                                              util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    conditions_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        Data data;
        size_t pos = 0;

        // NSID, DOF, ACCID, SF, VID, DEATH, BIRTH
        data.nsid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.dof = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.accid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.sf = parser.getDoubleAt(pos, realW).value_or(1.0);
        pos += realW;
        data.vid = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data.death = parser.getDoubleAt(pos, realW).value_or(1e28);
        pos += realW;
        data.birth = parser.getDoubleAt(pos, realW).value_or(0.0);

        if (data.nsid != 0) {
            conditions_.push_back(data);
        }
    }

    return true;
}

std::vector<std::string> BoundaryPrescribedAccelerometer::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& data : conditions_) {
        writer.clear();
        writer.writeInt(data.nsid);
        writer.writeInt(data.dof);
        writer.writeInt(data.accid);
        writer.writeDouble(data.sf);
        writer.writeDouble(data.vid);
        writer.writeDouble(data.death);
        writer.writeDouble(data.birth);
        result.push_back(writer.getLine());
    }

    return result;
}

void BoundaryPrescribedAccelerometer::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(BoundaryPrescribedAccelerometer, "*BOUNDARY_PRESCRIBED_ACCELEROMETER")

// ============================================================================
// BoundaryPrescribedOrientationRigid
// ============================================================================

bool BoundaryPrescribedOrientationRigid::parse(const std::vector<std::string>& lines,
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
        entry.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.cid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.sf = parser.getDoubleAt(pos, realW).value_or(1.0);
        pos += realW;
        entry.dir = static_cast<int>(parser.getInt64At(pos).value_or(0));

        if (entry.pid != 0) {
            data_.push_back(entry);
        }
    }

    return true;
}

std::vector<std::string> BoundaryPrescribedOrientationRigid::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& entry : data_) {
        writer.clear();
        writer.writeInt(entry.pid);
        writer.writeInt(entry.lcid);
        writer.writeInt(entry.cid);
        writer.writeDouble(entry.sf);
        writer.writeInt(entry.dir);
        result.push_back(writer.getLine());
    }

    return result;
}

void BoundaryPrescribedOrientationRigid::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(BoundaryPrescribedOrientationRigid, "*BOUNDARY_PRESCRIBED_ORIENTATION_RIGID")

// ============================================================================
// BoundarySpcSymmetryPlane
// ============================================================================

bool BoundarySpcSymmetryPlane::parse(const std::vector<std::string>& lines,
                                      util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    data_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        Data entry;
        size_t pos = 0;

        entry.nsid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.plane = static_cast<int>(parser.getInt64At(pos).value_or(0));

        if (entry.nsid != 0) {
            data_.push_back(entry);
        }
    }

    return true;
}

std::vector<std::string> BoundarySpcSymmetryPlane::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& entry : data_) {
        writer.clear();
        writer.writeInt(entry.nsid);
        writer.writeInt(entry.plane);
        result.push_back(writer.getLine());
    }

    return result;
}

void BoundarySpcSymmetryPlane::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(BoundarySpcSymmetryPlane, "*BOUNDARY_SPC_SYMMETRY_PLANE")

// ============================================================================
// BoundaryPrecrack
// ============================================================================

bool BoundaryPrecrack::parse(const std::vector<std::string>& lines,
                              util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    data_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        Data entry;
        size_t pos = 0;

        entry.pid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.n1 = parser.getInt64At(pos).value_or(0);
        pos += intW;
        entry.n2 = parser.getInt64At(pos).value_or(0);
        pos += intW;
        entry.n3 = parser.getInt64At(pos).value_or(0);
        pos += intW;
        entry.n4 = parser.getInt64At(pos).value_or(0);

        if (entry.pid != 0) {
            data_.push_back(entry);
        }
    }

    return true;
}

std::vector<std::string> BoundaryPrecrack::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& entry : data_) {
        writer.clear();
        writer.writeInt(entry.pid);
        writer.writeInt(entry.n1);
        writer.writeInt(entry.n2);
        writer.writeInt(entry.n3);
        writer.writeInt(entry.n4);
        result.push_back(writer.getLine());
    }

    return result;
}

void BoundaryPrecrack::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(BoundaryPrecrack, "*BOUNDARY_PRECRACK")

// ============================================================================
// BoundaryMcol
// ============================================================================

bool BoundaryMcol::parse(const std::vector<std::string>& lines,
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
        data_.psid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.sf = parser.getDoubleAt(pos, realW).value_or(1.0);
        break;
    }

    return true;
}

std::vector<std::string> BoundaryMcol::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.id);
    writer.writeInt(data_.type);
    writer.writeInt(data_.psid);
    writer.writeDouble(data_.sf);

    result.push_back(writer.getLine());
    return result;
}

void BoundaryMcol::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(BoundaryMcol, "*BOUNDARY_MCOL")

// ============================================================================
// BoundaryPrescribedFinalGeometry
// ============================================================================

bool BoundaryPrescribedFinalGeometry::parse(const std::vector<std::string>& lines,
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
        data_.ssid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.tol = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> BoundaryPrescribedFinalGeometry::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.psid);
    writer.writeInt(data_.ssid);
    writer.writeDouble(data_.tol);
    writer.writeInt(data_.lcid);

    result.push_back(writer.getLine());
    return result;
}

void BoundaryPrescribedFinalGeometry::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(BoundaryPrescribedFinalGeometry, "*BOUNDARY_PRESCRIBED_FINAL_GEOMETRY")

// ============================================================================
// BoundarySphSymmetryPlane
// ============================================================================

bool BoundarySphSymmetryPlane::parse(const std::vector<std::string>& lines,
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

        entry.plane = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.coord = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        entry.dir = static_cast<int>(parser.getInt64At(pos).value_or(0));

        if (entry.plane != 0) {
            data_.push_back(entry);
        }
    }

    return true;
}

std::vector<std::string> BoundarySphSymmetryPlane::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& entry : data_) {
        writer.clear();
        writer.writeInt(entry.plane);
        writer.writeDouble(entry.coord);
        writer.writeInt(entry.dir);
        result.push_back(writer.getLine());
    }

    return result;
}

void BoundarySphSymmetryPlane::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(BoundarySphSymmetryPlane, "*BOUNDARY_SPH_SYMMETRY_PLANE")

// ============================================================================
// BoundarySphFlow
// ============================================================================

bool BoundarySphFlow::parse(const std::vector<std::string>& lines,
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
        data_.opt = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> BoundarySphFlow::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.ssid);
    writer.writeInt(data_.lcid);
    writer.writeDouble(data_.sf);
    writer.writeInt(data_.opt);

    result.push_back(writer.getLine());
    return result;
}

void BoundarySphFlow::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(BoundarySphFlow, "*BOUNDARY_SPH_FLOW")

// ============================================================================
// BoundarySphNonReflecting
// ============================================================================

bool BoundarySphNonReflecting::parse(const std::vector<std::string>& lines,
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
        data_.opt = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.damp = parser.getDoubleAt(pos, realW).value_or(0.0);
        break;
    }

    return true;
}

std::vector<std::string> BoundarySphNonReflecting::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.ssid);
    writer.writeInt(data_.opt);
    writer.writeDouble(data_.damp);

    result.push_back(writer.getLine());
    return result;
}

void BoundarySphNonReflecting::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(BoundarySphNonReflecting, "*BOUNDARY_SPH_NON_REFLECTING")

// ============================================================================
// BoundaryAcousticCoupling
// ============================================================================

bool BoundaryAcousticCoupling::parse(const std::vector<std::string>& lines,
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
        data_.psid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.ctype = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.sf = parser.getDoubleAt(pos, realW).value_or(1.0);
        break;
    }

    return true;
}

std::vector<std::string> BoundaryAcousticCoupling::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.ssid);
    writer.writeInt(data_.psid);
    writer.writeInt(data_.ctype);
    writer.writeDouble(data_.sf);

    result.push_back(writer.getLine());
    return result;
}

void BoundaryAcousticCoupling::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(BoundaryAcousticCoupling, "*BOUNDARY_ACOUSTIC_COUPLING")

// ============================================================================
// BoundaryAcousticFreeSurface
// ============================================================================

bool BoundaryAcousticFreeSurface::parse(const std::vector<std::string>& lines,
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
        data_.pres = parser.getDoubleAt(pos, realW).value_or(0.0);
        break;
    }

    return true;
}

std::vector<std::string> BoundaryAcousticFreeSurface::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.ssid);
    writer.writeDouble(data_.pres);

    result.push_back(writer.getLine());
    return result;
}

void BoundaryAcousticFreeSurface::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(BoundaryAcousticFreeSurface, "*BOUNDARY_ACOUSTIC_FREE_SURFACE")

// ============================================================================
// BoundaryAcousticNonReflecting
// ============================================================================

bool BoundaryAcousticNonReflecting::parse(const std::vector<std::string>& lines,
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
        data_.ntype = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> BoundaryAcousticNonReflecting::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.ssid);
    writer.writeInt(data_.ntype);

    result.push_back(writer.getLine());
    return result;
}

void BoundaryAcousticNonReflecting::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(BoundaryAcousticNonReflecting, "*BOUNDARY_ACOUSTIC_NON_REFLECTING")

// ============================================================================
// BoundaryAcousticMapping
// ============================================================================

bool BoundaryAcousticMapping::parse(const std::vector<std::string>& lines,
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
        data_.msid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.mtype = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> BoundaryAcousticMapping::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.ssid);
    writer.writeInt(data_.msid);
    writer.writeInt(data_.mtype);

    result.push_back(writer.getLine());
    return result;
}

void BoundaryAcousticMapping::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(BoundaryAcousticMapping, "*BOUNDARY_ACOUSTIC_MAPPING")

// ============================================================================
// BoundaryAleMapping
// ============================================================================

bool BoundaryAleMapping::parse(const std::vector<std::string>& lines,
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
        data_.msid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.mtype = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.sf = parser.getDoubleAt(pos, realW).value_or(1.0);
        break;
    }

    return true;
}

std::vector<std::string> BoundaryAleMapping::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.ssid);
    writer.writeInt(data_.msid);
    writer.writeInt(data_.mtype);
    writer.writeDouble(data_.sf);

    result.push_back(writer.getLine());
    return result;
}

void BoundaryAleMapping::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(BoundaryAleMapping, "*BOUNDARY_ALE_MAPPING")

// ============================================================================
// BoundaryAmbient
// ============================================================================

bool BoundaryAmbient::parse(const std::vector<std::string>& lines,
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
        data_.ammgid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.rho = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.temp = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.pres = parser.getDoubleAt(pos, realW).value_or(0.0);
        break;
    }

    return true;
}

std::vector<std::string> BoundaryAmbient::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.ssid);
    writer.writeInt(data_.ammgid);
    writer.writeDouble(data_.rho);
    writer.writeDouble(data_.temp);
    writer.writeDouble(data_.pres);

    result.push_back(writer.getLine());
    return result;
}

void BoundaryAmbient::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(BoundaryAmbient, "*BOUNDARY_AMBIENT")

// ============================================================================
// BoundaryDeNonReflecting
// ============================================================================

bool BoundaryDeNonReflecting::parse(const std::vector<std::string>& lines,
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
        data_.opt = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> BoundaryDeNonReflecting::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.ssid);
    writer.writeInt(data_.opt);

    result.push_back(writer.getLine());
    return result;
}

void BoundaryDeNonReflecting::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(BoundaryDeNonReflecting, "*BOUNDARY_DE_NON_REFLECTING")

// ============================================================================
// BoundaryElementMethod
// ============================================================================

bool BoundaryElementMethod::parse(const std::vector<std::string>& lines,
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
        data_.sf = parser.getDoubleAt(pos, realW).value_or(1.0);
        break;
    }

    return true;
}

std::vector<std::string> BoundaryElementMethod::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.id);
    writer.writeInt(data_.type);
    writer.writeInt(data_.ssid);
    writer.writeDouble(data_.sf);

    result.push_back(writer.getLine());
    return result;
}

void BoundaryElementMethod::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(BoundaryElementMethod, "*BOUNDARY_ELEMENT_METHOD")

// ============================================================================
// BoundaryElementMethodControl
// ============================================================================

bool BoundaryElementMethodControl::parse(const std::vector<std::string>& lines,
                                           util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;

        data_.bemid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.solver = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.tol = parser.getDoubleAt(pos, realW).value_or(1e-6);
        pos += realW;
        data_.maxiter = static_cast<int>(parser.getInt64At(pos).value_or(100));
        break;
    }

    return true;
}

std::vector<std::string> BoundaryElementMethodControl::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.bemid);
    writer.writeInt(data_.solver);
    writer.writeDouble(data_.tol);
    writer.writeInt(data_.maxiter);

    result.push_back(writer.getLine());
    return result;
}

void BoundaryElementMethodControl::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(BoundaryElementMethodControl, "*BOUNDARY_ELEMENT_METHOD_CONTROL")

// ============================================================================
// BoundaryNonReflecting2D
// ============================================================================

bool BoundaryNonReflecting2D::parse(const std::vector<std::string>& lines,
                                      util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    conditions_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        Data data;
        size_t pos = 0;

        data.ssid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.ad = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.as = static_cast<int>(parser.getInt64At(pos).value_or(0));

        if (data.ssid != 0) {
            conditions_.push_back(data);
        }
    }

    return true;
}

std::vector<std::string> BoundaryNonReflecting2D::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& data : conditions_) {
        writer.clear();
        writer.writeInt(data.ssid);
        writer.writeInt(data.ad);
        writer.writeInt(data.as);
        result.push_back(writer.getLine());
    }

    return result;
}

void BoundaryNonReflecting2D::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(BoundaryNonReflecting2D, "*BOUNDARY_NON_REFLECTING_2D")

// ============================================================================
// BoundaryPoreFluid
// ============================================================================

bool BoundaryPoreFluid::parse(const std::vector<std::string>& lines,
                                util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    conditions_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        Data data;
        size_t pos = 0;

        data.nsid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.sf = parser.getDoubleAt(pos, realW).value_or(1.0);

        if (data.nsid != 0) {
            conditions_.push_back(data);
        }
    }

    return true;
}

std::vector<std::string> BoundaryPoreFluid::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& data : conditions_) {
        writer.clear();
        writer.writeInt(data.nsid);
        writer.writeInt(data.lcid);
        writer.writeDouble(data.sf);
        result.push_back(writer.getLine());
    }

    return result;
}

void BoundaryPoreFluid::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(BoundaryPoreFluid, "*BOUNDARY_PORE_FLUID")

// ============================================================================
// BoundaryPrescribedMotionRigidLocal
// ============================================================================

bool BoundaryPrescribedMotionRigidLocal::parse(const std::vector<std::string>& lines,
                                                 util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    motions_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        Data data;
        size_t pos = 0;

        data.pid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.dof = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.vad = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.sf = parser.getDoubleAt(pos, realW).value_or(1.0);
        pos += realW;
        data.vid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.death = parser.getDoubleAt(pos, realW).value_or(1e28);
        pos += realW;
        data.birth = parser.getDoubleAt(pos, realW).value_or(0.0);

        if (data.pid != 0) {
            motions_.push_back(data);
        }
    }

    return true;
}

std::vector<std::string> BoundaryPrescribedMotionRigidLocal::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& data : motions_) {
        writer.clear();
        writer.writeInt(data.pid);
        writer.writeInt(data.dof);
        writer.writeInt(data.vad);
        writer.writeInt(data.lcid);
        writer.writeDouble(data.sf);
        writer.writeInt(data.vid);
        writer.writeDouble(data.death);
        writer.writeDouble(data.birth);
        result.push_back(writer.getLine());
    }

    return result;
}

void BoundaryPrescribedMotionRigidLocal::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(BoundaryPrescribedMotionRigidLocal, "*BOUNDARY_PRESCRIBED_MOTION_RIGID_LOCAL")

// ============================================================================
// BoundaryPressureOutflow
// ============================================================================

bool BoundaryPressureOutflow::parse(const std::vector<std::string>& lines,
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
        data_.pref = parser.getDoubleAt(pos, realW).value_or(0.0);
        break;
    }

    return true;
}

std::vector<std::string> BoundaryPressureOutflow::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.ssid);
    writer.writeInt(data_.lcid);
    writer.writeDouble(data_.sf);
    writer.writeDouble(data_.pref);

    result.push_back(writer.getLine());
    return result;
}

void BoundaryPressureOutflow::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(BoundaryPressureOutflow, "*BOUNDARY_PRESSURE_OUTFLOW")

// ============================================================================
// BoundaryPwp
// ============================================================================

bool BoundaryPwp::parse(const std::vector<std::string>& lines,
                          util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    conditions_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        Data data;
        size_t pos = 0;

        data.nsid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.sf = parser.getDoubleAt(pos, realW).value_or(1.0);
        pos += realW;
        data.ref = parser.getDoubleAt(pos, realW).value_or(0.0);

        if (data.nsid != 0) {
            conditions_.push_back(data);
        }
    }

    return true;
}

std::vector<std::string> BoundaryPwp::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& data : conditions_) {
        writer.clear();
        writer.writeInt(data.nsid);
        writer.writeInt(data.lcid);
        writer.writeDouble(data.sf);
        writer.writeDouble(data.ref);
        result.push_back(writer.getLine());
    }

    return result;
}

void BoundaryPwp::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(BoundaryPwp, "*BOUNDARY_PWP")

// ============================================================================
// BoundaryPwpNode
// ============================================================================

bool BoundaryPwpNode::parse(const std::vector<std::string>& lines,
                              util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    conditions_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        Data data;
        size_t pos = 0;

        data.nid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.sf = parser.getDoubleAt(pos, realW).value_or(1.0);

        if (data.nid != 0) {
            conditions_.push_back(data);
        }
    }

    return true;
}

std::vector<std::string> BoundaryPwpNode::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& data : conditions_) {
        writer.clear();
        writer.writeInt(data.nid);
        writer.writeInt(data.lcid);
        writer.writeDouble(data.sf);
        result.push_back(writer.getLine());
    }

    return result;
}

void BoundaryPwpNode::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(BoundaryPwpNode, "*BOUNDARY_PWP_NODE")

// ============================================================================
// BoundaryPzepot
// ============================================================================

bool BoundaryPzepot::parse(const std::vector<std::string>& lines,
                             util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    conditions_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        Data data;
        size_t pos = 0;

        data.nsid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.sf = parser.getDoubleAt(pos, realW).value_or(1.0);

        if (data.nsid != 0) {
            conditions_.push_back(data);
        }
    }

    return true;
}

std::vector<std::string> BoundaryPzepot::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& data : conditions_) {
        writer.clear();
        writer.writeInt(data.nsid);
        writer.writeInt(data.lcid);
        writer.writeDouble(data.sf);
        result.push_back(writer.getLine());
    }

    return result;
}

void BoundaryPzepot::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(BoundaryPzepot, "*BOUNDARY_PZEPOT")

// ============================================================================
// BoundaryRadiationSegment
// ============================================================================

bool BoundaryRadiationSegment::parse(const std::vector<std::string>& lines,
                                       util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    conditions_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        Data data;
        size_t pos = 0;

        data.n1 = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data.n2 = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data.n3 = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data.n4 = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data.type = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.elcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.tlcid = static_cast<int>(parser.getInt64At(pos).value_or(0));

        if (data.n1 != 0) {
            conditions_.push_back(data);
        }
    }

    return true;
}

std::vector<std::string> BoundaryRadiationSegment::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& data : conditions_) {
        writer.clear();
        writer.writeInt(data.n1);
        writer.writeInt(data.n2);
        writer.writeInt(data.n3);
        writer.writeInt(data.n4);
        writer.writeInt(data.type);
        writer.writeInt(data.elcid);
        writer.writeInt(data.tlcid);
        result.push_back(writer.getLine());
    }

    return result;
}

void BoundaryRadiationSegment::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(BoundaryRadiationSegment, "*BOUNDARY_RADIATION_SEGMENT")

// ============================================================================
// BoundarySaleMeshFace
// ============================================================================

bool BoundarySaleMeshFace::parse(const std::vector<std::string>& lines,
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
        data_.opt = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.sf = parser.getDoubleAt(pos, realW).value_or(1.0);
        break;
    }

    return true;
}

std::vector<std::string> BoundarySaleMeshFace::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.ssid);
    writer.writeInt(data_.opt);
    writer.writeInt(data_.lcid);
    writer.writeDouble(data_.sf);

    result.push_back(writer.getLine());
    return result;
}

void BoundarySaleMeshFace::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(BoundarySaleMeshFace, "*BOUNDARY_SALE_MESH_FACE")

// ============================================================================
// BoundarySpc
// ============================================================================

bool BoundarySpc::parse(const std::vector<std::string>& lines,
                          util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    constraints_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        Data data;
        size_t pos = 0;

        data.nid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data.cid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.dofx = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.dofy = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.dofz = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.dofrx = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.dofry = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.dofrz = static_cast<int>(parser.getInt64At(pos).value_or(0));

        if (data.nid != 0) {
            constraints_.push_back(data);
        }
    }

    return true;
}

std::vector<std::string> BoundarySpc::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& data : constraints_) {
        writer.clear();
        writer.writeInt(data.nid);
        writer.writeInt(data.cid);
        writer.writeInt(data.dofx);
        writer.writeInt(data.dofy);
        writer.writeInt(data.dofz);
        writer.writeInt(data.dofrx);
        writer.writeInt(data.dofry);
        writer.writeInt(data.dofrz);
        result.push_back(writer.getLine());
    }

    return result;
}

void BoundarySpc::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(BoundarySpc, "*BOUNDARY_SPC")

// ============================================================================
// BoundarySphNoslip
// ============================================================================

bool BoundarySphNoslip::parse(const std::vector<std::string>& lines,
                                util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    conditions_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        Data data;
        size_t pos = 0;

        data.ssid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.type = static_cast<int>(parser.getInt64At(pos).value_or(0));

        if (data.ssid != 0) {
            conditions_.push_back(data);
        }
    }

    return true;
}

std::vector<std::string> BoundarySphNoslip::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& data : conditions_) {
        writer.clear();
        writer.writeInt(data.ssid);
        writer.writeInt(data.type);
        result.push_back(writer.getLine());
    }

    return result;
}

void BoundarySphNoslip::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(BoundarySphNoslip, "*BOUNDARY_SPH_NOSLIP")

// ============================================================================
// BoundarySphPeriodic
// ============================================================================

bool BoundarySphPeriodic::parse(const std::vector<std::string>& lines,
                                  util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    conditions_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        Data data;
        size_t pos = 0;

        data.ssid1 = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.ssid2 = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.dir = static_cast<int>(parser.getInt64At(pos).value_or(0));

        if (data.ssid1 != 0) {
            conditions_.push_back(data);
        }
    }

    return true;
}

std::vector<std::string> BoundarySphPeriodic::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& data : conditions_) {
        writer.clear();
        writer.writeInt(data.ssid1);
        writer.writeInt(data.ssid2);
        writer.writeInt(data.dir);
        result.push_back(writer.getLine());
    }

    return result;
}

void BoundarySphPeriodic::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(BoundarySphPeriodic, "*BOUNDARY_SPH_PERIODIC")

// ============================================================================
// BoundarySymmetryFailure
// ============================================================================

bool BoundarySymmetryFailure::parse(const std::vector<std::string>& lines,
                                      util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    conditions_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        Data data;
        size_t pos = 0;

        data.nsid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.plane = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.fail = parser.getDoubleAt(pos, realW).value_or(0.0);

        if (data.nsid != 0) {
            conditions_.push_back(data);
        }
    }

    return true;
}

std::vector<std::string> BoundarySymmetryFailure::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& data : conditions_) {
        writer.clear();
        writer.writeInt(data.nsid);
        writer.writeInt(data.plane);
        writer.writeDouble(data.fail);
        result.push_back(writer.getLine());
    }

    return result;
}

void BoundarySymmetryFailure::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(BoundarySymmetryFailure, "*BOUNDARY_SYMMETRY_FAILURE")

// ============================================================================
// BoundaryTemperatureRsw
// ============================================================================

bool BoundaryTemperatureRsw::parse(const std::vector<std::string>& lines,
                                     util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    conditions_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        Data data;
        size_t pos = 0;

        data.nsid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.sf = parser.getDoubleAt(pos, realW).value_or(1.0);
        pos += realW;
        data.opt = static_cast<int>(parser.getInt64At(pos).value_or(0));

        if (data.nsid != 0) {
            conditions_.push_back(data);
        }
    }

    return true;
}

std::vector<std::string> BoundaryTemperatureRsw::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& data : conditions_) {
        writer.clear();
        writer.writeInt(data.nsid);
        writer.writeInt(data.lcid);
        writer.writeDouble(data.sf);
        writer.writeInt(data.opt);
        result.push_back(writer.getLine());
    }

    return result;
}

void BoundaryTemperatureRsw::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(BoundaryTemperatureRsw, "*BOUNDARY_TEMPERATURE_RSW")

// ============================================================================
// BoundaryTemperatureTrajectory
// ============================================================================

bool BoundaryTemperatureTrajectory::parse(const std::vector<std::string>& lines,
                                            util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    conditions_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        Data data;
        size_t pos = 0;

        data.pid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.nsid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.sf = parser.getDoubleAt(pos, realW).value_or(1.0);

        if (data.pid != 0 || data.nsid != 0) {
            conditions_.push_back(data);
        }
    }

    return true;
}

std::vector<std::string> BoundaryTemperatureTrajectory::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& data : conditions_) {
        writer.clear();
        writer.writeInt(data.pid);
        writer.writeInt(data.nsid);
        writer.writeInt(data.lcid);
        writer.writeDouble(data.sf);
        result.push_back(writer.getLine());
    }

    return result;
}

void BoundaryTemperatureTrajectory::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(BoundaryTemperatureTrajectory, "*BOUNDARY_TEMPERATURE_TRAJECTORY")

// ============================================================================
// BoundaryThermalBulknode
// ============================================================================

bool BoundaryThermalBulknode::parse(const std::vector<std::string>& lines,
                                      util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    conditions_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        Data data;
        size_t pos = 0;

        data.nid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.sf = parser.getDoubleAt(pos, realW).value_or(1.0);
        pos += realW;
        data.hlc = parser.getDoubleAt(pos, realW).value_or(0.0);

        if (data.nid != 0) {
            conditions_.push_back(data);
        }
    }

    return true;
}

std::vector<std::string> BoundaryThermalBulknode::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& data : conditions_) {
        writer.clear();
        writer.writeInt(data.nid);
        writer.writeInt(data.lcid);
        writer.writeDouble(data.sf);
        writer.writeDouble(data.hlc);
        result.push_back(writer.getLine());
    }

    return result;
}

void BoundaryThermalBulknode::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(BoundaryThermalBulknode, "*BOUNDARY_THERMAL_BULKNODE")

// ============================================================================
// BoundaryThermalWeld
// ============================================================================

bool BoundaryThermalWeld::parse(const std::vector<std::string>& lines,
                                  util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    conditions_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        Data data;
        size_t pos = 0;

        data.nsid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.sf = parser.getDoubleAt(pos, realW).value_or(1.0);
        pos += realW;
        data.type = static_cast<int>(parser.getInt64At(pos).value_or(0));

        if (data.nsid != 0) {
            conditions_.push_back(data);
        }
    }

    return true;
}

std::vector<std::string> BoundaryThermalWeld::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& data : conditions_) {
        writer.clear();
        writer.writeInt(data.nsid);
        writer.writeInt(data.lcid);
        writer.writeDouble(data.sf);
        writer.writeInt(data.type);
        result.push_back(writer.getLine());
    }

    return result;
}

void BoundaryThermalWeld::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(BoundaryThermalWeld, "*BOUNDARY_THERMAL_WELD")

// ============================================================================
// BoundaryThermalWeldTrajectory
// ============================================================================

bool BoundaryThermalWeldTrajectory::parse(const std::vector<std::string>& lines,
                                            util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    conditions_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        Data data;
        size_t pos = 0;

        data.pid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.nsid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.sf = parser.getDoubleAt(pos, realW).value_or(1.0);
        pos += realW;
        data.vel = parser.getDoubleAt(pos, realW).value_or(0.0);

        if (data.pid != 0 || data.nsid != 0) {
            conditions_.push_back(data);
        }
    }

    return true;
}

std::vector<std::string> BoundaryThermalWeldTrajectory::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& data : conditions_) {
        writer.clear();
        writer.writeInt(data.pid);
        writer.writeInt(data.nsid);
        writer.writeInt(data.lcid);
        writer.writeDouble(data.sf);
        writer.writeDouble(data.vel);
        result.push_back(writer.getLine());
    }

    return result;
}

void BoundaryThermalWeldTrajectory::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(BoundaryThermalWeldTrajectory, "*BOUNDARY_THERMAL_WELD_TRAJECTORY")

// ============================================================================
// BoundaryUsaSurface
// ============================================================================

bool BoundaryUsaSurface::parse(const std::vector<std::string>& lines,
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
        data_.type = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.opt = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> BoundaryUsaSurface::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.ssid);
    writer.writeInt(data_.type);
    writer.writeInt(data_.opt);

    result.push_back(writer.getLine());
    return result;
}

void BoundaryUsaSurface::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(BoundaryUsaSurface, "*BOUNDARY_USA_SURFACE")

// ============================================================================
// BoundaryPrescribedAccelerometerRigid
// ============================================================================

bool BoundaryPrescribedAccelerometerRigid::parse(const std::vector<std::string>& lines,
                                                   util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    conditions_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        Data data;
        size_t pos = 0;

        data.pid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.dof = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.accid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.sf = parser.getDoubleAt(pos, realW).value_or(1.0);
        pos += realW;
        data.vid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.death = parser.getDoubleAt(pos, realW).value_or(1e28);
        pos += realW;
        data.birth = parser.getDoubleAt(pos, realW).value_or(0.0);

        if (data.pid != 0) {
            conditions_.push_back(data);
        }
    }

    return true;
}

std::vector<std::string> BoundaryPrescribedAccelerometerRigid::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& data : conditions_) {
        writer.clear();
        writer.writeInt(data.pid);
        writer.writeInt(data.dof);
        writer.writeInt(data.accid);
        writer.writeDouble(data.sf);
        writer.writeInt(data.vid);
        writer.writeDouble(data.death);
        writer.writeDouble(data.birth);
        result.push_back(writer.getLine());
    }

    return result;
}

void BoundaryPrescribedAccelerometerRigid::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(BoundaryPrescribedAccelerometerRigid, "*BOUNDARY_PRESCRIBED_ACCELEROMETER_RIGID")

// ============================================================================
// BoundaryFluidmFreeSurface
// ============================================================================

bool BoundaryFluidmFreeSurface::parse(const std::vector<std::string>& lines,
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
        data_.pref = parser.getDoubleAt(pos, realW).value_or(0.0);
        break;
    }

    return true;
}

std::vector<std::string> BoundaryFluidmFreeSurface::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.ssid);
    writer.writeDouble(data_.pref);

    result.push_back(writer.getLine());
    return result;
}

void BoundaryFluidmFreeSurface::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(BoundaryFluidmFreeSurface, "*BOUNDARY_FLUIDM_FREE_SURFACE")

// ============================================================================
// BoundaryCoupled
// ============================================================================

bool BoundaryCoupled::parse(const std::vector<std::string>& lines,
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
        data_.psid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.opt = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.sf = parser.getDoubleAt(pos, realW).value_or(1.0);
        break;
    }

    return true;
}

std::vector<std::string> BoundaryCoupled::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.ssid);
    writer.writeInt(data_.psid);
    writer.writeInt(data_.opt);
    writer.writeDouble(data_.sf);

    result.push_back(writer.getLine());
    return result;
}

void BoundaryCoupled::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(BoundaryCoupled, "*BOUNDARY_COUPLED")

// ============================================================================
// BoundaryPrescribedMotionNodeId
// ============================================================================

bool BoundaryPrescribedMotionNodeId::parse(const std::vector<std::string>& lines,
                                             util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    motions_.clear();

    size_t lineIdx = 0;
    while (lineIdx < lines.size()) {
        if (util::CardParser::isCommentLine(lines[lineIdx])) {
            ++lineIdx;
            continue;
        }

        Data data;

        // First line: ID and heading
        parser.setLine(lines[lineIdx]);
        data.id = static_cast<int>(parser.getInt64At(0).value_or(0));
        if (lines[lineIdx].size() > intW) {
            data.heading = util::StringUtils::trim(lines[lineIdx].substr(intW));
        }
        ++lineIdx;

        // Second line: node motion data
        if (lineIdx < lines.size() && !util::CardParser::isCommentLine(lines[lineIdx])) {
            parser.setLine(lines[lineIdx]);
            size_t pos = 0;

            data.nid = parser.getInt64At(pos).value_or(0);
            pos += intW;
            data.dof = static_cast<int>(parser.getInt64At(pos).value_or(0));
            pos += intW;
            data.vad = static_cast<int>(parser.getInt64At(pos).value_or(0));
            pos += intW;
            data.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
            pos += intW;
            data.sf = parser.getDoubleAt(pos, realW).value_or(1.0);
            pos += realW;
            data.vid = static_cast<int>(parser.getInt64At(pos).value_or(0));
            pos += intW;
            data.death = parser.getDoubleAt(pos, realW).value_or(1e28);
            pos += realW;
            data.birth = parser.getDoubleAt(pos, realW).value_or(0.0);
            ++lineIdx;
        }

        if (data.id != 0) {
            motions_.push_back(data);
        }
    }

    return true;
}

std::vector<std::string> BoundaryPrescribedMotionNodeId::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& data : motions_) {
        writer.clear();
        writer.writeInt(data.id);
        std::string line1 = writer.getLine();
        if (!data.heading.empty()) {
            line1 += data.heading;
        }
        result.push_back(line1);

        writer.clear();
        writer.writeInt(data.nid);
        writer.writeInt(data.dof);
        writer.writeInt(data.vad);
        writer.writeInt(data.lcid);
        writer.writeDouble(data.sf);
        writer.writeInt(data.vid);
        writer.writeDouble(data.death);
        writer.writeDouble(data.birth);
        result.push_back(writer.getLine());
    }

    return result;
}

void BoundaryPrescribedMotionNodeId::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(BoundaryPrescribedMotionNodeId, "*BOUNDARY_PRESCRIBED_MOTION_NODE_ID")

// ============================================================================
// BoundaryPrescribedMotionSetLine
// ============================================================================

bool BoundaryPrescribedMotionSetLine::parse(const std::vector<std::string>& lines,
                                              util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    motions_.clear();

    size_t lineIdx = 0;
    while (lineIdx < lines.size()) {
        if (util::CardParser::isCommentLine(lines[lineIdx])) {
            ++lineIdx;
            continue;
        }

        Data data;

        // First card
        parser.setLine(lines[lineIdx]);
        size_t pos = 0;

        data.nsid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.dof = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.vad = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.sf = parser.getDoubleAt(pos, realW).value_or(1.0);
        pos += realW;
        data.vid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data.death = parser.getDoubleAt(pos, realW).value_or(1e28);
        pos += realW;
        data.birth = parser.getDoubleAt(pos, realW).value_or(0.0);
        ++lineIdx;

        // Second card: line coordinates
        if (lineIdx < lines.size() && !util::CardParser::isCommentLine(lines[lineIdx])) {
            parser.setLine(lines[lineIdx]);
            pos = 0;

            data.x1 = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data.y1 = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data.z1 = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data.x2 = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data.y2 = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            data.z2 = parser.getDoubleAt(pos, realW).value_or(0.0);
            ++lineIdx;
        }

        if (data.nsid != 0) {
            motions_.push_back(data);
        }
    }

    return true;
}

std::vector<std::string> BoundaryPrescribedMotionSetLine::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& data : motions_) {
        writer.clear();
        writer.writeInt(data.nsid);
        writer.writeInt(data.dof);
        writer.writeInt(data.vad);
        writer.writeInt(data.lcid);
        writer.writeDouble(data.sf);
        writer.writeInt(data.vid);
        writer.writeDouble(data.death);
        writer.writeDouble(data.birth);
        result.push_back(writer.getLine());

        writer.clear();
        writer.writeDouble(data.x1);
        writer.writeDouble(data.y1);
        writer.writeDouble(data.z1);
        writer.writeDouble(data.x2);
        writer.writeDouble(data.y2);
        writer.writeDouble(data.z2);
        result.push_back(writer.getLine());
    }

    return result;
}

void BoundaryPrescribedMotionSetLine::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(BoundaryPrescribedMotionSetLine, "*BOUNDARY_PRESCRIBED_MOTION_SET_LINE")

} // namespace koo::dyna
