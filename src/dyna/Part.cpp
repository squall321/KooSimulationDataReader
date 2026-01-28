#include <koo/dyna/Part.hpp>
#include <koo/dyna/ModelVisitor.hpp>
#include <koo/dyna/KeywordFactory.hpp>
#include <koo/util/StringUtils.hpp>

namespace koo::dyna {

bool Part::parse(const std::vector<std::string>& lines,
                 util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();  // Always 10 for integers

    // *PART has two cards per part:
    // Card 1: Title (optional heading)
    // Card 2: PID(I), SECID(I), MID(I), EOSID(I), HGID(I), GRAV(I), ADPOPT(I), TMID(I)
    // Note: All fields are integers, so they're all 10 chars even in LONG=S format

    size_t i = 0;
    while (i < lines.size()) {
        // Skip comments and keyword lines
        while (i < lines.size() &&
               (util::CardParser::isCommentLine(lines[i]) ||
                util::CardParser::isKeywordLine(lines[i]))) {
            ++i;
        }
        if (i >= lines.size()) break;

        PartData part;

        // First line is title
        part.title = util::StringUtils::trim(lines[i]);
        ++i;

        if (i >= lines.size()) break;

        // Skip comments
        while (i < lines.size() && util::CardParser::isCommentLine(lines[i])) {
            ++i;
        }
        if (i >= lines.size()) break;

        // Second line is data - all integer fields
        parser.setLine(lines[i]);
        size_t pos = 0;
        part.id = parser.getInt64At(pos).value_or(0);
        pos += intW;
        part.secid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        part.mid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        part.eosid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        part.hgid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        part.grav = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        part.adpopt = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        part.tmid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        ++i;

        if (part.id != 0) {
            addPart(part);
        }
    }

    return true;
}

std::vector<std::string> Part::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (const auto& part : parts_) {
        // Title line
        result.push_back(part.title);

        // Data line
        writer.clear();
        writer.writeInt(part.id);
        writer.writeInt(part.secid);
        writer.writeInt(part.mid);
        writer.writeInt(part.eosid);
        writer.writeInt(part.hgid);
        writer.writeInt(part.grav);
        writer.writeInt(part.adpopt);
        writer.writeInt(part.tmid);
        result.push_back(writer.getLine());
    }

    return result;
}

void Part::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

void Part::addPart(const PartData& part) {
    auto it = idIndex_.find(part.id);
    if (it != idIndex_.end()) {
        parts_[it->second] = part;
    } else {
        idIndex_[part.id] = parts_.size();
        parts_.push_back(part);
    }
}

void Part::addPart(PartId id, SectionId secid, MaterialId mid,
                   const std::string& title) {
    addPart(PartData(id, secid, mid, title));
}

bool Part::hasPart(PartId id) const {
    return idIndex_.find(id) != idIndex_.end();
}

PartData* Part::getPart(PartId id) {
    auto it = idIndex_.find(id);
    if (it != idIndex_.end()) {
        return &parts_[it->second];
    }
    return nullptr;
}

const PartData* Part::getPart(PartId id) const {
    auto it = idIndex_.find(id);
    if (it != idIndex_.end()) {
        return &parts_[it->second];
    }
    return nullptr;
}

void Part::removePart(PartId id) {
    auto it = idIndex_.find(id);
    if (it != idIndex_.end()) {
        size_t index = it->second;
        parts_.erase(parts_.begin() + static_cast<std::ptrdiff_t>(index));
        rebuildIndex();
    }
}

void Part::clear() {
    parts_.clear();
    idIndex_.clear();
}

void Part::rebuildIndex() {
    idIndex_.clear();
    for (size_t i = 0; i < parts_.size(); ++i) {
        idIndex_[parts_[i].id] = i;
    }
}

// ============================================================================
// PartComposite
// ============================================================================

bool PartComposite::parse(const std::vector<std::string>& lines,
                          util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t i = 0;

    // Skip comments and keyword lines
    while (i < lines.size() &&
           (util::CardParser::isCommentLine(lines[i]) ||
            util::CardParser::isKeywordLine(lines[i]))) {
        ++i;
    }
    if (i >= lines.size()) return true;

    // Title line
    data_.title = util::StringUtils::trim(lines[i]);
    ++i;

    // Skip comments
    while (i < lines.size() && util::CardParser::isCommentLine(lines[i])) {
        ++i;
    }
    if (i >= lines.size()) return true;

    // Card 1: PID, ELFORM, SHRF, NLOC, MAREA, HGID, ADPOPT, ITHELFRM
    parser.setLine(lines[i]);
    size_t pos = 0;
    data_.pid = parser.getInt64At(pos).value_or(0);
    pos += intW;
    data_.elform = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.shrf = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.nloc = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.marea = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.hgid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.adpopt = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.ithelfrm = static_cast<int>(parser.getInt64At(pos).value_or(0));
    ++i;

    // Layer cards: MID, THICK, B, ITHID (4 layers per card)
    while (i < lines.size()) {
        if (util::CardParser::isCommentLine(lines[i])) {
            ++i;
            continue;
        }
        if (util::CardParser::isKeywordLine(lines[i])) {
            break;
        }

        parser.setLine(lines[i]);
        pos = 0;

        // Up to 4 layers per card
        for (int j = 0; j < 4; ++j) {
            auto mid = parser.getInt64At(pos);
            if (!mid || *mid == 0) {
                pos += intW + realW + realW + intW;
                continue;
            }

            PartCompositeLayer layer;
            layer.mid = *mid;
            pos += intW;
            layer.thick = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            layer.b = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            layer.ithid = static_cast<int>(parser.getInt64At(pos).value_or(0));
            pos += intW;

            data_.layers.push_back(layer);
        }
        ++i;
    }

    return true;
}

std::vector<std::string> PartComposite::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    // Title
    result.push_back(data_.title);

    // Card 1
    writer.writeInt(data_.pid);
    writer.writeInt(data_.elform);
    writer.writeInt(data_.shrf);
    writer.writeInt(data_.nloc);
    writer.writeDouble(data_.marea);
    writer.writeInt(data_.hgid);
    writer.writeInt(data_.adpopt);
    writer.writeInt(data_.ithelfrm);
    result.push_back(writer.getLine());

    // Layer cards (4 layers per card)
    for (size_t i = 0; i < data_.layers.size(); i += 4) {
        writer.clear();
        for (size_t j = i; j < std::min(i + 4, data_.layers.size()); ++j) {
            const auto& layer = data_.layers[j];
            writer.writeInt(layer.mid);
            writer.writeDouble(layer.thick);
            writer.writeDouble(layer.b);
            writer.writeInt(layer.ithid);
        }
        result.push_back(writer.getLine());
    }

    return result;
}

void PartComposite::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

// ============================================================================
// PartInertia
// ============================================================================

bool PartInertia::parse(const std::vector<std::string>& lines,
                        util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t i = 0;

    // Skip comments and keyword lines
    while (i < lines.size() &&
           (util::CardParser::isCommentLine(lines[i]) ||
            util::CardParser::isKeywordLine(lines[i]))) {
        ++i;
    }
    if (i >= lines.size()) return true;

    // Title line
    data_.title = util::StringUtils::trim(lines[i]);
    ++i;

    // Skip comments
    while (i < lines.size() && util::CardParser::isCommentLine(lines[i])) {
        ++i;
    }
    if (i >= lines.size()) return true;

    // Card 1: PID, SECID, MID, EOSID, HGID, GRAV, ADPOPT, TMID
    parser.setLine(lines[i]);
    size_t pos = 0;
    data_.pid = parser.getInt64At(pos).value_or(0);
    pos += intW;
    data_.secid = parser.getInt64At(pos).value_or(0);
    pos += intW;
    data_.mid = parser.getInt64At(pos).value_or(0);
    pos += intW;
    data_.eosid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.hgid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.grav = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.adpopt = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.tmid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    ++i;

    // Skip comments
    while (i < lines.size() && util::CardParser::isCommentLine(lines[i])) {
        ++i;
    }
    if (i >= lines.size()) return true;

    // Card 2: XC, YC, ZC, TM, IRCS, NODEID
    parser.setLine(lines[i]);
    pos = 0;
    data_.xc = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.yc = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.zc = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.tm = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.ircs = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.nodeid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    ++i;

    // Skip comments
    while (i < lines.size() && util::CardParser::isCommentLine(lines[i])) {
        ++i;
    }
    if (i >= lines.size()) return true;

    // Card 3: IXX, IXY, IXZ, IYY, IYZ, IZZ
    parser.setLine(lines[i]);
    pos = 0;
    data_.ixx = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.ixy = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.ixz = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.iyy = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.iyz = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.izz = parser.getDoubleAt(pos, realW).value_or(0.0);
    ++i;

    // Skip comments
    while (i < lines.size() && util::CardParser::isCommentLine(lines[i])) {
        ++i;
    }
    if (i >= lines.size()) return true;

    // Card 4: VX, VY, VZ, VXR, VYR, VZR (optional)
    parser.setLine(lines[i]);
    pos = 0;
    data_.vx = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.vy = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.vz = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.vxr = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.vyr = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.vzr = parser.getDoubleAt(pos, realW).value_or(0.0);

    return true;
}

std::vector<std::string> PartInertia::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    // Title
    result.push_back(data_.title);

    // Card 1
    writer.writeInt(data_.pid);
    writer.writeInt(data_.secid);
    writer.writeInt(data_.mid);
    writer.writeInt(data_.eosid);
    writer.writeInt(data_.hgid);
    writer.writeInt(data_.grav);
    writer.writeInt(data_.adpopt);
    writer.writeInt(data_.tmid);
    result.push_back(writer.getLine());

    // Card 2
    writer.clear();
    writer.writeDouble(data_.xc);
    writer.writeDouble(data_.yc);
    writer.writeDouble(data_.zc);
    writer.writeDouble(data_.tm);
    writer.writeInt(data_.ircs);
    writer.writeInt(data_.nodeid);
    result.push_back(writer.getLine());

    // Card 3
    writer.clear();
    writer.writeDouble(data_.ixx);
    writer.writeDouble(data_.ixy);
    writer.writeDouble(data_.ixz);
    writer.writeDouble(data_.iyy);
    writer.writeDouble(data_.iyz);
    writer.writeDouble(data_.izz);
    result.push_back(writer.getLine());

    // Card 4
    writer.clear();
    writer.writeDouble(data_.vx);
    writer.writeDouble(data_.vy);
    writer.writeDouble(data_.vz);
    writer.writeDouble(data_.vxr);
    writer.writeDouble(data_.vyr);
    writer.writeDouble(data_.vzr);
    result.push_back(writer.getLine());

    return result;
}

void PartInertia::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

// ============================================================================
// PartContact
// ============================================================================

bool PartContact::parse(const std::vector<std::string>& lines,
                        util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t i = 0;

    // Skip comments and keyword lines
    while (i < lines.size() &&
           (util::CardParser::isCommentLine(lines[i]) ||
            util::CardParser::isKeywordLine(lines[i]))) {
        ++i;
    }
    if (i >= lines.size()) return true;

    // Title line
    data_.title = util::StringUtils::trim(lines[i]);
    ++i;

    // Skip comments
    while (i < lines.size() && util::CardParser::isCommentLine(lines[i])) {
        ++i;
    }
    if (i >= lines.size()) return true;

    // Card 1: PID
    parser.setLine(lines[i]);
    data_.pid = parser.getInt64At(0).value_or(0);
    ++i;

    // Skip comments
    while (i < lines.size() && util::CardParser::isCommentLine(lines[i])) {
        ++i;
    }
    if (i >= lines.size()) return true;

    // Card 2: FS, FD, DC, VC, OPTEFLAG, SFS, SFM
    parser.setLine(lines[i]);
    size_t pos = 0;
    data_.fs = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.fd = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.dc = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.vc = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.opteflag = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.sfs = parser.getDoubleAt(pos, realW).value_or(1.0);
    pos += realW;
    data_.sfm = parser.getDoubleAt(pos, realW).value_or(1.0);

    return true;
}

std::vector<std::string> PartContact::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    // Title
    result.push_back(data_.title);

    // Card 1
    writer.writeInt(data_.pid);
    result.push_back(writer.getLine());

    // Card 2
    writer.clear();
    writer.writeDouble(data_.fs);
    writer.writeDouble(data_.fd);
    writer.writeDouble(data_.dc);
    writer.writeDouble(data_.vc);
    writer.writeInt(data_.opteflag);
    writer.writeDouble(data_.sfs);
    writer.writeDouble(data_.sfm);
    result.push_back(writer.getLine());

    return result;
}

void PartContact::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

// ============================================================================
// PartMove
// ============================================================================

bool PartMove::parse(const std::vector<std::string>& lines,
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
        data_.pid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data_.xmov = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.ymov = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.zmov = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.cid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.xx = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.yy = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.zz = parser.getDoubleAt(pos, realW).value_or(0.0);
        break;
    }

    return true;
}

std::vector<std::string> PartMove::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.pid);
    writer.writeDouble(data_.xmov);
    writer.writeDouble(data_.ymov);
    writer.writeDouble(data_.zmov);
    writer.writeInt(data_.cid);
    writer.writeDouble(data_.xx);
    writer.writeDouble(data_.yy);
    writer.writeDouble(data_.zz);
    result.push_back(writer.getLine());

    return result;
}

void PartMove::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

// ============================================================================
// PartStiffness
// ============================================================================

bool PartStiffness::parse(const std::vector<std::string>& lines,
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
        data_.pid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data_.k = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.lcid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.sf = parser.getDoubleAt(pos, realW).value_or(1.0);
        break;
    }

    return true;
}

std::vector<std::string> PartStiffness::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.pid);
    writer.writeDouble(data_.k);
    writer.writeInt(data_.lcid);
    writer.writeDouble(data_.sf);
    result.push_back(writer.getLine());

    return result;
}

void PartStiffness::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

// ============================================================================
// PartAnneal
// ============================================================================

bool PartAnneal::parse(const std::vector<std::string>& lines,
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
        data_.pid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data_.tanneal = parser.getDoubleAt(pos, realW).value_or(0.0);
        break;
    }

    return true;
}

std::vector<std::string> PartAnneal::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.pid);
    writer.writeDouble(data_.tanneal);
    result.push_back(writer.getLine());

    return result;
}

void PartAnneal::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

// ============================================================================
// PartSensor
// ============================================================================

bool PartSensor::parse(const std::vector<std::string>& lines,
                       util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);
        size_t pos = 0;
        data_.pid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data_.sensid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.typeid_ = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> PartSensor::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.pid);
    writer.writeInt(data_.sensid);
    writer.writeInt(data_.typeid_);
    result.push_back(writer.getLine());

    return result;
}

void PartSensor::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

// ============================================================================
// PartAveraged
// ============================================================================

bool PartAveraged::parse(const std::vector<std::string>& lines,
                         util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    size_t i = 0;

    // Skip comments and keyword lines
    while (i < lines.size() &&
           (util::CardParser::isCommentLine(lines[i]) ||
            util::CardParser::isKeywordLine(lines[i]))) {
        ++i;
    }
    if (i >= lines.size()) return true;

    // Title line
    data_.title = util::StringUtils::trim(lines[i]);
    ++i;

    // Skip comments
    while (i < lines.size() && util::CardParser::isCommentLine(lines[i])) {
        ++i;
    }
    if (i >= lines.size()) return true;

    // Data card: PID, SECID, MID, EOSID, HGID, GRAV, ADPOPT, TMID
    parser.setLine(lines[i]);
    size_t pos = 0;
    data_.pid = parser.getInt64At(pos).value_or(0);
    pos += intW;
    data_.secid = parser.getInt64At(pos).value_or(0);
    pos += intW;
    data_.mid = parser.getInt64At(pos).value_or(0);
    pos += intW;
    data_.eosid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.hgid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.grav = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.adpopt = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.tmid = static_cast<int>(parser.getInt64At(pos).value_or(0));

    return true;
}

std::vector<std::string> PartAveraged::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    // Title
    result.push_back(data_.title);

    // Data card
    writer.writeInt(data_.pid);
    writer.writeInt(data_.secid);
    writer.writeInt(data_.mid);
    writer.writeInt(data_.eosid);
    writer.writeInt(data_.hgid);
    writer.writeInt(data_.grav);
    writer.writeInt(data_.adpopt);
    writer.writeInt(data_.tmid);
    result.push_back(writer.getLine());

    return result;
}

void PartAveraged::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

// ============================================================================
// PartDuplicate
// ============================================================================

bool PartDuplicate::parse(const std::vector<std::string>& lines,
                          util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);
        size_t pos = 0;
        data_.pid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data_.pidcopy = parser.getInt64At(pos).value_or(0);
        break;
    }

    return true;
}

std::vector<std::string> PartDuplicate::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.pid);
    writer.writeInt(data_.pidcopy);
    result.push_back(writer.getLine());

    return result;
}

void PartDuplicate::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

// ============================================================================
// PartModes
// ============================================================================

bool PartModes::parse(const std::vector<std::string>& lines,
                      util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t i = 0;

    // Skip comments and keyword lines
    while (i < lines.size() &&
           (util::CardParser::isCommentLine(lines[i]) ||
            util::CardParser::isKeywordLine(lines[i]))) {
        ++i;
    }
    if (i >= lines.size()) return true;

    // Title line
    data_.title = util::StringUtils::trim(lines[i]);
    ++i;

    // Skip comments
    while (i < lines.size() && util::CardParser::isCommentLine(lines[i])) {
        ++i;
    }
    if (i >= lines.size()) return true;

    // PID card
    parser.setLine(lines[i]);
    data_.pid = parser.getInt64At(0).value_or(0);
    ++i;

    // Mode cards
    while (i < lines.size()) {
        if (util::CardParser::isCommentLine(lines[i])) {
            ++i;
            continue;
        }
        if (util::CardParser::isKeywordLine(lines[i])) {
            break;
        }

        parser.setLine(lines[i]);
        size_t pos = 0;

        auto modeid = parser.getInt64At(pos);
        if (!modeid || *modeid == 0) {
            ++i;
            continue;
        }

        ModeData mode;
        mode.modeid = static_cast<int>(*modeid);
        pos += intW;
        mode.freq = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        mode.damp = parser.getDoubleAt(pos, realW).value_or(0.0);
        data_.modes.push_back(mode);
        ++i;
    }

    return true;
}

std::vector<std::string> PartModes::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    // Title
    result.push_back(data_.title);

    // PID card
    writer.writeInt(data_.pid);
    result.push_back(writer.getLine());

    // Mode cards
    for (const auto& mode : data_.modes) {
        writer.clear();
        writer.writeInt(mode.modeid);
        writer.writeDouble(mode.freq);
        writer.writeDouble(mode.damp);
        result.push_back(writer.getLine());
    }

    return result;
}

void PartModes::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

// ============================================================================
// PartAdaptiveFailure
// ============================================================================

bool PartAdaptiveFailure::parse(const std::vector<std::string>& lines,
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
        data_.pid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data_.ifail = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.fs = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.fd = parser.getDoubleAt(pos, realW).value_or(0.0);
        break;
    }

    return true;
}

std::vector<std::string> PartAdaptiveFailure::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.pid);
    writer.writeInt(data_.ifail);
    writer.writeDouble(data_.fs);
    writer.writeDouble(data_.fd);
    result.push_back(writer.getLine());

    return result;
}

void PartAdaptiveFailure::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

// ============================================================================
// PartCompositeTshell
// ============================================================================

bool PartCompositeTshell::parse(const std::vector<std::string>& lines,
                                util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t i = 0;

    // Skip comments and keyword lines
    while (i < lines.size() &&
           (util::CardParser::isCommentLine(lines[i]) ||
            util::CardParser::isKeywordLine(lines[i]))) {
        ++i;
    }
    if (i >= lines.size()) return true;

    // Title line
    data_.title = util::StringUtils::trim(lines[i]);
    ++i;

    // Skip comments
    while (i < lines.size() && util::CardParser::isCommentLine(lines[i])) {
        ++i;
    }
    if (i >= lines.size()) return true;

    // Card 1: PID, ELFORM, SHRF, NLOC, MAREA, HGID, ADPOPT, ITHELFRM
    parser.setLine(lines[i]);
    size_t pos = 0;
    data_.pid = parser.getInt64At(pos).value_or(0);
    pos += intW;
    data_.elform = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.shrf = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.nloc = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.marea = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.hgid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.adpopt = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.ithelfrm = static_cast<int>(parser.getInt64At(pos).value_or(0));
    ++i;

    // Layer cards: MID, THICK, B, ITHID (4 layers per card)
    while (i < lines.size()) {
        if (util::CardParser::isCommentLine(lines[i])) {
            ++i;
            continue;
        }
        if (util::CardParser::isKeywordLine(lines[i])) {
            break;
        }

        parser.setLine(lines[i]);
        pos = 0;

        // Up to 4 layers per card
        for (int j = 0; j < 4; ++j) {
            auto mid = parser.getInt64At(pos);
            if (!mid || *mid == 0) {
                pos += intW + realW + realW + intW;
                continue;
            }

            PartCompositeLayer layer;
            layer.mid = *mid;
            pos += intW;
            layer.thick = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            layer.b = parser.getDoubleAt(pos, realW).value_or(0.0);
            pos += realW;
            layer.ithid = static_cast<int>(parser.getInt64At(pos).value_or(0));
            pos += intW;

            data_.layers.push_back(layer);
        }
        ++i;
    }

    return true;
}

std::vector<std::string> PartCompositeTshell::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    // Title
    result.push_back(data_.title);

    // Card 1
    writer.writeInt(data_.pid);
    writer.writeInt(data_.elform);
    writer.writeInt(data_.shrf);
    writer.writeInt(data_.nloc);
    writer.writeDouble(data_.marea);
    writer.writeInt(data_.hgid);
    writer.writeInt(data_.adpopt);
    writer.writeInt(data_.ithelfrm);
    result.push_back(writer.getLine());

    // Layer cards (4 layers per card)
    for (size_t i = 0; i < data_.layers.size(); i += 4) {
        writer.clear();
        for (size_t j = i; j < std::min(i + 4, data_.layers.size()); ++j) {
            const auto& layer = data_.layers[j];
            writer.writeInt(layer.mid);
            writer.writeDouble(layer.thick);
            writer.writeDouble(layer.b);
            writer.writeInt(layer.ithid);
        }
        result.push_back(writer.getLine());
    }

    return result;
}

void PartCompositeTshell::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

// ============================================================================
// ParticleBlast
// ============================================================================

bool ParticleBlast::parse(const std::vector<std::string>& lines,
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
        data_.pid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.mass = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.xc = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.yc = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.zc = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.energy = parser.getDoubleAt(pos, realW).value_or(0.0);
        break;
    }

    return true;
}

std::vector<std::string> ParticleBlast::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.pid);
    writer.writeDouble(data_.mass);
    writer.writeDouble(data_.xc);
    writer.writeDouble(data_.yc);
    writer.writeDouble(data_.zc);
    writer.writeDouble(data_.energy);
    result.push_back(writer.getLine());

    return result;
}

void ParticleBlast::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

// ============================================================================
// PartsDistribute
// ============================================================================

bool PartsDistribute::parse(const std::vector<std::string>& lines,
                            util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);
        size_t pos = 0;
        data_.method = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.nprc = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> PartsDistribute::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.method);
    writer.writeInt(data_.nprc);
    result.push_back(writer.getLine());

    return result;
}

void PartsDistribute::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

// ============================================================================
// PartsetDistribute
// ============================================================================

bool PartsetDistribute::parse(const std::vector<std::string>& lines,
                              util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);
        size_t pos = 0;
        data_.psid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.method = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> PartsetDistribute::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.psid);
    writer.writeInt(data_.method);
    result.push_back(writer.getLine());

    return result;
}

void PartsetDistribute::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

// ============================================================================
// PartStackedElements
// ============================================================================

bool PartStackedElements::parse(const std::vector<std::string>& lines,
                                util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line) ||
            util::CardParser::isKeywordLine(line)) {
            continue;
        }

        parser.setLine(line);
        size_t pos = 0;
        data_.pid = parser.getInt64At(pos).value_or(0);
        pos += intW;
        data_.nlayers = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.icoord = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> PartStackedElements::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.pid);
    writer.writeInt(data_.nlayers);
    writer.writeInt(data_.icoord);
    result.push_back(writer.getLine());

    return result;
}

void PartStackedElements::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

// Register keywords
REGISTER_KEYWORD(Part, "*PART")
REGISTER_KEYWORD(PartComposite, "*PART_COMPOSITE")
REGISTER_KEYWORD(PartInertia, "*PART_INERTIA")
REGISTER_KEYWORD(PartContact, "*PART_CONTACT")
REGISTER_KEYWORD(PartMove, "*PART_MOVE")
REGISTER_KEYWORD(PartStiffness, "*PART_STIFFNESS")
REGISTER_KEYWORD(PartAnneal, "*PART_ANNEAL")
REGISTER_KEYWORD(PartSensor, "*PART_SENSOR")
REGISTER_KEYWORD(PartAveraged, "*PART_AVERAGED")
REGISTER_KEYWORD(PartDuplicate, "*PART_DUPLICATE")
REGISTER_KEYWORD(PartModes, "*PART_MODES")
REGISTER_KEYWORD(PartAdaptiveFailure, "*PART_ADAPTIVE_FAILURE")
REGISTER_KEYWORD(PartCompositeTshell, "*PART_COMPOSITE_TSHELL")
REGISTER_KEYWORD(ParticleBlast, "*PARTICLE_BLAST")
REGISTER_KEYWORD(PartsDistribute, "*PARTS_DISTRIBUTE")
REGISTER_KEYWORD(PartsetDistribute, "*PARTSET_DISTRIBUTE")
REGISTER_KEYWORD(PartStackedElements, "*PART_STACKED_ELEMENTS")

} // namespace koo::dyna
