# OpenRadioss Keyword Manual - Groups and Sets

## Overview

This document describes group and set definitions in OpenRadioss. Groups and sets organize entities (nodes, elements, parts, surfaces) for use with loads, constraints, and output.

---

## Group Keyword Syntax

```
/GRxxxx/option/group_ID
group_title
[entity list or references]
```

---

## 1. GRNOD - Node Groups

Defines a group of nodes.

### Syntax Options

**Direct Node List:**
```
/GRNOD/NODE/grnod_ID
group_title
#node_list
```

**From Another Node Group:**
```
/GRNOD/GRNOD/grnod_ID
group_title
#grnod_list
```

**From Surface:**
```
/GRNOD/SURF/grnod_ID
group_title
#surf_ID
```

**From Part:**
```
/GRNOD/PART/grnod_ID
group_title
#part_list
```

### Available Options

| Option | Description |
|--------|-------------|
| NODE | Direct node list |
| NODENS | Sorted node list |
| BOX | Box selection by coordinates |
| GRNOD | Reference node groups |
| SURF | Nodes from surface |
| LINE | Nodes from line |
| PART | Nodes from part |
| MAT | Nodes from material |
| PROP | Nodes from property |
| GRSHEL | Nodes from shell group |
| GRBRIC | Nodes from brick group |
| GRBEAM | Nodes from beam group |
| GEN_INCR | Generate incremental list |
| GENE | Generate by pattern |

### Examples

**Direct Node List:**
```
/GRNOD/NODE/1
Fixed_Base_Nodes
         1         2         3         4         5         6         7         8
```

**Combine Node Groups:**
```
/GRNOD/GRNOD/10
Combined_Support
         1         2
```

**Nodes from Surface:**
```
/GRNOD/SURF/20
Surface_Nodes
         5
```

---

## 2. GRPART - Part Groups

Defines a group of parts.

### Syntax
```
/GRPART/PART/grpart_ID
group_title
#part_list
```

### Available Options

| Option | Description |
|--------|-------------|
| PART | Direct part list |
| GRPART | Reference part groups |
| MAT | Parts with material |
| PROP | Parts with property |
| SUBSET | Parts from subset |

### Example
```
/GRPART/PART/1
Vehicle_Body_Parts
         1         2         3         4         5
        10        11        12        13        14
```

---

## 3. GRSHEL - Shell Element Groups

Defines a group of 4-node shell elements.

### Syntax
```
/GRSHEL/SHEL/grshel_ID
group_title
#element_list
```

### Available Options

| Option | Description |
|--------|-------------|
| SHEL | Direct shell list |
| GRSHEL | Reference shell groups |
| BOX | Box selection |
| BOX2 | Extended box selection |
| PART | Shells from part |
| MAT | Shells with material |
| PROP | Shells with property |

### BOX Selection Sub-options

| Shape | Description |
|-------|-------------|
| 1 | Rectangular/Cartesian |
| 2 | Cylindrical |
| 3 | Spherical |

### Examples

**Direct Element List:**
```
/GRSHEL/SHEL/1
Critical_Shell_Elements
      5001      5002      5003      5004      5005
```

**Box Selection - Spherical:**
```
/GRSHEL/BOX/10
Shell_Sphere_Selection
         3         1
```

---

## 4. GRBRIC - Solid Element Groups

Defines a group of brick (solid) elements.

### Syntax
```
/GRBRIC/BRIC/grbric_ID
group_title
#element_list
```

### Available Options

| Option | Description |
|--------|-------------|
| BRIC | Direct brick list |
| GRBRIC | Reference brick groups |
| BOX | Box selection |
| BOX2 | Extended box selection |
| PART | Bricks from part |
| MAT | Bricks with material |
| PROP | Bricks with property |

### Example
```
/GRBRIC/BRIC/1
Engine_Block_Elements
     10001     10002     10003     10004     10005
     10006     10007     10008     10009     10010
```

---

## 5. GRSH3N - 3-Node Shell Element Groups

Defines a group of 3-node shell (triangular) elements.

### Syntax
```
/GRSH3N/SH3N/grsh3n_ID
group_title
#element_list
```

### Available Options

| Option | Description |
|--------|-------------|
| SH3N | Direct 3-node shell list |
| GRSH3N | Reference groups |
| BOX | Box selection |
| PART | Elements from part |
| MAT | Elements with material |
| PROP | Elements with property |

### Example
```
/GRSH3N/SH3N/1
Triangle_Elements
      3001      3002      3003      3004
```

---

## 6. GRBEAM - Beam Element Groups

Defines a group of beam elements.

### Syntax
```
/GRBEAM/BEAM/grbeam_ID
group_title
#element_list
```

### Available Options

| Option | Description |
|--------|-------------|
| BEAM | Direct beam list |
| GRBEAM | Reference groups |
| BOX | Box selection |
| PART | Beams from part |
| MAT | Beams with material |
| PROP | Beams with property |

### Example
```
/GRBEAM/BEAM/1
Frame_Beams
       100       101       102       103       104
```

---

## 7. GRTRUS - Truss Element Groups

Defines a group of truss elements.

### Syntax
```
/GRTRUS/TRUS/grtrus_ID
group_title
#element_list
```

### Example
```
/GRTRUS/TRUS/1
Cable_Trusses
       200       201       202       203
```

---

## 8. GRSPRI - Spring Element Groups

Defines a group of spring elements.

### Syntax
```
/GRSPRI/SPRI/grspri_ID
group_title
#element_list
```

### Example
```
/GRSPRI/SPRI/1
Suspension_Springs
       300       301       302       303
```

---

## 9. GRQUAD - Quad Element Groups

Defines a group of quadrilateral 2D elements.

### Syntax
```
/GRQUAD/QUAD/grquad_ID
group_title
#element_list
```

### Example
```
/GRQUAD/QUAD/1
2D_Mesh_Elements
      1001      1002      1003      1004
```

---

## 10. SURF - Surface Definitions

Defines a surface for contact interfaces and loads.

### Syntax Options

**From Shell Group:**
```
/SURF/GRSHEL/surf_ID
surface_title
#grshel_ID
```

**From Part:**
```
/SURF/PART/surf_ID
surface_title
#part_list
```

**From Segments:**
```
/SURF/SEG/surf_ID
surface_title
#segment_data
```

### Available Options

| Option | Description |
|--------|-------------|
| BOX | Box selection |
| GRSH3N | From 3-node shell group |
| GRSHEL | From 4-node shell group |
| GRBRIC | From brick group (external faces) |
| PART | From part |
| MAT | From material |
| PROP | From property |
| SEG | From segments |
| SURF | Reference surface |
| PLANE | Planar surface |
| ELLIPS | Ellipsoidal surface |

### Examples

**Surface from Part:**
```
/SURF/PART/1
Contact_Surface_Body
         1         2         3
```

**Surface from Shell Group:**
```
/SURF/GRSHEL/2
Impact_Surface
         5
```

---

## 11. LINE - Line Definitions

Defines a line for edge-to-edge contact or visualization.

### Syntax
```
/LINE/option/line_ID
line_title
#parameters
```

### Available Options

| Option | Description |
|--------|-------------|
| BOX | Box selection |
| SEG | From segments |
| LINE | Reference line |
| SURF | From surface |
| EDGE | From edges |
| GRBEAM | From beam group |
| PART | From part |

### Example
```
/LINE/EDGE/1
Contact_Edge
         1
```

---

## 12. BOX - Box Selection

Defines a geometric box for entity selection.

### Syntax
```
/BOX/box_ID
box_title
#  Xmin  Ymin  Zmin  Xmax  Ymax  Zmax
```

### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| Xmin | FLOAT | Minimum X coordinate |
| Ymin | FLOAT | Minimum Y coordinate |
| Zmin | FLOAT | Minimum Z coordinate |
| Xmax | FLOAT | Maximum X coordinate |
| Ymax | FLOAT | Maximum Y coordinate |
| Zmax | FLOAT | Maximum Z coordinate |

### Example
```
/BOX/1
Selection_Box
              0.0              0.0              0.0
            100.0            100.0             50.0
```

---

## 13. SUBSET - Subset Definition

Defines a subset of entities from another set.

### Syntax
```
/SUBSET/subset_ID
subset_title
#entity_references
```

---

## Group Summary Table

| Keyword | ID Pool | Entity Type |
|---------|---------|-------------|
| /GRNOD | 32 | Nodes |
| /GRSHEL | 33 | 4-node shells |
| /GRBEAM | 34 | Beams |
| /GRBRIC | 35 | Bricks (solids) |
| /GRTRUS | 36 | Trusses |
| /GRSPRI | 37 | Springs |
| /GRQUAD | 38 | Quads |
| /GRSH3N | 39 | 3-node shells |
| /SURF | 40 | Surfaces |
| /LINE | 41 | Lines |
| /GRPART | 44 | Parts |

---

## Common Selection Options

All group types support these common selection methods:

| Option | Description |
|--------|-------------|
| Direct List | Explicit entity IDs |
| GR[type] | Reference another group |
| BOX | Geometric box selection |
| PART | Entities from specific parts |
| MAT | Entities with specific material |
| PROP | Entities with specific property |
| SUBSET | From a subset |
| GEN_INCR | Generate incremental list |
| GENE | Generate by pattern |

---

## Example: Complete Group Setup

```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
# Node Groups
/GRNOD/NODE/1
Fixed_Support
         1         2         3         4

/GRNOD/NODE/2
Load_Application
       100       101       102       103

# Part Groups
/GRPART/PART/1
Vehicle_Structure
         1         2         3         4         5

# Shell Groups
/GRSHEL/PART/1
Body_Shells
         1         2         3

# Surfaces for Contact
/SURF/PART/1
Master_Surface
         1         2

/SURF/PART/2
Slave_Surface
         3         4

# Line for Edge Contact
/LINE/EDGE/1
Sharp_Edge
         1
```

---

## Best Practices

1. **Naming Convention**:
   - Use descriptive names for groups
   - Include entity type in name when helpful
   - Example: `BC_Fixed_Nodes`, `Contact_Surface_Hood`

2. **Group Organization**:
   - Create base groups for common entity sets
   - Use reference options to combine groups
   - Avoid duplicating entities across groups

3. **Selection Method**:
   - Use direct lists for small, specific sets
   - Use PART/MAT/PROP for large, consistent sets
   - Use BOX for geometric regions

4. **Surface Definition**:
   - GRSHEL/GRSH3N for shell surfaces
   - GRBRIC for external solid faces
   - PART for all elements in a part

---

## References

- Source files: `hm_cfg_files/config/CFG/radioss2025/` (data_hierarchy.cfg)
- Starter source: `starter/source/groups/`
