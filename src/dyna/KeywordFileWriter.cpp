#include <koo/dyna/KeywordFileWriter.hpp>
#include <fstream>
#include <sstream>

namespace koo::dyna {

KeywordFileWriter::KeywordFileWriter() = default;

KeywordFileWriter::KeywordFileWriter(const WriterOptions& options)
    : options_(options) {}

bool KeywordFileWriter::write(const Model& model,
                              const std::filesystem::path& filepath) {
    error_.clear();

    std::ofstream file(filepath);
    if (!file.is_open()) {
        error_ = "Cannot open file for writing: " + filepath.string();
        return false;
    }

    write(model, file);
    return !hasError();
}

std::string KeywordFileWriter::writeToString(const Model& model) {
    error_.clear();
    std::ostringstream stream;
    write(model, stream);
    return stream.str();
}

void KeywordFileWriter::write(const Model& model, std::ostream& stream) {
    const auto& keywords = model.getKeywords();
    size_t total = keywords.size();
    size_t current = 0;

    // Write *KEYWORD directive
    if (options_.writeKeywordDirective) {
        if (options_.format == util::CardParser::Format::Large) {
            writeLine("*KEYWORD LONG=S", stream);
        } else {
            writeLine("*KEYWORD", stream);
        }
    }

    // Write *TITLE
    if (options_.writeTitle && !model.getTitle().empty()) {
        writeLine("*TITLE", stream);
        writeLine(model.getTitle(), stream);
    }

    // Write all keywords
    for (const auto& keyword : keywords) {
        writeKeyword(*keyword, stream);

        current++;
        if (options_.progressCallback) {
            options_.progressCallback(static_cast<float>(current) /
                                      static_cast<float>(total));
        }
    }

    // Write *END
    if (options_.writeEnd) {
        writeLine("*END", stream);
    }
}

void KeywordFileWriter::writeKeyword(const Keyword& keyword,
                                     std::ostream& stream) {
    // Write keyword name
    std::string keywordName = keyword.getKeywordName();
    if (options_.format == util::CardParser::Format::Large) {
        keywordName += "+";
    }
    writeLine(keywordName, stream);

    // Write comment if present
    if (!keyword.getComment().empty()) {
        writeLine("$ " + keyword.getComment(), stream);
    }

    // Write keyword data
    auto lines = keyword.write(options_.format);
    for (const auto& line : lines) {
        writeLine(line, stream);
    }
}

void KeywordFileWriter::writeLine(const std::string& line,
                                  std::ostream& stream) {
    stream << line << options_.lineEnding;
}

} // namespace koo::dyna
