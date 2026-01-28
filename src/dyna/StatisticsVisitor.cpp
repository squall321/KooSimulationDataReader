#include <koo/dyna/StatisticsVisitor.hpp>
#include <iostream>
#include <iomanip>

namespace koo::dyna {

void StatisticsVisitor::printSummary(std::ostream& os) const {
    os << "\n";
    os << "===============================================\n";
    os << "           MODEL STATISTICS SUMMARY             \n";
    os << "===============================================\n\n";

    // Nodes
    if (nodeCount_ > 0) {
        os << "Nodes:\n";
        os << "  Total Nodes:           " << std::setw(10) << nodeCount_ << "\n";
        os << "\n";
    }

    // Elements
    size_t totalElements = getTotalElementCount();
    if (totalElements > 0) {
        os << "Elements:\n";
        os << "  Total Elements:        " << std::setw(10) << totalElements << "\n";
        if (shellElementCount_ > 0) {
            os << "    Shell Elements:      " << std::setw(10) << shellElementCount_ << "\n";
        }
        if (solidElementCount_ > 0) {
            os << "    Solid Elements:      " << std::setw(10) << solidElementCount_ << "\n";
        }
        if (beamElementCount_ > 0) {
            os << "    Beam Elements:       " << std::setw(10) << beamElementCount_ << "\n";
        }
        if (discreteElementCount_ > 0) {
            os << "    Discrete Elements:   " << std::setw(10) << discreteElementCount_ << "\n";
        }
        os << "\n";
    }

    // Parts
    if (partCount_ > 0) {
        os << "Parts:\n";
        os << "  Total Parts:           " << std::setw(10) << partCount_ << "\n";
        os << "\n";
    }

    // Materials
    if (materialCount_ > 0) {
        os << "Materials:\n";
        os << "  Total Materials:       " << std::setw(10) << materialCount_ << "\n";
        if (!materialTypes_.empty()) {
            os << "    Material Types:\n";
            for (const auto& [type, count] : materialTypes_) {
                os << "      - " << std::left << std::setw(30) << type
                   << std::right << std::setw(10) << count << "\n";
            }
        }
        os << "\n";
    }

    // Sections
    if (sectionCount_ > 0) {
        os << "Sections:\n";
        os << "  Total Sections:        " << std::setw(10) << sectionCount_ << "\n";
        if (!sectionTypes_.empty()) {
            os << "    Section Types:\n";
            for (const auto& [type, count] : sectionTypes_) {
                os << "      - " << std::left << std::setw(30) << type
                   << std::right << std::setw(10) << count << "\n";
            }
        }
        os << "\n";
    }

    os << "===============================================\n";
}

} // namespace koo::dyna
