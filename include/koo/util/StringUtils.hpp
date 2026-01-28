#pragma once

#include <koo/Export.hpp>
#include <string>
#include <string_view>
#include <vector>
#include <optional>

namespace koo::util {

class KOO_API StringUtils {
public:
    // Trim whitespace
    static std::string trim(std::string_view str);
    static std::string trimLeft(std::string_view str);
    static std::string trimRight(std::string_view str);

    // Case conversion
    static std::string toUpper(std::string_view str);
    static std::string toLower(std::string_view str);

    // Split string
    static std::vector<std::string> split(std::string_view str, char delimiter);
    static std::vector<std::string> split(std::string_view str, std::string_view delimiter);

    // Fixed-width field parsing (LS-DYNA card format)
    static std::string_view getField(std::string_view line, size_t start, size_t width);

    // Number parsing
    static std::optional<int> parseInt(std::string_view str);
    static std::optional<int64_t> parseInt64(std::string_view str);
    static std::optional<double> parseDouble(std::string_view str);

    // Number formatting (fixed width for LS-DYNA)
    static std::string formatInt(int64_t value, size_t width);
    static std::string formatDouble(double value, size_t width, int precision = -1);

    // String checks
    static bool startsWith(std::string_view str, std::string_view prefix);
    static bool endsWith(std::string_view str, std::string_view suffix);
    static bool contains(std::string_view str, std::string_view substr);

    // Replace
    static std::string replace(std::string_view str, std::string_view from, std::string_view to);
    static std::string replaceAll(std::string_view str, std::string_view from, std::string_view to);
};

} // namespace koo::util
