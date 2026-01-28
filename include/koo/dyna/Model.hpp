#pragma once

#include <koo/Export.hpp>
#include <koo/dyna/Keyword.hpp>
#include <koo/dyna/ModelVisitor.hpp>
#include <koo/dyna/Node.hpp>
#include <koo/dyna/Element.hpp>
#include <koo/dyna/Part.hpp>
#include <koo/dyna/Material.hpp>
#include <koo/dyna/Section.hpp>
#include <koo/util/Types.hpp>
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <filesystem>

namespace koo::dyna {

/**
 * @brief Container for all model data
 *
 * Holds all keywords from a K-file and provides
 * convenient access to common data types.
 */
class KOO_API Model {
public:
    Model() = default;
    Model(const Model& other);
    Model(Model&& other) noexcept = default;
    Model& operator=(const Model& other);
    Model& operator=(Model&& other) noexcept = default;
    ~Model() = default;

    // Deep copy
    std::unique_ptr<Model> clone() const;

    // Title
    void setTitle(const std::string& title) { title_ = title; }
    const std::string& getTitle() const { return title_; }

    // Source file
    void setFilePath(const std::filesystem::path& path) { filePath_ = path; }
    const std::filesystem::path& getFilePath() const { return filePath_; }

    // Add keywords
    void addKeyword(std::unique_ptr<Keyword> keyword);

    // Get all keywords
    const std::vector<std::unique_ptr<Keyword>>& getKeywords() const { return keywords_; }
    std::vector<std::unique_ptr<Keyword>>& getKeywords() { return keywords_; }

    // Get keywords by type
    template<typename T>
    std::vector<T*> getKeywordsOfType() {
        std::vector<T*> result;
        for (auto& kw : keywords_) {
            if (auto* typed = dynamic_cast<T*>(kw.get())) {
                result.push_back(typed);
            }
        }
        return result;
    }

    template<typename T>
    std::vector<const T*> getKeywordsOfType() const {
        std::vector<const T*> result;
        for (const auto& kw : keywords_) {
            if (const auto* typed = dynamic_cast<const T*>(kw.get())) {
                result.push_back(typed);
            }
        }
        return result;
    }

    // Convenience access to common data

    // Nodes
    Node* getNodes();
    const Node* getNodes() const;
    Node& getOrCreateNodes();
    size_t getNodeCount() const;
    NodeData* findNode(NodeId id);
    const NodeData* findNode(NodeId id) const;

    // Shell elements
    ElementShell* getShellElements();
    const ElementShell* getShellElements() const;
    ElementShell& getOrCreateShellElements();
    size_t getShellElementCount() const;

    // Solid elements
    ElementSolid* getSolidElements();
    const ElementSolid* getSolidElements() const;
    ElementSolid& getOrCreateSolidElements();
    size_t getSolidElementCount() const;

    // Parts
    Part* getParts();
    const Part* getParts() const;
    Part& getOrCreateParts();
    size_t getPartCount() const;
    PartData* findPart(PartId id);
    const PartData* findPart(PartId id) const;

    // Materials
    std::vector<MaterialBase*> getMaterials();
    std::vector<const MaterialBase*> getMaterials() const;
    MaterialBase* findMaterial(MaterialId id);
    const MaterialBase* findMaterial(MaterialId id) const;

    // Sections
    std::vector<SectionBase*> getSections();
    std::vector<const SectionBase*> getSections() const;
    SectionBase* findSection(SectionId id);
    const SectionBase* findSection(SectionId id) const;

    // Statistics
    BoundingBox getBoundingBox() const;
    size_t getTotalElementCount() const;

    // Clear all data
    void clear();

    // Visitor pattern
    void accept(ModelVisitor& visitor);

private:
    std::string title_;
    std::filesystem::path filePath_;
    std::vector<std::unique_ptr<Keyword>> keywords_;

    // Cached pointers for quick access (not owned)
    mutable Node* cachedNodes_ = nullptr;
    mutable ElementShell* cachedShells_ = nullptr;
    mutable ElementSolid* cachedSolids_ = nullptr;
    mutable Part* cachedParts_ = nullptr;

    void invalidateCache() const;
    void updateCache() const;
};

} // namespace koo::dyna
