#include <koo/dyna/Initial.hpp>
#include <koo/dyna/ModelVisitor.hpp>
#include <koo/dyna/KeywordFactory.hpp>
#include <koo/util/StringUtils.hpp>

namespace koo::dyna {

// ============================================================================
// InitialVelocity
// ============================================================================

bool InitialVelocity::parse(const std::vector<std::string>& lines,
                             util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    velocities_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        NodeVelocity vel;
        size_t pos = 0;

        vel.nid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        vel.vx = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        vel.vy = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        vel.vz = parser.getDoubleAt(pos, realW).value_or(0.0);

        if (vel.nid != 0) {
            velocities_.push_back(vel);
        }
    }

    return true;
}

std::vector<std::string> InitialVelocity::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& vel : velocities_) {
        writer.clear();
        writer.writeInt(vel.nid);
        writer.writeDouble(vel.vx);
        writer.writeDouble(vel.vy);
        writer.writeDouble(vel.vz);
        result.push_back(writer.getLine());
    }

    return result;
}

void InitialVelocity::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(InitialVelocity, "*INITIAL_VELOCITY")

// ============================================================================
// InitialVelocityNode
// ============================================================================

bool InitialVelocityNode::parse(const std::vector<std::string>& lines,
                                 util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    velocities_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        NodeVelocity vel;
        size_t pos = 0;

        vel.nid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        vel.vx = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        vel.vy = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        vel.vz = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        vel.vrx = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        vel.vry = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        vel.vrz = parser.getDoubleAt(pos, realW).value_or(0.0);

        if (vel.nid != 0) {
            velocities_.push_back(vel);
        }
    }

    return true;
}

std::vector<std::string> InitialVelocityNode::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& vel : velocities_) {
        writer.clear();
        writer.writeInt(vel.nid);
        writer.writeDouble(vel.vx);
        writer.writeDouble(vel.vy);
        writer.writeDouble(vel.vz);
        writer.writeDouble(vel.vrx);
        writer.writeDouble(vel.vry);
        writer.writeDouble(vel.vrz);
        result.push_back(writer.getLine());
    }

    return result;
}

void InitialVelocityNode::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(InitialVelocityNode, "*INITIAL_VELOCITY_NODE")

// ============================================================================
// InitialVelocityGeneration
// ============================================================================

bool InitialVelocityGeneration::parse(const std::vector<std::string>& lines,
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

    // Card 1: NSID, NSIDEX, BOXID, IRIGID
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.nsid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.nsidex = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.boxid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.irigid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    ++lineIdx;

    // Skip comments
    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 2: VX, VY, VZ, VRX, VRY, VRZ, ICID
    parser.setLine(lines[lineIdx]);
    pos = 0;

    data_.vx = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.vy = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.vz = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.vrx = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.vry = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.vrz = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.icid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    ++lineIdx;

    // Card 3 (optional): OMEGA, XC, YC, ZC, AX, AY, AZ, PHASE
    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx < lines.size()) {
        parser.setLine(lines[lineIdx]);
        pos = 0;

        data_.omega = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.xc = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.yc = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.zc = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.ax = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.ay = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.az = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.phase = static_cast<int>(parser.getInt64At(pos).value_or(0));
    }

    return true;
}

std::vector<std::string> InitialVelocityGeneration::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    // Card 1
    writer.writeInt(data_.nsid);
    writer.writeInt(data_.nsidex);
    writer.writeInt(data_.boxid);
    writer.writeInt(data_.irigid);
    result.push_back(writer.getLine());

    // Card 2
    writer.clear();
    writer.writeDouble(data_.vx);
    writer.writeDouble(data_.vy);
    writer.writeDouble(data_.vz);
    writer.writeDouble(data_.vrx);
    writer.writeDouble(data_.vry);
    writer.writeDouble(data_.vrz);
    writer.writeInt(data_.icid);
    result.push_back(writer.getLine());

    // Card 3 (if angular velocity is set)
    if (data_.omega != 0.0) {
        writer.clear();
        writer.writeDouble(data_.omega);
        writer.writeDouble(data_.xc);
        writer.writeDouble(data_.yc);
        writer.writeDouble(data_.zc);
        writer.writeDouble(data_.ax);
        writer.writeDouble(data_.ay);
        writer.writeDouble(data_.az);
        writer.writeInt(data_.phase);
        result.push_back(writer.getLine());
    }

    return result;
}

void InitialVelocityGeneration::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(InitialVelocityGeneration, "*INITIAL_VELOCITY_GENERATION")

// ============================================================================
// InitialStressShell
// ============================================================================

bool InitialStressShell::parse(const std::vector<std::string>& lines,
                                util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    stresses_.clear();
    size_t i = 0;

    while (i < lines.size()) {
        // Skip comments
        while (i < lines.size() && util::CardParser::isCommentLine(lines[i])) {
            ++i;
        }
        if (i >= lines.size()) break;

        // Card 1: EID, NPLANE, NTHICK, NHISV, LARGE
        parser.setLine(lines[i]);
        StressData stress;
        size_t pos = 0;

        stress.eid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        stress.nplane = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        stress.nthick = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        stress.nhisv = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        stress.large = static_cast<int>(parser.getInt64At(pos).value_or(0));
        ++i;

        if (stress.eid != 0) {
            stresses_.push_back(stress);
        }
        // Note: Actual stress values would follow in subsequent cards
        // This is a simplified implementation
    }

    return true;
}

std::vector<std::string> InitialStressShell::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& stress : stresses_) {
        writer.clear();
        writer.writeInt(stress.eid);
        writer.writeInt(stress.nplane);
        writer.writeInt(stress.nthick);
        writer.writeInt(stress.nhisv);
        writer.writeInt(stress.large);
        result.push_back(writer.getLine());
    }

    return result;
}

void InitialStressShell::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(InitialStressShell, "*INITIAL_STRESS_SHELL")

// ============================================================================
// InitialStressSolid
// ============================================================================

bool InitialStressSolid::parse(const std::vector<std::string>& lines,
                                util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    stresses_.clear();
    size_t i = 0;

    while (i < lines.size()) {
        // Skip comments
        while (i < lines.size() && util::CardParser::isCommentLine(lines[i])) {
            ++i;
        }
        if (i >= lines.size()) break;

        // Card 1: EID, NINT, NHISV, LARGE
        parser.setLine(lines[i]);
        StressData stress;
        size_t pos = 0;

        stress.eid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        stress.nint = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        stress.nhisv = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        stress.large = static_cast<int>(parser.getInt64At(pos).value_or(0));
        ++i;

        if (stress.eid != 0) {
            stresses_.push_back(stress);
        }
    }

    return true;
}

std::vector<std::string> InitialStressSolid::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& stress : stresses_) {
        writer.clear();
        writer.writeInt(stress.eid);
        writer.writeInt(stress.nint);
        writer.writeInt(stress.nhisv);
        writer.writeInt(stress.large);
        result.push_back(writer.getLine());
    }

    return result;
}

void InitialStressSolid::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(InitialStressSolid, "*INITIAL_STRESS_SOLID")

// ============================================================================
// InitialStrainShell
// ============================================================================

bool InitialStrainShell::parse(const std::vector<std::string>& lines,
                                util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    strains_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        StrainData strain;
        size_t pos = 0;

        strain.eid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        strain.nplane = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        strain.nthick = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        strain.eps_xx = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        strain.eps_yy = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        strain.eps_zz = parser.getDoubleAt(pos, realW).value_or(0.0);

        if (strain.eid != 0) {
            strains_.push_back(strain);
        }
    }

    return true;
}

std::vector<std::string> InitialStrainShell::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& strain : strains_) {
        writer.clear();
        writer.writeInt(strain.eid);
        writer.writeInt(strain.nplane);
        writer.writeInt(strain.nthick);
        writer.writeDouble(strain.eps_xx);
        writer.writeDouble(strain.eps_yy);
        writer.writeDouble(strain.eps_zz);
        result.push_back(writer.getLine());
    }

    return result;
}

void InitialStrainShell::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(InitialStrainShell, "*INITIAL_STRAIN_SHELL")

// ============================================================================
// InitialStrainSolid
// ============================================================================

bool InitialStrainSolid::parse(const std::vector<std::string>& lines,
                                util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    strains_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        StrainData strain;
        size_t pos = 0;

        strain.eid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        strain.nint = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        strain.eps_xx = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        strain.eps_yy = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        strain.eps_zz = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        strain.eps_xy = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        strain.eps_yz = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        strain.eps_zx = parser.getDoubleAt(pos, realW).value_or(0.0);

        if (strain.eid != 0) {
            strains_.push_back(strain);
        }
    }

    return true;
}

std::vector<std::string> InitialStrainSolid::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& strain : strains_) {
        writer.clear();
        writer.writeInt(strain.eid);
        writer.writeInt(strain.nint);
        writer.writeDouble(strain.eps_xx);
        writer.writeDouble(strain.eps_yy);
        writer.writeDouble(strain.eps_zz);
        writer.writeDouble(strain.eps_xy);
        writer.writeDouble(strain.eps_yz);
        writer.writeDouble(strain.eps_zx);
        result.push_back(writer.getLine());
    }

    return result;
}

void InitialStrainSolid::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(InitialStrainSolid, "*INITIAL_STRAIN_SOLID")

// ============================================================================
// InitialFoamReferenceGeometry
// ============================================================================

bool InitialFoamReferenceGeometry::parse(const std::vector<std::string>& lines,
                                          util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    data_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        Data d;
        size_t pos = 0;

        d.pid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        d.nid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        d.x = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        d.y = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        d.z = parser.getDoubleAt(pos, realW).value_or(0.0);

        if (d.pid != 0 || d.nid != 0) {
            data_.push_back(d);
        }
    }

    return true;
}

std::vector<std::string> InitialFoamReferenceGeometry::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& d : data_) {
        writer.clear();
        writer.writeInt(d.pid);
        writer.writeInt(d.nid);
        writer.writeDouble(d.x);
        writer.writeDouble(d.y);
        writer.writeDouble(d.z);
        result.push_back(writer.getLine());
    }

    return result;
}

void InitialFoamReferenceGeometry::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(InitialFoamReferenceGeometry, "*INITIAL_FOAM_REFERENCE_GEOMETRY")

// ============================================================================
// InitialDetonation
// ============================================================================

bool InitialDetonation::parse(const std::vector<std::string>& lines,
                               util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    data_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        Data d;
        size_t pos = 0;

        d.pid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        d.x = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        d.y = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        d.z = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        d.lt = parser.getDoubleAt(pos, realW).value_or(0.0);

        if (d.pid != 0) {
            data_.push_back(d);
        }
    }

    return true;
}

std::vector<std::string> InitialDetonation::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& d : data_) {
        writer.clear();
        writer.writeInt(d.pid);
        writer.writeDouble(d.x);
        writer.writeDouble(d.y);
        writer.writeDouble(d.z);
        writer.writeDouble(d.lt);
        result.push_back(writer.getLine());
    }

    return result;
}

void InitialDetonation::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(InitialDetonation, "*INITIAL_DETONATION")

// ============================================================================
// InitialAirbagParticlePosition
// ============================================================================

bool InitialAirbagParticlePosition::parse(const std::vector<std::string>& lines,
                                           util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    data_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        Data d;
        size_t pos = 0;

        d.id = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        d.x = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        d.y = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        d.z = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        d.u = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        d.v = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        d.w = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        d.mass = parser.getDoubleAt(pos, realW).value_or(0.0);

        if (d.id != 0) {
            data_.push_back(d);
        }
    }

    return true;
}

std::vector<std::string> InitialAirbagParticlePosition::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& d : data_) {
        writer.clear();
        writer.writeInt(d.id);
        writer.writeDouble(d.x);
        writer.writeDouble(d.y);
        writer.writeDouble(d.z);
        writer.writeDouble(d.u);
        writer.writeDouble(d.v);
        writer.writeDouble(d.w);
        writer.writeDouble(d.mass);
        result.push_back(writer.getLine());
    }

    return result;
}

void InitialAirbagParticlePosition::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(InitialAirbagParticlePosition, "*INITIAL_AIRBAG_PARTICLE_POSITION")

// ============================================================================
// InitialAxialForceBeam
// ============================================================================

bool InitialAxialForceBeam::parse(const std::vector<std::string>& lines,
                                   util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    entries_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        Entry e;
        size_t pos = 0;

        e.eid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        e.axial = parser.getDoubleAt(pos, realW).value_or(0.0);

        if (e.eid != 0) {
            entries_.push_back(e);
        }
    }

    return true;
}

std::vector<std::string> InitialAxialForceBeam::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& e : entries_) {
        writer.clear();
        writer.writeInt(e.eid);
        writer.writeDouble(e.axial);
        result.push_back(writer.getLine());
    }

    return result;
}

void InitialAxialForceBeam::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(InitialAxialForceBeam, "*INITIAL_AXIAL_FORCE_BEAM")

// Note: InitialTemperature is defined in Thermal.cpp

// ============================================================================
// InitialAlePressure
// ============================================================================

bool InitialAlePressure::parse(const std::vector<std::string>& lines,
                                util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    entries_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        Entry e;
        size_t pos = 0;

        e.pid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        e.pressure = parser.getDoubleAt(pos, realW).value_or(0.0);

        if (e.pid != 0) {
            entries_.push_back(e);
        }
    }

    return true;
}

std::vector<std::string> InitialAlePressure::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& e : entries_) {
        writer.clear();
        writer.writeInt(e.pid);
        writer.writeDouble(e.pressure);
        result.push_back(writer.getLine());
    }

    return result;
}

void InitialAlePressure::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(InitialAlePressure, "*INITIAL_ALE_PRESSURE")

// ============================================================================
// InitialGasMixture
// ============================================================================

bool InitialGasMixture::parse(const std::vector<std::string>& lines,
                               util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    data_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        Data d;
        size_t pos = 0;

        d.bagid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        d.gasid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        d.mf = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        d.temp = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        d.p = parser.getDoubleAt(pos, realW).value_or(0.0);

        if (d.bagid != 0) {
            data_.push_back(d);
        }
    }

    return true;
}

std::vector<std::string> InitialGasMixture::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& d : data_) {
        writer.clear();
        writer.writeInt(d.bagid);
        writer.writeInt(d.gasid);
        writer.writeDouble(d.mf);
        writer.writeDouble(d.temp);
        writer.writeDouble(d.p);
        result.push_back(writer.getLine());
    }

    return result;
}

void InitialGasMixture::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(InitialGasMixture, "*INITIAL_GAS_MIXTURE")

// ============================================================================
// InitialStressBeam
// ============================================================================

bool InitialStressBeam::parse(const std::vector<std::string>& lines,
                               util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    stresses_.clear();
    size_t i = 0;

    while (i < lines.size()) {
        while (i < lines.size() && util::CardParser::isCommentLine(lines[i])) {
            ++i;
        }
        if (i >= lines.size()) break;

        // Card 1: EID, NINT, NHISV
        parser.setLine(lines[i]);
        StressData stress;
        size_t pos = 0;

        stress.eid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        stress.nint = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        stress.nhisv = static_cast<int>(parser.getInt64At(pos).value_or(0));
        ++i;

        // Card 2: AXIAL, SHEAR_S, SHEAR_T, MOMENT_S, MOMENT_T, TORSION
        while (i < lines.size() && util::CardParser::isCommentLine(lines[i])) {
            ++i;
        }
        if (i < lines.size()) {
            parser.setLine(lines[i]);
            pos = 0;
            stress.axial = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            stress.shear_s = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            stress.shear_t = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            stress.moment_s = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            stress.moment_t = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            stress.torsion = parser.getDoubleAt(pos, realW).value_or(0.0);
            ++i;
        }

        if (stress.eid != 0) {
            stresses_.push_back(stress);
        }
    }

    return true;
}

std::vector<std::string> InitialStressBeam::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& stress : stresses_) {
        writer.clear();
        writer.writeInt(stress.eid);
        writer.writeInt(stress.nint);
        writer.writeInt(stress.nhisv);
        result.push_back(writer.getLine());

        writer.clear();
        writer.writeDouble(stress.axial);
        writer.writeDouble(stress.shear_s);
        writer.writeDouble(stress.shear_t);
        writer.writeDouble(stress.moment_s);
        writer.writeDouble(stress.moment_t);
        writer.writeDouble(stress.torsion);
        result.push_back(writer.getLine());
    }

    return result;
}

void InitialStressBeam::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(InitialStressBeam, "*INITIAL_STRESS_BEAM")

// ============================================================================
// InitialVelocitySet
// ============================================================================

bool InitialVelocitySet::parse(const std::vector<std::string>& lines,
                                util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    data_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        Data d;
        size_t pos = 0;

        d.nsid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        d.vx = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        d.vy = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        d.vz = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        d.vrx = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        d.vry = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        d.vrz = parser.getDoubleAt(pos, realW).value_or(0.0);

        if (d.nsid != 0) {
            data_.push_back(d);
        }
    }

    return true;
}

std::vector<std::string> InitialVelocitySet::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& d : data_) {
        writer.clear();
        writer.writeInt(d.nsid);
        writer.writeDouble(d.vx);
        writer.writeDouble(d.vy);
        writer.writeDouble(d.vz);
        writer.writeDouble(d.vrx);
        writer.writeDouble(d.vry);
        writer.writeDouble(d.vrz);
        result.push_back(writer.getLine());
    }

    return result;
}

void InitialVelocitySet::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(InitialVelocitySet, "*INITIAL_VELOCITY_SET")

// ============================================================================
// InitialStressSection
// ============================================================================

bool InitialStressSection::parse(const std::vector<std::string>& lines,
                                  util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    data_.clear();
    size_t i = 0;

    while (i < lines.size()) {
        while (i < lines.size() && util::CardParser::isCommentLine(lines[i])) {
            ++i;
        }
        if (i >= lines.size()) break;

        // Card 1: PSID, SECID
        parser.setLine(lines[i]);
        Data d;
        size_t pos = 0;

        d.psid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        d.secid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        ++i;

        // Card 2: SIGXX, SIGYY, SIGZZ, SIGXY, SIGYZ, SIGZX
        while (i < lines.size() && util::CardParser::isCommentLine(lines[i])) {
            ++i;
        }
        if (i < lines.size()) {
            parser.setLine(lines[i]);
            pos = 0;
            d.sigxx = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            d.sigyy = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            d.sigzz = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            d.sigxy = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            d.sigyz = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            d.sigzx = parser.getDoubleAt(pos, realW).value_or(0.0);
            ++i;
        }

        if (d.psid != 0 || d.secid != 0) {
            data_.push_back(d);
        }
    }

    return true;
}

std::vector<std::string> InitialStressSection::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& d : data_) {
        writer.clear();
        writer.writeInt(d.psid);
        writer.writeInt(d.secid);
        result.push_back(writer.getLine());

        writer.clear();
        writer.writeDouble(d.sigxx);
        writer.writeDouble(d.sigyy);
        writer.writeDouble(d.sigzz);
        writer.writeDouble(d.sigxy);
        writer.writeDouble(d.sigyz);
        writer.writeDouble(d.sigzx);
        result.push_back(writer.getLine());
    }

    return result;
}

void InitialStressSection::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(InitialStressSection, "*INITIAL_STRESS_SECTION")

// ============================================================================
// InitialStressTshell
// ============================================================================

bool InitialStressTshell::parse(const std::vector<std::string>& lines,
                                 util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    stresses_.clear();
    size_t i = 0;

    while (i < lines.size()) {
        while (i < lines.size() && util::CardParser::isCommentLine(lines[i])) {
            ++i;
        }
        if (i >= lines.size()) break;

        // Card 1: EID, NPLANE, NTHICK, NHISV, LARGE
        parser.setLine(lines[i]);
        StressData stress;
        size_t pos = 0;

        stress.eid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        stress.nplane = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        stress.nthick = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        stress.nhisv = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        stress.large = static_cast<int>(parser.getInt64At(pos).value_or(0));
        ++i;

        if (stress.eid != 0) {
            stresses_.push_back(stress);
        }
    }

    return true;
}

std::vector<std::string> InitialStressTshell::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& stress : stresses_) {
        writer.clear();
        writer.writeInt(stress.eid);
        writer.writeInt(stress.nplane);
        writer.writeInt(stress.nthick);
        writer.writeInt(stress.nhisv);
        writer.writeInt(stress.large);
        result.push_back(writer.getLine());
    }

    return result;
}

void InitialStressTshell::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(InitialStressTshell, "*INITIAL_STRESS_TSHELL")

// ============================================================================
// InitialHistoryNode
// ============================================================================

bool InitialHistoryNode::parse(const std::vector<std::string>& lines,
                                util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    entries_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        Entry e;
        size_t pos = 0;

        e.nid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        e.nhv = static_cast<int>(parser.getInt64At(pos).value_or(0));

        if (e.nid != 0) {
            entries_.push_back(e);
        }
    }

    return true;
}

std::vector<std::string> InitialHistoryNode::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& e : entries_) {
        writer.clear();
        writer.writeInt(e.nid);
        writer.writeInt(e.nhv);
        result.push_back(writer.getLine());
    }

    return result;
}

void InitialHistoryNode::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(InitialHistoryNode, "*INITIAL_HISTORY_NODE")

// ============================================================================
// InitialHistorySolid
// ============================================================================

bool InitialHistorySolid::parse(const std::vector<std::string>& lines,
                                 util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    entries_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        Entry e;
        size_t pos = 0;

        e.eid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        e.nint = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        e.nhv = static_cast<int>(parser.getInt64At(pos).value_or(0));

        if (e.eid != 0) {
            entries_.push_back(e);
        }
    }

    return true;
}

std::vector<std::string> InitialHistorySolid::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& e : entries_) {
        writer.clear();
        writer.writeInt(e.eid);
        writer.writeInt(e.nint);
        writer.writeInt(e.nhv);
        result.push_back(writer.getLine());
    }

    return result;
}

void InitialHistorySolid::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(InitialHistorySolid, "*INITIAL_HISTORY_SOLID")

// ============================================================================
// InitialHistoryShell
// ============================================================================

bool InitialHistoryShell::parse(const std::vector<std::string>& lines,
                                 util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    entries_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        Entry e;
        size_t pos = 0;

        e.eid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        e.nplane = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        e.nthick = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        e.nhv = static_cast<int>(parser.getInt64At(pos).value_or(0));

        if (e.eid != 0) {
            entries_.push_back(e);
        }
    }

    return true;
}

std::vector<std::string> InitialHistoryShell::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& e : entries_) {
        writer.clear();
        writer.writeInt(e.eid);
        writer.writeInt(e.nplane);
        writer.writeInt(e.nthick);
        writer.writeInt(e.nhv);
        result.push_back(writer.getLine());
    }

    return result;
}

void InitialHistoryShell::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(InitialHistoryShell, "*INITIAL_HISTORY_SHELL")

// ============================================================================
// InitialStressDepth
// ============================================================================

bool InitialStressDepth::parse(const std::vector<std::string>& lines,
                                util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: PSID
    parser.setLine(lines[lineIdx]);
    data_.psid = static_cast<int>(parser.getInt64At(0).value_or(0));
    ++lineIdx;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 2: SIGXX, SIGYY, SIGZZ, SIGXY, SIGYZ, SIGZX, Z0
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;
    data_.sigxx = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.sigyy = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.sigzz = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.sigxy = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.sigyz = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.sigzx = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.z0 = parser.getDoubleAt(pos, realW).value_or(0.0);

    return true;
}

std::vector<std::string> InitialStressDepth::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.psid);
    result.push_back(writer.getLine());

    writer.clear();
    writer.writeDouble(data_.sigxx);
    writer.writeDouble(data_.sigyy);
    writer.writeDouble(data_.sigzz);
    writer.writeDouble(data_.sigxy);
    writer.writeDouble(data_.sigyz);
    writer.writeDouble(data_.sigzx);
    writer.writeDouble(data_.z0);
    result.push_back(writer.getLine());

    return result;
}

void InitialStressDepth::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(InitialStressDepth, "*INITIAL_STRESS_DEPTH")

// ============================================================================
// InitialStressDiscrete
// ============================================================================

bool InitialStressDiscrete::parse(const std::vector<std::string>& lines,
                                   util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    entries_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        Entry e;
        size_t pos = 0;

        e.eid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        e.def = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        e.ddef = parser.getDoubleAt(pos, realW).value_or(0.0);

        if (e.eid != 0) {
            entries_.push_back(e);
        }
    }

    return true;
}

std::vector<std::string> InitialStressDiscrete::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& e : entries_) {
        writer.clear();
        writer.writeInt(e.eid);
        writer.writeDouble(e.def);
        writer.writeDouble(e.ddef);
        result.push_back(writer.getLine());
    }

    return result;
}

void InitialStressDiscrete::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(InitialStressDiscrete, "*INITIAL_STRESS_DISCRETE")

// ============================================================================
// InitialImpulseMine
// ============================================================================

bool InitialImpulseMine::parse(const std::vector<std::string>& lines,
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

    data_.nsid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.xc = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.yc = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.zc = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.impulse = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.lt = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.rt = parser.getDoubleAt(pos, realW).value_or(0.0);

    return true;
}

std::vector<std::string> InitialImpulseMine::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.nsid);
    writer.writeDouble(data_.xc);
    writer.writeDouble(data_.yc);
    writer.writeDouble(data_.zc);
    writer.writeDouble(data_.impulse);
    writer.writeDouble(data_.lt);
    writer.writeDouble(data_.rt);
    result.push_back(writer.getLine());

    return result;
}

void InitialImpulseMine::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(InitialImpulseMine, "*INITIAL_IMPULSE_MINE")

// ============================================================================
// InitialPwpDepth
// ============================================================================

bool InitialPwpDepth::parse(const std::vector<std::string>& lines,
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

    data_.psid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.pwp0 = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.dpwpdz = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.z0 = parser.getDoubleAt(pos, realW).value_or(0.0);

    return true;
}

std::vector<std::string> InitialPwpDepth::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.psid);
    writer.writeDouble(data_.pwp0);
    writer.writeDouble(data_.dpwpdz);
    writer.writeDouble(data_.z0);
    result.push_back(writer.getLine());

    return result;
}

void InitialPwpDepth::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(InitialPwpDepth, "*INITIAL_PWP_DEPTH")

// ============================================================================
// InitialContactWear
// ============================================================================

bool InitialContactWear::parse(const std::vector<std::string>& lines,
                                util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    entries_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        Entry e;
        size_t pos = 0;

        e.cid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        e.segid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        e.wear = parser.getDoubleAt(pos, realW).value_or(0.0);

        if (e.cid != 0) {
            entries_.push_back(e);
        }
    }

    return true;
}

std::vector<std::string> InitialContactWear::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& e : entries_) {
        writer.clear();
        writer.writeInt(e.cid);
        writer.writeInt(e.segid);
        writer.writeDouble(e.wear);
        result.push_back(writer.getLine());
    }

    return result;
}

void InitialContactWear::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(InitialContactWear, "*INITIAL_CONTACT_WEAR")

// ============================================================================
// InitialInternalDofSolid
// ============================================================================

bool InitialInternalDofSolid::parse(const std::vector<std::string>& lines,
                                     util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    entries_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        Entry e;
        size_t pos = 0;

        e.eid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        e.nint = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        e.ndof = static_cast<int>(parser.getInt64At(pos).value_or(0));

        if (e.eid != 0) {
            entries_.push_back(e);
        }
    }

    return true;
}

std::vector<std::string> InitialInternalDofSolid::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& e : entries_) {
        writer.clear();
        writer.writeInt(e.eid);
        writer.writeInt(e.nint);
        writer.writeInt(e.ndof);
        result.push_back(writer.getLine());
    }

    return result;
}

void InitialInternalDofSolid::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(InitialInternalDofSolid, "*INITIAL_INTERNAL_DOF_SOLID")

// ============================================================================
// InitialVolumeFraction
// ============================================================================

bool InitialVolumeFraction::parse(const std::vector<std::string>& lines,
                                   util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    data_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        Data d;
        size_t pos = 0;

        d.ammgid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        d.esid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        d.vf = parser.getDoubleAt(pos, realW).value_or(1.0);

        if (d.ammgid != 0) {
            data_.push_back(d);
        }
    }

    return true;
}

std::vector<std::string> InitialVolumeFraction::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& d : data_) {
        writer.clear();
        writer.writeInt(d.ammgid);
        writer.writeInt(d.esid);
        writer.writeDouble(d.vf);
        result.push_back(writer.getLine());
    }

    return result;
}

void InitialVolumeFraction::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(InitialVolumeFraction, "*INITIAL_VOLUME_FRACTION")

// ============================================================================
// InitialVolumeFractionGeometry
// ============================================================================

bool InitialVolumeFractionGeometry::parse(const std::vector<std::string>& lines,
                                           util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.ammgid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.conttyp = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.filltyp = static_cast<int>(parser.getInt64At(pos).value_or(0));
    ++lineIdx;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 2
    parser.setLine(lines[lineIdx]);
    pos = 0;

    data_.xc = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.yc = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.zc = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.radius = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.height = parser.getDoubleAt(pos, realW).value_or(0.0);

    return true;
}

std::vector<std::string> InitialVolumeFractionGeometry::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.ammgid);
    writer.writeInt(data_.conttyp);
    writer.writeInt(data_.filltyp);
    result.push_back(writer.getLine());

    writer.clear();
    writer.writeDouble(data_.xc);
    writer.writeDouble(data_.yc);
    writer.writeDouble(data_.zc);
    writer.writeDouble(data_.radius);
    writer.writeDouble(data_.height);
    result.push_back(writer.getLine());

    return result;
}

void InitialVolumeFractionGeometry::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(InitialVolumeFractionGeometry, "*INITIAL_VOLUME_FRACTION_GEOMETRY")

// ============================================================================
// InitialAleMapping
// ============================================================================

bool InitialAleMapping::parse(const std::vector<std::string>& lines,
                               util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: Filename
    data_.filename = util::StringUtils::trim(lines[lineIdx]);
    ++lineIdx;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 2
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.amgid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.psid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.sx = parser.getDoubleAt(pos, realW).value_or(1.0);
    pos += realW;
    data_.sy = parser.getDoubleAt(pos, realW).value_or(1.0);
    pos += realW;
    data_.sz = parser.getDoubleAt(pos, realW).value_or(1.0);

    return true;
}

std::vector<std::string> InitialAleMapping::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    result.push_back(data_.filename);

    writer.writeInt(data_.amgid);
    writer.writeInt(data_.psid);
    writer.writeDouble(data_.sx);
    writer.writeDouble(data_.sy);
    writer.writeDouble(data_.sz);
    result.push_back(writer.getLine());

    return result;
}

void InitialAleMapping::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(InitialAleMapping, "*INITIAL_ALE_MAPPING")

// ============================================================================
// InitialSpcRotationAngle
// ============================================================================

bool InitialSpcRotationAngle::parse(const std::vector<std::string>& lines,
                                     util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    entries_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        Entry e;
        size_t pos = 0;

        e.nid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        e.angle = parser.getDoubleAt(pos, realW).value_or(0.0);

        if (e.nid != 0) {
            entries_.push_back(e);
        }
    }

    return true;
}

std::vector<std::string> InitialSpcRotationAngle::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& e : entries_) {
        writer.clear();
        writer.writeInt(e.nid);
        writer.writeDouble(e.angle);
        result.push_back(writer.getLine());
    }

    return result;
}

void InitialSpcRotationAngle::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(InitialSpcRotationAngle, "*INITIAL_SPC_ROTATION_ANGLE")

// ============================================================================
// InitialMomentum
// ============================================================================

bool InitialMomentum::parse(const std::vector<std::string>& lines,
                             util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    data_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        Data d;
        size_t pos = 0;

        d.pid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        d.mx = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        d.my = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        d.mz = parser.getDoubleAt(pos, realW).value_or(0.0);

        if (d.pid != 0) {
            data_.push_back(d);
        }
    }

    return true;
}

std::vector<std::string> InitialMomentum::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& d : data_) {
        writer.clear();
        writer.writeInt(d.pid);
        writer.writeDouble(d.mx);
        writer.writeDouble(d.my);
        writer.writeDouble(d.mz);
        result.push_back(writer.getLine());
    }

    return result;
}

void InitialMomentum::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(InitialMomentum, "*INITIAL_MOMENTUM")

// ============================================================================
// InitialAngularMomentum
// ============================================================================

bool InitialAngularMomentum::parse(const std::vector<std::string>& lines,
                                    util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    data_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        Data d;
        size_t pos = 0;

        d.pid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        d.lx = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        d.ly = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        d.lz = parser.getDoubleAt(pos, realW).value_or(0.0);

        if (d.pid != 0) {
            data_.push_back(d);
        }
    }

    return true;
}

std::vector<std::string> InitialAngularMomentum::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& d : data_) {
        writer.clear();
        writer.writeInt(d.pid);
        writer.writeDouble(d.lx);
        writer.writeDouble(d.ly);
        writer.writeDouble(d.lz);
        result.push_back(writer.getLine());
    }

    return result;
}

void InitialAngularMomentum::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(InitialAngularMomentum, "*INITIAL_ANGULAR_MOMENTUM")

// ============================================================================
// InitialSphMassFraction
// ============================================================================

bool InitialSphMassFraction::parse(const std::vector<std::string>& lines,
                                    util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    entries_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        Entry e;
        size_t pos = 0;

        e.nid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        e.ammgid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        e.mf = parser.getDoubleAt(pos, realW).value_or(1.0);

        if (e.nid != 0) {
            entries_.push_back(e);
        }
    }

    return true;
}

std::vector<std::string> InitialSphMassFraction::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& e : entries_) {
        writer.clear();
        writer.writeInt(e.nid);
        writer.writeInt(e.ammgid);
        writer.writeDouble(e.mf);
        result.push_back(writer.getLine());
    }

    return result;
}

void InitialSphMassFraction::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(InitialSphMassFraction, "*INITIAL_SPH_MASS_FRACTION")

// ============================================================================
// InitialRotationalVelocity
// ============================================================================

bool InitialRotationalVelocity::parse(const std::vector<std::string>& lines,
                                       util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.nsid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.omega = parser.getDoubleAt(pos, realW).value_or(0.0);
    ++lineIdx;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 2
    parser.setLine(lines[lineIdx]);
    pos = 0;

    data_.xc = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.yc = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.zc = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.ax = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.ay = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.az = parser.getDoubleAt(pos, realW).value_or(0.0);

    return true;
}

std::vector<std::string> InitialRotationalVelocity::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.nsid);
    writer.writeDouble(data_.omega);
    result.push_back(writer.getLine());

    writer.clear();
    writer.writeDouble(data_.xc);
    writer.writeDouble(data_.yc);
    writer.writeDouble(data_.zc);
    writer.writeDouble(data_.ax);
    writer.writeDouble(data_.ay);
    writer.writeDouble(data_.az);
    result.push_back(writer.getLine());

    return result;
}

void InitialRotationalVelocity::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(InitialRotationalVelocity, "*INITIAL_ROTATIONAL_VELOCITY")

} // namespace koo::dyna
