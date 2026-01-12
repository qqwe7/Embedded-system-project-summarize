# 嵌入式系统开发项目合集 (Embedded Systems Projects)

![Embedded](https://img.shields.io/badge/Embedded-Systems-blue.svg) ![License](https://img.shields.io/badge/License-MIT-green.svg) ![Update](https://img.shields.io/badge/Update-2026-orange.svg)

## 📖 简介 (Introduction)

本项目仓库是一个综合性的嵌入式系统资源库，旨在收录从基础教学实验到复杂工业原型、毕业设计的各类项目。内容涵盖了从底层的 **8-bit MCU** 控制，到高层的 **32-bit Cortex-M** 开发，以及基于 **Linux** 的智能嵌入式应用。

这些项目不仅包含完整的**源代码**，还附带了详细的**硬件仿真电路**、**设计文档**以及**实验报告**，非常适合嵌入式学习者参考和开发者二次开发。

---

## 📂 项目分类 (Project Categories)

项目文件已按照技术栈进行了物理分类，结构如下：

### 🟢 [01_51_Series](./01_51_Series) (MCS-51 / 8-bit)
*   **定位**: 基础逻辑、外设驱动、工业控制入门。
*   **核心芯片**: AT89C51, STC89C52 等。
*   **内容**:
    *   **数码管时钟/秒表**: 定时器、中断与动态扫描逻辑。
    *   **温室大棚监控**: 传感器读取与阈值报警系统。
    *   **数码管显示实验**: 基础 GPIO 控制。

### 🔵 [02_STM32_Series](./02_STM32_Series) (ARM Cortex-M / 32-bit)
*   **定位**: 高性能控制、多任务处理、实时系统应用。
*   **核心芯片**: STM32F103, STM32F407。
*   **内容**:
    *   **毕设导盲杖**: 多传感器融合与语音交互。
    *   **简易逻辑分析仪**: 信号采集与 Python 上位机通信。
    *   **F4探索者应用**: 屏幕驱动与标准库闹钟设计。

### 🟠 [03_Linux_Vision](./03_Linux_Vision) (Advanced)
*   **定位**: 边缘计算、物联网网关、机器视觉。
*   **核心平台**: OrangePi (香橙派)。
*   **内容**:
    *   **YOLO视觉识别**: 基于深度学习的目标检测毕设项目。

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
Category_Folder/             # e.g., 02_STM32_Series
├── Project_Name/            # 具体项目文件夹
│   ├── 内容 (Core_Assets)/
│   │   ├── 代码 (Source_Code)/      # Keil工程、源文件
│   │   ├── 仿真 (Simulation)/       # Proteus (.pdsprj)
│   │   └── 文档 (Documentation)/    # 报告、论文、视频
│   └── 题目 (Requirements)/         # 任务书、需求说明
```

---

## 🚀 快速上手指南

1.  **克隆仓库**:
    ```bash
    git clone https://github.com/qqwe7/Embedded-system-project-summarize.git
    ```
2.  **进入分类文件夹**: 根据你的需求进入 `01_51_Series`, `02_STM32_Series` 或 `03_Linux_Vision`。
3.  **打开工程**:
    *   如果是 `.uvproj` 或 `.uvprojx`，请使用 Keil 打开。
    *   如果是 `.pdsprj`，请使用 Proteus 打开。
4.  **编译与运行**: 确保相关驱动库（如库函数路径）配置正确，编译无误后即可下载或启动仿真。

---

## ⚠️ 注意事项 (Disclaimers)

*   **路径问题**: 请不要在包含中文字符的路径下打开 Keil 工程，否则可能导致编译失败或路径报错。
*   **库支持**: STM32 项目可能需要对应的 Device Family Pack (DFP)，请通过 Keil Pack Installer 预先安装。
*   **版权声明**: 本项目集仅供个人学习与技术交流使用。

---
*Last Updated: 2026-01-13*
