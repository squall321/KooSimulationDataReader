# OpenRadioss Keyword Manual - Flexible Bodies and Clusters

## Overview

This document describes flexible body (/FXBODY) and element cluster (/CLUSTER) keywords in OpenRadioss for component-based and substructure modeling.

---

## 1. FXBODY - Flexible Body

The /FXBODY keyword imports a flexible body from a modal analysis for efficient crash simulation of deformable components.

### Syntax
```
/FXBODY/fxbody_ID
fxbody_title
# node_IDm     Ianim      Imin      Imax
#Filename
```

### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| fxbody_ID | INT | Unique flexible body identifier |
| fxbody_title | STRING | Flexible body description |
| node_IDm | INT | Main (master) node identifier |
| Ianim | INT | Animation output flag (0=no, 1=yes) |
| Imin | INT | Minimum mode index for animation |
| Imax | INT | Maximum mode index for animation |
| Filename | STRING | Flexible body input file name (.h3d) |

### Flexible Body File

The input file contains:
- Modal shapes from eigenvalue analysis
- Mass and stiffness matrices
- Damping information
- Node and element definitions

### Example
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/FXBODY/1
Steering_Column_FlexBody
# node_IDm     Ianim      Imin      Imax
      1000         1         1        20
steering_column.h3d
```

### Use Cases

1. **Steering column**: Deformable column with breakaway behavior
2. **Instrument panel**: Flexible IP for occupant interaction
3. **Seat frame**: Deformable seat structure
4. **Door assembly**: Flexible door for side impact

### Benefits

| Aspect | Flexible Body | Full FE |
|--------|---------------|---------|
| DOF count | Reduced (modes) | Full mesh |
| CPU time | Lower | Higher |
| Detail level | Modal response | Full detail |
| Pre-processing | Modal analysis required | Direct meshing |

---

## 2. EIG - Eigenvalue Analysis Request

The /EIG keyword requests eigenvalue (modal) analysis for creating flexible bodies.

### Syntax
```
/EIG/eig_ID
# Parameters for eigenvalue extraction
```

### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| Nmode | INT | Number of modes to extract |
| Fmin | FLOAT | Minimum frequency |
| Fmax | FLOAT | Maximum frequency |

---

## 3. CLUSTER - Element Clustering

The /CLUSTER keyword groups elements for global failure criteria and parallel processing optimization.

### Available Types

| Type | Keyword | Description |
|------|---------|-------------|
| Brick | /CLUSTER/BRICK | Solid element cluster |
| Spring | /CLUSTER/SPRING | Spring element cluster |

### Syntax
```
/CLUSTER/type/cluster_ID
cluster_title
# group_ID   skew_ID     Ifail
#           Fn_fail1                  a1                  b1
#            Fs_fail                  a2                  b2
#            Mt_fail                  a3                  b3
#            Mb_fail                  a4                  b4
```

### Parameters - Card 1

| Parameter | Type | Description |
|-----------|------|-------------|
| cluster_ID | INT | Unique cluster identifier |
| cluster_title | STRING | Cluster description |

### Parameters - Card 2

| Parameter | Type | Description |
|-----------|------|-------------|
| group_ID | INT | Element group identifier |
| skew_ID | INT | Local coordinate system |
| Ifail | INT | Failure model option |

### Parameters - Cards 3-6 (Failure Criteria)

| Parameter | Type | Description |
|-----------|------|-------------|
| Fn_fail1 | FLOAT | Normal force failure (tension) |
| a1, b1 | FLOAT | Multi-directional scale/exponent |
| Fs_fail | FLOAT | Shear force failure |
| a2, b2 | FLOAT | Multi-directional scale/exponent |
| Mt_fail | FLOAT | Torsion moment failure |
| a3, b3 | FLOAT | Multi-directional scale/exponent |
| Mb_fail | FLOAT | Bending moment failure |
| a4, b4 | FLOAT | Multi-directional scale/exponent |

### Ifail Values

| Value | Description |
|-------|-------------|
| 0 | No global failure criterion (default) |
| 1 | Unidirectional - fail if ANY direction exceeds limit |
| 2 | Unidirectional - fail if ALL directions exceed limit |
| 3 | Multi-directional failure criterion |

### Multi-directional Failure (Ifail = 3)

```
(Fn/Fn_fail)^b1 * a1 + (Fs/Fs_fail)^b2 * a2 + (Mt/Mt_fail)^b3 * a3 + (Mb/Mb_fail)^b4 * a4 ≥ 1
```

### Example - Brick Cluster
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/CLUSTER/BRICK/1
Spot_Weld_Cluster
# group_ID   skew_ID     Ifail
         1         0         3
#           Fn_fail1                  a1                  b1
             10000.0                1.0                2.0
#            Fs_fail                  a2                  b2
              8000.0                1.0                2.0
#            Mt_fail                  a3                  b3
              500.0                 1.0                2.0
#            Mb_fail                  a4                  b4
              800.0                 1.0                2.0
```

### Example - Spring Cluster
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/CLUSTER/SPRING/2
Adhesive_Bond_Cluster
# group_ID   skew_ID     Ifail
         2         1         1
#           Fn_fail1                  a1                  b1
              5000.0                1.0                1.0
#            Fs_fail                  a2                  b2
              4000.0                1.0                1.0
#            Mt_fail                  a3                  b3
         1.0E+30                    1.0                1.0
#            Mb_fail                  a4                  b4
         1.0E+30                    1.0                1.0
```

---

## 4. ACTIV - Element Activation/Deactivation

The /ACTIV keyword controls element activation and deactivation during simulation.

### Syntax
```
/ACTIV/activ_ID
activ_title
#  sens_ID grbric_ID grquad_ID grshel_ID grtrus_ID grbeam_ID  grspr_ID grsh3n_ID               Iform
#             Tstart               Tstop
```

### Parameters - Card 1

| Parameter | Type | Description |
|-----------|------|-------------|
| activ_ID | INT | Unique activation block identifier |
| activ_title | STRING | Activation description |

### Parameters - Card 2

| Parameter | Type | Description |
|-----------|------|-------------|
| sens_ID | INT | Triggering sensor |
| grbric_ID | INT | Brick element group |
| grquad_ID | INT | Quad element group |
| grshel_ID | INT | Shell element group |
| grtrus_ID | INT | Truss element group |
| grbeam_ID | INT | Beam element group |
| grspr_ID | INT | Spring element group |
| grsh3n_ID | INT | Triangle element group |
| Iform | INT | Formulation flag |

### Parameters - Card 3 (Iform = 2)

| Parameter | Type | Description |
|-----------|------|-------------|
| Tstart | FLOAT | Activation time |
| Tstop | FLOAT | Deactivation time |

### Iform Values

| Value | Description |
|-------|-------------|
| 1 | Deactivate when sensor triggers (default) |
| 2 | Activate at Tstart, deactivate at Tstop |

### Example - Sensor-Triggered Deactivation
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/ACTIV/1
Airbag_Vent_Activation
#  sens_ID grbric_ID grquad_ID grshel_ID grtrus_ID grbeam_ID  grspr_ID grsh3n_ID               Iform
        10         0         0         1         0         0         0         0                   1
```

### Example - Time-Based Activation
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/ACTIV/2
Delayed_Contact_Surface
#  sens_ID grbric_ID grquad_ID grshel_ID grtrus_ID grbeam_ID  grspr_ID grsh3n_ID               Iform
         0         0         0         2         0         0         0         0                   2
#             Tstart               Tstop
              0.005             1.0E+30
```

---

## Best Practices

### Flexible Body Usage

1. **Mode selection**: Include modes up to highest frequency of interest
2. **Interface nodes**: Define connection points carefully
3. **Damping**: Add modal damping for realistic response
4. **Validation**: Compare with full FE model for key responses

### Cluster Definition

1. **Group selection**: Include all related elements
2. **Failure values**: Use test data for calibration
3. **Skew system**: Align with load direction
4. **Multi-directional**: Use for combined loading

### Element Activation

1. **Timing**: Ensure sensor triggers at correct time
2. **Gradual activation**: Consider ramp-up if needed
3. **Contact surfaces**: Update contact when elements change

---

## Example - Complete Assembly

```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
# Flexible body for steering column
/FXBODY/1
Steering_Column
      1000         1         1        30
steering_col_modes.h3d

# Spot weld cluster with failure
/CLUSTER/BRICK/1
Hood_Hinge_Spotwelds
         1         0         3
             12000.0                1.0                2.0
             10000.0                1.0                2.0
               600.0                1.0                2.0
              1000.0                1.0                2.0

# Delayed contact activation
/ACTIV/1
Secondary_Contact
         0         0         0         5         0         0         0         0                   2
              0.010             1.0E+30
```

---

## References

- FXBODY source: `hm_cfg_files/config/CFG/radioss*/BODY/fxbody.cfg`
- CLUSTER source: `hm_cfg_files/config/CFG/radioss*/ELEMENTCLUSTER/cluster.cfg`
- ACTIV source: `hm_cfg_files/config/CFG/radioss*/LOADCOL/activ.cfg`

