# OpenRadioss Keyword Manual - Rigid Wall

## Overview

This document describes rigid wall (/RWALL) keywords in OpenRadioss. Rigid walls are used to define fixed or moving impactor surfaces for contact simulation without meshing the contact surface.

---

## Available Rigid Wall Types

| Type | Keyword | Description |
|------|---------|-------------|
| Infinite Plane | /RWALL/PLANE | Infinite planar rigid wall |
| Sphere | /RWALL/SPHER | Spherical rigid wall |
| Cylinder | /RWALL/CYL | Cylindrical rigid wall |
| Parallelogram | /RWALL/PARAL | Finite parallelogram wall |
| Lagrangian | /RWALL/LAGMUL | Lagrange multiplier wall |
| Thermal | /RWALL/THERM | Thermal rigid wall |

---

## 1. RWALL/PLANE - Infinite Planar Rigid Wall

The /RWALL/PLANE keyword defines an infinite planar rigid wall.

### Syntax
```
/RWALL/PLANE/rwall_ID
rwall_title
#  node_ID     Slide  grnd_ID1  grnd_ID2     Iform
#                  d                fric            Diameter                ffac       ifq
#                 XM                  YM                  ZM
#                XM1                 YM1                 ZM1
```

### Parameters - Card 1

| Parameter | Type | Description |
|-----------|------|-------------|
| rwall_ID | INT | Rigid wall identifier |
| rwall_title | STRING | Rigid wall description |

### Parameters - Card 2

| Parameter | Type | Description |
|-----------|------|-------------|
| node_ID | INT | Node ID for moving wall (0 = fixed wall) |
| Slide | INT | Sliding flag (0=slide, 1=tied, 2=slide+friction) |
| grnd_ID1 | INT | Node group to include |
| grnd_ID2 | INT | Node group to exclude |
| Iform | INT | Formulation flag (0=kinematic, 1=penalty) |

### Parameters - Card 3

| Parameter | Type | Description |
|-----------|------|-------------|
| d | FLOAT | Search distance for secondary nodes |
| fric | FLOAT | Friction coefficient (Slide=2 only) |
| Diameter | FLOAT | Not used for PLANE |
| ffac | FLOAT | Filtering factor |
| ifq | INT | Filtering flag |

### Parameters - Card 4 (Fixed Wall: node_ID = 0)

| Parameter | Type | Description |
|-----------|------|-------------|
| XM | FLOAT | X coordinate of wall point M |
| YM | FLOAT | Y coordinate of wall point M |
| ZM | FLOAT | Z coordinate of wall point M |

### Parameters - Card 4 (Moving Wall: node_ID ≠ 0)

| Parameter | Type | Description |
|-----------|------|-------------|
| Mass | FLOAT | Mass of rigid wall |
| VX0 | FLOAT | Initial velocity X |
| VY0 | FLOAT | Initial velocity Y |
| VZ0 | FLOAT | Initial velocity Z |

### Parameters - Card 5

| Parameter | Type | Description |
|-----------|------|-------------|
| XM1 | FLOAT | X component of wall normal |
| YM1 | FLOAT | Y component of wall normal |
| ZM1 | FLOAT | Z component of wall normal |

### Slide Flag Values

| Value | Description |
|-------|-------------|
| 0 | Nodes can slide on wall surface |
| 1 | Nodes tied to wall (no sliding) |
| 2 | Sliding with Coulomb friction |

### Iform Flag Values

| Value | Description |
|-------|-------------|
| 0 | Kinematic formulation (default) |
| 1 | Penalty formulation |

### ifq Filtering Flag

| Value | Description |
|-------|-------------|
| 0 | No filtering |
| 1 | User-defined coefficient |
| 2 | 3dB for user time step |
| 3 | 3dB for user frequency |

### Example - Fixed Plane Wall
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/RWALL/PLANE/1
Fixed_Ground_Plane
#  node_ID     Slide  grnd_ID1  grnd_ID2     Iform
         0         0         1         0         0
#                  d                fric            Diameter                ffac       ifq
               5.000                                                                     0
#                 XM                  YM                  ZM
               0.000               0.000               0.000
#                XM1                 YM1                 ZM1
               0.000               0.000               1.000
```

### Example - Moving Wall with Friction
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/RWALL/PLANE/2
Moving_Barrier
#  node_ID     Slide  grnd_ID1  grnd_ID2     Iform
       100         2         2         0         0
#                  d                fric            Diameter                ffac       ifq
              10.000               0.300                                                 0
#               Mass                 VX0                 VY0                 VZ0
            1000.000              15.000               0.000               0.000
#                XM1                 YM1                 ZM1
              -1.000               0.000               0.000
```

---

## 2. RWALL/SPHER - Spherical Rigid Wall

The /RWALL/SPHER keyword defines a spherical rigid wall.

### Syntax
```
/RWALL/SPHER/rwall_ID
rwall_title
#  node_ID     Slide  grnd_ID1  grnd_ID2     Iform
#                  d                fric            Diameter                ffac       ifq
#                 XM                  YM                  ZM    (or Mass, VX0, VY0, VZ0)
```

### Additional Parameter

| Parameter | Type | Description |
|-----------|------|-------------|
| Diameter | FLOAT | Diameter of the sphere |

### Example - Fixed Sphere
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/RWALL/SPHER/3
Spherical_Impactor
#  node_ID     Slide  grnd_ID1  grnd_ID2     Iform
         0         0         1         0         0
#                  d                fric            Diameter                ffac       ifq
               5.000                              200.000                               0
#                 XM                  YM                  ZM
             500.000               0.000             100.000
```

---

## 3. RWALL/CYL - Cylindrical Rigid Wall

The /RWALL/CYL keyword defines a cylindrical rigid wall.

### Syntax
```
/RWALL/CYL/rwall_ID
rwall_title
#  node_ID     Slide  grnd_ID1  grnd_ID2     Iform
#                  d                fric            Diameter                ffac       ifq
#                 XM                  YM                  ZM    (or Mass, VX0, VY0, VZ0)
#                XM1                 YM1                 ZM1
```

### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| Diameter | FLOAT | Diameter of the cylinder |
| XM1, YM1, ZM1 | FLOAT | Cylinder axis direction vector |

### Example - Cylindrical Roller
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/RWALL/CYL/4
Cylindrical_Roller
#  node_ID     Slide  grnd_ID1  grnd_ID2     Iform
         0         0         1         0         0
#                  d                fric            Diameter                ffac       ifq
               5.000                              100.000                               0
#                 XM                  YM                  ZM
               0.000               0.000              50.000
#                XM1                 YM1                 ZM1
               1.000               0.000               0.000
```

---

## 4. RWALL/PARAL - Parallelogram Rigid Wall

The /RWALL/PARAL keyword defines a finite parallelogram-shaped wall.

### Syntax
```
/RWALL/PARAL/rwall_ID
rwall_title
#  node_ID     Slide  grnd_ID1  grnd_ID2     Iform
#                  d                fric            Diameter                ffac       ifq
#                 XM                  YM                  ZM    (or Mass, VX0, VY0, VZ0)
#                XM1                 YM1                 ZM1
#                XM2                 YM2                 ZM2
```

### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| XM, YM, ZM | FLOAT | Corner point M coordinates |
| XM1, YM1, ZM1 | FLOAT | Corner point M1 coordinates |
| XM2, YM2, ZM2 | FLOAT | Corner point M2 coordinates |

### Geometry

The parallelogram is defined by three corners:
- M: First corner (origin)
- M1: Second corner (defines first edge M→M1)
- M2: Third corner (defines second edge M→M2)
- Fourth corner computed as M + (M1-M) + (M2-M)

### Example - Finite Wall Segment
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/RWALL/PARAL/5
Finite_Barrier_Wall
#  node_ID     Slide  grnd_ID1  grnd_ID2     Iform
         0         2         1         0         0
#                  d                fric            Diameter                ffac       ifq
               5.000               0.300                                                 0
#                 XM                  YM                  ZM
               0.000            -500.000               0.000
#                XM1                 YM1                 ZM1
               0.000             500.000               0.000
#                XM2                 YM2                 ZM2
               0.000            -500.000             200.000
```

---

## Contact Detection

### Search Distance

The `d` parameter defines the search distance for identifying contacting nodes:
- Nodes within distance `d` from the wall surface are checked for contact
- Larger values increase computation but ensure no missed contacts
- Typical: 1-5 times expected maximum penetration

### Node Groups

| Parameter | Usage |
|-----------|-------|
| grnd_ID1 | Include nodes from this group in contact check |
| grnd_ID2 | Exclude nodes from this group (override grnd_ID1) |

---

## Formulation Options

### Kinematic Formulation (Iform = 0)

- Direct velocity/position correction
- No contact force output
- Robust for high-speed impacts
- Default formulation

### Penalty Formulation (Iform = 1)

- Spring-based contact force
- Contact forces available in output
- Better for quasi-static loading
- May require tuning for stability

---

## Moving Rigid Walls

For moving walls (node_ID ≠ 0):

1. **Attached to Node**: Wall follows the motion of the specified node
2. **Free Motion**: If Mass and initial velocity are specified, wall moves as a rigid body with computed contact forces
3. **Coupled Motion**: The attached node receives contact reaction forces

### Example - Impactor with Mass
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/RWALL/PLANE/10
Dropping_Mass_Impactor
#  node_ID     Slide  grnd_ID1  grnd_ID2     Iform
      1000         0         1         0         0
#                  d                fric            Diameter                ffac       ifq
              10.000                                                                     0
#               Mass                 VX0                 VY0                 VZ0
             500.000               0.000               0.000             -10.000
#                XM1                 YM1                 ZM1
               0.000               0.000               1.000
```

---

## Best Practices

### General Guidelines

1. **Search Distance**: Set `d` to 2-5 times the expected contact gap
2. **Node Groups**: Always define grnd_ID1 to limit contact checking
3. **Friction**: Use Slide=2 with reasonable friction (0.1-0.5)
4. **Moving Walls**: Prefer attaching to a node for controlled motion

### Performance Tips

1. Minimize nodes in grnd_ID1 for faster computation
2. Use appropriate geometry (infinite plane is fastest)
3. Avoid overlapping rigid walls
4. Use kinematic formulation for explicit dynamics

### Troubleshooting

| Issue | Solution |
|-------|----------|
| Nodes pass through | Increase search distance `d` |
| Oscillations | Enable filtering (ifq > 0) |
| Contact noise | Use penalty formulation |
| Slow computation | Reduce grnd_ID1 size |

---

## Example - Complete Crash Barrier Setup

```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
# Ground plane
/RWALL/PLANE/1
Ground
         0         0         1         0         0
              10.000
               0.000               0.000               0.000
               0.000               0.000               1.000

# Front barrier (moving)
/RWALL/PLANE/2
Frontal_Barrier
      1000         2         2         0         0
              10.000               0.300                                                 0
            5000.000              15.000               0.000               0.000
              -1.000               0.000               0.000

# Side barrier (fixed)
/RWALL/PLANE/3
Side_Barrier
         0         2         3         0         0
              10.000               0.200                                                 0
            1000.000               0.000               0.000
               0.000               1.000               0.000
```

---

## References

- RWALL source: `hm_cfg_files/config/CFG/radioss*/RWALL/`
- Starter source: `starter/source/interfaces/rwall/`
- Engine source: `engine/source/interfaces/rwall/`

