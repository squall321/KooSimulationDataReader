# OpenRadioss Keyword Manual - Parameters and Expressions

## Overview

This document describes parameter and expression definitions in OpenRadioss. Parameters allow defining variables that can be referenced throughout the model, enabling parametric modeling and design optimization.

---

## Parameter Keyword Syntax

```
/PARAMETER/scope/type/param_ID
param_title
param_name    param_value
```

---

## 1. PARAMETER/GLOBAL/REAL - Global Real Parameter

Defines a global floating-point parameter.

### Syntax
```
/PARAMETER/GLOBAL/REAL/param_ID
param_title
#param_name            param_value
```

### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| param_ID | INT | Unique parameter identifier |
| param_title | STRING | Parameter description (100 chars max) |
| param_name | STRING | Parameter name (10 chars max) |
| param_value | FLOAT | Parameter value |

### Example
```
/PARAMETER/GLOBAL/REAL/1
Material_Thickness
Thick                 2.5
```

---

## 2. PARAMETER/GLOBAL/INTEGER - Global Integer Parameter

Defines a global integer parameter.

### Syntax
```
/PARAMETER/GLOBAL/INTEGER/param_ID
param_title
#param_name            param_value
```

### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| param_name | STRING | Parameter name (10 chars max) |
| param_value | INT | Integer parameter value |

### Example
```
/PARAMETER/GLOBAL/INTEGER/2
Number_of_Layers
NLayers                    8
```

---

## 3. PARAMETER/GLOBAL/TEXT - Global Text Parameter

Defines a global text/string parameter.

### Syntax
```
/PARAMETER/GLOBAL/TEXT/param_ID
param_title
#param_name    param_value
```

### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| param_name | STRING | Parameter name |
| param_value | STRING | Text parameter value |

### Example
```
/PARAMETER/GLOBAL/TEXT/3
Material_Name
MatName    Steel_AISI_1045
```

---

## 4. PARAMETER/LOCAL/REAL - Local Real Parameter

Defines a local floating-point parameter (file-scope only).

### Syntax
```
/PARAMETER/LOCAL/REAL/param_ID
param_title
#param_name            param_value
```

### Usage
Local parameters are only accessible within the current input file. They are useful for include files that should not affect global namespace.

### Example
```
/PARAMETER/LOCAL/REAL/1
Local_Thickness
LThick                1.5
```

---

## 5. PARAMETER/GLOBAL/REAL_EXPR - Real Expression Parameter

Defines a global parameter using a mathematical expression.

### Syntax
```
/PARAMETER/GLOBAL/REAL_EXPR/param_ID
param_title
#param_name    expression
```

### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| param_name | STRING | Parameter name |
| expression | STRING | Mathematical expression |

### Supported Operators

| Operator | Description |
|----------|-------------|
| + | Addition |
| - | Subtraction |
| * | Multiplication |
| / | Division |
| ^ | Power |
| ( ) | Parentheses |

### Supported Functions

| Function | Description |
|----------|-------------|
| sin(x) | Sine |
| cos(x) | Cosine |
| tan(x) | Tangent |
| asin(x) | Arc sine |
| acos(x) | Arc cosine |
| atan(x) | Arc tangent |
| sqrt(x) | Square root |
| exp(x) | Exponential |
| log(x) | Natural logarithm |
| log10(x) | Base-10 logarithm |
| abs(x) | Absolute value |
| min(a,b) | Minimum |
| max(a,b) | Maximum |

### Example
```
/PARAMETER/GLOBAL/REAL/1
Base_Thickness
t0                    2.0

/PARAMETER/GLOBAL/REAL_EXPR/2
Computed_Area
Area      3.14159*(t0/2)^2
```

---

## 6. PARAMETER/GLOBAL/INT_EXPR - Integer Expression Parameter

Defines a global integer parameter using an expression.

### Syntax
```
/PARAMETER/GLOBAL/INT_EXPR/param_ID
param_title
#param_name    expression
```

### Example
```
/PARAMETER/GLOBAL/INTEGER/1
Base_Count
N                     10

/PARAMETER/GLOBAL/INT_EXPR/2
Total_Elements
Total     N*N
```

---

## Parameter Scope Types

| Scope | Description |
|-------|-------------|
| GLOBAL | Accessible throughout entire model |
| LOCAL | Accessible only within current file |

## Parameter Data Types

| Type | Description | Example |
|------|-------------|---------|
| REAL | Floating-point number | 3.14159, 2.5e-3 |
| INTEGER | Whole number | 10, -5, 1000 |
| TEXT | Character string | "Steel", "Part_01" |
| REAL_EXPR | Float expression | "2*pi*r" |
| INT_EXPR | Integer expression | "n*m" |

---

## Using Parameters in Input

### Referencing Parameters

Parameters are referenced using their name preceded by `&`:

```
/MAT/ELAST/1
Material_Name
              &Density
           &Youngs_Mod               &Poisson
```

### Example: Parametric Material Definition

```
# Define parameters
/PARAMETER/GLOBAL/REAL/1
Steel_Density
Density               7850.0

/PARAMETER/GLOBAL/REAL/2
Steel_Young_Modulus
E                     210000.0

/PARAMETER/GLOBAL/REAL/3
Steel_Poisson
nu                    0.30

# Use parameters in material
/MAT/ELAST/1
Steel_Material
              &Density
                   &E                  &nu
```

### Example: Parametric Geometry

```
# Define dimensions
/PARAMETER/GLOBAL/REAL/1
Length
L                     100.0

/PARAMETER/GLOBAL/REAL/2
Width
W                     50.0

/PARAMETER/GLOBAL/REAL_EXPR/3
Computed_Area
Area      L*W

/PARAMETER/GLOBAL/REAL_EXPR/4
Diagonal
Diag      sqrt(L^2+W^2)
```

---

## Parameter Naming Rules

1. **Length**: Maximum 10 characters
2. **Characters**: Alphanumeric and underscore only
3. **Case**: Case-sensitive
4. **Reserved**: Cannot use keyword names
5. **Start**: Must start with letter

### Valid Names
- `Thick`
- `E_mod`
- `nu_12`
- `T_init`

### Invalid Names
- `12_thick` (starts with number)
- `my-param` (contains hyphen)
- `NODE` (reserved keyword)

---

## Parameter Evaluation Order

1. Local parameters are evaluated first
2. Global parameters are evaluated in ID order
3. Expressions are evaluated after all referenced parameters
4. Circular references cause errors

---

## Best Practices

### Organization
1. Group related parameters together
2. Use meaningful, descriptive names
3. Document parameter purpose in title

### Design Optimization
1. Define key dimensions as parameters
2. Use expressions for dependent values
3. Centralize material properties

### Include Files
1. Use LOCAL scope for file-specific parameters
2. Use GLOBAL for shared parameters
3. Avoid name conflicts between files

---

## Example: Complete Parametric Model

```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
# Global parameters for crash model
/PARAMETER/GLOBAL/REAL/1
Material_Density
rho                   7850.0

/PARAMETER/GLOBAL/REAL/2
Young_Modulus
E                     210000.0

/PARAMETER/GLOBAL/REAL/3
Poisson_Ratio
nu                    0.30

/PARAMETER/GLOBAL/REAL/4
Yield_Stress
sig_y                 350.0

/PARAMETER/GLOBAL/REAL/5
Shell_Thickness
thick                 1.5

/PARAMETER/GLOBAL/REAL/6
Impact_Velocity
V_imp                 15.0

/PARAMETER/GLOBAL/REAL_EXPR/7
Kinetic_Energy_Factor
KE_fac    0.5*rho*V_imp^2

# Material using parameters
/MAT/PLAS_JOHNS/1
Steel_Parametric
              &rho
                   &E                  &nu
         0
             &sig_y               500.0               0.35             1.0E20             1.0E20
              0.015               1.0                   1         0            1.0E20
               1.0              1800.0           3.5E+09             300.0            1.0E20
         -1.0E+20

# Property using parameters
/PROP/SHELL/1
Shell_Property
         1         0         0         0         0                    0.0
               0.01              0.01              0.01              0.0              0.0
         5                    &thick              0.833333                   0         0         0

# Initial velocity using parameters
/INIVEL/TRA/1
Impact_Velocity
           &V_imp               0.0               0.0        10         0
               0.0         0
```

---

## References

- Source files: `hm_cfg_files/config/CFG/radioss*/PARAMETER/`
- Starter source: `starter/source/model/parameter/`

