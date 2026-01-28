# OpenRadioss Keyword Manual

## Overview

This manual documents all keywords available in OpenRadioss 2025/2026 for finite element analysis.

OpenRadioss is a nonlinear dynamic finite element analysis software used for crash simulation, impact analysis, and other structural dynamics problems.

## Document Structure

| Document | Status | Description |
|----------|--------|-------------|
| [01_NODE_ELEMENTS.md](01_NODE_ELEMENTS.md) | Complete | Node and Element definitions (14 types) |
| [02_MATERIALS.md](02_MATERIALS.md) | Complete | Material laws (LAW1-LAW48, 10+ detailed) |
| [03_PROPERTIES.md](03_PROPERTIES.md) | Complete | Element properties (TYPE1-TYPE51) |
| [04_FAILURE.md](04_FAILURE.md) | Complete | Failure models (8 types) |
| [05_INTERFACES.md](05_INTERFACES.md) | Complete | Contact interfaces (TYPE2-TYPE24) |
| [06_LOADS_BC.md](06_LOADS_BC.md) | Complete | Loads and boundary conditions (12 types) |
| [07_INITIAL_CONDITIONS.md](07_INITIAL_CONDITIONS.md) | Complete | Initial conditions (14 types) |
| [08_CONSTRAINTS.md](08_CONSTRAINTS.md) | Complete | Constraints and rigid bodies (7 types) |
| [09_CONTROL_OUTPUT.md](09_CONTROL_OUTPUT.md) | Complete | Control cards and output (13 types) |
| [10_GROUPS_SETS.md](10_GROUPS_SETS.md) | Complete | Groups and sets (13 types) |
| [11_SENSORS.md](11_SENSORS.md) | Complete | Sensors and gauges (20+ types) |
| [12_ALE_SPH.md](12_ALE_SPH.md) | Complete | ALE and SPH methods (15+ types) |
| [13_AIRBAG.md](13_AIRBAG.md) | Complete | Airbag/Monitored volume (6 types) |
| [14_THERMAL.md](14_THERMAL.md) | Complete | Thermal analysis (7 types) |
| [15_FUNCTIONS_TABLES.md](15_FUNCTIONS_TABLES.md) | Complete | Functions and tables (6 types) |
| [16_PARAMETERS.md](16_PARAMETERS.md) | Complete | Parameters and expressions (6 types) |
| [17_LAMINATE_PLY.md](17_LAMINATE_PLY.md) | Complete | Laminate and ply definitions (2 types) |
| [18_FRICTION.md](18_FRICTION.md) | Complete | Friction models (4 types) |
| [19_EOS.md](19_EOS.md) | Complete | Equations of State (20 types) |
| [20_TRANSFORM.md](20_TRANSFORM.md) | Complete | Geometric transformations (6 types) |
| [21_ADVANCED.md](21_ADVANCED.md) | Complete | Advanced features: ADMAS, NONLOCAL, PERTURB |
| [22_CONTROL_CARDS.md](22_CONTROL_CARDS.md) | Complete | Control cards: BEGIN, RUN, STOP, DT, ANIM |
| [23_RIGID_WALL.md](23_RIGID_WALL.md) | Complete | Rigid walls (4 types) |
| [24_SYSTEM_SKEW.md](24_SYSTEM_SKEW.md) | Complete | Coordinate systems: SKEW, FRAME (5 types) |
| [25_MISC.md](25_MISC.md) | Complete | Miscellaneous: INCLUDE, SECT, DAMP, etc. |
| [26_SEATBELT.md](26_SEATBELT.md) | Complete | Seatbelt: RETRACTOR, SLIPRING |
| [27_BODY_CLUSTER.md](27_BODY_CLUSTER.md) | Complete | Flexible bodies: FXBODY, CLUSTER, ACTIV |
| [28_LAGMUL.md](28_LAGMUL.md) | Complete | Lagrange multipliers |

## Keyword Syntax

All OpenRadioss keywords follow the format:
```
/KEYWORD/option
field1  field2  field3  ...
```

### Input File Format
- Keywords start with `/`
- Comments start with `#` or `//`
- Field format: Fixed-format (legacy) or Free-format (recommended)
- Free-format uses comma `,` as delimiter

### Example
```
/NODE
# node_id    X           Y           Z
1            0.0         0.0         0.0
2            1.0         0.0         0.0
3            1.0         1.0         0.0
```

## Keyword Categories Summary

| Category | Count | Description |
|----------|-------|-------------|
| NODE | 1 | Node definition |
| ELEMENTS | 15+ | Shell, Solid, Beam, Spring, Truss, etc. |
| MATERIALS | 130+ | Material laws |
| PROPERTIES | 50+ | Element properties |
| FAILURE | 45+ | Failure criteria |
| INTERFACES | 25+ | Contact definitions |
| LOADS | 50+ | Loads and BCs |
| CONSTRAINTS | 20+ | Kinematic constraints |
| OUTPUT | 30+ | Output control |
| GROUPS | 15+ | Entity grouping |
| SENSORS | 20+ | Measurement sensors |
| ALE/SPH | 30+ | Advanced methods |
| CONTROL | 15+ | Simulation control |
| RWALL | 4 | Rigid walls |
| SKEW/FRAME | 5 | Coordinate systems |
| SEATBELT | 3 | Retractor, slipring |
| BODY | 2 | Flexible bodies |
| CLUSTER | 2 | Element clustering |
| LAGMUL | 3 | Lagrange multipliers |

## Version Information

- **Target Version**: OpenRadioss 2025/2026
- **CFG Files Location**: `hm_cfg_files/config/CFG/radioss2025/`
- **Last Updated**: 2026-01-23

## References

- OpenRadioss GitHub: https://github.com/OpenRadioss/OpenRadioss
- Altair Radioss Documentation
