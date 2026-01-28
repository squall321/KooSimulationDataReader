#pragma once

namespace koo::common {

struct Vec3 {
    double x = 0.0;
    double y = 0.0;
    double z = 0.0;

    Vec3() = default;
    Vec3(double x_, double y_, double z_) : x(x_), y(y_), z(z_) {}
};

struct BoundingBox {
    Vec3 min;
    Vec3 max;
};

} // namespace koo::common
