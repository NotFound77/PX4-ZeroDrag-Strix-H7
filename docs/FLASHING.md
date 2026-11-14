# ZeroDrag Strix H7 R0.2 — Firmware Flashing & Bootloader Recovery Guide

This guide details standard application firmware updates, bootloader installation, and disaster recovery procedures for the **ZeroDrag Strix H7 R0.2** flight controller.

---

## 1. Flash Memory Architecture

The STM32H743VIH6 contains **2 MB of internal dual-bank flash**, organized into 16 sectors of 128 KB each:

```text
Address Range               Sector        Allocation                 Protected By
0x08000000 - 0x0801FFFF     Sector 0      Custom PX4 Bootloader (128 KB)    PX4 Uploader Boundary
0x08020000 - 0x080FFFFF     Sectors 1-7   PX4 Application (Bank 1)          Flashed by Uploader
0x08100000 - 0x0819FFFF     Sectors 8-12  PX4 Application (Bank 2)          Flashed by Uploader
0x081A0000 - 0x081BFFFF     Sector 13     Spare / Overflow                  Preserved
0x081C0000 - 0x081DFFFF     Sector 14     Reserved                          Preserved
0x081E0000 - 0x081FFFFF     Sector 15     PX4 Parameter Storage (128 KB)    Preserved
```

---

## 2. Standard Firmware Update (Normal USB Mode)

Once the custom PX4 bootloader is installed in Sector 0, application firmware updates are performed over normal USB without entering DFU mode.

### Option A: Using the PX4 Command-Line Uploader
Connect the board to your host computer via USB Type-C:

```bash
python3 Tools/px4_uploader.py --port /dev/ttyACM0 build/zerodrag_strix-h7_default/zerodrag_strix-h7_default.px4
```

The script will automatically detect the board, negotiate PX4 Bootloader Protocol v5, erase Sectors 1–12, flash the application binary, verify the image, and reboot the flight controller.

### Option B: Using QGroundControl (QGC)
1. Disconnect the flight controller from USB.
2. Open QGroundControl and navigate to **Vehicle Setup** $\rightarrow$ **Firmware**.
3. Plug in the USB cable.
4. Select **Advanced settings** $\rightarrow$ **Custom firmware file...**.
5. Select `build/zerodrag_strix-h7_default/zerodrag_strix-h7_default.px4`.
6. Click **OK** to flash and reboot.

---

## 3. Bootloader Recovery via Factory ROM DFU

If the flight controller was previously flashed with Betaflight or another firmware that overwritten Sector 0, the custom PX4 bootloader must be restored before the standard PX4 uploader can communicate with the board.

### Step 1: Put the Board into ROM DFU Mode
* **Method 1 (Hardware Button)**: Hold down the physical `BOOT` button on the flight controller while connecting the USB-C cable to your PC.
* **Method 2 (From Betaflight CLI)**: Connect to `/dev/ttyACM0` via serial terminal and issue the `bl` command.

Confirm the device is in DFU mode using `lsusb`:
```text
Bus 001 Device 009: ID 0483:df11 STMicroelectronics STM Device in DFU Mode
```

### Step 2: Flash Bootloader to Sector 0 Only
Use `dfu-util` to flash the custom bootloader into Sector 0 (`0x08000000`):

```bash
dfu-util -a 0 -d 0483:df11 -s 0x08000000:leave -D build/zerodrag_strix-h7_bootloader/zerodrag_strix-h7_bootloader.bin
```

> [!CAUTION]
> * **Do not use mass erase (`-E`)**: A mass erase will wipe all sectors, including the parameter storage in Sector 15.
> * **Do not modify option bytes**: Never write to Alt 1 (`@Option Bytes /0x5200201C`) unless explicitly required. The targeted write above safely modifies only Sector 0.

### Step 3: Verify PX4 Bootloader Enumeration
Upon rebooting from DFU mode, the board will enumerate with the ZeroDrag USB identity:
```text
ID 26ac:0050 ZeroDrag ZeroDrag Strix H7 BL (Provision
```

Now proceed to Section 2 to flash the main `zerodrag_strix-h7_default.px4` application firmware.
