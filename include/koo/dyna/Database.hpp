#pragma once

#include <koo/Export.hpp>
#include <koo/dyna/Keyword.hpp>
#include <koo/util/Types.hpp>

namespace koo::dyna {

/**
 * @brief Base class for *DATABASE keywords
 */
class KOO_API DatabaseKeyword : public Keyword {
public:
    virtual ~DatabaseKeyword() = default;
};

/**
 * @brief *DATABASE_BINARY_D3PLOT
 *
 * Controls d3plot binary output frequency.
 */
class KOO_API DatabaseBinaryD3plot : public CloneableKeyword<DatabaseBinaryD3plot, DatabaseKeyword> {
public:
    struct Data {
        double dt = 0.0;        // Time interval between outputs
        int lcdt = 0;           // Load curve ID for time interval
        int beam = 0;           // Beam stress/strain output
        int npltc = 0;          // Number of complete states to skip
        int psetid = 0;         // Part set ID for output subset
    };

    DatabaseBinaryD3plot() = default;

    std::string getKeywordName() const override { return "*DATABASE_BINARY_D3PLOT"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    // Data access
    Data& getData() { return data_; }
    const Data& getData() const { return data_; }

    // Convenience
    double getOutputInterval() const { return data_.dt; }
    void setOutputInterval(double dt) { data_.dt = dt; }

private:
    Data data_;
};

/**
 * @brief *DATABASE_BINARY_D3THDT
 *
 * Controls d3thdt binary output frequency.
 */
class KOO_API DatabaseBinaryD3thdt : public CloneableKeyword<DatabaseBinaryD3thdt, DatabaseKeyword> {
public:
    struct Data {
        double dt = 0.0;        // Time interval between outputs
        int lcdt = 0;           // Load curve ID
    };

    DatabaseBinaryD3thdt() = default;

    std::string getKeywordName() const override { return "*DATABASE_BINARY_D3THDT"; }

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
 * @brief *DATABASE_EXTENT_BINARY
 *
 * Controls extent of binary output.
 */
class KOO_API DatabaseExtentBinary : public CloneableKeyword<DatabaseExtentBinary, DatabaseKeyword> {
public:
    struct Data {
        int neiph = 0;          // Additional history variables
        int neips = 0;          // Additional history variables for shells
        int maxint = 3;         // Max number of shell integration points
        int strflg = 0;         // Stress output flag
        int sigflg = 1;         // Stress tensor flag
        int epsflg = 1;         // Strain tensor flag
        int rltflg = 1;         // Resultant flag
        int engflg = 1;         // Energy flag
        int cmpflg = 0;         // Composite failure flag
        int ieverp = 0;         // Every output flag
        int beamip = 0;         // Beam integration points
        int dcomp = 1;          // Database component type
        int shge = 1;           // Shell hourglass energy
        int stssz = 0;          // Stress output size
        int n3thdt = 2;         // Number of binary output files
        int ialemat = 1;        // ALE multi-material group output
        int nintsld = 0;        // Integration points solid output
        int pkp_sen = 0;        // Peak sensor output
    };

    DatabaseExtentBinary() = default;

    std::string getKeywordName() const override { return "*DATABASE_EXTENT_BINARY"; }

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
 * @brief *DATABASE_GLSTAT
 *
 * Global statistics output.
 */
class KOO_API DatabaseGlstat : public CloneableKeyword<DatabaseGlstat, DatabaseKeyword> {
public:
    struct Data {
        double dt = 0.0;        // Time interval
        int binary = 0;         // Binary file flag
        int lcdt = 0;           // Load curve ID
    };

    DatabaseGlstat() = default;

    std::string getKeywordName() const override { return "*DATABASE_GLSTAT"; }

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
 * @brief *DATABASE_MATSUM
 *
 * Material energies output.
 */
class KOO_API DatabaseMatsum : public CloneableKeyword<DatabaseMatsum, DatabaseKeyword> {
public:
    struct Data {
        double dt = 0.0;
        int binary = 0;
        int lcdt = 0;
    };

    DatabaseMatsum() = default;

    std::string getKeywordName() const override { return "*DATABASE_MATSUM"; }

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
 * @brief *DATABASE_NODOUT
 *
 * Nodal output.
 */
class KOO_API DatabaseNodout : public CloneableKeyword<DatabaseNodout, DatabaseKeyword> {
public:
    struct Data {
        double dt = 0.0;
        int binary = 0;
        int lcdt = 0;
        int option = 0;
    };

    DatabaseNodout() = default;

    std::string getKeywordName() const override { return "*DATABASE_NODOUT"; }

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
 * @brief *DATABASE_ELOUT
 *
 * Element output.
 */
class KOO_API DatabaseElout : public CloneableKeyword<DatabaseElout, DatabaseKeyword> {
public:
    struct Data {
        double dt = 0.0;
        int binary = 0;
        int lcdt = 0;
        int option = 0;
    };

    DatabaseElout() = default;

    std::string getKeywordName() const override { return "*DATABASE_ELOUT"; }

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
 * @brief *DATABASE_RCFORC
 *
 * Resultant contact forces output.
 */
class KOO_API DatabaseRcforc : public CloneableKeyword<DatabaseRcforc, DatabaseKeyword> {
public:
    struct Data {
        double dt = 0.0;
        int binary = 0;
        int lcdt = 0;
    };

    DatabaseRcforc() = default;

    std::string getKeywordName() const override { return "*DATABASE_RCFORC"; }

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
 * @brief *DATABASE_SLEOUT
 *
 * Sliding interface energy output.
 */
class KOO_API DatabaseSleout : public CloneableKeyword<DatabaseSleout, DatabaseKeyword> {
public:
    struct Data {
        double dt = 0.0;
        int binary = 0;
        int lcdt = 0;
    };

    DatabaseSleout() = default;

    std::string getKeywordName() const override { return "*DATABASE_SLEOUT"; }

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
 * @brief *DATABASE_HISTORY_NODE
 *
 * History output for specific nodes.
 */
class KOO_API DatabaseHistoryNode : public CloneableKeyword<DatabaseHistoryNode, DatabaseKeyword> {
public:
    DatabaseHistoryNode() = default;

    std::string getKeywordName() const override { return "*DATABASE_HISTORY_NODE"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    // Node IDs to track
    std::vector<NodeId>& getNodeIds() { return nodeIds_; }
    const std::vector<NodeId>& getNodeIds() const { return nodeIds_; }

    void addNodeId(NodeId id) { nodeIds_.push_back(id); }
    void clearNodeIds() { nodeIds_.clear(); }

private:
    std::vector<NodeId> nodeIds_;
};

/**
 * @brief *DATABASE_HISTORY_SHELL
 *
 * History output for specific shell elements.
 */
class KOO_API DatabaseHistoryShell : public CloneableKeyword<DatabaseHistoryShell, DatabaseKeyword> {
public:
    DatabaseHistoryShell() = default;

    std::string getKeywordName() const override { return "*DATABASE_HISTORY_SHELL"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    std::vector<ElementId>& getElementIds() { return elementIds_; }
    const std::vector<ElementId>& getElementIds() const { return elementIds_; }

    void addElementId(ElementId id) { elementIds_.push_back(id); }
    void clearElementIds() { elementIds_.clear(); }

private:
    std::vector<ElementId> elementIds_;
};

/**
 * @brief *DATABASE_HISTORY_SOLID
 *
 * History output for specific solid elements.
 */
class KOO_API DatabaseHistorySolid : public CloneableKeyword<DatabaseHistorySolid, DatabaseKeyword> {
public:
    DatabaseHistorySolid() = default;

    std::string getKeywordName() const override { return "*DATABASE_HISTORY_SOLID"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    std::vector<ElementId>& getElementIds() { return elementIds_; }
    const std::vector<ElementId>& getElementIds() const { return elementIds_; }

    void addElementId(ElementId id) { elementIds_.push_back(id); }
    void clearElementIds() { elementIds_.clear(); }

private:
    std::vector<ElementId> elementIds_;
};

/**
 * @brief *DATABASE_SPCFORC
 *
 * SPC (boundary constraint) forces output.
 */
class KOO_API DatabaseSpcforc : public CloneableKeyword<DatabaseSpcforc, DatabaseKeyword> {
public:
    struct Data {
        double dt = 0.0;
        int binary = 0;
        int lcdt = 0;
    };

    DatabaseSpcforc() = default;

    std::string getKeywordName() const override { return "*DATABASE_SPCFORC"; }

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
 * @brief *DATABASE_RWFORC
 *
 * Rigid wall force output.
 */
class KOO_API DatabaseRwforc : public CloneableKeyword<DatabaseRwforc, DatabaseKeyword> {
public:
    struct Data {
        double dt = 0.0;
        int binary = 0;
        int lcdt = 0;
    };

    DatabaseRwforc() = default;

    std::string getKeywordName() const override { return "*DATABASE_RWFORC"; }

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
 * @brief *DATABASE_ABSTAT
 *
 * Airbag statistics output.
 */
class KOO_API DatabaseAbstat : public CloneableKeyword<DatabaseAbstat, DatabaseKeyword> {
public:
    struct Data {
        double dt = 0.0;
        int binary = 0;
        int lcdt = 0;
    };

    DatabaseAbstat() = default;

    std::string getKeywordName() const override { return "*DATABASE_ABSTAT"; }

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
 * @brief *DATABASE_SECFORC
 *
 * Cross-section forces output.
 */
class KOO_API DatabaseSecforc : public CloneableKeyword<DatabaseSecforc, DatabaseKeyword> {
public:
    struct Data {
        double dt = 0.0;
        int binary = 0;
        int lcdt = 0;
    };

    DatabaseSecforc() = default;

    std::string getKeywordName() const override { return "*DATABASE_SECFORC"; }

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
 * @brief *DATABASE_JNTFORC
 *
 * Joint forces output.
 */
class KOO_API DatabaseJntforc : public CloneableKeyword<DatabaseJntforc, DatabaseKeyword> {
public:
    struct Data {
        double dt = 0.0;
        int binary = 0;
        int lcdt = 0;
    };

    DatabaseJntforc() = default;

    std::string getKeywordName() const override { return "*DATABASE_JNTFORC"; }

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
 * @brief *DATABASE_BNDOUT
 *
 * Boundary output.
 */
class KOO_API DatabaseBndout : public CloneableKeyword<DatabaseBndout, DatabaseKeyword> {
public:
    struct Data {
        double dt = 0.0;
        int binary = 0;
        int lcdt = 0;
    };

    DatabaseBndout() = default;

    std::string getKeywordName() const override { return "*DATABASE_BNDOUT"; }

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
 * @brief *DATABASE_DEFORC
 *
 * Discrete element forces output.
 */
class KOO_API DatabaseDeforc : public CloneableKeyword<DatabaseDeforc, DatabaseKeyword> {
public:
    struct Data {
        double dt = 0.0;
        int binary = 0;
        int lcdt = 0;
    };

    DatabaseDeforc() = default;

    std::string getKeywordName() const override { return "*DATABASE_DEFORC"; }

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
 * @brief *DATABASE_SWFORC
 *
 * Spot weld forces output.
 */
class KOO_API DatabaseSwforc : public CloneableKeyword<DatabaseSwforc, DatabaseKeyword> {
public:
    struct Data {
        double dt = 0.0;
        int binary = 0;
        int lcdt = 0;
    };

    DatabaseSwforc() = default;

    std::string getKeywordName() const override { return "*DATABASE_SWFORC"; }

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
 * @brief *DATABASE_NCFORC
 *
 * Nodal interface forces output.
 */
class KOO_API DatabaseNcforc : public CloneableKeyword<DatabaseNcforc, DatabaseKeyword> {
public:
    struct Data {
        double dt = 0.0;
        int binary = 0;
        int lcdt = 0;
    };

    DatabaseNcforc() = default;

    std::string getKeywordName() const override { return "*DATABASE_NCFORC"; }

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
 * @brief *DATABASE_HISTORY_BEAM
 *
 * History output for specific beam elements.
 */
class KOO_API DatabaseHistoryBeam : public CloneableKeyword<DatabaseHistoryBeam, DatabaseKeyword> {
public:
    DatabaseHistoryBeam() = default;

    std::string getKeywordName() const override { return "*DATABASE_HISTORY_BEAM"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    std::vector<ElementId>& getElementIds() { return elementIds_; }
    const std::vector<ElementId>& getElementIds() const { return elementIds_; }

    void addElementId(ElementId id) { elementIds_.push_back(id); }
    void clearElementIds() { elementIds_.clear(); }

private:
    std::vector<ElementId> elementIds_;
};

/**
 * @brief *DATABASE_BINARY_D3DUMP
 *
 * Controls d3dump restart file output.
 */
class KOO_API DatabaseBinaryD3dump : public CloneableKeyword<DatabaseBinaryD3dump, DatabaseKeyword> {
public:
    struct Data {
        double dt = 0.0;
        int lcdt = 0;
        int nrst = 0;
    };

    DatabaseBinaryD3dump() = default;

    std::string getKeywordName() const override { return "*DATABASE_BINARY_D3DUMP"; }

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
 * @brief *DATABASE_BINARY_RUNRSF
 *
 * Controls running restart file output.
 */
class KOO_API DatabaseBinaryRunrsf : public CloneableKeyword<DatabaseBinaryRunrsf, DatabaseKeyword> {
public:
    struct Data {
        double dt = 0.0;
        int lcdt = 0;
        int nrst = 0;
    };

    DatabaseBinaryRunrsf() = default;

    std::string getKeywordName() const override { return "*DATABASE_BINARY_RUNRSF"; }

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
 * @brief *DATABASE_SSSTAT
 *
 * Subsystem statistics output.
 */
class KOO_API DatabaseSsstat : public CloneableKeyword<DatabaseSsstat, DatabaseKeyword> {
public:
    struct Data {
        double dt = 0.0;
        int binary = 0;
        int lcdt = 0;
    };

    DatabaseSsstat() = default;

    std::string getKeywordName() const override { return "*DATABASE_SSSTAT"; }

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
 * @brief *DATABASE_RBDOUT
 *
 * Rigid body output.
 */
class KOO_API DatabaseRbdout : public CloneableKeyword<DatabaseRbdout, DatabaseKeyword> {
public:
    struct Data {
        double dt = 0.0;
        int binary = 0;
        int lcdt = 0;
    };

    DatabaseRbdout() = default;

    std::string getKeywordName() const override { return "*DATABASE_RBDOUT"; }

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
 * @brief *DATABASE_CURVOUT
 *
 * Curve output.
 */
class KOO_API DatabaseCurvout : public CloneableKeyword<DatabaseCurvout, DatabaseKeyword> {
public:
    struct Data {
        double dt = 0.0;
        int binary = 0;
        int lcdt = 0;
    };

    DatabaseCurvout() = default;

    std::string getKeywordName() const override { return "*DATABASE_CURVOUT"; }

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
 * @brief *DATABASE_TPRINT
 *
 * Thermal print output.
 */
class KOO_API DatabaseTprint : public CloneableKeyword<DatabaseTprint, DatabaseKeyword> {
public:
    struct Data {
        double dt = 0.0;
        int binary = 0;
        int lcdt = 0;
    };

    DatabaseTprint() = default;

    std::string getKeywordName() const override { return "*DATABASE_TPRINT"; }

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
 * @brief *DATABASE_NODFOR
 *
 * Nodal force groups output.
 */
class KOO_API DatabaseNodfor : public CloneableKeyword<DatabaseNodfor, DatabaseKeyword> {
public:
    struct Data {
        double dt = 0.0;
        int binary = 0;
        int lcdt = 0;
    };

    DatabaseNodfor() = default;

    std::string getKeywordName() const override { return "*DATABASE_NODFOR"; }

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
 * @brief *DATABASE_DCFAIL
 *
 * Discrete element and contact failure output.
 */
class KOO_API DatabaseDcfail : public CloneableKeyword<DatabaseDcfail, DatabaseKeyword> {
public:
    struct Data {
        double dt = 0.0;
        int binary = 0;
        int lcdt = 0;
    };

    DatabaseDcfail() = default;

    std::string getKeywordName() const override { return "*DATABASE_DCFAIL"; }

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
 * @brief *DATABASE_BEARING
 *
 * Bearing force output.
 */
class KOO_API DatabaseBearing : public CloneableKeyword<DatabaseBearing, DatabaseKeyword> {
public:
    struct Data {
        double dt = 0.0;
        int binary = 0;
        int lcdt = 0;
    };

    DatabaseBearing() = default;

    std::string getKeywordName() const override { return "*DATABASE_BEARING"; }

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
 * @brief *DATABASE_CROSS_SECTION_SET
 *
 * Defines a cross-section plane for force output by node set.
 */
class KOO_API DatabaseCrossSectionSet : public CloneableKeyword<DatabaseCrossSectionSet, DatabaseKeyword> {
public:
    struct Data {
        int csid = 0;           // Cross-section ID
        std::string title;      // Title (optional)
        int nsid = 0;           // Node set ID
        int hsid = 0;           // Element set ID for history
        int bsid = 0;           // Beam set ID
        int ssid = 0;           // Shell set ID
        int tsid = 0;           // Thick shell set ID
        int dsid = 0;           // Solid set ID
        int id = 0;             // ID type flag
        int itype = 0;          // Output type
    };

    DatabaseCrossSectionSet() = default;

    std::string getKeywordName() const override { return "*DATABASE_CROSS_SECTION_SET"; }

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
 * @brief *DATABASE_CROSS_SECTION_PLANE
 *
 * Defines a cross-section plane for force output by geometry.
 */
class KOO_API DatabaseCrossSectionPlane : public CloneableKeyword<DatabaseCrossSectionPlane, DatabaseKeyword> {
public:
    struct Data {
        int csid = 0;           // Cross-section ID
        std::string title;      // Title (optional)
        int psid = 0;           // Part set ID
        double xct = 0.0;       // X coord of center (tail)
        double yct = 0.0;       // Y coord of center (tail)
        double zct = 0.0;       // Z coord of center (tail)
        double xch = 0.0;       // X coord of center (head)
        double ych = 0.0;       // Y coord of center (head)
        double zch = 0.0;       // Z coord of center (head)
        double radius = 0.0;    // Radius of cross-section
        double xhev = 0.0;      // X component of edge vector
        double yhev = 0.0;      // Y component of edge vector
        double zhev = 0.0;      // Z component of edge vector
        double lenl = 0.0;      // Length of local 1 axis
        double lenm = 0.0;      // Length of local 2 axis
        int id = 0;             // ID type flag
        int itype = 0;          // Output type
    };

    DatabaseCrossSectionPlane() = default;

    std::string getKeywordName() const override { return "*DATABASE_CROSS_SECTION_PLANE"; }

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
 * @brief *DATABASE_FORMAT
 *
 * Controls the format of ASCII output files.
 */
class KOO_API DatabaseFormat : public CloneableKeyword<DatabaseFormat, DatabaseKeyword> {
public:
    struct Data {
        int iform = 0;          // Output format flag (0=default, 1=LS-PrePost)
        int ibinary = 0;        // Binary output flag
    };

    DatabaseFormat() = default;

    std::string getKeywordName() const override { return "*DATABASE_FORMAT"; }

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
 * @brief *DATABASE_HISTORY_TSHELL
 *
 * Controls output of thick shell element history variables.
 */
class KOO_API DatabaseHistoryTshell : public CloneableKeyword<DatabaseHistoryTshell, DatabaseKeyword> {
public:
    struct Data {
        std::vector<int64_t> elemIds;
    };

    DatabaseHistoryTshell() = default;

    std::string getKeywordName() const override { return "*DATABASE_HISTORY_TSHELL"; }

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
 * @brief *DATABASE_SBTOUT
 *
 * Seatbelt output database.
 */
class KOO_API DatabaseSbtout : public CloneableKeyword<DatabaseSbtout, DatabaseKeyword> {
public:
    struct Data {
        double dt = 0.0;
        int binary = 0;
        int lcur = 0;
        int ioopt = 0;
    };

    DatabaseSbtout() = default;

    std::string getKeywordName() const override { return "*DATABASE_SBTOUT"; }

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
 * @brief *DATABASE_ATDOUT
 *
 * ATD (Anthropomorphic Test Device) output database.
 */
class KOO_API DatabaseAtdout : public CloneableKeyword<DatabaseAtdout, DatabaseKeyword> {
public:
    struct Data {
        double dt = 0.0;
        int binary = 0;
        int lcur = 0;
        int ioopt = 0;
    };

    DatabaseAtdout() = default;

    std::string getKeywordName() const override { return "*DATABASE_ATDOUT"; }

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
 * @brief *DATABASE_DISBOUT
 *
 * Disbond output database.
 */
class KOO_API DatabaseDisbout : public CloneableKeyword<DatabaseDisbout, DatabaseKeyword> {
public:
    struct Data {
        double dt = 0.0;
        int binary = 0;
        int lcur = 0;
        int ioopt = 0;
    };

    DatabaseDisbout() = default;

    std::string getKeywordName() const override { return "*DATABASE_DISBOUT"; }

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
 * @brief *DATABASE_DEFGEO
 *
 * Deformed geometry output database.
 */
class KOO_API DatabaseDefgeo : public CloneableKeyword<DatabaseDefgeo, DatabaseKeyword> {
public:
    struct Data {
        double dt = 0.0;
        int binary = 0;
        int lcur = 0;
        int ioopt = 0;
    };

    DatabaseDefgeo() = default;

    std::string getKeywordName() const override { return "*DATABASE_DEFGEO"; }

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
 * @brief *DATABASE_PRTUBE
 *
 * Pressure tube output database.
 */
class KOO_API DatabasePrtube : public CloneableKeyword<DatabasePrtube, DatabaseKeyword> {
public:
    struct Data {
        double dt = 0.0;
        int binary = 0;
        int lcur = 0;
        int ioopt = 0;
    };

    DatabasePrtube() = default;

    std::string getKeywordName() const override { return "*DATABASE_PRTUBE"; }

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
 * @brief *DATABASE_CPMFOR
 *
 * Corpuscular particle method force output.
 */
class KOO_API DatabaseCpmfor : public CloneableKeyword<DatabaseCpmfor, DatabaseKeyword> {
public:
    struct Data {
        double dt = 0.0;
        int binary = 0;
        int lcur = 0;
        int ioopt = 0;
    };

    DatabaseCpmfor() = default;

    std::string getKeywordName() const override { return "*DATABASE_CPMFOR"; }

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
 * @brief *DATABASE_PLLYOUT
 *
 * Pulley output database.
 */
class KOO_API DatabasePllyout : public CloneableKeyword<DatabasePllyout, DatabaseKeyword> {
public:
    struct Data {
        double dt = 0.0;
        int binary = 0;
        int lcur = 0;
        int ioopt = 0;
    };

    DatabasePllyout() = default;

    std::string getKeywordName() const override { return "*DATABASE_PLLYOUT"; }

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
 * @brief *DATABASE_DEMRCF
 *
 * DEM (Discrete Element Method) resultant contact force output.
 */
class KOO_API DatabaseDemrcf : public CloneableKeyword<DatabaseDemrcf, DatabaseKeyword> {
public:
    struct Data {
        double dt = 0.0;
        int binary = 0;
        int lcur = 0;
        int ioopt = 0;
    };

    DatabaseDemrcf() = default;

    std::string getKeywordName() const override { return "*DATABASE_DEMRCF"; }

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
 * @brief *DATABASE_MOVIE
 *
 * Movie output database configuration.
 */
class KOO_API DatabaseMovie : public CloneableKeyword<DatabaseMovie, DatabaseKeyword> {
public:
    struct Data {
        double dt = 0.0;
        int binary = 0;
        int lcur = 0;
        int ioopt = 0;
    };

    DatabaseMovie() = default;

    std::string getKeywordName() const override { return "*DATABASE_MOVIE"; }

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
 * @brief *DATABASE_FSI
 *
 * Fluid-Structure Interaction output database.
 */
class KOO_API DatabaseFsi : public CloneableKeyword<DatabaseFsi, DatabaseKeyword> {
public:
    struct Data {
        double dt = 0.0;
        int binary = 0;
        int lcur = 0;
        int ioopt = 0;
    };

    DatabaseFsi() = default;

    std::string getKeywordName() const override { return "*DATABASE_FSI"; }

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
 * @brief *DATABASE_MASSOUT
 *
 * Mass output database.
 */
class KOO_API DatabaseMassout : public CloneableKeyword<DatabaseMassout, DatabaseKeyword> {
public:
    struct Data {
        double dt = 0.0;
        int binary = 0;
        int lcur = 0;
        int ioopt = 0;
    };

    DatabaseMassout() = default;

    std::string getKeywordName() const override { return "*DATABASE_MASSOUT"; }

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
 * @brief *DATABASE_TRACER
 *
 * Tracer particle output database.
 */
class KOO_API DatabaseTracer : public CloneableKeyword<DatabaseTracer, DatabaseKeyword> {
public:
    struct Data {
        double dt = 0.0;
        int nid = 0;
        int track = 0;
        int ammgid = 0;
    };

    DatabaseTracer() = default;

    std::string getKeywordName() const override { return "*DATABASE_TRACER"; }

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
 * @brief *DATABASE_PWP_OUTPUT
 *
 * PWP (Parts with Piece-Wise) output database.
 */
class KOO_API DatabasePwpOutput : public CloneableKeyword<DatabasePwpOutput, DatabaseKeyword> {
public:
    struct Data {
        double dt = 0.0;
        int binary = 0;
        int lcur = 0;
        int ioopt = 0;
    };

    DatabasePwpOutput() = default;

    std::string getKeywordName() const override { return "*DATABASE_PWP_OUTPUT"; }

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
 * @brief *DATABASE_FSI_SENSOR
 *
 * Fluid-structure interaction sensor output database.
 */
class KOO_API DatabaseFsiSensor : public CloneableKeyword<DatabaseFsiSensor, DatabaseKeyword> {
public:
    struct Data {
        double dt = 0.0;
        int binary = 0;
        int lcur = 0;
        int ioopt = 0;
    };

    DatabaseFsiSensor() = default;

    std::string getKeywordName() const override { return "*DATABASE_FSI_SENSOR"; }

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
 * @brief *DATABASE_JNTFORC_LOCAL
 *
 * Local joint force output database.
 */
class KOO_API DatabaseJntforcLocal : public CloneableKeyword<DatabaseJntforcLocal, DatabaseKeyword> {
public:
    struct Data {
        double dt = 0.0;
        int binary = 0;
        int lcur = 0;
        int ioopt = 0;
    };

    DatabaseJntforcLocal() = default;

    std::string getKeywordName() const override { return "*DATABASE_JNTFORC_LOCAL"; }

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
 * @brief *DATABASE_BNDOUT_VENT
 *
 * Boundary vent output database for airbag.
 */
class KOO_API DatabaseBndoutVent : public CloneableKeyword<DatabaseBndoutVent, DatabaseKeyword> {
public:
    struct Data {
        double dt = 0.0;
        int binary = 0;
        int lcur = 0;
        int ioopt = 0;
    };

    DatabaseBndoutVent() = default;

    std::string getKeywordName() const override { return "*DATABASE_BNDOUT_VENT"; }

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
 * @brief *DATABASE_CPM_SENSOR
 *
 * CPM (Corpuscular Particle Method) sensor output database.
 */
class KOO_API DatabaseCpmSensor : public CloneableKeyword<DatabaseCpmSensor, DatabaseKeyword> {
public:
    struct Data {
        double dt = 0.0;
        int binary = 0;
        int lcur = 0;
        int ioopt = 0;
    };

    DatabaseCpmSensor() = default;

    std::string getKeywordName() const override { return "*DATABASE_CPM_SENSOR"; }

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
 * @brief *DATABASE_ALE_MAT
 *
 * ALE material output database.
 */
class KOO_API DatabaseAleMat : public CloneableKeyword<DatabaseAleMat, DatabaseKeyword> {
public:
    struct Data {
        double dt = 0.0;
        int binary = 0;
        int lcur = 0;
        int ioopt = 0;
    };

    DatabaseAleMat() = default;

    std::string getKeywordName() const override { return "*DATABASE_ALE_MAT"; }

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
 * @brief *DATABASE_NCFORC_FILTER
 *
 * Nodal contact force filter output database.
 */
class KOO_API DatabaseNcforcFilter : public CloneableKeyword<DatabaseNcforcFilter, DatabaseKeyword> {
public:
    struct Data {
        double dt = 0.0;
        int binary = 0;
        int lcur = 0;
        int ioopt = 0;
    };

    DatabaseNcforcFilter() = default;

    std::string getKeywordName() const override { return "*DATABASE_NCFORC_FILTER"; }

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
 * @brief *DATABASE_CURVOUT_EXTEND
 *
 * Extended curve output database.
 */
class KOO_API DatabaseCurvoutExtend : public CloneableKeyword<DatabaseCurvoutExtend, DatabaseKeyword> {
public:
    struct Data {
        double dt = 0.0;
        int binary = 0;
        int lcur = 0;
        int ioopt = 0;
    };

    DatabaseCurvoutExtend() = default;

    std::string getKeywordName() const override { return "*DATABASE_CURVOUT_EXTEND"; }

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
 * @brief *DATABASE_SBTOUT_RETRACTOR
 *
 * Seatbelt retractor output database.
 */
class KOO_API DatabaseSbtoutRetractor : public CloneableKeyword<DatabaseSbtoutRetractor, DatabaseKeyword> {
public:
    struct Data {
        double dt = 0.0;
        int binary = 0;
        int lcur = 0;
        int ioopt = 0;
    };

    DatabaseSbtoutRetractor() = default;

    std::string getKeywordName() const override { return "*DATABASE_SBTOUT_RETRACTOR"; }

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
 * @brief *DATABASE_SBTOUT_SENSOR
 *
 * Seatbelt sensor output database.
 */
class KOO_API DatabaseSbtoutSensor : public CloneableKeyword<DatabaseSbtoutSensor, DatabaseKeyword> {
public:
    struct Data {
        double dt = 0.0;
        int binary = 0;
        int lcur = 0;
        int ioopt = 0;
    };

    DatabaseSbtoutSensor() = default;

    std::string getKeywordName() const override { return "*DATABASE_SBTOUT_SENSOR"; }

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
 * @brief *DATABASE_PLLYOUT_RETRACTOR
 *
 * Pretensioner/Retractor pullout output database.
 */
class KOO_API DatabasePllyoutRetractor : public CloneableKeyword<DatabasePllyoutRetractor, DatabaseKeyword> {
public:
    struct Data {
        double dt = 0.0;
        int binary = 0;
        int lcur = 0;
        int ioopt = 0;
    };

    DatabasePllyoutRetractor() = default;

    std::string getKeywordName() const override { return "*DATABASE_PLLYOUT_RETRACTOR"; }

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
 * @brief *DATABASE_SPH_FLOW_SENSOR
 *
 * SPH flow sensor output database.
 */
class KOO_API DatabaseSphFlowSensor : public CloneableKeyword<DatabaseSphFlowSensor, DatabaseKeyword> {
public:
    struct Data {
        double dt = 0.0;
        int binary = 0;
        int lcur = 0;
        int ioopt = 0;
    };

    DatabaseSphFlowSensor() = default;

    std::string getKeywordName() const override { return "*DATABASE_SPH_FLOW_SENSOR"; }

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
 * @brief *DATABASE_DEMASSFLOW
 *
 * Discrete element mass flow output database.
 */
class KOO_API DatabaseDemassflow : public CloneableKeyword<DatabaseDemassflow, DatabaseKeyword> {
public:
    struct Data {
        double dt = 0.0;
        int binary = 0;
        int lcur = 0;
        int ioopt = 0;
    };

    DatabaseDemassflow() = default;

    std::string getKeywordName() const override { return "*DATABASE_DEMASSFLOW"; }

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
 * @brief *DATABASE_TOTGEO
 *
 * Total geometry output database.
 */
class KOO_API DatabaseTotgeo : public CloneableKeyword<DatabaseTotgeo, DatabaseKeyword> {
public:
    struct Data {
        double dt = 0.0;
        int binary = 0;
        int lcur = 0;
        int ioopt = 0;
    };

    DatabaseTotgeo() = default;

    std::string getKeywordName() const override { return "*DATABASE_TOTGEO"; }

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
 * @brief *DATABASE_PSD
 *
 * Power spectral density output database.
 */
class KOO_API DatabasePsd : public CloneableKeyword<DatabasePsd, DatabaseKeyword> {
public:
    struct Data {
        double dt = 0.0;
        int binary = 0;
        int lcur = 0;
        int ioopt = 0;
    };

    DatabasePsd() = default;

    std::string getKeywordName() const override { return "*DATABASE_PSD"; }

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
 * @brief *DATABASE_ABSTAT_MASS
 *
 * Airbag statistics mass output database.
 */
class KOO_API DatabaseAbstatMass : public CloneableKeyword<DatabaseAbstatMass, DatabaseKeyword> {
public:
    struct Data {
        double dt = 0.0;
        int binary = 0;
        int lcur = 0;
        int ioopt = 0;
    };

    DatabaseAbstatMass() = default;

    std::string getKeywordName() const override { return "*DATABASE_ABSTAT_MASS"; }

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
 * @brief *DATABASE_SWFORC_FILTER
 *
 * Spotweld force filter output database.
 */
class KOO_API DatabaseSwforcFilter : public CloneableKeyword<DatabaseSwforcFilter, DatabaseKeyword> {
public:
    struct Data {
        double dt = 0.0;
        int binary = 0;
        int lcur = 0;
        int ioopt = 0;
    };

    DatabaseSwforcFilter() = default;

    std::string getKeywordName() const override { return "*DATABASE_SWFORC_FILTER"; }

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
 * @brief *DATABASE_RVE
 *
 * Representative Volume Element output database.
 */
class KOO_API DatabaseRve : public CloneableKeyword<DatabaseRve, DatabaseKeyword> {
public:
    struct Data {
        double dt = 0.0;
        int binary = 0;
        int lcur = 0;
        int ioopt = 0;
    };

    DatabaseRve() = default;

    std::string getKeywordName() const override { return "*DATABASE_RVE"; }

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
