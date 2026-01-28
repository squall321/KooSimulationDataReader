#include <koo/dyna/Ale.hpp>
#include <koo/dyna/ModelVisitor.hpp>
#include <koo/dyna/KeywordFactory.hpp>
#include <koo/util/StringUtils.hpp>

namespace koo::dyna {

// ============================================================================
// AleMultiMaterialGroup
// ============================================================================

bool AleMultiMaterialGroup::parse(const std::vector<std::string>& lines,
                                   util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    entries_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        Entry entry;
        size_t pos = 0;

        entry.sid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.idtype = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.gpname = static_cast<int>(parser.getInt64At(pos).value_or(0));

        if (entry.sid != 0) {
            entries_.push_back(entry);
        }
    }

    return true;
}

std::vector<std::string> AleMultiMaterialGroup::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& entry : entries_) {
        writer.clear();
        writer.writeInt(entry.sid);
        writer.writeInt(entry.idtype);
        writer.writeInt(entry.gpname);
        result.push_back(writer.getLine());
    }

    return result;
}

void AleMultiMaterialGroup::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(AleMultiMaterialGroup, "*ALE_MULTI-MATERIAL_GROUP")

// ============================================================================
// AleSmoothing
// ============================================================================

bool AleSmoothing::parse(const std::vector<std::string>& lines,
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

    data_.pid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.afac = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.efac = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.dfac = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.start = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.end = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.prid = static_cast<int>(parser.getInt64At(pos).value_or(0));

    return true;
}

std::vector<std::string> AleSmoothing::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.pid);
    writer.writeInt(data_.afac);
    writer.writeDouble(data_.efac);
    writer.writeInt(data_.dfac);
    writer.writeInt(data_.start);
    writer.writeDouble(data_.end);
    writer.writeInt(data_.prid);
    result.push_back(writer.getLine());

    return result;
}

void AleSmoothing::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(AleSmoothing, "*ALE_SMOOTHING")

// ============================================================================
// AleReferenceSystemNode
// ============================================================================

bool AleReferenceSystemNode::parse(const std::vector<std::string>& lines,
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
    data_.nid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.bctran = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.bcexp = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.bcrot = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.pession = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.pression = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.nsid = static_cast<int>(parser.getInt64At(pos).value_or(0));

    return true;
}

std::vector<std::string> AleReferenceSystemNode::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.id);
    writer.writeInt(data_.nid);
    writer.writeInt(data_.bctran);
    writer.writeInt(data_.bcexp);
    writer.writeInt(data_.bcrot);
    writer.writeDouble(data_.pession);
    writer.writeInt(data_.pression);
    writer.writeInt(data_.nsid);
    result.push_back(writer.getLine());

    return result;
}

void AleReferenceSystemNode::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(AleReferenceSystemNode, "*ALE_REFERENCE_SYSTEM_NODE")

// ============================================================================
// AleReferenceSystemGroup
// ============================================================================

bool AleReferenceSystemGroup::parse(const std::vector<std::string>& lines,
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
    data_.pid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.bctran = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.bcexp = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.bcrot = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.pession = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.pression = static_cast<int>(parser.getInt64At(pos).value_or(0));

    return true;
}

std::vector<std::string> AleReferenceSystemGroup::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.id);
    writer.writeInt(data_.pid);
    writer.writeInt(data_.bctran);
    writer.writeInt(data_.bcexp);
    writer.writeInt(data_.bcrot);
    writer.writeDouble(data_.pession);
    writer.writeInt(data_.pression);
    result.push_back(writer.getLine());

    return result;
}

void AleReferenceSystemGroup::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(AleReferenceSystemGroup, "*ALE_REFERENCE_SYSTEM_GROUP")

// ============================================================================
// AleFsiSwitchMmg
// ============================================================================

bool AleFsiSwitchMmg::parse(const std::vector<std::string>& lines,
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
    data_.ammgid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.swtype = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.btime = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.dtime = parser.getDoubleAt(pos, realW).value_or(0.0);

    return true;
}

std::vector<std::string> AleFsiSwitchMmg::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.id);
    writer.writeInt(data_.ammgid);
    writer.writeInt(data_.swtype);
    writer.writeDouble(data_.btime);
    writer.writeDouble(data_.dtime);
    result.push_back(writer.getLine());

    return result;
}

void AleFsiSwitchMmg::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(AleFsiSwitchMmg, "*ALE_FSI_SWITCH_MMG")

// ============================================================================
// AleAmbientHydrostatic
// ============================================================================

bool AleAmbientHydrostatic::parse(const std::vector<std::string>& lines,
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

    data_.pid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.density = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.gx = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.gy = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.gz = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.refp = parser.getDoubleAt(pos, realW).value_or(0.0);

    return true;
}

std::vector<std::string> AleAmbientHydrostatic::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.pid);
    writer.writeDouble(data_.density);
    writer.writeDouble(data_.gx);
    writer.writeDouble(data_.gy);
    writer.writeDouble(data_.gz);
    writer.writeDouble(data_.refp);
    result.push_back(writer.getLine());

    return result;
}

void AleAmbientHydrostatic::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(AleAmbientHydrostatic, "*ALE_AMBIENT_HYDROSTATIC")

// ============================================================================
// AleStructuredMesh
// ============================================================================

bool AleStructuredMesh::parse(const std::vector<std::string>& lines,
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

    data_.mshid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.dpid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.nbxi = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.nbyi = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.nbzi = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.icase = static_cast<int>(parser.getInt64At(pos).value_or(0));

    return true;
}

std::vector<std::string> AleStructuredMesh::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.mshid);
    writer.writeInt(data_.dpid);
    writer.writeInt(data_.nbxi);
    writer.writeInt(data_.nbyi);
    writer.writeInt(data_.nbzi);
    writer.writeInt(data_.icase);
    result.push_back(writer.getLine());

    return result;
}

void AleStructuredMesh::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(AleStructuredMesh, "*ALE_STRUCTURED_MESH")

// ============================================================================
// AleStructuredMeshControlPoints
// ============================================================================

bool AleStructuredMeshControlPoints::parse(const std::vector<std::string>& lines,
                                            util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (size_t lineIdx = 0; lineIdx < lines.size(); ++lineIdx) {
        if (util::CardParser::isCommentLine(lines[lineIdx])) {
            continue;
        }

        parser.setLine(lines[lineIdx]);
        size_t pos = 0;

        Entry entry;
        entry.cpid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.x = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        entry.y = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        entry.z = parser.getDoubleAt(pos, realW).value_or(0.0);

        if (entry.cpid != 0) {
            entries_.push_back(entry);
        }
    }

    return true;
}

std::vector<std::string> AleStructuredMeshControlPoints::write(util::CardParser::Format format) const {
    std::vector<std::string> result;

    for (const auto& entry : entries_) {
        util::CardWriter writer(format);
        writer.writeInt(entry.cpid);
        writer.writeDouble(entry.x);
        writer.writeDouble(entry.y);
        writer.writeDouble(entry.z);
        result.push_back(writer.getLine());
    }

    return result;
}

void AleStructuredMeshControlPoints::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(AleStructuredMeshControlPoints, "*ALE_STRUCTURED_MESH_CONTROL_POINTS")

// ============================================================================
// AleFailSwitchMmg
// ============================================================================

bool AleFailSwitchMmg::parse(const std::vector<std::string>& lines,
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
    data_.ammgid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.tammgid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.fval = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.ftyp = static_cast<int>(parser.getInt64At(pos).value_or(0));

    return true;
}

std::vector<std::string> AleFailSwitchMmg::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.id);
    writer.writeInt(data_.ammgid);
    writer.writeInt(data_.tammgid);
    writer.writeDouble(data_.fval);
    writer.writeInt(data_.ftyp);
    result.push_back(writer.getLine());

    return result;
}

void AleFailSwitchMmg::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(AleFailSwitchMmg, "*ALE_FAIL_SWITCH_MMG")

// ============================================================================
// AleEssentialBoundary
// ============================================================================

bool AleEssentialBoundary::parse(const std::vector<std::string>& lines,
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

    data_.nsid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.ssid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.dofx = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.dofy = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.dofz = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.pid = static_cast<int>(parser.getInt64At(pos).value_or(0));

    return true;
}

std::vector<std::string> AleEssentialBoundary::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.nsid);
    writer.writeInt(data_.ssid);
    writer.writeInt(data_.dofx);
    writer.writeInt(data_.dofy);
    writer.writeInt(data_.dofz);
    writer.writeInt(data_.pid);
    result.push_back(writer.getLine());

    return result;
}

void AleEssentialBoundary::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(AleEssentialBoundary, "*ALE_ESSENTIAL_BOUNDARY")

// ============================================================================
// AleCouplingNodalConstraint
// ============================================================================

bool AleCouplingNodalConstraint::parse(const std::vector<std::string>& lines,
                                        util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: SLAVE, MASTER, CTYPE, MCOUP, START, END, PFAC
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.slave = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.master = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.ctype = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.mcoup = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.start = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.end = parser.getDoubleAt(pos, realW).value_or(1e28);
    pos += realW;
    data_.pfac = parser.getDoubleAt(pos, realW).value_or(0.1);

    return true;
}

std::vector<std::string> AleCouplingNodalConstraint::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.slave);
    writer.writeInt(data_.master);
    writer.writeInt(data_.ctype);
    writer.writeDouble(data_.mcoup);
    writer.writeDouble(data_.start);
    writer.writeDouble(data_.end);
    writer.writeDouble(data_.pfac);
    result.push_back(writer.getLine());

    return result;
}

void AleCouplingNodalConstraint::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(AleCouplingNodalConstraint, "*ALE_COUPLING_NODAL_CONSTRAINT")

} // namespace koo::dyna
