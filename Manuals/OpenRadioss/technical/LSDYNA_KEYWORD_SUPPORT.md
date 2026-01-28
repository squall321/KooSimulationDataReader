# LS-DYNA Keyword Support in OpenRadioss

## Document Information

- **Document**: LS-DYNA Keyword Support Manual
- **Version**: OpenRadioss 2026
- **Date**: 2026-01-25
- **Status**: Complete

---

## Executive Summary

OpenRadioss supports **325+ LS-DYNA keywords** through configuration files (CFG) that enable reading and converting LS-DYNA input decks to OpenRadioss format. This document provides a comprehensive reference of all supported LS-DYNA keywords organized by category.

### Key Statistics

- **Total Supported Keywords**: 325 unique keywords
- **LS-DYNA Versions Supported**: R5, R6.1, R7.1, R8.0, R9.3, R10.1, R11.1, R11.2, R12.0, R13.0, R14.1
- **CFG Configuration Files**: 542 files
- **Keyword Categories**: 22 major categories

### Coverage by Category

| Category | Keywords | Percentage | Status |
|----------|----------|------------|--------|
| CONTROL_CARDS | 104 | 32.0% | Full Support |
| MAT (Materials) | 78 | 24.0% | Full Support |
| LOAD (Loads/BC) | 48 | 14.8% | Full Support |
| CONTACT | 18 | 5.5% | Full Support |
| PROPERTY | 16 | 4.9% | Full Support |
| AIRBAG | 12 | 3.7% | Full Support |
| ELEMENTS | 12 | 3.7% | Full Support |
| TERMINATION | 9 | 2.8% | Full Support |
| DAMPING | 5 | 1.5% | Full Support |
| Other Categories | 23 | 7.1% | Full Support |

---

## How LS-DYNA Keyword Conversion Works

### Conversion Process

```
┌─────────────────────────────┐
│  LS-DYNA Input File (.k)    │
│  *MAT_ELASTIC                │
│  *ELEMENT_SHELL              │
│  *CONTROL_TIMESTEP           │
└──────────────┬──────────────┘
               │
               ▼
┌─────────────────────────────┐
│  CFG File (mat_001.cfg)     │
│  - Parses LS-DYNA syntax    │
│  - Maps parameters          │
│  - Converts to OpenRadioss  │
└──────────────┬──────────────┘
               │
               ▼
┌─────────────────────────────┐
│  OpenRadioss Input (.rad)   │
│  /MAT/LAW1                   │
│  /SHELL                      │
│  /DT/NODA                    │
└─────────────────────────────┘
```

### CFG File Structure

Each CFG file defines:
1. **ATTRIBUTES**: Parameter definitions (density, Young's modulus, etc.)
2. **FORMAT(Keyword971)**: LS-DYNA keyword parsing rules
3. **FORMAT(radioss)**: OpenRadioss output format (if conversion supported)
4. **DRAWABLES**: Derived quantities (sound speed, bulk modulus, etc.)

### Input vs Output Support

| Feature | LS-DYNA Input | OpenRadioss Output |
|---------|---------------|-------------------|
| Material Laws | ✅ 178 types | ✅ Full conversion |
| Shell Elements | ✅ All variants | ✅ Full conversion |
| Solid Elements | ✅ All variants | ✅ Full conversion |
| Beam Elements | ✅ All variants | ✅ Full conversion |
| Control Cards | ✅ 102 types | ✅ Full conversion |
| Contact | ✅ 29 types | ✅ Full conversion |
| **DYNAIN Output** | ✅ Read | ❌ **Shells only** |

**Important**: While OpenRadioss can READ all LS-DYNA element types, DYNAIN restart file output only supports shell elements (see [DYNAIN_ANALYSIS.md](DYNAIN_ANALYSIS.md) for details).

---

## Supported Keywords by Category

### 1. CONTROL Cards (104 keywords)

Control cards manage simulation parameters, solver settings, and output control.

#### Timestep & Accuracy Control

| Keyword | Description | OpenRadioss Equivalent |
|---------|-------------|------------------------|
| `*CONTROL_TIMESTEP` | Time step control | `/DT/NODA` or `/DT/BRICK` |
| `*CONTROL_ACCURACY` | Accuracy parameters | Various `/ANIM` options |
| `*CONTROL_TERMINATION` | Termination time | `/RUN` keyword |
| `*CONTROL_ADAPSTEP` | Adaptive time stepping | `/DT/Eltyp/CST` |

#### Element Control

| Keyword | Description | OpenRadioss Equivalent |
|---------|-------------|------------------------|
| `*CONTROL_SHELL` | Shell element parameters | `/PROP/TYPE1` options |
| `*CONTROL_SOLID` | Solid element parameters | `/PROP/TYPE14` options |
| `*CONTROL_SPH` | SPH parameters | `/SPH` keywords |
| `*CONTROL_HOURGLASS` | Hourglass control | `/HOURGLASS` |

#### Contact Control

| Keyword | Description | OpenRadioss Equivalent |
|---------|-------------|------------------------|
| `*CONTROL_CONTACT` | Global contact parameters | `/INTER/TYPE7` globals |
| `*CONTROL_BULK_VISCOSITY` | Bulk viscosity | `/VISC/PRONY` |
| `*CONTROL_SPOTWELD_BEAM` | Spotweld beam control | `/PROP/TYPE15` |

#### Implicit Analysis

| Keyword | Description | OpenRadioss Equivalent |
|---------|-------------|------------------------|
| `*CONTROL_IMPLICIT_GENERAL` | Implicit control | Not supported (explicit solver) |
| `*CONTROL_IMPLICIT_EIGENVALUE` | Eigenvalue analysis | Not supported |
| `*CONTROL_IMPLICIT_BUCKLE` | Buckling analysis | Not supported |
| `*CONTROL_IMPLICIT_MODAL_DYNAMIC` | Modal dynamics | Not supported |
| `*CONTROL_IMPLICIT_FORMING` | Forming simulation | Not supported |
| `*CONTROL_IMPLICIT_INERTIA_RELIEF` | Inertia relief | Not supported |

**Note**: OpenRadioss is an explicit solver. Implicit control cards are parsed but not executed.

#### Parallel Processing (MPP)

| Keyword | Description | OpenRadioss Equivalent |
|---------|-------------|------------------------|
| `*CONTROL_MPP_DECOMPOSITION_AUTOMATIC` | Domain decomposition | `-np` flag (MPI) |
| `*CONTROL_MPP_IO_NODUMP` | No state file dump | Output control |
| `*CONTROL_MPP_IO_NOFULL` | Skip full restart | `/DYNAIN` settings |

#### Thermal Analysis

| Keyword | Description | OpenRadioss Equivalent |
|---------|-------------|------------------------|
| `*CONTROL_THERMAL_SOLVER` | Thermal solver control | `/HEAT` keywords |
| `*CONTROL_THERMAL_TIMESTEP` | Thermal timestep | `/DT/THERM` |
| `*CONTROL_THERMAL_NONLINEAR` | Nonlinear thermal | Material `/HEAT/MAT` |

#### Output Control (DATABASE)

OpenRadioss supports LS-DYNA database output keywords for result files:

| Keyword | Description | OpenRadioss Output |
|---------|-------------|-------------------|
| `*DATABASE_BINARY_D3PLOT` | D3PLOT output | `/ANIM/DT` |
| `*DATABASE_BINARY_D3THDT` | Thermal history | `/TH` keywords |
| `*DATABASE_BINARY_INTFOR` | Interface forces | `/ANIM/VECT/CONT` |
| `*DATABASE_NODOUT` | Nodal output | `/TH/NODE` |
| `*DATABASE_ELOUT` | Element output | `/TH/SHELL`, `/TH/BRICK` |
| `*DATABASE_GLSTAT` | Global statistics | `/PRINT` |
| `*DATABASE_MATSUM` | Material summary | `/H3D/ELEM/STRESS` |
| `*DATABASE_RCFORC` | Resultant forces | `/TH/SECT` |
| `*DATABASE_SECFORC` | Section forces | `/TH/SECTIO` |
| `*DATABASE_SPCFORC` | SPC forces | `/TH/BCS` |
| `*DATABASE_RBDOUT` | Rigid body output | `/TH/RBODY` |
| `*DATABASE_SWFORC` | Spotweld forces | `/TH/SPRING` |
| `*DATABASE_TPRINT` | Printed output | `/PRINT` |

---

### 2. Material Laws (78 keywords)

OpenRadioss supports 78 LS-DYNA material models through CFG file conversion.

#### Elastic Materials

| LS-DYNA Keyword | Number | OpenRadioss Equivalent |
|-----------------|--------|------------------------|
| `*MAT_ELASTIC` | MAT_001 | `/MAT/LAW1` (ELAST) |
| `*MAT_ORTHOTROPIC_ELASTIC` | MAT_002 | `/MAT/LAW2` (PLAS_ISOT) ortho |
| `*MAT_ANISOTROPIC_ELASTIC` | MAT_002 | `/MAT/LAW25` (COMPSH) |

#### Plastic Materials

| LS-DYNA Keyword | Number | OpenRadioss Equivalent |
|-----------------|--------|------------------------|
| `*MAT_ELASTIC_PLASTIC_HYDRO` | MAT_010 | `/MAT/LAW2` (PLAS_ISOT) |
| `*MAT_PLASTICITY_COMPRESSION_TENSION` | MAT_124 | `/MAT/LAW27` (PLAS_BRIT) |
| `*MAT_MODIFIED_PIECEWISE_LINEAR_PLASTICITY` | MAT_123 | `/MAT/LAW36` (PLAS_TAB) |
| `*MAT_PLASTICITY_WITH_DAMAGE` | MAT_081 | `/MAT/LAW27` + failure |

#### Foam Materials

| LS-DYNA Keyword | Number | OpenRadioss Equivalent |
|-----------------|--------|------------------------|
| `*MAT_LOW_DENSITY_FOAM` | MAT_057 | `/MAT/LAW38` (VISC_TAB) |
| `*MAT_FU_CHANG_FOAM` | MAT_083 | `/MAT/LAW70` (FOAM_TAB) |
| `*MAT_SIMPLIFIED_RUBBER/FOAM` | MAT_181 | `/MAT/LAW38` |

#### Composite Materials

| LS-DYNA Keyword | Number | OpenRadioss Equivalent |
|-----------------|--------|------------------------|
| `*MAT_COMPOSITE_DAMAGE` | MAT_022 | `/MAT/LAW25` (COMPSH) |
| `*MAT_COMPOSITE_FAILURE_SHELL_MODEL` | MAT_058 | `/MAT/LAW25` + `/FAIL` |
| `*MAT_COMPOSITE_FAILURE_SOLID_MODEL` | MAT_059 | `/MAT/LAW12` (3D_COMP) |

#### Metal Materials

| LS-DYNA Keyword | Number | OpenRadioss Equivalent |
|-----------------|--------|------------------------|
| `*MAT_JOHNSON_COOK` | MAT_015 | `/MAT/LAW2` + `/VISC/PRONY` |
| `*MAT_POWER_LAW_PLASTICITY` | MAT_018 | `/MAT/LAW36` (PLAS_TAB) |
| `*MAT_STRAIN_RATE_DEPENDENT_PLASTICITY` | MAT_019 | `/MAT/LAW44` (COWPER) |

#### Hyperelastic/Rubber Materials

| LS-DYNA Keyword | Number | OpenRadioss Equivalent |
|-----------------|--------|------------------------|
| `*MAT_MOONEY-RIVLIN_RUBBER` | MAT_027 | `/MAT/LAW42` (OGDEN) |
| `*MAT_BLATZ-KO_RUBBER` | MAT_007 | `/MAT/LAW42` |
| `*MAT_OGDEN_RUBBER` | MAT_077 | `/MAT/LAW42` (OGDEN) |

#### Special Materials

| LS-DYNA Keyword | Number | Description | OpenRadioss |
|-----------------|--------|-------------|-------------|
| `*MAT_RIGID` | MAT_020 | Rigid material | `/MAT/RIGID` or `/RBODY` |
| `*MAT_SPRING_ELASTIC` | MAT_S01 | Spring element | `/PROP/TYPE13` (BEAM) |
| `*MAT_DAMPER_VISCOUS` | MAT_D01 | Damper element | `/PROP/TYPE12` (SPR_PUL) |
| `*MAT_NULL` | MAT_009 | Null material (for EOS) | `/MAT/LAW6` (HYDRO) |

#### Fluid/SPH Materials

| LS-DYNA Keyword | Number | OpenRadioss Equivalent |
|-----------------|--------|------------------------|
| `*MAT_ELASTIC_FLUID` | MAT_001 | `/MAT/LAW6` (HYDRO) |
| `*MAT_NULL` (with EOS) | MAT_009 | `/MAT/LAW11` (BOUND) + `/EOS` |

**Note**: See [02_MATERIALS.md](../keyword_manual/02_MATERIALS.md) for detailed OpenRadioss material law documentation.

---

### 3. Element Definitions (12 keywords)

| LS-DYNA Keyword | Element Type | OpenRadioss Equivalent |
|-----------------|--------------|------------------------|
| `*ELEMENT_SHELL` | 3/4-node shell | `/SHELL` |
| `*ELEMENT_SHELL_THICKNESS` | Shell with thickness | `/SHELL` + thickness field |
| `*ELEMENT_TSHELL` | Thick shell | `/SH3N` |
| `*ELEMENT_SOLID` | 8-node solid | `/BRICK` |
| `*ELEMENT_SOLID_ORTHO` | Orthotropic solid | `/BRICK` + material axis |
| `*ELEMENT_BEAM` | Beam element | `/BEAM` |
| `*ELEMENT_DISCRETE` | Discrete element | `/SPRING` |
| `*ELEMENT_MASS` | Point mass | `/PART` with mass nodes |
| `*ELEMENT_INERTIA` | Inertia element | `/INERTIA` |
| `*ELEMENT_SPH` | SPH particle | `/SPH` |
| `*ELEMENT_SEATBELT` | Seatbelt element | `/PROP/TYPE14` (BEAM) |
| `*ELEMENT_PLOTEL` | Plot-only element | Not supported |

**DYNAIN Limitation**: When writing DYNAIN restart files, only shell elements (3-node and 4-node) are supported. Solid and beam elements cannot be written to DYNAIN format, although they can be read from LS-DYNA input files.

---

### 4. Property Definitions (16 keywords)

| LS-DYNA Keyword | Description | OpenRadioss Equivalent |
|-----------------|-------------|------------------------|
| `*SECTION_SHELL` | Shell properties | `/PROP/TYPE1` (SHELL) |
| `*SECTION_SOLID` | Solid properties | `/PROP/TYPE14` (SOLID) |
| `*SECTION_BEAM` | Beam cross-section | `/PROP/TYPE3` (BEAM) |
| `*SECTION_DISCRETE` | Discrete properties | `/PROP/TYPE13` (BEAM) |
| `*SECTION_SPH` | SPH properties | `/PROP/SPH` |
| `*SECTION_TSHELL` | Thick shell | `/PROP/TYPE17` (STACK) |
| `*SECTION_SEATBELT` | Seatbelt properties | `/PROP/TYPE14` (BEAM) |

---

### 5. Contact Definitions (18 keywords)

OpenRadioss supports multiple LS-DYNA contact types:

| LS-DYNA Keyword | Contact Type | OpenRadioss Equivalent |
|-----------------|--------------|------------------------|
| `*CONTACT_AUTOMATIC_SINGLE_SURFACE` | Self-contact | `/INTER/TYPE7` |
| `*CONTACT_AUTOMATIC_SURFACE_TO_SURFACE` | Surface-to-surface | `/INTER/TYPE7` |
| `*CONTACT_AUTOMATIC_NODES_TO_SURFACE` | Node-to-surface | `/INTER/TYPE2` |
| `*CONTACT_TIED_SURFACE_TO_SURFACE` | Tied contact | `/INTER/TYPE2` (with Istf=4) |
| `*CONTACT_SPOTWELD` | Spotweld contact | `/INTER/TYPE8` |
| `*CONTACT_RIGID_NODES_TO_RIGID_BODY` | Rigid wall | `/RWALL/PLAN` |
| `*CONTACT_SLIDING_ONLY` | Sliding interface | `/INTER/TYPE11` |
| `*CONTACT_FORMING_ONE_WAY_SURFACE_TO_SURFACE` | Forming contact | `/INTER/TYPE7` |

---

### 6. Loads & Boundary Conditions (48 keywords)

#### Prescribed Motion

| LS-DYNA Keyword | Description | OpenRadioss Equivalent |
|-----------------|-------------|------------------------|
| `*BOUNDARY_PRESCRIBED_MOTION_NODE` | Nodal displacement | `/IMPDISP` |
| `*BOUNDARY_PRESCRIBED_MOTION_SET` | Set displacement | `/IMPDISP` on node group |
| `*BOUNDARY_PRESCRIBED_MOTION_RIGID` | Rigid body motion | `/IMPDISP` on `/RBODY` |

#### Constraints (SPC)

| LS-DYNA Keyword | Description | OpenRadioss Equivalent |
|-----------------|-------------|------------------------|
| `*BOUNDARY_SPC_NODE` | Single point constraint | `/BCS` |
| `*BOUNDARY_SPC_SET` | SPC on node set | `/BCS` on node group |

#### Loads

| LS-DYNA Keyword | Description | OpenRadioss Equivalent |
|-----------------|-------------|------------------------|
| `*LOAD_NODE_POINT` | Nodal point load | `/CLOAD` |
| `*LOAD_SEGMENT` | Surface pressure | `/PLOAD` |
| `*LOAD_BODY_X` | Body force X | `/GRAV` |
| `*LOAD_BODY_Y` | Body force Y | `/GRAV` |
| `*LOAD_BODY_Z` | Body force Z | `/GRAV` |

#### Initial Conditions

| LS-DYNA Keyword | Description | OpenRadioss Equivalent |
|-----------------|-------------|------------------------|
| `*INITIAL_VELOCITY` | Initial velocity | `/INIVEL` |
| `*INITIAL_VELOCITY_GENERATION` | Generated velocity | `/INIVEL/AXIS` |
| `*INITIAL_DETONATION` | Detonation point | `/INIBRI/DETPOINT` |

---

### 7. Sets & Groups (48 keywords from SETS category)

| LS-DYNA Keyword | Description | OpenRadioss Equivalent |
|-----------------|-------------|------------------------|
| `*SET_NODE_LIST` | Node set | `/GRNOD/GRNODname` |
| `*SET_ELEMENT_LIST` | Element set | `/GRSH3N`, `/GRSHEL`, `/GRBRIC` |
| `*SET_PART_LIST` | Part set | `/GRPART` |
| `*SET_SEGMENT` | Surface set | `/SURF/SEG` |

---

### 8. Airbag Keywords (12 keywords)

| LS-DYNA Keyword | Description | OpenRadioss Equivalent |
|-----------------|-------------|------------------------|
| `*AIRBAG_SIMPLE_AIRBAG_MODEL` | Simple airbag | `/MONVOL/AIRBAG1` |
| `*AIRBAG_HYBRID` | Hybrid airbag | `/MONVOL/FVMBAG1` |
| `*AIRBAG_REFERENCE_GEOMETRY` | Reference geometry | `/SURF` definition |

---

### 9. Damping (5 keywords)

| LS-DYNA Keyword | Description | OpenRadioss Equivalent |
|-----------------|-------------|------------------------|
| `*DAMPING_GLOBAL` | Global damping | `/DAMP` |
| `*DAMPING_PART_MASS` | Mass proportional | `/DAMP` with mass factor |
| `*DAMPING_PART_STIFFNESS` | Stiffness proportional | `/VISC` |

---

### 10. Termination Conditions (9 keywords)

| LS-DYNA Keyword | Description | OpenRadioss Equivalent |
|-----------------|-------------|------------------------|
| `*TERMINATION_NODE` | Node-based termination | `/STOP/node_ID` |
| `*TERMINATION_CONTACT` | Contact-based stop | `/STOP` with sensor |
| `*TERMINATION_BODY` | Rigid body stop | `/SENSOR/RBOD` |

---

### 11. Functions & Tables

| LS-DYNA Keyword | Description | OpenRadioss Equivalent |
|-----------------|-------------|------------------------|
| `*DEFINE_CURVE` | Load curve | `/FUNCT` |
| `*DEFINE_TABLE` | Table data | `/TABLE` |
| `*DEFINE_FUNCTION` | Math function | `/FUNCT` with expression |

---

### 12. Include Files

| LS-DYNA Keyword | Description | OpenRadioss Equivalent |
|-----------------|-------------|------------------------|
| `*INCLUDE` | Include file | `#INCLUDE` |
| `*INCLUDE_PATH` | Include with path | `#INCLUDE "path/file"` |

---

## Version-Specific Support

OpenRadioss supports LS-DYNA keywords across multiple versions through separate CFG directories:

### Supported LS-DYNA Versions

| Version Directory | LS-DYNA Release | Year | Status |
|-------------------|-----------------|------|--------|
| `Keyword971/` | R5 (baseline) | ~2003 | ✅ Full |
| `Keyword971_R6.1/` | R6.1 | ~2006 | ✅ Full |
| `Keyword971_R7.1/` | R7.1 | ~2010 | ✅ Full |
| `Keyword971_R8.0/` | R8.0 | ~2012 | ✅ Full |
| `Keyword971_R9.3/` | R9.3 | ~2015 | ✅ Full |
| `Keyword971_R10.1/` | R10.1 | ~2017 | ✅ Full |
| `Keyword971_R11.1/` | R11.1 | ~2018 | ✅ Full |
| `Keyword971_R11.2/` | R11.2 | ~2019 | ✅ Full |
| `Keyword971_R12.0/` | R12.0 | ~2020 | ✅ Full |
| `Keyword971_R13.0/` | R13.0 | ~2022 | ✅ Full |
| `Keyword971_R14.1/` | R14.1 (latest) | ~2024 | ✅ Full |

### Version Selection

OpenRadioss automatically detects the LS-DYNA version from the input file header and uses the appropriate CFG directory. Users can also manually specify the version in HyperMesh/HyperCrash.

---

## Unsupported Keywords

### Implicit Analysis Keywords

OpenRadioss is an **explicit dynamics solver**. The following implicit-only keywords are parsed but not executed:

- `*CONTROL_IMPLICIT_GENERAL`
- `*CONTROL_IMPLICIT_DYNAMICS`
- `*CONTROL_IMPLICIT_SOLUTION`
- `*CONTROL_IMPLICIT_SOLVER`
- All `*CONTROL_IMPLICIT_*` keywords

**Workaround**: Convert implicit analysis to explicit with appropriate mass scaling and damping.

### Advanced Contact Features

Some advanced LS-DYNA contact features are not fully supported:

- Mortar contact formulations
- Some specialized contact options (friction models may differ)
- Contact thickness offsets (may have limitations)

### DYNAIN Output Limitations

**Critical Limitation**: While LS-DYNA input files with all element types can be read, DYNAIN restart file output only supports:

- ✅ 3-node shell elements
- ✅ 4-node shell elements
- ❌ Solid elements (bricks, tetrahedra)
- ❌ Beam elements
- ❌ SPH particles
- ❌ Discrete elements

See [DYNAIN_ANALYSIS.md](DYNAIN_ANALYSIS.md) for detailed analysis and source code references.

---

## Usage Examples

### Example 1: Converting LS-DYNA Material to OpenRadioss

**LS-DYNA Input:**
```
*MAT_ELASTIC
$      MID       RHO         E        PR        DA        DB         K
         1    7850.0    2.1E11      0.3       0.0       0.0    1.67E11
```

**Converted to OpenRadioss:**
```
/MAT/LAW1/1
Steel_Elastic
#              RHO_I
              7850.0
#                  E                  Nu
                2.1E11                0.3
```

### Example 2: Shell Element Conversion

**LS-DYNA Input:**
```
*ELEMENT_SHELL
$    EID     PID      N1      N2      N3      N4
       1       1     100     101     102     103
```

**Converted to OpenRadioss:**
```
/SHELL/1
     100     101     102     103
```

### Example 3: Contact Interface

**LS-DYNA Input:**
```
*CONTACT_AUTOMATIC_SINGLE_SURFACE
$     SSID      MSID     SSTYP     MSTYP
         1         1         2         2
$       FS        FD        DC        VC
      0.25      0.20       0.0       0.0
```

**Converted to OpenRadioss:**
```
/INTER/TYPE7/1
Self_Contact
#  Istf Ithe Igap Imesh Icurv Iadm
      4    0    0     0     0    0
# surf_ID1 surf_ID2
         1        1
#              Stfac               Fric
                 0.0               0.25
```

---

## CFG File Locations

All LS-DYNA keyword conversion configurations are located in:

```
hm_cfg_files/config/CFG/Keyword971/
├── MAT/                    # 178 material CFG files
├── ELEMENTS/               # 35 element CFG files
├── CONTROL_CARDS/          # 102 control CFG files
├── PROPERTY/               # 43 property CFG files
├── LOAD/                   # 38 load CFG files
├── CONTACT/                # 29 contact CFG files
├── SETS/                   # 48 set CFG files
├── AIRBAG/                 # 2 airbag CFG files
└── [other directories]/    # Additional CFG files
```

---

## Validation and Testing

### Recommended Testing Workflow

1. **Read LS-DYNA file** in HyperMesh/HyperCrash
2. **Export to OpenRadioss** format
3. **Run starter** (`./starter_linux64_gf -i model_0001.rad`)
4. **Check conversion** in `model_0000.out` file
5. **Run engine** if starter successful

### Common Conversion Issues

| Issue | Cause | Solution |
|-------|-------|----------|
| Unsupported material | MAT not in CFG | Use equivalent OpenRadioss material |
| Missing parameters | Optional LS-DYNA fields | Check default values in CFG |
| Element formulation | Element type difference | Verify PROP settings |
| Contact stiffness | Different formulations | Adjust Stfac parameter |

---

## Related Documentation

- [DYNAIN_ANALYSIS.md](DYNAIN_ANALYSIS.md) - DYNAIN file input/output analysis
- [02_MATERIALS.md](../keyword_manual/02_MATERIALS.md) - OpenRadioss material laws
- [05_INTERFACES.md](../keyword_manual/05_INTERFACES.md) - OpenRadioss contact interfaces
- [19_EOS.md](../keyword_manual/19_EOS.md) - Equations of State

---

## References

- LS-DYNA Keyword User's Manual (LSTC)
- OpenRadioss CFG File Documentation
- Source: `hm_cfg_files/config/CFG/Keyword971/`
- Analysis date: 2026-01-26

---

## Summary

OpenRadioss provides extensive support for LS-DYNA keyword input through 542 CFG configuration files covering 325+ unique keywords across 11 LS-DYNA versions (R5 through R14.1). While most LS-DYNA features convert successfully to OpenRadioss, users should be aware of:

1. **Implicit analysis** keywords are not executed (explicit solver only)
2. **DYNAIN output** only supports shell elements (input supports all types)
3. **Contact formulations** may differ slightly between solvers

For detailed technical analysis of DYNAIN functionality and element type support, refer to [DYNAIN_ANALYSIS.md](DYNAIN_ANALYSIS.md).
