#include <koo/dyna/ValidationVisitor.hpp>
#include <iostream>

namespace koo::dyna {

void ValidationVisitor::printMessages(std::ostream& os, ValidationSeverity minSeverity) const {
    const char* severityStr[] = {"INFO", "WARNING", "ERROR"};
    const char* severityColor[] = {"\033[36m", "\033[33m", "\033[31m"}; // Cyan, Yellow, Red
    const char* resetColor = "\033[0m";

    size_t printedCount = 0;

    for (const auto& msg : messages_) {
        if (msg.severity < minSeverity) {
            continue;
        }

        os << severityColor[static_cast<int>(msg.severity)]
           << "[" << severityStr[static_cast<int>(msg.severity)] << "]"
           << resetColor
           << " " << msg.category << ": " << msg.message << "\n";

        printedCount++;
    }

    if (printedCount == 0 && minSeverity == ValidationSeverity::Info) {
        os << "\033[32m[SUCCESS]\033[0m No validation issues found.\n";
    }

    // Summary
    size_t errorCount = getErrorCount();
    size_t warningCount = getWarningCount();

    if (printedCount > 0 || errorCount > 0 || warningCount > 0) {
        os << "\n";
        os << "Validation Summary:\n";
        if (errorCount > 0) {
            os << "  \033[31mErrors:   " << errorCount << "\033[0m\n";
        }
        if (warningCount > 0) {
            os << "  \033[33mWarnings: " << warningCount << "\033[0m\n";
        }
    }
}

} // namespace koo::dyna
