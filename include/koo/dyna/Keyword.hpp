#pragma once

#include <koo/Export.hpp>
#include <koo/util/CardParser.hpp>
#include <memory>
#include <string>
#include <vector>
#include <functional>

namespace koo::dyna {

// Forward declaration
class ModelVisitor;

/**
 * @brief Base class for all LS-DYNA keywords
 *
 * All keywords inherit from this class and implement:
 * - clone(): Prototype pattern for deep copy
 * - parse(): Parse from card lines
 * - write(): Write to card format
 * - accept(): Visitor pattern for traversal
 */
class KOO_API Keyword {
public:
    virtual ~Keyword() = default;

    // Prototype pattern - deep copy
    virtual std::unique_ptr<Keyword> clone() const = 0;

    // Keyword name (e.g., "*NODE", "*ELEMENT_SHELL")
    virtual std::string getKeywordName() const = 0;

    // Parse from card lines (excluding keyword line)
    virtual bool parse(const std::vector<std::string>& lines,
                       util::CardParser::Format format = util::CardParser::Format::Standard) = 0;

    // Write to card format
    virtual std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const = 0;

    // Visitor pattern
    virtual void accept(ModelVisitor& visitor) = 0;

    // Comment associated with this keyword
    void setComment(const std::string& comment) { comment_ = comment; }
    const std::string& getComment() const { return comment_; }

protected:
    std::string comment_;
};

/**
 * @brief CRTP helper for automatic clone() implementation
 *
 * Usage:
 * class MatElastic : public CloneableKeyword<MatElastic, Material> {
 *     // clone() is automatically implemented
 * };
 */
template<typename Derived, typename Base = Keyword>
class CloneableKeyword : public Base {
public:
    std::unique_ptr<Keyword> clone() const override {
        return std::make_unique<Derived>(static_cast<const Derived&>(*this));
    }
};

/**
 * @brief Generic keyword for unknown/unsupported keywords
 *
 * Stores raw card lines for round-trip preservation
 */
class KOO_API GenericKeyword : public CloneableKeyword<GenericKeyword> {
public:
    GenericKeyword() = default;
    explicit GenericKeyword(const std::string& keywordName);

    std::string getKeywordName() const override { return keywordName_; }
    void setKeywordName(const std::string& name) { keywordName_ = name; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    // Raw line access
    const std::vector<std::string>& getRawLines() const { return rawLines_; }
    void setRawLines(const std::vector<std::string>& lines) { rawLines_ = lines; }

private:
    std::string keywordName_;
    std::vector<std::string> rawLines_;
    util::CardParser::Format format_ = util::CardParser::Format::Standard;
};

} // namespace koo::dyna
