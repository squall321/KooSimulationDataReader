#pragma once

#include <koo/Export.hpp>
#include <koo/dyna/Keyword.hpp>
#include <koo/util/Types.hpp>
#include <vector>
#include <string>

namespace koo::dyna {

/**
 * @brief Base class for *PARAMETER keywords
 */
class KOO_API ParameterKeyword : public Keyword {
public:
    virtual ~ParameterKeyword() = default;
};

/**
 * @brief *PARAMETER
 *
 * Defines a parameter value.
 */
class KOO_API Parameter : public CloneableKeyword<Parameter, ParameterKeyword> {
public:
    struct Entry {
        std::string name;     // Parameter name
        double value = 0.0;   // Parameter value
    };

    Parameter() = default;

    std::string getKeywordName() const override { return "*PARAMETER"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    std::vector<Entry>& getEntries() { return entries_; }
    const std::vector<Entry>& getEntries() const { return entries_; }

private:
    std::vector<Entry> entries_;
};

/**
 * @brief *PARAMETER_EXPRESSION
 *
 * Defines a parameter using an expression.
 */
class KOO_API ParameterExpression : public CloneableKeyword<ParameterExpression, ParameterKeyword> {
public:
    struct Entry {
        std::string name;       // Parameter name
        std::string expression; // Expression string
    };

    ParameterExpression() = default;

    std::string getKeywordName() const override { return "*PARAMETER_EXPRESSION"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    std::vector<Entry>& getEntries() { return entries_; }
    const std::vector<Entry>& getEntries() const { return entries_; }

private:
    std::vector<Entry> entries_;
};

/**
 * @brief *PARAMETER_LOCAL
 *
 * Defines a local parameter.
 */
class KOO_API ParameterLocal : public CloneableKeyword<ParameterLocal, ParameterKeyword> {
public:
    struct Entry {
        std::string name;     // Parameter name
        double value = 0.0;   // Parameter value
    };

    ParameterLocal() = default;

    std::string getKeywordName() const override { return "*PARAMETER_LOCAL"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    std::vector<Entry>& getEntries() { return entries_; }
    const std::vector<Entry>& getEntries() const { return entries_; }

private:
    std::vector<Entry> entries_;
};

/**
 * @brief *PARAMETER_DUPLICATION
 *
 * Controls parameter duplication behavior.
 */
class KOO_API ParameterDuplication : public CloneableKeyword<ParameterDuplication, ParameterKeyword> {
public:
    struct Data {
        int option = 0;       // Duplication option (0=error, 1=warn, 2=ignore)
    };

    ParameterDuplication() = default;

    std::string getKeywordName() const override { return "*PARAMETER_DUPLICATION"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    Data& getData() { return data_; }
    const Data& getData() const { return data_; }

private:
    Data data_;
};

/**
 * @brief *PARAMETER_TYPE
 *
 * Defines parameter type.
 */
class KOO_API ParameterType : public CloneableKeyword<ParameterType, ParameterKeyword> {
public:
    struct Entry {
        std::string name;     // Parameter name
        int type = 0;         // Type (0=integer, 1=real, 2=string)
        double value = 0.0;   // Parameter value
    };

    ParameterType() = default;

    std::string getKeywordName() const override { return "*PARAMETER_TYPE"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    std::vector<Entry>& getEntries() { return entries_; }
    const std::vector<Entry>& getEntries() const { return entries_; }

private:
    std::vector<Entry> entries_;
};

/**
 * @brief *PARAMETER_GLOBAL
 *
 * Defines a global parameter.
 */
class KOO_API ParameterGlobal : public CloneableKeyword<ParameterGlobal, ParameterKeyword> {
public:
    struct Entry {
        std::string name;     // Parameter name
        double value = 0.0;   // Parameter value
    };

    ParameterGlobal() = default;

    std::string getKeywordName() const override { return "*PARAMETER_GLOBAL"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    std::vector<Entry>& getEntries() { return entries_; }
    const std::vector<Entry>& getEntries() const { return entries_; }

private:
    std::vector<Entry> entries_;
};

/**
 * @brief *PARAMETER_EXPRESSION_LOCAL
 *
 * Defines a local parameter using an expression.
 */
class KOO_API ParameterExpressionLocal : public CloneableKeyword<ParameterExpressionLocal, ParameterKeyword> {
public:
    struct Entry {
        std::string name;       // Parameter name
        std::string expression; // Expression string
    };

    ParameterExpressionLocal() = default;

    std::string getKeywordName() const override { return "*PARAMETER_EXPRESSION_LOCAL"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    std::vector<Entry>& getEntries() { return entries_; }
    const std::vector<Entry>& getEntries() const { return entries_; }

private:
    std::vector<Entry> entries_;
};

} // namespace koo::dyna
