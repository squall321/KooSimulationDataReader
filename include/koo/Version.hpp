#pragma once

#include <string>

#define KOO_SIM_VERSION_MAJOR 0
#define KOO_SIM_VERSION_MINOR 1
#define KOO_SIM_VERSION_PATCH 0

#define KOO_SIM_VERSION_STRING "0.1.0"

namespace koo {

struct Version {
    static constexpr int major = KOO_SIM_VERSION_MAJOR;
    static constexpr int minor = KOO_SIM_VERSION_MINOR;
    static constexpr int patch = KOO_SIM_VERSION_PATCH;

    static std::string toString() {
        return KOO_SIM_VERSION_STRING;
    }
};

} // namespace koo
