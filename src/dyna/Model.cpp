#include <koo/dyna/Model.hpp>

namespace koo::dyna {

Model::Model(const Model& other)
    : title_(other.title_)
    , filePath_(other.filePath_) {
    keywords_.reserve(other.keywords_.size());
    for (const auto& kw : other.keywords_) {
        keywords_.push_back(kw->clone());
    }
}

Model& Model::operator=(const Model& other) {
    if (this != &other) {
        title_ = other.title_;
        filePath_ = other.filePath_;
        keywords_.clear();
        keywords_.reserve(other.keywords_.size());
        for (const auto& kw : other.keywords_) {
            keywords_.push_back(kw->clone());
        }
        invalidateCache();
    }
    return *this;
}

std::unique_ptr<Model> Model::clone() const {
    return std::make_unique<Model>(*this);
}

void Model::addKeyword(std::unique_ptr<Keyword> keyword) {
    keywords_.push_back(std::move(keyword));
    invalidateCache();
}

void Model::invalidateCache() const {
    cachedNodes_ = nullptr;
    cachedShells_ = nullptr;
    cachedSolids_ = nullptr;
    cachedParts_ = nullptr;
}

void Model::updateCache() const {
    for (auto& kw : keywords_) {
        if (!cachedNodes_) {
            cachedNodes_ = dynamic_cast<Node*>(kw.get());
        }
        if (!cachedShells_) {
            cachedShells_ = dynamic_cast<ElementShell*>(kw.get());
        }
        if (!cachedSolids_) {
            cachedSolids_ = dynamic_cast<ElementSolid*>(kw.get());
        }
        if (!cachedParts_) {
            cachedParts_ = dynamic_cast<Part*>(kw.get());
        }
    }
}

// Nodes
Node* Model::getNodes() {
    updateCache();
    return cachedNodes_;
}

const Node* Model::getNodes() const {
    updateCache();
    return cachedNodes_;
}

Node& Model::getOrCreateNodes() {
    updateCache();
    if (!cachedNodes_) {
        auto nodes = std::make_unique<Node>();
        cachedNodes_ = nodes.get();
        keywords_.push_back(std::move(nodes));
    }
    return *cachedNodes_;
}

size_t Model::getNodeCount() const {
    const Node* nodes = getNodes();
    return nodes ? nodes->getNodeCount() : 0;
}

NodeData* Model::findNode(NodeId id) {
    Node* nodes = getNodes();
    return nodes ? nodes->findNode(id) : nullptr;
}

const NodeData* Model::findNode(NodeId id) const {
    const Node* nodes = getNodes();
    return nodes ? nodes->findNode(id) : nullptr;
}

// Shell elements
ElementShell* Model::getShellElements() {
    updateCache();
    return cachedShells_;
}

const ElementShell* Model::getShellElements() const {
    updateCache();
    return cachedShells_;
}

ElementShell& Model::getOrCreateShellElements() {
    updateCache();
    if (!cachedShells_) {
        auto shells = std::make_unique<ElementShell>();
        cachedShells_ = shells.get();
        keywords_.push_back(std::move(shells));
    }
    return *cachedShells_;
}

size_t Model::getShellElementCount() const {
    const ElementShell* shells = getShellElements();
    return shells ? shells->getElementCount() : 0;
}

// Solid elements
ElementSolid* Model::getSolidElements() {
    updateCache();
    return cachedSolids_;
}

const ElementSolid* Model::getSolidElements() const {
    updateCache();
    return cachedSolids_;
}

ElementSolid& Model::getOrCreateSolidElements() {
    updateCache();
    if (!cachedSolids_) {
        auto solids = std::make_unique<ElementSolid>();
        cachedSolids_ = solids.get();
        keywords_.push_back(std::move(solids));
    }
    return *cachedSolids_;
}

size_t Model::getSolidElementCount() const {
    const ElementSolid* solids = getSolidElements();
    return solids ? solids->getElementCount() : 0;
}

// Parts
Part* Model::getParts() {
    updateCache();
    return cachedParts_;
}

const Part* Model::getParts() const {
    updateCache();
    return cachedParts_;
}

Part& Model::getOrCreateParts() {
    updateCache();
    if (!cachedParts_) {
        auto parts = std::make_unique<Part>();
        cachedParts_ = parts.get();
        keywords_.push_back(std::move(parts));
    }
    return *cachedParts_;
}

size_t Model::getPartCount() const {
    const Part* parts = getParts();
    return parts ? parts->getPartCount() : 0;
}

PartData* Model::findPart(PartId id) {
    Part* parts = getParts();
    return parts ? parts->getPart(id) : nullptr;
}

const PartData* Model::findPart(PartId id) const {
    const Part* parts = getParts();
    return parts ? parts->getPart(id) : nullptr;
}

// Materials
std::vector<MaterialBase*> Model::getMaterials() {
    std::vector<MaterialBase*> result;
    for (auto& kw : keywords_) {
        if (auto* mat = dynamic_cast<MaterialBase*>(kw.get())) {
            result.push_back(mat);
        }
    }
    return result;
}

std::vector<const MaterialBase*> Model::getMaterials() const {
    std::vector<const MaterialBase*> result;
    for (const auto& kw : keywords_) {
        if (const auto* mat = dynamic_cast<const MaterialBase*>(kw.get())) {
            result.push_back(mat);
        }
    }
    return result;
}

MaterialBase* Model::findMaterial(MaterialId id) {
    for (auto& kw : keywords_) {
        if (auto* mat = dynamic_cast<MaterialBase*>(kw.get())) {
            if (mat->getMaterialId() == id) {
                return mat;
            }
        }
    }
    return nullptr;
}

const MaterialBase* Model::findMaterial(MaterialId id) const {
    for (const auto& kw : keywords_) {
        if (const auto* mat = dynamic_cast<const MaterialBase*>(kw.get())) {
            if (mat->getMaterialId() == id) {
                return mat;
            }
        }
    }
    return nullptr;
}

// Sections
std::vector<SectionBase*> Model::getSections() {
    std::vector<SectionBase*> result;
    for (auto& kw : keywords_) {
        if (auto* sec = dynamic_cast<SectionBase*>(kw.get())) {
            result.push_back(sec);
        }
    }
    return result;
}

std::vector<const SectionBase*> Model::getSections() const {
    std::vector<const SectionBase*> result;
    for (const auto& kw : keywords_) {
        if (const auto* sec = dynamic_cast<const SectionBase*>(kw.get())) {
            result.push_back(sec);
        }
    }
    return result;
}

SectionBase* Model::findSection(SectionId id) {
    for (auto& kw : keywords_) {
        if (auto* sec = dynamic_cast<SectionBase*>(kw.get())) {
            if (sec->getSectionId() == id) {
                return sec;
            }
        }
    }
    return nullptr;
}

const SectionBase* Model::findSection(SectionId id) const {
    for (const auto& kw : keywords_) {
        if (const auto* sec = dynamic_cast<const SectionBase*>(kw.get())) {
            if (sec->getSectionId() == id) {
                return sec;
            }
        }
    }
    return nullptr;
}

// Statistics
BoundingBox Model::getBoundingBox() const {
    const Node* nodes = getNodes();
    if (nodes) {
        return nodes->getBoundingBox();
    }
    return BoundingBox();
}

size_t Model::getTotalElementCount() const {
    return getShellElementCount() + getSolidElementCount();
}

void Model::clear() {
    title_.clear();
    filePath_.clear();
    keywords_.clear();
    invalidateCache();
}

void Model::accept(ModelVisitor& visitor) {
    for (auto& kw : keywords_) {
        kw->accept(visitor);
    }
}

} // namespace koo::dyna
