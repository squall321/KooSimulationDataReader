# OpenRadioss Keyword Manual - Failure Models

## Overview

This document describes failure model definitions in OpenRadioss. Failure models define criteria for material failure and element deletion based on strain, stress, energy, or damage accumulation. Failure models are linked to materials and activate when specific criteria are met during simulation.

---

## Failure Keyword Syntax

```
/FAIL/fail_type/mat_ID
[failure parameters...]
```

**Note:** Failure models are associated with materials by referencing the material ID (mat_ID).

---

## Complete Failure Model Reference

| Model | Description | Application |
|-------|-------------|-------------|
| ALTER | Glass failure with subcritical crack growth | Automotive glass, windshields |
| BIQUAD | Bi-quadratic strain failure | Sheet metal forming |
| CHANG | Chang-Chang composite failure | Unidirectional composites |
| COCKCROFT | Cockcroft-Latham energy failure | Ductile metals |
| CONNECT | Connection element failure | Spot welds, adhesives |
| EMC | Extended Mohr-Coulomb | Ductile metals with triaxiality |
| ENERGY | Energy-based failure | General purpose |
| FLD | Forming Limit Diagram | Sheet metal stamping |
| GENE1 | General multi-criteria failure | Complex failure criteria |
| GURSON | Gurson-Tvergaard-Needleman | Void growth, porous metals |
| HASHIN | Hashin composite failure | Fabric/UD composites |
| JOHNSON | Johnson-Cook damage | High strain rate metals |
| LAD_DAMA | Ladeveze damage | Composite delamination |
| NXT | NXT strain failure | Principal stress envelope |
| ORTHBIQUAD | Orthotropic bi-quadratic | Anisotropic sheet metal |
| PUCK | Puck composite failure | Composites with IFF |
| RTCL | Rice-Tracey-Cockroft-Latham | Ductile metals |
| SAHRAEI | Sahraei battery failure | Li-ion battery cells |
| SNCONNECT | Spot-weld connection failure | Spot weld connections |
| SPALLING | Spalling failure | High velocity impact |
| TAB1 | Tabulated failure (type 1) | Custom failure curves |
| TAB2 | Tabulated failure (type 2) | Advanced tabulated |
| TBUTCHER | Tuler-Butcher | Dynamic fracture |
| TENSSTRAIN | Tensile strain failure | Simple strain criterion |
| USER | User-defined failure | Custom failure routines |
| WIERZBICKI | Modified Mohr-Coulomb | Ductile fracture |
| WILKINS | Wilkins cumulative damage | Cumulative damage |
| XFEM_FLD | XFEM with FLD | Crack propagation |
| XFEM_JOHNS | XFEM with Johnson-Cook | Crack propagation |
| XFEM_TBUTC | XFEM with Tuler-Butcher | Crack propagation |

---

## 1. JOHNSON - Johnson-Cook Damage

Johnson-Cook damage model based on stress triaxiality, strain rate, and temperature effects.

### Syntax
```
/FAIL/JOHNSON/mat_ID
#                 D1                  D2                  D3                  D4                  D5
#      EPSILON_DOT_0  IFAIL_SH  IFAIL_SO                                    DADV               IXFEM
```

### Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| D1 | FLOAT | - | First damage parameter |
| D2 | FLOAT | - | Second damage parameter |
| D3 | FLOAT | - | Third damage parameter (triaxiality) |
| D4 | FLOAT | - | Fourth damage parameter (strain rate) |
| D5 | FLOAT | - | Fifth damage parameter (temperature) |
| EPSILON_DOT_0 | FLOAT | - | Reference strain rate |
| IFAIL_SH | INT | 0 | Shell failure flag |
| IFAIL_SO | INT | 0 | Solid failure flag |
| DADV | FLOAT | 1.0 | Crack advancement criterion (0-1) |
| IXFEM | INT | 0 | XFEM flag |

### Johnson-Cook Damage Model
```
D = sum(d_eps_p / eps_f)

eps_f = (D1 + D2*exp(D3*sigma*/sigma_m)) * (1 + D4*ln(eps_dot/eps_dot0)) * (1 + D5*T*)

Failure when D >= 1
```

### IFAIL_SH Options

| Value | Description |
|-------|-------------|
| 0 | Default behavior |
| 1 | Shell deleted if condition reached for 1 integration point or layer |
| 2 | Stress tensor set to 0 if condition reached, deleted when all IPs fail |

### IFAIL_SO Options

| Value | Description |
|-------|-------------|
| 0 | Default behavior |
| 1 | Solid deleted if condition reached for 1 integration point |
| 2 | Deviatoric stress vanished if condition reached for 1 IP |

### Example
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/FAIL/JOHNSON/15
#                 D1                  D2                  D3                  D4                  D5
                0.13               -0.34               -0.47               0.016                0.61
#      EPSILON_DOT_0  IFAIL_SH  IFAIL_SO                                    DADV               IXFEM
                0.01         1         2                                     1.0                    0
```

---

## 2. BIQUAD - Bi-Quadratic Strain Failure

Bi-quadratic strain-based failure criterion using five characteristic strains for different stress states.

### Syntax
```
/FAIL/BIQUAD/mat_ID
#                 C1                  C2                  C3                  C4                  C5
#        P_THICKFAIL    M_FLAG    S_FLAG          INST_START
#                 R1                  R2                  R4                  R5  (if M_FLAG=99)
```

### Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| C1 | FLOAT | - | Failure plastic strain at uniaxial compression |
| C2 | FLOAT | - | Failure plastic strain at pure shear |
| C3 | FLOAT | - | Failure plastic strain at uniaxial tension |
| C4 | FLOAT | - | Failure plastic strain at plane strain tension |
| C5 | FLOAT | - | Failure plastic strain at biaxial tension |
| P_THICKFAIL | FLOAT | 0 | Thickness failure percentage (0-1) |
| M_FLAG | INT | 0 | Material preset flag |
| S_FLAG | INT | 1 | Behavior flag |
| INST_START | FLOAT | 0 | Instability start strain |
| R1-R5 | FLOAT | - | User-defined ratios (M_FLAG=99) |

### M_FLAG Material Presets

| Value | Material | Description |
|-------|----------|-------------|
| 0 | Custom | Use C1-C5 values directly |
| 1 | Mild Steel | Predefined failure ratios |
| 2 | HSS | High strength steel |
| 3 | UHSS | Ultra high strength steel |
| 4 | AA5182 | Aluminum 5182 alloy |
| 5 | AA6082-T6 | Aluminum 6082-T6 |
| 6 | PA6GF30 | Polyamide 30% glass fiber |
| 7 | PP T40 | Polypropylene T40 |
| 99 | User Ratios | Define R1-R5 ratios |

### S_FLAG Options

| Value | Description |
|-------|-------------|
| 0 | Set to 1 |
| 1 | Two quadratic functions (default) |
| 2 | Plane strain is global minimum |
| 3 | Plane strain + localized necking |

### Example
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/FAIL/BIQUAD/10
#                 C1                  C2                  C3                  C4                  C5
                 0.8                 0.5                 0.4                0.35                 0.6
#        P_THICKFAIL    M_FLAG    S_FLAG          INST_START
                 0.5         0         1                 0.0
```

---

## 3. TENSSTRAIN - Tensile Strain Failure

Simple tensile strain-based failure criterion with optional softening.

### Syntax
```
/FAIL/TENSSTRAIN/mat_ID
#  EPSILON_T1    EPSILON_T2    FCT_ID       EPSILON_F1       EPSILON_F2   S_FLAG
#   FCT_IDEL     FSCALE_EL        EI_REF
#   FCT_ID_T      FSCALE_T
#  FAILIP
```

### Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| EPSILON_T1 | FLOAT | - | Maximum strain (softening start) |
| EPSILON_T2 | FLOAT | - | Tensile failure strain (deletion) |
| FCT_ID | INT | 0 | Scale function for strains |
| EPSILON_F1 | FLOAT | - | Principal direction 1 failure strain |
| EPSILON_F2 | FLOAT | - | Principal direction 2 failure strain |
| S_FLAG | INT | 1 | Option flag |
| FCT_IDEL | INT | 0 | Element size function |
| FSCALE_EL | FLOAT | 1.0 | Element size scale factor |
| EI_REF | FLOAT | - | Reference element size |
| FCT_ID_T | INT | 0 | Temperature function |
| FSCALE_T | FLOAT | 1.0 | Temperature scale factor |
| FAILIP | INT | 0 | Failed IPs before element deletion |

### S_FLAG Options

| Value | Description |
|-------|-------------|
| 1 | Equivalent strain only (default) |
| 2 | Equivalent + element size + temperature |
| 3 | Max principal strain + scaling |
| 11 | Engineering strain |
| 12 | Engineering + scaling |
| 21 | True strain |
| 22 | True strain + scaling |

### Example
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/FAIL/TENSSTRAIN/30
#  EPSILON_T1         EPSILON_T2    FCT_ID       EPSILON_F1       EPSILON_F2   S_FLAG
                 0.3             0.5         0                0.4              0.6         1
#  FAILIP
         2
```

---

## 4. FLD - Forming Limit Diagram

Failure based on forming limit diagram for sheet metal forming applications.

### Syntax
```
/FAIL/FLD/mat_ID
#  FCT_ID  IFAIL_SH    I_MARG FCT_IDADV           RANI           DADV   ISTRAIN     IXFEM
```

### Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| FCT_ID | INT | - | FLD function ID |
| IFAIL_SH | INT | 0 | Shell failure flag (0-4) |
| I_MARG | INT | 0 | Margin definition flag (0-3) |
| FCT_IDADV | INT | 0 | Crack advancement function |
| RANI | FLOAT | - | Average anisotropy coefficient |
| DADV | FLOAT | 0.0 | Crack advancement criterion (0-1) |
| ISTRAIN | INT | 0 | Strain formulation |
| IXFEM | INT | 0 | XFEM flag |

### IFAIL_SH Options

| Value | Description |
|-------|-------------|
| 0 | Default behavior |
| 1 | Delete when 1 layer fails |
| 2 | Zero stress, delete when all layers fail |
| 3 | Membrane strain only |
| 4 | Criterion calculation only, no deletion |

### ISTRAIN Options

| Value | Description |
|-------|-------------|
| 0 | True strain |
| 1 | Engineering strain |
| 2 | Non-linear strain |

### Example
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/FAIL/FLD/12
#  FCT_ID  IFAIL_SH    I_MARG FCT_IDADV           RANI           DADV   ISTRAIN     IXFEM
        50         1         0         0                1.0              1.0         0         0
```

---

## 5. ENERGY - Energy-Based Failure

Failure based on specific energy (energy per unit volume).

### Syntax
```
/FAIL/ENERGY/mat_ID
#              E1                  E2    FCT_ID              XSCALE     I_DAM    FAILIP
```

### Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| E1 | FLOAT | - | Maximum specific energy (failure start) |
| E2 | FLOAT | - | Failure specific energy (deletion) |
| FCT_ID | INT | 0 | Function ID for E1/E2 scaling vs strain rate |
| XSCALE | FLOAT | 1.0 | X-axis scale factor (strain rate) |
| I_DAM | INT | 0 | Damage calculation flag |
| FAILIP | INT | 0 | Failed IPs before element deletion |

### Example
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/FAIL/ENERGY/8
#              E1                  E2    FCT_ID              XSCALE     I_DAM    FAILIP
             100.0               50.0         0               0.001         0         2
```

---

## 6. HASHIN - Hashin Composite Failure

Hashin failure criterion for fiber-reinforced composite materials.

### Syntax
```
/FAIL/HASHIN/mat_ID
#    IFORM  IFAIL_SH  IFAIL_SO               RATIO     I_DAM
#           SIGMA_1T            SIGMA_2T            SIGMA_3T            SIGMA_1C            SIGMA_2C
#            SIGMA_C           SIGMA_12F           SIGMA_12M           SIGMA_23M           SIGMA_13M
#                PHI                SDEL             TAU_MAX
```

### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| IFORM | INT | Formulation flag (1: UD, 2: Fabric) |
| IFAIL_SH | INT | Shell failure flag |
| IFAIL_SO | INT | Solid failure flag |
| RATIO | FLOAT | Ratio control parameter |
| I_DAM | INT | Damage calculation flag |
| SIGMA_1T | FLOAT | Longitudinal tensile strength |
| SIGMA_2T | FLOAT | Transverse tensile strength |
| SIGMA_3T | FLOAT | Through-thickness tensile strength |
| SIGMA_1C | FLOAT | Longitudinal compressive strength |
| SIGMA_2C | FLOAT | Transverse compressive strength |
| SIGMA_C | FLOAT | Crush strength |
| SIGMA_12F | FLOAT | Fiber shear strength |
| SIGMA_12M | FLOAT | Matrix shear strength (1-2 plane) |
| SIGMA_23M | FLOAT | Matrix shear strength (2-3 plane) |
| SIGMA_13M | FLOAT | Matrix shear strength (1-3 plane) |
| PHI | FLOAT | Coulomb friction angle (<90°) |
| SDEL | FLOAT | Scale factor for delamination |
| TAU_MAX | FLOAT | Dynamic time relaxation |

### IFORM Options

| Value | Description |
|-------|-------------|
| 1 | Unidirectional lamina model |
| 2 | Fabric lamina model |

### Example
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/FAIL/HASHIN/20
#    IFORM  IFAIL_SH  IFAIL_SO               RATIO     I_DAM
         1         1         1                 0.5         1
#           SIGMA_1T            SIGMA_2T            SIGMA_3T            SIGMA_1C            SIGMA_2C
              2000.0               60.0               60.0              1200.0              200.0
#            SIGMA_C           SIGMA_12F           SIGMA_12M           SIGMA_23M           SIGMA_13M
               400.0              100.0               70.0               70.0               70.0
#                PHI                SDEL             TAU_MAX
                53.0                1.0            0.0001
```

---

## 7. CHANG - Chang-Chang Composite Failure

Chang-Chang failure criterion for composite laminates.

### Syntax
```
/FAIL/CHANG/mat_ID
#           SIGMA_1T            SIGMA_2T            SIGMA_12            SIGMA_1C            SIGMA_2C
#               BETA             TAU_MAX  IFAIL_SH
```

### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| SIGMA_1T | FLOAT | Longitudinal tensile strength |
| SIGMA_2T | FLOAT | Transverse tensile strength |
| SIGMA_12 | FLOAT | Shear strength |
| SIGMA_1C | FLOAT | Longitudinal compressive strength |
| SIGMA_2C | FLOAT | Transverse compressive strength |
| BETA | FLOAT | Shear scaling factor |
| TAU_MAX | FLOAT | Dynamic time relaxation |
| IFAIL_SH | INT | Shell failure flag |

### IFAIL_SH Options

| Value | Description |
|-------|-------------|
| 1 | Delete if fiber or matrix damage in one layer |
| 2 | Delete if fiber or matrix damage in all layers |
| 3 | Delete if fiber damage in one layer only |
| 4 | Delete if fiber damage in all layers |

### Example
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/FAIL/CHANG/25
#           SIGMA_1T            SIGMA_2T            SIGMA_12            SIGMA_1C            SIGMA_2C
              1500.0               40.0               68.0              1200.0              180.0
#               BETA             TAU_MAX  IFAIL_SH
                 1.0            0.0001         2
```

---

## 8. PUCK - Puck Composite Failure

Puck failure criterion with inter-fiber failure (IFF) modes.

### Syntax
```
/FAIL/PUCK/mat_ID
#           SIGMA_1T            SIGMA_2T            SIGMA_12            SIGMA_1C            SIGMA_2C
#       P12_POSITIVE        P12_NEGATIVE        P22_NEGATIVE             TAU_MAX  IFAIL_SH  IFAIL_SO
#               FCUT
```

### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| SIGMA_1T | FLOAT | Longitudinal tensile strength |
| SIGMA_2T | FLOAT | Transverse tensile strength |
| SIGMA_12 | FLOAT | Shear strength |
| SIGMA_1C | FLOAT | Longitudinal compressive strength |
| SIGMA_2C | FLOAT | Transverse compressive strength |
| P12_POSITIVE | FLOAT | Failure envelope factor 12 (positive) |
| P12_NEGATIVE | FLOAT | Failure envelope factor 12 (negative) |
| P22_NEGATIVE | FLOAT | Failure envelope factor 22 (negative) |
| TAU_MAX | FLOAT | Dynamic time relaxation |
| IFAIL_SH | INT | Shell failure flag |
| IFAIL_SO | INT | Solid failure flag |
| FCUT | FLOAT | Cutoff frequency for stress tensor |

### Example
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/FAIL/PUCK/30
#           SIGMA_1T            SIGMA_2T            SIGMA_12            SIGMA_1C            SIGMA_2C
              1800.0               50.0               75.0              1400.0              200.0
#       P12_POSITIVE        P12_NEGATIVE        P22_NEGATIVE             TAU_MAX  IFAIL_SH  IFAIL_SO
                0.30                0.30                0.25            0.0001         1         1
#               FCUT
              1000.0
```

---

## 9. GURSON - Gurson-Tvergaard-Needleman Void Growth

Micromechanical failure model based on void nucleation, growth, and coalescence.

### Syntax
```
/FAIL/GURSON/mat_ID
#                 q1                  q2                                                        Iloc
#                 En                  As                  Kw
#                 fc                  fr                  f0
#               Rlen                Hchi              Le_max
```

### Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| q1 | FLOAT | 1.5 | First Gurson damage coefficient |
| q2 | FLOAT | 1.0 | Second Gurson damage coefficient |
| Iloc | INT | 1 | Damage accumulation method |
| En | FLOAT | - | Equivalent plastic strain at void nucleation |
| As | FLOAT | - | Linear void nucleation slope |
| Kw | FLOAT | - | Shear damage growth coefficient |
| fc | FLOAT | - | Critical void volume fraction at coalescence |
| fr | FLOAT | - | Void volume fraction at failure |
| f0 | FLOAT | - | Initial void volume fraction |
| Rlen | FLOAT | - | Radius of non-local influence |
| Hchi | FLOAT | - | Non-local penalty parameter |
| Le_max | FLOAT | - | Maximum element length for convergence |

### Iloc Options

| Value | Description |
|-------|-------------|
| 0 | Set to 1 |
| 1 | Local damage formulation (default) |
| 2 | Non-local (Micromorphic method) |
| 3 | Non-local (Peerlings method) |

### Example
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/FAIL/GURSON/35
#                 q1                  q2                                                        Iloc
                 1.5                 1.0                                                           1
#                 En                  As                  Kw
                 0.3                0.01                 1.0
#                 fc                  fr                  f0
                0.15                0.25               0.001
#               Rlen                Hchi              Le_max
                 2.0              1000.0                 1.0
```

---

## 10. WIERZBICKI - Modified Mohr-Coulomb

Modified Mohr-Coulomb (Wierzbicki) failure model for ductile fracture.

### Syntax
```
/FAIL/WIERZBICKI/mat_ID
#                 C1                  C2                  C3                  C4                   M
#                  N  IFAIL_SH  IFAIL_SO      IMOY
```

### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| C1 | FLOAT | First parameter |
| C2 | FLOAT | Second parameter |
| C3 | FLOAT | Third parameter |
| C4 | FLOAT | Fourth parameter |
| M | FLOAT | Fifth parameter (Lode angle exponent) |
| N | FLOAT | Hardening exponent |
| IFAIL_SH | INT | Shell failure flag |
| IFAIL_SO | INT | Solid failure flag |
| IMOY | INT | 3D failure model flag (brick) |

### Example
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/FAIL/WIERZBICKI/40
#                 C1                  C2                  C3                  C4                   M
                 0.2                 720                 1.0                0.04                 1.0
#                  N  IFAIL_SH  IFAIL_SO      IMOY
                0.15         1         1         0
```

---

## 11. WILKINS - Wilkins Cumulative Damage

Wilkins cumulative damage model for ductile materials.

### Syntax
```
/FAIL/WILKINS/mat_ID
#              ALPHA                BETA                PLIM                  DF  IFAIL_SH  IFAIL_SO
```

### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| ALPHA | FLOAT | Hydrostatic pressure exponent |
| BETA | FLOAT | Deviatoric coefficient |
| PLIM | FLOAT | Hydrostatic pressure limit |
| DF | FLOAT | Critical damage value |
| IFAIL_SH | INT | Shell failure flag |
| IFAIL_SO | INT | Solid failure flag |

### Example
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/FAIL/WILKINS/45
#              ALPHA                BETA                PLIM                  DF  IFAIL_SH  IFAIL_SO
                 1.0                 2.0             -500.0                 1.0         1         1
```

---

## 12. TBUTCHER - Tuler-Butcher

Tuler-Butcher dynamic failure criterion.

### Syntax
```
/FAIL/TBUTCHER/mat_ID
#             LAMBDA                   K             SIGMA_R  IFAIL_SH  IFAIL_SO     IDUCT     IXFEM
#                  A                   B                                    DADV
```

### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| LAMBDA | FLOAT | Exponent |
| K | FLOAT | Critical damage integral |
| SIGMA_R | FLOAT | Fracture stress |
| IFAIL_SH | INT | Shell failure flag |
| IFAIL_SO | INT | Solid failure flag |
| IDUCT | INT | Ductile-brittle flag |
| IXFEM | INT | XFEM flag |
| A | FLOAT | Brittle rupture parameter A |
| B | FLOAT | Brittle rupture parameter B |
| DADV | FLOAT | Crack advancement criterion (0-1) |

### IDUCT Options

| Value | Description |
|-------|-------------|
| 0 | Default |
| 1 | Ductile material |
| 2 | Brittle material (shell only) |

### Example
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/FAIL/TBUTCHER/50
#             LAMBDA                   K             SIGMA_R  IFAIL_SH  IFAIL_SO     IDUCT     IXFEM
                 2.0              0.001              500.0         1         1         1         0
#                  A                   B                                    DADV
                 1.0                 1.0                                     1.0
```

---

## 13. EMC - Extended Mohr-Coulomb

Extended Mohr-Coulomb failure criterion with strain rate sensitivity.

### Syntax
```
/FAIL/EMC/mat_ID
#                  A                   N                  B0                   C
#              GAMMA       EPSILON_DOT_0
```

### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| A | FLOAT | Hosford exponent |
| N | FLOAT | Stress state sensitivity exponent |
| B0 | FLOAT | Strain to fracture for uniaxial tension |
| C | FLOAT | Friction parameter for triaxiality |
| GAMMA | FLOAT | Strain rate sensitivity parameter |
| EPSILON_DOT_0 | FLOAT | Reference strain rate |

### Example
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/FAIL/EMC/55
#                  A                   N                  B0                   C
                 2.0                 0.1                 0.6                0.05
#              GAMMA       EPSILON_DOT_0
                0.02               0.001
```

---

## 14. CONNECT - Connection Element Failure

Failure criterion for connection elements (spot welds, adhesives).

### Syntax
```
/FAIL/CONNECT/mat_ID
#       EPSILON_MAXN          EXPONENT_N             ALPHA_N R_FCT_IDN     IFAIL  IFAIL_SO      ISYM
#       EPSILON_MAXT          EXPONENT_T             ALPHA_T R_FCT_ID_T
#             EI_MAX              EN_MAX              ET_MAX                 N_N                 N_T
#              T_MAX              N_SOFT          AREA_SCALE
```

### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| EPSILON_MAXN | FLOAT | Failure in normal direction |
| EXPONENT_N | FLOAT | Failure exponent (normal) |
| ALPHA_N | FLOAT | Scale factor (normal) |
| R_FCT_IDN | INT | Strain rate function (normal) |
| IFAIL | INT | Failure formulation flag |
| IFAIL_SO | INT | Solid failure flag |
| ISYM | INT | Compression deactivation flag |
| EPSILON_MAXT | FLOAT | Failure in tangential plane |
| EXPONENT_T | FLOAT | Failure exponent (tangential) |
| ALPHA_T | FLOAT | Scale factor (tangential) |
| R_FCT_ID_T | INT | Strain rate function (tangential) |
| EI_MAX | FLOAT | Failure internal energy |
| EN_MAX | FLOAT | Failure energy (normal) |
| ET_MAX | FLOAT | Failure energy (tangential) |
| N_N | FLOAT | Energy exponent (normal) |
| N_T | FLOAT | Energy exponent (tangential) |
| T_MAX | FLOAT | Duration parameter |
| N_SOFT | FLOAT | Softening exponent |
| AREA_SCALE | FLOAT | Area increase scale factor |

### Example
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/FAIL/CONNECT/60
#       EPSILON_MAXN          EXPONENT_N             ALPHA_N R_FCT_IDN     IFAIL  IFAIL_SO      ISYM
                 1.0                 2.0                 1.0         0         1         1         0
#       EPSILON_MAXT          EXPONENT_T             ALPHA_T R_FCT_ID_T
                 2.0                 2.0                 1.0         0
#             EI_MAX              EN_MAX              ET_MAX                 N_N                 N_T
              1000.0               500.0               700.0                 2.0                 2.0
#              T_MAX              N_SOFT          AREA_SCALE
              0.001                 1.0                 1.0
```

---

## 15. SPALLING - Spalling Failure

Spalling failure criterion for high velocity impact applications.

### Syntax
```
/FAIL/SPALLING/mat_ID
#                 D1                  D2                  D3                  D4                  D5
#      EPSILON_DOT_0               P_MIN  IFAIL_SO
```

### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| D1-D5 | FLOAT | Spalling damage parameters |
| EPSILON_DOT_0 | FLOAT | Reference strain rate |
| P_MIN | FLOAT | Pressure cutoff |
| IFAIL_SO | INT | Solid failure flag |

### IFAIL_SO Options

| Value | Description |
|-------|-------------|
| 0 | Default |
| 1 | Deviatoric stress to 0, pressure compressive |
| 2 | Element deleted if condition reached for 1 IP |
| 3 | Deviatoric stress vanished if damage for 1 IP |
| 4 | Element deleted if condition or P=Pmin for 1 IP |

### Example
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/FAIL/SPALLING/65
#                 D1                  D2                  D3                  D4                  D5
                 0.1                 0.5                -1.5                 0.0                 0.0
#      EPSILON_DOT_0               P_MIN  IFAIL_SO
               0.001             -1000.0         2
```

---

## 16. LAD_DAMA - Ladeveze Damage

Ladeveze continuum damage mechanics model for composites.

### Syntax
```
/FAIL/LAD_DAMA/mat_ID
#                 K1                  K2                  K3             GAMMA_1             GAMMA_2
#                 Y0                  YC                   K                   A             TAU_MAX
# IFAIL_SH  IFAIL_SO
```

### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| K1 | FLOAT | Stiffness in direction 1 |
| K2 | FLOAT | Stiffness in direction 2 |
| K3 | FLOAT | Stiffness in direction 3 |
| GAMMA_1 | FLOAT | Coupling factor 1 |
| GAMMA_2 | FLOAT | Coupling factor 2 |
| Y0 | FLOAT | Yield energy damage |
| YC | FLOAT | Critical energy damage |
| K | FLOAT | Crack propagation velocity constant |
| A | FLOAT | Crack propagation velocity parameter |
| TAU_MAX | FLOAT | Dynamic time relaxation |
| IFAIL_SH | INT | Shell failure flag |
| IFAIL_SO | INT | Solid failure flag |

### Example
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/FAIL/LAD_DAMA/70
#                 K1                  K2                  K3             GAMMA_1             GAMMA_2
              5000.0              5000.0              5000.0                 0.5                 0.5
#                 Y0                  YC                   K                   A             TAU_MAX
                10.0               100.0              0.001                 1.0            0.0001
# IFAIL_SH  IFAIL_SO
         1         1
```

---

## 17. NXT - NXT Strain Failure

NXT failure model based on principal stress envelope functions.

### Syntax
```
/FAIL/NXT/mat_ID
#  FCT_ID1   FCT_ID2  IFAIL_SH
```

### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| FCT_ID1 | INT | Function for lower fracture limit |
| FCT_ID2 | INT | Function for upper fracture limit |
| IFAIL_SH | INT | Shell failure flag |

### IFAIL_SH Options

| Value | Description |
|-------|-------------|
| 0 | Shell not deleted |
| 1 | Delete if one layer in failure zone |
| 2 | Delete if all layers in failure zone |

### Example
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/FAIL/NXT/75
#  FCT_ID1   FCT_ID2  IFAIL_SH
       100       101         1
```

---

## 18. RTCL - Rice-Tracey-Cockroft-Latham

RTCL combined failure criterion for ductile metals.

### Syntax
```
/FAIL/RTCL/mat_ID
#             EPScal      Inst                   N
```

### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| EPScal | FLOAT | Simple tension failure strain at reference element size |
| Inst | INT | Mesh sensitivity necking flag |
| N | FLOAT | Hardening exponent |

### Example
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/FAIL/RTCL/80
#             EPScal      Inst                   N
                 0.4         1                 0.2
```

---

## 19. ALTER - Glass Failure

Failure model for automotive glass with subcritical crack growth.

### Syntax
```
/FAIL/ALTER/mat_ID
#              EXP_N                  V0                  VC   NCYCLES     IRATE     ISIDE      MODE
#            CR_FOIL              CR_AIR             CR_CORE             CR_EDGE    GRSH4N    GRSH3N
#                KIC                 KTH                RLEN                TDEL   Out_flg
#              KRES1               KRES2
#               Eta1               Beta1                Tau1            Area_ref
#               Eta2               Beta2                Tau2
#               Sig0             P_scale  P_switch
```

### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| EXP_N | FLOAT | Crack growth exponent |
| V0 | FLOAT | Crack velocity at KIC |
| VC | FLOAT | Maximum crack propagation velocity |
| NCYCLES | INT | Stress filtering period |
| IRATE | INT | Stress filtering method |
| ISIDE | INT | Air/foil side strain rate flag |
| MODE | INT | Failure propagation mode |
| CR_FOIL | FLOAT | Crack depth at PVB surface |
| CR_AIR | FLOAT | Crack depth at air surface |
| CR_CORE | FLOAT | Crack depth in core |
| CR_EDGE | FLOAT | Crack depth at exposed edge |
| GRSH4N | INT | 4-node shell group ID |
| GRSH3N | INT | 3-node shell group ID |
| KIC | FLOAT | Fracture toughness |
| KTH | FLOAT | Fatigue threshold |
| RLEN | FLOAT | Reference length |
| TDEL | FLOAT | Time delay of stress relaxation |

### Example
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/FAIL/ALTER/85
#              EXP_N                  V0                  VC   NCYCLES     IRATE     ISIDE      MODE
                16.0               0.001              1500.0        10         0         0         1
#            CR_FOIL              CR_AIR             CR_CORE             CR_EDGE    GRSH4N    GRSH3N
             2.5e-05             1.5e-04             5.0e-05             2.0e-04         0         0
#                KIC                 KTH                RLEN                TDEL   Out_flg
                0.75                0.20               0.001              0.001         0
#              KRES1               KRES2
                 1.0                 1.0
```

---

## 20. ORTHBIQUAD - Orthotropic Bi-Quadratic

Orthotropic bi-quadratic failure for anisotropic sheet metals.

### Syntax
```
/FAIL/ORTHBIQUAD/mat_ID
#               PTHK     MFLAG     SFLAG    NANGLE                      FCT_IDEL              EI_REF
#                 C5               DEPS0             C_JCOOK         FCT_ID_RATE          RATE_SCALE
#                 R1                  R2                  R4                  R5  (if MFLAG=99)
#                 C1                  C2                  C3                  C4                INST  (per angle)
```

### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| PTHK | FLOAT | Thickness failure percentage |
| MFLAG | INT | Material selector flag |
| SFLAG | INT | Specific behavior flag |
| NANGLE | INT | Number of angles for plastic strains |
| FCT_IDEL | INT | Element size function |
| EI_REF | FLOAT | Reference element size |
| C5 | FLOAT | Biaxial tension failure strain |
| DEPS0 | FLOAT | Reference strain rate |
| C_JCOOK | FLOAT | Johnson-Cook rate parameter |
| FCT_ID_RATE | INT | Strain rate function |
| RATE_SCALE | FLOAT | Strain rate scale factor |
| C1-C4 | FLOAT | Failure strains per angle |
| INST | FLOAT | Necking trigger strain per angle |

---

## 21. SAHRAEI - Battery Cell Failure

Sahraei failure model for lithium-ion battery cell modeling.

### Syntax
```
/FAIL/SAHRAEI/mat_ID
#Fct_ratio       NUM     DENOM     ORDIN          VOL_STRAIN            Fct_IDEL              EL_REF
# COMP_DIR      IDEL     MAX_COMP_STRAIN               RATIO
```

### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| Fct_ratio | INT | Strain ratio function ID |
| NUM | INT | Numerator flag for strain ratio |
| DEN | INT | Denominator flag for strain ratio |
| ORDI | INT | Failure ordinate flag |
| VOL_STRAIN | FLOAT | Volumetric strain limit |
| Fct_IDEL | INT | Element size function |
| EL_REF | FLOAT | Reference element size |
| COMP_DIR | INT | In-plane compression direction |
| IDEL | INT | Compression damage deletion flag |
| MAX_COMP_STRAIN | FLOAT | Compression failure strain |
| RATIO | FLOAT | Strain ratio for compression failure |

---

## Failure Model Selection Guide

### By Application

| Application | Recommended Models |
|-------------|-------------------|
| Crash simulation (metals) | JOHNSON, BIQUAD, TAB2, WIERZBICKI |
| Sheet metal forming | FLD, BIQUAD, ORTHBIQUAD |
| High strain rate impact | JOHNSON, SPALLING, TBUTCHER |
| Composite materials | HASHIN, CHANG, PUCK, LAD_DAMA |
| Spot welds / adhesives | CONNECT, SNCONNECT |
| Void growth / porous metals | GURSON |
| Automotive glass | ALTER |
| Battery cells | SAHRAEI |
| General purpose | GENE1, TENSSTRAIN, ENERGY |

### By Material Type

| Material | Recommended Models |
|----------|-------------------|
| Mild steel | JOHNSON, BIQUAD (M_FLAG=1) |
| High strength steel | BIQUAD (M_FLAG=2), TAB2 |
| Ultra-high strength steel | BIQUAD (M_FLAG=3), WIERZBICKI |
| Aluminum alloys | BIQUAD (M_FLAG=4,5), JOHNSON |
| Unidirectional composites | HASHIN (IFORM=1), CHANG |
| Woven composites | HASHIN (IFORM=2), PUCK |
| Brittle materials | TENSSTRAIN, TBUTCHER (IDUCT=2) |
| Ductile metals | JOHNSON, GURSON, COCKCROFT |
| Polymers | BIQUAD (M_FLAG=6,7), TENSSTRAIN |

---

## Failure Model Comparison

| Model | Strain | Stress | Energy | Rate | Temp | Triaxiality | Lode |
|-------|--------|--------|--------|------|------|-------------|------|
| JOHNSON | Yes | No | No | Yes | Yes | Yes | No |
| BIQUAD | Yes | No | No | No | No | Yes | No |
| FLD | Yes | No | No | No | No | No | No |
| TENSSTRAIN | Yes | No | No | No | Opt | No | No |
| ENERGY | No | No | Yes | Yes | No | No | No |
| HASHIN | No | Yes | No | No | No | No | No |
| CHANG | No | Yes | No | No | No | No | No |
| PUCK | No | Yes | No | No | No | No | No |
| GURSON | Yes | No | No | No | No | Yes | Yes |
| WIERZBICKI | Yes | No | No | No | No | Yes | Yes |
| EMC | Yes | No | No | Yes | No | Yes | Yes |
| TAB2 | Yes | No | No | Yes | Yes | Yes | No |
| CONNECT | Yes | No | Yes | Yes | No | No | No |
| TBUTCHER | No | Yes | No | No | No | No | No |

---

## Common Parameters

### Shell Failure Flags (IFAIL_SH)

| Value | Description |
|-------|-------------|
| 0 | Default behavior |
| 1 | Delete when condition reached for 1 layer/IP |
| 2 | Zero stress when failed, delete when all layers fail |

### Solid Failure Flags (IFAIL_SO)

| Value | Description |
|-------|-------------|
| 0 | Default behavior |
| 1 | Delete when condition reached for 1 IP |
| 2 | Deviatoric stress vanished when condition reached |

### XFEM Integration

Several failure models support XFEM (eXtended Finite Element Method) for crack propagation:
- JOHNSON (IXFEM=1)
- FLD (IXFEM=1)
- TBUTCHER (IXFEM=1)
- XFEM_FLD, XFEM_JOHNS, XFEM_TBUTC (dedicated XFEM models)

---

## Best Practices

### General Guidelines

1. **Mesh sensitivity**: Use element size regularization (FCT_IDEL, EI_REF) for mesh-independent results
2. **Strain rate effects**: Include rate dependency for dynamic simulations
3. **Multiple criteria**: Combine failure models using /FAIL/GENE1 for complex scenarios
4. **Validation**: Calibrate failure parameters using material test data

### Calibration Order

1. Start with simple models (TENSSTRAIN) for initial validation
2. Progress to more complex models (JOHNSON, BIQUAD) for accuracy
3. Use tabulated models (TAB2) when extensive test data is available

### Composite Materials

1. Use HASHIN for 3D composites with through-thickness effects
2. Use CHANG for simpler 2D laminate analysis
3. Use PUCK when inter-fiber failure is critical
4. Always specify TAU_MAX for numerical stability

---

## Troubleshooting

| Issue | Cause | Solution |
|-------|-------|----------|
| Premature failure | Failure strain too low | Increase failure parameters |
| No failure | Failure strain too high | Decrease failure parameters |
| Mesh dependent results | No regularization | Add FCT_IDEL and EI_REF |
| Numerical instability | TAU_MAX too large | Reduce TAU_MAX |
| Unrealistic crack path | XFEM not enabled | Set IXFEM=1 |
| Composite failure issues | Wrong IFORM | Match IFORM to laminate type |

---

## References

- Source files: `hm_cfg_files/config/CFG/radioss*/FAIL/`
- Starter source: `starter/source/materials/fail/`
- Engine source: `engine/source/materials/fail/`
