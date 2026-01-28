#pragma once

#include <koo/Export.hpp>
#include <koo/util/Types.hpp>
#include <string>
#include <string_view>
#include <vector>
#include <optional>

namespace koo::util {

/**
 * @brief LS-DYNA card format parser
 *
 * LS-DYNA uses fixed-width card formats:
 * - Standard: 10 columns per field
 * - Large (LONG=S): Integer fields stay 10 columns, Real fields expand to 20 columns
 *
 * IMPORTANT: In LONG=S format, field positions depend on the data types of preceding
 * fields. Use getFieldAt() with explicit position, or use the field layout methods
 * that account for mixed int/real fields.
 */
class KOO_API CardParser {
public:
    enum class Format {
        Standard,  // All fields 10 columns
        Large      // LONG=S: integers 10 cols, reals 20 cols
    };

    explicit CardParser(Format format = Format::Standard);

    // Set the current line to parse
    void setLine(std::string_view line);

    // Get field at specific character position and width
    std::string_view getFieldAt(size_t startPos, size_t width) const;

    // Legacy index-based access (assumes uniform 10-column fields)
    // Use only for standard format or when all fields are integers
    std::string_view getField(size_t index) const;

    // Get field as specific type (index-based, assumes uniform 10-col fields)
    std::optional<int> getInt(size_t index) const;
    std::optional<int64_t> getInt64(size_t index) const;
    std::optional<double> getDouble(size_t index) const;
    std::string getString(size_t index) const;

    // Get with default value
    int getIntOr(size_t index, int defaultValue) const;
    int64_t getInt64Or(size_t index, int64_t defaultValue) const;
    double getDoubleOr(size_t index, double defaultValue) const;

    // Position-based access for LONG=S format
    std::optional<int64_t> getInt64At(size_t startPos) const;
    std::optional<double> getDoubleAt(size_t startPos, size_t width) const;

    // Format
    Format getFormat() const { return format_; }
    void setFormat(Format format) { format_ = format; }
    size_t getIntWidth() const { return 10; }  // Always 10 for integers
    size_t getRealWidth() const { return format_ == Format::Large ? 20 : 10; }

    // Check if field is empty
    bool isFieldEmpty(size_t index) const;
    bool isFieldEmptyAt(size_t startPos, size_t width) const;

    // Parse keyword line (starts with *)
    static bool isKeywordLine(std::string_view line);
    static std::string extractKeyword(std::string_view line);
    static bool isCommentLine(std::string_view line);
    static bool isLargeFormat(std::string_view keyword);

private:
    Format format_;
    std::string_view line_;
};

/**
 * @brief Card writer for LS-DYNA format output
 *
 * LS-DYNA LONG=S format:
 * - Integer fields: 10 characters (same as standard)
 * - Real fields: 20 characters (doubled from standard 10)
 * - No field-per-line limit; each keyword defines its own line structure
 */
class KOO_API CardWriter {
public:
    explicit CardWriter(CardParser::Format format = CardParser::Format::Standard);

    // Start a new line
    void newLine();

    // Write fields
    void writeInt(int64_t value);
    void writeDouble(double value);
    void writeString(std::string_view value);
    void writeBlank(bool isReal = false);

    // Get current line
    std::string getLine() const;

    // Get all lines
    std::vector<std::string> getLines() const;

    // Clear
    void clear();

    // Format settings
    void setFormat(CardParser::Format format) { format_ = format; }
    CardParser::Format getFormat() const { return format_; }

private:
    void finishCurrentLine();
    // In LONG=S format: integers stay 10 chars, reals become 20 chars
    size_t getIntWidth() const { return 10; }  // Always 10 for integers
    size_t getRealWidth() const { return format_ == CardParser::Format::Large ? 20 : 10; }

    CardParser::Format format_;
    std::vector<std::string> lines_;
    std::string currentLine_;
};

} // namespace koo::util
