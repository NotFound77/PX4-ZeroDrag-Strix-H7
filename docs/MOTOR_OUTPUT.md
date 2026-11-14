# ZeroDrag Strix H7 R0.2 — Motor Output & DShot Configuration

This document explains the actuator output architecture, timer assignments, and DShot configuration for the **ZeroDrag Strix H7 R0.2** flight controller under PX4 Autopilot.

---

## 1. Output Architecture & Timer Grouping

The flight controller exposes 12 output channels across 4 hardware timers:

```text
TIM3 (Timer 0, DMA1) ──► S1 (CH 0), S2 (CH 1)           ──► Controlled by PWM_MAIN_TIM0
TIM5 (Timer 1, DMA1) ──► S3 (CH 2), S4 (CH 3), S5, S6   ──► Controlled by PWM_MAIN_TIM1
TIM4 (Timer 2, DMA1) ──► S7 (CH 6), S8, S9, S10        ──► Controlled by PWM_MAIN_TIM2
TIM15(Timer 3, No DMA)──► S11 (CH 10), S12 (CH 11)      ──► Controlled by PWM_MAIN_TIM3
```

### Critical Architectural Notes:
1. **Four-Motor Quadcopter Span**: Motors 1–4 span two distinct hardware timers:
   * S1 and S2 belong to **Timer 0** (`TIM3`).
   * S3 and S4 belong to **Timer 1** (`TIM5`).
2. **Frequency Matching Requirement**: Because motors 1–4 are on different timers, **both `PWM_MAIN_TIM0` and `PWM_MAIN_TIM1` must be set to the exact same protocol value**. The PX4 DShot driver will reject mixed frequencies across motor timers.
3. **TIM8 Exclusivity**: `TIM8` is reserved exclusively for the 32-bit PX4 High-Resolution Timer (`hrt`). Motor outputs do not use `TIM8`.

---

## 2. Protocol Configuration Parameters

In PX4 v1.14 and later (including v1.18), actuator protocols are configured dynamically per-timer via `PWM_MAIN_TIM<timer_index>`:

| Protocol | Value to Set | Description |
| :--- | :---: | :--- |
| **PWM 50 Hz** | `50` | Standard analog servo rate |
| **PWM 400 Hz** | `400` | Standard digital PWM motor rate (Default) |
| **OneShot125** | `-1` | Fast analog pulse protocol |
| **DShot150** | `-5` | Digital DShot @ 150 kbit/s |
| **DShot300** | **`-4`** | **Digital DShot @ 300 kbit/s (Recommended for Bluejay)** |
| **DShot600** | **`-3`** | **Digital DShot @ 600 kbit/s (High-rate DShot)** |
| **BDShot300** | `-7` | Bidirectional DShot300 with RPM telemetry *(Under evaluation)* |
| **BDShot600** | `-6` | Bidirectional DShot600 with RPM telemetry *(Under evaluation)* |

> [!NOTE]
> The legacy global parameter `DSHOT_CONFIG` from PX4 v1.11–v1.13 **does not exist** in this version. Protocol selection is performed exclusively via `PWM_MAIN_TIMx`.

---

## 3. Configuring S1–S4 for DShot300

To configure the four main motor outputs (S1–S4) for DShot300 while keeping auxiliary outputs S7–S12 on standard PWM:

### Via MAVLink Shell / NSH Console:
```sh
# Set Timer 0 (S1-S2) and Timer 1 (S3-S6) to DShot300
param set PWM_MAIN_TIM0 -4
param set PWM_MAIN_TIM1 -4

# Keep Timer 2 (S7-S10) and Timer 3 (S11-S12) as 400 Hz PWM
param set PWM_MAIN_TIM2 400
param set PWM_MAIN_TIM3 400

# Save to internal flash (Sector 15) and reboot
param save
reboot
```

### Via QGroundControl:
1. Navigate to **Vehicle Setup** $\rightarrow$ **Parameters**.
2. Search for `PWM_MAIN_TIM0` and set to `DShot300` (`-4`).
3. Search for `PWM_MAIN_TIM1` and set to `DShot300` (`-4`).
4. Click **Tools** $\rightarrow$ **Reboot Vehicle**.

---

## 4. Verification & Testing Procedure

> [!WARNING]
> **ALWAYS REMOVE PROPELLERS BEFORE BENCH-TESTING MOTOR OUTPUTS!**

### Step 1: Check Driver Status
In the MAVLink shell, query driver status:

```sh
dshot status
```
*Expected Output*:
```text
INFO  [dshot] Configuration:
INFO  [dshot]   Output Mask:        0x0f (4 channels)
INFO  [dshot]   _dshot_frequency:   300000
Channel Configuration:
Channel  0: func: 101 (Motor 1)
Channel  1: func: 102 (Motor 2)
Channel  2: func: 103 (Motor 3)
Channel  3: func: 104 (Motor 4)
```

Query the PWM driver to confirm it released channels 0–3:
```sh
pwm_out status
```
*Expected Output*:
```text
Timer 2: rate: 400 channels: 6 7 8 9
Timer 3: rate: 400 channels: 10 11
```

### Step 2: ESC Initialization Chime
Connect a charged flight battery (4S LiPo). The LANRC 45A ESCs running Bluejay will emit:
1. **Three rising tones**: ESC power-up signal.
2. **Two short rising tones**: Valid DShot signal detected and armed.

### Step 3: Spin-Testing Individual Motors
Test each motor individually at 5% idle throttle:
```sh
# Spin Motor 1 at 5% idle
actuator_test set -m 1 -v 0.05

# Stop Motor 1
actuator_test set -m 1 -v 0

# Test Motors 2, 3, and 4
actuator_test set -m 2 -v 0.05
actuator_test set -m 2 -v 0
actuator_test set -m 3 -v 0.05
actuator_test set -m 3 -v 0
actuator_test set -m 4 -v 0.05
actuator_test set -m 4 -v 0
```
