# OpenRadioss Keyword Manual - Materials (Complete Reference)

## Overview

This document provides a complete reference for all material laws available in OpenRadioss. Materials define the constitutive behavior of elements including elasticity, plasticity, viscosity, damage, and failure.

---

## Material Keyword Syntax

```
/MAT/law_name/mat_ID
mat_title
#              RHO_I
[material parameters...]
```

Common parameters for all materials:
- **mat_ID**: Unique material identifier
- **mat_title**: Material description (up to 100 characters)
- **RHO_I**: Initial density [M/L³]

---

## Material Law Categories

| Category | Laws | Description |
|----------|------|-------------|
| Elastic | LAW1, LAW69, LAW71 | Linear and nonlinear elastic |
| Elasto-Plastic | LAW2, LAW27, LAW32, LAW36, LAW43, LAW44, LAW48, LAW60, LAW63, LAW64, LAW66, LAW72, LAW73, LAW74, LAW76, LAW78, LAW84, LAW87, LAW93 | Metal plasticity |
| Visco-Elastic | LAW33, LAW34, LAW35, LAW38, LAW40, LAW42, LAW62, LAW70, LAW77, LAW82, LAW88, LAW90, LAW94 | Time-dependent behavior |
| Hydrodynamic | LAW3, LAW4, LAW5, LAW6, LAW37, LAW41, LAW51, LAW97 | Fluid and high-pressure |
| Composite | LAW12, LAW14, LAW15, LAW19, LAW25, LAW58 | Fiber-reinforced materials |
| Foam/Honeycomb | LAW28, LAW33, LAW50, LAW68, LAW70 | Cellular materials |
| Damage | LAW22, LAW23, LAW52, LAW79, LAW81 | Progressive damage |
| Connection | LAW54, LAW59, LAW80, LAW83 | Spot welds, rivets |
| Concrete/Rock | LAW10, LAW21, LAW24, LAW81, LAW124 | Geomaterials |
| User | LAW29, LAW30, LAW31 | User-defined materials |

---

## 1. LAW1 - ELAST (Linear Elastic)

Linear elastic isotropic material for general structural analysis.

### Syntax
```
/MAT/ELAST/mat_ID
mat_title
#              RHO_I
#                  E                  nu
```

### Parameters

| Parameter | Type | Unit | Description |
|-----------|------|------|-------------|
| RHO_I | FLOAT | [M/L³] | Initial density |
| E | FLOAT | [P] | Young's modulus |
| nu | FLOAT | [-] | Poisson's ratio (0 < nu < 0.5) |

### Constitutive Equation
```
σ = E × ε (uniaxial)
σ = D × ε (3D, where D is elastic stiffness matrix)
```

### Example
```
/MAT/ELAST/1
Steel_Elastic
              7850.0
           210000.0               0.30
```

### Applicable Elements
- Solid (BRICK, TETRA4, TETRA10, PENTA6)
- Shell (SHELL, SH3N)
- Beam, Truss, Spring

---

## 2. LAW2 - PLAS_JOHNS (Johnson-Cook Plasticity)

Elasto-plastic material with Johnson-Cook strain rate and temperature dependency. Most commonly used for metal impact simulation.

### Syntax
```
/MAT/PLAS_JOHNS/mat_ID
mat_title
#              RHO_I
#                  E                  nu
#Iflag
#                  a                   b                   n          eps_max          sig_max
#                  c           eps_dot0               ICC           Fsmooth             Fcut
#                  m              T_melt                  Cp               T_r            T_max
#              P_min
```

### Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| RHO_I | FLOAT | - | Initial density |
| E | FLOAT | - | Young's modulus |
| nu | FLOAT | - | Poisson's ratio |
| Iflag | INT | 0 | Input type flag |
| a | FLOAT | - | Yield stress (Johnson-Cook A) |
| b | FLOAT | - | Hardening modulus (Johnson-Cook B) |
| n | FLOAT | 1.0 | Hardening exponent |
| eps_max | FLOAT | 1E20 | Maximum plastic strain for element deletion |
| sig_max | FLOAT | 1E20 | Maximum stress |
| c | FLOAT | 0.0 | Strain rate coefficient |
| eps_dot0 | FLOAT | 1.0 | Reference strain rate |
| ICC | INT | 1 | Strain rate formulation (1=original, 2=modified) |
| Fsmooth | INT | 0 | Strain rate smoothing flag |
| Fcut | FLOAT | 1E20 | Cutoff frequency for filtering |
| m | FLOAT | 1.0 | Temperature softening exponent |
| T_melt | FLOAT | 1E20 | Melting temperature |
| Cp | FLOAT | - | Specific heat capacity per unit volume |
| T_r | FLOAT | 298 | Reference (room) temperature |
| T_max | FLOAT | 1E20 | Maximum temperature |
| P_min | FLOAT | -1E20 | Minimum pressure (tension cutoff) |

### Iflag Values

| Value | Description |
|-------|-------------|
| 0 | Classic Johnson-Cook input |
| 1 | Simplified input (yield stress, tangent modulus) |

### ICC Values

| Value | Description |
|-------|-------------|
| 1 | σ = (a + b×εp^n) × (1 + c×ln(ε̇/ε̇₀)) |
| 2 | σ = (a + b×εp^n) × (ε̇/ε̇₀)^c |

### Johnson-Cook Model
```
σ = (a + b × εp^n) × (1 + c × ln(ε̇/ε̇₀)) × (1 - T*^m)

where:
  T* = (T - Tr) / (Tmelt - Tr)
  εp = equivalent plastic strain
  ε̇ = plastic strain rate
```

### Example
```
/MAT/PLAS_JOHNS/2
Steel_Johnson_Cook
              7850.0
           210000.0               0.30
         0
               350.0              275.0               0.36              1.00              1E20
              0.022               1.0                   1         0               1E20
               1.0              1793.0           3.5E+06             298.0               1E20
            -1E+20
```

### Material Data Examples

| Material | a (MPa) | b (MPa) | n | c | m |
|----------|---------|---------|---|---|---|
| AISI 4340 Steel | 792 | 510 | 0.26 | 0.014 | 1.03 |
| OFHC Copper | 90 | 292 | 0.31 | 0.025 | 1.09 |
| Al 2024-T351 | 265 | 426 | 0.34 | 0.015 | 1.0 |
| Ti-6Al-4V | 1098 | 1092 | 0.93 | 0.014 | 1.1 |

### Applicable Elements
- Solid, Shell

---

## 3. LAW3 - HYDPLA (Hydrodynamic Elasto-Plastic)

Hydrodynamic material with plasticity for high-pressure shock applications.

### Syntax
```
/MAT/HYDPLA/mat_ID
mat_title
#              RHO_I
#                  E                  nu
#                  a                   b                   n          eps_max          sig_max
#                 C0                  C1                  C2                  C3
#              P_min                 P_sh
#                 C4                  C5                  E0
```

### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| RHO_I | FLOAT | Initial density |
| E | FLOAT | Young's modulus |
| nu | FLOAT | Poisson's ratio |
| a, b, n | FLOAT | Plasticity parameters (same as LAW2) |
| eps_max | FLOAT | Maximum plastic strain |
| sig_max | FLOAT | Maximum stress |
| C0-C3 | FLOAT | Polynomial EOS coefficients |
| P_min | FLOAT | Minimum pressure (tension cutoff) |
| P_sh | FLOAT | Pressure shift |
| C4, C5 | FLOAT | Energy coefficients |
| E0 | FLOAT | Initial internal energy per unit volume |

### Polynomial Equation of State
```
P = C0 + C1×μ + C2×μ² + C3×μ³ + (C4 + C5×μ)×E

where:
  μ = ρ/ρ₀ - 1 (compression)
  E = internal energy per unit volume
```

### Example
```
/MAT/HYDPLA/3
Steel_Hydrodynamic
              7850.0
           210000.0               0.30
               350.0              275.0               0.36              1.00              1E20
           1.57E+05           1.92E+06                 0.0                 0.0
             -1E+20                 0.0
                0.5                 0.0                 0.0
```

### Applicable Elements
- Solid (Lagrangian, ALE, Eulerian)

---

## 4. LAW4 - HYD_JCOOK (Hydrodynamic Johnson-Cook)

Combines hydrodynamic EOS with full Johnson-Cook plasticity including temperature effects.

### Syntax
```
/MAT/HYD_JCOOK/mat_ID
mat_title
#              RHO_I
#                  E                  nu
#                  a                   b                   n          eps_max          sig_max
#                  c           eps_dot0
#                  m              T_melt                  Cp               T_r
#                 C0                  C1                  C2                  C3
#              P_min                 P_sh
#                 C4                  C5                  E0
```

### Parameters
Combines parameters from LAW2 (Johnson-Cook) and LAW3 (Hydrodynamic).

### Example
```
/MAT/HYD_JCOOK/4
Steel_HYD_JC
              7850.0
           210000.0               0.30
               350.0              275.0               0.36              1.00              1E20
              0.022               1.0
               1.0              1793.0           3.5E+06             298.0
           1.57E+05           1.92E+06                 0.0                 0.0
             -1E+20                 0.0
                0.5                 0.0                 0.0
```

### Applicable Elements
- Solid

---

## 5. LAW5 - JWL (Jones-Wilkins-Lee Explosive)

Equation of state for detonation products of high explosives.

### Syntax
```
/MAT/JWL/mat_ID
mat_title
#              RHO_I
#                  A                   B                  R1                  R2               OMEGA
#                  D                P_CJ                  E0
```

### Parameters

| Parameter | Type | Unit | Description |
|-----------|------|------|-------------|
| RHO_I | FLOAT | [M/L³] | Initial density |
| A | FLOAT | [P] | JWL parameter A |
| B | FLOAT | [P] | JWL parameter B |
| R1 | FLOAT | [-] | JWL parameter R1 |
| R2 | FLOAT | [-] | JWL parameter R2 |
| OMEGA | FLOAT | [-] | Grüneisen coefficient ω |
| D | FLOAT | [L/T] | Detonation velocity |
| P_CJ | FLOAT | [P] | Chapman-Jouguet pressure |
| E0 | FLOAT | [P] | Initial detonation energy per unit volume |

### JWL Equation of State
```
P = A × (1 - ω/(R1×V)) × exp(-R1×V) + B × (1 - ω/(R2×V)) × exp(-R2×V) + ω×E/V

where:
  V = ρ₀/ρ (relative volume)
  E = internal energy per unit volume
```

### Example - TNT
```
/MAT/JWL/5
TNT_Explosive
              1630.0
           3.712E+08           3.231E+06               4.15               0.95               0.30
              6930.0           2.10E+07           7.00E+06
```

### Common JWL Parameters

| Explosive | ρ₀ (kg/m³) | A (GPa) | B (GPa) | R1 | R2 | ω | D (m/s) | P_CJ (GPa) |
|-----------|------------|---------|---------|----|----|---|---------|------------|
| TNT | 1630 | 371.2 | 3.231 | 4.15 | 0.95 | 0.30 | 6930 | 21.0 |
| C-4 | 1601 | 609.8 | 12.95 | 4.50 | 1.40 | 0.25 | 8193 | 28.0 |
| PETN | 1770 | 617.0 | 16.93 | 4.40 | 1.20 | 0.25 | 8300 | 33.5 |

### Applicable Elements
- Solid (ALE, Eulerian, SPH)

---

## 6. LAW6 - HYDRO (Hydrodynamic Fluid)

Simple hydrodynamic material for fluids without strength.

### Syntax
```
/MAT/HYDRO/mat_ID
mat_title
#              RHO_I
#                 C0                  C1                  C2                  C3
#              P_min                 P_sh
#                 C4                  C5                  E0
```

### Parameters
Same polynomial EOS as LAW3, but without deviatoric stress (pure fluid).

### Example - Water
```
/MAT/HYDRO/6
Water
              1000.0
                 0.0           2.20E+06                 0.0                 0.0
             -1E+20                 0.0
                0.0                 0.0                 0.0
```

### Applicable Elements
- Solid (ALE, Eulerian, SPH)

---

## 7. LAW10 - DPRAG (Drucker-Prager Rock/Soil)

Elasto-plastic material with Drucker-Prager yield criterion for geomaterials.

### Syntax
```
/MAT/DPRAG/mat_ID  (or /MAT/LAW10/)
mat_title
#              RHO_I
#                  E                  nu
#                 A0                  A1                  A2                Amax
#                 C0                  C1                  C2                  C3
#              P_min
#                  B              Mu_max
```

### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| E | FLOAT | Young's modulus |
| nu | FLOAT | Poisson's ratio |
| A0, A1, A2 | FLOAT | Drucker-Prager yield coefficients |
| Amax | FLOAT | Maximum von Mises stress squared |
| C0-C3 | FLOAT | Polynomial EOS coefficients |
| P_min | FLOAT | Minimum pressure (< 0, tension cutoff) |
| B | FLOAT | Unloading bulk modulus |
| Mu_max | FLOAT | Maximum compression μ = ρ/ρ₀ - 1 |

### Drucker-Prager Yield Criterion
```
J2 = A0 + A1×P + A2×P²

where:
  J2 = second invariant of deviatoric stress
  P = pressure
```

### Example
```
/MAT/DPRAG/10
Concrete_DP
              2400.0
            30000.0               0.20
           1.0E+06           2.0E+03                 0.0           1.0E+08
           2.0E+04           5.0E+04                 0.0                 0.0
            -5000.0
           3.0E+04               0.20
```

### Applicable Elements
- Solid

---

## 8. LAW14 - COMPSO (Composite Solid)

Orthotropic composite material for solid elements.

### Syntax
```
/MAT/COMPSO/mat_ID
mat_title
#              RHO_I
#                 E1                  E2                  E3
#                Nu21                Nu31                Nu32
#                 G12                 G23                 G31
#              sig1_c              sig1_t              sig2_c              sig2_t
#             sig12_s             sig23_s              wp_max
```

### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| E1, E2, E3 | FLOAT | Young's moduli in directions 1, 2, 3 |
| Nu21, Nu31, Nu32 | FLOAT | Poisson's ratios |
| G12, G23, G31 | FLOAT | Shear moduli |
| sig1_c, sig1_t | FLOAT | Compressive/tensile strength direction 1 |
| sig2_c, sig2_t | FLOAT | Compressive/tensile strength direction 2 |
| sig12_s, sig23_s | FLOAT | Shear strengths |
| wp_max | FLOAT | Maximum plastic work for failure |

### Example
```
/MAT/COMPSO/14
Carbon_Fiber_Solid
              1600.0
           135000.0            10000.0            10000.0
               0.02               0.02               0.30
             5000.0             3500.0             5000.0
              1500.0             2000.0              200.0              60.0
               100.0               60.0               0.05
```

### Applicable Elements
- Solid (TYPE6, TYPE14)

---

## 9. LAW15 - CHANG (Chang-Chang Composite Shell)

Chang-Chang failure model for fiber-reinforced composite shells.

### Syntax
```
/MAT/CHANG/mat_ID
mat_title
#              RHO_I
#                 E1                  E2               Nu12
#                 G12                 G23                 G31
#             sigma_1t            sigma_1c            sigma_2t            sigma_2c
#             sigma_12               beta       wpmax_fiber       wpmax_matrix
```

### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| E1, E2 | FLOAT | In-plane Young's moduli |
| Nu12 | FLOAT | In-plane Poisson's ratio |
| G12, G23, G31 | FLOAT | Shear moduli |
| sigma_1t/c | FLOAT | Fiber tensile/compressive strength |
| sigma_2t/c | FLOAT | Matrix tensile/compressive strength |
| sigma_12 | FLOAT | Shear strength |
| beta | FLOAT | Shear stress weighting factor |
| wpmax_fiber | FLOAT | Max plastic work for fiber failure |
| wpmax_matrix | FLOAT | Max plastic work for matrix failure |

### Chang-Chang Failure Criteria

**Fiber Tension (σ₁ > 0):**
```
(σ₁/Xt)² + β×(σ₁₂/S)² ≥ 1
```

**Fiber Compression (σ₁ < 0):**
```
(σ₁/Xc)² ≥ 1
```

**Matrix Tension (σ₂ > 0):**
```
(σ₂/Yt)² + (σ₁₂/S)² ≥ 1
```

**Matrix Compression (σ₂ < 0):**
```
(σ₂/(2×S))² + [(Yc/(2×S))² - 1]×(σ₂/Yc) + (σ₁₂/S)² ≥ 1
```

### Example
```
/MAT/CHANG/15
CFRP_Shell
              1550.0
           120000.0             8000.0               0.30
             4500.0             3000.0             4500.0
              2100.0             1200.0               80.0              180.0
               100.0               0.50               0.10               0.05
```

### Applicable Elements
- Shell (TYPE10)

---

## 10. LAW19 - FABRI (Elastic Fabric)

Elastic fabric material with no compressive stiffness.

### Syntax
```
/MAT/FABRI/mat_ID
mat_title
#              RHO_I
#                 E1                  E2               Nu12
#                 G12                 G23                 G31
```

### Features
- No compressive stiffness (fabric behavior)
- Membrane elements only
- Suitable for airbag, seatbelt fabric

### Example
```
/MAT/FABRI/19
Airbag_Fabric
               800.0
             500.0              500.0               0.30
              200.0              200.0              200.0
```

### Applicable Elements
- Shell (membrane, TYPE16)

---

## 11. LAW21 - DPRAG2 (Drucker-Prager Variant)

Alternative Drucker-Prager formulation for soils and concrete.

### Syntax
Same as LAW10 with additional parameters for associative/non-associative flow.

---

## 12. LAW22 - DAMA (Damage Material)

Elastic-plastic material with isotropic damage.

### Syntax
```
/MAT/DAMA/mat_ID
mat_title
#              RHO_I
#                  E                  nu
#                  a                   b                   n
#              eps_d             eps_max             D_max               alpha
```

### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| a, b, n | FLOAT | Plasticity parameters |
| eps_d | FLOAT | Plastic strain at damage initiation |
| eps_max | FLOAT | Plastic strain at failure |
| D_max | FLOAT | Maximum damage (0-1) |
| alpha | FLOAT | Damage exponent |

### Damage Evolution
```
D = D_max × ((εp - εd) / (εmax - εd))^α

Effective stress: σ_eff = σ / (1 - D)
```

### Example
```
/MAT/DAMA/22
Steel_Damage
              7850.0
           210000.0               0.30
               250.0              400.0               0.50
               0.10               0.80               0.99               2.0
```

### Applicable Elements
- Solid, Shell

---

## 13. LAW24 - CONC (Concrete)

Concrete material with compression cap and tensile cutoff.

### Syntax
```
/MAT/CONC/mat_ID
mat_title
#              RHO_I
#                  E                  nu
#                 fc                  ft               beta
#             Gf_ten             Gf_com
```

### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| fc | FLOAT | Compressive strength (> 0) |
| ft | FLOAT | Tensile strength |
| beta | FLOAT | Shear retention factor |
| Gf_ten | FLOAT | Tensile fracture energy |
| Gf_com | FLOAT | Compressive fracture energy |

### Example
```
/MAT/CONC/24
Concrete_30MPa
              2400.0
            30000.0               0.20
                30.0               3.0               0.5
               0.10               5.0
```

### Applicable Elements
- Solid

---

## 14. LAW25 - COMPSH (Composite Shell)

General composite shell material with various failure options.

### Syntax
```
/MAT/COMPSH/mat_ID
mat_title
#              RHO_I
#                 E1                  E2               Nu12
#                 G12                 G23                 G31
#            alpha_1            alpha_2            alpha_12
```

### Example
```
/MAT/COMPSH/25
Glass_Fiber_Shell
              1900.0
            40000.0            10000.0               0.28
             4000.0             3000.0             4000.0
           8.0E-06           2.5E-05                 0.0
```

### Applicable Elements
- Shell (TYPE10, TYPE11)

---

## 15. LAW27 - PLAS_BRIT (Brittle Plastic)

Elasto-plastic with directional damage for brittle materials.

### Syntax
```
/MAT/PLAS_BRIT/mat_ID
mat_title
#              RHO_I
#                  E                  nu
#                  a                   b                   n          eps_max          sig_max
#                  c           eps_dot0
#          eps_t1           eps_m1              d1           eps_f1
#          eps_t2           eps_m2              d2           eps_f2
```

### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| eps_t1/2 | FLOAT | Tensile failure strain direction 1/2 |
| eps_m1/2 | FLOAT | Maximum tensile strain |
| d1/2 | FLOAT | Maximum damage factor |
| eps_f1/2 | FLOAT | Element deletion strain |

### Example
```
/MAT/PLAS_BRIT/27
Ceramic_Brittle
              3900.0
           380000.0               0.22
              3000.0             2000.0               0.50              0.10              5000.0
              0.001               1.0
              0.002               0.010               0.99               0.015
              0.002               0.010               0.99               0.015
```

### Applicable Elements
- Shell

---

## 16. LAW28 - HONEYCOMB (Honeycomb Crushable)

Orthotropic honeycomb material with different crush behaviors per direction.

### Syntax
```
/MAT/HONEYCOMB/mat_ID
mat_title
#              RHO_I
#                  E                  nu
#                 G12                 G23                 G31
#       fct_ID11  Fscale11       fct_ID22  Fscale22       fct_ID33  Fscale33
#       fct_ID12  Fscale12       fct_ID23  Fscale23       fct_ID31  Fscale31
```

### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| E | FLOAT | Reference Young's modulus |
| G12, G23, G31 | FLOAT | Shear moduli |
| fct_ID11/22/33 | INT | Function IDs for normal stress vs strain |
| fct_ID12/23/31 | INT | Function IDs for shear stress vs strain |
| Fscale | FLOAT | Scale factors for functions |

### Example
```
/MAT/HONEYCOMB/28
Aluminum_Honeycomb
               50.0
             500.0               0.01
              50.0               25.0               50.0
       1        1.0       2        1.0       1        1.0
       3        1.0       3        1.0       3        1.0
```

### Applicable Elements
- Solid (TYPE14)

---

## 17. LAW32 - HILL (Hill Orthotropic Plasticity)

Anisotropic plasticity using Hill's yield criterion with Lankford coefficients.

### Syntax
```
/MAT/HILL/mat_ID
mat_title
#              RHO_I
#                  E                  nu
#              sig_y                   b                   n          eps_max          sig_max
#                R00                 R45                 R90
#           eps_dot0                   c
```

### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| sig_y | FLOAT | Initial yield stress |
| b | FLOAT | Hardening modulus |
| n | FLOAT | Hardening exponent |
| R00 | FLOAT | Lankford parameter at 0° to rolling |
| R45 | FLOAT | Lankford parameter at 45° |
| R90 | FLOAT | Lankford parameter at 90° |
| eps_dot0 | FLOAT | Reference strain rate |
| c | FLOAT | Strain rate coefficient |

### Hill Yield Function
```
F×(σ22-σ33)² + G×(σ33-σ11)² + H×(σ11-σ22)² + 2L×τ23² + 2M×τ31² + 2N×τ12² = σ_y²

where F, G, H, L, M, N are computed from R00, R45, R90
```

### Example
```
/MAT/HILL/32
Aluminum_Sheet_6061
              2700.0
            70000.0               0.33
               250.0              350.0               0.45              1E20              1E20
               0.85               1.10               0.90
               0.01               0.01
```

### Applicable Elements
- Shell, Solid

---

## 18. LAW33 - FOAM_PLAS (Plastic Foam)

Low density foam material with optional viscoelasticity.

### Syntax
```
/MAT/FOAM_PLAS/mat_ID
mat_title
#              RHO_I
#                  E        Ka  func_ID          Fscale
#                 P0                 Phi             Gamma_0
#                  A                   B                   C
# (if Ka=1):
#                 E1                  E2                  Et            eta_comp           eta_shear
```

### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| E | FLOAT | Young's modulus |
| Ka | INT | Analysis type (0=elastic, 1=visco-elastic) |
| func_ID | INT | Function ID for yield stress vs volumetric strain |
| Fscale | FLOAT | Y-scale factor for function |
| P0 | FLOAT | Initial air pressure |
| Phi | FLOAT | Ratio of foam to polymer density |
| Gamma_0 | FLOAT | Initial volumetric strain |
| A, B, C | FLOAT | Yield parameters if no function |
| E1, E2 | FLOAT | Viscoelastic modulus coefficients |
| Et | FLOAT | Tangent modulus |
| eta_comp/shear | FLOAT | Viscosity coefficients |

### Foam Pressure Model
```
P_air = P0 × (1 - Phi) / (1 - Phi + Phi×(1 - εv))

where εv = volumetric strain
```

### Example
```
/MAT/FOAM_PLAS/33
Polyurethane_Foam
                60.0
             200.0         0       100        1.0
               0.1               0.02                 0.0
               0.5               1.0                0.01
```

### Applicable Elements
- Solid

---

## 19. LAW34 - BOLTZ (Boltzmann Viscoelastic)

Viscoelastic material using Boltzmann hereditary integral.

### Syntax
```
/MAT/BOLTZ/mat_ID
mat_title
#              RHO_I
#                 G0
# N_terms
#                 Gi                 tau_i
# [repeat for each term]
#                  K
```

### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| G0 | FLOAT | Long-term shear modulus |
| N_terms | INT | Number of Prony series terms |
| Gi | FLOAT | Shear modulus for term i |
| tau_i | FLOAT | Relaxation time for term i |
| K | FLOAT | Bulk modulus |

### Boltzmann Model
```
G(t) = G0 + Σ Gi × exp(-t/τi)
```

### Example
```
/MAT/BOLTZ/34
Rubber_Viscoelastic
              1100.0
               0.50
         3
               0.20               0.001
               0.15               0.01
               0.10               0.1
            2000.0
```

### Applicable Elements
- Solid, Shell

---

## 20. LAW35 - FOAM_VISC (Viscous Foam)

Rate-dependent foam with hysteresis.

### Syntax
```
/MAT/FOAM_VISC/mat_ID
mat_title
#              RHO_I
#                  E                  nu
#  N_fct
#       fct_ID  Fscale   eps_dot
# [repeat for each function]
#            Hyst               Damp
```

### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| E | FLOAT | Young's modulus |
| nu | FLOAT | Poisson's ratio |
| N_fct | INT | Number of stress-strain curves |
| fct_ID | INT | Function ID |
| Fscale | FLOAT | Scale factor |
| eps_dot | FLOAT | Strain rate for this curve |
| Hyst | FLOAT | Hysteresis unloading factor |
| Damp | FLOAT | Damping coefficient |

### Example
```
/MAT/FOAM_VISC/35
EPP_Foam
                30.0
             100.0               0.01
         3
       100        1.0        0.001
       101        1.0        1.0
       102        1.0        100.0
               0.10               0.05
```

### Applicable Elements
- Solid

---

## 21. LAW36 - PLAS_TAB (Tabulated Plasticity)

Piecewise linear plasticity with multiple strain rate curves.

### Syntax
```
/MAT/PLAS_TAB/mat_ID
mat_title
#              RHO_I
#                  E                  nu           Eps_max           Eps_t1           Eps_t2
#  N_fct
# fct_ID    Fscale   eps_dot
# [repeat for each function]
#          Fsmooth             Fcut            alpha       fct_Xr      Fscale_Xr           Eps_f
```

### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| N_fct | INT | Number of stress-strain curves |
| fct_ID | INT | Function ID for σ vs εp |
| Fscale | FLOAT | Scale factor for stress |
| eps_dot | FLOAT | Strain rate for this curve |
| Fsmooth | INT | Strain rate smoothing (0=off, 1=on) |
| Fcut | FLOAT | Cutoff frequency |
| alpha | FLOAT | Isotropic/kinematic hardening ratio |
| fct_Xr | INT | Function for pressure-dependent yield |
| Eps_f | FLOAT | Failure strain |

### Example
```
/MAT/PLAS_TAB/36
Steel_Tabulated
              7850.0
           210000.0               0.30              1E20               0.5               0.8
         3
       100        1.0       0.001
       101        1.1       1.0
       102        1.3       1000.0
         0              1E20               0.0         0               1.0              1E20
```

### Applicable Elements
- Solid, Shell

---

## 22. LAW40 - KELVINMAX (Kelvin-Maxwell Viscoelastic)

Generalized Maxwell viscoelastic model.

### Syntax
```
/MAT/KELVINMAX/mat_ID
mat_title
#              RHO_I
#                 G0                 K0                  nu
# N_Maxwell
#                 Gi              tau_i
# [repeat for each Maxwell element]
# N_Kelvin
#                 Gj              tau_j
# [repeat for each Kelvin element]
```

### Example
```
/MAT/KELVINMAX/40
Polymer_KM
              1200.0
               1.0E+03           2.0E+03               0.45
         2
               0.5E+03           0.001
               0.3E+03           0.01
         1
               0.2E+03           0.1
```

### Applicable Elements
- Solid, Shell

---

## 23. LAW42 - OGDEN (Ogden Hyperelastic)

Hyperelastic material for rubber and elastomers using Ogden strain energy function.

### Syntax
```
/MAT/OGDEN/mat_ID
mat_title
#              RHO_I                  nu           sig_cut
# N_order
#              mu_1            alpha_1
#              mu_2            alpha_2
# [up to 10 terms]
```

### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| RHO_I | FLOAT | Initial density |
| nu | FLOAT | Poisson's ratio (use 0.495-0.4999 for incompressibility) |
| sig_cut | FLOAT | Tensile stress cutoff |
| N_order | INT | Number of Ogden terms (1-10) |
| mu_i | FLOAT | Shear modulus for term i |
| alpha_i | FLOAT | Ogden exponent for term i |

### Ogden Strain Energy Function
```
W = Σ (μi/αi) × (λ1^αi + λ2^αi + λ3^αi - 3)

where λ1, λ2, λ3 = principal stretches
```

### Special Cases
- Mooney-Rivlin: N=2, α1=2, α2=-2
- Neo-Hookean: N=1, α1=2

### Example
```
/MAT/OGDEN/42
Natural_Rubber
              1100.0               0.4999            1E+10
         3
               0.63               1.30
              0.0012              -2.00
             -0.010               5.00
```

### Applicable Elements
- Solid, Shell

---

## 24. LAW43 - HILL_TAB (Tabulated Hill)

Tabulated Hill orthotropic plasticity with multiple curves.

### Syntax
```
/MAT/HILL_TAB/mat_ID
mat_title
#              RHO_I
#                  E                  nu
#                R00                 R45                 R90
# N_fct
# fct_ID    Fscale   eps_dot
# [repeat for each function]
```

### Example
```
/MAT/HILL_TAB/43
Steel_Sheet_Tabulated
              7850.0
           210000.0               0.30
               0.90               1.20               1.00
         3
       200        1.0       0.001
       201        1.0       1.0
       202        1.0       100.0
```

### Applicable Elements
- Shell

---

## 25. LAW44 - COWPER (Cowper-Symonds)

Elasto-plastic with Cowper-Symonds strain rate hardening.

### Syntax
```
/MAT/COWPER/mat_ID
mat_title
#              RHO_I
#                  E                  nu
#              sig_y                   b                   n           alpha          sig_max
#                  D                   p          eps_max
#            eps_t1            eps_t2
#               ICC           Fsmooth             Fcut
```

### Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| sig_y | FLOAT | - | Initial yield stress |
| b | FLOAT | - | Hardening modulus |
| n | FLOAT | - | Hardening exponent |
| alpha | FLOAT | 0 | Isotropic/kinematic parameter (0-1) |
| sig_max | FLOAT | 1E20 | Maximum stress |
| D | FLOAT | - | Cowper-Symonds D parameter |
| p | FLOAT | - | Cowper-Symonds p parameter |
| eps_max | FLOAT | 1E20 | Failure plastic strain |
| eps_t1/t2 | FLOAT | - | Tensile failure strains |
| ICC | INT | 1 | Strain rate formulation |
| Fsmooth | INT | 0 | Strain rate smoothing |
| Fcut | FLOAT | 1E20 | Cutoff frequency |

### Cowper-Symonds Model
```
σ = (σy + b×εp^n) × (1 + (ε̇/D)^(1/p))
```

### Common Parameters

| Material | D (s⁻¹) | p |
|----------|---------|---|
| Mild Steel | 40.4 | 5 |
| High Strength Steel | 3200 | 5 |
| Aluminum Alloy | 6500 | 4 |
| Stainless Steel | 100 | 10 |

### Example
```
/MAT/COWPER/44
Mild_Steel_CS
              7850.0
           210000.0               0.30
               235.0              400.0               0.50               0.0              500.0
              40.4                5.0              1E20
               0.0                0.0
         1         0              1E20
```

### Applicable Elements
- Solid, Shell

---

## 26. LAW48 - ZHAO (Zhao Plasticity)

Enhanced strain rate formulation with multiple rate mechanisms.

### Syntax
```
/MAT/ZHAO/mat_ID
mat_title
#              RHO_I
#                  E                  nu
#              sig_y                   b                   n           alpha          sig_max
#                  C                   D                   m               E1                   K           eps_dot0
#          eps_max            eps_t1            eps_t2
#             Fcut
```

### Parameters
Similar to LAW44 with additional C, D, m, E1, K rate parameters.

### Applicable Elements
- Solid, Shell

---

## 27. LAW49 - STEINB (Steinberg-Guinan)

High strain rate plasticity for metals under shock loading.

### Syntax
```
/MAT/STEINB/mat_ID
mat_title
#              RHO_I
#                 G0                 Y0                Ymax
#                  b                   n               Gp'               Yp'
#               beta               GT'
#                 C0                  S1                  S2                  S3
#              gamma               eta_0
```

### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| G0 | FLOAT | Initial shear modulus |
| Y0 | FLOAT | Initial yield strength |
| Ymax | FLOAT | Maximum yield strength |
| Gp' | FLOAT | Pressure derivative of G |
| Yp' | FLOAT | Pressure derivative of Y |
| GT' | FLOAT | Temperature derivative of G |
| C0 | FLOAT | Bulk sound speed |
| S1, S2, S3 | FLOAT | Hugoniot parameters |
| gamma | FLOAT | Grüneisen coefficient |

### Steinberg Model
```
G = G0 × (1 + Gp'×P/η^(1/3) - GT'×(T-300))
Y = Y0 × (1 + β×εp)^n × (1 + Yp'×P/η^(1/3))  [Y ≤ Ymax]
```

### Example
```
/MAT/STEINB/49
Copper_Steinberg
              8930.0
            47700.0              120.0              640.0
               0.36              0.45            0.00130           0.000695
               1.0E-05         -0.00180
            3940.0               1.49               0.0               0.0
               2.0           0.000010
```

### Applicable Elements
- Solid

---

## 28. LAW50 - VISC_HONEY (Viscous Honeycomb)

Rate-dependent orthotropic crushable foam/honeycomb.

### Syntax
Similar to LAW28 with added rate dependency.

### Applicable Elements
- Solid (TYPE14)

---

## 29. LAW52 - GURSON (Gurson Damage Model)

Elasto-plastic with Gurson-Tvergaard-Needleman ductile damage.

### Syntax
```
/MAT/GURSON/mat_ID
mat_title
#              RHO_I
#                  E                  nu     Iflag   Fsmooth                Fcut
#                  A                   B                   N                   c                   p
#                 q1                  q2                  q3                  SN                EpsN
#                 Fi                  FN                  Fc                  FF
```

### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| Iflag | INT | Yield criterion flag |
| A, B, N | FLOAT | Plasticity parameters |
| c, p | FLOAT | Cowper-Symonds rate parameters |
| q1, q2, q3 | FLOAT | Gurson parameters (typically q1=1.5, q2=1.0, q3=q1²) |
| SN | FLOAT | Standard deviation for nucleation |
| EpsN | FLOAT | Mean strain for nucleation |
| Fi | FLOAT | Initial void volume fraction |
| FN | FLOAT | Nucleated void volume fraction |
| Fc | FLOAT | Critical void fraction at coalescence |
| FF | FLOAT | Void fraction at failure |

### Gurson Yield Function
```
Φ = (σeq/σm)² + 2×q1×f* ×cosh(3×q2×P/(2×σm)) - 1 - q3×f*² = 0

where:
  f* = modified void volume fraction
  σm = matrix flow stress
```

### Void Evolution
```
ḟ = ḟgrowth + ḟnucleation
ḟgrowth = (1-f) × ε̇p_kk
ḟnucleation = A(εp) × ε̇p_eq
```

### Example
```
/MAT/GURSON/52
Steel_Gurson
              7850.0
           210000.0               0.30         0         0              1E20
               400.0              600.0               0.40               0.0               0.0
               1.50               1.00               2.25               0.10               0.30
              0.001              0.040               0.15               0.25
```

### Applicable Elements
- Solid, Shell

---

## 30. LAW54 - CONNECTION (Spotweld Material)

Material for spot weld and connection elements.

### Syntax
```
/MAT/CONNECT/mat_ID
mat_title
#              RHO_I
#                  E                  nu
#              sig_y                Etan           eps_max           sig_max
#      fct_IDn       Nn  fct_IDs       Ns   fct_IDsr     Nsr
#              F_max                M_max
```

### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| fct_IDn | INT | Function for normal force vs displacement |
| fct_IDs | INT | Function for shear force vs displacement |
| fct_IDsr | INT | Function for strain rate |
| F_max | FLOAT | Maximum force |
| M_max | FLOAT | Maximum moment |

### Example
```
/MAT/CONNECT/54
Spotweld_Mat
              7850.0
           210000.0               0.30
               400.0            1000.0               0.20             500.0
       100        1.0       101        1.0       102        1.0
             5000.0              50.0
```

### Applicable Elements
- Spring (TYPE8, TYPE13)

---

## 31. LAW57 - BARLAT3 (Barlat 3-Parameter)

Barlat-Lian yield criterion for sheet metals.

### Syntax
```
/MAT/BARLAT3/mat_ID
mat_title
#              RHO_I
#                  E                  nu
#                  a                   h                   p
# N_fct
# fct_ID    Fscale   eps_dot
```

### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| a | FLOAT | Barlat exponent (6 for BCC, 8 for FCC) |
| h | FLOAT | Anisotropy parameter |
| p | FLOAT | Second anisotropy parameter |

### Barlat Yield Function
```
Φ = a×|K1 + K2|^m + a×|K1 - K2|^m + (2-a)×|2×K2|^m = 2×σy^m
```

### Example
```
/MAT/BARLAT3/57
Aluminum_Barlat
              2700.0
            70000.0               0.33
               8.0               1.05               0.95
         2
       200        1.0       0.001
       201        1.1       100.0
```

### Applicable Elements
- Shell

---

## 32. LAW58 - FABR_A (Advanced Fabric)

Nonlinear fabric with progressive failure.

### Syntax
```
/MAT/FABR_A/mat_ID
mat_title
#              RHO_I
#                 E1                  E2               Nu12
#  N_fct
# fct_ID1  Fscale1  fct_ID2  Fscale2  fct_ID12  Fscale12
#           eps_f1           eps_f2          eps_f12
```

### Applicable Elements
- Shell (TYPE16)

---

## 33. LAW59 - CONNECT2 (Advanced Connection)

Enhanced connection element material with detailed failure.

### Applicable Elements
- Spring (TYPE8, TYPE13)

---

## 34. LAW62 - VISC_HYP (Viscous Hyperelastic)

Hyperelastic with viscous damping for rubber.

### Syntax
Similar to LAW42 with added Prony series viscosity.

### Applicable Elements
- Solid, Shell

---

## 35. LAW63 - Tabulated Elastoplastic

Enhanced piecewise linear plasticity.

### Applicable Elements
- Solid, Shell

---

## 36. LAW64 - Tabulated Orthotropic

Tabulated orthotropic material with multiple failure modes.

### Applicable Elements
- Shell

---

## 37. LAW65 - ELASTOMER

Elastomer material with Mullins effect.

### Applicable Elements
- Solid, Shell

---

## 38. LAW69 - ELAST_HYST (Elastic with Hysteresis)

Nonlinear elastic with hysteresis.

### Syntax
```
/MAT/ELAST_HYST/mat_ID
mat_title
#              RHO_I                  nu
# N_order
#              mu_i            alpha_i
# [repeat]
#              Hyst
```

### Applicable Elements
- Solid

---

## 39. LAW70 - FOAM_TAB (Tabulated Foam)

Tabulated crushable foam with unloading.

### Syntax
```
/MAT/FOAM_TAB/mat_ID
mat_title
#              RHO_I                  nu
#  N_fct
#       fct_ID  Fscale   eps_dot
#              Hyst              Damp            shape
```

### Example
```
/MAT/FOAM_TAB/70
EPP_Foam_Tab
                35.0               0.0
         3
       100        1.0       0.001
       101        1.0       1.0
       102        1.0       100.0
               0.10               0.05               1.0
```

### Applicable Elements
- Solid

---

## 40. LAW72 - HILL_MMC (Hill with Mohr-Coulomb)

Hill orthotropic with Mohr-Coulomb-Max failure.

### Applicable Elements
- Shell

---

## 41. LAW76 - SAMP (Semi-Analytical Model for Polymers)

Rate and temperature dependent polymer model.

### Applicable Elements
- Solid, Shell

---

## 42. LAW77 - Generalized Maxwell

Advanced viscoelastic with multiple mechanisms.

### Applicable Elements
- Solid, Shell

---

## 43. LAW78 - YOSHIDA_UEMORI

Kinematic hardening model for cyclic loading.

### Syntax
```
/MAT/YOSHIDA_UEMORI/mat_ID
mat_title
#              RHO_I
#                  E                  nu
#              sig_y                   B                   C                Rsat
#                  b                   m                 h_l                h_sat
```

### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| B | FLOAT | Initial size of bounding surface |
| C | FLOAT | Material parameter |
| Rsat | FLOAT | Saturated value of R |
| b | FLOAT | Parameter for R |
| m | FLOAT | Bounding surface exponent |

### Applicable Elements
- Shell

---

## 44. LAW79 - JOHNSON_HOLMQUIST

Johnson-Holmquist model for ceramics and glass.

### Syntax
```
/MAT/JOHNSON_HOLMQUIST/mat_ID
mat_title
#              RHO_I
#                  G                   K
#                  A                   B                   C                   M                   N
#          sig_HEL              P_HEL               T_max
#                 D1                  D2               FS_max
#                 K1                  K2                  K3                 beta
```

### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| G | FLOAT | Shear modulus |
| K | FLOAT | Bulk modulus |
| A | FLOAT | Intact strength coefficient |
| B | FLOAT | Fractured strength coefficient |
| C | FLOAT | Strain rate coefficient |
| M | FLOAT | Pressure exponent (fractured) |
| N | FLOAT | Pressure exponent (intact) |
| sig_HEL | FLOAT | Hugoniot elastic limit stress |
| P_HEL | FLOAT | Pressure at HEL |
| D1, D2 | FLOAT | Damage parameters |
| K1, K2, K3 | FLOAT | Pressure constants |

### JH2 Model
```
σ* = A×(P* + T*)^N × (1 + C×ln(ε̇*))  [intact]
σ*f = B×(P*)^M × (1 + C×ln(ε̇*))     [fractured]
σ = σi - D×(σi - σf)                  [partially damaged]
```

### Example - Alumina Ceramic
```
/MAT/JOHNSON_HOLMQUIST/79
Alumina_JH2
              3890.0
           1.52E+05           2.20E+05
               0.93               0.31              0.007               0.60               0.60
             2600.0             1460.0              260.0
              0.005               1.00               1.00
           2.20E+05           0.00E+00           0.00E+00               1.0
```

### Applicable Elements
- Solid

---

## 45. LAW80 - CONNECT3 (General Connection)

General connection material with 6 DOF.

### Applicable Elements
- Spring (TYPE8)

---

## 46. LAW81 - Rock Damage

Coupled damage-plasticity for rocks.

### Applicable Elements
- Solid

---

## 47. LAW82 - MOONEY (Mooney-Rivlin)

Mooney-Rivlin hyperelastic.

### Syntax
```
/MAT/MOONEY/mat_ID
mat_title
#              RHO_I                  nu
#                C10                 C01                 C20
```

### Mooney-Rivlin Energy
```
W = C10×(I1-3) + C01×(I2-3) + C20×(I1-3)²
```

### Example
```
/MAT/MOONEY/82
Silicone_Rubber
              1100.0               0.499
               0.20               0.05                 0.0
```

### Applicable Elements
- Solid

---

## 48. LAW83 - CONNECTION4

Advanced spot weld with strain rate.

### Applicable Elements
- Spring (TYPE8, TYPE13)

---

## 49. LAW84 - Tabulated Anisotropic

General tabulated anisotropic material.

### Applicable Elements
- Shell

---

## 50. LAW87 - BARLAT2000 (Barlat Yld2000-2d)

Advanced Barlat yield criterion for aluminum sheets.

### Syntax
```
/MAT/BARLAT2000/mat_ID
mat_title
#              RHO_I
#                  E                  nu
#                  a
#             alpha1             alpha2             alpha3             alpha4
#             alpha5             alpha6             alpha7             alpha8
```

### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| a | FLOAT | Barlat exponent (8 for FCC aluminum) |
| alpha1-8 | FLOAT | Anisotropy coefficients |

### Applicable Elements
- Shell

---

## 51. LAW88 - Viscoplastic Tabulated

Viscoplastic with tabulated functions.

### Applicable Elements
- Solid, Shell

---

## 52. LAW90 - Tabulated Foam with Damage

Rate-dependent foam with progressive damage.

### Applicable Elements
- Solid

---

## 53. LAW92 - Elastic Orthotropic

General orthotropic elastic.

### Applicable Elements
- Solid, Shell

---

## 54. LAW93 - ORTHO_HILL

Orthotropic Hill plasticity.

### Applicable Elements
- Shell

---

## 55. LAW94 - YEOH (Yeoh Hyperelastic)

Yeoh hyperelastic for rubber.

### Syntax
```
/MAT/YEOH/mat_ID
mat_title
#              RHO_I                  nu
#                C10                 C20                 C30
```

### Yeoh Energy
```
W = C10×(I1-3) + C20×(I1-3)² + C30×(I1-3)³
```

### Applicable Elements
- Solid

---

## 56. LAW95 - Elastic Viscoplastic

General elastic-viscoplastic material.

### Applicable Elements
- Solid

---

## 57. LAW100 - Advanced Tabulated

Comprehensive tabulated material.

### Applicable Elements
- Solid, Shell

---

## 58. LAW103 - Hensel-Spittel

Hot forming material with temperature dependency.

### Applicable Elements
- Solid, Shell

---

## 59. LAW104 - Drucker Enhanced

Enhanced Drucker-Prager for polymers.

### Applicable Elements
- Solid

---

## 60. LAW106 - Advanced Foam

General foam with damage.

### Applicable Elements
- Solid

---

## 61. LAW108 - SPRING_MAT

General spring material law.

### Applicable Elements
- Spring

---

## 62. LAW109 - Advanced Metal

Comprehensive metal plasticity.

### Applicable Elements
- Solid, Shell

---

## 63. LAW110 - VEGTER

Vegter yield criterion for sheet metals.

### Applicable Elements
- Shell

---

## 64. LAW111 - MARLOW (Marlow Hyperelastic)

Test data-based hyperelastic.

### Applicable Elements
- Solid

---

## 65. LAW112 - PAPER

Paper and cardboard material.

### Applicable Elements
- Shell

---

## 66. LAW113 - Advanced Composite

Enhanced composite with progressive failure.

### Applicable Elements
- Solid, Shell

---

## 67. LAW114 - Foam Damage

Foam with anisotropic damage.

### Applicable Elements
- Solid

---

## 68. LAW115 - Deshpande-Fleck

Foam with Deshpande-Fleck yield.

### Applicable Elements
- Solid

---

## 69. LAW116 - Cohesive Mixed Mode

Cohesive zone for delamination.

### Applicable Elements
- Solid (TYPE43)

---

## 70. LAW117 - Linear Cohesive

Linear mixed-mode cohesive.

### Applicable Elements
- Solid (TYPE43)

---

## 71. LAW119 - Advanced Rate

Advanced rate-dependent material.

### Applicable Elements
- Solid, Shell

---

## 72. LAW120 - Adhesive Polymer

Toughened adhesive material.

### Applicable Elements
- Solid (TYPE43)

---

## 73. LAW121-122 - Enhanced Plasticity

Advanced plasticity models.

### Applicable Elements
- Solid, Shell

---

## 74. LAW124 - CDPM2 (Concrete Damage Plasticity)

Comprehensive concrete damage model.

### Applicable Elements
- Solid

---

## 75. LAW125-129 - Advanced Models

Various advanced material models for specific applications.

---

## 76. LAW163 - CRUSHABLE_FOAM

Crushable foam with volumetric hardening.

### Syntax
```
/MAT/CRUSHABLE_FOAM/mat_ID
mat_title
#              RHO_I
#                  E                  nu
#  fct_ID    Fscale
#            Damp              Hyst
```

### Applicable Elements
- Solid

---

## 77. LAW169 - Rupture Cohesive

Cohesive element with rupture.

### Applicable Elements
- Solid (TYPE43)

---

## 78. User-Defined Materials (LAW29, LAW30, LAW31)

User-programmable material subroutines.

### Applicable Elements
- Depends on user implementation

---

## Material Selection Guide

### By Application

| Application | Recommended Laws |
|-------------|------------------|
| Static elastic | LAW1 |
| Metal plasticity (general) | LAW2, LAW36, LAW44 |
| High-speed impact | LAW2, LAW3, LAW4, LAW49 |
| Sheet metal forming | LAW32, LAW43, LAW57, LAW78, LAW87, LAW110 |
| Rubber/Elastomers | LAW42, LAW62, LAW69, LAW82, LAW94, LAW111 |
| Composites (shell) | LAW15, LAW25 |
| Composites (solid) | LAW14 |
| Concrete | LAW10, LAW24, LAW124 |
| Ceramics | LAW27, LAW79 |
| Foam (crushable) | LAW33, LAW35, LAW70, LAW163 |
| Honeycomb | LAW28, LAW50 |
| Explosives | LAW5 |
| Fluids | LAW6, LAW16 |
| Ductile damage | LAW22, LAW52 |
| Spot welds | LAW54, LAW59, LAW80, LAW83 |
| Adhesives | LAW116, LAW117, LAW120 |
| Polymers | LAW76 |
| Cyclic loading | LAW78 |

### By Element Type

| Element | Applicable Laws |
|---------|-----------------|
| SHELL | LAW1, LAW2, LAW15, LAW25, LAW27, LAW32, LAW36, LAW42, LAW44, LAW57, LAW87 |
| SOLID | LAW1-LAW6, LAW10, LAW14, LAW21, LAW24, LAW33, LAW42, LAW49, LAW52, LAW79 |
| BEAM | LAW1 |
| SPRING | LAW1, LAW54, LAW108 |
| SPH | LAW1, LAW5, LAW6 |

---

## Best Practices

### General Guidelines

1. **Unit Consistency**: Ensure all input parameters use consistent units
2. **Density Check**: Verify density is reasonable for element size
3. **Poisson's Ratio**: Keep nu < 0.5 (use 0.499 for near-incompressible)
4. **Rate Dependency**: Use appropriate strain rate parameters for dynamic analysis
5. **Failure Criteria**: Combine with /FAIL cards for element deletion

### Numerical Stability

1. **Bulk Modulus**: K = E / (3×(1-2×nu)), check for high nu
2. **Shear Modulus**: G = E / (2×(1+nu))
3. **Time Step**: Material stiffness affects critical time step
4. **Mass Scaling**: May be needed for stiff materials

### Troubleshooting

| Issue | Possible Cause | Solution |
|-------|----------------|----------|
| Negative volume | Excessive compression | Increase bulk modulus or add P_min |
| Hourglassing | Low stiffness | Use appropriate hourglass control |
| Instability | High rate dependency | Reduce strain rate coefficient |
| No plasticity | Wrong input | Check yield stress and hardening |

---

## References

- CFG Files: `hm_cfg_files/config/CFG/radioss*/MAT/`
- Starter Source: `starter/source/materials/mat/`
- Engine Source: `engine/source/materials/mat_share/`

