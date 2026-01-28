#include <koo/dyna/managers/LoadManager.hpp>
#include <koo/dyna/managers/PartManager.hpp>
#include <koo/dyna/Curve.hpp>
#include <koo/dyna/Define.hpp>
#include <sstream>

namespace koo::dyna::managers {

// ============================================================================
// LoadParameters Presets
// ============================================================================

LoadManager::LoadParameters LoadManager::LoadParameters::constant(double mag) {
    LoadParameters params;
    params.magnitude = mag;
    params.lcid = 0;            // 0 = constant load
    params.scaleFactor = 1.0;
    params.activationTime = 0.0;
    params.direction = {0, 0, 0};
    return params;
}

LoadManager::LoadParameters LoadManager::LoadParameters::timeVarying(int lcid, double sf) {
    LoadParameters params;
    params.magnitude = 1.0;     // Magnitude from curve
    params.lcid = lcid;
    params.scaleFactor = sf;
    params.activationTime = 0.0;
    params.direction = {0, 0, 0};
    return params;
}

// ============================================================================
// Constructor
// ============================================================================

LoadManager::LoadManager(Model& model, SetManager& setMgr)
    : model_(model)
    , setManager_(setMgr)
{
    scanExistingLoadCurves();
}

void LoadManager::scanExistingLoadCurves() {
    // Scan existing load curves to avoid ID conflicts
    auto curves = model_.getKeywordsOfType<DefineCurve>();
    for (const auto* curve : curves) {
        int lcid = curve->getCurveId();
        if (lcid >= nextLoadCurveId_) {
            nextLoadCurveId_ = lcid + 1;
        }
    }

    auto curvesTitled = model_.getKeywordsOfType<DefineCurveTitle>();
    for (const auto* curve : curvesTitled) {
        int lcid = curve->getCurveId();
        if (lcid >= nextLoadCurveId_) {
            nextLoadCurveId_ = lcid + 1;
        }
    }
}

// ============================================================================
// High-Level Load Creation
// ============================================================================

int LoadManager::applyPressureToPart(
    PartId pid,
    double pressure,
    const LoadParameters& params)
{
    // Create segment set from part's external surface
    std::ostringstream setTitleOss;
    setTitleOss << "Part" << pid << "_Pressure_Surface";
    int segmentSetId = setManager_.createSegmentSetFromPartSurface(pid, setTitleOss.str());

    // Apply pressure to segment set
    return applyPressureToSegmentSet(segmentSetId, pressure, params);
}

int LoadManager::applyPressureToSegmentSet(
    int segmentSetId,
    double pressure,
    const LoadParameters& params)
{
    auto keyword = std::make_unique<LoadSegmentSet>();

    LoadSegmentSet::Data loadData;
    loadData.ssid = segmentSetId;
    loadData.lcid = params.lcid;
    loadData.sf = pressure * params.scaleFactor;
    loadData.at = params.activationTime;

    keyword->addLoad(loadData);
    model_.addKeyword(std::move(keyword));

    return 1;  // Return number of loads created
}

int LoadManager::applyForceToNodes(
    const std::vector<NodeId>& nodeIds,
    const Vec3& force,
    const LoadParameters& params)
{
    if (nodeIds.empty()) {
        return 0;
    }

    auto keyword = std::make_unique<LoadNodePoint>();

    // Distribute force equally across all nodes
    double forcePerNode = params.magnitude / static_cast<double>(nodeIds.size());

    // Create load for each direction (X, Y, Z) for each node
    for (NodeId nid : nodeIds) {
        // X component
        if (force.x != 0.0) {
            NodeLoadData loadX;
            loadX.nid = nid;
            loadX.dof = 1;  // X direction
            loadX.lcid = params.lcid;
            loadX.sf = force.x * forcePerNode * params.scaleFactor;
            loadX.cid = 0;
            keyword->addLoad(loadX);
        }

        // Y component
        if (force.y != 0.0) {
            NodeLoadData loadY;
            loadY.nid = nid;
            loadY.dof = 2;  // Y direction
            loadY.lcid = params.lcid;
            loadY.sf = force.y * forcePerNode * params.scaleFactor;
            loadY.cid = 0;
            keyword->addLoad(loadY);
        }

        // Z component
        if (force.z != 0.0) {
            NodeLoadData loadZ;
            loadZ.nid = nid;
            loadZ.dof = 3;  // Z direction
            loadZ.lcid = params.lcid;
            loadZ.sf = force.z * forcePerNode * params.scaleFactor;
            loadZ.cid = 0;
            keyword->addLoad(loadZ);
        }
    }

    model_.addKeyword(std::move(keyword));
    return 1;
}

int LoadManager::applyForceToNodeSet(
    int nodeSetId,
    const Vec3& force,
    const LoadParameters& params)
{
    auto keyword = std::make_unique<LoadNodeSet>();

    // Create load for each direction
    if (force.x != 0.0) {
        LoadNodeSet::Data loadX;
        loadX.nsid = nodeSetId;
        loadX.dof = 1;  // X direction
        loadX.lcid = params.lcid;
        loadX.sf = force.x * params.magnitude * params.scaleFactor;
        loadX.cid = 0;
        keyword->addLoad(loadX);
    }

    if (force.y != 0.0) {
        LoadNodeSet::Data loadY;
        loadY.nsid = nodeSetId;
        loadY.dof = 2;  // Y direction
        loadY.lcid = params.lcid;
        loadY.sf = force.y * params.magnitude * params.scaleFactor;
        loadY.cid = 0;
        keyword->addLoad(loadY);
    }

    if (force.z != 0.0) {
        LoadNodeSet::Data loadZ;
        loadZ.nsid = nodeSetId;
        loadZ.dof = 3;  // Z direction
        loadZ.lcid = params.lcid;
        loadZ.sf = force.z * params.magnitude * params.scaleFactor;
        loadZ.cid = 0;
        keyword->addLoad(loadZ);
    }

    model_.addKeyword(std::move(keyword));
    return 1;
}

int LoadManager::applyGravity(
    double magnitude,
    const LoadParameters& params)
{
    return applyAcceleration(Direction::NegativeZ, magnitude, params);
}

int LoadManager::applyAcceleration(
    Direction direction,
    double magnitude,
    const LoadParameters& params)
{
    double sign = 1.0;
    char axis = directionToAxis(direction, sign);

    double finalMagnitude = sign * magnitude * params.scaleFactor;

    if (axis == 'X') {
        auto keyword = std::make_unique<LoadBodyX>();
        auto& data = keyword->getData();
        data.lcid = params.lcid;
        data.sf = finalMagnitude;
        data.lciddr = 0;
        data.xc = 0.0;
        data.yc = 0.0;
        data.zc = 0.0;
        model_.addKeyword(std::move(keyword));
    } else if (axis == 'Y') {
        auto keyword = std::make_unique<LoadBodyY>();
        auto& data = keyword->getData();
        data.lcid = params.lcid;
        data.sf = finalMagnitude;
        data.lciddr = 0;
        data.xc = 0.0;
        data.yc = 0.0;
        data.zc = 0.0;
        model_.addKeyword(std::move(keyword));
    } else {  // Z
        auto keyword = std::make_unique<LoadBodyZ>();
        auto& data = keyword->getData();
        data.lcid = params.lcid;
        data.sf = finalMagnitude;
        data.lciddr = 0;
        data.xc = 0.0;
        data.yc = 0.0;
        data.zc = 0.0;
        model_.addKeyword(std::move(keyword));
    }

    return 1;
}

// ============================================================================
// Load Curve Management
// ============================================================================

int LoadManager::createLoadCurve(
    const std::vector<double>& times,
    const std::vector<double>& values,
    const std::string& title)
{
    if (times.size() != values.size() || times.empty()) {
        return 0;
    }

    int lcid = getNextLoadCurveId();

    if (title.empty()) {
        auto keyword = std::make_unique<DefineCurve>();
        keyword->setCurveId(lcid);

        for (size_t i = 0; i < times.size(); ++i) {
            keyword->addPoint(times[i], values[i]);
        }

        model_.addKeyword(std::move(keyword));
    } else {
        auto keyword = std::make_unique<DefineCurveTitle>();
        keyword->setCurveId(lcid);
        keyword->setTitle(title);

        for (size_t i = 0; i < times.size(); ++i) {
            keyword->addPoint(times[i], values[i]);
        }

        model_.addKeyword(std::move(keyword));
    }

    return lcid;
}

int LoadManager::createStepLoadCurve(
    double stepTime,
    double rampTime,
    const std::string& title)
{
    std::vector<double> times;
    std::vector<double> values;

    if (rampTime <= 0.0) {
        // Instant step
        times = {0.0, stepTime - 1e-6, stepTime};
        values = {0.0, 0.0, 1.0};
    } else {
        // Ramped step
        times = {0.0, stepTime, stepTime + rampTime};
        values = {0.0, 0.0, 1.0};
    }

    return createLoadCurve(times, values, title);
}

int LoadManager::createRampLoadCurve(
    double startTime,
    double endTime,
    const std::string& title)
{
    std::vector<double> times = {0.0, startTime, endTime};
    std::vector<double> values = {0.0, 0.0, 1.0};

    return createLoadCurve(times, values, title);
}

int LoadManager::createPulseLoadCurve(
    double riseTime,
    double holdTime,
    double fallTime,
    const std::string& title)
{
    std::vector<double> times = {
        0.0,
        riseTime,
        riseTime + holdTime,
        riseTime + holdTime + fallTime
    };
    std::vector<double> values = {0.0, 1.0, 1.0, 0.0};

    return createLoadCurve(times, values, title);
}

// ============================================================================
// Load Query
// ============================================================================

std::vector<const LoadKeyword*> LoadManager::getAllLoads() const {
    std::vector<const LoadKeyword*> result;

    // Get all load keywords from model
    auto keywords = model_.getKeywordsOfType<LoadKeyword>();
    result.insert(result.end(), keywords.begin(), keywords.end());

    return result;
}

size_t LoadManager::getLoadCount() const {
    return getAllLoads().size();
}

int LoadManager::getNextLoadCurveId() {
    return nextLoadCurveId_++;
}

// ============================================================================
// Helper Methods
// ============================================================================

char LoadManager::directionToAxis(Direction dir, double& outSign) const {
    switch (dir) {
        case Direction::X:
            outSign = 1.0;
            return 'X';
        case Direction::Y:
            outSign = 1.0;
            return 'Y';
        case Direction::Z:
            outSign = 1.0;
            return 'Z';
        case Direction::NegativeX:
            outSign = -1.0;
            return 'X';
        case Direction::NegativeY:
            outSign = -1.0;
            return 'Y';
        case Direction::NegativeZ:
            outSign = -1.0;
            return 'Z';
        default:
            outSign = 1.0;
            return 'Z';
    }
}

} // namespace koo::dyna::managers
