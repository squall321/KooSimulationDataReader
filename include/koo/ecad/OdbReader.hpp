#pragma once

#include <koo/Export.hpp>
#include <koo/ecad/OdbJob.hpp>
#include <filesystem>
#include <string>
#include <vector>
#include <functional>

namespace koo::ecad {

/**
 * @brief ODB++ directory reader
 *
 * Parses ODB++ job directory structure and loads data into OdbJob object.
 *
 * ODB++ directory structure:
 * odb_job/
 * ├── matrix/matrix           - Layer/step definitions
 * ├── steps/<step_name>/      - Step data
 * │   ├── stephdr             - Step header
 * │   ├── profile             - Board outline
 * │   ├── attrlist            - Step attributes
 * │   ├── layers/<layer>/     - Layer data
 * │   │   ├── features(.z)    - Graphic features
 * │   │   └── attrlist        - Layer attributes
 * │   └── eda/data            - EDA netlist data
 * ├── symbols/<symbol>/       - User symbols
 * │   └── features            - Symbol features
 * ├── fonts/                  - Font definitions
 * └── misc/                   - Metadata
 *     ├── info                - Job info
 *     └── attrlist            - Global attributes
 */
class KOO_API OdbReader {
public:
    /// Reading options
    struct Options {
        bool loadFeatures = true;       ///< Load layer features
        bool loadEdaData = true;        ///< Load EDA netlist data
        bool loadSymbols = true;        ///< Load user symbols
        bool decompressFeatures = true; ///< Decompress .z files

        /// Filter to load specific steps only (empty = load all)
        std::vector<std::string> stepFilter;

        /// Filter to load specific layers only (empty = load all)
        std::vector<std::string> layerFilter;
    };

    /// Progress callback type
    using ProgressCallback = std::function<void(const std::string& message, double progress)>;

    OdbReader() = default;

    /// Set progress callback
    void setProgressCallback(ProgressCallback callback) { progressCallback_ = callback; }

    // ========== Full Read ==========

    /// Read complete ODB++ job
    OdbJob read(const std::filesystem::path& odbPath);

    /// Read with options
    OdbJob read(const std::filesystem::path& odbPath, const Options& options);

    // ========== Partial Read ==========

    /// Read only the matrix (layer definitions)
    LayerMatrix readMatrix(const std::filesystem::path& odbPath);

    /// List steps in ODB++ job
    std::vector<std::string> listSteps(const std::filesystem::path& odbPath);

    /// Read single step
    std::unique_ptr<Step> readStep(const std::filesystem::path& odbPath,
                                   const std::string& stepName);

    /// Read single layer from step
    std::unique_ptr<Layer> readLayer(const std::filesystem::path& odbPath,
                                     const std::string& stepName,
                                     const std::string& layerName);

    // ========== Error Handling ==========

    /// Get last error message
    const std::string& getLastError() const { return lastError_; }

    /// Check if last read was successful
    bool hasError() const { return !lastError_.empty(); }

private:
    // ========== Parsing Functions ==========

    /// Parse matrix file
    void parseMatrix(OdbJob& job, const std::filesystem::path& matrixPath);

    /// Parse misc/info file
    void parseJobInfo(OdbJob& job, const std::filesystem::path& infoPath);

    /// Parse misc/attrlist file
    void parseJobAttributes(OdbJob& job, const std::filesystem::path& attrPath);

    /// Parse step directory
    void parseStep(Step& step, const std::filesystem::path& stepPath);

    /// Parse stephdr file
    void parseStepHeader(Step& step, const std::filesystem::path& stephdrPath);

    /// Parse profile file
    void parseProfile(std::vector<Contour>& profile, const std::filesystem::path& profilePath);

    /// Parse layer directory
    void parseLayer(Layer& layer, const std::filesystem::path& layerPath);

    /// Parse features file
    void parseFeatures(Layer& layer, const std::filesystem::path& featuresPath);

    /// Parse EDA data
    void parseEdaData(EdaData& eda, const std::filesystem::path& edaPath);

    /// Parse user symbol
    void parseSymbol(Symbol& symbol, const std::filesystem::path& symbolPath);

    /// Parse attrlist file
    void parseAttrList(AttributeList& attrs, const std::filesystem::path& attrPath);

    /// Parse components file (eda/data components section)
    void parseComponents(EdaData& eda, const std::string& content);

    /// Parse netlist file (cadnet/refnet)
    void parseNetlist(EdaData& eda, const std::filesystem::path& netlistPath);

    /// Parse tools file (drill tools)
    void parseTools(std::vector<DrillTool>& tools, const std::filesystem::path& toolsPath);

    /// Parse BOM file (if present)
    void parseBom(EdaData& eda, const std::filesystem::path& bomPath);

    /// Parse zones file
    void parseZones(std::vector<Zone>& zones, const std::filesystem::path& zonesPath);

    /// Parse dimensions file
    void parseDimensions(std::vector<Dimension>& dims, const std::filesystem::path& dimsPath);

    /// Parse stackup file (stackup.xml or structured format)
    void parseStackup(std::vector<StackupLayer>& stackup, const std::filesystem::path& stackupPath);

    /// Parse impedance file (impedance.xml or structured format)
    void parseImpedance(std::vector<ImpedanceConstraint>& impedance, const std::filesystem::path& impedancePath);

    /// Parse shortf file (intentional shorts)
    void parseShortf(std::vector<IntentionalShort>& shorts, const std::filesystem::path& shortfPath);

    /// Parse HDI netlist
    void parseHdiNetlist(EdaData& eda, const std::filesystem::path& hdiPath);

    /// Parse user-defined symbol
    void parseUserSymbol(Symbol& symbol, const std::filesystem::path& symbolPath);

    /// Parse wheel file (Gerber aperture wheel)
    void parseWheel(std::vector<std::string>& apertures, const std::filesystem::path& wheelPath);

    /// Parse font file
    void parseFont(const std::filesystem::path& fontPath);

    /// Parse system attribute definitions (sysattr files)
    void parseSysAttr(std::vector<AttributeDefinition>& attrs, const std::filesystem::path& attrPath);

    /// Parse metadata file (misc/metadata or misc/metadata.xml)
    void parseMetadata(Metadata& metadata, const std::filesystem::path& metadataPath);

    /// Parse variants file (misc/variants)
    void parseVariants(std::vector<ComponentVariant>& variants, const std::filesystem::path& variantsPath);

    /// Parse embedded components file
    void parseEmbeddedComponents(std::vector<EmbeddedComponent>& components, const std::filesystem::path& embeddedPath);

    /// Parse build-up information file
    void parseBuildup(BuildupInfo& buildup, const std::filesystem::path& buildupPath);

    /// Parse VPL (Vendor Part List) file
    void parseVpl(std::vector<VendorPart>& parts, const std::filesystem::path& vplPath);

    /// Parse customer information file
    void parseCustomerInfo(CustomerInfo& info, const std::filesystem::path& infoPath);

    // ========== Feature Parsing ==========

    /// Parse single feature line
    std::unique_ptr<Feature> parseFeatureLine(const std::string& line,
                                               const std::vector<std::string>& symbolNames);

    /// Parse line feature
    std::unique_ptr<LineFeature> parseLineFeature(const std::vector<std::string>& tokens,
                                                  const std::vector<std::string>& symbolNames);

    /// Parse pad feature
    std::unique_ptr<PadFeature> parsePadFeature(const std::vector<std::string>& tokens,
                                                const std::vector<std::string>& symbolNames);

    /// Parse arc feature
    std::unique_ptr<ArcFeature> parseArcFeature(const std::vector<std::string>& tokens,
                                                const std::vector<std::string>& symbolNames);

    /// Parse text feature
    std::unique_ptr<TextFeature> parseTextFeature(const std::vector<std::string>& tokens);

    /// Parse surface feature
    std::unique_ptr<SurfaceFeature> parseSurfaceFeature(std::istream& stream,
                                                        const std::string& firstLine);

    // ========== Utility ==========

    /// Decompress .z file to string
    std::string decompressFile(const std::filesystem::path& compressedPath);

    /// Read file contents (handling .z compression)
    std::string readFileContents(const std::filesystem::path& filePath, bool decompress = true);

    /// Report progress
    void reportProgress(const std::string& message, double progress);

    /// Parse structured text file into key-value pairs
    std::unordered_map<std::string, std::string> parseStructuredFile(const std::string& content);

    /// Tokenize line
    std::vector<std::string> tokenize(const std::string& line);

    /// Parse orientation definition
    void parseOrientDef(const std::string& orientStr, double& rotation, bool& mirror);

    /// Parse feature attributes
    void parseFeatureAttributes(const std::string& attrStr, Feature& feature,
                                const std::vector<std::string>& attrNames,
                                const std::vector<std::string>& attrStrings);

private:
    Options options_;
    std::string lastError_;
    ProgressCallback progressCallback_;
};

} // namespace koo::ecad
