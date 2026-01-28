# OpenRadioss Keyword Manual - Miscellaneous Keywords

## Overview

This document describes miscellaneous keywords in OpenRadioss including file inclusion, cross-sections, damping, and other specialized features.

---

## 1. INCLUDE - File Inclusion

The /INCLUDE keyword includes external files into the main input deck.

### Syntax
```
#include filename
```

or

```
/INCLUDE
filename
```

### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| filename | STRING | Path to include file (relative or absolute) |

### Example
```
#include materials.inc
#include ../common/nodes.rad
#include /data/models/parts.inc
```

### Use Cases

1. **Modular model organization** - Separate nodes, elements, materials
2. **Shared components** - Common materials, properties libraries
3. **Model variants** - Same base with different includes

---

## 2. INCLUDE_DYNA - LS-DYNA Include

The /INCLUDE_DYNA keyword includes LS-DYNA format files with optional transformation.

### Syntax
```
/INCLUDE_DYNA
filename
#  Id_offset
```

### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| filename | STRING | LS-DYNA input file path |
| Id_offset | INT | Offset added to all entity IDs |

### Example
```
/INCLUDE_DYNA
barrier_model.k
   1000000
```

---

## 3. SECT - Cross Section Output

The /SECT keyword defines cross-sections for force and moment output.

### Syntax
```
/SECT/sect_ID
sect_title
# node_ID1  node_ID2  node_ID3  grnod_ID     ISAVE  Frame_ID              deltaT               alpha
#file_name
#grbric_ID           grshel_ID grtrus_ID grbeam_ID grsprg_ID grtria_ID     Niter              Iframe
```

### Parameters - Card 1

| Parameter | Type | Description |
|-----------|------|-------------|
| sect_ID | INT | Section identifier |
| sect_title | STRING | Section description |

### Parameters - Card 2

| Parameter | Type | Description |
|-----------|------|-------------|
| node_ID1 | INT | Origin node N1 |
| node_ID2 | INT | Axis node N2 (defines X direction) |
| node_ID3 | INT | Plane node N3 (defines XY plane) |
| grnod_ID | INT | Node group for section cut |
| ISAVE | INT | Data save flag |
| Frame_ID | INT | Moving frame identifier |
| deltaT | FLOAT | Time step for data saving |
| alpha | FLOAT | Filtering coefficient (< 1) |

### Parameters - Card 3

| Parameter | Type | Description |
|-----------|------|-------------|
| file_name | STRING | Output file root name |

### Parameters - Card 4

| Parameter | Type | Description |
|-----------|------|-------------|
| grbric_ID | INT | Brick element group |
| grshel_ID | INT | Shell element group |
| grtrus_ID | INT | Truss element group |
| grbeam_ID | INT | Beam element group |
| grsprg_ID | INT | Spring element group |
| grtria_ID | INT | Triangle element group |
| Niter | INT | Number of interfaces |
| Iframe | INT | Local skew computation flag |

### ISAVE Values

| Value | Description |
|-------|-------------|
| 0 | Do not save |
| 1 | Save displacements |
| 2 | Save displacements and forces |
| 100 | Read data from file |
| 200 | Read + recompute forces |

### Iframe Values

| Value | Description |
|-------|-------------|
| 0 | Origin at local skew center (C0) |
| 1 | Origin at geometric center (C1) |
| 2 | Origin at center of gravity (C2) |
| 3 | Origin at point O(0,0,0) |
| 10-13 | Same as 0-3 in global skew |

### Example
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/SECT/1
Front_Bumper_Section
#node_ID1  node_ID2  node_ID3  grnod_ID     ISAVE  Frame_ID              deltaT               alpha
       100       101       102         1         2         0            0.0001               0.5
BUMPER_SECT
#grbric_ID           grshel_ID grtrus_ID grbeam_ID grsprg_ID grtria_ID     Niter              Iframe
         0                   1         0         0         0         0         0                   1
```

---

## 4. SECT/PARAL - Parallelogram Section

The /SECT/PARAL keyword defines a parallelogram-shaped section cut.

### Syntax
```
/SECT/PARAL/sect_ID
sect_title
# Additional parameters for parallelogram definition
```

---

## 5. SECT/CIRCLE - Circular Section

The /SECT/CIRCLE keyword defines a circular section cut.

### Syntax
```
/SECT/CIRCLE/sect_ID
sect_title
# Parameters for circular section definition
```

---

## 6. DAMP - Damping

### 6.1 Global Rayleigh Damping

#### Syntax
```
/DAMP
#             alpha                beta
```

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| alpha | FLOAT | Mass proportional damping coefficient |
| beta | FLOAT | Stiffness proportional damping coefficient |

#### Damping Formula

```
C = alpha × M + beta × K
```

Where:
- C = Damping matrix
- M = Mass matrix
- K = Stiffness matrix

#### Example
```
/DAMP
               0.1                0.0001
```

### 6.2 DAMP/INTER - Interface Damping

Defines damping for specific interfaces.

---

## 7. ACTIV - Load Collector Activation

The /ACTIV keyword activates/deactivates load collectors during simulation.

### Syntax
```
/ACTIV/activ_ID
activ_title
#   sens_ID   Iflag
#loadcol_ID1 loadcol_ID2 ... loadcol_IDn
```

### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| activ_ID | INT | Activation identifier |
| sens_ID | INT | Sensor for triggering |
| Iflag | INT | Activation flag (0=deactivate, 1=activate) |
| loadcol_ID | INT | Load collector IDs to control |

### Example
```
/ACTIV/1
Airbag_Deployment
#   sens_ID   Iflag
         1         1
#loadcol_ID
        10        11        12
```

---

## 8. SUBMODEL - Submodel Definition

The /SUBMODEL keyword defines a submodel for hierarchical model organization.

### Syntax
```
/SUBMODEL/submodel_ID
submodel_title
```

### Use Cases

1. Component organization in large models
2. Model assembly from parts
3. Selective output control

---

## 9. ASSEMBLY - Assembly Definition

The /ASSEMBLY keyword groups parts and submodels into assemblies.

### Syntax
```
/ASSEMBLY/assembly_ID
assembly_title
# part_ID1 part_ID2 ... part_IDn
```

---

## 10. EXTERN - External Boundary

The /EXTERN keyword defines external boundaries for domain decomposition.

### Syntax
```
/EXTERN/extern_ID
extern_title
# External boundary parameters
```

---

## 11. RETRACTOR - Seatbelt Retractor

The /RETRACTOR keyword defines a seatbelt retractor device.

### Syntax
```
/RETRACTOR/retractor_ID
retractor_title
# Retractor parameters: spring, spool, belt properties
```

### Parameters

| Parameter | Description |
|-----------|-------------|
| Spring stiffness | Retractor spring force vs. displacement |
| Locking criteria | Sensor-based or acceleration-based |
| Payout curve | Belt payout force characteristics |

---

## 12. SLIPRING - Belt Slip Ring

The /SLIPRING keyword defines a slip ring for seatbelt routing.

### Syntax
```
/SLIPRING/slipring_ID
slipring_title
# Slip ring parameters
```

### Use Cases

1. Seatbelt D-ring modeling
2. Belt path redirection
3. Friction at belt guides

---

## 13. CLUSTER - Element Clustering

The /CLUSTER keyword groups elements for parallel computation.

### Syntax
```
/CLUSTER/cluster_ID
cluster_title
# Element or part groups
```

### Use Cases

1. Domain decomposition optimization
2. Load balancing in parallel runs
3. Memory management

---

## 14. XREF - Cross Reference

The /XREF keyword creates cross-references between entities.

### Syntax
```
/XREF/xref_ID
# Entity cross-reference definitions
```

---

## Best Practices

### File Organization

```
# Main input file structure
/BEGIN
...
#include geometry/nodes.inc
#include geometry/elements.inc
#include materials/steel.inc
#include materials/plastic.inc
#include contacts/interfaces.inc
#include loads/boundary.inc
/END
```

### Section Cut Placement

1. **B-pillar section** - Vehicle structural load path
2. **Rocker section** - Side impact force transfer
3. **Rail sections** - Front/rear crash load paths
4. **Steering column** - Occupant interaction forces

### Damping Guidelines

| Application | Alpha | Beta |
|-------------|-------|------|
| Light damping | 0.01 | 0.0001 |
| Moderate | 0.1 | 0.001 |
| Heavy damping | 1.0 | 0.01 |
| Quasi-static | 10.0 | 0.1 |

---

## Example - Complete Model with Sections

```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/BEGIN
Vehicle_Crash_Model
       110
                  kg                  mm                  ms
                  kg                  mm                  ms

# Include files
#include nodes.inc
#include shells.inc
#include solids.inc
#include materials.inc
#include properties.inc
#include contacts.inc

# Section cuts for force output
/SECT/1
B_Pillar_Lower
       100       101       102         1         2         0            0.0001               0.5
BPIL_LOW
         0                   1         0         0         0         0         0                   1

/SECT/2
B_Pillar_Upper
       200       201       202         2         2         0            0.0001               0.5
BPIL_UPP
         0                   2         0         0         0         0         0                   1

/SECT/3
Front_Rail_Left
       300       301       302         3         2         0            0.0001               0.5
FRAIL_L
         3                   0         0         0         0         0         0                   1

# Global damping
/DAMP
               0.05               0.0001

/END
```

---

## References

- INCLUDE source: `hm_cfg_files/config/CFG/radioss*/INCLUDE/`
- SECT source: `hm_cfg_files/config/CFG/radioss*/SECT/`
- Starter source: `starter/source/model/`
- Engine source: `engine/source/`

