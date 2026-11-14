# ZeroDrag Strix H7 R0.2 — Hardware Specification & Pinout

This document details the hardware architecture, microcontroller configuration, sensor interfaces, and pin assignments for the **ZeroDrag Strix H7 R0.2** flight controller under PX4 Autopilot.

---

## 1. Microcontroller Core

| Component | Specification |
| :--- | :--- |
| **MCU** | STMicroelectronics STM32H743VIH6 |
| **Package** | TFBGA-100 (100-ball grid array) |
| **Core Architecture** | ARM 32-bit Cortex®-M7 with double-precision FPU & L1 cache |
| **Core Clock Frequency** | 480 MHz |
| **Internal Flash Memory** | 2 MB Dual-Bank Flash (16 sectors × 128 KB) |
| **Internal RAM** | 1024 KB total (512 KB AXI SRAM, 128 KB DTCM, 64 KB ITCM, 288 KB SRAM1/2/3, 4 KB Backup SRAM) |
| **High-Resolution Timer (HRT)**| Dedicated hardware 32-bit timer on **TIM8** (PX4 HRT requirement) |

---

## 2. Sensor Suite

The ZeroDrag Strix H7 R0.2 integrates a dual-IMU redundant array and a high-precision internal digital barometer:

### Primary IMU (IMU 1)
* **Part**: InvenSense ICM-42688-P (6-axis Accel + Gyro, 32 kHz FIFO)
* **Bus Interface**: `SPI1`
* **Bus Pins**: SCK: `PA5`, MISO: `PA6`, MOSI: `PB5`
* **Chip Select (CS)**: `PB12` (active low)
* **Data Ready (DRDY)**: `PC4`
* **DMA Stream**: Dedicated on `DMA2` (`DMAMAP_DMA12_SPI1RX_1`, `DMAMAP_DMA12_SPI1TX_1`)
* **Orientation / Mounting Rotation**: **`-R 6`** (`ROTATION_YAW_270`)
* **PX4 Driver**: `icm42688p -R 6 -b 1 -s start`

### Secondary IMU (IMU 2)
* **Part**: InvenSense ICM-42688-P (6-axis Accel + Gyro, 32 kHz FIFO)
* **Bus Interface**: `SPI4`
* **Bus Pins**: SCK: `PE2`, MISO: `PE13`, MOSI: `PE6`
* **Chip Select (CS)**: `PE11` (active low)
* **Data Ready (DRDY)**: `PE15`
* **DMA Stream**: Dedicated on `DMA2` (`DMAMAP_DMA12_SPI4RX_1`, `DMAMAP_DMA12_SPI4TX_1`)
* **Orientation / Mounting Rotation**: **`-R 6`** (`ROTATION_YAW_270`)
* **PX4 Driver**: `icm42688p -R 6 -b 4 -s start`

### Digital Barometer
* **Part**: Infineon DPS310 (ultra-high precision barometric air pressure sensor)
* **Bus Interface**: `I2C2`
* **Bus Pins**: SCL: `PB10`, SDA: `PB11`
* **I2C Address**: `0x76` (118 decimal)
* **PX4 Driver**: `dps310 -I -b 2 -a 118 start`

### Magnetometer
* **Internal**: No onboard compass (minimizes motor electromagnetic interference).
* **External**: External magnetometers (e.g., IST8310, QMC5883L, BMM150) connect via the external `I2C1` port on the GPS connector.

---

## 3. Actuator Output Mapping (S1–S12)

The board provides 12 actuator signal pads divided into 4 hardware timer groups:

| Pad Label | PX4 Channel | Hardware Timer | Timer Channel | Pin | DMA Support | Protocol Capability | Default Role |
| :---: | :---: | :---: | :---: | :---: | :---: | :---: | :--- |
| **S1** | Channel 0 | TIM3 | Channel 3 (AF2) | `PB0` | DMA1 | DShot300/600, PWM | Motor 1 |
| **S2** | Channel 1 | TIM3 | Channel 4 (AF2) | `PB1` | DMA1 | DShot300/600, PWM | Motor 2 |
| **S3** | Channel 2 | TIM5 | Channel 1 (AF2) | `PA0` | DMA1 | DShot300/600, PWM | Motor 3 |
| **S4** | Channel 3 | TIM5 | Channel 2 (AF2) | `PA1` | DMA1 | DShot300/600, PWM | Motor 4 |
| **S5** | Channel 4 | TIM5 | Channel 3 (AF2) | `PA2` | DMA1 | DShot300/600, PWM | Aux 1 / Motor 5 |
| **S6** | Channel 5 | TIM5 | Channel 4 (AF2) | `PA3` | DMA1 | DShot300/600, PWM | Aux 2 / Motor 6 |
| **S7** | Channel 6 | TIM4 | Channel 1 (AF2) | `PD12` | DMA1 | DShot300/600, PWM | Aux 3 / Servo 1 |
| **S8** | Channel 7 | TIM4 | Channel 2 (AF2) | `PD13` | DMA1 | DShot300/600, PWM | Aux 4 / Servo 2 |
| **S9** | Channel 8 | TIM4 | Channel 3 (AF2) | `PD14` | DMA1 | DShot300/600, PWM | Aux 5 / Servo 3 |
| **S10** | Channel 9 | TIM4 | Channel 4 (AF2) | `PD15` | DMA1 | DShot300/600, PWM | Aux 6 / Servo 4 |
| **S11** | Channel 10 | TIM15 | Channel 1 (AF4) | `PE5` | None | PWM Only (50–400 Hz) | Aux 7 / Servo 5 |
| **S12** | Channel 11 | TIM15 | Channel 2 (AF4) | `PE6` | None | PWM Only (50–400 Hz) | Aux 8 / Servo 6 |

> [!IMPORTANT]
> * **TIM8 Reservation**: `TIM8` is reserved exclusively for the PX4 High-Resolution Timer (HRT). S1/S2 utilize primary channels on `TIM3` (AF2) to avoid HRT timer conflicts.
> * **Timer Decoupling**: S1–S2 are on Timer 0 (`TIM3`); S3–S6 are on Timer 1 (`TIM5`). To run DShot on motors 1–4, both `PWM_MAIN_TIM0` and `PWM_MAIN_TIM1` must be set to the same DShot rate (e.g. `-4` for DShot300).

---

## 4. Communication Ports & Serial Mapping

| Connector Label | Peripheral | PX4 Device Path | TX Pin | RX Pin | Hardware Flow Control | Intended Use |
| :--- | :--- | :--- | :--- | :--- | :--- | :--- |
| **GPS** | USART1 | `/dev/ttyS1` | `PA9` | `PA10` | None | Primary GNSS Module |
| **RC** | UART5 | `/dev/ttyS4` | `PC12` (TX) | `PD2` (RX) | Inverted / Direct | CRSF, SBUS, DSMX, ExpressLRS |
| **TELEM 1** | USART6 | `/dev/ttyS5` | `PC6` | `PC7` | None | Primary Telemetry Radio / Companion |
| **TELEM 2** | USART2 | `/dev/ttyS0` | `PD5` | `PD6` | None | Secondary Telemetry / OSD |
| **TELEM 3** | USART3 | `/dev/ttyS2` | `PD8` | `PD9` | None | Payload / Gimbal |
| **TELEM 4** | UART7 | `/dev/ttyS6` | `PE8` | `PE7` | None | Spare Serial |
| **USB** | OTG_FS | `/dev/ttyACM0` | `PA12` (DP) | `PA11` (DM) | USB CDC-ACM | GCS MAVLink & NSH Shell |

---

## 5. Storage & Peripheral Interfaces

* **MicroSD Card**: SDMMC1 interface operating in 4-bit SD mode (`PC8` D0, `PC9` D1, `PC10` D2, `PC11` D3, `PC12` CK, `PD2` CMD). Mounted at `/fs/microsd/`.
* **Analog Power Monitor 1 (BAT1)**:
  - Voltage sense: `PC0` (ADC1 Channel 10, nominal divider: 11.0)
  - Current sense: `PC1` (ADC1 Channel 11, nominal scale: 40.0 A/V)
* **Analog Power Monitor 2 (BAT2)**:
  - Voltage sense: `PA4` (ADC1/2 Channel 18, nominal divider: 11.0)
* **Status LEDs**:
  - Blue LED: `PE3` (Active Low)
  - Red LED: `PE4` (Active Low)
