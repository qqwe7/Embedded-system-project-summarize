# Embedded Systems Project Collection

> 🇨🇳 [中文版 (Main Version)](README.md)

![Platform](https://img.shields.io/badge/Platform-51%20%7C%20STM32%20%7C%20Linux-blue) ![Language](https://img.shields.io/badge/Language-C%20%7C%20C%2B%2B%20%7C%20Python-green) ![License](https://img.shields.io/badge/License-MIT-green)

## 📖 Introduction
This repository is a comprehensive archive of embedded system projects, ranging from **8-bit microcontroller basics** to **32-bit RTOS applications** and **Linux-based Computer Vision**. 

It is designed for:
*   **Students**: As a reference for course designs and graduation theses.
*   **Engineers**: As a library of modular drivers and system architectures.

---

## 📂 Project Categories & Tech Stack

### 🟢 1. MCS-51 Series (`01_51_Series`)
Focuses on the fundamentals of digital logic and register-level programming.
*   **Core MCU**: STC89C52RC, AT89C51.
*   **Key Modules**:
    *   **Timers/Interrupts**: Digital clocks, stopwatches.
    *   **Display**: Dynamic scanning for 7-segment displays, LCD1602 drivers.
    *   **Sensors**: DS18B20 (Temperature), ADC0832 (Analog-to-Digital).
*   **Dev Tools**: Keil C51 (uVision 5), Proteus 8.9+.

### 🔵 2. STM32 Series (`02_STM32_Series`)
Focuses on ARM Cortex-M3/M4 architectures, covering advanced peripherals and bus protocols.
*   **Core MCU**: STM32F103C8T6 (BluePill), STM32F407ZGT6 (Explorer).
*   **Key Modules**:
    *   **Bus Protocols**: I2C (OLED), SPI (Flash/Screen), CAN (Automotive), UART (Debug).
    *   **Advanced Control**: PID Motor Control, PWM generation, DMA data transfer.
    *   **System**: FreeRTOS integration, FatFs file system.
*   **Dev Tools**: Keil MDK-ARM, STM32CubeMX, ST-Link Utility.

### 🟠 3. Linux & AI Vision (`03_Linux_Vision`)
Focuses on Single Board Computers (SBC) running Embedded Linux.
*   **Core Platform**: OrangePi, Raspberry Pi.
*   **Key Modules**:
    *   **Computer Vision**: YOLOv5/v8 model deployment, OpenCV image processing.
    *   **IoT**: MQTT publication/subscription, Socket programming.
    *   **Scripting**: Python for hardware control (RPi.GPIO / OPi.GPIO).
*   **Dev Tools**: VS Code (Remote SSH), Mobaxterm.

---

## 🛠️ Detailed Setup Guide

### Phase 1: Installation
1.  **Keil uVision 5**: Ensure you install both **C51** (for 8-bit) and **MDK-ARM** (for 32-bit) toolchains.
2.  **Device Packs**: Use the "Pack Installer" in Keil to install support for `Gigadevice::GD32` or `Keil::STM32F1xx` depending on your chip.
3.  **Proteus**: Version **8.9** or higher is strictly recommended to avoid "File Version" errors.

### Phase 2: Compilation
*   **51 Projects**: Open `.uvproj`, press `F7` to build. Output is usually in `Objects/Project.hex`.
*   **STM32 Projects**: Open `.uvprojx`. If using HAL lib, ensure the path to the library files is valid (no absolute paths from other PCs). Output is `.hex` or `.axf`.

### Phase 3: Simulation (Proteus)
1.  Open the `.pdsprj` file.
2.  Double-click the MCU component.
3.  Click the folder icon next to **Program File**.
4.  Navigate to the project folder and select the generated `.hex` file.
5.  Press the **Play** button at the bottom left.

---

## ⚠️ Troubleshooting

| Error Message | Solution |
| :--- | :--- |
| **"Target not created"** | You haven't enabled HEX output. Go to *Flash* -> *Configure Flash Tools* -> *Output* -> Check *Create HEX File*. |
| **"L6235E: More than one section..."** | You added the same file twice in the Project Tree. Remove duplicates. |
| **Chinese Characters are Garbage** | Change Editor Encoding to **ANSI** (GB2312) or **UTF-8** in *Edit* -> *Configuration*. |
| **Proteus: "Simulation is not running in real time"** | This is normal for complex circuits. Ignore it or increase CPU power. |

---

## 🤝 Contributing
1.  Fork the repository.
2.  Create a new branch (`git checkout -b feature/NewProject`).
3.  Commit your changes.
4.  Push to the branch and open a Pull Request.

---
*Maintained by: qqwe7*
