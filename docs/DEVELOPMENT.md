# ZeroDrag Strix H7 R0.2 — Board Port Development Guide

This guide explains the internal architecture of the ZeroDrag Strix H7 R0.2 custom board port and how to contribute, extend, or maintain the implementation.

---

## 1. Board Port Structure

All custom board files are housed cleanly within the board target directory:

```text
boards/zerodrag/strix-h7/
├── default.px4board              # Main application Kconfig/board configuration
├── bootloader.px4board           # Bootloader Kconfig configuration
├── firmware.prototype            # Board identification metadata and manifest
├── init/
│   ├── rc.board_defaults         # Board-specific default parameter overrides
│   └── rc.board_sensors          # Sensor startup script (ICM42688P -R 6, DPS310)
├── nuttx-config/
│   ├── bootloader/
│   │   └── defconfig             # NuttX RTOS kernel config for bootloader
│   ├── nsh/
│   │   └── defconfig             # NuttX RTOS kernel config for flight application
│   ├── include/
│   │   ├── board.h               # Clocks, HSE (8 MHz), pin multiplexing definitions
│   │   └── board_dma_map.h       # DMA1/DMA2 routing (Timers vs IMU SPI)
│   └── scripts/
│       ├── bootloader_script.ld  # Bootloader linker script (0x08000000, 128 KB)
│       └── script.ld             # Application linker script (0x08020000)
└── src/
    ├── board_config.h            # Main board pin and peripheral definitions
    ├── hw_config.h               # Hardware revision and peripheral assignments
    ├── timer_config.cpp          # 12-channel timer & DMA allocation
    ├── spi.cpp                   # SPI bus definitions (SPI1, SPI4 IMUs)
    ├── i2c.cpp                   # I2C bus definitions (I2C2 DPS310)
    ├── init.c                    # Board early hardware initialization
    ├── led.c                     # User status LED driver (PE3 Blue, PE4 Red)
    ├── sdio.c                    # SDMMC1 4-bit MicroSD card interface
    ├── usb.c                     # USB OTG FS interface
    ├── bootloader_main.c         # Bootloader entry & hardware deinit
    └── CMakeLists.txt            # Board target build rules
```

---

## 2. Key Design Principles of this Port

1. **HRT Exclusivity**:
   PX4 requires a dedicated, unshared 32-bit hardware timer for its High-Resolution Timer (`hrt_absolute_time()`). `TIM8` is assigned exclusively to HRT in `board_config.h`. Outputs S1 and S2 avoid `TIM8` and use `TIM3` channels instead.
2. **DMA Bus Isolation**:
   To avoid latency spikes or dropped DMA transactions during high-speed motor updates:
   * **DMA1** is dedicated exclusively to DShot timer update bursts (TIM3, TIM5, TIM4).
   * **DMA2** is dedicated exclusively to high-rate sensor polling (SPI1 and SPI4 IMUs).
3. **Application Bootloader Updater Disabled**:
   `CONFIG_SYSTEMCMDS_BL_UPDATE` is intentionally disabled in `default.px4board`. Bootloader updates must only be performed via external DFU recovery to prevent accidental corruption of Sector 0.
4. **Sensor Rotations**:
   The board PCB places both ICM-42688-P IMUs rotated 270° clockwise relative to the FC forward arrow. `rc.board_sensors` starts both drivers with `-R 6` (`ROTATION_YAW_270`), giving level attitude readings without manual user calibration offsets.

---

## 3. Contribution Workflow

### Commit Guidelines
We follow standard PX4 conventional commit formatting with topic-based scopes:
```text
type(scope): concise description

Detailed explanation of the change, motivation, and verification steps.

Assisted-by: <name or tool>
```
* **Types**: `feat`, `fix`, `refactor`, `docs`, `test`, `chore`.
* **Scope**: `zerodrag_strix-h7` or specific subsystem.

### Code Formatting
All C/C++ code must adhere to PX4's style rules enforced via `astyle`:
```bash
# Verify formatting
make check_format

# Apply format automatically
make format
```
Pull requests failing `make check_format` cannot be merged.
