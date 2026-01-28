#pragma once

#include <koo/Export.hpp>
#include <koo/dyna/Keyword.hpp>
#include <koo/util/Types.hpp>
#include <vector>
#include <string>

namespace koo::dyna {

/**
 * @brief Base class for *CONTACT keywords
 */
class KOO_API ContactKeyword : public Keyword {
public:
    virtual ~ContactKeyword() = default;
};

/**
 * @brief *CONTACT_AUTOMATIC_SINGLE_SURFACE
 *
 * Automatic single surface contact - most commonly used contact type.
 */
class KOO_API ContactAutomaticSingleSurface : public CloneableKeyword<ContactAutomaticSingleSurface, ContactKeyword> {
public:
    struct Card1 {
        int ssid = 0;           // Slave segment set ID
        int msid = 0;           // Master segment set ID
        int sstyp = 0;          // Slave surface type
        int mstyp = 0;          // Master surface type
        int sboxid = 0;         // Slave box ID
        int mboxid = 0;         // Master box ID
        int spr = 0;            // Slave penalty stiffness ratio
        int mpr = 0;            // Master penalty stiffness ratio
    };

    struct Card2 {
        double fs = 0.0;        // Static friction coefficient
        double fd = 0.0;        // Dynamic friction coefficient
        double dc = 0.0;        // Decay constant
        double vc = 0.0;        // Viscous damping coefficient
        double vdc = 0.0;       // Viscous damping coefficient
        int penchk = 0;         // Penetration check
        double bt = 0.0;        // Birth time
        double dt = 1e20;       // Death time
    };

    struct Card3 {
        double sfs = 1.0;       // Scale factor for slave penalty
        double sfm = 1.0;       // Scale factor for master penalty
        double sst = 0.0;       // Slave surface thickness
        double mst = 0.0;       // Master surface thickness
        double sfst = 1.0;      // Scale factor for slave thickness
        double sfmt = 1.0;      // Scale factor for master thickness
        double fsf = 1.0;       // Friction scale factor
        double vsf = 1.0;       // Viscous scale factor
    };

    ContactAutomaticSingleSurface() = default;

    std::string getKeywordName() const override { return "*CONTACT_AUTOMATIC_SINGLE_SURFACE"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    Card1& getCard1() { return card1_; }
    const Card1& getCard1() const { return card1_; }
    Card2& getCard2() { return card2_; }
    const Card2& getCard2() const { return card2_; }
    Card3& getCard3() { return card3_; }
    const Card3& getCard3() const { return card3_; }

private:
    Card1 card1_;
    Card2 card2_;
    Card3 card3_;
};

/**
 * @brief *CONTACT_AUTOMATIC_SURFACE_TO_SURFACE
 *
 * Automatic surface to surface contact.
 */
class KOO_API ContactAutomaticSurfaceToSurface : public CloneableKeyword<ContactAutomaticSurfaceToSurface, ContactKeyword> {
public:
    using Card1 = ContactAutomaticSingleSurface::Card1;
    using Card2 = ContactAutomaticSingleSurface::Card2;
    using Card3 = ContactAutomaticSingleSurface::Card3;

    ContactAutomaticSurfaceToSurface() = default;

    std::string getKeywordName() const override { return "*CONTACT_AUTOMATIC_SURFACE_TO_SURFACE"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    Card1& getCard1() { return card1_; }
    const Card1& getCard1() const { return card1_; }
    Card2& getCard2() { return card2_; }
    const Card2& getCard2() const { return card2_; }
    Card3& getCard3() { return card3_; }
    const Card3& getCard3() const { return card3_; }

private:
    Card1 card1_;
    Card2 card2_;
    Card3 card3_;
};

/**
 * @brief *CONTACT_AUTOMATIC_NODES_TO_SURFACE
 *
 * Automatic nodes to surface contact.
 */
class KOO_API ContactAutomaticNodesToSurface : public CloneableKeyword<ContactAutomaticNodesToSurface, ContactKeyword> {
public:
    using Card1 = ContactAutomaticSingleSurface::Card1;
    using Card2 = ContactAutomaticSingleSurface::Card2;
    using Card3 = ContactAutomaticSingleSurface::Card3;

    ContactAutomaticNodesToSurface() = default;

    std::string getKeywordName() const override { return "*CONTACT_AUTOMATIC_NODES_TO_SURFACE"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    Card1& getCard1() { return card1_; }
    const Card1& getCard1() const { return card1_; }
    Card2& getCard2() { return card2_; }
    const Card2& getCard2() const { return card2_; }
    Card3& getCard3() { return card3_; }
    const Card3& getCard3() const { return card3_; }

private:
    Card1 card1_;
    Card2 card2_;
    Card3 card3_;
};

/**
 * @brief *CONTACT_AUTOMATIC_GENERAL
 *
 * General automatic contact.
 */
class KOO_API ContactAutomaticGeneral : public CloneableKeyword<ContactAutomaticGeneral, ContactKeyword> {
public:
    using Card1 = ContactAutomaticSingleSurface::Card1;
    using Card2 = ContactAutomaticSingleSurface::Card2;
    using Card3 = ContactAutomaticSingleSurface::Card3;

    ContactAutomaticGeneral() = default;

    std::string getKeywordName() const override { return "*CONTACT_AUTOMATIC_GENERAL"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    Card1& getCard1() { return card1_; }
    const Card1& getCard1() const { return card1_; }
    Card2& getCard2() { return card2_; }
    const Card2& getCard2() const { return card2_; }
    Card3& getCard3() { return card3_; }
    const Card3& getCard3() const { return card3_; }

private:
    Card1 card1_;
    Card2 card2_;
    Card3 card3_;
};

/**
 * @brief *CONTACT_SURFACE_TO_SURFACE
 *
 * Standard surface to surface contact (non-automatic).
 */
class KOO_API ContactSurfaceToSurface : public CloneableKeyword<ContactSurfaceToSurface, ContactKeyword> {
public:
    using Card1 = ContactAutomaticSingleSurface::Card1;
    using Card2 = ContactAutomaticSingleSurface::Card2;
    using Card3 = ContactAutomaticSingleSurface::Card3;

    ContactSurfaceToSurface() = default;

    std::string getKeywordName() const override { return "*CONTACT_SURFACE_TO_SURFACE"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    Card1& getCard1() { return card1_; }
    const Card1& getCard1() const { return card1_; }
    Card2& getCard2() { return card2_; }
    const Card2& getCard2() const { return card2_; }
    Card3& getCard3() { return card3_; }
    const Card3& getCard3() const { return card3_; }

private:
    Card1 card1_;
    Card2 card2_;
    Card3 card3_;
};

/**
 * @brief *CONTACT_NODES_TO_SURFACE
 *
 * Standard nodes to surface contact.
 */
class KOO_API ContactNodesToSurface : public CloneableKeyword<ContactNodesToSurface, ContactKeyword> {
public:
    using Card1 = ContactAutomaticSingleSurface::Card1;
    using Card2 = ContactAutomaticSingleSurface::Card2;
    using Card3 = ContactAutomaticSingleSurface::Card3;

    ContactNodesToSurface() = default;

    std::string getKeywordName() const override { return "*CONTACT_NODES_TO_SURFACE"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    Card1& getCard1() { return card1_; }
    const Card1& getCard1() const { return card1_; }
    Card2& getCard2() { return card2_; }
    const Card2& getCard2() const { return card2_; }
    Card3& getCard3() { return card3_; }
    const Card3& getCard3() const { return card3_; }

private:
    Card1 card1_;
    Card2 card2_;
    Card3 card3_;
};

/**
 * @brief *CONTACT_TIED_NODES_TO_SURFACE
 *
 * Tied contact - nodes tied to surface.
 */
class KOO_API ContactTiedNodesToSurface : public CloneableKeyword<ContactTiedNodesToSurface, ContactKeyword> {
public:
    using Card1 = ContactAutomaticSingleSurface::Card1;
    using Card2 = ContactAutomaticSingleSurface::Card2;

    ContactTiedNodesToSurface() = default;

    std::string getKeywordName() const override { return "*CONTACT_TIED_NODES_TO_SURFACE"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    Card1& getCard1() { return card1_; }
    const Card1& getCard1() const { return card1_; }
    Card2& getCard2() { return card2_; }
    const Card2& getCard2() const { return card2_; }

private:
    Card1 card1_;
    Card2 card2_;
};

/**
 * @brief *CONTACT_TIED_SURFACE_TO_SURFACE
 *
 * Tied surface to surface contact.
 */
class KOO_API ContactTiedSurfaceToSurface : public CloneableKeyword<ContactTiedSurfaceToSurface, ContactKeyword> {
public:
    using Card1 = ContactAutomaticSingleSurface::Card1;
    using Card2 = ContactAutomaticSingleSurface::Card2;

    ContactTiedSurfaceToSurface() = default;

    std::string getKeywordName() const override { return "*CONTACT_TIED_SURFACE_TO_SURFACE"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    Card1& getCard1() { return card1_; }
    const Card1& getCard1() const { return card1_; }
    Card2& getCard2() { return card2_; }
    const Card2& getCard2() const { return card2_; }

private:
    Card1 card1_;
    Card2 card2_;
};

/**
 * @brief *CONTACT_SPOTWELD
 *
 * Spotweld contact.
 */
class KOO_API ContactSpotweld : public CloneableKeyword<ContactSpotweld, ContactKeyword> {
public:
    using Card1 = ContactAutomaticSingleSurface::Card1;
    using Card2 = ContactAutomaticSingleSurface::Card2;

    ContactSpotweld() = default;

    std::string getKeywordName() const override { return "*CONTACT_SPOTWELD"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    Card1& getCard1() { return card1_; }
    const Card1& getCard1() const { return card1_; }
    Card2& getCard2() { return card2_; }
    const Card2& getCard2() const { return card2_; }

private:
    Card1 card1_;
    Card2 card2_;
};

/**
 * @brief *CONTACT_RIGID_BODY_ONE_WAY
 *
 * One-way rigid body contact.
 */
class KOO_API ContactRigidBodyOneWay : public CloneableKeyword<ContactRigidBodyOneWay, ContactKeyword> {
public:
    using Card1 = ContactAutomaticSingleSurface::Card1;
    using Card2 = ContactAutomaticSingleSurface::Card2;

    ContactRigidBodyOneWay() = default;

    std::string getKeywordName() const override { return "*CONTACT_RIGID_BODY_ONE_WAY"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    Card1& getCard1() { return card1_; }
    const Card1& getCard1() const { return card1_; }
    Card2& getCard2() { return card2_; }
    const Card2& getCard2() const { return card2_; }

private:
    Card1 card1_;
    Card2 card2_;
};

/**
 * @brief *CONTACT_ERODING_SINGLE_SURFACE
 *
 * Eroding single surface contact for element deletion.
 */
class KOO_API ContactErodingSingleSurface : public CloneableKeyword<ContactErodingSingleSurface, ContactKeyword> {
public:
    using Card1 = ContactAutomaticSingleSurface::Card1;
    using Card2 = ContactAutomaticSingleSurface::Card2;
    using Card3 = ContactAutomaticSingleSurface::Card3;

    ContactErodingSingleSurface() = default;

    std::string getKeywordName() const override { return "*CONTACT_ERODING_SINGLE_SURFACE"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    Card1& getCard1() { return card1_; }
    const Card1& getCard1() const { return card1_; }
    Card2& getCard2() { return card2_; }
    const Card2& getCard2() const { return card2_; }
    Card3& getCard3() { return card3_; }
    const Card3& getCard3() const { return card3_; }

private:
    Card1 card1_;
    Card2 card2_;
    Card3 card3_;
};

/**
 * @brief *CONTACT_ERODING_SURFACE_TO_SURFACE
 *
 * Eroding surface to surface contact.
 */
class KOO_API ContactErodingSurfaceToSurface : public CloneableKeyword<ContactErodingSurfaceToSurface, ContactKeyword> {
public:
    using Card1 = ContactAutomaticSingleSurface::Card1;
    using Card2 = ContactAutomaticSingleSurface::Card2;
    using Card3 = ContactAutomaticSingleSurface::Card3;

    ContactErodingSurfaceToSurface() = default;

    std::string getKeywordName() const override { return "*CONTACT_ERODING_SURFACE_TO_SURFACE"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    Card1& getCard1() { return card1_; }
    const Card1& getCard1() const { return card1_; }
    Card2& getCard2() { return card2_; }
    const Card2& getCard2() const { return card2_; }
    Card3& getCard3() { return card3_; }
    const Card3& getCard3() const { return card3_; }

private:
    Card1 card1_;
    Card2 card2_;
    Card3 card3_;
};

/**
 * @brief *CONTACT_ERODING_NODES_TO_SURFACE
 *
 * Eroding nodes to surface contact.
 */
class KOO_API ContactErodingNodesToSurface : public CloneableKeyword<ContactErodingNodesToSurface, ContactKeyword> {
public:
    using Card1 = ContactAutomaticSingleSurface::Card1;
    using Card2 = ContactAutomaticSingleSurface::Card2;
    using Card3 = ContactAutomaticSingleSurface::Card3;

    ContactErodingNodesToSurface() = default;

    std::string getKeywordName() const override { return "*CONTACT_ERODING_NODES_TO_SURFACE"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    Card1& getCard1() { return card1_; }
    const Card1& getCard1() const { return card1_; }
    Card2& getCard2() { return card2_; }
    const Card2& getCard2() const { return card2_; }
    Card3& getCard3() { return card3_; }
    const Card3& getCard3() const { return card3_; }

private:
    Card1 card1_;
    Card2 card2_;
    Card3 card3_;
};

/**
 * @brief *CONTACT_FORMING_ONE_WAY_SURFACE_TO_SURFACE
 *
 * One-way forming contact for stamping simulations.
 */
class KOO_API ContactFormingOneWaySurfaceToSurface : public CloneableKeyword<ContactFormingOneWaySurfaceToSurface, ContactKeyword> {
public:
    using Card1 = ContactAutomaticSingleSurface::Card1;
    using Card2 = ContactAutomaticSingleSurface::Card2;
    using Card3 = ContactAutomaticSingleSurface::Card3;

    ContactFormingOneWaySurfaceToSurface() = default;

    std::string getKeywordName() const override { return "*CONTACT_FORMING_ONE_WAY_SURFACE_TO_SURFACE"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    Card1& getCard1() { return card1_; }
    const Card1& getCard1() const { return card1_; }
    Card2& getCard2() { return card2_; }
    const Card2& getCard2() const { return card2_; }
    Card3& getCard3() { return card3_; }
    const Card3& getCard3() const { return card3_; }

private:
    Card1 card1_;
    Card2 card2_;
    Card3 card3_;
};

/**
 * @brief *CONTACT_INTERIOR
 *
 * Interior contact for detecting interior penetrations.
 */
class KOO_API ContactInterior : public CloneableKeyword<ContactInterior, ContactKeyword> {
public:
    struct Data {
        int ssid = 0;
        int stype = 0;
    };

    ContactInterior() = default;

    std::string getKeywordName() const override { return "*CONTACT_INTERIOR"; }

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
 * @brief *CONTACT_AUTOMATIC_SINGLE_SURFACE_MORTAR
 *
 * Mortar-based automatic single surface contact.
 */
class KOO_API ContactAutomaticSingleSurfaceMortar : public CloneableKeyword<ContactAutomaticSingleSurfaceMortar, ContactKeyword> {
public:
    using Card1 = ContactAutomaticSingleSurface::Card1;
    using Card2 = ContactAutomaticSingleSurface::Card2;
    using Card3 = ContactAutomaticSingleSurface::Card3;

    ContactAutomaticSingleSurfaceMortar() = default;

    std::string getKeywordName() const override { return "*CONTACT_AUTOMATIC_SINGLE_SURFACE_MORTAR"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    Card1& getCard1() { return card1_; }
    const Card1& getCard1() const { return card1_; }
    Card2& getCard2() { return card2_; }
    const Card2& getCard2() const { return card2_; }
    Card3& getCard3() { return card3_; }
    const Card3& getCard3() const { return card3_; }

private:
    Card1 card1_;
    Card2 card2_;
    Card3 card3_;
};

/**
 * @brief *CONTACT_AUTOMATIC_SURFACE_TO_SURFACE_MORTAR
 *
 * Mortar-based automatic surface to surface contact.
 */
class KOO_API ContactAutomaticSurfaceToSurfaceMortar : public CloneableKeyword<ContactAutomaticSurfaceToSurfaceMortar, ContactKeyword> {
public:
    using Card1 = ContactAutomaticSingleSurface::Card1;
    using Card2 = ContactAutomaticSingleSurface::Card2;
    using Card3 = ContactAutomaticSingleSurface::Card3;

    ContactAutomaticSurfaceToSurfaceMortar() = default;

    std::string getKeywordName() const override { return "*CONTACT_AUTOMATIC_SURFACE_TO_SURFACE_MORTAR"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    Card1& getCard1() { return card1_; }
    const Card1& getCard1() const { return card1_; }
    Card2& getCard2() { return card2_; }
    const Card2& getCard2() const { return card2_; }
    Card3& getCard3() { return card3_; }
    const Card3& getCard3() const { return card3_; }

private:
    Card1 card1_;
    Card2 card2_;
    Card3 card3_;
};

/**
 * @brief *CONTACT_TIED_SHELL_EDGE_TO_SURFACE
 *
 * Tied contact for shell edges.
 */
class KOO_API ContactTiedShellEdgeToSurface : public CloneableKeyword<ContactTiedShellEdgeToSurface, ContactKeyword> {
public:
    using Card1 = ContactAutomaticSingleSurface::Card1;
    using Card2 = ContactAutomaticSingleSurface::Card2;

    ContactTiedShellEdgeToSurface() = default;

    std::string getKeywordName() const override { return "*CONTACT_TIED_SHELL_EDGE_TO_SURFACE"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    Card1& getCard1() { return card1_; }
    const Card1& getCard1() const { return card1_; }
    Card2& getCard2() { return card2_; }
    const Card2& getCard2() const { return card2_; }

private:
    Card1 card1_;
    Card2 card2_;
};

/**
 * @brief *CONTACT_DRAWBEAD
 *
 * Drawbead contact for forming simulations.
 */
class KOO_API ContactDrawbead : public CloneableKeyword<ContactDrawbead, ContactKeyword> {
public:
    struct Data {
        int ssid = 0;           // Slave segment set ID
        int msid = 0;           // Master segment set ID
        double lcidrf = 0.0;    // Restraining force load curve
        int nid = 0;            // Normal force node ID
        double dbdth = 0.0;     // Drawbead depth
    };

    ContactDrawbead() = default;

    std::string getKeywordName() const override { return "*CONTACT_DRAWBEAD"; }

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
 * @brief *CONTACT_FORCE_TRANSDUCER_PENALTY
 *
 * Contact force transducer using penalty formulation.
 */
class KOO_API ContactForceTransducerPenalty : public CloneableKeyword<ContactForceTransducerPenalty, ContactKeyword> {
public:
    using Card1 = ContactAutomaticSingleSurface::Card1;
    using Card2 = ContactAutomaticSingleSurface::Card2;

    ContactForceTransducerPenalty() = default;

    std::string getKeywordName() const override { return "*CONTACT_FORCE_TRANSDUCER_PENALTY"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    Card1& getCard1() { return card1_; }
    const Card1& getCard1() const { return card1_; }
    Card2& getCard2() { return card2_; }
    const Card2& getCard2() const { return card2_; }

private:
    Card1 card1_;
    Card2 card2_;
};

/**
 * @brief *CONTACT_2D_AUTOMATIC_SINGLE_SURFACE
 *
 * 2D automatic single surface contact.
 */
class KOO_API Contact2dAutomaticSingleSurface : public CloneableKeyword<Contact2dAutomaticSingleSurface, ContactKeyword> {
public:
    struct Data {
        int ssid = 0;           // Slave segment set ID
        int msid = 0;           // Master segment set ID
        int sstyp = 0;          // Slave surface type
        int mstyp = 0;          // Master surface type
        double fs = 0.0;        // Static friction coefficient
        double fd = 0.0;        // Dynamic friction coefficient
        double dc = 0.0;        // Decay constant
        double soft = 0.0;      // Soft constraint option
    };

    Contact2dAutomaticSingleSurface() = default;

    std::string getKeywordName() const override { return "*CONTACT_2D_AUTOMATIC_SINGLE_SURFACE"; }

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
 * @brief *CONTACT_GEBOD_SEGMENT
 *
 * GEBOD (Human body model) segment contact.
 */
class KOO_API ContactGebodSegment : public CloneableKeyword<ContactGebodSegment, ContactKeyword> {
public:
    struct Data {
        int pid = 0;            // Part ID
        int did = 0;            // Dummy ID
        int segtyp = 0;         // Segment type
        double sf = 1.0;        // Scale factor
        int opt = 0;            // Option
    };

    ContactGebodSegment() = default;

    std::string getKeywordName() const override { return "*CONTACT_GEBOD_SEGMENT"; }

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
 * @brief *CONTACT_AUTOMATIC_SURFACE_TO_SURFACE_TIEBREAK
 *
 * Tiebreak contact for bonded surfaces with failure criteria.
 */
class KOO_API ContactAutomaticSurfaceToSurfaceTiebreak : public CloneableKeyword<ContactAutomaticSurfaceToSurfaceTiebreak, ContactKeyword> {
public:
    using Card1 = ContactAutomaticSingleSurface::Card1;
    using Card2 = ContactAutomaticSingleSurface::Card2;
    using Card3 = ContactAutomaticSingleSurface::Card3;

    struct TiebreakCard {
        int option = 0;         // Tiebreak option (1-6)
        double nfls = 0.0;      // Normal failure stress
        double sfls = 0.0;      // Shear failure stress
        double param = 0.0;     // Optional parameter (depends on option)
        double eraten = 0.0;    // Normal energy release rate (option 6)
        double erates = 0.0;    // Shear energy release rate (option 6)
        double ct2cn = 1.0;     // Ratio of tangential to normal stiffness
        double cn = 0.0;        // Normal stiffness (optional override)
    };

    ContactAutomaticSurfaceToSurfaceTiebreak() = default;

    std::string getKeywordName() const override { return "*CONTACT_AUTOMATIC_SURFACE_TO_SURFACE_TIEBREAK"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    Card1& getCard1() { return card1_; }
    const Card1& getCard1() const { return card1_; }
    Card2& getCard2() { return card2_; }
    const Card2& getCard2() const { return card2_; }
    Card3& getCard3() { return card3_; }
    const Card3& getCard3() const { return card3_; }
    TiebreakCard& getTiebreakCard() { return tiebreak_; }
    const TiebreakCard& getTiebreakCard() const { return tiebreak_; }

private:
    Card1 card1_;
    Card2 card2_;
    Card3 card3_;
    TiebreakCard tiebreak_;
};

/**
 * @brief *CONTACT_AUTOMATIC_SINGLE_SURFACE_ID
 *
 * Automatic single surface contact with title/ID card.
 */
class KOO_API ContactAutomaticSingleSurfaceId : public CloneableKeyword<ContactAutomaticSingleSurfaceId, ContactKeyword> {
public:
    using Card1 = ContactAutomaticSingleSurface::Card1;
    using Card2 = ContactAutomaticSingleSurface::Card2;
    using Card3 = ContactAutomaticSingleSurface::Card3;

    struct IdCard {
        int cid = 0;            // Contact ID
        std::string heading;    // Contact heading/title
    };

    ContactAutomaticSingleSurfaceId() = default;

    std::string getKeywordName() const override { return "*CONTACT_AUTOMATIC_SINGLE_SURFACE_ID"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    IdCard& getIdCard() { return idCard_; }
    const IdCard& getIdCard() const { return idCard_; }
    Card1& getCard1() { return card1_; }
    const Card1& getCard1() const { return card1_; }
    Card2& getCard2() { return card2_; }
    const Card2& getCard2() const { return card2_; }
    Card3& getCard3() { return card3_; }
    const Card3& getCard3() const { return card3_; }

private:
    IdCard idCard_;
    Card1 card1_;
    Card2 card2_;
    Card3 card3_;
};

/**
 * @brief *CONTACT_ENTITY
 *
 * Contact with a geometric entity (sphere, cylinder, plane, etc.).
 */
class KOO_API ContactEntity : public CloneableKeyword<ContactEntity, ContactKeyword> {
public:
    struct Data {
        int slave = 0;          // Slave set ID
        int master = 0;         // Entity ID
        int sstyp = 0;          // Slave set type
        int mstyp = 0;          // Master entity type (1=plane, 2=sphere, 3=cylinder, etc.)
        int sboxid = 0;         // Slave box ID
        int mboxid = 0;         // Master box ID
        double fs = 0.0;        // Static friction coefficient
        double fd = 0.0;        // Dynamic friction coefficient
        double dc = 0.0;        // Decay constant
        double vc = 0.0;        // Viscous friction
        double penchk = 0.0;    // Penetration check flag
        double bt = 0.0;        // Birth time
        double dt = 1.0e20;     // Death time
    };

    ContactEntity() = default;

    std::string getKeywordName() const override { return "*CONTACT_ENTITY"; }

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
 * @brief *CONTACT_AUTOMATIC_SURFACE_TO_SURFACE_ID
 *
 * Automatic surface to surface contact with ID card.
 */
class KOO_API ContactAutomaticSurfaceToSurfaceId : public CloneableKeyword<ContactAutomaticSurfaceToSurfaceId, ContactKeyword> {
public:
    using Card1 = ContactAutomaticSingleSurface::Card1;
    using Card2 = ContactAutomaticSingleSurface::Card2;
    using Card3 = ContactAutomaticSingleSurface::Card3;

    ContactAutomaticSurfaceToSurfaceId() = default;

    std::string getKeywordName() const override { return "*CONTACT_AUTOMATIC_SURFACE_TO_SURFACE_ID"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    int getContactId() const { return cid_; }
    void setContactId(int id) { cid_ = id; }
    const std::string& getHeading() const { return heading_; }
    void setHeading(const std::string& h) { heading_ = h; }

    Card1& getCard1() { return card1_; }
    const Card1& getCard1() const { return card1_; }
    Card2& getCard2() { return card2_; }
    const Card2& getCard2() const { return card2_; }
    Card3& getCard3() { return card3_; }
    const Card3& getCard3() const { return card3_; }

private:
    int cid_ = 0;
    std::string heading_;
    Card1 card1_;
    Card2 card2_;
    Card3 card3_;
};

/**
 * @brief *CONTACT_FORMING_SURFACE_TO_SURFACE
 *
 * Forming surface to surface contact.
 */
class KOO_API ContactFormingSurfaceToSurface : public CloneableKeyword<ContactFormingSurfaceToSurface, ContactKeyword> {
public:
    using Card1 = ContactAutomaticSingleSurface::Card1;
    using Card2 = ContactAutomaticSingleSurface::Card2;
    using Card3 = ContactAutomaticSingleSurface::Card3;

    ContactFormingSurfaceToSurface() = default;

    std::string getKeywordName() const override { return "*CONTACT_FORMING_SURFACE_TO_SURFACE"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    Card1& getCard1() { return card1_; }
    const Card1& getCard1() const { return card1_; }
    Card2& getCard2() { return card2_; }
    const Card2& getCard2() const { return card2_; }
    Card3& getCard3() { return card3_; }
    const Card3& getCard3() const { return card3_; }

private:
    Card1 card1_;
    Card2 card2_;
    Card3 card3_;
};

/**
 * @brief *CONTACT_SINGLE_SURFACE
 *
 * Single surface contact (non-automatic).
 */
class KOO_API ContactSingleSurface : public CloneableKeyword<ContactSingleSurface, ContactKeyword> {
public:
    using Card1 = ContactAutomaticSingleSurface::Card1;
    using Card2 = ContactAutomaticSingleSurface::Card2;
    using Card3 = ContactAutomaticSingleSurface::Card3;

    ContactSingleSurface() = default;

    std::string getKeywordName() const override { return "*CONTACT_SINGLE_SURFACE"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    Card1& getCard1() { return card1_; }
    const Card1& getCard1() const { return card1_; }
    Card2& getCard2() { return card2_; }
    const Card2& getCard2() const { return card2_; }
    Card3& getCard3() { return card3_; }
    const Card3& getCard3() const { return card3_; }

private:
    Card1 card1_;
    Card2 card2_;
    Card3 card3_;
};

/**
 * @brief *CONTACT_AUTOMATIC_BEAMS_TO_SURFACE
 *
 * Automatic beams to surface contact.
 */
class KOO_API ContactAutomaticBeamsToSurface : public CloneableKeyword<ContactAutomaticBeamsToSurface, ContactKeyword> {
public:
    using Card1 = ContactAutomaticSingleSurface::Card1;
    using Card2 = ContactAutomaticSingleSurface::Card2;
    using Card3 = ContactAutomaticSingleSurface::Card3;

    ContactAutomaticBeamsToSurface() = default;

    std::string getKeywordName() const override { return "*CONTACT_AUTOMATIC_BEAMS_TO_SURFACE"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    Card1& getCard1() { return card1_; }
    const Card1& getCard1() const { return card1_; }
    Card2& getCard2() { return card2_; }
    const Card2& getCard2() const { return card2_; }
    Card3& getCard3() { return card3_; }
    const Card3& getCard3() const { return card3_; }

private:
    Card1 card1_;
    Card2 card2_;
    Card3 card3_;
};

/**
 * @brief *CONTACT_TIED_SHELL_EDGE_TO_SOLID
 *
 * Tied contact between shell edge and solid elements.
 */
class KOO_API ContactTiedShellEdgeToSolid : public CloneableKeyword<ContactTiedShellEdgeToSolid, ContactKeyword> {
public:
    using Card1 = ContactAutomaticSingleSurface::Card1;
    using Card2 = ContactAutomaticSingleSurface::Card2;

    ContactTiedShellEdgeToSolid() = default;

    std::string getKeywordName() const override { return "*CONTACT_TIED_SHELL_EDGE_TO_SOLID"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    Card1& getCard1() { return card1_; }
    const Card1& getCard1() const { return card1_; }
    Card2& getCard2() { return card2_; }
    const Card2& getCard2() const { return card2_; }

private:
    Card1 card1_;
    Card2 card2_;
};

/**
 * @brief *CONTACT_RIGID_BODY_TWO_WAY
 *
 * Two-way rigid body contact.
 */
class KOO_API ContactRigidBodyTwoWay : public CloneableKeyword<ContactRigidBodyTwoWay, ContactKeyword> {
public:
    using Card1 = ContactAutomaticSingleSurface::Card1;
    using Card2 = ContactAutomaticSingleSurface::Card2;

    ContactRigidBodyTwoWay() = default;

    std::string getKeywordName() const override { return "*CONTACT_RIGID_BODY_TWO_WAY"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    Card1& getCard1() { return card1_; }
    const Card1& getCard1() const { return card1_; }
    Card2& getCard2() { return card2_; }
    const Card2& getCard2() const { return card2_; }

private:
    Card1 card1_;
    Card2 card2_;
};

/**
 * @brief *CONTACT_AUTOMATIC_NODES_TO_SURFACE_ID
 *
 * Automatic nodes to surface contact with ID.
 */
class KOO_API ContactAutomaticNodesToSurfaceId : public CloneableKeyword<ContactAutomaticNodesToSurfaceId, ContactKeyword> {
public:
    using Card1 = ContactAutomaticSingleSurface::Card1;
    using Card2 = ContactAutomaticSingleSurface::Card2;
    using Card3 = ContactAutomaticSingleSurface::Card3;

    struct IdCard {
        int cid = 0;
        std::string heading;
    };

    ContactAutomaticNodesToSurfaceId() = default;

    std::string getKeywordName() const override { return "*CONTACT_AUTOMATIC_NODES_TO_SURFACE_ID"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    IdCard& getIdCard() { return idCard_; }
    const IdCard& getIdCard() const { return idCard_; }
    Card1& getCard1() { return card1_; }
    const Card1& getCard1() const { return card1_; }
    Card2& getCard2() { return card2_; }
    const Card2& getCard2() const { return card2_; }
    Card3& getCard3() { return card3_; }
    const Card3& getCard3() const { return card3_; }

private:
    IdCard idCard_;
    Card1 card1_;
    Card2 card2_;
    Card3 card3_;
};

/**
 * @brief *CONTACT_AIRBAG_SINGLE_SURFACE
 *
 * Airbag single surface contact for inflatable structures.
 */
class KOO_API ContactAirbagSingleSurface : public CloneableKeyword<ContactAirbagSingleSurface, ContactKeyword> {
public:
    using Card1 = ContactAutomaticSingleSurface::Card1;
    using Card2 = ContactAutomaticSingleSurface::Card2;

    ContactAirbagSingleSurface() = default;

    std::string getKeywordName() const override { return "*CONTACT_AIRBAG_SINGLE_SURFACE"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    Card1& getCard1() { return card1_; }
    const Card1& getCard1() const { return card1_; }
    Card2& getCard2() { return card2_; }
    const Card2& getCard2() const { return card2_; }

private:
    Card1 card1_;
    Card2 card2_;
};

/**
 * @brief *CONTACT_GUIDED_CABLE
 *
 * Guided cable contact for seatbelt and cable modeling.
 */
class KOO_API ContactGuidedCable : public CloneableKeyword<ContactGuidedCable, ContactKeyword> {
public:
    struct Data {
        int ssid = 0;           // Slave segment set ID
        int msid = 0;           // Master segment set ID
        int sstyp = 0;          // Slave surface type
        int mstyp = 0;          // Master surface type
        double fs = 0.0;        // Static friction coefficient
        double fd = 0.0;        // Dynamic friction coefficient
    };

    ContactGuidedCable() = default;

    std::string getKeywordName() const override { return "*CONTACT_GUIDED_CABLE"; }

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
 * @brief *CONTACT_TIEBREAK_NODES_TO_SURFACE
 *
 * Tiebreak contact between nodes and surface.
 */
class KOO_API ContactTiebreakNodesToSurface : public CloneableKeyword<ContactTiebreakNodesToSurface, ContactKeyword> {
public:
    using Card1 = ContactAutomaticSingleSurface::Card1;
    using Card2 = ContactAutomaticSingleSurface::Card2;

    struct TiebreakCard {
        double nfls = 0.0;      // Normal failure stress
        double sfls = 0.0;      // Shear failure stress
        double eraten = 0.0;    // Normal energy release rate
        double erates = 0.0;    // Shear energy release rate
        double ct2cn = 1.0;     // Ratio of tangent to normal stiffness
        int option = 0;         // Tiebreak option
    };

    ContactTiebreakNodesToSurface() = default;

    std::string getKeywordName() const override { return "*CONTACT_TIEBREAK_NODES_TO_SURFACE"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    Card1& getCard1() { return card1_; }
    const Card1& getCard1() const { return card1_; }
    Card2& getCard2() { return card2_; }
    const Card2& getCard2() const { return card2_; }
    TiebreakCard& getTiebreakCard() { return tiebreakCard_; }
    const TiebreakCard& getTiebreakCard() const { return tiebreakCard_; }

private:
    Card1 card1_;
    Card2 card2_;
    TiebreakCard tiebreakCard_;
};

/**
 * @brief *CONTACT_TIEBREAK_SURFACE_TO_SURFACE
 *
 * Tiebreak contact between two surfaces.
 */
class KOO_API ContactTiebreakSurfaceToSurface : public CloneableKeyword<ContactTiebreakSurfaceToSurface, ContactKeyword> {
public:
    using Card1 = ContactAutomaticSingleSurface::Card1;
    using Card2 = ContactAutomaticSingleSurface::Card2;
    using TiebreakCard = ContactTiebreakNodesToSurface::TiebreakCard;

    ContactTiebreakSurfaceToSurface() = default;

    std::string getKeywordName() const override { return "*CONTACT_TIEBREAK_SURFACE_TO_SURFACE"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    Card1& getCard1() { return card1_; }
    const Card1& getCard1() const { return card1_; }
    Card2& getCard2() { return card2_; }
    const Card2& getCard2() const { return card2_; }
    TiebreakCard& getTiebreakCard() { return tiebreakCard_; }
    const TiebreakCard& getTiebreakCard() const { return tiebreakCard_; }

private:
    Card1 card1_;
    Card2 card2_;
    TiebreakCard tiebreakCard_;
};

/**
 * @brief *CONTACT_SLIDING_ONLY_PENALTY
 *
 * Sliding only contact with penalty method.
 */
class KOO_API ContactSlidingOnlyPenalty : public CloneableKeyword<ContactSlidingOnlyPenalty, ContactKeyword> {
public:
    using Card1 = ContactAutomaticSingleSurface::Card1;
    using Card2 = ContactAutomaticSingleSurface::Card2;

    ContactSlidingOnlyPenalty() = default;

    std::string getKeywordName() const override { return "*CONTACT_SLIDING_ONLY_PENALTY"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    Card1& getCard1() { return card1_; }
    const Card1& getCard1() const { return card1_; }
    Card2& getCard2() { return card2_; }
    const Card2& getCard2() const { return card2_; }

private:
    Card1 card1_;
    Card2 card2_;
};

/**
 * @brief *CONTACT_OPTION
 *
 * Defines additional contact options.
 */
class KOO_API ContactOption : public CloneableKeyword<ContactOption, ContactKeyword> {
public:
    struct Data {
        int cid = 0;            // Contact ID
        int option = 0;         // Option type
        double value = 0.0;     // Option value
    };

    ContactOption() = default;

    std::string getKeywordName() const override { return "*CONTACT_OPTION"; }

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
 * @brief *CONTACT_ADD_WEAR
 *
 * Adds wear modeling to contact surfaces.
 */
class KOO_API ContactAddWear : public CloneableKeyword<ContactAddWear, ContactKeyword> {
public:
    struct Data {
        int cid = 0;            // Contact ID
        double k1 = 0.0;        // Wear coefficient for surface 1
        double k2 = 0.0;        // Wear coefficient for surface 2
        int lcid = 0;           // Load curve for wear
    };

    ContactAddWear() = default;

    std::string getKeywordName() const override { return "*CONTACT_ADD_WEAR"; }

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
 * @brief *CONTACT_2D_AUTOMATIC_SURFACE_TO_SURFACE
 *
 * 2D automatic surface to surface contact.
 */
class KOO_API Contact2DAutomaticSurfaceToSurface : public CloneableKeyword<Contact2DAutomaticSurfaceToSurface, ContactKeyword> {
public:
    using Card1 = ContactAutomaticSingleSurface::Card1;
    using Card2 = ContactAutomaticSingleSurface::Card2;

    Contact2DAutomaticSurfaceToSurface() = default;

    std::string getKeywordName() const override { return "*CONTACT_2D_AUTOMATIC_SURFACE_TO_SURFACE"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    Card1& getCard1() { return card1_; }
    const Card1& getCard1() const { return card1_; }
    Card2& getCard2() { return card2_; }
    const Card2& getCard2() const { return card2_; }

private:
    Card1 card1_;
    Card2 card2_;
};

/**
 * @brief *CONTACT_2D_NODES_TO_SURFACE
 *
 * 2D nodes to surface contact.
 */
class KOO_API Contact2DNodesToSurface : public CloneableKeyword<Contact2DNodesToSurface, ContactKeyword> {
public:
    using Card1 = ContactAutomaticSingleSurface::Card1;
    using Card2 = ContactAutomaticSingleSurface::Card2;

    Contact2DNodesToSurface() = default;

    std::string getKeywordName() const override { return "*CONTACT_2D_NODES_TO_SURFACE"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    Card1& getCard1() { return card1_; }
    const Card1& getCard1() const { return card1_; }
    Card2& getCard2() { return card2_; }
    const Card2& getCard2() const { return card2_; }

private:
    Card1 card1_;
    Card2 card2_;
};

} // namespace koo::dyna
