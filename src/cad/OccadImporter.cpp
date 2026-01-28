#include <koo/cad/OccadImporter.hpp>

#ifdef KOO_HAS_OPENCASCADE

// OpenCASCADE includes
#include <STEPControl_Reader.hxx>
#include <IGESControl_Reader.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <BRep_Tool.hxx>
#include <Bnd_Box.hxx>
#include <BRepBndLib.hxx>
#include <GProp_GProps.hxx>
#include <BRepGProp.hxx>
#include <ShapeAnalysis_ShapeTolerance.hxx>
#include <ShapeFix_Shape.hxx>
#include <gp_Pnt.hxx>

#include <algorithm>
#include <sstream>

namespace koo::cad {

// ============================================================================
// Pimpl Implementation
// ============================================================================

class OccadImporter::Impl {
public:
    TopoDS_Shape shape_;
    std::shared_ptr<Geometry> geometry_;
    std::string lastError_;
    double tolerance_ = 1e-6;
    bool shapeHealing_ = true;

    // Import STEP file
    bool importSTEP(const std::string& filepath) {
        try {
            STEPControl_Reader reader;
            IFSelect_ReturnStatus status = reader.ReadFile(filepath.c_str());

            if (status != IFSelect_RetDone) {
                lastError_ = "Failed to read STEP file: " + filepath;
                return false;
            }

            // Transfer shape
            reader.TransferRoots();
            shape_ = reader.OneShape();

            if (shape_.IsNull()) {
                lastError_ = "No shapes found in STEP file";
                return false;
            }

            return true;
        } catch (const std::exception& e) {
            lastError_ = std::string("STEP import exception: ") + e.what();
            return false;
        }
    }

    // Import IGES file
    bool importIGES(const std::string& filepath) {
        try {
            IGESControl_Reader reader;
            Standard_Integer status = reader.ReadFile(filepath.c_str());

            if (status != IFSelect_RetDone) {
                lastError_ = "Failed to read IGES file: " + filepath;
                return false;
            }

            // Transfer shape
            reader.TransferRoots();
            shape_ = reader.OneShape();

            if (shape_.IsNull()) {
                lastError_ = "No shapes found in IGES file";
                return false;
            }

            return true;
        } catch (const std::exception& e) {
            lastError_ = std::string("IGES import exception: ") + e.what();
            return false;
        }
    }

    // Apply shape healing
    void healShape() {
        if (!shapeHealing_ || shape_.IsNull()) {
            return;
        }

        try {
            Handle(ShapeFix_Shape) fixer = new ShapeFix_Shape(shape_);
            fixer->SetPrecision(tolerance_);
            fixer->SetMaxTolerance(tolerance_ * 10.0);
            fixer->Perform();
            shape_ = fixer->Shape();
        } catch (const std::exception& e) {
            // Healing failed, continue with original shape
            lastError_ = std::string("Shape healing warning: ") + e.what();
        }
    }

    // Extract topology and create Geometry
    void extractTopology() {
        geometry_ = std::make_shared<Geometry>();

        if (shape_.IsNull()) {
            return;
        }

        // Store native shape
        geometry_->setNativeGeometry(new TopoDS_Shape(shape_));

        // Extract solids
        TopExp_Explorer solidExp(shape_, TopAbs_SOLID);
        int solidId = 1;
        for (; solidExp.More(); solidExp.Next()) {
            TopoDS_Solid solid = TopoDS::Solid(solidExp.Current());
            GeometryEntity entity(
                EntityType::Solid,
                solidId++,
                computeBoundingBox(solid),
                new TopoDS_Shape(solid)
            );
            geometry_->addSolid(entity);
        }

        // Extract faces
        TopExp_Explorer faceExp(shape_, TopAbs_FACE);
        int faceId = 1;
        for (; faceExp.More(); faceExp.Next()) {
            TopoDS_Face face = TopoDS::Face(faceExp.Current());
            GeometryEntity entity(
                EntityType::Face,
                faceId++,
                computeBoundingBox(face),
                new TopoDS_Shape(face)
            );
            geometry_->addFace(entity);
        }

        // Extract edges
        TopExp_Explorer edgeExp(shape_, TopAbs_EDGE);
        int edgeId = 1;
        for (; edgeExp.More(); edgeExp.Next()) {
            TopoDS_Edge edge = TopoDS::Edge(edgeExp.Current());
            GeometryEntity entity(
                EntityType::Edge,
                edgeId++,
                computeBoundingBox(edge),
                new TopoDS_Shape(edge)
            );
            geometry_->addEdge(entity);
        }

        // Extract vertices
        TopExp_Explorer vertexExp(shape_, TopAbs_VERTEX);
        int vertexId = 1;
        for (; vertexExp.More(); vertexExp.Next()) {
            TopoDS_Vertex vertex = TopoDS::Vertex(vertexExp.Current());
            GeometryEntity entity(
                EntityType::Vertex,
                vertexId++,
                computeBoundingBox(vertex),
                new TopoDS_Shape(vertex)
            );
            geometry_->addVertex(entity);
        }

        // Compute global bounding box
        geometry_->setBoundingBox(computeBoundingBox(shape_));
    }

    // Compute bounding box for a shape
    common::BoundingBox computeBoundingBox(const TopoDS_Shape& shape) const {
        if (shape.IsNull()) {
            return common::BoundingBox();
        }

        Bnd_Box box;
        BRepBndLib::Add(shape, box);

        if (box.IsVoid()) {
            return common::BoundingBox();
        }

        double xmin, ymin, zmin, xmax, ymax, zmax;
        box.Get(xmin, ymin, zmin, xmax, ymax, zmax);

        return common::BoundingBox{
            common::Vec3(xmin, ymin, zmin),
            common::Vec3(xmax, ymax, zmax)
        };
    }

    // Compute total volume
    double computeTotalVolume() const {
        if (shape_.IsNull()) {
            return 0.0;
        }

        double totalVolume = 0.0;
        TopExp_Explorer solidExp(shape_, TopAbs_SOLID);
        for (; solidExp.More(); solidExp.Next()) {
            GProp_GProps props;
            BRepGProp::VolumeProperties(solidExp.Current(), props);
            totalVolume += props.Mass();
        }

        return totalVolume;
    }

    // Compute total surface area
    double computeTotalSurfaceArea() const {
        if (shape_.IsNull()) {
            return 0.0;
        }

        GProp_GProps props;
        BRepGProp::SurfaceProperties(shape_, props);
        return props.Mass();
    }
};

// ============================================================================
// OccadImporter Implementation
// ============================================================================

OccadImporter::OccadImporter()
    : impl_(std::make_unique<Impl>())
{
}

OccadImporter::~OccadImporter() {
    clear();
}

OccadImporter::OccadImporter(OccadImporter&&) noexcept = default;
OccadImporter& OccadImporter::operator=(OccadImporter&&) noexcept = default;

bool OccadImporter::importFile(const std::string& filepath) {
    clear();

    // Determine file type from extension
    std::string ext;
    size_t dotPos = filepath.find_last_of('.');
    if (dotPos != std::string::npos) {
        ext = filepath.substr(dotPos + 1);
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    }

    bool success = false;

    if (ext == "step" || ext == "stp") {
        success = impl_->importSTEP(filepath);
    } else if (ext == "iges" || ext == "igs") {
        success = impl_->importIGES(filepath);
    } else {
        impl_->lastError_ = "Unsupported file format: " + ext;
        return false;
    }

    if (!success) {
        return false;
    }

    // Apply shape healing
    impl_->healShape();

    // Extract topology
    impl_->extractTopology();

    return true;
}

std::string OccadImporter::getLastError() const {
    return impl_->lastError_;
}

std::shared_ptr<Geometry> OccadImporter::getGeometry() const {
    return impl_->geometry_;
}

bool OccadImporter::hasGeometry() const {
    return impl_->geometry_ && !impl_->geometry_->isEmpty();
}

std::vector<GeometryEntity> OccadImporter::getSolids() const {
    if (!impl_->geometry_) {
        return {};
    }
    return impl_->geometry_->getSolids();
}

std::vector<GeometryEntity> OccadImporter::getFaces() const {
    if (!impl_->geometry_) {
        return {};
    }
    return impl_->geometry_->getFaces();
}

std::vector<GeometryEntity> OccadImporter::getEdges() const {
    if (!impl_->geometry_) {
        return {};
    }
    return impl_->geometry_->getEdges();
}

std::vector<GeometryEntity> OccadImporter::getVertices() const {
    if (!impl_->geometry_) {
        return {};
    }
    return impl_->geometry_->getVertices();
}

int OccadImporter::getNumSolids() const {
    return impl_->geometry_ ? static_cast<int>(impl_->geometry_->getNumSolids()) : 0;
}

int OccadImporter::getNumFaces() const {
    return impl_->geometry_ ? static_cast<int>(impl_->geometry_->getNumFaces()) : 0;
}

int OccadImporter::getNumEdges() const {
    return impl_->geometry_ ? static_cast<int>(impl_->geometry_->getNumEdges()) : 0;
}

int OccadImporter::getNumVertices() const {
    return impl_->geometry_ ? static_cast<int>(impl_->geometry_->getNumVertices()) : 0;
}

common::BoundingBox OccadImporter::getBoundingBox() const {
    if (!impl_->geometry_) {
        return common::BoundingBox();
    }
    return impl_->geometry_->getBoundingBox();
}

double OccadImporter::getTotalVolume() const {
    return impl_->computeTotalVolume();
}

double OccadImporter::getTotalSurfaceArea() const {
    return impl_->computeTotalSurfaceArea();
}

void OccadImporter::clear() {
    if (impl_->geometry_) {
        // Clean up native handles
        void* nativeGeom = impl_->geometry_->getNativeGeometry();
        if (nativeGeom) {
            delete static_cast<TopoDS_Shape*>(nativeGeom);
        }

        for (const auto& solid : impl_->geometry_->getSolids()) {
            if (solid.nativeHandle) {
                delete static_cast<TopoDS_Shape*>(solid.nativeHandle);
            }
        }

        for (const auto& face : impl_->geometry_->getFaces()) {
            if (face.nativeHandle) {
                delete static_cast<TopoDS_Shape*>(face.nativeHandle);
            }
        }

        for (const auto& edge : impl_->geometry_->getEdges()) {
            if (edge.nativeHandle) {
                delete static_cast<TopoDS_Shape*>(edge.nativeHandle);
            }
        }

        for (const auto& vertex : impl_->geometry_->getVertices()) {
            if (vertex.nativeHandle) {
                delete static_cast<TopoDS_Shape*>(vertex.nativeHandle);
            }
        }

        impl_->geometry_->clear();
        impl_->geometry_.reset();
    }

    impl_->shape_.Nullify();
    impl_->lastError_.clear();
}

void* OccadImporter::getNativeShape() const {
    if (impl_->shape_.IsNull()) {
        return nullptr;
    }
    return new TopoDS_Shape(impl_->shape_);
}

void OccadImporter::setImportTolerance(double tolerance) {
    impl_->tolerance_ = tolerance;
}

double OccadImporter::getImportTolerance() const {
    return impl_->tolerance_;
}

void OccadImporter::setShapeHealing(bool enable) {
    impl_->shapeHealing_ = enable;
}

bool OccadImporter::isShapeHealingEnabled() const {
    return impl_->shapeHealing_;
}

} // namespace koo::cad

#else // !KOO_HAS_OPENCASCADE

// Stub implementation when OpenCASCADE is not available

namespace koo::cad {

class OccadImporter::Impl {};

OccadImporter::OccadImporter() : impl_(nullptr) {}
OccadImporter::~OccadImporter() = default;
OccadImporter::OccadImporter(OccadImporter&&) noexcept = default;
OccadImporter& OccadImporter::operator=(OccadImporter&&) noexcept = default;

bool OccadImporter::importFile(const std::string&) {
    return false;
}

std::string OccadImporter::getLastError() const {
    return "OpenCASCADE support not compiled";
}

std::shared_ptr<Geometry> OccadImporter::getGeometry() const {
    return nullptr;
}

bool OccadImporter::hasGeometry() const {
    return false;
}

std::vector<GeometryEntity> OccadImporter::getSolids() const { return {}; }
std::vector<GeometryEntity> OccadImporter::getFaces() const { return {}; }
std::vector<GeometryEntity> OccadImporter::getEdges() const { return {}; }
std::vector<GeometryEntity> OccadImporter::getVertices() const { return {}; }

int OccadImporter::getNumSolids() const { return 0; }
int OccadImporter::getNumFaces() const { return 0; }
int OccadImporter::getNumEdges() const { return 0; }
int OccadImporter::getNumVertices() const { return 0; }

common::BoundingBox OccadImporter::getBoundingBox() const {
    return common::BoundingBox();
}

double OccadImporter::getTotalVolume() const { return 0.0; }
double OccadImporter::getTotalSurfaceArea() const { return 0.0; }
void OccadImporter::clear() {}

void* OccadImporter::getNativeShape() const { return nullptr; }
void OccadImporter::setImportTolerance(double) {}
double OccadImporter::getImportTolerance() const { return 0.0; }
void OccadImporter::setShapeHealing(bool) {}
bool OccadImporter::isShapeHealingEnabled() const { return false; }

} // namespace koo::cad

#endif // KOO_HAS_OPENCASCADE
