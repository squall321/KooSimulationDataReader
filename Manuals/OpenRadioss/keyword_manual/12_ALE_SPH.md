# OpenRadioss Keyword Manual - ALE and SPH Methods

## Overview

This document describes ALE (Arbitrary Lagrangian-Eulerian) and SPH (Smoothed Particle Hydrodynamics) method keywords in OpenRadioss. These methods are used for fluid-structure interaction, blast, and large deformation problems.

---

## Part 1: ALE Methods

ALE methods combine Lagrangian and Eulerian formulations to handle large deformations while maintaining mesh quality.

---

## 1. ALE/MAT - ALE Material Definition

Defines material properties for ALE elements.

### Syntax
```
/ALE/MAT/mat_ID
mat_title
# ALE-specific material parameters
```

### Compatible Material Laws

| Law | Description |
|-----|-------------|
| LAW5 | Jones Wilkins Lee (JWL) explosive |
| LAW11 | Bound/Boundary conditions |
| LAW20 | Bi-material |
| LAW26 | SESAME-Johnson-Cook |
| LAW37 | Bi-phase |
| LAW41 | Lee-Tarver reactive |
| LAW51 | Multi-material |
| LAW97 | JWLB explosive |

---

## 2. ALE/BCS - ALE Boundary Conditions

Defines boundary conditions for ALE domains.

### Syntax
```
/ALE/BCS/bcs_ID
bcs_title
# ALE boundary condition parameters
```

### Types of ALE Boundary Conditions
- Velocity constraints
- Non-reflecting boundaries
- Inflow/outflow conditions

---

## 3. ALE/GRID - ALE Grid Control

Controls mesh motion in ALE simulations.

### 3.1 ALE/GRID/DISP - Displacement-Based Grid

```
/ALE/GRID/DISP
# Grid displacement parameters
```

### 3.2 ALE/GRID/DONEA - Donea Smoothing

```
/ALE/GRID/DONEA
# Donea smoothing parameters
```

Uses Donea algorithm for mesh smoothing.

### 3.3 ALE/GRID/SPRING - Spring-Based Grid

```
/ALE/GRID/SPRING
# Spring smoothing parameters
```

Uses spring analogy for mesh motion.

### 3.4 ALE/GRID/STANDARD - Standard Grid Motion

```
/ALE/GRID/STANDARD
# Standard grid motion parameters
```

### 3.5 ALE/GRID/LAPLACIAN - Laplacian Smoothing

```
/ALE/GRID/LAPLACIAN
# Laplacian smoothing parameters
```

### 3.6 ALE/GRID/VOLUME - Volume-Based Grid

```
/ALE/GRID/VOLUME
# Volume-based grid motion parameters
```

### 3.7 ALE/GRID/FLOW-TRACKING

```
/ALE/GRID/FLOW-TRACKING
# Flow-tracking grid parameters
```

---

## 4. ALE/LINK - ALE Coupling

Links ALE domains to Lagrangian structures.

### Syntax
```
/ALE/LINK/VEL/link_ID
link_title
# Velocity coupling parameters
```

### Coupling Types
- Velocity coupling
- Penalty-based coupling
- Constraint-based coupling

---

## 5. ALE/MUSCL - MUSCL Scheme

Defines MUSCL (Monotonic Upstream-centered Scheme for Conservation Laws) parameters.

### Syntax
```
/ALE/MUSCL
# MUSCL scheme parameters
```

### Purpose
- Higher-order advection scheme
- Reduces numerical diffusion
- Improves shock capturing

---

## 6. ALE/SOLVER - ALE Solver Options

Controls ALE solution algorithm.

### Syntax
```
/ALE/SOLVER/FINT
# Solver options
```

### Options
- FINT: Force integration method
- Time integration controls
- Convergence criteria

---

## 7. ALE/UPWIND - Upwind Scheme

Defines upwind advection parameters.

### Syntax
```
/UPWIND
# Upwind scheme parameters
```

---

## 8. ALE/CLOSE - ALE Domain Closure

Defines closed ALE domain boundaries.

### Syntax
```
/ALE/CLOSE/close_ID
close_title
# Closure parameters
```

---

## Part 2: SPH Methods

SPH (Smoothed Particle Hydrodynamics) is a meshless method for simulating fluid flow and large deformations.

---

## 9. SPHCEL - SPH Cell Element

Defines SPH particle cells.

### Syntax
```
/SPHCEL/sphcel_ID
sphcel_title
# SPH cell parameters
```

---

## 10. PROP/SPH (TYPE34) - SPH Property

Defines properties for SPH particles.

### Syntax
```
/PROP/SPH/prop_ID
prop_title
#         qa          qb       Iform      Ismooth       Ikern
#                   h                   C                   D
```

### Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| qa | FLOAT | 1.0 | Quadratic bulk viscosity |
| qb | FLOAT | 1.0 | Linear bulk viscosity |
| Iform | INT | 1 | Formulation flag |
| Ismooth | INT | 0 | Smoothing type |
| Ikern | INT | 0 | Kernel type |
| h | FLOAT | - | Smoothing length |
| C | FLOAT | - | Speed of sound |
| D | FLOAT | - | Dimension factor |

### Iform Options

| Value | Description |
|-------|-------------|
| 0 | Default formulation |
| 1 | Standard SPH |
| 2 | Normalized SPH |
| 3 | Tensor corrected SPH |

### Ismooth Options

| Value | Description |
|-------|-------------|
| 0 | Constant smoothing length |
| 1 | Variable smoothing length |
| 2 | Adaptive smoothing |

### Ikern Options

| Value | Description |
|-------|-------------|
| 0 | Cubic B-spline kernel |
| 1 | Gaussian kernel |
| 2 | Quintic spline kernel |

---

## 11. SPHBCS - SPH Boundary Conditions

Defines boundary conditions for SPH particles.

### Syntax
```
/SPHBCS/type/bcs_ID
bcs_title
# SPH boundary parameters
```

### Types

| Type | Description |
|------|-------------|
| SLIDE | Sliding boundary |
| TIED | Fixed/tied boundary |

---

## 12. SPH/INOUT - SPH Inlet/Outlet

Defines inlet and outlet conditions for SPH flow.

### Syntax
```
/SPH/INOUT/inout_ID
inout_title
# Inlet/outlet parameters
```

---

## 13. SPH/RESERVE - SPH Particle Reserve

Reserves particles for SPH simulations.

### Syntax
```
/SPH/RESERVE/reserve_ID
reserve_title
# Reserve parameters
```

---

## 14. SPHGLO - Global SPH Controls

Sets global parameters for SPH simulation.

### Syntax
```
/SPHGLO
# Global SPH parameters
```

### Parameters
- Particle search algorithm
- Neighbor list update frequency
- Smoothing length control

---

## 15. INISPHCEL - Initial SPH Cell State

Initializes SPH cell state variables.

### Syntax
```
/INISPHCEL/FULL/ini_ID
ini_title
# Initial state data
```

---

## ALE/SPH Material Laws

### LAW5 - JWL (Jones Wilkins Lee)

For high explosive materials.

```
/MAT/LAW5/mat_ID
mat_title
#           Rho_I
#               A               B              R1              R2           omega
#               D               E0             Pcut
```

### LAW51 - Multi-material

For ALE multi-material simulations.

```
/MAT/LAW51/mat_ID
mat_title
# Multi-material parameters
```

---

## Time History Output

### TH/SPH_FLOW
```
/TH/SPH_FLOW/th_ID
th_title
# Variables
# SPH element list
```

### TH/SPHCEL
```
/TH/SPHCEL/th_ID
th_title
# Variables
# SPH cell list
```

---

## Summary Tables

### ALE Keywords

| Keyword | Purpose |
|---------|---------|
| /ALE/MAT | ALE material definition |
| /ALE/BCS | ALE boundary conditions |
| /ALE/GRID/* | Grid motion control |
| /ALE/LINK | Lagrangian coupling |
| /ALE/MUSCL | MUSCL advection scheme |
| /ALE/SOLVER | Solver options |
| /UPWIND | Upwind scheme |
| /ALE/CLOSE | Domain closure |

### SPH Keywords

| Keyword | Purpose |
|---------|---------|
| /SPHCEL | SPH cell element |
| /PROP/SPH | SPH property |
| /SPHBCS | SPH boundary conditions |
| /SPH/INOUT | Inlet/outlet |
| /SPH/RESERVE | Particle reserve |
| /SPHGLO | Global controls |
| /INISPHCEL | Initial state |

---

## Application Guidelines

### When to Use ALE
- Fluid-structure interaction
- Sloshing simulations
- Metal forming with large deformations
- Explosive loading
- Underwater explosions

### When to Use SPH
- Free surface flows
- Fragmentation
- Bird strike simulations
- Airbag deployment (internal gas)
- High-velocity impact with material failure

---

## Example: ALE Setup

```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
# ALE material (explosive)
/MAT/LAW5/1
JWL_Explosive
#           Rho_I
           1630.0
#               A               B              R1              R2           omega
      3.712e+11       3.21e+9             4.15             0.95             0.30
#               D               E0             Pcut
          6930.0          7.0e+9             0.0

# ALE grid control
/ALE/GRID/DONEA
# Donea smoothing

# ALE solver
/ALE/SOLVER/FINT
```

---

## Example: SPH Setup

```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
# SPH property
/PROP/SPH/1
Water_SPH
#         qa          qb       Iform      Ismooth       Ikern
         1.0         1.0           1           1           0
#                   h                   C                   D
             0.005          1480.0             3.0

# Global SPH controls
/SPHGLO
# Global SPH parameters

# SPH boundary (tied)
/SPHBCS/TIED/1
Fixed_Boundary
# Boundary parameters
```

---

## Best Practices

### ALE Simulations
1. Use appropriate grid motion algorithm for problem type
2. Monitor mesh quality during simulation
3. Use MUSCL scheme for sharp interfaces
4. Ensure proper coupling between ALE and Lagrangian domains

### SPH Simulations
1. Choose appropriate smoothing length (typically 1.2-1.5 times particle spacing)
2. Use sufficient particles for resolution
3. Select kernel appropriate for problem physics
4. Monitor particle distribution and neighbor counts

### General
1. Validate with analytical solutions when possible
2. Perform mesh/particle convergence studies
3. Check energy balance and mass conservation
4. Use appropriate time step for stability

---

## References

- Source files: `hm_cfg_files/config/CFG/radioss2025/CARDS/`
- Source files: `hm_cfg_files/config/CFG/radioss2025/PROP/`
- Source files: `hm_cfg_files/config/CFG/radioss2025/MAT/`
- Engine source: `engine/source/ale/`
- Engine source: `engine/source/sph/`
