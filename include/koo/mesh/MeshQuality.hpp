#pragma once

#include <koo/Export.hpp>
#include <string>
#include <sstream>
#include <iomanip>
#include <iostream>

namespace koo::mesh {

/**
 * @brief Mesh quality metrics
 *
 * Provides quality measures for mesh validation:
 * - Aspect ratio: ratio of largest to smallest element dimension
 * - Jacobian: measure of element distortion
 * - Skewness: deviation from ideal element shape
 * - Degenerate/inverted element detection
 */
struct KOO_API MeshQuality {
    // ========================================================================
    // Aspect Ratio (AR)
    // ========================================================================

    double minAspectRatio = 1.0;   ///< Minimum aspect ratio (best: 1.0)
    double maxAspectRatio = 1.0;   ///< Maximum aspect ratio (worst)
    double avgAspectRatio = 1.0;   ///< Average aspect ratio

    // ========================================================================
    // Jacobian (J)
    // ========================================================================

    double minJacobian = 1.0;      ///< Minimum Jacobian (negative = inverted)
    double maxJacobian = 1.0;      ///< Maximum Jacobian
    double avgJacobian = 1.0;      ///< Average Jacobian

    // ========================================================================
    // Skewness (S)
    // ========================================================================

    double minSkewness = 0.0;      ///< Minimum skewness (best: 0.0)
    double maxSkewness = 0.0;      ///< Maximum skewness (worst: 1.0)
    double avgSkewness = 0.0;      ///< Average skewness

    // ========================================================================
    // Problem Elements
    // ========================================================================

    size_t numDegenerateElements = 0;  ///< Elements with zero/negative volume
    size_t numInvertedElements = 0;    ///< Elements with negative Jacobian
    size_t numPoorQualityElements = 0; ///< Elements below quality threshold

    // ========================================================================
    // Overall Statistics
    // ========================================================================

    size_t totalElements = 0;      ///< Total number of elements
    double qualityThreshold = 0.1; ///< Quality threshold for "poor" elements

    // ========================================================================
    // Quality Assessment
    // ========================================================================

    /**
     * @brief Check if mesh has acceptable quality
     * @param minAR Minimum acceptable aspect ratio
     * @param minJ Minimum acceptable Jacobian
     * @param maxS Maximum acceptable skewness
     * @return true if all quality metrics are acceptable
     */
    bool isAcceptable(double minAR = 0.1, double minJ = 0.01, double maxS = 0.95) const {
        return minAspectRatio >= minAR &&
               minJacobian >= minJ &&
               maxSkewness <= maxS &&
               numDegenerateElements == 0 &&
               numInvertedElements == 0;
    }

    /**
     * @brief Get quality grade (A-F)
     * @return Quality grade letter
     */
    char getGrade() const {
        if (numDegenerateElements > 0 || numInvertedElements > 0) return 'F';

        double poorRatio = static_cast<double>(numPoorQualityElements) / totalElements;
        if (poorRatio > 0.5) return 'F';
        if (poorRatio > 0.3) return 'D';
        if (poorRatio > 0.1) return 'C';
        if (poorRatio > 0.01) return 'B';
        return 'A';
    }

    /**
     * @brief Get quality summary string
     * @return Human-readable quality summary
     */
    std::string getSummary() const {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(3);

        oss << "Mesh Quality Summary\n";
        oss << "====================\n";
        oss << "Total elements: " << totalElements << "\n";
        oss << "\n";

        oss << "Aspect Ratio:\n";
        oss << "  Min: " << minAspectRatio << "\n";
        oss << "  Max: " << maxAspectRatio << "\n";
        oss << "  Avg: " << avgAspectRatio << "\n";
        oss << "\n";

        oss << "Jacobian:\n";
        oss << "  Min: " << minJacobian << "\n";
        oss << "  Max: " << maxJacobian << "\n";
        oss << "  Avg: " << avgJacobian << "\n";
        oss << "\n";

        oss << "Skewness:\n";
        oss << "  Min: " << minSkewness << "\n";
        oss << "  Max: " << maxSkewness << "\n";
        oss << "  Avg: " << avgSkewness << "\n";
        oss << "\n";

        oss << "Problem Elements:\n";
        oss << "  Degenerate: " << numDegenerateElements << "\n";
        oss << "  Inverted:   " << numInvertedElements << "\n";
        oss << "  Poor:       " << numPoorQualityElements << "\n";
        oss << "\n";

        oss << "Grade: " << getGrade() << "\n";

        return oss.str();
    }

    /**
     * @brief Print summary to console
     */
    void print() const {
        std::cout << getSummary();
    }
};

} // namespace koo::mesh
