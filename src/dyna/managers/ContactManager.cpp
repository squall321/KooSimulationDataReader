#include <koo/dyna/managers/ContactManager.hpp>
#include <koo/dyna/managers/PartManager.hpp>
#include <sstream>

namespace koo::dyna::managers {

// ============================================================================
// ContactParameters Presets
// ============================================================================

ContactManager::ContactParameters ContactManager::ContactParameters::getDefault() {
    ContactParameters params;
    params.fs = 0.2;        // Moderate static friction
    params.fd = 0.2;        // Moderate dynamic friction
    params.dc = 0.0;        // No decay
    params.vc = 0.0;        // No viscous friction
    params.vdc = 0.0;       // No viscous damping
    params.sfs = 1.0;       // Default slave penalty scale
    params.sfm = 1.0;       // Default master penalty scale
    params.sst = 0.0;       // Use element thickness
    params.mst = 0.0;       // Use element thickness
    params.sfst = 1.0;      // Default slave thickness scale
    params.sfmt = 1.0;      // Default master thickness scale
    params.bt = 0.0;        // Active from start
    params.dt = 1e20;       // Active until end
    params.sstyp = 0;       // Segment set
    params.mstyp = 0;       // Segment set
    return params;
}

ContactManager::ContactParameters ContactManager::ContactParameters::getAggressive() {
    ContactParameters params = getDefault();
    params.fs = 0.5;        // High static friction
    params.fd = 0.4;        // High dynamic friction
    params.sfs = 1.2;       // Increased slave penalty (tighter contact)
    params.sfm = 1.2;       // Increased master penalty
    return params;
}

ContactManager::ContactParameters ContactManager::ContactParameters::getSoft() {
    ContactParameters params = getDefault();
    params.fs = 0.1;        // Low static friction
    params.fd = 0.1;        // Low dynamic friction
    params.sfs = 0.5;       // Reduced slave penalty (softer contact)
    params.sfm = 0.5;       // Reduced master penalty
    params.vdc = 20.0;      // Add viscous damping for stability
    return params;
}

ContactManager::ContactParameters ContactManager::ContactParameters::getTied() {
    ContactParameters params;
    params.fs = 0.0;        // No friction (tied)
    params.fd = 0.0;
    params.dc = 0.0;
    params.vc = 0.0;
    params.vdc = 0.0;
    params.sfs = 1.0;
    params.sfm = 1.0;
    params.sst = 0.0;
    params.mst = 0.0;
    params.sfst = 1.0;
    params.sfmt = 1.0;
    params.bt = 0.0;
    params.dt = 1e20;
    params.sstyp = 0;
    params.mstyp = 0;
    return params;
}

ContactManager::ContactParameters ContactManager::ContactParameters::getForming() {
    ContactParameters params = getDefault();
    params.fs = 0.12;       // Typical forming friction
    params.fd = 0.12;
    params.dc = 0.0;
    params.vc = 0.0;
    params.vdc = 0.0;
    params.sfs = 1.0;
    params.sfm = 1.0;
    params.sstyp = 0;
    params.mstyp = 0;
    return params;
}

// ============================================================================
// Constructor
// ============================================================================

ContactManager::ContactManager(Model& model, SetManager& setMgr)
    : model_(model)
    , setManager_(setMgr)
{
}

// ============================================================================
// High-Level Contact Creation
// ============================================================================

int ContactManager::createPartBasedContact(
    PartId slavePid,
    PartId masterPid,
    const ContactParameters& params,
    const std::string& title)
{
    // Create segment sets from part surfaces
    std::ostringstream slaveTitleOss, masterTitleOss;
    slaveTitleOss << "Part" << slavePid << "_Surface_Slave";
    masterTitleOss << "Part" << masterPid << "_Surface_Master";

    int slaveSetId = setManager_.createSegmentSetFromPartSurface(slavePid, slaveTitleOss.str());
    int masterSetId = setManager_.createSegmentSetFromPartSurface(masterPid, masterTitleOss.str());

    // Generate title if not provided
    std::string contactTitle = title;
    if (contactTitle.empty()) {
        std::ostringstream oss;
        oss << "Contact_Part" << slavePid << "_to_Part" << masterPid;
        contactTitle = oss.str();
    }

    // Create contact
    return createContact(slaveSetId, masterSetId, ContactType::AutomaticSurfaceToSurface, params, contactTitle);
}

int ContactManager::createSelfContact(
    PartId pid,
    const ContactParameters& params,
    const std::string& title)
{
    // Create segment set from part surface
    std::ostringstream setTitleOss;
    setTitleOss << "Part" << pid << "_Surface_Self";
    int setId = setManager_.createSegmentSetFromPartSurface(pid, setTitleOss.str());

    // Generate title if not provided
    std::string contactTitle = title;
    if (contactTitle.empty()) {
        std::ostringstream oss;
        oss << "SelfContact_Part" << pid;
        contactTitle = oss.str();
    }

    // Create self-contact (slave and master are the same)
    return createContact(setId, setId, ContactType::AutomaticSingleSurface, params, contactTitle);
}

int ContactManager::createTiedContact(
    PartId slavePid,
    PartId masterPid,
    const ContactParameters& params,
    const std::string& title)
{
    // Create segment sets from part surfaces
    std::ostringstream slaveTitleOss, masterTitleOss;
    slaveTitleOss << "Part" << slavePid << "_Surface_TiedSlave";
    masterTitleOss << "Part" << masterPid << "_Surface_TiedMaster";

    int slaveSetId = setManager_.createSegmentSetFromPartSurface(slavePid, slaveTitleOss.str());
    int masterSetId = setManager_.createSegmentSetFromPartSurface(masterPid, masterTitleOss.str());

    // Generate title if not provided
    std::string contactTitle = title;
    if (contactTitle.empty()) {
        std::ostringstream oss;
        oss << "TiedContact_Part" << slavePid << "_to_Part" << masterPid;
        contactTitle = oss.str();
    }

    // Create tied contact
    return createContact(slaveSetId, masterSetId, ContactType::TiedSurfaceToSurface, params, contactTitle);
}

int ContactManager::createErodingContact(
    PartId slavePid,
    PartId masterPid,
    const ContactParameters& params,
    const std::string& title)
{
    // Create segment sets from part surfaces
    std::ostringstream slaveTitleOss, masterTitleOss;
    slaveTitleOss << "Part" << slavePid << "_Surface_ErodeSlave";
    masterTitleOss << "Part" << masterPid << "_Surface_ErodeMaster";

    int slaveSetId = setManager_.createSegmentSetFromPartSurface(slavePid, slaveTitleOss.str());
    int masterSetId = setManager_.createSegmentSetFromPartSurface(masterPid, masterTitleOss.str());

    // Generate title if not provided
    std::string contactTitle = title;
    if (contactTitle.empty()) {
        std::ostringstream oss;
        oss << "ErodingContact_Part" << slavePid << "_to_Part" << masterPid;
        contactTitle = oss.str();
    }

    // Create eroding contact
    return createContact(slaveSetId, masterSetId, ContactType::ErodingSurfaceToSurface, params, contactTitle);
}

int ContactManager::createFormingContact(
    PartId slavePid,
    PartId masterPid,
    const ContactParameters& params,
    const std::string& title)
{
    // Create segment sets from part surfaces
    std::ostringstream slaveTitleOss, masterTitleOss;
    slaveTitleOss << "Part" << slavePid << "_Surface_Blank";
    masterTitleOss << "Part" << masterPid << "_Surface_Tool";

    int slaveSetId = setManager_.createSegmentSetFromPartSurface(slavePid, slaveTitleOss.str());
    int masterSetId = setManager_.createSegmentSetFromPartSurface(masterPid, masterTitleOss.str());

    // Generate title if not provided
    std::string contactTitle = title;
    if (contactTitle.empty()) {
        std::ostringstream oss;
        oss << "FormingContact_Part" << slavePid << "_to_Part" << masterPid;
        contactTitle = oss.str();
    }

    // Create forming contact
    return createContact(slaveSetId, masterSetId, ContactType::FormingOneWay, params, contactTitle);
}

int ContactManager::createNodesToSurfaceContact(
    int slaveNodeSetId,
    int masterSegmentSetId,
    const ContactParameters& params,
    const std::string& title)
{
    // Generate title if not provided
    std::string contactTitle = title;
    if (contactTitle.empty()) {
        std::ostringstream oss;
        oss << "Contact_NodeSet" << slaveNodeSetId << "_to_SegSet" << masterSegmentSetId;
        contactTitle = oss.str();
    }

    // Create nodes-to-surface contact
    return createContact(slaveNodeSetId, masterSegmentSetId, ContactType::AutomaticNodesToSurface, params, contactTitle);
}

int ContactManager::createContact(
    int slaveSetId,
    int masterSetId,
    ContactType type,
    const ContactParameters& params,
    const std::string& title)
{
    int contactId = getNextContactId();

    // Create appropriate contact keyword based on type
    switch (type) {
        case ContactType::AutomaticSingleSurface: {
            auto keyword = std::make_unique<ContactAutomaticSingleSurface>();
            auto& card1 = keyword->getCard1();
            auto& card2 = keyword->getCard2();
            auto& card3 = keyword->getCard3();

            applyCard1Parameters(card1, slaveSetId, slaveSetId, params);  // Single surface: slave == master
            applyCard2Parameters(card2, params);
            applyCard3Parameters(card3, params);

            model_.addKeyword(std::move(keyword));
            break;
        }

        case ContactType::AutomaticSurfaceToSurface: {
            auto keyword = std::make_unique<ContactAutomaticSurfaceToSurface>();
            auto& card1 = keyword->getCard1();
            auto& card2 = keyword->getCard2();
            auto& card3 = keyword->getCard3();

            applyCard1Parameters(card1, slaveSetId, masterSetId, params);
            applyCard2Parameters(card2, params);
            applyCard3Parameters(card3, params);

            model_.addKeyword(std::move(keyword));
            break;
        }

        case ContactType::AutomaticNodesToSurface: {
            auto keyword = std::make_unique<ContactAutomaticNodesToSurface>();
            auto& card1 = keyword->getCard1();
            auto& card2 = keyword->getCard2();
            auto& card3 = keyword->getCard3();

            applyCard1Parameters(card1, slaveSetId, masterSetId, params);
            applyCard2Parameters(card2, params);
            applyCard3Parameters(card3, params);

            model_.addKeyword(std::move(keyword));
            break;
        }

        case ContactType::TiedSurfaceToSurface: {
            auto keyword = std::make_unique<ContactTiedSurfaceToSurface>();
            auto& card1 = keyword->getCard1();
            auto& card2 = keyword->getCard2();

            applyCard1Parameters(card1, slaveSetId, masterSetId, params);
            applyCard2Parameters(card2, params);

            model_.addKeyword(std::move(keyword));
            break;
        }

        case ContactType::TiedNodesToSurface: {
            auto keyword = std::make_unique<ContactTiedNodesToSurface>();
            auto& card1 = keyword->getCard1();
            auto& card2 = keyword->getCard2();

            applyCard1Parameters(card1, slaveSetId, masterSetId, params);
            applyCard2Parameters(card2, params);

            model_.addKeyword(std::move(keyword));
            break;
        }

        case ContactType::ErodingSingleSurface: {
            auto keyword = std::make_unique<ContactErodingSingleSurface>();
            auto& card1 = keyword->getCard1();
            auto& card2 = keyword->getCard2();
            auto& card3 = keyword->getCard3();

            applyCard1Parameters(card1, slaveSetId, slaveSetId, params);
            applyCard2Parameters(card2, params);
            applyCard3Parameters(card3, params);

            model_.addKeyword(std::move(keyword));
            break;
        }

        case ContactType::ErodingSurfaceToSurface: {
            auto keyword = std::make_unique<ContactErodingSurfaceToSurface>();
            auto& card1 = keyword->getCard1();
            auto& card2 = keyword->getCard2();
            auto& card3 = keyword->getCard3();

            applyCard1Parameters(card1, slaveSetId, masterSetId, params);
            applyCard2Parameters(card2, params);
            applyCard3Parameters(card3, params);

            model_.addKeyword(std::move(keyword));
            break;
        }

        case ContactType::FormingOneWay: {
            auto keyword = std::make_unique<ContactFormingOneWaySurfaceToSurface>();
            auto& card1 = keyword->getCard1();
            auto& card2 = keyword->getCard2();
            auto& card3 = keyword->getCard3();

            applyCard1Parameters(card1, slaveSetId, masterSetId, params);
            applyCard2Parameters(card2, params);
            applyCard3Parameters(card3, params);

            model_.addKeyword(std::move(keyword));
            break;
        }
    }

    return contactId;
}

// ============================================================================
// Contact Query and Modification
// ============================================================================

std::vector<const ContactKeyword*> ContactManager::getAllContacts() const {
    std::vector<const ContactKeyword*> result;

    // Get all contact keywords from model
    auto keywords = model_.getKeywordsOfType<ContactKeyword>();
    result.insert(result.end(), keywords.begin(), keywords.end());

    return result;
}

size_t ContactManager::getContactCount() const {
    return getAllContacts().size();
}

const ContactKeyword* ContactManager::getContact(int contactId) const {
    // For ID-based contacts, we'd need to search through ContactAutomaticSurfaceToSurfaceId etc.
    // This is a simplified implementation - full implementation would check ID cards

    auto allContacts = getAllContacts();
    if (contactId > 0 && static_cast<size_t>(contactId) <= allContacts.size()) {
        return allContacts[contactId - 1];
    }
    return nullptr;
}

bool ContactManager::removeContact(int contactId) {
    // This would require Model::removeKeyword() which might not exist
    // Placeholder implementation
    (void)contactId;
    return false;
}

// ============================================================================
// Batch Operations
// ============================================================================

int ContactManager::createAllSelfContacts(const ContactParameters& params) {
    int count = 0;

    // Get all parts from SetManager's parent PartManager
    // We'd need access to PartManager for this
    // Placeholder: would need to be passed in constructor or added as member

    return count;
}

int ContactManager::createAllToAllContacts(const ContactParameters& params) {
    int count = 0;

    // Get all parts
    // Create pairwise contacts
    // Placeholder

    return count;
}

int ContactManager::createMultiPartContact(
    const std::vector<PartId>& partIds,
    const ContactParameters& params)
{
    int count = 0;

    // Create pairwise contacts between all parts
    for (size_t i = 0; i < partIds.size(); ++i) {
        for (size_t j = i + 1; j < partIds.size(); ++j) {
            createPartBasedContact(partIds[i], partIds[j], params);
            count++;
        }
    }

    return count;
}

// ============================================================================
// Helper Methods
// ============================================================================

int ContactManager::getNextContactId() {
    return nextContactId_++;
}

void ContactManager::applyCard1Parameters(
    ContactAutomaticSingleSurface::Card1& card,
    int slaveSetId,
    int masterSetId,
    const ContactParameters& params)
{
    card.ssid = slaveSetId;
    card.msid = masterSetId;
    card.sstyp = params.sstyp;
    card.mstyp = params.mstyp;
    card.sboxid = 0;
    card.mboxid = 0;
    card.spr = 0;
    card.mpr = 0;
}

void ContactManager::applyCard2Parameters(
    ContactAutomaticSingleSurface::Card2& card,
    const ContactParameters& params)
{
    card.fs = params.fs;
    card.fd = params.fd;
    card.dc = params.dc;
    card.vc = params.vc;
    card.vdc = params.vdc;
    card.penchk = 0;
    card.bt = params.bt;
    card.dt = params.dt;
}

void ContactManager::applyCard3Parameters(
    ContactAutomaticSingleSurface::Card3& card,
    const ContactParameters& params)
{
    card.sfs = params.sfs;
    card.sfm = params.sfm;
    card.sst = params.sst;
    card.mst = params.mst;
    card.sfst = params.sfst;
    card.sfmt = params.sfmt;
    card.fsf = 1.0;
    card.vsf = 1.0;
}

} // namespace koo::dyna::managers
