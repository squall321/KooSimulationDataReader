#pragma once

#include <koo/Export.hpp>
#include <koo/dyna/Keyword.hpp>
#include <koo/util/Types.hpp>
#include <vector>
#include <string>

namespace koo::dyna {

/**
 * @brief Base class for *SET keywords
 */
class KOO_API SetKeyword : public Keyword {
public:
    virtual ~SetKeyword() = default;
};

/**
 * @brief *SET_NODE
 *
 * Defines a set of nodes.
 */
class KOO_API SetNode : public CloneableKeyword<SetNode, SetKeyword> {
public:
    SetNode() = default;

    std::string getKeywordName() const override { return "*SET_NODE"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    // Set ID
    int getSetId() const { return sid_; }
    void setSetId(int id) { sid_ = id; }

    // Node access
    std::vector<NodeId>& getNodes() { return nodes_; }
    const std::vector<NodeId>& getNodes() const { return nodes_; }

    void addNode(NodeId id) { nodes_.push_back(id); }
    void clearNodes() { nodes_.clear(); }
    size_t getNodeCount() const { return nodes_.size(); }

private:
    int sid_ = 0;
    std::vector<NodeId> nodes_;
};

/**
 * @brief *SET_NODE_LIST
 *
 * Defines a set of nodes using explicit node IDs.
 */
class KOO_API SetNodeList : public CloneableKeyword<SetNodeList, SetKeyword> {
public:
    SetNodeList() = default;

    std::string getKeywordName() const override { return "*SET_NODE_LIST"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    int getSetId() const { return sid_; }
    void setSetId(int id) { sid_ = id; }

    std::vector<NodeId>& getNodes() { return nodes_; }
    const std::vector<NodeId>& getNodes() const { return nodes_; }

    void addNode(NodeId id) { nodes_.push_back(id); }

private:
    int sid_ = 0;
    std::vector<NodeId> nodes_;
};

/**
 * @brief *SET_NODE_LIST_TITLE
 *
 * SET_NODE_LIST with title.
 */
class KOO_API SetNodeListTitle : public CloneableKeyword<SetNodeListTitle, SetKeyword> {
public:
    SetNodeListTitle() = default;

    std::string getKeywordName() const override { return "*SET_NODE_LIST_TITLE"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    const std::string& getTitle() const { return title_; }
    void setTitle(const std::string& title) { title_ = title; }

    int getSetId() const { return sid_; }
    void setSetId(int id) { sid_ = id; }

    std::vector<NodeId>& getNodes() { return nodes_; }
    const std::vector<NodeId>& getNodes() const { return nodes_; }

private:
    std::string title_;
    int sid_ = 0;
    std::vector<NodeId> nodes_;
};

/**
 * @brief *SET_PART
 *
 * Defines a set of parts.
 */
class KOO_API SetPart : public CloneableKeyword<SetPart, SetKeyword> {
public:
    SetPart() = default;

    std::string getKeywordName() const override { return "*SET_PART"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    int getSetId() const { return sid_; }
    void setSetId(int id) { sid_ = id; }

    std::vector<PartId>& getParts() { return parts_; }
    const std::vector<PartId>& getParts() const { return parts_; }

    void addPart(PartId id) { parts_.push_back(id); }
    void clearParts() { parts_.clear(); }
    size_t getPartCount() const { return parts_.size(); }

private:
    int sid_ = 0;
    std::vector<PartId> parts_;
};

/**
 * @brief *SET_PART_LIST
 *
 * Defines a set of parts using explicit part IDs.
 */
class KOO_API SetPartList : public CloneableKeyword<SetPartList, SetKeyword> {
public:
    SetPartList() = default;

    std::string getKeywordName() const override { return "*SET_PART_LIST"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    int getSetId() const { return sid_; }
    void setSetId(int id) { sid_ = id; }

    std::vector<PartId>& getParts() { return parts_; }
    const std::vector<PartId>& getParts() const { return parts_; }

private:
    int sid_ = 0;
    std::vector<PartId> parts_;
};

/**
 * @brief *SET_PART_LIST_TITLE
 *
 * SET_PART_LIST with title.
 */
class KOO_API SetPartListTitle : public CloneableKeyword<SetPartListTitle, SetKeyword> {
public:
    SetPartListTitle() = default;

    std::string getKeywordName() const override { return "*SET_PART_LIST_TITLE"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    const std::string& getTitle() const { return title_; }
    void setTitle(const std::string& title) { title_ = title; }

    int getSetId() const { return sid_; }
    void setSetId(int id) { sid_ = id; }

    std::vector<PartId>& getParts() { return parts_; }
    const std::vector<PartId>& getParts() const { return parts_; }

private:
    std::string title_;
    int sid_ = 0;
    std::vector<PartId> parts_;
};

/**
 * @brief *SET_SEGMENT
 *
 * Defines a set of segments (element faces).
 */
class KOO_API SetSegment : public CloneableKeyword<SetSegment, SetKeyword> {
public:
    struct Segment {
        NodeId n1 = 0;
        NodeId n2 = 0;
        NodeId n3 = 0;
        NodeId n4 = 0;          // 0 for triangles
    };

    SetSegment() = default;

    std::string getKeywordName() const override { return "*SET_SEGMENT"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    int getSetId() const { return sid_; }
    void setSetId(int id) { sid_ = id; }

    std::vector<Segment>& getSegments() { return segments_; }
    const std::vector<Segment>& getSegments() const { return segments_; }

    void addSegment(NodeId n1, NodeId n2, NodeId n3, NodeId n4 = 0) {
        segments_.push_back({n1, n2, n3, n4});
    }
    void clearSegments() { segments_.clear(); }

private:
    int sid_ = 0;
    std::vector<Segment> segments_;
};

/**
 * @brief *SET_SEGMENT_TITLE
 *
 * SET_SEGMENT with title.
 */
class KOO_API SetSegmentTitle : public CloneableKeyword<SetSegmentTitle, SetKeyword> {
public:
    using Segment = SetSegment::Segment;

    SetSegmentTitle() = default;

    std::string getKeywordName() const override { return "*SET_SEGMENT_TITLE"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    const std::string& getTitle() const { return title_; }
    void setTitle(const std::string& title) { title_ = title; }

    int getSetId() const { return sid_; }
    void setSetId(int id) { sid_ = id; }

    std::vector<Segment>& getSegments() { return segments_; }
    const std::vector<Segment>& getSegments() const { return segments_; }

private:
    std::string title_;
    int sid_ = 0;
    std::vector<Segment> segments_;
};

/**
 * @brief *SET_SHELL
 *
 * Defines a set of shell elements.
 */
class KOO_API SetShell : public CloneableKeyword<SetShell, SetKeyword> {
public:
    SetShell() = default;

    std::string getKeywordName() const override { return "*SET_SHELL"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    int getSetId() const { return sid_; }
    void setSetId(int id) { sid_ = id; }

    std::vector<ElementId>& getElements() { return elements_; }
    const std::vector<ElementId>& getElements() const { return elements_; }

    void addElement(ElementId id) { elements_.push_back(id); }

private:
    int sid_ = 0;
    std::vector<ElementId> elements_;
};

/**
 * @brief *SET_SHELL_LIST
 *
 * Defines a set of shell elements using explicit IDs.
 */
class KOO_API SetShellList : public CloneableKeyword<SetShellList, SetKeyword> {
public:
    SetShellList() = default;

    std::string getKeywordName() const override { return "*SET_SHELL_LIST"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    int getSetId() const { return sid_; }
    void setSetId(int id) { sid_ = id; }

    std::vector<ElementId>& getElements() { return elements_; }
    const std::vector<ElementId>& getElements() const { return elements_; }

private:
    int sid_ = 0;
    std::vector<ElementId> elements_;
};

/**
 * @brief *SET_SOLID
 *
 * Defines a set of solid elements.
 */
class KOO_API SetSolid : public CloneableKeyword<SetSolid, SetKeyword> {
public:
    SetSolid() = default;

    std::string getKeywordName() const override { return "*SET_SOLID"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    int getSetId() const { return sid_; }
    void setSetId(int id) { sid_ = id; }

    std::vector<ElementId>& getElements() { return elements_; }
    const std::vector<ElementId>& getElements() const { return elements_; }

    void addElement(ElementId id) { elements_.push_back(id); }

private:
    int sid_ = 0;
    std::vector<ElementId> elements_;
};

/**
 * @brief *SET_SOLID_LIST
 *
 * Defines a set of solid elements using explicit IDs.
 */
class KOO_API SetSolidList : public CloneableKeyword<SetSolidList, SetKeyword> {
public:
    SetSolidList() = default;

    std::string getKeywordName() const override { return "*SET_SOLID_LIST"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    int getSetId() const { return sid_; }
    void setSetId(int id) { sid_ = id; }

    std::vector<ElementId>& getElements() { return elements_; }
    const std::vector<ElementId>& getElements() const { return elements_; }

private:
    int sid_ = 0;
    std::vector<ElementId> elements_;
};

/**
 * @brief *SET_NODE_GENERATE
 *
 * Defines a set of nodes using ranges.
 */
class KOO_API SetNodeGenerate : public CloneableKeyword<SetNodeGenerate, SetKeyword> {
public:
    struct Range {
        NodeId nid1 = 0;      // Starting node ID
        NodeId nid2 = 0;      // Ending node ID
        int dnidn = 1;        // Increment
    };

    SetNodeGenerate() = default;

    std::string getKeywordName() const override { return "*SET_NODE_GENERATE"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    int getSetId() const { return sid_; }
    void setSetId(int id) { sid_ = id; }

    std::vector<Range>& getRanges() { return ranges_; }
    const std::vector<Range>& getRanges() const { return ranges_; }

private:
    int sid_ = 0;
    std::vector<Range> ranges_;
};

/**
 * @brief *SET_PART_GENERATE
 *
 * Defines a set of parts using ranges.
 */
class KOO_API SetPartGenerate : public CloneableKeyword<SetPartGenerate, SetKeyword> {
public:
    struct Range {
        PartId pid1 = 0;      // Starting part ID
        PartId pid2 = 0;      // Ending part ID
        int dpidp = 1;        // Increment
    };

    SetPartGenerate() = default;

    std::string getKeywordName() const override { return "*SET_PART_GENERATE"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    int getSetId() const { return sid_; }
    void setSetId(int id) { sid_ = id; }

    std::vector<Range>& getRanges() { return ranges_; }
    const std::vector<Range>& getRanges() const { return ranges_; }

private:
    int sid_ = 0;
    std::vector<Range> ranges_;
};

/**
 * @brief *SET_SHELL_GENERATE
 *
 * Defines a set of shell elements using ranges.
 */
class KOO_API SetShellGenerate : public CloneableKeyword<SetShellGenerate, SetKeyword> {
public:
    struct Range {
        ElementId eid1 = 0;   // Starting element ID
        ElementId eid2 = 0;   // Ending element ID
        int deide = 1;        // Increment
    };

    SetShellGenerate() = default;

    std::string getKeywordName() const override { return "*SET_SHELL_GENERATE"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    int getSetId() const { return sid_; }
    void setSetId(int id) { sid_ = id; }

    std::vector<Range>& getRanges() { return ranges_; }
    const std::vector<Range>& getRanges() const { return ranges_; }

private:
    int sid_ = 0;
    std::vector<Range> ranges_;
};

/**
 * @brief *SET_SOLID_GENERATE
 *
 * Defines a set of solid elements using ranges.
 */
class KOO_API SetSolidGenerate : public CloneableKeyword<SetSolidGenerate, SetKeyword> {
public:
    struct Range {
        ElementId eid1 = 0;   // Starting element ID
        ElementId eid2 = 0;   // Ending element ID
        int deide = 1;        // Increment
    };

    SetSolidGenerate() = default;

    std::string getKeywordName() const override { return "*SET_SOLID_GENERATE"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    int getSetId() const { return sid_; }
    void setSetId(int id) { sid_ = id; }

    std::vector<Range>& getRanges() { return ranges_; }
    const std::vector<Range>& getRanges() const { return ranges_; }

private:
    int sid_ = 0;
    std::vector<Range> ranges_;
};

/**
 * @brief *SET_BEAM
 *
 * Defines a set of beam elements.
 */
class KOO_API SetBeam : public CloneableKeyword<SetBeam, SetKeyword> {
public:
    SetBeam() = default;

    std::string getKeywordName() const override { return "*SET_BEAM"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    int getSetId() const { return sid_; }
    void setSetId(int id) { sid_ = id; }

    std::vector<ElementId>& getElements() { return elements_; }
    const std::vector<ElementId>& getElements() const { return elements_; }

private:
    int sid_ = 0;
    std::vector<ElementId> elements_;
};

/**
 * @brief *SET_BEAM_GENERATE
 *
 * Defines a set of beam elements using ranges.
 */
class KOO_API SetBeamGenerate : public CloneableKeyword<SetBeamGenerate, SetKeyword> {
public:
    struct Range {
        ElementId eid1 = 0;
        ElementId eid2 = 0;
        int deide = 1;
    };

    SetBeamGenerate() = default;

    std::string getKeywordName() const override { return "*SET_BEAM_GENERATE"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    int getSetId() const { return sid_; }
    void setSetId(int id) { sid_ = id; }

    std::vector<Range>& getRanges() { return ranges_; }
    const std::vector<Range>& getRanges() const { return ranges_; }

private:
    int sid_ = 0;
    std::vector<Range> ranges_;
};

/**
 * @brief *SET_DISCRETE
 *
 * Defines a set of discrete elements.
 */
class KOO_API SetDiscrete : public CloneableKeyword<SetDiscrete, SetKeyword> {
public:
    SetDiscrete() = default;

    std::string getKeywordName() const override { return "*SET_DISCRETE"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    int getSetId() const { return sid_; }
    void setSetId(int id) { sid_ = id; }

    std::vector<ElementId>& getElements() { return elements_; }
    const std::vector<ElementId>& getElements() const { return elements_; }

private:
    int sid_ = 0;
    std::vector<ElementId> elements_;
};

/**
 * @brief *SET_NODE_ADD
 *
 * Add nodes to existing set.
 */
class KOO_API SetNodeAdd : public CloneableKeyword<SetNodeAdd, SetKeyword> {
public:
    SetNodeAdd() = default;

    std::string getKeywordName() const override { return "*SET_NODE_ADD"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    int getSetId() const { return sid_; }
    void setSetId(int id) { sid_ = id; }

    std::vector<NodeId>& getNodes() { return nodes_; }
    const std::vector<NodeId>& getNodes() const { return nodes_; }

private:
    int sid_ = 0;
    std::vector<NodeId> nodes_;
};

/**
 * @brief *SET_PART_ADD
 *
 * Add parts to existing set.
 */
class KOO_API SetPartAdd : public CloneableKeyword<SetPartAdd, SetKeyword> {
public:
    SetPartAdd() = default;

    std::string getKeywordName() const override { return "*SET_PART_ADD"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    int getSetId() const { return sid_; }
    void setSetId(int id) { sid_ = id; }

    std::vector<PartId>& getParts() { return parts_; }
    const std::vector<PartId>& getParts() const { return parts_; }

private:
    int sid_ = 0;
    std::vector<PartId> parts_;
};

/**
 * @brief *SET_NODE_COLUMN
 *
 * Defines node set from column of nodes.
 */
class KOO_API SetNodeColumn : public CloneableKeyword<SetNodeColumn, SetKeyword> {
public:
    struct Data {
        int sid = 0;
        int da1 = 0;
        int da2 = 0;
        int da3 = 0;
        int da4 = 0;
    };

    SetNodeColumn() = default;

    std::string getKeywordName() const override { return "*SET_NODE_COLUMN"; }

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
 * @brief *SET_SEGMENT_GENERAL
 *
 * General segment set definition.
 */
class KOO_API SetSegmentGeneral : public CloneableKeyword<SetSegmentGeneral, SetKeyword> {
public:
    struct Data {
        int sid = 0;
        int option = 0;
        int e1 = 0;
        int e2 = 0;
        int e3 = 0;
        int e4 = 0;
    };

    SetSegmentGeneral() = default;

    std::string getKeywordName() const override { return "*SET_SEGMENT_GENERAL"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    std::vector<Data>& getData() { return data_; }
    const std::vector<Data>& getData() const { return data_; }

private:
    std::vector<Data> data_;
};

/**
 * @brief *SET_2D_SHELL
 *
 * 2D shell set for planar analyses.
 */
class KOO_API Set2dShell : public CloneableKeyword<Set2dShell, SetKeyword> {
public:
    Set2dShell() = default;

    std::string getKeywordName() const override { return "*SET_2D_SHELL"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    int getSetId() const { return sid_; }
    void setSetId(int id) { sid_ = id; }

    std::vector<ElementId>& getElements() { return elements_; }
    const std::vector<ElementId>& getElements() const { return elements_; }

private:
    int sid_ = 0;
    std::vector<ElementId> elements_;
};

/**
 * @brief *SET_TSHELL
 *
 * Thick shell element set.
 */
class KOO_API SetTshell : public CloneableKeyword<SetTshell, SetKeyword> {
public:
    SetTshell() = default;

    std::string getKeywordName() const override { return "*SET_TSHELL"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    int getSetId() const { return sid_; }
    void setSetId(int id) { sid_ = id; }

    std::vector<ElementId>& getElements() { return elements_; }
    const std::vector<ElementId>& getElements() const { return elements_; }

private:
    int sid_ = 0;
    std::vector<ElementId> elements_;
};

/**
 * @brief *SET_TSHELL_LIST
 *
 * Thick shell element list set.
 */
class KOO_API SetTshellList : public CloneableKeyword<SetTshellList, SetKeyword> {
public:
    SetTshellList() = default;

    std::string getKeywordName() const override { return "*SET_TSHELL_LIST"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    int getSetId() const { return sid_; }
    void setSetId(int id) { sid_ = id; }

    std::vector<ElementId>& getElements() { return elements_; }
    const std::vector<ElementId>& getElements() const { return elements_; }

private:
    int sid_ = 0;
    std::vector<ElementId> elements_;
};

/**
 * @brief *SET_BEAM_LIST
 *
 * Beam element list set.
 */
class KOO_API SetBeamList : public CloneableKeyword<SetBeamList, SetKeyword> {
public:
    SetBeamList() = default;

    std::string getKeywordName() const override { return "*SET_BEAM_LIST"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    int getSetId() const { return sid_; }
    void setSetId(int id) { sid_ = id; }

    std::vector<ElementId>& getElements() { return elements_; }
    const std::vector<ElementId>& getElements() const { return elements_; }

private:
    int sid_ = 0;
    std::vector<ElementId> elements_;
};

/**
 * @brief *SET_DISCRETE_LIST
 *
 * Discrete element list set.
 */
class KOO_API SetDiscreteList : public CloneableKeyword<SetDiscreteList, SetKeyword> {
public:
    SetDiscreteList() = default;

    std::string getKeywordName() const override { return "*SET_DISCRETE_LIST"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    int getSetId() const { return sid_; }
    void setSetId(int id) { sid_ = id; }

    std::vector<ElementId>& getElements() { return elements_; }
    const std::vector<ElementId>& getElements() const { return elements_; }

private:
    int sid_ = 0;
    std::vector<ElementId> elements_;
};

/**
 * @brief *SET_NODE_GENERAL
 *
 * General node set with options.
 */
class KOO_API SetNodeGeneral : public CloneableKeyword<SetNodeGeneral, SetKeyword> {
public:
    struct Data {
        int sid = 0;
        int da1 = 0;
        int da2 = 0;
        int da3 = 0;
        int da4 = 0;
        double solver = 0.0;
    };

    SetNodeGeneral() = default;

    std::string getKeywordName() const override { return "*SET_NODE_GENERAL"; }

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
 * @brief *SET_PART_TREE
 *
 * Part set defined by assembly tree.
 */
class KOO_API SetPartTree : public CloneableKeyword<SetPartTree, SetKeyword> {
public:
    struct Data {
        int sid = 0;
        int pid = 0;
        int level = 0;
    };

    SetPartTree() = default;

    std::string getKeywordName() const override { return "*SET_PART_TREE"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    std::vector<Data>& getData() { return data_; }
    const std::vector<Data>& getData() const { return data_; }

private:
    std::vector<Data> data_;
};

/**
 * @brief *SET_MULTI_MATERIAL_GROUP_LIST
 *
 * Multi-material group list set.
 */
class KOO_API SetMultiMaterialGroupList : public CloneableKeyword<SetMultiMaterialGroupList, SetKeyword> {
public:
    SetMultiMaterialGroupList() = default;

    std::string getKeywordName() const override { return "*SET_MULTI_MATERIAL_GROUP_LIST"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    int getSetId() const { return sid_; }
    void setSetId(int id) { sid_ = id; }

    std::vector<int>& getGroups() { return groups_; }
    const std::vector<int>& getGroups() const { return groups_; }

private:
    int sid_ = 0;
    std::vector<int> groups_;
};

/**
 * @brief *SET_IGA_EDGE_UVW
 *
 * IGA edge set by parametric coordinates.
 */
class KOO_API SetIgaEdgeUvw : public CloneableKeyword<SetIgaEdgeUvw, SetKeyword> {
public:
    struct Data {
        int sid = 0;
        int pid = 0;
        double u = 0.0;
        double v = 0.0;
        double w = 0.0;
    };

    SetIgaEdgeUvw() = default;

    std::string getKeywordName() const override { return "*SET_IGA_EDGE_UVW"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    std::vector<Data>& getData() { return data_; }
    const std::vector<Data>& getData() const { return data_; }

private:
    std::vector<Data> data_;
};

/**
 * @brief *SET_IGA_FACE_UVW
 *
 * IGA face set by parametric coordinates.
 */
class KOO_API SetIgaFaceUvw : public CloneableKeyword<SetIgaFaceUvw, SetKeyword> {
public:
    struct Data {
        int sid = 0;
        int pid = 0;
        double u = 0.0;
        double v = 0.0;
        double w = 0.0;
        int idir = 0;
    };

    SetIgaFaceUvw() = default;

    std::string getKeywordName() const override { return "*SET_IGA_FACE_UVW"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    std::vector<Data>& getData() { return data_; }
    const std::vector<Data>& getData() const { return data_; }

private:
    std::vector<Data> data_;
};

/**
 * @brief *SET_NODE_INTERSECT
 *
 * Node set from intersection of two sets.
 */
class KOO_API SetNodeIntersect : public CloneableKeyword<SetNodeIntersect, SetKeyword> {
public:
    struct Data {
        int sid = 0;
        int sid1 = 0;
        int sid2 = 0;
    };

    SetNodeIntersect() = default;

    std::string getKeywordName() const override { return "*SET_NODE_INTERSECT"; }

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
 * @brief *SET_PART_INTERSECT
 *
 * Part set from intersection of two sets.
 */
class KOO_API SetPartIntersect : public CloneableKeyword<SetPartIntersect, SetKeyword> {
public:
    struct Data {
        int sid = 0;
        int sid1 = 0;
        int sid2 = 0;
    };

    SetPartIntersect() = default;

    std::string getKeywordName() const override { return "*SET_PART_INTERSECT"; }

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
 * @brief *SET_SEATBELT
 *
 * Seatbelt element set.
 */
class KOO_API SetSeatbelt : public CloneableKeyword<SetSeatbelt, SetKeyword> {
public:
    SetSeatbelt() = default;

    std::string getKeywordName() const override { return "*SET_SEATBELT"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    int getSetId() const { return sid_; }
    void setSetId(int id) { sid_ = id; }

    std::vector<ElementId>& getElements() { return elements_; }
    const std::vector<ElementId>& getElements() const { return elements_; }

private:
    int sid_ = 0;
    std::vector<ElementId> elements_;
};

/**
 * @brief *SET_SHELL_LIST_TITLE
 *
 * Shell element list set with title.
 */
class KOO_API SetShellListTitle : public CloneableKeyword<SetShellListTitle, SetKeyword> {
public:
    SetShellListTitle() = default;

    std::string getKeywordName() const override { return "*SET_SHELL_LIST_TITLE"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    const std::string& getTitle() const { return title_; }
    void setTitle(const std::string& t) { title_ = t; }

    int getSetId() const { return sid_; }
    void setSetId(int id) { sid_ = id; }

    std::vector<ElementId>& getElements() { return elements_; }
    const std::vector<ElementId>& getElements() const { return elements_; }

private:
    std::string title_;
    int sid_ = 0;
    std::vector<ElementId> elements_;
};

/**
 * @brief *SET_SOLID_LIST_TITLE
 *
 * Solid element list set with title.
 */
class KOO_API SetSolidListTitle : public CloneableKeyword<SetSolidListTitle, SetKeyword> {
public:
    SetSolidListTitle() = default;

    std::string getKeywordName() const override { return "*SET_SOLID_LIST_TITLE"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    const std::string& getTitle() const { return title_; }
    void setTitle(const std::string& t) { title_ = t; }

    int getSetId() const { return sid_; }
    void setSetId(int id) { sid_ = id; }

    std::vector<ElementId>& getElements() { return elements_; }
    const std::vector<ElementId>& getElements() const { return elements_; }

private:
    std::string title_;
    int sid_ = 0;
    std::vector<ElementId> elements_;
};

/**
 * @brief *SET_BEAM_LIST_TITLE
 *
 * Beam element list set with title.
 */
class KOO_API SetBeamListTitle : public CloneableKeyword<SetBeamListTitle, SetKeyword> {
public:
    SetBeamListTitle() = default;

    std::string getKeywordName() const override { return "*SET_BEAM_LIST_TITLE"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    const std::string& getTitle() const { return title_; }
    void setTitle(const std::string& t) { title_ = t; }

    int getSetId() const { return sid_; }
    void setSetId(int id) { sid_ = id; }

    std::vector<ElementId>& getElements() { return elements_; }
    const std::vector<ElementId>& getElements() const { return elements_; }

private:
    std::string title_;
    int sid_ = 0;
    std::vector<ElementId> elements_;
};

/**
 * @brief *SET_DISCRETE_LIST_TITLE
 *
 * Discrete element list set with title.
 */
class KOO_API SetDiscreteListTitle : public CloneableKeyword<SetDiscreteListTitle, SetKeyword> {
public:
    SetDiscreteListTitle() = default;

    std::string getKeywordName() const override { return "*SET_DISCRETE_LIST_TITLE"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    const std::string& getTitle() const { return title_; }
    void setTitle(const std::string& t) { title_ = t; }

    int getSetId() const { return sid_; }
    void setSetId(int id) { sid_ = id; }

    std::vector<ElementId>& getElements() { return elements_; }
    const std::vector<ElementId>& getElements() const { return elements_; }

private:
    std::string title_;
    int sid_ = 0;
    std::vector<ElementId> elements_;
};

/**
 * @brief *SET_NODE_TITLE
 *
 * Node set with title.
 */
class KOO_API SetNodeTitle : public CloneableKeyword<SetNodeTitle, SetKeyword> {
public:
    SetNodeTitle() = default;

    std::string getKeywordName() const override { return "*SET_NODE_TITLE"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    const std::string& getTitle() const { return title_; }
    void setTitle(const std::string& t) { title_ = t; }

    int getSetId() const { return sid_; }
    void setSetId(int id) { sid_ = id; }

    std::vector<NodeId>& getNodes() { return nodes_; }
    const std::vector<NodeId>& getNodes() const { return nodes_; }

private:
    std::string title_;
    int sid_ = 0;
    std::vector<NodeId> nodes_;
};

/**
 * @brief *SET_PART_TITLE
 *
 * Part set with title.
 */
class KOO_API SetPartTitle : public CloneableKeyword<SetPartTitle, SetKeyword> {
public:
    SetPartTitle() = default;

    std::string getKeywordName() const override { return "*SET_PART_TITLE"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    const std::string& getTitle() const { return title_; }
    void setTitle(const std::string& t) { title_ = t; }

    int getSetId() const { return sid_; }
    void setSetId(int id) { sid_ = id; }

    std::vector<PartId>& getParts() { return parts_; }
    const std::vector<PartId>& getParts() const { return parts_; }

private:
    std::string title_;
    int sid_ = 0;
    std::vector<PartId> parts_;
};

/**
 * @brief *SET_SHELL_GENERATE_TITLE
 *
 * Shell element generate set with title.
 */
class KOO_API SetShellGenerateTitle : public CloneableKeyword<SetShellGenerateTitle, SetKeyword> {
public:
    using Range = SetShellGenerate::Range;

    SetShellGenerateTitle() = default;

    std::string getKeywordName() const override { return "*SET_SHELL_GENERATE_TITLE"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    const std::string& getTitle() const { return title_; }
    void setTitle(const std::string& t) { title_ = t; }

    int getSetId() const { return sid_; }
    void setSetId(int id) { sid_ = id; }

    std::vector<Range>& getRanges() { return ranges_; }
    const std::vector<Range>& getRanges() const { return ranges_; }

private:
    std::string title_;
    int sid_ = 0;
    std::vector<Range> ranges_;
};

/**
 * @brief *SET_SOLID_GENERATE_TITLE
 *
 * Solid element generate set with title.
 */
class KOO_API SetSolidGenerateTitle : public CloneableKeyword<SetSolidGenerateTitle, SetKeyword> {
public:
    using Range = SetSolidGenerate::Range;

    SetSolidGenerateTitle() = default;

    std::string getKeywordName() const override { return "*SET_SOLID_GENERATE_TITLE"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    const std::string& getTitle() const { return title_; }
    void setTitle(const std::string& t) { title_ = t; }

    int getSetId() const { return sid_; }
    void setSetId(int id) { sid_ = id; }

    std::vector<Range>& getRanges() { return ranges_; }
    const std::vector<Range>& getRanges() const { return ranges_; }

private:
    std::string title_;
    int sid_ = 0;
    std::vector<Range> ranges_;
};

/**
 * @brief *SET_BEAM_GENERATE_TITLE
 *
 * Beam element generate set with title.
 */
class KOO_API SetBeamGenerateTitle : public CloneableKeyword<SetBeamGenerateTitle, SetKeyword> {
public:
    using Range = SetBeamGenerate::Range;

    SetBeamGenerateTitle() = default;

    std::string getKeywordName() const override { return "*SET_BEAM_GENERATE_TITLE"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    const std::string& getTitle() const { return title_; }
    void setTitle(const std::string& t) { title_ = t; }

    int getSetId() const { return sid_; }
    void setSetId(int id) { sid_ = id; }

    std::vector<Range>& getRanges() { return ranges_; }
    const std::vector<Range>& getRanges() const { return ranges_; }

private:
    std::string title_;
    int sid_ = 0;
    std::vector<Range> ranges_;
};

/**
 * @brief *SET_NODE_GENERATE_TITLE
 *
 * Node generate set with title.
 */
class KOO_API SetNodeGenerateTitle : public CloneableKeyword<SetNodeGenerateTitle, SetKeyword> {
public:
    using Range = SetNodeGenerate::Range;

    SetNodeGenerateTitle() = default;

    std::string getKeywordName() const override { return "*SET_NODE_GENERATE_TITLE"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    const std::string& getTitle() const { return title_; }
    void setTitle(const std::string& t) { title_ = t; }

    int getSetId() const { return sid_; }
    void setSetId(int id) { sid_ = id; }

    std::vector<Range>& getRanges() { return ranges_; }
    const std::vector<Range>& getRanges() const { return ranges_; }

private:
    std::string title_;
    int sid_ = 0;
    std::vector<Range> ranges_;
};

/**
 * @brief *SET_PART_GENERATE_TITLE
 *
 * Part generate set with title.
 */
class KOO_API SetPartGenerateTitle : public CloneableKeyword<SetPartGenerateTitle, SetKeyword> {
public:
    using Range = SetPartGenerate::Range;

    SetPartGenerateTitle() = default;

    std::string getKeywordName() const override { return "*SET_PART_GENERATE_TITLE"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    const std::string& getTitle() const { return title_; }
    void setTitle(const std::string& t) { title_ = t; }

    int getSetId() const { return sid_; }
    void setSetId(int id) { sid_ = id; }

    std::vector<Range>& getRanges() { return ranges_; }
    const std::vector<Range>& getRanges() const { return ranges_; }

private:
    std::string title_;
    int sid_ = 0;
    std::vector<Range> ranges_;
};

/**
 * @brief *SET_BEAM_ADD
 *
 * Add beam elements to existing set.
 */
class KOO_API SetBeamAdd : public CloneableKeyword<SetBeamAdd, SetKeyword> {
public:
    SetBeamAdd() = default;

    std::string getKeywordName() const override { return "*SET_BEAM_ADD"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    int getSetId() const { return sid_; }
    void setSetId(int id) { sid_ = id; }

    std::vector<ElementId>& getElements() { return elements_; }
    const std::vector<ElementId>& getElements() const { return elements_; }

private:
    int sid_ = 0;
    std::vector<ElementId> elements_;
};

/**
 * @brief *SET_BEAM_GENERAL
 *
 * General beam element set with options.
 */
class KOO_API SetBeamGeneral : public CloneableKeyword<SetBeamGeneral, SetKeyword> {
public:
    struct Data {
        int sid = 0;
        int option = 0;
        int da1 = 0;
        int da2 = 0;
    };

    SetBeamGeneral() = default;

    std::string getKeywordName() const override { return "*SET_BEAM_GENERAL"; }

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
 * @brief *SET_BEAM_INTERSECT
 *
 * Beam set from intersection of two sets.
 */
class KOO_API SetBeamIntersect : public CloneableKeyword<SetBeamIntersect, SetKeyword> {
public:
    struct Data {
        int sid = 0;
        int sid1 = 0;
        int sid2 = 0;
    };

    SetBeamIntersect() = default;

    std::string getKeywordName() const override { return "*SET_BEAM_INTERSECT"; }

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
 * @brief *SET_BOX
 *
 * Defines a box-shaped region for set generation.
 */
class KOO_API SetBox : public CloneableKeyword<SetBox, SetKeyword> {
public:
    struct Data {
        int boxid = 0;
        double xmin = 0.0;
        double xmax = 0.0;
        double ymin = 0.0;
        double ymax = 0.0;
        double zmin = 0.0;
        double zmax = 0.0;
    };

    SetBox() = default;

    std::string getKeywordName() const override { return "*SET_BOX"; }

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
 * @brief *SET_DISCRETE_ADD
 *
 * Add discrete elements to existing set.
 */
class KOO_API SetDiscreteAdd : public CloneableKeyword<SetDiscreteAdd, SetKeyword> {
public:
    SetDiscreteAdd() = default;

    std::string getKeywordName() const override { return "*SET_DISCRETE_ADD"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    int getSetId() const { return sid_; }
    void setSetId(int id) { sid_ = id; }

    std::vector<ElementId>& getElements() { return elements_; }
    const std::vector<ElementId>& getElements() const { return elements_; }

private:
    int sid_ = 0;
    std::vector<ElementId> elements_;
};

/**
 * @brief *SET_DISCRETE_GENERAL
 *
 * General discrete element set with options.
 */
class KOO_API SetDiscreteGeneral : public CloneableKeyword<SetDiscreteGeneral, SetKeyword> {
public:
    struct Data {
        int sid = 0;
        int option = 0;
        int da1 = 0;
        int da2 = 0;
    };

    SetDiscreteGeneral() = default;

    std::string getKeywordName() const override { return "*SET_DISCRETE_GENERAL"; }

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
 * @brief *SET_IGA_EDGE
 *
 * IGA edge set definition.
 */
class KOO_API SetIgaEdge : public CloneableKeyword<SetIgaEdge, SetKeyword> {
public:
    struct Data {
        int sid = 0;
        int pid = 0;
        int edge = 0;
    };

    SetIgaEdge() = default;

    std::string getKeywordName() const override { return "*SET_IGA_EDGE"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    std::vector<Data>& getData() { return data_; }
    const std::vector<Data>& getData() const { return data_; }

private:
    std::vector<Data> data_;
};

/**
 * @brief *SET_IGA_FACE
 *
 * IGA face set definition.
 */
class KOO_API SetIgaFace : public CloneableKeyword<SetIgaFace, SetKeyword> {
public:
    struct Data {
        int sid = 0;
        int pid = 0;
        int face = 0;
    };

    SetIgaFace() = default;

    std::string getKeywordName() const override { return "*SET_IGA_FACE"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    std::vector<Data>& getData() { return data_; }
    const std::vector<Data>& getData() const { return data_; }

private:
    std::vector<Data> data_;
};

/**
 * @brief *SET_IGA_POINT_UVW
 *
 * IGA point set by parametric coordinates.
 */
class KOO_API SetIgaPointUvw : public CloneableKeyword<SetIgaPointUvw, SetKeyword> {
public:
    struct Data {
        int sid = 0;
        int pid = 0;
        double u = 0.0;
        double v = 0.0;
        double w = 0.0;
    };

    SetIgaPointUvw() = default;

    std::string getKeywordName() const override { return "*SET_IGA_POINT_UVW"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    std::vector<Data>& getData() { return data_; }
    const std::vector<Data>& getData() const { return data_; }

private:
    std::vector<Data> data_;
};

/**
 * @brief *SET_MODE
 *
 * Mode set for modal analysis.
 */
class KOO_API SetMode : public CloneableKeyword<SetMode, SetKeyword> {
public:
    SetMode() = default;

    std::string getKeywordName() const override { return "*SET_MODE"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    int getSetId() const { return sid_; }
    void setSetId(int id) { sid_ = id; }

    std::vector<int>& getModes() { return modes_; }
    const std::vector<int>& getModes() const { return modes_; }

private:
    int sid_ = 0;
    std::vector<int> modes_;
};

/**
 * @brief *SET_MULTI
 *
 * Multiple set definition.
 */
class KOO_API SetMulti : public CloneableKeyword<SetMulti, SetKeyword> {
public:
    struct Data {
        int sid = 0;
        int settype = 0;
        std::vector<int> sets;
    };

    SetMulti() = default;

    std::string getKeywordName() const override { return "*SET_MULTI"; }

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
 * @brief *SET_PART_COLUMN
 *
 * Part set from column.
 */
class KOO_API SetPartColumn : public CloneableKeyword<SetPartColumn, SetKeyword> {
public:
    struct Data {
        int sid = 0;
        int da1 = 0;
        int da2 = 0;
        int da3 = 0;
        int da4 = 0;
    };

    SetPartColumn() = default;

    std::string getKeywordName() const override { return "*SET_PART_COLUMN"; }

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
 * @brief *SET_PART_GENERAL
 *
 * General part set with options.
 */
class KOO_API SetPartGeneral : public CloneableKeyword<SetPartGeneral, SetKeyword> {
public:
    struct Data {
        int sid = 0;
        int option = 0;
        int da1 = 0;
        int da2 = 0;
    };

    SetPartGeneral() = default;

    std::string getKeywordName() const override { return "*SET_PART_GENERAL"; }

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
 * @brief *SET_PART_LIST_GENERATE
 *
 * Part list set with generation.
 */
class KOO_API SetPartListGenerate : public CloneableKeyword<SetPartListGenerate, SetKeyword> {
public:
    struct Range {
        PartId pid1 = 0;
        PartId pid2 = 0;
        int dpidp = 1;
    };

    SetPartListGenerate() = default;

    std::string getKeywordName() const override { return "*SET_PART_LIST_GENERATE"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    int getSetId() const { return sid_; }
    void setSetId(int id) { sid_ = id; }

    std::vector<Range>& getRanges() { return ranges_; }
    const std::vector<Range>& getRanges() const { return ranges_; }

private:
    int sid_ = 0;
    std::vector<Range> ranges_;
};

/**
 * @brief *SET_SEGMENT_ADD
 *
 * Add segments to existing set.
 */
class KOO_API SetSegmentAdd : public CloneableKeyword<SetSegmentAdd, SetKeyword> {
public:
    using Segment = SetSegment::Segment;

    SetSegmentAdd() = default;

    std::string getKeywordName() const override { return "*SET_SEGMENT_ADD"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    int getSetId() const { return sid_; }
    void setSetId(int id) { sid_ = id; }

    std::vector<Segment>& getSegments() { return segments_; }
    const std::vector<Segment>& getSegments() const { return segments_; }

private:
    int sid_ = 0;
    std::vector<Segment> segments_;
};

/**
 * @brief *SET_SEGMENT_INTERSECT
 *
 * Segment set from intersection of two sets.
 */
class KOO_API SetSegmentIntersect : public CloneableKeyword<SetSegmentIntersect, SetKeyword> {
public:
    struct Data {
        int sid = 0;
        int sid1 = 0;
        int sid2 = 0;
    };

    SetSegmentIntersect() = default;

    std::string getKeywordName() const override { return "*SET_SEGMENT_INTERSECT"; }

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
 * @brief *SET_SHELL_ADD
 *
 * Add shell elements to existing set.
 */
class KOO_API SetShellAdd : public CloneableKeyword<SetShellAdd, SetKeyword> {
public:
    SetShellAdd() = default;

    std::string getKeywordName() const override { return "*SET_SHELL_ADD"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    int getSetId() const { return sid_; }
    void setSetId(int id) { sid_ = id; }

    std::vector<ElementId>& getElements() { return elements_; }
    const std::vector<ElementId>& getElements() const { return elements_; }

private:
    int sid_ = 0;
    std::vector<ElementId> elements_;
};

/**
 * @brief *SET_SHELL_GENERAL
 *
 * General shell element set with options.
 */
class KOO_API SetShellGeneral : public CloneableKeyword<SetShellGeneral, SetKeyword> {
public:
    struct Data {
        int sid = 0;
        int option = 0;
        int da1 = 0;
        int da2 = 0;
    };

    SetShellGeneral() = default;

    std::string getKeywordName() const override { return "*SET_SHELL_GENERAL"; }

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
 * @brief *SET_SHELL_INTERSECT
 *
 * Shell set from intersection of two sets.
 */
class KOO_API SetShellIntersect : public CloneableKeyword<SetShellIntersect, SetKeyword> {
public:
    struct Data {
        int sid = 0;
        int sid1 = 0;
        int sid2 = 0;
    };

    SetShellIntersect() = default;

    std::string getKeywordName() const override { return "*SET_SHELL_INTERSECT"; }

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
 * @brief *SET_SOLID_ADD
 *
 * Add solid elements to existing set.
 */
class KOO_API SetSolidAdd : public CloneableKeyword<SetSolidAdd, SetKeyword> {
public:
    SetSolidAdd() = default;

    std::string getKeywordName() const override { return "*SET_SOLID_ADD"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    int getSetId() const { return sid_; }
    void setSetId(int id) { sid_ = id; }

    std::vector<ElementId>& getElements() { return elements_; }
    const std::vector<ElementId>& getElements() const { return elements_; }

private:
    int sid_ = 0;
    std::vector<ElementId> elements_;
};

/**
 * @brief *SET_SOLID_GENERAL
 *
 * General solid element set with options.
 */
class KOO_API SetSolidGeneral : public CloneableKeyword<SetSolidGeneral, SetKeyword> {
public:
    struct Data {
        int sid = 0;
        int option = 0;
        int da1 = 0;
        int da2 = 0;
    };

    SetSolidGeneral() = default;

    std::string getKeywordName() const override { return "*SET_SOLID_GENERAL"; }

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
 * @brief *SET_SOLID_INTERSECT
 *
 * Solid set from intersection of two sets.
 */
class KOO_API SetSolidIntersect : public CloneableKeyword<SetSolidIntersect, SetKeyword> {
public:
    struct Data {
        int sid = 0;
        int sid1 = 0;
        int sid2 = 0;
    };

    SetSolidIntersect() = default;

    std::string getKeywordName() const override { return "*SET_SOLID_INTERSECT"; }

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
 * @brief *SET_TSHELL_GENERAL
 *
 * General thick shell element set with options.
 */
class KOO_API SetTshellGeneral : public CloneableKeyword<SetTshellGeneral, SetKeyword> {
public:
    struct Data {
        int sid = 0;
        int option = 0;
        int da1 = 0;
        int da2 = 0;
    };

    SetTshellGeneral() = default;

    std::string getKeywordName() const override { return "*SET_TSHELL_GENERAL"; }

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
