#pragma once

#include <koo/Export.hpp>
#include <koo/dyna/Keyword.hpp>
#include <koo/util/Types.hpp>

namespace koo::dyna {

/**
 * @brief Base class for *CONTROL keywords
 */
class KOO_API ControlKeyword : public Keyword {
public:
    virtual ~ControlKeyword() = default;
};

/**
 * @brief *CONTROL_TERMINATION
 *
 * Defines termination conditions for the analysis.
 */
class KOO_API ControlTermination : public CloneableKeyword<ControlTermination, ControlKeyword> {
public:
    struct Data {
        double endtim = 0.0;    // Termination time
        double endcyc = 0.0;    // Termination cycle
        double dtmin = 0.0;     // Minimum time step (if > 0, terminates when dt < dtmin)
        double endeng = 0.0;    // Termination energy ratio
        double endmas = 0.0;    // Termination mass ratio
        int nosol = 0;          // No solution flag (1 = no solution, data check only)
    };

    ControlTermination() = default;

    std::string getKeywordName() const override { return "*CONTROL_TERMINATION"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    // Data access
    Data& getData() { return data_; }
    const Data& getData() const { return data_; }

    // Convenience accessors
    double getEndTime() const { return data_.endtim; }
    void setEndTime(double t) { data_.endtim = t; }

private:
    Data data_;
};

/**
 * @brief *CONTROL_TIMESTEP
 *
 * Controls time step size during explicit analysis.
 */
class KOO_API ControlTimestep : public CloneableKeyword<ControlTimestep, ControlKeyword> {
public:
    struct Data {
        double dtinit = 0.0;    // Initial time step (0.0 = auto)
        double tssfac = 0.9;    // Scale factor for computed time step
        int isdo = 0;           // Basis of time step size (0=char length, 1=1/(2*f))
        double tslimt = 0.0;    // Shell min time step
        double dt2ms = 0.0;     // Time step for mass scaling
        int lctm = 0;           // Load curve ID for time step vs time
        int erode = 0;          // Erosion flag
        double ms1st = 0.0;     // Mass scaling first step
    };

    ControlTimestep() = default;

    std::string getKeywordName() const override { return "*CONTROL_TIMESTEP"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    // Data access
    Data& getData() { return data_; }
    const Data& getData() const { return data_; }

    // Convenience accessors
    double getScaleFactor() const { return data_.tssfac; }
    void setScaleFactor(double sf) { data_.tssfac = sf; }

private:
    Data data_;
};

/**
 * @brief *CONTROL_ENERGY
 *
 * Controls energy dissipation options.
 */
class KOO_API ControlEnergy : public CloneableKeyword<ControlEnergy, ControlKeyword> {
public:
    struct Data {
        int hgen = 2;           // Hourglass energy (1=computed, 2=not computed)
        int rwen = 2;           // Stonewall energy (1=computed, 2=not computed)
        int slnten = 2;         // Sliding interface energy (1=computed, 2=not computed)
        int rylen = 2;          // Rayleigh energy dissipation (1=computed, 2=not computed)
        int irgen = 2;          // Initial reference geometry energy (1=computed, 2=not computed)
        int mession = 2;        // Lumped mass extra energy (1=computed, 2=not computed)
        int rayession = 2;      // Rayleigh energy session
        int fession = 2;        // Energy session flag
    };

    ControlEnergy() = default;

    std::string getKeywordName() const override { return "*CONTROL_ENERGY"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    // Data access
    Data& getData() { return data_; }
    const Data& getData() const { return data_; }

private:
    Data data_;
};

/**
 * @brief *CONTROL_OUTPUT
 *
 * Controls various output options.
 */
class KOO_API ControlOutput : public CloneableKeyword<ControlOutput, ControlKeyword> {
public:
    struct Data {
        int npopt = 0;          // Print options (0=default)
        int netefr = 0;         // External work print flag
        int nrefup = 0;         // Flag to update reference geometry
        int iaccop = 0;         // Acceleration output option
        int optefr = 0;         // Optimized elform ref flag
        int ipnint = 0;         // Integration point print
        int iketefr = 0;        // Kinetic energy flag
        int iftefr = 0;         // Internal force flag
    };

    ControlOutput() = default;

    std::string getKeywordName() const override { return "*CONTROL_OUTPUT"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    // Data access
    Data& getData() { return data_; }
    const Data& getData() const { return data_; }

private:
    Data data_;
};

/**
 * @brief *CONTROL_CONTACT
 *
 * Global contact control parameters.
 */
class KOO_API ControlContact : public CloneableKeyword<ControlContact, ControlKeyword> {
public:
    struct Data {
        double slsfac = 0.1;    // Scale factor for sliding penalty
        double rwpnal = 0.0;    // Scale factor for rigid wall penalty
        int islchk = 1;         // Initial penetration check
        int shlthk = 0;         // Shell thickness consideration
        int penopt = 0;         // Penalty stiffness option
        int thkchg = 0;         // Shell thickness change flag
        int otefr = 0;          // Optimization flag
        int enmass = 0;         // Mass treatment flag
    };

    ControlContact() = default;

    std::string getKeywordName() const override { return "*CONTROL_CONTACT"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    // Data access
    Data& getData() { return data_; }
    const Data& getData() const { return data_; }

private:
    Data data_;
};

/**
 * @brief *CONTROL_HOURGLASS
 *
 * Default hourglass control.
 */
class KOO_API ControlHourglass : public CloneableKeyword<ControlHourglass, ControlKeyword> {
public:
    struct Data {
        int ihq = 0;            // Hourglass control type (default)
        double qh = 0.1;        // Hourglass coefficient
    };

    ControlHourglass() = default;

    std::string getKeywordName() const override { return "*CONTROL_HOURGLASS"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    // Data access
    Data& getData() { return data_; }
    const Data& getData() const { return data_; }

private:
    Data data_;
};

/**
 * @brief *CONTROL_BULK_VISCOSITY
 *
 * Bulk viscosity coefficients.
 */
class KOO_API ControlBulkViscosity : public CloneableKeyword<ControlBulkViscosity, ControlKeyword> {
public:
    struct Data {
        double q1 = 1.5;        // Quadratic viscosity coefficient
        double q2 = 0.06;       // Linear viscosity coefficient
        int type = 1;           // Bulk viscosity type
        double btype = 0.0;     // Beam viscosity type
    };

    ControlBulkViscosity() = default;

    std::string getKeywordName() const override { return "*CONTROL_BULK_VISCOSITY"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    // Data access
    Data& getData() { return data_; }
    const Data& getData() const { return data_; }

private:
    Data data_;
};

/**
 * @brief *CONTROL_SHELL
 *
 * Shell element default settings.
 */
class KOO_API ControlShell : public CloneableKeyword<ControlShell, ControlKeyword> {
public:
    struct Data {
        double wrpang = 20.0;   // Warpage angle in degrees
        int esort = 0;          // Element sorting
        int irnxx = -1;         // Shell normal update option
        int istupd = 0;         // Shell thickness update
        double theory = 2.0;    // Shell theory type
        double bwc = 1.0;       // Bandwidth control
        int miter = 1;          // Plane stress iteration
        int proj = 0;           // Projection type
    };

    ControlShell() = default;

    std::string getKeywordName() const override { return "*CONTROL_SHELL"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    // Data access
    Data& getData() { return data_; }
    const Data& getData() const { return data_; }

private:
    Data data_;
};

/**
 * @brief *CONTROL_SOLID
 *
 * Solid element default settings.
 */
class KOO_API ControlSolid : public CloneableKeyword<ControlSolid, ControlKeyword> {
public:
    struct Data {
        int esort = 0;          // Element sorting
        int fmatrx = 0;         // Formation of [F] matrix
        int niptets = 0;        // Number of IPs in tets
        int swlocl = 1;         // Output stress/strain in local
        int pession = 0;        // Pressure session
        int niptso = 8;         // Number of IPs solid output
        int dt2msso = 0;        // Mass scaling for solid
        int lmc = 0;            // Mass matrix form
    };

    ControlSolid() = default;

    std::string getKeywordName() const override { return "*CONTROL_SOLID"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    // Data access
    Data& getData() { return data_; }
    const Data& getData() const { return data_; }

private:
    Data data_;
};

/**
 * @brief *CONTROL_ACCURACY
 *
 * Accuracy control settings.
 */
class KOO_API ControlAccuracy : public CloneableKeyword<ControlAccuracy, ControlKeyword> {
public:
    struct Data {
        int osu = 0;            // Objective stress update
        int inn = 0;            // Invariant node numbering
        int pidosu = 0;         // Part set for OSU
        int iatefr = 0;         // Accuracy flag
    };

    ControlAccuracy() = default;

    std::string getKeywordName() const override { return "*CONTROL_ACCURACY"; }

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
 * @brief *CONTROL_RIGID
 *
 * Global rigid body control options.
 */
class KOO_API ControlRigid : public CloneableKeyword<ControlRigid, ControlKeyword> {
public:
    struct Data {
        int lmf = 0;            // Local to material formulation
        int jntf = 0;           // Joint formulation
        int orthmd = 0;         // Orthotropic material direction
        int partm = 0;          // Partition method
        int sparse = 0;         // Sparse storage
        int metalf = 0;         // Metal forming flag
    };

    ControlRigid() = default;

    std::string getKeywordName() const override { return "*CONTROL_RIGID"; }

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
 * @brief *CONTROL_CPU
 *
 * CPU time control settings.
 */
class KOO_API ControlCpu : public CloneableKeyword<ControlCpu, ControlKeyword> {
public:
    struct Data {
        double cputime = 0.0;   // Maximum CPU time in seconds
    };

    ControlCpu() = default;

    std::string getKeywordName() const override { return "*CONTROL_CPU"; }

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
 * @brief *CONTROL_PARALLEL
 *
 * Parallel processing control settings.
 */
class KOO_API ControlParallel : public CloneableKeyword<ControlParallel, ControlKeyword> {
public:
    struct Data {
        int ncpu = 0;           // Number of CPUs
        int numrhs = 0;         // Number of RHS
        int const1 = 0;         // Constant 1
        int const2 = 0;         // Constant 2
    };

    ControlParallel() = default;

    std::string getKeywordName() const override { return "*CONTROL_PARALLEL"; }

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
 * @brief *CONTROL_DYNAMIC_RELAXATION
 *
 * Dynamic relaxation control for static analysis.
 */
class KOO_API ControlDynamicRelaxation : public CloneableKeyword<ControlDynamicRelaxation, ControlKeyword> {
public:
    struct Data {
        int nrcyck = 250;       // Number of cycles between convergence checks
        double drterm = 0.0;    // Termination tolerance
        double drfctr = 0.995;  // Relaxation factor
        double drrel = 0.0;     // Relative convergence tolerance
        int itefr = 0;          // Iteration flag
        int dtefr = 0;          // Dynamic flag
        double tssfdr = 1.0;    // Time step scale factor
    };

    ControlDynamicRelaxation() = default;

    std::string getKeywordName() const override { return "*CONTROL_DYNAMIC_RELAXATION"; }

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
 * @brief *CONTROL_MPP_DECOMPOSITION_DISTRIBUTE_ALE_ELEMENTS
 *
 * MPP decomposition options for ALE elements.
 */
class KOO_API ControlMppDecompositionDistributeAleElements : public CloneableKeyword<ControlMppDecompositionDistributeAleElements, ControlKeyword> {
public:
    struct Data {
        int option = 0;         // Distribution option
    };

    ControlMppDecompositionDistributeAleElements() = default;

    std::string getKeywordName() const override { return "*CONTROL_MPP_DECOMPOSITION_DISTRIBUTE_ALE_ELEMENTS"; }

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

// Note: ControlImplicitGeneral, ControlImplicitAuto, ControlImplicitSolution
// are defined in Implicit.hpp

/**
 * @brief *CONTROL_ALE
 *
 * ALE (Arbitrary Lagrangian-Eulerian) controls.
 */
class KOO_API ControlAle : public CloneableKeyword<ControlAle, ControlKeyword> {
public:
    struct Data {
        int dtefr = 0;          // Default ALE element formulation
        int mtefr = 0;          // Multi-material ALE flag
        int smtefr = 0;         // Structured mesh flag
        int start = 0;          // Start cycle
        int end = 0;            // End cycle
        double dttefr = 0.0;    // Time step
    };

    ControlAle() = default;

    std::string getKeywordName() const override { return "*CONTROL_ALE"; }

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

// Note: ControlSph is defined in Sph.hpp
// Note: ControlThermalSolver, ControlThermalTimestep are defined in Thermal.hpp

/**
 * @brief *CONTROL_IMPLICIT_FORMING
 *
 * Controls implicit forming analysis parameters.
 */
class KOO_API ControlImplicitForming : public CloneableKeyword<ControlImplicitForming, ControlKeyword> {
public:
    struct Data {
        int tefr = 0;           // Type of forming analysis
        int nsrs = 0;           // Number of simulation restart states
        int psid = 0;           // Part set ID
        int nrcf = 0;           // Number of reforming cycles per step
        int ilcnt = 0;          // Initial load curve number for forming
        int ncpf = 0;           // Number of cycles per forming
        double dtefr = 0.0;     // Delta time factor
        int rfgap = 0;          // Reform gap flag
    };

    ControlImplicitForming() = default;

    std::string getKeywordName() const override { return "*CONTROL_IMPLICIT_FORMING"; }

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
 * @brief *CONTROL_FORMING_BESTFIT
 *
 * Controls best-fit options for forming simulation comparison.
 */
class KOO_API ControlFormingBestfit : public CloneableKeyword<ControlFormingBestfit, ControlKeyword> {
public:
    struct Data {
        int pid = 0;            // Part ID
        int thick = 0;          // Thickness flag
        int spcrpt = 0;         // Spot check report
        int extend = 0;         // Extension flag
        double sf = 1.0;        // Scale factor
        int irefn = 0;          // Reference node
        int local = 0;          // Local coordinate flag
        int orient = 0;         // Orientation flag
    };

    ControlFormingBestfit() = default;

    std::string getKeywordName() const override { return "*CONTROL_FORMING_BESTFIT"; }

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
 * @brief *CONTROL_FORMING_ONESTEP
 *
 * Controls one-step forming analysis parameters.
 */
class KOO_API ControlFormingOnestep : public CloneableKeyword<ControlFormingOnestep, ControlKeyword> {
public:
    struct Data {
        int blkid = 0;          // Blank ID
        int blktyp = 0;         // Blank type
        int dieid = 0;          // Die part ID
        int pid = 0;            // Part ID for forming
        int ftype = 0;          // Forming type
        int option = 0;         // Analysis option
        int nstep = 1;          // Number of steps
        double tol = 0.001;     // Tolerance
    };

    ControlFormingOnestep() = default;

    std::string getKeywordName() const override { return "*CONTROL_FORMING_ONESTEP"; }

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
 * @brief *CONTROL_REMESHING
 *
 * Controls remeshing parameters for adaptive mesh refinement.
 */
class KOO_API ControlRemeshing : public CloneableKeyword<ControlRemeshing, ControlKeyword> {
public:
    struct Data {
        int rmshtyp = 0;        // Remeshing type
        int rmshint = 0;        // Remeshing interval
        double rmshtol = 0.0;   // Remeshing tolerance
        int rmshopt = 0;        // Remeshing option
        int rmshmax = 0;        // Maximum remeshing level
        double sizscl = 1.0;    // Size scale factor
        int nsmth = 0;          // Number of smoothing iterations
        int rmshfq = 0;         // Remeshing frequency
    };

    ControlRemeshing() = default;

    std::string getKeywordName() const override { return "*CONTROL_REMESHING"; }

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
 * @brief *CONTROL_ADAPTIVE
 *
 * Controls adaptive mesh refinement parameters.
 */
class KOO_API ControlAdaptive : public CloneableKeyword<ControlAdaptive, ControlKeyword> {
public:
    struct Data {
        int adpfreq = 0;        // Adaptive frequency (cycles)
        double adptol = 0.0;    // Adaptive tolerance
        int adpopt = 0;         // Adaptation option
        int maxlvl = 3;         // Maximum refinement level
        double tbirth = 0.0;    // Birth time for adaptive
        double tdeath = 1e28;   // Death time for adaptive
        int lcadp = 0;          // Load curve ID for adaptive frequency
        int ioflag = 0;         // Output flag
    };

    ControlAdaptive() = default;

    std::string getKeywordName() const override { return "*CONTROL_ADAPTIVE"; }

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
 * @brief *CONTROL_SPOTWELD_BEAM
 *
 * Controls spotweld beam element behavior.
 */
class KOO_API ControlSpotweldBeam : public CloneableKeyword<ControlSpotweldBeam, ControlKeyword> {
public:
    struct Data {
        int nfail = 0;          // Failure flag
        int tbirth = 0;         // Birth time flag
        int tdeath = 0;         // Death time flag
        double sclmr = 1.0;     // Scale factor for mass ratio
    };

    ControlSpotweldBeam() = default;

    std::string getKeywordName() const override { return "*CONTROL_SPOTWELD_BEAM"; }

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

// Note: ControlImplicitGeneral, ControlImplicitAuto, ControlImplicitSolution,
// ControlImplicitSolver, ControlImplicitEigenvalue, ControlImplicitBuckle,
// ControlImplicitDynamics are defined in Implicit.hpp

/**
 * @brief *CONTROL_BEAM
 *
 * Controls beam element default settings.
 */
class KOO_API ControlBeam : public CloneableKeyword<ControlBeam, ControlKeyword> {
public:
    struct Data {
        int elform = 0;         // Default element formulation
        int shtefr = 0;         // Shear treatment flag
        int warpflag = 0;       // Warping flag
        int betefr = 0;         // Beam theory flag
    };

    ControlBeam() = default;

    std::string getKeywordName() const override { return "*CONTROL_BEAM"; }

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
 * @brief *CONTROL_SUBCYCLE
 *
 * Controls subcycling for mixed time step integration.
 */
class KOO_API ControlSubcycle : public CloneableKeyword<ControlSubcycle, ControlKeyword> {
public:
    struct Data {
        int sctefr = 0;         // Subcycle type flag
        int mtefr = 0;          // Multi-scale flag
        int ltefr = 0;          // Local flag
        int dtefr = 0;          // Domain flag
    };

    ControlSubcycle() = default;

    std::string getKeywordName() const override { return "*CONTROL_SUBCYCLE"; }

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
