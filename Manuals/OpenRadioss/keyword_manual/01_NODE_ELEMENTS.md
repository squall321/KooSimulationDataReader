# OpenRadioss Keyword Manual - Node and Elements

## Overview

This document describes all node and element definitions in OpenRadioss.

---

## 1. NODE

Defines node coordinates in the finite element model.

### Syntax
```
/NODE
#  node_ID                 Xc                  Yc                  Zc
```

### Parameters

| Parameter | Type | Required | Description |
|-----------|------|----------|-------------|
| node_ID | INT | Yes | Unique node identifier |
| Xc | FLOAT | Yes | X coordinate |
| Yc | FLOAT | Yes | Y coordinate |
| Zc | FLOAT | Yes | Z coordinate |

### Format
- Radioss 110+: `%10d%20lg%20lg%20lg`
- Radioss 41: `%8d%16lg%16lg%16lg`

### Example
```
/NODE
#  node_ID                 Xc                  Yc                  Zc
         1                0.0                0.0                0.0
         2               10.0                0.0                0.0
         3               10.0               10.0                0.0
         4                0.0               10.0                0.0
         5                0.0                0.0               10.0
         6               10.0                0.0               10.0
         7               10.0               10.0               10.0
         8                0.0               10.0               10.0
```

---

## 2. SHELL (4-Node Shell Element)

Defines 4-node shell elements for thin-walled structures.

### Syntax
```
/SHELL/part_ID
#shell_ID  node_ID1  node_ID2  node_ID3  node_ID4                         PHI    Thickness
```

### Parameters

| Parameter | Type | Required | Default | Description |
|-----------|------|----------|---------|-------------|
| part_ID | INT | Yes | - | Part identifier |
| shell_ID | INT | Yes | - | Element identifier |
| node_ID1-4 | INT | Yes | - | Node IDs (counter-clockwise order) |
| PHI | FLOAT | No | 0.0 | Orthotropy angle (degrees) |
| Thickness | FLOAT | No | - | Element thickness (overrides property) |

### Node Ordering
```
     4 -------- 3
     |          |
     |    +     |  Normal direction: from 1-2-3-4 by right-hand rule
     |          |
     1 -------- 2
```

### Example
```
/SHELL/1
#shell_ID  node_ID1  node_ID2  node_ID3  node_ID4                         PHI    Thickness
         1         1         2         3         4                        0.0          2.0
         2         5         6         7         8                       45.0          2.0
```

---

## 3. SH3N (3-Node Shell Element)

Defines 3-node triangular shell elements.

### Syntax
```
/SH3N/part_ID
#  tria_ID  node_ID1  node_ID2  node_ID3                                  PHI    Thickness
```

### Parameters

| Parameter | Type | Required | Default | Description |
|-----------|------|----------|---------|-------------|
| part_ID | INT | Yes | - | Part identifier |
| tria_ID | INT | Yes | - | Element identifier |
| node_ID1-3 | INT | Yes | - | Node IDs (counter-clockwise) |
| PHI | FLOAT | No | 0.0 | Orthotropy angle (degrees) |
| Thickness | FLOAT | No | - | Element thickness |

### Node Ordering
```
        3
       /\
      /  \
     /    \
    1------2
```

### Example
```
/SH3N/2
#  tria_ID  node_ID1  node_ID2  node_ID3                                  PHI    Thickness
         1         1         2         5                                  0.0          1.5
         2         2         3         6                                  0.0          1.5
```

---

## 4. BRICK (8-Node Solid Element)

Defines 8-node hexahedral solid elements.

### Syntax
```
/BRICK/part_ID
#brick_ID  node_ID1  node_ID2  node_ID3  node_ID4  node_ID5  node_ID6  node_ID7  node_ID8
```

### Parameters

| Parameter | Type | Required | Description |
|-----------|------|----------|-------------|
| part_ID | INT | Yes | Part identifier |
| brick_ID | INT | Yes | Element identifier |
| node_ID1-4 | INT | Yes | Bottom face nodes (counter-clockwise) |
| node_ID5-8 | INT | Yes | Top face nodes (counter-clockwise) |

### Node Ordering
```
        8-------7
       /|      /|
      / |     / |
     5-------6  |
     |  4----|--3
     | /     | /
     |/      |/
     1-------2
```

### Degenerated Forms
- **6-node pentahedron**: Set node_ID4=node_ID3, node_ID8=node_ID7
- **5-node pyramid**: Set node_ID5=node_ID6=node_ID7=node_ID8
- **4-node tetrahedron**: Set node_ID3=node_ID4, node_ID5=node_ID6=node_ID7=node_ID8

### Example
```
/BRICK/1
#brick_ID  node_ID1  node_ID2  node_ID3  node_ID4  node_ID5  node_ID6  node_ID7  node_ID8
         1         1         2         3         4         5         6         7         8
         2         5         6         7         8         9        10        11        12
```

---

## 5. TETRA4 (4-Node Tetrahedral Element)

Defines 4-node tetrahedral solid elements.

### Syntax
```
/TETRA4/part_ID
#tetra_ID  node_ID1  node_ID2  node_ID3  node_ID4
```

### Parameters

| Parameter | Type | Required | Description |
|-----------|------|----------|-------------|
| part_ID | INT | Yes | Part identifier |
| tetra_ID | INT | Yes | Element identifier |
| node_ID1-4 | INT | Yes | Four corner nodes |

### Node Ordering
```
        4
       /|\
      / | \
     /  |  \
    /   |   \
   1----|----3
    \   |   /
     \  |  /
      \ | /
        2
```

### Example
```
/TETRA4/1
#tetra_ID  node_ID1  node_ID2  node_ID3  node_ID4
         1         1         2         3         4
         2         1         3         4         5
```

---

## 6. TETRA10 (10-Node Tetrahedral Element)

Defines 10-node quadratic tetrahedral elements.

### Syntax
```
/TETRA10/part_ID
#tetra_ID
#node_ID1  node_ID2  node_ID3  node_ID4  node_ID5  node_ID6  node_ID7  node_ID8  node_ID9 node_ID10
```

### Parameters

| Parameter | Type | Required | Description |
|-----------|------|----------|-------------|
| part_ID | INT | Yes | Part identifier |
| tetra_ID | INT | Yes | Element identifier |
| node_ID1-4 | INT | Yes | Corner nodes |
| node_ID5-10 | INT | Yes | Mid-edge nodes |

### Node Ordering
- Nodes 1-4: Corner vertices
- Nodes 5-10: Mid-edge nodes
  - Node 5: Mid-point of edge 1-2
  - Node 6: Mid-point of edge 2-3
  - Node 7: Mid-point of edge 1-3
  - Node 8: Mid-point of edge 1-4
  - Node 9: Mid-point of edge 2-4
  - Node 10: Mid-point of edge 3-4

### Example
```
/TETRA10/1
         1
         1         2         3         4         5         6         7         8         9        10
```

---

## 7. PENTA6 (6-Node Pentahedral Element)

Defines 6-node pentahedral (wedge) solid elements.

### Syntax
```
/PENTA6/part_ID
#penta_ID  node_ID1  node_ID2  node_ID3  node_ID4  node_ID5  node_ID6
```

### Parameters

| Parameter | Type | Required | Description |
|-----------|------|----------|-------------|
| part_ID | INT | Yes | Part identifier |
| penta_ID | INT | Yes | Element identifier |
| node_ID1-3 | INT | Yes | Bottom triangular face |
| node_ID4-6 | INT | Yes | Top triangular face |

### Node Ordering
```
        6
       /|\
      / | \
     4-----5
     |  3  |
     | /|\ |
     |/ | \|
     1-----2
```

### Example
```
/PENTA6/1
#penta_ID  node_ID1  node_ID2  node_ID3  node_ID4  node_ID5  node_ID6
         1         1         2         3         4         5         6
```

---

## 8. BRIC20 (20-Node Hexahedral Element)

Defines 20-node quadratic hexahedral solid elements.

### Syntax
```
/BRIC20/part_ID
#brick_ID  node_ID1  node_ID2  node_ID3  node_ID4  node_ID5  node_ID6  node_ID7  node_ID8
#          node_ID9 node_ID10 node_ID11 node_ID12 node_ID13 node_ID14 node_ID15 node_ID16
#         node_ID17 node_ID18 node_ID19 node_ID20
```

### Parameters

| Parameter | Type | Required | Description |
|-----------|------|----------|-------------|
| part_ID | INT | Yes | Part identifier |
| brick_ID | INT | Yes | Element identifier |
| node_ID1-8 | INT | Yes | Corner nodes |
| node_ID9-20 | INT | Yes | Mid-edge nodes |

### Example
```
/BRIC20/1
         1         1         2         3         4         5         6         7         8
         9        10        11        12        13        14        15        16
        17        18        19        20
```

---

## 9. SHEL16 (16-Node Thick Shell Element)

Defines 16-node quadratic thick shell elements.

### Syntax
```
/SHEL16/part_ID
#shell_ID  node_ID1  node_ID2  node_ID3  node_ID4  node_ID5  node_ID6  node_ID7  node_ID8
#          node_ID9 node_ID10 node_ID11 node_ID12
#         node_ID13 node_ID14 node_ID15 node_ID16
```

### Parameters

| Parameter | Type | Required | Description |
|-----------|------|----------|-------------|
| part_ID | INT | Yes | Part identifier |
| shell_ID | INT | Yes | Element identifier |
| node_ID1-8 | INT | Yes | Bottom face nodes |
| node_ID9-12 | INT | Yes | Mid-layer nodes |
| node_ID13-16 | INT | Yes | Top face nodes |

---

## 10. QUAD (4-Node 2D Element)

Defines 4-node quadrilateral 2D elements for axisymmetric or plane strain analysis.

### Syntax
```
/QUAD/part_ID
#  quad_ID  node_ID1  node_ID2  node_ID3  node_ID4
```

### Parameters

| Parameter | Type | Required | Description |
|-----------|------|----------|-------------|
| part_ID | INT | Yes | Part identifier |
| quad_ID | INT | Yes | Element identifier |
| node_ID1-4 | INT | Yes | Node IDs (counter-clockwise) |

### Example
```
/QUAD/1
#  quad_ID  node_ID1  node_ID2  node_ID3  node_ID4
         1         1         2         3         4
```

---

## 11. TRIA (3-Node 2D Element)

Defines 3-node triangular 2D elements.

### Syntax
```
/TRIA/part_ID
#  tria_ID  node_ID1  node_ID2  node_ID3
```

### Parameters

| Parameter | Type | Required | Description |
|-----------|------|----------|-------------|
| part_ID | INT | Yes | Part identifier |
| tria_ID | INT | Yes | Element identifier |
| node_ID1-3 | INT | Yes | Node IDs (counter-clockwise) |

### Example
```
/TRIA/1
#  tria_ID  node_ID1  node_ID2  node_ID3
         1         1         2         3
```

---

## 12. BEAM (Beam Element)

Defines 2-node beam elements with cross-section orientation.

### Syntax
```
/BEAM/part_ID
#  beam_ID  node_ID1  node_ID2  node_ID3                  Vx                  Vy                  Vz
```

### Parameters

| Parameter | Type | Required | Description |
|-----------|------|----------|-------------|
| part_ID | INT | Yes | Part identifier |
| beam_ID | INT | Yes | Element identifier |
| node_ID1 | INT | Yes | First node |
| node_ID2 | INT | Yes | Second node |
| node_ID3 | INT | No | Orientation node (if Vx=Vy=Vz=0) |
| Vx, Vy, Vz | FLOAT | No | Orientation vector components |

### Orientation Definition
- If node_ID3 > 0: Y-axis of beam is defined by vector from beam midpoint to node_ID3
- If Vx, Vy, Vz are defined: Y-axis is perpendicular to beam axis and lies in the plane containing Vx, Vy, Vz

### Example
```
/BEAM/3
#  beam_ID  node_ID1  node_ID2  node_ID3                  Vx                  Vy                  Vz
         1         1         2         0                 0.0                 1.0                 0.0
         2         2         4         5                 0.0                 0.0                 0.0
```

---

## 13. TRUSS (Truss Element)

Defines 2-node truss elements (axial force only).

### Syntax
```
/TRUSS/part_ID
#truss_ID  node_ID1  node_ID2
```

### Parameters

| Parameter | Type | Required | Description |
|-----------|------|----------|-------------|
| part_ID | INT | Yes | Part identifier |
| truss_ID | INT | Yes | Element identifier |
| node_ID1 | INT | Yes | First node |
| node_ID2 | INT | Yes | Second node |

### Example
```
/TRUSS/1
#truss_ID  node_ID1  node_ID2
         1         1         2
         2         2         3
         3         3         4
```

---

## 14. SPRING (Spring Element)

Defines spring elements for point-to-point connections.

### Syntax
```
/SPRING/part_ID
#sprg_ID  node_ID1  node_ID2  node_ID3  node_ID4  node_ID5  node_ID6                    Skew_ID
```

### Parameters

| Parameter | Type | Required | Default | Description |
|-----------|------|----------|---------|-------------|
| part_ID | INT | Yes | - | Part identifier |
| sprg_ID | INT | Yes | - | Element identifier |
| node_ID1 | INT | Yes | - | First node |
| node_ID2 | INT | Yes | - | Second node |
| node_ID3-6 | INT | No | 0 | Additional nodes (for multi-node springs) |
| Skew_ID | INT | No | 0 | Local coordinate system ID |

### Example
```
/SPRING/1
#sprg_ID  node_ID1  node_ID2  node_ID3  node_ID4  node_ID5  node_ID6                    Skew_ID
         1         1         2         0         0         0         0                       0
         2         2         3         0         0         0         0                       1
```

---

## 15. SPHCEL (SPH Cell Element)

Defines SPH (Smoothed Particle Hydrodynamics) particles.

### Syntax
```
/SPHCEL/part_ID
#      id      Flag                MASS
```

### Parameters

| Parameter | Type | Required | Description |
|-----------|------|----------|-------------|
| part_ID | INT | Yes | Part identifier |
| id | INT | Yes | Particle identifier |
| Flag | INT | Yes | Particle state flag |
| MASS | FLOAT | Yes | Particle mass |

### Example
```
/SPHCEL/1
#      id      Flag                MASS
         1         0                1.0
         2         0                1.0
         3         1                1.5
```

---

## Element Type Summary

| Keyword | Nodes | Type | Application |
|---------|-------|------|-------------|
| SHELL | 4 | Shell | Thin-walled structures |
| SH3N | 3 | Shell | Triangular mesh regions |
| BRICK | 8 | Solid | General 3D solids |
| TETRA4 | 4 | Solid | Tetrahedral mesh |
| TETRA10 | 10 | Solid | High-order tetrahedral |
| PENTA6 | 6 | Solid | Wedge elements |
| BRIC20 | 20 | Solid | High-order hexahedral |
| SHEL16 | 16 | Solid | Thick shell |
| QUAD | 4 | 2D | Axisymmetric/plane |
| TRIA | 3 | 2D | 2D triangular |
| BEAM | 2 | 1D | Beam structures |
| TRUSS | 2 | 1D | Truss structures |
| SPRING | 2+ | 0D | Point connections |
| SPHCEL | 1 | SPH | Particle methods |

---

## References

- Source files: `hm_cfg_files/config/CFG/radioss2025/ELEM/`
- Hierarchy definition: `hm_cfg_files/config/CFG/radioss2025/data_hierarchy.cfg`
