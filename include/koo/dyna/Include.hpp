#pragma once

#include <koo/Export.hpp>
#include <koo/dyna/Keyword.hpp>
#include <string>
#include <vector>

namespace koo::dyna {

/**
 * @brief *INCLUDE
 *
 * Includes another keyword file. The included file is processed
 * as if its contents were part of the main file.
 */
class KOO_API Include : public CloneableKeyword<Include, Keyword> {
public:
    Include() = default;
    explicit Include(const std::string& filename) : filename_(filename) {}

    std::string getKeywordName() const override { return "*INCLUDE"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    // Filename access
    const std::string& getFilename() const { return filename_; }
    void setFilename(const std::string& filename) { filename_ = filename; }

private:
    std::string filename_;
};

/**
 * @brief *INCLUDE_PATH
 *
 * Specifies a search path for included files.
 */
class KOO_API IncludePath : public CloneableKeyword<IncludePath, Keyword> {
public:
    IncludePath() = default;
    explicit IncludePath(const std::string& path) : path_(path) {}

    std::string getKeywordName() const override { return "*INCLUDE_PATH"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    const std::string& getPath() const { return path_; }
    void setPath(const std::string& path) { path_ = path; }

private:
    std::string path_;
};

/**
 * @brief *INCLUDE_PATH_RELATIVE
 *
 * Specifies that include paths are relative to the current file.
 */
class KOO_API IncludePathRelative : public CloneableKeyword<IncludePathRelative, Keyword> {
public:
    IncludePathRelative() = default;
    explicit IncludePathRelative(const std::string& path) : path_(path) {}

    std::string getKeywordName() const override { return "*INCLUDE_PATH_RELATIVE"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    const std::string& getPath() const { return path_; }
    void setPath(const std::string& path) { path_ = path; }

private:
    std::string path_;
};

/**
 * @brief *INCLUDE_TRANSFORM
 *
 * Includes a file with coordinate transformation applied.
 */
class KOO_API IncludeTransform : public CloneableKeyword<IncludeTransform, Keyword> {
public:
    struct TransformData {
        std::string filename;
        int idnoff = 0;         // Node ID offset
        int ideoff = 0;         // Element ID offset
        int idpoff = 0;         // Part ID offset
        int idmoff = 0;         // Material ID offset
        int idsoff = 0;         // Section ID offset
        int idfoff = 0;         // Function/curve ID offset
        int iddoff = 0;         // Definition ID offset
        int idroff = 0;         // Rigid body ID offset
        int tranid = 0;         // Transform ID
        double fctmas = 1.0;    // Mass scale factor
        double fcttim = 1.0;    // Time scale factor
        double fctlen = 1.0;    // Length scale factor
        double fcttem = 0.0;    // Temperature offset
        int incout1 = 0;        // Output control 1
        int tranidout = 0;      // Transform output
    };

    IncludeTransform() = default;

    std::string getKeywordName() const override { return "*INCLUDE_TRANSFORM"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    TransformData& getData() { return data_; }
    const TransformData& getData() const { return data_; }

private:
    TransformData data_;
};

/**
 * @brief *INCLUDE_STAMPED_PART
 *
 * Includes stamped part data.
 */
class KOO_API IncludeStampedPart : public CloneableKeyword<IncludeStampedPart, Keyword> {
public:
    struct Data {
        std::string filename;
        int pid = 0;          // Part ID
        int idnoff = 0;       // Node ID offset
        int ideoff = 0;       // Element ID offset
    };

    IncludeStampedPart() = default;

    std::string getKeywordName() const override { return "*INCLUDE_STAMPED_PART"; }

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
 * @brief *INCLUDE_AUTO_OFFSET
 *
 * Includes with automatic ID offset.
 */
class KOO_API IncludeAutoOffset : public CloneableKeyword<IncludeAutoOffset, Keyword> {
public:
    IncludeAutoOffset() = default;
    explicit IncludeAutoOffset(const std::string& filename) : filename_(filename) {}

    std::string getKeywordName() const override { return "*INCLUDE_AUTO_OFFSET"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    const std::string& getFilename() const { return filename_; }
    void setFilename(const std::string& filename) { filename_ = filename; }

private:
    std::string filename_;
};

/**
 * @brief *INCLUDE_COMPENSATE
 *
 * Includes with compensation data.
 */
class KOO_API IncludeCompensate : public CloneableKeyword<IncludeCompensate, Keyword> {
public:
    struct Data {
        std::string filename;
        int niter = 0;        // Number of iterations
        double tol = 0.0;     // Tolerance
    };

    IncludeCompensate() = default;

    std::string getKeywordName() const override { return "*INCLUDE_COMPENSATE"; }

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
 * @brief *INCLUDE_BINARY
 *
 * Includes binary format file.
 */
class KOO_API IncludeBinary : public CloneableKeyword<IncludeBinary, Keyword> {
public:
    IncludeBinary() = default;
    explicit IncludeBinary(const std::string& filename) : filename_(filename) {}

    std::string getKeywordName() const override { return "*INCLUDE_BINARY"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    const std::string& getFilename() const { return filename_; }
    void setFilename(const std::string& filename) { filename_ = filename; }

private:
    std::string filename_;
};

} // namespace koo::dyna
