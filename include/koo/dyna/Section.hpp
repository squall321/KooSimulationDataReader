#pragma once

#include <koo/Export.hpp>
#include <koo/dyna/Keyword.hpp>
#include <koo/util/Types.hpp>
#include <vector>

namespace koo::dyna {

/**
 * @brief Section type enumeration
 */
enum class SectionType {
    Unknown,
    Shell,
    Solid,
    Beam,
    Discrete,
    Seatbelt,
    Tshell,
    Ale2d,
    Ale1d
};

/**
 * @brief Base class for section keywords
 */
class KOO_API SectionBase : public Keyword {
public:
    virtual SectionType getSectionType() const = 0;
    virtual SectionId getSectionId() const = 0;
    virtual void setSectionId(SectionId id) = 0;
};

/**
 * @brief *SECTION_SHELL keyword
 */
class KOO_API SectionShell : public CloneableKeyword<SectionShell, SectionBase> {
public:
    SectionShell() = default;

    std::string getKeywordName() const override { return "*SECTION_SHELL"; }
    SectionType getSectionType() const override { return SectionType::Shell; }

    SectionId getSectionId() const override { return secid_; }
    void setSectionId(SectionId id) override { secid_ = id; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    // Data access
    double getThickness() const { return t1_; }
    void setThickness(double t) { t1_ = t2_ = t3_ = t4_ = t; }

    int getElform() const { return elform_; }
    void setElform(int elform) { elform_ = elform; }

    int getNip() const { return nip_; }
    void setNip(int nip) { nip_ = nip; }

    const std::string& getTitle() const { return title_; }
    void setTitle(const std::string& title) { title_ = title; }

private:
    SectionId secid_ = 0;
    int elform_ = 2;      // Element formulation
    double shrf_ = 1.0;   // Shear factor
    int nip_ = 2;         // Number of integration points
    double propt_ = 0.0;  // Printout option
    int qr_irid_ = 0;     // Quadrature rule
    int icomp_ = 0;       // Flag for composite
    double setyp_ = 1.0;  // 2D plane stress/strain flag

    double t1_ = 0.0;     // Thickness at node 1
    double t2_ = 0.0;     // Thickness at node 2
    double t3_ = 0.0;     // Thickness at node 3
    double t4_ = 0.0;     // Thickness at node 4
    double nloc_ = 0.0;   // Location of reference surface

    std::string title_;
};

/**
 * @brief *SECTION_SOLID keyword
 */
class KOO_API SectionSolid : public CloneableKeyword<SectionSolid, SectionBase> {
public:
    SectionSolid() = default;

    std::string getKeywordName() const override { return "*SECTION_SOLID"; }
    SectionType getSectionType() const override { return SectionType::Solid; }

    SectionId getSectionId() const override { return secid_; }
    void setSectionId(SectionId id) override { secid_ = id; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    // Data access
    int getElform() const { return elform_; }
    void setElform(int elform) { elform_ = elform; }

    const std::string& getTitle() const { return title_; }
    void setTitle(const std::string& title) { title_ = title; }

private:
    SectionId secid_ = 0;
    int elform_ = 1;      // Element formulation
    int aet_ = 0;         // Ambient element type

    std::string title_;
};

/**
 * @brief *SECTION_BEAM keyword
 */
class KOO_API SectionBeam : public CloneableKeyword<SectionBeam, SectionBase> {
public:
    struct CrossSectionData {
        double ts1 = 0.0;    // Beam thickness (s-direction, node 1)
        double ts2 = 0.0;    // Beam thickness (s-direction, node 2)
        double tt1 = 0.0;    // Beam thickness (t-direction, node 1)
        double tt2 = 0.0;    // Beam thickness (t-direction, node 2)
        double nsloc = 0.0;  // Location of reference surface (s)
        double ntloc = 0.0;  // Location of reference surface (t)
    };

    SectionBeam() = default;

    std::string getKeywordName() const override { return "*SECTION_BEAM"; }
    SectionType getSectionType() const override { return SectionType::Beam; }

    SectionId getSectionId() const override { return secid_; }
    void setSectionId(SectionId id) override { secid_ = id; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    // Data access
    int getElform() const { return elform_; }
    void setElform(int elform) { elform_ = elform; }

    double getShrf() const { return shrf_; }
    double getArea() const { return a_; }
    void setArea(double a) { a_ = a; }

    CrossSectionData& getCrossSection() { return cs_; }
    const CrossSectionData& getCrossSection() const { return cs_; }

private:
    SectionId secid_ = 0;
    int elform_ = 1;      // Element formulation
    double shrf_ = 1.0;   // Shear factor
    int qr_irid_ = 2;     // Quadrature rule
    int cst_ = 1;         // Cross section type
    double scoor_ = 0.0;  // Stress output location
    int nsm_ = 0;         // Non-structural mass
    double a_ = 0.0;      // Cross section area
    double iss_ = 0.0;    // Area moment of inertia (s)
    double itt_ = 0.0;    // Area moment of inertia (t)
    double j_ = 0.0;      // Torsional constant
    double sa_ = 0.0;     // Shear area (s)
    double irr_ = 0.0;    // Shear area (t)
    CrossSectionData cs_;
};

/**
 * @brief *SECTION_DISCRETE keyword
 */
class KOO_API SectionDiscrete : public CloneableKeyword<SectionDiscrete, SectionBase> {
public:
    SectionDiscrete() = default;

    std::string getKeywordName() const override { return "*SECTION_DISCRETE"; }
    SectionType getSectionType() const override { return SectionType::Discrete; }

    SectionId getSectionId() const override { return secid_; }
    void setSectionId(SectionId id) override { secid_ = id; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    // Data access
    int getDro() const { return dro_; }
    void setDro(int dro) { dro_ = dro; }

    int getKd() const { return kd_; }
    void setKd(int kd) { kd_ = kd; }

private:
    SectionId secid_ = 0;
    int dro_ = 0;         // Displacement/rotation option
    int kd_ = 0;          // Spring/damper type flag
    int v0_ = 0;          // Initial velocity flag
    double cl_ = 0.0;     // Clearance
    double fd_ = 0.0;     // Failure displacement
    int cdl_ = 0;         // Curve for dynamic deflection limit
    int tdl_ = 0;         // Table for dynamic deflection limit
};

/**
 * @brief *SECTION_SEATBELT keyword
 */
class KOO_API SectionSeatbelt : public CloneableKeyword<SectionSeatbelt, SectionBase> {
public:
    SectionSeatbelt() = default;

    std::string getKeywordName() const override { return "*SECTION_SEATBELT"; }
    SectionType getSectionType() const override { return SectionType::Seatbelt; }

    SectionId getSectionId() const override { return secid_; }
    void setSectionId(SectionId id) override { secid_ = id; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    // Data access
    double getArea() const { return area_; }
    void setArea(double a) { area_ = a; }

private:
    SectionId secid_ = 0;
    double area_ = 0.0;   // Cross section area
    double thick_ = 0.0;  // Element thickness (for contact)
};

/**
 * @brief *SECTION_TSHELL keyword
 *
 * Thick shell section.
 */
class KOO_API SectionTshell : public CloneableKeyword<SectionTshell, SectionBase> {
public:
    SectionTshell() = default;

    std::string getKeywordName() const override { return "*SECTION_TSHELL"; }
    SectionType getSectionType() const override { return SectionType::Tshell; }

    SectionId getSectionId() const override { return secid_; }
    void setSectionId(SectionId id) override { secid_ = id; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    // Data access
    int getElform() const { return elform_; }
    void setElform(int elform) { elform_ = elform; }

    int getNip() const { return nip_; }
    void setNip(int nip) { nip_ = nip; }

private:
    SectionId secid_ = 0;
    int elform_ = 1;      // Element formulation
    int shrf_ = 1;        // Shear factor flag
    int nip_ = 2;         // Number of integration points
    int propt_ = 0;       // Integration point printout
    int qr_irid_ = 0;     // Quadrature rule
    int icomp_ = 0;       // Composite flag
    int tshear_ = 0;      // Transverse shear correction
};

/**
 * @brief *SECTION_ALE2D keyword
 *
 * 2D ALE (Arbitrary Lagrangian-Eulerian) section.
 */
class KOO_API SectionAle2d : public CloneableKeyword<SectionAle2d, SectionBase> {
public:
    struct Data {
        SectionId secid = 0;
        int ateflag = 0;        // ATE flag
        int elform = 0;         // Element formulation
        double thick = 0.0;     // Thickness
        int nip = 0;            // Number of integration points
        int nxdof = 0;          // Extra degrees of freedom
        int isamp = 0;          // Sampling points
    };

    SectionAle2d() = default;

    std::string getKeywordName() const override { return "*SECTION_ALE2D"; }
    SectionType getSectionType() const override { return SectionType::Ale2d; }

    SectionId getSectionId() const override { return data_.secid; }
    void setSectionId(SectionId id) override { data_.secid = id; }

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
 * @brief *SECTION_ALE1D keyword
 *
 * 1D ALE (Arbitrary Lagrangian-Eulerian) section.
 */
class KOO_API SectionAle1d : public CloneableKeyword<SectionAle1d, SectionBase> {
public:
    struct Data {
        SectionId secid = 0;
        int elform = 0;         // Element formulation
        double area = 0.0;      // Cross sectional area
    };

    SectionAle1d() = default;

    std::string getKeywordName() const override { return "*SECTION_ALE1D"; }
    SectionType getSectionType() const override { return SectionType::Ale1d; }

    SectionId getSectionId() const override { return data_.secid; }
    void setSectionId(SectionId id) override { data_.secid = id; }

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
 * @brief *SECTION_SHELL_TITLE keyword
 */
class KOO_API SectionShellTitle : public CloneableKeyword<SectionShellTitle, SectionBase> {
public:
    SectionShellTitle() = default;

    std::string getKeywordName() const override { return "*SECTION_SHELL_TITLE"; }
    SectionType getSectionType() const override { return SectionType::Shell; }

    SectionId getSectionId() const override { return secid_; }
    void setSectionId(SectionId id) override { secid_ = id; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    const std::string& getTitle() const { return title_; }
    void setTitle(const std::string& title) { title_ = title; }

private:
    SectionId secid_ = 0;
    int elform_ = 2;
    double shrf_ = 1.0;
    int nip_ = 2;
    double propt_ = 0.0;
    int qr_irid_ = 0;
    int icomp_ = 0;
    double t1_ = 0.0;
    double t2_ = 0.0;
    double t3_ = 0.0;
    double t4_ = 0.0;
    std::string title_;
};

/**
 * @brief *SECTION_SOLID_TITLE keyword
 */
class KOO_API SectionSolidTitle : public CloneableKeyword<SectionSolidTitle, SectionBase> {
public:
    SectionSolidTitle() = default;

    std::string getKeywordName() const override { return "*SECTION_SOLID_TITLE"; }
    SectionType getSectionType() const override { return SectionType::Solid; }

    SectionId getSectionId() const override { return secid_; }
    void setSectionId(SectionId id) override { secid_ = id; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    const std::string& getTitle() const { return title_; }
    void setTitle(const std::string& title) { title_ = title; }

private:
    SectionId secid_ = 0;
    int elform_ = 1;
    int aet_ = 0;
    std::string title_;
};

/**
 * @brief *SECTION_BEAM_TITLE keyword
 */
class KOO_API SectionBeamTitle : public CloneableKeyword<SectionBeamTitle, SectionBase> {
public:
    SectionBeamTitle() = default;

    std::string getKeywordName() const override { return "*SECTION_BEAM_TITLE"; }
    SectionType getSectionType() const override { return SectionType::Beam; }

    SectionId getSectionId() const override { return secid_; }
    void setSectionId(SectionId id) override { secid_ = id; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    const std::string& getTitle() const { return title_; }
    void setTitle(const std::string& title) { title_ = title; }

private:
    SectionId secid_ = 0;
    int elform_ = 1;
    double shrf_ = 1.0;
    int qr_irid_ = 2;
    int cst_ = 1;
    double scoor_ = 0.0;
    std::string title_;
};

/**
 * @brief *SECTION_POINT_SOURCE keyword
 *
 * Defines section properties for point source elements.
 */
class KOO_API SectionPointSource : public CloneableKeyword<SectionPointSource, SectionBase> {
public:
    struct Data {
        SectionId secid = 0;
        int elform = 0;           // Element formulation
        double mass = 0.0;        // Mass
        double momx = 0.0;        // Momentum in x
        double momy = 0.0;        // Momentum in y
        double momz = 0.0;        // Momentum in z
        double energy = 0.0;      // Energy
    };

    SectionPointSource() = default;

    std::string getKeywordName() const override { return "*SECTION_POINT_SOURCE"; }
    SectionType getSectionType() const override { return SectionType::Unknown; }

    SectionId getSectionId() const override { return data_.secid; }
    void setSectionId(SectionId id) override { data_.secid = id; }

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
 * @brief *SECTION_POINT_SOURCE_MIXTURE keyword
 *
 * Defines section properties for point source mixture elements.
 */
class KOO_API SectionPointSourceMixture : public CloneableKeyword<SectionPointSourceMixture, SectionBase> {
public:
    struct Data {
        SectionId secid = 0;
        int elform = 0;           // Element formulation
        int ngas = 0;             // Number of gases
        double mmass = 0.0;       // Molecular mass
        double gamma = 1.4;       // Ratio of specific heats
    };

    SectionPointSourceMixture() = default;

    std::string getKeywordName() const override { return "*SECTION_POINT_SOURCE_MIXTURE"; }
    SectionType getSectionType() const override { return SectionType::Unknown; }

    SectionId getSectionId() const override { return data_.secid; }
    void setSectionId(SectionId id) override { data_.secid = id; }

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
 * @brief *SECTION_SHELL_EFG keyword
 *
 * Defines section properties for Element-Free Galerkin (EFG) shell elements.
 */
class KOO_API SectionShellEfg : public CloneableKeyword<SectionShellEfg, SectionBase> {
public:
    struct Data {
        SectionId secid = 0;
        int elform = 41;          // Element formulation (41 = EFG shell)
        double shrf = 1.0;        // Shear factor
        int nip = 2;              // Number of through-thickness integration points
        double propt = 0.0;       // Printout option
        int qr_irid = 0;          // Quadrature rule
        int icomp = 0;            // Composite flag
        // Card 2
        double t1 = 0.0;          // Thickness at node 1
        double t2 = 0.0;          // Thickness at node 2
        double t3 = 0.0;          // Thickness at node 3
        double t4 = 0.0;          // Thickness at node 4
        double nloc = 0.0;        // Reference surface location
        // EFG-specific parameters
        double dx = 0.0;          // Normalized dilation in x
        double dy = 0.0;          // Normalized dilation in y
        int ispline = 0;          // Spline type
        int idila = 0;            // Dilation type
        int iebt = 0;             // Essential boundary treatment
        int idim = 0;             // Dimension flag
    };

    SectionShellEfg() = default;

    std::string getKeywordName() const override { return "*SECTION_SHELL_EFG"; }
    SectionType getSectionType() const override { return SectionType::Shell; }

    SectionId getSectionId() const override { return data_.secid; }
    void setSectionId(SectionId id) override { data_.secid = id; }

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
 * @brief *SECTION_SOLID_EFG keyword
 *
 * Defines section properties for Element-Free Galerkin (EFG) solid elements.
 */
class KOO_API SectionSolidEfg : public CloneableKeyword<SectionSolidEfg, SectionBase> {
public:
    struct Data {
        SectionId secid = 0;
        int elform = 41;          // Element formulation (41 = EFG solid)
        int aet = 0;              // Ambient element type
        // EFG-specific parameters
        double dx = 0.0;          // Normalized dilation in x
        double dy = 0.0;          // Normalized dilation in y
        double dz = 0.0;          // Normalized dilation in z
        int ispline = 0;          // Spline type
        int idila = 0;            // Dilation type
        int iebt = 0;             // Essential boundary treatment
        int idim = 0;             // Dimension flag
        int nip = 0;              // Number of integration points
    };

    SectionSolidEfg() = default;

    std::string getKeywordName() const override { return "*SECTION_SOLID_EFG"; }
    SectionType getSectionType() const override { return SectionType::Solid; }

    SectionId getSectionId() const override { return data_.secid; }
    void setSectionId(SectionId id) override { data_.secid = id; }

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
 * @brief *SECTION_SHELL_ALE keyword
 *
 * Defines section properties for ALE shell elements.
 */
class KOO_API SectionShellAle : public CloneableKeyword<SectionShellAle, SectionBase> {
public:
    struct Data {
        SectionId secid = 0;
        int elform = 5;           // Element formulation for ALE
        double shrf = 1.0;        // Shear factor
        int nip = 2;              // Number of integration points
        double propt = 0.0;       // Printout option
        int qr_irid = 0;          // Quadrature rule
        double t1 = 0.0;          // Shell thickness
        double t2 = 0.0;
        double t3 = 0.0;
        double t4 = 0.0;
        int afac = 0;             // ALE factor
    };

    SectionShellAle() = default;

    std::string getKeywordName() const override { return "*SECTION_SHELL_ALE"; }
    SectionType getSectionType() const override { return SectionType::Shell; }

    SectionId getSectionId() const override { return data_.secid; }
    void setSectionId(SectionId id) override { data_.secid = id; }

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
 * @brief *SECTION_SOLID_ALE keyword
 *
 * Defines section properties for ALE solid elements.
 */
class KOO_API SectionSolidAle : public CloneableKeyword<SectionSolidAle, SectionBase> {
public:
    struct Data {
        SectionId secid = 0;
        int elform = 11;          // Element formulation for ALE solid
        int aet = 0;              // Ambient element type
        int afac = 0;             // ALE smoothing factor
        int bfac = 0;             // Boundary factor
        int cfac = 0;             // Corner factor
        int dfac = 0;             // Domain factor
    };

    SectionSolidAle() = default;

    std::string getKeywordName() const override { return "*SECTION_SOLID_ALE"; }
    SectionType getSectionType() const override { return SectionType::Solid; }

    SectionId getSectionId() const override { return data_.secid; }
    void setSectionId(SectionId id) override { data_.secid = id; }

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
 * @brief *SECTION_TSHELL_TITLE keyword
 *
 * Thick shell section with title.
 */
class KOO_API SectionTshellTitle : public CloneableKeyword<SectionTshellTitle, SectionBase> {
public:
    SectionTshellTitle() = default;

    std::string getKeywordName() const override { return "*SECTION_TSHELL_TITLE"; }
    SectionType getSectionType() const override { return SectionType::Tshell; }

    SectionId getSectionId() const override { return secid_; }
    void setSectionId(SectionId id) override { secid_ = id; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    const std::string& getTitle() const { return title_; }
    void setTitle(const std::string& title) { title_ = title; }

private:
    SectionId secid_ = 0;
    int elform_ = 1;
    int shrf_ = 1;
    int nip_ = 2;
    int propt_ = 0;
    int qr_irid_ = 0;
    int icomp_ = 0;
    int tshear_ = 0;
    std::string title_;
};

/**
 * @brief *SECTION_DISCRETE_TITLE keyword
 */
class KOO_API SectionDiscreteTitle : public CloneableKeyword<SectionDiscreteTitle, SectionBase> {
public:
    SectionDiscreteTitle() = default;

    std::string getKeywordName() const override { return "*SECTION_DISCRETE_TITLE"; }
    SectionType getSectionType() const override { return SectionType::Discrete; }

    SectionId getSectionId() const override { return secid_; }
    void setSectionId(SectionId id) override { secid_ = id; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    const std::string& getTitle() const { return title_; }
    void setTitle(const std::string& title) { title_ = title; }

private:
    SectionId secid_ = 0;
    int dro_ = 0;
    int kd_ = 0;
    int v0_ = 0;
    double cl_ = 0.0;
    double fd_ = 0.0;
    std::string title_;
};

} // namespace koo::dyna
