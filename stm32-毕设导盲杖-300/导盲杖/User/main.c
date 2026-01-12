/**
 ******************************************************************************
 * @file    main.c
 * @brief   主程序 
 ******************************************************************************
 */

#include "stm32f10x.h"
#include "Delay.h"
#include "pin_config.h"
#include "HCSR04.h"
#include "Buzzer.h"
#include "LED.h"
#include "USART.h"
#include "Key.h"
#include "OLED.h"
#include <stdio.h>

// ... 宏定义 ...
#define DEFAULT_ALARM_THRESHOLD     30
#define MIN_ALARM_THRESHOLD         5
#define MAX_ALARM_THRESHOLD         200
#define THRESHOLD_STEP              5
#define MEASURE_INTERVAL_MS         50      // 50ms采样
#define REPORT_INTERVAL_MS          200
#define USART_BAUDRATE              9600

// ... 全局变量 ...
float g_distance = 0;
uint16_t g_alarm_threshold = DEFAULT_ALARM_THRESHOLD;
uint8_t g_alarm_enable = 1;
uint8_t g_alarm_mode = 1;

static uint32_t g_report_timer = 0;
static uint32_t g_measure_timer = 0;
static uint32_t g_led_timer = 0;
static uint8_t g_display_need_update = 1; 

void System_Init(void);
void Distance_Measure(void);
void Alarm_Process(void);
void Key_Process(void);
void WIFI_Report(void);
void Update_Display(void);

int main(void)
{
    SystemInit();
    SystemCoreClockUpdate();
    Delay_Init();
    
    System_Init();
    
    USART1_SendString("System Start!\r\n");
    Buzzer_Beep(200);
    LED_On();
    Delay_ms(200); // 开机自检闪烁
    LED_Off();
    
    // 初始化界面
    OLED_Clear();
    OLED_ShowString(1, 1, "Ultrasonic Alarm");
    OLED_ShowString(2, 1, "Dist: 0.0cm");
    OLED_ShowString(3, 1, "Thres: 30cm");
    OLED_ShowString(4, 1, "Mode: S  Alm:ON");
    OLED_UpdateScreen();
    
    while(1)
    {
        Distance_Measure();   
        Alarm_Process();      
        Key_Process();        
        WIFI_Report();        
        
        // 集中刷新屏幕
        if(g_display_need_update)
        {
            Update_Display();
            OLED_UpdateScreen(); 
            g_display_need_update = 0;
        }
        
        Delay_ms(10);
    }
}

void Update_Display(void)
{
    char buf[16];
    
    // Line 2: Distance
    sprintf(buf, "%5.1fcm", g_distance);
    OLED_ShowString(2, 7, buf);
    
    // Line 3: Threshold
    sprintf(buf, "%3dcm  ", g_alarm_threshold);
    OLED_ShowString(3, 8, buf);
    
    // Line 4: Mode & Status
    OLED_ShowString(4, 7, (g_alarm_mode == 1) ? "S" : "M");
    OLED_ShowString(4, 14, g_alarm_enable ? "ON " : "OFF");
}

void Distance_Measure(void)
{
    g_measure_timer += 10;
    
    // 每50ms测量一次
    if(g_measure_timer >= MEASURE_INTERVAL_MS)
    {
        g_measure_timer = 0;
        float new_dist = HCSR04_GetDistance();
        
        if(new_dist < 0 || new_dist > 400) new_dist = 999.9;
        
        // 只要数值有变化（0.1cm精度），就更新显示
        if((int)(new_dist*10) != (int)(g_distance*10)) 
        {
            g_distance = new_dist;
            g_display_need_update = 1;
        }
    }
}

void Key_Process(void)
{
    uint8_t key = Key_Scan();
    
    if(key != KEY_NONE)
    {
        switch(key)
        {
            case KEY1_PRESSED: // 阈值++
                if(g_alarm_threshold + THRESHOLD_STEP <= MAX_ALARM_THRESHOLD)
                    g_alarm_threshold += THRESHOLD_STEP;
                break;
            case KEY2_PRESSED: // 阈值--
                if(g_alarm_threshold - THRESHOLD_STEP >= MIN_ALARM_THRESHOLD)
                    g_alarm_threshold -= THRESHOLD_STEP;
                break;
            case KEY3_PRESSED: // 切换报警模式
                g_alarm_mode = (g_alarm_mode == 1) ? 2 : 1;
                break;
            case KEY4_PRESSED: // 报警开关
                g_alarm_enable = !g_alarm_enable;
                break;
            case KEY5_PRESSED: // [新功能] 快速设为50cm
                g_alarm_threshold = 50;
                break;
            case KEY6_PRESSED: // [新功能] 快速设为100cm
                g_alarm_threshold = 100;
                break;
        }
        Buzzer_Beep(50);
        g_display_need_update = 1; 
    }
}

void WIFI_Report(void)
{
    char buf[64];
    g_report_timer += 10;
    if(g_report_timer >= REPORT_INTERVAL_MS)
    {
        g_report_timer = 0;
        sprintf(buf, "D:%.1f T:%d M:%d S:%d\r\n", g_distance, g_alarm_threshold, g_alarm_mode, g_alarm_enable);
        USART1_SendString(buf);
    }
}

void System_Init(void)
{
    HCSR04_Init();
    Buzzer_Init();
    LED_Init();
    USART1_Init(USART_BAUDRATE);
    Key_Init();
    OLED_Init(); 
}

void Alarm_Process(void)
{
    static uint8_t buzzer_cycle = 0;
    
    if(!g_alarm_enable)
    {
        Buzzer_Off();
        LED_Off();
        return;
    }
    
    if(g_distance > 0 && g_distance < g_alarm_threshold)
    {
        g_led_timer += 10;
        uint16_t blink_period = (g_alarm_mode == 1) ? 100 : 
            (g_distance < g_alarm_threshold/4) ? 50 : 
            (g_distance < g_alarm_threshold/2) ? 100 : 200;
        
        if(g_led_timer >= blink_period)
        {
            g_led_timer = 0;
            LED_Toggle();
        }
        
        buzzer_cycle++;
        if(buzzer_cycle >= 5)
        {
            buzzer_cycle = 0;
            Buzzer_Beep(20); 
        }
    }
    else
    {
        Buzzer_Off();
        LED_Off();
        g_led_timer = 0;
        buzzer_cycle = 0;
    }
}
