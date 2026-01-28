# OpenRadioss Keyword Manual - Constraints

## Overview

This document describes constraint definitions in OpenRadioss. Constraints define kinematic relationships between nodes, rigid bodies, and contact with fixed surfaces.

---

## 1. RBODY - Rigid Body

Defines a rigid body entity that constrains secondary nodes to move with a primary node.

### Syntax
```
/RBODY/rbody_ID
rbody_title
#  rb_ID sens_ID  skew_ID   Ispher                Mass grnod_ID    Ikrem     ICoG  surf_ID
#                  Jxx                 Jyy                 Jzz
#                  Jxy                 Jyz                 Jxz
#Ioptoff
```

### Parameters - Card 1

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| rb_ID | INT | - | Primary node ID (center of mass) |
| sens_ID | INT | 0 | Sensor property ID |
| skew_ID | INT | 0 | Skew system for inertia definition |
| Ispher | INT | 0 | Spherical inertia flag |
| Mass | FLOAT | 0.0 | Total mass of rigid body |
| grnod_ID | INT | - | Node group of secondary nodes |
| Ikrem | INT | 0 | Rigid wall deactivation flag |
| ICoG | INT | 1 | Center of gravity computation flag |
| surf_ID | INT | 0 | Surface defining envelope |

### Ispher Options

| Value | Description |
|-------|-------------|
| 0 | Use provided data |
| 1 | Spherical inertia |
| 2 | Auto-correct inertia |
| 3 | Use data as-is |

### ICoG Options

| Value | Description |
|-------|-------------|
| 0 | Default (→1) |
| 1 | CoG at main node coordinates |
| 2 | Secondary node mass only |
| 3 | CoG at main node (variant) |
| 4 | CoG at main node (variant) |

### Parameters - Card 2-3 (Inertia)

| Parameter | Type | Description |
|-----------|------|-------------|
| Jxx, Jyy, Jzz | FLOAT | Principal inertia components |
| Jxy, Jyz, Jxz | FLOAT | Off-diagonal inertia components |

### Example
```
/RBODY/1
Rigid_Mass
       100         0        10         0              50.0       200         0         1         0
               10.5               12.3               11.8
                0.2                0.1               0.15
         0
```

---

## 2. RBE2 - Rigid Body Element Type 2

Creates a rigid connection between an independent node and dependent nodes.

### Syntax
```
/RBE2/rbe2_ID
rbe2_title
#node_ID  Trarot  skew_ID grnod_ID    Iflag
```

### Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| node_ID | INT | - | Independent (master) node |
| Trarot | INT | - | DOF codes (6 digits: Tx Ty Tz Rx Ry Rz) |
| skew_ID | INT | 0 | Local coordinate system |
| grnod_ID | INT | - | Group of dependent nodes |
| Iflag | INT | 0 | Formulation flag |

### Trarot Encoding
- Format: `TXTYTZRXRYRZ` (each digit is 0 or 1)
- Example: `111111` = all DOF constrained (full rigid)
- Example: `111000` = translations only (hinge-like)
- Example: `100100` = X translation and X rotation only

### Iflag Options

| Value | Description |
|-------|-------------|
| 0 | Rigid body formulation (OptiStruct) |
| 1 | Rigid link formulation |

### Example
```
/RBE2/1
Rigid_Connection
       100  111111        0       200         0
```

---

## 3. RBE3 - Rigid Body Element Type 3 (Interpolation)

Creates an interpolation constraint where a reference node is computed from weighted independent nodes.

### Syntax
```
/RBE3/rbe3_ID
rbe3_title
#Node_IDr Trarot_ref   N_set I_modif
#   WTi Trarot_Mi skew_IDi grnod_IDi
# [repeat for each set]
```

### Parameters - Card 1

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| Node_IDr | INT | - | Reference (dependent) node |
| Trarot_ref | INT | - | DOF codes for reference node |
| N_set | INT | 1 | Number of interpolation sets |
| I_modif | INT | 1 | Weight modification flag |

### Parameters - Per Set

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| WTi | FLOAT | 1.0 | Weighting factor for set i |
| Trarot_Mi | INT | - | DOF codes for set i |
| skew_IDi | INT | 0 | Skew for set i |
| grnod_IDi | INT | - | Node group for set i |

### I_modif Options

| Value | Description |
|-------|-------------|
| 0 | Default (→1) |
| 1 | Auto-normalize weights |
| 2 | Forbidden (error) |
| 3 | Unity weights |

### Usage Notes
- Dependent node motion = weighted average of independent nodes
- Useful for load distribution and stress smoothing
- Does not add stiffness (distributes loads only)

### Example
```
/RBE3/1
Load_Distribution
        50  111111        2         1
              20.0  111111        0       100
              30.0  111111        0       200
```

---

## 4. MPC - Multi-Point Constraint

Creates linear constraint equations linking multiple nodes.

### Syntax
```
/MPC/mpc_ID
mpc_title
#node_id    idof     isk              alpha
# [repeat for each node]
```

### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| node_id | INT | Node ID in constraint equation |
| idof | INT | DOF code (1=Tx, 2=Ty, 3=Tz, 4=Rx, 5=Ry, 6=Rz) |
| isk | INT | Local skew for this DOF |
| alpha | FLOAT | Coefficient in constraint equation |

### Constraint Equation
```
alpha_1 * u_1 + alpha_2 * u_2 + ... + alpha_n * u_n = 0
```

### Example - Proportional Displacement
```
/MPC/1
Proportional_Constraint
# 2*u1 - u2 = 0 (u2 = 2*u1)
        10         1         0               2.0
        20         1         0              -1.0
```

### Example - Gear Ratio
```
/MPC/2
Gear_Ratio_3to1
# rot1 + 3*rot2 = 0
       100         4         0               1.0
       200         4         0               3.0
```

---

## 5. RLINK - Rigid Link

Creates a rigid link between primary and secondary nodes with optional polar coordinates.

### Syntax
```
/RLINK/rlink_ID
rlink_title
#Trarot  skew_ID frame_ID grnod_ID    Ipol
```

### Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| Trarot | INT | - | DOF constraint code |
| skew_ID | INT | 0 | Frame/skew identifier |
| frame_ID | INT | - | Second frame (for Ipol=1) |
| grnod_ID | INT | - | Secondary node group |
| Ipol | INT | 0 | Polar rigid link flag |

### Ipol Options

| Value | Description |
|-------|-------------|
| 0 | Cartesian rigid link |
| 1 | Polar/cylindrical coordinates |

### Example
```
/RLINK/1
Standard_Link
  123456        0         0       200         0
```

---

## 6. RWALL - Rigid Wall

Defines a rigid, non-deformable contact surface.

### Common Syntax
```
/RWALL/type/rwall_ID
rwall_title
#node_ID    Slide grnod_ID1 grnod_ID2
#                 d                fric            Diameter                         ifq
#               XM1                YM1                ZM1
#               XM2                YM2                ZM2
```

### RWALL Types

| Type | Description |
|------|-------------|
| PLANE | Infinite plane |
| SPHER | Spherical surface |
| CYL | Cylindrical surface |
| PARAL | Parallelogram (finite plane) |

### Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| node_ID | INT | - | Base/moving node |
| Slide | INT | - | Sliding flag |
| grnod_ID1 | INT | 0 | Nodes to add to wall |
| grnod_ID2 | INT | 0 | Nodes to exclude |
| d | FLOAT | 0.0 | Search distance |
| fric | FLOAT | 0.0 | Friction coefficient |
| Diameter | FLOAT | - | Size parameter |
| ifq | INT | 0 | Filtering flag |
| XM1, YM1, ZM1 | FLOAT | - | First point coordinates |
| XM2, YM2, ZM2 | FLOAT | - | Second point coordinates |

### Slide Options

| Value | Description |
|-------|-------------|
| 0 | Sliding contact (no friction) |
| 1 | Tied contact (no sliding) |
| 2 | Sliding with friction |

### Example - Plane Wall
```
/RWALL/PLANE/1
Ground_Plane
         0         0       100         0
               0.0               0.3               0.0                         0
               0.0               0.0               0.0
               1.0               0.0               0.0
```

### Example - Cylindrical Wall
```
/RWALL/CYL/2
Cylinder_Die
       200         2        50         0
               0.0               0.3             100.0                         0
               0.0               0.0               0.0
               0.0               0.0             500.0
```

---

## 7. SKEW - Local Coordinate System

Defines local coordinate systems for use with constraints and loads.

### 7.1 SKEW/FIX - Fixed Coordinate System

```
/SKEW/FIX/skew_ID
skew_title
#                Ox                 Oy                 Oz
#                X1                 Y1                 Z1
#                X2                 Y2                 Z2
```

### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| Ox, Oy, Oz | FLOAT | Origin coordinates |
| X1, Y1, Z1 | FLOAT | Y' axis direction vector |
| X2, Y2, Z2 | FLOAT | Z' axis direction vector |

### Coordinate System Definition
- Origin: (Ox, Oy, Oz)
- Z' axis: Direction (X2, Y2, Z2)
- Y' axis: Direction (X1, Y1, Z1)
- X' axis: Computed as Z' × Y'

### Example
```
/SKEW/FIX/1
Local_System_45deg
               0.0               0.0               0.0
            0.7071            0.7071               0.0
               0.0               0.0               1.0
```

### 7.2 SKEW/MOV2 - Node-Based Moving Skew

```
/SKEW/MOV2/skew_ID
skew_title
#node_ID1 node_ID2 node_ID3
```

### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| node_ID1 | INT | Origin node |
| node_ID2 | INT | Z-axis node (N1→N2 defines Z') |
| node_ID3 | INT | Plane node (defines XZ' plane) |

### Example
```
/SKEW/MOV2/2
Moving_Frame
       100       200       300
```

---

## Constraint Summary Table

| Keyword | Type | Masters | Slaves | DOF Control | Main Use |
|---------|------|---------|--------|-------------|----------|
| /RBODY | Rigid | 1 | Many | All 6 | General rigid body |
| /RBE2 | Rigid | 1 | Many | Selectable | Rigid connection |
| /RBE3 | Interp | Many | 1 | Selectable | Load distribution |
| /MPC | Linear | Many | Many | Per-node | Complex coupling |
| /RLINK | Rigid | 1 | Many | Encoded | Efficient link |
| /RWALL | Contact | 0 | Many | Normal | Contact surface |
| /SKEW | Coord | - | - | - | Local system |

---

## Constraint Type Comparison

| Feature | RBODY | RBE2 | RBE3 | MPC |
|---------|-------|------|------|-----|
| Rigid constraint | Yes | Yes | No | Depends |
| Adds mass | Optional | No | No | No |
| Adds inertia | Optional | No | No | No |
| DOF selection | No | Yes | Yes | Yes |
| Load distribution | No | No | Yes | Yes |
| Linear relation | No | No | Yes | Yes |

---

## Best Practices

1. **Rigid Body (RBODY)**:
   - Use for parts that should remain rigid
   - Specify mass and inertia for accurate dynamics
   - Primary node should be at center of mass

2. **RBE2**:
   - Use for rigid connections between parts
   - Select only needed DOF to avoid over-constraining
   - Check for redundant constraints

3. **RBE3**:
   - Use for load distribution without adding stiffness
   - Good for applying loads to multiple nodes
   - Weights determine load sharing

4. **MPC**:
   - Use for complex kinematic relations
   - Can model gear ratios, proportional motion
   - Requires careful coefficient definition

5. **Rigid Wall**:
   - Use for contact with fixed surfaces
   - Moving walls: define node_ID
   - Set appropriate friction coefficient

---

## Example: Complete Constraint Setup

```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
# Rigid body for engine block
/RBODY/1
Engine_Block
       500         0         0         0             150.0       100         0         1         0
              5.0e6              8.0e6              6.0e6
                0.0                0.0                0.0
         0

# Rigid connection for mounting bolts
/RBE2/1
Bolt_Connection
       600  111111        0       200         0

# Load distribution spider
/RBE3/1
Load_Spider
       700  111111        4         1
               1.0  111111        0       300
               1.0  111111        0       301
               1.0  111111        0       302
               1.0  111111        0       303

# Ground contact
/RWALL/PLANE/1
Ground
         0         2      1000         0
               0.0               0.3               0.0                         0
               0.0               0.0               0.0
               1.0               0.0               0.0
```

---

## References

- Source files: `hm_cfg_files/config/CFG/radioss110/RBODY/`
- Source files: `hm_cfg_files/config/CFG/radioss110/RWALL/`
- Starter source: `starter/source/constraints/`
