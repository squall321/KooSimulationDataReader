#pragma once

#include <koo/Export.hpp>
#include <koo/dyna/Keyword.hpp>
#include <koo/util/Types.hpp>
#include <unordered_map>
#include <vector>

namespace koo::dyna {

/**
 * @brief Single node data
 */
struct KOO_API NodeData {
    NodeId id = 0;
    Vec3 position;
    int tc = 0;      // Translational constraint
    int rc = 0;      // Rotational constraint

    NodeData() = default;
    NodeData(NodeId id_, double x, double y, double z)
        : id(id_), position(x, y, z) {}
    NodeData(NodeId id_, const Vec3& pos)
        : id(id_), position(pos) {}
};

/**
 * @brief *NODE keyword - collection of nodes
 */
class KOO_API Node : public CloneableKeyword<Node> {
public:
    Node() = default;

    std::string getKeywordName() const override { return "*NODE"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    // Node access
    void addNode(const NodeData& node);
    void addNode(NodeId id, double x, double y, double z);
    void addNode(NodeId id, const Vec3& position);

    bool hasNode(NodeId id) const;
    NodeData* getNode(NodeId id);
    const NodeData* getNode(NodeId id) const;

    void removeNode(NodeId id);
    void clear();

    // Iteration
    const std::vector<NodeData>& getNodes() const { return nodes_; }
    std::vector<NodeData>& getNodes() { return nodes_; }
    size_t getNodeCount() const { return nodes_.size(); }

    // Find node by ID (returns nullptr if not found)
    NodeData* findNode(NodeId id);
    const NodeData* findNode(NodeId id) const;

    // Bounding box
    BoundingBox getBoundingBox() const;

    // Transform all nodes
    void transform(const Matrix4x4& matrix);

private:
    void rebuildIndex();

    std::vector<NodeData> nodes_;
    std::unordered_map<NodeId, size_t> idIndex_;  // id -> index in nodes_
};

/**
 * @brief *NODE_TRANSFORM keyword
 *
 * Applies a coordinate transformation to nodes.
 */
class KOO_API NodeTransform : public CloneableKeyword<NodeTransform> {
public:
    struct Data {
        int nsid = 0;       // Node set ID
        int cid = 0;        // Coordinate system ID
    };

    NodeTransform() = default;

    std::string getKeywordName() const override { return "*NODE_TRANSFORM"; }

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
 * @brief *NODE_MERGE keyword
 *
 * Merges coincident nodes within a tolerance.
 */
class KOO_API NodeMerge : public CloneableKeyword<NodeMerge> {
public:
    struct Data {
        double tol = 0.0;    // Merge tolerance
        int nsid = 0;        // Node set ID (0=all)
    };

    NodeMerge() = default;

    std::string getKeywordName() const override { return "*NODE_MERGE"; }

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
 * @brief *NODE_SCALAR keyword
 *
 * Defines a scalar node for thermal or other scalar analyses.
 */
class KOO_API NodeScalar : public CloneableKeyword<NodeScalar> {
public:
    struct NodeScalarData {
        NodeId nid = 0;
        double value = 0.0;  // Initial scalar value
    };

    NodeScalar() = default;

    std::string getKeywordName() const override { return "*NODE_SCALAR"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    void addNode(const NodeScalarData& node);
    const std::vector<NodeScalarData>& getNodes() const { return nodes_; }
    std::vector<NodeScalarData>& getNodes() { return nodes_; }

private:
    std::vector<NodeScalarData> nodes_;
};

/**
 * @brief *NODE_RIGID_BODY keyword
 *
 * Defines additional nodes for rigid bodies.
 */
class KOO_API NodeRigidBody : public CloneableKeyword<NodeRigidBody> {
public:
    struct Data {
        PartId pid = 0;     // Part ID of the rigid body
        NodeId nid = 0;     // Node ID
        int pnode = 0;      // Node type (1=inertia, 2=extra)
    };

    NodeRigidBody() = default;

    std::string getKeywordName() const override { return "*NODE_RIGID_BODY"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    void addEntry(const Data& entry);
    const std::vector<Data>& getEntries() const { return entries_; }
    std::vector<Data>& getEntries() { return entries_; }

private:
    std::vector<Data> entries_;
};

/**
 * @brief *NODE_THICKNESS keyword
 *
 * Defines nodal thickness for shell elements.
 */
class KOO_API NodeThickness : public CloneableKeyword<NodeThickness> {
public:
    struct NodeThicknessData {
        NodeId nid = 0;
        double thick = 0.0;
    };

    NodeThickness() = default;

    std::string getKeywordName() const override { return "*NODE_THICKNESS"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    void addNode(const NodeThicknessData& node);
    const std::vector<NodeThicknessData>& getNodes() const { return nodes_; }
    std::vector<NodeThicknessData>& getNodes() { return nodes_; }

private:
    std::vector<NodeThicknessData> nodes_;
};

/**
 * @brief *NODE_TO_TARGET keyword
 *
 * Defines nodes to target surface mapping for contact.
 */
class KOO_API NodeToTarget : public CloneableKeyword<NodeToTarget> {
public:
    struct Data {
        NodeId nid = 0;     // Node ID
        int segid = 0;      // Segment ID
    };

    NodeToTarget() = default;

    std::string getKeywordName() const override { return "*NODE_TO_TARGET"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    void addEntry(const Data& entry);
    const std::vector<Data>& getEntries() const { return entries_; }
    std::vector<Data>& getEntries() { return entries_; }

private:
    std::vector<Data> entries_;
};

/**
 * @brief *NODE_SPOT_WELD keyword
 *
 * Defines spot weld nodes for joining.
 */
class KOO_API NodeSpotWeld : public CloneableKeyword<NodeSpotWeld> {
public:
    struct Data {
        NodeId nid = 0;     // Node ID
        int nsid1 = 0;      // Node set 1 (master)
        int nsid2 = 0;      // Node set 2 (slave)
        double dn = 0.0;    // Normal failure displacement
        double dt = 0.0;    // Tangential failure displacement
    };

    NodeSpotWeld() = default;

    std::string getKeywordName() const override { return "*NODE_SPOT_WELD"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    void addEntry(const Data& entry);
    const std::vector<Data>& getEntries() const { return entries_; }
    std::vector<Data>& getEntries() { return entries_; }

private:
    std::vector<Data> entries_;
};

/**
 * @brief *NODE_MERGE_SET keyword
 *
 * Merges coincident nodes within a tolerance for specific node sets.
 */
class KOO_API NodeMergeSet : public CloneableKeyword<NodeMergeSet> {
public:
    struct Data {
        int nsid1 = 0;      // First node set ID
        int nsid2 = 0;      // Second node set ID
        double tol = 0.0;   // Merge tolerance
    };

    NodeMergeSet() = default;

    std::string getKeywordName() const override { return "*NODE_MERGE_SET"; }

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
 * @brief *NODE_MERGE_TOLERANCE keyword
 *
 * Defines tolerance for automatic node merging.
 */
class KOO_API NodeMergeTolerance : public CloneableKeyword<NodeMergeTolerance> {
public:
    struct Data {
        double tol = 0.0;   // Merge tolerance
    };

    NodeMergeTolerance() = default;

    std::string getKeywordName() const override { return "*NODE_MERGE_TOLERANCE"; }

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
 * @brief *NODE_REFERENCE keyword
 *
 * Defines reference nodes for coordinate systems.
 */
class KOO_API NodeReference : public CloneableKeyword<NodeReference> {
public:
    struct Data {
        NodeId nid = 0;     // Node ID
        int cid = 0;        // Coordinate system ID
    };

    NodeReference() = default;

    std::string getKeywordName() const override { return "*NODE_REFERENCE"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    void addEntry(const Data& entry);
    const std::vector<Data>& getEntries() const { return entries_; }
    std::vector<Data>& getEntries() { return entries_; }

private:
    std::vector<Data> entries_;
};

/**
 * @brief *NODE_RIGID_SURFACE keyword
 *
 * Defines nodes on a rigid surface.
 */
class KOO_API NodeRigidSurface : public CloneableKeyword<NodeRigidSurface> {
public:
    struct Data {
        NodeId nid = 0;     // Node ID
        int surfid = 0;     // Rigid surface ID
    };

    NodeRigidSurface() = default;

    std::string getKeywordName() const override { return "*NODE_RIGID_SURFACE"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    void addEntry(const Data& entry);
    const std::vector<Data>& getEntries() const { return entries_; }
    std::vector<Data>& getEntries() { return entries_; }

private:
    std::vector<Data> entries_;
};

/**
 * @brief *NODE_SCALAR_VALUE keyword
 *
 * Defines scalar values at nodes.
 */
class KOO_API NodeScalarValue : public CloneableKeyword<NodeScalarValue> {
public:
    struct Data {
        NodeId nid = 0;     // Node ID
        double value = 0.0; // Scalar value
    };

    NodeScalarValue() = default;

    std::string getKeywordName() const override { return "*NODE_SCALAR_VALUE"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    void addEntry(const Data& entry);
    const std::vector<Data>& getEntries() const { return entries_; }
    std::vector<Data>& getEntries() { return entries_; }

private:
    std::vector<Data> entries_;
};

/**
 * @brief *NODE_THICKNESS_SET keyword
 *
 * Defines nodal thickness for a node set.
 */
class KOO_API NodeThicknessSet : public CloneableKeyword<NodeThicknessSet> {
public:
    struct Data {
        int nsid = 0;       // Node set ID
        double thick = 0.0; // Thickness value
    };

    NodeThicknessSet() = default;

    std::string getKeywordName() const override { return "*NODE_THICKNESS_SET"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    void addEntry(const Data& entry);
    const std::vector<Data>& getEntries() const { return entries_; }
    std::vector<Data>& getEntries() { return entries_; }

private:
    std::vector<Data> entries_;
};

/**
 * @brief *NODE_THICKNESS_SET_GENERATE keyword
 *
 * Generates nodal thickness for a node set with interpolation.
 */
class KOO_API NodeThicknessSetGenerate : public CloneableKeyword<NodeThicknessSetGenerate> {
public:
    struct Data {
        int nsid = 0;           // Node set ID
        double thick1 = 0.0;    // Start thickness
        double thick2 = 0.0;    // End thickness
    };

    NodeThicknessSetGenerate() = default;

    std::string getKeywordName() const override { return "*NODE_THICKNESS_SET_GENERATE"; }

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
 * @brief *NODE_TO_TARGET_VECTOR keyword
 *
 * Defines nodes to target mapping with vector direction.
 */
class KOO_API NodeToTargetVector : public CloneableKeyword<NodeToTargetVector> {
public:
    struct Data {
        NodeId nid = 0;     // Node ID
        int segid = 0;      // Segment ID
        double vx = 0.0;    // Vector X component
        double vy = 0.0;    // Vector Y component
        double vz = 0.0;    // Vector Z component
    };

    NodeToTargetVector() = default;

    std::string getKeywordName() const override { return "*NODE_TO_TARGET_VECTOR"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    void addEntry(const Data& entry);
    const std::vector<Data>& getEntries() const { return entries_; }
    std::vector<Data>& getEntries() { return entries_; }

private:
    std::vector<Data> entries_;
};

} // namespace koo::dyna
