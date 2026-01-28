# OpenRadioss Keyword Manual - Loads and Boundary Conditions

## Overview

This document describes load and boundary condition definitions in OpenRadioss. These keywords define external forces, prescribed motions, constraints, and initial conditions.

---

## 1. BCS - Boundary Conditions (Fixed Constraints)

Defines fixed boundary conditions applied to node groups.

### Syntax
```
/BCS/bcs_ID
bcs_title
#Tra rot  skew_ID  grnod_ID
```

### Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| bcs_ID | INT | - | Unique boundary condition ID |
| bcs_title | STRING | - | Description (100 chars max) |
| Tra | INT | 0 | Translation constraint code (3 digits: XYZ) |
| rot | INT | 0 | Rotation constraint code (3 digits: XYZ) |
| skew_ID | INT | 0 | Local coordinate system ID |
| grnod_ID | INT | - | Node group ID |

### DOF Code Format
- Each digit is 0 (free) or 1 (fixed)
- Example: 111 = fix X, Y, Z
- Example: 100 = fix X only
- Example: 011 = fix Y and Z

### Example
```
/BCS/1
Fixed_Support
#Tra rot  skew_ID  grnod_ID
 111 111        0        10
```
This fixes all 6 DOFs on node group 10.

```
/BCS/2
Roller_Support
#Tra rot  skew_ID  grnod_ID
 001 000        0        20
```
This fixes only Z translation (roller in XY plane).

---

## 2. IMPDISP - Imposed Displacement

Applies time-dependent prescribed displacement.

### Syntax
```
/IMPDISP/impdisp_ID
impdisp_title
#funct_ID       Dir  skew_ID sens_ID  grnod_ID frame_ID    Icoor
#         Ascale_x          Fscale_y            Tstart             Tstop
```

### Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| funct_ID | INT | - | Function ID (time curve) |
| Dir | STRING | - | Direction: X, Y, Z (trans) or XX, YY, ZZ (rot) |
| skew_ID | INT | 0 | Local coordinate system |
| sens_ID | INT | 0 | Sensor ID for triggering |
| grnod_ID | INT | - | Node group ID |
| frame_ID | INT | 0 | Frame ID |
| Icoor | INT | 0 | Coordinate type (0=Cartesian, 1=Cylindrical) |
| Ascale_x | FLOAT | 1.0 | Time scale factor |
| Fscale_y | FLOAT | 1.0 | Displacement scale factor |
| Tstart | FLOAT | 0.0 | Start time |
| Tstop | FLOAT | 1e30 | Stop time |

### Example
```
/IMPDISP/1
Ramp_Displacement_X
        10        X         0        0        20         0         0
               1.0             0.001               0.0            10.0
```

---

## 3. IMPVEL - Imposed Velocity

Applies time-dependent prescribed velocity.

### Syntax
```
/IMPVEL/impvel_ID
impvel_title
#funct_ID       Dir  skew_ID sens_ID  grnod_ID frame_ID    Icoor
#         Ascale_x          Fscale_y            Tstart             Tstop
```

### Parameters

Same structure as IMPDISP, but Fscale_y applies to velocity.

### Example
```
/IMPVEL/1
Constant_Velocity
        15        Y         0        0        30         0         0
               1.0              10.0               0.0           100.0
```

---

## 4. IMPACC - Imposed Acceleration

Applies time-dependent prescribed acceleration.

### Syntax
```
/IMPACC/impacc_ID
impacc_title
#funct_ID       Dir  skew_ID sens_ID  grnod_ID
#         Ascale_x          Fscale_y            Tstart             Tstop
```

### Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| funct_ID | INT | - | Function ID |
| Dir | STRING | - | Direction (X, Y, Z, XX, YY, ZZ) |
| skew_ID | INT | 0 | Local coordinate system |
| sens_ID | INT | 0 | Sensor ID |
| grnod_ID | INT | - | Node group ID |
| Ascale_x | FLOAT | 1.0 | Time scale factor |
| Fscale_y | FLOAT | 1.0 | Acceleration scale factor |
| Tstart | FLOAT | 0.0 | Start time |
| Tstop | FLOAT | 1e30 | Stop time |

### Example
```
/IMPACC/1
Crash_Pulse
        20        X         0        0       100
               1.0             -9.81               0.0            0.15
```

---

## 5. CLOAD - Concentrated Load

Applies time-dependent concentrated forces or moments.

### Syntax
```
/CLOAD/cload_ID
cload_title
#funct_ID       Dir  skew_ID sens_ID  grnod_ID  Itypfun          Ascale_x          Fscale_y
```

### Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| funct_ID | INT | - | Function ID |
| Dir | STRING | - | Direction (X, Y, Z for force; XX, YY, ZZ for moment) |
| skew_ID | INT | 0 | Local coordinate system |
| sens_ID | INT | 0 | Sensor ID |
| grnod_ID | INT | - | Node group ID |
| Itypfun | INT | 1 | Function type (1=time, 2=disp, 3=vel) |
| Ascale_x | FLOAT | 1.0 | X-axis scale factor |
| Fscale_y | FLOAT | 1.0 | Force/moment scale factor |

### Example
```
/CLOAD/1
Axial_Force
        25        X         0        0        15         1               1.0            1000.0
```
Applies 1000N force in X direction from function 25.

---

## 6. PLOAD - Pressure Load

Applies time-dependent pressure on surfaces.

### Syntax
```
/PLOAD/pload_ID
pload_title
#surf_ID  funct_ID sens_ID   Ipinch     Idel  Itypfun          Ascale_x          Fscale_y
```

### Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| surf_ID | INT | - | Surface ID |
| funct_ID | INT | - | Function ID |
| sens_ID | INT | 0 | Sensor ID |
| Ipinch | INT | 0 | Pinching type |
| Idel | INT | 1 | Deletion flag (1=stop when elements deleted) |
| Itypfun | INT | 1 | Function type |
| Ascale_x | FLOAT | 1.0 | Time scale factor |
| Fscale_y | FLOAT | 1.0 | Pressure scale factor |

### Example
```
/PLOAD/1
Wind_Pressure
        30        35         0         0         1         1               1.0             0.005
```
Applies 0.005 MPa pressure on surface 30.

---

## 7. GRAV - Gravity Load

Applies gravitational acceleration (body force).

### Syntax
```
/GRAV/grav_ID
grav_title
#funct_ID       Dir  skew_ID sens_ID  grnod_ID                      Ascale_x          Fscale_y
```

### Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| funct_ID | INT | - | Function ID |
| Dir | STRING | - | Direction (X, Y, Z) |
| skew_ID | INT | 0 | Local coordinate system |
| sens_ID | INT | 0 | Sensor ID |
| grnod_ID | INT | - | Node group ID (or ALL) |
| Ascale_x | FLOAT | 1.0 | Time scale factor |
| Fscale_y | FLOAT | 1.0 | Gravity scale factor |

### Example
```
/GRAV/1
Earth_Gravity
       100        Z         0        0       ALL                         1.0             -9.81
```

---

## 8. INIVEL - Initial Velocity

Defines initial velocities at analysis start.

### Syntax
```
/INIVEL/TRA/inivel_ID
inivel_title
#                Vx                 Vy                 Vz  grnod_ID  skew_ID
#           Tstart   sens_ID
```

For rotational:
```
/INIVEL/ROT/inivel_ID
```

### Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| Vx, Vy, Vz | FLOAT | 0.0 | Velocity components |
| grnod_ID | INT | - | Node group ID |
| skew_ID | INT | 0 | Local coordinate system |
| Tstart | FLOAT | 0.0 | Start time (for restart) |
| sens_ID | INT | 0 | Sensor ID |

### Example - Translational
```
/INIVEL/TRA/1
Moving_Parts
              10.5               5.0               0.0        50         0
               0.0         0
```

### Example - Rotational
```
/INIVEL/ROT/2
Spinning_Assembly
               0.0               0.0             100.0        60         0
               0.0         0
```

---

## 9. INIVEL/AXIS - Initial Velocity with Rotation Axis

Defines initial velocity for rotating bodies about an axis.

### Syntax
```
/INIVEL/AXIS/inivel_ID
inivel_title
#     Dir frame_ID  grnod_ID
#                Vxt                Vyt                Vzt                 VR
#           Tstart   sens_ID
```

### Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| Dir | STRING | - | Rotation axis designation |
| frame_ID | INT | - | Coordinate system for axis |
| grnod_ID | INT | - | Node group ID |
| Vxt, Vyt, Vzt | FLOAT | 0.0 | Translation velocity components |
| VR | FLOAT | 0.0 | Angular velocity (rad/s) |
| Tstart | FLOAT | 0.0 | Start time |
| sens_ID | INT | 0 | Sensor ID |

### Example
```
/INIVEL/AXIS/3
Wheel_Rotation
         Z        10        100
               0.0               0.0               0.0            50.0
               0.0         0
```

---

## 10. INIVEL/FVM - Initial Velocity for FVM

Defines initial velocities for Fluid-Volume Method elements.

### Syntax
```
/INIVEL/FVM/inivel_ID
inivel_title
#                Vx                 Vy                 Vz grbric_ID  grqd_ID grtria_ID  skew_ID
#           Tstart   sens_ID
```

### Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| Vx, Vy, Vz | FLOAT | 0.0 | Velocity components |
| grbric_ID | INT | 0 | Brick group ID |
| grqd_ID | INT | 0 | Quad group ID |
| grtria_ID | INT | 0 | Tria group ID |
| skew_ID | INT | 0 | Local coordinate system |

---

## 11. INISTA - Initial State

Loads initial state from a previous analysis restart file.

### Syntax
```
/INISTA
filename.sty
#    Ibal   IoutyyFMT  Ioutynnn
```

### Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| filename | STRING | - | Path to .sty restart file |
| Ibal | INT | 1 | Initial balance flag |
| IoutyyFMT | INT | 1 | File format (1=Format 51, 2=Format 44) |
| Ioutynnn | INT | 1 | Filename format |

### Ibal Options

| Value | Description |
|-------|-------------|
| 0 | Default (→1) |
| 1 | No nodal balance |
| 2 | Nodal balance |
| 3 | Shell local frame balance |

### Example
```
/INISTA
output_0000.sty
         1         1         1
```

---

## 12. LOAD/PCYL - Cylindrical Pressure Load

Applies pressure in cylindrical coordinates as function of radius and time.

### Syntax
```
/LOAD/PCYL/pcyl_ID
pcyl_title
#surf_ID sens_ID  frame_ID
#tableID                   Ascale_r          Ascale_t          Fscale_y
```

### Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| surf_ID | INT | - | Surface ID |
| sens_ID | INT | 0 | Sensor ID |
| frame_ID | INT | - | Cylindrical coordinate frame |
| tableID | INT | - | Table ID for R-T relationship |
| Ascale_r | FLOAT | 1.0 | Radius scale factor |
| Ascale_t | FLOAT | 1.0 | Time scale factor |
| Fscale_y | FLOAT | 1.0 | Pressure scale factor |

---

## Load Type Summary

| Keyword | Application | Time-Dependent |
|---------|-------------|----------------|
| /BCS | Fixed constraints | No |
| /IMPDISP | Prescribed displacement | Yes |
| /IMPVEL | Prescribed velocity | Yes |
| /IMPACC | Prescribed acceleration | Yes |
| /CLOAD | Concentrated force/moment | Yes |
| /PLOAD | Surface pressure | Yes |
| /GRAV | Gravity/body force | Yes |
| /INIVEL | Initial velocity | No (initial) |
| /INISTA | Restart state | No (initial) |

---

## Common Features

### Time-Dependent Loading
All load keywords support:
- Function reference (funct_ID)
- Scale factors (Ascale_x for time, Fscale_y for magnitude)
- Start/stop time control

### Coordinate Systems
- Global (default)
- Skew (local Cartesian)
- Cylindrical (Icoor=1)

### Sensor Integration
- Sensor_ID enables event-triggered loading
- Coordinates with /SENSOR definitions

---

## Example: Complete Load Case

```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
# Fixed support at base
/BCS/1
Fixed_Base
 111 111        0        10

# Gravity load
/GRAV/1
Gravity
       100        Z         0        0         0                         1.0             -9.81

# Impact velocity
/INIVEL/TRA/1
Impact_Velocity
              -5.0               0.0               0.0        20         0
               0.0         0

# Time-varying pressure
/PLOAD/1
Blast_Pressure
        50       200         0         0         1         1               1.0           100000.0
```

---

## References

- Source files: `hm_cfg_files/config/CFG/radioss2025/LOADS/`
- Starter source: `starter/source/loads/`
