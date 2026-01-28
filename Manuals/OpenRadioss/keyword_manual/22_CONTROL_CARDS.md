# OpenRadioss Keyword Manual - Control Cards

## Overview

This document describes control cards that define global simulation parameters in OpenRadioss including BEGIN, TITLE, RUN, STOP, and other essential simulation control keywords.

---

## 1. BEGIN - Simulation Start Card

The /BEGIN keyword defines simulation initialization parameters including run name and unit system.

### Syntax
```
/BEGIN
run_name
#    INVERS
#       Input_mass           Input_length            Input_time
#        Work_mass            Work_length             Work_time
```

### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| run_name | STRING | Run name identifier (100 chars max) |
| INVERS | INT | Input version number |
| Input_mass | STRING | Mass unit for input (kg, g, mg, etc.) |
| Input_length | STRING | Length unit for input (m, mm, cm, etc.) |
| Input_time | STRING | Time unit for input (s, ms, μs, etc.) |
| Work_mass | STRING | Mass unit for computation |
| Work_length | STRING | Length unit for computation |
| Work_time | STRING | Time unit for computation |

### Common Unit Systems

| System | Mass | Length | Time |
|--------|------|--------|------|
| SI | kg | m | s |
| mm-ton-s | Mg (ton) | mm | s |
| mm-kg-ms | kg | mm | ms |
| CGS | g | cm | s |

### Example
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/BEGIN
Crash_Simulation_Model
       110
                  kg                   m                   s
                  kg                   m                   s
```

---

## 2. TITLE - Model Title

The /TITLE keyword specifies the model title/description.

### Syntax
```
/TITLE
model_title
```

### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| model_title | STRING | Model description (100 chars max) |

### Example
```
/TITLE
Full Vehicle Frontal Crash at 56 km/h
```

---

## 3. RUN - Run Name

The /RUN keyword specifies the simulation run name and stop time.

### Syntax (Starter)
```
/RUN/run_name/run_number
```

### Syntax (Engine)
```
/RUN/run_name/run_number/T_stop
```

### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| run_name | STRING | Base name for output files |
| run_number | INT | Run number identifier |
| T_stop | FLOAT | Simulation stop time (Engine only) |

### Example
```
/RUN/CRASH_MODEL/1/0.150
```

---

## 4. STOP - Termination Criteria

The /STOP keyword defines simulation termination conditions.

### Syntax
```
/STOP
#              Tmax           Fext_min
```

### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| Tmax | FLOAT | Maximum simulation time |
| Fext_min | FLOAT | Minimum external force (optional termination) |

### Example
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/STOP
              0.150
```

---

## 5. DT - Time Step Control

### 5.1 DT/NODA - Nodal Time Step

Controls the minimum nodal time step.

#### Syntax
```
/DT/NODA/Iflag
#         Tmin_scale          Tmin_value
```

#### Iflag Values

| Value | Description |
|-------|-------------|
| 0 | Elementary time step only |
| 1 | Nodal time step enabled |
| 2 | Nodal time step with mass scaling |

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| Tmin_scale | FLOAT | Scale factor for minimum time step |
| Tmin_value | FLOAT | Minimum time step value |

#### Example
```
/DT/NODA/1
                0.9           1.0E-07
```

### 5.2 DT/BRICK/Iflag - Solid Element Time Step

Controls time step for solid elements.

### 5.3 DT/SHELL/Iflag - Shell Element Time Step

Controls time step for shell elements.

### 5.4 DT/INTER/Iflag - Interface Time Step

Controls time step for contact interfaces.

---

## 6. ANIM - Animation Output

The /ANIM keywords control animation file output.

### 6.1 ANIM/DT - Animation Time Step

#### Syntax
```
/ANIM/DT
#         Tstart              Tfreq
```

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| Tstart | FLOAT | Animation start time |
| Tfreq | FLOAT | Animation output frequency |

#### Example
```
/ANIM/DT
                0.0           0.001
```

### 6.2 ANIM/ELEM - Element Results

#### Available Element Variables

| Keyword | Description |
|---------|-------------|
| /ANIM/ELEM/EPSP | Equivalent plastic strain |
| /ANIM/ELEM/ENER | Internal energy density |
| /ANIM/ELEM/VONM | Von Mises stress |
| /ANIM/ELEM/SIGX | Stress XX component |
| /ANIM/ELEM/SIGY | Stress YY component |
| /ANIM/ELEM/SIGZ | Stress ZZ component |
| /ANIM/ELEM/SIGXY | Stress XY component |
| /ANIM/ELEM/FAIL | Failure flag |
| /ANIM/ELEM/THICK | Current thickness |

### 6.3 ANIM/VECT - Vector Results

#### Available Vector Variables

| Keyword | Description |
|---------|-------------|
| /ANIM/VECT/DISP | Displacement vector |
| /ANIM/VECT/VEL | Velocity vector |
| /ANIM/VECT/ACC | Acceleration vector |
| /ANIM/VECT/CONT | Contact force vector |
| /ANIM/VECT/FOPT | Optional force vector |

### Example - Complete Animation Setup
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/ANIM/DT
                0.0            0.001
/ANIM/VECT/DISP
/ANIM/VECT/VEL
/ANIM/ELEM/EPSP
/ANIM/ELEM/VONM
/ANIM/ELEM/ENER
```

---

## 7. PRINT - Output Control

The /PRINT keyword controls text output frequency.

### Syntax
```
/PRINT/-frequency
```

### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| frequency | INT | Print output every N cycles (negative = -N) |

### Example
```
/PRINT/-1000
```

---

## 8. TFILE - Time History Output

The /TFILE keyword controls time history file output.

### Syntax
```
/TFILE/type
#              Tfreq
```

### Type Values

| Type | Description |
|------|-------------|
| (none) | Default ASCII output |
| 4 | Binary output (recommended) |

### Example
```
/TFILE/4
           0.0001
```

---

## 9. VERS - Version Control

The /VERS keyword specifies solver version compatibility.

### Syntax
```
/VERS/version
```

### Common Versions

| Version | Description |
|---------|-------------|
| 100 | Radioss 10.0 format |
| 110 | Radioss 11.0 format |
| 120 | Radioss 12.0 format |
| 2017-2026 | Year-based versions |

### Example
```
/VERS/2026
```

---

## 10. IOFLAG - Input/Output Flags

The /IOFLAG keyword controls various I/O behaviors.

### Syntax
```
/IOFLAG
#   Ipri    Igtyp    Ioutp    Iwrit    Iturbo
```

### Parameters

| Parameter | Description |
|-----------|-------------|
| Ipri | Print flag |
| Igtyp | Geometry type output |
| Ioutp | Output type control |
| Iwrit | Write control |
| Iturbo | Turbo mode flag |

---

## 11. DEF_SHELL - Default Shell Properties

The /DEF_SHELL keyword sets default shell element parameters.

### Syntax
```
/DEF_SHELL
#   Ishell    Ismstr     Ish3n     Idrill    Ithick    Iplas
```

### Parameters

| Parameter | Values | Description |
|-----------|--------|-------------|
| Ishell | 1-24 | Default shell formulation |
| Ismstr | 0-4 | Small strain flag |
| Ish3n | 1-30 | 3-node shell formulation |
| Idrill | 0,1 | Drilling DOF |
| Ithick | 0,1 | Thickness change flag |
| Iplas | 0,1 | Plasticity formulation |

### Example
```
/DEF_SHELL
        24         0         2         0         1         1
```

---

## 12. DEF_SOLID - Default Solid Properties

The /DEF_SOLID keyword sets default solid element parameters.

### Syntax
```
/DEF_SOLID
#  Isolid    Ismstr    Icpre   Itetra4   Itetra10   Iframe       Dn
```

### Parameters

| Parameter | Description |
|-----------|-------------|
| Isolid | Default solid formulation |
| Ismstr | Small strain flag |
| Icpre | Constant pressure flag |
| Itetra4 | 4-node tetra formulation |
| Itetra10 | 10-node tetra formulation |
| Iframe | Reference frame flag |
| Dn | Numerical damping |

---

## Best Practices

### Control Card Order

Recommended order for control cards in Starter input:
1. /BEGIN
2. /TITLE
3. /VERS
4. /UNIT (if used)
5. /DEF_SHELL, /DEF_SOLID (defaults)
6. /IOFLAG
7. Model data (nodes, elements, materials, etc.)

### Engine Control Cards

Typical Engine input structure:
1. /RUN
2. /STOP
3. /DT control cards
4. /ANIM output cards
5. /PRINT
6. /TFILE

---

## Example - Complete Control Card Setup

### Starter Input
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/BEGIN
Crash_Model_2026
       110
                  kg                  mm                  ms
                  kg                  mm                  ms
/TITLE
Frontal Barrier Test - Full Vehicle
/VERS/2026
/DEF_SHELL
        24         0         2         0         1         1
/DEF_SOLID
        24         0         0         0         0         0
```

### Engine Input
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/RUN/CRASH_MODEL/1/0.150
/STOP
              0.150
/DT/NODA/1
               0.67           1.0E-07
/DT/INTER/1
               0.50           1.0E-07
/ANIM/DT
               0.0            0.001
/ANIM/VECT/DISP
/ANIM/VECT/VEL
/ANIM/ELEM/EPSP
/ANIM/ELEM/VONM
/ANIM/ELEM/ENER
/PRINT/-1000
/TFILE/4
            0.0001
```

---

## References

- Control cards source: `hm_cfg_files/config/CFG/radioss*/CARDS/`
- Starter source: `starter/source/input/`
- Engine source: `engine/source/input/`

