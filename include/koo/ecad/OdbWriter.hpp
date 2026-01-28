#pragma once

#include <koo/Export.hpp>
#include <koo/ecad/OdbJob.hpp>
#include <filesystem>
#include <string>
#include <functional>

namespace koo::ecad {

/**
 * @brief ODB++ directory writer
 *
 * Writes OdbJob data to ODB++ directory structure.
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
 *     ├── attrlist            - Global attributes
 *     ├── stackup             - Stackup definition
 *     └── ...
 */
class KOO_API OdbWriter {
public:
    /// Writing options
    struct Options {
        bool compressFeatures = true;    ///< Compress features files with zlib
        bool writeEdaData = true;        ///< Write EDA netlist data
        bool writeSymbols = true;        ///< Write user symbols
        bool writeStackup = true;        ///< Write stackup information
        bool writeImpedance = true;      ///< Write impedance constraints
        bool writeVariants = true;       ///< Write component variants
        bool writeEmbedded = true;       ///< Write embedded components
        bool writeBom = true;            ///< Write BOM data
        bool overwrite = false;          ///< Overwrite existing directory
        int compressionLevel = 6;        ///< zlib compression level (1-9)
    };

    /// Progress callback type
    using ProgressCallback = std::function<void(const std::string& message, double progress)>;

    OdbWriter() = default;

    /// Set progress callback
    void setProgressCallback(ProgressCallback callback) { progressCallback_ = callback; }

    // ========== Full Write ==========

    /// Write complete ODB++ job to directory
    bool write(const OdbJob& job, const std::filesystem::path& odbPath);

    /// Write with options
    bool write(const OdbJob& job, const std::filesystem::path& odbPath, const Options& options);

    // ========== Partial Write ==========

    /// Write only the matrix file
    bool writeMatrix(const OdbJob& job, const std::filesystem::path& odbPath);

    /// Write single step
    bool writeStep(const Step& step, const std::filesystem::path& stepPath);

    /// Write single layer
    bool writeLayer(const Layer& layer, const std::filesystem::path& layerPath);

    /// Write EDA data
    bool writeEdaData(const EdaData& eda, const std::filesystem::path& edaPath);

    /// Write symbol
    bool writeSymbol(const Symbol& symbol, const std::filesystem::path& symbolPath);

    // ========== Error Handling ==========

    /// Get last error message
    const std::string& getLastError() const { return lastError_; }

    /// Check if last write was successful
    bool hasError() const { return !lastError_.empty(); }

private:
    // ========== Writing Functions ==========

    /// Create ODB++ directory structure
    bool createDirectoryStructure(const std::filesystem::path& odbPath);

    /// Write matrix file
    bool writeMatrixFile(const OdbJob& job, const std::filesystem::path& matrixPath);

    /// Write misc/info file
    bool writeJobInfo(const OdbJob& job, const std::filesystem::path& infoPath);

    /// Write misc/attrlist file
    bool writeJobAttributes(const OdbJob& job, const std::filesystem::path& attrPath);

    /// Write step directory
    bool writeStepDir(const Step& step, const std::filesystem::path& stepPath);

    /// Write stephdr file
    bool writeStepHeader(const Step& step, const std::filesystem::path& stephdrPath);

    /// Write profile file
    bool writeProfile(const std::vector<Contour>& profile, const std::filesystem::path& profilePath);

    /// Write layer directory
    bool writeLayerDir(const Layer& layer, const std::filesystem::path& layerPath);

    /// Write features file
    bool writeFeatures(const Layer& layer, const std::filesystem::path& featuresPath);

    /// Write EDA data file
    bool writeEdaDataFile(const EdaData& eda, const std::filesystem::path& edaPath);

    /// Write symbol directory
    bool writeSymbolDir(const Symbol& symbol, const std::filesystem::path& symbolPath);

    /// Write attrlist file
    bool writeAttrList(const AttributeList& attrs, const std::filesystem::path& attrPath);

    /// Write stackup file
    bool writeStackup(const std::vector<StackupLayer>& stackup, const std::filesystem::path& stackupPath);

    /// Write impedance file
    bool writeImpedance(const std::vector<ImpedanceConstraint>& impedance,
                        const std::filesystem::path& impedancePath);

    /// Write intentional shorts file
    bool writeShortf(const std::vector<IntentionalShort>& shorts,
                     const std::filesystem::path& shortfPath);

    /// Write drill tools file
    bool writeTools(const std::vector<DrillTool>& tools, const std::filesystem::path& toolsPath);

    /// Write metadata file
    bool writeMetadata(const Metadata& metadata, const std::filesystem::path& metadataPath);

    /// Write variants file
    bool writeVariants(const std::vector<ComponentVariant>& variants,
                       const std::filesystem::path& variantsPath);

    /// Write embedded components file
    bool writeEmbeddedComponents(const std::vector<EmbeddedComponent>& components,
                                 const std::filesystem::path& embeddedPath);

    /// Write build-up information file
    bool writeBuildup(const BuildupInfo& buildup, const std::filesystem::path& buildupPath);

    /// Write VPL (Vendor Part List) file
    bool writeVpl(const std::vector<VendorPart>& parts, const std::filesystem::path& vplPath);

    /// Write customer information file
    bool writeCustomerInfo(const CustomerInfo& info, const std::filesystem::path& infoPath);

    // ========== Feature Writing ==========

    /// Write single feature to stream
    void writeFeature(std::ostream& out, const Feature& feature,
                      const std::vector<std::string>& symbolNames);

    /// Write line feature
    void writeLineFeature(std::ostream& out, const LineFeature& line,
                          const std::vector<std::string>& symbolNames);

    /// Write pad feature
    void writePadFeature(std::ostream& out, const PadFeature& pad,
                         const std::vector<std::string>& symbolNames);

    /// Write arc feature
    void writeArcFeature(std::ostream& out, const ArcFeature& arc,
                         const std::vector<std::string>& symbolNames);

    /// Write text feature
    void writeTextFeature(std::ostream& out, const TextFeature& text);

    /// Write surface feature
    void writeSurfaceFeature(std::ostream& out, const SurfaceFeature& surface);

    // ========== EDA Data Writing ==========

    /// Write components section
    void writeComponents(std::ostream& out, const EdaData& eda);

    /// Write nets section
    void writeNets(std::ostream& out, const EdaData& eda);

    /// Write packages section
    void writePackages(std::ostream& out, const EdaData& eda);

    /// Write BOM section
    void writeBomData(std::ostream& out, const EdaData& eda);

    // ========== Utility ==========

    /// Compress string with zlib
    std::vector<uint8_t> compressData(const std::string& data);

    /// Write compressed file
    bool writeCompressedFile(const std::string& content, const std::filesystem::path& filePath);

    /// Write plain file
    bool writePlainFile(const std::string& content, const std::filesystem::path& filePath);

    /// Report progress
    void reportProgress(const std::string& message, double progress);

    /// Format double value for output
    std::string formatDouble(double value, int precision = 6) const;

    /// Get symbol index in list
    int getSymbolIndex(const std::string& symbolName,
                       const std::vector<std::string>& symbolNames) const;

    /// Build symbol name list from layer
    std::vector<std::string> buildSymbolList(const Layer& layer) const;

private:
    Options options_;
    std::string lastError_;
    ProgressCallback progressCallback_;
};

} // namespace koo::ecad
