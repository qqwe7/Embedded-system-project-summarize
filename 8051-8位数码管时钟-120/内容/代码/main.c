/*******************************************************************************
 * 课题2：LED数码管显示的电子钟设计
 * 功能：
 *   1. 8位数码管显示时间 "时时:分分:秒秒"
 *   2. DS1302实时时钟芯片驱动
 *   3. 4个功能键设置时间（K1进入设置，K2设置小时，K3设置分钟，K4确认）
 *   4. 工作指示灯LED闪烁
 *   5. 闹钟功能（蜂鸣器）- K5设置闹钟小时，K6设置闹钟分钟
 ******************************************************************************/

#include "reg51.h"

#define uchar unsigned char
#define uint unsigned int

/*******************************************************************************
 * 引脚定义 
 ******************************************************************************/
// DS1302时钟芯片引脚
sbit DS1302_RST = P1^4;   // DS1302复位引脚
sbit DS1302_SCLK = P1^2;  // DS1302时钟引脚
sbit DS1302_IO = P1^3;    // DS1302数据引脚

// 按键引脚
sbit KEY1 = P3^0;  // K1 - 进入设置模式
sbit KEY2 = P3^1;  // K2 - 设置小时
sbit KEY3 = P3^2;  // K3 - 设置分钟
sbit KEY4 = P3^3;  // K4 - 确认设置
sbit KEY5 = P3^5;  // K5 - 设置闹钟小时
sbit KEY6 = P3^6;  // K6 - 设置闹钟分钟

// 蜂鸣器引脚
sbit BUZZER = P3^4;

// 继电器引脚 (通过三极管Q1控制)
sbit RELAY = P3^7;

// 工作指示灯
sbit LED_WORK = P1^7;

/*******************************************************************************
 * DS1302寄存器地址定义
 ******************************************************************************/
#define DS1302_SEC_W    0x80  // 秒寄存器写地址
#define DS1302_SEC_R    0x81  // 秒寄存器读地址
#define DS1302_MIN_W    0x82  // 分寄存器写地址
#define DS1302_MIN_R    0x83  // 分寄存器读地址
#define DS1302_HOUR_W   0x84  // 时寄存器写地址
#define DS1302_HOUR_R   0x85  // 时寄存器读地址
#define DS1302_WP_W     0x8E  // 写保护寄存器

/*******************************************************************************
 * 全局变量
 ******************************************************************************/
// 数码管段码表 (共阳极，0-9)
uchar code seg[] = {0xc0, 0xf9, 0xa4, 0xb0, 0x99, 0x92, 0x82, 0xf8, 0x80, 0x90};
// 分隔符 "-" 的段码 (共阳极)
#define SEG_DASH 0xbf

// 位选码表 (8位数码管，高电平选中)
uchar code wei[] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};

// 时间变量
uchar hour = 0;    // 小时 (0-23)
uchar minute = 0;  // 分钟 (0-59)
uchar second = 0;  // 秒 (0-59)

// 闹钟时间
uchar alarm_hour = 0;
uchar alarm_minute = 1;
uchar alarm_on = 1;      // 闹钟开关
uchar alarm_ringing = 0; // 闹钟正在响

// 显示缓冲区
uchar dispbuf[8];

// 当前显示位
uchar disp_index = 0;

// 模式变量: 0=正常显示, 1=设置小时, 2=设置分钟, 3=设置闹钟小时, 4=设置闹钟分钟
uchar mode = 0;

// 闪烁控制
uchar blink_cnt = 0;
uchar blink_on = 1;

// 100ms计数
uchar cnt_100ms = 0;

/*******************************************************************************
 * 延时函数
 ******************************************************************************/
void delay_ms(uint t)
{
    uint i, j;
    for(i = 0; i < t; i++)
        for(j = 0; j < 120; j++);
}

/*******************************************************************************
 * DS1302驱动函数
 ******************************************************************************/
// 向DS1302写入一个字节
void DS1302_WriteByte(uchar dat)
{
    uchar i;
    for(i = 0; i < 8; i++)
    {
        DS1302_SCLK = 0;
        DS1302_IO = dat & 0x01;
        dat >>= 1;
        DS1302_SCLK = 1;
    }
}

// 从DS1302读取一个字节
uchar DS1302_ReadByte(void)
{
    uchar i, dat = 0;
    for(i = 0; i < 8; i++)
    {
        dat >>= 1;
        DS1302_SCLK = 0;
        if(DS1302_IO)
            dat |= 0x80;
        DS1302_SCLK = 1;
    }
    return dat;
}

// 向DS1302指定地址写入数据
void DS1302_Write(uchar addr, uchar dat)
{
    DS1302_RST = 0;
    DS1302_SCLK = 0;
    DS1302_RST = 1;
    DS1302_WriteByte(addr);
    DS1302_WriteByte(dat);
    DS1302_RST = 0;
}

// 从DS1302指定地址读取数据
uchar DS1302_Read(uchar addr)
{
    uchar dat;
    DS1302_RST = 0;
    DS1302_SCLK = 0;
    DS1302_RST = 1;
    DS1302_WriteByte(addr);
    dat = DS1302_ReadByte();
    DS1302_RST = 0;
    return dat;
}

// DS1302初始化
void DS1302_Init(void)
{
    DS1302_RST = 0;
    DS1302_SCLK = 0;
    DS1302_Write(DS1302_WP_W, 0x00);  // 关闭写保护
    // 初始化时间为 00:00:00
    DS1302_Write(DS1302_SEC_W, 0x00);
    DS1302_Write(DS1302_MIN_W, 0x00);
    DS1302_Write(DS1302_HOUR_W, 0x00);
    DS1302_Write(DS1302_WP_W, 0x80);  // 开启写保护
}

// 设置DS1302时间
void DS1302_SetTime(uchar h, uchar m, uchar s)
{
    DS1302_Write(DS1302_WP_W, 0x00);  // 关闭写保护
    DS1302_Write(DS1302_SEC_W, ((s / 10) << 4) | (s % 10));  // BCD码
    DS1302_Write(DS1302_MIN_W, ((m / 10) << 4) | (m % 10));
    DS1302_Write(DS1302_HOUR_W, ((h / 10) << 4) | (h % 10));
    DS1302_Write(DS1302_WP_W, 0x80);  // 开启写保护
}

// 从DS1302读取时间
void DS1302_ReadTime(void)
{
    uchar tmp;
    
    tmp = DS1302_Read(DS1302_SEC_R);
    second = ((tmp >> 4) & 0x07) * 10 + (tmp & 0x0f);  // BCD转十进制
    
    tmp = DS1302_Read(DS1302_MIN_R);
    minute = ((tmp >> 4) & 0x07) * 10 + (tmp & 0x0f);
    
    tmp = DS1302_Read(DS1302_HOUR_R);
    hour = ((tmp >> 4) & 0x03) * 10 + (tmp & 0x0f);
}

/*******************************************************************************
 * 数码管显示函数
 ******************************************************************************/
// 更新显示缓冲区
void UpdateDispBuf(void)
{
    uchar disp_h, disp_m, disp_s;
    
    // 根据模式选择显示的内容
    if(mode == 3 || mode == 4)  // 显示闹钟时间
    {
        disp_h = alarm_hour;
        disp_m = alarm_minute;
        disp_s = 0;
    }
    else  // 显示当前时间
    {
        disp_h = hour;
        disp_m = minute;
        disp_s = second;
    }
    
    // 格式: HH-MM-SS
    if(mode == 0)  // 正常显示模式
    {
        dispbuf[0] = seg[disp_h / 10];
        dispbuf[1] = seg[disp_h % 10];
        dispbuf[2] = SEG_DASH;
        dispbuf[3] = seg[disp_m / 10];
        dispbuf[4] = seg[disp_m % 10];
        dispbuf[5] = SEG_DASH;
        dispbuf[6] = seg[disp_s / 10];
        dispbuf[7] = seg[disp_s % 10];
    }
    else if(mode == 1)  // 设置小时模式
    {
        if(blink_on)
        {
            dispbuf[0] = seg[disp_h / 10];
            dispbuf[1] = seg[disp_h % 10];
        }
        else
        {
            dispbuf[0] = 0xff;
            dispbuf[1] = 0xff;
        }
        dispbuf[2] = SEG_DASH;
        dispbuf[3] = seg[disp_m / 10];
        dispbuf[4] = seg[disp_m % 10];
        dispbuf[5] = SEG_DASH;
        dispbuf[6] = seg[disp_s / 10];
        dispbuf[7] = seg[disp_s % 10];
    }
    else if(mode == 2)  // 设置分钟模式
    {
        dispbuf[0] = seg[disp_h / 10];
        dispbuf[1] = seg[disp_h % 10];
        dispbuf[2] = SEG_DASH;
        if(blink_on)
        {
            dispbuf[3] = seg[disp_m / 10];
            dispbuf[4] = seg[disp_m % 10];
        }
        else
        {
            dispbuf[3] = 0xff;
            dispbuf[4] = 0xff;
        }
        dispbuf[5] = SEG_DASH;
        dispbuf[6] = seg[disp_s / 10];
        dispbuf[7] = seg[disp_s % 10];
    }
    else if(mode == 3)  // 设置闹钟小时模式
    {
        if(blink_on)
        {
            dispbuf[0] = seg[disp_h / 10];
            dispbuf[1] = seg[disp_h % 10];
        }
        else
        {
            dispbuf[0] = 0xff;
            dispbuf[1] = 0xff;
        }
        dispbuf[2] = SEG_DASH;
        dispbuf[3] = seg[disp_m / 10];
        dispbuf[4] = seg[disp_m % 10];
        dispbuf[5] = SEG_DASH;
        dispbuf[6] = seg[disp_s / 10];
        dispbuf[7] = seg[disp_s % 10];
    }
    else if(mode == 4)  // 设置闹钟分钟模式
    {
        dispbuf[0] = seg[disp_h / 10];
        dispbuf[1] = seg[disp_h % 10];
        dispbuf[2] = SEG_DASH;
        if(blink_on)
        {
            dispbuf[3] = seg[disp_m / 10];
            dispbuf[4] = seg[disp_m % 10];
        }
        else
        {
            dispbuf[3] = 0xff;
            dispbuf[4] = 0xff;
        }
        dispbuf[5] = SEG_DASH;
        dispbuf[6] = seg[disp_s / 10];
        dispbuf[7] = seg[disp_s % 10];
    }
}

// 数码管扫描显示
void Display(void)
{
    P0 = dispbuf[disp_index];
    P2 = wei[disp_index];
    
    disp_index++;
    if(disp_index >= 8) 
        disp_index = 0;
}

/*******************************************************************************
 * 按键扫描函数
 ******************************************************************************/
uchar KeyScan(void)
{
    static uchar key_up = 1;
    uchar key_val = 0;
    
    if(key_up && (KEY1 == 0 || KEY2 == 0 || KEY3 == 0 || KEY4 == 0 || KEY5 == 0 || KEY6 == 0))
    {
        delay_ms(10);  // 消抖
        key_up = 0;
        
        if(KEY1 == 0) key_val = 1;
        else if(KEY2 == 0) key_val = 2;
        else if(KEY3 == 0) key_val = 3;
        else if(KEY4 == 0) key_val = 4;
        else if(KEY5 == 0) key_val = 5;
        else if(KEY6 == 0) key_val = 6;
    }
    else if(KEY1 == 1 && KEY2 == 1 && KEY3 == 1 && KEY4 == 1 && KEY5 == 1 && KEY6 == 1)
    {
        key_up = 1;
    }
    
    return key_val;
}

// 处理按键
void ProcessKey(uchar key)
{
    if(key == 0) return;
    
    // 任意按键停止闹钟响铃
    if(alarm_ringing)
    {
        alarm_ringing = 0;
        BUZZER = 0;
        RELAY = 0;
        return;
    }
    
    switch(key)
    {
        case 1:  // K1 - 进入/切换设置时间模式
            if(mode == 0)
                mode = 1;  // 进入设置小时模式
            else if(mode == 1)
                mode = 2;  // 切换到设置分钟模式
            else if(mode == 2)
                mode = 0;  // 退出设置模式
            else if(mode == 3 || mode == 4)
                mode = 0;  // 从闹钟设置退出
            break;
            
        case 2:  // K2 - 设置小时 (加1)
            if(mode == 1)
            {
                hour++;
                if(hour >= 24) hour = 0;
            }
            break;
            
        case 3:  // K3 - 设置分钟 (加1)
            if(mode == 2)
            {
                minute++;
                if(minute >= 60) minute = 0;
            }
            break;
            
        case 4:  // K4 - 确认设置
            if(mode == 1 || mode == 2)
            {
                second = 0;
                DS1302_SetTime(hour, minute, second);
                mode = 0;
            }
            else if(mode == 3 || mode == 4)
            {
                alarm_on = 1;  // 确认并开启闹钟
                mode = 0;
            }
            break;
            
        case 5:  // K5 - 设置闹钟小时
            if(mode == 0)
            {
                mode = 3;  // 进入设置闹钟小时模式
            }
            else if(mode == 3)
            {
                alarm_hour++;
                if(alarm_hour >= 24) alarm_hour = 0;
            }
            else if(mode == 4)
            {
                mode = 3;  // 切换回设置闹钟小时
            }
            break;
            
        case 6:  // K6 - 设置闹钟分钟
            if(mode == 0)
            {
                mode = 4;  // 进入设置闹钟分钟模式
            }
            else if(mode == 4)
            {
                alarm_minute++;
                if(alarm_minute >= 60) alarm_minute = 0;
            }
            else if(mode == 3)
            {
                mode = 4;  // 切换到设置闹钟分钟
            }
            break;
    }
}

/*******************************************************************************
 * 闹钟检测
 ******************************************************************************/
void CheckAlarm(void)
{
    if(alarm_on && hour == alarm_hour && minute == alarm_minute && second == 0)
    {
        alarm_ringing = 1;
    }
}

/*******************************************************************************
 * 定时器初始化 - 2ms定时
 ******************************************************************************/
void Timer0_Init(void)
{
    TMOD |= 0x01;     // 定时器0，模式1
    TH0 = 0xF8;       // 定时2ms (12MHz晶振)
    TL0 = 0x30;
    ET0 = 1;
    TR0 = 1;
    EA = 1;
}

/*******************************************************************************
 * 定时器0中断服务函数 - 每2ms执行一次
 ******************************************************************************/
void Timer0_ISR(void) interrupt 1
{
    static uchar ms_cnt = 0;
    static uint buzzer_cnt = 0;
    
    TH0 = 0xF8;
    TL0 = 0x30;
    
    // 消隐
    P2 = 0x00;
    
    // 数码管扫描
    Display();
    
    ms_cnt++;
    
    // 每100ms
    if(ms_cnt >= 50)
    {
        ms_cnt = 0;
        cnt_100ms++;
        
        // 每500ms切换闪烁
        if(cnt_100ms % 5 == 0)
        {
            blink_on = !blink_on;
        }
        
        // 每1000ms
        if(cnt_100ms >= 10)
        {
            cnt_100ms = 0;
            
            // 工作指示灯闪烁
            LED_WORK = !LED_WORK;
            
            // 读取DS1302时间（仅在正常模式）
            if(mode == 0)
            {
                DS1302_ReadTime();
            }
            
            // 检查闹钟
            CheckAlarm();
        }
    }
    
    // 蜂鸣器控制
    if(alarm_ringing)
    {
        buzzer_cnt++;
        if(buzzer_cnt < 250)
        {
            BUZZER = 1;
            RELAY = 1;  // 继电器同时动作
        }
        else if(buzzer_cnt < 500)
        {
            BUZZER = 0;
            RELAY = 0;
        }
        else
            buzzer_cnt = 0;
    }
    else
    {
        BUZZER = 0;
        RELAY = 0;
        buzzer_cnt = 0;
    }
}

/*******************************************************************************
 * 主函数
 ******************************************************************************/
void main(void)
{
    uchar key;
    
    // 初始化IO
    P0 = 0xff;
    P2 = 0x00;
    BUZZER = 0;
    RELAY = 0;
    LED_WORK = 0;
    
    // 初始化DS1302
    DS1302_Init();
    DS1302_SetTime(0, 0, 0);
    
    // 默认闹钟时间 00:01
    alarm_hour = 0;
    alarm_minute = 1;
    alarm_on = 1;
    
    // 初始化显示
    UpdateDispBuf();
    
    // 初始化定时器
    Timer0_Init();
    
    while(1)
    {
        key = KeyScan();
        ProcessKey(key);
        UpdateDispBuf();
    }
}