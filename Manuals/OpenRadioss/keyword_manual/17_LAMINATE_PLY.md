# OpenRadioss Keyword Manual - Laminate and Ply

## Overview

This document describes composite laminate modeling in OpenRadioss. The /STACK and /PLY keywords enable definition of composite shell structures with multiple layers of different materials and orientations.

---

## Composite Modeling Hierarchy

```
/STACK (Laminate Stack)
   └── /PLY (Individual Ply Definition)
         └── /MAT (Material for Ply)
```

---

## 1. PLY - Ply Definition

Defines individual ply properties including material, thickness, and orientation.

### Syntax
```
/PLY/ply_ID
ply_title
#   mat_ID                   t           delta_phi grsh4n_ID grsh3n_ID   Npt_ply             alpha_i
# drape_ID  def_orth
```

### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| ply_ID | INT | Unique ply identifier |
| ply_title | STRING | Ply description (100 chars max) |
| mat_ID | INT | Material identifier for this ply |
| t | FLOAT | Ply thickness |
| delta_phi | FLOAT | Incremental angle for layer (degrees) |
| grsh4n_ID | INT | Shell 4-node group identifier (optional) |
| grsh3n_ID | INT | Shell 3-node group identifier (optional) |
| Npt_ply | INT | Number of integration points through ply thickness (1-10, default: 1) |
| alpha_i | FLOAT | Angle between first and second material direction (default: 90.0) |
| drape_ID | INT | Drape table identifier for fiber draping (optional) |
| def_orth | INT | Orthotropy angle definition flag |

### def_orth Values

| Value | Description |
|-------|-------------|
| 1 | Orthotropy angle from PHIs in /SHELL and /SH3N |
| 2 | (Default) Orthotropy angle from skew and rotation in /STACK |

### Example
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/PLY/1
Carbon_Fiber_0deg
#   mat_ID                   t           delta_phi grsh4n_ID grsh3n_ID   Npt_ply             alpha_i
         1                0.25                 0.0         0         0         3                90.0
# drape_ID  def_orth
         0         2
```

---

## 2. STACK - Laminate Stack Definition

Defines a complete laminate stack with multiple plies.

### Syntax
```
/STACK/stack_ID
stack_title
#   Ishell    Ismstr     Ish3n    Idrill        P_thick_fail                  Z0
#                 Hm                  Hf                  Hr                  Dm                  Dn
#                                 Ashear                Iint              Ithick
#                 Vx                  Vy                  Vz   skew_ID     Iorth      Ipos        Ip
# Pply_IDi               Phi_i                 Z_i      P_thick_fail_i          F_weight_i
```

### Card 1 - Shell Formulation Parameters

| Parameter | Type | Description | Default |
|-----------|------|-------------|---------|
| Ishell | INT | Shell element formulation flag | 0 |
| Ismstr | INT | Small strain formulation flag | 0 |
| Ish3n | INT | 3-node shell formulation flag | 0 |
| Idrill | INT | Drilling DOF stiffness flag | 0 |
| P_thick_fail | FLOAT | Percentage of layer thickness for deletion (0.0-1.0) | 0.0 |
| Z0 | FLOAT | Distance from reference plane to bottom surface | 0.0 |

### Ishell Values (Shell Element Formulation)

| Value | Description |
|-------|-------------|
| 0 | Use value in /DEF_SHELL |
| 1 | Q4, visco-elastic hourglass (Belytschko) |
| 2 | Q4, visco-elastic hourglass (Hallquist) |
| 3 | Q4, elasto-plastic hourglass with orthogonality |
| 4 | Q4 with improved type 1 formulation |
| 12 | QBAT shell formulation |
| 24 | QEPH shell formulation |

### Ismstr Values (Small Strain Formulation)

| Value | Description |
|-------|-------------|
| 0 | Use value in /DEF_SHELL |
| 1 | Small strain from time=0 |
| 2 | Full geometric nonlinearities with possible small strain activation |
| 4 | Full geometric nonlinearities |

### Ish3n Values (3-Node Shell Formulation)

| Value | Description |
|-------|-------------|
| 0 | Use value in /DEF_SHELL |
| 1 | Standard triangle (C0) |
| 2 | Standard triangle (C0) with large rotation modification |
| 30 | DKT18 |
| 31 | DKT_S3 based on DKT12 of BATOZ |

### Card 2 - Hourglass Parameters

| Parameter | Type | Description | Default | Range |
|-----------|------|-------------|---------|-------|
| Hm | FLOAT | Shell membrane hourglass coefficient | 0.01 | 0.0-0.05 |
| Hf | FLOAT | Shell out-of-plane hourglass coefficient | 0.01 | 0.0-0.05 |
| Hr | FLOAT | Shell rotation hourglass coefficient | 0.01 | 0.0-0.05 |
| Dm | FLOAT | Shell membrane damping | 0.0 | - |
| Dn | FLOAT | Shell numerical damping (only for Ishell=12,24) | 0.0 | - |

### Card 3 - Integration Parameters

| Parameter | Type | Description | Default |
|-----------|------|-------------|---------|
| Ashear | FLOAT | Shear correction factor | 0.833333 |
| Iint | INT | Thickness integration scheme | 1 |
| Ithick | INT | Shell thickness calculation flag | 0 |

### Iint Values (Thickness Integration)

| Value | Description |
|-------|-------------|
| 1 | (Default) Uniform integration scheme |
| 2 | Gauss integration scheme |

### Ithick Values (Thickness Calculation)

| Value | Description |
|-------|-------------|
| 0 | Use value in /DEF_SHELL |
| 1 | Thickness change is taken into account |
| 2 | Thickness is constant |

### Card 4 - Reference Direction Parameters

| Parameter | Type | Description | Default |
|-----------|------|-------------|---------|
| Vx | FLOAT | X component of reference vector | 1.0 |
| Vy | FLOAT | Y component of reference vector | 0.0 |
| Vz | FLOAT | Z component of reference vector | 0.0 |
| skew_ID | INT | Skew identifier for reference vector | 0 |
| Iorth | INT | Orthotropic system formulation flag | 0 |
| Ipos | INT | Ply positioning flag | 0 |
| Ip | INT | Reference direction in shell plane | 0 |

### Iorth Values (Orthotropic Formulation)

| Value | Description |
|-------|-------------|
| 0 | (Default) First axis maintained at constant angle with co-rotational system |
| 1 | First direction constant with respect to deformed element |

### Ipos Values (Ply Positioning)

| Value | Description |
|-------|-------------|
| 0 | Ply positions calculated automatically from thicknesses |
| 1 | All ply positions are user-defined |
| 2 | Reference plane at Z0 from bottom surface |
| 3 | Top surface is element reference plane |
| 4 | Bottom surface is element reference plane |

### Ip Values (Reference Direction)

| Value | Description |
|-------|-------------|
| 0 | Use first direction of skew_ID or vector V projected on shell |
| 20 | From element connectivity (N1,N2) |
| 22 | From 1st direction of skew_ID and angle phi |
| 23 | From reference vector V and angle phi |

### Ply Cards (Repeated for Each Ply)

| Parameter | Type | Description |
|-----------|------|-------------|
| Pply_IDi | INT | Ply property identifier for layer i |
| Phi_i | FLOAT | Orientation angle for layer i (degrees) |
| Z_i | FLOAT | Z position for layer i (when Ipos=1) |
| P_thick_fail_i | FLOAT | Failure thickness percentage for layer i |
| F_weight_i | FLOAT | Relative failure weight factor for layer i |

### Example - Basic Stack
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/STACK/1
CFRP_Laminate_0_90_0
#   Ishell    Ismstr     Ish3n    Idrill        P_thick_fail                  Z0
        24         0         0         0                 0.5                 0.0
#                 Hm                  Hf                  Hr                  Dm                  Dn
              0.01                0.01                0.01                 0.0                 0.0
#                                 Ashear                Iint              Ithick
                   0            0.833333                   1                   1
#                 Vx                  Vy                  Vz   skew_ID     Iorth      Ipos        Ip
                 1.0                 0.0                 0.0         0         0         0         0
# Pply_IDi               Phi_i                 Z_i      P_thick_fail_i          F_weight_i
         1                 0.0                 0.0                 0.5                 1.0
         1                90.0                 0.0                 0.5                 1.0
         1                 0.0                 0.0                 0.5                 1.0
```

---

## 3. Sublaminate Definition (SUB)

For complex laminates, sublaminates can be defined to group plies.

### Syntax
```
SUB       Nsub      Nply
sublaminate_title
#   Ply_id                 Phi                  Zi
```

### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| Nsub | INT | Substack identification number |
| Nply | INT | Number of plies in sublaminate |
| Ply_id | INT | Ply property identifier |
| Phi | FLOAT | Orientation angle (degrees) |
| Zi | FLOAT | Z position for the ply |

### Example - Stack with Sublaminate
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/STACK/2
Composite_with_Sublaminates
#   Ishell    Ismstr     Ish3n    Idrill        P_thick_fail                  Z0
        24         0         0         0                 0.5                 0.0
#                 Hm                  Hf                  Hr                  Dm                  Dn
              0.01                0.01                0.01                 0.0                 0.0
#                                 Ashear                Iint              Ithick
                   0            0.833333                   1                   1
#                 Vx                  Vy                  Vz   skew_ID     Iorth      Ipos        Ip
                 1.0                 0.0                 0.0         0         0         0         0
SUB                1         4
Sublaminate_1
#   Ply_id                 Phi                  Zi
         1                 0.0                 0.0
         1                45.0                 0.0
         1               -45.0                 0.0
         1                90.0                 0.0
```

---

## 4. Interface Pairs (INT)

Defines interfaces between plies for delamination modeling.

### Syntax
```
INT       Pply_IDt  Pply_IDb
```

### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| Pply_IDt | INT | Top ply identifier at interface |
| Pply_IDb | INT | Bottom ply identifier at interface |

### Example with Interface
```
/STACK/3
Laminate_with_Interface
...
# Ply cards
         1                 0.0                 0.0                 0.5                 1.0
         2                90.0                 0.0                 0.5                 1.0
# Interface between ply 1 and ply 2
INT                1         2
```

---

## Ply Orientation Convention

```
             Y (Material Dir 2)
             ^
             |
             |
             +-----> X (Material Dir 1 = Fiber Direction)
            /
           /
          Z (Normal)

Phi = Rotation angle from reference direction to fiber direction
alpha_i = Angle between material direction 1 and 2 (default 90°)
```

---

## Complete Example - Aerospace Panel

```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
# Carbon/Epoxy Material for Ply
/MAT/LAW25/1
Carbon_Epoxy_UD
#              RHO
             1600.0
#                 E1                  E2                 NU12                 G12                 G23                 G31
          135000.0             9000.0               0.30            5000.0            3000.0            5000.0
#
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
# Ply Definition
/PLY/1
UD_Ply_0.25mm
#   mat_ID                   t           delta_phi grsh4n_ID grsh3n_ID   Npt_ply             alpha_i
         1                0.25                 0.0         0         0         3                90.0
# drape_ID  def_orth
         0         2
#
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
# Stack Definition [0/45/-45/90]s (Quasi-Isotropic)
/STACK/1
QuasiIsotropic_Panel
#   Ishell    Ismstr     Ish3n    Idrill        P_thick_fail                  Z0
        24         0         0         0                 0.8                 0.0
#                 Hm                  Hf                  Hr                  Dm                  Dn
              0.01                0.01                0.01                 0.0                 0.0
#                                 Ashear                Iint              Ithick
                   0            0.833333                   1                   1
#                 Vx                  Vy                  Vz   skew_ID     Iorth      Ipos        Ip
                 1.0                 0.0                 0.0         0         0         0         0
# Pply_IDi               Phi_i                 Z_i      P_thick_fail_i          F_weight_i
# Bottom half
         1                 0.0                 0.0                 0.8                 1.0
         1                45.0                 0.0                 0.8                 1.0
         1               -45.0                 0.0                 0.8                 1.0
         1                90.0                 0.0                 0.8                 1.0
# Top half (symmetric)
         1                90.0                 0.0                 0.8                 1.0
         1               -45.0                 0.0                 0.8                 1.0
         1                45.0                 0.0                 0.8                 1.0
         1                 0.0                 0.0                 0.8                 1.0
```

---

## Best Practices

### 1. Ply Definition
- Define separate /PLY entries for each distinct material/thickness combination
- Use appropriate number of integration points (3 recommended for nonlinear analysis)
- Set alpha_i = 90.0 for orthotropic materials

### 2. Stack Definition
- Use QEPH formulation (Ishell=24) for best accuracy
- Set P_thick_fail appropriately for progressive failure analysis
- Use consistent reference direction (Vx, Vy, Vz or skew_ID)

### 3. Layup Conventions
- Define plies from bottom to top
- Use symmetric layups when possible [0/45/-45/90]s
- Standard aerospace layups: [0/±45/90]ns, [±45]ns, [0/90]ns

### 4. Element Selection
- Use shell elements with /PROP/TYPE51 for composite shells
- Ensure adequate mesh density for ply drop-offs
- Consider through-thickness stress requirements

---

## Common Material Laws for Composites

| Law | Description | Usage |
|-----|-------------|-------|
| LAW25 | Orthotropic elastic | Basic composite materials |
| LAW15 | Orthotropic with damage | Damage progression |
| LAW58 | Composite with failure | Detailed failure analysis |

---

## Troubleshooting

### Common Issues

1. **Inconsistent ply orientation**
   - Check reference direction (V vector or skew_ID)
   - Verify def_orth setting in /PLY

2. **Element deletion too early**
   - Increase P_thick_fail value
   - Check failure weight factors F_weight_i

3. **Hourglass modes**
   - Adjust Hm, Hf, Hr coefficients
   - Consider using Ishell=12 or 24

4. **Poor stress distribution**
   - Increase Npt_ply (integration points)
   - Use Iint=2 (Gauss integration) for more accuracy

---

## References

- Source files: `hm_cfg_files/config/CFG/radioss*/LAMINATE/`
- PLY files: `hm_cfg_files/config/CFG/radioss*/PLY/`
- Starter source: `starter/source/materials/composite/`

