#include <koo/Version.hpp>
#include <koo/dyna/Model.hpp>
#include <koo/dyna/KeywordFileReader.hpp>
#include <koo/dyna/KeywordFileWriter.hpp>
#include <koo/dyna/KeywordFactory.hpp>

#include <iostream>
#include <string>
#include <vector>
#include <filesystem>

namespace fs = std::filesystem;

void printUsage(const char* programName) {
    std::cout << "KooSimulationDataReader CLI v" << koo::Version::toString() << "\n\n";
    std::cout << "Usage: " << programName << " <command> [options]\n\n";
    std::cout << "Commands:\n";
    std::cout << "  info <file>           Show model information\n";
    std::cout << "  validate <file>       Validate K-file\n";
    std::cout << "  convert <in> <out>    Convert between formats\n";
    std::cout << "  keywords              List supported keywords\n";
    std::cout << "  version               Show version\n";
    std::cout << "  help                  Show this help\n";
}

void printVersion() {
    std::cout << "KooSimulationDataReader v" << koo::Version::toString() << "\n";
    std::cout << "LS-DYNA keyword file processing library\n";
}

void printModelInfo(const koo::dyna::Model& model) {
    std::cout << "Model Information:\n";
    std::cout << "  Title: " << model.getTitle() << "\n";
    std::cout << "  File: " << model.getFilePath().string() << "\n";
    std::cout << "\n";
    std::cout << "Statistics:\n";
    std::cout << "  Keywords: " << model.getKeywords().size() << "\n";
    std::cout << "  Nodes: " << model.getNodeCount() << "\n";
    std::cout << "  Shell Elements: " << model.getShellElementCount() << "\n";
    std::cout << "  Solid Elements: " << model.getSolidElementCount() << "\n";
    std::cout << "  Parts: " << model.getPartCount() << "\n";
    std::cout << "  Materials: " << model.getMaterials().size() << "\n";
    std::cout << "  Sections: " << model.getSections().size() << "\n";

    auto bbox = model.getBoundingBox();
    if (bbox.isValid()) {
        std::cout << "\n";
        std::cout << "Bounding Box:\n";
        std::cout << "  Min: (" << bbox.min.x << ", " << bbox.min.y << ", " << bbox.min.z << ")\n";
        std::cout << "  Max: (" << bbox.max.x << ", " << bbox.max.y << ", " << bbox.max.z << ")\n";
        auto size = bbox.size();
        std::cout << "  Size: (" << size.x << ", " << size.y << ", " << size.z << ")\n";
    }
}

int cmdInfo(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Error: Missing file argument\n";
        std::cerr << "Usage: " << argv[0] << " info <file>\n";
        return 1;
    }

    fs::path filepath = argv[2];
    if (!fs::exists(filepath)) {
        std::cerr << "Error: File not found: " << filepath << "\n";
        return 1;
    }

    koo::dyna::KeywordFileReader reader;
    auto model = reader.read(filepath);

    if (reader.hasErrors()) {
        std::cerr << "Errors:\n";
        for (const auto& error : reader.getErrors()) {
            std::cerr << "  " << error << "\n";
        }
        return 1;
    }

    printModelInfo(model);

    if (!reader.getWarnings().empty()) {
        std::cout << "\nWarnings:\n";
        for (const auto& warning : reader.getWarnings()) {
            std::cout << "  " << warning << "\n";
        }
    }

    return 0;
}

int cmdValidate(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Error: Missing file argument\n";
        std::cerr << "Usage: " << argv[0] << " validate <file>\n";
        return 1;
    }

    fs::path filepath = argv[2];
    if (!fs::exists(filepath)) {
        std::cerr << "Error: File not found: " << filepath << "\n";
        return 1;
    }

    koo::dyna::KeywordFileReader reader;
    auto model = reader.read(filepath);

    bool valid = true;

    if (reader.hasErrors()) {
        valid = false;
        std::cerr << "Parse Errors:\n";
        for (const auto& error : reader.getErrors()) {
            std::cerr << "  " << error << "\n";
        }
    }

    if (!reader.getWarnings().empty()) {
        std::cout << "Warnings:\n";
        for (const auto& warning : reader.getWarnings()) {
            std::cout << "  " << warning << "\n";
        }
    }

    if (valid) {
        std::cout << "Validation passed.\n";
        printModelInfo(model);
        return 0;
    }

    return 1;
}

int cmdConvert(int argc, char* argv[]) {
    if (argc < 4) {
        std::cerr << "Error: Missing arguments\n";
        std::cerr << "Usage: " << argv[0] << " convert <input> <output>\n";
        return 1;
    }

    fs::path inputPath = argv[2];
    fs::path outputPath = argv[3];

    if (!fs::exists(inputPath)) {
        std::cerr << "Error: Input file not found: " << inputPath << "\n";
        return 1;
    }

    // Read input
    koo::dyna::KeywordFileReader reader;
    auto model = reader.read(inputPath);

    if (reader.hasErrors()) {
        std::cerr << "Read Errors:\n";
        for (const auto& error : reader.getErrors()) {
            std::cerr << "  " << error << "\n";
        }
        return 1;
    }

    // Write output
    koo::dyna::KeywordFileWriter writer;
    if (!writer.write(model, outputPath)) {
        std::cerr << "Write Error: " << writer.getError() << "\n";
        return 1;
    }

    std::cout << "Converted: " << inputPath << " -> " << outputPath << "\n";
    return 0;
}

int cmdKeywords(int /*argc*/, char* /*argv*/[]) {
    auto& factory = koo::dyna::KeywordFactory::instance();
    auto keywords = factory.getRegisteredKeywords();

    std::cout << "Registered Keywords (" << keywords.size() << "):\n";
    for (const auto& kw : keywords) {
        std::cout << "  " << kw << "\n";
    }

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage(argv[0]);
        return 0;
    }

    std::string command = argv[1];

    if (command == "help" || command == "-h" || command == "--help") {
        printUsage(argv[0]);
        return 0;
    }

    if (command == "version" || command == "-v" || command == "--version") {
        printVersion();
        return 0;
    }

    if (command == "info") {
        return cmdInfo(argc, argv);
    }

    if (command == "validate") {
        return cmdValidate(argc, argv);
    }

    if (command == "convert") {
        return cmdConvert(argc, argv);
    }

    if (command == "keywords") {
        return cmdKeywords(argc, argv);
    }

    std::cerr << "Unknown command: " << command << "\n";
    printUsage(argv[0]);
    return 1;
}
