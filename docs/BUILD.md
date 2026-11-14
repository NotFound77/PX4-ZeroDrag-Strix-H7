# ZeroDrag Strix H7 R0.2 — Firmware Build Guide

This document describes how to set up the toolchain and build the PX4 Autopilot firmware for the **ZeroDrag Strix H7 R0.2** flight controller.

---

## 1. Prerequisites & Toolchain Requirements

### Recommended Host Operating Systems
* **Ubuntu Linux**: 24.04 LTS or 22.04 LTS (recommended)
* **Debian**: 11 or 12
* **macOS**: Sonoma / Sequoia with Docker or native ARM toolchain

### Required Packages & Compilers
1. **ARM Bare-Metal Cross-Compiler**:
   * `arm-none-eabi-gcc` & `arm-none-eabi-g++` (Version 10.3 through 13.2)
   * `arm-none-eabi-binutils` & `newlib`
2. **Build System**:
   * `cmake` (>= 3.20)
   * `ninja-build` (recommended for fast parallel builds) or GNU `make`
3. **Python Environment**:
   * Python 3.8+
   * Required Python packages: `jinja2`, `empy==3.3.4`, `kconfiglib`, `pyros-genmsg`, `pyyaml`, `toml`, `packaging`, `cerberus`

---

## 2. Setting Up the Build Environment

### Ubuntu Linux (Automated Setup)
You can run the official PX4 dependency installer script:
```bash
# Clone the repository first (see below)
cd PX4-ZeroDrag-Strix-H7
bash ./Tools/setup/ubuntu.sh
```
Or install the core toolchain manually:
```bash
sudo apt update
sudo apt install -y \
    build-essential \
    cmake \
    ninja-build \
    gcc-arm-none-eabi \
    libnewlib-arm-none-eabi \
    libstdc++-arm-none-eabi-newlib \
    python3 \
    python3-pip \
    python3-setuptools \
    git \
    astyle

pip3 install --user kconfiglib jinja2 empy==3.3.4 pyros-genmsg pyyaml toml packaging cerberus
```

---

## 3. Cloning the Repository

The repository relies on git submodules (including NuttX, standard libraries, and drivers). You must clone recursively:

```bash
git clone --recursive https://github.com/<USERNAME>/PX4-ZeroDrag-Strix-H7.git
cd PX4-ZeroDrag-Strix-H7
```

If you previously cloned without `--recursive`, initialize submodules manually:
```bash
git submodule update --init --recursive
```

---

## 4. Building the Application Firmware

To compile the standard application flight firmware:

```bash
make zerodrag_strix-h7_default
```

### Build Output Artifacts
Upon successful compilation, build artifacts are located in:
```text
build/zerodrag_strix-h7_default/
├── zerodrag_strix-h7_default.px4     # Complete compressed firmware package for QGC / px4_uploader
├── zerodrag_strix-h7_default.bin     # Raw flat binary (starts at 0x08020000, Sector 1)
├── zerodrag_strix-h7_default.elf     # Full ELF binary with debug symbols
└── zerodrag_strix-h7_default.map     # Linker memory map
```

* **Target Memory Layout**:
  - Code entry point: `0x08020000` (Sector 1)
  - Sector 0 (`0x08000000–0x0801FFFF`, 128 KB) is reserved exclusively for the bootloader.
  - Sector 15 (`0x081E0000–0x081FFFFF`, 128 KB) is reserved exclusively for parameter storage.

---

## 5. Building the Custom Bootloader

The custom PX4 NuttX bootloader can be compiled independently:

```bash
make zerodrag_strix-h7_bootloader
```

Artifacts are generated in:
```text
build/zerodrag_strix-h7_bootloader/
├── zerodrag_strix-h7_bootloader.bin  # Bootloader binary for Sector 0 (0x08000000)
└── zerodrag_strix-h7_bootloader.elf
```

---

## 6. Code Style & Verification

PX4 enforces formatting rules for all C and C++ source code. Before submitting changes:

```bash
# Verify formatting
make check_format

# Automatically format changed files
make format
```
