#pragma once

#include <koo/Export.hpp>
#include <koo/mesh/MeshTypes.hpp>
#include <koo/common/Vec3.hpp>
#include <vector>
#include <map>
#include <sstream>

namespace koo::mesh {

/**
 * @brief Mesh data representation (before conversion to LS-DYNA)
 *
 * Contains nodes, elements, and geometric associations.
 * This is a library-neutral representation that can be converted
 * to different formats (LS-DYNA, Abaqus, etc.).
 */
class KOO_API MeshData {
public:
    /**
     * @brief Mesh node
     */
    struct Node {
        int id;                ///< Node ID
        common::Vec3 position; ///< Node coordinates

        Node() : id(-1), position() {}
        Node(int nodeId, const common::Vec3& pos)
            : id(nodeId), position(pos) {}
    };

    /**
     * @brief Mesh element
     */
    struct Element {
        int id;                        ///< Element ID
        ElementType type;              ///< Element type
        std::vector<int> nodeIds;      ///< Node connectivity
        int geometryEntityId;          ///< CAD entity this came from (-1 = unknown)

        Element()
            : id(-1)
            , type(ElementType::Unknown)
            , nodeIds()
            , geometryEntityId(-1)
        {}

        Element(int elemId, ElementType elemType, const std::vector<int>& nodes, int geomId = -1)
            : id(elemId)
            , type(elemType)
            , nodeIds(nodes)
            , geometryEntityId(geomId)
        {}
    };

    // ========================================================================
    // Construction
    // ========================================================================

    /**
     * @brief Default constructor
     */
    MeshData() = default;

    // ========================================================================
    // Data Access
    // ========================================================================

    /**
     * @brief Get all nodes
     */
    const std::vector<Node>& getNodes() const { return nodes_; }
    std::vector<Node>& getNodes() { return nodes_; }

    /**
     * @brief Get all elements
     */
    const std::vector<Element>& getElements() const { return elements_; }
    std::vector<Element>& getElements() { return elements_; }

    /**
     * @brief Get number of nodes
     */
    size_t getNumNodes() const { return nodes_.size(); }

    /**
     * @brief Get number of elements
     */
    size_t getNumElements() const { return elements_.size(); }

    // ========================================================================
    // Filtered Access
    // ========================================================================

    /**
     * @brief Get elements of specific type
     */
    std::vector<Element> getElementsByType(ElementType type) const {
        std::vector<Element> result;
        for (const auto& elem : elements_) {
            if (elem.type == type) {
                result.push_back(elem);
            }
        }
        return result;
    }

    /**
     * @brief Get 2D elements (surface mesh)
     */
    std::vector<Element> get2DElements() const {
        std::vector<Element> result;
        for (const auto& elem : elements_) {
            if (is2D(elem.type)) {
                result.push_back(elem);
            }
        }
        return result;
    }

    /**
     * @brief Get 3D elements (volume mesh)
     */
    std::vector<Element> get3DElements() const {
        std::vector<Element> result;
        for (const auto& elem : elements_) {
            if (is3D(elem.type)) {
                result.push_back(elem);
            }
        }
        return result;
    }

    // ========================================================================
    // Geometric Associations
    // ========================================================================

    /**
     * @brief Get elements belonging to a geometry entity
     * @param geomId Geometry entity ID
     * @return Vector of element IDs
     */
    std::vector<int> getElementsForGeometry(int geomId) const {
        auto it = geometryToElements_.find(geomId);
        if (it != geometryToElements_.end()) {
            return it->second;
        }
        return {};
    }

    /**
     * @brief Set geometry association map
     */
    void setGeometryAssociations(const std::map<int, std::vector<int>>& assoc) {
        geometryToElements_ = assoc;
    }

    /**
     * @brief Get geometry association map
     */
    const std::map<int, std::vector<int>>& getGeometryAssociations() const {
        return geometryToElements_;
    }

    // ========================================================================
    // Modification
    // ========================================================================

    /**
     * @brief Add a node
     */
    void addNode(const Node& node) {
        nodes_.push_back(node);
    }

    /**
     * @brief Add an element
     */
    void addElement(const Element& element) {
        elements_.push_back(element);

        // Update geometry association
        if (element.geometryEntityId >= 0) {
            geometryToElements_[element.geometryEntityId].push_back(element.id);
        }
    }

    /**
     * @brief Clear all data
     */
    void clear() {
        nodes_.clear();
        elements_.clear();
        geometryToElements_.clear();
    }

    /**
     * @brief Reserve space for nodes and elements
     */
    void reserve(size_t numNodes, size_t numElements) {
        nodes_.reserve(numNodes);
        elements_.reserve(numElements);
    }

    // ========================================================================
    // Statistics
    // ========================================================================

    /**
     * @brief Get mesh statistics summary
     */
    std::string getStatistics() const {
        std::ostringstream oss;

        oss << "Mesh Statistics\n";
        oss << "===============\n";
        oss << "Nodes:    " << nodes_.size() << "\n";
        oss << "Elements: " << elements_.size() << "\n";
        oss << "\n";

        // Count by element type
        std::map<ElementType, int> typeCounts;
        for (const auto& elem : elements_) {
            typeCounts[elem.type]++;
        }

        oss << "Element Types:\n";
        for (const auto& [type, count] : typeCounts) {
            oss << "  " << toString(type) << ": " << count << "\n";
        }

        return oss.str();
    }

private:
    std::vector<Node> nodes_;                      ///< All nodes
    std::vector<Element> elements_;                ///< All elements
    std::map<int, std::vector<int>> geometryToElements_;  ///< Geometry entity → elements
};

} // namespace koo::mesh
