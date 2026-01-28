#pragma once

#include <koo/Export.hpp>
#include <vector>

namespace koo::dyna {

/**
 * @brief Load curve for time-dependent values
 */
struct KOO_API Curve {
    int id = 0;
    std::vector<double> time;
    std::vector<double> value;
};

} // namespace koo::dyna
