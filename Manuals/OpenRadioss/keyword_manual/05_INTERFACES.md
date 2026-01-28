# OpenRadioss Keyword Manual - Interfaces (Contact)

## Overview

This document describes interface (contact) definitions in OpenRadioss. Interfaces define contact conditions between surfaces, including tied connections, sliding contact, friction, and heat transfer. OpenRadioss provides 25+ interface types for various contact scenarios.

---

## Complete Interface Type Reference

| Type | Description | Application |
|------|-------------|-------------|
| TYPE1 | ALE-Lagrangian coupling | Fluid-structure interaction |
| TYPE2 | Tied interface (Spotweld) | Spot welds, adhesive bonds, mesh connection |
| TYPE3 | Rigid surface contact | Rigid tool forming, stamping |
| TYPE5 | Tied/Glued interface | General tied connections |
| TYPE6 | Rigid-to-rigid contact | Mechanism contact, rigid body interaction |
| TYPE7 | General contact | Deformable-to-deformable, crashworthiness |
| TYPE8 | Drawbead contact | Sheet metal forming drawbeads |
| TYPE9 | ALE sliding interface | Fluid-structure with sliding |
| TYPE10 | One-way contact | Impact problems, one-way sliding |
| TYPE11 | Edge-to-edge contact | Line-to-line contact |
| TYPE12 | Grid-to-grid interface | Domain decomposition, periodic BC |
| TYPE14 | Nonlinear spring contact | Bump stops, nonlinear springs |
| TYPE15 | Symmetric surface contact | Simple rigid tool contact |
| TYPE17 | Hertz/Lagrange contact | Spherical contact, gear teeth |
| TYPE18 | ALE/Eulerian-Lagrangian | Fluid-solid coupling |
| TYPE19 | Tied symmetry interface | Symmetry plane connections |
| TYPE20 | Symmetric contact with edges | Self-contact with edge treatment |
| TYPE21 | Advanced tied with friction | Metal forming, heat transfer |
| TYPE22 | Fluid-structure coupling | CFD-structural coupling |
| TYPE23 | One-sided contact | Tool-workpiece contact |
| TYPE24 | Contact with friction | General friction contact |
| TYPE25 | Self-contact | Crash self-contact, folding |
| SUB | Sub-interface definition | Local property override |

---

## Interface Categories

### Tied/Glued Interfaces
- **TYPE2**: Spotweld with rupture criteria
- **TYPE5**: Simple tied connection
- **TYPE19**: Tied symmetry plane
- **TYPE21**: Advanced tied with friction and heat

### Deformable Contact
- **TYPE7**: General surface-to-surface
- **TYPE11**: Edge-to-edge
- **TYPE20**: Symmetric with edge treatment
- **TYPE23**: One-sided penalty contact
- **TYPE24**: Friction emphasis
- **TYPE25**: Self-contact

### Rigid Tool Contact
- **TYPE3**: Rigid surface sliding
- **TYPE6**: Rigid-to-rigid
- **TYPE8**: Drawbead
- **TYPE15**: Simple rigid tool

### ALE/Eulerian Interfaces
- **TYPE1**: ALE-Lagrangian coupling
- **TYPE9**: ALE sliding with friction
- **TYPE18**: ALE/Eulerian-Lagrangian

### Special Purpose
- **TYPE10**: One-way impact
- **TYPE12**: Grid-to-grid (domain decomposition)
- **TYPE14**: Nonlinear spring
- **TYPE17**: Hertz/Lagrange multiplier
- **TYPE22**: Fluid-structure CFD

---

## 1. TYPE1 - ALE-Lagrangian Coupling Interface

Defines contact between ALE (Arbitrary Lagrangian-Eulerian) and Lagrangian surfaces.

### Syntax
```
/INTER/TYPE1/inter_ID
inter_title
# surf_IDA  surf_IDL
```

### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| inter_ID | INT | Unique interface identifier |
| inter_title | STRING | Interface description |
| surf_IDA | INT | ALE surface identifier |
| surf_IDL | INT | Lagrangian surface identifier |

### Example
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/INTER/TYPE1/1
Fluid_Structure_Coupling
#surf_IDA surf_IDL
       100       200
```

---

## 2. TYPE2 - Tied Interface (Spotweld)

Node-to-surface tied contact with rupture criteria support. Used for spot welds, adhesive bonds, and mesh connections.

### Syntax
```
/INTER/TYPE2/inter_ID/inter_title
#grnod_ID  surf_IDm   Ignore Spotflag    Level  Isearch    Idel2  surf_IDs          dsearch
# Optional rupture cards when Spotflag = 20, 21, or 22
```

### Parameters - Card 1

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| grnod_ID | INT | - | Secondary node group identifier |
| surf_IDm | INT | - | Main surface identifier |
| Ignore | INT | 0 | Flag for secondary nodes with no main segment |
| Spotflag | INT | 0 | Formulation flag (0-30) |
| Level | INT | - | Interface hierarchy level |
| Isearch | INT | 0 | Search formulation flag |
| Idel2 | INT | 0 | Node deletion flag |
| surf_IDs | INT | - | Secondary surface identifier |
| dsearch | FLOAT | 0.0 | Search distance |

### Ignore Options

| Value | Description |
|-------|-------------|
| 0 | Use /DEFAULT/INTER/TYPE2 value |
| 1 | Delete secondary nodes with no main segment |
| 2 | Delete and recalculate dsearch if dsearch=0 |
| 1000 | No deletion (default if /DEFAULT not defined) |

### Spotflag Options

| Value | Description |
|-------|-------------|
| 0 | Use default value |
| 2 | Standard formulation |
| 4 | Rotational DOF not transmitted (shells only) |
| 20 | Surface from shell and brick faces |
| 21 | Surface from shell only |
| 22 | Surface from brick faces only |
| 25 | Penalty formulation |
| 27 | Kinematic with auto-switch to penalty |
| 30 | Cubic curvature formulation |

### Rupture Parameters (Spotflag = 20, 21, 22)

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| RUPT | INT | 0 | Failure criterion flag |
| Ifiltr | INT | 0 | Stress filtering flag |
| FUNCT_ID_sr | INT | - | Function ID for stress rate |
| FUNCT_ID_sn | INT | - | Function ID for normal stress |
| FUNCT_ID_st | INT | - | Function ID for tangential stress |
| Ismstr | INT | 0 | Asymmetric rupture flag |
| MAX_N_DIST | FLOAT | 1e20 | Maximum normal displacement |
| MAX_T_DIST | FLOAT | 1e20 | Maximum tangential displacement |

### Example
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/INTER/TYPE2/1/Spotweld_Connection
       100       200         0         2         1         2         0       201         0.0
```

---

## 3. TYPE3 - Rigid Surface Contact

Surface-to-surface contact between rigid surfaces with friction.

### Syntax
```
/INTER/TYPE3/inter_ID
inter_title
# surf_ID1  surf_ID2                                                        Idel
#              Stfac                Fric                 Gap              Tstart               Tstop
#      IBC       IRS       IRm
#                                                                          Ptlim
```

### Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| surf_ID1 | INT | - | First surface identifier |
| surf_ID2 | INT | - | Second surface identifier |
| Idel | INT | 0 | Node and segment deletion flag |
| Stfac | FLOAT | 0.2 | Interface stiffness scale factor (must be ≤1.0) |
| Fric | FLOAT | 0.0 | Coulomb friction coefficient |
| Gap | FLOAT | 0.0 | Gap for impact activation |
| Tstart | FLOAT | 0.0 | Start time for contact |
| Tstop | FLOAT | 1e30 | Time for temporary deactivation |
| IBC_x/y/z | INT | 0 | Deactivation flags of boundary conditions |
| IRS | INT | 0 | Renumbering flag for first surface |
| IRm | INT | 0 | Renumbering flag for second surface |
| Ptlim | FLOAT | 1e30 | Maximum tangential pressure |

### Idel Options

| Value | Description |
|-------|-------------|
| 0 | No deletion (default) |
| 1 | Delete segment when all associated elements are deleted |
| 2 | Delete segment when any associated element is deleted |
| -1 | Same as 1, but non-connected nodes not removed |
| -2 | Same as 2, but non-connected nodes not removed |

### Normal Orientation Flags (IRS, IRm)

| Value | Description |
|-------|-------------|
| 0 | Auto-orient normals for solid-connected segments |
| 1 | Always reverse normal |
| 2 | Never reverse normal |

### Example
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/INTER/TYPE3/1
Rigid_Tool_Contact
# surf_ID1  surf_ID2                                                        Idel
       100       200                                                           0
#              Stfac                Fric                 Gap              Tstart               Tstop
                0.2                 0.3                 1.0                 0.0               1e30
#      IBC       IRS       IRm
   000         0         0
#                                                                          Ptlim
                                                                            1e30
```

---

## 4. TYPE5 - Tied/Glued Interface

General tied contact with simpler options than TYPE2.

### Syntax
```
/INTER/TYPE5/inter_ID
inter_title
#grnod_ID  surf_IDm   Ignore  Ifiltr   Level  Isearch     Idel
```

### Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| grnod_ID | INT | - | Secondary node group |
| surf_IDm | INT | - | Main surface |
| Ignore | INT | 0 | Missing segment flag |
| Ifiltr | INT | 0 | Filtering flag |
| Level | INT | - | Hierarchy level |
| Isearch | INT | 0 | Search formulation |
| Idel | INT | 0 | Deletion flag |

### Example
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/INTER/TYPE5/2
Glued_Surfaces
        50       100         0         0         1         2         0
```

---

## 5. TYPE6 - Rigid-to-Rigid Contact

Contact between two rigid surfaces with advanced friction and nonlinear force options.

### Syntax
```
/INTER/TYPE6/inter_ID
inter_title
# surf_ID1  surf_ID2
#              Sfric                Fric                 Gap              Tstart               Tstop
#                IRS       IRM    Inacti  fct_IDff  fct_IDfv             Ascalef             Ascalev
# fct_IDId     Iform             Ascalex            Fscaleid      Icor
# fct_IDul                         Stiff            Fscaleul
#               Visc  fct_IDdv  fct_IDdf             Fscalev
```

### Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| surf_ID1 | INT | - | First rigid surface identifier |
| surf_ID2 | INT | - | Second rigid surface identifier |
| Sfric | FLOAT | 0.0 | Static friction force |
| Fric | FLOAT | 0.0 | Coulomb friction coefficient |
| Gap | FLOAT | 0.0 | Gap for impact activation |
| Tstart | FLOAT | 0.0 | Interface activation time |
| Tstop | FLOAT | 1e30 | Interface deactivation time |
| fct_IDff | INT | - | Friction multiplier vs normal force function |
| fct_IDfv | INT | - | Friction multiplier vs sliding velocity function |
| fct_IDId | INT | - | Force vs penetration curve function |
| Iform | INT | 0 | Contact formulation flag |
| fct_IDul | INT | - | Force vs penetration unload function |
| Stiff | FLOAT | - | Loading/unloading transition stiffness |
| Visc | FLOAT | 0.0 | Damping coefficient |
| fct_IDdv | INT | - | Damping force vs penetration velocity function |
| fct_IDdf | INT | - | Damping multiplier vs normal force function |

### Iform Options

| Value | Description |
|-------|-------------|
| 0 | Nonlinear elastic |
| 1 | Nonlinear elasto-plastic with unloading curve |
| 2 | Nonlinear elasto-plastic with decoupled hardening |

### Example
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/INTER/TYPE6/1
Rigid_Body_Contact
# surf_ID1  surf_ID2
       100       200
#              Sfric                Fric                 Gap              Tstart               Tstop
                 0.0                 0.3                 1.0                 0.0               1e30
#                IRS       IRM    Inacti  fct_IDff  fct_IDfv             Ascalef             Ascalev
                   0         0         0         0         0                 0.0                 0.0
# fct_IDId     Iform             Ascalex            Fscaleid      Icor
         1         1                 1.0               1000.         0
# fct_IDul                         Stiff            Fscaleul
         2                        1000.0               1000.
#               Visc  fct_IDdv  fct_IDdf             Fscalev
                 0.1         0         0                 0.0
```

---

## 6. TYPE7 - General Contact (Deformable-to-Deformable)

Surface-to-surface contact between deformable bodies. The most commonly used interface for crashworthiness.

### Syntax
```
/INTER/TYPE7/inter_ID
inter_title
#surf_IDs  surf_IDm      Istf      Ithe      Igap                Idel  Icurv      Iadm
#Fscalegap    Gapmax      Pmax         Stmin      Stmax
#   Stfac      Fric    Gapmin    Tstart     Tstop
#  Iform     Sens_ID      Inacti      VISs    Bumult  Ifric  Ifiltr
```

### Parameters - Card 1

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| surf_IDs | INT | - | Secondary surface |
| surf_IDm | INT | - | Main surface |
| Istf | INT | 0 | Stiffness definition flag |
| Ithe | INT | 0 | Heat transfer flag |
| Igap | INT | 2 | Gap option flag |
| Idel | INT | 0 | Node deletion flag |
| Icurv | INT | 0 | Curvature correction flag |
| Iadm | INT | 0 | Adaptive meshing flag |

### Igap Options

| Value | Description |
|-------|-------------|
| 0 | Gap is constant (minimum gap) |
| 1 | Gap from secondary node characteristics |
| 2 | Gap varies with thickness changes (default) |

### Parameters - Card 2

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| Fscalegap | FLOAT | 1.0 | Gap scale factor |
| Gapmax | FLOAT | - | Maximum gap |
| Pmax | FLOAT | 1e30 | Maximum contact pressure |
| Stmin | FLOAT | - | Minimum stiffness |
| Stmax | FLOAT | 1e30 | Maximum stiffness |

### Parameters - Card 3

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| Stfac | FLOAT | 0.01 | Stiffness factor |
| Fric | FLOAT | 0.0 | Friction coefficient |
| Gapmin | FLOAT | - | Minimum gap for activation |
| Tstart | FLOAT | 0.0 | Start time |
| Tstop | FLOAT | 1e30 | Stop time |

### Parameters - Card 4

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| Iform | INT | 0 | Formulation flag |
| Sens_ID | INT | 0 | Sensor ID |
| Inacti | INT | 0 | Initial penetration flag |
| VISs | FLOAT | 1.0 | Critical damping coefficient |
| Bumult | FLOAT | 0.20 | Sorting factor |
| Ifric | INT | 0 | Friction formulation flag |
| Ifiltr | INT | 0 | Friction filtering flag |

### Inacti Options

| Value | Description |
|-------|-------------|
| 0 | No action |
| 1 | Deactivate stiffness on penetrating nodes |
| 5 | Variable gap: gap0 = Gap - P0 |
| 6 | Variable gap with 5% depenetration |

### Example
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/INTER/TYPE7/3
General_Contact
        10        20         0         0         2         0         0         0
       1.0      10.0     1e10       100.0     1e10
      0.01       0.3       0.1       0.0     1e30
         0         0         0       1.0      0.2         0         0
```

---

## 7. TYPE8 - Drawbead Contact

Defines drawbead contact for sheet metal forming simulations.

### Syntax
```
/INTER/TYPE8/inter_ID
inter_title
# grnd_IDs  surf_IDm               Iform
#                 FN                  Ft               DEPTH              Tstart               Tstop
#            FN_LAST             Ft_LAST
```

### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| grnd_IDs | INT | Secondary node group identifier |
| surf_IDm | INT | Main surface identifier |
| Iform | INT | Friction penalty formulation type |
| Fn | FLOAT | Drawbead normal force per unit length |
| Ft | FLOAT | Drawbead restraining force per unit length |
| DEPTH | FLOAT | Drawbead depth |
| Tstart | FLOAT | Start time for contact |
| Tstop | FLOAT | Temporary deactivation time |
| Fn_LAST | FLOAT | Normal force per unit length at last node |
| Ft_LAST | FLOAT | Restraining force per unit length at last node |

### Iform Options

| Value | Description |
|-------|-------------|
| 1 | Viscous (total) formulation |
| 2 | Stiffness (incremental) formulation - recommended for low speed |

### Example
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/INTER/TYPE8/1
Drawbead_Contact
# grnd_IDs  surf_IDm               Iform
       100       200                   2
#                 FN                  Ft               DEPTH              Tstart               Tstop
              1000.0               500.0                 5.0                 0.0               1e30
#            FN_LAST             Ft_LAST
               800.0               400.0
```

---

## 8. TYPE9 - ALE Sliding Interface

ALE-Lagrangian contact with thermal bridge and surface tension.

### Syntax
```
/INTER/TYPE9/inter_ID
inter_title
# surf_IDA  surf_IDL
#                RTH                Fric                 Gap
#                ITH      IEUL                        Upwind                  Fs
```

### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| surf_IDA | INT | ALE secondary surface identifier |
| surf_IDL | INT | Lagrangian main surface identifier |
| RTH | FLOAT | Thermal resistance per surface unit |
| Fric | FLOAT | Coulomb friction coefficient |
| Gap | FLOAT | Gap for impact activation |
| ITH | INT | Thermal bridge flag (0=No, 1=Yes) |
| IEUL | INT | Eulerian behavior in tangent direction |
| Upwind | FLOAT | Upwind for free surface normal (0.0 or 1.0) |
| Fs | FLOAT | Surface tension force |

### IEUL Options

| Value | Description |
|-------|-------------|
| 0 | Grid velocity is not set |
| 1 | Grid velocity set to zero in tangent direction for non-impacted nodes |

### Example
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/INTER/TYPE9/1
ALE_Sliding_Interface
# surf_IDA  surf_IDL
       100       200
#                RTH                Fric                 Gap
               0.001                 0.1                 1.0
#                ITH      IEUL                        Upwind                  Fs
                   1         0                           0.0                 0.0
```

---

## 9. TYPE10 - One-Way Contact

One-way sliding contact between node group and surface.

### Syntax
```
/INTER/TYPE10/inter_ID
inter_title
# grnod_ID   surf_ID                                                        Idel
#              STFAC                                     GAP              Tstart               Tstop
#                        ITIED    INACTI               VIS_S                                  BUMULT
```

### Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| grnod_ID | INT | - | Secondary node group identifier |
| surf_ID | INT | - | Main surface identifier |
| Idel | INT | 0 | Node and segment deletion flag |
| STFAC | FLOAT | 0.2 | Interface stiffness scale factor |
| GAP | FLOAT | 0.0 | Gap for impact activation |
| Tstart | FLOAT | 0.0 | Start time |
| Tstop | FLOAT | 1e30 | Temporary deactivation time |
| ITIED | INT | 0 | Tied option flag |
| INACTI | INT | 0 | Deactivation of stiffness flag |
| VIS_S | FLOAT | 0.05 | Critical damping coefficient |
| BUMULT | FLOAT | 0.20 | Sorting factor |

### ITIED Options

| Value | Description |
|-------|-------------|
| 0 | Secondary node is tied during impact with possible rebound |
| 1 | Secondary node is tied after impact without possible rebound |

### INACTI Options

| Value | Description |
|-------|-------------|
| 0 | No action |
| 1 | Deactivation of stiffness on nodes |
| 2 | Deactivation of stiffness on elements |
| 3 | Change node coordinates to avoid initial penetrations |

### Example
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/INTER/TYPE10/1
One_Way_Contact
# grnod_ID   surf_ID                                                        Idel
       100       200                                                           0
#              STFAC                                     GAP              Tstart               Tstop
                 0.2                                     1.0                 0.0               1e30
#                        ITIED    INACTI               VIS_S                                  BUMULT
                             0         0                0.05                                     0.2
```

---

## 10. TYPE11 - Edge-to-Edge Contact

Edge-to-edge contact between deformable bodies.

### Syntax
```
/INTER/TYPE11/inter_ID
inter_title
#grline_IDs grline_IDm   Ignore    Stfac      Fric    Gapmin    Gapmax
```

### Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| grline_IDs | INT | - | Secondary line group |
| grline_IDm | INT | - | Main line group |
| Ignore | INT | 0 | Missing segment flag |
| Stfac | FLOAT | 0.01 | Stiffness factor |
| Fric | FLOAT | 0.0 | Friction coefficient |
| Gapmin | FLOAT | - | Minimum gap |
| Gapmax | FLOAT | - | Maximum gap |

### Example
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/INTER/TYPE11/4
Edge_Contact
        30        40         0      0.01       0.2       0.5       5.0
```

---

## 11. TYPE12 - Grid-to-Grid Interface

Grid-to-grid interface for domain decomposition and periodic boundary conditions.

### Syntax
```
/INTER/TYPE12/inter_ID
inter_title
# surf_IDs  surf_IDm            Interpol
#                                                        Tol              Tstart               Tstop
#    ITIED     Bcopt   skew_ID   node_ID
# Periodic transformation cards (if ITIED=2)
```

### Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| surf_IDs | INT | - | Secondary surface identifier |
| surf_IDm | INT | - | Main surface identifier |
| Interpol | INT | 0 | Interpolation flag (0=Linear, 1=Polar) |
| Tol | FLOAT | 0.02 | Tolerance for segment search |
| Tstart | FLOAT | 0.0 | Start time |
| Tstop | FLOAT | 1e30 | Temporary deactivation time |
| ITIED | INT | 0 | Option for surface connection |
| Bcopt | INT | 0 | Kinematic constraint deactivation flag |
| skew_ID | INT | - | Skew system identifier for polar interpolation |
| node_ID | INT | - | Reference node for polar interpolation |

### ITIED Options

| Value | Description |
|-------|-------------|
| 0 | Free |
| 1 | Tied |
| 2 | Periodic |
| 3 | No convection |

### Bcopt Options

| Value | Description |
|-------|-------------|
| 0 | Default |
| 1 | All nodes considered |
| 2 | Secondary nodes will be omitted |
| 3 | Secondary nodes and fully fixed nodes will be omitted |

### Periodic Transformation (ITIED=2)

| Parameter | Type | Description |
|-----------|------|-------------|
| Xc, Yc, Zc | FLOAT | Center of rotation coordinates |
| XN, YN, ZN | FLOAT | Rotation axis vector components |
| THETA | FLOAT | Angle of rotation |
| XT, YT, ZT | FLOAT | Translation vector components |

### Example
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/INTER/TYPE12/1
Periodic_Interface
# surf_IDs  surf_IDm            Interpol
       100       200                   1
#                                                        Tol
                                                        0.02
#    ITIED     Bcopt   skew_ID   node_ID
         2         0         1       500
#                 XC                  YC                  ZC
                 0.0                 0.0                 0.0
#                 XR                  YR                  ZR               THETA
                 0.0                 0.0                 1.0                60.0
#                 XT                  YT                  ZT
                 0.0                 0.0                 0.0
```

---

## 12. TYPE14 - Nonlinear Spring Contact

Contact with nonlinear spring characteristics.

### Syntax
```
/INTER/TYPE14/inter_ID
inter_title
# grnd_IDs  surf_IDm  fct_IDld   fct_IDf  fct_IDd1  fct_IDd2
#               Stif                Fric                Visc                 Gap
```

### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| grnd_IDs | INT | Secondary node group identifier |
| surf_IDm | INT | Main surface identifier |
| fct_IDld | INT | Elastic force vs penetration function |
| fct_IDf | INT | Friction coefficient vs elastic force function |
| fct_IDd1 | INT | Damping coefficient vs normal velocity function |
| fct_IDd2 | INT | Damping coefficient vs elastic force function |
| Stif | FLOAT | Interface stiffness |
| Fric | FLOAT | Friction coefficient |
| Visc | FLOAT | Normal viscosity coefficient |
| Gap | FLOAT | Gap for impact activation |

### Example
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/INTER/TYPE14/1
Bump_Stop_Contact
# grnd_IDs  surf_IDm  fct_IDld   fct_IDf  fct_IDd1  fct_IDd2
       100       200         1         2         3         4
#               Stif                Fric                Visc                 Gap
              1000.0                 0.3               100.0                 5.0
```

---

## 13. TYPE15 - Symmetric Surface Contact

Simple contact between two surfaces.

### Syntax
```
/INTER/TYPE15/inter_ID
inter_title
# surf_IDs  surf_IDm
#               Stif                Fric
```

### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| surf_IDs | INT | Secondary surface identifier |
| surf_IDm | INT | Main surface identifier |
| Stif | FLOAT | Stiffness factor |
| Fric | FLOAT | Friction coefficient |

### Example
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/INTER/TYPE15/1
Simple_Tool_Contact
# surf_IDs  surf_IDm
       100       200
#               Stif                Fric
              1000.0                 0.2
```

---

## 14. TYPE18 - ALE/Eulerian-Lagrangian Contact

Contact between ALE/Eulerian material and Lagrangian structure.

### Syntax
```
/INTER/TYPE18/inter_ID
inter_title
#            surf_ID grbric_ID                Igap               Ipres      Idel                Iauto
#             Stfval                Vref                 Gap              Tstart               Tstop
```

### Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| surf_ID | INT | - | Lagrangian surface identifier |
| grbric_ID | INT | - | Eulerian/ALE brick element group identifier |
| Igap | INT | 0 | Gap formulation flag |
| Ipres | INT | 0 | Pressure correction flag |
| Idel | INT | 0 | Node and segment deletion flag |
| Iauto | INT | 0 | Automatic stiffness formulation flag |
| Stfval | FLOAT | - | Interface stiffness value |
| Vref | FLOAT | 0.0 | Reference velocity |
| Gap | FLOAT | - | Interface gap |
| Tstart | FLOAT | 0.0 | Start time |
| Tstop | FLOAT | 1e30 | Temporary deactivation time |

### Igap Options

| Value | Description |
|-------|-------------|
| 0 | No pressure correction |
| 1 | Reduce pressure by median pressure |
| 2 | Reduce pressure by mean surface pressure |
| 3 | Reduce pressure by mean estimated volumetric pressure |

### Ipres (Ibag) Options

| Value | Description |
|-------|-------------|
| 0 | No pressure correction |
| 1 | Reduce pressure by median pressure |
| 2 | Reduce pressure by mean surface pressure |
| 3 | Reduce pressure by mean estimated volumetric pressure |

### Iauto Options

| Value | Description |
|-------|-------------|
| 0 | Stiffness value from Stfval |
| 1 | Automatic stiffness calculation from reference velocity |

### Example
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/INTER/TYPE18/1
Fluid_Structure_Contact
#            surf_ID grbric_ID                Igap               Ipres      Idel                Iauto
                 100       200                   0                   0         0                   0
#             Stfval                Vref                 Gap              Tstart               Tstop
            100000.0                 0.0                 1.0                 0.0               1e30
```

---

## 15. TYPE19 - Tied Symmetry Interface

Symmetric tied interface for symmetry plane connections.

### Syntax
```
/INTER/TYPE19/inter_ID
inter_title
#grnod_ID  surf_IDm   Ignore    Level  Isearch     dsearch
```

### Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| grnod_ID | INT | - | Node group |
| surf_IDm | INT | - | Symmetry surface |
| Ignore | INT | 0 | Missing segment flag |
| Level | INT | - | Hierarchy level |
| Isearch | INT | 0 | Search flag |
| dsearch | FLOAT | 0.0 | Search distance |

### Example
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/INTER/TYPE19/5
Symmetry_Plane
        50       100         0         1         2       1.0
```

---

## 16. TYPE20 - Symmetric Contact with Edges

Symmetric surface contact with edge-to-edge treatment.

### Syntax
```
/INTER/TYPE20/inter_ID
inter_title
# surf_ID1  surf_ID2      Isym     Iedge  grnod_ID  line_ID1  line_ID2                    edge_angle
#                                             Igap                Ibag      Idel
#                                                    Fpenmax
# Blank card
#              Stfac                Fric              Gap0                Tstart               Tstop
#      IBC                        Inacti                VISs                VISf
#    Ifric    Ifiltr               Xfreq     Iform
# C1-C6 cards if Ifric > 0
```

### Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| surf_ID1 | INT | - | First surface identifier |
| surf_ID2 | INT | - | Second surface identifier |
| Isym | INT | 0 | Symmetric contact treatment flag |
| Iedge | INT | 0 | Edge definition flag |
| grnod_ID | INT | - | Additional node group |
| line_ID1 | INT | - | First line identifier |
| line_ID2 | INT | - | Second line identifier |
| edge_angle | FLOAT | 91.0 | Edge angle threshold (degrees) |
| Igap | INT | 0 | Gap/element option flag |
| Ibag | INT | 0 | Airbag vent hole closure flag |
| Idel | INT | 0 | Node and segment deletion flag |
| Fpenmax | FLOAT | 1.0 | Maximum initial penetration factor |
| Stfac | FLOAT | 1.0 | Interface stiffness scale factor |
| Fric | FLOAT | 0.0 | Coulomb friction |
| Gap0 | FLOAT | 0.0 | Minimum gap |
| VISs | FLOAT | 0.05 | Critical damping on stiffness |
| VISf | FLOAT | 1.0 | Critical damping on friction |
| Ifric | INT | 0 | Friction formulation flag |
| Ifiltr | INT | 0 | Friction filtering flag |
| Iform | INT | 2 | Friction penalty formulation type |

### Isym Options

| Value | Description |
|-------|-------------|
| 0 | Set to 1 |
| 1 | Nodes from surfaces 1, 2, and grnd_ID impact surfaces 1 and 2 |
| 2 | Nodes from surface 2 and grnd_ID impact surface 1 |

### Iedge Options

| Value | Description |
|-------|-------------|
| 0 | No edge generation |
| 1 | Only external border edges are generated |
| 2 | All segment edges are generated |
| 3 | Same as 1 + sharp edges between segments |

### Example
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/INTER/TYPE20/1
Symmetric_Self_Contact
# surf_ID1  surf_ID2      Isym     Iedge  grnod_ID  line_ID1  line_ID2                    edge_angle
       100       100         1         1         0         0         0                          91.0
#                                             Igap                Ibag      Idel
                                                 1                   0         0
#                                                    Fpenmax
                                                        1.0
#
#              Stfac                Fric              Gap0                Tstart               Tstop
                 1.0                 0.3                 0.5                 0.0               1e30
#      IBC                        Inacti                VISs                VISf
   000                               0                0.05                 1.0
#    Ifric    Ifiltr               Xfreq     Iform
         0         0                 0.0         2
```

---

## 17. TYPE21 - Advanced Tied with Friction and Heat

Advanced tied interface with friction models, heat transfer, adaptive meshing, and rigid body support.

### Syntax
```
/INTER/TYPE21/inter_ID
inter_title
#surf_IDs  surf_IDm      Istf      Ithe      Igap                Idel      Invn      Iadm
#Fscalegap    Gapmax     DEPTH      Pmax     ITlim
#   Stmin     Stmax               Pskid
#   Stfac      Fric    Gapmin    Tstart     Tstop
#   IBC_x     IBC_y     IBC_z    Inacti      VISs    Bumult
#   Ifric    Ifiltr     Xfreq            sens_ID  fct_IDF  AscaleF  fric_ID
#      C1        C2        C3        C4        C5
#      C6
#   NRadm      Padm   Angladm
#    Kthe  fct_IDK   AscaleK      Tint Ithe_form
#    Frad      Drad     Fheat            F_COND    D_COND
#   IDrby     IDref      Damp     Dampr
```

### Parameters - Surface Definition

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| surf_IDs | INT | - | Secondary surface |
| surf_IDm | INT | - | Main surface |
| Istf | INT | 0 | Stiffness definition flag |
| Ithe | INT | 0 | Heat transfer flag (0-2) |
| Igap | INT | 2 | Gap option flag |
| Idel | INT | 0 | Deletion flag |
| Invn | INT | 0 | Normal orientation check flag |
| Iadm | INT | 2 | Adaptive meshing flag |

### Friction Models (Ifric)

| Value | Description |
|-------|-------------|
| 0 | Static Coulomb friction (default) |
| 1 | Generalized viscous friction law |
| 2 | Modified Darmstad friction law |
| 3 | Renard friction law |
| 4 | Exponential decay law |

### Friction Coefficients (C1-C6)

Different friction laws use these coefficients differently:
- Ifric=1: Viscous (pressure and velocity dependent)
- Ifric=2: Darmstad (pressure-squared dependent)
- Ifric=3: Renard (advanced model)
- Ifric=4: Exponential decay

### Adaptive Meshing Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| NRadm | INT | 3 | Elements through 90° radius |
| Padm | FLOAT | 1.0 | Penetration percentage criteria |
| Angladm | FLOAT | 30.0 | Angle criteria (degrees) |

### Heat Transfer Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| Kthe | FLOAT | 0.0 | Heat exchange coefficient |
| fct_IDK | INT | - | Function for heat vs pressure |
| Tint | FLOAT | - | Interface temperature |
| Ithe_form | INT | 0 | Heat formulation flag |
| Frad | FLOAT | 1.0 | Radiation factor |
| Drad | FLOAT | 1.0 | Maximum radiation distance |
| Fheat | FLOAT | - | Frictional heating factor |

### Rigid Body Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| IDrby | INT | - | Rigid body ID |
| IDref | INT | - | Reference interface ID |
| Damp | FLOAT | - | Translational damping factor |
| Dampr | FLOAT | - | Rotational damping factor |

### Example
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/INTER/TYPE21/1
Metal_Forming_Contact
        50       100         0         1         2         0         0         2
       1.0      50.0      10.0    1000.0         0
     100.0    1000.0              500.0
       1.0       0.3       0.1       0.0    1000.0
         0         0         0         0       1.0       0.2
         1         0       0.5              0         0       1.0         0
       0.0       0.0       0.0       0.0       0.0
       0.0
         3       1.0      30.0
     100.0         0       1.0     300.0         0
       1.0       1.0       0.1              0         0
         0         0      0.05      0.05
```

---

## 18. TYPE22 - Fluid-Structure Coupling

Fluid-structure coupling interface for CFD applications.

### Syntax
```
/INTER/TYPE22/inter_ID
inter_title
#grbric_ID   surf_ID
# Blank card
```

### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| grbric_ID | INT | Fluid domain (brick element) group identifier |
| surf_ID | INT | Lagrangian surface identifier |

### Example
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/INTER/TYPE22/1
CFD_Structure_Interface
#grbric_ID   surf_ID
       100       200

```

---

## 19. TYPE23 - One-Sided Contact

One-sided penalty contact similar to TYPE7 but with single-side formulation.

### Syntax
```
/INTER/TYPE23/inter_ID
inter_title
# surf_IDs  surf_IDm      Istf                Igap                Ibag      Idel
#          Fscalegap             Gap_max             Fpenmax
#              Stmin               Stmax
#              Stfac                Fric              Gapmin              Tstart               Tstop
#      IBC                        Inacti                VISs                                  Bumult
#    Ifric    Ifiltr               Xfreq
# C1-C6 cards if Ifric > 0
```

### Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| surf_IDs | INT | - | Secondary surface |
| surf_IDm | INT | - | Main surface |
| Istf | INT | 0 | Stiffness definition flag |
| Igap | INT | 0 | Gap/element option flag |
| Ibag | INT | 0 | Airbag vent hole closure flag |
| Idel | INT | 0 | Node deletion flag |
| Fscalegap | FLOAT | 1.0 | Gap scale factor |
| Gapmax | FLOAT | - | Maximum gap |
| Fpenmax | FLOAT | - | Maximum fraction of initial penetration |
| Stmin | FLOAT | - | Minimum stiffness |
| Stmax | FLOAT | 1e30 | Maximum stiffness |
| Stfac | FLOAT | 0.0 | Stiffness factor |
| Fric | FLOAT | 0.0 | Friction coefficient |
| Gapmin | FLOAT | - | Minimum gap for activation |
| VISs | FLOAT | 1.0 | Critical damping coefficient |
| Bumult | FLOAT | 0.20 | Sorting factor |
| Ifric | INT | 0 | Friction formulation flag |
| Ifiltr | INT | 0 | Friction filtering flag |
| Xfreq | FLOAT | 0.0 | Filtering coefficient |

### Example
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/INTER/TYPE23/1
One_Sided_Contact
# surf_IDs  surf_IDm      Istf                Igap                Ibag      Idel
       100       200         0                   1                   0         0
#          Fscalegap             Gap_max             Fpenmax
                 1.0                10.0                 1.0
#              Stmin               Stmax
              100.0               1e30
#              Stfac                Fric              Gapmin              Tstart               Tstop
                0.01                 0.3                 0.5                 0.0               1e30
#      IBC                        Inacti                VISs                                  Bumult
   000                               0                 1.0                                     0.2
#    Ifric    Ifiltr               Xfreq
         0         0                 0.0
```

---

## 20. TYPE24 - Contact with Friction

Surface contact with friction emphasis (similar to TYPE7).

### Syntax
```
/INTER/TYPE24/inter_ID
inter_title
#surf_IDs  surf_IDm      Istf    Gapmax      Fric    Gapmin    Stfac
```

### Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| surf_IDs | INT | - | Secondary surface |
| surf_IDm | INT | - | Main surface |
| Istf | INT | 0 | Stiffness definition flag |
| Gapmax | FLOAT | - | Maximum gap |
| Fric | FLOAT | 0.0 | Friction coefficient |
| Gapmin | FLOAT | - | Minimum gap |
| Stfac | FLOAT | 0.01 | Stiffness factor |

### Example
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/INTER/TYPE24/6
Friction_Contact
        60        70         0      10.0       0.4       0.5      0.01
```

---

## 21. TYPE25 - Self-Contact

Advanced self-contact for crash simulations with folding and buckling.

### Syntax
```
/INTER/TYPE25/inter_ID
inter_title
# surf_ID1  surf_ID2      Istf      Ithe      Igap   Irem_i2                Idel     Iedge
# grnd_IDs                     Gap_scale          %mesh_size           Gap_max_s           Gap_max_m
#              Stmin               Stmax     Igap0    Ishape          Edge_angle
#              Stfac                Fric                                  Tstart               Tstop
#      IBC               IVIS2    Inacti                VISs    Ithick                          Pmax
#    Ifric    Ifiltr               Xfreq             sens_ID                                 fric_ID
# C1-C6 cards if Ifric > 0
# ViscFluid/SigMaxAdh/ViscAdhFact card if IVIS2 = -1
# Heat transfer cards if Ithe > 0
```

### Parameters - Surface Definition

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| surf_ID1 | INT | - | First surface identifier |
| surf_ID2 | INT | - | Second surface identifier |
| Istf | INT | 0 | Interface stiffness definition flag |
| Ithe | INT | 0 | Heat contact flag |
| Igap | INT | 0 | Gap/element option flag |
| Irem_i2 | INT | 0 | TYPE2 node deactivation flag |
| Idel | INT | 0 | Node and segment deletion flag |
| Iedge | INT | 0 | Edge contact options |

### Istf Options

| Value | Description |
|-------|-------------|
| 0 | Use /DEFAULT/INTER/TYPE25 value |
| 2 | Average of main and secondary stiffness |
| 3 | Maximum of main and secondary stiffness |
| 4 | Minimum of main and secondary stiffness |
| 5 | Main and secondary stiffness in series |
| 1000 | Main side stiffness only (default) |

### Igap Options

| Value | Description |
|-------|-------------|
| 0 | Use /DEFAULT/INTER/TYPE25 value |
| 1 | Variable gap (default) |
| 2 | Variable gap with node deactivation for small elements |
| 3 | Variable gap considering mesh size |

### Iedge Options

| Value | Description |
|-------|-------------|
| 0 | Use /DEFAULT/INTER/TYPE25 value |
| 1 | External border edges of shell segments |
| 11 | Sharp edges of solid + shell border edges |
| 13 | Sharp edges + all shell edges |
| 22 | All edges from solids and shells |
| 1000 | No edge-to-edge contact (default) |

### Parameters - Gap Control

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| grnd_IDs | INT | - | Optional node group identifier |
| Gap_scale | FLOAT | 1.0 | Gap scale factor |
| %mesh_size | FLOAT | 0.4 | Percentage of mesh size (Igap=3) |
| Gap_max_s | FLOAT | 1e30 | Secondary maximum gap |
| Gap_max_m | FLOAT | 1e30 | Main maximum gap |
| Stmin | FLOAT | - | Minimum stiffness |
| Stmax | FLOAT | 1e30 | Maximum stiffness |
| Igap0 | INT | 0 | Gap modification flag for shell edges |
| Ishape | INT | 0 | Gap shape flag (1=square, 2=round) |
| Edge_angle | FLOAT | 135.0 | Edge angle threshold (degrees) |

### Parameters - Contact Properties

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| Stfac | FLOAT | 1.0 | Interface stiffness scale factor |
| Fric | FLOAT | 0.0 | Coulomb friction coefficient |
| Tstart | FLOAT | 0.0 | Start time |
| Tstop | FLOAT | 1e30 | Temporary deactivation time |
| IVIS2 | INT | 0 | Interface adhesion flag |
| Inacti | INT | 0 | Initial penetration flag |
| VISs | FLOAT | 0.05 | Critical damping coefficient |
| Ithick | INT | 0 | Gap variation with thickness flag |
| Pmax | FLOAT | 1e30 | Maximum contact pressure |
| sens_ID | INT | - | Sensor identifier |
| fric_ID | INT | - | Friction definition identifier |

### Adhesion Parameters (IVIS2 = -1)

| Parameter | Type | Description |
|-----------|------|-------------|
| ViscFluid | FLOAT | Viscosity of interface fluid |
| SigMaxAdh | FLOAT | Maximum transverse adhesive stress |
| ViscAdhFact | FLOAT | Tangential viscous resistive force factor |

### Heat Transfer Parameters (Ithe > 0)

| Parameter | Type | Description |
|-----------|------|-------------|
| Kthe | FLOAT | Heat exchange coefficient |
| fct_IDK | INT | Function for heat vs pressure |
| Tint | FLOAT | Interface temperature |
| Ithe_form | INT | Heat formulation flag |
| Frad | FLOAT | Radiation factor |
| Drad | FLOAT | Maximum radiation distance |
| Fheats | FLOAT | Secondary frictional heating factor |
| Fheatm | FLOAT | Main frictional heating factor |
| FRIC_FUN | INT | Friction vs temperature function |
| Fcond | INT | Conductive heat exchange function |
| Dcond | FLOAT | Maximum conduction distance |

### Example
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/INTER/TYPE25/1
Crash_Self_Contact
# surf_ID1  surf_ID2      Istf      Ithe      Igap   Irem_i2                Idel     Iedge
       100       100      1000         0         1         1                   0         1
# grnd_IDs                     Gap_scale          %mesh_size           Gap_max_s           Gap_max_m
         0                           1.0                 0.4               100.0               100.0
#              Stmin               Stmax     Igap0    Ishape          Edge_angle
                 0.0               1e30      1000         1               135.0
#              Stfac                Fric                                  Tstart               Tstop
                 1.0                 0.2                                     0.0               1e30
#      IBC               IVIS2    Inacti                VISs    Ithick                          Pmax
   000                    0      1000                0.05      1000                          1e30
#    Ifric    Ifiltr               Xfreq             sens_ID                                 fric_ID
         0         0                 1.0                   0                                       0
```

---

## 22. SUB - Sub-Interface Definition

Defines a sub-interface to override local contact properties.

### Syntax
```
/INTER/SUB/sub_ID
sub_inter_title
# inter_ID  Main_ID1 Second_ID  Main_ID2
```

### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| sub_ID | INT | Unique sub-interface identifier |
| sub_inter_title | STRING | Sub-interface description |
| inter_ID | INT | Parent interface identifier |
| Main_ID1 | INT | First main surface/line identifier |
| Second_ID | INT | Secondary node group/surface/line identifier |
| Main_ID2 | INT | Second main surface identifier |

### Example
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/INTER/SUB/1
Local_Friction_Override
# inter_ID  Main_ID1 Second_ID  Main_ID2
         1       100       200       101
```

---

## Friction Models

### Static Coulomb Friction (Ifric=0)

The default friction model:
```
Ft = Fric * Fn
```

### Generalized Viscous Friction (Ifric=1)

Pressure and velocity dependent:
```
mu = Fric / (1 + C1*P + C2*V + C3*P*V + C4*P² + C5*V²)
```

### Modified Darmstad Friction (Ifric=2)

Pressure-squared dependency:
```
mu = Fric * (1 - C1*P + C2*V) / (1 + C3*P² + C4*V + C5*V²)
```
With C6 for additional velocity dependency.

### Renard Friction (Ifric=3)

Advanced friction law:
```
mu = Fric * (C1 + C2*(1 - exp(-C3*V))) * (1 - C4*(1 - exp(-C5*P)))
```

### Exponential Decay Friction (Ifric=4)

Static to dynamic transition:
```
mu = C1 + (Fric - C1) * exp(-C2*V)
```

---

## Friction Filtering

### Filter Options (Ifiltr)

| Value | Description |
|-------|-------------|
| 0 | No filter (default) |
| 1 | Simple numerical filter |
| 2 | Standard -3dB filter with filtering period |
| 3 | Standard -3dB filter with cutting frequency |

The filtering coefficient `Xfreq` specifies:
- For Ifiltr=2: Filtering period
- For Ifiltr=3: Cutting frequency

---

## Default Interface Settings

### /DEFAULT/INTER/TYPE2
```
/DEFAULT/INTER/TYPE2
#Ignore Spotflag  Isearch    Idel2
```

### /DEFAULT/INTER/TYPE7
```
/DEFAULT/INTER/TYPE7
#  Istf      Igap      Idel    Icurv     Iadm
```

### /DEFAULT/INTER/TYPE25
```
/DEFAULT/INTER/TYPE25
#  Istf      Igap   Irem_i2      Idel     Iedge
```

---

## Interface Type Selection Guide

### By Application

| Application | Recommended Type |
|-------------|------------------|
| General crash contact | TYPE7, TYPE25 |
| Self-contact (folding) | TYPE25, TYPE20 |
| Spot welds | TYPE2 |
| Adhesive bonds | TYPE2, TYPE5 |
| Sheet metal forming | TYPE7, TYPE21 |
| Drawbeads | TYPE8 |
| Rigid tool contact | TYPE3, TYPE6, TYPE15 |
| Fluid-structure | TYPE1, TYPE9, TYPE18, TYPE22 |
| Domain decomposition | TYPE12 |
| Edge contact | TYPE11 |
| Symmetry plane | TYPE19 |

### By Contact Type

| Contact Type | Interface Types |
|--------------|-----------------|
| Tied | TYPE2, TYPE5, TYPE19, TYPE21 |
| Sliding with friction | TYPE3, TYPE7, TYPE20, TYPE21, TYPE23, TYPE24, TYPE25 |
| One-way | TYPE10, TYPE23 |
| Heat transfer | TYPE7, TYPE21, TYPE25 |
| Adaptive meshing | TYPE21 |

---

## Interface Type Comparison

| Feature | TYPE2 | TYPE7 | TYPE21 | TYPE25 |
|---------|-------|-------|--------|--------|
| Tied Connection | Yes | No | Yes | No |
| Sliding Contact | No | Yes | Yes | Yes |
| Friction | No | Yes | Yes (5 models) | Yes (4 models) |
| Heat Transfer | Basic | Basic | Advanced | Advanced |
| Adaptive Meshing | No | No | Yes | No |
| Rigid Body | No | No | Yes | No |
| Rupture Criteria | Yes | No | No | No |
| Self-Contact | No | Possible | Possible | Optimized |
| Edge Treatment | No | No | No | Yes |

---

## Best Practices

### 1. Type Selection

- **TYPE7**: Standard choice for deformable contact
- **TYPE25**: Best for self-contact and crash simulations
- **TYPE2**: Simple tied connections and spot welds
- **TYPE21**: Metal forming with complex friction and heat

### 2. Stiffness Settings

- Default Stfac=0.01 suitable for most cases
- Increase for stiffer contact (0.05-0.1)
- Use Stmin/Stmax to bound stiffness
- For soft materials, reduce stiffness factor

### 3. Gap Settings

- Igap=2 recommended for shell thickness changes
- Set appropriate Gapmax to limit search distance
- Gap should be ~0.5-1.0 times minimum element size
- For self-contact, Gap_scale=1.0 is typical

### 4. Friction

- Ifric=0 (Coulomb) for most applications
- Use advanced models (Ifric=1-4) for metal forming
- Enable friction filtering for high-frequency oscillations

### 5. Initial Penetration

- Inacti=0: No action (may cause instability)
- Inacti=1 or 2: Deactivate penetrating nodes/elements
- Inacti=5 or 6: Adjust gap for initial penetration

### 6. Performance Optimization

- Use Bumult=0.2-0.3 for typical models
- Limit Gapmax to reduce search overhead
- Consider symmetry to reduce contact pairs
- Use TYPE25 Igap=2 for self-contact optimization

---

## Troubleshooting

### Common Issues

| Issue | Cause | Solution |
|-------|-------|----------|
| Excessive penetration | Stiffness too low | Increase Stfac or Stmin |
| Instability | Stiffness too high | Reduce Stfac or increase damping |
| Nodes passing through | Gap too small | Increase Gap or Gapmax |
| Slow runtime | Large search distance | Reduce Gapmax, use Bumult |
| Initial penetration | Poor mesh alignment | Use Inacti=5 or 6 |
| No contact detected | Gap too large | Reduce Gap, check surface normals |
| Friction issues | High-frequency oscillation | Enable Ifiltr=2 or 3 |
| Heat transfer problems | Wrong Ithe setting | Check Ithe and Kthe values |

### Validation Checks

1. Check surface normals point outward
2. Verify gap is appropriate for mesh size
3. Monitor contact force output
4. Check for hourglassing in contact zone
5. Verify stiffness is appropriate for material

---

## References

- Source files: `hm_cfg_files/config/CFG/radioss*/INTER/`
- Starter source: `starter/source/interfaces/`
