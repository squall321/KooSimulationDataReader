# OpenRadioss Keyword Manual - Thermal Analysis

## Overview

This document describes thermal analysis keywords in OpenRadioss. These keywords define heat transfer properties, thermal boundary conditions, initial temperatures, and radiation/convection loads for coupled thermo-mechanical analysis.

---

## 1. HEAT/MAT - Material Heat Properties

Defines thermal properties for materials in heat transfer analysis.

### Syntax
```
/HEAT/MAT/mat_ID
#                 T0             RHO0_CP                  AS                  BS
#                 T1                  AL                  BL               EFRAC
```

### Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| T0 | FLOAT | - | Initial temperature (K) |
| RHO0_CP | FLOAT | - | Specific heat per unit volume (rho * Cp) |
| AS | FLOAT | - | Thermal conductivity coefficient A for solid |
| BS | FLOAT | - | Thermal conductivity coefficient B for solid |
| T1 | FLOAT | 1e30 | Transition temperature solid/liquid (K) |
| AL | FLOAT | - | Thermal conductivity coefficient A for liquid |
| BL | FLOAT | - | Thermal conductivity coefficient B for liquid |
| EFRAC | FLOAT | 1.0 | Fraction of strain energy converted to heat |

### Thermal Conductivity Model
```
For T < T1 (Solid):
  k(T) = AS + BS * T

For T >= T1 (Liquid):
  k(T) = AL + BL * T
```

### Heat Generation from Plastic Work
```
Q_plastic = EFRAC * sigma * d_eps_p
```

### Example
```
/HEAT/MAT/10
#                 T0             RHO0_CP                  AS                  BS
               293.0           3.5e+06               45.0                0.0
#                 T1                  AL                  BL               EFRAC
              1800.0               30.0                0.0                0.9
```

---

## 2. INITEMP - Initial Temperature

Defines initial temperature distribution for nodes.

### Syntax
```
/INITEMP/initemp_ID
initemp_title
#                 T0   grnd_ID  fld_type
```

### Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| T0 | FLOAT | - | Initial temperature (K) |
| grnd_ID | INT | - | Node group ID |
| fld_type | INT | 0 | Temperature distribution type |

### fld_type Options

| Value | Description |
|-------|-------------|
| 0 | Uniform temperature on node group |
| 1 | Node-by-node temperature (with subobject) |

### Example - Uniform Temperature
```
/INITEMP/1
Initial_Room_Temperature
#                 T0   grnd_ID  fld_type
               293.0       100         0
```

### Example - Variable Temperature
```
/INITEMP/2
Temperature_Gradient
#                 T0   grnd_ID  fld_type
               300.0       200         1
# Additional node-temperature pairs follow
```

---

## 3. IMPTEMP - Imposed Temperature

Applies time-dependent temperature boundary condition.

### Syntax
```
/IMPTEMP/imptemp_ID
imptemp_title
# func_IDT sensor_ID  grnod_ID
#           Ascale_x            Fscale_y             T_start              T_stop
```

### Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| func_IDT | INT | - | Temperature function ID (time curve) |
| sensor_ID | INT | 0 | Sensor ID for triggering |
| grnod_ID | INT | - | Node group ID |
| Ascale_x | FLOAT | 1.0 | Time scale factor |
| Fscale_y | FLOAT | 1.0 | Temperature scale factor (K) |
| T_start | FLOAT | 0.0 | Start time |
| T_stop | FLOAT | 1e30 | Stop time |

### Example
```
/IMPTEMP/1
Heating_Cycle
#  func_IDT sensor_ID  grnod_ID
        50         0       100
#           Ascale_x            Fscale_y             T_start              T_stop
               1.0               1.0                 0.0              1000.0

/FUNCT/50
Temperature_Ramp
#                  X                   Y
                 0.0               293.0
               100.0               500.0
               200.0               800.0
               500.0               800.0
              1000.0               293.0
```

---

## 4. IMPFLUX - Imposed Heat Flux

Applies time-dependent heat flux on surfaces or volumes.

### Syntax
```
/IMPFLUX/impflux_ID
impflux_title
#  SURF_ID  FUNCT_ID SENSOR_ID GRBRIC_ID
#             ASCALE              FSCALE              TSTART               TSTOP
```

### Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| SURF_ID | INT | 0 | Surface ID for surface heat flux |
| FUNCT_ID | INT | - | Heat flux function ID |
| SENSOR_ID | INT | 0 | Sensor ID for triggering |
| GRBRIC_ID | INT | 0 | Brick group ID for volumetric heat generation |
| ASCALE | FLOAT | 1.0 | Time scale factor |
| FSCALE | FLOAT | 1.0 | Heat flux scale factor |
| TSTART | FLOAT | 0.0 | Start time |
| TSTOP | FLOAT | 1e30 | Stop time |

### Heat Flux Types
- **Surface flux**: When SURF_ID is specified (W/m^2)
- **Volumetric heat generation**: When GRBRIC_ID is specified (W/m^3)

### Example - Surface Heat Flux
```
/IMPFLUX/1
Laser_Heating
#  SURF_ID  FUNCT_ID SENSOR_ID GRBRIC_ID
       100        60         0         0
#             ASCALE              FSCALE              TSTART               TSTOP
               1.0           50000.0                 0.0               10.0
```

### Example - Volumetric Heat Generation
```
/IMPFLUX/2
Internal_Heat_Source
#  SURF_ID  FUNCT_ID SENSOR_ID GRBRIC_ID
         0        61         0       200
#             ASCALE              FSCALE              TSTART               TSTOP
               1.0          100000.0                 0.0              100.0
```

---

## 5. CONVEC - Convection Boundary Condition

Applies convective heat transfer boundary condition.

### Syntax
```
/CONVEC/convec_ID
convec_title
#  SURF_ID  FUNCT_ID SENSOR_ID
#             ASCALE              FSCALE              TSTART               TSTOP                   H
```

### Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| SURF_ID | INT | - | Surface ID |
| FUNCT_ID | INT | - | Ambient temperature function ID |
| SENSOR_ID | INT | 0 | Sensor ID |
| ASCALE | FLOAT | 1.0 | Time scale factor |
| FSCALE | FLOAT | 1.0 | Temperature scale factor |
| TSTART | FLOAT | 0.0 | Start time |
| TSTOP | FLOAT | 1e30 | Stop time |
| H | FLOAT | - | Heat transfer coefficient (W/m^2·K) |

### Newton's Law of Cooling
```
q = H * (T_surface - T_ambient)
```

### Example
```
/CONVEC/1
Natural_Convection
#  SURF_ID  FUNCT_ID SENSOR_ID
       150        70         0
#             ASCALE              FSCALE              TSTART               TSTOP                   H
               1.0               1.0                 0.0              1e30                  25.0

/FUNCT/70
Ambient_Temperature
#                  X                   Y
                 0.0               293.0
              1000.0               293.0
```

---

## 6. RADIATION - Radiation Boundary Condition

Applies radiative heat transfer boundary condition.

### Syntax
```
/RADIATION/radiation_ID
radiation_title
#  SURF_ID  FUNCT_ID SENSOR_ID
#             ASCALE              FSCALE              TSTART               TSTOP                   E
```

### Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| SURF_ID | INT | - | Surface ID |
| FUNCT_ID | INT | - | Wall temperature function ID |
| SENSOR_ID | INT | 0 | Sensor ID |
| ASCALE | FLOAT | 1.0 | Time scale factor |
| FSCALE | FLOAT | 1.0 | Temperature scale factor |
| TSTART | FLOAT | 0.0 | Start time |
| TSTOP | FLOAT | 1e30 | Stop time |
| E | FLOAT | - | Surface emissivity (0-1) |

### Stefan-Boltzmann Law
```
q = E * sigma * (T_surface^4 - T_wall^4)

where sigma = 5.67e-8 W/m^2·K^4 (Stefan-Boltzmann constant)
```

### Example
```
/RADIATION/1
Furnace_Radiation
#  SURF_ID  FUNCT_ID SENSOR_ID
       200        80         0
#             ASCALE              FSCALE              TSTART               TSTOP                   E
               1.0               1.0                 0.0              1e30                   0.8

/FUNCT/80
Wall_Temperature
#                  X                   Y
                 0.0              1200.0
               500.0              1200.0
              1000.0               800.0
```

---

## 7. Interface Heat Transfer (TYPE7/TYPE21 with Ithe)

Contact interfaces can include thermal coupling.

### TYPE7 Thermal Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| Ithe | INT | Heat transfer flag (0=off, 1=on) |
| Kthe | FLOAT | Contact thermal conductance |

### TYPE21 Thermal Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| Ithe | INT | Heat transfer flag (0-2) |
| Kthe | FLOAT | Heat exchange coefficient |
| fct_IDK | INT | Function for heat vs pressure |
| AscaleK | FLOAT | Scale for Kthe function |
| Tint | FLOAT | Interface temperature |
| Ithe_form | INT | Heat formulation flag |
| Frad | FLOAT | Radiation factor |
| Drad | FLOAT | Maximum radiation distance |
| Fheat | FLOAT | Frictional heating factor |

### Ithe Options (TYPE21)

| Value | Description |
|-------|-------------|
| 0 | No heat transfer |
| 1 | Conduction only |
| 2 | Conduction + frictional heating |

### Example
```
/INTER/TYPE21/1
Hot_Forming_Contact
#  ... standard contact parameters ...
#    Kthe  fct_IDK   AscaleK      Tint Ithe_form
     100.0         0       1.0     500.0         0
#    Frad      Drad     Fheat
       0.0       0.0       0.5
```

---

## 8. THERM_STRESS Control

Controls thermal stress analysis options.

### Related Controls

```
/THERM_STRESS/1
# Enables thermal stress calculation
# Temperature changes cause thermal expansion

/DEF_THERM
# Default thermal parameters
```

### Thermal Expansion
Requires material thermal expansion coefficient (alpha):
```
eps_thermal = alpha * (T - T_ref)
```

---

## Thermal Analysis Workflow

### 1. Define Heat Properties
```
/HEAT/MAT/mat_ID
# For each material requiring thermal analysis
```

### 2. Set Initial Conditions
```
/INITEMP/1
# Initial temperature distribution
```

### 3. Apply Boundary Conditions
Choose appropriate combination:
- IMPTEMP: Known temperature boundaries
- IMPFLUX: Known heat flux
- CONVEC: Convective cooling/heating
- RADIATION: Radiative heat exchange

### 4. Enable Contact Heat Transfer
```
/INTER/TYPE7 or /INTER/TYPE21 with Ithe > 0
```

---

## Thermal Material Properties

### Common Material Properties

| Material | k (W/m·K) | rho*Cp (J/m^3·K) | alpha (1/K) |
|----------|-----------|------------------|-------------|
| Steel | 45-50 | 3.5e6 | 1.2e-5 |
| Aluminum | 200-240 | 2.4e6 | 2.3e-5 |
| Copper | 380-400 | 3.4e6 | 1.7e-5 |
| Titanium | 15-20 | 2.3e6 | 8.6e-6 |
| Polymer | 0.1-0.5 | 1.5e6 | 5e-5 |

### Heat Transfer Coefficients

| Condition | h (W/m^2·K) |
|-----------|-------------|
| Natural convection (air) | 5-25 |
| Forced convection (air) | 10-500 |
| Forced convection (water) | 100-15000 |
| Boiling water | 3000-100000 |

### Surface Emissivity

| Surface | Emissivity |
|---------|------------|
| Polished metal | 0.02-0.1 |
| Oxidized metal | 0.3-0.9 |
| Painted surface | 0.85-0.95 |
| Black body | 1.0 |

---

## Example: Complete Thermal Analysis

```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
# Thermal material properties
/HEAT/MAT/10
#                 T0             RHO0_CP                  AS                  BS
               293.0           3.5e+06               45.0                0.0
#                 T1                  AL                  BL               EFRAC
              1800.0               30.0                0.0                0.9

# Initial temperature
/INITEMP/1
Room_Temperature
#                 T0   grnd_ID  fld_type
               293.0       100         0

# Imposed temperature boundary (hot surface)
/IMPTEMP/1
Hot_Die_Surface
#  func_IDT sensor_ID  grnod_ID
        50         0       200
#           Ascale_x            Fscale_y             T_start              T_stop
               1.0               1.0                 0.0              100.0

# Convection boundary (cooling)
/CONVEC/1
Air_Cooling
#  SURF_ID  FUNCT_ID SENSOR_ID
       300        60         0
#             ASCALE              FSCALE              TSTART               TSTOP                   H
               1.0               1.0                 0.0              1e30                  50.0

# Radiation boundary
/RADIATION/1
Furnace_Walls
#  SURF_ID  FUNCT_ID SENSOR_ID
       400        70         0
#             ASCALE              FSCALE              TSTART               TSTOP                   E
               1.0               1.0                 0.0              1e30                   0.8

# Temperature functions
/FUNCT/50
Die_Temperature
#                  X                   Y
                 0.0               773.0
               100.0               773.0

/FUNCT/60
Ambient_Temperature
#                  X                   Y
                 0.0               293.0
               100.0               293.0

/FUNCT/70
Wall_Temperature
#                  X                   Y
                 0.0              1073.0
               100.0              1073.0
```

---

## Best Practices

### Mesh Considerations
1. **Element size**: Fine mesh near thermal gradients
2. **Through-thickness**: Multiple elements for shell thermal analysis
3. **Contact regions**: Adequate refinement for contact heat transfer

### Time Step
1. Thermal time step may differ from mechanical
2. Use /DT/THERM for explicit thermal control
3. Consider thermal mass for stability

### Convergence
1. Start with simple boundary conditions
2. Gradually add complexity
3. Verify energy balance

### Validation
1. Check temperature ranges are physical
2. Verify heat flux directions
3. Compare with analytical solutions when possible

---

## Coupled Analysis Types

### Thermo-Mechanical
- Temperature affects material properties
- Thermal expansion generates stress
- Plastic work generates heat (EFRAC)

### Thermo-Forming
- Hot forming with temperature-dependent yield
- Die contact with heat transfer
- Cooling during forming

### Welding Simulation
- Moving heat source (IMPFLUX)
- Temperature-dependent properties
- Phase transformation effects

---

## References

- Source files: `hm_cfg_files/config/CFG/radioss*/LOADS/`
- Engine source: `engine/source/thermal/`
- Starter source: `starter/source/thermal/`

