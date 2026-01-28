# OpenRadioss Keyword Manual - Initial Conditions

## Overview

This document describes initial condition definitions in OpenRadioss. Initial conditions set the starting state for simulations including velocities, stresses, strains, and state data from previous analyses.

---

## 1. INIVEL - Initial Velocity

Defines initial velocities applied to nodes at the start of analysis. See [06_LOADS_BC.md](06_LOADS_BC.md) for full documentation of INIVEL keywords.

### Types

| Keyword | Description |
|---------|-------------|
| /INIVEL/TRA | Translational initial velocity |
| /INIVEL/ROT | Rotational initial velocity |
| /INIVEL/AXIS | Initial velocity with rotation axis |
| /INIVEL/FVM | Initial velocity for Fluid Volume Method |
| /INIVEL/NODE | Node-specific initial velocity |

---

## 2. INISTA - Initial State

Loads initial state from a previous analysis restart file.

### Syntax
```
/INISTA
filename.sty
#    Ibal   IoutyyFMT  Ioutynnn
```

### Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| filename | STRING | - | Path to .sty restart file |
| Ibal | INT | 1 | Initial balance flag |
| IoutyyFMT | INT | 1 | File format |
| Ioutynnn | INT | 1 | Filename format |

### Ibal Options

| Value | Description |
|-------|-------------|
| 0 | Default (set to 1) |
| 1 | Do not perform nodal initial balance |
| 2 | Nodal initial balance performed |
| 3 | Initial balance in shell local frame |

### IoutyyFMT Options

| Value | Description |
|-------|-------------|
| 1 | Format 51 |
| 2 | Format 44 |

### Ioutynnn Options

| Value | Description |
|-------|-------------|
| 1 | Reading file format is Runname_run#.sty |
| 2 | Reading file format is RunnameYnnn (old format) |

### Example
```
/INISTA
output_0000.sty
         1         1         1
```

---

## 3. INISHE - Initial Shell Element State

Initializes shell element state variables including stress, strain, plastic strain, thickness, and failure.

### Available Subkeywords

| Keyword | Description |
|---------|-------------|
| /INISHE/EPSP | Initial plastic strain |
| /INISHE/EPSP_F | Initial plastic strain (per layer) |
| /INISHE/THICK | Initial thickness |
| /INISHE/FAIL | Initial failure state |
| /INISHE/STRS_F | Initial stress (per layer) |
| /INISHE/STRS_F_GLOB | Initial stress in global frame |
| /INISHE/STRA_F | Initial strain (per layer) |
| /INISHE/STRA_F_GLOB | Initial strain in global frame |
| /INISHE/ORTHO | Initial orthotropic directions |
| /INISHE/ORTH_LOC | Initial local orthotropic directions |
| /INISHE/SCALE_YLD | Initial yield stress scale factor |
| /INISHE/AUX | Auxiliary initial data |

### General Syntax
```
/INISHE/option/inishe_ID
inishe_title
# element-specific data cards
```

### Example - Initial Plastic Strain
```
/INISHE/EPSP/1
Initial_Plastic_Strain
# shell_ID           EPSP
      1001            0.05
      1002            0.08
      1003            0.12
```

### Example - Initial Thickness
```
/INISHE/THICK/2
Modified_Thickness
# shell_ID          Thick
      2001            1.8
      2002            1.9
      2003            2.0
```

---

## 4. INISH3 - Initial 3-Node Shell Element State

Initializes 3-node shell element state variables. Similar to INISHE but for triangular shells.

### Available Subkeywords

| Keyword | Description |
|---------|-------------|
| /INISH3/EPSP | Initial plastic strain |
| /INISH3/EPSP_F | Initial plastic strain (per layer) |
| /INISH3/THICK | Initial thickness |
| /INISH3/FAIL | Initial failure state |
| /INISH3/STRS_F | Initial stress (per layer) |
| /INISH3/STRS_F_GLOB | Initial stress in global frame |
| /INISH3/STRA_F | Initial strain (per layer) |
| /INISH3/STRA_F_GLOB | Initial strain in global frame |
| /INISH3/ORTHO | Initial orthotropic directions |
| /INISH3/ORTH_LOC | Initial local orthotropic directions |
| /INISH3/SCALE_YLD | Initial yield stress scale factor |
| /INISH3/AUX | Auxiliary initial data |

---

## 5. INIBRI - Initial Solid (Brick) Element State

Initializes solid element state variables including stress, strain, density, energy, and failure.

### Available Subkeywords

| Keyword | Description |
|---------|-------------|
| /INIBRI/EPSP | Initial plastic strain |
| /INIBRI/DENS | Initial density |
| /INIBRI/ENER | Initial energy |
| /INIBRI/STRESS | Initial stress tensor |
| /INIBRI/STRS_F | Initial stress (full format) |
| /INIBRI/STRS_FGLO | Initial stress in global frame |
| /INIBRI/STRA_F | Initial strain |
| /INIBRI/STRA_FGLO | Initial strain in global frame |
| /INIBRI/FAIL | Initial failure state |
| /INIBRI/FILL | Initial fill fraction (ALE) |
| /INIBRI/ORTHO | Initial orthotropic directions |
| /INIBRI/SCALE_YLD | Initial yield stress scale factor |
| /INIBRI/EREF | Initial reference energy |
| /INIBRI/DEEF | Initial deformation |
| /INIBRI/AUX | Auxiliary initial data |

### Example - Initial Stress
```
/INIBRI/STRESS/1
Pre_Stressed_Solid
# brick_ID            Sxx            Syy            Szz            Sxy            Syz            Sxz
     10001         -100.0         -100.0         -100.0            0.0            0.0            0.0
```

### Example - Initial Density
```
/INIBRI/DENS/2
Variable_Density
# brick_ID           Dens
     20001         7850.0
     20002         7800.0
```

---

## 6. INIVOL - Initial Volume Fraction

Initializes volume fractions for ALE/Eulerian multi-material elements.

### Syntax
```
/INIVOL/inivol_ID
inivol_title
# Material volume fraction data
```

### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| mat_ID | INT | Material identifier |
| Vfrac | FLOAT | Volume fraction (0.0 to 1.0) |

### Notes
- Sum of volume fractions should equal 1.0
- Used with ALE and multi-material solids
- Defines initial material distribution

---

## 7. INIQUA - Initial Quadrilateral Element State

Initializes 2D quadrilateral element state variables.

### Available Subkeywords

| Keyword | Description |
|---------|-------------|
| /INIQUA/EPSP | Initial plastic strain |
| /INIQUA/ENER | Initial energy |
| /INIQUA/DENS | Initial density |
| /INIQUA/STRESS | Initial stress |

---

## 8. INISPRI - Initial Spring Element State

Initializes spring element state variables.

### Syntax
```
/INISPRI/FULL/inispri_ID
inispri_title
# spring_ID        Disp        Force
```

### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| spring_ID | INT | Spring element identifier |
| Disp | FLOAT | Initial displacement |
| Force | FLOAT | Initial force |

---

## 9. INIBEAM - Initial Beam Element State

Initializes beam element state variables.

### Syntax
```
/INIBEAM/FULL/inibeam_ID
inibeam_title
# beam_ID  beam-specific state data
```

---

## 10. INITRUSS - Initial Truss Element State

Initializes truss element state variables.

### Syntax
```
/INITRUSS/initruss_ID
initruss_title
# truss_ID        Strain        Force
```

---

## 11. INISPHCEL - Initial SPH Cell State

Initializes SPH (Smoothed Particle Hydrodynamics) cell state variables.

### Syntax
```
/INISPHCEL/FULL/inisphcel_ID
inisphcel_title
# sphcel_ID  SPH-specific state data
```

---

## 12. INIMAP1D/INIMAP2D - Initial Mapping

Maps initial conditions from external data sources.

### INIMAP1D Syntax
```
/INIMAP1D/inimap_ID
inimap_title
# 1D mapping parameters
```

### INIMAP2D Syntax
```
/INIMAP2D/inimap_ID
inimap_title
# 2D mapping parameters
```

---

## 13. INIGRAV - Initial Gravity State

Initializes stress state due to gravity loading.

### Syntax
```
/INIGRAV/inigrav_ID
inigrav_title
# Gravity initialization parameters
```

---

## 14. INICRACK - Initial Crack

Defines initial crack locations for fracture analysis.

### Syntax
```
/INICRACK/inicrack_ID
inicrack_title
# Crack geometry and parameters
```

---

## Initial Condition Summary

| Keyword | Element Type | Variables |
|---------|--------------|-----------|
| /INIVEL | Nodes | Velocity (TRA, ROT, AXIS) |
| /INISTA | Model | Complete restart state |
| /INISHE | 4-node Shell | Stress, strain, thickness, failure |
| /INISH3 | 3-node Shell | Stress, strain, thickness, failure |
| /INIBRI | Solid | Stress, strain, density, energy |
| /INIQUA | Quad | Stress, strain, density |
| /INISPRI | Spring | Displacement, force |
| /INIBEAM | Beam | Beam state variables |
| /INITRUSS | Truss | Strain, force |
| /INISPHCEL | SPH | SPH cell state |
| /INIVOL | ALE | Volume fraction |
| /INIMAP | Various | Mapped data |
| /INIGRAV | Model | Gravity stress |
| /INICRACK | Model | Crack definition |

---

## Usage Guidelines

### 1. State Variables
- Initial conditions override default zero values
- Stress must be in equilibrium if no balancing requested
- Plastic strain should be consistent with stress state

### 2. Restart Analysis
- INISTA provides complete state from previous run
- Ibal=2 performs equilibrium check
- Use consistent unit systems

### 3. Pre-stressed States
- Use INIBRI/STRESS for pre-stressed solids
- Use INISHE/STRS_F for pre-stressed shells
- Ensure stress is self-equilibrating

### 4. ALE/Multi-material
- Use INIVOL for volume fractions
- Sum of fractions must equal 1.0
- INIBRI/FILL for material fill state

### 5. Best Practices
- Verify initial state consistency
- Check equilibrium after initialization
- Document units clearly in titles

---

## Example: Complete Initial Condition Setup

```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
# Initial velocity for impactor
/INIVEL/TRA/1
Impactor_Velocity
              -5.0               0.0               0.0        10         0
               0.0         0

# Pre-stressed shell elements (formed state)
/INISHE/THICK/1
Formed_Thickness
#shell_ID         Thick
     1001           1.85
     1002           1.90
     1003           1.95

/INISHE/EPSP/2
Forming_Plastic_Strain
#shell_ID          EPSP
     1001           0.15
     1002           0.12
     1003           0.08

# Pre-stressed solid (residual stress)
/INIBRI/STRESS/1
Residual_Stress
# brick_ID            Sxx            Syy            Szz            Sxy            Syz            Sxz
     2001          -50.0          -50.0            0.0            0.0            0.0            0.0
```

---

## References

- Source files: `hm_cfg_files/config/CFG/radioss2025/TABLE/`
- Source files: `hm_cfg_files/config/CFG/radioss2025/LOADS/`
- Starter source: `starter/source/initial_conditions/`
