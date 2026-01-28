#include <koo/dyna/KeywordFileReader.hpp>
#include <koo/dyna/KeywordFactory.hpp>
#include <koo/util/StringUtils.hpp>
#include <fstream>
#include <sstream>
#include <algorithm>

namespace koo::dyna {

KeywordFileReader::KeywordFileReader()
    : currentFormat_(util::CardParser::Format::Standard) {}

KeywordFileReader::KeywordFileReader(const ReaderOptions& options)
    : options_(options)
    , currentFormat_(options.defaultFormat) {}

Model KeywordFileReader::read(const std::filesystem::path& filepath) {
    errors_.clear();
    warnings_.clear();
    currentFormat_ = options_.defaultFormat;

    Model model;
    model.setFilePath(filepath);

    if (options_.baseDirectory.empty()) {
        options_.baseDirectory = filepath.parent_path();
    }

    parseFile(filepath, model);

    return model;
}

Model KeywordFileReader::readFromString(const std::string& content,
                                        const std::filesystem::path& basePath) {
    errors_.clear();
    warnings_.clear();
    currentFormat_ = options_.defaultFormat;

    Model model;

    if (!basePath.empty()) {
        options_.baseDirectory = basePath;
    }

    // Split content into lines
    std::vector<std::string> lines;
    std::istringstream stream(content);
    std::string line;
    while (std::getline(stream, line)) {
        // Handle Windows line endings
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        lines.push_back(line);
    }

    parseContent(lines, basePath, model);

    return model;
}

void KeywordFileReader::parseFile(const std::filesystem::path& filepath,
                                  Model& model) {
    currentFile_ = filepath;
    currentLine_ = 0;

    std::ifstream file(filepath);
    if (!file.is_open()) {
        reportError("Cannot open file: " + filepath.string());
        return;
    }

    std::vector<std::string> lines;
    std::string line;
    while (std::getline(file, line)) {
        // Handle Windows line endings
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        lines.push_back(line);
    }

    parseContent(lines, filepath.parent_path(), model);
}

void KeywordFileReader::parseContent(const std::vector<std::string>& lines,
                                     const std::filesystem::path& basePath,
                                     Model& model) {
    std::string currentKeyword;
    std::vector<std::string> currentBlock;
    util::CardParser::Format blockFormat = currentFormat_;

    auto finishBlock = [&]() {
        if (!currentKeyword.empty() && !currentBlock.empty()) {
            parseKeywordBlock(currentKeyword, currentBlock, blockFormat, model);
        }
        currentKeyword.clear();
        currentBlock.clear();
    };

    for (size_t i = 0; i < lines.size(); ++i) {
        currentLine_ = i + 1;
        const std::string& line = lines[i];

        // Skip empty lines within blocks
        if (line.empty()) {
            if (!currentKeyword.empty()) {
                currentBlock.push_back(line);
            }
            continue;
        }

        // Check for keyword line
        if (util::CardParser::isKeywordLine(line)) {
            // Finish previous block
            finishBlock();

            std::string keyword = util::CardParser::extractKeyword(line);

            // Handle special directives
            if (keyword == "*KEYWORD") {
                handleKeywordDirective(line);
                continue;
            }

            if (keyword == "*END") {
                // End of file marker
                break;
            }

            if (keyword == "*INCLUDE" || keyword == "*INCLUDE_PATH" ||
                keyword == "*INCLUDE_TRANSFORM") {
                // Collect include block
                currentKeyword = keyword;
                blockFormat = currentFormat_;
                continue;
            }

            if (keyword == "*TITLE") {
                // Next line is the title
                if (i + 1 < lines.size()) {
                    model.setTitle(util::StringUtils::trim(lines[i + 1]));
                    ++i;
                }
                continue;
            }

            // Check for large format indicator
            if (util::CardParser::isLargeFormat(keyword)) {
                blockFormat = util::CardParser::Format::Large;
                // Remove the trailing '+'
                keyword = keyword.substr(0, keyword.length() - 1);
            } else {
                blockFormat = currentFormat_;
            }

            currentKeyword = keyword;
            continue;
        }

        // Skip comment lines at file level
        if (currentKeyword.empty() && util::CardParser::isCommentLine(line)) {
            continue;
        }

        // Add line to current block
        if (!currentKeyword.empty()) {
            currentBlock.push_back(line);
        }
    }

    // Finish last block
    finishBlock();
}

void KeywordFileReader::parseKeywordBlock(const std::string& keywordName,
                                          const std::vector<std::string>& lines,
                                          util::CardParser::Format format,
                                          Model& model) {
    // Handle *INCLUDE specially
    if (keywordName == "*INCLUDE" || keywordName == "*INCLUDE_PATH" ||
        keywordName == "*INCLUDE_TRANSFORM") {
        if (options_.followIncludes) {
            handleInclude(lines, options_.baseDirectory, model);
        }
        return;
    }

    // Create keyword using factory
    auto keyword = KeywordFactory::instance().create(keywordName);
    if (!keyword) {
        reportWarning("Unknown keyword: " + keywordName);
        return;
    }

    // Parse the keyword data
    if (!keyword->parse(lines, format)) {
        reportWarning("Failed to parse keyword: " + keywordName);
    }

    model.addKeyword(std::move(keyword));
}

void KeywordFileReader::handleInclude(const std::vector<std::string>& lines,
                                      const std::filesystem::path& basePath,
                                      Model& model) {
    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) {
            continue;
        }

        std::string filename = util::StringUtils::trim(line);
        if (filename.empty()) {
            continue;
        }

        // Resolve path
        std::filesystem::path includePath = filename;
        if (includePath.is_relative()) {
            includePath = basePath / includePath;
        }

        // Parse included file
        if (std::filesystem::exists(includePath)) {
            parseFile(includePath, model);
        } else {
            reportWarning("Include file not found: " + includePath.string());
        }
    }
}

void KeywordFileReader::handleKeywordDirective(const std::string& line) {
    // Check for memory/format options
    std::string upper = util::StringUtils::toUpper(line);

    if (util::StringUtils::contains(upper, "LONG=Y") ||
        util::StringUtils::contains(upper, "LONG=S")) {
        currentFormat_ = util::CardParser::Format::Large;
    } else if (util::StringUtils::contains(upper, "LONG=N")) {
        currentFormat_ = util::CardParser::Format::Standard;
    }
}

void KeywordFileReader::reportError(const std::string& message) {
    std::string fullMessage = message;
    if (!currentFile_.empty()) {
        fullMessage = currentFile_.string() + ":" +
                      std::to_string(currentLine_) + ": " + message;
    }
    errors_.push_back(fullMessage);

    if (options_.errorCallback) {
        options_.errorCallback(fullMessage);
    }
}

void KeywordFileReader::reportWarning(const std::string& message) {
    std::string fullMessage = message;
    if (!currentFile_.empty()) {
        fullMessage = currentFile_.string() + ":" +
                      std::to_string(currentLine_) + ": " + message;
    }
    warnings_.push_back(fullMessage);
}

} // namespace koo::dyna
