# OpenRadioss Keyword Manual - Transform

## Overview

This document describes geometric transformation keywords in OpenRadioss. The /TRANSFORM keyword family allows manipulation of node coordinates through translation, rotation, scaling, symmetry, and matrix operations. These transformations are applied during the Starter phase.

---

## Available Transform Types

| Type | Keyword | Description |
|------|---------|-------------|
| Translation | /TRANSFORM/TRA | Move nodes along a vector |
| Rotation | /TRANSFORM/ROT | Rotate nodes around an axis |
| Scaling | /TRANSFORM/SCA | Scale nodes relative to a point |
| Symmetry | /TRANSFORM/SYM | Mirror nodes across a plane |
| Matrix | /TRANSFORM/MATRIX | General affine transformation |
| Autoposition | /TRANSFORM/AUTOPOSITION | Automatic positioning relative to surface |

---

## 1. TRANSFORM/TRA - Translation

Translates a group of nodes along a specified direction.

### Syntax
```
/TRANSFORM/TRA/trans_ID
trans_title
# grnod_ID       X_translation       Y_translation       Z_translation  node_ID1  node_ID2    sub_ID
#  skew_ID
```

### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| trans_ID | INT | Unique transformation identifier |
| trans_title | STRING | Transformation description (100 chars max) |
| grnod_ID | INT | Node group identifier to transform |
| X_translation | FLOAT | Translation value along global X axis |
| Y_translation | FLOAT | Translation value along global Y axis |
| Z_translation | FLOAT | Translation value along global Z axis |
| node_ID1 | INT | Start node for direction definition (optional) |
| node_ID2 | INT | End node for direction definition (optional) |
| sub_ID | INT | Submodel identifier to transform (optional) |
| skew_ID | INT | Skew identifier for local coordinate system |

### Definition Methods

1. **By coordinates**: Specify X, Y, Z translation values directly
2. **By nodes**: Define direction vector from node_ID1 to node_ID2

### Example
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/TRANSFORM/TRA/1
Translate_100mm_X
# grnod_ID       X_translation       Y_translation       Z_translation  node_ID1  node_ID2    sub_ID
         1                 100.0                 0.0                 0.0         0         0         0
#  skew_ID
         0
```

### Example - Using Node Direction
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/TRANSFORM/TRA/2
Translate_Node_Direction
# grnod_ID       X_translation       Y_translation       Z_translation  node_ID1  node_ID2    sub_ID
         1                   0.0                 0.0                 0.0       101       102         0
#  skew_ID
         0
```

---

## 2. TRANSFORM/ROT - Rotation

Rotates a group of nodes around a specified axis.

### Syntax
```
/TRANSFORM/ROT/trans_ID
trans_title
# grnod_ID           X_point_1           Y_point_1           Z_point_1  node_ID1  node_ID2    sub_ID
#                    X_point_2           Y_point_2           Z_point_2               Angle
```

### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| grnod_ID | INT | Node group identifier to transform |
| X_point_1, Y_point_1, Z_point_1 | FLOAT | First point on rotation axis |
| X_point_2, Y_point_2, Z_point_2 | FLOAT | Second point on rotation axis |
| node_ID1 | INT | First node defining rotation axis (optional) |
| node_ID2 | INT | Second node defining rotation axis (optional) |
| sub_ID | INT | Submodel identifier to transform (optional) |
| Angle | FLOAT | Rotation angle in degrees |

### Definition Methods

1. **By coordinates**: Specify two points defining the rotation axis
2. **By nodes**: Use node_ID1 and node_ID2 to define the axis

### Right-Hand Rule
Rotation follows the right-hand rule: thumb points along the axis (from point 1 to point 2), fingers curl in the positive rotation direction.

### Example - Rotate 90 Degrees Around Z-Axis
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/TRANSFORM/ROT/1
Rotate_90deg_Z
# grnod_ID           X_point_1           Y_point_1           Z_point_1  node_ID1  node_ID2    sub_ID
         1                 0.0                 0.0                 0.0         0         0         0
#                    X_point_2           Y_point_2           Z_point_2               Angle
                           0.0                 0.0                 1.0                90.0
```

### Example - Rotate Around Custom Axis
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/TRANSFORM/ROT/2
Rotate_45deg_Custom_Axis
# grnod_ID           X_point_1           Y_point_1           Z_point_1  node_ID1  node_ID2    sub_ID
         2               100.0               100.0                 0.0         0         0         0
#                    X_point_2           Y_point_2           Z_point_2               Angle
                         100.0               100.0               100.0                45.0
```

---

## 3. TRANSFORM/SCA - Scaling

Scales a group of nodes relative to a center point.

### Syntax
```
/TRANSFORM/SCA/trans_ID
trans_title
# grnod_ID            Fscale_X            Fscale_Y            Fscale_Z  node_IDc              sub_ID
```

### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| grnod_ID | INT | Node group identifier to transform |
| Fscale_X | FLOAT | Scale factor along X axis (must be > 0) |
| Fscale_Y | FLOAT | Scale factor along Y axis (must be > 0) |
| Fscale_Z | FLOAT | Scale factor along Z axis (must be > 0) |
| node_IDc | INT | Center node for scaling (optional, default: origin) |
| sub_ID | INT | Submodel identifier to transform (optional) |

### Formulation
```
X_new = Xc + Fscale_X * (X_old - Xc)
Y_new = Yc + Fscale_Y * (Y_old - Yc)
Z_new = Zc + Fscale_Z * (Z_old - Zc)
```

### Example - Uniform Scaling by 2x
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/TRANSFORM/SCA/1
Scale_2x_Uniform
# grnod_ID            Fscale_X            Fscale_Y            Fscale_Z  node_IDc              sub_ID
         1                 2.0                 2.0                 2.0         0                   0
```

### Example - Non-Uniform Scaling with Center Node
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/TRANSFORM/SCA/2
Scale_NonUniform_Center
# grnod_ID            Fscale_X            Fscale_Y            Fscale_Z  node_IDc              sub_ID
         2                 1.5                 1.0                 2.0       101                   0
```

---

## 4. TRANSFORM/SYM - Symmetry (Mirror)

Reflects nodes across a symmetry plane.

### Syntax
```
/TRANSFORM/SYM/trans_ID
trans_title
# grnod_ID           X_point_1           Y_point_1           Z_point_1  node_ID1  node_ID2    sub_ID
#                    X_point_2           Y_point_2           Z_point_2
```

### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| grnod_ID | INT | Node group identifier to transform |
| X_point_1, Y_point_1, Z_point_1 | FLOAT | First point on symmetry plane |
| X_point_2, Y_point_2, Z_point_2 | FLOAT | Second point defining plane normal |
| node_ID1 | INT | First node on symmetry plane (optional) |
| node_ID2 | INT | Second node defining plane normal (optional) |
| sub_ID | INT | Submodel identifier to transform (optional) |

### Plane Definition
The symmetry plane passes through point 1, with normal direction from point 1 to point 2.

### Example - Mirror Across YZ Plane
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/TRANSFORM/SYM/1
Mirror_YZ_Plane
# grnod_ID           X_point_1           Y_point_1           Z_point_1  node_ID1  node_ID2    sub_ID
         1                 0.0                 0.0                 0.0         0         0         0
#                    X_point_2           Y_point_2           Z_point_2
                           1.0                 0.0                 0.0
```

### Example - Mirror Across Custom Plane
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/TRANSFORM/SYM/2
Mirror_Custom_Plane
# grnod_ID           X_point_1           Y_point_1           Z_point_1  node_ID1  node_ID2    sub_ID
         2               100.0                 0.0                 0.0         0         0         0
#                    X_point_2           Y_point_2           Z_point_2
                         100.0               100.0                 0.0
```

---

## 5. TRANSFORM/MATRIX - General Affine Transformation

Applies a general affine transformation using a 3x3 matrix and translation vector.

### Syntax
```
/TRANSFORM/MATRIX/trans_ID
trans_title
#  grnd_ID                 M11                 M12                 M13                  Tx    sub_ID
#                          M21                 M22                 M23                  Ty
#                          M31                 M32                 M33                  Tz
```

### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| grnd_ID | INT | Node group identifier to transform |
| M11-M33 | FLOAT | 3x3 transformation matrix elements |
| Tx, Ty, Tz | FLOAT | Translation vector components |
| sub_ID | INT | Submodel identifier to transform |

### Formulation
```
| X' |   | M11 M12 M13 | | X |   | Tx |
| Y' | = | M21 M22 M23 | | Y | + | Ty |
| Z' |   | M31 M32 M33 | | Z |   | Tz |
```

### Default (Identity Matrix)
```
M11=1, M12=0, M13=0
M21=0, M22=1, M23=0
M31=0, M32=0, M33=1
Tx=0, Ty=0, Tz=0
```

### Example - Combined Rotation and Translation
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/TRANSFORM/MATRIX/1
Rotate_Z_45deg_Translate
#  grnd_ID                 M11                 M12                 M13                  Tx    sub_ID
         1            0.707107           -0.707107                 0.0               100.0         0
#                          M21                 M22                 M23                  Ty
                       0.707107            0.707107                 0.0                50.0
#                          M31                 M32                 M33                  Tz
                            0.0                 0.0                 1.0                 0.0
```

---

## 6. TRANSFORM/AUTOPOSITION - Automatic Positioning

Automatically positions a node group relative to a reference surface with a specified gap.

### Syntax
```
/TRANSFORM/AUTOPOSITION/trans_ID
trans_title
# grnod_ID   Surf_ID   skew_ID       Dir                 Gap     Pflag
#               Xpos                Ypos                Zpos     Xflag     Yflag     Zflag
```

### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| grnod_ID | INT | Node group identifier to transform |
| Surf_ID | INT | Reference surface identifier |
| skew_ID | INT | Skew identifier for local coordinate system |
| Dir | STRING | Direction of transformation (X, Y, or Z) |
| Gap | FLOAT | Minimum distance between node group and surface |
| Pflag | INT | Position flag (1 or 2) |
| Xpos, Ypos, Zpos | FLOAT | Position constraints |
| Xflag, Yflag, Zflag | INT | Position flags (1: max, 2: min) |

### Pflag Values

| Value | Description |
|-------|-------------|
| 1 | Nodes in grnod_ID are below Surf_ID |
| 2 | Nodes in grnod_ID are above Surf_ID |

### Position Flags

| Value | Description |
|-------|-------------|
| 1 | Position value is maximum coordinate |
| 2 | Position value is minimum coordinate |

### Example - Position Above Surface with 5mm Gap
```
#---1----|----2----|----3----|----5----|----6----|----7----|----8----|
/TRANSFORM/AUTOPOSITION/1
Position_Above_Surface
# grnod_ID   Surf_ID   skew_ID       Dir                 Gap     Pflag
         1         1         0         Z                 5.0         2
#               Xpos                Ypos                Zpos     Xflag     Yflag     Zflag
                 0.0                 0.0                 0.0         1         1         1
```

---

## Transformation Order

Multiple transformations are applied in order of their IDs (ascending). The ORDER parameter can be used to override this:

```
ORDER = -1  : Default (use ID order)
ORDER = n   : Apply in custom order (lower values first)
```

---

## Common Use Cases

### 1. Assembly Positioning
```
# Position component 100mm in X
/TRANSFORM/TRA/1
Position_Component
         1               100.0                 0.0                 0.0         0         0         0
         0
```

### 2. Creating Symmetric Models
```
# Mirror half model to create full model
/TRANSFORM/SYM/1
Mirror_Half_Model
         1                 0.0                 0.0                 0.0         0         0         0
                           1.0                 0.0                 0.0
```

### 3. Unit Conversion
```
# Convert mm to m (scale by 0.001)
/TRANSFORM/SCA/1
Convert_mm_to_m
         1               0.001               0.001               0.001         0                   0
```

### 4. Rotating Components for Assembly
```
# Rotate wheel 45 degrees around Z-axis
/TRANSFORM/ROT/1
Rotate_Wheel
         1                 0.0                 0.0                 0.0         0         0         0
                           0.0                 0.0                 1.0                45.0
```

---

## Best Practices

1. **Order Matters**: Apply transformations in logical order (rotate then translate vs. translate then rotate give different results)

2. **Use Node Groups**: Create specific node groups for transformations to avoid unintended changes

3. **Center Points**: For rotation and scaling, carefully choose the center point/axis

4. **Submodel Support**: Use sub_ID to transform entire submodels consistently

5. **Verification**: Check transformed geometry visually before running analysis

---

## Troubleshooting

### Common Issues

1. **Transformation not applied**
   - Check grnod_ID references correct node group
   - Verify transformation parameters are non-zero

2. **Unexpected rotation center**
   - Verify axis points are correct
   - Check if origin should be used instead

3. **Scale factors must be positive**
   - Use symmetry (SYM) instead for negative scaling

4. **Node group not found**
   - Ensure node group is defined before transformation

---

## References

- Source files: `hm_cfg_files/config/CFG/radioss*/TRANSFORM/`
- Starter source: `starter/source/model/transform/`

