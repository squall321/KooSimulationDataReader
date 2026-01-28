# OpenRadioss Keyword Manual - Properties (Complete Reference)

## Overview

This document provides comprehensive documentation for all element property definitions in OpenRadioss. Properties define element-specific parameters such as formulation type, thickness, integration points, hourglass control, and numerical damping. Each property type is designed for specific element types and applications.

---

## Property Keyword Syntax

```
/PROP/prop_type/prop_ID
prop_title
[property parameters...]
```

Or using TYPE number:
```
/PROP/TYPEnn/prop_ID
prop_title
[property parameters...]
```

---

## Complete Property Type Reference

| Type | Keyword | Application |
|------|---------|-------------|
| TYPE0 | VOID | Void/placeholder property |
| TYPE1 | SHELL | Standard shell elements |
| TYPE2 | TRUSS | Truss elements |
| TYPE3 | BEAM | Beam elements |
| TYPE4 | SPRING | General spring elements |
| TYPE5 | RIVET | Rivet connections |
| TYPE6 | SOL_ORTH | Orthotropic solid elements |
| TYPE8 | SPR_GENE | General spring with 6 DOF |
| TYPE9 | SH_ORTH | Orthotropic shell elements |
| TYPE10 | SH_COMP | Composite shell elements |
| TYPE11 | SH_SANDW | Sandwich shell elements |
| TYPE12 | SPR_PUL | Pulley spring elements |
| TYPE13 | SPR_BEAM | Spring-beam elements |
| TYPE14 | SOLID | General solid elements |
| TYPE15 | POROUS | Porous material property |
| TYPE16 | SH_FABR | Fabric shell elements |
| TYPE17 | STACK | Stacked shell composite |
| TYPE18 | INT_BEAM | Integrated beam |
| TYPE20 | TSHELL | Thick shell elements |
| TYPE21 | TSH_ORTH | Thick shell orthotropic |
| TYPE22 | TSH_COMP | Thick shell composite |
| TYPE25 | SPR_AXI | Axisymmetric spring |
| TYPE26 | SPR_TAB | Tabulated spring |
| TYPE28 | NSTRAND | N-strand cable |
| TYPE29 | - | Spring type 29 |
| TYPE30 | - | Spring type 30 |
| TYPE31 | - | Spring type 31 |
| TYPE32 | SPR_PRE | Pre-tensioned spring |
| TYPE33 | KJOINT | K-joint connection |
| TYPE34 | SPH | SPH particle property |
| TYPE35 | STITCH | Stitching property |
| TYPE36 | PREDIT | Pre-integration beam |
| TYPE43 | CONNECT | Connector property |
| TYPE51 | COMPOSITE | Layered composite |

---

## 1. TYPE1 - SHELL (Standard Shell Property)

Defines properties for 4-node (Q4) and 3-node (T3) shell elements.

### Syntax
```
/PROP/SHELL/prop_ID
prop_title
#   Ishell    Ismstr     Ish3n    Idrill    Ipinch                                P_thick
#                 Hm                  Hf                  Hr                  Dm                  Dn
#        N   Istrain               Thick              Ashear              Ithick     Iplas      Ipos
```

### Parameters - Card 1 (Formulation Flags)

| Parameter | Type | Range | Default | Description |
|-----------|------|-------|---------|-------------|
| Ishell | INT | 0-24 | 0 | Shell element formulation flag |
| Ismstr | INT | 0-10 | 0 | Small strain formulation flag |
| Ish3n | INT | 0-31 | 0 | 3-node shell formulation flag |
| Idrill | INT | 0-2 | 0 | Drilling DOF stiffness flag |
| Ipinch | INT | 0-1 | 0 | Pinching DOF flag |
| P_thick | FLOAT | 0-1 | 0.0 | Thickness failure percentage (0=deactivated) |

### Parameters - Card 2 (Hourglass Control)

| Parameter | Type | Range | Default | Unit | Description |
|-----------|------|-------|---------|------|-------------|
| Hm | FLOAT | 0-0.05 | 0.01 | - | Membrane hourglass coefficient |
| Hf | FLOAT | 0-0.05 | 0.01 | - | Out-of-plane hourglass coefficient |
| Hr | FLOAT | 0-0.05 | 0.01 | - | Rotation hourglass coefficient |
| Dm | FLOAT | ≥0 | 0.0 | [t]⁻¹ | Membrane damping coefficient |
| Dn | FLOAT | ≥0 | 0.0 | [t]⁻¹ | Numerical damping coefficient |

### Parameters - Card 3 (Integration)

| Parameter | Type | Range | Default | Description |
|-----------|------|-------|---------|-------------|
| N | INT | 0-10 | 0 | Integration points through thickness |
| Istrain | INT | 0-2 | 0 | Compute strains for post-processing |
| Thick | FLOAT | >0 | Required | Shell thickness [L] |
| Ashear | FLOAT | - | 0.833 | Shear correction factor (5/6) |
| Ithick | INT | 0-2 | 0 | Thickness change calculation flag |
| Iplas | INT | 0-2 | 0 | Plasticity algorithm flag |
| Ipos | INT | 0,3,4 | 0 | Reference surface position |

### Ishell (4-Node Shell Formulation) Options

| Value | Description | Application |
|-------|-------------|-------------|
| 0 | Default from /DEF_SHELL | General use |
| 1 | Q4 Belytschko-Tsay with orthogonality | Standard crashworthiness |
| 2 | Q4 Hallquist without orthogonality | Simple problems |
| 3 | Q4 with elasto-plastic hourglass | High deformation |
| 4 | Improved type 1 (warped elements) | Warped geometry |
| 12 | QBAT (fully integrated) | Accurate, no hourglass |
| 24 | QEPH (physical stabilization) | Best accuracy/cost ratio |

### Ismstr (Strain Formulation) Options

| Value | Description |
|-------|-------------|
| 0 | Default from /DEF_SHELL |
| 1 | Small strain from t=0 |
| 2 | Large strain, possible switch to small in Engine |
| 3 | Simplified small strain from t=0 |
| 4 | Full geometric nonlinearities |
| 10 | Lagrange total strain |

### Ish3n (3-Node Shell Formulation) Options

| Value | Description |
|-------|-------------|
| 0 | Default from /DEF_SHELL |
| 1 | Standard C0 triangle |
| 2 | Standard C0 with large rotation fix |
| 30 | DKT18 (Discrete Kirchhoff) |
| 31 | DKT_S3 based on DKT12 Batoz |

### Example - Standard Automotive Steel Shell
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/PROP/SHELL/1
Automotive_Steel_Panel_1.2mm
#   Ishell    Ismstr     Ish3n    Idrill    Ipinch                                P_thick
        24         0         0         0         0                                    0.0
#                 Hm                  Hf                  Hr                  Dm                  Dn
              0.01                0.01                0.01                 0.0                 0.0
#        N   Istrain               Thick              Ashear              Ithick     Iplas      Ipos
         5         1                 1.2            0.83333                    1         1         0
```

---

## 2. TYPE2 - TRUSS (Truss Property)

Defines properties for truss (rod) elements that carry only axial loads.

### Syntax
```
/PROP/TRUSS/prop_ID
prop_title
#               Area
```

### Parameters

| Parameter | Type | Default | Unit | Description |
|-----------|------|---------|------|-------------|
| Area | FLOAT | Required | [L²] | Cross-sectional area |

### Example - Steel Rod
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/PROP/TRUSS/10
Steel_Rod_10mm_dia
#               Area
              78.54
```

---

## 3. TYPE3 - BEAM (Standard Beam Property)

Defines properties for Timoshenko beam elements.

### Syntax
```
/PROP/BEAM/prop_ID
prop_title
#        N
#               Area                 Iyy                 Izz                 Ixx
```

### Parameters

| Parameter | Type | Default | Unit | Description |
|-----------|------|---------|------|-------------|
| N | INT | 3 | - | Number of integration points (1-9) |
| Area | FLOAT | Required | [L²] | Cross-sectional area |
| Iyy | FLOAT | Required | [L⁴] | Second moment of area about Y |
| Izz | FLOAT | Required | [L⁴] | Second moment of area about Z |
| Ixx | FLOAT | Required | [L⁴] | Torsional moment (polar) |

### Cross-Section Formulas

**Rectangular (b × h):**
```
Area = b × h
Iyy = b × h³ / 12
Izz = h × b³ / 12
Ixx ≈ (b × h / 12) × (b² + h²)
```

**Circular (diameter d):**
```
Area = π × d² / 4
Iyy = Izz = π × d⁴ / 64
Ixx = π × d⁴ / 32
```

**Hollow Circular (outer D, inner d):**
```
Area = π × (D² - d²) / 4
Iyy = Izz = π × (D⁴ - d⁴) / 64
Ixx = π × (D⁴ - d⁴) / 32
```

### Example - Rectangular Beam 10×20mm
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/PROP/BEAM/20
Rectangular_Beam_10x20
#        N
         3
#               Area                 Iyy                 Izz                 Ixx
               200.0             6666.67             1666.67             8333.33
```

---

## 4. TYPE4 - SPRING (General Spring Property)

Defines properties for general spring elements with up to 6 degrees of freedom.

### Syntax
```
/PROP/SPRING/prop_ID
prop_title
#    Itype              Mass   Skew_ID    Isflag      Iflg
#                 K1                  K2                  K3                  K4                  K5                  K6
#                 C1                  C2                  C3                  C4                  C5                  C6
```

### Parameters - Card 1

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| Itype | INT | 0 | Spring type identifier |
| Mass | FLOAT | 0.0 | Concentrated mass [M] |
| Skew_ID | INT | 0 | Local coordinate system ID |
| Isflag | INT | 0 | Sensor flag (0=active, 1=inactive, 2=both) |
| Iflg | INT | 0 | Formulation flag |

### Parameters - Cards 2-3 (Stiffness and Damping)

| Parameter | Type | Unit | Description |
|-----------|------|------|-------------|
| K1, K2, K3 | FLOAT | [F/L] | Translational stiffness X, Y, Z |
| K4, K5, K6 | FLOAT | [F·L/rad] | Rotational stiffness X, Y, Z |
| C1, C2, C3 | FLOAT | [F·t/L] | Translational damping X, Y, Z |
| C4, C5, C6 | FLOAT | [F·L·t/rad] | Rotational damping X, Y, Z |

### Itype Options

| Value | Description |
|-------|-------------|
| 0 | General linear spring |
| 4 | Multidirectional spring |
| 8 | Nonlinear elastic spring |
| 12 | General nonlinear spring |
| 25 | Bolt spring |

### Example - Shock Absorber
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/PROP/SPRING/40
Shock_Absorber
#    Itype              Mass   Skew_ID    Isflag      Iflg
         4               1.0         0         0         0
#                 K1                  K2                  K3                  K4                  K5                  K6
            1000.0                 0.0              1000.0                 0.0                 0.0                 0.0
#                 C1                  C2                  C3                  C4                  C5                  C6
              10.0                 0.0                10.0                 0.0                 0.0                 0.0
```

---

## 5. TYPE6 - SOL_ORTH (Orthotropic Solid Property)

Defines properties for orthotropic solid elements with material direction control.

### Syntax
```
/PROP/SOL_ORTH/prop_ID
prop_title
#   Isolid    Ismstr               Icpre               Inpts              Iframe                  Dn
#                 qa                  qb                   h
#                 Vx                  Vy                  Vz   skew_ID        Ip     Iorth
#                Phi
#         DeltaT_min
```

### Parameters - Card 1 (Formulation)

| Parameter | Type | Range | Default | Description |
|-----------|------|-------|---------|-------------|
| Isolid | INT | 0,1,2,12,14,17,24 | 0 | Solid formulation |
| Ismstr | INT | 0-10 | 0 | Small strain flag |
| Icpre | INT | 0-3 | 0 | Constant pressure flag |
| Inpts | INT | 222-999 | 222 | Integration points (RST) |
| Iframe | INT | 0-2 | 1 | Element frame flag |
| Dn | FLOAT | 0-0.15 | 0.1 | Numerical damping |

### Parameters - Card 2 (Bulk Viscosity)

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| qa | FLOAT | 1.10 | Quadratic bulk viscosity |
| qb | FLOAT | 0.05 | Linear bulk viscosity |
| h | FLOAT | 0.10 | Hourglass viscosity coefficient |

### Parameters - Card 3 (Orientation)

| Parameter | Type | Description |
|-----------|------|-------------|
| Vx, Vy, Vz | FLOAT | Reference vector components |
| skew_ID | INT | Skew frame identifier |
| Ip | INT | Reference plane (0-13) |
| Iorth | INT | Orthotropy formulation (0-1) |

### Parameters - Card 4

| Parameter | Type | Unit | Description |
|-----------|------|------|-------------|
| Phi | FLOAT | [deg] | Angle to first orthotropy direction |

### Ip (Reference Plane) Options

| Value | Description |
|-------|-------------|
| 0 | Use skew_ID |
| 1 | (r,s) plane + Phi angle |
| 2 | (s,t) plane + Phi angle |
| 3 | (t,r) plane + Phi angle |
| 11 | (r,s) plane + projected vector |
| 12 | (s,t) plane + projected vector |
| 13 | (t,r) plane + projected vector |

### Example - Orthotropic Composite Solid
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/PROP/SOL_ORTH/3
Composite_Solid_Layup
#   Isolid    Ismstr               Icpre               Inpts              Iframe                  Dn
        24         2                    1                 222                   1                 0.1
#                 qa                  qb                   h
                1.10                0.05                0.10
#                 Vx                  Vy                  Vz   skew_ID        Ip     Iorth
                 1.0                 0.0                 0.0         0         1         0
#                Phi
                45.0
#         DeltaT_min
                 0.0
```

---

## 6. TYPE8 - SPR_GENE (General Spring with 6 DOF)

Defines a general nonlinear spring with independent stiffness, damping, and failure for all 6 degrees of freedom.

### Syntax
```
/PROP/SPR_GENE/prop_ID
prop_title
#               Mass             Inertia   skew_ID   sens_ID    Isflag     Ifail              Iequil
# For each direction (Tx, Ty, Tz, Rx, Ry, Rz):
#                  K                   C                   A                   B                   D
#  fct_ID1         H   fct_ID2   fct_ID3                           DeltaMin            DeltaMax
#            Fscale              Escale              Ascale
```

### Parameters - Card 1

| Parameter | Type | Unit | Description |
|-----------|------|------|-------------|
| Mass | FLOAT | [M] | Concentrated mass |
| Inertia | FLOAT | [M·L²] | Rotational inertia |
| skew_ID | INT | - | Local coordinate system |
| sens_ID | INT | - | Sensor identifier |
| Isflag | INT | - | Sensor activation flag |
| Ifail | INT | - | Failure criteria (0=uni, 1=multi) |
| Iequil | INT | - | Force equilibrium flag |

### Parameters - Per Direction (6 sets)

| Parameter | Type | Description |
|-----------|------|-------------|
| K | FLOAT | Stiffness |
| C | FLOAT | Damping coefficient |
| A | FLOAT | Scale factor A (force) |
| B | FLOAT | Scale factor B (force) |
| D | FLOAT | Scale factor D (velocity) |
| fct_ID1 | INT | Force-displacement function |
| H | INT | Hardening flag |
| fct_ID2 | INT | Strain rate function |
| fct_ID3 | INT | Lower yield function |
| DeltaMin | FLOAT | Negative failure displacement |
| DeltaMax | FLOAT | Positive failure displacement |
| Fscale | FLOAT | Velocity scale factor |
| Escale | FLOAT | Force scale factor |
| Ascale | FLOAT | Abscissa scale factor |

### Hardening Flag (H) Options

| Value | Description |
|-------|-------------|
| 0 | Non-linear elastic spring |
| 1 | Elasto-plastic with isotropic hardening |
| 2 | Elasto-plastic with decoupled tension/compression |
| 4 | Kinematic hardening |
| 5 | Elasto-plastic with non-linear unloading |

### Example - 6-DOF Connection
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/PROP/SPR_GENE/50
General_Connection_6DOF
#               Mass             Inertia   skew_ID   sens_ID    Isflag     Ifail              Iequil
                 1.0               1.0e-3         0         0         0         0                   1
# Tx
#                  K                   C                   A                   B                   D
             10000.0               100.0                 1.0                 0.0                 1.0
#  fct_ID1         H   fct_ID2   fct_ID3                           DeltaMin            DeltaMax
         1         1         2         0                               -50.0                50.0
#            Fscale              Escale              Ascale
                 0.0                 0.0                 1.0
```

---

## 7. TYPE9 - SH_ORTH (Orthotropic Shell Property)

Defines properties for orthotropic shell elements with material orientation.

### Syntax
```
/PROP/SH_ORTH/prop_ID
prop_title
#   Ishell    Ismstr     Ish3n    Idrill                                                      P_thick
#                 Hm                  Hf                  Hr                  Dm                  Dn
#        N   Istrain               Thick              Ashear              Ithick     Iplas      Ipos
#                 Vx                  Vy                  Vz   skew_ID                Iorth
#                Phi
```

### Additional Parameters (vs TYPE1)

| Parameter | Type | Description |
|-----------|------|-------------|
| Vx, Vy, Vz | FLOAT | Reference vector components |
| skew_ID | INT | Skew frame for orientation |
| Iorth | INT | Orthotropy formulation (0-1) |
| Phi | FLOAT | Angle to material direction [deg] |

### Example - Orthotropic Composite Shell
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/PROP/SH_ORTH/5
Carbon_Fiber_Panel
#   Ishell    Ismstr     Ish3n    Idrill                                                      P_thick
        24         0         0         0                                                          0.0
#                 Hm                  Hf                  Hr                  Dm                  Dn
              0.01                0.01                0.01                 0.0                 0.0
#        N   Istrain               Thick              Ashear              Ithick     Iplas      Ipos
         5         1                 2.5            0.83333                    1         0         0
#                 Vx                  Vy                  Vz   skew_ID                Iorth
                 1.0                 0.0                 0.0         0                    0
#                Phi
                 0.0
```

---

## 8. TYPE10 - SH_COMP (Composite Shell Property)

Defines properties for composite shell with multiple integration points and individual ply angles.

### Syntax
```
/PROP/SH_COMP/prop_ID
prop_title
#   Ishell    Ismstr     Ish3n    Idrill
#                 Hm                  Hf                  Hr                  Dm                  Dn
#        N   Istrain               Thick              Ashear              Ithick     Iplas
#                 Vx                  Vy                  Vz
#              Phi_1               Phi_2               Phi_3...
```

### Parameters - Card 4 (Ply Angles)

| Parameter | Type | Unit | Description |
|-----------|------|------|-------------|
| Phi_i | FLOAT | [deg] | Ply orientation angle for layer i |

### Example - [0/45/-45/90]s Composite
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/PROP/SH_COMP/6
Carbon_Fiber_0_45_-45_90_s
#   Ishell    Ismstr     Ish3n    Idrill
        24         0        31         0
#                 Hm                  Hf                  Hr                  Dm                  Dn
              0.01                0.01                0.01                 0.0                 0.0
#        N   Istrain               Thick              Ashear              Ithick     Iplas
         8         1                 2.0            0.83333                    1         1
#                 Vx                  Vy                  Vz
                 1.0                 0.0                 0.0
#              Phi_1               Phi_2               Phi_3               Phi_4               Phi_5
                 0.0                45.0               -45.0                90.0                90.0
#              Phi_6               Phi_7               Phi_8
               -45.0                45.0                 0.0
```

---

## 9. TYPE11 - SH_SANDW (Sandwich Shell Property)

Defines properties for sandwich shell structures with multiple layers of different materials.

### Syntax
```
/PROP/SH_SANDW/prop_ID
prop_title
#   Ishell    Ismstr     Ish3n    Idrill
#                 Hm                  Hf                  Hr                  Dm                  Dn
#        N   Istrain               Thick              Ashear              Ithick     Iplas
#                 Vx                  Vy                  Vz   skew_ID     Iorth      Ipos
# For each layer:
#                Phi                   t                   Z    mat_ID
```

### Parameters - Layer Definition (repeated N times)

| Parameter | Type | Unit | Description |
|-----------|------|------|-------------|
| Phi | FLOAT | [deg] | Layer orientation angle |
| t | FLOAT | [L] | Layer thickness |
| Z | FLOAT | [L] | Layer Z position (if Ipos=1) |
| mat_ID | INT | - | Material ID for layer |

### Ipos Options

| Value | Description |
|-------|-------------|
| 0 | Layer positions auto-calculated from thicknesses |
| 1 | User-defined Z positions for all layers |

### Example - Honeycomb Sandwich Panel
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/PROP/SH_SANDW/7
Honeycomb_Sandwich_Panel
#   Ishell    Ismstr     Ish3n    Idrill
        24         0         0         0
#                 Hm                  Hf                  Hr                  Dm                  Dn
              0.01                0.01                0.01                 0.0                 0.0
#        N   Istrain               Thick              Ashear              Ithick     Iplas
         3         1                12.0            0.83333                    1         1
#                 Vx                  Vy                  Vz   skew_ID     Iorth      Ipos
                 1.0                 0.0                 0.0         0         0         0
# Top face sheet
#                Phi                   t                   Z    mat_ID
                 0.0                 0.5                 0.0        10
# Honeycomb core
                 0.0                11.0                 0.0        20
# Bottom face sheet
                 0.0                 0.5                 0.0        10
```

---

## 10. TYPE12 - SPR_PUL (Pulley Spring Property)

Defines properties for pulley spring elements used in seatbelt and cable systems.

### Syntax
```
/PROP/SPR_PUL/prop_ID
prop_title
#               Mass                                 sens_ID    Isflag     Ileng                Fric
#                  K                   C                   A                   B                   D
#  fct_ID1         H   fct_ID2                                          DeltaMin            DeltaMax
#            Fscale1                   E             AScalex
```

### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| Mass | FLOAT | Spring mass [M] |
| sens_ID | INT | Sensor identifier |
| Isflag | INT | Sensor activation flag |
| Ileng | INT | Input per unit length flag (0=absolute, 1=per length) |
| Fric | FLOAT | Coulomb friction coefficient |
| K | FLOAT | Stiffness |
| C | FLOAT | Damping coefficient |
| A | FLOAT | A coefficient (force scale) |
| B | FLOAT | B coefficient (force scale) |
| D | FLOAT | D coefficient (velocity scale) |
| fct_ID1 | INT | Force-displacement function |
| H | INT | Hardening flag |
| fct_ID2 | INT | Strain rate function |
| DeltaMin | FLOAT | Negative failure displacement |
| DeltaMax | FLOAT | Positive failure displacement |

### Hardening Flag (H) Options

| Value | Description |
|-------|-------------|
| 0 | Nonlinear elastic spring |
| 1 | Nonlinear elasto-plastic spring |
| 2 | Nonlinear elasto-plastic with decoupled tension/compression |

### Example - Seatbelt Pulley
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/PROP/SPR_PUL/15
Seatbelt_Pulley
#               Mass                                 sens_ID    Isflag     Ileng                Fric
               0.01                                       0         0         0                0.15
#                  K                   C                   A                   B                   D
            1000.0                 10.0                 1.0                 0.0                 1.0
#  fct_ID1         H   fct_ID2                                          DeltaMin            DeltaMax
         1         0         0                                           -1.0e30             1.0e30
#            Fscale1                   E             AScalex
                 0.0                 0.0                 1.0
```

---

## 11. TYPE14 - SOLID (General Solid Property)

Defines properties for general solid elements (8-node brick, tetrahedra, etc.).

### Syntax
```
/PROP/SOLID/prop_ID
prop_title
#   Isolid    Ismstr          Iale     Icpre  Itetra10     Inpts   Itetra4    Iframe                  Dn
#                 qa                  qb                   h              Lambda                  Mu
#         DeltaT_min           vdef_min           vdef_max            ASP_max            COL_min
#     Ndir sphpart_ID  Icontrol
```

### Parameters - Card 1 (Formulation)

| Parameter | Type | Range | Default | Description |
|-----------|------|-------|---------|-------------|
| Isolid | INT | 0-24 | 0 | Solid element formulation |
| Ismstr | INT | 0-12 | 0 | Small strain formulation |
| Iale | INT | 0-2 | 0 | ALE/Lagrangian flag |
| Icpre | INT | 0-3 | 0 | Constant pressure flag |
| Itetra10 | INT | 0,2,1000 | 0 | 10-node tetra formulation |
| Inpts | INT | 222-999 | 222 | Integration points (RST digits) |
| Itetra4 | INT | 0,1,3,1000 | 0 | 4-node tetra formulation |
| Iframe | INT | 0-2 | 0 | Element frame flag |
| Dn | FLOAT | 0-0.15 | 0.1 | Numerical damping |

### Parameters - Card 2 (Viscosity)

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| qa | FLOAT | 1.10 | Quadratic bulk viscosity |
| qb | FLOAT | 0.05 | Linear bulk viscosity |
| h | FLOAT | 0.10 | Hourglass viscosity (0-0.15) |
| Lambda | FLOAT | 0.0 | Navier-Stokes 1st viscosity |
| Mu | FLOAT | 0.0 | Navier-Stokes 2nd viscosity |

### Parameters - Card 3 (Limits)

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| DeltaT_min | FLOAT | 0.0 | Minimum time step |
| vdef_min | FLOAT | 0.0 | Minimum volume strain |
| vdef_max | FLOAT | 0.0 | Maximum volume strain |
| ASP_max | FLOAT | 0.0 | Maximum aspect ratio |
| COL_min | FLOAT | 0.0 | Minimum collapse ratio |

### Isolid Options

| Value | Description | Integration |
|-------|-------------|-------------|
| 0 | Default from /DEF_SOLID | - |
| 1 | Standard Belytschko | 1 point, hourglass |
| 2 | Standard Hallquist | 1 point, hourglass |
| 12 | Full integration | 2×2×2 |
| 14 | HA8 co-rotational | Variable Gauss |
| 16 | 20-node quadratic | Full integration |
| 17 | H8C compatible | 2×2×2 full |
| 18 | 8-node co-rotational | Full integration |
| 24 | HEPH physical stabilization | Reduced, no hourglass |

### Iale Options

| Value | Description |
|-------|-------------|
| 0 | Lagrangian |
| 1 | Eulerian |
| 2 | ALE (Arbitrary Lagrangian-Eulerian) |

### Example - Standard Solid Element
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/PROP/SOLID/100
Standard_Brick_Element
#   Isolid    Ismstr          Iale     Icpre  Itetra10     Inpts   Itetra4    Iframe                  Dn
        24         0             0         1         0       222         0         1                 0.1
#                 qa                  qb                   h              Lambda                  Mu
                1.10                0.05                0.10                 0.0                 0.0
#         DeltaT_min           vdef_min           vdef_max            ASP_max            COL_min
                 0.0                 0.0                 0.0                 0.0                 0.0
#     Ndir sphpart_ID  Icontrol
         1          0          0
```

---

## 12. TYPE20 - TSHELL (Thick Shell Property)

Defines properties for thick shell elements that include transverse shear.

### Syntax
```
/PROP/TSHELL/prop_ID
prop_title
#   Isolid    Ismstr               Icpre     Icstr     Inpts      Iint                            Dn
#                 qa                  qb                   h
#         DeltaT_min
```

### Parameters

| Parameter | Type | Range | Default | Description |
|-----------|------|-------|---------|-------------|
| Isolid | INT | 14-16 | 15 | Thick shell formulation |
| Ismstr | INT | 0-4 | 0 | Small strain flag |
| Icpre | INT | 0-2 | 0 | Constant pressure flag |
| Icstr | INT | 0,1,10,100 | 0 | Constant stress flag (TYPE14 only) |
| Inpts | INT | - | 222 | Integration points (RST or N for TYPE15) |
| Iint | INT | 0-2 | 1 | Integration scheme (TYPE16 only) |
| Dn | FLOAT | 0-0.15 | 0.1 | Numerical damping |
| qa | FLOAT | - | 1.10 | Quadratic bulk viscosity |
| qb | FLOAT | - | 0.05 | Linear bulk viscosity |
| h | FLOAT | 0-0.15 | 0.10 | Hourglass coefficient |
| DeltaT_min | FLOAT | - | 1.0e6 | Minimum time step |

### Isolid Options for Thick Shell

| Value | Description |
|-------|-------------|
| 14 | HA8 locking-free, full integration |
| 15 | HSEPH/PA6 co-rotational, reduced integration |
| 16 | 16-node quadratic, full integration |

### Icstr (Reduced Stress Integration) for TYPE14

| Value | Description |
|-------|-------------|
| 0 | No reduced stress integration |
| 001 | Reduced in T direction |
| 010 | Reduced in S direction |
| 100 | Reduced in R direction |

### Example - Thick Shell for Composites
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/PROP/TSHELL/25
Thick_Composite_Shell
#   Isolid    Ismstr               Icpre     Icstr     Inpts      Iint                            Dn
        15         0                    0         0         5         1                           0.1
#                 qa                  qb                   h
                1.10                0.05                0.10
#         DeltaT_min
                 0.0
```

---

## 13. TYPE34 - SPH (SPH Particle Property)

Defines properties for Smoothed Particle Hydrodynamics (SPH) elements.

### Syntax
```
/PROP/SPH/prop_ID
prop_title
#                 Mp                  qa                  qb            Alpha_CS
#    Order                   h
```

### Parameters

| Parameter | Type | Default | Unit | Description |
|-----------|------|---------|------|-------------|
| Mp | FLOAT | Required | [M] | Particle mass |
| qa | FLOAT | 2.0 | - | Quadratic bulk viscosity |
| qb | FLOAT | 1.0 | - | Linear bulk viscosity |
| Alpha_CS | FLOAT | 0.0 | - | Conservative smoothing coefficient |
| Order | INT | 0 | - | SPH correction order |
| h | FLOAT | Required | [L] | Smoothing length |

### Order Options

| Value | Description |
|-------|-------------|
| -1 | No correction |
| 0 | Order 0 correction (default) |
| 1 | Order 1 correction (not for SPMD parallel) |

### Example - Water SPH Particles
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/PROP/SPH/60
Water_SPH_Particles
#                 Mp                  qa                  qb            Alpha_CS
            1.0e-6                 2.0                 1.0                 0.0
#    Order                   h
         0                 0.5
```

---

## 14. TYPE5 - RIVET (Rivet Connection Property)

Defines properties for rivet elements connecting shell components.

### Syntax
```
/PROP/RIVET/prop_ID
prop_title
#               Mass
#             Yield_F            Yield_M             Rupt_F             Rupt_M
```

### Parameters

| Parameter | Type | Unit | Description |
|-----------|------|------|-------------|
| Mass | FLOAT | [M] | Rivet mass |
| Yield_F | FLOAT | [F] | Yield force |
| Yield_M | FLOAT | [F·L] | Yield moment |
| Rupt_F | FLOAT | [F] | Rupture force |
| Rupt_M | FLOAT | [F·L] | Rupture moment |

---

## Default Property Cards

### /DEF_SHELL

Sets default values for all shell properties in the model.

```
/DEF_SHELL
#   Ishell    Ismstr     Ish3n    Idrill                                                      P_thick
#                 Hm                  Hf                  Hr                  Dm                  Dn
```

### Example
```
/DEF_SHELL
        24         0        31         0                                                          0.0
              0.01                0.01                0.01                 0.0                 0.0
```

### /DEF_SOLID

Sets default values for all solid properties in the model.

```
/DEF_SOLID
#   Isolid    Ismstr
#                 qa                  qb                   h
```

### Example
```
/DEF_SOLID
        24         0
                1.10                0.05                0.10
```

---

## Property Selection Guidelines

### Element Type vs Property Type

| Element Type | Recommended Properties |
|--------------|----------------------|
| 4-node shell (QUAD) | TYPE1 (SHELL), TYPE9 (SH_ORTH), TYPE10 (SH_COMP) |
| 3-node shell (TRIA) | TYPE1 (SHELL), TYPE9 (SH_ORTH) |
| 8-node solid (BRICK) | TYPE14 (SOLID), TYPE6 (SOL_ORTH) |
| 4-node tetra (TETRA4) | TYPE14 (SOLID) |
| 10-node tetra (TETRA10) | TYPE14 (SOLID) |
| Thick shell | TYPE20 (TSHELL), TYPE21, TYPE22 |
| Beam | TYPE3 (BEAM), TYPE18 (INT_BEAM) |
| Truss | TYPE2 (TRUSS) |
| Spring | TYPE4 (SPRING), TYPE8 (SPR_GENE) |
| SPH particles | TYPE34 (SPH) |

### Application Guidelines

| Application | Recommended Formulations |
|-------------|-------------------------|
| Automotive crash | Ishell=24 (QEPH), Ismstr=2 |
| Metal forming | Ishell=12 (QBAT), Ismstr=4 |
| Composites | TYPE10 (SH_COMP), TYPE11 (SH_SANDW) |
| Drop test | Isolid=24 (HEPH), Ismstr=0 |
| Blast loading | Iale=2 (ALE), Isolid=1 |
| Fluid dynamics | TYPE34 (SPH), Alpha_CS>0 |

### Hourglass Control Recommendations

| Situation | Hm/Hf/Hr Range |
|-----------|----------------|
| General use | 0.01 |
| High deformation | 0.03-0.05 |
| Stiff contact | 0.02-0.03 |
| Reduced energy error | 0.005-0.01 |

---

## Troubleshooting

### Common Issues

| Issue | Possible Cause | Solution |
|-------|----------------|----------|
| Hourglass modes | Low viscosity | Increase Hm/Hf/Hr |
| Locking (bending) | Under-integrated | Use Ishell=12 or 24 |
| Shear locking | Full integration | Use selective reduced |
| Volume locking | Incompressible | Set Icpre=1 |
| Spurious modes | Single point | Add stabilization |
| Time step too small | Thin element | Check thickness |

### Validation Checks

1. **Hourglass energy** < 5% of internal energy
2. **Mass added** < 1% of total mass
3. **Thickness change** reasonable for material
4. **Integration points** sufficient for through-thickness stress

---

## References

- Property CFG files: `hm_cfg_files/config/CFG/radioss*/PROP/`
- Starter source: `starter/source/properties/`
- Engine source: `engine/source/properties/`
