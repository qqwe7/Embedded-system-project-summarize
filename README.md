# Embedded Systems Project Collection

> 阅读 [中文版](README_CN.md)

![Platform](https://img.shields.io/badge/Platform-51%20%7C%20STM32%20%7C%20Linux-blue) ![Language](https://img.shields.io/badge/Language-C%20%7C%20C%2B%2B%20%7C%20Python-green) ![Tools](https://img.shields.io/badge/Tools-Keil%20%7C%20Proteus%20%7C%20VSCode-orange)

## 📖 Overview
This repository serves as a centralized archive for embedded system development resources. It is structured to support learners and engineers by providing modular, well-documented examples across different hardware architectures. The projects cover everything from basic peripheral drivers to complex system designs.

**Key Features:**
*   **Peripheral Drivers**: GPIO, UART, I2C, SPI, ADC/DAC, Timers.
*   **System Design**: FSM (Finite State Machines), RTOS integration, Interrupt handling.
*   **Hardware Simulation**: Proteus circuit simulation for verification without physical hardware.
*   **Interaction**: Data visualization and control using Python or Serial tools.

---

## 📂 Repository Structure

The repository is organized by hardware architecture.

### 🟢 `01_51_Series` (MCS-51 Architecture)
*   **Target Hardware**: AT89C51, STC89C52, and compatible 8-bit MCUs.
*   **Focus Areas**:
    *   Register-level programming.
    *   Direct IO manipulation.
    *   Basic display driving: LED, 7-Segment, LCD1602.
*   **Required Tools**: Keil C51, Proteus.

### 🔵 `02_STM32_Series` (ARM Cortex-M Architecture)
*   **Target Hardware**: STM32F1 (Cortex-M3), STM32F4 (Cortex-M4).
*   **Focus Areas**:
    *   High-performance signal processing.
    *   Complex bus protocols: CAN, USB, FSMC.
    *   Standard Peripheral Lib (SPL) & HAL Library development.
    *   RTOS & Advanced HMI (Human-Machine Interfaces).
*   **Required Tools**: Keil MDK-ARM, STM32CubeMX, Serial Port Utility.

### 🟠 `03_Linux_Vision` (Advanced / SBC)
*   **Target Hardware**: Single Board Computers like OrangePi, Raspberry Pi.
*   **Focus Areas**:
    *   Embedded Linux environment setup.
    *   Computer Vision: OpenCV, YOLO.
    *   Python hardware control.
    *   IoT Communication: MQTT, Socket.
*   **Required Tools**: VS Code (Remote-SSH), Python 3.x.

---

## ⚙️ Environment Setup Guide

### 1. Keil uVision Setup (For 51 & STM32)
*   **Installation**: Install **Keil uVision 5**.
*   **Toolchain**:
    *   `01_51_Series`: Requires **C51** toolchain.
    *   `02_STM32_Series`: Requires **MDK-ARM** toolchain.
*   **Pack Management**:
    *   Open "Pack Installer" within Keil.
    *   Install the Device Family Pack (DFP) for your specific chip (e.g., `Keil::STM32F1xx_DFP`).
    *   *Note: You must install the DFP to select the target chip in your project settings.*
*   **Encoding**:
    *   Ensure "Encode in ANSI" (for legacy GB2312 files) or "UTF-8" is selected in your editor settings to avoid garbled comments.

### 2. Proteus Simulation
*   **Version**: Recommended **Proteus 8.9** or newer.
*   **Linking Firmware**:
    1.  Double-click the MCU component in the schematic.
    2.  Locate the `Program File` field.
    3.  Browse and select the `.hex` or `.axf` file from the project's `Objects` or `Output` directory.

### 3. Hardware Debugging
*   **Drivers**: Install **CH340** or **CP2102** drivers for USB-to-Serial communication.
*   **Debugger**: Install ST-Link or J-Link drivers for STM32 hardware debugging.
*   **Tools**: Use tools like XCOM, SSCOM, or MobaXterm for UART monitoring and debugging.

---

## 📝 Standard Project Layout

Projects typically follow this internal structure:

```text
Project_Root/
├── Core/               # Main application logic (main.c, interrupts)
├── Drivers/            # Hardware abstraction layers (BSP)
│   ├── Inc/            # Header files (.h)
│   └── Src/            # Source files (.c)
├── Simulation/         # Proteus .pdsprj files
├── Docs/               # Datasheets, Reports, Schematics
└── Output/             # Compiled binaries (.hex, .axf)
```

---

## ⚠️ Common Troubleshooting

| Issue | Possible Cause | Solution |
| :--- | :--- | :--- |
| **Target not created** | Missing compiler output setting. | Check "Options for Target" -> "Output" -> Enable "Create HEX File". |
| **L6235E: More than one...** | Duplicate files in project. | Remove duplicate source files from the Project file tree in Keil. |
| **Simulation Crash** | High CPU load or version mismatch. | Increase the simulation timestep or use a compatible Proteus version. |
| **Garbled Comments** | Encoding mismatch. | Change Editor encoding in Keil (Configuration -> Editor). |

---

## 🤝 Contribution
Contributions are welcome! Please ensure you follow the existing directory structure when submitting a Pull Request.

---
*Maintained by: qqwe7*