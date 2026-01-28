#include <koo/util/StringUtils.hpp>
#include <algorithm>
#include <cctype>
#include <charconv>
#include <sstream>
#include <iomanip>
#include <cmath>

namespace koo::util {

std::string StringUtils::trim(std::string_view str) {
    return trimRight(trimLeft(str));
}

std::string StringUtils::trimLeft(std::string_view str) {
    auto it = std::find_if(str.begin(), str.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    });
    return std::string(it, str.end());
}

std::string StringUtils::trimRight(std::string_view str) {
    auto it = std::find_if(str.rbegin(), str.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    });
    return std::string(str.begin(), it.base());
}

std::string StringUtils::toUpper(std::string_view str) {
    std::string result(str);
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return static_cast<char>(std::toupper(c)); });
    return result;
}

std::string StringUtils::toLower(std::string_view str) {
    std::string result(str);
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return result;
}

std::vector<std::string> StringUtils::split(std::string_view str, char delimiter) {
    std::vector<std::string> result;
    size_t start = 0;
    size_t end = str.find(delimiter);

    while (end != std::string_view::npos) {
        result.emplace_back(str.substr(start, end - start));
        start = end + 1;
        end = str.find(delimiter, start);
    }
    result.emplace_back(str.substr(start));
    return result;
}

std::vector<std::string> StringUtils::split(std::string_view str, std::string_view delimiter) {
    std::vector<std::string> result;
    size_t start = 0;
    size_t end = str.find(delimiter);

    while (end != std::string_view::npos) {
        result.emplace_back(str.substr(start, end - start));
        start = end + delimiter.length();
        end = str.find(delimiter, start);
    }
    result.emplace_back(str.substr(start));
    return result;
}

std::string_view StringUtils::getField(std::string_view line, size_t start, size_t width) {
    if (start >= line.length()) {
        return {};
    }
    size_t actualWidth = std::min(width, line.length() - start);
    return line.substr(start, actualWidth);
}

std::optional<int> StringUtils::parseInt(std::string_view str) {
    str = trim(str);
    if (str.empty()) {
        return std::nullopt;
    }

    int value = 0;
    auto result = std::from_chars(str.data(), str.data() + str.size(), value);
    if (result.ec == std::errc() && result.ptr == str.data() + str.size()) {
        return value;
    }
    return std::nullopt;
}

std::optional<int64_t> StringUtils::parseInt64(std::string_view str) {
    str = trim(str);
    if (str.empty()) {
        return std::nullopt;
    }

    int64_t value = 0;
    auto result = std::from_chars(str.data(), str.data() + str.size(), value);
    if (result.ec == std::errc() && result.ptr == str.data() + str.size()) {
        return value;
    }
    return std::nullopt;
}

std::optional<double> StringUtils::parseDouble(std::string_view str) {
    std::string trimmed = trim(str);
    if (trimmed.empty()) {
        return std::nullopt;
    }

    // Handle LS-DYNA's exponential format (e.g., "1.0-5" means 1.0e-5)
    // Insert 'e' before +/- if not preceded by 'e' or 'E'
    std::string normalized;
    normalized.reserve(trimmed.size() + 1);
    for (size_t i = 0; i < trimmed.size(); ++i) {
        char c = trimmed[i];
        if ((c == '+' || c == '-') && i > 0) {
            char prev = trimmed[i - 1];
            if (prev != 'e' && prev != 'E' && prev != 'd' && prev != 'D' &&
                std::isdigit(static_cast<unsigned char>(prev))) {
                normalized += 'e';
            }
        }
        // Convert 'd' to 'e' for Fortran-style exponents
        if (c == 'd' || c == 'D') {
            normalized += 'e';
        } else {
            normalized += c;
        }
    }

    try {
        size_t pos = 0;
        double value = std::stod(normalized, &pos);
        if (pos == normalized.size()) {
            return value;
        }
    } catch (...) {
        // Fall through to return nullopt
    }
    return std::nullopt;
}

std::string StringUtils::formatInt(int64_t value, size_t width) {
    std::ostringstream oss;
    oss << std::setw(static_cast<int>(width)) << value;
    return oss.str();
}

std::string StringUtils::formatDouble(double value, size_t width, int precision) {
    std::ostringstream oss;

    if (precision < 0) {
        // Auto precision based on width
        precision = static_cast<int>(width) - 7; // Account for sign, decimal, exponent
        if (precision < 1) precision = 1;
        if (precision > 10) precision = 10;
    }

    // Try fixed format first
    oss << std::setw(static_cast<int>(width)) << std::setprecision(precision) << std::fixed << value;
    std::string result = oss.str();

    // If too long, use scientific notation
    if (result.length() > width) {
        oss.str("");
        oss.clear();
        int sciPrecision = static_cast<int>(width) - 7;
        if (sciPrecision < 1) sciPrecision = 1;
        oss << std::setw(static_cast<int>(width)) << std::setprecision(sciPrecision)
            << std::scientific << value;
        result = oss.str();
    }

    // Truncate if still too long
    if (result.length() > width) {
        result = result.substr(0, width);
    }

    return result;
}

bool StringUtils::startsWith(std::string_view str, std::string_view prefix) {
    if (prefix.length() > str.length()) {
        return false;
    }
    return str.substr(0, prefix.length()) == prefix;
}

bool StringUtils::endsWith(std::string_view str, std::string_view suffix) {
    if (suffix.length() > str.length()) {
        return false;
    }
    return str.substr(str.length() - suffix.length()) == suffix;
}

bool StringUtils::contains(std::string_view str, std::string_view substr) {
    return str.find(substr) != std::string_view::npos;
}

std::string StringUtils::replace(std::string_view str, std::string_view from, std::string_view to) {
    std::string result(str);
    size_t pos = result.find(from);
    if (pos != std::string::npos) {
        result.replace(pos, from.length(), to);
    }
    return result;
}

std::string StringUtils::replaceAll(std::string_view str, std::string_view from, std::string_view to) {
    std::string result(str);
    size_t pos = 0;
    while ((pos = result.find(from, pos)) != std::string::npos) {
        result.replace(pos, from.length(), to);
        pos += to.length();
    }
    return result;
}

} // namespace koo::util
