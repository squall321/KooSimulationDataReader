# OpenRadioss Keyword Manual - Coordinate Systems (SKEW/FRAME)

## Overview

This document describes coordinate system and reference frame keywords in OpenRadioss. Skew systems define local coordinate systems for applying loads, constraints, and outputting results.

---

## Available Coordinate System Types

| Type | Keyword | Description |
|------|---------|-------------|
| Fixed Skew | /SKEW/FIX | Fixed local coordinate system |
| Moving Skew | /SKEW/MOV | Moving coordinate system (follows nodes) |
| Fixed Frame | /FRAME/FIX | Fixed reference frame |
| Moving Frame | /FRAME/MOV | Moving reference frame |
| Nodal Frame | /FRAME/NOD | Frame defined by node positions |

---

## 1. SKEW/FIX - Fixed Skew System

The /SKEW/FIX keyword defines a fixed (inertial) local coordinate system using direction vectors.

### Syntax
```
/SKEW/FIX/skew_ID
skew_title
#                 Ox                  Oy                  Oz
#                 X1                  Y1                  Z1
#                 X2                  Y2                  Z2
```

### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| skew_ID | INT | Unique skew system identifier |
| skew_title | STRING | Skew system description |
| Ox, Oy, Oz | FLOAT | Origin point coordinates |
| X1, Y1, Z1 | FLOAT | Y' axis direction vector |
| X2, Y2, Z2 | FLOAT | Z' axis direction vector |

### Coordinate System Definition

The local axes are computed as:
- **Y' axis**: Normalized (X1, Y1, Z1) vector
- **Z' axis**: Component of (X2, Y2, Z2) perpendicular to Y'
- **X' axis**: Cross product Y' × Z'

### Example
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/SKEW/FIX/1
Vehicle_Local_Skew
#                 Ox                  Oy                  Oz
               0.000               0.000               0.000
#                 X1                  Y1                  Z1
               0.000               1.000               0.000
#                 X2                  Y2                  Z2
               0.000               0.000               1.000
```

---

## 2. SKEW/MOV - Moving Skew System

The /SKEW/MOV keyword defines a coordinate system that moves with the model (follows nodes).

### Syntax
```
/SKEW/MOV/skew_ID
skew_title
#       N1        N2        N3       DIR
```

### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| skew_ID | INT | Unique skew system identifier |
| skew_title | STRING | Skew system description |
| N1 | INT | Origin node identifier |
| N2 | INT | Node defining primary axis direction |
| N3 | INT | Node defining secondary axis plane |
| DIR | STRING | Primary axis direction (X, Y, or Z) |

### Coordinate System Definition

- **Origin**: Located at node N1
- **Primary axis** (specified by DIR): Points from N1 toward N2
- **Secondary axis**: Lies in the plane N1-N2-N3
- **Tertiary axis**: Perpendicular to both (right-hand rule)

### DIR Values

| Value | Description |
|-------|-------------|
| X | N1→N2 defines local X axis |
| Y | N1→N2 defines local Y axis (default) |
| Z | N1→N2 defines local Z axis |

### Example
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/SKEW/MOV/2
Deforming_Body_Skew
#       N1        N2        N3       DIR
       100       101       102         X
```

---

## 3. FRAME/FIX - Fixed Reference Frame

The /FRAME/FIX keyword defines a fixed reference frame, similar to SKEW/FIX but with additional features for frame-based calculations.

### Syntax
```
/FRAME/FIX/frame_ID
frame_title
#                 Ox                  Oy                  Oz
#                 X1                  Y1                  Z1
#                 X2                  Y2                  Z2
```

### Parameters

Same as /SKEW/FIX.

### Example
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/FRAME/FIX/1
Global_Reference_Frame
#                 Ox                  Oy                  Oz
               0.000               0.000               0.000
#                 X1                  Y1                  Z1
               1.000               0.000               0.000
#                 X2                  Y2                  Z2
               0.000               1.000               0.000
```

---

## 4. FRAME/MOV - Moving Reference Frame

The /FRAME/MOV keyword defines a reference frame that follows nodal motion.

### Syntax
```
/FRAME/MOV/frame_ID
frame_title
#       N1        N2        N3
```

### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| frame_ID | INT | Unique frame identifier |
| frame_title | STRING | Frame description |
| N1 | INT | Origin node identifier |
| N2 | INT | Node defining X' axis direction |
| N3 | INT | Node in X'Y' plane |

### Example
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/FRAME/MOV/3
Moving_Body_Frame
#       N1        N2        N3
       200       201       202
```

---

## 5. FRAME/NOD - Nodal Reference Frame

The /FRAME/NOD keyword defines a reference frame based on nodal positions that updates during analysis.

### Syntax
```
/FRAME/NOD/frame_ID
frame_title
#       N1        N2        N3
```

### Parameters

Same as /FRAME/MOV.

### Difference from FRAME/MOV

- **FRAME/MOV**: Rigid body motion between N1, N2, N3
- **FRAME/NOD**: Updates based on actual deformed nodal positions

---

## Applications

### Load Direction

Skew systems can specify load directions:

```
/CLOAD/cload_ID
#  func_ID   sens_ID  skew_ID     Grnod    Icoor       Dir   Icoordinate_System
```

### Constraint Direction

Boundary conditions in local coordinates:

```
/BCS/bcs_ID
#  Itranslat      Irotation   skew_ID   grnod_ID
```

### Output Transformation

Results in local coordinate system:

```
/TH/NODE/thnode_ID
# Variables output in skew coordinates when skew_ID specified
```

---

## Comparison: SKEW vs FRAME

| Feature | SKEW | FRAME |
|---------|------|-------|
| Basic coordinate system | ✓ | ✓ |
| Load/BC direction | ✓ | ✓ |
| Output transformation | ✓ | ✓ |
| Section cut reference | Limited | Full support |
| Accelerometer reference | Limited | Full support |

---

## Coordinate Transformation

### Global to Local

For a point P in global coordinates:
```
P_local = R^T × (P_global - Origin)
```

Where R is the rotation matrix formed by local axes.

### Local to Global

```
P_global = R × P_local + Origin
```

---

## Best Practices

### Choosing Skew Type

| Scenario | Recommended Type |
|----------|------------------|
| Fixed boundary conditions | /SKEW/FIX |
| Moving rigid body loads | /SKEW/MOV |
| Deformable body tracking | /FRAME/MOV |
| Section force output | /FRAME/MOV or /FRAME/NOD |
| Accelerometer in deforming body | /FRAME/NOD |

### Node Selection for Moving Skew

1. Choose nodes that are well-separated (not collinear)
2. Ensure nodes won't fail during simulation
3. Prefer nodes on rigid or stiff regions
4. Avoid highly deformed areas

### Numerical Stability

1. Ensure direction vectors are not parallel
2. Use normalized vectors when possible
3. Check that origin is not at singular locations

---

## Example - Complete Skew Setup for Crash Test

```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
# Vehicle-fixed coordinate system (SAE J211 convention)
/SKEW/FIX/1
Vehicle_SAE_J211
#                 Ox                  Oy                  Oz
               0.000               0.000               0.000
#                 X1                  Y1                  Z1
               1.000               0.000               0.000
#                 X2                  Y2                  Z2
               0.000               0.000               1.000

# Moving frame on B-pillar for accelerometer
/FRAME/MOV/10
B_Pillar_Accelerometer_Frame
#       N1        N2        N3
     10001     10002     10003

# Moving frame on steering column
/FRAME/NOD/20
Steering_Column_Frame
#       N1        N2        N3
     20001     20002     20003

# Angled load application
/SKEW/FIX/100
Angled_Load_Direction
#                 Ox                  Oy                  Oz
               0.000               0.000               0.000
#                 X1                  Y1                  Z1
               0.866               0.500               0.000
#                 X2                  Y2                  Z2
               0.000               0.000               1.000
```

---

## Troubleshooting

### Common Issues

| Issue | Cause | Solution |
|-------|-------|----------|
| Invalid skew warning | Parallel direction vectors | Use non-parallel vectors |
| Skew jumps during analysis | Poor node selection | Choose more stable nodes |
| Zero moment arm | Origin at constraint point | Move origin away |
| Incorrect output direction | Wrong axis definition | Verify axis vectors |

### Validation

Check skew definition by:
1. Visualizing in preprocessor
2. Outputting unit vectors in listing file
3. Applying simple test loads and verifying direction

---

## References

- SKEW source: `hm_cfg_files/config/CFG/radioss*/SYSTEM/`
- FRAME source: `hm_cfg_files/config/CFG/radioss*/SYSTEM/`
- Starter source: `starter/source/model/skew/`

