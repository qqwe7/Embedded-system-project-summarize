# Embedded Systems Project Collection | 嵌入式系统项目合集

![Platform](https://img.shields.io/badge/Platform-51%20%7C%20STM32%20%7C%20Linux-blue) ![Language](https://img.shields.io/badge/Language-C%20%7C%20C%2B%2B%20%7C%20Python-green) ![Tools](https://img.shields.io/badge/Tools-Keil%20%7C%20Proteus%20%7C%20VSCode-orange)

## 📖 Overview | 项目简介

**English**
This repository serves as a centralized archive for embedded system development resources. It is structured to support learners and engineers by providing modular, well-documented examples across different hardware architectures. The projects cover everything from basic peripheral drivers to complex system designs.

**中文**
本项目仓库是一个综合性的嵌入式系统资源库，旨在为学习者和工程师提供模块化、文档完善的开发示例。内容涵盖了从基础的底层外设驱动到复杂的系统级设计，支持多种主流硬件架构。

**Key Features / 核心内容:**
*   **Peripheral Drivers / 外设驱动**: GPIO, UART, I2C, SPI, ADC/DAC, Timers.
*   **System Design / 系统设计**: FSM (Finite State Machines), RTOS integration, Interrupt handling.
*   **Simulation / 硬件仿真**: Proteus circuit simulation for verification without physical hardware.
*   **Interaction / 上位机交互**: Data visualization and control using Python or Serial tools.

---

## 📂 Repository Structure | 仓库结构

The repository is organized by hardware architecture.
仓库按照硬件架构进行分类组织。

### 🟢 `01_51_Series` (MCS-51 Architecture)
*   **Hardware / 硬件平台**: AT89C51, STC89C52, and compatible 8-bit MCUs.
*   **Focus / 侧重**:
    *   Register-level programming (寄存器级编程).
    *   Direct IO manipulation (直接 IO 控制).
    *   Basic display driving: LED, 7-Segment, LCD1602 (基础显示驱动).
*   **Tools / 工具**: Keil C51, Proteus.

### 🔵 `02_STM32_Series` (ARM Cortex-M Architecture)
*   **Hardware / 硬件平台**: STM32F1 (Cortex-M3), STM32F4 (Cortex-M4).
*   **Focus / 侧重**:
    *   High-performance signal processing (高性能信号处理).
    *   Complex bus protocols: CAN, USB, FSMC (复杂总线协议).
    *   Standard Peripheral Lib (SPL) & HAL Library (标准库与HAL库开发).
    *   RTOS & Advanced HMI (实时操作系统与高级人机交互).
*   **Tools / 工具**: Keil MDK-ARM, STM32CubeMX, Serial Port Utility.

### 🟠 `03_Linux_Vision` (Advanced / SBC)
*   **Hardware / 硬件平台**: Single Board Computers like OrangePi, Raspberry Pi (香橙派/树莓派).
*   **Focus / 侧重**:
    *   Embedded Linux setup (嵌入式 Linux 环境搭建).
    *   Computer Vision: OpenCV, YOLO (机器视觉).
    *   Python hardware control (Python 硬件控制).
    *   IoT Communication: MQTT, Socket (物联网通信).
*   **Tools / 工具**: VS Code (Remote-SSH), Python 3.x.

---

## ⚙️ Environment Setup | 环境配置指南

### 1. Keil uVision Setup (For 51 & STM32)
*   **Installation / 安装**: Install **Keil uVision 5**.
*   **Toolchain / 工具链**:
    *   `01_51_Series`: Requires **C51** toolchain.
    *   `02_STM32_Series`: Requires **MDK-ARM** toolchain.
*   **Pack Management / 芯片包**:
    *   Open "Pack Installer" and install the Device Family Pack (DFP) for your specific chip (e.g., `Keil::STM32F1xx_DFP`).
    *   *请务必安装对应芯片的 DFP 包，否则无法选择目标芯片。*
*   **Encoding / 编码**:
    *   Ensure "Encode in ANSI" (for GB2312) or "UTF-8" is selected to avoid garbled Chinese comments.
    *   *建议检查编辑器的编码设置，防止中文注释乱码。*

### 2. Proteus Simulation | Proteus 仿真
*   **Version / 版本**: Recommended **Proteus 8.9** or newer. (建议使用 8.9 或更高版本).
*   **Linking Firmware / 固件关联**:
    1.  Double-click the MCU in the schematic. (双击原理图中的单片机).
    2.  Locate `Program File`. (找到 Program File 选项).
    3.  Select the `.hex` or `.axf` file from the `Objects/Output` folder. (选择编译生成的 hex 或 axf 文件).

### 3. Hardware Debugging | 硬件调试
*   **Drivers / 驱动**: Install **CH340** or **CP2102** USB-TTL drivers.
*   **Debugger / 调试器**: Install ST-Link or J-Link drivers for STM32.
*   **Tools / 串口工具**: Use XCOM, SSCOM, or MobaXterm for UART debugging.

---

## 📝 Standard Project Layout | 标准项目结构

Projects typically follow this internal structure:
项目通常遵循以下目录结构：

```text
Project_Root/
├── Core/               # Main logic (main.c, interrupts) | 主程序逻辑
├── Drivers/            # Hardware abstraction (BSP)      | 硬件驱动层
│   ├── Inc/            # Header files (.h)               | 头文件
│   └── Src/            # Source files (.c)               | 源文件
├── Simulation/         # Proteus .pdsprj files           | 仿真工程文件
├── Docs/               # Datasheets, Reports             | 设计文档与报告
└── Output/             # Compiled binaries (.hex)        | 编译输出文件
```

---

## ⚠️ Common Troubleshooting | 常见问题排查

| Issue (问题) | Possible Cause (可能原因) | Solution (解决方法) |
| :--- | :--- | :--- |
| **Target not created** | Missing compiler output setting. | Check "Options for Target" -> "Output" -> "Create HEX File". (勾选生成 HEX 文件) |
| **L6235E: More than one...** | Duplicate files in project. | Remove duplicate source files in Keil project tree. (移除工程中重复添加的源文件) |
| **Simulation Crash** | High CPU load or version mismatch. | Increase timestep or use compatible Proteus version. (增加仿真步长或更换版本) |
| **Garbled Comments (乱码)** | Encoding mismatch (ANSI/UTF-8). | Change Editor encoding in Keil (Configuration -> Editor). (修改编辑器编码设置) |

---

## 🤝 Contribution | 贡献
Contributions are welcome! Please ensure you follow the existing directory structure when submitting a Pull Request.
欢迎提交 PR！提交时请遵循现有的目录结构规范。

---
*Maintained by: qqwe7*
