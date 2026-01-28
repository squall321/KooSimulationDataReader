#pragma once

#include <koo/Export.hpp>
#include <koo/dyna/Keyword.hpp>
#include <koo/util/Types.hpp>
#include <vector>
#include <string>

namespace koo::dyna {

/**
 * @brief Base class for *SENSOR keywords
 */
class KOO_API SensorKeyword : public Keyword {
public:
    virtual ~SensorKeyword() = default;
};

/**
 * @brief *SENSOR_DEFINE_NODE
 *
 * Defines a sensor at a node.
 */
class KOO_API SensorDefineNode : public CloneableKeyword<SensorDefineNode, SensorKeyword> {
public:
    struct Data {
        int sensid = 0;       // Sensor ID
        int nid = 0;          // Node ID
        int ctype = 0;        // Component type (1=disp, 2=vel, 3=acc)
        int dir = 0;          // Direction (1=X, 2=Y, 3=Z)
    };

    SensorDefineNode() = default;

    std::string getKeywordName() const override { return "*SENSOR_DEFINE_NODE"; }

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
 * @brief *SENSOR_DEFINE_ELEMENT
 *
 * Defines a sensor at an element.
 */
class KOO_API SensorDefineElement : public CloneableKeyword<SensorDefineElement, SensorKeyword> {
public:
    struct Data {
        int sensid = 0;       // Sensor ID
        int eid = 0;          // Element ID
        int ctype = 0;        // Component type
        int comp = 0;         // Stress/strain component
    };

    SensorDefineElement() = default;

    std::string getKeywordName() const override { return "*SENSOR_DEFINE_ELEMENT"; }

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
 * @brief *SENSOR_SWITCH
 *
 * Defines a sensor switch for triggering actions.
 */
class KOO_API SensorSwitch : public CloneableKeyword<SensorSwitch, SensorKeyword> {
public:
    struct Data {
        int swid = 0;         // Switch ID
        int sensid = 0;       // Sensor ID
        int typeid_ = 0;      // Type (1=>, 2=<, 3=>=, 4=<=)
        double value = 0.0;   // Threshold value
        int output = 0;       // Output flag
    };

    SensorSwitch() = default;

    std::string getKeywordName() const override { return "*SENSOR_SWITCH"; }

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
 * @brief *SENSOR_CONTROL
 *
 * Sensor control parameters.
 */
class KOO_API SensorControl : public CloneableKeyword<SensorControl, SensorKeyword> {
public:
    struct Data {
        int cntlid = 0;       // Control ID
        int swid = 0;         // Switch ID
        int birth = 0;        // Birth time flag
        double birthtime = 0.0; // Birth time
        int death = 0;        // Death time flag
        double deathtime = 0.0; // Death time
    };

    SensorControl() = default;

    std::string getKeywordName() const override { return "*SENSOR_CONTROL"; }

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
 * @brief *SENSOR_DEFINE_FORCE
 *
 * Defines a sensor to measure force.
 */
class KOO_API SensorDefineForce : public CloneableKeyword<SensorDefineForce, SensorKeyword> {
public:
    struct Data {
        int sensid = 0;       // Sensor ID
        int ftype = 0;        // Force type (0=resultant, 1=x, 2=y, 3=z)
        int id1 = 0;          // First ID (contact/joint)
        int id2 = 0;          // Second ID
        int cid = 0;          // Coordinate system ID
    };

    SensorDefineForce() = default;

    std::string getKeywordName() const override { return "*SENSOR_DEFINE_FORCE"; }

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
 * @brief *SENSOR_DEFINE_MISC
 *
 * Defines a sensor to measure miscellaneous quantities.
 */
class KOO_API SensorDefineMisc : public CloneableKeyword<SensorDefineMisc, SensorKeyword> {
public:
    struct Data {
        int sensid = 0;       // Sensor ID
        int mtype = 0;        // Measurement type
        int id = 0;           // Associated ID
        int vid = 0;          // Variable ID
    };

    SensorDefineMisc() = default;

    std::string getKeywordName() const override { return "*SENSOR_DEFINE_MISC"; }

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
 * @brief *SENSOR_DEFINE_CALC_MATH
 *
 * Defines a sensor with mathematical calculation.
 */
class KOO_API SensorDefineCalcMath : public CloneableKeyword<SensorDefineCalcMath, SensorKeyword> {
public:
    struct Data {
        int sensid = 0;       // Sensor ID
        int mathop = 0;       // Math operation type
        int sens1 = 0;        // First sensor ID
        int sens2 = 0;        // Second sensor ID
        double const1 = 0.0;  // Constant 1
        double const2 = 0.0;  // Constant 2
    };

    SensorDefineCalcMath() = default;

    std::string getKeywordName() const override { return "*SENSOR_DEFINE_CALC_MATH"; }

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
 * @brief *SENSOR_SWITCH_CALC_LOGIC
 *
 * Defines logical operations between switches.
 */
class KOO_API SensorSwitchCalcLogic : public CloneableKeyword<SensorSwitchCalcLogic, SensorKeyword> {
public:
    struct Data {
        int swid = 0;         // Switch ID
        int logicop = 0;      // Logic operation (0=AND, 1=OR, 2=XOR, 3=NOT)
        int sw1 = 0;          // First switch ID
        int sw2 = 0;          // Second switch ID
    };

    SensorSwitchCalcLogic() = default;

    std::string getKeywordName() const override { return "*SENSOR_SWITCH_CALC_LOGIC"; }

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
 * @brief *SENSOR_DEFINE_FUNCTION
 *
 * Defines a sensor using a function.
 */
class KOO_API SensorDefineFunction : public CloneableKeyword<SensorDefineFunction, SensorKeyword> {
public:
    struct Data {
        int sensid = 0;       // Sensor ID
        int funcid = 0;       // Function ID
        int arg1 = 0;         // Argument 1
        int arg2 = 0;         // Argument 2
        int arg3 = 0;         // Argument 3
        int arg4 = 0;         // Argument 4
    };

    SensorDefineFunction() = default;

    std::string getKeywordName() const override { return "*SENSOR_DEFINE_FUNCTION"; }

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
 * @brief *SENSOR_DEFINE_COORDINATE
 *
 * Defines a sensor to measure coordinate position.
 */
class KOO_API SensorDefineCoordinate : public CloneableKeyword<SensorDefineCoordinate, SensorKeyword> {
public:
    struct Data {
        int sensid = 0;       // Sensor ID
        int nid = 0;          // Node ID
        int axis = 0;         // Coordinate axis (1=X, 2=Y, 3=Z)
        int cid = 0;          // Coordinate system ID
    };

    SensorDefineCoordinate() = default;

    std::string getKeywordName() const override { return "*SENSOR_DEFINE_COORDINATE"; }

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
 * @brief *SENSOR_SWITCH_SHELL_TO_SOLID
 *
 * Sensor switch for shell-to-solid conversion.
 */
class KOO_API SensorSwitchShellToSolid : public CloneableKeyword<SensorSwitchShellToSolid, SensorKeyword> {
public:
    struct Data {
        int swid = 0;         // Switch ID
        int pid = 0;          // Part ID
        int method = 0;       // Conversion method
        double thick = 0.0;   // Target thickness
    };

    SensorSwitchShellToSolid() = default;

    std::string getKeywordName() const override { return "*SENSOR_SWITCH_SHELL_TO_SOLID"; }

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
