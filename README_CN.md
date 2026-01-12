# 嵌入式系统项目合集 (Embedded Systems Project Collection)

> Read in [English](README.md)

![Platform](https://img.shields.io/badge/Platform-51%20%7C%20STM32%20%7C%20Linux-blue) ![Language](https://img.shields.io/badge/Language-C%20%7C%20C%2B%2B%20%7C%20Python-green) ![Tools](https://img.shields.io/badge/Tools-Keil%20%7C%20Proteus%20%7C%20VSCode-orange)

## 📖 项目简介
本项目仓库是一个综合性的嵌入式系统资源库，旨在为学习者和工程师提供模块化、文档完善的开发示例。内容涵盖了从基础的底层外设驱动到复杂的系统级设计，支持多种主流硬件架构。

**核心内容:**
*   **外设驱动**: GPIO, UART, I2C, SPI, ADC/DAC, 定时器等。
*   **系统设计**: 有限状态机 (FSM), 实时操作系统 (RTOS) 集成, 中断处理。
*   **硬件仿真**: 使用 Proteus 进行全电路仿真，无需实体硬件即可验证。
*   **上位机交互**: 使用 Python 或串口工具进行数据可视化与控制。

---

## 📂 仓库结构

仓库按照硬件架构进行分类组织。

### 🟢 `01_51_Series` (MCS-51 架构)
*   **硬件平台**: AT89C51, STC89C52 及兼容的 8 位单片机。
*   **侧重**:
    *   寄存器级编程。
    *   直接 IO 操作与逻辑控制。
    *   基础显示驱动 (LED, 数码管, LCD1602)。
*   **工具**: Keil C51, Proteus.

### 🔵 `02_STM32_Series` (ARM Cortex-M 架构)
*   **硬件平台**: STM32F1 (Cortex-M3), STM32F4 (Cortex-M4)。
*   **侧重**:
    *   高性能信号处理。
    *   复杂总线协议 (CAN, USB, FSMC)。
    *   标准库 (SPL) 与 HAL 库开发。
    *   RTOS 与高级人机交互 (HMI)。
*   **工具**: Keil MDK-ARM, STM32CubeMX, 串口调试助手.

### 🟠 `03_Linux_Vision` (高级应用 / 单板机)
*   **硬件平台**: 香橙派 (OrangePi), 树莓派 (Raspberry Pi) 等单板计算机。
*   **侧重**:
    *   嵌入式 Linux 环境搭建。
    *   机器视觉应用 (OpenCV, YOLO)。
    *   Python 硬件控制。
    *   物联网通信 (MQTT, Socket)。
*   **工具**: VS Code (Remote-SSH), Python 3.x.

---

## ⚙️ 环境配置指南

### 1. Keil uVision 配置 (针对 51 & STM32)
*   **安装**: 安装 **Keil uVision 5**。
*   **工具链**:
    *   `01_51_Series`: 需要安装 **C51** 工具链。
    *   `02_STM32_Series`: 需要安装 **MDK-ARM** 工具链。
*   **芯片包 (Pack)**:
    *   打开 Keil 中的 "Pack Installer"。
    *   安装你所使用芯片对应的 Device Family Pack (DFP) (例如 `Keil::STM32F1xx_DFP`)。
    *   *注意：必须安装 DFP 才能在工程中选择目标芯片。*
*   **编码**:
    *   建议检查编辑器的编码设置（配置 -> 编辑器），确保选择 "Encode in ANSI" (针对 GB2312) 或 "UTF-8"，防止中文注释乱码。

### 2. Proteus 仿真
*   **版本**: 建议使用 **Proteus 8.9** 或更高版本。
*   **固件关联**:
    1.  双击原理图中的单片机组件。
    2.  找到 `Program File` 选项。
    3.  浏览并选择 `Objects` 或 `Output` 文件夹下生成的 `.hex` 或 `.axf` 文件。

### 3. 硬件调试
*   **驱动**: 安装 **CH340** 或 **CP2102** USB 转串口驱动。
*   **调试器**: STM32 开发需安装 ST-Link 或 J-Link 驱动。
*   **工具**: 使用 XCOM, SSCOM, 或 MobaXterm 等串口助手进行数据调试。

---

## 📝 标准项目结构

为了保持整洁，项目通常遵循以下目录结构：

```text
Project_Root/
├── Core/               # 主程序逻辑 (main.c, 中断服务函数)
├── Drivers/            # 硬件抽象层/驱动 (BSP)
│   ├── Inc/            # 头文件 (.h)
│   └── Src/            # 源文件 (.c)
├── Simulation/         # Proteus 仿真工程文件 (.pdsprj)
├── Docs/               # 数据手册, 设计报告, 原理图
└── Output/             # 编译输出文件 (.hex, .axf)
```

---

## ⚠️ 常见问题排查

| 问题现象 | 可能原因 | 解决方法 |
| :--- | :--- | :--- |
| **Target not created** | 未勾选生成输出文件。 | 检查 "Options for Target" -> "Output" -> 勾选 "Create HEX File"。 |
| **L6235E: More than one...** | 工程中添加了重复文件。 | 在 Keil 工程目录树中检查并移除重复添加的源文件。 |
| **Simulation Crash (仿真崩溃)** | CPU 负载过高或版本不兼容。 | 增加仿真步长 (Timestep) 或尝试更换兼容的 Proteus 版本。 |
| **中文注释乱码** | 文件编码不匹配 (ANSI/UTF-8)。 | 修改 Keil 编辑器编码设置 (Configuration -> Editor -> Encoding)。 |

---

## 🤝 贡献
欢迎提交 Pull Request！提交代码时，请务必遵循现有的目录结构规范。

---
*维护者: qqwe7*
