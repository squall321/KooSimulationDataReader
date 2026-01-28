#include <koo/ecad/OdbWriter.hpp>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <unordered_set>
#include <zlib.h>

namespace koo::ecad {

namespace {

// Helper to convert layer type to string
std::string layerTypeToString(LayerType type) {
    switch (type) {
        case LayerType::Signal: return "SIGNAL";
        case LayerType::PowerGround: return "POWER_GROUND";
        case LayerType::Mixed: return "MIXED";
        case LayerType::SolderMask: return "SOLDER_MASK";
        case LayerType::SolderPaste: return "SOLDER_PASTE";
        case LayerType::SilkScreen: return "SILK_SCREEN";
        case LayerType::Drill: return "DRILL";
        case LayerType::Rout: return "ROUT";
        case LayerType::Document: return "DOCUMENT";
        case LayerType::Component: return "COMPONENT";
        case LayerType::Mask: return "MASK";
        default: return "DOCUMENT";
    }
}

// Helper to convert layer context to string
std::string layerContextToString(LayerContext context) {
    switch (context) {
        case LayerContext::Board: return "BOARD";
        case LayerContext::Misc: return "MISC";
        default: return "BOARD";
    }
}

// Helper to convert polarity to string
std::string polarityToString(Polarity polarity) {
    return polarity == Polarity::Positive ? "POSITIVE" : "NEGATIVE";
}

// Helper to convert ODB version to string
std::string versionToString(OdbVersion version) {
    switch (version) {
        case OdbVersion::V7_0: return "7.0";
        case OdbVersion::V8_0: return "8.0";
        case OdbVersion::V8_1: return "8.1";
        default: return "8.1";
    }
}

// Helper to convert material type to string
std::string materialTypeToString(StackupMaterialType type) {
    switch (type) {
        case StackupMaterialType::Copper: return "COPPER";
        case StackupMaterialType::Core: return "CORE";
        case StackupMaterialType::Prepreg: return "PREPREG";
        case StackupMaterialType::SolderMask: return "SOLDER_MASK";
        case StackupMaterialType::Coverlay: return "COVERLAY";
        case StackupMaterialType::Stiffener: return "STIFFENER";
        case StackupMaterialType::Adhesive: return "ADHESIVE";
        case StackupMaterialType::Other: return "OTHER";
        default: return "OTHER";
    }
}

// Helper to convert drill tool type to string
std::string drillToolTypeToString(DrillToolType type) {
    switch (type) {
        case DrillToolType::Plated: return "PLATED";
        case DrillToolType::NonPlated: return "NON_PLATED";
        case DrillToolType::Via: return "VIA";
        default: return "PLATED";
    }
}

// Helper to convert variant state to string
std::string variantStateToString(ComponentVariantState state) {
    switch (state) {
        case ComponentVariantState::Enabled: return "ENABLED";
        case ComponentVariantState::Disabled: return "DISABLED";
        case ComponentVariantState::Replaced: return "REPLACED";
        default: return "ENABLED";
    }
}

// Helper to convert embedded component type to string
std::string embeddedTypeToString(EmbeddedComponentType type) {
    switch (type) {
        case EmbeddedComponentType::Resistor: return "RESISTOR";
        case EmbeddedComponentType::Capacitor: return "CAPACITOR";
        case EmbeddedComponentType::Inductor: return "INDUCTOR";
        default: return "RESISTOR";
    }
}

// Helper to convert via span type to string
std::string viaSpanTypeToString(ViaSpanType type) {
    switch (type) {
        case ViaSpanType::Through: return "THROUGH";
        case ViaSpanType::Blind: return "BLIND";
        case ViaSpanType::Buried: return "BURIED";
        case ViaSpanType::Microvia: return "MICROVIA";
        default: return "THROUGH";
    }
}

} // anonymous namespace

// ============================================================================
// Main Write Functions
// ============================================================================

bool OdbWriter::write(const OdbJob& job, const std::filesystem::path& odbPath) {
    return write(job, odbPath, Options());
}

bool OdbWriter::write(const OdbJob& job, const std::filesystem::path& odbPath,
                      const Options& options) {
    lastError_.clear();
    options_ = options;

    try {
        // Check if directory exists
        if (std::filesystem::exists(odbPath)) {
            if (!options_.overwrite) {
                lastError_ = "Directory already exists: " + odbPath.string();
                return false;
            }
            std::filesystem::remove_all(odbPath);
        }

        reportProgress("Creating directory structure...", 0.0);

        // Create directory structure
        if (!createDirectoryStructure(odbPath)) {
            return false;
        }

        reportProgress("Writing matrix...", 0.05);

        // Write matrix
        auto matrixPath = odbPath / "matrix" / "matrix";
        if (!writeMatrixFile(job, matrixPath)) {
            return false;
        }

        reportProgress("Writing job info...", 0.1);

        // Write misc/info
        auto infoPath = odbPath / "misc" / "info";
        if (!writeJobInfo(job, infoPath)) {
            return false;
        }

        // Write misc/attrlist
        auto attrPath = odbPath / "misc" / "attrlist";
        if (!writeJobAttributes(job, attrPath)) {
            return false;
        }

        // Write steps
        const auto& stepNames = job.getStepNames();
        double stepProgress = 0.15;
        double stepIncrement = 0.5 / static_cast<double>(std::max(size_t(1), stepNames.size()));

        for (const auto& stepName : stepNames) {
            reportProgress("Writing step: " + stepName, stepProgress);

            const Step* step = job.getStep(stepName);
            if (step) {
                auto stepPath = odbPath / "steps" / stepName;
                if (!writeStepDir(*step, stepPath)) {
                    return false;
                }
            }

            stepProgress += stepIncrement;
        }

        // Write symbols
        if (options_.writeSymbols) {
            reportProgress("Writing symbols...", 0.7);

            const auto& symbolNames = job.getSymbolNames();
            for (const auto& symbolName : symbolNames) {
                const Symbol* symbol = job.getSymbol(symbolName);
                if (symbol && !symbol->isStandard()) {
                    auto symbolPath = odbPath / "symbols" / symbolName;
                    if (!writeSymbolDir(*symbol, symbolPath)) {
                        return false;
                    }
                }
            }
        }

        // Write stackup
        if (options_.writeStackup && !job.getStackup().empty()) {
            reportProgress("Writing stackup...", 0.75);

            auto stackupPath = odbPath / "misc" / "stackup";
            if (!writeStackup(job.getStackup(), stackupPath)) {
                return false;
            }
        }

        // Write impedance
        if (options_.writeImpedance && !job.getImpedanceConstraints().empty()) {
            reportProgress("Writing impedance...", 0.8);

            auto impedancePath = odbPath / "misc" / "impedance";
            if (!writeImpedance(job.getImpedanceConstraints(), impedancePath)) {
                return false;
            }
        }

        // Write drill tools
        if (!job.getDrillTools().empty()) {
            reportProgress("Writing tools...", 0.82);

            auto toolsPath = odbPath / "misc" / "tools";
            if (!writeTools(job.getDrillTools(), toolsPath)) {
                return false;
            }
        }

        // Write intentional shorts
        if (!job.getIntentionalShorts().empty()) {
            reportProgress("Writing shortcuts...", 0.84);

            auto shortfPath = odbPath / "misc" / "shortf";
            if (!writeShortf(job.getIntentionalShorts(), shortfPath)) {
                return false;
            }
        }

        // Write metadata
        reportProgress("Writing metadata...", 0.86);
        auto metadataPath = odbPath / "misc" / "metadata";
        if (!writeMetadata(job.getMetadata(), metadataPath)) {
            return false;
        }

        // Write variants
        if (options_.writeVariants && job.getVariantCount() > 0) {
            reportProgress("Writing variants...", 0.88);

            auto variantsPath = odbPath / "misc" / "variants";
            if (!writeVariants(job.getVariants(), variantsPath)) {
                return false;
            }
        }

        // Write embedded components
        if (options_.writeEmbedded && job.getEmbeddedComponentCount() > 0) {
            reportProgress("Writing embedded components...", 0.9);

            auto embeddedPath = odbPath / "misc" / "embedded";
            if (!writeEmbeddedComponents(job.getEmbeddedComponents(), embeddedPath)) {
                return false;
            }
        }

        // Write build-up info
        if (!job.getBuildupInfo().name.empty()) {
            reportProgress("Writing buildup...", 0.92);

            auto buildupPath = odbPath / "misc" / "buildup";
            if (!writeBuildup(job.getBuildupInfo(), buildupPath)) {
                return false;
            }
        }

        // Write VPL
        if (!job.getVendorParts().empty()) {
            reportProgress("Writing VPL...", 0.94);

            auto vplPath = odbPath / "misc" / "vpl";
            if (!writeVpl(job.getVendorParts(), vplPath)) {
                return false;
            }
        }

        // Write customer info
        if (!job.getCustomerInfo().name.empty()) {
            reportProgress("Writing customer info...", 0.96);

            auto customerPath = odbPath / "misc" / "customer";
            if (!writeCustomerInfo(job.getCustomerInfo(), customerPath)) {
                return false;
            }
        }

        reportProgress("Complete", 1.0);
        return true;

    } catch (const std::exception& e) {
        lastError_ = e.what();
        return false;
    }
}

bool OdbWriter::writeMatrix(const OdbJob& job, const std::filesystem::path& odbPath) {
    lastError_.clear();

    try {
        auto matrixDir = odbPath / "matrix";
        std::filesystem::create_directories(matrixDir);

        auto matrixPath = matrixDir / "matrix";
        return writeMatrixFile(job, matrixPath);

    } catch (const std::exception& e) {
        lastError_ = e.what();
        return false;
    }
}

bool OdbWriter::writeStep(const Step& step, const std::filesystem::path& stepPath) {
    lastError_.clear();
    options_ = Options();

    try {
        return writeStepDir(step, stepPath);
    } catch (const std::exception& e) {
        lastError_ = e.what();
        return false;
    }
}

bool OdbWriter::writeLayer(const Layer& layer, const std::filesystem::path& layerPath) {
    lastError_.clear();
    options_ = Options();

    try {
        return writeLayerDir(layer, layerPath);
    } catch (const std::exception& e) {
        lastError_ = e.what();
        return false;
    }
}

bool OdbWriter::writeEdaData(const EdaData& eda, const std::filesystem::path& edaPath) {
    lastError_.clear();

    try {
        std::filesystem::create_directories(edaPath);
        auto dataPath = edaPath / "data";
        return writeEdaDataFile(eda, dataPath);
    } catch (const std::exception& e) {
        lastError_ = e.what();
        return false;
    }
}

bool OdbWriter::writeSymbol(const Symbol& symbol, const std::filesystem::path& symbolPath) {
    lastError_.clear();
    options_ = Options();

    try {
        return writeSymbolDir(symbol, symbolPath);
    } catch (const std::exception& e) {
        lastError_ = e.what();
        return false;
    }
}

// ============================================================================
// Directory Structure
// ============================================================================

bool OdbWriter::createDirectoryStructure(const std::filesystem::path& odbPath) {
    try {
        std::filesystem::create_directories(odbPath / "matrix");
        std::filesystem::create_directories(odbPath / "steps");
        std::filesystem::create_directories(odbPath / "symbols");
        // NOTE: fonts directory reserved for future font writing implementation
        std::filesystem::create_directories(odbPath / "fonts");
        std::filesystem::create_directories(odbPath / "misc");
        // NOTE: input directory is part of ODB++ spec for storing original input files
        std::filesystem::create_directories(odbPath / "input");
        return true;
    } catch (const std::exception& e) {
        lastError_ = "Failed to create directory structure: " + std::string(e.what());
        return false;
    }
}

// ============================================================================
// Matrix File
// ============================================================================

bool OdbWriter::writeMatrixFile(const OdbJob& job, const std::filesystem::path& matrixPath) {
    std::ostringstream out;

    const auto& matrix = job.getMatrix();

    // Write steps
    for (const auto& stepDef : matrix.getStepDefinitions()) {
        out << "STEP {\n";
        out << "   COL=" << stepDef.col << "\n";
        out << "   NAME=" << stepDef.name << "\n";
        out << "}\n\n";
    }

    // Write layers
    for (const auto& layerDef : matrix.getLayerDefinitions()) {
        out << "LAYER {\n";
        out << "   ROW=" << layerDef.row << "\n";
        out << "   CONTEXT=" << layerContextToString(layerDef.context) << "\n";
        out << "   TYPE=" << layerTypeToString(layerDef.type) << "\n";
        out << "   NAME=" << layerDef.name << "\n";
        out << "   POLARITY=" << polarityToString(layerDef.polarity) << "\n";

        if (layerDef.startName > 0) {
            out << "   START_NAME=" << layerDef.startName << "\n";
        }
        if (layerDef.endName > 0) {
            out << "   END_NAME=" << layerDef.endName << "\n";
        }
        if (!layerDef.oldName.empty()) {
            out << "   OLD_NAME=" << layerDef.oldName << "\n";
        }

        out << "}\n\n";
    }

    return writePlainFile(out.str(), matrixPath);
}

// ============================================================================
// Job Info and Attributes
// ============================================================================

bool OdbWriter::writeJobInfo(const OdbJob& job, const std::filesystem::path& infoPath) {
    std::ostringstream out;

    const auto& info = job.getInfo();

    out << "UNITS=" << info.units << "\n";
    out << "JOB_NAME=" << info.name << "\n";
    out << "ODB_VERSION_MAJOR=" << versionToString(info.version) << "\n";
    out << "ODB_VERSION_MINOR=0\n";
    out << "ODB_SOURCE=" << info.saveApp << "\n";
    out << "CREATION_DATE=" << info.creationDate << "\n";
    out << "SAVE_DATE=" << info.modificationDate << "\n";
    out << "SAVE_APP=" << info.saveApp << "\n";
    out << "SAVE_USER=" << info.saveUser << "\n";

    return writePlainFile(out.str(), infoPath);
}

bool OdbWriter::writeJobAttributes(const OdbJob& job, const std::filesystem::path& attrPath) {
    return writeAttrList(job.getAttributes(), attrPath);
}

// ============================================================================
// Step Writing
// ============================================================================

bool OdbWriter::writeStepDir(const Step& step, const std::filesystem::path& stepPath) {
    try {
        std::filesystem::create_directories(stepPath);
        std::filesystem::create_directories(stepPath / "layers");
        std::filesystem::create_directories(stepPath / "eda");

        // Write stephdr
        auto stephdrPath = stepPath / "stephdr";
        if (!writeStepHeader(step, stephdrPath)) {
            return false;
        }

        // Write profile
        if (!step.getProfile().empty()) {
            auto profilePath = stepPath / "profile";
            if (!writeProfile(step.getProfile(), profilePath)) {
                return false;
            }
        }

        // Write step attrlist
        auto attrPath = stepPath / "attrlist";
        if (!writeAttrList(step.getAttributes(), attrPath)) {
            return false;
        }

        // Write layers
        for (const auto& layerName : step.getLayerNames()) {
            const Layer* layer = step.getLayer(layerName);
            if (layer) {
                auto layerPath = stepPath / "layers" / layerName;
                if (!writeLayerDir(*layer, layerPath)) {
                    return false;
                }
            }
        }

        // Write EDA data
        if (options_.writeEdaData) {
            auto edaPath = stepPath / "eda" / "data";
            if (!writeEdaDataFile(step.getEdaData(), edaPath)) {
                return false;
            }
        }

        return true;

    } catch (const std::exception& e) {
        lastError_ = "Failed to write step: " + std::string(e.what());
        return false;
    }
}

bool OdbWriter::writeStepHeader(const Step& step, const std::filesystem::path& stephdrPath) {
    std::ostringstream out;

    auto datum = step.getDatum();
    auto activeArea = step.getActiveArea();

    out << "X_DATUM=" << step.getXDatum() << "\n";
    out << "Y_DATUM=" << step.getYDatum() << "\n";
    out << "X_ORIGIN=" << formatDouble(datum.x) << "\n";
    out << "Y_ORIGIN=" << formatDouble(datum.y) << "\n";

    if (activeArea.isValid()) {
        out << "TOP_ACTIVE=" << formatDouble(activeArea.max.y) << "\n";
        out << "BOTTOM_ACTIVE=" << formatDouble(activeArea.min.y) << "\n";
        out << "RIGHT_ACTIVE=" << formatDouble(activeArea.max.x) << "\n";
        out << "LEFT_ACTIVE=" << formatDouble(activeArea.min.x) << "\n";
    }

    out << "AFFECT_HOLES=" << (step.getAffectHoles() ? "YES" : "NO") << "\n";

    // Write step and repeat
    for (const auto& sr : step.getStepRepeats()) {
        out << "\nSTEP-AND-REPEAT {\n";
        out << "   NAME=" << sr.stepName << "\n";
        out << "   X=" << formatDouble(sr.x) << "\n";
        out << "   Y=" << formatDouble(sr.y) << "\n";
        out << "   DX=" << formatDouble(sr.dx) << "\n";
        out << "   DY=" << formatDouble(sr.dy) << "\n";
        out << "   NX=" << sr.nx << "\n";
        out << "   NY=" << sr.ny << "\n";
        out << "   ANGLE=" << formatDouble(sr.angle) << "\n";
        out << "   MIRROR=" << (sr.mirror ? "YES" : "NO") << "\n";
        out << "}\n";
    }

    return writePlainFile(out.str(), stephdrPath);
}

bool OdbWriter::writeProfile(const std::vector<Contour>& profile,
                              const std::filesystem::path& profilePath) {
    std::ostringstream out;

    out << "#\n";
    out << "# Profile data\n";
    out << "#\n\n";
    out << "UNITS=MM\n\n";

    for (const auto& contour : profile) {
        // Start of contour
        auto start = contour.getStart();
        out << "OB " << formatDouble(start.x) << " "
            << formatDouble(start.y);

        if (contour.getPolygonType() == PolygonType::Hole) {
            out << " H";
        } else {
            out << " I";
        }
        out << "\n";

        // Segments
        for (const auto& seg : contour.getSegments()) {
            if (seg.type == ContourSegmentType::Arc) {
                out << "OC " << formatDouble(seg.x) << " " << formatDouble(seg.y)
                    << " " << formatDouble(seg.xc) << " " << formatDouble(seg.yc)
                    << (seg.clockwise ? " Y" : " N") << "\n";
            } else {
                out << "OS " << formatDouble(seg.x) << " " << formatDouble(seg.y) << "\n";
            }
        }

        out << "OE\n\n";
    }

    return writePlainFile(out.str(), profilePath);
}

// ============================================================================
// Layer Writing
// ============================================================================

bool OdbWriter::writeLayerDir(const Layer& layer, const std::filesystem::path& layerPath) {
    try {
        std::filesystem::create_directories(layerPath);

        // Write features
        auto featuresPath = layerPath / "features";
        if (!writeFeatures(layer, featuresPath)) {
            return false;
        }

        // Write attrlist
        auto attrPath = layerPath / "attrlist";
        if (!writeAttrList(layer.getAttributes(), attrPath)) {
            return false;
        }

        return true;

    } catch (const std::exception& e) {
        lastError_ = "Failed to write layer: " + std::string(e.what());
        return false;
    }
}

bool OdbWriter::writeFeatures(const Layer& layer, const std::filesystem::path& featuresPath) {
    std::ostringstream out;

    // Build symbol list
    auto symbolNames = buildSymbolList(layer);

    // Write header
    out << "#\n";
    out << "# Layer: " << layer.getName() << "\n";
    out << "#\n\n";
    out << "UNITS=MM\n\n";

    // Write symbol names section
    // Format: $<index> <name>
    for (size_t i = 0; i < symbolNames.size(); ++i) {
        out << "$" << i << " " << symbolNames[i] << "\n";
    }
    out << "\n";

    // Write features
    for (const auto& feature : layer.getFeatures()) {
        writeFeature(out, *feature, symbolNames);
    }

    // Compress if requested
    if (options_.compressFeatures) {
        return writeCompressedFile(out.str(), std::filesystem::path(featuresPath.string() + ".z"));
    } else {
        return writePlainFile(out.str(), featuresPath);
    }
}

std::vector<std::string> OdbWriter::buildSymbolList(const Layer& layer) const {
    std::vector<std::string> symbolNames;
    std::unordered_set<std::string> seen;

    for (const auto& feature : layer.getFeatures()) {
        std::string symName;

        if (auto* line = dynamic_cast<const LineFeature*>(feature.get())) {
            symName = line->getSymbolName();
        } else if (auto* pad = dynamic_cast<const PadFeature*>(feature.get())) {
            symName = pad->getSymbolName();
        } else if (auto* arc = dynamic_cast<const ArcFeature*>(feature.get())) {
            symName = arc->getSymbolName();
        }

        if (!symName.empty() && seen.find(symName) == seen.end()) {
            symbolNames.push_back(symName);
            seen.insert(symName);
        }
    }

    return symbolNames;
}

// ============================================================================
// Feature Writing
// ============================================================================

void OdbWriter::writeFeature(std::ostream& out, const Feature& feature,
                              const std::vector<std::string>& symbolNames) {
    if (auto* line = dynamic_cast<const LineFeature*>(&feature)) {
        writeLineFeature(out, *line, symbolNames);
    } else if (auto* pad = dynamic_cast<const PadFeature*>(&feature)) {
        writePadFeature(out, *pad, symbolNames);
    } else if (auto* arc = dynamic_cast<const ArcFeature*>(&feature)) {
        writeArcFeature(out, *arc, symbolNames);
    } else if (auto* text = dynamic_cast<const TextFeature*>(&feature)) {
        writeTextFeature(out, *text);
    } else if (auto* surface = dynamic_cast<const SurfaceFeature*>(&feature)) {
        writeSurfaceFeature(out, *surface);
    }
}

void OdbWriter::writeLineFeature(std::ostream& out, const LineFeature& line,
                                  const std::vector<std::string>& symbolNames) {
    int symIndex = getSymbolIndex(line.getSymbolName(), symbolNames);

    auto start = line.getStart();
    auto end = line.getEnd();
    out << "L " << formatDouble(start.x) << " " << formatDouble(start.y)
        << " " << formatDouble(end.x) << " " << formatDouble(end.y)
        << " " << symIndex
        << " " << (line.getPolarity() == Polarity::Positive ? "P" : "N");

    if (line.getDcode() > 0) {
        out << " " << line.getDcode();
    }

    out << "\n";
}

void OdbWriter::writePadFeature(std::ostream& out, const PadFeature& pad,
                                 const std::vector<std::string>& symbolNames) {
    int symIndex = getSymbolIndex(pad.getSymbolName(), symbolNames);

    auto pos = pad.getPosition();
    out << "P " << formatDouble(pos.x) << " " << formatDouble(pos.y)
        << " " << symIndex
        << " " << (pad.getPolarity() == Polarity::Positive ? "P" : "N");

    // Write dcode (required field, default to 0 if not set)
    out << " " << pad.getDcode();

    // Write orientation if not default
    double rotation = pad.getRotation();
    bool mirror = pad.isMirrored();
    if (std::abs(rotation) > 0.001 || mirror) {
        out << " " << static_cast<int>(rotation);
        if (mirror) {
            out << " M";
        }
    }

    out << "\n";
}

void OdbWriter::writeArcFeature(std::ostream& out, const ArcFeature& arc,
                                 const std::vector<std::string>& symbolNames) {
    int symIndex = getSymbolIndex(arc.getSymbolName(), symbolNames);

    auto start = arc.getStart();
    auto end = arc.getEnd();
    auto center = arc.getCenter();
    // Format: A xs ys xe ye xc yc symNum polarity dcode cw
    out << "A " << formatDouble(start.x) << " " << formatDouble(start.y)
        << " " << formatDouble(end.x) << " " << formatDouble(end.y)
        << " " << formatDouble(center.x) << " " << formatDouble(center.y)
        << " " << symIndex
        << " " << (arc.getPolarity() == Polarity::Positive ? "P" : "N")
        << " " << arc.getDcode()
        << " " << (arc.isClockwise() ? "Y" : "N");

    out << "\n";
}

void OdbWriter::writeTextFeature(std::ostream& out, const TextFeature& text) {
    auto pos = text.getPosition();
    out << "T " << formatDouble(pos.x) << " " << formatDouble(pos.y)
        << " " << text.getFont()
        << " " << (text.getPolarity() == Polarity::Positive ? "P" : "N")
        << " " << static_cast<int>(text.getRotation())
        << " " << (text.isMirrored() ? "M" : "N")
        << " " << formatDouble(text.getXSize())
        << " " << formatDouble(text.getYSize())
        << " " << formatDouble(text.getWidthFactor())
        << " '" << text.getText() << "'";

    if (text.getVersion() > 0) {
        out << " " << text.getVersion();
    }

    out << "\n";
}

void OdbWriter::writeSurfaceFeature(std::ostream& out, const SurfaceFeature& surface) {
    out << "S " << (surface.getPolarity() == Polarity::Positive ? "P" : "N");

    if (surface.getDcode() > 0) {
        out << " " << surface.getDcode();
    }

    out << "\n";

    // Write contours
    for (const auto& contour : surface.getContours()) {
        auto start = contour.getStart();
        out << "OB " << formatDouble(start.x) << " "
            << formatDouble(start.y);

        if (contour.getPolygonType() == PolygonType::Hole) {
            out << " H";
        } else {
            out << " I";
        }
        out << "\n";

        for (const auto& seg : contour.getSegments()) {
            if (seg.type == ContourSegmentType::Arc) {
                out << "OC " << formatDouble(seg.x) << " " << formatDouble(seg.y)
                    << " " << formatDouble(seg.xc) << " " << formatDouble(seg.yc)
                    << (seg.clockwise ? " Y" : " N") << "\n";
            } else {
                out << "OS " << formatDouble(seg.x) << " " << formatDouble(seg.y) << "\n";
            }
        }

        out << "OE\n";
    }

    out << "SE\n";
}

int OdbWriter::getSymbolIndex(const std::string& symbolName,
                               const std::vector<std::string>& symbolNames) const {
    auto it = std::find(symbolNames.begin(), symbolNames.end(), symbolName);
    if (it != symbolNames.end()) {
        return static_cast<int>(std::distance(symbolNames.begin(), it));
    }
    return 0;
}

// ============================================================================
// EDA Data Writing
// ============================================================================

bool OdbWriter::writeEdaDataFile(const EdaData& eda, const std::filesystem::path& edaPath) {
    std::ostringstream out;

    out << "#\n";
    out << "# EDA Data\n";
    out << "#\n\n";
    out << "UNITS=MM\n\n";

    // Write layer names
    out << "#\n# Layer names\n#\n";
    out << "LYR {\n";
    for (const auto& layerName : eda.getLayerNames()) {
        out << "   " << layerName << "\n";
    }
    out << "}\n\n";

    // Write packages
    writePackages(out, eda);

    // Write components
    writeComponents(out, eda);

    // Write nets
    writeNets(out, eda);

    // Write BOM if requested
    if (options_.writeBom) {
        writeBomData(out, eda);
    }

    // Compress if requested
    if (options_.compressFeatures) {
        return writeCompressedFile(out.str(), std::filesystem::path(edaPath.string() + ".z"));
    } else {
        return writePlainFile(out.str(), edaPath);
    }
}

void OdbWriter::writePackages(std::ostream& out, const EdaData& eda) {
    out << "#\n# Packages\n#\n";

    for (int i = 0; i < static_cast<int>(eda.getPackageCount()); ++i) {
        const Package* pkg = eda.getPackage(i);
        if (!pkg) continue;

        // Format: PKG <name> [<pitch> <xmin> <ymin> <xmax> <ymax>]
        out << "PKG " << pkg->getName();

        if (pkg->getPitch() > 0) {
            const auto& bbox = pkg->getBoundingBox();
            out << " " << formatDouble(pkg->getPitch())
                << " " << formatDouble(bbox.min.x)
                << " " << formatDouble(bbox.min.y)
                << " " << formatDouble(bbox.max.x)
                << " " << formatDouble(bbox.max.y);
        }

        out << "\n";

        // Write pins
        // Format: PIN <name> <type> <x> <y> [<f_l> <e_l> <m_l>]
        for (const auto& pin : pkg->getPins()) {
            char typeChar = 'S';  // Default to SMD
            if (pin.type == PinType::ThroughHole) {
                typeChar = 'T';
            } else if (pin.type == PinType::Bga) {
                typeChar = 'B';
            } else if (pin.type == PinType::Fiducial) {
                typeChar = 'F';
            }

            out << "PIN " << pin.name
                << " " << typeChar
                << " " << formatDouble(pin.x)
                << " " << formatDouble(pin.y);

            // Optional layer indices
            if (pin.featureLayerIndex >= 0 || pin.electricalLayerIndex >= 0 || pin.mechanicalLayerIndex >= 0) {
                out << " " << pin.featureLayerIndex
                    << " " << pin.electricalLayerIndex
                    << " " << pin.mechanicalLayerIndex;
            }

            out << "\n";
        }
    }
}

void OdbWriter::writeComponents(std::ostream& out, const EdaData& eda) {
    out << "#\n# Components\n#\n";

    for (const auto& refDes : eda.getComponentRefDes()) {
        const Component* comp = eda.getComponent(refDes);
        if (!comp) continue;

        // Format: CMP <pkg_ref> <x> <y> <rotation> <mirror> <comp_name> [<part_name>]
        // Reader expects: tokens[1]=pkg_idx, tokens[2]=x, tokens[3]=y,
        //                 tokens[4]=rotation, tokens[5]=mirror, tokens[6]=refDes
        auto pos = comp->getPosition();

        // Look up package index from package name
        int pkgIndex = comp->getPackageIndex();
        if (pkgIndex < 0 && !comp->getPackageName().empty()) {
            // Try to find package by name
            for (int i = 0; i < static_cast<int>(eda.getPackageCount()); ++i) {
                const Package* pkg = eda.getPackage(i);
                if (pkg && pkg->getName() == comp->getPackageName()) {
                    pkgIndex = i;
                    break;
                }
            }
        }

        out << "CMP " << pkgIndex
            << " " << formatDouble(pos.x)
            << " " << formatDouble(pos.y)
            << " " << formatDouble(comp->getRotation())
            << " " << (comp->isMirrored() ? "M" : "N")
            << " " << comp->getRefDes();

        if (!comp->getPartNumber().empty()) {
            out << " " << comp->getPartNumber();
        }

        out << "\n";

        // Write component pins with net assignments using TOP format
        // Format: TOP <toe_num> <x> <y> <rot> <mir> <net_num> <sn_num> <pin_name>
        int pinNum = 0;
        for (const auto& pin : comp->getPins()) {
            out << "TOP " << pinNum
                << " " << formatDouble(pin.x)
                << " " << formatDouble(pin.y)
                << " " << formatDouble(pin.rotation)
                << " " << (pin.mirror ? "M" : "N")
                << " " << 0  // net_num - would need lookup
                << " " << 0  // subnet_num
                << " " << pin.name
                << "\n";
            pinNum++;
        }
    }
}

void OdbWriter::writeNets(std::ostream& out, const EdaData& eda) {
    out << "#\n# Nets\n#\n";

    for (const auto& netName : eda.getNetNames()) {
        const Net* net = eda.getNet(netName);
        if (!net) continue;

        out << "NET " << net->getName() << " {\n";
        out << "   NET_NUMBER=" << net->getNetNumber() << "\n";

        // Net class
        if (net->isPower()) {
            out << "   NET_CLASS=POWER\n";
        } else if (net->isGround()) {
            out << "   NET_CLASS=GROUND\n";
        } else {
            out << "   NET_CLASS=SIGNAL\n";
        }

        // Write pins
        for (const auto& pin : net->getPins()) {
            out << "   PIN " << pin.refDes << " " << pin.pinName << "\n";
        }

        // Write subnets
        for (const auto& subnet : net->getSubnets()) {
            out << "   SNT {\n";

            std::string typeStr;
            switch (subnet.type) {
                case Net::Subnet::Type::Trace: typeStr = "TRACE"; break;
                case Net::Subnet::Type::Via: typeStr = "VIA"; break;
                case Net::Subnet::Type::Plane: typeStr = "PLANE"; break;
                case Net::Subnet::Type::Toeprint: typeStr = "TOEPRINT"; break;
                default: typeStr = "TRACE"; break;
            }
            out << "      TYPE=" << typeStr << "\n";

            for (int fid : subnet.featureIds) {
                out << "      FID " << fid << "\n";
            }

            out << "   }\n";
        }

        out << "}\n\n";
    }
}

void OdbWriter::writeBomData(std::ostream& out, const EdaData& eda) {
    const auto& bomItems = eda.getBomItems();
    if (bomItems.empty()) return;

    out << "#\n# BOM Items\n#\n";

    for (const auto& item : bomItems) {
        out << "BOM " << item.refDes << " {\n";
        out << "   PART_NUMBER=" << item.partNumber << "\n";

        if (!item.description.empty()) {
            out << "   DESCRIPTION=" << item.description << "\n";
        }

        if (!item.manufacturer.empty()) {
            out << "   MANUFACTURER=" << item.manufacturer << "\n";
        }

        if (item.quantity > 0) {
            out << "   QUANTITY=" << item.quantity << "\n";
        }

        // Write additional descriptions (DESC1, DESC2, ...)
        for (size_t d = 0; d < item.descriptions.size(); ++d) {
            out << "   DESC" << (d + 1) << "=" << item.descriptions[d] << "\n";
        }

        out << "}\n\n";
    }
}

// ============================================================================
// Symbol Writing
// ============================================================================

bool OdbWriter::writeSymbolDir(const Symbol& symbol, const std::filesystem::path& symbolPath) {
    try {
        std::filesystem::create_directories(symbolPath);

        std::ostringstream out;

        out << "#\n";
        out << "# Symbol: " << symbol.getName() << "\n";
        out << "#\n\n";
        out << "UNITS=MM\n\n";

        // Build symbol list from features
        std::vector<std::string> symbolNames;

        // Write features
        for (const auto& feature : symbol.getFeatures()) {
            writeFeature(out, *feature, symbolNames);
        }

        auto featuresPath = symbolPath / "features";

        if (options_.compressFeatures) {
            return writeCompressedFile(out.str(),
                                        std::filesystem::path(featuresPath.string() + ".z"));
        } else {
            return writePlainFile(out.str(), featuresPath);
        }

    } catch (const std::exception& e) {
        lastError_ = "Failed to write symbol: " + std::string(e.what());
        return false;
    }
}

// ============================================================================
// Stackup Writing
// ============================================================================

bool OdbWriter::writeStackup(const std::vector<StackupLayer>& stackup,
                              const std::filesystem::path& stackupPath) {
    std::ostringstream out;

    out << "#\n";
    out << "# Stackup\n";
    out << "#\n\n";

    int layerNum = 1;
    for (const auto& layer : stackup) {
        out << "LAYER " << layerNum++ << " {\n";
        out << "   NAME=" << layer.name << "\n";
        out << "   TYPE=" << materialTypeToString(layer.materialType) << "\n";
        out << "   THICKNESS=" << formatDouble(layer.thickness) << "\n";

        if (!layer.material.empty()) {
            out << "   MATERIAL=" << layer.material << "\n";
        }

        if (layer.dielectricConstant > 0) {
            out << "   DK=" << formatDouble(layer.dielectricConstant) << "\n";
        }

        if (layer.lossTangent > 0) {
            out << "   DF=" << formatDouble(layer.lossTangent) << "\n";
        }

        out << "}\n\n";
    }

    return writePlainFile(out.str(), stackupPath);
}

// ============================================================================
// Impedance Writing
// ============================================================================

bool OdbWriter::writeImpedance(const std::vector<ImpedanceConstraint>& impedance,
                                const std::filesystem::path& impedancePath) {
    std::ostringstream out;

    out << "#\n";
    out << "# Impedance Constraints\n";
    out << "#\n\n";

    for (const auto& constraint : impedance) {
        out << "CONSTRAINT " << constraint.id << " {\n";
        out << "   ID=" << constraint.id << "\n";
        out << "   TYPE=" << constraint.type << "\n";
        out << "   IMPEDANCE=" << formatDouble(constraint.impedance) << "\n";
        out << "   TOLERANCE=" << formatDouble(constraint.tolerance) << "\n";

        if (!constraint.layer.empty()) {
            out << "   LAYER=" << constraint.layer << "\n";
        }

        if (constraint.traceWidth > 0) {
            out << "   TRACE_WIDTH=" << formatDouble(constraint.traceWidth) << "\n";
        }

        if (constraint.spacing > 0) {
            out << "   SPACING=" << formatDouble(constraint.spacing) << "\n";
        }

        out << "}\n\n";
    }

    return writePlainFile(out.str(), impedancePath);
}

// ============================================================================
// Shortf Writing
// ============================================================================

bool OdbWriter::writeShortf(const std::vector<IntentionalShort>& shorts,
                             const std::filesystem::path& shortfPath) {
    std::ostringstream out;

    out << "#\n";
    out << "# Intentional Shorts\n";
    out << "#\n\n";

    for (const auto& shortDef : shorts) {
        out << "SHORT " << shortDef.id << " {\n";

        for (uint32_t uid : shortDef.netUids) {
            out << "   NET_UID=" << uid << "\n";
        }

        out << "}\n\n";
    }

    return writePlainFile(out.str(), shortfPath);
}

// ============================================================================
// Tools Writing
// ============================================================================

bool OdbWriter::writeTools(const std::vector<DrillTool>& tools,
                            const std::filesystem::path& toolsPath) {
    std::ostringstream out;

    out << "#\n";
    out << "# Drill Tools\n";
    out << "#\n\n";
    out << "UNITS=MM\n\n";

    for (const auto& tool : tools) {
        out << "TOOL " << tool.num << " {\n";
        out << "   DIAMETER=" << formatDouble(tool.diameter) << "\n";
        out << "   TYPE=" << drillToolTypeToString(tool.type) << "\n";

        if (!tool.bit.empty()) {
            out << "   BIT=" << tool.bit << "\n";
        }

        if (tool.finishSize > 0) {
            out << "   FINISHED_SIZE=" << formatDouble(tool.finishSize) << "\n";
        }

        if (tool.drillCount > 0) {
            out << "   HIT_COUNT=" << tool.drillCount << "\n";
        }

        out << "}\n\n";
    }

    return writePlainFile(out.str(), toolsPath);
}

// ============================================================================
// Metadata Writing
// ============================================================================

bool OdbWriter::writeMetadata(const Metadata& metadata,
                               const std::filesystem::path& metadataPath) {
    std::ostringstream out;

    out << "#\n";
    out << "# Metadata\n";
    out << "#\n\n";

    out << "CREATION_DATE=" << metadata.creationDate << "\n";
    out << "MODIFICATION_DATE=" << metadata.modificationDate << "\n";
    out << "SOURCE=" << metadata.source << "\n";
    out << "VERSION=" << metadata.version << "\n";

    return writePlainFile(out.str(), metadataPath);
}

// ============================================================================
// Variants Writing
// ============================================================================

bool OdbWriter::writeVariants(const std::vector<ComponentVariant>& variants,
                               const std::filesystem::path& variantsPath) {
    std::ostringstream out;

    out << "#\n";
    out << "# Component Variants\n";
    out << "#\n\n";

    for (const auto& variant : variants) {
        out << "VARIANT " << variant.name << " {\n";

        if (!variant.description.empty()) {
            out << "   DESCRIPTION=" << variant.description << "\n";
        }

        for (const auto& entry : variant.components) {
            out << "   COMPONENT " << entry.refDes << " {\n";
            out << "      STATE=" << variantStateToString(entry.state) << "\n";

            if (!entry.alternatePartNumber.empty()) {
                out << "      ALT_PART=" << entry.alternatePartNumber << "\n";
            }

            out << "   }\n";
        }

        out << "}\n\n";
    }

    return writePlainFile(out.str(), variantsPath);
}

// ============================================================================
// Embedded Components Writing
// ============================================================================

bool OdbWriter::writeEmbeddedComponents(const std::vector<EmbeddedComponent>& components,
                                         const std::filesystem::path& embeddedPath) {
    std::ostringstream out;

    out << "#\n";
    out << "# Embedded Components\n";
    out << "#\n\n";

    for (const auto& comp : components) {
        out << "EMBEDDED " << comp.name << " {\n";
        out << "   TYPE=" << embeddedTypeToString(comp.type) << "\n";
        out << "   VALUE=" << formatDouble(comp.value) << "\n";
        out << "   LAYER=" << comp.layerName << "\n";
        out << "   X=" << formatDouble(comp.x) << "\n";
        out << "   Y=" << formatDouble(comp.y) << "\n";

        if (comp.width > 0) {
            out << "   WIDTH=" << formatDouble(comp.width) << "\n";
        }
        if (comp.height > 0) {
            out << "   HEIGHT=" << formatDouble(comp.height) << "\n";
        }

        out << "}\n\n";
    }

    return writePlainFile(out.str(), embeddedPath);
}

// ============================================================================
// Buildup Writing
// ============================================================================

bool OdbWriter::writeBuildup(const BuildupInfo& buildup,
                              const std::filesystem::path& buildupPath) {
    std::ostringstream out;

    out << "#\n";
    out << "# Build-up Information\n";
    out << "#\n\n";

    out << "NAME=" << buildup.name << "\n";
    out << "TOTAL_LAYERS=" << buildup.totalLayers << "\n";
    out << "TOTAL_THICKNESS=" << formatDouble(buildup.totalThickness) << "\n\n";

    for (const auto& span : buildup.viaSpans) {
        out << "VIA_SPAN " << span.name << " {\n";
        out << "   TYPE=" << viaSpanTypeToString(span.type) << "\n";
        out << "   START_LAYER=" << span.startLayer << "\n";
        out << "   END_LAYER=" << span.endLayer << "\n";

        if (!span.startLayerName.empty()) {
            out << "   START_LAYER_NAME=" << span.startLayerName << "\n";
        }

        if (!span.endLayerName.empty()) {
            out << "   END_LAYER_NAME=" << span.endLayerName << "\n";
        }

        out << "}\n\n";
    }

    return writePlainFile(out.str(), buildupPath);
}

// ============================================================================
// VPL Writing
// ============================================================================

bool OdbWriter::writeVpl(const std::vector<VendorPart>& parts,
                          const std::filesystem::path& vplPath) {
    std::ostringstream out;

    out << "#\n";
    out << "# Vendor Part List\n";
    out << "#\n\n";

    for (const auto& part : parts) {
        out << "VENDOR_PART " << part.partNumber << " {\n";
        out << "   VENDOR=" << part.vendorName << "\n";

        if (!part.description.empty()) {
            out << "   DESCRIPTION=" << part.description << "\n";
        }

        if (!part.datasheet.empty()) {
            out << "   DATASHEET=" << part.datasheet << "\n";
        }

        out << "}\n\n";
    }

    return writePlainFile(out.str(), vplPath);
}

// ============================================================================
// Customer Info Writing
// ============================================================================

bool OdbWriter::writeCustomerInfo(const CustomerInfo& info,
                                   const std::filesystem::path& infoPath) {
    std::ostringstream out;

    out << "#\n";
    out << "# Customer Information\n";
    out << "#\n\n";

    out << "NAME=" << info.name << "\n";

    if (!info.company.empty()) {
        out << "COMPANY=" << info.company << "\n";
    }

    if (!info.projectName.empty()) {
        out << "PROJECT_NAME=" << info.projectName << "\n";
    }

    if (!info.projectNumber.empty()) {
        out << "PROJECT_NUMBER=" << info.projectNumber << "\n";
    }

    if (!info.revision.empty()) {
        out << "REVISION=" << info.revision << "\n";
    }

    return writePlainFile(out.str(), infoPath);
}

// ============================================================================
// Attribute List Writing
// ============================================================================

bool OdbWriter::writeAttrList(const AttributeList& attrs,
                               const std::filesystem::path& attrPath) {
    std::ostringstream out;

    out << "#\n";
    out << "# Attributes\n";
    out << "#\n\n";

    for (const auto& [key, value] : attrs) {
        out << key << "=" << value << "\n";
    }

    return writePlainFile(out.str(), attrPath);
}

// ============================================================================
// Utility Functions
// ============================================================================

std::vector<uint8_t> OdbWriter::compressData(const std::string& data) {
    std::vector<uint8_t> compressed;

    uLongf compressedSize = compressBound(static_cast<uLong>(data.size()));
    compressed.resize(compressedSize);

    int result = compress2(
        compressed.data(),
        &compressedSize,
        reinterpret_cast<const Bytef*>(data.data()),
        static_cast<uLong>(data.size()),
        options_.compressionLevel
    );

    if (result != Z_OK) {
        throw std::runtime_error("zlib compression failed");
    }

    compressed.resize(compressedSize);
    return compressed;
}

bool OdbWriter::writeCompressedFile(const std::string& content,
                                     const std::filesystem::path& filePath) {
    try {
        auto compressed = compressData(content);

        std::ofstream file(filePath, std::ios::binary);
        if (!file) {
            lastError_ = "Failed to open file for writing: " + filePath.string();
            return false;
        }

        file.write(reinterpret_cast<const char*>(compressed.data()),
                   static_cast<std::streamsize>(compressed.size()));

        if (!file.good()) {
            lastError_ = "Failed to write compressed data to file: " + filePath.string();
            return false;
        }

        return true;

    } catch (const std::exception& e) {
        lastError_ = "Failed to write compressed file: " + std::string(e.what());
        return false;
    }
}

bool OdbWriter::writePlainFile(const std::string& content,
                                const std::filesystem::path& filePath) {
    try {
        std::ofstream file(filePath);
        if (!file) {
            lastError_ = "Failed to open file for writing: " + filePath.string();
            return false;
        }

        file << content;

        if (!file.good()) {
            lastError_ = "Failed to write data to file: " + filePath.string();
            return false;
        }

        return true;

    } catch (const std::exception& e) {
        lastError_ = "Failed to write file: " + std::string(e.what());
        return false;
    }
}

void OdbWriter::reportProgress(const std::string& message, double progress) {
    if (progressCallback_) {
        progressCallback_(message, progress);
    }
}

std::string OdbWriter::formatDouble(double value, int precision) const {
    std::ostringstream out;
    out << std::fixed << std::setprecision(precision) << value;

    // Remove trailing zeros
    std::string result = out.str();
    size_t dotPos = result.find('.');
    if (dotPos != std::string::npos) {
        size_t lastNonZero = result.find_last_not_of('0');
        if (lastNonZero > dotPos) {
            result = result.substr(0, lastNonZero + 1);
        } else {
            result = result.substr(0, dotPos);
        }
    }

    return result;
}

} // namespace koo::ecad
