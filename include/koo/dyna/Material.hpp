#pragma once

#include <koo/Export.hpp>
#include <koo/dyna/Keyword.hpp>
#include <koo/util/Types.hpp>
#include <vector>
#include <unordered_map>

namespace koo::dyna {

/**
 * @brief Material type enumeration
 */
enum class MaterialType {
    Unknown,
    Elastic,               // *MAT_ELASTIC / *MAT_001
    Rigid,                 // *MAT_RIGID / *MAT_020
    PlasticKinematic,      // *MAT_PLASTIC_KINEMATIC / *MAT_003
    Piecewise,             // *MAT_PIECEWISE_LINEAR_PLASTICITY / *MAT_024
    JohnsonCook,           // *MAT_JOHNSON_COOK / *MAT_015
    Null,                  // *MAT_NULL / *MAT_009
    Viscoelastic,          // *MAT_VISCOELASTIC / *MAT_006
    BlatzKo,               // *MAT_BLATZ-KO_RUBBER / *MAT_007
    PowerLawPlasticity,    // *MAT_POWER_LAW_PLASTICITY / *MAT_018
    StrainRateDep,         // *MAT_STRAIN_RATE_DEPENDENT_PLASTICITY / *MAT_019
    Honeycomb,             // *MAT_HONEYCOMB / *MAT_026
    ModifiedPiecewise,     // *MAT_MODIFIED_PIECEWISE_LINEAR_PLASTICITY / *MAT_123
    CrushableFoam,         // *MAT_CRUSHABLE_FOAM / *MAT_063
    SpotWeld,              // *MAT_SPOTWELD / *MAT_100
    OgdenRubber,           // *MAT_OGDEN_RUBBER / *MAT_077
    Fabric,                // *MAT_FABRIC / *MAT_034
};

/**
 * @brief Base material data
 */
struct KOO_API MaterialData {
    MaterialId id = 0;
    double ro = 0.0;      // Density
    double e = 0.0;       // Young's modulus
    double pr = 0.0;      // Poisson's ratio
    std::string title;
    MaterialType type = MaterialType::Unknown;

    MaterialData() = default;
};

/**
 * @brief Elastic material (*MAT_ELASTIC, *MAT_001)
 */
struct KOO_API MatElasticData : public MaterialData {
    double da = 0.0;  // Axial damping factor
    double db = 0.0;  // Bending damping factor
    double k = 0.0;   // Bulk modulus

    MatElasticData() { type = MaterialType::Elastic; }
};

/**
 * @brief Rigid material (*MAT_RIGID, *MAT_020)
 */
struct KOO_API MatRigidData : public MaterialData {
    int cmo = 0;      // Center of mass constraint
    int con1 = 0;     // Constraint parameter 1
    int con2 = 0;     // Constraint parameter 2

    MatRigidData() { type = MaterialType::Rigid; }
};

/**
 * @brief Base class for material keywords
 */
class KOO_API MaterialBase : public Keyword {
public:
    virtual MaterialType getMaterialType() const = 0;
    virtual MaterialId getMaterialId() const = 0;
    virtual void setMaterialId(MaterialId id) = 0;
};

/**
 * @brief *MAT_ELASTIC keyword
 */
class KOO_API MatElastic : public CloneableKeyword<MatElastic, MaterialBase> {
public:
    MatElastic() = default;

    std::string getKeywordName() const override { return "*MAT_ELASTIC"; }
    MaterialType getMaterialType() const override { return MaterialType::Elastic; }

    MaterialId getMaterialId() const override { return data_.id; }
    void setMaterialId(MaterialId id) override { data_.id = id; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    // Data access
    MatElasticData& getData() { return data_; }
    const MatElasticData& getData() const { return data_; }
    void setData(const MatElasticData& data) { data_ = data; }

private:
    MatElasticData data_;
};

/**
 * @brief *MAT_RIGID keyword
 */
class KOO_API MatRigid : public CloneableKeyword<MatRigid, MaterialBase> {
public:
    MatRigid() = default;

    std::string getKeywordName() const override { return "*MAT_RIGID"; }
    MaterialType getMaterialType() const override { return MaterialType::Rigid; }

    MaterialId getMaterialId() const override { return data_.id; }
    void setMaterialId(MaterialId id) override { data_.id = id; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    // Data access
    MatRigidData& getData() { return data_; }
    const MatRigidData& getData() const { return data_; }
    void setData(const MatRigidData& data) { data_ = data; }

private:
    MatRigidData data_;
};

/**
 * @brief *MAT_PLASTIC_KINEMATIC (*MAT_003)
 *
 * Isotropic/kinematic hardening plasticity with strain rate effects.
 */
class KOO_API MatPlasticKinematic : public CloneableKeyword<MatPlasticKinematic, MaterialBase> {
public:
    struct Data {
        MaterialId mid = 0;     // Material ID
        double ro = 0.0;        // Density
        double e = 0.0;         // Young's modulus
        double pr = 0.0;        // Poisson's ratio
        double sigy = 0.0;      // Yield stress
        double etan = 0.0;      // Tangent modulus
        double beta = 0.0;      // Hardening parameter (0=kinematic, 1=isotropic)
        double src = 0.0;       // Strain rate parameter C
        double srp = 0.0;       // Strain rate parameter P
        double fs = 0.0;        // Failure strain
        int vp = 0;             // Viscoplasticity flag
    };

    MatPlasticKinematic() = default;

    std::string getKeywordName() const override { return "*MAT_PLASTIC_KINEMATIC"; }
    MaterialType getMaterialType() const override { return MaterialType::PlasticKinematic; }

    MaterialId getMaterialId() const override { return data_.mid; }
    void setMaterialId(MaterialId id) override { data_.mid = id; }

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
 * @brief *MAT_PIECEWISE_LINEAR_PLASTICITY (*MAT_024)
 *
 * Elasto-plastic material with piecewise linear stress-strain curve.
 */
class KOO_API MatPiecewiseLinearPlasticity : public CloneableKeyword<MatPiecewiseLinearPlasticity, MaterialBase> {
public:
    struct Data {
        MaterialId mid = 0;
        double ro = 0.0;
        double e = 0.0;
        double pr = 0.0;
        double sigy = 0.0;      // Yield stress
        double etan = 0.0;      // Tangent modulus
        double fail = 0.0;      // Failure flag/strain
        double tdel = 0.0;      // Minimum timestep for element deletion
        double c = 0.0;         // Strain rate parameter C (Cowper-Symonds)
        double p = 0.0;         // Strain rate parameter P
        int lcss = 0;           // Load curve for stress-strain
        int lcsr = 0;           // Load curve for strain rate
        double vp = 0.0;        // Viscoplasticity
        double eps1 = 0.0;      // Effective plastic strain values (up to 8)
        double eps2 = 0.0;
        double eps3 = 0.0;
        double eps4 = 0.0;
        double eps5 = 0.0;
        double eps6 = 0.0;
        double eps7 = 0.0;
        double eps8 = 0.0;
        double es1 = 0.0;       // Corresponding stress values
        double es2 = 0.0;
        double es3 = 0.0;
        double es4 = 0.0;
        double es5 = 0.0;
        double es6 = 0.0;
        double es7 = 0.0;
        double es8 = 0.0;
    };

    MatPiecewiseLinearPlasticity() = default;

    std::string getKeywordName() const override { return "*MAT_PIECEWISE_LINEAR_PLASTICITY"; }
    MaterialType getMaterialType() const override { return MaterialType::Piecewise; }

    MaterialId getMaterialId() const override { return data_.mid; }
    void setMaterialId(MaterialId id) override { data_.mid = id; }

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
 * @brief *MAT_JOHNSON_COOK (*MAT_015)
 *
 * Johnson-Cook plasticity model with strain rate and temperature effects.
 */
class KOO_API MatJohnsonCook : public CloneableKeyword<MatJohnsonCook, MaterialBase> {
public:
    struct Data {
        MaterialId mid = 0;
        double ro = 0.0;        // Density
        double g = 0.0;         // Shear modulus
        double e = 0.0;         // Young's modulus
        double pr = 0.0;        // Poisson's ratio
        double dtf = 0.0;       // Minimum timestep
        double vp = 0.0;        // Viscoplasticity
        double rateop = 0.0;    // Rate formulation
        double a = 0.0;         // Yield stress constant
        double b = 0.0;         // Hardening constant
        double n = 0.0;         // Hardening exponent
        double c = 0.0;         // Strain rate constant
        double m = 0.0;         // Thermal softening exponent
        double tm = 0.0;        // Melt temperature
        double tr = 0.0;        // Room temperature
        double epso = 0.0;      // Reference strain rate
        double cp = 0.0;        // Specific heat
        double pc = 0.0;        // Pressure cutoff
        double spall = 0.0;     // Spall model
        double it = 0.0;        // Iteration flag
        double d1 = 0.0;        // Damage constants (D1-D5)
        double d2 = 0.0;
        double d3 = 0.0;
        double d4 = 0.0;
        double d5 = 0.0;
    };

    MatJohnsonCook() = default;

    std::string getKeywordName() const override { return "*MAT_JOHNSON_COOK"; }
    MaterialType getMaterialType() const override { return MaterialType::JohnsonCook; }

    MaterialId getMaterialId() const override { return data_.mid; }
    void setMaterialId(MaterialId id) override { data_.mid = id; }

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
 * @brief *MAT_NULL (*MAT_009)
 *
 * Null material for contact surfaces or pressure-only elements.
 */
class KOO_API MatNull : public CloneableKeyword<MatNull, MaterialBase> {
public:
    struct Data {
        MaterialId mid = 0;
        double ro = 0.0;        // Density
        double pc = 0.0;        // Pressure cutoff
        double mu = 0.0;        // Dynamic viscosity
        double terod = 0.0;     // Erosion in tension
        double cerod = 0.0;     // Erosion in compression
        double ym = 0.0;        // Young's modulus (for contact)
        double prr = 0.0;       // Poisson's ratio (for contact)
    };

    MatNull() = default;

    std::string getKeywordName() const override { return "*MAT_NULL"; }
    MaterialType getMaterialType() const override { return MaterialType::Null; }

    MaterialId getMaterialId() const override { return data_.mid; }
    void setMaterialId(MaterialId id) override { data_.mid = id; }

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
 * @brief *MAT_VISCOELASTIC (*MAT_006)
 *
 * Viscoelastic material model.
 */
class KOO_API MatViscoelastic : public CloneableKeyword<MatViscoelastic, MaterialBase> {
public:
    struct Data {
        MaterialId mid = 0;
        double ro = 0.0;        // Density
        double bulk = 0.0;      // Bulk modulus
        double g0 = 0.0;        // Short-time shear modulus
        double gi = 0.0;        // Long-time shear modulus
        double beta = 0.0;      // Decay constant
    };

    MatViscoelastic() = default;

    std::string getKeywordName() const override { return "*MAT_VISCOELASTIC"; }
    MaterialType getMaterialType() const override { return MaterialType::Viscoelastic; }

    MaterialId getMaterialId() const override { return data_.mid; }
    void setMaterialId(MaterialId id) override { data_.mid = id; }

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
 * @brief *MAT_POWER_LAW_PLASTICITY (*MAT_018)
 *
 * Power law isotropic plasticity.
 */
class KOO_API MatPowerLawPlasticity : public CloneableKeyword<MatPowerLawPlasticity, MaterialBase> {
public:
    struct Data {
        MaterialId mid = 0;
        double ro = 0.0;        // Density
        double e = 0.0;         // Young's modulus
        double pr = 0.0;        // Poisson's ratio
        double k = 0.0;         // Strength coefficient
        double n = 0.0;         // Hardening exponent
        double src = 0.0;       // Strain rate parameter C
        double srp = 0.0;       // Strain rate parameter P
        double sigy = 0.0;      // Initial yield stress
        double vp = 0.0;        // Viscoplasticity
    };

    MatPowerLawPlasticity() = default;

    std::string getKeywordName() const override { return "*MAT_POWER_LAW_PLASTICITY"; }
    MaterialType getMaterialType() const override { return MaterialType::PowerLawPlasticity; }

    MaterialId getMaterialId() const override { return data_.mid; }
    void setMaterialId(MaterialId id) override { data_.mid = id; }

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
 * @brief *MAT_HONEYCOMB (*MAT_026)
 *
 * Orthotropic material for modeling honeycomb and foam materials.
 */
class KOO_API MatHoneycomb : public CloneableKeyword<MatHoneycomb, MaterialBase> {
public:
    struct Data {
        MaterialId mid = 0;
        double ro = 0.0;        // Density
        double e = 0.0;         // Young's modulus (fully compacted)
        double pr = 0.0;        // Poisson's ratio
        double sigy = 0.0;      // Yield stress (fully compacted)
        double vf = 0.0;        // Relative volume at compaction
        double mu = 0.0;        // Material viscosity coefficient
        double bulk = 0.0;      // Bulk viscosity flag
        double lca = 0.0;       // Load curve ID (sigma-aa vs strain)
        double lcb = 0.0;       // Load curve ID (sigma-bb vs strain)
        double lcc = 0.0;       // Load curve ID (sigma-cc vs strain)
        double lcs = 0.0;       // Load curve ID (shear vs strain)
        double lcab = 0.0;      // Load curve ID (sigma-ab vs strain)
        double lcbc = 0.0;      // Load curve ID (sigma-bc vs strain)
        double lcca = 0.0;      // Load curve ID (sigma-ca vs strain)
        double lcsr = 0.0;      // Load curve for strain rate
        double eaau = 0.0;      // Elastic modulus Eaa (uncompacted)
        double ebbu = 0.0;      // Elastic modulus Ebb (uncompacted)
        double eccu = 0.0;      // Elastic modulus Ecc (uncompacted)
        double gabu = 0.0;      // Shear modulus Gab (uncompacted)
        double gbcu = 0.0;      // Shear modulus Gbc (uncompacted)
        double gcau = 0.0;      // Shear modulus Gca (uncompacted)
        int apts = 0;           // Material axes option
        int macf = 0;           // Material axes change flag
    };

    MatHoneycomb() = default;

    std::string getKeywordName() const override { return "*MAT_HONEYCOMB"; }
    MaterialType getMaterialType() const override { return MaterialType::Honeycomb; }

    MaterialId getMaterialId() const override { return data_.mid; }
    void setMaterialId(MaterialId id) override { data_.mid = id; }

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
 * @brief *MAT_MODIFIED_PIECEWISE_LINEAR_PLASTICITY (*MAT_123)
 *
 * Modified piecewise linear plasticity with additional failure options.
 */
class KOO_API MatModifiedPiecewiseLinearPlasticity : public CloneableKeyword<MatModifiedPiecewiseLinearPlasticity, MaterialBase> {
public:
    struct Data {
        MaterialId mid = 0;
        double ro = 0.0;
        double e = 0.0;
        double pr = 0.0;
        double sigy = 0.0;
        double etan = 0.0;
        double fail = 0.0;
        double tdel = 0.0;
        double c = 0.0;
        double p = 0.0;
        int lcss = 0;
        int lcsr = 0;
        double vp = 0.0;
        double epsthin = 0.0;   // Thinning strain at failure
        double epsmaj = 0.0;    // Major in-plane strain at failure
        double numint = 0.0;    // Number of integration points for failure
        double epsc = 0.0;      // Effective plastic strain for element erosion
        double epsr = 0.0;      // Effective plastic strain for output
        double epsab = 0.0;     // Effective plastic strain
        double lcab = 0.0;      // Load curve defining epsmaj vs epsmin
    };

    MatModifiedPiecewiseLinearPlasticity() = default;

    std::string getKeywordName() const override { return "*MAT_MODIFIED_PIECEWISE_LINEAR_PLASTICITY"; }
    MaterialType getMaterialType() const override { return MaterialType::ModifiedPiecewise; }

    MaterialId getMaterialId() const override { return data_.mid; }
    void setMaterialId(MaterialId id) override { data_.mid = id; }

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
 * @brief *MAT_CRUSHABLE_FOAM (*MAT_063)
 *
 * Crushable foam material model.
 */
class KOO_API MatCrushableFoam : public CloneableKeyword<MatCrushableFoam, MaterialBase> {
public:
    struct Data {
        MaterialId mid = 0;
        double ro = 0.0;
        double e = 0.0;
        double pr = 0.0;
        double lcid = 0.0;      // Load curve ID for yield stress vs volumetric strain
        double tsc = 0.0;       // Tensile stress cutoff
        double damp = 0.0;      // Rate sensitivity via damping
        double ncycle = 0.0;    // Number of cycles for averaging
        double maxeps = 0.0;    // Maximum volumetric strain
    };

    MatCrushableFoam() = default;

    std::string getKeywordName() const override { return "*MAT_CRUSHABLE_FOAM"; }
    MaterialType getMaterialType() const override { return MaterialType::CrushableFoam; }

    MaterialId getMaterialId() const override { return data_.mid; }
    void setMaterialId(MaterialId id) override { data_.mid = id; }

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
 * @brief *MAT_SPOTWELD (*MAT_100)
 *
 * Spot weld material for beam/solid elements.
 */
class KOO_API MatSpotWeld : public CloneableKeyword<MatSpotWeld, MaterialBase> {
public:
    struct Data {
        MaterialId mid = 0;
        double ro = 0.0;
        double e = 0.0;
        double pr = 0.0;
        double sigy = 0.0;      // Initial yield stress
        double eh = 0.0;        // Hardening modulus
        double dt = 0.0;        // Time step
        int tfail = 0;          // Failure time flag
        double efail = 0.0;     // Effective plastic strain at failure
        int nf = 0;             // Number of force components for failure
        int rs = 0.0;           // Rupture strain
        double dtf = 0.0;       // Failure time
        double tf = 0.0;        // Failure parameter
    };

    MatSpotWeld() = default;

    std::string getKeywordName() const override { return "*MAT_SPOTWELD"; }
    MaterialType getMaterialType() const override { return MaterialType::SpotWeld; }

    MaterialId getMaterialId() const override { return data_.mid; }
    void setMaterialId(MaterialId id) override { data_.mid = id; }

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
 * @brief *MAT_OGDEN_RUBBER (*MAT_077)
 *
 * Ogden rubber material for hyperelastic behavior.
 */
class KOO_API MatOgdenRubber : public CloneableKeyword<MatOgdenRubber, MaterialBase> {
public:
    struct Data {
        MaterialId mid = 0;
        double ro = 0.0;
        double pr = 0.0;        // Poisson's ratio
        int n = 0;              // Order of fit (1-3)
        int nv = 0;             // Number of Prony terms
        double g = 0.0;         // Shear modulus (for linear)
        double sigf = 0.0;      // Limit stress for element erosion
        double ref = 0.0;       // Use reference geometry
        double mu1 = 0.0;       // Shear modulus 1
        double mu2 = 0.0;       // Shear modulus 2
        double mu3 = 0.0;       // Shear modulus 3
        double alpha1 = 0.0;    // Exponent 1
        double alpha2 = 0.0;    // Exponent 2
        double alpha3 = 0.0;    // Exponent 3
    };

    MatOgdenRubber() = default;

    std::string getKeywordName() const override { return "*MAT_OGDEN_RUBBER"; }
    MaterialType getMaterialType() const override { return MaterialType::OgdenRubber; }

    MaterialId getMaterialId() const override { return data_.mid; }
    void setMaterialId(MaterialId id) override { data_.mid = id; }

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
 * @brief *MAT_FABRIC (*MAT_034)
 *
 * Fabric material model for airbag and seat belt.
 */
class KOO_API MatFabric : public CloneableKeyword<MatFabric, MaterialBase> {
public:
    struct Data {
        MaterialId mid = 0;
        double ro = 0.0;
        double ea = 0.0;        // Young's modulus in a-direction
        double eb = 0.0;        // Young's modulus in b-direction
        double ec = 0.0;        // Young's modulus in c-direction
        double prba = 0.0;      // Poisson's ratio ba
        double prca = 0.0;      // Poisson's ratio ca
        double prcb = 0.0;      // Poisson's ratio cb
        double gab = 0.0;       // Shear modulus ab
        double gbc = 0.0;       // Shear modulus bc
        double gca = 0.0;       // Shear modulus ca
        int cse = 0;            // Compressive stress elimination
        double el = 0.0;        // Locking strain
        double prl = 0.0;       // Locking Poisson's ratio
        int lratio = 0;         // Ratio of in-plane to bending stiffness
        double dtefail = 0.0;   // Element erosion time step
        int apts = 0;           // Material axes option
    };

    MatFabric() = default;

    std::string getKeywordName() const override { return "*MAT_FABRIC"; }
    MaterialType getMaterialType() const override { return MaterialType::Fabric; }

    MaterialId getMaterialId() const override { return data_.mid; }
    void setMaterialId(MaterialId id) override { data_.mid = id; }

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
 * @brief *MAT_MOONEY-RIVLIN_RUBBER (*MAT_027)
 *
 * Mooney-Rivlin rubber material model.
 */
class KOO_API MatMooneyRivlinRubber : public CloneableKeyword<MatMooneyRivlinRubber, MaterialBase> {
public:
    struct Data {
        MaterialId mid = 0;
        double ro = 0.0;        // Density
        double pr = 0.0;        // Poisson's ratio
        double a = 0.0;         // First Mooney-Rivlin constant
        double b = 0.0;         // Second Mooney-Rivlin constant
        int ref = 0;            // Use reference configuration flag
        double sgl = 0.0;       // Specimen gauge length
        double sw = 0.0;        // Specimen width
        double st = 0.0;        // Specimen thickness
    };

    MatMooneyRivlinRubber() = default;

    std::string getKeywordName() const override { return "*MAT_MOONEY-RIVLIN_RUBBER"; }
    MaterialType getMaterialType() const override { return MaterialType::Unknown; }

    MaterialId getMaterialId() const override { return data_.mid; }
    void setMaterialId(MaterialId id) override { data_.mid = id; }

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
 * @brief *MAT_LOW_DENSITY_FOAM (*MAT_057)
 *
 * Low density foam material model.
 */
class KOO_API MatLowDensityFoam : public CloneableKeyword<MatLowDensityFoam, MaterialBase> {
public:
    struct Data {
        MaterialId mid = 0;
        double ro = 0.0;        // Density
        double e = 0.0;         // Young's modulus
        int lcid = 0;           // Load curve ID for stress-strain
        double tc = 0.0;        // Tension cutoff
        double hu = 0.0;        // Hysteretic unloading factor
        double beta = 0.0;      // Shape factor for unloading
        double damp = 0.0;      // Viscous damping coefficient
        double shape = 0.0;     // Shape factor
        double fail = 0.0;      // Failure flag
        int kcon = 0;           // Bulk modulus flag
        double ref = 0.0;       // Reference flag
    };

    MatLowDensityFoam() = default;

    std::string getKeywordName() const override { return "*MAT_LOW_DENSITY_FOAM"; }
    MaterialType getMaterialType() const override { return MaterialType::Unknown; }

    MaterialId getMaterialId() const override { return data_.mid; }
    void setMaterialId(MaterialId id) override { data_.mid = id; }

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
 * @brief *MAT_ORTHOTROPIC_ELASTIC (*MAT_002)
 *
 * Orthotropic elastic material model.
 */
class KOO_API MatOrthotropicElastic : public CloneableKeyword<MatOrthotropicElastic, MaterialBase> {
public:
    struct Data {
        MaterialId mid = 0;
        double ro = 0.0;        // Density
        double ea = 0.0;        // Ea - Young's modulus in a-direction
        double eb = 0.0;        // Eb - Young's modulus in b-direction
        double ec = 0.0;        // Ec - Young's modulus in c-direction (solids)
        double prba = 0.0;      // Poisson's ratio ba
        double prca = 0.0;      // Poisson's ratio ca
        double prcb = 0.0;      // Poisson's ratio cb
        double gab = 0.0;       // Shear modulus ab
        double gbc = 0.0;       // Shear modulus bc
        double gca = 0.0;       // Shear modulus ca
        int aopt = 0;           // Material axes option
        double xp = 0.0;        // Point on local x-axis
        double yp = 0.0;
        double zp = 0.0;
        double a1 = 0.0;        // Components of a-direction vector
        double a2 = 0.0;
        double a3 = 0.0;
        double d1 = 0.0;        // Components of d-direction vector
        double d2 = 0.0;
        double d3 = 0.0;
    };

    MatOrthotropicElastic() = default;

    std::string getKeywordName() const override { return "*MAT_ORTHOTROPIC_ELASTIC"; }
    MaterialType getMaterialType() const override { return MaterialType::Unknown; }

    MaterialId getMaterialId() const override { return data_.mid; }
    void setMaterialId(MaterialId id) override { data_.mid = id; }

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
 * @brief *MAT_ENHANCED_COMPOSITE_DAMAGE (*MAT_054 / *MAT_055)
 *
 * Enhanced composite damage model with Chang-Chang failure criterion.
 */
class KOO_API MatEnhancedCompositeDamage : public CloneableKeyword<MatEnhancedCompositeDamage, MaterialBase> {
public:
    struct Data {
        MaterialId mid = 0;
        double ro = 0.0;        // Density
        double ea = 0.0;        // Ea - Young's modulus a-direction
        double eb = 0.0;        // Eb - Young's modulus b-direction
        double ec = 0.0;        // Ec - Young's modulus c-direction
        double prba = 0.0;      // Poisson's ratio ba
        double prca = 0.0;      // Poisson's ratio ca
        double prcb = 0.0;      // Poisson's ratio cb
        double gab = 0.0;       // Shear modulus ab
        double gbc = 0.0;       // Shear modulus bc
        double gca = 0.0;       // Shear modulus ca
        double kf = 0.0;        // Bulk modulus of failed material
        int aopt = 0;           // Material axes option
        // Strength parameters
        double xc = 0.0;        // Compressive strength a-direction
        double xt = 0.0;        // Tensile strength a-direction
        double yc = 0.0;        // Compressive strength b-direction
        double yt = 0.0;        // Tensile strength b-direction
        double sc = 0.0;        // Shear strength
        double crit = 0.0;      // Failure criterion flag
        double beta = 0.0;      // Weighting factor for shear term
        double pel = 0.0;       // Percentage of element layers to fail
        double epsf = 0.0;      // Fiber tensile failure strain
        double epsr = 0.0;      // Fiber compressive failure strain
        double tsmd = 0.0;      // Soft material tensile failure strain
        double soft = 0.0;      // Softening reduction factor
        // Material axes vectors
        double a1 = 0.0;
        double a2 = 0.0;
        double a3 = 0.0;
        double d1 = 0.0;
        double d2 = 0.0;
        double d3 = 0.0;
        // Additional damage parameters
        double dfailm = 0.0;    // Maximum strain for matrix failure
        double dfails = 0.0;    // Maximum strain for shear failure
        double dfailt = 0.0;    // Maximum strain for tensile fiber failure
        double dfailc = 0.0;    // Maximum strain for compressive fiber failure
        double efs = 0.0;       // Effective strain at failure
        double alph = 0.0;      // Shear stress nonlinearity
        double soft2 = 0.0;     // Softening factor 2
        int tfail = 0;          // Time step for failure
        double fbrt = 0.0;      // Fiber bridging reduction factor
        double ycfac = 0.0;     // Yc factor
        double slimt1 = 0.0;    // Limit on reduction factor
        double slimc1 = 0.0;    // Limit on reduction factor
        double slimt2 = 0.0;
        double slimc2 = 0.0;
        double slims = 0.0;
        double ncyred = 0.0;    // Number of cycles for reduction
    };

    MatEnhancedCompositeDamage() = default;

    std::string getKeywordName() const override { return "*MAT_ENHANCED_COMPOSITE_DAMAGE"; }
    MaterialType getMaterialType() const override { return MaterialType::Unknown; }

    MaterialId getMaterialId() const override { return data_.mid; }
    void setMaterialId(MaterialId id) override { data_.mid = id; }

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
 * @brief *MAT_LAMINATED_COMPOSITE_FABRIC (*MAT_058)
 *
 * Laminated composite fabric material model.
 */
class KOO_API MatLaminatedCompositeFabric : public CloneableKeyword<MatLaminatedCompositeFabric, MaterialBase> {
public:
    struct Data {
        MaterialId mid = 0;
        double ro = 0.0;        // Density
        double ea = 0.0;        // Ea - Young's modulus a-direction
        double eb = 0.0;        // Eb - Young's modulus b-direction
        double ec = 0.0;        // Ec - Young's modulus c-direction
        double prba = 0.0;      // Poisson's ratio ba
        double prca = 0.0;      // Poisson's ratio ca
        double prcb = 0.0;      // Poisson's ratio cb
        double gab = 0.0;       // Shear modulus ab
        double gbc = 0.0;       // Shear modulus bc
        double gca = 0.0;       // Shear modulus ca
        int aopt = 0;           // Material axes option
        double tsize = 0.0;     // Reduction factor for time step
        double erods = 0.0;     // Maximum effective strain
        double soft = 0.0;      // Softening reduction factor
        int fs = 0;             // Failure surface type
        // Strength parameters
        double xt = 0.0;        // a-direction tensile strength
        double xc = 0.0;        // a-direction compressive strength
        double yt = 0.0;        // b-direction tensile strength
        double yc = 0.0;        // b-direction compressive strength
        double sc = 0.0;        // Shear strength ab
        // Strain softening parameters
        double e11c = 0.0;      // Compressive strain at which a-direction
        double e11t = 0.0;      // Tensile strain at which a-direction
        double e22c = 0.0;      // Compressive strain b-direction
        double e22t = 0.0;      // Tensile strain b-direction
        double gms = 0.0;       // Strain at shear strength
        // Material axes vectors
        double xp = 0.0;
        double yp = 0.0;
        double zp = 0.0;
        double a1 = 0.0;
        double a2 = 0.0;
        double a3 = 0.0;
        double d1 = 0.0;
        double d2 = 0.0;
        double d3 = 0.0;
        // Additional parameters
        double slimt1 = 0.0;
        double slimc1 = 0.0;
        double slimt2 = 0.0;
        double slimc2 = 0.0;
        double slims = 0.0;
        int lcxc = 0;           // Load curve for xc vs strain rate
        int lcxt = 0;           // Load curve for xt vs strain rate
        int lcyc = 0;
        int lcyt = 0;
        int lcsc = 0;
        double dt = 0.0;        // Time step
    };

    MatLaminatedCompositeFabric() = default;

    std::string getKeywordName() const override { return "*MAT_LAMINATED_COMPOSITE_FABRIC"; }
    MaterialType getMaterialType() const override { return MaterialType::Unknown; }

    MaterialId getMaterialId() const override { return data_.mid; }
    void setMaterialId(MaterialId id) override { data_.mid = id; }

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
 * @brief *MAT_ELASTIC_PLASTIC_THERMAL (*MAT_004)
 *
 * Elastic-plastic material with thermal effects.
 */
class KOO_API MatElasticPlasticThermal : public CloneableKeyword<MatElasticPlasticThermal, MaterialBase> {
public:
    struct Data {
        MaterialId mid = 0;
        double ro = 0.0;        // Density
        double e = 0.0;         // Young's modulus
        double pr = 0.0;        // Poisson's ratio
        double sigy = 0.0;      // Yield stress
        double etan = 0.0;      // Tangent modulus
        double alpha = 0.0;     // Coefficient of thermal expansion
        int lcss = 0;           // Load curve for stress-strain
        int lcth = 0;           // Load curve for thermal effects
        double th0 = 0.0;       // Reference temperature
    };

    MatElasticPlasticThermal() = default;

    std::string getKeywordName() const override { return "*MAT_ELASTIC_PLASTIC_THERMAL"; }
    MaterialType getMaterialType() const override { return MaterialType::Unknown; }

    MaterialId getMaterialId() const override { return data_.mid; }
    void setMaterialId(MaterialId id) override { data_.mid = id; }

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
 * @brief *MAT_SOIL_AND_FOAM (*MAT_005)
 *
 * Soil and foam material model with pressure-dependent yield surface.
 */
class KOO_API MatSoilAndFoam : public CloneableKeyword<MatSoilAndFoam, MaterialBase> {
public:
    struct Data {
        MaterialId mid = 0;
        double ro = 0.0;        // Density
        double g = 0.0;         // Shear modulus
        double ku = 0.0;        // Bulk modulus (unloading)
        double a0 = 0.0;        // Yield function constant a0
        double a1 = 0.0;        // Yield function constant a1
        double a2 = 0.0;        // Yield function constant a2
        double pc = 0.0;        // Pressure cutoff for tensile fracture
        double vcr = 0.0;       // Volumetric crushing option
        double ref = 0.0;       // Reference geometry
        int lcid = 0;           // Load curve ID for pressure vs volumetric strain
        std::vector<std::pair<double, double>> eps_p; // Volumetric strain, pressure pairs
    };

    MatSoilAndFoam() = default;

    std::string getKeywordName() const override { return "*MAT_SOIL_AND_FOAM"; }
    MaterialType getMaterialType() const override { return MaterialType::Unknown; }

    MaterialId getMaterialId() const override { return data_.mid; }
    void setMaterialId(MaterialId id) override { data_.mid = id; }

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
 * @brief *MAT_ELASTIC_PLASTIC_HYDRO (*MAT_010)
 *
 * Elastic-plastic hydrodynamic material model.
 */
class KOO_API MatElasticPlasticHydro : public CloneableKeyword<MatElasticPlasticHydro, MaterialBase> {
public:
    struct Data {
        MaterialId mid = 0;
        double ro = 0.0;        // Density
        double g = 0.0;         // Shear modulus
        double sigy = 0.0;      // Yield stress
        double eh = 0.0;        // Hardening modulus
        double pc = 0.0;        // Pressure cutoff
        double fs = 0.0;        // Failure stress
        int eosid = 0;          // Equation of state ID
    };

    MatElasticPlasticHydro() = default;

    std::string getKeywordName() const override { return "*MAT_ELASTIC_PLASTIC_HYDRO"; }
    MaterialType getMaterialType() const override { return MaterialType::Unknown; }

    MaterialId getMaterialId() const override { return data_.mid; }
    void setMaterialId(MaterialId id) override { data_.mid = id; }

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
 * @brief *MAT_COMPOSITE_DAMAGE (*MAT_022)
 *
 * Composite damage model with orthotropic properties.
 */
class KOO_API MatCompositeDamage : public CloneableKeyword<MatCompositeDamage, MaterialBase> {
public:
    struct Data {
        MaterialId mid = 0;
        double ro = 0.0;        // Density
        double ea = 0.0;        // Young's modulus a-direction
        double eb = 0.0;        // Young's modulus b-direction
        double ec = 0.0;        // Young's modulus c-direction
        double prba = 0.0;      // Poisson's ratio ba
        double prca = 0.0;      // Poisson's ratio ca
        double prcb = 0.0;      // Poisson's ratio cb
        double gab = 0.0;       // Shear modulus ab
        double gbc = 0.0;       // Shear modulus bc
        double gca = 0.0;       // Shear modulus ca
        int aopt = 0;           // Material axes option
        double sc = 0.0;        // Shear strength
        double xt = 0.0;        // Tensile strength a-direction
        double yt = 0.0;        // Tensile strength b-direction
        double yc = 0.0;        // Compressive strength b-direction
        double alph = 0.0;      // Shear stress weighting factor
        double sn = 0.0;        // Normal tensile strength
        double syz = 0.0;       // Out-of-plane shear strength
        double szx = 0.0;       // Out-of-plane shear strength
        double a1 = 0.0;        // Material axes components
        double a2 = 0.0;
        double a3 = 0.0;
        double d1 = 0.0;
        double d2 = 0.0;
        double d3 = 0.0;
        double beta = 0.0;      // Damage softening parameter
    };

    MatCompositeDamage() = default;

    std::string getKeywordName() const override { return "*MAT_COMPOSITE_DAMAGE"; }
    MaterialType getMaterialType() const override { return MaterialType::Unknown; }

    MaterialId getMaterialId() const override { return data_.mid; }
    void setMaterialId(MaterialId id) override { data_.mid = id; }

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
 * @brief *MAT_GEOLOGIC_CAP_MODEL (*MAT_025)
 *
 * Geologic cap model for soil and rock materials.
 */
class KOO_API MatGeologicCapModel : public CloneableKeyword<MatGeologicCapModel, MaterialBase> {
public:
    struct Data {
        MaterialId mid = 0;
        double ro = 0.0;        // Density
        double bulk = 0.0;      // Bulk modulus
        double g = 0.0;         // Shear modulus
        double alpha = 0.0;     // Failure envelope parameter
        double theta = 0.0;     // Failure envelope angle
        double gamma = 0.0;     // Failure envelope exponential
        double beta = 0.0;      // Cap hardening parameter
        double r = 0.0;         // Cap aspect ratio
        double d = 0.0;         // Hardening law exponent
        double w = 0.0;         // Hardening law coefficient
        double x0 = 0.0;        // Initial cap position
        double tcut = 0.0;      // Tension cutoff
        double conv1 = 0.0;     // Convergence tolerance 1
        double conv2 = 0.0;     // Convergence tolerance 2
    };

    MatGeologicCapModel() = default;

    std::string getKeywordName() const override { return "*MAT_GEOLOGIC_CAP_MODEL"; }
    MaterialType getMaterialType() const override { return MaterialType::Unknown; }

    MaterialId getMaterialId() const override { return data_.mid; }
    void setMaterialId(MaterialId id) override { data_.mid = id; }

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
 * @brief *MAT_PLASTICITY_WITH_DAMAGE (*MAT_081)
 *
 * Plasticity with damage material model.
 */
class KOO_API MatPlasticityWithDamage : public CloneableKeyword<MatPlasticityWithDamage, MaterialBase> {
public:
    struct Data {
        MaterialId mid = 0;
        double ro = 0.0;        // Density
        double e = 0.0;         // Young's modulus
        double pr = 0.0;        // Poisson's ratio
        double sigy = 0.0;      // Initial yield stress
        double etan = 0.0;      // Plastic hardening modulus
        double fail = 0.0;      // Failure strain
        double tdel = 0.0;      // Time step for element deletion
        double c = 0.0;         // Strain rate parameter C
        double p = 0.0;         // Strain rate parameter P
        int lcss = 0;           // Load curve ID for stress-strain
        int lcsr = 0;           // Load curve ID for strain rate scaling
        double vp = 0.0;        // Viscoplastic formulation
        double eppf = 0.0;      // Effective plastic strain at damage start
        double dtefail = 0.0;   // Element deletion time step
        int numint = 0;         // Number of integration points for failure
        // Damage parameters
        double dc = 0.0;        // Critical damage value
        double flag = 0.0;      // Damage flag
    };

    MatPlasticityWithDamage() = default;

    std::string getKeywordName() const override { return "*MAT_PLASTICITY_WITH_DAMAGE"; }
    MaterialType getMaterialType() const override { return MaterialType::Unknown; }

    MaterialId getMaterialId() const override { return data_.mid; }
    void setMaterialId(MaterialId id) override { data_.mid = id; }

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
 * @brief *MAT_SIMPLIFIED_JOHNSON_COOK (*MAT_098)
 *
 * Simplified Johnson-Cook plasticity model.
 */
class KOO_API MatSimplifiedJohnsonCook : public CloneableKeyword<MatSimplifiedJohnsonCook, MaterialBase> {
public:
    struct Data {
        MaterialId mid = 0;
        double ro = 0.0;        // Density
        double e = 0.0;         // Young's modulus
        double pr = 0.0;        // Poisson's ratio
        double vp = 0.0;        // Viscoplastic formulation
        double a = 0.0;         // Yield stress constant
        double b = 0.0;         // Strain hardening coefficient
        double n = 0.0;         // Strain hardening exponent
        double c = 0.0;         // Strain rate coefficient
        double psfail = 0.0;    // Effective plastic strain at failure
        double sigmax = 0.0;    // Maximum flow stress
        double sigsat = 0.0;    // Saturation stress
        double epso = 1.0;      // Reference strain rate
    };

    MatSimplifiedJohnsonCook() = default;

    std::string getKeywordName() const override { return "*MAT_SIMPLIFIED_JOHNSON_COOK"; }
    MaterialType getMaterialType() const override { return MaterialType::Unknown; }

    MaterialId getMaterialId() const override { return data_.mid; }
    void setMaterialId(MaterialId id) override { data_.mid = id; }

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
 * @brief *MAT_SAMP-1 (*MAT_187)
 *
 * Semi-Analytical Model for Polymers.
 * Advanced material model for polymers with pressure dependency and rate effects.
 */
class KOO_API MatSamp1 : public CloneableKeyword<MatSamp1, MaterialBase> {
public:
    struct Data {
        MaterialId mid = 0;
        double ro = 0.0;        // Density
        double e = 0.0;         // Young's modulus
        double pr = 0.0;        // Poisson's ratio
        // Card 2
        int lcid_t = 0;         // Load curve for tensile stress-strain
        int lcid_c = 0;         // Load curve for compressive stress-strain
        int lcid_s = 0;         // Load curve for shear stress-strain
        int lcid_b = 0;         // Load curve for biaxial stress-strain
        double beta_t = 0.0;    // Hardening parameter (tension)
        double beta_c = 0.0;    // Hardening parameter (compression)
        double beta_s = 0.0;    // Hardening parameter (shear)
        double beta_b = 0.0;    // Hardening parameter (biaxial)
        // Card 3
        double na = 0.0;        // Exponent for associated flow
        double lcid_r = 0;      // Load curve for strain rate
        double fi = 0.0;        // Failure initiation parameter
        double macf = 0.0;      // Material axes change flag
        double apts = 0.0;      // Material axes option
        double hta = 0.0;       // Triaxiality hardening parameter
        double soft = 1.0;      // Softening factor
        double ihis = 0;        // History variable flag
        // Card 4
        double ivol = 0.0;      // Volumetric response flag
        double ftype = 0.0;     // Failure type
        double fpar1 = 0.0;     // Failure parameter 1
        double fpar2 = 0.0;     // Failure parameter 2
        double fpar3 = 0.0;     // Failure parameter 3
        double fpar4 = 0.0;     // Failure parameter 4
        double eppfr = 0.0;     // Plastic strain at failure
        double numfi = 0.0;     // Number of failed integration points
    };

    MatSamp1() = default;

    std::string getKeywordName() const override { return "*MAT_SAMP-1"; }
    MaterialType getMaterialType() const override { return MaterialType::Unknown; }

    MaterialId getMaterialId() const override { return data_.mid; }
    void setMaterialId(MaterialId id) override { data_.mid = id; }

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
 * @brief *MAT_ORTHOTROPIC_ELASTIC_PLASTIC (*MAT_108)
 *
 * Orthotropic elastic-plastic material model with Hill's yield criterion.
 */
class KOO_API MatOrthoElasticPlastic : public CloneableKeyword<MatOrthoElasticPlastic, MaterialBase> {
public:
    struct Data {
        MaterialId mid = 0;
        double ro = 0.0;        // Density
        // Card 1 - elastic constants
        double ea = 0.0;        // Young's modulus in a-direction
        double eb = 0.0;        // Young's modulus in b-direction
        double ec = 0.0;        // Young's modulus in c-direction
        double prba = 0.0;      // Poisson's ratio ba
        double prca = 0.0;      // Poisson's ratio ca
        double prcb = 0.0;      // Poisson's ratio cb
        // Card 2 - shear moduli and yield
        double gab = 0.0;       // Shear modulus ab
        double gbc = 0.0;       // Shear modulus bc
        double gca = 0.0;       // Shear modulus ca
        double sigy = 0.0;      // Yield stress
        double etan = 0.0;      // Tangent modulus
        int iopt = 0;           // Formulation option
        int macf = 1;           // Material axes change flag
        // Card 3 - Hill parameters
        double r00 = 1.0;       // R-value 0 degrees
        double r45 = 1.0;       // R-value 45 degrees
        double r90 = 1.0;       // R-value 90 degrees
        int lcid = 0;           // Load curve ID for hardening
        double beta = 0.0;      // Material angle
    };

    MatOrthoElasticPlastic() = default;

    std::string getKeywordName() const override { return "*MAT_ORTHOTROPIC_ELASTIC_PLASTIC"; }
    MaterialType getMaterialType() const override { return MaterialType::Unknown; }

    MaterialId getMaterialId() const override { return data_.mid; }
    void setMaterialId(MaterialId id) override { data_.mid = id; }

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
 * @brief *MAT_HIGH_EXPLOSIVE_BURN (*MAT_008)
 *
 * High explosive material with burn characteristics.
 * Used with JWL equation of state for detonation modeling.
 */
class KOO_API MatHighExplosiveBurn : public CloneableKeyword<MatHighExplosiveBurn, MaterialBase> {
public:
    struct Data {
        MaterialId mid = 0;
        double ro = 0.0;        // Density
        double d = 0.0;         // Detonation velocity
        double pcj = 0.0;       // Chapman-Jouget pressure
        double beta = 0.0;      // Beta burn flag
        double k = 0.0;         // K parameter
        double g = 0.0;         // G parameter
        double sigy = 0.0;      // Yield stress (optional)
    };

    MatHighExplosiveBurn() = default;

    std::string getKeywordName() const override { return "*MAT_HIGH_EXPLOSIVE_BURN"; }
    MaterialType getMaterialType() const override { return MaterialType::Unknown; }

    MaterialId getMaterialId() const override { return data_.mid; }
    void setMaterialId(MaterialId id) override { data_.mid = id; }

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
 * @brief *MAT_BLATZ_KO_RUBBER (*MAT_007)
 *
 * Blatz-Ko rubber model for highly compressible foams.
 */
class KOO_API MatBlatzKoRubber : public CloneableKeyword<MatBlatzKoRubber, MaterialBase> {
public:
    struct Data {
        MaterialId mid = 0;
        double ro = 0.0;        // Density
        double g = 0.0;         // Shear modulus
        double ref = 0.0;       // Reference flag (0=model 1, 1=model 2)
    };

    MatBlatzKoRubber() = default;

    std::string getKeywordName() const override { return "*MAT_BLATZ_KO_RUBBER"; }
    MaterialType getMaterialType() const override { return MaterialType::Unknown; }

    MaterialId getMaterialId() const override { return data_.mid; }
    void setMaterialId(MaterialId id) override { data_.mid = id; }

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
 * @brief *MAT_STEINBERG (*MAT_011)
 *
 * Steinberg-Guinan rate-dependent plasticity model for metals at high strain rates.
 */
class KOO_API MatSteinberg : public CloneableKeyword<MatSteinberg, MaterialBase> {
public:
    struct Data {
        MaterialId mid = 0;
        double ro = 0.0;        // Density
        double g = 0.0;         // Shear modulus
        double y0 = 0.0;        // Initial yield stress
        double ymax = 0.0;      // Maximum yield stress
        double b = 0.0;         // Hardening parameter
        double n = 0.0;         // Hardening exponent
        double h = 0.0;         // Thermal softening coefficient
        double tm = 0.0;        // Melt temperature
    };

    MatSteinberg() = default;

    std::string getKeywordName() const override { return "*MAT_STEINBERG"; }
    MaterialType getMaterialType() const override { return MaterialType::Unknown; }

    MaterialId getMaterialId() const override { return data_.mid; }
    void setMaterialId(MaterialId id) override { data_.mid = id; }

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
 * @brief *MAT_ISOTROPIC_ELASTIC_FAILURE (*MAT_013)
 *
 * Isotropic elastic material with failure criteria.
 */
class KOO_API MatIsotropicElasticFailure : public CloneableKeyword<MatIsotropicElasticFailure, MaterialBase> {
public:
    struct Data {
        MaterialId mid = 0;
        double ro = 0.0;        // Density
        double e = 0.0;         // Young's modulus
        double pr = 0.0;        // Poisson's ratio
        double sigy = 0.0;      // Yield stress (optional)
        double etan = 0.0;      // Tangent modulus
        double bulk = 0.0;      // Bulk modulus (optional)
        double psfail = 0.0;    // Plastic strain at failure
    };

    MatIsotropicElasticFailure() = default;

    std::string getKeywordName() const override { return "*MAT_ISOTROPIC_ELASTIC_FAILURE"; }
    MaterialType getMaterialType() const override { return MaterialType::Elastic; }

    MaterialId getMaterialId() const override { return data_.mid; }
    void setMaterialId(MaterialId id) override { data_.mid = id; }

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
 * @brief *MAT_ISOTROPIC_ELASTIC_PLASTIC (*MAT_012)
 *
 * Simple isotropic elastic-plastic model with linear hardening.
 */
class KOO_API MatIsotropicElasticPlastic : public CloneableKeyword<MatIsotropicElasticPlastic, MaterialBase> {
public:
    struct Data {
        MaterialId mid = 0;
        double ro = 0.0;        // Density
        double e = 0.0;         // Young's modulus
        double pr = 0.0;        // Poisson's ratio
        double sigy = 0.0;      // Yield stress
        double etan = 0.0;      // Tangent modulus
        double bulk = 0.0;      // Bulk modulus (optional)
    };

    MatIsotropicElasticPlastic() = default;

    std::string getKeywordName() const override { return "*MAT_ISOTROPIC_ELASTIC_PLASTIC"; }
    MaterialType getMaterialType() const override { return MaterialType::Unknown; }

    MaterialId getMaterialId() const override { return data_.mid; }
    void setMaterialId(MaterialId id) override { data_.mid = id; }

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
 * @brief *MAT_SOIL_AND_FOAM_FAILURE (*MAT_014)
 *
 * Soil and foam material model with failure criteria.
 */
class KOO_API MatSoilAndFoamFailure : public CloneableKeyword<MatSoilAndFoamFailure, MaterialBase> {
public:
    struct Data {
        MaterialId mid = 0;
        double ro = 0.0;        // Density
        double g = 0.0;         // Shear modulus
        double bulk = 0.0;      // Bulk unloading modulus
        double a0 = 0.0;        // Yield function constant
        double a1 = 0.0;        // Yield function linear coefficient
        double a2 = 0.0;        // Yield function quadratic coefficient
        double pc = 0.0;        // Pressure cutoff
        double vcr = 0.0;       // Volumetric crushing option
    };

    MatSoilAndFoamFailure() = default;

    std::string getKeywordName() const override { return "*MAT_SOIL_AND_FOAM_FAILURE"; }
    MaterialType getMaterialType() const override { return MaterialType::Unknown; }

    MaterialId getMaterialId() const override { return data_.mid; }
    void setMaterialId(MaterialId id) override { data_.mid = id; }

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
 * @brief *MAT_PSEUDO_TENSOR (*MAT_016)
 *
 * Pseudo-tensor geological material model.
 */
class KOO_API MatPseudoTensor : public CloneableKeyword<MatPseudoTensor, MaterialBase> {
public:
    struct Data {
        MaterialId mid = 0;
        double ro = 0.0;        // Density
        double g = 0.0;         // Shear modulus
        double pr = 0.0;        // Poisson's ratio
        double sigy = 0.0;      // Yield stress
        double etan = 0.0;      // Tangent modulus
        double lcp = 0.0;       // Plastic strain rate curve ID
        double lcr = 0.0;       // Strain rate dependent curve ID
    };

    MatPseudoTensor() = default;

    std::string getKeywordName() const override { return "*MAT_PSEUDO_TENSOR"; }
    MaterialType getMaterialType() const override { return MaterialType::Unknown; }

    MaterialId getMaterialId() const override { return data_.mid; }
    void setMaterialId(MaterialId id) override { data_.mid = id; }

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
 * @brief *MAT_ORIENTED_CRACK (*MAT_017)
 *
 * Oriented crack model for concrete-like materials.
 */
class KOO_API MatOrientedCrack : public CloneableKeyword<MatOrientedCrack, MaterialBase> {
public:
    struct Data {
        MaterialId mid = 0;
        double ro = 0.0;        // Density
        double e = 0.0;         // Young's modulus
        double pr = 0.0;        // Poisson's ratio
        double sigy = 0.0;      // Yield stress
        double etan = 0.0;      // Tangent modulus
        double fs = 0.0;        // Failure strain
        double soft = 0.0;      // Softening parameter
    };

    MatOrientedCrack() = default;

    std::string getKeywordName() const override { return "*MAT_ORIENTED_CRACK"; }
    MaterialType getMaterialType() const override { return MaterialType::Unknown; }

    MaterialId getMaterialId() const override { return data_.mid; }
    void setMaterialId(MaterialId id) override { data_.mid = id; }

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
 * @brief *MAT_STRAIN_RATE_DEPENDENT_PLASTICITY (*MAT_019)
 *
 * Strain rate dependent plasticity model.
 */
class KOO_API MatStrainRateDependentPlasticity : public CloneableKeyword<MatStrainRateDependentPlasticity, MaterialBase> {
public:
    struct Data {
        MaterialId mid = 0;
        double ro = 0.0;        // Density
        double e = 0.0;         // Young's modulus
        double pr = 0.0;        // Poisson's ratio
        double sigy = 0.0;      // Yield stress
        double etan = 0.0;      // Tangent modulus
        double c = 0.0;         // Strain rate parameter C
        double p = 0.0;         // Strain rate parameter P
        double lcss = 0.0;      // Load curve ID for stress-strain
    };

    MatStrainRateDependentPlasticity() = default;

    std::string getKeywordName() const override { return "*MAT_STRAIN_RATE_DEPENDENT_PLASTICITY"; }
    MaterialType getMaterialType() const override { return MaterialType::Unknown; }

    MaterialId getMaterialId() const override { return data_.mid; }
    void setMaterialId(MaterialId id) override { data_.mid = id; }

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
 * @brief *MAT_THERMAL_ORTHOTROPIC (*MAT_021)
 *
 * Orthotropic elastic material with thermal effects.
 */
class KOO_API MatThermalOrthotropic : public CloneableKeyword<MatThermalOrthotropic, MaterialBase> {
public:
    struct Data {
        MaterialId mid = 0;
        double ro = 0.0;        // Density
        double ea = 0.0;        // Young's modulus in a-direction
        double eb = 0.0;        // Young's modulus in b-direction
        double ec = 0.0;        // Young's modulus in c-direction
        double prba = 0.0;      // Poisson's ratio ba
        double prca = 0.0;      // Poisson's ratio ca
        double prcb = 0.0;      // Poisson's ratio cb
    };

    MatThermalOrthotropic() = default;

    std::string getKeywordName() const override { return "*MAT_THERMAL_ORTHOTROPIC"; }
    MaterialType getMaterialType() const override { return MaterialType::Unknown; }

    MaterialId getMaterialId() const override { return data_.mid; }
    void setMaterialId(MaterialId id) override { data_.mid = id; }

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
 * @brief *MAT_TEMPERATURE_DEPENDENT_ORTHOTROPIC (*MAT_023)
 *
 * Temperature dependent orthotropic material.
 */
class KOO_API MatTempDependentOrthotropic : public CloneableKeyword<MatTempDependentOrthotropic, MaterialBase> {
public:
    struct Data {
        MaterialId mid = 0;
        double ro = 0.0;        // Density
        double ea = 0.0;        // Young's modulus in a-direction
        double eb = 0.0;        // Young's modulus in b-direction
        double ec = 0.0;        // Young's modulus in c-direction
        double prba = 0.0;      // Poisson's ratio ba
        double prca = 0.0;      // Poisson's ratio ca
        double prcb = 0.0;      // Poisson's ratio cb
    };

    MatTempDependentOrthotropic() = default;

    std::string getKeywordName() const override { return "*MAT_TEMPERATURE_DEPENDENT_ORTHOTROPIC"; }
    MaterialType getMaterialType() const override { return MaterialType::Unknown; }

    MaterialId getMaterialId() const override { return data_.mid; }
    void setMaterialId(MaterialId id) override { data_.mid = id; }

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
 * @brief *MAT_RESULTANT_PLASTICITY (*MAT_028)
 *
 * Resultant plasticity model for shell elements.
 * Uses resultant forces and moments for plasticity.
 */
class KOO_API MatResultantPlasticity : public CloneableKeyword<MatResultantPlasticity, MaterialBase> {
public:
    struct Data {
        MaterialId mid = 0;
        double ro = 0.0;        // Density
        double e = 0.0;         // Young's modulus
        double pr = 0.0;        // Poisson's ratio
        double sigy = 0.0;      // Yield stress
        double etan = 0.0;      // Tangent modulus
        double fail = 0.0;      // Failure flag
        double tdel = 0.0;      // Time step for deletion
        double c = 0.0;         // Strain rate parameter C
        double p = 0.0;         // Strain rate parameter P
    };

    MatResultantPlasticity() = default;

    std::string getKeywordName() const override { return "*MAT_RESULTANT_PLASTICITY"; }
    MaterialType getMaterialType() const override { return MaterialType::Unknown; }

    MaterialId getMaterialId() const override { return data_.mid; }
    void setMaterialId(MaterialId id) override { data_.mid = id; }

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
 * @brief *MAT_FORCE_LIMITED (*MAT_029)
 *
 * Force-limited resultant formulation for shell elements.
 */
class KOO_API MatForceLimited : public CloneableKeyword<MatForceLimited, MaterialBase> {
public:
    struct Data {
        MaterialId mid = 0;
        double ro = 0.0;        // Density
        double e = 0.0;         // Young's modulus
        double pr = 0.0;        // Poisson's ratio
        double df = 0.0;        // Damping factor
        double apts = 0.0;      // Material axes option
        double failm = 0.0;     // Failure mode
        int lcid = 0;           // Load curve ID for force limit
    };

    MatForceLimited() = default;

    std::string getKeywordName() const override { return "*MAT_FORCE_LIMITED"; }
    MaterialType getMaterialType() const override { return MaterialType::Unknown; }

    MaterialId getMaterialId() const override { return data_.mid; }
    void setMaterialId(MaterialId id) override { data_.mid = id; }

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
 * @brief *MAT_SHAPE_MEMORY (*MAT_030)
 *
 * Shape memory alloy material model.
 */
class KOO_API MatShapeMemory : public CloneableKeyword<MatShapeMemory, MaterialBase> {
public:
    struct Data {
        MaterialId mid = 0;
        double ro = 0.0;        // Density
        double e = 0.0;         // Young's modulus (austenite)
        double pr = 0.0;        // Poisson's ratio
        double em = 0.0;        // Young's modulus (martensite)
        double sigasm = 0.0;    // Start transformation stress (A->M)
        double sigafm = 0.0;    // Finish transformation stress (A->M)
        double sigsma = 0.0;    // Start transformation stress (M->A)
        double sigfma = 0.0;    // Finish transformation stress (M->A)
        double eps_l = 0.0;     // Maximum residual strain
    };

    MatShapeMemory() = default;

    std::string getKeywordName() const override { return "*MAT_SHAPE_MEMORY"; }
    MaterialType getMaterialType() const override { return MaterialType::Unknown; }

    MaterialId getMaterialId() const override { return data_.mid; }
    void setMaterialId(MaterialId id) override { data_.mid = id; }

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
 * @brief *MAT_FRAZER_NASH_RUBBER (*MAT_031)
 *
 * Frazer-Nash rubber model.
 */
class KOO_API MatFrazerNashRubber : public CloneableKeyword<MatFrazerNashRubber, MaterialBase> {
public:
    struct Data {
        MaterialId mid = 0;
        double ro = 0.0;        // Density
        double pr = 0.0;        // Poisson's ratio
        double n = 0.0;         // Number of chain segments
        double g = 0.0;         // Shear modulus
        double sigf = 0.0;      // Limit stress for erosion
        double ref = 0.0;       // Use reference geometry
    };

    MatFrazerNashRubber() = default;

    std::string getKeywordName() const override { return "*MAT_FRAZER_NASH_RUBBER"; }
    MaterialType getMaterialType() const override { return MaterialType::Unknown; }

    MaterialId getMaterialId() const override { return data_.mid; }
    void setMaterialId(MaterialId id) override { data_.mid = id; }

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
 * @brief *MAT_LAMINATED_GLASS (*MAT_032)
 *
 * Laminated glass material for windshield modeling.
 */
class KOO_API MatLaminatedGlass : public CloneableKeyword<MatLaminatedGlass, MaterialBase> {
public:
    struct Data {
        MaterialId mid = 0;
        double ro = 0.0;        // Density
        double e = 0.0;         // Young's modulus
        double pr = 0.0;        // Poisson's ratio
        double sigy = 0.0;      // Yield stress
        double etan = 0.0;      // Tangent modulus
        double fail = 0.0;      // Failure strain
        double tdel = 0.0;      // Time step for deletion
    };

    MatLaminatedGlass() = default;

    std::string getKeywordName() const override { return "*MAT_LAMINATED_GLASS"; }
    MaterialType getMaterialType() const override { return MaterialType::Unknown; }

    MaterialId getMaterialId() const override { return data_.mid; }
    void setMaterialId(MaterialId id) override { data_.mid = id; }

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
 * @brief *MAT_BARLAT_ANISOTROPIC_PLASTICITY (*MAT_033)
 *
 * Barlat anisotropic plasticity for sheet metal forming.
 */
class KOO_API MatBarlatAnisotropicPlasticity : public CloneableKeyword<MatBarlatAnisotropicPlasticity, MaterialBase> {
public:
    struct Data {
        MaterialId mid = 0;
        double ro = 0.0;        // Density
        double e = 0.0;         // Young's modulus
        double pr = 0.0;        // Poisson's ratio
        double sigy = 0.0;      // Initial yield stress
        int lcid = 0;           // Load curve ID for hardening
        double m = 0.0;         // Exponent for yield surface
        double r00 = 1.0;       // R-value 0 degrees
        double r45 = 1.0;       // R-value 45 degrees
        double r90 = 1.0;       // R-value 90 degrees
        double cb = 0.0;        // Barlat coefficient
    };

    MatBarlatAnisotropicPlasticity() = default;

    std::string getKeywordName() const override { return "*MAT_BARLAT_ANISOTROPIC_PLASTICITY"; }
    MaterialType getMaterialType() const override { return MaterialType::Unknown; }

    MaterialId getMaterialId() const override { return data_.mid; }
    void setMaterialId(MaterialId id) override { data_.mid = id; }

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
 * @brief *MAT_SPRING_ELASTIC (*MAT_S01)
 *
 * Linear elastic spring material for discrete elements.
 */
class KOO_API MatSpringElastic : public CloneableKeyword<MatSpringElastic, MaterialBase> {
public:
    struct Data {
        MaterialId mid = 0;
        double k = 0.0;         // Spring stiffness
        double f0 = 0.0;        // Optional preload
    };

    MatSpringElastic() = default;

    std::string getKeywordName() const override { return "*MAT_SPRING_ELASTIC"; }
    MaterialType getMaterialType() const override { return MaterialType::Unknown; }

    MaterialId getMaterialId() const override { return data_.mid; }
    void setMaterialId(MaterialId id) override { data_.mid = id; }

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
 * @brief *MAT_DAMPER_VISCOUS (*MAT_S02)
 *
 * Linear viscous damper material for discrete elements.
 */
class KOO_API MatDamperViscous : public CloneableKeyword<MatDamperViscous, MaterialBase> {
public:
    struct Data {
        MaterialId mid = 0;
        double dc = 0.0;        // Damping coefficient
    };

    MatDamperViscous() = default;

    std::string getKeywordName() const override { return "*MAT_DAMPER_VISCOUS"; }
    MaterialType getMaterialType() const override { return MaterialType::Unknown; }

    MaterialId getMaterialId() const override { return data_.mid; }
    void setMaterialId(MaterialId id) override { data_.mid = id; }

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
 * @brief *MAT_SPRING_NONLINEAR_ELASTIC (*MAT_S03)
 *
 * Nonlinear elastic spring using load curve.
 */
class KOO_API MatSpringNonlinearElastic : public CloneableKeyword<MatSpringNonlinearElastic, MaterialBase> {
public:
    struct Data {
        MaterialId mid = 0;
        int lcid = 0;           // Load curve ID for force vs displacement
        double stiff = 0.0;     // Initial stiffness (optional)
    };

    MatSpringNonlinearElastic() = default;

    std::string getKeywordName() const override { return "*MAT_SPRING_NONLINEAR_ELASTIC"; }
    MaterialType getMaterialType() const override { return MaterialType::Unknown; }

    MaterialId getMaterialId() const override { return data_.mid; }
    void setMaterialId(MaterialId id) override { data_.mid = id; }

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
 * @brief *MAT_SPRING_ELASTOPLASTIC (*MAT_S04)
 *
 * Elastoplastic spring material.
 */
class KOO_API MatSpringElastoplastic : public CloneableKeyword<MatSpringElastoplastic, MaterialBase> {
public:
    struct Data {
        MaterialId mid = 0;
        int lcid = 0;           // Load curve ID for yield
        double k = 0.0;         // Elastic stiffness
        double fy = 0.0;        // Yield force
        double beta = 0.0;      // Hardening parameter
    };

    MatSpringElastoplastic() = default;

    std::string getKeywordName() const override { return "*MAT_SPRING_ELASTOPLASTIC"; }
    MaterialType getMaterialType() const override { return MaterialType::Unknown; }

    MaterialId getMaterialId() const override { return data_.mid; }
    void setMaterialId(MaterialId id) override { data_.mid = id; }

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
 * @brief *MAT_SPRING_GENERAL_NONLINEAR (*MAT_S05)
 *
 * General nonlinear spring with separate loading/unloading curves.
 */
class KOO_API MatSpringGeneralNonlinear : public CloneableKeyword<MatSpringGeneralNonlinear, MaterialBase> {
public:
    struct Data {
        MaterialId mid = 0;
        int lcidl = 0;          // Load curve ID for loading
        int lcidu = 0;          // Load curve ID for unloading
        double k = 0.0;         // Elastic stiffness
        double beta = 0.0;      // Decay constant
    };

    MatSpringGeneralNonlinear() = default;

    std::string getKeywordName() const override { return "*MAT_SPRING_GENERAL_NONLINEAR"; }
    MaterialType getMaterialType() const override { return MaterialType::Unknown; }

    MaterialId getMaterialId() const override { return data_.mid; }
    void setMaterialId(MaterialId id) override { data_.mid = id; }

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
 * @brief *MAT_SPRING_MAXWELL (*MAT_S06)
 *
 * Maxwell viscoelastic spring model.
 */
class KOO_API MatSpringMaxwell : public CloneableKeyword<MatSpringMaxwell, MaterialBase> {
public:
    struct Data {
        MaterialId mid = 0;
        double k = 0.0;         // Spring stiffness
        double c = 0.0;         // Damper coefficient
    };

    MatSpringMaxwell() = default;

    std::string getKeywordName() const override { return "*MAT_SPRING_MAXWELL"; }
    MaterialType getMaterialType() const override { return MaterialType::Unknown; }

    MaterialId getMaterialId() const override { return data_.mid; }
    void setMaterialId(MaterialId id) override { data_.mid = id; }

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
 * @brief *MAT_CABLE_DISCRETE_BEAM (*MAT_071)
 *
 * Cable material for discrete beam elements.
 */
class KOO_API MatCableDiscreteBeam : public CloneableKeyword<MatCableDiscreteBeam, MaterialBase> {
public:
    struct Data {
        MaterialId mid = 0;
        double ro = 0.0;        // Mass per unit length
        double e = 0.0;         // Young's modulus
        double area = 0.0;      // Cross-sectional area
        double fo = 0.0;        // Initial tension (preload)
        double lmin = 0.0;      // Minimum length (slack control)
    };

    MatCableDiscreteBeam() = default;

    std::string getKeywordName() const override { return "*MAT_CABLE_DISCRETE_BEAM"; }
    MaterialType getMaterialType() const override { return MaterialType::Unknown; }

    MaterialId getMaterialId() const override { return data_.mid; }
    void setMaterialId(MaterialId id) override { data_.mid = id; }

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
 * @brief *MAT_ELASTIC_VISCOPLASTIC_THERMAL (*MAT_106)
 *
 * Elastic viscoplastic material with thermal effects.
 */
class KOO_API MatElasticViscoplasticThermal : public CloneableKeyword<MatElasticViscoplasticThermal, MaterialBase> {
public:
    struct Data {
        MaterialId mid = 0;
        double ro = 0.0;        // Density
        double e = 0.0;         // Young's modulus
        double pr = 0.0;        // Poisson's ratio
        double sigy = 0.0;      // Yield stress
        double alpha = 0.0;     // Thermal expansion coefficient
        double lcss = 0.0;      // Load curve for stress-strain
        double qt1 = 0.0;       // Thermal softening parameter
        double qt2 = 0.0;       // Reference temperature
    };

    MatElasticViscoplasticThermal() = default;

    std::string getKeywordName() const override { return "*MAT_ELASTIC_VISCOPLASTIC_THERMAL"; }
    MaterialType getMaterialType() const override { return MaterialType::Unknown; }

    MaterialId getMaterialId() const override { return data_.mid; }
    void setMaterialId(MaterialId id) override { data_.mid = id; }

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
 * @brief *MAT_USER_DEFINED_MATERIAL_MODELS (*MAT_041-050)
 *
 * User-defined material model interface.
 */
class KOO_API MatUserDefined : public CloneableKeyword<MatUserDefined, MaterialBase> {
public:
    struct Data {
        MaterialId mid = 0;
        double ro = 0.0;        // Density
        int mt = 0;             // Material type (41-50)
        int lmc = 0;            // Length of material constant array
        int nhv = 0;            // Number of history variables
        int iortho = 0;         // Orthotropic flag
        int ibulk = 0;          // Bulk modulus flag
        int ig = 0;             // Shear modulus flag
        int ivect = 0;          // Vectorization flag
        std::vector<double> p;  // Material constants
    };

    MatUserDefined() = default;

    std::string getKeywordName() const override { return "*MAT_USER_DEFINED_MATERIAL_MODELS"; }
    MaterialType getMaterialType() const override { return MaterialType::Unknown; }

    MaterialId getMaterialId() const override { return data_.mid; }
    void setMaterialId(MaterialId id) override { data_.mid = id; }

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
 * @brief *MAT_FU_CHANG_FOAM (*MAT_083)
 *
 * Fu Chang foam material with rate effects.
 */
class KOO_API MatFuChangFoam : public CloneableKeyword<MatFuChangFoam, MaterialBase> {
public:
    struct Data {
        MaterialId mid = 0;
        double ro = 0.0;        // Density
        double e = 0.0;         // Young's modulus (for contact)
        double kcon = 0.0;      // Stiffness coefficient
        int lcid = 0;           // Load curve for stress vs strain
        double tc = 0.0;        // Tension cutoff
        double hu = 0.0;        // Hysteretic unloading factor
        double beta = 0.0;      // Viscous damping coefficient
        double damp = 0.0;      // Rate type
        double shape = 0.0;     // Shape factor for unloading
        double fail = 0.0;      // Failure flag
        int bvflag = 0;         // Bulk viscosity flag
        double ed = 0.0;        // Energy dissipation option
        double beta1 = 0.0;     // Fit parameter
        int kflag = 0;          // Bulk modulus flag
        double ref = 0.0;       // Reference configuration
    };

    MatFuChangFoam() = default;

    std::string getKeywordName() const override { return "*MAT_FU_CHANG_FOAM"; }
    MaterialType getMaterialType() const override { return MaterialType::Unknown; }

    MaterialId getMaterialId() const override { return data_.mid; }
    void setMaterialId(MaterialId id) override { data_.mid = id; }

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
 * @brief *MAT_WINFRITH_CONCRETE (*MAT_084 / *MAT_085)
 *
 * Winfrith concrete model for reinforced concrete.
 */
class KOO_API MatWinfrithConcrete : public CloneableKeyword<MatWinfrithConcrete, MaterialBase> {
public:
    struct Data {
        MaterialId mid = 0;
        double ro = 0.0;        // Density
        double tm = 0.0;        // Maximum aggregate size
        double pr = 0.0;        // Poisson's ratio
        double ucs = 0.0;       // Unconfined compressive strength
        double uts = 0.0;       // Unconfined tensile strength
        double fe = 0.0;        // Fracture energy
        double apts = 0.0;      // Aggregate size for rate effects
        double strfl = 0.0;     // Strain at failure
        double e = 0.0;         // Young's modulus (optional)
        double ys = 0.0;        // Yield surface parameter
        double erate = 0.0;     // Rate effects flag
        int conm = 0;           // Concrete model flag
    };

    MatWinfrithConcrete() = default;

    std::string getKeywordName() const override { return "*MAT_WINFRITH_CONCRETE"; }
    MaterialType getMaterialType() const override { return MaterialType::Unknown; }

    MaterialId getMaterialId() const override { return data_.mid; }
    void setMaterialId(MaterialId id) override { data_.mid = id; }

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
 * @brief *MAT_CONCRETE_DAMAGE_REL3 (*MAT_072R3)
 *
 * Concrete damage model release 3 (Karagozian & Case).
 */
class KOO_API MatConcreteDamageRel3 : public CloneableKeyword<MatConcreteDamageRel3, MaterialBase> {
public:
    struct Data {
        MaterialId mid = 0;
        double ro = 0.0;        // Density
        double pr = 0.0;        // Poisson's ratio
        double ft = 0.0;        // Uniaxial tensile strength
        double a0 = 0.0;        // Cohesion
        double a1 = 0.0;        // Pressure hardening coefficient
        double a2 = 0.0;        // Pressure hardening coefficient
        double b1 = 0.0;        // Damage scaling
        double omega = 0.0;     // Fractional dilatancy
        double a1f = 0.0;       // Residual failure surface
        double srate = 0.0;     // Strain rate enhancement factor
        double eta = 0.0;       // Damage recovery
        double fc = 0.0;        // Unconfined compressive strength
        int loctype = 0;        // Localization width type
        double wlz = 0.0;       // Localization width
        double lcrate = 0;      // Load curve for rate effects
    };

    MatConcreteDamageRel3() = default;

    std::string getKeywordName() const override { return "*MAT_CONCRETE_DAMAGE_REL3"; }
    MaterialType getMaterialType() const override { return MaterialType::Unknown; }

    MaterialId getMaterialId() const override { return data_.mid; }
    void setMaterialId(MaterialId id) override { data_.mid = id; }

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
 * @brief *MAT_CSCM / *MAT_CSCM_CONCRETE (*MAT_159)
 *
 * Continuous surface cap model for concrete.
 */
class KOO_API MatCscmConcrete : public CloneableKeyword<MatCscmConcrete, MaterialBase> {
public:
    struct Data {
        MaterialId mid = 0;
        double ro = 0.0;        // Density
        int nplot = 1;          // Plotting option
        int incre = 0;          // Strain increment for substepping
        int irate = 0;          // Rate effects flag
        double erode = 0.0;     // Element erosion flag
        double recov = 0.0;     // Recovery factor
        int iretrc = 0;         // Cap retraction option
        double pred = 0.0;      // Pre-damage flag
        double fc = 0.0;        // Unconfined compressive strength
        double datefac = 1.0;   // Stiffness recovery factor
        int units = 0;          // Units (GPa, mm, msec, kg/mm3, kN)
    };

    MatCscmConcrete() = default;

    std::string getKeywordName() const override { return "*MAT_CSCM_CONCRETE"; }
    MaterialType getMaterialType() const override { return MaterialType::Unknown; }

    MaterialId getMaterialId() const override { return data_.mid; }
    void setMaterialId(MaterialId id) override { data_.mid = id; }

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
 * @brief *MAT_PLASTIC_GREEN_NAGHDI (*MAT_035)
 *
 * Rate-independent plasticity using Green-Naghdi stress rate.
 */
class KOO_API MatPlasticGreenNaghdi : public CloneableKeyword<MatPlasticGreenNaghdi, MaterialBase> {
public:
    struct Data {
        MaterialId mid = 0;
        double ro = 0.0;        // Density
        double e = 0.0;         // Young's modulus
        double pr = 0.0;        // Poisson's ratio
        double sigy = 0.0;      // Yield stress
        double etan = 0.0;      // Tangent modulus
        double bulk = 0.0;      // Bulk modulus (optional)
        int lcss = 0;           // Load curve ID for stress-strain
        double q1 = 0.0;        // Isotropic hardening parameter
        double c1 = 0.0;        // Kinematic hardening parameter
        double q2 = 0.0;        // Isotropic hardening parameter
        double c2 = 0.0;        // Kinematic hardening parameter
    };

    MatPlasticGreenNaghdi() = default;

    std::string getKeywordName() const override { return "*MAT_PLASTIC_GREEN_NAGHDI"; }
    MaterialType getMaterialType() const override { return MaterialType::Unknown; }

    MaterialId getMaterialId() const override { return data_.mid; }
    void setMaterialId(MaterialId id) override { data_.mid = id; }

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
 * @brief *MAT_3-PARAMETER_BARLAT (*MAT_036)
 *
 * 3-parameter Barlat anisotropic plasticity model for sheets.
 */
class KOO_API Mat3ParameterBarlat : public CloneableKeyword<Mat3ParameterBarlat, MaterialBase> {
public:
    struct Data {
        MaterialId mid = 0;
        double ro = 0.0;        // Density
        double e = 0.0;         // Young's modulus
        double pr = 0.0;        // Poisson's ratio
        double k = 0.0;         // Strength coefficient
        double m = 0.0;         // Exponent in Barlat
        double r00 = 0.0;       // Lankford parameter at 0 degrees
        double r45 = 0.0;       // Lankford parameter at 45 degrees
        double r90 = 0.0;       // Lankford parameter at 90 degrees
        int lcss = 0;           // Load curve ID for stress-strain
        double aopt = 0.0;      // Material axes option
    };

    Mat3ParameterBarlat() = default;

    std::string getKeywordName() const override { return "*MAT_3-PARAMETER_BARLAT"; }
    MaterialType getMaterialType() const override { return MaterialType::Unknown; }

    MaterialId getMaterialId() const override { return data_.mid; }
    void setMaterialId(MaterialId id) override { data_.mid = id; }

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
 * @brief *MAT_TRANSVERSELY_ANISOTROPIC_ELASTIC_PLASTIC (*MAT_037)
 *
 * Transversely anisotropic elastic-plastic model.
 */
class KOO_API MatTransverselyAnisotropicElasticPlastic : public CloneableKeyword<MatTransverselyAnisotropicElasticPlastic, MaterialBase> {
public:
    struct Data {
        MaterialId mid = 0;
        double ro = 0.0;        // Density
        double e = 0.0;         // Young's modulus in plane
        double pr = 0.0;        // Poisson's ratio
        double sigy = 0.0;      // Yield stress
        double etan = 0.0;      // Tangent modulus
        double r = 0.0;         // Anisotropic hardening parameter
        int hlcid = 0;          // Load curve ID for hardening
        double aopt = 0.0;      // Material axes option
    };

    MatTransverselyAnisotropicElasticPlastic() = default;

    std::string getKeywordName() const override { return "*MAT_TRANSVERSELY_ANISOTROPIC_ELASTIC_PLASTIC"; }
    MaterialType getMaterialType() const override { return MaterialType::Unknown; }

    MaterialId getMaterialId() const override { return data_.mid; }
    void setMaterialId(MaterialId id) override { data_.mid = id; }

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
 * @brief *MAT_BLATZ-KO_FOAM (*MAT_038)
 *
 * Blatz-Ko compressible foam rubber model.
 */
class KOO_API MatBlatzKoFoam : public CloneableKeyword<MatBlatzKoFoam, MaterialBase> {
public:
    struct Data {
        MaterialId mid = 0;
        double ro = 0.0;        // Density
        double g = 0.0;         // Shear modulus
        double ref = 0.0;       // Reference configuration
        double dtefac = 0.0;    // Time step scale factor
    };

    MatBlatzKoFoam() = default;

    std::string getKeywordName() const override { return "*MAT_BLATZ-KO_FOAM"; }
    MaterialType getMaterialType() const override { return MaterialType::Unknown; }

    MaterialId getMaterialId() const override { return data_.mid; }
    void setMaterialId(MaterialId id) override { data_.mid = id; }

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
 * @brief *MAT_FLD_TRANSVERSELY_ANISOTROPIC (*MAT_039)
 *
 * Forming limit diagram model with transverse anisotropy.
 */
class KOO_API MatFldTransverselyAnisotropic : public CloneableKeyword<MatFldTransverselyAnisotropic, MaterialBase> {
public:
    struct Data {
        MaterialId mid = 0;
        double ro = 0.0;        // Density
        double e = 0.0;         // Young's modulus
        double pr = 0.0;        // Poisson's ratio
        double sigy = 0.0;      // Initial yield stress
        double etan = 0.0;      // Hardening modulus
        double r = 0.0;         // Anisotropic hardening parameter
        int hlcid = 0;          // Hardening load curve ID
        int fld = 0;            // Forming limit diagram curve ID
    };

    MatFldTransverselyAnisotropic() = default;

    std::string getKeywordName() const override { return "*MAT_FLD_TRANSVERSELY_ANISOTROPIC"; }
    MaterialType getMaterialType() const override { return MaterialType::Unknown; }

    MaterialId getMaterialId() const override { return data_.mid; }
    void setMaterialId(MaterialId id) override { data_.mid = id; }

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
 * @brief *MAT_NONLINEAR_ORTHOTROPIC (*MAT_040)
 *
 * Nonlinear orthotropic material model.
 */
class KOO_API MatNonlinearOrthotropic : public CloneableKeyword<MatNonlinearOrthotropic, MaterialBase> {
public:
    struct Data {
        MaterialId mid = 0;
        double ro = 0.0;        // Density
        double ea = 0.0;        // Young's modulus in a-direction
        double eb = 0.0;        // Young's modulus in b-direction
        double ec = 0.0;        // Young's modulus in c-direction
        double prba = 0.0;      // Poisson's ratio ba
        double prca = 0.0;      // Poisson's ratio ca
        double prcb = 0.0;      // Poisson's ratio cb
        double gab = 0.0;       // Shear modulus ab
        double gbc = 0.0;       // Shear modulus bc
        double gca = 0.0;       // Shear modulus ca
        double aopt = 0.0;      // Material axes option
    };

    MatNonlinearOrthotropic() = default;

    std::string getKeywordName() const override { return "*MAT_NONLINEAR_ORTHOTROPIC"; }
    MaterialType getMaterialType() const override { return MaterialType::Unknown; }

    MaterialId getMaterialId() const override { return data_.mid; }
    void setMaterialId(MaterialId id) override { data_.mid = id; }

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
 * @brief *MAT_BAMMAN (*MAT_051)
 *
 * Sandia's Bamman temperature-dependent plasticity model.
 */
class KOO_API MatBamman : public CloneableKeyword<MatBamman, MaterialBase> {
public:
    struct Data {
        MaterialId mid = 0;
        double ro = 0.0;        // Density
        double e = 0.0;         // Young's modulus
        double pr = 0.0;        // Poisson's ratio
        double t = 0.0;         // Temperature
        double hc = 0.0;        // Heat capacity
        double c1 = 0.0;        // Material constant
        double c2 = 0.0;        // Material constant
        double c3 = 0.0;        // Material constant
        double c4 = 0.0;        // Material constant
        double c5 = 0.0;        // Material constant
        double c6 = 0.0;        // Material constant
    };

    MatBamman() = default;

    std::string getKeywordName() const override { return "*MAT_BAMMAN"; }
    MaterialType getMaterialType() const override { return MaterialType::Unknown; }

    MaterialId getMaterialId() const override { return data_.mid; }
    void setMaterialId(MaterialId id) override { data_.mid = id; }

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
 * @brief *MAT_BAMMAN_DAMAGE (*MAT_052)
 *
 * Bamman plasticity with damage evolution.
 */
class KOO_API MatBammanDamage : public CloneableKeyword<MatBammanDamage, MaterialBase> {
public:
    struct Data {
        MaterialId mid = 0;
        double ro = 0.0;        // Density
        double e = 0.0;         // Young's modulus
        double pr = 0.0;        // Poisson's ratio
        double t = 0.0;         // Temperature
        double hc = 0.0;        // Heat capacity
        double d1 = 0.0;        // Damage parameter
        double d2 = 0.0;        // Damage parameter
        double d3 = 0.0;        // Damage parameter
        double phic = 0.0;      // Critical damage
        double spall = 0.0;     // Spall model
        double fail = 0.0;      // Failure strain
    };

    MatBammanDamage() = default;

    std::string getKeywordName() const override { return "*MAT_BAMMAN_DAMAGE"; }
    MaterialType getMaterialType() const override { return MaterialType::Unknown; }

    MaterialId getMaterialId() const override { return data_.mid; }
    void setMaterialId(MaterialId id) override { data_.mid = id; }

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
 * @brief *MAT_CLOSED_CELL_FOAM (*MAT_053)
 *
 * Closed-cell polyurethane foam model.
 */
class KOO_API MatClosedCellFoam : public CloneableKeyword<MatClosedCellFoam, MaterialBase> {
public:
    struct Data {
        MaterialId mid = 0;
        double ro = 0.0;        // Density
        double e = 0.0;         // Young's modulus
        double lcid = 0;        // Load curve ID for stress-strain
        double tc = 0.0;        // Tension cutoff
        double hu = 0.0;        // Hysteretic unloading
        double beta = 0.0;      // Decay constant
        double damp = 0.0;      // Viscous damping coefficient
        double shape = 0.0;     // Shape factor for unloading
        double fail = 0.0;      // Failure option
        double bvflag = 0;      // Bulk viscosity flag
    };

    MatClosedCellFoam() = default;

    std::string getKeywordName() const override { return "*MAT_CLOSED_CELL_FOAM"; }
    MaterialType getMaterialType() const override { return MaterialType::Unknown; }

    MaterialId getMaterialId() const override { return data_.mid; }
    void setMaterialId(MaterialId id) override { data_.mid = id; }

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
 * @brief *MAT_ELASTIC_WITH_VISCOSITY (*MAT_060)
 *
 * Elastic material with viscosity for explicit analysis.
 */
class KOO_API MatElasticWithViscosity : public CloneableKeyword<MatElasticWithViscosity, MaterialBase> {
public:
    struct Data {
        MaterialId mid = 0;
        double ro = 0.0;        // Density
        double k = 0.0;         // Bulk modulus
        double g0 = 0.0;        // Short-time shear modulus
        double gi = 0.0;        // Long-time shear modulus
        double beta = 0.0;      // Decay constant
        double ref = 0.0;       // Reference configuration
    };

    MatElasticWithViscosity() = default;

    std::string getKeywordName() const override { return "*MAT_ELASTIC_WITH_VISCOSITY"; }
    MaterialType getMaterialType() const override { return MaterialType::Viscoelastic; }

    MaterialId getMaterialId() const override { return data_.mid; }
    void setMaterialId(MaterialId id) override { data_.mid = id; }

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
 * @brief *MAT_KELVIN_MAXWELL_VISCOELASTIC (*MAT_061)
 *
 * Kelvin-Maxwell viscoelastic model.
 */
class KOO_API MatKelvinMaxwellViscoelastic : public CloneableKeyword<MatKelvinMaxwellViscoelastic, MaterialBase> {
public:
    struct Data {
        MaterialId mid = 0;
        double ro = 0.0;        // Density
        double bulk = 0.0;      // Bulk modulus
        double g0 = 0.0;        // Short-time shear modulus
        double gi = 0.0;        // Long-time shear modulus
        double beta = 0.0;      // Decay constant
        double dc = 0.0;        // Damping coefficient
    };

    MatKelvinMaxwellViscoelastic() = default;

    std::string getKeywordName() const override { return "*MAT_KELVIN-MAXWELL_VISCOELASTIC"; }
    MaterialType getMaterialType() const override { return MaterialType::Viscoelastic; }

    MaterialId getMaterialId() const override { return data_.mid; }
    void setMaterialId(MaterialId id) override { data_.mid = id; }

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
 * @brief *MAT_VISCOUS_FOAM (*MAT_062)
 *
 * Viscous foam model.
 */
class KOO_API MatViscousFoam : public CloneableKeyword<MatViscousFoam, MaterialBase> {
public:
    struct Data {
        MaterialId mid = 0;
        double ro = 0.0;        // Density
        double e = 0.0;         // Young's modulus
        int lcid = 0;           // Load curve ID
        double tc = 0.0;        // Tension cutoff
        double hu = 0.0;        // Hysteretic unloading
        double beta = 0.0;      // Decay constant
        double damp = 0.0;      // Damping coefficient
    };

    MatViscousFoam() = default;

    std::string getKeywordName() const override { return "*MAT_VISCOUS_FOAM"; }
    MaterialType getMaterialType() const override { return MaterialType::Unknown; }

    MaterialId getMaterialId() const override { return data_.mid; }
    void setMaterialId(MaterialId id) override { data_.mid = id; }

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
 * @brief *MAT_RATE_SENSITIVE_COMPOSITE_FABRIC (*MAT_058)
 *
 * Rate-sensitive composite fabric model.
 */
class KOO_API MatRateSensitiveCompositeFabric : public CloneableKeyword<MatRateSensitiveCompositeFabric, MaterialBase> {
public:
    struct Data {
        MaterialId mid = 0;
        double ro = 0.0;        // Density
        double ea = 0.0;        // Young's modulus in a-direction
        double eb = 0.0;        // Young's modulus in b-direction
        double prba = 0.0;      // Poisson's ratio ba
        double gab = 0.0;       // Shear modulus ab
        double gbc = 0.0;       // Shear modulus bc
        double gca = 0.0;       // Shear modulus ca
        double aopt = 0.0;      // Material axes option
    };

    MatRateSensitiveCompositeFabric() = default;

    std::string getKeywordName() const override { return "*MAT_RATE_SENSITIVE_COMPOSITE_FABRIC"; }
    MaterialType getMaterialType() const override { return MaterialType::Fabric; }

    MaterialId getMaterialId() const override { return data_.mid; }
    void setMaterialId(MaterialId id) override { data_.mid = id; }

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
 * @brief *MAT_COMPOSITE_FAILURE_SOLID_MODEL (*MAT_059)
 *
 * Composite failure model for solid elements.
 */
class KOO_API MatCompositeFailureSolidModel : public CloneableKeyword<MatCompositeFailureSolidModel, MaterialBase> {
public:
    struct Data {
        MaterialId mid = 0;
        double ro = 0.0;        // Density
        double ea = 0.0;        // Young's modulus in a-direction
        double eb = 0.0;        // Young's modulus in b-direction
        double ec = 0.0;        // Young's modulus in c-direction
        double prba = 0.0;      // Poisson's ratio ba
        double prca = 0.0;      // Poisson's ratio ca
        double prcb = 0.0;      // Poisson's ratio cb
        double gab = 0.0;       // Shear modulus ab
        double gbc = 0.0;       // Shear modulus bc
        double gca = 0.0;       // Shear modulus ca
        double aopt = 0.0;      // Material axes option
    };

    MatCompositeFailureSolidModel() = default;

    std::string getKeywordName() const override { return "*MAT_COMPOSITE_FAILURE_SOLID_MODEL"; }
    MaterialType getMaterialType() const override { return MaterialType::Unknown; }

    MaterialId getMaterialId() const override { return data_.mid; }
    void setMaterialId(MaterialId id) override { data_.mid = id; }

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
 * @brief *MAT_VISCOELASTIC_THERMAL (*MAT_064)
 *
 * Temperature-dependent viscoelastic model.
 */
class KOO_API MatViscoelasticThermal : public CloneableKeyword<MatViscoelasticThermal, MaterialBase> {
public:
    struct Data {
        MaterialId mid = 0;
        double ro = 0.0;        // Density
        double bulk = 0.0;      // Bulk modulus
        double g0 = 0.0;        // Short-time shear modulus
        double gi = 0.0;        // Long-time shear modulus
        double beta = 0.0;      // Decay constant
        int lcte = 0;           // Load curve ID for thermal expansion
        double tref = 0.0;      // Reference temperature
    };

    MatViscoelasticThermal() = default;

    std::string getKeywordName() const override { return "*MAT_VISCOELASTIC_THERMAL"; }
    MaterialType getMaterialType() const override { return MaterialType::Viscoelastic; }

    MaterialId getMaterialId() const override { return data_.mid; }
    void setMaterialId(MaterialId id) override { data_.mid = id; }

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
 * @brief *MAT_BILKHU_DUBOIS_FOAM (*MAT_075)
 *
 * Bilkhu/Dubois foam model.
 */
class KOO_API MatBilkhuDuboisFoam : public CloneableKeyword<MatBilkhuDuboisFoam, MaterialBase> {
public:
    struct Data {
        MaterialId mid = 0;
        double ro = 0.0;        // Density
        double e = 0.0;         // Young's modulus
        double pr = 0.0;        // Poisson's ratio
        int lcid = 0;           // Load curve ID
        double tc = 0.0;        // Tension cutoff
        double damp = 0.0;      // Damping coefficient
        double lcidr = 0;       // Load curve ID for rate effects
    };

    MatBilkhuDuboisFoam() = default;

    std::string getKeywordName() const override { return "*MAT_BILKHU_DUBOIS_FOAM"; }
    MaterialType getMaterialType() const override { return MaterialType::Unknown; }

    MaterialId getMaterialId() const override { return data_.mid; }
    void setMaterialId(MaterialId id) override { data_.mid = id; }

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
 * @brief *MAT_GENERAL_VISCOELASTIC (*MAT_076)
 *
 * General viscoelastic model with Prony series.
 */
class KOO_API MatGeneralViscoelastic : public CloneableKeyword<MatGeneralViscoelastic, MaterialBase> {
public:
    struct Data {
        MaterialId mid = 0;
        double ro = 0.0;        // Density
        double bulk = 0.0;      // Bulk modulus
        int lcg = 0;            // Load curve ID for shear relaxation
        int lck = 0;            // Load curve ID for bulk relaxation
        double nt = 0;          // Number of terms
    };

    MatGeneralViscoelastic() = default;

    std::string getKeywordName() const override { return "*MAT_GENERAL_VISCOELASTIC"; }
    MaterialType getMaterialType() const override { return MaterialType::Viscoelastic; }

    MaterialId getMaterialId() const override { return data_.mid; }
    void setMaterialId(MaterialId id) override { data_.mid = id; }

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
 * @brief *MAT_PLASTICITY_WITH_DAMAGE_ORTHO (*MAT_082)
 *
 * Orthotropic plasticity model with damage.
 */
class KOO_API MatPlasticityWithDamageOrtho : public CloneableKeyword<MatPlasticityWithDamageOrtho, MaterialBase> {
public:
    struct Data {
        MaterialId mid = 0;
        double ro = 0.0;        // Density
        double ea = 0.0;        // Young's modulus a
        double eb = 0.0;        // Young's modulus b
        double ec = 0.0;        // Young's modulus c
        double prba = 0.0;      // Poisson's ratio ba
        double prca = 0.0;      // Poisson's ratio ca
        double prcb = 0.0;      // Poisson's ratio cb
        double gab = 0.0;       // Shear modulus ab
    };

    MatPlasticityWithDamageOrtho() = default;

    std::string getKeywordName() const override { return "*MAT_PLASTICITY_WITH_DAMAGE_ORTHO"; }
    MaterialType getMaterialType() const override { return MaterialType::Unknown; }

    MaterialId getMaterialId() const override { return data_.mid; }
    void setMaterialId(MaterialId id) override { data_.mid = id; }

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
 * @brief *MAT_PIECEWISE_LINEAR_PLASTICITY_STOCHASTIC (*MAT_089)
 *
 * Piecewise linear plasticity with stochastic variation.
 */
class KOO_API MatPiecewiseLinearPlasticityStochastic : public CloneableKeyword<MatPiecewiseLinearPlasticityStochastic, MaterialBase> {
public:
    struct Data {
        MaterialId mid = 0;
        double ro = 0.0;        // Density
        double e = 0.0;         // Young's modulus
        double pr = 0.0;        // Poisson's ratio
        double sigy = 0.0;      // Yield stress
        double etan = 0.0;      // Tangent modulus
        double fail = 0.0;      // Failure strain
        double tdel = 0.0;      // Minimum time step
        double c = 0.0;         // Strain rate parameter
        double p = 0.0;         // Strain rate exponent
        int lcss = 0;           // Load curve ID
    };

    MatPiecewiseLinearPlasticityStochastic() = default;

    std::string getKeywordName() const override { return "*MAT_PIECEWISE_LINEAR_PLASTICITY_STOCHASTIC"; }
    MaterialType getMaterialType() const override { return MaterialType::Piecewise; }

    MaterialId getMaterialId() const override { return data_.mid; }
    void setMaterialId(MaterialId id) override { data_.mid = id; }

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
 * @brief *MAT_ACOUSTIC (*MAT_090)
 *
 * Acoustic material for fluid elements.
 */
class KOO_API MatAcoustic : public CloneableKeyword<MatAcoustic, MaterialBase> {
public:
    struct Data {
        MaterialId mid = 0;
        double ro = 0.0;        // Density
        double c = 0.0;         // Sound speed
        double beta = 0.0;      // Damping coefficient
    };

    MatAcoustic() = default;

    std::string getKeywordName() const override { return "*MAT_ACOUSTIC"; }
    MaterialType getMaterialType() const override { return MaterialType::Unknown; }

    MaterialId getMaterialId() const override { return data_.mid; }
    void setMaterialId(MaterialId id) override { data_.mid = id; }

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
 * @brief *MAT_SOFT_TISSUE (*MAT_091)
 *
 * Soft tissue material model for biomedical applications.
 */
class KOO_API MatSoftTissue : public CloneableKeyword<MatSoftTissue, MaterialBase> {
public:
    struct Data {
        MaterialId mid = 0;
        double ro = 0.0;        // Density
        double c1 = 0.0;        // Material constant
        double c2 = 0.0;        // Material constant
        double c3 = 0.0;        // Material constant
        double c4 = 0.0;        // Material constant
        double c5 = 0.0;        // Material constant
        double xk = 0.0;        // Bulk modulus
        double xlam = 0.0;      // Stretch limit
    };

    MatSoftTissue() = default;

    std::string getKeywordName() const override { return "*MAT_SOFT_TISSUE"; }
    MaterialType getMaterialType() const override { return MaterialType::Unknown; }

    MaterialId getMaterialId() const override { return data_.mid; }
    void setMaterialId(MaterialId id) override { data_.mid = id; }

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
 * @brief *MAT_ARRUDA_BOYCE (*MAT_127)
 *
 * Arruda-Boyce hyperelastic rubber model.
 */
class KOO_API MatArrudaBoyce : public CloneableKeyword<MatArrudaBoyce, MaterialBase> {
public:
    struct Data {
        MaterialId mid = 0;
        double ro = 0.0;        // Density
        double k = 0.0;         // Bulk modulus
        double mu = 0.0;        // Shear modulus
        double n = 0.0;         // Chain parameter
        double ref = 0.0;       // Reference configuration
    };

    MatArrudaBoyce() = default;

    std::string getKeywordName() const override { return "*MAT_ARRUDA_BOYCE_RUBBER"; }
    MaterialType getMaterialType() const override { return MaterialType::Unknown; }

    MaterialId getMaterialId() const override { return data_.mid; }
    void setMaterialId(MaterialId id) override { data_.mid = id; }

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
 * @brief *MAT_SIMPLIFIED_RUBBER (*MAT_181)
 *
 * Simplified rubber/foam model.
 */
class KOO_API MatSimplifiedRubber : public CloneableKeyword<MatSimplifiedRubber, MaterialBase> {
public:
    struct Data {
        MaterialId mid = 0;
        double ro = 0.0;        // Density
        double km = 0.0;        // Bulk modulus multiplier
        double mu = 0.0;        // Damping coefficient
        double g = 0.0;         // Shear modulus
        int sigf = 0;           // Load curve for failure stress
        double ref = 0.0;       // Reference configuration
        double prten = 0.0;     // Optional tension curve
    };

    MatSimplifiedRubber() = default;

    std::string getKeywordName() const override { return "*MAT_SIMPLIFIED_RUBBER"; }
    MaterialType getMaterialType() const override { return MaterialType::Unknown; }

    MaterialId getMaterialId() const override { return data_.mid; }
    void setMaterialId(MaterialId id) override { data_.mid = id; }

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
 * @brief *MAT_ARUP_ADHESIVE (*MAT_169)
 *
 * Adhesive material model for bonded structures.
 */
class KOO_API MatArupAdhesive : public CloneableKeyword<MatArupAdhesive, MaterialBase> {
public:
    struct Data {
        MaterialId mid = 0;
        double ro = 0.0;        // Density
        double e = 0.0;         // Young's modulus
        double pr = 0.0;        // Poisson's ratio
        double etan = 0.0;      // Tangent modulus
        double sigy = 0.0;      // Yield stress
        double fail = 0.0;      // Failure strain
        double gic = 0.0;       // Mode I fracture energy
        double giic = 0.0;      // Mode II fracture energy
    };

    MatArupAdhesive() = default;

    std::string getKeywordName() const override { return "*MAT_ARUP_ADHESIVE"; }
    MaterialType getMaterialType() const override { return MaterialType::Unknown; }

    MaterialId getMaterialId() const override { return data_.mid; }
    void setMaterialId(MaterialId id) override { data_.mid = id; }

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
 * @brief *MAT_COHESIVE_GENERAL (*MAT_184)
 *
 * General cohesive material for interface elements.
 */
class KOO_API MatCohesiveGeneral : public CloneableKeyword<MatCohesiveGeneral, MaterialBase> {
public:
    struct Data {
        MaterialId mid = 0;
        double ro = 0.0;        // Density
        int roflg = 0;          // Density flag
        int intfail = 0;        // Integration point failure
        double ten = 0.0;       // Peak tensile traction
        double she = 0.0;       // Peak shear traction
        double psr = 0.0;       // Power for rate effect
        double gic = 0.0;       // Mode I energy release rate
        double giic = 0.0;      // Mode II energy release rate
    };

    MatCohesiveGeneral() = default;

    std::string getKeywordName() const override { return "*MAT_COHESIVE_GENERAL"; }
    MaterialType getMaterialType() const override { return MaterialType::Unknown; }

    MaterialId getMaterialId() const override { return data_.mid; }
    void setMaterialId(MaterialId id) override { data_.mid = id; }

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
 * @brief *MAT_COHESIVE_ELASTIC (*MAT_185)
 *
 * Elastic cohesive material for interface elements.
 */
class KOO_API MatCohesiveElastic : public CloneableKeyword<MatCohesiveElastic, MaterialBase> {
public:
    struct Data {
        MaterialId mid = 0;
        double ro = 0.0;        // Density
        double en = 0.0;        // Normal stiffness
        double et = 0.0;        // Tangential stiffness
        double gic = 0.0;       // Mode I fracture energy
        double giic = 0.0;      // Mode II fracture energy
    };

    MatCohesiveElastic() = default;

    std::string getKeywordName() const override { return "*MAT_COHESIVE_ELASTIC"; }
    MaterialType getMaterialType() const override { return MaterialType::Unknown; }

    MaterialId getMaterialId() const override { return data_.mid; }
    void setMaterialId(MaterialId id) override { data_.mid = id; }

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
 * @brief *MAT_TABULATED_JOHNSON_COOK (*MAT_224)
 *
 * Tabulated Johnson-Cook material model.
 */
class KOO_API MatTabulatedJohnsonCook : public CloneableKeyword<MatTabulatedJohnsonCook, MaterialBase> {
public:
    struct Data {
        MaterialId mid = 0;
        double ro = 0.0;        // Density
        double e = 0.0;         // Young's modulus
        double pr = 0.0;        // Poisson's ratio
        double cp = 0.0;        // Specific heat
        double tr = 0.0;        // Room temperature
        double beta = 0.0;      // Taylor-Quinney coefficient
        int numint = 0;         // Number of integration points
        int lcss = 0;           // Load curve ID for stress-strain
        int lcts = 0;           // Load curve ID for temperature softening
    };

    MatTabulatedJohnsonCook() = default;

    std::string getKeywordName() const override { return "*MAT_TABULATED_JOHNSON_COOK"; }
    MaterialType getMaterialType() const override { return MaterialType::JohnsonCook; }

    MaterialId getMaterialId() const override { return data_.mid; }
    void setMaterialId(MaterialId id) override { data_.mid = id; }

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
 * @brief *MAT_ANISOTROPIC_VISCOPLASTIC (*MAT_103)
 *
 * Anisotropic viscoplastic model for metal forming.
 */
class KOO_API MatAnisotropicViscoplastic : public CloneableKeyword<MatAnisotropicViscoplastic, MaterialBase> {
public:
    struct Data {
        MaterialId mid = 0;
        double ro = 0.0;        // Density
        double e = 0.0;         // Young's modulus
        double pr = 0.0;        // Poisson's ratio
        double sigy = 0.0;      // Yield stress
        double r00 = 0.0;       // Lankford parameter 0 deg
        double r45 = 0.0;       // Lankford parameter 45 deg
        double r90 = 0.0;       // Lankford parameter 90 deg
        int lcss = 0;           // Load curve ID
        double c = 0.0;         // Strain rate parameter
        double p = 0.0;         // Strain rate exponent
    };

    MatAnisotropicViscoplastic() = default;

    std::string getKeywordName() const override { return "*MAT_ANISOTROPIC_VISCOPLASTIC"; }
    MaterialType getMaterialType() const override { return MaterialType::Unknown; }

    MaterialId getMaterialId() const override { return data_.mid; }
    void setMaterialId(MaterialId id) override { data_.mid = id; }

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
 * @brief *MAT_DAMAGE_3 (*MAT_153)
 *
 * Isotropic damage model with three parameters.
 */
class KOO_API MatDamage3 : public CloneableKeyword<MatDamage3, MaterialBase> {
public:
    struct Data {
        MaterialId mid = 0;
        double ro = 0.0;        // Density
        double e = 0.0;         // Young's modulus
        double pr = 0.0;        // Poisson's ratio
        int lcss = 0;           // Load curve ID
        double d1 = 0.0;        // Damage parameter 1
        double d2 = 0.0;        // Damage parameter 2
        double d3 = 0.0;        // Damage parameter 3
        double d4 = 0.0;        // Damage parameter 4
    };

    MatDamage3() = default;

    std::string getKeywordName() const override { return "*MAT_DAMAGE_3"; }
    MaterialType getMaterialType() const override { return MaterialType::Unknown; }

    MaterialId getMaterialId() const override { return data_.mid; }
    void setMaterialId(MaterialId id) override { data_.mid = id; }

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
 * @brief *MAT_SEISMIC_ISOLATOR (*MAT_197)
 *
 * Material model for seismic base isolators.
 */
class KOO_API MatSeismicIsolator : public CloneableKeyword<MatSeismicIsolator, MaterialBase> {
public:
    struct Data {
        MaterialId mid = 0;
        double ro = 0.0;        // Density
        double ky = 0.0;        // Elastic stiffness in yield
        double kd = 0.0;        // Post-yield stiffness
        double fy = 0.0;        // Yield force
        double alpha = 0.0;     // Post-yield stiffness ratio
        double mu = 0.0;        // Friction coefficient
        double td = 0.0;        // Vertical stiffness
    };

    MatSeismicIsolator() = default;

    std::string getKeywordName() const override { return "*MAT_SEISMIC_ISOLATOR"; }
    MaterialType getMaterialType() const override { return MaterialType::Unknown; }

    MaterialId getMaterialId() const override { return data_.mid; }
    void setMaterialId(MaterialId id) override { data_.mid = id; }

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
 * @brief *MAT_SPRING_INELASTIC (*MAT_SPRING_INELASTIC)
 *
 * Inelastic spring material model.
 */
class KOO_API MatSpringInelastic : public CloneableKeyword<MatSpringInelastic, MaterialBase> {
public:
    struct Data {
        MaterialId mid = 0;
        int lcid = 0;           // Load curve ID
        int lcu = 0;            // Unloading curve ID
        double beta = 0.0;      // Hardening parameter
        double ty = 0.0;        // Tension yield
        double cy = 0.0;        // Compression yield
    };

    MatSpringInelastic() = default;

    std::string getKeywordName() const override { return "*MAT_SPRING_INELASTIC"; }
    MaterialType getMaterialType() const override { return MaterialType::Unknown; }

    MaterialId getMaterialId() const override { return data_.mid; }
    void setMaterialId(MaterialId id) override { data_.mid = id; }

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
 * @brief *MAT_DAMPER_NONLINEAR_VISCOUS (*MAT_DAMPER_NONLINEAR_VISCOUS)
 *
 * Nonlinear viscous damper material model.
 */
class KOO_API MatDamperNonlinearViscous : public CloneableKeyword<MatDamperNonlinearViscous, MaterialBase> {
public:
    struct Data {
        MaterialId mid = 0;
        int lcdr = 0;           // Load curve for velocity-force relationship
    };

    MatDamperNonlinearViscous() = default;

    std::string getKeywordName() const override { return "*MAT_DAMPER_NONLINEAR_VISCOUS"; }
    MaterialType getMaterialType() const override { return MaterialType::Unknown; }

    MaterialId getMaterialId() const override { return data_.mid; }
    void setMaterialId(MaterialId id) override { data_.mid = id; }

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
 * @brief *MAT_HYSTERETIC_BEAM (*MAT_HYSTERETIC_BEAM)
 *
 * Hysteretic beam material model for seismic analysis.
 */
class KOO_API MatHystereticBeam : public CloneableKeyword<MatHystereticBeam, MaterialBase> {
public:
    struct Data {
        MaterialId mid = 0;
        double ro = 0.0;        // Density
        double e = 0.0;         // Young's modulus
        double pr = 0.0;        // Poisson's ratio
        int lcpms = 0;          // Load curve for positive moment
        int lcpma = 0;          // Load curve for positive axial
        int lcnms = 0;          // Load curve for negative moment
        int lcnma = 0;          // Load curve for negative axial
    };

    MatHystereticBeam() = default;

    std::string getKeywordName() const override { return "*MAT_HYSTERETIC_BEAM"; }
    MaterialType getMaterialType() const override { return MaterialType::Unknown; }

    MaterialId getMaterialId() const override { return data_.mid; }
    void setMaterialId(MaterialId id) override { data_.mid = id; }

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
