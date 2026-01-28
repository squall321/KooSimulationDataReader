# OpenRadioss Keyword Manual - Functions and Tables

## Overview

This document describes function and table definitions in OpenRadioss. Functions define time-varying or parameter-dependent curves, while tables define multi-dimensional data.

---

## 1. FUNCT - Basic Function Definition

Defines a 1D function as (X, Y) data pairs.

### Syntax
```
/FUNCT/funct_ID
funct_title
#                  X                   Y
                 x1                  y1
                 x2                  y2
                ...                 ...
```

### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| funct_ID | INT | Unique function identifier |
| funct_title | STRING | Function description (100 chars) |
| X | FLOAT | Independent variable (abscissa) |
| Y | FLOAT | Dependent variable (ordinate) |

### Notes
- Points should be in ascending X order
- Linear interpolation between points
- Extrapolation uses end slope or constant

### Example - Time-Force Curve
```
/FUNCT/1
Impact_Force_vs_Time
#                  X                   Y
             0.0000             0.0000
             0.0010          1000.0000
             0.0050          5000.0000
             0.0100          5000.0000
             0.0150          2000.0000
             0.0200             0.0000
```

### Example - Stress-Strain Curve
```
/FUNCT/10
Steel_Stress_Strain
#                  X                   Y
             0.0000             0.0000
             0.0010           210.0000
             0.0020           350.0000
             0.0050           400.0000
             0.1000           450.0000
             0.2000           480.0000
```

---

## 2. FUNCT_SMOOTH - Function with Scale and Shift

Defines a function with scale and shift transformations.

### Syntax
```
/FUNCT_SMOOTH/funct_ID
funct_title
#         Ascalex          Fscaley          Ashiftx          Fshifty

#                  X                   Y
                 x1                  y1
                 x2                  y2
                ...                 ...
```

### Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| Ascalex | FLOAT | 1.0 | X-axis scale factor |
| Fscaley | FLOAT | 1.0 | Y-axis scale factor |
| Ashiftx | FLOAT | 0.0 | X-axis shift value |
| Fshifty | FLOAT | 0.0 | Y-axis shift value |

### Transformation
```
X_transformed = X * Ascalex + Ashiftx
Y_transformed = Y * Fscaley + Fshifty
```

### Example
```
/FUNCT_SMOOTH/20
Scaled_Force_Curve
#         Ascalex          Fscaley          Ashiftx          Fshifty
              1.0             1000.0              0.0              0.0
#                  X                   Y
             0.0000             0.0000
             0.0010             1.0000
             0.0050             5.0000
             0.0100             5.0000
```

---

## 3. MOVE_FUNCT - Function Transformation

Applies scale and shift to an existing function.

### Syntax
```
/MOVE_FUNCT/funct_ID
funct_title
#         Ascalex          Fscaley          Ashiftx          Fshifty
```

### Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| Ascalex | FLOAT | 1.0 | X-axis scale factor |
| Fscaley | FLOAT | 1.0 | Y-axis scale factor |
| Ashiftx | FLOAT | 0.0 | X-axis shift |
| Fshifty | FLOAT | 0.0 | Y-axis shift |

### Example
```
/MOVE_FUNCT/1
Shifted_Function
#         Ascalex          Fscaley          Ashiftx          Fshifty
              1.0              1.5            0.001              0.0
```

---

## 4. FUNC_2D - 2D Function Definition

Defines a 2D function with (X, Y) coordinates and Z values.

### Syntax
```
/FUNC_2D/func_ID
func_title
#     dim
#                  X                   Y                  Z1                  Z2                  Z3
```

### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| dim | INT | Dimension of values (1, 2, or 3) |
| X | FLOAT | First coordinate |
| Y | FLOAT | Second coordinate |
| Z1 | FLOAT | Value along first axis |
| Z2 | FLOAT | Value along second axis (dim ≥ 2) |
| Z3 | FLOAT | Value along third axis (dim = 3) |

### Example - 2D Stress Function (dim=2)
```
/FUNC_2D/30
2D_Stress_Field
#     dim
         2
#                  X                   Y                  Z1                  Z2
             0.0000             0.0000             0.0000             0.0000
             0.1000             0.1000            10.0000             5.0000
             0.2000             0.2000            20.0000            10.0000
             0.3000             0.3000            25.0000            12.0000
```

---

## 5. TABLE/0 - Multi-Dimensional Table

Defines a table with up to 4 dimensions.

### Syntax
```
/TABLE/0/table_ID
table_title
#dimension        n1        n2        n3        n4
#                 X1                  X2                  X3  ... (n1 values)
#                 Y1                  Y2                  Y3  ... (n2 values)
#                 Z1                  Z2                  Z3  ... (n3 values, if dim>2)
#                 W1                  W2                  W3  ... (n4 values, if dim>3)
#                 T1                  T2                  T3  ... (n1*n2*n3*n4 values)
```

### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| dimension | INT | Number of dimensions (1-4) |
| n1, n2, n3, n4 | INT | Size of each dimension |

### Example - 2D Table (Strain Rate vs Temperature)
```
/TABLE/0/50
Yield_vs_StrainRate_Temp
#dimension        n1        n2        n3        n4
         2         3         4         0         0
#                 X1                  X2                  X3
           0.001000           1.000000        1000.000000
#                 Y1                  Y2                  Y3                  Y4
          20.000000         100.000000         300.000000         500.000000
#                 T1                  T2                  T3                  T4
         250.000000         245.000000         220.000000         180.000000
         280.000000         275.000000         250.000000         210.000000
         320.000000         315.000000         290.000000         250.000000
```

---

## 6. TABLE/1 - Function-Interpolated Table

Defines a table that references multiple functions with interpolation.

### Syntax
```
/TABLE/1/table_ID
table_title
#dimension
# fct_ID1                             A                   B                   C            Scale_y1
```

### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| dimension | INT | Interpolation order (1-4) |
| fct_ID | INT | Reference to /FUNCT |
| A, B, C | FLOAT | Interpolation coefficients |
| Scale_y | FLOAT | Y-axis scale factor |

### Interpolation Order

| Order | Parameters Used |
|-------|-----------------|
| 1 | X, Y values from functions |
| 2 | A, Scale_y |
| 3 | A, B, Scale_y |
| 4 | A, B, C, Scale_y |

### Example - Table of Functions (Order=2)
```
/TABLE/1/60
StrainRate_Dependent_Curves
#dimension
         2
# fct_ID1                             A                                                    Scale_y1
        10                             0.001000                                               1.000000
        11                             0.100000                                               1.100000
        12                             1.000000                                               1.200000
        13                           100.000000                                               1.400000
```

Where functions 10-13 are stress-strain curves at different strain rates.

---

## Function Application Summary

| Use Case | Keyword | Description |
|----------|---------|-------------|
| Time-varying load | /FUNCT | Force vs time |
| Material curve | /FUNCT | Stress vs strain |
| Rate dependency | /TABLE/1 | Multiple curves vs strain rate |
| Temperature dependency | /TABLE/0 | Property vs temperature |
| 2D mapping | /FUNC_2D | Spatial variation |
| Scaling | /MOVE_FUNCT | Transform existing function |

---

## Common Function Types

### Load Curves
```
/FUNCT/1
Ramp_Load
             0.0000             0.0000
             0.0010             1.0000
             0.0100             1.0000
```

### Step Function
```
/FUNCT/2
Step_Function
             0.0000             0.0000
             0.0010             0.0000
             0.0011             1.0000
             0.1000             1.0000
```

### Sinusoidal (Approximation)
```
/FUNCT/3
Sine_Wave
             0.0000             0.0000
             0.0025             0.7071
             0.0050             1.0000
             0.0075             0.7071
             0.0100             0.0000
            -0.0025            -0.7071
```

### Decay Function
```
/FUNCT/4
Exponential_Decay
             0.0000             1.0000
             0.0020             0.6065
             0.0040             0.3679
             0.0060             0.2231
             0.0080             0.1353
             0.0100             0.0821
```

---

## Material Curve Examples

### Steel Stress-Strain (LAW36)
```
/FUNCT/100
Steel_True_Stress_Strain
#                  X                   Y
             0.0000             0.0000
             0.0012           250.0000
             0.0100           280.0000
             0.0500           350.0000
             0.1000           400.0000
             0.2000           450.0000
             0.3000           480.0000
```

### Rubber Stress-Strain
```
/FUNCT/101
Rubber_Engineering_Stress_Strain
#                  X                   Y
             0.0000             0.0000
             0.1000             0.1000
             0.5000             0.8000
             1.0000             2.5000
             2.0000            10.0000
             3.0000            30.0000
```

---

## Best Practices

1. **Data Points**:
   - Use sufficient points for smooth curves
   - Minimum 2 points required
   - More points in regions of high curvature

2. **X-Values**:
   - Must be monotonically increasing
   - No duplicate X values
   - Cover expected range of independent variable

3. **Extrapolation**:
   - Values outside range use end behavior
   - Consider extending curves beyond expected range
   - Check for unexpected extrapolation in results

4. **Units**:
   - Ensure consistent units with model
   - Use scale factors if necessary
   - Document units in function title

5. **Rate-Dependent Materials**:
   - Use TABLE/1 for strain rate effects
   - Include sufficient strain rate curves
   - Cover expected strain rate range

---

## Example: Complete Function Setup

```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
# Load Function
/FUNCT/1
Impact_Force_vs_Time
#                  X                   Y
             0.0000             0.0000
             0.0010          5000.0000
             0.0050         10000.0000
             0.0100         10000.0000
             0.0150          5000.0000
             0.0200             0.0000

# Gravity Ramp
/FUNCT/2
Gravity_Ramp
#                  X                   Y
             0.0000             0.0000
             0.0010             1.0000
             1.0000             1.0000

# Material Curves at Different Strain Rates
/FUNCT/10
Steel_Rate_0.001
#                  X                   Y
             0.0000             0.0000
             0.0012           250.0000
             0.2000           400.0000

/FUNCT/11
Steel_Rate_1.0
#                  X                   Y
             0.0000             0.0000
             0.0012           280.0000
             0.2000           450.0000

/FUNCT/12
Steel_Rate_1000
#                  X                   Y
             0.0000             0.0000
             0.0012           320.0000
             0.2000           520.0000

# Table for Rate Dependency
/TABLE/1/100
Steel_Rate_Dependent
#dimension
         2
# fct_ID1                             A                                                    Scale_y1
        10                             0.001000                                               1.000000
        11                             1.000000                                               1.000000
        12                          1000.000000                                               1.000000
```

---

## References

- Source files: `hm_cfg_files/config/CFG/radioss2025/CURVE/`
- Source files: `hm_cfg_files/config/CFG/radioss2025/TABLE/`
