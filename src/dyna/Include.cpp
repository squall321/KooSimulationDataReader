#include <koo/dyna/Include.hpp>
#include <koo/dyna/ModelVisitor.hpp>
#include <koo/dyna/KeywordFactory.hpp>
#include <koo/util/StringUtils.hpp>

namespace koo::dyna {

// ============================================================================
// Include
// ============================================================================

bool Include::parse(const std::vector<std::string>& lines,
                     util::CardParser::Format /*format*/) {
    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        // Filename is on a single line
        filename_ = util::StringUtils::trim(line);
        if (!filename_.empty()) break;
    }

    return true;
}

std::vector<std::string> Include::write(util::CardParser::Format /*format*/) const {
    std::vector<std::string> result;
    if (!filename_.empty()) {
        result.push_back(filename_);
    }
    return result;
}

void Include::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(Include, "*INCLUDE")

// ============================================================================
// IncludePath
// ============================================================================

bool IncludePath::parse(const std::vector<std::string>& lines,
                         util::CardParser::Format /*format*/) {
    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        path_ = util::StringUtils::trim(line);
        if (!path_.empty()) break;
    }

    return true;
}

std::vector<std::string> IncludePath::write(util::CardParser::Format /*format*/) const {
    std::vector<std::string> result;
    if (!path_.empty()) {
        result.push_back(path_);
    }
    return result;
}

void IncludePath::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(IncludePath, "*INCLUDE_PATH")

// ============================================================================
// IncludePathRelative
// ============================================================================

bool IncludePathRelative::parse(const std::vector<std::string>& lines,
                                 util::CardParser::Format /*format*/) {
    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        path_ = util::StringUtils::trim(line);
        if (!path_.empty()) break;
    }

    return true;
}

std::vector<std::string> IncludePathRelative::write(util::CardParser::Format /*format*/) const {
    std::vector<std::string> result;
    if (!path_.empty()) {
        result.push_back(path_);
    }
    return result;
}

void IncludePathRelative::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(IncludePathRelative, "*INCLUDE_PATH_RELATIVE")

// ============================================================================
// IncludeTransform
// ============================================================================

bool IncludeTransform::parse(const std::vector<std::string>& lines,
                              util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    // Skip comments to find filename
    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: Filename
    data_.filename = util::StringUtils::trim(lines[lineIdx]);
    ++lineIdx;

    // Skip comments
    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 2: IDNOFF, IDEOFF, IDPOFF, IDMOFF, IDSOFF, IDFOFF, IDDOFF, IDROFF
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.idnoff = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.ideoff = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.idpoff = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.idmoff = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.idsoff = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.idfoff = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.iddoff = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.idroff = static_cast<int>(parser.getInt64At(pos).value_or(0));
    ++lineIdx;

    // Skip comments
    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 3: TRANID, FCTMAS, FCTTIM, FCTLEN, FCTTEM, INCOUT1, TRANIDOUT
    parser.setLine(lines[lineIdx]);
    pos = 0;

    data_.tranid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.fctmas = parser.getDoubleAt(pos, realW).value_or(1.0);
    pos += realW;
    data_.fcttim = parser.getDoubleAt(pos, realW).value_or(1.0);
    pos += realW;
    data_.fctlen = parser.getDoubleAt(pos, realW).value_or(1.0);
    pos += realW;
    data_.fcttem = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.incout1 = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.tranidout = static_cast<int>(parser.getInt64At(pos).value_or(0));

    return true;
}

std::vector<std::string> IncludeTransform::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    // Card 1: Filename
    result.push_back(data_.filename);

    // Card 2: ID offsets
    writer.writeInt(data_.idnoff);
    writer.writeInt(data_.ideoff);
    writer.writeInt(data_.idpoff);
    writer.writeInt(data_.idmoff);
    writer.writeInt(data_.idsoff);
    writer.writeInt(data_.idfoff);
    writer.writeInt(data_.iddoff);
    writer.writeInt(data_.idroff);
    result.push_back(writer.getLine());

    // Card 3: Transform and scale factors
    writer.clear();
    writer.writeInt(data_.tranid);
    writer.writeDouble(data_.fctmas);
    writer.writeDouble(data_.fcttim);
    writer.writeDouble(data_.fctlen);
    writer.writeDouble(data_.fcttem);
    writer.writeInt(data_.incout1);
    writer.writeInt(data_.tranidout);
    result.push_back(writer.getLine());

    return result;
}

void IncludeTransform::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(IncludeTransform, "*INCLUDE_TRANSFORM")

// ============================================================================
// IncludeStampedPart
// ============================================================================

bool IncludeStampedPart::parse(const std::vector<std::string>& lines,
                                util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    size_t lineIdx = 0;

    // Skip comments to find filename
    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: Filename
    data_.filename = util::StringUtils::trim(lines[lineIdx]);
    ++lineIdx;

    // Skip comments
    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 2: PID, IDNOFF, IDEOFF
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.pid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.idnoff = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.ideoff = static_cast<int>(parser.getInt64At(pos).value_or(0));

    return true;
}

std::vector<std::string> IncludeStampedPart::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    result.push_back(data_.filename);

    writer.writeInt(data_.pid);
    writer.writeInt(data_.idnoff);
    writer.writeInt(data_.ideoff);
    result.push_back(writer.getLine());

    return result;
}

void IncludeStampedPart::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(IncludeStampedPart, "*INCLUDE_STAMPED_PART")

// ============================================================================
// IncludeAutoOffset
// ============================================================================

bool IncludeAutoOffset::parse(const std::vector<std::string>& lines,
                               util::CardParser::Format /*format*/) {
    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        filename_ = util::StringUtils::trim(line);
        if (!filename_.empty()) break;
    }

    return true;
}

std::vector<std::string> IncludeAutoOffset::write(util::CardParser::Format /*format*/) const {
    std::vector<std::string> result;
    if (!filename_.empty()) {
        result.push_back(filename_);
    }
    return result;
}

void IncludeAutoOffset::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(IncludeAutoOffset, "*INCLUDE_AUTO_OFFSET")

// ============================================================================
// IncludeCompensate
// ============================================================================

bool IncludeCompensate::parse(const std::vector<std::string>& lines,
                               util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    // Skip comments to find filename
    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: Filename
    data_.filename = util::StringUtils::trim(lines[lineIdx]);
    ++lineIdx;

    // Skip comments
    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 2: NITER, TOL
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.niter = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.tol = parser.getDoubleAt(pos, realW).value_or(0.0);

    return true;
}

std::vector<std::string> IncludeCompensate::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    result.push_back(data_.filename);

    writer.writeInt(data_.niter);
    writer.writeDouble(data_.tol);
    result.push_back(writer.getLine());

    return result;
}

void IncludeCompensate::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(IncludeCompensate, "*INCLUDE_COMPENSATE")

// ============================================================================
// IncludeBinary
// ============================================================================

bool IncludeBinary::parse(const std::vector<std::string>& lines,
                           util::CardParser::Format /*format*/) {
    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        filename_ = util::StringUtils::trim(line);
        if (!filename_.empty()) break;
    }

    return true;
}

std::vector<std::string> IncludeBinary::write(util::CardParser::Format /*format*/) const {
    std::vector<std::string> result;
    if (!filename_.empty()) {
        result.push_back(filename_);
    }
    return result;
}

void IncludeBinary::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(IncludeBinary, "*INCLUDE_BINARY")

} // namespace koo::dyna
