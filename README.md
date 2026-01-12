# 嵌入式系统开发项目合集 (Embedded Systems Projects)

![Embedded](https://img.shields.io/badge/Embedded-Systems-blue.svg) ![License](https://img.shields.io/badge/License-MIT-green.svg) ![Update](https://img.shields.io/badge/Update-2026-orange.svg)

## 📖 简介 (Introduction)

本项目仓库是一个综合性的嵌入式系统资源库，旨在收录从基础教学实验到复杂工业原型、毕业设计的各类项目。内容涵盖了从底层的 **8-bit MCU** 控制，到高层的 **32-bit Cortex-M** 开发，以及基于 **Linux** 的智能嵌入式应用。

这些项目不仅包含完整的**源代码**，还附带了详细的**硬件仿真电路**、**设计文档**以及**实验报告**，非常适合嵌入式学习者参考和开发者二次开发。

---

## 📂 项目分类与技术细节 (Project Categories)

### 🟢 51单片机系列 (MCS-51 / 8-bit)
*   **定位**: 基础逻辑、外设驱动、工业控制入门。
*   **核心芯片**: AT89C51, STC89C52 等。
*   **技术栈**: 
    *   寄存器开发、汇编/C语言混编。
    *   定时器/计数器、外部中断处理。
    *   LCD1602/12864 显示驱动、数码管动态扫描、矩阵键盘。
    *   I2C/SPI 软件模拟协议。
*   **仿真**: 深度集成 Proteus 电路仿真。

### 🔵 STM32 系列 (ARM Cortex-M / 32-bit)
*   **定位**: 高性能控制、多任务处理、实时系统应用。
*   **核心芯片**: STM32F103 (C8T6/ZET6), STM32F407 (探索者/最小系统板) 等。
*   **技术栈**:
    *   **库文件**: 标准库 (Standard Peripheral Lib) 与 HAL 库开发。
    *   **外设**: ADC/DAC 高速采集、PWM 电机控制、DMA 内存传输。
    *   **总线**: CAN 总线通信、FSMC 屏幕驱动、USB 设备开发。
    *   **RTOS**: 部分项目集成 FreeRTOS 或 RT-Thread。

### 🟠 智能硬件与 Linux 视觉 (Advanced)
*   **定位**: 边缘计算、物联网网关、机器视觉。
*   **核心平台**: OrangePi (香橙派), Raspberry Pi, Jetson Nano。
*   **技术栈**:
    *   嵌入式 Linux 系统构建与 Shell 脚本。
    *   Python 驱动开发 (GPIO, I2C, UART)。
    *   OpenCV 图像处理、YOLO 目标检测模型部署。
    *   MQTT/HTTP 物联网协议接入云平台。

---

## 🛠️ 推荐开发环境 (Development Environment)

为了确保项目能够顺利运行，建议安装以下软件工具：

| 工具名称 | 推荐版本 | 用途 |
| :--- | :--- | :--- |
| **Keil uVision5** | C51 & MDK-ARM | 51 与 STM32 的代码编写与编译 |
| **Proteus** | 8.9 或更高 | 硬件电路仿真与代码联调 |
| **STM32CubeMX** | 最新版 | STM32 初始化代码生成与时钟配置 |
| **VS Code** | - | 代码阅读、Python 脚本开发 |
| **Serial Port Utility** | - | 串口调试工具，用于波形观察与数据交互 |

---

## 📁 标准化目录结构 (Directory Structure)

为了维护代码的长期可维护性，本仓库所有项目原则上遵循以下结构：

```text
Project_Name/
├── 内容 (Core_Assets)/
│   ├── 代码 (Source_Code)/      # Keil工程、源文件、头文件
│   ├── 仿真 (Simulation)/       # Proteus (.pdsprj) 或其他仿真文件
│   └── 文档 (Documentation)/    # 设计报告 (PDF/Word)、原理图、演示视频
└── 题目 (Requirements)/         # 项目任务书、原始需求说明、参考资料
```

---

## 🚀 快速上手指南

1.  **克隆仓库**:
    ```bash
    git clone https://github.com/qqwe7/Embedded-system-project-summarize.git
    ```
2.  **选择项目**: 进入对应的文件夹，查看 `README` 或 `使用说明`。
3.  **打开工程**:
    *   如果是 `.uvproj` 或 `.uvprojx`，请使用 Keil 打开。
    *   如果是 `.pdsprj`，请使用 Proteus 打开。
4.  **编译与运行**: 确保相关驱动库（如库函数路径）配置正确，编译无误后即可下载或启动仿真。

## ⚠️ 注意事项 (Disclaimers)

*   **路径问题**: 请不要在包含中文字符的路径下打开 Keil 工程，否则可能导致编译失败或路径报错。
*   **库支持**: STM32 项目可能需要对应的 Device Family Pack (DFP)，请通过 Keil Pack Installer 预先安装。
*   **版权声明**: 本项目集仅供个人学习与技术交流使用。

---

## 🤝 贡献与反馈

如果你有新的嵌入式项目想要分享，或者发现现有项目存在 Bug，欢迎通过以下方式参与：
- 提交 **Issue** 报告问题。
- 提交 **Pull Request** 贡献代码。

---
*Last Updated: 2026-01-13*