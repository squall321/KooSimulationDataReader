# OpenRadioss Keyword Manual - Seatbelt Components

## Overview

This document describes seatbelt modeling keywords in OpenRadioss including retractors and slip rings for occupant restraint system simulation.

---

## Available Seatbelt Keywords

| Keyword | Description |
|---------|-------------|
| /RETRACTOR/SPRING | Seatbelt retractor with spring mechanism |
| /SLIPRING/SPRING | Belt slip ring (D-ring) connection |
| /SLIPRING/SHELL | Shell-based slip ring |

---

## 1. RETRACTOR/SPRING - Seatbelt Retractor

The /RETRACTOR/SPRING keyword defines a seatbelt retractor mechanism that controls belt payout and pretensioning.

### Syntax
```
/RETRACTOR/SPRING/retractor_ID
retractor_title
#    EL_ID   Node_ID           Elem_size
# Sens_ID1             Pullout   Fct_ID1   Fct_ID2             Yscale1             Xscale1
# Sens_ID2  Tens_typ               Force   Fct_ID3             Yscale2             Xscale2
```

### Parameters - Card 1

| Parameter | Type | Description |
|-----------|------|-------------|
| retractor_ID | INT | Unique retractor identifier |
| retractor_title | STRING | Retractor description |

### Parameters - Card 2

| Parameter | Type | Description |
|-----------|------|-------------|
| EL_ID | INT | First connected element (belt element) |
| Node_ID | INT | Anchorage node identifier |
| Elem_size | FLOAT | Belt element size |

### Parameters - Card 3 (Loading/Unloading)

| Parameter | Type | Description |
|-----------|------|-------------|
| Sens_ID1 | INT | Sensor for activation |
| Pullout | FLOAT | Maximum belt pullout length |
| Fct_ID1 | INT | Loading function (force vs. displacement) |
| Fct_ID2 | INT | Unloading function |
| Yscale1 | FLOAT | Force scale factor |
| Xscale1 | FLOAT | Displacement scale factor |

### Parameters - Card 4 (Pretensioner)

| Parameter | Type | Description |
|-----------|------|-------------|
| Sens_ID2 | INT | Pretensioner activation sensor |
| Tens_typ | INT | Pretensioner type |
| Force | FLOAT | Pretensioner force or scale factor |
| Fct_ID3 | INT | Pretensioner pull-in function |
| Yscale2 | FLOAT | Pretensioner force scale |
| Xscale2 | FLOAT | Pretensioner displacement scale |

### Tens_typ Values

| Value | Description |
|-------|-------------|
| 0 | No pretensioner |
| 1 | Pyrotechnic pretensioner |
| 2 | Mechanical pretensioner |

### Example
```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
/RETRACTOR/SPRING/1
Driver_Seatbelt_Retractor
#    EL_ID   Node_ID           Elem_size
      1000      2000              10.0
# Sens_ID1             Pullout   Fct_ID1   Fct_ID2             Yscale1             Xscale1
         1             300.0         1         2               1.0              1.0
# Sens_ID2  Tens_typ               Force   Fct_ID3             Yscale2             Xscale2
         2         1            2500.0         3               1.0              1.0
```

---

## 2. SLIPRING/SPRING - Belt Slip Ring

The /SLIPRING/SPRING keyword defines a slip ring (D-ring) where the belt can slide with friction.

### Syntax
```
/SLIPRING/SPRING/slipring_ID
slipring_title
#   El1_ID    El2_ID   Node_ID  Node_ID2   Sens_ID Flow_flag                   A           Ed_factor
#  Fct_ID1   Fct_ID2               Fricd             Xscale1             Yscale2             Xscale2
#  Fct_ID3   Fct_ID4               Frics             Xscale3             Yscale4             Xscale4
```

### Parameters - Card 1

| Parameter | Type | Description |
|-----------|------|-------------|
| slipring_ID | INT | Unique slip ring identifier |
| slipring_title | STRING | Slip ring description |

### Parameters - Card 2

| Parameter | Type | Description |
|-----------|------|-------------|
| El1_ID | INT | First connected belt element |
| El2_ID | INT | Second connected belt element |
| Node_ID | INT | Anchorage node |
| Node_ID2 | INT | Direction node (optional) |
| Sens_ID | INT | Sensor identifier |
| Flow_flag | INT | Belt flow direction flag |
| A | FLOAT | Orientation friction factor |
| Ed_factor | FLOAT | Exponential decay factor |

### Parameters - Card 3 (Dynamic Friction)

| Parameter | Type | Description |
|-----------|------|-------------|
| Fct_ID1 | INT | Dynamic friction vs. time function |
| Fct_ID2 | INT | Dynamic friction vs. normal force function |
| Fricd | FLOAT | Dynamic friction coefficient |
| Xscale1 | FLOAT | Time scale factor |
| Yscale2 | FLOAT | Force scale factor |
| Xscale2 | FLOAT | Normal force scale factor |

### Parameters - Card 4 (Static Friction)

| Parameter | Type | Description |
|-----------|------|-------------|
| Fct_ID3 | INT | Static friction vs. time function |
| Fct_ID4 | INT | Static friction vs. normal force function |
| Frics | FLOAT | Static friction coefficient |
| Xscale3 | FLOAT | Time scale factor |
| Yscale4 | FLOAT | Force scale factor |
| Xscale4 | FLOAT | Normal force scale factor |

### Flow_flag Values

| Value | Description |
|-------|-------------|
| 0 | Bidirectional flow |
| 1 | Flow from El1 to El2 only |
| -1 | Flow from El2 to El1 only |

### Example
```
#---1----|----2----|----3----|----5----|----5----|----6----|----7----|----8----|
/SLIPRING/SPRING/1
Shoulder_D_Ring
#   El1_ID    El2_ID   Node_ID  Node_ID2   Sens_ID Flow_flag                   A           Ed_factor
      1001      1002      3000      3001         0         0                0.0              0.01
#  Fct_ID1   Fct_ID2               Fricd             Xscale1             Yscale2             Xscale2
         0         0                0.15               1.0               1.0               1.0
#  Fct_ID3   Fct_ID4               Frics             Xscale3             Yscale4             Xscale4
         0         0                0.20               1.0               1.0               1.0
```

---

## 3. SLIPRING/SHELL - Shell-Based Slip Ring

The /SLIPRING/SHELL keyword defines a slip ring using shell element representation.

### Syntax
Similar to /SLIPRING/SPRING but connects to shell elements.

---

## Friction Modeling

### Static vs. Dynamic Friction

The slip ring uses both static and dynamic friction:
- **Static friction (Frics)**: Applied when belt is not sliding
- **Dynamic friction (Fricd)**: Applied during belt motion
- Transition controlled by Ed_factor

### Friction Formula

```
mu = Fricd + (Frics - Fricd) * exp(-Ed_factor * velocity)
```

### Function-Based Friction

Friction can vary with:
- Time (crash event progression)
- Normal force (belt tension dependent)

---

## Retractor Behavior

### Loading Phase

1. Belt pays out from retractor
2. Force follows loading function (Fct_ID1)
3. Controlled by spring stiffness

### Unloading Phase

1. Belt retracts when load decreases
2. Force follows unloading function (Fct_ID2)
3. Hysteresis behavior captured

### Pretensioner Activation

1. Sensor triggers pretensioner
2. Belt rapidly pulled in (Fct_ID3)
3. Force limited by pretensioner capacity

---

## Best Practices

### Retractor Modeling

1. **Element size**: Match belt element length
2. **Pullout limit**: Set realistic maximum extension
3. **Loading curve**: Use test data for accuracy
4. **Pretensioner timing**: Align with airbag deployment

### Slip Ring Modeling

1. **Friction coefficients**: Use material test data
2. **Static > Dynamic**: Ensure Frics ≥ Fricd
3. **Exponential decay**: Typical range 0.001-0.1
4. **Flow direction**: Match belt routing

### Sensor Setup

| Component | Sensor Type | Trigger |
|-----------|-------------|---------|
| Retractor lock | Acceleration | Vehicle deceleration |
| Pretensioner | Time or sensor | Crash detection |
| Load limiter | Force | Belt tension threshold |

---

## Example - Complete Seatbelt System

```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
# Seatbelt retractor with pretensioner
/RETRACTOR/SPRING/1
Driver_Belt_Retractor
      1000      5000              15.0
         1             350.0         1         2               1.0               1.0
         2         1            3000.0         3               1.0               1.0

# Shoulder D-ring slip ring
/SLIPRING/SPRING/1
Shoulder_Anchor_Ring
      1010      1020      5100      5101         0         0                0.0               0.01
         0        10                0.12               1.0               1.0            1000.0
         0        11                0.18               1.0               1.0            1000.0

# Lap belt anchor slip ring
/SLIPRING/SPRING/2
Lap_Belt_Anchor
      1030      1040      5200         0         0         0                0.0               0.01
         0         0                0.10               1.0               1.0               1.0
         0         0                0.15               1.0               1.0               1.0

# Retractor locking sensor (acceleration-based)
/SENSOR/ACCE/1
Retractor_Lock_Sensor
      5000         0         1
                3.0
                0.0               0.0              -1.0

# Pretensioner activation sensor (time-based)
/SENSOR/TIME/2
Pretensioner_Trigger
              0.015
```

---

## Troubleshooting

### Common Issues

| Issue | Cause | Solution |
|-------|-------|----------|
| Belt passes through D-ring | Friction too low | Increase Fricd/Frics |
| Excessive belt slack | Pullout too large | Reduce Pullout value |
| Pretensioner not firing | Sensor not triggered | Check sensor setup |
| Unstable belt behavior | Ed_factor too high | Reduce decay factor |
| No load limiting | No unloading function | Define Fct_ID2 |

### Validation Checks

1. Belt payout matches sled test
2. Pretensioner timing correct
3. Load limiter force level appropriate
4. Friction behavior realistic

---

## References

- RETRACTOR source: `hm_cfg_files/config/CFG/radioss*/SEATBELTS/retractor.cfg`
- SLIPRING source: `hm_cfg_files/config/CFG/radioss*/SEATBELTS/slipring.cfg`
- Starter source: `starter/source/model/seatbelt/`

