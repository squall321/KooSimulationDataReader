# OpenRadioss Keyword Manual - Airbag (Monitored Volume)

## Overview

This document describes airbag and monitored volume definitions in OpenRadioss. These keywords define enclosed volumes with gas dynamics for airbag deployment, fuel tank sloshing, and other applications requiring uniform pressure assumptions.

---

## Monitored Volume Keyword Syntax

```
/MONVOL/type/monvol_ID
monvol_title
[monitored volume parameters...]
```

---

## 1. MONVOL/AIRBAG - Uniform Pressure Airbag

Defines a monitored volume with uniform pressure assumption for airbag simulation.

### Syntax
```
/MONVOL/AIRBAG/monvol_ID
monvol_title
#  surf_ID   Nb_inj  Nb_vent   Iequi       Ittf   Iflow1                 Pext
#   Scal_T    Scal_P    Scal_S    Scal_A    Scal_D         MU
#   GAMMAi       cpa       cpb       cpc                          T_Initial
# For each injector (Nb_inj times):
#   gamma_i     cpa_i     cpb_i     cpc_i
#    Imass   fct_Mflow   Smass    Itemp   fct_Temp   Stemp   Isensor     Ijet
#       N1        N2        N3
#      IPt   IPtheta    IPdel
# For each vent hole (Nb_vent times):
#  surfvent_ID    Avent     Bvent    Tstart     Ivent     IvdP   fct_vdP
#     dPdef    dtPdef     Iporp  fct_porp     Ipora  fct_porA
```

### Parameters - Card 1 (General)

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| surf_ID | INT | - | Surface ID enclosing the volume |
| Nb_inj | INT | 0 | Number of gas injectors |
| Nb_vent | INT | 0 | Number of vent holes |
| Iequi | INT | 0 | Equilibrium flag |
| Ittf | INT | 0 | Time-to-fire computation flag |
| Iflow1 | INT | 0 | Initial flow flag |
| Pext | FLOAT | 0.0 | External pressure |

### Parameters - Card 2 (Scale Factors)

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| Scal_T | FLOAT | 1.0 | Temperature scale factor |
| Scal_P | FLOAT | 1.0 | Pressure scale factor |
| Scal_S | FLOAT | 1.0 | Surface scale factor |
| Scal_A | FLOAT | 1.0 | Area scale factor |
| Scal_D | FLOAT | 1.0 | Density scale factor |
| MU | FLOAT | 0.0 | Gas viscosity |

### Parameters - Card 3 (Initial Gas Properties)

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| GAMMAi | FLOAT | 1.4 | Heat capacity ratio (Cp/Cv) |
| cpa | FLOAT | - | Specific heat coefficient a |
| cpb | FLOAT | - | Specific heat coefficient b |
| cpc | FLOAT | - | Specific heat coefficient c |
| T_Initial | FLOAT | 293.0 | Initial temperature (K) |

### Specific Heat Model
```
Cp(T) = cpa + cpb*T + cpc*T^2
```

### Injector Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| gamma_i | FLOAT | - | Heat capacity ratio for injected gas |
| cpa_i, cpb_i, cpc_i | FLOAT | - | Specific heat coefficients |
| Imass | INT | 0 | Mass flow input type |
| fct_Mflow | INT | - | Mass flow rate function ID |
| Smass | FLOAT | 1.0 | Mass flow scale factor |
| Itemp | INT | 0 | Temperature input type |
| fct_Temp | INT | - | Temperature function ID |
| Stemp | FLOAT | 1.0 | Temperature scale factor |
| Isensor | INT | 0 | Sensor ID for triggering |
| Ijet | INT | 0 | Jetting flag |
| N1, N2, N3 | INT | - | Jetting direction node IDs |
| IPt | INT | 0 | Pressure table ID |
| IPtheta | INT | 0 | Angle table ID |
| IPdel | INT | 0 | Deactivation table ID |

### Imass Options

| Value | Description |
|-------|-------------|
| 0 | Mass flow rate input |
| 1 | Mass input (cumulative) |
| 2 | Tank test data |

### Vent Hole Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| surfvent_ID | INT | - | Vent hole surface ID |
| Avent | FLOAT | - | Vent area coefficient A |
| Bvent | FLOAT | - | Vent area coefficient B |
| Tstart | FLOAT | 0.0 | Vent activation time |
| Ivent | INT | 0 | Vent type flag |
| IvdP | INT | 0 | Pressure-dependent area flag |
| fct_vdP | INT | - | Function for pressure-area |
| dPdef | FLOAT | 0.0 | Pressure difference threshold |
| dtPdef | FLOAT | 0.0 | Time for pressure threshold |
| Iporp | INT | 0 | Porosity pressure flag |
| fct_porp | INT | - | Porosity vs pressure function |
| Ipora | INT | 0 | Porosity area flag |
| fct_porA | INT | - | Porosity vs area function |

### Vent Area Calculation
```
A_vent = Avent * (1 + Bvent * dP)
where dP = P_internal - P_external
```

### Example
```
/MONVOL/AIRBAG/1
Driver_Airbag
#  surf_ID   Nb_inj  Nb_vent   Iequi       Ittf   Iflow1                 Pext
       100        1        2         0         0         0           101325.0
#   Scal_T    Scal_P    Scal_S    Scal_A    Scal_D         MU
       1.0       1.0       1.0       1.0       1.0              0.0
#   GAMMAi       cpa       cpb       cpc                          T_Initial
       1.4    1005.0       0.0       0.0                              293.0
# Injector 1
#   gamma_i     cpa_i     cpb_i     cpc_i
       1.28    1200.0       0.0       0.0
#    Imass   fct_Mflow   Smass    Itemp   fct_Temp   Stemp   Isensor     Ijet
         0          10     1.0         0          11     1.0         1         0
#       N1        N2        N3
         0         0         0
#      IPt   IPtheta    IPdel
         0         0         0
# Vent hole 1
#  surfvent_ID    Avent     Bvent    Tstart     Ivent     IvdP   fct_vdP
           101     0.8       0.0      0.03         0         0         0
#     dPdef    dtPdef     Iporp  fct_porp     Ipora  fct_porA
       0.0       0.0         0         0         0         0
# Vent hole 2
           102     0.6       0.0      0.05         0         0         0
       0.0       0.0         0         0         0         0
```

---

## 2. MONVOL/AIRBAG1 - Enhanced Airbag with Material

Enhanced uniform pressure airbag with material reference and additional features.

### Syntax
```
/MONVOL/AIRBAG1/monvol_ID
monvol_title
#   mat_ID  sens_ID                 Pext            T_Initial
#  surf_ID                hconv
# Injector section (INJECT subobjects)
# Vent hole section (VENT subobjects)
# Porous surface section (POROUS subobjects)
```

### Parameters - Card 1

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| mat_ID | INT | - | Material ID for gas properties |
| sens_ID | INT | 0 | Sensor ID for activation |
| Pext | FLOAT | 101325.0 | External pressure |
| T_Initial | FLOAT | 293.0 | Initial temperature |

### Parameters - Card 2

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| surf_ID | INT | - | Enclosing surface ID |
| hconv | FLOAT | 0.0 | Heat convection coefficient |

### Injector Subobject Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| mat_inj | INT | Injector gas material ID |
| sens_inj | INT | Injector activation sensor |
| fct_Minj | INT | Mass flow function ID |
| Fscale_Minj | FLOAT | Mass flow scale factor |
| fct_Tinj | INT | Temperature function ID |
| Fscale_Tinj | FLOAT | Temperature scale factor |
| node1, node2, node3 | INT | Jet direction nodes |
| Ijet | INT | Jetting model flag |

### Vent Hole Subobject Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| surf_vent | INT | Vent surface ID |
| Avent | FLOAT | Vent area coefficient |
| Bvent | FLOAT | Pressure coefficient |
| tstart_vent | FLOAT | Activation start time |
| tstop_vent | FLOAT | Deactivation stop time |
| fct_Avent | INT | Area function ID |
| Fscale_Avent | FLOAT | Area scale factor |

### Porous Surface Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| surf_ps | INT | Porous surface ID |
| iform_ps | INT | Porosity formulation |
| iblockage_ps | INT | Blockage model flag |
| tstart_ps | FLOAT | Activation start time |
| tstop_ps | FLOAT | Deactivation time |
| dpdef_ps | FLOAT | Pressure difference threshold |
| dtpdef_ps | FLOAT | Time constant |

### Example
```
/MONVOL/AIRBAG1/2
Passenger_Airbag_Enhanced
#   mat_ID  sens_ID                 Pext            T_Initial
        50        1           101325.0               293.0
#  surf_ID                hconv
       200                  10.0
# Injector definition follows with INJECT subobjects
# Vent hole definition follows with VENT subobjects
```

---

## 3. MONVOL/COMMU1 - Communicating Volumes

Defines communication between multiple monitored volumes.

### Syntax
```
/MONVOL/COMMU1/monvol_ID
monvol_title
#  N_monvol
#  monvol_ID1  monvol_ID2  ...
#  surf_commu_ID  Acommu  Bcommu
```

### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| N_monvol | INT | Number of communicating volumes |
| monvol_IDi | INT | List of monitored volume IDs |
| surf_commu_ID | INT | Communication surface ID |
| Acommu | FLOAT | Communication area coefficient |
| Bcommu | FLOAT | Pressure coefficient |

---

## 4. MONVOL/FVMBAG1 - Finite Volume Method Airbag

FVM-based airbag with non-uniform pressure distribution.

### Syntax
```
/MONVOL/FVMBAG1/monvol_ID
monvol_title
#  surf_ID   mat_ID  sens_ID   Nb_inj  Nb_vent
#                 Pext            T_Initial
#  Iequi     Igasdef
# For each injector:
#  mat_inj  sens_inj  fct_Minj  Fscale_M  fct_Tinj  Fscale_T
#  node1     node2     node3     Ijet
```

### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| mat_ID | INT | Initial gas material ID |
| Igasdef | INT | Gas definition flag |
| Iequi | INT | Initial equilibrium flag |

### Features
- Non-uniform pressure distribution
- 3D gas flow simulation
- Suitable for complex airbag geometries

---

## 5. MONVOL/GAS - Ideal Gas Volume

Simple monitored volume with ideal gas law.

### Syntax
```
/MONVOL/GAS/monvol_ID
monvol_title
#  surf_ID                 Pext                 Pinit            T_Initial
#   GAMMA                   Rgas
```

### Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| Pinit | FLOAT | Pext | Initial internal pressure |
| GAMMA | FLOAT | 1.4 | Heat capacity ratio |
| Rgas | FLOAT | 287.0 | Gas constant (J/kg·K) |

### Ideal Gas Law
```
P * V = m * R * T
P = rho * R * T
```

---

## 6. MONVOL/LFLUID - Liquid Fluid Volume

Monitored volume for incompressible or slightly compressible liquid.

### Syntax
```
/MONVOL/LFLUID/monvol_ID
monvol_title
#  surf_ID   mat_ID  sens_ID
#                 Pext              P_ini            T_Initial
#   rho0                Kbulk
```

### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| rho0 | FLOAT | Reference density |
| Kbulk | FLOAT | Bulk modulus |
| P_ini | FLOAT | Initial pressure |

### Applications
- Fuel tank sloshing
- Hydraulic systems
- Liquid containers

---

## Monitored Volume Type Summary

| Type | Description | Pressure | Application |
|------|-------------|----------|-------------|
| AIRBAG | Basic uniform pressure | Uniform | Standard airbag |
| AIRBAG1 | Enhanced with material | Uniform | Advanced airbag |
| COMMU1 | Communicating volumes | Multi-zone | Chambered airbag |
| FVMBAG1 | Finite volume method | Non-uniform | Complex airbag |
| GAS | Ideal gas | Uniform | Gas containers |
| LFLUID | Liquid fluid | Uniform | Fuel tanks |

---

## Gas Properties

### Common Gas Properties

| Gas | GAMMA | R (J/kg·K) | Cp (J/kg·K) |
|-----|-------|------------|-------------|
| Air | 1.40 | 287.0 | 1005 |
| Nitrogen | 1.40 | 296.8 | 1039 |
| Argon | 1.67 | 208.1 | 520 |
| Helium | 1.67 | 2077 | 5193 |
| Airbag gas | 1.28 | 320 | 1200 |

### Temperature-Dependent Specific Heat
```
Cp(T) = cpa + cpb*T + cpc*T^2
Cv = Cp - R
GAMMA = Cp/Cv
```

---

## Injector Modeling

### Mass Flow Rate
- Direct mass flow rate function (Imass=0)
- Cumulative mass function (Imass=1)
- Tank test data conversion (Imass=2)

### Jetting Model (Ijet)

| Value | Description |
|-------|-------------|
| 0 | No jetting (momentum neglected) |
| 1 | Simple jet model |
| 2 | Directional jet using nodes |

### Jet Direction
Defined by three nodes (N1, N2, N3):
- N1: Jet origin
- N2, N3: Define jet direction plane

---

## Vent Hole Modeling

### Vent Types (Ivent)

| Value | Description |
|-------|-------------|
| 0 | Standard vent (outflow only) |
| 1 | Bi-directional vent |
| 2 | Check valve (one-way) |

### Pressure-Dependent Venting
- dPdef: Pressure difference to activate
- dtPdef: Time delay for activation
- fct_vdP: Function defining area vs pressure

### Fabric Porosity
- Iporp: Porosity vs pressure model
- Ipora: Porosity vs area model
- Accounts for fabric permeability

---

## Best Practices

### Mesh Requirements
1. **Surface mesh quality**: Consistent normal directions
2. **Watertight volume**: No gaps in enclosing surface
3. **Sufficient resolution**: Capture inflation dynamics

### Initial Conditions
1. **Folded airbag**: Small initial volume
2. **Atmospheric pressure**: Set Pext correctly
3. **Room temperature**: T_Initial typically 293K

### Stability
1. **Time step**: May require /DT/MONVOL control
2. **Mass scaling**: Avoid on airbag fabric
3. **Contact**: Use appropriate interface type

### Validation
1. **Tank tests**: Compare with experimental data
2. **Mass conservation**: Check total mass
3. **Energy balance**: Monitor internal energy

---

## Example: Complete Airbag Model

```
#---1----|----2----|----3----|----4----|----5----|----6----|----7----|----8----|
# Driver airbag with single injector and two vent holes
/MONVOL/AIRBAG/1
Driver_Side_Airbag
#  surf_ID   Nb_inj  Nb_vent   Iequi       Ittf   Iflow1                 Pext
       100        1        2         0         0         0           101325.0
#   Scal_T    Scal_P    Scal_S    Scal_A    Scal_D         MU
       1.0       1.0       1.0       1.0       1.0              0.0
#   GAMMAi       cpa       cpb       cpc                          T_Initial
      1.28    1200.0       0.0       0.0                              293.0
# Injector 1 - Main inflator
      1.28    1200.0       0.0       0.0
         0        10       1.0         0        11       1.0         1         0
         0         0         0
         0         0         0
# Vent hole 1 - Primary vent
       101       0.8       0.0      0.03         0         0         0
       0.0       0.0         0         0         0         0
# Vent hole 2 - Secondary vent
       102       0.6       0.0      0.05         0         0         0
       0.0       0.0         0         0         0         0

# Mass flow rate function
/FUNCT/10
Inflator_Mass_Flow
#                  X                   Y
                 0.0                 0.0
               0.001                 0.5
               0.005                 0.8
                0.01                 1.0
                0.03                 0.8
                0.05                 0.4
                0.10                 0.0

# Temperature function
/FUNCT/11
Inflator_Temperature
#                  X                   Y
                 0.0               800.0
               0.005               750.0
                0.01               700.0
                0.05               500.0
                0.10               350.0

# Sensor for airbag triggering
/SENSOR/TIME/1
Crash_Sensor
#           Tdelay
              0.01
```

---

## References

- Source files: `hm_cfg_files/config/CFG/radioss*/MONVOL/`
- Engine source: `engine/source/airbag/`
- Starter source: `starter/source/airbag/`

