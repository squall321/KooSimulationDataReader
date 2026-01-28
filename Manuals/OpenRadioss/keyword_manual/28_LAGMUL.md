# OpenRadioss Keyword Manual - Lagrange Multipliers

## Overview

This document describes Lagrange multiplier keywords in OpenRadioss used for constraint enforcement in interfaces, rigid walls, and other contact/constraint applications.

---

## 1. LAGMUL - Lagrange Multiplier Control

The /LAGMUL keyword controls the Lagrange multiplier algorithm parameters for constraint enforcement.

### Syntax
```
/LAGMUL
#   Lagmod    Lagopt                 Tol               Alpha             Alpha_s
```

### Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| Lagmod | INT | 1 | Pre-conditioning algorithm |
| Lagopt | INT | 0 | Matrix scaling option |
| Tol | FLOAT | 1.0E-11 | Convergence tolerance |
| Alpha | FLOAT | 0.001 | Iterative shift parameter |
| Alpha_s | FLOAT | 0.0 | Initial shift value |

### Lagmod Values

| Value | Description |
|-------|-------------|
| 1 | Cholesky pre-conditioning (default) |
| 2 | Polynomial first degree pre-conditioning |

### Lagopt Values

| Value | Description |
|-------|-------------|
| 0 | No scaling (default) |
| 1 | Diagonal scaling |
| 2 | L2 norm matrix scaling |

### Example
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/LAGMUL
#   Lagmod    Lagopt                 Tol               Alpha             Alpha_s
         1         0           1.0E-11               0.001               0.0
```

---

## 2. Lagrange Multiplier Interfaces

Lagrange multiplier formulation is used in specific interface types for exact constraint enforcement.

### Available Lagrange Multiplier Interfaces

| Interface | Keyword | Description |
|-----------|---------|-------------|
| TYPE2 | /INTER/LAGMUL/TYPE2 | Tied contact with Lagrange multipliers |
| TYPE7 | /INTER/LAGMUL/TYPE7 | General contact with Lagrange |
| TYPE16 | /INTER/LAGMUL/TYPE16 | Node-to-segment Lagrange |
| TYPE17 | /INTER/LAGMUL/TYPE17 | Edge-to-edge Lagrange |

### Comparison: Penalty vs Lagrange Multiplier

| Aspect | Penalty Method | Lagrange Multiplier |
|--------|---------------|---------------------|
| Penetration | Small, controlled | Zero (exact) |
| Stability | Generally stable | May require tuning |
| CPU cost | Lower per cycle | Higher per cycle |
| Contact force | Proportional to penetration | Computed directly |
| Time step | May reduce | Less impact |

---

## 3. RWALL/LAGMUL - Lagrange Multiplier Rigid Wall

The /RWALL/LAGMUL keyword defines a rigid wall with Lagrange multiplier contact formulation.

### Available Types

| Type | Keyword | Description |
|------|---------|-------------|
| Plane | /RWALL/LAGMUL/PLANE | Infinite planar wall |

### Syntax
```
/RWALL/LAGMUL/PLANE/rwall_ID
rwall_title
#  node_ID     Slide  grnd_ID1  grnd_ID2
#                  d                fric            Diameter                ffac       ifq
#                 XM                  YM                  ZM
#                XM1                 YM1                 ZM1
```

### Parameters

Same as /RWALL/PLANE but uses Lagrange multiplier formulation for contact.

### Example
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/RWALL/LAGMUL/PLANE/1
Ground_LagMul_Wall
#  node_ID     Slide  grnd_ID1  grnd_ID2
         0         0         1         0
#                  d                fric            Diameter                ffac       ifq
              10.000                                                                     0
#                 XM                  YM                  ZM
               0.000               0.000               0.000
#                XM1                 YM1                 ZM1
               0.000               0.000               1.000
```

---

## 4. Algorithm Details

### Conjugate Gradient Method

The Lagrange multiplier equations are solved using iterative methods:

```
[K  G^T] [u]   [f]
[G   0 ] [λ] = [g]
```

Where:
- K = Stiffness matrix
- G = Constraint matrix
- u = Displacements
- λ = Lagrange multipliers (contact forces)
- f = External forces
- g = Gap constraints

### Pre-conditioning

Pre-conditioning improves convergence:

| Method | Advantage | Use Case |
|--------|-----------|----------|
| Cholesky | More robust | General use |
| Polynomial | Faster per iteration | Large problems |

### Convergence Control

| Parameter | Effect |
|-----------|--------|
| Tol | Tighter = more accurate, more iterations |
| Alpha | Larger = faster convergence, less stable |
| Alpha_s | Non-zero helps initial convergence |

---

## 5. When to Use Lagrange Multipliers

### Recommended Applications

1. **Exact tied contact**: No separation allowed
2. **Incompressibility**: Volume constraint enforcement
3. **Rigid coupling**: Perfect kinematic connection
4. **High accuracy needs**: Zero penetration required

### Not Recommended

1. **Large sliding contact**: Use penalty method
2. **High-speed impact**: Penalty more stable
3. **Many contact pairs**: CPU cost concern
4. **Explicit dynamics**: Generally use penalty

---

## Best Practices

### Convergence Issues

| Issue | Solution |
|-------|----------|
| Non-convergence | Reduce Tol, increase Alpha |
| Slow convergence | Try Lagmod=2 |
| Oscillations | Reduce Alpha |
| Initial instability | Increase Alpha_s |

### Parameter Guidelines

| Scenario | Tol | Alpha |
|----------|-----|-------|
| General use | 1.0E-11 | 0.001 |
| Stiff problems | 1.0E-12 | 0.0001 |
| Fast convergence | 1.0E-10 | 0.01 |

### Scaling Options

| Lagopt | When to Use |
|--------|-------------|
| 0 | Default, well-conditioned problems |
| 1 | Ill-conditioned, varying stiffness |
| 2 | Large stiffness variations |

---

## Example - Complete Lagrange Multiplier Setup

```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
# Lagrange multiplier control
/LAGMUL
         1         1           1.0E-12               0.001               0.0

# Tied interface with Lagrange multipliers
/INTER/LAGMUL/TYPE2/1
Tied_Connection
#  Grnd_IDs  Grnd_IDm                                               Spotflag     Isym
         1         2                                                       0         0
#             Gapmin
              0.100

# Rigid wall with Lagrange multiplier contact
/RWALL/LAGMUL/PLANE/1
Fixed_Ground
         0         0         3         0
               5.000
               0.000               0.000               0.000
               0.000               0.000               1.000
```

---

## Troubleshooting

### Common Issues

| Issue | Cause | Solution |
|-------|-------|----------|
| Solver diverges | Poor conditioning | Use Lagopt=1 or 2 |
| Very slow | Too tight tolerance | Relax Tol |
| Residual oscillates | Alpha too large | Reduce Alpha |
| Initial failure | Zero Alpha_s | Set Alpha_s > 0 |

### Diagnostic Output

Check listing file for:
- Number of iterations per cycle
- Residual norm convergence
- Lagrange multiplier values

---

## References

- LAGMUL source: `hm_cfg_files/config/CFG/radioss*/GROUP/lagmul.cfg`
- RWALL/LAGMUL source: `hm_cfg_files/config/CFG/radioss*/RWALL/lagmul.cfg`
- Solver source: `engine/source/mpi/interfaces/`

