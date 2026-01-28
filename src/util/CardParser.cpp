#include <koo/util/CardParser.hpp>
#include <koo/util/StringUtils.hpp>
#include <algorithm>
#include <iomanip>
#include <sstream>

namespace koo::util {

CardParser::CardParser(Format format)
    : format_(format) {}

void CardParser::setLine(std::string_view line) {
    line_ = line;
}

std::string_view CardParser::getFieldAt(size_t startPos, size_t width) const {
    return StringUtils::getField(line_, startPos, width);
}

std::string_view CardParser::getField(size_t index) const {
    // Legacy method: assumes uniform 10-column fields (standard format behavior)
    size_t width = 10;
    size_t start = index * width;
    return StringUtils::getField(line_, start, width);
}

std::optional<int> CardParser::getInt(size_t index) const {
    return StringUtils::parseInt(getField(index));
}

std::optional<int64_t> CardParser::getInt64(size_t index) const {
    return StringUtils::parseInt64(getField(index));
}

std::optional<double> CardParser::getDouble(size_t index) const {
    return StringUtils::parseDouble(getField(index));
}

std::string CardParser::getString(size_t index) const {
    return StringUtils::trim(getField(index));
}

int CardParser::getIntOr(size_t index, int defaultValue) const {
    return getInt(index).value_or(defaultValue);
}

int64_t CardParser::getInt64Or(size_t index, int64_t defaultValue) const {
    return getInt64(index).value_or(defaultValue);
}

double CardParser::getDoubleOr(size_t index, double defaultValue) const {
    return getDouble(index).value_or(defaultValue);
}

std::optional<int64_t> CardParser::getInt64At(size_t startPos) const {
    return StringUtils::parseInt64(getFieldAt(startPos, getIntWidth()));
}

std::optional<double> CardParser::getDoubleAt(size_t startPos, size_t width) const {
    return StringUtils::parseDouble(getFieldAt(startPos, width));
}

bool CardParser::isFieldEmpty(size_t index) const {
    std::string_view field = getField(index);
    return std::all_of(field.begin(), field.end(), [](unsigned char c) {
        return std::isspace(c);
    });
}

bool CardParser::isFieldEmptyAt(size_t startPos, size_t width) const {
    std::string_view field = getFieldAt(startPos, width);
    return std::all_of(field.begin(), field.end(), [](unsigned char c) {
        return std::isspace(c);
    });
}

bool CardParser::isKeywordLine(std::string_view line) {
    std::string trimmed = StringUtils::trimLeft(line);
    return !trimmed.empty() && trimmed[0] == '*';
}

std::string CardParser::extractKeyword(std::string_view line) {
    std::string trimmed = StringUtils::trim(line);
    if (trimmed.empty() || trimmed[0] != '*') {
        return "";
    }

    // Find end of keyword (space, tab, or $)
    size_t end = trimmed.find_first_of(" \t$");
    if (end == std::string::npos) {
        end = trimmed.length();
    }

    return StringUtils::toUpper(trimmed.substr(0, end));
}

bool CardParser::isCommentLine(std::string_view line) {
    std::string trimmed = StringUtils::trimLeft(line);
    return trimmed.empty() || trimmed[0] == '$';
}

bool CardParser::isLargeFormat(std::string_view keyword) {
    // Large format keywords end with '+'
    return !keyword.empty() && keyword.back() == '+';
}

// CardWriter implementation
//
// LS-DYNA LONG=S format specification:
// - Integer fields remain 10 characters wide
// - Real (floating-point) fields expand to 20 characters
// - Each keyword defines its own card structure; no automatic line wrapping

CardWriter::CardWriter(CardParser::Format format)
    : format_(format) {}

void CardWriter::newLine() {
    if (!currentLine_.empty()) {
        finishCurrentLine();
    }
}

void CardWriter::writeInt(int64_t value) {
    // Integers always use 10-character width regardless of format
    size_t width = getIntWidth();
    std::ostringstream oss;
    oss << std::setw(static_cast<int>(width)) << value;
    std::string field = oss.str();

    // Truncate if necessary (keep rightmost digits)
    if (field.length() > width) {
        field = field.substr(field.length() - width);
    }

    currentLine_ += field;
}

void CardWriter::writeDouble(double value) {
    // Real fields use 10 chars in standard, 20 chars in Large format
    currentLine_ += StringUtils::formatDouble(value, getRealWidth());
}

void CardWriter::writeString(std::string_view value) {
    // Strings use integer width (10 chars) - typically for string fields in LS-DYNA
    size_t width = getIntWidth();
    std::string field(value);

    // Pad or truncate
    if (field.length() < width) {
        field.resize(width, ' ');
    } else if (field.length() > width) {
        field = field.substr(0, width);
    }

    currentLine_ += field;
}

void CardWriter::writeBlank(bool isReal) {
    size_t width = isReal ? getRealWidth() : getIntWidth();
    currentLine_ += std::string(width, ' ');
}

std::string CardWriter::getLine() const {
    return currentLine_;
}

std::vector<std::string> CardWriter::getLines() const {
    std::vector<std::string> result = lines_;
    if (!currentLine_.empty()) {
        result.push_back(currentLine_);
    }
    return result;
}

void CardWriter::clear() {
    lines_.clear();
    currentLine_.clear();
}

void CardWriter::finishCurrentLine() {
    lines_.push_back(currentLine_);
    currentLine_.clear();
}

} // namespace koo::util
