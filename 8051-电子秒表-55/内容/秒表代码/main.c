#include "reg51.h"

// --- 类型定义 ---
#define uchar unsigned char
#define uint unsigned int

// --- 引脚定义 ---
// 按键
sbit k1 = P3^2;    // 复位按键：按下后秒数归零
sbit k2 = P3^4;    // 开始/暂停按键：切换计时状态

// 数码管位选
sbit smg1 = P2^0;  // 数码管十位 位选控制
sbit smg2 = P2^1;  // 数码管个位 位选控制

// --- 全局变量 ---
// 共阴极数码管段码表 0-9
uchar code smgduan[10] = {
    0x3f, 0x06, 0x5b, 0x4f, 0x66, 
    0x6d, 0x7d, 0x07, 0x7f, 0x6f
};

uchar time = 0; // 定时器中断计数 (50ms * 20 = 1s)
uchar miao = 0; // 当前显示的秒数 (0-99)
uchar flag = 0; // 运行状态标志 (0:暂停, 1:计时)

/**
 * @brief 简单的软件延时函数
 * @param i 延时循环次数
 */
void delay(uchar i)
{
    while(i--);
}

/**
 * @brief 主程序
 */
void main()
{
    // --- 1. 定时器0 初始化 ---
    TMOD |= 0X01;  // 设置定时器0为工作方式1 (16位)
    TH0 = 0X3C;    // 设置定时初值高8位
    TL0 = 0XB0;    // 设置定时初值低8位 (0x3CB0 => 50ms @ 12MHz)
    
    ET0 = 1;       // 开启定时器0中断
    EA = 1;        // 开启总中断
    TR0 = 1;       // 启动定时器0

    // --- 2. 主循环 ---
    while(1)
    {
        // === 数码管动态扫描 ===
        
        // 显示十位
        P0 = smgduan[miao / 10]; // 取十位段码
        smg1 = 0;                // 选中十位 (低电平有效)
        delay(100);              // 延时保持亮度
        smg1 = 1;                // 消隐

        // 显示个位
        P0 = smgduan[miao % 10]; // 取个位段码
        smg2 = 0;                // 选中个位
        delay(100);              // 延时保持亮度
        smg2 = 1;                // 消隐

        // === 按键检测 ===
        
        // 复位功能
        if(!k1)
        {
            miao = 0; // 秒数归零
            flag = 0; // 暂停计时
        }

        // 开始/暂停功能
        if(!k2)
        {
            delay(10); // 简单消抖
            if(!k2)    // 确认按键按下
            {
                flag = !flag; // 切换状态
                while(!k2);   // 等待按键释放
            }
        }
    }
}

/**
 * @brief 定时器0 中断服务函数
 * 每 50ms 进入一次中断
 */
void Timer0() interrupt 1 
{
    // 重装载定时器初值
    TH0 = 0X3C;
    TL0 = 0XB0;

    // 计时逻辑
    if(time < 20) // 未满1秒 (20 * 50ms = 1000ms)
    {
        time++;
    }
    else
    {
        time = 0; // 1秒时间到
        
        // 如果处于运行状态，则秒数增加
        if(flag)
        {
            if(miao < 99)
            {
                miao++;
            }
            else
            {
                miao = 0; // 满99秒后归零
            }
        }
    }
}
