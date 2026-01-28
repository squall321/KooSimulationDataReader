#include <koo/ecad/OdbReader.hpp>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <regex>
#include <stdexcept>
#include <zlib.h>
#include <vector>

namespace koo::ecad {

namespace {

// Helper to trim whitespace
std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, last - first + 1);
}

// Helper to convert string to uppercase
std::string toUpper(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::toupper(c); });
    return result;
}

// Helper to check if file exists
bool fileExists(const std::filesystem::path& path) {
    return std::filesystem::exists(path);
}

// Helper to check for compressed file
std::filesystem::path findFile(const std::filesystem::path& basePath,
                               const std::string& filename) {
    auto plain = basePath / filename;
    if (fileExists(plain)) return plain;

    auto compressed = basePath / (filename + ".z");
    if (fileExists(compressed)) return compressed;

    return plain;  // Return plain path even if not found
}

} // anonymous namespace

// ============================================================================
// Main Read Functions
// ============================================================================

OdbJob OdbReader::read(const std::filesystem::path& odbPath) {
    return read(odbPath, Options());
}

OdbJob OdbReader::read(const std::filesystem::path& odbPath, const Options& options) {
    lastError_.clear();
    options_ = options;

    OdbJob job;

    try {
        // Validate path
        if (!std::filesystem::exists(odbPath)) {
            lastError_ = "ODB++ path does not exist: " + odbPath.string();
            throw std::runtime_error(lastError_);
        }

        // Get job name from directory name
        job.setName(odbPath.filename().string());
        job.setSourcePath(odbPath);

        reportProgress("Reading matrix...", 0.0);

        // Parse matrix
        auto matrixPath = odbPath / "matrix" / "matrix";
        if (fileExists(matrixPath)) {
            parseMatrix(job, matrixPath);
        }

        // Parse misc/info
        auto infoPath = odbPath / "misc" / "info";
        if (fileExists(infoPath)) {
            parseJobInfo(job, infoPath);
        }

        // Parse misc/attrlist
        auto attrPath = odbPath / "misc" / "attrlist";
        if (fileExists(attrPath)) {
            parseJobAttributes(job, attrPath);
        }

        // Get step names
        auto stepNames = job.getMatrix().getStepNames();
        if (stepNames.empty()) {
            // Try to list directories in steps folder
            auto stepsDir = odbPath / "steps";
            if (std::filesystem::exists(stepsDir)) {
                for (const auto& entry : std::filesystem::directory_iterator(stepsDir)) {
                    if (entry.is_directory()) {
                        stepNames.push_back(entry.path().filename().string());
                    }
                }
            }
        }

        // Filter steps if specified
        if (!options_.stepFilter.empty()) {
            std::vector<std::string> filtered;
            for (const auto& name : stepNames) {
                if (std::find(options_.stepFilter.begin(), options_.stepFilter.end(), name)
                    != options_.stepFilter.end()) {
                    filtered.push_back(name);
                }
            }
            stepNames = filtered;
        }

        // Parse each step
        double stepProgress = 0.1;
        double stepIncrement = 0.7 / static_cast<double>(std::max(size_t(1), stepNames.size()));

        for (const auto& stepName : stepNames) {
            reportProgress("Reading step: " + stepName, stepProgress);

            auto stepPath = odbPath / "steps" / stepName;

            // Try case-insensitive match if exact path doesn't exist
            if (!std::filesystem::exists(stepPath)) {
                std::string lowerStepName = stepName;
                std::transform(lowerStepName.begin(), lowerStepName.end(),
                               lowerStepName.begin(), ::tolower);
                stepPath = odbPath / "steps" / lowerStepName;
            }

            if (std::filesystem::exists(stepPath)) {
                auto step = std::make_unique<Step>(stepName);
                parseStep(*step, stepPath);
                job.addStep(std::move(step));
            }

            stepProgress += stepIncrement;
        }

        // Parse symbols if requested
        if (options_.loadSymbols) {
            reportProgress("Reading symbols...", 0.8);

            auto symbolsDir = odbPath / "symbols";
            if (std::filesystem::exists(symbolsDir)) {
                for (const auto& entry : std::filesystem::directory_iterator(symbolsDir)) {
                    if (entry.is_directory()) {
                        auto symbol = std::make_unique<Symbol>(entry.path().filename().string());
                        parseSymbol(*symbol, entry.path());
                        job.addSymbol(std::move(symbol));
                    }
                }
            }
        }

        reportProgress("Reading stackup...", 0.85);

        // Parse stackup (misc/stackup or misc/stackup.xml)
        auto stackupPath = odbPath / "misc" / "stackup";
        if (!fileExists(stackupPath)) {
            stackupPath = odbPath / "misc" / "stackup.xml";
        }
        if (fileExists(stackupPath)) {
            parseStackup(job.getStackup(), stackupPath);
        }

        // Parse impedance constraints (misc/impedance or misc/impedance.xml)
        auto impedancePath = odbPath / "misc" / "impedance";
        if (!fileExists(impedancePath)) {
            impedancePath = odbPath / "misc" / "impedance.xml";
        }
        if (fileExists(impedancePath)) {
            parseImpedance(job.getImpedanceConstraints(), impedancePath);
        }

        // Parse drill tools (misc/tools)
        auto toolsPath = odbPath / "misc" / "tools";
        if (fileExists(toolsPath)) {
            parseTools(job.getDrillTools(), toolsPath);
        }

        reportProgress("Reading additional data...", 0.9);

        // Parse intentional shorts (misc/shortf)
        auto shortfPath = odbPath / "misc" / "shortf";
        if (fileExists(shortfPath)) {
            parseShortf(const_cast<std::vector<IntentionalShort>&>(job.getIntentionalShorts()), shortfPath);
        }

        // Parse fonts directory
        auto fontsDir = odbPath / "fonts";
        if (std::filesystem::exists(fontsDir)) {
            for (const auto& entry : std::filesystem::directory_iterator(fontsDir)) {
                if (entry.is_directory()) {
                    parseFont(entry.path());
                }
            }
        }

        // Parse metadata (misc/metadata or misc/metadata.xml)
        auto metadataPath = odbPath / "misc" / "metadata";
        if (!fileExists(metadataPath)) {
            metadataPath = odbPath / "misc" / "metadata.xml";
        }
        if (fileExists(metadataPath)) {
            parseMetadata(job.getMetadata(), metadataPath);
        }

        // Parse component variants (misc/variants)
        auto variantsPath = odbPath / "misc" / "variants";
        if (fileExists(variantsPath)) {
            parseVariants(job.getVariants(), variantsPath);
        }

        // Parse embedded components (misc/embedded or misc/embedded_passives)
        auto embeddedPath = odbPath / "misc" / "embedded";
        if (!fileExists(embeddedPath)) {
            embeddedPath = odbPath / "misc" / "embedded_passives";
        }
        if (fileExists(embeddedPath)) {
            parseEmbeddedComponents(job.getEmbeddedComponents(), embeddedPath);
        }

        // Parse build-up information (misc/buildup)
        auto buildupPath = odbPath / "misc" / "buildup";
        if (fileExists(buildupPath)) {
            parseBuildup(job.getBuildupInfo(), buildupPath);
        }

        // Parse VPL - Vendor Part List (misc/vpl)
        auto vplPath = odbPath / "misc" / "vpl";
        if (fileExists(vplPath)) {
            parseVpl(job.getVendorParts(), vplPath);
        }

        // Parse customer information (misc/customer)
        auto customerPath = odbPath / "misc" / "customer";
        if (fileExists(customerPath)) {
            parseCustomerInfo(job.getCustomerInfo(), customerPath);
        }

        reportProgress("Complete", 1.0);

    } catch (const std::exception& e) {
        if (lastError_.empty()) {
            lastError_ = e.what();
        }
        throw;
    }

    return job;
}

LayerMatrix OdbReader::readMatrix(const std::filesystem::path& odbPath) {
    OdbJob tempJob;
    auto matrixPath = odbPath / "matrix" / "matrix";
    if (fileExists(matrixPath)) {
        parseMatrix(tempJob, matrixPath);
    }
    return tempJob.getMatrix();
}

std::vector<std::string> OdbReader::listSteps(const std::filesystem::path& odbPath) {
    std::vector<std::string> steps;

    auto stepsDir = odbPath / "steps";
    if (std::filesystem::exists(stepsDir)) {
        for (const auto& entry : std::filesystem::directory_iterator(stepsDir)) {
            if (entry.is_directory()) {
                steps.push_back(entry.path().filename().string());
            }
        }
    }

    return steps;
}

std::unique_ptr<Step> OdbReader::readStep(const std::filesystem::path& odbPath,
                                          const std::string& stepName) {
    auto stepPath = odbPath / "steps" / stepName;
    if (!std::filesystem::exists(stepPath)) {
        lastError_ = "Step not found: " + stepName;
        return nullptr;
    }

    auto step = std::make_unique<Step>(stepName);
    parseStep(*step, stepPath);
    return step;
}

std::unique_ptr<Layer> OdbReader::readLayer(const std::filesystem::path& odbPath,
                                            const std::string& stepName,
                                            const std::string& layerName) {
    auto layerPath = odbPath / "steps" / stepName / "layers" / layerName;
    if (!std::filesystem::exists(layerPath)) {
        lastError_ = "Layer not found: " + stepName + "/" + layerName;
        return nullptr;
    }

    auto layer = std::make_unique<Layer>(layerName);
    parseLayer(*layer, layerPath);
    return layer;
}

// ============================================================================
// Parsing Functions
// ============================================================================

void OdbReader::parseMatrix(OdbJob& job, const std::filesystem::path& matrixPath) {
    std::string content = readFileContents(matrixPath);
    if (content.empty()) return;

    std::istringstream stream(content);
    std::string line;
    bool inStepArray = false;
    bool inLayerArray = false;

    LayerDefinition currentLayer;
    LayerMatrix::StepDefinition currentStep;

    while (std::getline(stream, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;

        // Check for array markers
        if (line == "STEP {") {
            inStepArray = true;
            currentStep = LayerMatrix::StepDefinition();
            continue;
        }
        if (line == "LAYER {") {
            inLayerArray = true;
            currentLayer = LayerDefinition();
            continue;
        }
        if (line == "}") {
            if (inStepArray) {
                job.getMatrix().addStep(currentStep);
                inStepArray = false;
            }
            if (inLayerArray) {
                job.getMatrix().addLayer(currentLayer);
                inLayerArray = false;
            }
            continue;
        }

        // Parse key=value
        auto eqPos = line.find('=');
        if (eqPos == std::string::npos) continue;

        std::string key = trim(line.substr(0, eqPos));
        std::string value = trim(line.substr(eqPos + 1));

        // Remove quotes
        if (value.size() >= 2 && value.front() == '\'' && value.back() == '\'') {
            value = value.substr(1, value.size() - 2);
        }

        if (inStepArray) {
            if (key == "NAME") currentStep.name = value;
            else if (key == "COL") currentStep.col = std::stoi(value);
        }
        else if (inLayerArray) {
            if (key == "NAME") currentLayer.name = value;
            else if (key == "ROW") currentLayer.row = std::stoi(value);
            else if (key == "CONTEXT") {
                if (value == "BOARD") currentLayer.context = LayerContext::Board;
                else if (value == "MISC") currentLayer.context = LayerContext::Misc;
            }
            else if (key == "TYPE") {
                std::string upper = toUpper(value);
                if (upper == "SIGNAL") currentLayer.type = LayerType::Signal;
                else if (upper == "POWER_GROUND" || upper == "PLANE") currentLayer.type = LayerType::PowerGround;
                else if (upper == "MIXED") currentLayer.type = LayerType::Mixed;
                else if (upper == "SOLDER_MASK") currentLayer.type = LayerType::SolderMask;
                else if (upper == "SILK_SCREEN") currentLayer.type = LayerType::SilkScreen;
                else if (upper == "SOLDER_PASTE") currentLayer.type = LayerType::SolderPaste;
                else if (upper == "DRILL") currentLayer.type = LayerType::Drill;
                else if (upper == "ROUT") currentLayer.type = LayerType::Rout;
                else if (upper == "DOCUMENT") currentLayer.type = LayerType::Document;
                else if (upper == "COMPONENT") currentLayer.type = LayerType::Component;
            }
            else if (key == "POLARITY") {
                currentLayer.polarity = (value == "POSITIVE") ? Polarity::Positive : Polarity::Negative;
            }
            else if (key == "START_NAME" && !value.empty()) {
                try {
                    currentLayer.startName = std::stoi(value);
                } catch (const std::exception&) {
                    // Invalid integer, keep default value
                }
            }
            else if (key == "END_NAME" && !value.empty()) {
                try {
                    currentLayer.endName = std::stoi(value);
                } catch (const std::exception&) {
                    // Invalid integer, keep default value
                }
            }
        }
    }
}

void OdbReader::parseJobInfo(OdbJob& job, const std::filesystem::path& infoPath) {
    std::string content = readFileContents(infoPath);
    auto pairs = parseStructuredFile(content);

    JobInfo& info = job.getInfo();
    if (pairs.count("UNITS")) info.units = pairs["UNITS"];
    if (pairs.count("CREATION_DATE")) info.creationDate = pairs["CREATION_DATE"];
    if (pairs.count("SAVE_DATE")) info.modificationDate = pairs["SAVE_DATE"];
    if (pairs.count("SAVE_APP")) info.saveApp = pairs["SAVE_APP"];
    if (pairs.count("SAVE_USER")) info.saveUser = pairs["SAVE_USER"];
}

void OdbReader::parseJobAttributes(OdbJob& job, const std::filesystem::path& attrPath) {
    parseAttrList(const_cast<AttributeList&>(job.getAttributes()), attrPath);
}

void OdbReader::parseStep(Step& step, const std::filesystem::path& stepPath) {
    // Parse step header
    auto stephdrPath = stepPath / "stephdr";
    if (fileExists(stephdrPath)) {
        parseStepHeader(step, stephdrPath);
    }

    // Parse profile
    auto profilePath = findFile(stepPath, "profile");
    if (fileExists(profilePath)) {
        parseProfile(step.getProfile(), profilePath);
    }

    // Parse attributes
    auto attrPath = stepPath / "attrlist";
    if (fileExists(attrPath)) {
        AttributeList attrs;
        parseAttrList(attrs, attrPath);
        for (const auto& pair : attrs) {
            step.setAttribute(pair.first, pair.second);
        }
    }

    // Parse layers
    auto layersDir = stepPath / "layers";
    if (std::filesystem::exists(layersDir)) {
        for (const auto& entry : std::filesystem::directory_iterator(layersDir)) {
            if (entry.is_directory()) {
                std::string layerName = entry.path().filename().string();

                // Check layer filter
                if (!options_.layerFilter.empty() &&
                    std::find(options_.layerFilter.begin(), options_.layerFilter.end(), layerName)
                    == options_.layerFilter.end()) {
                    continue;
                }

                auto layer = std::make_unique<Layer>(layerName);
                parseLayer(*layer, entry.path());
                step.addLayer(std::move(layer));
            }
        }
    }

    // Parse EDA data
    if (options_.loadEdaData) {
        auto edaPath = stepPath / "eda" / "data";
        if (fileExists(edaPath)) {
            parseEdaData(step.getEdaData(), edaPath);
            step.setHasEdaData(true);
        }

        // Parse BOM (eda/bom or eda/bom.csv)
        auto bomPath = stepPath / "eda" / "bom";
        if (!fileExists(bomPath)) {
            bomPath = stepPath / "eda" / "bom.csv";
        }
        if (fileExists(bomPath)) {
            parseBom(step.getEdaData(), bomPath);
        }

        // Parse netlist (eda/cadnet or eda/refnet)
        auto cadnetPath = stepPath / "eda" / "cadnet";
        if (fileExists(cadnetPath)) {
            parseNetlist(step.getEdaData(), cadnetPath);
        }
        auto refnetPath = stepPath / "eda" / "refnet";
        if (fileExists(refnetPath)) {
            parseNetlist(step.getEdaData(), refnetPath);
        }

        // Parse HDI netlist (eda/hdi)
        auto hdiPath = stepPath / "eda" / "hdi";
        if (fileExists(hdiPath)) {
            parseHdiNetlist(step.getEdaData(), hdiPath);
        }
    }

    // Parse zones (step-level zones)
    auto zonesPath = stepPath / "zones";
    if (fileExists(zonesPath)) {
        parseZones(step.getZones(), zonesPath);
    }

    // Parse dimensions (step-level dimensions)
    auto dimsPath = stepPath / "dimensions";
    if (fileExists(dimsPath)) {
        parseDimensions(step.getDimensions(), dimsPath);
    }
}

void OdbReader::parseStepHeader(Step& step, const std::filesystem::path& stephdrPath) {
    std::string content = readFileContents(stephdrPath);
    std::istringstream stream(content);
    std::string line;

    while (std::getline(stream, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;

        auto tokens = tokenize(line);
        if (tokens.empty()) continue;

        // Parse key=value pairs
        size_t eqPos = line.find('=');
        if (eqPos != std::string::npos) {
            std::string key = trim(line.substr(0, eqPos));
            std::string value = trim(line.substr(eqPos + 1));

            if (key == "X_DATUM" && !value.empty()) {
                step.setXDatum(value[0]);
            } else if (key == "Y_DATUM" && !value.empty()) {
                step.setYDatum(value[0]);
            } else if (key == "X_ORIGIN") {
                step.setDatum(std::stod(value), step.getDatum().y);
            } else if (key == "Y_ORIGIN") {
                step.setDatum(step.getDatum().x, std::stod(value));
            } else if (key == "AFFECT_HOLES") {
                step.setAffectHoles(toUpper(value) == "YES");
            }
        }
        // Parse STEP_REPEAT records
        // Format: STEP_REPEAT <name> <x> <y> [dx dy nx ny] [angle] [mirror]
        else if (tokens[0] == "STEP_REPEAT" && tokens.size() >= 4) {
            StepRepeat sr;
            sr.stepName = tokens[1];
            sr.x = std::stod(tokens[2]);
            sr.y = std::stod(tokens[3]);

            // Optional array parameters
            if (tokens.size() >= 8) {
                sr.dx = std::stod(tokens[4]);
                sr.dy = std::stod(tokens[5]);
                sr.nx = std::stoi(tokens[6]);
                sr.ny = std::stoi(tokens[7]);
            }
            // Optional rotation
            if (tokens.size() >= 9) {
                sr.angle = std::stod(tokens[8]);
            }
            // Optional mirror
            if (tokens.size() >= 10) {
                sr.mirror = (toUpper(tokens[9]) == "Y" || toUpper(tokens[9]) == "YES");
            }

            step.addStepRepeat(sr);

            // If this step has step repeats, it's a panel
            step.setType(StepType::Panel);
        }
    }
}

void OdbReader::parseProfile(std::vector<Contour>& profile, const std::filesystem::path& profilePath) {
    std::string content = readFileContents(profilePath);
    std::istringstream stream(content);
    std::string line;

    std::unique_ptr<Contour> currentContour;

    while (std::getline(stream, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;

        auto tokens = tokenize(line);
        if (tokens.empty()) continue;

        if (tokens[0] == "OB" && tokens.size() >= 4) {
            // Begin polygon: OB <x> <y> <type>
            double x = std::stod(tokens[1]);
            double y = std::stod(tokens[2]);
            PolygonType type = (tokens[3] == "H") ? PolygonType::Hole : PolygonType::Island;
            currentContour = std::make_unique<Contour>(x, y, type);
        }
        else if (tokens[0] == "OS" && tokens.size() >= 3 && currentContour) {
            // Segment: OS <x> <y>
            double x = std::stod(tokens[1]);
            double y = std::stod(tokens[2]);
            currentContour->addLineSegment(x, y);
        }
        else if (tokens[0] == "OC" && tokens.size() >= 6 && currentContour) {
            // Arc: OC <xe> <ye> <xc> <yc> <cw>
            double xe = std::stod(tokens[1]);
            double ye = std::stod(tokens[2]);
            double xc = std::stod(tokens[3]);
            double yc = std::stod(tokens[4]);
            bool cw = (tokens[5] == "Y");
            currentContour->addArcSegment(xe, ye, xc, yc, cw);
        }
        else if (tokens[0] == "OE" && currentContour) {
            // End polygon
            profile.push_back(std::move(*currentContour));
            currentContour.reset();
        }
    }
}

void OdbReader::parseLayer(Layer& layer, const std::filesystem::path& layerPath) {
    // Parse attributes
    auto attrPath = layerPath / "attrlist";
    if (fileExists(attrPath)) {
        AttributeList attrs;
        parseAttrList(attrs, attrPath);
        for (const auto& pair : attrs) {
            layer.setAttribute(pair.first, pair.second);
        }
    }

    // Parse features
    if (options_.loadFeatures) {
        auto featuresPath = findFile(layerPath, "features");
        if (fileExists(featuresPath)) {
            parseFeatures(layer, featuresPath);
        }
    }
}

void OdbReader::parseFeatures(Layer& layer, const std::filesystem::path& featuresPath) {
    std::string content = readFileContents(featuresPath);
    std::istringstream stream(content);
    std::string line;

    std::vector<std::string> symbolNames;
    std::vector<std::string> attrNames;
    std::vector<std::string> attrStrings;

    while (std::getline(stream, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;

        // Units
        if (line.find("UNITS=") == 0) {
            layer.setUnits(line.substr(6));
            continue;
        }

        // Symbol names section
        if (line[0] == '$') {
            // $<num> <name>
            auto tokens = tokenize(line);
            if (tokens.size() >= 2) {
                std::string numStr = tokens[0].substr(1);
                int num = std::stoi(numStr);
                std::string name = tokens[1];

                // Ensure vector is large enough
                if (num >= static_cast<int>(symbolNames.size())) {
                    symbolNames.resize(static_cast<size_t>(num) + 1);
                }
                symbolNames[static_cast<size_t>(num)] = name;
            }
            continue;
        }

        // Attribute names section
        if (line[0] == '@') {
            // @<num> <name>
            auto tokens = tokenize(line);
            if (tokens.size() >= 2) {
                std::string numStr = tokens[0].substr(1);
                int num = std::stoi(numStr);
                std::string name = tokens[1];

                if (num >= static_cast<int>(attrNames.size())) {
                    attrNames.resize(static_cast<size_t>(num) + 1);
                }
                attrNames[static_cast<size_t>(num)] = name;
            }
            continue;
        }

        // Attribute text strings
        if (line[0] == '&') {
            // &<num> <string>
            auto pos = line.find(' ');
            if (pos != std::string::npos) {
                std::string numStr = line.substr(1, pos - 1);
                int num = std::stoi(numStr);
                std::string text = line.substr(pos + 1);

                if (num >= static_cast<int>(attrStrings.size())) {
                    attrStrings.resize(static_cast<size_t>(num) + 1);
                }
                attrStrings[static_cast<size_t>(num)] = text;
            }
            continue;
        }

        // Parse features
        if (line[0] == 'L' || line[0] == 'P' || line[0] == 'A' ||
            line[0] == 'T' || line[0] == 'B') {
            auto feature = parseFeatureLine(line, symbolNames);
            if (feature) {
                layer.addFeature(std::move(feature));
            }
        }
        else if (line[0] == 'S' && line.find("SE") != 0) {
            // Surface feature - need to read until SE
            auto feature = parseSurfaceFeature(stream, line);
            if (feature) {
                layer.addFeature(std::move(feature));
            }
        }
    }

    layer.setSymbolNames(symbolNames);
}

void OdbReader::parseEdaData(EdaData& eda, const std::filesystem::path& edaPath) {
    std::string content = readFileContents(edaPath);
    std::istringstream stream(content);
    std::string line;

    std::unique_ptr<Net> currentNet;
    std::unique_ptr<Package> currentPackage;
    std::unique_ptr<Component> currentComponent;
    int currentNetNum = -1;
    int currentSubnetNum = -1;
    std::string currentNetName;

    // Attribute name/text lookup tables
    std::vector<std::string> attrNames;
    std::vector<std::string> attrTexts;

    while (std::getline(stream, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;

        auto tokens = tokenize(line);
        if (tokens.empty()) continue;

        // ========== Header Section ==========

        // Attribute names: @<num> <name>
        if (line[0] == '@') {
            size_t spacePos = line.find(' ');
            if (spacePos != std::string::npos) {
                int num = std::stoi(line.substr(1, spacePos - 1));
                std::string name = trim(line.substr(spacePos + 1));
                if (num >= static_cast<int>(attrNames.size())) {
                    attrNames.resize(static_cast<size_t>(num) + 1);
                }
                attrNames[static_cast<size_t>(num)] = name;
            }
            continue;
        }

        // Attribute text strings: &<num> <text>
        if (line[0] == '&') {
            size_t spacePos = line.find(' ');
            if (spacePos != std::string::npos) {
                int num = std::stoi(line.substr(1, spacePos - 1));
                std::string text = line.substr(spacePos + 1);
                if (num >= static_cast<int>(attrTexts.size())) {
                    attrTexts.resize(static_cast<size_t>(num) + 1);
                }
                attrTexts[static_cast<size_t>(num)] = text;
            }
            continue;
        }

        // Layer names: LYR <name>
        if (tokens[0] == "LYR" && tokens.size() >= 2) {
            eda.addLayerName(tokens[1]);
        }

        // ========== NET Records ==========

        // Net: NET <name>
        else if (tokens[0] == "NET" && tokens.size() >= 2) {
            if (currentNet) {
                eda.addNet(std::move(currentNet));
            }
            currentNetName = tokens[1];
            currentNet = std::make_unique<Net>(currentNetName);
            currentNetNum++;
            currentNet->setNetNumber(currentNetNum);
            currentSubnetNum = -1;
        }

        // Subnet: SNT <type> ...
        else if (tokens[0] == "SNT" && tokens.size() >= 2 && currentNet) {
            currentSubnetNum++;
            Subnet subnet;

            std::string subnetType = toUpper(tokens[1]);
            if (subnetType == "TOP" || subnetType == "T") {
                subnet.type = SubnetType::Toeprint;
                // SNT TOP <side> <comp_num> <toe_num>
                if (tokens.size() >= 5) {
                    subnet.side = tokens[2][0];
                    subnet.componentNum = std::stoi(tokens[3]);
                    subnet.toeprintNum = std::stoi(tokens[4]);
                }
            }
            else if (subnetType == "VIA" || subnetType == "V") {
                subnet.type = SubnetType::Via;
            }
            else if (subnetType == "TRC" || subnetType == "TR") {
                subnet.type = SubnetType::Trace;
            }
            else if (subnetType == "PLN" || subnetType == "P") {
                subnet.type = SubnetType::Plane;
                // SNT PLN <fill_type> <cutout_type> <fill_size>
                if (tokens.size() >= 3) {
                    if (tokens[2] == "S") subnet.fillType = PlaneFillType::Solid;
                    else subnet.fillType = PlaneFillType::Outline;
                }
                if (tokens.size() >= 4) {
                    char ct = tokens[3][0];
                    switch (ct) {
                        case 'C': subnet.cutoutType = PlaneCutoutType::Circle; break;
                        case 'R': subnet.cutoutType = PlaneCutoutType::Rectangle; break;
                        case 'O': subnet.cutoutType = PlaneCutoutType::Octagon; break;
                        default: subnet.cutoutType = PlaneCutoutType::Exact; break;
                    }
                }
                if (tokens.size() >= 5) {
                    subnet.fillSize = std::stod(tokens[4]);
                }
            }

            eda.addSubnet(currentNetName, subnet);
        }

        // Feature ID: FID <type> <layer_num> <feature_num>
        else if (tokens[0] == "FID" && tokens.size() >= 4) {
            EdaData::FeatureIdRecord record;
            record.featureId.type = tokens[1][0];
            record.featureId.layerNum = std::stoi(tokens[2]);
            record.featureId.featureNum = std::stoi(tokens[3]);
            record.netNum = currentNetNum;
            record.subnetNum = currentSubnetNum;
            eda.addFeatureIdRecord(record);
        }

        // ========== PKG Records ==========

        // Package: PKG <name> [<pitch> <xmin> <ymin> <xmax> <ymax>]
        else if (tokens[0] == "PKG" && tokens.size() >= 2) {
            if (currentPackage) {
                eda.addPackage(std::move(currentPackage));
            }
            currentPackage = std::make_unique<Package>(tokens[1]);
            if (tokens.size() >= 3) {
                currentPackage->setPitch(std::stod(tokens[2]));
            }
            if (tokens.size() >= 7) {
                BoundingBox2D box;
                box.min.x = std::stod(tokens[3]);
                box.min.y = std::stod(tokens[4]);
                box.max.x = std::stod(tokens[5]);
                box.max.y = std::stod(tokens[6]);
                currentPackage->setBoundingBox(box);
            }
        }

        // Pin: PIN <name> <type> <x> <y> [<f_l> <e_l> <m_l>]
        else if (tokens[0] == "PIN" && tokens.size() >= 5 && currentPackage) {
            Pin pin;
            pin.name = tokens[1];
            // Type: T=TH, S=SMD, B=BGA, F=Fiducial
            if (tokens[2] == "T") pin.type = PinType::ThroughHole;
            else if (tokens[2] == "S") pin.type = PinType::Smd;
            else if (tokens[2] == "B") pin.type = PinType::Bga;
            else if (tokens[2] == "F") pin.type = PinType::Fiducial;
            pin.x = std::stod(tokens[3]);
            pin.y = std::stod(tokens[4]);
            if (tokens.size() >= 8) {
                pin.featureLayerIndex = std::stoi(tokens[5]);
                pin.electricalLayerIndex = std::stoi(tokens[6]);
                pin.mechanicalLayerIndex = std::stoi(tokens[7]);
            }
            currentPackage->addPin(pin);
        }

        // ========== CMP (Component) Records ==========

        // Component: CMP <pkg_ref> <x> <y> <rotation> <mirror> <comp_name> [<part_name>]
        else if (tokens[0] == "CMP" && tokens.size() >= 7) {
            if (currentComponent) {
                eda.addComponent(std::move(currentComponent));
            }
            // comp_name is refdes
            std::string refDes = tokens[6];
            currentComponent = std::make_unique<Component>(refDes);
            currentComponent->setPackageIndex(std::stoi(tokens[1]));
            currentComponent->setPosition(std::stod(tokens[2]), std::stod(tokens[3]));
            currentComponent->setRotation(std::stod(tokens[4]));
            currentComponent->setMirrored(tokens[5] == "M" || tokens[5] == "Y");
            if (currentComponent->isMirrored()) {
                currentComponent->setSide(MountSide::Bottom);
            }
            if (tokens.size() >= 8) {
                currentComponent->setPartNumber(tokens[7]);
            }
        }

        // Property: PRP <name> '<value>'
        else if (tokens[0] == "PRP" && tokens.size() >= 2 && currentComponent) {
            // Find the property value (may be quoted)
            size_t propStart = line.find("PRP");
            if (propStart != std::string::npos) {
                size_t nameStart = propStart + 4;
                while (nameStart < line.size() && std::isspace(line[nameStart])) nameStart++;
                size_t nameEnd = nameStart;
                while (nameEnd < line.size() && !std::isspace(line[nameEnd])) nameEnd++;
                std::string propName = line.substr(nameStart, nameEnd - nameStart);

                // Find value
                size_t valStart = line.find('\'', nameEnd);
                if (valStart != std::string::npos) {
                    size_t valEnd = line.find('\'', valStart + 1);
                    if (valEnd != std::string::npos) {
                        std::string propValue = line.substr(valStart + 1, valEnd - valStart - 1);
                        currentComponent->setAttribute(propName, propValue);
                    }
                }
            }
        }

        // TOP (toeprint) record: TOP <toe_num> <x> <y> <rot> <mir> <net_num> <sn_num> <pin_name>
        else if (tokens[0] == "TOP" && tokens.size() >= 8 && currentComponent) {
            Toeprint toe;
            toe.pinNum = std::stoi(tokens[1]);
            toe.x = std::stod(tokens[2]);
            toe.y = std::stod(tokens[3]);
            toe.rotation = std::stod(tokens[4]);
            toe.mirror = (tokens[5] == "M" || tokens[5] == "Y");
            toe.netNum = std::stoi(tokens[6]);
            toe.subnetNum = std::stoi(tokens[7]);
            if (tokens.size() >= 9) {
                toe.name = tokens[8];
            }

            // Create pin from toeprint
            Pin pin;
            pin.name = toe.name.empty() ? std::to_string(toe.pinNum) : toe.name;
            pin.x = toe.x;
            pin.y = toe.y;
            pin.rotation = toe.rotation;
            pin.mirror = toe.mirror;
            // We'd need to look up net name from net number
            currentComponent->addPin(pin);
        }

        // ========== FGR (Feature Group) Records ==========

        // FGR <type> <id>
        else if (tokens[0] == "FGR" && tokens.size() >= 2) {
            EdaData::FeatureGroup group;
            group.type = tokens[1];
            eda.addFeatureGroup(group);
        }
    }

    // Add last items
    if (currentNet) {
        eda.addNet(std::move(currentNet));
    }
    if (currentPackage) {
        eda.addPackage(std::move(currentPackage));
    }
    if (currentComponent) {
        eda.addComponent(std::move(currentComponent));
    }

    // Store attribute names/texts
    eda.setNetAttributeNames(attrNames);
    eda.setNetAttributeStrings(attrTexts);
}

void OdbReader::parseSymbol(Symbol& symbol, const std::filesystem::path& symbolPath) {
    auto featuresPath = findFile(symbolPath, "features");
    if (fileExists(featuresPath)) {
        // Create a temporary layer to parse features
        Layer tempLayer;
        parseFeatures(tempLayer, featuresPath);

        // Move features to symbol
        for (auto& f : tempLayer.getFeatures()) {
            symbol.addFeature(std::move(f));
        }
    }
}

// ============================================================================
// Netlist Parsing (cadnet/refnet)
// ============================================================================

void OdbReader::parseNetlist(EdaData& eda, const std::filesystem::path& netlistPath) {
    std::string content = readFileContents(netlistPath);
    std::istringstream stream(content);
    std::string line;

    std::string currentNetName;
    std::unique_ptr<Net> currentNet;

    while (std::getline(stream, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;

        auto tokens = tokenize(line);
        if (tokens.empty()) continue;

        // Net: $<net_serial_number> <net_name>
        if (line[0] == '$' && tokens.size() >= 2) {
            // Save previous net
            if (currentNet) {
                eda.addNet(std::move(currentNet));
            }
            currentNetName = tokens[1];
            currentNet = std::make_unique<Net>(currentNetName);
            continue;
        }

        // Point: P <x> <y> <radius> <side> <epoint> <exp>
        if (tokens[0] == "P" && tokens.size() >= 4 && currentNet) {
            NetlistPoint point;
            point.netName = currentNetName;
            point.x = std::stod(tokens[1]);
            point.y = std::stod(tokens[2]);
            point.radius = std::stod(tokens[3]);
            if (tokens.size() >= 5) {
                point.side = tokens[4][0];
            }
            if (tokens.size() >= 6) {
                point.epoint = std::stoi(tokens[5]);
            }
            if (tokens.size() >= 7) {
                point.exp = std::stoi(tokens[6]);
            }
            // Store point as attribute
            std::string pointKey = "netpoint_" + std::to_string(currentNet->getPinCount());
            currentNet->setAttribute(pointKey,
                std::to_string(point.x) + "," + std::to_string(point.y));
        }

        // Staggered Point: ST <x> <y> <radius>
        else if (tokens[0] == "ST" && tokens.size() >= 4 && currentNet) {
            NetlistPoint point;
            point.netName = currentNetName;
            point.x = std::stod(tokens[1]);
            point.y = std::stod(tokens[2]);
            point.radius = std::stod(tokens[3]);
            point.staggered = true;
        }

        // Via: V <x> <y> <layer1> <layer2>
        else if (tokens[0] == "V" && tokens.size() >= 5 && currentNet) {
            Net::Subnet subnet;
            subnet.type = Net::Subnet::Type::Via;
            currentNet->addSubnet(subnet);
        }

        // Trace: T <x1> <y1> <x2> <y2> <width>
        else if (tokens[0] == "T" && tokens.size() >= 6 && currentNet) {
            Net::Subnet subnet;
            subnet.type = Net::Subnet::Type::Trace;
            currentNet->addSubnet(subnet);
        }
    }

    // Save last net
    if (currentNet) {
        eda.addNet(std::move(currentNet));
    }
}

// ============================================================================
// Drill Tools Parsing
// ============================================================================

void OdbReader::parseTools(std::vector<DrillTool>& tools, const std::filesystem::path& toolsPath) {
    std::string content = readFileContents(toolsPath);
    std::istringstream stream(content);
    std::string line;

    // Tools file format:
    // T<num> <type> <type2> <min_tol> <max_tol> <bit> <finish_size> <drill_size>
    // Or structured format with key=value pairs

    bool inToolSection = false;
    DrillTool currentTool;

    while (std::getline(stream, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;

        auto tokens = tokenize(line);
        if (tokens.empty()) continue;

        // Check for structured format
        if (line.find('=') != std::string::npos) {
            auto eqPos = line.find('=');
            std::string key = trim(line.substr(0, eqPos));
            std::string value = trim(line.substr(eqPos + 1));

            if (key == "NUM") {
                if (inToolSection) {
                    tools.push_back(currentTool);
                }
                currentTool = DrillTool();
                currentTool.num = std::stoi(value);
                inToolSection = true;
            }
            else if (key == "TYPE" && inToolSection) {
                std::string upper = toUpper(value);
                if (upper == "PLATED") currentTool.type = DrillToolType::Plated;
                else if (upper == "NON_PLATED") currentTool.type = DrillToolType::NonPlated;
                else if (upper == "VIA") currentTool.type = DrillToolType::Via;
            }
            else if (key == "TYPE2" && inToolSection) {
                std::string upper = toUpper(value);
                if (upper == "STANDARD") currentTool.type2 = DrillToolType2::Standard;
                else if (upper == "PRESS_FIT") currentTool.type2 = DrillToolType2::PressFit;
                else if (upper == "PHOTO") currentTool.type2 = DrillToolType2::Photo;
                else if (upper == "LASER") currentTool.type2 = DrillToolType2::Laser;
            }
            else if (key == "MIN_TOL" && inToolSection) {
                currentTool.minTol = std::stod(value);
            }
            else if (key == "MAX_TOL" && inToolSection) {
                currentTool.maxTol = std::stod(value);
            }
            else if (key == "BIT" && inToolSection) {
                currentTool.bit = value;
            }
            else if (key == "FINISH_SIZE" && inToolSection) {
                currentTool.finishSize = std::stod(value);
            }
            else if (key == "DRILL_SIZE" && inToolSection) {
                currentTool.drillSize = std::stod(value);
                // Calculate diameter from drill size
                currentTool.diameter = currentTool.drillSize;
            }
            else if (key == "DRILL_COUNT" && inToolSection) {
                currentTool.drillCount = std::stoi(value);
            }
            continue;
        }

        // Simple T<num> format
        if (tokens[0][0] == 'T' && tokens[0].size() > 1) {
            if (inToolSection) {
                tools.push_back(currentTool);
            }
            currentTool = DrillTool();
            currentTool.num = std::stoi(tokens[0].substr(1));
            inToolSection = true;

            // Parse remaining tokens
            if (tokens.size() >= 2) {
                // Try to parse as diameter
                try {
                    currentTool.diameter = std::stod(tokens[1]);
                    currentTool.drillSize = currentTool.diameter;
                } catch (const std::exception&) {
                    // Not a number, might be type
                    std::string upper = toUpper(tokens[1]);
                    if (upper == "PLATED") currentTool.type = DrillToolType::Plated;
                    else if (upper == "NON_PLATED") currentTool.type = DrillToolType::NonPlated;
                }
            }
            if (tokens.size() >= 3) {
                try {
                    currentTool.finishSize = std::stod(tokens[2]);
                } catch (const std::exception&) {
                    // Invalid finish size, keep default
                }
            }
        }
    }

    // Save last tool
    if (inToolSection) {
        tools.push_back(currentTool);
    }
}

// ============================================================================
// BOM Parsing
// ============================================================================

void OdbReader::parseBom(EdaData& eda, const std::filesystem::path& bomPath) {
    std::string content = readFileContents(bomPath);
    std::istringstream stream(content);
    std::string line;

    // BOM file can be structured or CSV-like format
    // Structured format: key=value pairs
    // Or header line followed by data lines

    std::vector<std::string> headers;
    bool headerParsed = false;

    while (std::getline(stream, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;

        auto tokens = tokenize(line);
        if (tokens.empty()) continue;

        // Check for structured format (key=value)
        if (line.find("REFDES=") != std::string::npos ||
            line.find("PART_NUMBER=") != std::string::npos) {
            // Parse structured BOM item
            BomItem item;
            std::istringstream lineStream(line);
            std::string token;

            while (std::getline(lineStream, token, ';')) {
                token = trim(token);
                auto eqPos = token.find('=');
                if (eqPos != std::string::npos) {
                    std::string key = trim(token.substr(0, eqPos));
                    std::string value = trim(token.substr(eqPos + 1));

                    if (key == "REFDES") item.refDes = value;
                    else if (key == "PART_NUMBER") item.partNumber = value;
                    else if (key == "MANUFACTURER") item.manufacturer = value;
                    else if (key == "DESCRIPTION" || key == "DESC") item.description = value;
                    else if (key == "DESC1") item.descriptions.push_back(value);
                    else if (key == "DESC2") {
                        while (item.descriptions.size() < 1) item.descriptions.push_back("");
                        item.descriptions.push_back(value);
                    }
                    else if (key == "DESC3") {
                        while (item.descriptions.size() < 2) item.descriptions.push_back("");
                        item.descriptions.push_back(value);
                    }
                    else if (key == "QTY") item.quantity = std::stoi(value);
                    else item.attributes[key] = value;
                }
            }

            if (!item.refDes.empty()) {
                eda.addBomItem(item);
            }
            continue;
        }

        // CSV-like format - first line is header
        if (!headerParsed) {
            headers = tokens;
            headerParsed = true;
            continue;
        }

        // Parse data line based on headers
        BomItem item;
        for (size_t i = 0; i < tokens.size() && i < headers.size(); ++i) {
            std::string header = toUpper(headers[i]);
            std::string value = tokens[i];

            if (header == "REFDES" || header == "REF" || header == "REFERENCE") {
                item.refDes = value;
            } else if (header == "PARTNUMBER" || header == "PART_NUMBER" || header == "PN") {
                item.partNumber = value;
            } else if (header == "MANUFACTURER" || header == "MFR") {
                item.manufacturer = value;
            } else if (header == "DESCRIPTION" || header == "DESC") {
                item.description = value;
            } else if (header == "QTY" || header == "QUANTITY") {
                try {
                    item.quantity = std::stoi(value);
                } catch (...) {
                    item.quantity = 1;
                }
            } else {
                item.attributes[headers[i]] = value;
            }
        }

        if (!item.refDes.empty()) {
            eda.addBomItem(item);
        }
    }
}

// ============================================================================
// Zones Parsing
// ============================================================================

void OdbReader::parseZones(std::vector<Zone>& zones, const std::filesystem::path& zonesPath) {
    std::string content = readFileContents(zonesPath);
    std::istringstream stream(content);
    std::string line;

    // Zones file format:
    // ZN <name>
    // SL <start_layer>
    // EL <end_layer>
    // OB <x> <y> I/H
    // OS <x> <y>
    // OE
    // ZE

    std::unique_ptr<Zone> currentZone;

    while (std::getline(stream, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;

        auto tokens = tokenize(line);
        if (tokens.empty()) continue;

        // Zone start: ZN <name>
        if (tokens[0] == "ZN" && tokens.size() >= 2) {
            if (currentZone) {
                zones.push_back(std::move(*currentZone));
            }
            currentZone = std::make_unique<Zone>();
            currentZone->name = tokens[1];
        }

        // Start layer: SL <layer>
        else if (tokens[0] == "SL" && tokens.size() >= 2 && currentZone) {
            currentZone->startLayer = std::stoi(tokens[1]);
        }

        // End layer: EL <layer>
        else if (tokens[0] == "EL" && tokens.size() >= 2 && currentZone) {
            currentZone->endLayer = std::stoi(tokens[1]);
        }

        // Outline begin: OB <x> <y>
        else if (tokens[0] == "OB" && tokens.size() >= 3 && currentZone) {
            double x = std::stod(tokens[1]);
            double y = std::stod(tokens[2]);
            currentZone->boundary.push_back({x, y});
        }

        // Outline segment: OS <x> <y>
        else if (tokens[0] == "OS" && tokens.size() >= 3 && currentZone) {
            double x = std::stod(tokens[1]);
            double y = std::stod(tokens[2]);
            currentZone->boundary.push_back({x, y});
        }

        // Attribute
        else if (tokens[0] == "ATTR" && tokens.size() >= 3 && currentZone) {
            currentZone->attributes[tokens[1]] = tokens[2];
        }

        // Zone end: ZE
        else if (tokens[0] == "ZE" && currentZone) {
            zones.push_back(std::move(*currentZone));
            currentZone.reset();
        }
    }

    // Save last zone
    if (currentZone) {
        zones.push_back(std::move(*currentZone));
    }
}

// ============================================================================
// Dimensions Parsing
// ============================================================================

void OdbReader::parseDimensions(std::vector<Dimension>& dims, const std::filesystem::path& dimsPath) {
    std::string content = readFileContents(dimsPath);
    std::istringstream stream(content);
    std::string line;

    // Dimensions file format:
    // DIM <type>
    // START <x> <y>
    // END <x> <y>
    // TEXT <x> <y> '<text>'
    // VALUE <value>
    // UNITS <units>
    // DEND

    std::unique_ptr<Dimension> currentDim;

    while (std::getline(stream, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;

        auto tokens = tokenize(line);
        if (tokens.empty()) continue;

        // Dimension start: DIM <type>
        if (tokens[0] == "DIM" && tokens.size() >= 2) {
            if (currentDim) {
                dims.push_back(std::move(*currentDim));
            }
            currentDim = std::make_unique<Dimension>();

            std::string typeStr = toUpper(tokens[1]);
            if (typeStr == "LINEAR") currentDim->type = DimensionType::Linear;
            else if (typeStr == "RADIAL") currentDim->type = DimensionType::Radial;
            else if (typeStr == "ANGULAR") currentDim->type = DimensionType::Angular;
            else if (typeStr == "LEADER") currentDim->type = DimensionType::Leader;
            else if (typeStr == "NOTE") currentDim->type = DimensionType::Note;
        }

        // Start point: START <x> <y>
        else if (tokens[0] == "START" && tokens.size() >= 3 && currentDim) {
            currentDim->start.x = std::stod(tokens[1]);
            currentDim->start.y = std::stod(tokens[2]);
        }

        // End point: END <x> <y>
        else if (tokens[0] == "END" && tokens.size() >= 3 && currentDim) {
            currentDim->end.x = std::stod(tokens[1]);
            currentDim->end.y = std::stod(tokens[2]);
        }

        // Text position and content: TEXT <x> <y> '<text>'
        else if (tokens[0] == "TEXT" && tokens.size() >= 4 && currentDim) {
            currentDim->textPosition.x = std::stod(tokens[1]);
            currentDim->textPosition.y = std::stod(tokens[2]);
            // Text may be quoted
            std::string text = tokens[3];
            if (text.size() >= 2 && text.front() == '\'' && text.back() == '\'') {
                text = text.substr(1, text.size() - 2);
            }
            currentDim->text = text;
        }

        // Value: VALUE <value>
        else if (tokens[0] == "VALUE" && tokens.size() >= 2 && currentDim) {
            currentDim->value = std::stod(tokens[1]);
        }

        // Units: UNITS <units>
        else if (tokens[0] == "UNITS" && tokens.size() >= 2 && currentDim) {
            currentDim->units = tokens[1];
        }

        // Attribute
        else if (tokens[0] == "ATTR" && tokens.size() >= 3 && currentDim) {
            currentDim->attributes[tokens[1]] = tokens[2];
        }

        // Dimension end: DEND
        else if (tokens[0] == "DEND" && currentDim) {
            dims.push_back(std::move(*currentDim));
            currentDim.reset();
        }
    }

    // Save last dimension
    if (currentDim) {
        dims.push_back(std::move(*currentDim));
    }
}

// ============================================================================
// Components Parsing (for parseComponents method in header)
// ============================================================================

void OdbReader::parseComponents(EdaData& eda, const std::string& content) {
    // This parses the components section of eda/data file
    // Already handled in parseEdaData, this is for standalone component parsing
    std::istringstream stream(content);
    std::string line;

    std::unique_ptr<Component> currentComponent;

    while (std::getline(stream, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;

        auto tokens = tokenize(line);
        if (tokens.empty()) continue;

        // CMP record
        if (tokens[0] == "CMP" && tokens.size() >= 7) {
            if (currentComponent) {
                eda.addComponent(std::move(currentComponent));
            }
            std::string refDes = tokens[6];
            currentComponent = std::make_unique<Component>(refDes);
            currentComponent->setPackageIndex(std::stoi(tokens[1]));
            currentComponent->setPosition(std::stod(tokens[2]), std::stod(tokens[3]));
            currentComponent->setRotation(std::stod(tokens[4]));
            currentComponent->setMirrored(tokens[5] == "M" || tokens[5] == "Y");
            if (currentComponent->isMirrored()) {
                currentComponent->setSide(MountSide::Bottom);
            }
            if (tokens.size() >= 8) {
                currentComponent->setPartNumber(tokens[7]);
            }
        }

        // PRP (property) record
        else if (tokens[0] == "PRP" && tokens.size() >= 2 && currentComponent) {
            size_t propStart = line.find("PRP");
            if (propStart != std::string::npos) {
                size_t nameStart = propStart + 4;
                while (nameStart < line.size() && std::isspace(line[nameStart])) nameStart++;
                size_t nameEnd = nameStart;
                while (nameEnd < line.size() && !std::isspace(line[nameEnd])) nameEnd++;
                std::string propName = line.substr(nameStart, nameEnd - nameStart);

                size_t valStart = line.find('\'', nameEnd);
                if (valStart != std::string::npos) {
                    size_t valEnd = line.find('\'', valStart + 1);
                    if (valEnd != std::string::npos) {
                        std::string propValue = line.substr(valStart + 1, valEnd - valStart - 1);
                        currentComponent->setAttribute(propName, propValue);
                    }
                }
            }
        }

        // TOP (toeprint) record
        else if (tokens[0] == "TOP" && tokens.size() >= 8 && currentComponent) {
            Pin pin;
            pin.name = tokens.size() >= 9 ? tokens[8] : std::to_string(std::stoi(tokens[1]));
            pin.x = std::stod(tokens[2]);
            pin.y = std::stod(tokens[3]);
            pin.rotation = std::stod(tokens[4]);
            pin.mirror = (tokens[5] == "M" || tokens[5] == "Y");
            currentComponent->addPin(pin);
        }
    }

    if (currentComponent) {
        eda.addComponent(std::move(currentComponent));
    }
}

// ============================================================================
// Stackup Parsing
// ============================================================================

void OdbReader::parseStackup(std::vector<StackupLayer>& stackup, const std::filesystem::path& stackupPath) {
    std::string content = readFileContents(stackupPath);
    std::istringstream stream(content);
    std::string line;

    // ODB++ stackup can be XML or structured text format
    // Check if it's XML by looking for <?xml or <stackup>
    bool isXml = content.find("<?xml") != std::string::npos ||
                 content.find("<stackup") != std::string::npos ||
                 content.find("<STACKUP") != std::string::npos;

    if (isXml) {
        // Simple XML parsing for stackup
        // Look for <layer> or <LAYER> elements
        std::regex layerRegex(R"(<[Ll][Aa][Yy][Ee][Rr][^>]*>([\s\S]*?)</[Ll][Aa][Yy][Ee][Rr]>)");
        std::smatch match;
        std::string::const_iterator searchStart(content.cbegin());

        while (std::regex_search(searchStart, content.cend(), match, layerRegex)) {
            StackupLayer layer;
            std::string layerContent = match[1].str();

            // Parse layer attributes
            std::regex attrRegex(R"(<([^>]+)>([^<]*)</\1>)");
            std::smatch attrMatch;
            std::string::const_iterator attrStart(layerContent.cbegin());

            while (std::regex_search(attrStart, layerContent.cend(), attrMatch, attrRegex)) {
                std::string attrName = toUpper(attrMatch[1].str());
                std::string attrValue = attrMatch[2].str();

                if (attrName == "NAME") layer.name = attrValue;
                else if (attrName == "THICKNESS") layer.thickness = std::stod(attrValue);
                else if (attrName == "DIELECTRIC" || attrName == "ER") layer.dielectricConstant = std::stod(attrValue);
                else if (attrName == "LOSS_TANGENT" || attrName == "DF") layer.lossTangent = std::stod(attrValue);
                else if (attrName == "MATERIAL") layer.material = attrValue;
                else if (attrName == "TYPE") {
                    std::string typeUpper = toUpper(attrValue);
                    if (typeUpper == "COPPER" || typeUpper == "CONDUCTOR") layer.materialType = StackupMaterialType::Copper;
                    else if (typeUpper == "CORE") layer.materialType = StackupMaterialType::Core;
                    else if (typeUpper == "PREPREG" || typeUpper == "PP") layer.materialType = StackupMaterialType::Prepreg;
                    else if (typeUpper == "SOLDER_MASK" || typeUpper == "SM") layer.materialType = StackupMaterialType::SolderMask;
                    else if (typeUpper == "COVERLAY") layer.materialType = StackupMaterialType::Coverlay;
                    else if (typeUpper == "ADHESIVE") layer.materialType = StackupMaterialType::Adhesive;
                    else if (typeUpper == "STIFFENER") layer.materialType = StackupMaterialType::Stiffener;
                }

                attrStart = attrMatch.suffix().first;
            }

            stackup.push_back(layer);
            searchStart = match.suffix().first;
        }
    } else {
        // Structured text format
        // LAYER {
        //   NAME = 'top_copper'
        //   TYPE = COPPER
        //   THICKNESS = 0.035
        //   ...
        // }

        std::unique_ptr<StackupLayer> currentLayer;
        bool inLayerSection = false;

        while (std::getline(stream, line)) {
            line = trim(line);
            if (line.empty() || line[0] == '#') continue;

            if (line.find("LAYER") != std::string::npos && line.find("{") != std::string::npos) {
                if (currentLayer) {
                    stackup.push_back(std::move(*currentLayer));
                }
                currentLayer = std::make_unique<StackupLayer>();
                inLayerSection = true;
                continue;
            }

            if (line == "}" && inLayerSection) {
                if (currentLayer) {
                    stackup.push_back(std::move(*currentLayer));
                    currentLayer.reset();
                }
                inLayerSection = false;
                continue;
            }

            if (inLayerSection && currentLayer) {
                auto eqPos = line.find('=');
                if (eqPos != std::string::npos) {
                    std::string key = trim(line.substr(0, eqPos));
                    std::string value = trim(line.substr(eqPos + 1));

                    // Remove quotes
                    if (value.size() >= 2 && value.front() == '\'' && value.back() == '\'') {
                        value = value.substr(1, value.size() - 2);
                    }

                    std::string keyUpper = toUpper(key);
                    if (keyUpper == "NAME") currentLayer->name = value;
                    else if (keyUpper == "THICKNESS") currentLayer->thickness = std::stod(value);
                    else if (keyUpper == "DIELECTRIC" || keyUpper == "ER") currentLayer->dielectricConstant = std::stod(value);
                    else if (keyUpper == "LOSS_TANGENT" || keyUpper == "DF") currentLayer->lossTangent = std::stod(value);
                    else if (keyUpper == "MATERIAL") currentLayer->material = value;
                    else if (keyUpper == "LAYER_INDEX") currentLayer->layerIndex = std::stoi(value);
                    else if (keyUpper == "TYPE") {
                        std::string typeUpper = toUpper(value);
                        if (typeUpper == "COPPER" || typeUpper == "CONDUCTOR") currentLayer->materialType = StackupMaterialType::Copper;
                        else if (typeUpper == "CORE") currentLayer->materialType = StackupMaterialType::Core;
                        else if (typeUpper == "PREPREG" || typeUpper == "PP") currentLayer->materialType = StackupMaterialType::Prepreg;
                        else if (typeUpper == "SOLDER_MASK" || typeUpper == "SM") currentLayer->materialType = StackupMaterialType::SolderMask;
                        else if (typeUpper == "COVERLAY") currentLayer->materialType = StackupMaterialType::Coverlay;
                        else if (typeUpper == "ADHESIVE") currentLayer->materialType = StackupMaterialType::Adhesive;
                        else if (typeUpper == "STIFFENER") currentLayer->materialType = StackupMaterialType::Stiffener;
                    }
                    else {
                        currentLayer->properties[key] = value;
                    }
                }
            }
        }

        // Save last layer
        if (currentLayer) {
            stackup.push_back(std::move(*currentLayer));
        }
    }
}

// ============================================================================
// Impedance Parsing
// ============================================================================

void OdbReader::parseImpedance(std::vector<ImpedanceConstraint>& impedance, const std::filesystem::path& impedancePath) {
    std::string content = readFileContents(impedancePath);
    std::istringstream stream(content);
    std::string line;

    // Check for XML format
    bool isXml = content.find("<?xml") != std::string::npos ||
                 content.find("<impedance") != std::string::npos ||
                 content.find("<IMPEDANCE") != std::string::npos;

    if (isXml) {
        // Simple XML parsing for impedance constraints
        std::regex constraintRegex(R"(<[Cc][Oo][Nn][Ss][Tt][Rr][Aa][Ii][Nn][Tt][^>]*>([\s\S]*?)</[Cc][Oo][Nn][Ss][Tt][Rr][Aa][Ii][Nn][Tt]>)");
        std::smatch match;
        std::string::const_iterator searchStart(content.cbegin());

        while (std::regex_search(searchStart, content.cend(), match, constraintRegex)) {
            ImpedanceConstraint constraint;
            std::string constraintContent = match[1].str();

            std::regex attrRegex(R"(<([^>]+)>([^<]*)</\1>)");
            std::smatch attrMatch;
            std::string::const_iterator attrStart(constraintContent.cbegin());

            while (std::regex_search(attrStart, constraintContent.cend(), attrMatch, attrRegex)) {
                std::string attrName = toUpper(attrMatch[1].str());
                std::string attrValue = attrMatch[2].str();

                if (attrName == "ID") constraint.id = attrValue;
                else if (attrName == "TYPE") constraint.type = attrValue;
                else if (attrName == "IMPEDANCE" || attrName == "VALUE") constraint.impedance = std::stod(attrValue);
                else if (attrName == "TOLERANCE") constraint.tolerance = std::stod(attrValue);
                else if (attrName == "LAYER") constraint.layer = attrValue;
                else if (attrName == "TRACE_WIDTH" || attrName == "WIDTH") constraint.traceWidth = std::stod(attrValue);
                else if (attrName == "SPACING") constraint.spacing = std::stod(attrValue);

                attrStart = attrMatch.suffix().first;
            }

            impedance.push_back(constraint);
            searchStart = match.suffix().first;
        }
    } else {
        // Structured text format
        std::unique_ptr<ImpedanceConstraint> current;
        bool inConstraintSection = false;

        while (std::getline(stream, line)) {
            line = trim(line);
            if (line.empty() || line[0] == '#') continue;

            if (line.find("CONSTRAINT") != std::string::npos && line.find("{") != std::string::npos) {
                if (current) {
                    impedance.push_back(std::move(*current));
                }
                current = std::make_unique<ImpedanceConstraint>();
                inConstraintSection = true;
                continue;
            }

            if (line == "}" && inConstraintSection) {
                if (current) {
                    impedance.push_back(std::move(*current));
                    current.reset();
                }
                inConstraintSection = false;
                continue;
            }

            if (inConstraintSection && current) {
                auto eqPos = line.find('=');
                if (eqPos != std::string::npos) {
                    std::string key = trim(line.substr(0, eqPos));
                    std::string value = trim(line.substr(eqPos + 1));

                    if (value.size() >= 2 && value.front() == '\'' && value.back() == '\'') {
                        value = value.substr(1, value.size() - 2);
                    }

                    std::string keyUpper = toUpper(key);
                    if (keyUpper == "ID") current->id = value;
                    else if (keyUpper == "TYPE") current->type = value;
                    else if (keyUpper == "IMPEDANCE" || keyUpper == "VALUE") current->impedance = std::stod(value);
                    else if (keyUpper == "TOLERANCE") current->tolerance = std::stod(value);
                    else if (keyUpper == "LAYER") current->layer = value;
                    else if (keyUpper == "TRACE_WIDTH" || keyUpper == "WIDTH") current->traceWidth = std::stod(value);
                    else if (keyUpper == "SPACING") current->spacing = std::stod(value);
                }
            }
        }

        if (current) {
            impedance.push_back(std::move(*current));
        }
    }
}

// ============================================================================
// Shortf (Intentional Shorts) Parsing
// ============================================================================

void OdbReader::parseShortf(std::vector<IntentionalShort>& shorts, const std::filesystem::path& shortfPath) {
    std::string content = readFileContents(shortfPath);
    std::istringstream stream(content);
    std::string line;

    // Shortf file format:
    // SHT <id>
    // NET <net_uid>
    // NET <net_uid>
    // FID <type> <layer> <feature>
    // SHE

    std::unique_ptr<IntentionalShort> current;

    while (std::getline(stream, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;

        auto tokens = tokenize(line);
        if (tokens.empty()) continue;

        // Short start: SHT <id>
        if (tokens[0] == "SHT" && tokens.size() >= 2) {
            if (current) {
                shorts.push_back(std::move(*current));
            }
            current = std::make_unique<IntentionalShort>();
            current->id = static_cast<uint32_t>(std::stoul(tokens[1]));
        }

        // Net UID: NET <uid>
        else if (tokens[0] == "NET" && tokens.size() >= 2 && current) {
            current->netUids.push_back(static_cast<uint32_t>(std::stoul(tokens[1])));
        }

        // Feature ID: FID <type> <layer> <feature>
        else if (tokens[0] == "FID" && tokens.size() >= 4 && current) {
            FeatureId fid;
            fid.type = tokens[1][0];
            fid.layerNum = std::stoi(tokens[2]);
            fid.featureNum = std::stoi(tokens[3]);
            current->features.push_back(fid);
        }

        // Short end: SHE
        else if (tokens[0] == "SHE" && current) {
            shorts.push_back(std::move(*current));
            current.reset();
        }
    }

    if (current) {
        shorts.push_back(std::move(*current));
    }
}

// ============================================================================
// HDI Netlist Parsing
// ============================================================================

void OdbReader::parseHdiNetlist(EdaData& eda, const std::filesystem::path& hdiPath) {
    std::string content = readFileContents(hdiPath);
    std::istringstream stream(content);
    std::string line;

    // HDI netlist format:
    // PKG DIE <name> <refdes>
    // PKG BGA <name> <refdes>
    // N <net_name>
    // P <x> <y> <symbol> <die_name> <bump_name> <dest_comp> <dest_pin>

    std::unique_ptr<Net> currentNet;

    while (std::getline(stream, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;

        auto tokens = tokenize(line);
        if (tokens.empty()) continue;

        // Package: PKG <type> <name> <refdes>
        if (tokens[0] == "PKG" && tokens.size() >= 4) {
            // Store as component
            auto comp = std::make_unique<Component>(tokens[3]);  // refdes
            comp->setPackageName(tokens[2]);
            // Store type as attribute
            comp->setAttribute("hdi_type", tokens[1]);
            eda.addComponent(std::move(comp));
        }

        // Net: N <net_name>
        else if (tokens[0] == "N" && tokens.size() >= 2) {
            if (currentNet) {
                eda.addNet(std::move(currentNet));
            }
            currentNet = std::make_unique<Net>(tokens[1]);
        }

        // Point: P <x> <y> <symbol> <die> <bump> <dest_comp> <dest_pin>
        else if (tokens[0] == "P" && tokens.size() >= 3 && currentNet) {
            // Store HDI net point as attribute on net
            std::string pointStr = tokens[1] + "," + tokens[2];
            if (tokens.size() >= 8) {
                pointStr += "," + tokens[3];  // symbol
                pointStr += "," + tokens[4];  // die
                pointStr += "," + tokens[5];  // bump
                pointStr += "," + tokens[6];  // dest_comp
                pointStr += "," + tokens[7];  // dest_pin
            }
            currentNet->setAttribute("hdi_point_" + std::to_string(currentNet->getPinCount()), pointStr);

            // Add pin reference if we have dest component info
            if (tokens.size() >= 8) {
                currentNet->addPin(tokens[6], tokens[7]);
            }
        }
    }

    if (currentNet) {
        eda.addNet(std::move(currentNet));
    }
}

// ============================================================================
// User Symbol Parsing
// ============================================================================

void OdbReader::parseUserSymbol(Symbol& symbol, const std::filesystem::path& symbolPath) {
    // User symbol directory contains:
    // - features file (graphics)
    // - attrlist (optional attributes)

    auto featuresPath = findFile(symbolPath, "features");
    if (fileExists(featuresPath)) {
        Layer tempLayer;
        parseFeatures(tempLayer, featuresPath);

        for (auto& f : tempLayer.getFeatures()) {
            symbol.addFeature(std::move(f));
        }
    }

    // Parse attributes
    auto attrPath = symbolPath / "attrlist";
    if (fileExists(attrPath)) {
        AttributeList attrs;
        parseAttrList(attrs, attrPath);
        for (const auto& pair : attrs) {
            symbol.setAttribute(pair.first, pair.second);
        }
    }

    symbol.setType(SymbolType::User);
}

// ============================================================================
// Wheel Parsing (Gerber Aperture Wheel)
// ============================================================================

void OdbReader::parseWheel(std::vector<std::string>& apertures, const std::filesystem::path& wheelPath) {
    std::string content = readFileContents(wheelPath);
    std::istringstream stream(content);
    std::string line;

    // Wheel file format:
    // D<num> <symbol_name>
    // or
    // <num> <symbol_name>

    while (std::getline(stream, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;

        auto tokens = tokenize(line);
        if (tokens.empty()) continue;

        std::string firstToken = tokens[0];
        std::string symbolName;

        // D-code format: D10 round20
        if (firstToken[0] == 'D' && firstToken.size() > 1) {
            int dcode = std::stoi(firstToken.substr(1));
            if (tokens.size() >= 2) {
                symbolName = tokens[1];
            }
            // Ensure vector is large enough
            if (dcode >= static_cast<int>(apertures.size())) {
                apertures.resize(static_cast<size_t>(dcode) + 1);
            }
            apertures[static_cast<size_t>(dcode)] = symbolName;
        }
        // Numeric format: 10 round20
        else {
            try {
                int num = std::stoi(firstToken);
                if (tokens.size() >= 2) {
                    symbolName = tokens[1];
                }
                if (num >= static_cast<int>(apertures.size())) {
                    apertures.resize(static_cast<size_t>(num) + 1);
                }
                apertures[static_cast<size_t>(num)] = symbolName;
            } catch (...) {
                // Not a valid D-code line, skip
            }
        }
    }
}

// ============================================================================
// Font Parsing
// ============================================================================

void OdbReader::parseFont(const std::filesystem::path& fontPath) {
    // ODB++ fonts are stored in fonts/<font_name>/
    // Each font has:
    // - char_<ascii_code> files containing graphics for each character
    // - Or a combined standard file

    // For now, just validate the font exists
    // Full font parsing would create character graphics
    if (!std::filesystem::exists(fontPath)) {
        lastError_ = "Font path does not exist: " + fontPath.string();
    }

    // Fonts are stored in Layer's font list during feature parsing
    // This method is mainly for validation and preloading
}

// ============================================================================
// System Attribute Definitions Parsing
// ============================================================================

void OdbReader::parseSysAttr(std::vector<AttributeDefinition>& attrs, const std::filesystem::path& attrPath) {
    std::string content = readFileContents(attrPath);
    std::istringstream stream(content);
    std::string line;

    // System attribute file format:
    // @<num> <name> <type> [<options>]
    // Or structured format

    while (std::getline(stream, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;

        // Attribute definition: @<num> <name> ...
        if (line[0] == '@') {
            auto tokens = tokenize(line);
            if (tokens.size() >= 2) {
                AttributeDefinition def;
                // Remove @ prefix
                def.name = tokens[1];

                // Parse type if present
                if (tokens.size() >= 3) {
                    std::string typeStr = toUpper(tokens[2]);
                    if (typeStr == "BOOLEAN" || typeStr == "B") def.type = AttributeType::Boolean;
                    else if (typeStr == "INTEGER" || typeStr == "I") def.type = AttributeType::Integer;
                    else if (typeStr == "FLOAT" || typeStr == "F") def.type = AttributeType::Float;
                    else if (typeStr == "OPTION" || typeStr == "O") def.type = AttributeType::Option;
                    else def.type = AttributeType::Text;
                }

                // Parse options if present (for OPTION type)
                for (size_t i = 3; i < tokens.size(); ++i) {
                    if (def.type == AttributeType::Option) {
                        def.options.push_back(tokens[i]);
                    }
                }

                attrs.push_back(def);
            }
        }
        // Structured format with key=value
        else if (line.find('=') != std::string::npos) {
            auto eqPos = line.find('=');
            std::string key = trim(line.substr(0, eqPos));
            std::string value = trim(line.substr(eqPos + 1));

            if (value.size() >= 2 && value.front() == '\'' && value.back() == '\'') {
                value = value.substr(1, value.size() - 2);
            }

            // Find or create attribute definition
            AttributeDefinition* currentDef = nullptr;
            if (toUpper(key) == "NAME") {
                attrs.push_back(AttributeDefinition());
                currentDef = &attrs.back();
                currentDef->name = value;
            } else if (!attrs.empty()) {
                currentDef = &attrs.back();
                std::string keyUpper = toUpper(key);

                if (keyUpper == "TYPE") {
                    std::string typeStr = toUpper(value);
                    if (typeStr == "BOOLEAN") currentDef->type = AttributeType::Boolean;
                    else if (typeStr == "INTEGER") currentDef->type = AttributeType::Integer;
                    else if (typeStr == "FLOAT") currentDef->type = AttributeType::Float;
                    else if (typeStr == "OPTION") currentDef->type = AttributeType::Option;
                    else currentDef->type = AttributeType::Text;
                }
                else if (keyUpper == "DESCRIPTION") currentDef->description = value;
                else if (keyUpper == "UNIT_TYPE") currentDef->unitType = value;
                else if (keyUpper == "UNITS") currentDef->units = value;
                else if (keyUpper == "MIN") currentDef->minValue = std::stod(value);
                else if (keyUpper == "MAX") currentDef->maxValue = std::stod(value);
                else if (keyUpper == "DEFAULT") currentDef->defaultValue = value;
            }
        }
    }
}

void OdbReader::parseAttrList(AttributeList& attrs, const std::filesystem::path& attrPath) {
    std::string content = readFileContents(attrPath);
    std::istringstream stream(content);
    std::string line;

    while (std::getline(stream, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;

        auto eqPos = line.find('=');
        if (eqPos != std::string::npos) {
            std::string key = trim(line.substr(0, eqPos));
            std::string value = trim(line.substr(eqPos + 1));
            attrs[key] = value;
        }
    }
}

// ============================================================================
// Metadata Parsing
// ============================================================================

void OdbReader::parseMetadata(Metadata& metadata, const std::filesystem::path& metadataPath) {
    std::string content = readFileContents(metadataPath);
    std::istringstream stream(content);
    std::string line;

    // Check for XML format
    bool isXml = content.find("<?xml") != std::string::npos ||
                 content.find("<metadata") != std::string::npos ||
                 content.find("<METADATA") != std::string::npos;

    if (isXml) {
        // Parse XML metadata
        // Look for <entry> or common metadata tags

        // Parse ODB++ version
        std::regex versionRegex(R"(<[Vv]ersion>([^<]*)</[Vv]ersion>)");
        std::smatch match;
        if (std::regex_search(content, match, versionRegex)) {
            metadata.version = match[1].str();
        }

        // Parse source application
        std::regex sourceRegex(R"(<[Ss]ource>([^<]*)</[Ss]ource>)");
        if (std::regex_search(content, match, sourceRegex)) {
            metadata.source = match[1].str();
        }

        // Parse creation date
        std::regex creationRegex(R"(<[Cc]reation[Dd]ate>([^<]*)</[Cc]reation[Dd]ate>)");
        if (std::regex_search(content, match, creationRegex)) {
            metadata.creationDate = match[1].str();
        }

        // Parse modification date
        std::regex modRegex(R"(<[Mm]odification[Dd]ate>([^<]*)</[Mm]odification[Dd]ate>)");
        if (std::regex_search(content, match, modRegex)) {
            metadata.modificationDate = match[1].str();
        }

        // Parse generic entries
        std::regex entryRegex(R"(<([A-Za-z_][A-Za-z0-9_]*)([^>]*)>([^<]*)</\1>)");
        std::string::const_iterator searchStart(content.cbegin());
        while (std::regex_search(searchStart, content.cend(), match, entryRegex)) {
            MetadataEntry entry;
            entry.key = match[1].str();
            entry.value = match[3].str();

            // Parse attributes in the tag (e.g., type="string" unit="mm")
            std::string attrStr = match[2].str();
            std::regex attrRegex(R"((\w+)=[\"']([^\"']*)[\"'])");
            std::smatch attrMatch;
            std::string::const_iterator attrStart(attrStr.cbegin());
            while (std::regex_search(attrStart, attrStr.cend(), attrMatch, attrRegex)) {
                std::string attrName = attrMatch[1].str();
                std::string attrValue = attrMatch[2].str();
                if (attrName == "type") entry.type = attrValue;
                else if (attrName == "unit") entry.unit = attrValue;
                attrStart = attrMatch.suffix().first;
            }

            metadata.entries.push_back(entry);
            metadata.attributes[entry.key] = entry.value;
            searchStart = match.suffix().first;
        }
    } else {
        // Parse structured text format
        // Key = Value or key=value format

        while (std::getline(stream, line)) {
            line = trim(line);
            if (line.empty() || line[0] == '#') continue;

            auto eqPos = line.find('=');
            if (eqPos != std::string::npos) {
                std::string key = trim(line.substr(0, eqPos));
                std::string value = trim(line.substr(eqPos + 1));

                // Remove quotes if present
                if (value.size() >= 2 && value.front() == '\'' && value.back() == '\'') {
                    value = value.substr(1, value.size() - 2);
                }

                std::string keyUpper = toUpper(key);

                if (keyUpper == "ODB_VERSION" || keyUpper == "VERSION") {
                    metadata.version = value;
                } else if (keyUpper == "SOURCE" || keyUpper == "SOURCE_APP") {
                    metadata.source = value;
                } else if (keyUpper == "CREATION_DATE" || keyUpper == "CREATE_DATE") {
                    metadata.creationDate = value;
                } else if (keyUpper == "MODIFICATION_DATE" || keyUpper == "SAVE_DATE") {
                    metadata.modificationDate = value;
                }

                MetadataEntry entry;
                entry.key = key;
                entry.value = value;
                metadata.entries.push_back(entry);
                metadata.attributes[key] = value;
            }
        }
    }
}

// ============================================================================
// Component Variants Parsing
// ============================================================================

void OdbReader::parseVariants(std::vector<ComponentVariant>& variants,
                              const std::filesystem::path& variantsPath) {
    std::string content = readFileContents(variantsPath);
    std::istringstream stream(content);
    std::string line;

    ComponentVariant* currentVariant = nullptr;

    while (std::getline(stream, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;

        auto tokens = tokenize(line);
        if (tokens.empty()) continue;

        std::string keyword = toUpper(tokens[0]);

        // VARIANT <name> [description]
        if (keyword == "VARIANT" && tokens.size() >= 2) {
            ComponentVariant variant;
            variant.name = tokens[1];
            if (tokens.size() >= 3) {
                variant.description = tokens[2];
            }
            variants.push_back(variant);
            currentVariant = &variants.back();
        }
        // CMP <refdes> <state> [alternate_pn] [alternate_pkg]
        // States: E=Enabled, D=Disabled (DNP), R=Replaced
        else if (keyword == "CMP" && tokens.size() >= 3 && currentVariant) {
            ComponentVariantEntry entry;
            entry.refDes = tokens[1];

            std::string stateStr = toUpper(tokens[2]);
            if (stateStr == "D" || stateStr == "DNP" || stateStr == "DISABLED") {
                entry.state = ComponentVariantState::Disabled;
            } else if (stateStr == "R" || stateStr == "REPLACED") {
                entry.state = ComponentVariantState::Replaced;
                if (tokens.size() >= 4) entry.alternatePartNumber = tokens[3];
                if (tokens.size() >= 5) entry.alternatePackage = tokens[4];
            } else {
                entry.state = ComponentVariantState::Enabled;
            }

            currentVariant->components.push_back(entry);
        }
        // Handle key=value pairs for current variant
        else if (currentVariant) {
            size_t eqPos = line.find('=');
            if (eqPos != std::string::npos) {
                std::string key = trim(line.substr(0, eqPos));
                std::string value = trim(line.substr(eqPos + 1));
                currentVariant->attributes[key] = value;
            }
        }
    }
}

// ============================================================================
// Embedded Components Parsing
// ============================================================================

void OdbReader::parseEmbeddedComponents(std::vector<EmbeddedComponent>& components,
                                        const std::filesystem::path& embeddedPath) {
    std::string content = readFileContents(embeddedPath);
    std::istringstream stream(content);
    std::string line;

    EmbeddedComponent* currentComp = nullptr;

    while (std::getline(stream, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;

        auto tokens = tokenize(line);
        if (tokens.empty()) continue;

        std::string keyword = toUpper(tokens[0]);

        // EMBEDDED_R <name> <value> [unit] [tolerance]
        if (keyword == "EMBEDDED_R" && tokens.size() >= 3) {
            EmbeddedComponent comp;
            comp.name = tokens[1];
            comp.type = EmbeddedComponentType::Resistor;
            comp.value = std::stod(tokens[2]);
            comp.valueUnit = (tokens.size() >= 4) ? tokens[3] : "ohm";
            if (tokens.size() >= 5) comp.tolerance = std::stod(tokens[4]);
            components.push_back(comp);
            currentComp = &components.back();
        }
        // EMBEDDED_C <name> <value> [unit] [tolerance]
        else if (keyword == "EMBEDDED_C" && tokens.size() >= 3) {
            EmbeddedComponent comp;
            comp.name = tokens[1];
            comp.type = EmbeddedComponentType::Capacitor;
            comp.value = std::stod(tokens[2]);
            comp.valueUnit = (tokens.size() >= 4) ? tokens[3] : "pF";
            if (tokens.size() >= 5) comp.tolerance = std::stod(tokens[4]);
            components.push_back(comp);
            currentComp = &components.back();
        }
        // EMBEDDED_L <name> <value> [unit] [tolerance]
        else if (keyword == "EMBEDDED_L" && tokens.size() >= 3) {
            EmbeddedComponent comp;
            comp.name = tokens[1];
            comp.type = EmbeddedComponentType::Inductor;
            comp.value = std::stod(tokens[2]);
            comp.valueUnit = (tokens.size() >= 4) ? tokens[3] : "nH";
            if (tokens.size() >= 5) comp.tolerance = std::stod(tokens[4]);
            components.push_back(comp);
            currentComp = &components.back();
        }
        // POS <x> <y>
        else if (keyword == "POS" && tokens.size() >= 3 && currentComp) {
            currentComp->x = std::stod(tokens[1]);
            currentComp->y = std::stod(tokens[2]);
        }
        // SIZE <width> <height>
        else if (keyword == "SIZE" && tokens.size() >= 3 && currentComp) {
            currentComp->width = std::stod(tokens[1]);
            currentComp->height = std::stod(tokens[2]);
        }
        // LAYER <layer_name> or LAYERS <start> <end>
        else if (keyword == "LAYER" && tokens.size() >= 2 && currentComp) {
            currentComp->layerName = tokens[1];
        }
        else if (keyword == "LAYERS" && tokens.size() >= 3 && currentComp) {
            currentComp->startLayer = std::stoi(tokens[1]);
            currentComp->endLayer = std::stoi(tokens[2]);
        }
        // MATERIAL <material_name>
        else if (keyword == "MATERIAL" && tokens.size() >= 2 && currentComp) {
            currentComp->material = tokens[1];
        }
        // THICKNESS <value>
        else if (keyword == "THICKNESS" && tokens.size() >= 2 && currentComp) {
            currentComp->thickness = std::stod(tokens[1]);
        }
        // Handle key=value pairs
        else if (currentComp) {
            size_t eqPos = line.find('=');
            if (eqPos != std::string::npos) {
                std::string key = trim(line.substr(0, eqPos));
                std::string value = trim(line.substr(eqPos + 1));
                currentComp->attributes[key] = value;
            }
        }
    }
}

// ============================================================================
// Build-up Information Parsing
// ============================================================================

void OdbReader::parseBuildup(BuildupInfo& buildup, const std::filesystem::path& buildupPath) {
    std::string content = readFileContents(buildupPath);
    std::istringstream stream(content);
    std::string line;

    ViaSpan* currentVia = nullptr;
    BuildupLayer* currentLayer = nullptr;

    while (std::getline(stream, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;

        auto tokens = tokenize(line);
        if (tokens.empty()) continue;

        std::string keyword = toUpper(tokens[0]);

        // VIA_SPAN <name> <type>
        if (keyword == "VIA_SPAN" && tokens.size() >= 2) {
            ViaSpan via;
            via.name = tokens[1];
            if (tokens.size() >= 3) {
                std::string typeStr = toUpper(tokens[2]);
                if (typeStr == "THROUGH") via.type = ViaSpanType::Through;
                else if (typeStr == "BLIND") via.type = ViaSpanType::Blind;
                else if (typeStr == "BURIED") via.type = ViaSpanType::Buried;
                else if (typeStr == "MICROVIA") via.type = ViaSpanType::Microvia;
            }
            buildup.viaSpans.push_back(via);
            currentVia = &buildup.viaSpans.back();
            currentLayer = nullptr;
        }
        // LAYER <name> <type>
        else if (keyword == "LAYER" && tokens.size() >= 2) {
            BuildupLayer layer;
            layer.name = tokens[1];
            if (tokens.size() >= 3) layer.type = tokens[2];
            buildup.layers.push_back(layer);
            currentLayer = &buildup.layers.back();
            currentVia = nullptr;
        }
        // START_LAYER / END_LAYER for via spans
        else if (keyword == "START_LAYER" && tokens.size() >= 2 && currentVia) {
            currentVia->startLayerName = tokens[1];
            if (tokens.size() >= 3) currentVia->startLayer = std::stoi(tokens[2]);
        }
        else if (keyword == "END_LAYER" && tokens.size() >= 2 && currentVia) {
            currentVia->endLayerName = tokens[1];
            if (tokens.size() >= 3) currentVia->endLayer = std::stoi(tokens[2]);
        }
        // DRILL_METHOD
        else if (keyword == "DRILL_METHOD" && tokens.size() >= 2 && currentVia) {
            currentVia->drillMethod = tokens[1];
        }
        // DIAMETER <min> [max]
        else if (keyword == "DIAMETER" && tokens.size() >= 2 && currentVia) {
            currentVia->minDiameter = std::stod(tokens[1]);
            if (tokens.size() >= 3) currentVia->maxDiameter = std::stod(tokens[2]);
        }
        // THICKNESS / MATERIAL / SEQUENCE for layers
        else if (keyword == "THICKNESS" && tokens.size() >= 2 && currentLayer) {
            currentLayer->thickness = std::stod(tokens[1]);
        }
        else if (keyword == "MATERIAL" && tokens.size() >= 2 && currentLayer) {
            currentLayer->material = tokens[1];
        }
        else if (keyword == "SEQUENCE" && tokens.size() >= 2 && currentLayer) {
            currentLayer->sequence = std::stoi(tokens[1]);
        }
        else if (keyword == "CORE" && currentLayer) {
            currentLayer->isCore = true;
        }
        // TOTAL_LAYERS / TOTAL_THICKNESS
        else if (keyword == "TOTAL_LAYERS" && tokens.size() >= 2) {
            buildup.totalLayers = std::stoi(tokens[1]);
        }
        else if (keyword == "TOTAL_THICKNESS" && tokens.size() >= 2) {
            buildup.totalThickness = std::stod(tokens[1]);
        }
        // Key=value pairs
        else {
            size_t eqPos = line.find('=');
            if (eqPos != std::string::npos) {
                std::string key = trim(line.substr(0, eqPos));
                std::string value = trim(line.substr(eqPos + 1));
                std::string keyUpper = toUpper(key);

                if (keyUpper == "NAME") buildup.name = value;
                else if (keyUpper == "TOTAL_LAYERS") buildup.totalLayers = std::stoi(value);
                else if (keyUpper == "TOTAL_THICKNESS") buildup.totalThickness = std::stod(value);
                else buildup.attributes[key] = value;
            }
        }
    }
}

// ============================================================================
// VPL (Vendor Part List) Parsing
// ============================================================================

void OdbReader::parseVpl(std::vector<VendorPart>& parts, const std::filesystem::path& vplPath) {
    std::string content = readFileContents(vplPath);
    std::istringstream stream(content);
    std::string line;

    VendorPart* currentPart = nullptr;

    while (std::getline(stream, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;

        auto tokens = tokenize(line);
        if (tokens.empty()) continue;

        std::string keyword = toUpper(tokens[0]);

        // PART <part_number>
        if (keyword == "PART" && tokens.size() >= 2) {
            VendorPart part;
            part.partNumber = tokens[1];
            parts.push_back(part);
            currentPart = &parts.back();
        }
        // VENDOR <vendor_name> <vendor_pn>
        else if (keyword == "VENDOR" && tokens.size() >= 2 && currentPart) {
            currentPart->vendorName = tokens[1];
            if (tokens.size() >= 3) currentPart->vendorPartNumber = tokens[2];
        }
        // DESCRIPTION <text>
        else if (keyword == "DESCRIPTION" && tokens.size() >= 2 && currentPart) {
            currentPart->description = tokens[1];
        }
        // CATEGORY <category>
        else if (keyword == "CATEGORY" && tokens.size() >= 2 && currentPart) {
            currentPart->category = tokens[1];
        }
        // PRICE <value> [currency]
        else if (keyword == "PRICE" && tokens.size() >= 2 && currentPart) {
            currentPart->unitPrice = std::stod(tokens[1]);
            if (tokens.size() >= 3) currentPart->currency = tokens[2];
        }
        // LEAD_TIME <days>
        else if (keyword == "LEAD_TIME" && tokens.size() >= 2 && currentPart) {
            currentPart->leadTime = std::stoi(tokens[1]);
        }
        // MIN_QTY <qty>
        else if (keyword == "MIN_QTY" && tokens.size() >= 2 && currentPart) {
            currentPart->minOrderQty = std::stoi(tokens[1]);
        }
        // DATASHEET <url>
        else if (keyword == "DATASHEET" && tokens.size() >= 2 && currentPart) {
            currentPart->datasheet = tokens[1];
        }
        // Key=value pairs
        else if (currentPart) {
            size_t eqPos = line.find('=');
            if (eqPos != std::string::npos) {
                std::string key = trim(line.substr(0, eqPos));
                std::string value = trim(line.substr(eqPos + 1));
                currentPart->attributes[key] = value;
            }
        }
    }
}

// ============================================================================
// Customer Information Parsing
// ============================================================================

void OdbReader::parseCustomerInfo(CustomerInfo& info, const std::filesystem::path& infoPath) {
    std::string content = readFileContents(infoPath);
    std::istringstream stream(content);
    std::string line;

    while (std::getline(stream, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;

        size_t eqPos = line.find('=');
        if (eqPos != std::string::npos) {
            std::string key = trim(line.substr(0, eqPos));
            std::string value = trim(line.substr(eqPos + 1));

            // Remove quotes if present
            if (value.size() >= 2 && value.front() == '\'' && value.back() == '\'') {
                value = value.substr(1, value.size() - 2);
            }
            if (value.size() >= 2 && value.front() == '"' && value.back() == '"') {
                value = value.substr(1, value.size() - 2);
            }

            std::string keyUpper = toUpper(key);

            if (keyUpper == "NAME" || keyUpper == "CUSTOMER_NAME") {
                info.name = value;
            } else if (keyUpper == "COMPANY" || keyUpper == "COMPANY_NAME") {
                info.company = value;
            } else if (keyUpper == "PROJECT" || keyUpper == "PROJECT_NAME") {
                info.projectName = value;
            } else if (keyUpper == "PROJECT_NUMBER" || keyUpper == "ORDER_NUMBER") {
                info.projectNumber = value;
            } else if (keyUpper == "REVISION" || keyUpper == "REV") {
                info.revision = value;
            } else if (keyUpper == "CONTACT" || keyUpper == "CONTACT_NAME") {
                info.contact = value;
            } else if (keyUpper == "EMAIL" || keyUpper == "CONTACT_EMAIL") {
                info.email = value;
            } else if (keyUpper == "PHONE" || keyUpper == "CONTACT_PHONE") {
                info.phone = value;
            } else if (keyUpper == "ADDRESS") {
                info.address = value;
            } else if (keyUpper == "NOTES" || keyUpper == "COMMENTS") {
                info.notes = value;
            } else {
                info.attributes[key] = value;
            }
        }
    }
}

// ============================================================================
// Feature Parsing
// ============================================================================

std::unique_ptr<Feature> OdbReader::parseFeatureLine(const std::string& line,
                                                     const std::vector<std::string>& symbolNames) {
    auto tokens = tokenize(line);
    if (tokens.empty()) return nullptr;

    char type = tokens[0][0];

    switch (type) {
        case 'L':
            return parseLineFeature(tokens, symbolNames);
        case 'P':
            return parsePadFeature(tokens, symbolNames);
        case 'A':
            return parseArcFeature(tokens, symbolNames);
        case 'T':
            return parseTextFeature(tokens);
        default:
            return nullptr;
    }
}

std::unique_ptr<LineFeature> OdbReader::parseLineFeature(const std::vector<std::string>& tokens,
                                                         const std::vector<std::string>& symbolNames) {
    // L <xs> <ys> <xe> <ye> <sym_num> <polarity> <dcode>
    if (tokens.size() < 7) return nullptr;

    double xs = std::stod(tokens[1]);
    double ys = std::stod(tokens[2]);
    double xe = std::stod(tokens[3]);
    double ye = std::stod(tokens[4]);
    int symNum = std::stoi(tokens[5]);

    std::string symbolName;
    if (symNum >= 0 && static_cast<size_t>(symNum) < symbolNames.size()) {
        symbolName = symbolNames[static_cast<size_t>(symNum)];
    }

    auto line = std::make_unique<LineFeature>(xs, ys, xe, ye, symbolName);
    line->setSymbolIndex(symNum);

    if (tokens[6] == "N") {
        line->setPolarity(Polarity::Negative);
    }

    if (tokens.size() > 7) {
        line->setDcode(std::stoi(tokens[7]));
    }

    return line;
}

std::unique_ptr<PadFeature> OdbReader::parsePadFeature(const std::vector<std::string>& tokens,
                                                       const std::vector<std::string>& symbolNames) {
    // P <x> <y> <apt_def> <polarity> <dcode> <orient_def>
    if (tokens.size() < 6) return nullptr;

    double x = std::stod(tokens[1]);
    double y = std::stod(tokens[2]);

    auto pad = std::make_unique<PadFeature>();
    pad->setPosition(x, y);

    // Parse apt_def (can be -1 <sym_num> <resize> or just <sym_num>)
    size_t idx = 3;
    int symNum = std::stoi(tokens[idx]);

    if (symNum == -1 && tokens.size() > idx + 2) {
        // Resized symbol
        pad->setHasResize(true);
        symNum = std::stoi(tokens[++idx]);
        pad->setResizeFactor(std::stod(tokens[++idx]));
    }

    if (symNum >= 0 && static_cast<size_t>(symNum) < symbolNames.size()) {
        pad->setSymbolName(symbolNames[static_cast<size_t>(symNum)]);
    }
    pad->setSymbolIndex(symNum);

    ++idx;
    if (idx < tokens.size() && tokens[idx] == "N") {
        pad->setPolarity(Polarity::Negative);
    }

    ++idx;
    if (idx < tokens.size()) {
        pad->setDcode(std::stoi(tokens[idx]));
    }

    // Parse orientation
    ++idx;
    if (idx < tokens.size()) {
        double rotation = 0.0;
        bool mirror = false;
        parseOrientDef(tokens[idx], rotation, mirror);

        // Check for additional rotation angle
        if ((tokens[idx] == "8" || tokens[idx] == "9") && idx + 1 < tokens.size()) {
            rotation = std::stod(tokens[idx + 1]);
        }

        pad->setRotation(rotation);
        pad->setMirrored(mirror);
    }

    return pad;
}

std::unique_ptr<ArcFeature> OdbReader::parseArcFeature(const std::vector<std::string>& tokens,
                                                       const std::vector<std::string>& symbolNames) {
    // A <xs> <ys> <xe> <ye> <xc> <yc> <sym_num> <polarity> <dcode> <cw>
    if (tokens.size() < 10) return nullptr;

    double xs = std::stod(tokens[1]);
    double ys = std::stod(tokens[2]);
    double xe = std::stod(tokens[3]);
    double ye = std::stod(tokens[4]);
    double xc = std::stod(tokens[5]);
    double yc = std::stod(tokens[6]);
    int symNum = std::stoi(tokens[7]);

    std::string symbolName;
    if (symNum >= 0 && static_cast<size_t>(symNum) < symbolNames.size()) {
        symbolName = symbolNames[static_cast<size_t>(symNum)];
    }

    bool cw = (tokens[10] == "Y");

    auto arc = std::make_unique<ArcFeature>(xs, ys, xe, ye, xc, yc, symbolName, cw);
    arc->setSymbolIndex(symNum);

    if (tokens[8] == "N") {
        arc->setPolarity(Polarity::Negative);
    }

    arc->setDcode(std::stoi(tokens[9]));

    return arc;
}

std::unique_ptr<TextFeature> OdbReader::parseTextFeature(const std::vector<std::string>& tokens) {
    // T <x> <y> <font> <polarity> <orient_def> <xsize> <ysize> <width_factor> <text> <version>
    if (tokens.size() < 10) return nullptr;

    double x = std::stod(tokens[1]);
    double y = std::stod(tokens[2]);
    std::string font = tokens[3];

    auto text = std::make_unique<TextFeature>();
    text->setPosition(x, y);
    text->setFont(font);

    if (tokens[4] == "N") {
        text->setPolarity(Polarity::Negative);
    }

    // Orientation
    double rotation = 0.0;
    bool mirror = false;
    parseOrientDef(tokens[5], rotation, mirror);
    size_t nextIdx = 6;
    if ((tokens[5] == "8" || tokens[5] == "9") && tokens.size() > 6) {
        rotation = std::stod(tokens[6]);
        nextIdx = 7;
    }
    text->setRotation(rotation);
    text->setMirrored(mirror);

    if (nextIdx + 2 < tokens.size()) {
        text->setSize(std::stod(tokens[nextIdx]), std::stod(tokens[nextIdx + 1]));
        nextIdx += 2;
    }

    if (nextIdx < tokens.size()) {
        text->setWidthFactor(std::stod(tokens[nextIdx]));
        nextIdx++;
    }

    // Text string (may be quoted)
    if (nextIdx < tokens.size()) {
        std::string textStr = tokens[nextIdx];
        // Remove quotes
        if (textStr.size() >= 2 &&
            ((textStr.front() == '\'' && textStr.back() == '\'') ||
             (textStr.front() == '"' && textStr.back() == '"'))) {
            textStr = textStr.substr(1, textStr.size() - 2);
        }
        text->setText(textStr);
        nextIdx++;
    }

    if (nextIdx < tokens.size()) {
        text->setVersion(std::stoi(tokens[nextIdx]));
    }

    return text;
}

std::unique_ptr<SurfaceFeature> OdbReader::parseSurfaceFeature(std::istream& stream,
                                                               const std::string& firstLine) {
    auto surface = std::make_unique<SurfaceFeature>();

    // Parse first line: S <polarity> <dcode>;...
    auto tokens = tokenize(firstLine);
    if (tokens.size() >= 2 && tokens[1] == "N") {
        surface->setPolarity(Polarity::Negative);
    }
    if (tokens.size() >= 3) {
        surface->setDcode(std::stoi(tokens[2]));
    }

    // Parse contours
    std::string line;
    std::unique_ptr<Contour> currentContour;

    while (std::getline(stream, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;

        if (line == "SE") {
            // End of surface
            if (currentContour) {
                surface->addContour(std::move(*currentContour));
            }
            break;
        }

        tokens = tokenize(line);
        if (tokens.empty()) continue;

        if (tokens[0] == "OB" && tokens.size() >= 4) {
            if (currentContour) {
                surface->addContour(std::move(*currentContour));
            }
            double x = std::stod(tokens[1]);
            double y = std::stod(tokens[2]);
            PolygonType type = (tokens[3] == "H") ? PolygonType::Hole : PolygonType::Island;
            currentContour = std::make_unique<Contour>(x, y, type);
        }
        else if (tokens[0] == "OS" && tokens.size() >= 3 && currentContour) {
            double x = std::stod(tokens[1]);
            double y = std::stod(tokens[2]);
            currentContour->addLineSegment(x, y);
        }
        else if (tokens[0] == "OC" && tokens.size() >= 6 && currentContour) {
            double xe = std::stod(tokens[1]);
            double ye = std::stod(tokens[2]);
            double xc = std::stod(tokens[3]);
            double yc = std::stod(tokens[4]);
            bool cw = (tokens[5] == "Y");
            currentContour->addArcSegment(xe, ye, xc, yc, cw);
        }
        else if (tokens[0] == "OE" && currentContour) {
            surface->addContour(std::move(*currentContour));
            currentContour.reset();
        }
    }

    return surface;
}

// ============================================================================
// Utility Functions
// ============================================================================

std::string OdbReader::decompressFile(const std::filesystem::path& compressedPath) {
    // Open compressed file
    gzFile gz = gzopen(compressedPath.string().c_str(), "rb");
    if (!gz) {
        lastError_ = "Cannot open compressed file: " + compressedPath.string();
        return "";
    }

    // Read decompressed data in chunks
    std::string result;
    constexpr size_t bufferSize = 65536;  // 64KB buffer
    std::vector<char> buffer(bufferSize);

    int bytesRead;
    while ((bytesRead = gzread(gz, buffer.data(), static_cast<unsigned int>(bufferSize))) > 0) {
        result.append(buffer.data(), static_cast<size_t>(bytesRead));
    }

    // Check for errors
    int errnum;
    const char* errMsg = gzerror(gz, &errnum);
    if (errnum != Z_OK && errnum != Z_STREAM_END) {
        lastError_ = "Decompression error: " + std::string(errMsg ? errMsg : "unknown");
        gzclose(gz);
        return "";
    }

    gzclose(gz);
    return result;
}

std::string OdbReader::readFileContents(const std::filesystem::path& filePath, bool decompress) {
    std::filesystem::path actualPath = filePath;

    // Check for compressed version
    if (!fileExists(actualPath)) {
        auto compressed = std::filesystem::path(filePath.string() + ".z");
        if (fileExists(compressed)) {
            if (decompress && options_.decompressFeatures) {
                return decompressFile(compressed);
            }
            return "";
        }
        return "";
    }

    // Check if file is compressed
    if (actualPath.extension() == ".z") {
        if (decompress && options_.decompressFeatures) {
            return decompressFile(actualPath);
        }
        return "";
    }

    // Read plain file
    std::ifstream file(actualPath);
    if (!file) {
        lastError_ = "Cannot open file: " + actualPath.string();
        return "";
    }

    std::ostringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void OdbReader::reportProgress(const std::string& message, double progress) {
    if (progressCallback_) {
        progressCallback_(message, progress);
    }
}

std::unordered_map<std::string, std::string> OdbReader::parseStructuredFile(const std::string& content) {
    std::unordered_map<std::string, std::string> result;
    std::istringstream stream(content);
    std::string line;

    while (std::getline(stream, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;

        auto eqPos = line.find('=');
        if (eqPos != std::string::npos) {
            std::string key = trim(line.substr(0, eqPos));
            std::string value = trim(line.substr(eqPos + 1));
            result[key] = value;
        }
    }

    return result;
}

std::vector<std::string> OdbReader::tokenize(const std::string& line) {
    std::vector<std::string> tokens;
    std::string current;
    bool inQuote = false;
    char quoteChar = 0;

    for (size_t i = 0; i < line.length(); ++i) {
        char c = line[i];

        // Handle quotes
        if ((c == '\'' || c == '"') && !inQuote) {
            inQuote = true;
            quoteChar = c;
            current += c;
            continue;
        }
        if (c == quoteChar && inQuote) {
            inQuote = false;
            current += c;
            continue;
        }

        // Handle separators
        if (!inQuote && (c == ' ' || c == '\t' || c == ';')) {
            if (!current.empty()) {
                tokens.push_back(current);
                current.clear();
            }
            continue;
        }

        current += c;
    }

    if (!current.empty()) {
        tokens.push_back(current);
    }

    return tokens;
}

void OdbReader::parseOrientDef(const std::string& orientStr, double& rotation, bool& mirror) {
    if (orientStr.empty()) return;

    int orient = std::stoi(orientStr);

    switch (orient) {
        case 0: rotation = 0.0; mirror = false; break;
        case 1: rotation = 90.0; mirror = false; break;
        case 2: rotation = 180.0; mirror = false; break;
        case 3: rotation = 270.0; mirror = false; break;
        case 4: rotation = 0.0; mirror = true; break;
        case 5: rotation = 90.0; mirror = true; break;
        case 6: rotation = 180.0; mirror = true; break;
        case 7: rotation = 270.0; mirror = true; break;
        case 8: rotation = 0.0; mirror = false; break;  // Angle follows
        case 9: rotation = 0.0; mirror = true; break;   // Angle follows
        default: rotation = 0.0; mirror = false; break;
    }
}

void OdbReader::parseFeatureAttributes(const std::string& attrStr, Feature& feature,
                                       const std::vector<std::string>& attrNames,
                                       const std::vector<std::string>& /* attrStrings */) {
    // Parse attribute string: <num>=<value>,<num>=<value>,...
    // Or just <num>,<num> for boolean attributes
    std::istringstream stream(attrStr);
    std::string token;

    while (std::getline(stream, token, ',')) {
        token = trim(token);
        if (token.empty()) continue;

        auto eqPos = token.find('=');
        if (eqPos != std::string::npos) {
            int attrNum = std::stoi(token.substr(0, eqPos));
            std::string value = token.substr(eqPos + 1);

            if (attrNum >= 0 && static_cast<size_t>(attrNum) < attrNames.size()) {
                feature.setAttribute(attrNames[static_cast<size_t>(attrNum)], value);
            }
        } else {
            // Boolean attribute
            int attrNum = std::stoi(token);
            if (attrNum >= 0 && static_cast<size_t>(attrNum) < attrNames.size()) {
                feature.setAttribute(attrNames[static_cast<size_t>(attrNum)], "true");
            }
        }
    }
}

} // namespace koo::ecad
