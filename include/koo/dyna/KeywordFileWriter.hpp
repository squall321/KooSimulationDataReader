#pragma once

#include <koo/Export.hpp>
#include <koo/dyna/Model.hpp>
#include <koo/util/CardParser.hpp>
#include <filesystem>
#include <string>
#include <vector>
#include <ostream>
#include <functional>

namespace koo::dyna {

/**
 * @brief Write options for K-file output
 */
struct KOO_API WriterOptions {
    // Card format
    util::CardParser::Format format = util::CardParser::Format::Standard;

    // Write *KEYWORD directive at beginning
    bool writeKeywordDirective = true;

    // Write *TITLE
    bool writeTitle = true;

    // Write *END at end of file
    bool writeEnd = true;

    // Line ending ("\n" or "\r\n")
    std::string lineEnding = "\n";

    // Progress callback (0.0 to 1.0)
    std::function<void(float progress)> progressCallback;
};

/**
 * @brief LS-DYNA keyword file writer
 */
class KOO_API KeywordFileWriter {
public:
    KeywordFileWriter();
    explicit KeywordFileWriter(const WriterOptions& options);

    // Write to file
    bool write(const Model& model, const std::filesystem::path& filepath);

    // Write to string
    std::string writeToString(const Model& model);

    // Write to stream
    void write(const Model& model, std::ostream& stream);

    // Options access
    WriterOptions& options() { return options_; }
    const WriterOptions& options() const { return options_; }

    // Error information
    const std::string& getError() const { return error_; }
    bool hasError() const { return !error_.empty(); }

private:
    void writeKeyword(const Keyword& keyword, std::ostream& stream);
    void writeLine(const std::string& line, std::ostream& stream);

    WriterOptions options_;
    std::string error_;
};

} // namespace koo::dyna
