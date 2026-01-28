#include <koo/dyna/Parameter.hpp>
#include <koo/dyna/ModelVisitor.hpp>
#include <koo/dyna/KeywordFactory.hpp>
#include <koo/util/StringUtils.hpp>
#include <sstream>

namespace koo::dyna {

// ============================================================================
// Parameter
// ============================================================================

bool Parameter::parse(const std::vector<std::string>& lines,
                       util::CardParser::Format format) {
    util::CardParser parser(format);

    for (size_t lineIdx = 0; lineIdx < lines.size(); ++lineIdx) {
        if (util::CardParser::isCommentLine(lines[lineIdx])) {
            continue;
        }

        // Parameter lines: PNAME, VALUE (can use R for real, I for int)
        const std::string& line = lines[lineIdx];
        if (line.empty()) continue;

        // Parse parameter: format is "Rname value" or "Iname value" or just "name value"
        std::string trimmed = util::StringUtils::trim(line);
        if (trimmed.empty()) continue;

        Entry entry;

        // Find the first space to separate name and value
        size_t spacePos = trimmed.find_first_of(" \t");
        if (spacePos != std::string::npos) {
            entry.name = util::StringUtils::trim(trimmed.substr(0, spacePos));
            std::string valueStr = util::StringUtils::trim(trimmed.substr(spacePos + 1));
            try {
                entry.value = std::stod(valueStr);
            } catch (...) {
                entry.value = 0.0;
            }
        } else {
            entry.name = trimmed;
            entry.value = 0.0;
        }

        if (!entry.name.empty()) {
            entries_.push_back(entry);
        }
    }

    return true;
}

std::vector<std::string> Parameter::write(util::CardParser::Format /*format*/) const {
    std::vector<std::string> result;

    for (const auto& entry : entries_) {
        std::ostringstream oss;
        oss << entry.name << " " << entry.value;
        result.push_back(oss.str());
    }

    return result;
}

void Parameter::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(Parameter, "*PARAMETER")

// ============================================================================
// ParameterExpression
// ============================================================================

bool ParameterExpression::parse(const std::vector<std::string>& lines,
                                 util::CardParser::Format format) {
    util::CardParser parser(format);

    for (size_t lineIdx = 0; lineIdx < lines.size(); ++lineIdx) {
        if (util::CardParser::isCommentLine(lines[lineIdx])) {
            continue;
        }

        const std::string& line = lines[lineIdx];
        if (line.empty()) continue;

        std::string trimmed = util::StringUtils::trim(line);
        if (trimmed.empty()) continue;

        Entry entry;

        // Find the first space to separate name and expression
        size_t spacePos = trimmed.find_first_of(" \t");
        if (spacePos != std::string::npos) {
            entry.name = util::StringUtils::trim(trimmed.substr(0, spacePos));
            entry.expression = util::StringUtils::trim(trimmed.substr(spacePos + 1));
        } else {
            entry.name = trimmed;
            entry.expression = "";
        }

        if (!entry.name.empty()) {
            entries_.push_back(entry);
        }
    }

    return true;
}

std::vector<std::string> ParameterExpression::write(util::CardParser::Format /*format*/) const {
    std::vector<std::string> result;

    for (const auto& entry : entries_) {
        result.push_back(entry.name + " " + entry.expression);
    }

    return result;
}

void ParameterExpression::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ParameterExpression, "*PARAMETER_EXPRESSION")

// ============================================================================
// ParameterLocal
// ============================================================================

bool ParameterLocal::parse(const std::vector<std::string>& lines,
                            util::CardParser::Format format) {
    util::CardParser parser(format);

    for (size_t lineIdx = 0; lineIdx < lines.size(); ++lineIdx) {
        if (util::CardParser::isCommentLine(lines[lineIdx])) {
            continue;
        }

        const std::string& line = lines[lineIdx];
        if (line.empty()) continue;

        std::string trimmed = util::StringUtils::trim(line);
        if (trimmed.empty()) continue;

        Entry entry;

        size_t spacePos = trimmed.find_first_of(" \t");
        if (spacePos != std::string::npos) {
            entry.name = util::StringUtils::trim(trimmed.substr(0, spacePos));
            std::string valueStr = util::StringUtils::trim(trimmed.substr(spacePos + 1));
            try {
                entry.value = std::stod(valueStr);
            } catch (...) {
                entry.value = 0.0;
            }
        } else {
            entry.name = trimmed;
            entry.value = 0.0;
        }

        if (!entry.name.empty()) {
            entries_.push_back(entry);
        }
    }

    return true;
}

std::vector<std::string> ParameterLocal::write(util::CardParser::Format /*format*/) const {
    std::vector<std::string> result;

    for (const auto& entry : entries_) {
        std::ostringstream oss;
        oss << entry.name << " " << entry.value;
        result.push_back(oss.str());
    }

    return result;
}

void ParameterLocal::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ParameterLocal, "*PARAMETER_LOCAL")

// ============================================================================
// ParameterDuplication
// ============================================================================

bool ParameterDuplication::parse(const std::vector<std::string>& lines,
                                  util::CardParser::Format format) {
    util::CardParser parser(format);

    size_t lineIdx = 0;

    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: OPTION
    parser.setLine(lines[lineIdx]);
    data_.option = static_cast<int>(parser.getInt64At(0).value_or(0));

    return true;
}

std::vector<std::string> ParameterDuplication::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.option);
    result.push_back(writer.getLine());

    return result;
}

void ParameterDuplication::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ParameterDuplication, "*PARAMETER_DUPLICATION")

// ============================================================================
// ParameterType
// ============================================================================

bool ParameterType::parse(const std::vector<std::string>& lines,
                           util::CardParser::Format format) {
    util::CardParser parser(format);

    for (size_t lineIdx = 0; lineIdx < lines.size(); ++lineIdx) {
        if (util::CardParser::isCommentLine(lines[lineIdx])) {
            continue;
        }

        const std::string& line = lines[lineIdx];
        if (line.empty()) continue;

        std::string trimmed = util::StringUtils::trim(line);
        if (trimmed.empty()) continue;

        Entry entry;

        // Parse: TYPE NAME VALUE
        std::istringstream iss(trimmed);
        std::string typeStr;
        iss >> typeStr >> entry.name >> entry.value;

        if (typeStr == "R" || typeStr == "r") {
            entry.type = 1; // Real
        } else if (typeStr == "I" || typeStr == "i") {
            entry.type = 0; // Integer
        } else if (typeStr == "C" || typeStr == "c") {
            entry.type = 2; // String
        } else {
            entry.name = typeStr;
            entry.type = 1; // Default to real
        }

        if (!entry.name.empty()) {
            entries_.push_back(entry);
        }
    }

    return true;
}

std::vector<std::string> ParameterType::write(util::CardParser::Format /*format*/) const {
    std::vector<std::string> result;

    for (const auto& entry : entries_) {
        std::ostringstream oss;
        if (entry.type == 0) {
            oss << "I ";
        } else if (entry.type == 2) {
            oss << "C ";
        } else {
            oss << "R ";
        }
        oss << entry.name << " " << entry.value;
        result.push_back(oss.str());
    }

    return result;
}

void ParameterType::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ParameterType, "*PARAMETER_TYPE")

// ============================================================================
// ParameterGlobal
// ============================================================================

bool ParameterGlobal::parse(const std::vector<std::string>& lines,
                             util::CardParser::Format format) {
    util::CardParser parser(format);

    for (size_t lineIdx = 0; lineIdx < lines.size(); ++lineIdx) {
        if (util::CardParser::isCommentLine(lines[lineIdx])) {
            continue;
        }

        const std::string& line = lines[lineIdx];
        if (line.empty()) continue;

        std::string trimmed = util::StringUtils::trim(line);
        if (trimmed.empty()) continue;

        Entry entry;

        size_t spacePos = trimmed.find_first_of(" \t");
        if (spacePos != std::string::npos) {
            entry.name = util::StringUtils::trim(trimmed.substr(0, spacePos));
            std::string valueStr = util::StringUtils::trim(trimmed.substr(spacePos + 1));
            try {
                entry.value = std::stod(valueStr);
            } catch (...) {
                entry.value = 0.0;
            }
        } else {
            entry.name = trimmed;
            entry.value = 0.0;
        }

        if (!entry.name.empty()) {
            entries_.push_back(entry);
        }
    }

    return true;
}

std::vector<std::string> ParameterGlobal::write(util::CardParser::Format /*format*/) const {
    std::vector<std::string> result;

    for (const auto& entry : entries_) {
        std::ostringstream oss;
        oss << entry.name << " " << entry.value;
        result.push_back(oss.str());
    }

    return result;
}

void ParameterGlobal::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ParameterGlobal, "*PARAMETER_GLOBAL")

// ============================================================================
// ParameterExpressionLocal
// ============================================================================

bool ParameterExpressionLocal::parse(const std::vector<std::string>& lines,
                                      util::CardParser::Format format) {
    util::CardParser parser(format);

    for (size_t lineIdx = 0; lineIdx < lines.size(); ++lineIdx) {
        if (util::CardParser::isCommentLine(lines[lineIdx])) {
            continue;
        }

        const std::string& line = lines[lineIdx];
        if (line.empty()) continue;

        std::string trimmed = util::StringUtils::trim(line);
        if (trimmed.empty()) continue;

        Entry entry;

        size_t spacePos = trimmed.find_first_of(" \t");
        if (spacePos != std::string::npos) {
            entry.name = util::StringUtils::trim(trimmed.substr(0, spacePos));
            entry.expression = util::StringUtils::trim(trimmed.substr(spacePos + 1));
        } else {
            entry.name = trimmed;
            entry.expression = "";
        }

        if (!entry.name.empty()) {
            entries_.push_back(entry);
        }
    }

    return true;
}

std::vector<std::string> ParameterExpressionLocal::write(util::CardParser::Format /*format*/) const {
    std::vector<std::string> result;

    for (const auto& entry : entries_) {
        result.push_back(entry.name + " " + entry.expression);
    }

    return result;
}

void ParameterExpressionLocal::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(ParameterExpressionLocal, "*PARAMETER_EXPRESSION_LOCAL")

} // namespace koo::dyna
