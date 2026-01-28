# OpenRadioss Keyword Manual - Equations of State (EOS)

## Overview

This document describes the Equation of State (EOS) keywords in OpenRadioss. EOS defines the relationship between pressure, density, and internal energy for hydrodynamic materials, essential for shock wave propagation, explosion, and high-velocity impact simulations.

---

## EOS Keyword Syntax

```
/EOS/type/eos_ID
eos_title
# Type-specific parameters
```

---

## Complete EOS Type Reference

| Type | Description | Applications |
|------|-------------|--------------|
| GRUNEISEN | Mie-Gruneisen EOS | Metals under shock loading |
| POLYNOMIAL | Polynomial EOS | General fluids, explosives |
| PUFF | Puff EOS | Porous materials with phase change |
| TILLOTSON | Tillotson EOS | Hypervelocity impact, planetary science |
| LINEAR | Linear EOS | Simple compressible fluids |
| MURNAGHAN | Murnaghan EOS | Solids under moderate compression |
| NOBLE-ABEL | Noble-Abel EOS | Propellant gases with covolume |
| IDEAL-GAS-VE | Ideal gas (V,E) | Low pressure gases (energy-based) |
| IDEAL-GAS-VT | Ideal gas (V,T) | Low pressure gases (temperature-based) |
| STIFF-GAS | Stiff gas EOS | Liquids modeled as gas |
| COMPACTION | Compaction EOS | Porous/crushable materials |
| COMPACTION2 | Tabulated compaction | User-defined compaction curve |
| COMPACTION_TAB | Advanced tabulated compaction | Multi-function compaction |
| NASG | Noble-Abel Stiffened Gas | Two-phase flow, cavitation |
| OSBORNE | Osborne EOS | Explosives, detonation products |
| LSZK | LSZK EOS | Water, liquids under high pressure |
| SESAME | SESAME tables | Tabulated EOS from databases |
| TABULATED | Tabulated EOS | User-defined tables |
| EXPONENTIAL | Exponential EOS | Soft materials, foams |
| POWDER-BURN | Powder burn EOS | Propellant combustion, solid rocket |

---

## EOS Categories

### Shock/Metal EOS
- **GRUNEISEN**: Standard for metals under shock (most commonly used)
- **POLYNOMIAL**: General purpose, also for explosives (JWL-like)
- **PUFF**: Porous metals with phase transitions
- **TILLOTSON**: Hypervelocity impact with vaporization

### Gas EOS
- **IDEAL-GAS-VE**: Ideal gas with energy variable
- **IDEAL-GAS-VT**: Ideal gas with temperature variable
- **NOBLE-ABEL**: Real gas with covolume correction
- **STIFF-GAS**: Liquid approximation as stiffened gas
- **NASG**: Combined Noble-Abel and stiffened gas

### Compaction EOS
- **COMPACTION**: Polynomial compaction for porous materials
- **COMPACTION2**: Tabulated compaction curve
- **COMPACTION_TAB**: Advanced multi-function compaction

### Liquid EOS
- **LINEAR**: Simple bulk modulus model
- **MURNAGHAN**: Nonlinear bulk modulus
- **LSZK**: Water under extreme pressure

### Special Purpose
- **OSBORNE**: Explosive detonation products
- **EXPONENTIAL**: Soft/viscoelastic materials
- **POWDER-BURN**: Propellant combustion
- **SESAME/TABULATED**: User-defined data

---

## 1. EOS/GRUNEISEN - Mie-Gruneisen Equation of State

Most common EOS for metals under shock loading. Based on Hugoniot reference curve.

### Syntax
```
/EOS/GRUNEISEN/eos_ID
eos_title
#                  C                  S1                  S2                  S3
#                 Y0                   a                  E0
#                 P0                 PSH
```

### Parameters

| Parameter | Type | Description | Default |
|-----------|------|-------------|---------|
| C | FLOAT | Sound speed (bulk) | - |
| S1 | FLOAT | First Hugoniot slope coefficient | - |
| S2 | FLOAT | Second Hugoniot slope coefficient | 0.0 |
| S3 | FLOAT | Third Hugoniot slope coefficient | 0.0 |
| Y0 (GAMMA) | FLOAT | Gruneisen gamma coefficient | - |
| a | FLOAT | First-order volume correction to gamma | 0.0 |
| E0 | FLOAT | Initial energy per unit reference volume | 0.0 |
| P0 | FLOAT | Initial pressure | 0.0 |
| PSH | FLOAT | Pressure shift | 0.0 |

### Formulation

For compression (μ > 0):
```
P = [ρ₀C²μ(1 + (1-γ₀/2)μ - a/2·μ²)] / [1 - (S1-1)μ - S2·μ²/(μ+1) - S3·μ³/(μ+1)²] + (γ₀ + a·μ)E
```

For expansion (μ < 0):
```
P = ρ₀C²μ + (γ₀ + a·μ)E
```

Where: μ = ρ/ρ₀ - 1 (relative volume change)

### Example
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/EOS/GRUNEISEN/1
Steel_4340_EOS
#                  C                  S1                  S2                  S3
             4570.0              1.490                 0.0                 0.0
#                 Y0                   a                  E0
               1.93               0.47                 0.0
#                 P0                 PSH
                0.0                 0.0
```

---

## 2. EOS/POLYNOMIAL - Polynomial Equation of State

General-purpose EOS with polynomial pressure-density relationship. Common for explosives and fluids.

### Syntax
```
/EOS/POLYNOMIAL/eos_ID
eos_title
#                 C0                  C1                  C2                  C3
#                 C4                  C5                  E0                P_sh                  C6
```

### Parameters

| Parameter | Type | Description | Default |
|-----------|------|-------------|---------|
| C0 | FLOAT | Pressure constant term | 0.0 |
| C1 | FLOAT | Linear pressure coefficient | - |
| C2 | FLOAT | Quadratic pressure coefficient | 0.0 |
| C3 | FLOAT | Cubic pressure coefficient | 0.0 |
| C4 | FLOAT | Linear energy coefficient | 0.0 |
| C5 | FLOAT | Energy-density coefficient | 0.0 |
| C6 | FLOAT | Additional coefficient (radioss2026+) | 0.0 |
| E0 | FLOAT | Initial internal energy per unit volume | 0.0 |
| P_sh | FLOAT | Pressure shift | 0.0 |

### Formulation

For compression (μ > 0):
```
P = C0 + C1·μ + C2·μ² + C3·μ³ + (C4 + C5·μ)·E
```

For expansion (μ < 0):
```
P = C0 + C1·μ + (C4 + C5·μ)·E
```

### JWL Equivalent
For explosive detonation products, use JWL coefficients mapped to polynomial:
```
C0 = A·exp(-R1·V₀) + B·exp(-R2·V₀) - A·R1 - B·R2
C1 = A·R1·exp(-R1) + B·R2·exp(-R2)
C4 = C5 = ω (omega)
```

### Example
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/EOS/POLYNOMIAL/2
Water_EOS
#                 C0                  C1                  C2                  C3
                0.0           2.25E+09           9.54E+09           1.46E+10
#                 C4                  C5                  E0                P_sh                  C6
               0.28               0.28                 0.0                 0.0                 0.0
```

---

## 3. EOS/PUFF - Puff Equation of State

For porous materials with phase transitions under shock loading.

### Syntax
```
/EOS/PUFF/eos_ID
eos_title
#                 C1                  C2                  C3             Gamma_0
#                 T1                  T2                  ES
#                  H                  E0
```

### Parameters

| Parameter | Type | Description | Default |
|-----------|------|-------------|---------|
| C1 | FLOAT | Pressure coefficient 1 | - |
| C2 | FLOAT | Pressure coefficient 2 | - |
| C3 | FLOAT | Pressure coefficient 3 | 0.0 |
| Gamma_0 | FLOAT | Gruneisen gamma at reference | - |
| T1 | FLOAT | Temperature coefficient 1 | 0.0 |
| T2 | FLOAT | Temperature coefficient 2 | 0.0 |
| ES | FLOAT | Sublimation energy per unit mass | - |
| H | FLOAT | H coefficient for phase transition | 0.0 |
| E0 | FLOAT | Initial energy per unit mass | 0.0 |

### Example
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/EOS/PUFF/3
Porous_Aluminum_EOS
#                 C1                  C2                  C3             Gamma_0
           7.50E+10           1.50E+11                 0.0                2.0
#                 T1                  T2                  ES
           1.0E+07            2.0E+06            1.2E+07
#                  H                  E0
                3.0                 0.0
```

---

## 4. EOS/TILLOTSON - Tillotson Equation of State

For hypervelocity impact and planetary science applications with vaporization.

### Syntax
```
/EOS/TILLOTSON/eos_ID
eos_title
#                 C1                  C2                   a                   b
#                 ER                  ES                  VS                  EO
#              Alpha                Beta
```

### Parameters

| Parameter | Type | Description | Default |
|-----------|------|-------------|---------|
| C1 | FLOAT | Bulk modulus parameter | - |
| C2 | FLOAT | Compression parameter | - |
| a | FLOAT | EOS parameter (typically 0.5) | 0.5 |
| b | FLOAT | EOS parameter | - |
| ER | FLOAT | Internal energy at reference | - |
| ES | FLOAT | Sublimation energy per unit mass | - |
| VS | FLOAT | Sublimation relative volume | - |
| E0 | FLOAT | Initial energy per unit mass | 0.0 |
| Alpha | FLOAT | Exponential decay parameter | 5.0 |
| Beta | FLOAT | Exponential decay parameter | 5.0 |

### Formulation

Compressed state (μ > 0):
```
P = [a + b/(E/(E₀·η²)+1)]·ρ·E + C1·μ + C2·μ²
```

Expanded state with partial vaporization:
```
P = a·ρ·E + [b·ρ·E/(E/(E₀·η²)+1) + C1·μ·exp(-β(ρ₀/ρ-1))]·exp(-α(ρ₀/ρ-1)²)
```

### Example
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/EOS/TILLOTSON/4
Aluminum_Impact_EOS
#                 C1                  C2                   a                   b
           7.68E+10           6.50E+10                 0.5                1.63
#                 ER                  ES                  VS                  EO
           5.0E+06            3.0E+06                 0.9                 0.0
#              Alpha                Beta
                5.0                 5.0
```

---

## 5. EOS/MURNAGHAN - Murnaghan Equation of State

For solids under moderate compression with pressure-dependent bulk modulus.

### Syntax
```
/EOS/MURNAGHAN/eos_ID
eos_title
#                 K0                  K1                  P0                 PSH
```

### Parameters

| Parameter | Type | Description | Default |
|-----------|------|-------------|---------|
| K0 | FLOAT | Bulk modulus at reference state | - |
| K1 | FLOAT | Pressure derivative of bulk modulus (dK/dP) | 4.0 |
| P0 | FLOAT | Initial pressure | 0.0 |
| PSH | FLOAT | Pressure shift | 0.0 |

### Formulation
```
P = (K0/K1)·[(ρ/ρ₀)^K1 - 1] + P0 + PSH
```

### Example
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/EOS/MURNAGHAN/5
Solid_Compression_EOS
#                 K0                  K1                  P0                 PSH
          1.62E+11                 4.0                 0.0                 0.0
```

---

## 6. EOS/OSBORNE - Osborne Equation of State

For explosive detonation products and high-energy reactions.

### Syntax
```
/EOS/OSBORNE/eos_ID
eos_title
#                 A1                  A2                  B0                  B1                  B2
#             EOS_C0                  C1                  D0                  P0
```

### Parameters

| Parameter | Type | Description | Default |
|-----------|------|-------------|---------|
| A1 | FLOAT | Osborne A parameter 1 | - |
| A2 | FLOAT | Osborne A parameter 2 | - |
| B0 | FLOAT | Osborne B parameter 0 | - |
| B1 | FLOAT | Osborne B parameter 1 | - |
| B2 | FLOAT | Osborne B parameter 2 | 0.0 |
| C0 | FLOAT | Osborne C parameter 0 | 0.0 |
| C1 | FLOAT | Osborne C parameter 1 | 0.0 |
| D0 | FLOAT | Osborne D parameter | 0.0 |
| P0 | FLOAT | Initial pressure | 0.0 |

### Example
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/EOS/OSBORNE/6
Explosive_Product_EOS
#                 A1                  A2                  B0                  B1                  B2
           8.5E+18           5.2E+18           1.2E+10           5.5E+09                 0.0
#             EOS_C0                  C1                  D0                  P0
               0.35                 0.0            1.0E+09                 0.0
```

---

## 7. EOS/LSZK - LSZK Equation of State

For water and liquids under high pressure conditions.

### Syntax
```
/EOS/LSZK/eos_ID
eos_title
#              Gamma                  P0                 PSH                   a                   b
```

### Parameters

| Parameter | Type | Description | Default |
|-----------|------|-------------|---------|
| Gamma | FLOAT | Gruneisen coefficient | - |
| P0 | FLOAT | Initial pressure | 0.0 |
| PSH | FLOAT | Pressure shift | 0.0 |
| a | FLOAT | Pressure coefficient | - |
| b | FLOAT | Density exponent | - |

### Formulation
```
P = a·[(ρ/ρ₀)^b - 1] + Γ·ρ·E + P0 + PSH
```

### Example (Water)
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/EOS/LSZK/7
Water_High_Pressure_EOS
#              Gamma                  P0                 PSH                   a                   b
               0.35           101325.0                 0.0           3.04E+08                 7.15
```

---

## 8. EOS/NOBLE-ABEL - Noble-Abel Gas Equation

For real gas effects with covolume correction (propellant gases).

### Syntax
```
/EOS/NOBLE-ABEL/eos_ID
eos_title
#                  b               Gamma                  E0                 PSH
```

### Parameters

| Parameter | Type | Description | Default |
|-----------|------|-------------|---------|
| b | FLOAT | Covolume (1/density at maximum compression) | - |
| Gamma | FLOAT | Heat capacity ratio (Cp/Cv) | - |
| E0 | FLOAT | Initial internal energy per unit volume | 0.0 |
| PSH | FLOAT | Pressure shift | 0.0 |

### Formulation
```
P = (γ - 1)·ρ·E / (1 - b·ρ) + PSH
```

### Example
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/EOS/NOBLE-ABEL/8
Propellant_Gas_EOS
#                  b               Gamma                  E0                 PSH
            0.00095                1.25           5.0E+05                 0.0
```

---

## 9. EOS/STIFF-GAS - Stiffened Gas Equation

For modeling liquids as stiffened gas (water, biological tissues).

### Syntax
```
/EOS/STIFF-GAS/eos_ID
eos_title
#              Gamma                  P0                 PSH              P_star
```

### Parameters

| Parameter | Type | Description | Default |
|-----------|------|-------------|---------|
| Gamma | FLOAT | Heat capacity ratio | - |
| P0 | FLOAT | Initial pressure | 0.0 |
| PSH | FLOAT | Pressure shift | 0.0 |
| P_star | FLOAT | Stiffening pressure (P∞) | - |

### Formulation
```
P = (γ - 1)·ρ·E - γ·P* + PSH
```

### Example (Water as Stiffened Gas)
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/EOS/STIFF-GAS/9
Water_Stiffened_Gas_EOS
#              Gamma                  P0                 PSH              P_star
                4.4           101325.0                 0.0            6.0E+08
```

---

## 10. EOS/IDEAL-GAS-VE - Ideal Gas (Volume-Energy)

For ideal gas behavior with energy as the thermodynamic variable.

### Syntax
```
/EOS/IDEAL-GAS-VE/eos_ID
eos_title
#              Gamma                  P0                 PSH                  T0
```

### Parameters

| Parameter | Type | Description | Default |
|-----------|------|-------------|---------|
| Gamma | FLOAT | Heat capacity ratio (Cp/Cv) | - |
| P0 | FLOAT | Initial pressure | 101325.0 |
| PSH | FLOAT | Pressure shift | 0.0 |
| T0 | FLOAT | Initial temperature | 293.15 |

### Formulation
```
P = (γ - 1)·ρ·E + PSH
E = Cv·T = P / [(γ-1)·ρ]
```

### Example (Air)
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/EOS/IDEAL-GAS-VE/10
Air_EOS
#              Gamma                  P0                 PSH                  T0
                1.4           101325.0                 0.0              293.15
```

---

## 11. EOS/IDEAL-GAS-VT - Ideal Gas (Volume-Temperature)

For ideal gas behavior with temperature as the thermodynamic variable and variable heat capacity.

### Syntax
```
/EOS/IDEAL-GAS-VT/eos_ID
eos_title
#                  r                  T0                  P0                 PSH
#                 A0                  A1                  A2                  A3                  A4
```

### Parameters

| Parameter | Type | Description | Default |
|-----------|------|-------------|---------|
| r | FLOAT | Specific gas constant (R/M) | - |
| T0 | FLOAT | Initial temperature | 293.15 |
| P0 | FLOAT | Initial pressure | 101325.0 |
| PSH | FLOAT | Pressure shift | 0.0 |
| A0-A4 | FLOAT | Polynomial coefficients for Cv(T) | A0=Cv, others=0 |

### Formulation
```
P = ρ·r·T + PSH
Cv(T) = A0 + A1·T + A2·T² + A3·T³ + A4·T⁴
```

### Example
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/EOS/IDEAL-GAS-VT/11
Air_Variable_Cv_EOS
#                  r                  T0                  P0                 PSH
              287.0              293.15           101325.0                 0.0
#                 A0                  A1                  A2                  A3                  A4
              717.0               0.05                 0.0                 0.0                 0.0
```

---

## 12. EOS/LINEAR - Linear Equation of State

Simplest EOS for weakly compressible materials.

### Syntax
```
/EOS/LINEAR/eos_ID
eos_title
#                 P0                   B                 PSH
```

### Parameters

| Parameter | Type | Description | Default |
|-----------|------|-------------|---------|
| P0 | FLOAT | Initial pressure | 0.0 |
| B | FLOAT | Bulk modulus | - |
| PSH | FLOAT | Pressure shift | 0.0 |

### Formulation
```
P = P0 + B·μ + PSH
```

Where μ = ρ/ρ₀ - 1

### Example
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/EOS/LINEAR/12
Simple_Fluid_EOS
#                 P0                   B                 PSH
                0.0            2.2E+09                 0.0
```

---

## 13. EOS/COMPACTION - Compaction EOS for Porous Materials

For crushable/porous materials with irreversible compaction.

### Syntax
```
/EOS/COMPACTION/eos_ID
eos_title
#                 C0                  C1                  C2                  C3               IFORM
#            Mue_min             Mue_max                   B
#                PSH
```

### Parameters

| Parameter | Type | Description | Default |
|-----------|------|-------------|---------|
| C0 | FLOAT | Pressure coefficient 0 | 0.0 |
| C1 | FLOAT | Pressure coefficient 1 | - |
| C2 | FLOAT | Pressure coefficient 2 | 0.0 |
| C3 | FLOAT | Pressure coefficient 3 | 0.0 |
| IFORM | INT | Unload formulation flag | 0 |
| Mue_min | FLOAT | Elastic limit (μ at onset of compaction) | - |
| Mue_max | FLOAT | Maximum compaction (μ at full density) | - |
| B | FLOAT | Unloading bulk modulus | - |
| PSH | FLOAT | Pressure shift | 0.0 |

### IFORM Values

| Value | Description |
|-------|-------------|
| 0 | Standard unloading with bulk modulus B |
| 1 | Unload to zero pressure at locked density |

### Formulation

Loading (μ between Mue_min and Mue_max):
```
P = C0 + C1·μ + C2·μ² + C3·μ³
```

Unloading:
```
P = B·(μ - μ_plastic)
```

### Example
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/EOS/COMPACTION/13
Foam_Compaction_EOS
#                 C0                  C1                  C2                  C3               IFORM
                0.0            5.0E+07            1.0E+08            2.0E+08                   0
#            Mue_min             Mue_max                   B
               0.05                 0.7            1.0E+09
#                PSH
                0.0
```

---

## 14. EOS/COMPACTION2 - Tabulated Compaction EOS

User-defined compaction curve for porous materials.

### Syntax
```
/EOS/COMPACTION2/eos_ID
eos_title
#           P_fun_id              Fscale              Xscale                                   IFORM
#            Mue_min             Mue_max                Bmin                Bmax
#                PSH
```

### Parameters

| Parameter | Type | Description | Default |
|-----------|------|-------------|---------|
| P_fun_id | INT | Function ID for P(μ) curve | - |
| Fscale | FLOAT | Pressure scale factor | 1.0 |
| Xscale | FLOAT | μ scale factor | 1.0 |
| IFORM | INT | Unload formulation flag | 0 |
| Mue_min | FLOAT | Elastic limit | - |
| Mue_max | FLOAT | Maximum compaction | - |
| Bmin | FLOAT | Minimum bulk modulus (tension) | - |
| Bmax | FLOAT | Maximum bulk modulus (compression) | - |
| PSH | FLOAT | Pressure shift | 0.0 |

### Example
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/EOS/COMPACTION2/14
Crushable_Foam_EOS
#           P_fun_id              Fscale              Xscale                                   IFORM
         1            1.0E+06                 1.0                                        0
#            Mue_min             Mue_max                Bmin                Bmax
               0.02                 0.8            1.0E+07            5.0E+09
#                PSH
                0.0
```

---

## 15. EOS/COMPACTION_TAB - Advanced Tabulated Compaction

Multi-function tabulated compaction for complex porous materials.

### Syntax
```
/EOS/COMPACTION_TAB/eos_ID
eos_title
#            RHO_TMD                                                                          I_PLAS
#  fct_IDp                       P-scale
#  fct_IDc                       c-scale
#  fct_IDG                       g-scale
```

### Parameters

| Parameter | Type | Description | Default |
|-----------|------|-------------|---------|
| RHO_TMD | FLOAT | Theoretical maximum density (TMD) | - |
| I_PLAS | INT | Plastic expansion flag | 0 |
| fct_IDp | INT | Pressure function ID P(ρ) | - |
| P-scale | FLOAT | Pressure scale factor | 1.0 |
| fct_IDc | INT | Sound speed function ID c(ρ) | - |
| c-scale | FLOAT | Sound speed scale factor | 1.0 |
| fct_IDG | INT | Gruneisen gamma function ID Γ(ρ) | - |
| g-scale | FLOAT | Gamma scale factor | 1.0 |

### Example
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/EOS/COMPACTION_TAB/15
Advanced_Porous_EOS
#            RHO_TMD                                                                          I_PLAS
             2700.0                                                                                0
#  fct_IDp                       P-scale
         1                     1.0E+09
#  fct_IDc                       c-scale
         2                     1.0E+03
#  fct_IDG                       g-scale
         3                         1.0
```

---

## 16. EOS/NASG - Noble-Abel Stiffened Gas

Combined Noble-Abel and stiffened gas for two-phase flow and cavitation.

### Syntax
```
/EOS/NASG/eos_ID
eos_title
#                  b               Gamma                PINF                   q
#                Psh                  P0                  Cv
```

### Parameters

| Parameter | Type | Description | Default |
|-----------|------|-------------|---------|
| b | FLOAT | Covolume | 0.0 |
| Gamma | FLOAT | Heat capacity ratio | - |
| PINF | FLOAT | Stiffening pressure (P∞) | 0.0 |
| q | FLOAT | Reference energy | 0.0 |
| Psh | FLOAT | Pressure shift | 0.0 |
| P0 | FLOAT | Initial pressure | 0.0 |
| Cv | FLOAT | Specific heat at constant volume | - |

### Formulation
```
P = (γ - 1)·ρ·(E - q) / (1 - b·ρ) - γ·P∞ + Psh
```

### Example (Water Liquid-Vapor)
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/EOS/NASG/16
Water_Two_Phase_EOS
#                  b               Gamma                PINF                   q
           6.61E-04                2.35            1.0E+09          -1.167E+06
#                Psh                  P0                  Cv
                0.0           101325.0            1816.0
```

---

## 17. EOS/TABULATED - Tabulated Equation of State

User-defined EOS using function tables for A(μ) and B(μ).

### Syntax
```
/EOS/TABULATED/eos_ID
eos_title
# A_fun_id                       XscaleA             FscaleA
# B_fun_id                       XscaleB             FscaleB
#                 E0                 PSH
```

### Parameters

| Parameter | Type | Description | Default |
|-----------|------|-------------|---------|
| A_fun_id | INT | Function ID for A(μ) table | - |
| XscaleA | FLOAT | Scale factor for A function abscissa | 1.0 |
| FscaleA | FLOAT | Scale factor for A function ordinate | 1.0 |
| B_fun_id | INT | Function ID for B(μ) table | - |
| XscaleB | FLOAT | Scale factor for B function abscissa | 1.0 |
| FscaleB | FLOAT | Scale factor for B function ordinate | 1.0 |
| E0 | FLOAT | Initial energy per unit volume | 0.0 |
| PSH | FLOAT | Pressure shift | 0.0 |

### Formulation
```
P = A(μ) + B(μ)·E + PSH
```

### Example
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/EOS/TABULATED/17
Custom_Material_EOS
# A_fun_id                       XscaleA             FscaleA
         1                         1.0            1.0E+09
# B_fun_id                       XscaleB             FscaleB
         2                         1.0                 1.0
#                 E0                 PSH
                0.0                 0.0
```

---

## 18. EOS/EXPONENTIAL - Exponential Equation of State

For soft materials with exponential pressure response.

### Syntax
```
/EOS/EXPONENTIAL/eos_ID
eos_title
#                 P0               alpha                 PSH
```

### Parameters

| Parameter | Type | Description | Default |
|-----------|------|-------------|---------|
| P0 | FLOAT | Reference pressure | - |
| alpha | FLOAT | Exponential coefficient | - |
| PSH | FLOAT | Pressure shift | 0.0 |

### Formulation
```
P = P0·[exp(α·μ) - 1] + PSH
```

### Example
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/EOS/EXPONENTIAL/18
Soft_Foam_EOS
#                 P0               alpha                 PSH
            1.0E+05               10.0                 0.0
```

---

## 19. EOS/POWDER-BURN - Powder Burn Equation of State

For propellant combustion in solid rocket motors and guns.

### Syntax
```
/EOS/POWDER-BURN/eos_ID
eos_title
#                  B                  P0                 PSH
#                  D                  EG
#                 Gr                   C               alpha
#F_id_b(P)                       SCALE_B             SCALE_P
#F_id_g(r)                   SCALE_GAMMA           SCALE_RHO                  C1                  C2
```

### Parameters

| Parameter | Type | Description | Default |
|-----------|------|-------------|---------|
| B | FLOAT | Bulk modulus of unreacted propellant | - |
| P0 | FLOAT | Initial pressure | 0.0 |
| PSH | FLOAT | Pressure shift | 0.0 |
| D | FLOAT | Burnt gas density parameter | - |
| EG | FLOAT | Burnt gas energy parameter | - |
| Gr | FLOAT | Growth parameter | - |
| C | FLOAT | Growth reaction ratio exponent | - |
| alpha | FLOAT | Reaction ratio factor | - |
| F_id_b | INT | Burn rate function ID b(P) | - |
| SCALE_B | FLOAT | Burn rate velocity scale | 1.0 |
| SCALE_P | FLOAT | Burn rate pressure scale | 1.0 |
| F_id_g | INT | Ignition function ID γ(ρ) | - |
| SCALE_GAMMA | FLOAT | Ignition gamma scale | 1.0 |
| SCALE_RHO | FLOAT | Ignition density scale | 1.0 |
| C1 | FLOAT | Ignition speed coefficient | - |
| C2 | FLOAT | Ignition exponent | - |

### Example
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/EOS/POWDER-BURN/19
Propellant_Combustion_EOS
#                  B                  P0                 PSH
           1.0E+10           101325.0                 0.0
#                  D                  EG
             1800.0            4.5E+06
#                 Gr                   C               alpha
            1.0E+05                 0.5                 1.0
#F_id_b(P)                       SCALE_B             SCALE_P
         1                     0.01            1.0E+06
#F_id_g(r)                   SCALE_GAMMA           SCALE_RHO                  C1                  C2
         2                         1.0            1000.0           1000.0                 0.5
```

---

## 20. EOS/SESAME - SESAME Tabulated EOS

For using SESAME library tabulated EOS data.

### Syntax
```
/EOS/SESAME/eos_ID
eos_title
#                 E0
#          SESAME301
sesame_filename
```

### Parameters

| Parameter | Type | Description | Default |
|-----------|------|-------------|---------|
| E0 | FLOAT | Initial energy per unit mass | 0.0 |
| SESAME301 | STRING | SESAME data file name (up to 100 characters) | - |

### Example
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/EOS/SESAME/20
Aluminum_SESAME_EOS
#                 E0
                0.0
#          SESAME301
aluminum_3700.ses
```

---

## Associating EOS with Materials

EOS is used with hydrodynamic material laws:

| Material Law | Description | EOS Usage |
|--------------|-------------|-----------|
| LAW3 | Hydrodynamic elastic-plastic | Requires EOS |
| LAW4 | Hydrodynamic Johnson-Cook | Requires EOS |
| LAW6 | Hydrodynamic viscous | Requires EOS |
| LAW10 | Tabulated elastic-plastic | Requires EOS |
| LAW26 | SESAME | Built-in SESAME EOS |
| LAW49 | Steinberg-Guinan | Requires EOS |
| LAW51 | Closed-cell foam | Optional EOS |

### Example Material with EOS
```
/MAT/LAW3/1
Steel_Hydrodynamic
#              RHO
             7850.0
#                  G
           7.69E+10
# ... other material parameters ...

/EOS/GRUNEISEN/1
Steel_EOS
#                  C                  S1                  S2                  S3
             4570.0              1.490                 0.0                 0.0
#                 Y0                   a                  E0
               1.93               0.47                 0.0
#                 P0                 PSH
                0.0                 0.0
```

---

## Common EOS Data for Materials

### Metals (Gruneisen Parameters)

| Material | ρ₀ (kg/m³) | C (m/s) | S1 | S2 | γ₀ | a |
|----------|------------|---------|----|----|----|----|
| Aluminum 6061 | 2703 | 5240 | 1.40 | 0 | 1.97 | 0.48 |
| Aluminum 2024 | 2785 | 5328 | 1.338 | 0 | 2.0 | 0.48 |
| Copper | 8930 | 3940 | 1.489 | 0 | 2.02 | 0.47 |
| Iron | 7870 | 3574 | 1.920 | 0 | 1.69 | 0.46 |
| Steel 4340 | 7830 | 4570 | 1.490 | 0 | 1.93 | 0.47 |
| Stainless Steel | 7900 | 4580 | 1.49 | 0 | 1.93 | 0.5 |
| Titanium | 4510 | 5220 | 1.029 | 0 | 1.23 | 0.48 |
| Lead | 11340 | 2006 | 1.429 | 0 | 2.74 | 0.47 |
| Tungsten | 19300 | 4040 | 1.237 | 0 | 1.67 | 0.38 |
| Tantalum | 16650 | 3410 | 1.201 | 0 | 1.67 | 0.49 |

### Gases (Ideal Gas Parameters)

| Material | γ (Gamma) | R (J/kg·K) | M (kg/mol) |
|----------|-----------|------------|------------|
| Air | 1.40 | 287.0 | 0.029 |
| Helium | 1.67 | 2077 | 0.004 |
| Hydrogen | 1.41 | 4124 | 0.002 |
| Nitrogen | 1.40 | 297 | 0.028 |
| Oxygen | 1.40 | 260 | 0.032 |
| Argon | 1.67 | 208 | 0.040 |
| CO₂ | 1.29 | 189 | 0.044 |
| Water vapor | 1.33 | 461 | 0.018 |

### Liquids (Stiffened Gas/LSZK Parameters)

| Material | ρ₀ (kg/m³) | γ | P* (Pa) | a (Pa) | b |
|----------|------------|---|---------|--------|---|
| Water | 1000 | 4.4 | 6.0E+08 | 3.0E+08 | 7.15 |
| Glycerin | 1260 | 4.5 | 5.0E+08 | - | - |
| Oil | 900 | 3.5 | 3.0E+08 | - | - |

### Explosives (JWL Polynomial Coefficients)

| Explosive | ρ₀ (kg/m³) | D (m/s) | PCJ (GPa) | C1 (GPa) | C4, C5 |
|-----------|------------|---------|-----------|----------|--------|
| TNT | 1630 | 6930 | 21.0 | 371.2 | 0.30 |
| C4/RDX | 1601 | 8193 | 28.0 | 609.8 | 0.35 |
| PETN | 1770 | 8300 | 33.5 | 617.0 | 0.25 |
| Composition B | 1717 | 7980 | 29.5 | 524.2 | 0.34 |

---

## EOS Selection Guide

### By Application

| Application | Recommended EOS | Notes |
|-------------|-----------------|-------|
| Metal plate impact | GRUNEISEN | Standard shock physics |
| Shaped charge jet | GRUNEISEN | High velocity jets |
| Underwater explosion | POLYNOMIAL + STIFF-GAS | Water as stiffened gas |
| Airbag inflation | IDEAL-GAS-VE | Simple gas model |
| Propellant burn | POWDER-BURN | Combustion modeling |
| Foam crush | COMPACTION | Irreversible compaction |
| Planetary impact | TILLOTSON | Vaporization included |
| Nuclear materials | SESAME | Tabulated data |
| Two-phase flow | NASG | Cavitation modeling |

### By Material Type

| Material Type | Primary EOS | Alternative |
|---------------|-------------|-------------|
| Metals | GRUNEISEN | POLYNOMIAL |
| Polymers | POLYNOMIAL | LINEAR |
| Ceramics | POLYNOMIAL | MURNAGHAN |
| Gases | IDEAL-GAS-VE/VT | NOBLE-ABEL |
| Liquids | STIFF-GAS | LSZK, POLYNOMIAL |
| Explosives | POLYNOMIAL (JWL) | OSBORNE |
| Foams | COMPACTION | EXPONENTIAL |
| Porous metals | PUFF | COMPACTION |

---

## Best Practices

### 1. EOS Selection
- Use **GRUNEISEN** for metals under shock - most validated and widely used
- Use **POLYNOMIAL** for general fluids and JWL explosives
- Use **IDEAL-GAS-VE** for low-pressure gas simulations (airbags, etc.)
- Use **COMPACTION** for crushable porous materials
- Use **TABULATED** when experimental data is available

### 2. Parameter Determination
- Obtain Hugoniot data from shock experiments
- Use published material databases (LASL, LLNL)
- Validate against known shock response
- Gruneisen: C = bulk sound speed, S1 from Us-Up curve

### 3. Numerical Considerations
- Ensure consistent units (SI recommended: kg, m, s, Pa)
- Match reference density (ρ₀) in material and EOS
- Set appropriate initial energy E0 for thermal equilibrium
- Use PSH for non-zero initial pressure states

### 4. Coupling with Materials
- Match ρ₀ in material and EOS definitions
- Use compatible material laws (LAW3, LAW4, LAW6)
- Include thermal effects (Cv, Cp) when needed
- Verify EOS covers expected pressure range

### 5. Stability Guidelines
- Ensure positive sound speed throughout domain
- Check for negative pressures in tension
- Avoid extreme compression ratios (μ > 1)
- Use appropriate tension cutoff in material

---

## Troubleshooting

### Common Issues

| Issue | Cause | Solution |
|-------|-------|----------|
| Negative pressure in expansion | EOS undefined for tension | Add tension cutoff in material |
| Unrealistic shock speeds | Wrong C (sound speed) | Verify from Hugoniot data |
| Energy conservation errors | Wrong E0 initialization | Match initial conditions |
| Numerical instabilities | Time step too large | Reduce Δt, check mesh quality |
| Pressure oscillations | Bulk viscosity too low | Increase artificial viscosity |
| Sound speed errors | Incompatible EOS/material | Check reference densities |

### Validation Checks

1. **Hugoniot validation**: Compare Us-Up relation with experimental data
2. **Isentrope check**: Verify release wave behavior
3. **Energy check**: Monitor total energy conservation
4. **Pressure limits**: Check against expected peak pressures
5. **Sound speed**: Verify positive throughout simulation

### Debugging Tips

1. Start with simple GRUNEISEN or LINEAR EOS
2. Use single element tests for EOS validation
3. Compare with analytical solutions when available
4. Check units consistency throughout model
5. Monitor pressure, density, energy time histories

---

## References

- **CFG source files**: `hm_cfg_files/config/CFG/radioss*/MAT/mat_EOS.cfg`
- **Engine source**: `engine/source/materials/eos/`
- **Starter source**: `starter/source/materials/eos/`

### External References
- LASL Shock Hugoniot Data (Marsh, 1980)
- Los Alamos Equations of State Library
- SESAME EOS Library (LANL)
- Meyers, Dynamic Behavior of Materials (1994)
- Steinberg, Equation of State and Strength Properties (1996)

