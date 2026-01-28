#pragma once

#include <cmath>

namespace koo::util {

// Basic math constants
constexpr double PI = 3.14159265358979323846;
constexpr double DEG_TO_RAD = PI / 180.0;
constexpr double RAD_TO_DEG = 180.0 / PI;

// Basic math functions
inline double toRadians(double degrees) {
    return degrees * DEG_TO_RAD;
}

inline double toDegrees(double radians) {
    return radians * RAD_TO_DEG;
}

inline double clamp(double value, double min, double max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

} // namespace koo::util
