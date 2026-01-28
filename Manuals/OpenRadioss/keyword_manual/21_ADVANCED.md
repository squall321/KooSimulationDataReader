# OpenRadioss Keyword Manual - Advanced Features

## Overview

This document describes advanced modeling keywords in OpenRadioss including added mass (ADMAS), non-local formulations (NONLOCAL), and perturbation/stochastic analysis (PERTURB).

---

## 1. ADMAS - Added Mass

The /ADMAS keyword adds non-structural mass to nodes, surfaces, or parts for inertia simulation without adding stiffness.

### Available Types

| Type | Description | Entity |
|------|-------------|--------|
| 0 | Mass per node | Node group |
| 1 | Total mass for all nodes | Node group |
| 2 | Mass on surface | Surface |
| 3 | Mass on parts (area weighted) | Part group |
| 4 | Mass on parts (volume weighted) | Part group |
| 5 | Non-uniform mass per node | Individual nodes |
| 6 | Non-uniform mass per part (area weighted) | Individual parts |
| 7 | Non-uniform mass per part (volume weighted) | Individual parts |

### Syntax (Types 0, 1)
```
/ADMAS/type/admas_ID
admas_title
#               MASS   grnd_ID
```

### Syntax (Type 2)
```
/ADMAS/2/admas_ID
admas_title
#               MASS   surf_ID
```

### Syntax (Types 3, 4)
```
/ADMAS/type/admas_ID
admas_title
#               MASS grpart_ID     IFLAG
```

### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| admas_ID | INT | Unique added mass identifier |
| admas_title | STRING | Added mass description |
| MASS | FLOAT | Mass value |
| grnd_ID | INT | Node group identifier (types 0, 1) |
| surf_ID | INT | Surface identifier (type 2) |
| grpart_ID | INT | Part group identifier (types 3, 4) |
| IFLAG | INT | Distribution flag (types 3, 4) |

### IFLAG Values (Types 3, 4)

| Value | Description |
|-------|-------------|
| 0 | Area-weighted distribution (type 3 default) |
| 1 | Volume-weighted distribution (type 4 default) |

### Example - Type 0: Mass Per Node
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/ADMAS/0/1
Added_Mass_Per_Node
#               MASS   grnd_ID
                 1.5         1
```

### Example - Type 1: Total Mass for Node Group
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/ADMAS/1/2
Total_Added_Mass
#               MASS   grnd_ID
               100.0         2
```

### Example - Type 3: Mass on Parts (Area Weighted)
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/ADMAS/3/3
Part_Mass_Area_Weighted
#               MASS grpart_ID     IFLAG
               500.0         1         0
```

### Use Cases

1. **Simulating non-structural mass** (paint, insulation, trim)
2. **Ballast or payload representation**
3. **Mass distribution from attached components**
4. **Inertia tuning for dynamic response**

---

## 2. NONLOCAL/MAT - Non-Local Material Formulation

The /NONLOCAL/MAT keyword defines non-local averaging for damage and failure models to achieve mesh-independent results.

### Syntax
```
/NONLOCAL/MAT/mat_ID
#             LENGTH              LE_MAX
```

### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| mat_ID | INT | Material identifier to apply non-local formulation |
| LENGTH | FLOAT | Non-local internal length (characteristic length) |
| LE_MAX | FLOAT | Element length target for mesh convergence |

### Description

Non-local formulations address mesh dependency in damage and failure simulations by averaging strain/damage over a characteristic length. This prevents:
- Mesh-dependent localization
- Unrealistic strain concentration
- Pathological mesh sensitivity

### Formulation

The non-local variable at a point is computed by averaging over neighboring points:
```
f_nonlocal(x) = integral[ w(|x-y|) * f(y) dy ] / integral[ w(|x-y|) dy ]
```

Where:
- w = weighting function (typically Gaussian)
- LENGTH defines the averaging radius

### Example
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/NONLOCAL/MAT/1
#             LENGTH              LE_MAX
                 5.0                 2.0
```

### Guidelines

| Parameter | Guideline |
|-----------|-----------|
| LENGTH | 3-5 times the element size |
| LE_MAX | Target element size for converged results |

### Applicable Material Laws

Non-local formulation is typically used with damage/failure models:
- Damage materials (LAW27, etc.)
- Failure criteria with softening
- Materials with strain localization issues

---

## 3. PERTURB - Perturbation and Stochastic Analysis

The /PERTURB keywords introduce random variations in material properties, geometry, or failure parameters for stochastic simulations.

### Available Perturbation Types

| Keyword | Description |
|---------|-------------|
| /PERTURB | General perturbation on part group |
| /PERTURB/PART/SHELL | Shell property perturbation |
| /PERTURB/PART/SOLID | Solid property perturbation |
| /PERTURB/FAIL/BIQUAD | Failure parameter perturbation |

### 3.1 PERTURB - General Perturbation

#### Syntax
```
/PERTURB/perturb_ID
perturb_title
#             F_Mean           Deviation             Min_cut             Max_cut      Seed   Idistri
#grpart_ID
```

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| perturb_ID | INT | Unique perturbation identifier |
| perturb_title | STRING | Perturbation description |
| F_Mean | FLOAT | Mean value of the distribution |
| Deviation | FLOAT | Standard deviation (for normal distribution) |
| Min_cut | FLOAT | Minimum cutoff value |
| Max_cut | FLOAT | Maximum cutoff value |
| Seed | INT | Random seed (0 = use system time) |
| Idistri | INT | Distribution type |
| grpart_ID | INT | Part group identifier |

#### Idistri Values

| Value | Description |
|-------|-------------|
| 0 | Default, set to 2 |
| 1 | Uniform random distribution |
| 2 | Normal (Gaussian) distribution |

#### Example
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/PERTURB/1
Thickness_Perturbation
#             F_Mean           Deviation             Min_cut             Max_cut      Seed   Idistri
                 1.0               0.05                 0.9                 1.1         0         2
#grpart_ID
         1
```

### 3.2 PERTURB/PART/SHELL - Shell Property Perturbation

Perturbs shell properties for a part group.

#### Syntax
```
/PERTURB/PART/SHELL/perturb_ID
perturb_title
#             F_Mean           Deviation             Min_cut             Max_cut      Seed   Idistri
#grpart_ID           parameter
```

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| grpart_ID | INT | Part group identifier |
| parameter | STRING | Property to perturb (e.g., "THICKNESS") |

#### Example
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/PERTURB/PART/SHELL/1
Shell_Thickness_Variation
#             F_Mean           Deviation             Min_cut             Max_cut      Seed   Idistri
                 1.0               0.03                0.94                1.06      1234         2
#grpart_ID           parameter
         1           THICKNESS
```

### 3.3 PERTURB/FAIL/BIQUAD - Failure Parameter Perturbation

Perturbs failure model parameters.

#### Syntax
```
/PERTURB/FAIL/BIQUAD/perturb_ID
perturb_title
#             F_Mean           Deviation             Min_cut             Max_cut      Seed   Idistri
#  fail_ID           parameter
```

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| fail_ID | INT | Failure model identifier |
| parameter | STRING | Parameter to perturb (e.g., "c3", "eps_max") |

#### Example
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/PERTURB/FAIL/BIQUAD/1
Failure_Strain_Variation
#             F_Mean           Deviation             Min_cut             Max_cut      Seed   Idistri
                 1.0               0.10                 0.8                 1.2      5678         2
#  fail_ID           parameter
         1                  c3
```

---

## Statistical Distributions

### Normal (Gaussian) Distribution

```
f(x) = (1 / (sigma * sqrt(2*pi))) * exp(-(x-mu)^2 / (2*sigma^2))
```

Where:
- mu = F_Mean
- sigma = Deviation

### Uniform Distribution

```
f(x) = 1 / (Max_cut - Min_cut) for Min_cut <= x <= Max_cut
```

---

## Best Practices

### ADMAS
1. Use Type 1 (total mass) for distributing known total mass
2. Use Type 3/4 for realistic mass distribution on parts
3. Consider how added mass affects natural frequencies
4. Verify total mass matches expected values

### NONLOCAL
1. Set LENGTH to 3-5 times element size
2. LE_MAX should match mesh size in critical regions
3. Use with damage models to achieve mesh convergence
4. Validate results with different mesh densities

### PERTURB
1. Use consistent seeds for reproducible results
2. Set appropriate Min_cut/Max_cut to avoid unrealistic values
3. Run multiple simulations for statistical analysis
4. Start with small deviations and increase gradually

---

## Stochastic Analysis Workflow

1. **Define perturbations**
   ```
   /PERTURB/PART/SHELL/1
   ...
   ```

2. **Run multiple simulations** with different seeds

3. **Post-process results** for statistical measures:
   - Mean response
   - Standard deviation
   - Confidence intervals
   - Extreme values

4. **Validate against physical variability**

---

## Example: Complete Stochastic Model Setup

```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
# Added mass for non-structural components
/ADMAS/1/1
Trim_Mass
               50.0         1

# Non-local formulation for damage model
/NONLOCAL/MAT/1
                 3.0                 1.0

# Material thickness variation
/PERTURB/PART/SHELL/1
Thickness_Variation
                 1.0               0.05                0.90                1.10         0         2
         1           THICKNESS

# Yield stress variation
/PERTURB/PART/SHELL/2
Yield_Stress_Variation
                 1.0               0.08                0.85                1.15         0         2
         1           SIGMA_Y

# Failure strain variation
/PERTURB/FAIL/BIQUAD/1
Failure_Variation
                 1.0               0.10                0.80                1.20         0         2
         1                  c3
```

---

## Troubleshooting

### ADMAS Issues

1. **Mass not applied**
   - Verify node/part group contains elements
   - Check type matches entity type

2. **Unexpected dynamics**
   - Added mass changes natural frequencies
   - Check mass magnitude is appropriate

### NONLOCAL Issues

1. **Results still mesh-dependent**
   - Increase LENGTH
   - Verify LE_MAX matches mesh

2. **Excessive smoothing**
   - Reduce LENGTH
   - Check for appropriate material law compatibility

### PERTURB Issues

1. **No variation observed**
   - Check Deviation > 0
   - Verify seed produces different values

2. **Unrealistic values**
   - Tighten Min_cut/Max_cut bounds
   - Reduce Deviation

3. **Reproducibility problems**
   - Use fixed seed values
   - Document random seed for each run

---

## References

- ADMAS source: `hm_cfg_files/config/CFG/radioss*/ADMAS/`
- NONLOCAL source: `hm_cfg_files/config/CFG/radioss*/MAT/mat_NONLOCAL.cfg`
- PERTURB source: `hm_cfg_files/config/CFG/radioss*/PERTURBATION/`
- Starter source: `starter/source/model/`

