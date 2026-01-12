#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "lcd.h"
#include "usmart.h"
#include "rtc.h"
#include "key.h"
#include "beep.h"
#include "stdio.h"
#include "student_info.h"
#include "display_str.h"

/* ============ 调试串口输出配置 ============ */
#define DEBUG_SERIAL    1

#if DEBUG_SERIAL
#define DEBUG_PRINT(fmt, ...)  printf(fmt, ##__VA_ARGS__)
#else
#define DEBUG_PRINT(fmt, ...)
#endif
/* =========================================== */

/* 蜂鸣器响铃时长 (单位: 10ms, 300 = 3秒) */
#define BEEP_DURATION   300

/* 动画帧切换间隔 (单位: 10ms, 50 = 500ms) */
#define VIDEO_FRAME_INTERVAL  50

/* 闹钟图标显示位置 - 调整以防止重叠 */
#define ALARM_ICON_X    30
#define ALARM_ICON_Y    200
#define ALARM_ICON_W    100
#define ALARM_ICON_H    100

/* 动画显示位置 */
#define VIDEO_X         350
#define VIDEO_Y         160
#define VIDEO_W         100
#define VIDEO_H         100

/* 全局变量 */
u8 alarm_triggered = 0;       /* 闹钟触发标志 */
u16 beep_counter = 0;         /* 蜂鸣器计数器 */
u8 set_mode = 0;              /* 时间设置模式标志 */
u8 new_hour = 0;              /* 设置模式下的临时小时 */
u8 new_min = 0;               /* 设置模式下的临时分钟 */

/* 闹钟触发状态记录 (防止一秒内重复触发) */
u8 alarm_a_triggered_this_second = 0;
u8 alarm_b_triggered_this_second = 0;
/* 上一次处理的秒数，用于检测秒更迭 */
u8 last_second = 0xFF;

/* 视频帧循环变量 */
u8 current_frame = 0;
u16 video_timer = 0;

/* 系统心跳LED计数器 */
u16 heartbeat_counter = 0;
/* 周期状态打印计数器 */
u16 status_print_counter = 0;

/* 外部声明 (pic.c中定义) - 10帧视频 */
extern const unsigned short video_frame_1[10000];
extern const unsigned short video_frame_2[10000];
extern const unsigned short video_frame_3[10000];
extern const unsigned short video_frame_4[10000];
extern const unsigned short video_frame_5[10000];
extern const unsigned short video_frame_6[10000];
extern const unsigned short video_frame_7[10000];
extern const unsigned short video_frame_8[10000];
extern const unsigned short video_frame_9[10000];
extern const unsigned short video_frame_10[10000];
extern const unsigned short* video_frames[10];
extern void Display_VideoFrame(u16 x, u16 y, u16 width, u16 height, u8 frame_index);
/* 闹钟图标 */
extern const unsigned char gImage_R[20000];

int main(void)
{ 
    RTC_TimeTypeDef RTC_TimeStruct;
    RTC_DateTypeDef RTC_DateStruct;
    RTC_AlarmTypeDef RTC_AlarmStruct_A;
    RTC_AlarmTypeDef RTC_AlarmStruct_B;

    u8 tbuf[50];
    u8 key = 0;
    u8 str_buf[100]; 

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    delay_init(168);      
    uart_init(115200);    
    
    DEBUG_PRINT("\r\n");
    DEBUG_PRINT("==========================================\r\n");
    DEBUG_PRINT(" RTC Alarm System - STM32F407 (V2 Fix)\r\n");
    DEBUG_PRINT(" Build: %s %s\r\n", __DATE__, __TIME__);
    DEBUG_PRINT("==========================================\r\n");
    
    usmart_dev.init(84);  
    LED_Init();           
    LCD_Init();           
    KEY_Init();           
    BEEP_Init();          
    My_RTC_Init();
    
    /* 关闭RTC中断，使用轮询方式检测闹钟 */
    RTC_ITConfig(RTC_IT_ALRA, DISABLE);
    RTC_ITConfig(RTC_IT_ALRB, DISABLE);

    LCD_Display_Dir(1);  /* 横屏显示 */
    RTC_Set_WakeUp(RTC_WakeUpClock_CK_SPRE_16bits, 0); 
    
    POINT_COLOR = RED;      
    
    /* ============================================================ */
    /* LCD界面初始化                                                */
    /* ============================================================ */
    
    /* 标题 */
    LCD_ShowChinese(120, 30, 240, 32, "嵌入式系统期末大作业", BLACK, WHITE, 16, 0);
    
    /* 信息框：调整高度以容纳4行信息 (60-160) */
    LCD_DrawRectangle(20, 60, 470, 160); 
    LCD_DrawLine(290, 60, 290, 160); 

    /* 显示学生信息 - 4行布局 */
    /* 1. 学院 */
    
    /* 2. 班级 */
    sprintf((char*)str_buf, "班级：%s", STUDENT_MAJOR);
    Show_Str(30, 70, (u8*)str_buf, BLACK, WHITE, 16, 0);
    
    /* 3. 姓名 */
    sprintf((char*)str_buf, "姓名：%s", STUDENT_NAME);
    Show_Str(30, 90, (u8*)str_buf, BLACK, WHITE, 16, 0);
    
    /* 4. 学号 */
    sprintf((char*)str_buf, "学号：%s", STUDENT_ID);
    Show_Str(30, 110, (u8*)str_buf, BLACK, WHITE, 16, 0);
    
    /* 提示语已删除 */
    
    BEEP = 0;

    while(1) 
    {        
        /* 1. 按键扫描 */
        key = KEY_Scan(0);
        
        if(key && alarm_triggered) 
        {
            BEEP = 0;
            alarm_triggered = 0;
            beep_counter = 0;
            /* 
             * 修复：清除整个底部区域 (X:0-480, Y:180-320)
             * 确保消除所有闹钟文字和图片残留
             */
            LCD_Fill(0, 180, 480, 320, WHITE); 
            DEBUG_PRINT("[ALARM] Alarm cleared by key press!\r\n");
        }
        
        /* 设置时间模式逻辑 */
        if(!set_mode && key == 1) 
        {
            static u16 long_cnt = 0;
            if(KEY0 == 0) 
            {
                if(++long_cnt > 100) 
                {
                    long_cnt = 0;
                    set_mode = 1;
                    RTC_GetTime(RTC_Format_BIN, &RTC_TimeStruct);
                    new_hour = RTC_TimeStruct.RTC_Hours;
                    new_min  = RTC_TimeStruct.RTC_Minutes;
                    
                    LCD_Fill(10, 160, 300, 200, WHITE);
                    Show_Str(10, 160, "设置时间: KEY1=时+ KEY2=分+ WK_UP=确认", RED, WHITE, 12, 0);
                }
            }
            else 
            {
                long_cnt = 0;
            }
        }

        if(set_mode)
        {
            if(key == 2) new_hour = (new_hour + 1) % 24;
            if(key == 3) new_min = (new_min + 1) % 60;
            if(key == 4) 
            {
                rtc_set_time(new_hour, new_min, 0);
                set_mode = 0;
                LCD_Fill(10, 160, 300, 200, WHITE);
                Show_Str(10, 160, "时间已更新!", GREEN, WHITE, 12, 0);
            }
            
            sprintf((char*)tbuf, ">>> %02d:%02d <<<", new_hour, new_min);
            LCD_ShowString(10, 180, 240, 16, 16, tbuf);
            delay_ms(100);
        }

        /* 2. 时间显示与闹钟 */
        RTC_GetTime(RTC_Format_BIN, &RTC_TimeStruct);
        RTC_GetDate(RTC_Format_BIN, &RTC_DateStruct);

        if (last_second != RTC_TimeStruct.RTC_Seconds)
        {
            last_second = RTC_TimeStruct.RTC_Seconds;
            alarm_a_triggered_this_second = 0;
            alarm_b_triggered_this_second = 0;
            
            /* 显示时间、日期、星期 */
            sprintf((char*)tbuf, "时间:%02d:%02d:%02d", RTC_TimeStruct.RTC_Hours, RTC_TimeStruct.RTC_Minutes, RTC_TimeStruct.RTC_Seconds); 
            Show_Str(300, 110, (u8*)tbuf, BLACK, WHITE, 16, 0);    
            
            sprintf((char*)tbuf, "日期:20%02d-%02d-%02d", RTC_DateStruct.RTC_Year, RTC_DateStruct.RTC_Month, RTC_DateStruct.RTC_Date); 
            Show_Str(300, 70, (u8*)tbuf, BLACK, WHITE, 16, 0);    
            
            sprintf((char*)tbuf, "星期:%d", RTC_DateStruct.RTC_WeekDay); 
            Show_Str(300, 90, (u8*)tbuf, BLACK, WHITE, 16, 0);

            RTC_GetAlarm(RTC_Format_BIN, RTC_Alarm_A, &RTC_AlarmStruct_A);
            RTC_GetAlarm(RTC_Format_BIN, RTC_Alarm_B, &RTC_AlarmStruct_B);
            
            /* Check Alarm A */
            if(RTC_TimeStruct.RTC_Hours == RTC_AlarmStruct_A.RTC_AlarmTime.RTC_Hours &&
               RTC_TimeStruct.RTC_Minutes == RTC_AlarmStruct_A.RTC_AlarmTime.RTC_Minutes &&
               RTC_TimeStruct.RTC_Seconds == RTC_AlarmStruct_A.RTC_AlarmTime.RTC_Seconds)
            {
                if(1)
                {
                     if(!alarm_a_triggered_this_second) {
                         alarm_triggered = 1;
                         alarm_a_triggered_this_second = 1;
                         
                         /* 触发时先清空底部显示区域，避免与旧图像或鬼影重叠 */
                         LCD_Fill(0, 180, 480, 320, WHITE);
                         
                         Show_Str(30, 180, "闹钟A响铃!", RED, WHITE, 16, 0); 
                         LCD_ShowPicture(ALARM_ICON_X, ALARM_ICON_Y, ALARM_ICON_W, ALARM_ICON_H, (u8*)gImage_R);
                     }
                }
            }

            /* Check Alarm B */
            if(RTC_TimeStruct.RTC_Hours == RTC_AlarmStruct_B.RTC_AlarmTime.RTC_Hours &&
               RTC_TimeStruct.RTC_Minutes == RTC_AlarmStruct_B.RTC_AlarmTime.RTC_Minutes &&
               RTC_TimeStruct.RTC_Seconds == RTC_AlarmStruct_B.RTC_AlarmTime.RTC_Seconds)
            {
                 if(!alarm_b_triggered_this_second) {
                     alarm_triggered = 1;
                     alarm_b_triggered_this_second = 1;
                     
                     /* 触发时先清空底部显示区域 */
                     LCD_Fill(0, 180, 480, 320, WHITE);
                     
                     Show_Str(180, 180, "闹钟B响铃!", RED, WHITE, 16, 0); 
                     LCD_ShowPicture(ALARM_ICON_X + 150, ALARM_ICON_Y, ALARM_ICON_W, ALARM_ICON_H, (u8*)gImage_R);
                 }
            }
        }
        
        if(alarm_triggered)
        {
            beep_counter++;
            if(beep_counter < BEEP_DURATION) BEEP = 1; 
            else BEEP = 0;
        }
        else BEEP = 0;

        video_timer++;
        if(video_timer > VIDEO_FRAME_INTERVAL)
        {
            current_frame = (current_frame + 1) % 10;
            video_timer = 0;
            Display_VideoFrame(VIDEO_X, VIDEO_Y, VIDEO_W, VIDEO_H, current_frame);
        }

        heartbeat_counter++;
        if(heartbeat_counter >= 30) 
        {
            heartbeat_counter = 0;
            LED0 = !LED0; 
        }
        
        delay_ms(10);
    } 
}
