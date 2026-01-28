# OpenRadioss Keyword Manual - Sensors and Gauges

## Overview

This document describes sensor and gauge definitions in OpenRadioss. Sensors trigger events based on physical criteria, while gauges and accelerometers measure quantities at specific locations.

---

## 1. SENSOR - Event Sensors

Sensors detect physical conditions and trigger events such as load activation, contact changes, or element deletion.

### General Syntax
```
/SENSOR/type/sens_ID
sens_title
# Type-specific parameters
```

### Sensor Types Overview

| Type | Keyword | Description |
|------|---------|-------------|
| 0 | TIME | Time-based activation |
| 1 | ACCE | Accelerometer-based |
| 2 | DIST | Distance between nodes |
| 3 | SENS | Sensor combination (AND/OR) |
| 4 | AND | Logical AND of sensors |
| 5 | OR | Logical OR of sensors |
| 6 | INTER | Interface-based |
| 7 | RWALL | Rigid wall contact |
| 8 | NOT | Logical NOT of sensor |
| 9 | VEL | Velocity-based |
| 10 | GAUGE | Gauge-based |
| 11 | RBODY | Rigid body |
| 12 | SECT | Section force |
| 13 | WORK | Work/energy |
| 14 | ENERGY | Energy threshold |
| 15 | DIST_SURF | Distance to surface |
| 16 | HIC | Head Injury Criterion |
| 18 | TEMP | Temperature-based |
| 19 | NIC_NIJ | Neck Injury Criteria |

---

## 2. SENSOR/TIME - Time-Based Sensor

Activates at a specified time.

### Syntax
```
/SENSOR/TIME/sens_ID
sens_title
#             Tdelay
```

### Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| Tdelay | FLOAT | 0.0 | Activation time |

### Example
```
/SENSOR/TIME/1
Activation_at_10ms
             0.010
```

---

## 3. SENSOR/ACCE - Accelerometer-Based Sensor

Activates when accelerometer reading exceeds threshold.

### Syntax
```
/SENSOR/ACCE/sens_ID
sens_title
#             Tdelay
#     Nacc
# accel_ID       dir               Tomin                Tmin
```

### Parameters - Card 1

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| Tdelay | FLOAT | 0.0 | Time delay after trigger |

### Parameters - Card 2

| Parameter | Type | Description |
|-----------|------|-------------|
| Nacc | INT | Number of accelerometers (1-6) |

### Parameters - Per Accelerometer

| Parameter | Type | Description |
|-----------|------|-------------|
| accel_ID | INT | Accelerometer identifier |
| dir | STRING | Direction (X, Y, Z, XY, YZ, ZX, XYZ) |
| Tomin | FLOAT | Minimum absolute acceleration threshold |
| Tmin | FLOAT | Minimum duration above threshold |

### Example
```
/SENSOR/ACCE/2
Crash_Detection
               0.0
         2
# accel_ID       dir               Tomin                Tmin
         1         X              20.0             0.001
         2        XY              15.0             0.002
```

---

## 4. SENSOR/DIST - Distance-Based Sensor

Activates when distance between nodes reaches threshold.

### Syntax
```
/SENSOR/DIST/sens_ID
sens_title
#             Tdelay
# node_ID1  node_ID2                Dmin                Dmax
```

### Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| Tdelay | FLOAT | 0.0 | Time delay after trigger |
| node_ID1 | INT | - | First node identifier |
| node_ID2 | INT | - | Second node identifier |
| Dmin | FLOAT | 0.0 | Minimum distance threshold |
| Dmax | FLOAT | 1e30 | Maximum distance threshold |

### Activation Condition
- Sensor activates when: `Dmin <= distance(N1,N2) <= Dmax`

### Example
```
/SENSOR/DIST/3
Contact_Detection
               0.0
       100       200              0.0             5.0
```

---

## 5. SENSOR/VEL - Velocity-Based Sensor

Activates when node velocity reaches threshold.

### Syntax
```
/SENSOR/VEL/sens_ID
sens_title
#             Tdelay
# node_ID               Vmax                Fcut
```

### Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| Tdelay | FLOAT | 0.0 | Time delay |
| node_ID | INT | - | Node identifier |
| Vmax | FLOAT | - | Maximum velocity threshold |
| Fcut | FLOAT | 0.0 | Cutoff frequency for filtering |

### Example
```
/SENSOR/VEL/4
Velocity_Trigger
               0.0
       500              10.0          1000.0
```

---

## 6. SENSOR/HIC - Head Injury Criterion Sensor

Activates when HIC value exceeds threshold.

### Syntax
```
/SENSOR/HIC/sens_ID
sens_title
#             Tdelay
#   Acc_ID                 DIR
#              Period                Value                Gravity                                Tmin
```

### Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| Tdelay | FLOAT | 0.0 | Time delay |
| Acc_ID | INT | - | Accelerometer identifier |
| DIR | STRING | R | Direction (R=resultant, X, Y, Z) |
| Period | FLOAT | 0.015 | HIC evaluation period (usually 15ms or 36ms) |
| Value | FLOAT | - | HIC threshold value |
| Gravity | FLOAT | 9.81 | Gravity value for normalization |
| Tmin | FLOAT | 0.0 | Minimum time duration |

### HIC Formula
```
HIC = max{ (t2-t1) * [(1/(t2-t1)) * integral(a(t)dt)]^2.5 }
```

### Example
```
/SENSOR/HIC/5
Head_Impact_Limit
               0.0
         1                   R
             0.015          1000.0               9.81                                   0.0
```

---

## 7. SENSOR/AND - Logical AND Sensor

Activates when all referenced sensors are active.

### Syntax
```
/SENSOR/AND/sens_ID
sens_title
#             Tdelay
#   Nsens
# sens_ID1  sens_ID2  sens_ID3 ...
```

### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| Tdelay | FLOAT | Time delay after trigger |
| Nsens | INT | Number of sensors to combine |
| sens_IDi | INT | Sensor identifiers |

### Example
```
/SENSOR/AND/10
Combined_Trigger
               0.0
         2
         1         2
```

---

## 8. SENSOR/OR - Logical OR Sensor

Activates when any referenced sensor is active.

### Syntax
```
/SENSOR/OR/sens_ID
sens_title
#             Tdelay
#   Nsens
# sens_ID1  sens_ID2  sens_ID3 ...
```

### Example
```
/SENSOR/OR/11
Either_Trigger
               0.0
         3
         1         2         3
```

---

## 9. SENSOR/NOT - Logical NOT Sensor

Activates when referenced sensor is NOT active.

### Syntax
```
/SENSOR/NOT/sens_ID
sens_title
#             Tdelay
# sens_ID_ref
```

### Example
```
/SENSOR/NOT/12
Inverse_Trigger
               0.0
         5
```

---

## 10. SENSOR/INTER - Interface-Based Sensor

Activates based on interface contact state.

### Syntax
```
/SENSOR/INTER/sens_ID
sens_title
#             Tdelay
#inter_ID                Fmin                Tmin
```

### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| inter_ID | INT | Interface identifier |
| Fmin | FLOAT | Minimum contact force threshold |
| Tmin | FLOAT | Minimum contact duration |

---

## 11. SENSOR/RWALL - Rigid Wall Sensor

Activates based on rigid wall contact.

### Syntax
```
/SENSOR/RWALL/sens_ID
sens_title
#             Tdelay
#rwall_ID                Fmin                Tmin
```

### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| rwall_ID | INT | Rigid wall identifier |
| Fmin | FLOAT | Minimum contact force |
| Tmin | FLOAT | Minimum duration |

---

## 12. SENSOR/TEMP - Temperature-Based Sensor

Activates when temperature reaches threshold.

### Syntax
```
/SENSOR/TEMP/sens_ID
sens_title
#             Tdelay
# node_ID               Tmax                Tmin_dur
```

### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| node_ID | INT | Node to monitor |
| Tmax | FLOAT | Temperature threshold |
| Tmin_dur | FLOAT | Minimum duration above threshold |

---

## 13. ACCEL - Accelerometer

Defines measurement points for acceleration output.

### Syntax
```
/ACCEL/accel_ID
accel_title
#     Node     Iskew                          Fcut
```

### Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| Node | INT | - | Node identifier for measurement |
| Iskew | INT | 0 | Local coordinate system ID |
| Fcut | FLOAT | 0.0 | Cutoff frequency for filtering |

### Example
```
/ACCEL/1
Head_CG_Accelerometer
       100         0                      1000.0
```

---

## 14. GAUGE - Measurement Gauge

Defines measurement points for various output quantities.

### Syntax - Node Definition
```
/GAUGE/gauge_ID
gauge_title
#  node_ID
```

### Syntax - Element Definition
```
/GAUGE/gauge_ID
gauge_title
#                                                   shell_ID                DIST
```

### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| node_ID | INT | Node for gauge location |
| shell_ID | INT | Shell element for gauge location |
| DIST | FLOAT | Distance from shell surface |

### Example - Node-Based
```
/GAUGE/1
Measurement_Point
       500
```

### Example - Element-Based
```
/GAUGE/2
Shell_Surface_Gauge
                                                      1001              0.0
```

---

## 15. GAUGE_SPH - SPH Gauge

Defines measurement points for SPH simulations.

### Syntax
```
/GAUGE_SPH/gauge_ID
gauge_title
# SPH-specific parameters
```

---

## 16. GAUGE_POINT - Point Gauge

Defines measurement at arbitrary point in space.

### Syntax
```
/GAUGE_POINT/gauge_ID
gauge_title
# Point coordinates and parameters
```

---

## Sensor Summary Table

| Type | Keyword | Trigger Condition |
|------|---------|-------------------|
| TIME | Time threshold | t >= Tdelay |
| ACCE | Acceleration | \|a\| >= Tomin for t >= Tmin |
| DIST | Distance | Dmin <= d <= Dmax |
| VEL | Velocity | \|v\| >= Vmax |
| HIC | Head Injury | HIC >= Value |
| AND | Multiple sensors | All sensors active |
| OR | Multiple sensors | Any sensor active |
| NOT | Single sensor | Referenced sensor inactive |
| INTER | Interface | Contact force >= Fmin |
| RWALL | Rigid wall | Wall force >= Fmin |
| TEMP | Temperature | T >= Tmax |

---

## Sensor Applications

| Application | Recommended Sensor |
|-------------|-------------------|
| Airbag deployment | ACCE, HIC |
| Seatbelt pretensioner | ACCE, DIST |
| Contact detection | DIST, INTER |
| Time-triggered events | TIME |
| Safety system activation | AND, OR combinations |
| Injury assessment | HIC, NIC_NIJ |
| Thermal events | TEMP |

---

## Time History Output

Sensors can be output using TH/SENSOR:

```
/TH/SENSOR/1
Sensor_History
DEF
1  2  3
```

---

## Example: Complete Sensor Setup

```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
# Accelerometer definition
/ACCEL/1
Sled_Accelerometer
       100         0                       600.0

# Time-based sensor
/SENSOR/TIME/1
Start_Time
             0.005

# Acceleration sensor
/SENSOR/ACCE/2
Crash_Detect
               0.0
         1
# accel_ID       dir               Tomin                Tmin
         1       XYZ              15.0             0.001

# Distance sensor
/SENSOR/DIST/3
Bumper_Contact
               0.0
       200       300              0.0            50.0

# Combined sensor (airbag trigger)
/SENSOR/AND/10
Airbag_Fire
             0.002
         2
         1         2

# HIC sensor for safety assessment
/SENSOR/HIC/20
Head_Injury_Check
               0.0
         1                   R
             0.015          1000.0               9.81                                 0.001

# Gauge for output
/GAUGE/1
Impact_Point
       500
```

---

## Best Practices

1. **Accelerometer Placement**
   - Place at center of gravity for rigid body motion
   - Use appropriate cutoff frequency for signal filtering
   - Match accelerometer skew with measurement direction

2. **Sensor Timing**
   - Use Tdelay to prevent false triggering
   - Set Tmin for duration-based criteria
   - Consider signal filtering effects

3. **Threshold Selection**
   - Base thresholds on expected physical behavior
   - Include safety margin for variability
   - Validate with test data when available

4. **Combined Sensors**
   - Use AND for multiple conditions required
   - Use OR for alternative triggering paths
   - Chain sensors for complex logic

5. **Output**
   - Include sensors in TH output for debugging
   - Monitor activation status in animation
   - Verify sensor behavior in post-processing

---

## References

- Source files: `hm_cfg_files/config/CFG/radioss2025/SENSOR/`
- Source files: `hm_cfg_files/config/CFG/radioss2025/ACCEL/`
- Starter source: `starter/source/sensors/`
