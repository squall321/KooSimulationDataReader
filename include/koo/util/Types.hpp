#pragma once

#include <array>
#include <cmath>
#include <cstdint>

namespace koo {

// Basic numeric types
using NodeId = int64_t;
using ElementId = int64_t;
using PartId = int64_t;
using MaterialId = int64_t;
using SectionId = int64_t;
using SetId = int64_t;

// 3D Vector
struct Vec3 {
    double x = 0.0;
    double y = 0.0;
    double z = 0.0;

    Vec3() = default;
    Vec3(double x_, double y_, double z_) : x(x_), y(y_), z(z_) {}

    Vec3 operator+(const Vec3& other) const {
        return {x + other.x, y + other.y, z + other.z};
    }

    Vec3 operator-(const Vec3& other) const {
        return {x - other.x, y - other.y, z - other.z};
    }

    Vec3 operator*(double scalar) const {
        return {x * scalar, y * scalar, z * scalar};
    }

    Vec3 operator/(double scalar) const {
        return {x / scalar, y / scalar, z / scalar};
    }

    Vec3& operator+=(const Vec3& other) {
        x += other.x; y += other.y; z += other.z;
        return *this;
    }

    Vec3& operator-=(const Vec3& other) {
        x -= other.x; y -= other.y; z -= other.z;
        return *this;
    }

    double dot(const Vec3& other) const {
        return x * other.x + y * other.y + z * other.z;
    }

    Vec3 cross(const Vec3& other) const {
        return {
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
        };
    }

    double length() const {
        return std::sqrt(x * x + y * y + z * z);
    }

    double lengthSquared() const {
        return x * x + y * y + z * z;
    }

    Vec3 normalized() const {
        double len = length();
        if (len > 0.0) {
            return *this / len;
        }
        return *this;
    }

    bool operator==(const Vec3& other) const {
        return x == other.x && y == other.y && z == other.z;
    }

    bool operator!=(const Vec3& other) const {
        return !(*this == other);
    }
};

inline Vec3 operator*(double scalar, const Vec3& v) {
    return v * scalar;
}

// Bounding Box
struct BoundingBox {
    Vec3 min;
    Vec3 max;

    BoundingBox() : min{1e30, 1e30, 1e30}, max{-1e30, -1e30, -1e30} {}
    BoundingBox(const Vec3& min_, const Vec3& max_) : min(min_), max(max_) {}

    void expand(const Vec3& point) {
        if (point.x < min.x) min.x = point.x;
        if (point.y < min.y) min.y = point.y;
        if (point.z < min.z) min.z = point.z;
        if (point.x > max.x) max.x = point.x;
        if (point.y > max.y) max.y = point.y;
        if (point.z > max.z) max.z = point.z;
    }

    void expand(const BoundingBox& other) {
        expand(other.min);
        expand(other.max);
    }

    bool contains(const Vec3& point) const {
        return point.x >= min.x && point.x <= max.x &&
               point.y >= min.y && point.y <= max.y &&
               point.z >= min.z && point.z <= max.z;
    }

    Vec3 center() const {
        return (min + max) * 0.5;
    }

    Vec3 size() const {
        return max - min;
    }

    bool isValid() const {
        return min.x <= max.x && min.y <= max.y && min.z <= max.z;
    }
};

// 4x4 Transformation Matrix (row-major)
struct Matrix4x4 {
    std::array<double, 16> data;

    Matrix4x4() : data{1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1} {}

    double& operator()(int row, int col) {
        return data[static_cast<size_t>(row * 4 + col)];
    }

    double operator()(int row, int col) const {
        return data[static_cast<size_t>(row * 4 + col)];
    }

    Vec3 operator*(const Vec3& v) const {
        double w = data[12] * v.x + data[13] * v.y + data[14] * v.z + data[15];
        return {
            (data[0] * v.x + data[1] * v.y + data[2] * v.z + data[3]) / w,
            (data[4] * v.x + data[5] * v.y + data[6] * v.z + data[7]) / w,
            (data[8] * v.x + data[9] * v.y + data[10] * v.z + data[11]) / w
        };
    }

    static Matrix4x4 identity() {
        return Matrix4x4();
    }

    static Matrix4x4 translation(double tx, double ty, double tz) {
        Matrix4x4 m;
        m(0, 3) = tx;
        m(1, 3) = ty;
        m(2, 3) = tz;
        return m;
    }

    static Matrix4x4 scale(double sx, double sy, double sz) {
        Matrix4x4 m;
        m(0, 0) = sx;
        m(1, 1) = sy;
        m(2, 2) = sz;
        return m;
    }
};

// Element quality metrics
struct ElementQuality {
    double aspectRatio = 0.0;
    double jacobian = 0.0;
    double skewness = 0.0;
    double warpage = 0.0;
};

} // namespace koo
