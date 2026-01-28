#pragma once

#include <koo/Export.hpp>

namespace koo::dyna {

// Forward declarations for all keyword classes
class AirbagAdiabaticGasModel;
class AirbagHybrid;
class AirbagInteraction;
class AirbagLinearFluid;
class AirbagLoadCurve;
class AirbagParticle;
class AirbagReferenceGeometry;
class AirbagSimpleAirbagModel;
class AirbagSimplePressureVolume;
class AirbagWangNefske;
class AleAmbientHydrostatic;
class AleCouplingNodalConstraint;
class AleEssentialBoundary;
class AleFailSwitchMmg;
class AleFsiSwitchMmg;
class AleMultiMaterialGroup;
class AleReferenceSystemGroup;
class AleReferenceSystemNode;
class AleSmoothing;
class AleStructuredMesh;
class AleStructuredMeshControlPoints;
class BoundaryAcousticCoupling;
class BoundaryAcousticFreeSurface;
class BoundaryAcousticImpedance;
class BoundaryAcousticMapping;
class BoundaryAcousticNonReflecting;
class BoundaryAleMapping;
class BoundaryAmbient;
class BoundaryAmbientEos;
class BoundaryConvection;
class BoundaryConvectionSet;
class BoundaryCoupled;
class BoundaryCyclic;
class BoundaryDeNonReflecting;
class BoundaryElementMethod;
class BoundaryElementMethodControl;
class BoundaryFluidmFreeSurface;
class BoundaryFlux;
class BoundaryFluxSet;
class BoundaryFluxTrajectory;
class BoundaryMcol;
class BoundaryNonReflecting;
class BoundaryNonReflecting2D;
class BoundaryPap;
class BoundaryPoreFluid;
class BoundaryPrecrack;
class BoundaryPrescribedAccelerometer;
class BoundaryPrescribedAccelerometerRigid;
class BoundaryPrescribedFinalGeometry;
class BoundaryPrescribedMotionNode;
class BoundaryPrescribedMotionNodeId;
class BoundaryPrescribedMotionRigid;
class BoundaryPrescribedMotionRigidLocal;
class BoundaryPrescribedMotionSet;
class BoundaryPrescribedMotionSetBox;
class BoundaryPrescribedMotionSetLine;
class BoundaryPrescribedOrientationRigid;
class BoundaryPressureOutflow;
class BoundaryPwp;
class BoundaryPwpNode;
class BoundaryPzepot;
class BoundaryRadiation;
class BoundaryRadiationSegment;
class BoundaryRadiationSet;
class BoundarySaleMeshFace;
class BoundarySlidingPlane;
class BoundarySpc;
class BoundarySpcNode;
class BoundarySpcSet;
class BoundarySpcSetBirthDeath;
class BoundarySpcSymmetryPlane;
class BoundarySphFlow;
class BoundarySphNonReflecting;
class BoundarySphNoslip;
class BoundarySphPeriodic;
class BoundarySphSymmetryPlane;
class BoundarySymmetryFailure;
class BoundaryTemperature;
class BoundaryTemperatureRsw;
class BoundaryTemperatureTrajectory;
class BoundaryThermalBulknode;
class BoundaryThermalNode;
class BoundaryThermalSet;
class BoundaryThermalWeld;
class BoundaryThermalWeldTrajectory;
class BoundaryUsaSurface;
class CeseBoundaryNonReflective;
class CeseBoundaryPrescribed;
class CeseBoundaryReflective;
class CeseControlMesh;
class CeseControlOutput;
class CeseControlSolver;
class CeseControlTimestep;
class CeseInitial;
class CeseMat;
class CesePart;
class ChemistryComposition;
class ChemistryControl;
class ChemistryControlInflator;
class ChemistryControlZnd;
class ChemistryDetonation;
class ChemistryInflatorProperties;
class ChemistryModel;
class ChemistryReaction;
class ChemistrySpecies;
class ConstrainedBeamInSolid;
class ConstrainedExtraNodesNode;
class ConstrainedExtraNodesSet;
class ConstrainedGeneralizedWeldNode;
class ConstrainedGlobal;
class ConstrainedInterpolation;
class ConstrainedJointCylindrical;
class ConstrainedJointPlanar;
class ConstrainedJointRevolute;
class ConstrainedJointSpherical;
class ConstrainedJointStiffness;
class ConstrainedJointTranslational;
class ConstrainedJointUniversal;
class ConstrainedLagrangeInSolid;
class ConstrainedLinear;
class ConstrainedNodalRigidBody;
class ConstrainedNodeSet;
class ConstrainedRigidBodies;
class ConstrainedRigidBodyStoppers;
class ConstrainedShellToSolid;
class ConstrainedSpotweld;
class ConstrainedTieBreak;
class Contact2dAutomaticSingleSurface;
class Contact2DAutomaticSurfaceToSurface;
class Contact2DNodesToSurface;
class ContactAddWear;
class ContactAirbagSingleSurface;
class ContactAutomaticBeamsToSurface;
class ContactAutomaticGeneral;
class ContactAutomaticNodesToSurface;
class ContactAutomaticNodesToSurfaceId;
class ContactAutomaticSingleSurface;
class ContactAutomaticSingleSurfaceId;
class ContactAutomaticSingleSurfaceMortar;
class ContactAutomaticSurfaceToSurface;
class ContactAutomaticSurfaceToSurfaceId;
class ContactAutomaticSurfaceToSurfaceMortar;
class ContactAutomaticSurfaceToSurfaceTiebreak;
class ContactDrawbead;
class ContactEntity;
class ContactErodingNodesToSurface;
class ContactErodingSingleSurface;
class ContactErodingSurfaceToSurface;
class ContactForceTransducerPenalty;
class ContactFormingOneWaySurfaceToSurface;
class ContactFormingSurfaceToSurface;
class ContactGebodSegment;
class ContactGuidedCable;
class ContactInterior;
class ContactNodesToSurface;
class ContactOption;
class ContactRigidBodyOneWay;
class ContactRigidBodyTwoWay;
class ContactSingleSurface;
class ContactSlidingOnlyPenalty;
class ContactSpotweld;
class ContactSurfaceToSurface;
class ContactTiebreakNodesToSurface;
class ContactTiebreakSurfaceToSurface;
class ContactTiedNodesToSurface;
class ContactTiedShellEdgeToSolid;
class ContactTiedShellEdgeToSurface;
class ContactTiedSurfaceToSurface;
class ControlAccuracy;
class ControlAdaptive;
class ControlAle;
class ControlBeam;
class ControlBulkViscosity;
class ControlContact;
class ControlCpu;
class ControlDynamicRelaxation;
class ControlEnergy;
class ControlFormingBestfit;
class ControlFormingOnestep;
class ControlHourglass;
class ControlImplicitAuto;
class ControlImplicitBuckle;
class ControlImplicitDynamics;
class ControlImplicitEigenvalue;
class ControlImplicitForming;
class ControlImplicitGeneral;
class ControlImplicitModalDynamic;
class ControlImplicitSolution;
class ControlImplicitSolver;
class ControlImplicitStabilization;
class ControlMppDecompositionDistributeAleElements;
class ControlOutput;
class ControlParallel;
class ControlRemeshing;
class ControlRigid;
class ControlShell;
class ControlSolid;
class ControlSph;
class ControlSpotweldBeam;
class ControlSubcycle;
class ControlTermination;
class ControlThermalNonlinear;
class ControlThermalSolver;
class ControlThermalTimestep;
class ControlTimestep;
class DampingFrequencyRange;
class DampingFrequencyRangeDeform;
class DampingFrequencyRangeRigid;
class DampingGlobal;
class DampingModal;
class DampingPartMass;
class DampingPartMassSet;
class DampingPartStiffness;
class DampingPartStiffnessSet;
class DampingRelative;
class DampingStructural;
class DatabaseAbstat;
class DatabaseAbstatMass;
class DatabaseAleMat;
class DatabaseAtdout;
class DatabaseBearing;
class DatabaseBinaryD3dump;
class DatabaseBinaryD3plot;
class DatabaseBinaryD3thdt;
class DatabaseBinaryRunrsf;
class DatabaseBndout;
class DatabaseBndoutVent;
class DatabaseCpmfor;
class DatabaseCpmSensor;
class DatabaseCrossSectionPlane;
class DatabaseCrossSectionSet;
class DatabaseCurvout;
class DatabaseCurvoutExtend;
class DatabaseDcfail;
class DatabaseDefgeo;
class DatabaseDeforc;
class DatabaseDemassflow;
class DatabaseDemrcf;
class DatabaseDisbout;
class DatabaseElout;
class DatabaseExtentBinary;
class DatabaseFormat;
class DatabaseFsi;
class DatabaseFsiSensor;
class DatabaseGlstat;
class DatabaseHistoryBeam;
class DatabaseHistoryNode;
class DatabaseHistoryShell;
class DatabaseHistorySolid;
class DatabaseHistoryTshell;
class DatabaseJntforc;
class DatabaseJntforcLocal;
class DatabaseMassout;
class DatabaseMatsum;
class DatabaseMovie;
class DatabaseNcforc;
class DatabaseNcforcFilter;
class DatabaseNodfor;
class DatabaseNodout;
class DatabasePllyout;
class DatabasePllyoutRetractor;
class DatabasePrtube;
class DatabasePsd;
class DatabasePwpOutput;
class DatabaseRbdout;
class DatabaseRcforc;
class DatabaseRve;
class DatabaseRwforc;
class DatabaseSbtout;
class DatabaseSbtoutRetractor;
class DatabaseSbtoutSensor;
class DatabaseSecforc;
class DatabaseSleout;
class DatabaseSpcforc;
class DatabaseSphFlow;
class DatabaseSphFlowSensor;
class DatabaseSsstat;
class DatabaseSwforc;
class DatabaseSwforcFilter;
class DatabaseTotgeo;
class DatabaseTprint;
class DatabaseTracer;
class DefineAdaptiveSolidToSph;
class DefineBirthTimes;
class DefineBox;
class DefineCarpetPlot;
class DefineConnectionProperties;
class DefineContactVolume;
class DefineCoordinateNodes;
class DefineCoordinateSystem;
class DefineCoordinateVector;
class DefineCpmVent;
class DefineCurve;
class DefineCurveCompensated;
class DefineCurveDuplicate;
class DefineCurveEntity;
class DefineCurveFunction;
class DefineCurveSmooth;
class DefineCurveTitle;
class DefineCurveTrim2d;
class DefineCurveTrim3d;
class DefineDeathTimes;
class DefineDeMeshBeam;
class DefineDeMeshSurface;
class DefineElementDeath;
class DefineFilter;
class DefineFormingBlankmesh;
class DefineFriction;
class DefineFunction;
class DefineFunctionTabulated;
class DefineGroundMotion;
class DefineHazProperties;
class DefineHexSpotweldAssembly;
class DefineLoadBlastSegment;
class DefineMaterialHistories;
class DefinePblastAirburst;
class DefinePblastSurface;
class DefinePorousChemistry;
class DefinePressureTube;
class DefineRegion;
class DefineSdOrientation;
class DefineSphAmbientDrag;
class DefineSphInjection;
class DefineSphMeshBox;
class DefineSphToSphCoupling;
class DefineSpotweldMultiscale;
class DefineSpotweldRuptureParameter;
class DefineStochasticVariation;
class DefineTable;
class DefineTable2D;
class DefineTable3D;
class DefineTransformation;
class DefineTrimSeed;
class DefineVector;
class DeformableToRigid;
class DeformableToRigidAutomatic;
class DeformableToRigidAutomaticId;
class DeformableToRigidInertia;
class DeformableToRigidTitle;
class DualceseBoundaryPrescribedPre;
class DualceseBoundaryPrescribedVel;
class DualceseBoundaryReflective;
class DualceseControlGeneral;
class DualceseControlSolver;
class DualceseControlTimestep;
class DualceseInitial;
class DualceseMat;
class DualcesePart;
class ElementBeam;
class ElementBeamOrientation;
class ElementBeamPulley;
class ElementBearing;
class ElementDirectMatrixInput;
class ElementDiscrete;
class ElementGeneralizedShell;
class ElementInertia;
class ElementInterpolationShell;
class ElementLancing;
class ElementMass;
class ElementMassNodeSet;
class ElementMassPartSet;
class ElementPlotel;
class ElementSeatbelt;
class ElementSeatbeltAccelerometer;
class ElementSeatbeltPretensioner;
class ElementSeatbeltRetractor;
class ElementSeatbeltSensor;
class ElementSeatbeltSlipring;
class ElementShell;
class ElementShellComposite;
class ElementShellSourceSink;
class ElementShellThickness;
class ElementSolid;
class ElementSolidOrtho;
class ElementSph;
class ElementTrim;
class ElementTshell;
class EmBoundary;
class EmCircuit;
class EmContact;
class EmControl;
class EmControlCoupling;
class EmIsopotential;
class EmMat001;
class EmMat002;
class EmOutput;
class EmRandlesBatmac;
class EmSolverFem;
class EosGasket;
class EosGruneisen;
class EosIdealGas;
class EosIgnitionGrowth;
class EosJwl;
class EosJwlb;
class EosLinearPolynomial;
class EosLinearPolynomialWithEnergyLeak;
class EosMurnaghan;
class EosOsborne;
class EosPowderBurn;
class EosPropellantDeflagration;
class EosRatioOfPolynomials;
class EosSack;
class EosSesame;
class EosStiffGas;
class EosTabulated;
class EosTabulatedCompaction;
class EosTillotson;
class EosUserDefined;
class FrequencyDomainAcousticBem;
class FrequencyDomainAcousticFem;
class FrequencyDomainFrf;
class FrequencyDomainMode;
class FrequencyDomainPath;
class FrequencyDomainRandomVibration;
class FrequencyDomainResponseSpectrum;
class FrequencyDomainSsd;
class FrequencyDomainSsdDirect;
class FrequencyDomainVibrationFatigue;
class GenericKeyword;
class Hourglass;
class HourglassBeam;
class HourglassId;
class HourglassPart;
class HourglassShell;
class HourglassSolid;
class HourglassThicknessChange;
class HourglassTitle;
class IcfdBoundaryPrescribedPre;
class IcfdBoundaryPrescribedVel;
class IcfdControlFsi;
class IcfdControlGeneral;
class IcfdControlMesh;
class IcfdControlOutput;
class IcfdControlTime;
class IcfdControlTurbulence;
class IcfdDefineMesh;
class IcfdInitial;
class IcfdMat;
class IcfdPart;
class Include;
class IncludeAutoOffset;
class IncludeBinary;
class IncludeCompensate;
class IncludePath;
class IncludePathRelative;
class IncludeStampedPart;
class IncludeTransform;
class InitialAirbagParticlePosition;
class InitialAleMapping;
class InitialAlePressure;
class InitialAngularMomentum;
class InitialAxialForceBeam;
class InitialContactWear;
class InitialDetonation;
class InitialFoamReferenceGeometry;
class InitialGasMixture;
class InitialHistoryNode;
class InitialHistoryShell;
class InitialHistorySolid;
class InitialImpulseMine;
class InitialInternalDofSolid;
class InitialMomentum;
class InitialPwpDepth;
class InitialRotationalVelocity;
class InitialSpcRotationAngle;
class InitialSphMassFraction;
class InitialStrainShell;
class InitialStrainSolid;
class InitialStressBeam;
class InitialStressDepth;
class InitialStressDiscrete;
class InitialStressSection;
class InitialStressShell;
class InitialStressSolid;
class InitialStressTshell;
class InitialTemperature;
class InitialTemperatureSet;
class InitialVelocity;
class InitialVelocityGeneration;
class InitialVelocityNode;
class InitialVelocitySet;
class InitialVolumeFraction;
class InitialVolumeFractionGeometry;
class IntegrationBeam;
class IntegrationPoint;
class IntegrationPointTransform;
class IntegrationShell;
class IntegrationShellThickness;
class IntegrationSolid;
class IntegrationTshell;
class InterfaceBlanksize;
class InterfaceCompensationNew;
class InterfaceComponent;
class InterfaceFrictionalSlideline;
class InterfaceLinking;
class InterfaceLinkingDiscrete;
class InterfaceSpringbackLsdyna;
class InterfaceSpringbackSeamless;
class InterfaceSsi;
class LoadAcousticSource;
class LoadAirbagPressure;
class LoadAirmix;
class LoadAleConvection;
class LoadBeam;
class LoadBeamSet;
class LoadBlast;
class LoadBlastEnhanced;
class LoadBlastSegmentSet;
class LoadBodyGeneralized;
class LoadBodyPart;
class LoadBodyParts;
class LoadBodyPartSet;
class LoadBodyPorous;
class LoadBodyRx;
class LoadBodyRy;
class LoadBodyRz;
class LoadBodyVector;
class LoadBodyX;
class LoadBodyY;
class LoadBodyZ;
class LoadBrode;
class LoadConvection;
class LoadDensity;
class LoadDensityDepth;
class LoadErodingPartSet;
class LoadExpansionPressure;
class LoadFluidPressure;
class LoadGravity;
class LoadGravityPart;
class LoadHeatController;
class LoadHeatExothermicReaction;
class LoadHeatFlux;
class LoadHeatGeneration;
class LoadHeatGenerationSet;
class LoadInteriorPressure;
class LoadLanczos;
class LoadMask;
class LoadMotionNode;
class LoadMotionNodeSet;
class LoadMovingPressure;
class LoadNegativeVolume;
class LoadNode;
class LoadNodePoint;
class LoadNodeSet;
class LoadPendulum;
class LoadPressurePenetration;
class LoadPze;
class LoadRadiation;
class LoadRail;
class LoadRemovePart;
class LoadRigidBody;
class LoadRigidBodyInertia;
class LoadSegment;
class LoadSegmentData;
class LoadSegmentFile;
class LoadSegmentId;
class LoadSegmentNonuniform;
class LoadSegmentPressure;
class LoadSegmentSet;
class LoadSegmentSetAngle;
class LoadSeismic;
class LoadSeismicSsi;
class LoadSeismicSsiAux;
class LoadShellElement;
class LoadShellSet;
class LoadSoftElement;
class LoadSpc;
class LoadSpcForce;
class LoadSpcSet;
class LoadSsa;
class LoadSsaGravity;
class LoadSteadyStateRolling;
class LoadStiffenPart;
class LoadSuperplasticForming;
class LoadSurfaceStress;
class LoadSurfaceStressSegment;
class LoadThermalBinout;
class LoadThermalBody;
class LoadThermalConstant;
class LoadThermalConstantNode;
class LoadThermalD3plot;
class LoadThermalElement;
class LoadThermalLoadCurve;
class LoadThermalRsw;
class LoadThermalTopaz;
class LoadThermalTopaz3d;
class LoadThermalVariable;
class LoadThermalVariableBeam;
class LoadThermalVariableElement;
class LoadThermalVariableNode;
class LoadThermalVariableShell;
class LoadThermalVariableSolid;
class LoadThermalVariableTshell;
class LoadTrackTurn;
class LoadTyrePress;
class LoadVolumeLoss;
class LoadWave;
class LoadWheelPatch;
class Mat3ParameterBarlat;
class MatAcoustic;
class MatAddCohesive;
class MatAddDamageGissmo;
class MatAddErosion;
class MatAddFatigue;
class MatAddInelasticity;
class MatAddPermeability;
class MatAddPoreAir;
class MatAddThermalExpansion;
class MatAnisotropicViscoplastic;
class MatArrudaBoyce;
class MatArupAdhesive;
class MatBamman;
class MatBammanDamage;
class MatBarlatAnisotropicPlasticity;
class MatBilkhuDuboisFoam;
class MatBlatzKoFoam;
class MatBlatzKoRubber;
class MatCableDiscreteBeam;
class MatClosedCellFoam;
class MatCohesiveElastic;
class MatCohesiveGeneral;
class MatCompositeDamage;
class MatCompositeFailureSolidModel;
class MatConcreteDamageRel3;
class MatCrushableFoam;
class MatCscmConcrete;
class MatDamage3;
class MatDamperNonlinearViscous;
class MatDamperViscous;
class MatElastic;
class MatElasticPlasticHydro;
class MatElasticPlasticThermal;
class MatElasticViscoplasticThermal;
class MatElasticWithViscosity;
class MatEnhancedCompositeDamage;
class MatFabric;
class MatFldTransverselyAnisotropic;
class MatForceLimited;
class MatFrazerNashRubber;
class MatFuChangFoam;
class MatGeneralViscoelastic;
class MatGeologicCapModel;
class MatHighExplosiveBurn;
class MatHoneycomb;
class MatHystereticBeam;
class MatIsotropicElasticFailure;
class MatIsotropicElasticPlastic;
class MatJohnsonCook;
class MatKelvinMaxwellViscoelastic;
class MatLaminatedCompositeFabric;
class MatLaminatedGlass;
class MatLowDensityFoam;
class MatModifiedPiecewiseLinearPlasticity;
class MatMooneyRivlinRubber;
class MatNonlinearOrthotropic;
class MatNull;
class MatOgdenRubber;
class MatOrientedCrack;
class MatOrthoElasticPlastic;
class MatOrthotropicElastic;
class MatPiecewiseLinearPlasticity;
class MatPiecewiseLinearPlasticityStochastic;
class MatPlasticGreenNaghdi;
class MatPlasticityWithDamage;
class MatPlasticityWithDamageOrtho;
class MatPlasticKinematic;
class MatPowerLawPlasticity;
class MatPseudoTensor;
class MatRateSensitiveCompositeFabric;
class MatResultantPlasticity;
class MatRigid;
class MatSamp1;
class MatSeismicIsolator;
class MatShapeMemory;
class MatSimplifiedJohnsonCook;
class MatSimplifiedRubber;
class MatSoftTissue;
class MatSoilAndFoam;
class MatSoilAndFoamFailure;
class MatSpotWeld;
class MatSpringElastic;
class MatSpringElastoplastic;
class MatSpringGeneralNonlinear;
class MatSpringInelastic;
class MatSpringMaxwell;
class MatSpringNonlinearElastic;
class MatSteinberg;
class MatStrainRateDependentPlasticity;
class MatTabulatedJohnsonCook;
class MatTempDependentOrthotropic;
class MatThermalIsotropic;
class MatThermalOrthotropic;
class MatTransverselyAnisotropicElasticPlastic;
class MatUserDefined;
class MatViscoelastic;
class MatViscoelasticThermal;
class MatViscousFoam;
class MatWinfrithConcrete;
class Node;
class NodeMerge;
class NodeMergeSet;
class NodeMergeTolerance;
class NodeReference;
class NodeRigidBody;
class NodeRigidSurface;
class NodeScalar;
class NodeScalarValue;
class NodeSpotWeld;
class NodeThickness;
class NodeThicknessSet;
class NodeThicknessSetGenerate;
class NodeToTarget;
class NodeToTargetVector;
class NodeTransform;
class Parameter;
class ParameterDuplication;
class ParameterExpression;
class ParameterExpressionLocal;
class ParameterGlobal;
class ParameterLocal;
class ParameterType;
class Part;
class PartAdaptiveFailure;
class PartAnneal;
class PartAveraged;
class PartComposite;
class PartCompositeTshell;
class PartContact;
class PartDuplicate;
class ParticleBlast;
class PartInertia;
class PartModes;
class PartMove;
class PartsDistribute;
class PartSensor;
class PartsetDistribute;
class PartStackedElements;
class PartStiffness;
class PerturbationContact;
class PerturbationElement;
class PerturbationGeometry;
class PerturbationLoad;
class PerturbationMat;
class PerturbationNode;
class PerturbationPart;
class PerturbationSection;
class PerturbationThickness;
class RigidToDeformable;
class RigidwallGeometricCone;
class RigidwallGeometricCurved;
class RigidwallGeometricCylinder;
class RigidwallGeometricFlat;
class RigidwallGeometricMotion;
class RigidwallGeometricPrism;
class RigidwallGeometricSphere;
class RigidwallGeometricTorus;
class RigidwallPlanar;
class RigidwallPlanarFinite;
class RigidwallPlanarFiniteId;
class RigidwallPlanarForces;
class RigidwallPlanarId;
class RigidwallPlanarMoving;
class RigidwallPlanarMovingFinite;
class RigidwallPlanarMovingForces;
class RigidwallPlanarOrtho;
class SectionAle1d;
class SectionAle2d;
class SectionBeam;
class SectionBeamTitle;
class SectionDiscrete;
class SectionDiscreteTitle;
class SectionPointSource;
class SectionPointSourceMixture;
class SectionSeatbelt;
class SectionShell;
class SectionShellAle;
class SectionShellEfg;
class SectionShellTitle;
class SectionSolid;
class SectionSolidAle;
class SectionSolidEfg;
class SectionSolidTitle;
class SectionSph;
class SectionTshell;
class SectionTshellTitle;
class SensorControl;
class SensorDefineCalcMath;
class SensorDefineCoordinate;
class SensorDefineElement;
class SensorDefineForce;
class SensorDefineFunction;
class SensorDefineMisc;
class SensorDefineNode;
class SensorSwitch;
class SensorSwitchCalcLogic;
class SensorSwitchShellToSolid;
class Set2dShell;
class SetBeam;
class SetBeamAdd;
class SetBeamGeneral;
class SetBeamGenerate;
class SetBeamGenerateTitle;
class SetBeamIntersect;
class SetBeamList;
class SetBeamListTitle;
class SetBox;
class SetDiscrete;
class SetDiscreteAdd;
class SetDiscreteGeneral;
class SetDiscreteList;
class SetDiscreteListTitle;
class SetIgaEdge;
class SetIgaEdgeUvw;
class SetIgaFace;
class SetIgaFaceUvw;
class SetIgaPointUvw;
class SetMode;
class SetMulti;
class SetMultiMaterialGroupList;
class SetNode;
class SetNodeAdd;
class SetNodeColumn;
class SetNodeGeneral;
class SetNodeGenerate;
class SetNodeGenerateTitle;
class SetNodeIntersect;
class SetNodeList;
class SetNodeListTitle;
class SetNodeTitle;
class SetPart;
class SetPartAdd;
class SetPartColumn;
class SetPartGeneral;
class SetPartGenerate;
class SetPartGenerateTitle;
class SetPartIntersect;
class SetPartList;
class SetPartListGenerate;
class SetPartListTitle;
class SetPartTitle;
class SetPartTree;
class SetSeatbelt;
class SetSegment;
class SetSegmentAdd;
class SetSegmentGeneral;
class SetSegmentIntersect;
class SetSegmentTitle;
class SetShell;
class SetShellAdd;
class SetShellGeneral;
class SetShellGenerate;
class SetShellGenerateTitle;
class SetShellIntersect;
class SetShellList;
class SetShellListTitle;
class SetSolid;
class SetSolidAdd;
class SetSolidGeneral;
class SetSolidGenerate;
class SetSolidGenerateTitle;
class SetSolidIntersect;
class SetSolidList;
class SetSolidListTitle;
class SetTshell;
class SetTshellGeneral;
class SetTshellList;
class StochasticCorrelatedField;
class StochasticMaterialProperty;
class StochasticMonteCarloRun;
class StochasticOutput;
class StochasticPart;
class StochasticRandomization;
class StochasticSample;
class StochasticStructureField;
class StochasticVariable;

/**
 * @brief Visitor pattern interface for traversing Model keywords
 * 
 * Implement this interface to perform operations on all keywords in a model.
 * All visit() methods have empty default implementations, so you only need 
 * to override the methods for keyword types you're interested in.
 * 
 * Example usage:
 * @code
 * class StatisticsVisitor : public ModelVisitor {
 *     size_t nodeCount = 0;
 *     size_t elementCount = 0;
 * 
 *     void visit(Node& node) override {
 *         nodeCount += node.nodes.size();
 *     }
 * 
 *     void visit(ElementShell& elem) override {
 *         elementCount += elem.elements.size();
 *     }
 * };
 * 
 * StatisticsVisitor stats;
 * model->accept(stats);
 * std::cout << "Nodes: " << stats.nodeCount << "\n";
 * @endcode
 */
class KOO_API ModelVisitor {
public:
    virtual ~ModelVisitor() = default;

    // Visit methods for all keyword types (empty default implementations)
    virtual void visit(AirbagAdiabaticGasModel& keyword) {}
    virtual void visit(AirbagHybrid& keyword) {}
    virtual void visit(AirbagInteraction& keyword) {}
    virtual void visit(AirbagLinearFluid& keyword) {}
    virtual void visit(AirbagLoadCurve& keyword) {}
    virtual void visit(AirbagParticle& keyword) {}
    virtual void visit(AirbagReferenceGeometry& keyword) {}
    virtual void visit(AirbagSimpleAirbagModel& keyword) {}
    virtual void visit(AirbagSimplePressureVolume& keyword) {}
    virtual void visit(AirbagWangNefske& keyword) {}
    virtual void visit(AleAmbientHydrostatic& keyword) {}
    virtual void visit(AleCouplingNodalConstraint& keyword) {}
    virtual void visit(AleEssentialBoundary& keyword) {}
    virtual void visit(AleFailSwitchMmg& keyword) {}
    virtual void visit(AleFsiSwitchMmg& keyword) {}
    virtual void visit(AleMultiMaterialGroup& keyword) {}
    virtual void visit(AleReferenceSystemGroup& keyword) {}
    virtual void visit(AleReferenceSystemNode& keyword) {}
    virtual void visit(AleSmoothing& keyword) {}
    virtual void visit(AleStructuredMesh& keyword) {}
    virtual void visit(AleStructuredMeshControlPoints& keyword) {}
    virtual void visit(BoundaryAcousticCoupling& keyword) {}
    virtual void visit(BoundaryAcousticFreeSurface& keyword) {}
    virtual void visit(BoundaryAcousticImpedance& keyword) {}
    virtual void visit(BoundaryAcousticMapping& keyword) {}
    virtual void visit(BoundaryAcousticNonReflecting& keyword) {}
    virtual void visit(BoundaryAleMapping& keyword) {}
    virtual void visit(BoundaryAmbient& keyword) {}
    virtual void visit(BoundaryAmbientEos& keyword) {}
    virtual void visit(BoundaryConvection& keyword) {}
    virtual void visit(BoundaryConvectionSet& keyword) {}
    virtual void visit(BoundaryCoupled& keyword) {}
    virtual void visit(BoundaryCyclic& keyword) {}
    virtual void visit(BoundaryDeNonReflecting& keyword) {}
    virtual void visit(BoundaryElementMethod& keyword) {}
    virtual void visit(BoundaryElementMethodControl& keyword) {}
    virtual void visit(BoundaryFluidmFreeSurface& keyword) {}
    virtual void visit(BoundaryFlux& keyword) {}
    virtual void visit(BoundaryFluxSet& keyword) {}
    virtual void visit(BoundaryFluxTrajectory& keyword) {}
    virtual void visit(BoundaryMcol& keyword) {}
    virtual void visit(BoundaryNonReflecting& keyword) {}
    virtual void visit(BoundaryNonReflecting2D& keyword) {}
    virtual void visit(BoundaryPap& keyword) {}
    virtual void visit(BoundaryPoreFluid& keyword) {}
    virtual void visit(BoundaryPrecrack& keyword) {}
    virtual void visit(BoundaryPrescribedAccelerometer& keyword) {}
    virtual void visit(BoundaryPrescribedAccelerometerRigid& keyword) {}
    virtual void visit(BoundaryPrescribedFinalGeometry& keyword) {}
    virtual void visit(BoundaryPrescribedMotionNode& keyword) {}
    virtual void visit(BoundaryPrescribedMotionNodeId& keyword) {}
    virtual void visit(BoundaryPrescribedMotionRigid& keyword) {}
    virtual void visit(BoundaryPrescribedMotionRigidLocal& keyword) {}
    virtual void visit(BoundaryPrescribedMotionSet& keyword) {}
    virtual void visit(BoundaryPrescribedMotionSetBox& keyword) {}
    virtual void visit(BoundaryPrescribedMotionSetLine& keyword) {}
    virtual void visit(BoundaryPrescribedOrientationRigid& keyword) {}
    virtual void visit(BoundaryPressureOutflow& keyword) {}
    virtual void visit(BoundaryPwp& keyword) {}
    virtual void visit(BoundaryPwpNode& keyword) {}
    virtual void visit(BoundaryPzepot& keyword) {}
    virtual void visit(BoundaryRadiation& keyword) {}
    virtual void visit(BoundaryRadiationSegment& keyword) {}
    virtual void visit(BoundaryRadiationSet& keyword) {}
    virtual void visit(BoundarySaleMeshFace& keyword) {}
    virtual void visit(BoundarySlidingPlane& keyword) {}
    virtual void visit(BoundarySpc& keyword) {}
    virtual void visit(BoundarySpcNode& keyword) {}
    virtual void visit(BoundarySpcSet& keyword) {}
    virtual void visit(BoundarySpcSetBirthDeath& keyword) {}
    virtual void visit(BoundarySpcSymmetryPlane& keyword) {}
    virtual void visit(BoundarySphFlow& keyword) {}
    virtual void visit(BoundarySphNonReflecting& keyword) {}
    virtual void visit(BoundarySphNoslip& keyword) {}
    virtual void visit(BoundarySphPeriodic& keyword) {}
    virtual void visit(BoundarySphSymmetryPlane& keyword) {}
    virtual void visit(BoundarySymmetryFailure& keyword) {}
    virtual void visit(BoundaryTemperature& keyword) {}
    virtual void visit(BoundaryTemperatureRsw& keyword) {}
    virtual void visit(BoundaryTemperatureTrajectory& keyword) {}
    virtual void visit(BoundaryThermalBulknode& keyword) {}
    virtual void visit(BoundaryThermalNode& keyword) {}
    virtual void visit(BoundaryThermalSet& keyword) {}
    virtual void visit(BoundaryThermalWeld& keyword) {}
    virtual void visit(BoundaryThermalWeldTrajectory& keyword) {}
    virtual void visit(BoundaryUsaSurface& keyword) {}
    virtual void visit(CeseBoundaryNonReflective& keyword) {}
    virtual void visit(CeseBoundaryPrescribed& keyword) {}
    virtual void visit(CeseBoundaryReflective& keyword) {}
    virtual void visit(CeseControlMesh& keyword) {}
    virtual void visit(CeseControlOutput& keyword) {}
    virtual void visit(CeseControlSolver& keyword) {}
    virtual void visit(CeseControlTimestep& keyword) {}
    virtual void visit(CeseInitial& keyword) {}
    virtual void visit(CeseMat& keyword) {}
    virtual void visit(CesePart& keyword) {}
    virtual void visit(ChemistryComposition& keyword) {}
    virtual void visit(ChemistryControl& keyword) {}
    virtual void visit(ChemistryControlInflator& keyword) {}
    virtual void visit(ChemistryControlZnd& keyword) {}
    virtual void visit(ChemistryDetonation& keyword) {}
    virtual void visit(ChemistryInflatorProperties& keyword) {}
    virtual void visit(ChemistryModel& keyword) {}
    virtual void visit(ChemistryReaction& keyword) {}
    virtual void visit(ChemistrySpecies& keyword) {}
    virtual void visit(ConstrainedBeamInSolid& keyword) {}
    virtual void visit(ConstrainedExtraNodesNode& keyword) {}
    virtual void visit(ConstrainedExtraNodesSet& keyword) {}
    virtual void visit(ConstrainedGeneralizedWeldNode& keyword) {}
    virtual void visit(ConstrainedGlobal& keyword) {}
    virtual void visit(ConstrainedInterpolation& keyword) {}
    virtual void visit(ConstrainedJointCylindrical& keyword) {}
    virtual void visit(ConstrainedJointPlanar& keyword) {}
    virtual void visit(ConstrainedJointRevolute& keyword) {}
    virtual void visit(ConstrainedJointSpherical& keyword) {}
    virtual void visit(ConstrainedJointStiffness& keyword) {}
    virtual void visit(ConstrainedJointTranslational& keyword) {}
    virtual void visit(ConstrainedJointUniversal& keyword) {}
    virtual void visit(ConstrainedLagrangeInSolid& keyword) {}
    virtual void visit(ConstrainedLinear& keyword) {}
    virtual void visit(ConstrainedNodalRigidBody& keyword) {}
    virtual void visit(ConstrainedNodeSet& keyword) {}
    virtual void visit(ConstrainedRigidBodies& keyword) {}
    virtual void visit(ConstrainedRigidBodyStoppers& keyword) {}
    virtual void visit(ConstrainedShellToSolid& keyword) {}
    virtual void visit(ConstrainedSpotweld& keyword) {}
    virtual void visit(ConstrainedTieBreak& keyword) {}
    virtual void visit(Contact2dAutomaticSingleSurface& keyword) {}
    virtual void visit(Contact2DAutomaticSurfaceToSurface& keyword) {}
    virtual void visit(Contact2DNodesToSurface& keyword) {}
    virtual void visit(ContactAddWear& keyword) {}
    virtual void visit(ContactAirbagSingleSurface& keyword) {}
    virtual void visit(ContactAutomaticBeamsToSurface& keyword) {}
    virtual void visit(ContactAutomaticGeneral& keyword) {}
    virtual void visit(ContactAutomaticNodesToSurface& keyword) {}
    virtual void visit(ContactAutomaticNodesToSurfaceId& keyword) {}
    virtual void visit(ContactAutomaticSingleSurface& keyword) {}
    virtual void visit(ContactAutomaticSingleSurfaceId& keyword) {}
    virtual void visit(ContactAutomaticSingleSurfaceMortar& keyword) {}
    virtual void visit(ContactAutomaticSurfaceToSurface& keyword) {}
    virtual void visit(ContactAutomaticSurfaceToSurfaceId& keyword) {}
    virtual void visit(ContactAutomaticSurfaceToSurfaceMortar& keyword) {}
    virtual void visit(ContactAutomaticSurfaceToSurfaceTiebreak& keyword) {}
    virtual void visit(ContactDrawbead& keyword) {}
    virtual void visit(ContactEntity& keyword) {}
    virtual void visit(ContactErodingNodesToSurface& keyword) {}
    virtual void visit(ContactErodingSingleSurface& keyword) {}
    virtual void visit(ContactErodingSurfaceToSurface& keyword) {}
    virtual void visit(ContactForceTransducerPenalty& keyword) {}
    virtual void visit(ContactFormingOneWaySurfaceToSurface& keyword) {}
    virtual void visit(ContactFormingSurfaceToSurface& keyword) {}
    virtual void visit(ContactGebodSegment& keyword) {}
    virtual void visit(ContactGuidedCable& keyword) {}
    virtual void visit(ContactInterior& keyword) {}
    virtual void visit(ContactNodesToSurface& keyword) {}
    virtual void visit(ContactOption& keyword) {}
    virtual void visit(ContactRigidBodyOneWay& keyword) {}
    virtual void visit(ContactRigidBodyTwoWay& keyword) {}
    virtual void visit(ContactSingleSurface& keyword) {}
    virtual void visit(ContactSlidingOnlyPenalty& keyword) {}
    virtual void visit(ContactSpotweld& keyword) {}
    virtual void visit(ContactSurfaceToSurface& keyword) {}
    virtual void visit(ContactTiebreakNodesToSurface& keyword) {}
    virtual void visit(ContactTiebreakSurfaceToSurface& keyword) {}
    virtual void visit(ContactTiedNodesToSurface& keyword) {}
    virtual void visit(ContactTiedShellEdgeToSolid& keyword) {}
    virtual void visit(ContactTiedShellEdgeToSurface& keyword) {}
    virtual void visit(ContactTiedSurfaceToSurface& keyword) {}
    virtual void visit(ControlAccuracy& keyword) {}
    virtual void visit(ControlAdaptive& keyword) {}
    virtual void visit(ControlAle& keyword) {}
    virtual void visit(ControlBeam& keyword) {}
    virtual void visit(ControlBulkViscosity& keyword) {}
    virtual void visit(ControlContact& keyword) {}
    virtual void visit(ControlCpu& keyword) {}
    virtual void visit(ControlDynamicRelaxation& keyword) {}
    virtual void visit(ControlEnergy& keyword) {}
    virtual void visit(ControlFormingBestfit& keyword) {}
    virtual void visit(ControlFormingOnestep& keyword) {}
    virtual void visit(ControlHourglass& keyword) {}
    virtual void visit(ControlImplicitAuto& keyword) {}
    virtual void visit(ControlImplicitBuckle& keyword) {}
    virtual void visit(ControlImplicitDynamics& keyword) {}
    virtual void visit(ControlImplicitEigenvalue& keyword) {}
    virtual void visit(ControlImplicitForming& keyword) {}
    virtual void visit(ControlImplicitGeneral& keyword) {}
    virtual void visit(ControlImplicitModalDynamic& keyword) {}
    virtual void visit(ControlImplicitSolution& keyword) {}
    virtual void visit(ControlImplicitSolver& keyword) {}
    virtual void visit(ControlImplicitStabilization& keyword) {}
    virtual void visit(ControlMppDecompositionDistributeAleElements& keyword) {}
    virtual void visit(ControlOutput& keyword) {}
    virtual void visit(ControlParallel& keyword) {}
    virtual void visit(ControlRemeshing& keyword) {}
    virtual void visit(ControlRigid& keyword) {}
    virtual void visit(ControlShell& keyword) {}
    virtual void visit(ControlSolid& keyword) {}
    virtual void visit(ControlSph& keyword) {}
    virtual void visit(ControlSpotweldBeam& keyword) {}
    virtual void visit(ControlSubcycle& keyword) {}
    virtual void visit(ControlTermination& keyword) {}
    virtual void visit(ControlThermalNonlinear& keyword) {}
    virtual void visit(ControlThermalSolver& keyword) {}
    virtual void visit(ControlThermalTimestep& keyword) {}
    virtual void visit(ControlTimestep& keyword) {}
    virtual void visit(DampingFrequencyRange& keyword) {}
    virtual void visit(DampingFrequencyRangeDeform& keyword) {}
    virtual void visit(DampingFrequencyRangeRigid& keyword) {}
    virtual void visit(DampingGlobal& keyword) {}
    virtual void visit(DampingModal& keyword) {}
    virtual void visit(DampingPartMass& keyword) {}
    virtual void visit(DampingPartMassSet& keyword) {}
    virtual void visit(DampingPartStiffness& keyword) {}
    virtual void visit(DampingPartStiffnessSet& keyword) {}
    virtual void visit(DampingRelative& keyword) {}
    virtual void visit(DampingStructural& keyword) {}
    virtual void visit(DatabaseAbstat& keyword) {}
    virtual void visit(DatabaseAbstatMass& keyword) {}
    virtual void visit(DatabaseAleMat& keyword) {}
    virtual void visit(DatabaseAtdout& keyword) {}
    virtual void visit(DatabaseBearing& keyword) {}
    virtual void visit(DatabaseBinaryD3dump& keyword) {}
    virtual void visit(DatabaseBinaryD3plot& keyword) {}
    virtual void visit(DatabaseBinaryD3thdt& keyword) {}
    virtual void visit(DatabaseBinaryRunrsf& keyword) {}
    virtual void visit(DatabaseBndout& keyword) {}
    virtual void visit(DatabaseBndoutVent& keyword) {}
    virtual void visit(DatabaseCpmfor& keyword) {}
    virtual void visit(DatabaseCpmSensor& keyword) {}
    virtual void visit(DatabaseCrossSectionPlane& keyword) {}
    virtual void visit(DatabaseCrossSectionSet& keyword) {}
    virtual void visit(DatabaseCurvout& keyword) {}
    virtual void visit(DatabaseCurvoutExtend& keyword) {}
    virtual void visit(DatabaseDcfail& keyword) {}
    virtual void visit(DatabaseDefgeo& keyword) {}
    virtual void visit(DatabaseDeforc& keyword) {}
    virtual void visit(DatabaseDemassflow& keyword) {}
    virtual void visit(DatabaseDemrcf& keyword) {}
    virtual void visit(DatabaseDisbout& keyword) {}
    virtual void visit(DatabaseElout& keyword) {}
    virtual void visit(DatabaseExtentBinary& keyword) {}
    virtual void visit(DatabaseFormat& keyword) {}
    virtual void visit(DatabaseFsi& keyword) {}
    virtual void visit(DatabaseFsiSensor& keyword) {}
    virtual void visit(DatabaseGlstat& keyword) {}
    virtual void visit(DatabaseHistoryBeam& keyword) {}
    virtual void visit(DatabaseHistoryNode& keyword) {}
    virtual void visit(DatabaseHistoryShell& keyword) {}
    virtual void visit(DatabaseHistorySolid& keyword) {}
    virtual void visit(DatabaseHistoryTshell& keyword) {}
    virtual void visit(DatabaseJntforc& keyword) {}
    virtual void visit(DatabaseJntforcLocal& keyword) {}
    virtual void visit(DatabaseMassout& keyword) {}
    virtual void visit(DatabaseMatsum& keyword) {}
    virtual void visit(DatabaseMovie& keyword) {}
    virtual void visit(DatabaseNcforc& keyword) {}
    virtual void visit(DatabaseNcforcFilter& keyword) {}
    virtual void visit(DatabaseNodfor& keyword) {}
    virtual void visit(DatabaseNodout& keyword) {}
    virtual void visit(DatabasePllyout& keyword) {}
    virtual void visit(DatabasePllyoutRetractor& keyword) {}
    virtual void visit(DatabasePrtube& keyword) {}
    virtual void visit(DatabasePsd& keyword) {}
    virtual void visit(DatabasePwpOutput& keyword) {}
    virtual void visit(DatabaseRbdout& keyword) {}
    virtual void visit(DatabaseRcforc& keyword) {}
    virtual void visit(DatabaseRve& keyword) {}
    virtual void visit(DatabaseRwforc& keyword) {}
    virtual void visit(DatabaseSbtout& keyword) {}
    virtual void visit(DatabaseSbtoutRetractor& keyword) {}
    virtual void visit(DatabaseSbtoutSensor& keyword) {}
    virtual void visit(DatabaseSecforc& keyword) {}
    virtual void visit(DatabaseSleout& keyword) {}
    virtual void visit(DatabaseSpcforc& keyword) {}
    virtual void visit(DatabaseSphFlow& keyword) {}
    virtual void visit(DatabaseSphFlowSensor& keyword) {}
    virtual void visit(DatabaseSsstat& keyword) {}
    virtual void visit(DatabaseSwforc& keyword) {}
    virtual void visit(DatabaseSwforcFilter& keyword) {}
    virtual void visit(DatabaseTotgeo& keyword) {}
    virtual void visit(DatabaseTprint& keyword) {}
    virtual void visit(DatabaseTracer& keyword) {}
    virtual void visit(DefineAdaptiveSolidToSph& keyword) {}
    virtual void visit(DefineBirthTimes& keyword) {}
    virtual void visit(DefineBox& keyword) {}
    virtual void visit(DefineCarpetPlot& keyword) {}
    virtual void visit(DefineConnectionProperties& keyword) {}
    virtual void visit(DefineContactVolume& keyword) {}
    virtual void visit(DefineCoordinateNodes& keyword) {}
    virtual void visit(DefineCoordinateSystem& keyword) {}
    virtual void visit(DefineCoordinateVector& keyword) {}
    virtual void visit(DefineCpmVent& keyword) {}
    virtual void visit(DefineCurve& keyword) {}
    virtual void visit(DefineCurveCompensated& keyword) {}
    virtual void visit(DefineCurveDuplicate& keyword) {}
    virtual void visit(DefineCurveEntity& keyword) {}
    virtual void visit(DefineCurveFunction& keyword) {}
    virtual void visit(DefineCurveSmooth& keyword) {}
    virtual void visit(DefineCurveTitle& keyword) {}
    virtual void visit(DefineCurveTrim2d& keyword) {}
    virtual void visit(DefineCurveTrim3d& keyword) {}
    virtual void visit(DefineDeathTimes& keyword) {}
    virtual void visit(DefineDeMeshBeam& keyword) {}
    virtual void visit(DefineDeMeshSurface& keyword) {}
    virtual void visit(DefineElementDeath& keyword) {}
    virtual void visit(DefineFilter& keyword) {}
    virtual void visit(DefineFormingBlankmesh& keyword) {}
    virtual void visit(DefineFriction& keyword) {}
    virtual void visit(DefineFunction& keyword) {}
    virtual void visit(DefineFunctionTabulated& keyword) {}
    virtual void visit(DefineGroundMotion& keyword) {}
    virtual void visit(DefineHazProperties& keyword) {}
    virtual void visit(DefineHexSpotweldAssembly& keyword) {}
    virtual void visit(DefineLoadBlastSegment& keyword) {}
    virtual void visit(DefineMaterialHistories& keyword) {}
    virtual void visit(DefinePblastAirburst& keyword) {}
    virtual void visit(DefinePblastSurface& keyword) {}
    virtual void visit(DefinePorousChemistry& keyword) {}
    virtual void visit(DefinePressureTube& keyword) {}
    virtual void visit(DefineRegion& keyword) {}
    virtual void visit(DefineSdOrientation& keyword) {}
    virtual void visit(DefineSphAmbientDrag& keyword) {}
    virtual void visit(DefineSphInjection& keyword) {}
    virtual void visit(DefineSphMeshBox& keyword) {}
    virtual void visit(DefineSphToSphCoupling& keyword) {}
    virtual void visit(DefineSpotweldMultiscale& keyword) {}
    virtual void visit(DefineSpotweldRuptureParameter& keyword) {}
    virtual void visit(DefineStochasticVariation& keyword) {}
    virtual void visit(DefineTable& keyword) {}
    virtual void visit(DefineTable2D& keyword) {}
    virtual void visit(DefineTable3D& keyword) {}
    virtual void visit(DefineTransformation& keyword) {}
    virtual void visit(DefineTrimSeed& keyword) {}
    virtual void visit(DefineVector& keyword) {}
    virtual void visit(DeformableToRigid& keyword) {}
    virtual void visit(DeformableToRigidAutomatic& keyword) {}
    virtual void visit(DeformableToRigidAutomaticId& keyword) {}
    virtual void visit(DeformableToRigidInertia& keyword) {}
    virtual void visit(DeformableToRigidTitle& keyword) {}
    virtual void visit(DualceseBoundaryPrescribedPre& keyword) {}
    virtual void visit(DualceseBoundaryPrescribedVel& keyword) {}
    virtual void visit(DualceseBoundaryReflective& keyword) {}
    virtual void visit(DualceseControlGeneral& keyword) {}
    virtual void visit(DualceseControlSolver& keyword) {}
    virtual void visit(DualceseControlTimestep& keyword) {}
    virtual void visit(DualceseInitial& keyword) {}
    virtual void visit(DualceseMat& keyword) {}
    virtual void visit(DualcesePart& keyword) {}
    virtual void visit(ElementBeam& keyword) {}
    virtual void visit(ElementBeamOrientation& keyword) {}
    virtual void visit(ElementBeamPulley& keyword) {}
    virtual void visit(ElementBearing& keyword) {}
    virtual void visit(ElementDirectMatrixInput& keyword) {}
    virtual void visit(ElementDiscrete& keyword) {}
    virtual void visit(ElementGeneralizedShell& keyword) {}
    virtual void visit(ElementInertia& keyword) {}
    virtual void visit(ElementInterpolationShell& keyword) {}
    virtual void visit(ElementLancing& keyword) {}
    virtual void visit(ElementMass& keyword) {}
    virtual void visit(ElementMassNodeSet& keyword) {}
    virtual void visit(ElementMassPartSet& keyword) {}
    virtual void visit(ElementPlotel& keyword) {}
    virtual void visit(ElementSeatbelt& keyword) {}
    virtual void visit(ElementSeatbeltAccelerometer& keyword) {}
    virtual void visit(ElementSeatbeltPretensioner& keyword) {}
    virtual void visit(ElementSeatbeltRetractor& keyword) {}
    virtual void visit(ElementSeatbeltSensor& keyword) {}
    virtual void visit(ElementSeatbeltSlipring& keyword) {}
    virtual void visit(ElementShell& keyword) {}
    virtual void visit(ElementShellComposite& keyword) {}
    virtual void visit(ElementShellSourceSink& keyword) {}
    virtual void visit(ElementShellThickness& keyword) {}
    virtual void visit(ElementSolid& keyword) {}
    virtual void visit(ElementSolidOrtho& keyword) {}
    virtual void visit(ElementSph& keyword) {}
    virtual void visit(ElementTrim& keyword) {}
    virtual void visit(ElementTshell& keyword) {}
    virtual void visit(EmBoundary& keyword) {}
    virtual void visit(EmCircuit& keyword) {}
    virtual void visit(EmContact& keyword) {}
    virtual void visit(EmControl& keyword) {}
    virtual void visit(EmControlCoupling& keyword) {}
    virtual void visit(EmIsopotential& keyword) {}
    virtual void visit(EmMat001& keyword) {}
    virtual void visit(EmMat002& keyword) {}
    virtual void visit(EmOutput& keyword) {}
    virtual void visit(EmRandlesBatmac& keyword) {}
    virtual void visit(EmSolverFem& keyword) {}
    virtual void visit(EosGasket& keyword) {}
    virtual void visit(EosGruneisen& keyword) {}
    virtual void visit(EosIdealGas& keyword) {}
    virtual void visit(EosIgnitionGrowth& keyword) {}
    virtual void visit(EosJwl& keyword) {}
    virtual void visit(EosJwlb& keyword) {}
    virtual void visit(EosLinearPolynomial& keyword) {}
    virtual void visit(EosLinearPolynomialWithEnergyLeak& keyword) {}
    virtual void visit(EosMurnaghan& keyword) {}
    virtual void visit(EosOsborne& keyword) {}
    virtual void visit(EosPowderBurn& keyword) {}
    virtual void visit(EosPropellantDeflagration& keyword) {}
    virtual void visit(EosRatioOfPolynomials& keyword) {}
    virtual void visit(EosSack& keyword) {}
    virtual void visit(EosSesame& keyword) {}
    virtual void visit(EosStiffGas& keyword) {}
    virtual void visit(EosTabulated& keyword) {}
    virtual void visit(EosTabulatedCompaction& keyword) {}
    virtual void visit(EosTillotson& keyword) {}
    virtual void visit(EosUserDefined& keyword) {}
    virtual void visit(FrequencyDomainAcousticBem& keyword) {}
    virtual void visit(FrequencyDomainAcousticFem& keyword) {}
    virtual void visit(FrequencyDomainFrf& keyword) {}
    virtual void visit(FrequencyDomainMode& keyword) {}
    virtual void visit(FrequencyDomainPath& keyword) {}
    virtual void visit(FrequencyDomainRandomVibration& keyword) {}
    virtual void visit(FrequencyDomainResponseSpectrum& keyword) {}
    virtual void visit(FrequencyDomainSsd& keyword) {}
    virtual void visit(FrequencyDomainSsdDirect& keyword) {}
    virtual void visit(FrequencyDomainVibrationFatigue& keyword) {}
    virtual void visit(GenericKeyword& keyword) {}
    virtual void visit(Hourglass& keyword) {}
    virtual void visit(HourglassBeam& keyword) {}
    virtual void visit(HourglassId& keyword) {}
    virtual void visit(HourglassPart& keyword) {}
    virtual void visit(HourglassShell& keyword) {}
    virtual void visit(HourglassSolid& keyword) {}
    virtual void visit(HourglassThicknessChange& keyword) {}
    virtual void visit(HourglassTitle& keyword) {}
    virtual void visit(IcfdBoundaryPrescribedPre& keyword) {}
    virtual void visit(IcfdBoundaryPrescribedVel& keyword) {}
    virtual void visit(IcfdControlFsi& keyword) {}
    virtual void visit(IcfdControlGeneral& keyword) {}
    virtual void visit(IcfdControlMesh& keyword) {}
    virtual void visit(IcfdControlOutput& keyword) {}
    virtual void visit(IcfdControlTime& keyword) {}
    virtual void visit(IcfdControlTurbulence& keyword) {}
    virtual void visit(IcfdDefineMesh& keyword) {}
    virtual void visit(IcfdInitial& keyword) {}
    virtual void visit(IcfdMat& keyword) {}
    virtual void visit(IcfdPart& keyword) {}
    virtual void visit(Include& keyword) {}
    virtual void visit(IncludeAutoOffset& keyword) {}
    virtual void visit(IncludeBinary& keyword) {}
    virtual void visit(IncludeCompensate& keyword) {}
    virtual void visit(IncludePath& keyword) {}
    virtual void visit(IncludePathRelative& keyword) {}
    virtual void visit(IncludeStampedPart& keyword) {}
    virtual void visit(IncludeTransform& keyword) {}
    virtual void visit(InitialAirbagParticlePosition& keyword) {}
    virtual void visit(InitialAleMapping& keyword) {}
    virtual void visit(InitialAlePressure& keyword) {}
    virtual void visit(InitialAngularMomentum& keyword) {}
    virtual void visit(InitialAxialForceBeam& keyword) {}
    virtual void visit(InitialContactWear& keyword) {}
    virtual void visit(InitialDetonation& keyword) {}
    virtual void visit(InitialFoamReferenceGeometry& keyword) {}
    virtual void visit(InitialGasMixture& keyword) {}
    virtual void visit(InitialHistoryNode& keyword) {}
    virtual void visit(InitialHistoryShell& keyword) {}
    virtual void visit(InitialHistorySolid& keyword) {}
    virtual void visit(InitialImpulseMine& keyword) {}
    virtual void visit(InitialInternalDofSolid& keyword) {}
    virtual void visit(InitialMomentum& keyword) {}
    virtual void visit(InitialPwpDepth& keyword) {}
    virtual void visit(InitialRotationalVelocity& keyword) {}
    virtual void visit(InitialSpcRotationAngle& keyword) {}
    virtual void visit(InitialSphMassFraction& keyword) {}
    virtual void visit(InitialStrainShell& keyword) {}
    virtual void visit(InitialStrainSolid& keyword) {}
    virtual void visit(InitialStressBeam& keyword) {}
    virtual void visit(InitialStressDepth& keyword) {}
    virtual void visit(InitialStressDiscrete& keyword) {}
    virtual void visit(InitialStressSection& keyword) {}
    virtual void visit(InitialStressShell& keyword) {}
    virtual void visit(InitialStressSolid& keyword) {}
    virtual void visit(InitialStressTshell& keyword) {}
    virtual void visit(InitialTemperature& keyword) {}
    virtual void visit(InitialTemperatureSet& keyword) {}
    virtual void visit(InitialVelocity& keyword) {}
    virtual void visit(InitialVelocityGeneration& keyword) {}
    virtual void visit(InitialVelocityNode& keyword) {}
    virtual void visit(InitialVelocitySet& keyword) {}
    virtual void visit(InitialVolumeFraction& keyword) {}
    virtual void visit(InitialVolumeFractionGeometry& keyword) {}
    virtual void visit(IntegrationBeam& keyword) {}
    virtual void visit(IntegrationPoint& keyword) {}
    virtual void visit(IntegrationPointTransform& keyword) {}
    virtual void visit(IntegrationShell& keyword) {}
    virtual void visit(IntegrationShellThickness& keyword) {}
    virtual void visit(IntegrationSolid& keyword) {}
    virtual void visit(IntegrationTshell& keyword) {}
    virtual void visit(InterfaceBlanksize& keyword) {}
    virtual void visit(InterfaceCompensationNew& keyword) {}
    virtual void visit(InterfaceComponent& keyword) {}
    virtual void visit(InterfaceFrictionalSlideline& keyword) {}
    virtual void visit(InterfaceLinking& keyword) {}
    virtual void visit(InterfaceLinkingDiscrete& keyword) {}
    virtual void visit(InterfaceSpringbackLsdyna& keyword) {}
    virtual void visit(InterfaceSpringbackSeamless& keyword) {}
    virtual void visit(InterfaceSsi& keyword) {}
    virtual void visit(LoadAcousticSource& keyword) {}
    virtual void visit(LoadAirbagPressure& keyword) {}
    virtual void visit(LoadAirmix& keyword) {}
    virtual void visit(LoadAleConvection& keyword) {}
    virtual void visit(LoadBeam& keyword) {}
    virtual void visit(LoadBeamSet& keyword) {}
    virtual void visit(LoadBlast& keyword) {}
    virtual void visit(LoadBlastEnhanced& keyword) {}
    virtual void visit(LoadBlastSegmentSet& keyword) {}
    virtual void visit(LoadBodyGeneralized& keyword) {}
    virtual void visit(LoadBodyPart& keyword) {}
    virtual void visit(LoadBodyParts& keyword) {}
    virtual void visit(LoadBodyPartSet& keyword) {}
    virtual void visit(LoadBodyPorous& keyword) {}
    virtual void visit(LoadBodyRx& keyword) {}
    virtual void visit(LoadBodyRy& keyword) {}
    virtual void visit(LoadBodyRz& keyword) {}
    virtual void visit(LoadBodyVector& keyword) {}
    virtual void visit(LoadBodyX& keyword) {}
    virtual void visit(LoadBodyY& keyword) {}
    virtual void visit(LoadBodyZ& keyword) {}
    virtual void visit(LoadBrode& keyword) {}
    virtual void visit(LoadConvection& keyword) {}
    virtual void visit(LoadDensity& keyword) {}
    virtual void visit(LoadDensityDepth& keyword) {}
    virtual void visit(LoadErodingPartSet& keyword) {}
    virtual void visit(LoadExpansionPressure& keyword) {}
    virtual void visit(LoadFluidPressure& keyword) {}
    virtual void visit(LoadGravity& keyword) {}
    virtual void visit(LoadGravityPart& keyword) {}
    virtual void visit(LoadHeatController& keyword) {}
    virtual void visit(LoadHeatExothermicReaction& keyword) {}
    virtual void visit(LoadHeatFlux& keyword) {}
    virtual void visit(LoadHeatGeneration& keyword) {}
    virtual void visit(LoadHeatGenerationSet& keyword) {}
    virtual void visit(LoadInteriorPressure& keyword) {}
    virtual void visit(LoadLanczos& keyword) {}
    virtual void visit(LoadMask& keyword) {}
    virtual void visit(LoadMotionNode& keyword) {}
    virtual void visit(LoadMotionNodeSet& keyword) {}
    virtual void visit(LoadMovingPressure& keyword) {}
    virtual void visit(LoadNegativeVolume& keyword) {}
    virtual void visit(LoadNode& keyword) {}
    virtual void visit(LoadNodePoint& keyword) {}
    virtual void visit(LoadNodeSet& keyword) {}
    virtual void visit(LoadPendulum& keyword) {}
    virtual void visit(LoadPressurePenetration& keyword) {}
    virtual void visit(LoadPze& keyword) {}
    virtual void visit(LoadRadiation& keyword) {}
    virtual void visit(LoadRail& keyword) {}
    virtual void visit(LoadRemovePart& keyword) {}
    virtual void visit(LoadRigidBody& keyword) {}
    virtual void visit(LoadRigidBodyInertia& keyword) {}
    virtual void visit(LoadSegment& keyword) {}
    virtual void visit(LoadSegmentData& keyword) {}
    virtual void visit(LoadSegmentFile& keyword) {}
    virtual void visit(LoadSegmentId& keyword) {}
    virtual void visit(LoadSegmentNonuniform& keyword) {}
    virtual void visit(LoadSegmentPressure& keyword) {}
    virtual void visit(LoadSegmentSet& keyword) {}
    virtual void visit(LoadSegmentSetAngle& keyword) {}
    virtual void visit(LoadSeismic& keyword) {}
    virtual void visit(LoadSeismicSsi& keyword) {}
    virtual void visit(LoadSeismicSsiAux& keyword) {}
    virtual void visit(LoadShellElement& keyword) {}
    virtual void visit(LoadShellSet& keyword) {}
    virtual void visit(LoadSoftElement& keyword) {}
    virtual void visit(LoadSpc& keyword) {}
    virtual void visit(LoadSpcForce& keyword) {}
    virtual void visit(LoadSpcSet& keyword) {}
    virtual void visit(LoadSsa& keyword) {}
    virtual void visit(LoadSsaGravity& keyword) {}
    virtual void visit(LoadSteadyStateRolling& keyword) {}
    virtual void visit(LoadStiffenPart& keyword) {}
    virtual void visit(LoadSuperplasticForming& keyword) {}
    virtual void visit(LoadSurfaceStress& keyword) {}
    virtual void visit(LoadSurfaceStressSegment& keyword) {}
    virtual void visit(LoadThermalBinout& keyword) {}
    virtual void visit(LoadThermalBody& keyword) {}
    virtual void visit(LoadThermalConstant& keyword) {}
    virtual void visit(LoadThermalConstantNode& keyword) {}
    virtual void visit(LoadThermalD3plot& keyword) {}
    virtual void visit(LoadThermalElement& keyword) {}
    virtual void visit(LoadThermalLoadCurve& keyword) {}
    virtual void visit(LoadThermalRsw& keyword) {}
    virtual void visit(LoadThermalTopaz& keyword) {}
    virtual void visit(LoadThermalTopaz3d& keyword) {}
    virtual void visit(LoadThermalVariable& keyword) {}
    virtual void visit(LoadThermalVariableBeam& keyword) {}
    virtual void visit(LoadThermalVariableElement& keyword) {}
    virtual void visit(LoadThermalVariableNode& keyword) {}
    virtual void visit(LoadThermalVariableShell& keyword) {}
    virtual void visit(LoadThermalVariableSolid& keyword) {}
    virtual void visit(LoadThermalVariableTshell& keyword) {}
    virtual void visit(LoadTrackTurn& keyword) {}
    virtual void visit(LoadTyrePress& keyword) {}
    virtual void visit(LoadVolumeLoss& keyword) {}
    virtual void visit(LoadWave& keyword) {}
    virtual void visit(LoadWheelPatch& keyword) {}
    virtual void visit(Mat3ParameterBarlat& keyword) {}
    virtual void visit(MatAcoustic& keyword) {}
    virtual void visit(MatAddCohesive& keyword) {}
    virtual void visit(MatAddDamageGissmo& keyword) {}
    virtual void visit(MatAddErosion& keyword) {}
    virtual void visit(MatAddFatigue& keyword) {}
    virtual void visit(MatAddInelasticity& keyword) {}
    virtual void visit(MatAddPermeability& keyword) {}
    virtual void visit(MatAddPoreAir& keyword) {}
    virtual void visit(MatAddThermalExpansion& keyword) {}
    virtual void visit(MatAnisotropicViscoplastic& keyword) {}
    virtual void visit(MatArrudaBoyce& keyword) {}
    virtual void visit(MatArupAdhesive& keyword) {}
    virtual void visit(MatBamman& keyword) {}
    virtual void visit(MatBammanDamage& keyword) {}
    virtual void visit(MatBarlatAnisotropicPlasticity& keyword) {}
    virtual void visit(MatBilkhuDuboisFoam& keyword) {}
    virtual void visit(MatBlatzKoFoam& keyword) {}
    virtual void visit(MatBlatzKoRubber& keyword) {}
    virtual void visit(MatCableDiscreteBeam& keyword) {}
    virtual void visit(MatClosedCellFoam& keyword) {}
    virtual void visit(MatCohesiveElastic& keyword) {}
    virtual void visit(MatCohesiveGeneral& keyword) {}
    virtual void visit(MatCompositeDamage& keyword) {}
    virtual void visit(MatCompositeFailureSolidModel& keyword) {}
    virtual void visit(MatConcreteDamageRel3& keyword) {}
    virtual void visit(MatCrushableFoam& keyword) {}
    virtual void visit(MatCscmConcrete& keyword) {}
    virtual void visit(MatDamage3& keyword) {}
    virtual void visit(MatDamperNonlinearViscous& keyword) {}
    virtual void visit(MatDamperViscous& keyword) {}
    virtual void visit(MatElastic& keyword) {}
    virtual void visit(MatElasticPlasticHydro& keyword) {}
    virtual void visit(MatElasticPlasticThermal& keyword) {}
    virtual void visit(MatElasticViscoplasticThermal& keyword) {}
    virtual void visit(MatElasticWithViscosity& keyword) {}
    virtual void visit(MatEnhancedCompositeDamage& keyword) {}
    virtual void visit(MatFabric& keyword) {}
    virtual void visit(MatFldTransverselyAnisotropic& keyword) {}
    virtual void visit(MatForceLimited& keyword) {}
    virtual void visit(MatFrazerNashRubber& keyword) {}
    virtual void visit(MatFuChangFoam& keyword) {}
    virtual void visit(MatGeneralViscoelastic& keyword) {}
    virtual void visit(MatGeologicCapModel& keyword) {}
    virtual void visit(MatHighExplosiveBurn& keyword) {}
    virtual void visit(MatHoneycomb& keyword) {}
    virtual void visit(MatHystereticBeam& keyword) {}
    virtual void visit(MatIsotropicElasticFailure& keyword) {}
    virtual void visit(MatIsotropicElasticPlastic& keyword) {}
    virtual void visit(MatJohnsonCook& keyword) {}
    virtual void visit(MatKelvinMaxwellViscoelastic& keyword) {}
    virtual void visit(MatLaminatedCompositeFabric& keyword) {}
    virtual void visit(MatLaminatedGlass& keyword) {}
    virtual void visit(MatLowDensityFoam& keyword) {}
    virtual void visit(MatModifiedPiecewiseLinearPlasticity& keyword) {}
    virtual void visit(MatMooneyRivlinRubber& keyword) {}
    virtual void visit(MatNonlinearOrthotropic& keyword) {}
    virtual void visit(MatNull& keyword) {}
    virtual void visit(MatOgdenRubber& keyword) {}
    virtual void visit(MatOrientedCrack& keyword) {}
    virtual void visit(MatOrthoElasticPlastic& keyword) {}
    virtual void visit(MatOrthotropicElastic& keyword) {}
    virtual void visit(MatPiecewiseLinearPlasticity& keyword) {}
    virtual void visit(MatPiecewiseLinearPlasticityStochastic& keyword) {}
    virtual void visit(MatPlasticGreenNaghdi& keyword) {}
    virtual void visit(MatPlasticityWithDamage& keyword) {}
    virtual void visit(MatPlasticityWithDamageOrtho& keyword) {}
    virtual void visit(MatPlasticKinematic& keyword) {}
    virtual void visit(MatPowerLawPlasticity& keyword) {}
    virtual void visit(MatPseudoTensor& keyword) {}
    virtual void visit(MatRateSensitiveCompositeFabric& keyword) {}
    virtual void visit(MatResultantPlasticity& keyword) {}
    virtual void visit(MatRigid& keyword) {}
    virtual void visit(MatSamp1& keyword) {}
    virtual void visit(MatSeismicIsolator& keyword) {}
    virtual void visit(MatShapeMemory& keyword) {}
    virtual void visit(MatSimplifiedJohnsonCook& keyword) {}
    virtual void visit(MatSimplifiedRubber& keyword) {}
    virtual void visit(MatSoftTissue& keyword) {}
    virtual void visit(MatSoilAndFoam& keyword) {}
    virtual void visit(MatSoilAndFoamFailure& keyword) {}
    virtual void visit(MatSpotWeld& keyword) {}
    virtual void visit(MatSpringElastic& keyword) {}
    virtual void visit(MatSpringElastoplastic& keyword) {}
    virtual void visit(MatSpringGeneralNonlinear& keyword) {}
    virtual void visit(MatSpringInelastic& keyword) {}
    virtual void visit(MatSpringMaxwell& keyword) {}
    virtual void visit(MatSpringNonlinearElastic& keyword) {}
    virtual void visit(MatSteinberg& keyword) {}
    virtual void visit(MatStrainRateDependentPlasticity& keyword) {}
    virtual void visit(MatTabulatedJohnsonCook& keyword) {}
    virtual void visit(MatTempDependentOrthotropic& keyword) {}
    virtual void visit(MatThermalIsotropic& keyword) {}
    virtual void visit(MatThermalOrthotropic& keyword) {}
    virtual void visit(MatTransverselyAnisotropicElasticPlastic& keyword) {}
    virtual void visit(MatUserDefined& keyword) {}
    virtual void visit(MatViscoelastic& keyword) {}
    virtual void visit(MatViscoelasticThermal& keyword) {}
    virtual void visit(MatViscousFoam& keyword) {}
    virtual void visit(MatWinfrithConcrete& keyword) {}
    virtual void visit(Node& keyword) {}
    virtual void visit(NodeMerge& keyword) {}
    virtual void visit(NodeMergeSet& keyword) {}
    virtual void visit(NodeMergeTolerance& keyword) {}
    virtual void visit(NodeReference& keyword) {}
    virtual void visit(NodeRigidBody& keyword) {}
    virtual void visit(NodeRigidSurface& keyword) {}
    virtual void visit(NodeScalar& keyword) {}
    virtual void visit(NodeScalarValue& keyword) {}
    virtual void visit(NodeSpotWeld& keyword) {}
    virtual void visit(NodeThickness& keyword) {}
    virtual void visit(NodeThicknessSet& keyword) {}
    virtual void visit(NodeThicknessSetGenerate& keyword) {}
    virtual void visit(NodeToTarget& keyword) {}
    virtual void visit(NodeToTargetVector& keyword) {}
    virtual void visit(NodeTransform& keyword) {}
    virtual void visit(Parameter& keyword) {}
    virtual void visit(ParameterDuplication& keyword) {}
    virtual void visit(ParameterExpression& keyword) {}
    virtual void visit(ParameterExpressionLocal& keyword) {}
    virtual void visit(ParameterGlobal& keyword) {}
    virtual void visit(ParameterLocal& keyword) {}
    virtual void visit(ParameterType& keyword) {}
    virtual void visit(Part& keyword) {}
    virtual void visit(PartAdaptiveFailure& keyword) {}
    virtual void visit(PartAnneal& keyword) {}
    virtual void visit(PartAveraged& keyword) {}
    virtual void visit(PartComposite& keyword) {}
    virtual void visit(PartCompositeTshell& keyword) {}
    virtual void visit(PartContact& keyword) {}
    virtual void visit(PartDuplicate& keyword) {}
    virtual void visit(ParticleBlast& keyword) {}
    virtual void visit(PartInertia& keyword) {}
    virtual void visit(PartModes& keyword) {}
    virtual void visit(PartMove& keyword) {}
    virtual void visit(PartsDistribute& keyword) {}
    virtual void visit(PartSensor& keyword) {}
    virtual void visit(PartsetDistribute& keyword) {}
    virtual void visit(PartStackedElements& keyword) {}
    virtual void visit(PartStiffness& keyword) {}
    virtual void visit(PerturbationContact& keyword) {}
    virtual void visit(PerturbationElement& keyword) {}
    virtual void visit(PerturbationGeometry& keyword) {}
    virtual void visit(PerturbationLoad& keyword) {}
    virtual void visit(PerturbationMat& keyword) {}
    virtual void visit(PerturbationNode& keyword) {}
    virtual void visit(PerturbationPart& keyword) {}
    virtual void visit(PerturbationSection& keyword) {}
    virtual void visit(PerturbationThickness& keyword) {}
    virtual void visit(RigidToDeformable& keyword) {}
    virtual void visit(RigidwallGeometricCone& keyword) {}
    virtual void visit(RigidwallGeometricCurved& keyword) {}
    virtual void visit(RigidwallGeometricCylinder& keyword) {}
    virtual void visit(RigidwallGeometricFlat& keyword) {}
    virtual void visit(RigidwallGeometricMotion& keyword) {}
    virtual void visit(RigidwallGeometricPrism& keyword) {}
    virtual void visit(RigidwallGeometricSphere& keyword) {}
    virtual void visit(RigidwallGeometricTorus& keyword) {}
    virtual void visit(RigidwallPlanar& keyword) {}
    virtual void visit(RigidwallPlanarFinite& keyword) {}
    virtual void visit(RigidwallPlanarFiniteId& keyword) {}
    virtual void visit(RigidwallPlanarForces& keyword) {}
    virtual void visit(RigidwallPlanarId& keyword) {}
    virtual void visit(RigidwallPlanarMoving& keyword) {}
    virtual void visit(RigidwallPlanarMovingFinite& keyword) {}
    virtual void visit(RigidwallPlanarMovingForces& keyword) {}
    virtual void visit(RigidwallPlanarOrtho& keyword) {}
    virtual void visit(SectionAle1d& keyword) {}
    virtual void visit(SectionAle2d& keyword) {}
    virtual void visit(SectionBeam& keyword) {}
    virtual void visit(SectionBeamTitle& keyword) {}
    virtual void visit(SectionDiscrete& keyword) {}
    virtual void visit(SectionDiscreteTitle& keyword) {}
    virtual void visit(SectionPointSource& keyword) {}
    virtual void visit(SectionPointSourceMixture& keyword) {}
    virtual void visit(SectionSeatbelt& keyword) {}
    virtual void visit(SectionShell& keyword) {}
    virtual void visit(SectionShellAle& keyword) {}
    virtual void visit(SectionShellEfg& keyword) {}
    virtual void visit(SectionShellTitle& keyword) {}
    virtual void visit(SectionSolid& keyword) {}
    virtual void visit(SectionSolidAle& keyword) {}
    virtual void visit(SectionSolidEfg& keyword) {}
    virtual void visit(SectionSolidTitle& keyword) {}
    virtual void visit(SectionSph& keyword) {}
    virtual void visit(SectionTshell& keyword) {}
    virtual void visit(SectionTshellTitle& keyword) {}
    virtual void visit(SensorControl& keyword) {}
    virtual void visit(SensorDefineCalcMath& keyword) {}
    virtual void visit(SensorDefineCoordinate& keyword) {}
    virtual void visit(SensorDefineElement& keyword) {}
    virtual void visit(SensorDefineForce& keyword) {}
    virtual void visit(SensorDefineFunction& keyword) {}
    virtual void visit(SensorDefineMisc& keyword) {}
    virtual void visit(SensorDefineNode& keyword) {}
    virtual void visit(SensorSwitch& keyword) {}
    virtual void visit(SensorSwitchCalcLogic& keyword) {}
    virtual void visit(SensorSwitchShellToSolid& keyword) {}
    virtual void visit(Set2dShell& keyword) {}
    virtual void visit(SetBeam& keyword) {}
    virtual void visit(SetBeamAdd& keyword) {}
    virtual void visit(SetBeamGeneral& keyword) {}
    virtual void visit(SetBeamGenerate& keyword) {}
    virtual void visit(SetBeamGenerateTitle& keyword) {}
    virtual void visit(SetBeamIntersect& keyword) {}
    virtual void visit(SetBeamList& keyword) {}
    virtual void visit(SetBeamListTitle& keyword) {}
    virtual void visit(SetBox& keyword) {}
    virtual void visit(SetDiscrete& keyword) {}
    virtual void visit(SetDiscreteAdd& keyword) {}
    virtual void visit(SetDiscreteGeneral& keyword) {}
    virtual void visit(SetDiscreteList& keyword) {}
    virtual void visit(SetDiscreteListTitle& keyword) {}
    virtual void visit(SetIgaEdge& keyword) {}
    virtual void visit(SetIgaEdgeUvw& keyword) {}
    virtual void visit(SetIgaFace& keyword) {}
    virtual void visit(SetIgaFaceUvw& keyword) {}
    virtual void visit(SetIgaPointUvw& keyword) {}
    virtual void visit(SetMode& keyword) {}
    virtual void visit(SetMulti& keyword) {}
    virtual void visit(SetMultiMaterialGroupList& keyword) {}
    virtual void visit(SetNode& keyword) {}
    virtual void visit(SetNodeAdd& keyword) {}
    virtual void visit(SetNodeColumn& keyword) {}
    virtual void visit(SetNodeGeneral& keyword) {}
    virtual void visit(SetNodeGenerate& keyword) {}
    virtual void visit(SetNodeGenerateTitle& keyword) {}
    virtual void visit(SetNodeIntersect& keyword) {}
    virtual void visit(SetNodeList& keyword) {}
    virtual void visit(SetNodeListTitle& keyword) {}
    virtual void visit(SetNodeTitle& keyword) {}
    virtual void visit(SetPart& keyword) {}
    virtual void visit(SetPartAdd& keyword) {}
    virtual void visit(SetPartColumn& keyword) {}
    virtual void visit(SetPartGeneral& keyword) {}
    virtual void visit(SetPartGenerate& keyword) {}
    virtual void visit(SetPartGenerateTitle& keyword) {}
    virtual void visit(SetPartIntersect& keyword) {}
    virtual void visit(SetPartList& keyword) {}
    virtual void visit(SetPartListGenerate& keyword) {}
    virtual void visit(SetPartListTitle& keyword) {}
    virtual void visit(SetPartTitle& keyword) {}
    virtual void visit(SetPartTree& keyword) {}
    virtual void visit(SetSeatbelt& keyword) {}
    virtual void visit(SetSegment& keyword) {}
    virtual void visit(SetSegmentAdd& keyword) {}
    virtual void visit(SetSegmentGeneral& keyword) {}
    virtual void visit(SetSegmentIntersect& keyword) {}
    virtual void visit(SetSegmentTitle& keyword) {}
    virtual void visit(SetShell& keyword) {}
    virtual void visit(SetShellAdd& keyword) {}
    virtual void visit(SetShellGeneral& keyword) {}
    virtual void visit(SetShellGenerate& keyword) {}
    virtual void visit(SetShellGenerateTitle& keyword) {}
    virtual void visit(SetShellIntersect& keyword) {}
    virtual void visit(SetShellList& keyword) {}
    virtual void visit(SetShellListTitle& keyword) {}
    virtual void visit(SetSolid& keyword) {}
    virtual void visit(SetSolidAdd& keyword) {}
    virtual void visit(SetSolidGeneral& keyword) {}
    virtual void visit(SetSolidGenerate& keyword) {}
    virtual void visit(SetSolidGenerateTitle& keyword) {}
    virtual void visit(SetSolidIntersect& keyword) {}
    virtual void visit(SetSolidList& keyword) {}
    virtual void visit(SetSolidListTitle& keyword) {}
    virtual void visit(SetTshell& keyword) {}
    virtual void visit(SetTshellGeneral& keyword) {}
    virtual void visit(SetTshellList& keyword) {}
    virtual void visit(StochasticCorrelatedField& keyword) {}
    virtual void visit(StochasticMaterialProperty& keyword) {}
    virtual void visit(StochasticMonteCarloRun& keyword) {}
    virtual void visit(StochasticOutput& keyword) {}
    virtual void visit(StochasticPart& keyword) {}
    virtual void visit(StochasticRandomization& keyword) {}
    virtual void visit(StochasticSample& keyword) {}
    virtual void visit(StochasticStructureField& keyword) {}
    virtual void visit(StochasticVariable& keyword) {}
};

} // namespace koo::dyna
