#include <koo/dyna/Sph.hpp>
#include <koo/dyna/ModelVisitor.hpp>
#include <koo/dyna/KeywordFactory.hpp>
#include <koo/util/StringUtils.hpp>

namespace koo::dyna {

// ============================================================================
// ControlSph
// ============================================================================

bool ControlSph::parse(const std::vector<std::string>& lines,
                        util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: NCBS, BOXID, DT2MSF, STEFR, PSTEFR, SWSF, ISHIFT, MEMORY
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.ncbs = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.boxid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.dt2msf = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.stefr = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.pstefr = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.swsf = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.ishift = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.memory = parser.getDoubleAt(pos, realW).value_or(0.0);

    return true;
}

std::vector<std::string> ControlSph::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.ncbs);
    writer.writeInt(data_.boxid);
    writer.writeInt(data_.dt2msf);
    writer.writeDouble(data_.stefr);
    writer.writeInt(data_.pstefr);
    writer.writeInt(data_.swsf);
    writer.writeInt(data_.ishift);
    writer.writeDouble(data_.memory);
    result.push_back(writer.getLine());

    return result;
}

void ControlSph::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ControlSph, "*CONTROL_SPH")

// ============================================================================
// SectionSph
// ============================================================================

bool SectionSph::parse(const std::vector<std::string>& lines,
                        util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: SECID, CSLH, HMIN, HMAX, SPHINI, DEATH, START
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.secid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.cslh = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.hmin = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.hmax = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.sphini = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.death = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.start = parser.getDoubleAt(pos, realW).value_or(0.0);

    return true;
}

std::vector<std::string> SectionSph::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.secid);
    writer.writeDouble(data_.cslh);
    writer.writeDouble(data_.hmin);
    writer.writeDouble(data_.hmax);
    writer.writeDouble(data_.sphini);
    writer.writeDouble(data_.death);
    writer.writeDouble(data_.start);
    result.push_back(writer.getLine());

    return result;
}

void SectionSph::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(SectionSph, "*SECTION_SPH")

// ============================================================================
// ElementSph
// ============================================================================

bool ElementSph::parse(const std::vector<std::string>& lines,
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
        entry.nid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.pid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        entry.mass = parser.getDoubleAt(pos, realW).value_or(0.0);

        if (entry.nid != 0) {
            entries_.push_back(entry);
        }
    }

    return true;
}

std::vector<std::string> ElementSph::write(util::CardParser::Format format) const {
    std::vector<std::string> result;

    for (const auto& entry : entries_) {
        util::CardWriter writer(format);
        writer.writeInt(entry.nid);
        writer.writeInt(entry.pid);
        writer.writeDouble(entry.mass);
        result.push_back(writer.getLine());
    }

    return result;
}

void ElementSph::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ElementSph, "*ELEMENT_SPH")

// ============================================================================
// DefineSphMeshBox
// ============================================================================

bool DefineSphMeshBox::parse(const std::vector<std::string>& lines,
                              util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: BOXID, XMIN, XMAX, YMIN, YMAX, ZMIN, ZMAX
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.boxid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.xmin = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.xmax = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.ymin = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.ymax = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.zmin = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.zmax = parser.getDoubleAt(pos, realW).value_or(0.0);

    return true;
}

std::vector<std::string> DefineSphMeshBox::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.boxid);
    writer.writeDouble(data_.xmin);
    writer.writeDouble(data_.xmax);
    writer.writeDouble(data_.ymin);
    writer.writeDouble(data_.ymax);
    writer.writeDouble(data_.zmin);
    writer.writeDouble(data_.zmax);
    result.push_back(writer.getLine());

    return result;
}

void DefineSphMeshBox::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DefineSphMeshBox, "*DEFINE_SPH_MESH_BOX")

// ============================================================================
// DefineSphAmbientDrag
// ============================================================================

bool DefineSphAmbientDrag::parse(const std::vector<std::string>& lines,
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
    data_.cd = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.rho = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.vx = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.vy = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.vz = parser.getDoubleAt(pos, realW).value_or(0.0);

    return true;
}

std::vector<std::string> DefineSphAmbientDrag::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.pid);
    writer.writeDouble(data_.cd);
    writer.writeDouble(data_.rho);
    writer.writeDouble(data_.vx);
    writer.writeDouble(data_.vy);
    writer.writeDouble(data_.vz);
    result.push_back(writer.getLine());

    return result;
}

void DefineSphAmbientDrag::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DefineSphAmbientDrag, "*DEFINE_SPH_AMBIENT_DRAG")

// ============================================================================
// DefineSphToSphCoupling
// ============================================================================

bool DefineSphToSphCoupling::parse(const std::vector<std::string>& lines,
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

    data_.pid1 = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.pid2 = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.ctype = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.stiff = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.damp = parser.getDoubleAt(pos, realW).value_or(0.0);

    return true;
}

std::vector<std::string> DefineSphToSphCoupling::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.pid1);
    writer.writeInt(data_.pid2);
    writer.writeInt(data_.ctype);
    writer.writeDouble(data_.stiff);
    writer.writeDouble(data_.damp);
    result.push_back(writer.getLine());

    return result;
}

void DefineSphToSphCoupling::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DefineSphToSphCoupling, "*DEFINE_SPH_TO_SPH_COUPLING")

// ============================================================================
// DefineSphInjection
// ============================================================================

bool DefineSphInjection::parse(const std::vector<std::string>& lines,
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
    data_.nsid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.rate = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.vx = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.vy = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.vz = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.temp = parser.getDoubleAt(pos, realW).value_or(0.0);

    return true;
}

std::vector<std::string> DefineSphInjection::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.id);
    writer.writeInt(data_.pid);
    writer.writeInt(data_.nsid);
    writer.writeDouble(data_.rate);
    writer.writeDouble(data_.vx);
    writer.writeDouble(data_.vy);
    writer.writeDouble(data_.vz);
    writer.writeDouble(data_.temp);
    result.push_back(writer.getLine());

    return result;
}

void DefineSphInjection::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DefineSphInjection, "*DEFINE_SPH_INJECTION")

// ============================================================================
// DatabaseSphFlow
// ============================================================================

bool DatabaseSphFlow::parse(const std::vector<std::string>& lines,
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

    data_.dt = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.lcdt = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.binary = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.lcur = static_cast<int>(parser.getInt64At(pos).value_or(0));

    return true;
}

std::vector<std::string> DatabaseSphFlow::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeDouble(data_.dt);
    writer.writeInt(data_.lcdt);
    writer.writeInt(data_.binary);
    writer.writeInt(data_.lcur);
    result.push_back(writer.getLine());

    return result;
}

void DatabaseSphFlow::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DatabaseSphFlow, "*DATABASE_SPH_FLOW")

} // namespace koo::dyna
