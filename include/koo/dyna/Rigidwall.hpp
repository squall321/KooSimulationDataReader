#pragma once

#include <koo/Export.hpp>
#include <koo/dyna/Keyword.hpp>
#include <koo/util/Types.hpp>
#include <vector>

namespace koo::dyna {

/**
 * @brief Base class for *RIGIDWALL keywords
 */
class KOO_API RigidwallKeyword : public Keyword {
public:
    virtual ~RigidwallKeyword() = default;
};

/**
 * @brief *RIGIDWALL_PLANAR
 *
 * Defines a planar rigid wall.
 */
class KOO_API RigidwallPlanar : public CloneableKeyword<RigidwallPlanar, RigidwallKeyword> {
public:
    struct Data {
        int nsid = 0;           // Node set ID
        int nsidex = 0;         // Excluded node set ID
        int boxid = 0;          // Box ID
        int birth = 0;          // Birth time (uses offset for double)
        int death = 0;          // Death time (uses offset for double)
        double xt = 0.0;        // X coordinate of tail
        double yt = 0.0;        // Y coordinate of tail
        double zt = 0.0;        // Z coordinate of tail
        double xh = 0.0;        // X coordinate of head
        double yh = 0.0;        // Y coordinate of head
        double zh = 0.0;        // Z coordinate of head
        double fric = 0.0;      // Friction coefficient
        double wvel = 0.0;      // Wall velocity
    };

    RigidwallPlanar() = default;

    std::string getKeywordName() const override { return "*RIGIDWALL_PLANAR"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    Data& getData() { return data_; }
    const Data& getData() const { return data_; }

private:
    Data data_;
};

/**
 * @brief *RIGIDWALL_GEOMETRIC_FLAT
 *
 * Defines a flat geometric rigid wall.
 */
class KOO_API RigidwallGeometricFlat : public CloneableKeyword<RigidwallGeometricFlat, RigidwallKeyword> {
public:
    struct Data {
        int id = 0;             // Rigid wall ID
        std::string title;      // Title
        int nsid = 0;           // Node set ID
        int nsidex = 0;         // Excluded node set
        int boxid = 0;          // Box ID
        double birth = 0.0;     // Birth time
        double death = 1e20;    // Death time
        double xt = 0.0;        // Tail x
        double yt = 0.0;        // Tail y
        double zt = 0.0;        // Tail z
        double xh = 0.0;        // Head x
        double yh = 0.0;        // Head y
        double zh = 0.0;        // Head z
        double fric = 0.0;      // Friction
        double xhev = 0.0;      // Head velocity x
        double yhev = 0.0;      // Head velocity y
        double zhev = 0.0;      // Head velocity z
        double lenl = 0.0;      // Length in local x
        double lenm = 0.0;      // Length in local y
    };

    RigidwallGeometricFlat() = default;

    std::string getKeywordName() const override { return "*RIGIDWALL_GEOMETRIC_FLAT"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    Data& getData() { return data_; }
    const Data& getData() const { return data_; }

private:
    Data data_;
};

/**
 * @brief *RIGIDWALL_GEOMETRIC_CYLINDER
 *
 * Defines a cylindrical geometric rigid wall.
 */
class KOO_API RigidwallGeometricCylinder : public CloneableKeyword<RigidwallGeometricCylinder, RigidwallKeyword> {
public:
    struct Data {
        int id = 0;
        int nsid = 0;
        int nsidex = 0;
        int boxid = 0;
        double birth = 0.0;
        double death = 1e20;
        double xt = 0.0;        // Tail x
        double yt = 0.0;        // Tail y
        double zt = 0.0;        // Tail z
        double xh = 0.0;        // Head x
        double yh = 0.0;        // Head y
        double zh = 0.0;        // Head z
        double fric = 0.0;
        double radius = 0.0;    // Cylinder radius
        double length = 0.0;    // Cylinder length
    };

    RigidwallGeometricCylinder() = default;

    std::string getKeywordName() const override { return "*RIGIDWALL_GEOMETRIC_CYLINDER"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    Data& getData() { return data_; }
    const Data& getData() const { return data_; }

private:
    Data data_;
};

/**
 * @brief *RIGIDWALL_GEOMETRIC_SPHERE
 *
 * Defines a spherical geometric rigid wall.
 */
class KOO_API RigidwallGeometricSphere : public CloneableKeyword<RigidwallGeometricSphere, RigidwallKeyword> {
public:
    struct Data {
        int id = 0;
        int nsid = 0;
        int nsidex = 0;
        int boxid = 0;
        double birth = 0.0;
        double death = 1e20;
        double xc = 0.0;        // Center x
        double yc = 0.0;        // Center y
        double zc = 0.0;        // Center z
        double radius = 0.0;    // Sphere radius
        double fric = 0.0;
    };

    RigidwallGeometricSphere() = default;

    std::string getKeywordName() const override { return "*RIGIDWALL_GEOMETRIC_SPHERE"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    Data& getData() { return data_; }
    const Data& getData() const { return data_; }

private:
    Data data_;
};

/**
 * @brief *RIGIDWALL_PLANAR_MOVING
 *
 * Defines a moving planar rigid wall.
 */
class KOO_API RigidwallPlanarMoving : public CloneableKeyword<RigidwallPlanarMoving, RigidwallKeyword> {
public:
    struct Data {
        int nsid = 0;
        int nsidex = 0;
        int boxid = 0;
        double xt = 0.0;
        double yt = 0.0;
        double zt = 0.0;
        double xh = 0.0;
        double yh = 0.0;
        double zh = 0.0;
        double fric = 0.0;
        double vx = 0.0;        // Velocity x
        double vy = 0.0;        // Velocity y
        double vz = 0.0;        // Velocity z
    };

    RigidwallPlanarMoving() = default;

    std::string getKeywordName() const override { return "*RIGIDWALL_PLANAR_MOVING"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    Data& getData() { return data_; }
    const Data& getData() const { return data_; }

private:
    Data data_;
};

/**
 * @brief *RIGIDWALL_PLANAR_FINITE
 *
 * Defines a finite planar rigid wall.
 */
class KOO_API RigidwallPlanarFinite : public CloneableKeyword<RigidwallPlanarFinite, RigidwallKeyword> {
public:
    struct Data {
        int nsid = 0;
        int nsidex = 0;
        int boxid = 0;
        double xt = 0.0;
        double yt = 0.0;
        double zt = 0.0;
        double xh = 0.0;
        double yh = 0.0;
        double zh = 0.0;
        double fric = 0.0;
        double lenl = 0.0;      // Length in local l direction
        double lenm = 0.0;      // Length in local m direction
    };

    RigidwallPlanarFinite() = default;

    std::string getKeywordName() const override { return "*RIGIDWALL_PLANAR_FINITE"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    Data& getData() { return data_; }
    const Data& getData() const { return data_; }

private:
    Data data_;
};

/**
 * @brief *RIGIDWALL_PLANAR_FORCES
 *
 * Defines a planar rigid wall with force output.
 */
class KOO_API RigidwallPlanarForces : public CloneableKeyword<RigidwallPlanarForces, RigidwallKeyword> {
public:
    struct Data {
        int nsid = 0;
        int nsidex = 0;
        int boxid = 0;
        double xt = 0.0;
        double yt = 0.0;
        double zt = 0.0;
        double xh = 0.0;
        double yh = 0.0;
        double zh = 0.0;
        double fric = 0.0;
    };

    RigidwallPlanarForces() = default;

    std::string getKeywordName() const override { return "*RIGIDWALL_PLANAR_FORCES"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    Data& getData() { return data_; }
    const Data& getData() const { return data_; }

private:
    Data data_;
};

/**
 * @brief *RIGIDWALL_GEOMETRIC_CONE
 *
 * Defines a conical geometric rigid wall.
 */
class KOO_API RigidwallGeometricCone : public CloneableKeyword<RigidwallGeometricCone, RigidwallKeyword> {
public:
    struct Data {
        int id = 0;
        int nsid = 0;
        int nsidex = 0;
        int boxid = 0;
        double birth = 0.0;
        double death = 1e20;
        double xc = 0.0;        // Apex x
        double yc = 0.0;        // Apex y
        double zc = 0.0;        // Apex z
        double xh = 0.0;        // Axis direction x
        double yh = 0.0;        // Axis direction y
        double zh = 0.0;        // Axis direction z
        double angle = 0.0;     // Half angle
        double fric = 0.0;
    };

    RigidwallGeometricCone() = default;

    std::string getKeywordName() const override { return "*RIGIDWALL_GEOMETRIC_CONE"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    Data& getData() { return data_; }
    const Data& getData() const { return data_; }

private:
    Data data_;
};

/**
 * @brief *RIGIDWALL_PLANAR_ORTHO
 *
 * Defines an orthotropic planar rigid wall.
 */
class KOO_API RigidwallPlanarOrtho : public CloneableKeyword<RigidwallPlanarOrtho, RigidwallKeyword> {
public:
    struct Data {
        int nsid = 0;
        int nsidex = 0;
        int boxid = 0;
        double xt = 0.0;
        double yt = 0.0;
        double zt = 0.0;
        double xh = 0.0;
        double yh = 0.0;
        double zh = 0.0;
        double fricl = 0.0;     // Friction in local l direction
        double fricm = 0.0;     // Friction in local m direction
    };

    RigidwallPlanarOrtho() = default;

    std::string getKeywordName() const override { return "*RIGIDWALL_PLANAR_ORTHO"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    Data& getData() { return data_; }
    const Data& getData() const { return data_; }

private:
    Data data_;
};

/**
 * @brief *RIGIDWALL_GEOMETRIC_PRISM
 *
 * Defines a prismatic geometric rigid wall.
 */
class KOO_API RigidwallGeometricPrism : public CloneableKeyword<RigidwallGeometricPrism, RigidwallKeyword> {
public:
    struct Data {
        int id = 0;
        int nsid = 0;
        int nsidex = 0;
        int boxid = 0;
        double birth = 0.0;
        double death = 1e20;
        double xt = 0.0;          // Tail x
        double yt = 0.0;          // Tail y
        double zt = 0.0;          // Tail z
        double xh = 0.0;          // Head x
        double yh = 0.0;          // Head y
        double zh = 0.0;          // Head z
        double fric = 0.0;
        double lenl = 0.0;        // Length in local l
        double lenm = 0.0;        // Length in local m
        double height = 0.0;      // Height
    };

    RigidwallGeometricPrism() = default;

    std::string getKeywordName() const override { return "*RIGIDWALL_GEOMETRIC_PRISM"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    Data& getData() { return data_; }
    const Data& getData() const { return data_; }

private:
    Data data_;
};

/**
 * @brief *RIGIDWALL_GEOMETRIC_TORUS
 *
 * Defines a torus-shaped geometric rigid wall.
 */
class KOO_API RigidwallGeometricTorus : public CloneableKeyword<RigidwallGeometricTorus, RigidwallKeyword> {
public:
    struct Data {
        int id = 0;
        int nsid = 0;
        int nsidex = 0;
        int boxid = 0;
        double birth = 0.0;
        double death = 1e20;
        double xc = 0.0;          // Center x
        double yc = 0.0;          // Center y
        double zc = 0.0;          // Center z
        double rMajor = 0.0;      // Major radius
        double rMinor = 0.0;      // Minor radius
        double fric = 0.0;
    };

    RigidwallGeometricTorus() = default;

    std::string getKeywordName() const override { return "*RIGIDWALL_GEOMETRIC_TORUS"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    Data& getData() { return data_; }
    const Data& getData() const { return data_; }

private:
    Data data_;
};

/**
 * @brief *RIGIDWALL_PLANAR_ID
 *
 * Defines a planar rigid wall with ID.
 */
class KOO_API RigidwallPlanarId : public CloneableKeyword<RigidwallPlanarId, RigidwallKeyword> {
public:
    struct Data {
        int id = 0;
        std::string title;
        int nsid = 0;
        int nsidex = 0;
        int boxid = 0;
        double birth = 0.0;
        double death = 1e20;
        double xt = 0.0;
        double yt = 0.0;
        double zt = 0.0;
        double xh = 0.0;
        double yh = 0.0;
        double zh = 0.0;
        double fric = 0.0;
        double wvel = 0.0;
    };

    RigidwallPlanarId() = default;

    std::string getKeywordName() const override { return "*RIGIDWALL_PLANAR_ID"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    Data& getData() { return data_; }
    const Data& getData() const { return data_; }

private:
    Data data_;
};

/**
 * @brief *RIGIDWALL_PLANAR_MOVING_FORCES
 *
 * Defines a moving planar rigid wall with force output.
 */
class KOO_API RigidwallPlanarMovingForces : public CloneableKeyword<RigidwallPlanarMovingForces, RigidwallKeyword> {
public:
    struct Data {
        int nsid = 0;
        int nsidex = 0;
        int boxid = 0;
        double xt = 0.0;
        double yt = 0.0;
        double zt = 0.0;
        double xh = 0.0;
        double yh = 0.0;
        double zh = 0.0;
        double fric = 0.0;
        double vx = 0.0;
        double vy = 0.0;
        double vz = 0.0;
        int lcid = 0;             // Load curve ID for motion
    };

    RigidwallPlanarMovingForces() = default;

    std::string getKeywordName() const override { return "*RIGIDWALL_PLANAR_MOVING_FORCES"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    Data& getData() { return data_; }
    const Data& getData() const { return data_; }

private:
    Data data_;
};

/**
 * @brief *RIGIDWALL_GEOMETRIC_CURVED
 *
 * Defines a curved geometric rigid wall.
 */
class KOO_API RigidwallGeometricCurved : public CloneableKeyword<RigidwallGeometricCurved, RigidwallKeyword> {
public:
    struct Data {
        int id = 0;
        int nsid = 0;
        int nsidex = 0;
        int boxid = 0;
        double birth = 0.0;
        double death = 1e20;
        int curveid = 0;          // Curve ID
        double fric = 0.0;
    };

    RigidwallGeometricCurved() = default;

    std::string getKeywordName() const override { return "*RIGIDWALL_GEOMETRIC_CURVED"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    Data& getData() { return data_; }
    const Data& getData() const { return data_; }

private:
    Data data_;
};

/**
 * @brief *RIGIDWALL_PLANAR_FINITE_ID
 *
 * Defines a finite planar rigid wall with ID.
 */
class KOO_API RigidwallPlanarFiniteId : public CloneableKeyword<RigidwallPlanarFiniteId, RigidwallKeyword> {
public:
    struct Data {
        int id = 0;
        std::string title;
        int nsid = 0;
        int nsidex = 0;
        int boxid = 0;
        double birth = 0.0;
        double death = 1e20;
        double xt = 0.0;
        double yt = 0.0;
        double zt = 0.0;
        double xh = 0.0;
        double yh = 0.0;
        double zh = 0.0;
        double fric = 0.0;
        double lenl = 0.0;
        double lenm = 0.0;
    };

    RigidwallPlanarFiniteId() = default;

    std::string getKeywordName() const override { return "*RIGIDWALL_PLANAR_FINITE_ID"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    Data& getData() { return data_; }
    const Data& getData() const { return data_; }

private:
    Data data_;
};

/**
 * @brief *RIGIDWALL_GEOMETRIC_MOTION
 *
 * Defines motion for geometric rigid walls.
 */
class KOO_API RigidwallGeometricMotion : public CloneableKeyword<RigidwallGeometricMotion, RigidwallKeyword> {
public:
    struct Data {
        int id = 0;               // Rigid wall ID
        int lcidvx = 0;           // Load curve for vx
        int lcidvy = 0;           // Load curve for vy
        int lcidvz = 0;           // Load curve for vz
        int lcidax = 0;           // Load curve for angular vx
        int lciday = 0;           // Load curve for angular vy
        int lcidaz = 0;           // Load curve for angular vz
        double sfvx = 1.0;        // Scale factor for vx
        double sfvy = 1.0;        // Scale factor for vy
        double sfvz = 1.0;        // Scale factor for vz
    };

    RigidwallGeometricMotion() = default;

    std::string getKeywordName() const override { return "*RIGIDWALL_GEOMETRIC_MOTION"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    Data& getData() { return data_; }
    const Data& getData() const { return data_; }

private:
    Data data_;
};

/**
 * @brief *RIGIDWALL_PLANAR_MOVING_FINITE
 *
 * Defines a moving finite planar rigid wall.
 */
class KOO_API RigidwallPlanarMovingFinite : public CloneableKeyword<RigidwallPlanarMovingFinite, RigidwallKeyword> {
public:
    struct Data {
        int nsid = 0;
        int nsidex = 0;
        int boxid = 0;
        double xt = 0.0;
        double yt = 0.0;
        double zt = 0.0;
        double xh = 0.0;
        double yh = 0.0;
        double zh = 0.0;
        double fric = 0.0;
        double lenl = 0.0;
        double lenm = 0.0;
        double vx = 0.0;
        double vy = 0.0;
        double vz = 0.0;
    };

    RigidwallPlanarMovingFinite() = default;

    std::string getKeywordName() const override { return "*RIGIDWALL_PLANAR_MOVING_FINITE"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    Data& getData() { return data_; }
    const Data& getData() const { return data_; }

private:
    Data data_;
};

} // namespace koo::dyna
