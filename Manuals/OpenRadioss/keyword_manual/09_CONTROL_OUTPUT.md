# OpenRadioss Keyword Manual - Control Cards and Output

## Overview

This document describes control cards and output definitions in OpenRadioss. Control cards set simulation parameters, while output keywords control animation and time history data.

---

## 1. TITLE - Simulation Title

Defines a descriptive title for the simulation.

### Syntax
```
/TITLE
simulation_title
```

### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| simulation_title | STRING | Title (up to 100 characters) |

### Example
```
/TITLE
Crash Test Model - Frontal Impact Simulation
```

---

## 2. RUN - Run Control Parameters

Defines run identification and final time.

### Syntax
```
/RUN/run_name/run_number/restart_letter
Tstop
```

### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| run_name | STRING | Problem identification |
| run_number | INT | Run number |
| restart_letter | STRING | Restart file identifier |
| Tstop | FLOAT | Final simulation time |

### Example
```
/RUN/Drop_Test/1/A
0.015
```

---

## 3. UNIT - Unit System Definition

Defines the unit system for the model.

### Syntax
```
/UNIT/unit_ID
unit_title
#    MUNIT    LUNIT    TUNIT
```

### Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| MUNIT | FLOAT/STRING | 1.0 | Mass unit scale factor or code |
| LUNIT | FLOAT/STRING | 1.0 | Length unit scale factor or code |
| TUNIT | FLOAT/STRING | 1.0 | Time unit scale factor or code |

### Common Unit Codes

| Mass | Length | Time |
|------|--------|------|
| kg | m | s |
| g | mm | ms |
| Mg | mm | s |
| kg | mm | ms |

### Example - SI Units
```
/UNIT/1
SI Units (kg, m, s)
               1.0               1.0               1.0
```

### Example - Automotive (Mg, mm, s)
```
/UNIT/2
Automotive Units (Mg, mm, s)
                Mg                mm                 s
```

---

## 4. DEF_SHELL - Shell Element Defaults

Sets default values for shell element properties.

### Syntax
```
/DEF_SHELL
#Ishell   Ismstr   Ithick    Iplas  Istrain  Ioffset   ISH3N   Idrill
```

### Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| Ishell | INT | 24 | Shell formulation flag (0-24) |
| Ismstr | INT | 2 | Small strain flag (0-4) |
| Ithick | INT | 2 | Thickness change flag (0-2) |
| Iplas | INT | 2 | Plasticity flag (0-2) |
| Istrain | INT | 2 | Strain computation flag (0-2) |
| Ioffset | INT | 2 | Offset treatment flag (0-2) |
| ISH3N | INT | 2 | 3-node shell formulation (0,1,2,30,31) |
| Idrill | INT | 2 | Drilling DOF flag (0-2) |

### Ishell Options

| Value | Description |
|-------|-------------|
| 0 | Default (→24) |
| 1 | Q4 Belytschko visco-elastic hourglass |
| 2 | Q4 Hallquist visco-elastic hourglass |
| 3 | Q4 elasto-plastic hourglass |
| 4 | Q4 improved type 1 |
| 12 | QBAT shell formulation |
| 24 | QEPH shell formulation (recommended) |

### Ismstr Options

| Value | Description |
|-------|-------------|
| 0 | Default (→2) |
| 1 | Small strain from t=0 |
| 2 | Full geometric nonlinearities |
| 3 | Old small strain (Ishell=2 only) |
| 4 | Full geometric nonlinearities (no CST) |

### ISH3N Options

| Value | Description |
|-------|-------------|
| 0 | Default (→2) |
| 1 | Standard triangle (C0) |
| 2 | Standard with large rotation |
| 30 | DKT18 |
| 31 | DKT_S3 |

### Example
```
/DEF_SHELL
        24         2         2         2         2         2         2         2
```

---

## 5. DEF_SOLID - Solid Element Defaults

Sets default values for solid element properties.

### Syntax
```
/DEF_SOLID
#Isolid   Ismstr    Icpre  Itetra4 Itetra10     Imas   Iframe Icontrol
```

### Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| Isolid | INT | 1 | Solid formulation flag (0-24) |
| Ismstr | INT | 4 | Small strain flag (-2 to 12) |
| Icpre | INT | 1 | Constant pressure flag (-2 to 3) |
| Itetra4 | INT | 1000 | 4-node tetra formulation |
| Itetra10 | INT | 1000 | 10-node tetra formulation |
| Imas | INT | 2 | Mass distribution (0-2) |
| Iframe | INT | 1 | Coordinate system (0-2) |
| Icontrol | INT | 2 | Distortion control (-2 to 2) |

### Isolid Options

| Value | Description |
|-------|-------------|
| 0 | Default (→1) |
| 1 | 8-node, 1 IP (Belytschko hourglass) |
| 2 | 8-node, 1 IP (Hallquist hourglass) |
| 14 | HA8 full integration |
| 17 | H8C compatible solid |
| 18 | 8-node co-rotational |
| 24 | HEPH 8-node under-integrated |

### Itetra4 Options

| Value | Description |
|-------|-------------|
| 0 | Default (→1000) |
| 1 | Quadratic, 6 DOF/node, 4 IP |
| 3 | Linear with average nodal pressure |
| 1000 | Linear, 1 IP |

### Example
```
/DEF_SOLID
         1         4         1                  1000      1000         2         1         2
```

---

## 6. ANIM/DT - Animation Output Time Frequency

Defines animation output frequency.

### Syntax
```
/ANIM/DT
#          Tstart            Tfreq
```

### Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| Tstart | FLOAT | 0.0 | Start time for output |
| Tfreq | FLOAT | - | Output frequency/interval |

### Example
```
/ANIM/DT
               0.0             0.001
```

---

## 7. ANIM/GZIP - Animation Compression

Enables GZIP compression for animation files.

### Syntax
```
/ANIM/GZIP
```

### Example
```
/ANIM/GZIP
```

---

## 8. ANIM/NODA - Nodal Animation Variables

Specifies nodal variables for animation output.

### Syntax
```
/ANIM/NODA/variable
```

### Available Variables

| Variable | Description |
|----------|-------------|
| DT | Nodal time step |
| DMAS | Mass variation |
| DINER | Added inertia |
| NDMAS | Non-diagonal mass variation |
| DAMA2 | Damage for TYPE2 interface |
| DENS | Nodal density |
| P | Nodal pressure |
| TEMP | Nodal temperature |
| VFRAC | Volumetric fraction |
| SSP | Sound speed |

### Example
```
/ANIM/NODA/DT
/ANIM/NODA/TEMP
/ANIM/NODA/DENS
```

---

## 9. ANIM/ELEM - Element Animation Variables

Specifies element variables for animation output.

### Syntax
```
/ANIM/ELEM/variable
```

### Available Variables

| Variable | Description |
|----------|-------------|
| EPSP | Plastic strain |
| ENER | Internal energy |
| VONM | Von Mises stress |
| HOURG | Hourglass energy |
| FAIL | Failure indicator |
| THICK | Shell thickness |
| DAMA | Damage |

### Example
```
/ANIM/ELEM/EPSP
/ANIM/ELEM/VONM
/ANIM/ELEM/FAIL
```

---

## 10. TH/PART - Part Time History Output

Defines time history output for parts.

### Syntax
```
/TH/PART/th_ID
th_title
#variables
#part_list
```

### Available Variables

| Category | Variables |
|----------|-----------|
| Energy | DEF, IE, KE, HE, RIE, HEAT, PW |
| Momentum | XMOM, YMOM, ZMOM, XXMOM, YYMOM, ZZMOM |
| Physical | MASS, DENS, TEMP |
| Kinetic | KERB, RKERB, RKE, TURBKE |
| Geometry | XCG, YCG, ZCG, IXX, IYY, IZZ |
| Velocity | VX, VY, VZ |
| Status | ERODED |

### Example
```
/TH/PART/1
Vehicle_Body_TH
DEF  KE  IE  MASS  ERODED
1  2  3
```

---

## 11. TH/NODE - Node Time History Output

Defines time history output for nodes.

### Syntax
```
/TH/NODE/th_ID
th_title
#variables
#node_list
```

### Available Variables

| Variable | Description |
|----------|-------------|
| DEF | Default set |
| DX, DY, DZ | Displacement components |
| VX, VY, VZ | Velocity components |
| AX, AY, AZ | Acceleration components |
| FX, FY, FZ | Force components |
| TEMP | Temperature |

### Example
```
/TH/NODE/2
Impact_Point_TH
DEF  VX  VY  VZ  AX  AY  AZ
100  101  102
```

---

## 12. TH/SHEL - Shell Element Time History

Defines time history output for shell elements.

### Syntax
```
/TH/SHEL/th_ID
th_title
#variables
#element_list
```

### Available Variables

| Category | Variables |
|----------|-----------|
| Stress | SX, SY, SZ, SXY, SYZ, SXZ, VONM |
| Strain | EPSX, EPSY, EPSZ, EPSXY |
| Other | PLAS, TEMP, THICK, FAIL |

### Example
```
/TH/SHEL/3
Critical_Shell_TH
VONM  PLAS  THICK  FAIL
5001  5002  5003
```

---

## 13. TH/BRIC - Solid Element Time History

Defines time history output for solid elements.

### Syntax
```
/TH/BRIC/th_ID
th_title
#variables
#element_list
```

### Available Variables

| Category | Variables |
|----------|-----------|
| Stress | STRESS, SX, SY, SZ, SXY, SYZ, SXZ |
| Strain | STRAIN, EPSXX, EPSYY, EPSZZ, VSTRAIN |
| Physical | IE, DENS, PLAS, TEMP, VOL |
| Damage | DAM1, DAM2, DAM3, DAMA |

### Example
```
/TH/BRIC/4
Solid_Element_TH
STRESS  PLAS  TEMP
10001  10002
```

---

## Control Card Summary

| Keyword | Purpose | Type |
|---------|---------|------|
| /TITLE | Simulation description | Header |
| /RUN | Run control | Control |
| /UNIT | Unit system | Control |
| /DEF_SHELL | Shell defaults | Control |
| /DEF_SOLID | Solid defaults | Control |
| /ANIM/DT | Animation frequency | Output |
| /ANIM/GZIP | Animation compression | Output |
| /ANIM/NODA | Nodal animation | Output |
| /ANIM/ELEM | Element animation | Output |
| /TH/PART | Part time history | Output |
| /TH/NODE | Node time history | Output |
| /TH/SHEL | Shell time history | Output |
| /TH/BRIC | Solid time history | Output |

---

## Example: Complete Control Setup

```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/TITLE
Crash Simulation - Side Impact Test

/RUN/Side_Impact/1/A
0.100

/UNIT/1
SI Units
                kg                 m                 s

/DEF_SHELL
        24         2         2         2         2         2         2         2

/DEF_SOLID
         1         4         1                  1000      1000         2         1         2

# Animation Output
/ANIM/DT
               0.0             0.001

/ANIM/GZIP

/ANIM/ELEM/EPSP
/ANIM/ELEM/VONM
/ANIM/NODA/TEMP

# Time History Output
/TH/PART/1
Vehicle_Energy
DEF  KE  IE  MASS
1  2  3  4  5

/TH/NODE/1
Impact_Points
DEF  VX  AX
100  101  102  103
```

---

## References

- Source files: `hm_cfg_files/config/CFG/radioss2025/CARDS/`
- Source files: `hm_cfg_files/config/CFG/radioss2025/OUTPUTBLOCK/`
