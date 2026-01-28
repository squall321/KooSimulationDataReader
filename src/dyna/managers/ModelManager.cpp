#include <koo/dyna/managers/ModelManager.hpp>
#include <sstream>
#include <iostream>

namespace koo::dyna::managers {

// ============================================================================
// Constructor
// ============================================================================

ModelManager::ModelManager(Model& model, bool autoBuildIndices)
    : model_(model)
{
    // Create all sub-managers
    partManager_ = std::make_unique<PartManager>(model);
    elementManager_ = std::make_unique<ElementManager>(model);
    nodeManager_ = std::make_unique<NodeManager>(model);
    setManager_ = std::make_unique<SetManager>(model, *partManager_, *elementManager_);
    contactManager_ = std::make_unique<ContactManager>(model, *setManager_);
    loadManager_ = std::make_unique<LoadManager>(model, *setManager_);

    // Build indices if requested
    if (autoBuildIndices) {
        buildIndices();
    }
}

// ============================================================================
// Index Management
// ============================================================================

void ModelManager::buildIndices() {
    if (!indicesBuilt_) {
        partManager_->buildIndex();
        elementManager_->buildIndex();
        nodeManager_->buildIndex();
        indicesBuilt_ = true;
    }
}

void ModelManager::rebuildIndices() {
    partManager_->clearIndex();
    elementManager_->clearIndex();
    nodeManager_->clearIndex();

    partManager_->buildIndex();
    elementManager_->buildIndex();
    nodeManager_->buildIndex();

    indicesBuilt_ = true;
}

bool ModelManager::hasIndices() const {
    return indicesBuilt_;
}

// ============================================================================
// Workflow Automation - Crash Simulations
// ============================================================================

int ModelManager::setupCrashSimulation(
    const std::vector<PartId>& vehicleParts,
    const std::vector<PartId>& barrierParts,
    bool enableGravity,
    double gravityMagnitude)
{
    int contactCount = 0;

    // Create vehicle-to-barrier contacts
    for (PartId vehiclePid : vehicleParts) {
        for (PartId barrierPid : barrierParts) {
            contactManager_->createPartBasedContact(vehiclePid, barrierPid);
            contactCount++;
        }
    }

    // Create self-contact for vehicle parts
    for (PartId vehiclePid : vehicleParts) {
        contactManager_->createSelfContact(vehiclePid);
        contactCount++;
    }

    // Create self-contact for barrier parts
    for (PartId barrierPid : barrierParts) {
        contactManager_->createSelfContact(barrierPid);
        contactCount++;
    }

    // Apply gravity if enabled
    if (enableGravity) {
        loadManager_->applyGravity(gravityMagnitude);
    }

    return contactCount;
}

int ModelManager::setupCrashSimulation(
    PartId vehiclePid,
    PartId barrierPid,
    bool enableGravity,
    double gravityMagnitude)
{
    return setupCrashSimulation({vehiclePid}, {barrierPid}, enableGravity, gravityMagnitude);
}

// ============================================================================
// Workflow Automation - Forming Simulations
// ============================================================================

int ModelManager::setupFormingSimulation(
    PartId blankPid,
    PartId diePid,
    PartId punchPid,
    PartId blankHolderPid,
    double blankHolderForce)
{
    int contactCount = 0;
    auto formingParams = ContactManager::ContactParameters::getForming();

    // Create forming contacts
    contactManager_->createFormingContact(blankPid, diePid, formingParams);
    contactCount++;

    contactManager_->createFormingContact(blankPid, punchPid, formingParams);
    contactCount++;

    if (blankHolderPid > 0) {
        contactManager_->createFormingContact(blankPid, blankHolderPid, formingParams);
        contactCount++;

        // Apply blank holder force
        loadManager_->applyPressureToPart(blankHolderPid, blankHolderForce);
    }

    return contactCount;
}

// ============================================================================
// Workflow Automation - Pressure Vessel
// ============================================================================

int ModelManager::setupPressureVessel(
    PartId vesselPid,
    double internalPressure,
    bool enableGravity,
    double gravityMagnitude,
    bool usePulseLoad,
    double pulseRiseTime,
    double pulseHoldTime,
    double pulseFallTime)
{
    // Apply internal pressure
    if (usePulseLoad) {
        // Create pulse load curve
        int lcid = loadManager_->createPulseLoadCurve(
            pulseRiseTime,
            pulseHoldTime,
            pulseFallTime,
            "PressureVessel_Pulse"
        );

        auto pulseParams = LoadManager::LoadParameters::timeVarying(lcid, internalPressure);
        loadManager_->applyPressureToPart(vesselPid, 1.0, pulseParams);
    } else {
        // Constant pressure
        loadManager_->applyPressureToPart(vesselPid, internalPressure);
    }

    // Apply gravity if enabled
    if (enableGravity) {
        loadManager_->applyGravity(gravityMagnitude);
    }

    return 1;
}

// ============================================================================
// Workflow Automation - Multi-Part Contact
// ============================================================================

int ModelManager::createAllPairwiseContacts(
    const std::vector<PartId>& partIds,
    const ContactManager::ContactParameters& params)
{
    int contactCount = 0;

    // Create pairwise contacts
    for (size_t i = 0; i < partIds.size(); ++i) {
        for (size_t j = i + 1; j < partIds.size(); ++j) {
            contactManager_->createPartBasedContact(partIds[i], partIds[j], params);
            contactCount++;
        }
    }

    return contactCount;
}

int ModelManager::createAllSelfContacts(
    const std::vector<PartId>& partIds,
    const ContactManager::ContactParameters& params)
{
    int contactCount = 0;

    for (PartId pid : partIds) {
        contactManager_->createSelfContact(pid, params);
        contactCount++;
    }

    return contactCount;
}

// ============================================================================
// Workflow Automation - Tied Connections
// ============================================================================

int ModelManager::createTiedConnections(
    const std::vector<std::pair<PartId, PartId>>& partPairs,
    const ContactManager::ContactParameters& params)
{
    int contactCount = 0;

    for (const auto& pair : partPairs) {
        contactManager_->createTiedContact(pair.first, pair.second, params);
        contactCount++;
    }

    return contactCount;
}

// ============================================================================
// High-Level Query Operations
// ============================================================================

std::string ModelManager::getModelSummary() const {
    std::ostringstream oss;

    oss << "========================================\n";
    oss << "Model Summary\n";
    oss << "========================================\n\n";

    // Part information
    auto allPartIds = partManager_->getAllPartIds();
    oss << "Parts: " << allPartIds.size() << "\n";

    // Element information
    size_t shellCount = elementManager_->getShellElements().size();
    size_t solidCount = elementManager_->getSolidElements().size();
    size_t beamCount = elementManager_->getBeamElements().size();
    size_t discreteCount = elementManager_->getDiscreteElements().size();
    size_t totalElements = elementManager_->getElementCount();

    oss << "Elements: " << totalElements << "\n";
    oss << "  Shell:    " << shellCount << "\n";
    oss << "  Solid:    " << solidCount << "\n";
    oss << "  Beam:     " << beamCount << "\n";
    oss << "  Discrete: " << discreteCount << "\n";

    // Node information
    size_t nodeCount = nodeManager_->getNodeCount();
    oss << "Nodes: " << nodeCount << "\n\n";

    // Contact information
    size_t contactCount = contactManager_->getContactCount();
    oss << "Contacts: " << contactCount << "\n";

    // Load information
    size_t loadCount = loadManager_->getLoadCount();
    oss << "Loads: " << loadCount << "\n";

    oss << "========================================\n";

    return oss.str();
}

void ModelManager::printModelSummary() const {
    std::cout << getModelSummary();
}

std::vector<PartId> ModelManager::getAllPartIds() const {
    return partManager_->getAllPartIds();
}

size_t ModelManager::getElementCount() const {
    return elementManager_->getElementCount();
}

size_t ModelManager::getNodeCount() const {
    return nodeManager_->getNodeCount();
}

size_t ModelManager::getContactCount() const {
    return contactManager_->getContactCount();
}

size_t ModelManager::getLoadCount() const {
    return loadManager_->getLoadCount();
}

} // namespace koo::dyna::managers
