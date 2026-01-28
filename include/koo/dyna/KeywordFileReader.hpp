#pragma once

#include <koo/Export.hpp>
#include <koo/dyna/Model.hpp>
#include <koo/util/CardParser.hpp>
#include <filesystem>
#include <string>
#include <vector>
#include <functional>

namespace koo::dyna {

/**
 * @brief Parse options for K-file reading
 */
struct KOO_API ReaderOptions {
    // Follow *INCLUDE directives
    bool followIncludes = true;

    // Base directory for relative include paths
    std::filesystem::path baseDirectory;

    // Default card format (Standard or Large)
    util::CardParser::Format defaultFormat = util::CardParser::Format::Standard;

    // Progress callback (0.0 to 1.0)
    std::function<void(float progress)> progressCallback;

    // Error handling
    bool stopOnError = false;
    std::function<void(const std::string& error)> errorCallback;
};

/**
 * @brief LS-DYNA keyword file reader
 *
 * Parses K-files and returns a Model object containing all keywords.
 */
class KOO_API KeywordFileReader {
public:
    KeywordFileReader();
    explicit KeywordFileReader(const ReaderOptions& options);

    // Read from file
    Model read(const std::filesystem::path& filepath);

    // Read from string
    Model readFromString(const std::string& content,
                         const std::filesystem::path& basePath = "");

    // Options access
    ReaderOptions& options() { return options_; }
    const ReaderOptions& options() const { return options_; }

    // Error information
    const std::vector<std::string>& getErrors() const { return errors_; }
    const std::vector<std::string>& getWarnings() const { return warnings_; }
    bool hasErrors() const { return !errors_.empty(); }

private:
    // Parse a single file
    void parseFile(const std::filesystem::path& filepath, Model& model);

    // Parse content lines
    void parseContent(const std::vector<std::string>& lines,
                      const std::filesystem::path& basePath,
                      Model& model);

    // Parse a keyword block
    void parseKeywordBlock(const std::string& keywordName,
                           const std::vector<std::string>& lines,
                           util::CardParser::Format format,
                           Model& model);

    // Handle *INCLUDE
    void handleInclude(const std::vector<std::string>& lines,
                       const std::filesystem::path& basePath,
                       Model& model);

    // Handle *KEYWORD
    void handleKeywordDirective(const std::string& line);

    // Report error/warning
    void reportError(const std::string& message);
    void reportWarning(const std::string& message);

    ReaderOptions options_;
    std::vector<std::string> errors_;
    std::vector<std::string> warnings_;
    util::CardParser::Format currentFormat_;
    size_t currentLine_ = 0;
    std::filesystem::path currentFile_;
};

} // namespace koo::dyna
