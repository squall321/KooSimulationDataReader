# OpenRadioss Keyword Manual - Friction

## Overview

This document describes friction modeling in OpenRadioss. The /FRICTION keyword enables definition of friction properties for contact interfaces with various friction formulations.

---

## FRICTION - Friction Definition

Defines friction properties that can be applied to contact interfaces.

### Syntax
```
/FRICTION/fric_ID
friction_title
#   I_fric   I_filtr              X_freq    I_form
#                 C1                  C2                  C3                  C4                  C5
#                 C6                FRIC               VIS_f
# Part-specific friction (optional, repeated for each pair)
#grpartID1 grpartID2   partID1   partID2                IDIR
#            C1_DIR1             C2_DIR1             C3_DIR1             C4_DIR1             C5_DIR1
#            C6_DIR1           FRIC_DIR1          VIS_F_DIR1
```

---

## Global Parameters

### Card 1 - Friction Formulation

| Parameter | Type | Description |
|-----------|------|-------------|
| fric_ID | INT | Unique friction identifier |
| friction_title | STRING | Friction description (100 chars max) |
| I_fric | INT | Friction formulation flag |
| I_filtr | INT | Friction filtering flag |
| X_freq | FLOAT | Filtering coefficient |
| I_form | INT | Friction penalty formulation type |

### I_fric Values (Friction Formulation)

| Value | Description | Formula |
|-------|-------------|---------|
| 0 | Static Coulomb friction | mu = FRIC |
| 1 | Generalized viscous friction | mu = FRIC + C1*p + C2*v + C3*p*v + C4*p^2 + C5*v^2 |
| 2 | Modified Darmstadt friction | mu = FRIC*(1 + C1*p^2)/(1 + C3*p) * (1 + C5)/(1 + C5*exp(C2*v)) * (1 - exp(-C4*v))/(1 + C6*v) |
| 3 | Renard friction | mu = FRIC*(C1 + C2*v)/(C3 + C4*v) * (1 - exp(-v/C5))*(1 + exp(-v/C6)) |

Where:
- p = contact pressure
- v = relative sliding velocity
- mu = friction coefficient

### I_filtr Values (Friction Filtering)

| Value | Description |
|-------|-------------|
| 0 | No filtering |
| 1 | Simple numerical filter |
| 2 | Standard -3dB filter with filtering period |
| 3 | Standard -3dB filter with cutting frequency |

### I_form Values (Penalty Formulation)

| Value | Description |
|-------|-------------|
| 0 | Default, set to 1 |
| 1 | Viscous (total) formulation (default) |
| 2 | Stiffness (incremental) formulation |

### Card 2 - Friction Coefficients

| Parameter | Type | Description |
|-----------|------|-------------|
| C1 | FLOAT | Friction law coefficient 1 |
| C2 | FLOAT | Friction law coefficient 2 |
| C3 | FLOAT | Friction law coefficient 3 |
| C4 | FLOAT | Friction law coefficient 4 |
| C5 | FLOAT | Friction law coefficient 5 |

### Card 3 - Additional Coefficients

| Parameter | Type | Description |
|-----------|------|-------------|
| C6 | FLOAT | Friction law coefficient 6 |
| FRIC | FLOAT | Base Coulomb friction coefficient |
| VIS_f | FLOAT | Critical damping coefficient on interface friction |

---

## Part-Specific Friction (Optional)

Allows defining different friction properties for specific part pairs.

### Part Pair Definition

| Parameter | Type | Description |
|-----------|------|-------------|
| grpartID1 | INT | Part group identifier for first set |
| grpartID2 | INT | Part group identifier for second set |
| partID1 | INT | Part identifier for first part |
| partID2 | INT | Part identifier for second part |
| IDIR | INT | Orthotropic friction flag |

### IDIR Values

| Value | Description |
|-------|-------------|
| 0 | Isotropic friction |
| 1 | Orthotropic friction (requires DIR2 coefficients) |

### Part-Specific Coefficients

Same coefficient structure (C1-C6, FRIC, VIS_f) as global, but applied only to the specified part pair.

For orthotropic friction (IDIR=1), a second set of coefficients (C1_DIR2 to VIS_F_DIR2) defines friction in the perpendicular direction.

---

## Coefficient Dimensions by Friction Law

### I_fric = 1 (Generalized Viscous)

| Coefficient | Dimension | Description |
|-------------|-----------|-------------|
| C1 | 1/pressure | Pressure coefficient |
| C2 | 1/velocity | Velocity coefficient |
| C3 | 1/(pressure*velocity) | Mixed coefficient |
| C4 | 1/pressure^2 | Squared pressure coefficient |
| C5 | 1/velocity^2 | Squared velocity coefficient |

### I_fric = 2 (Darmstadt)

| Coefficient | Dimension | Description |
|-------------|-----------|-------------|
| C1 | 1/pressure^2 | Pressure squared coefficient |
| C2 | 1/velocity | Velocity decay coefficient |
| C3 | 1/pressure | Pressure coefficient |
| C4 | 1/velocity | Transition velocity coefficient |
| C5 | dimensionless | Amplitude factor |
| C6 | 1/velocity | High velocity decay |

### I_fric = 3 (Renard)

| Coefficient | Dimension | Description |
|-------------|-----------|-------------|
| C1 | dimensionless | Static coefficient |
| C2 | dimensionless | Velocity ratio coefficient |
| C3 | dimensionless | Reference coefficient |
| C4 | dimensionless | Velocity ratio denominator |
| C5 | velocity | Transition velocity 1 |
| C6 | velocity | Transition velocity 2 |

---

## Examples

### Example 1 - Simple Coulomb Friction
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/FRICTION/1
Simple_Coulomb_0.3
#   I_fric   I_filtr              X_freq    I_form
         0         0                 0.0         1
#                 C1                  C2                  C3                  C4                  C5
                 0.0                 0.0                 0.0                 0.0                 0.0
#                 C6                FRIC               VIS_f
                 0.0                 0.3                 0.0
```

### Example 2 - Velocity-Dependent Friction (Generalized Viscous)
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/FRICTION/2
Velocity_Dependent_Friction
#   I_fric   I_filtr              X_freq    I_form
         1         0                 0.0         1
#                 C1                  C2                  C3                  C4                  C5
               1E-06              0.001                 0.0                 0.0               1E-06
#                 C6                FRIC               VIS_f
                 0.0                 0.2                 0.0
```

### Example 3 - Darmstadt Friction Model
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/FRICTION/3
Darmstadt_Friction_Model
#   I_fric   I_filtr              X_freq    I_form
         2         0                 0.0         1
#                 C1                  C2                  C3                  C4                  C5
               1E-12                0.01               1E-06                0.05                 0.1
#                 C6                FRIC               VIS_f
               0.001                0.15                 0.0
```

### Example 4 - Part-Specific Friction with Orthotropic Properties
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/FRICTION/4
Orthotropic_Friction
#   I_fric   I_filtr              X_freq    I_form
         0         0                 0.0         1
#                 C1                  C2                  C3                  C4                  C5
                 0.0                 0.0                 0.0                 0.0                 0.0
#                 C6                FRIC               VIS_f
                 0.0                 0.3                 0.0
# Part-specific with orthotropic friction
#grpartID1 grpartID2   partID1   partID2                IDIR
         0         0         1         2                   1
#            C1_DIR1             C2_DIR1             C3_DIR1             C4_DIR1             C5_DIR1
                 0.0                 0.0                 0.0                 0.0                 0.0
#            C6_DIR1           FRIC_DIR1          VIS_F_DIR1
                 0.0                 0.3                 0.0
#            C1_DIR2             C2_DIR2             C3_DIR2             C4_DIR2             C5_DIR2
                 0.0                 0.0                 0.0                 0.0                 0.0
#            C6_DIR2           FRIC_DIR2          VIS_F_DIR2
                 0.0                 0.1                 0.0
```

### Example 5 - Friction with Filtering
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/FRICTION/5
Filtered_Friction
#   I_fric   I_filtr              X_freq    I_form
         0         3              1000.0         1
#                 C1                  C2                  C3                  C4                  C5
                 0.0                 0.0                 0.0                 0.0                 0.0
#                 C6                FRIC               VIS_f
                 0.0                0.25                 0.0
```

---

## Applying Friction to Interfaces

Friction is applied to contact interfaces by referencing the friction ID in the interface definition:

```
/INTER/TYPE7/1
Contact_Interface
# ... interface parameters ...
#   sens_ID  Fric_ID  Ibag
         0         1         0
```

---

## Friction Formulation Details

### Static Coulomb (I_fric = 0)

The simplest friction model:
```
F_friction = mu * F_normal
mu = FRIC (constant)
```

### Generalized Viscous (I_fric = 1)

Friction coefficient varies with pressure and velocity:
```
mu = FRIC + C1*p + C2*v + C3*p*v + C4*p^2 + C5*v^2
```

Useful for:
- Capturing stick-slip behavior
- Modeling velocity weakening/strengthening
- Pressure-dependent friction

### Darmstadt Model (I_fric = 2)

Complex model for metal forming:
```
mu = FRIC * f(p) * g(v)
where:
f(p) = (1 + C1*p^2)/(1 + C3*p)
g(v) = (1 + C5)/(1 + C5*exp(C2*v)) * (1 - exp(-C4*v))/(1 + C6*v)
```

Features:
- Pressure saturation effects
- Velocity transition behavior
- Stribeck curve representation

### Renard Model (I_fric = 3)

Empirical model for dry friction:
```
mu = FRIC * (C1 + C2*v)/(C3 + C4*v) * (1 - exp(-v/C5)) * (1 + exp(-v/C6))
```

Suitable for:
- Mixed lubrication regimes
- Boundary lubrication transition
- Industrial contact applications

---

## Best Practices

### 1. Model Selection
- Use I_fric=0 (Coulomb) for simple analyses
- Use I_fric=1 for velocity-dependent effects
- Use I_fric=2 (Darmstadt) for metal forming
- Use I_fric=3 (Renard) for empirical calibration

### 2. Numerical Stability
- Use I_filtr=2 or 3 for noisy friction forces
- Set VIS_f > 0 for damping in stick-slip
- I_form=2 (stiffness) may improve convergence

### 3. Part-Specific Friction
- Define only when different parts need different friction
- Use orthotropic (IDIR=1) for directional surfaces
- Group similar parts to reduce definitions

### 4. Typical Friction Values

| Material Pair | Typical mu |
|---------------|------------|
| Steel-Steel (dry) | 0.5-0.8 |
| Steel-Steel (lubricated) | 0.1-0.2 |
| Aluminum-Steel | 0.4-0.6 |
| Rubber-Steel | 0.5-0.9 |
| Polymer-Steel | 0.2-0.4 |
| Composite-Metal | 0.2-0.5 |

---

## Troubleshooting

### Common Issues

1. **Stick-slip oscillations**
   - Enable filtering (I_filtr > 0)
   - Increase VIS_f damping
   - Use I_form=2 (stiffness formulation)

2. **Unrealistic friction forces**
   - Check coefficient units match dimensions
   - Verify contact pressure is reasonable
   - Check velocity scale in model

3. **Friction not activating**
   - Verify Fric_ID is referenced in interface
   - Check contact is properly detecting
   - Ensure parts are in contact

4. **Orthotropic friction issues**
   - Verify IDIR=1 is set
   - Check DIR2 coefficients are defined
   - Confirm element coordinate system orientation

---

## References

- Source files: `hm_cfg_files/config/CFG/radioss*/FRICTION/`
- Engine source: `engine/source/interfaces/friction/`

