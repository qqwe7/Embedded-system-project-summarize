# 嵌入式系统项目实战合集 (Embedded Systems Projects)

> 🇺🇸 [English Version (英文版)](README_EN.md)

![Platform](https://img.shields.io/badge/Platform-51%20%7C%20STM32%20%7C%20Linux-blue) ![Language](https://img.shields.io/badge/Language-C%20%7C%20C%2B%2B%20%7C%20Python-green) ![License](https://img.shields.io/badge/License-MIT-green)

## 📖 项目简介 (Introduction)
本项目仓库致力于构建一个**模块化、规范化**的嵌入式开发资源库。内容不仅包含基础的裸机驱动，还涵盖了 RTOS 实时系统、机器视觉算法以及物联网通信等高级应用。

无论是用于**课程设计参考**、**毕业设计选题**，还是**工程项目复用**，你都能在这里找到高质量的源码和文档。

---

## 📂 核心分类与技术详解 (Architecture & Tech Stack)

### 🟢 1. MCS-51 单片机系列 (`01_51_Series`)
> **定位**：嵌入式入门基石，重点掌握寄存器操作与时序逻辑。

*   **核心芯片**: STC89C52RC, AT89C51。
*   **典型项目**:
    *   **多功能数字钟**: 结合定时器中断与数码管动态扫描，实现精准计时与闹钟功能。
    *   **温室环境监控**: 集成 DS18B20 (温度) 与 ADC0832 (湿度/光照模拟)，实现阈值报警。
    *   **交通灯控制系统**: 模拟十字路口红绿灯逻辑，涉及复杂的有限状态机 (FSM) 设计。
*   **开发环境**: Keil C51 (uVision 5), Proteus 8.9+ (仿真)。

### 🔵 2. STM32 Cortex-M 系列 (`02_STM32_Series`)
> **定位**: 工业级控制核心，重点掌握总线协议、DMA 与操作系统。

*   **核心芯片**: STM32F103 (标准型), STM32F407 (高性能型)。
*   **技术亮点**:
    *   **驱动开发**: 涵盖标准库 (SPL) 与 HAL 库两种开发模式。
    *   **总线与外设**: 深入 I2C (OLED屏幕), SPI (W25Qxx Flash), CAN (车载通信), TIM (PWM电机控制)。
    *   **系统级应用**: 
        *   **RTOS**: 移植 FreeRTOS/RT-Thread 实现多任务调度。
        *   **FATFS**: SD 卡文件系统读写。
*   **开发环境**: Keil MDK-ARM, STM32CubeMX, Serial Port Utility。

### 🟠 3. Linux 嵌入式与机器视觉 (`03_Linux_Vision`)
> **定位**: 边缘计算与人工智能，重点掌握 Linux 系统编程与 AI 模型部署。

*   **核心平台**: 香橙派 (OrangePi), 树莓派 (Raspberry Pi)。
*   **核心技术**:
    *   **AI 部署**: 使用 YOLOv5/v8 进行实时目标检测（如火焰识别、口罩佩戴检测）。
    *   **图像处理**: OpenCV-Python 进行颜色追踪、轮廓提取。
    *   **网络通信**: MQTT 协议接入云平台 (阿里云/腾讯云)，Socket 局域网传输。
*   **开发环境**: VS Code (Remote-SSH), Mobaxterm (终端管理)。

---

## 🛠️ 详细环境搭建与使用指南 (Setup Guide)

为了确保项目能够顺利编译和运行，请严格按照以下步骤配置环境：

### 第一步：工具链安装
1.  **Keil uVision 5**:
    *   开发 51 单片机请安装 **C51** 核心。
    *   开发 STM32 请安装 **MDK-ARM** 核心。
    *   *注意：两者可安装在同一目录下实现共存。*
2.  **芯片支持包 (Device Pack)**:
    *   打开 Keil 中的 `Pack Installer`，下载并安装对应芯片的 DFP 包（如 `Keil::STM32F1xx_DFP`）。
    *   如果不安装，打开工程时会提示 "Device not found"。

### 第二步：Proteus 仿真配置
大多数项目包含 `.pdsprj` 仿真文件，使用方法如下：
1.  使用 **Proteus 8.9** 或更高版本打开工程（低版本可能无法打开）。
2.  双击原理图中的单片机芯片。
3.  在 **Program File** 一栏，点击文件夹图标。
4.  浏览到项目的 `Objects` 或 `Output` 目录，选择编译好的 `.hex` 文件。
5.  点击左下角的 **运行/Play** 按钮开始仿真。

### 第三步：解决编译错误
*   **错误**: `Target not created`
    *   **原因**: 未勾选生成 HEX 文件。
    *   **解决**: 点击魔术棒图标 (Options for Target) -> Output -> 勾选 `Create HEX File`。
*   **错误**: `L6235E: More than one section...`
    *   **原因**: 工程目录树中重复添加了同名文件。
    *   **解决**: 在左侧 Project 窗口中仔细检查，移除重复的 `.c` 文件。

---

## 📝 目录结构规范 (Project Structure)

本仓库采用统一的工程结构，便于阅读与移植：

```text
Project_Name/
├── Core/ (或 User)     # 核心应用层
│   ├── main.c          # 主函数入口
│   ├── stm32f10x_it.c  # 中断服务函数
│   └── FreeRTOS_Conf.h # 系统配置文件
├── Drivers/ (或 Hardware)# 硬件驱动层 (BSP)
│   ├── led.c           # LED 驱动源文件
│   ├── led.h           # LED 驱动头文件
│   └── oled.c          # OLED 屏幕驱动
├── Simulation/         # 仿真文件
│   └── circuit.pdsprj  # Proteus 电路原理图
├── Output/             # 编译输出
│   └── project.hex     # 可烧录文件
└── Docs/               # 文档
    ├── 原理图.pdf
    └── 设计报告.docx
```

---

## ⚠️ 常见问题 FAQ

| 问题描述 | 解决方案 |
| :--- | :--- |
| **打开工程全是乱码** | Keil 编辑器编码设置问题。请尝试在 Edit -> Configuration -> Editor -> Encoding 中切换 **ANSI** 或 **UTF-8**。 |
| **Proteus 仿真运行极慢** | 这是复杂电路仿真的正常现象。可以尝试关闭电脑后台高负载软件，或在仿真属性中调整时钟频率。 |
| **ST-Link 无法下载** | 检查接线 (SWDIO, SWCLK, GND, 3.3V) 是否正确；检查 Keil 中 Debug 选项卡是否选择了 ST-Link Debugger。 |

---

## 🤝 参与贡献 (Contributing)
我们非常欢迎您提交优质的嵌入式项目！
1.  Fork 本仓库。
2.  新建分支 (`git checkout -b new-project`).
3.  提交代码 (`git commit -m "Add new STM32 project"`).
4.  推送到远程 (`git push origin new-project`).
5.  提交 Pull Request。

---
*维护者: qqwe7*
