/**
 ****************************************************************************************************
 * @file        main.c
 * @author      STM32F407综合实验
 * @version     V2.1
 * @date        2024-12-26
 * @brief       STM32F407 综合实验主程序 (含调试信息)
 ****************************************************************************************************
 * 
 * 【实验功能】
 * ============
 *   1. LCD显示 - 显示学院/专业/学号/姓名信息（全英文）
 *   2. RTC时钟 - 实时显示日期和时间（24小时制）
 *   3. 双闹钟  - 支持设置闹钟A和闹钟B，触发时蜂鸣器报警
 *   4. 呼吸灯  - LED0(PF9)通过PWM实现亮度渐变效果
 *   5. 输入捕获 - PA0捕获PWM信号，测量高电平时间
 *   6. 按键控制 - KEY0加快呼吸速度，KEY1减慢呼吸速度
 *   7. 独立看门狗 - IWDG超时1秒，TIM3中断500ms喂狗
 *
 * 【调试说明】
 * ============
 *   本版本代码添加了详细的调试输出，通过串口(115200波特率)可观察：
 *   - [INIT] 开头：初始化阶段信息
 *   - [LOOP] 开头：主循环执行信息
 *   - [ALARM] 开头：闹钟触发信息
 *   - [KEY] 开头：按键检测信息
 *   - [CAP] 开头：输入捕获信息
 *   - [PWM] 开头：呼吸灯信息
 *
 * 【硬件连接】
 * ============
 *   ★ 重要：使用杜邦线连接 PF9 (LED0) → PA0 (WK_UP)
 *
 ****************************************************************************************************
 */

#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/LED/led.h"
#include "./BSP/LCD/lcd.h"
#include "./USMART/usmart.h"
#include "./BSP/RTC/rtc.h"
#include "./BSP/BEEP/beep.h"
#include "./BSP/WDG/wdg.h"
#include "./BSP/KEY/key.h"
#include "./BSP/WDG/wdg.h"
#include "./BSP/TIMER/gtim.h"

/*===========================================================================*/
/*                          调试开关配置                                      */
/*===========================================================================*/
#define DEBUG_ENABLE        1       /* 1=开启调试输出, 0=关闭 */
#define DEBUG_INIT          1       /* 1=显示初始化调试 */
#define DEBUG_LOOP          0       /* 1=显示循环调试(数据量大) */
#define DEBUG_ALARM         1       /* 1=显示闹钟调试 */
#define DEBUG_KEY           1       /* 1=显示按键调试 */
#define DEBUG_CAPTURE       1       /* 1=显示捕获调试 */
#define DEBUG_PWM           0       /* 1=显示PWM调试(数据量大) */

#if DEBUG_ENABLE
    #define DBG_PRINT(fmt, ...)  printf(fmt, ##__VA_ARGS__)
#else
    #define DBG_PRINT(fmt, ...)
#endif

/*===========================================================================*/
/*                           用户信息配置区                                   */
/*              ★★★ 请修改以下内容为您的实际信息 ★★★                      */
/*===========================================================================*/
#define USER_COLLEGE    "College: CST"           /* 学院名称 */
#define USER_MAJOR      "Major: IoT"             /* 专业名称 */
#define USER_ID         "ID: 123456789"          /* 学号 */
#define USER_NAME       "Name: Student"          /* 姓名 */
/*===========================================================================*/

/* 调试计数器 - 用于跟踪各模块执行次数 */
volatile uint32_t g_debug_loop_cnt = 0;     /* 主循环计数 */
volatile uint32_t g_debug_rtc_cnt = 0;      /* RTC刷新次数 */
volatile uint32_t g_debug_pwm_cnt = 0;      /* PWM更新次数 */
volatile uint32_t g_debug_cap_cnt = 0;      /* 捕获完成次数 */
volatile uint32_t g_debug_key_cnt = 0;      /* 按键检测次数 */

/* 闹钟触发标志: 0-无闹钟, 1-闹钟A触发, 2-闹钟B触发 */
volatile uint8_t g_alarm_flag = 0;

/* 呼吸灯控制参数 */
uint16_t g_pwm_val = 0;         /* 当前PWM占空比 (范围: 0~500) */
uint8_t  g_pwm_dir = 1;         /* 渐变方向: 1=渐亮, 0=渐暗 */
uint16_t g_pwm_step = 10;       /* 步长: 10=慢速, 50=快速 */

/* 输入捕获变量 (定义在gtim.c中) */
extern uint8_t  g_timxchy_cap_sta;
extern uint32_t g_timxchy_cap_val;

/* 星期显示字符串 */
const char *weekday_str[] = {"", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};

/*===========================================================================*/
/*                            函数声明                                        */
/*===========================================================================*/
void display_user_info(void);
void display_rtc_time(void);
void alarm_check_and_handle(void);
void breathing_led_update(void);
void key_scan_and_handle(void);
void debug_print_status(void);

/**
 * @brief   显示用户信息
 */
void display_user_info(void)
{
#if DEBUG_INIT
    DBG_PRINT("[INIT] Displaying user info on LCD...\r\n");
    DBG_PRINT("[INIT]   %s\r\n", USER_COLLEGE);
    DBG_PRINT("[INIT]   %s\r\n", USER_MAJOR);
    DBG_PRINT("[INIT]   %s\r\n", USER_ID);
    DBG_PRINT("[INIT]   %s\r\n", USER_NAME);
#endif
    
    lcd_show_string(30, 30, 200, 16, 16, USER_COLLEGE, BLUE);
    lcd_show_string(30, 50, 200, 16, 16, USER_MAJOR, BLUE);
    lcd_show_string(30, 70, 200, 16, 16, USER_ID, BLUE);
    lcd_show_string(30, 90, 200, 16, 16, USER_NAME, BLUE);
}

/**
 * @brief   显示RTC时间
 */
void display_rtc_time(void)
{
    uint8_t hour, min, sec, ampm;
    uint8_t year, month, date, week;
    char tbuf[40];
    
    rtc_get_time(&hour, &min, &sec, &ampm);
    rtc_get_date(&year, &month, &date, &week);
    
    g_debug_rtc_cnt++;
    
#if DEBUG_LOOP
    DBG_PRINT("[LOOP] RTC Update #%lu: 20%02d-%02d-%02d %02d:%02d:%02d\r\n",
              g_debug_rtc_cnt, year, month, date, hour, min, sec);
#endif
    
    sprintf(tbuf, "20%02d-%02d-%02d %s", year, month, date, 
            (week >= 1 && week <= 7) ? weekday_str[week] : "???");
    lcd_show_string(30, 120, 260, 16, 16, tbuf, RED);
    
    sprintf(tbuf, "%02d:%02d:%02d", hour, min, sec);
    lcd_show_string(30, 140, 210, 24, 24, tbuf, RED);
}

/**
 * @brief   闹钟检测和处理
 */
void alarm_check_and_handle(void)
{
    static uint8_t alarm_beep_cnt = 0;
    
    if (g_alarm_flag != 0)
    {
#if DEBUG_ALARM
        DBG_PRINT("[ALARM] !!! Alarm %c Triggered !!!\r\n", 
                  (g_alarm_flag == 1) ? 'A' : 'B');
        DBG_PRINT("[ALARM] Beep counter: %d/30\r\n", alarm_beep_cnt);
#endif
        
        if (g_alarm_flag == 1)
        {
            lcd_show_string(30, 170, 260, 16, 16, ">>> Alarm A Triggered! <<<", MAGENTA);
        }
        else if (g_alarm_flag == 2)
        {
            lcd_show_string(30, 170, 260, 16, 16, ">>> Alarm B Triggered! <<<", MAGENTA);
        }
        
        BEEP(1);
        alarm_beep_cnt++;
        
        if (alarm_beep_cnt >= 30)
        {
#if DEBUG_ALARM
            DBG_PRINT("[ALARM] Beep stopped, alarm cleared\r\n");
#endif
            BEEP(0);
            g_alarm_flag = 0;
            alarm_beep_cnt = 0;
            lcd_fill(30, 170, 290, 186, WHITE);
        }
    }
}

/**
 * @brief   呼吸灯更新
 */
void breathing_led_update(void)
{
    g_debug_pwm_cnt++;
    
    if (g_pwm_dir)
    {
        if (g_pwm_val + g_pwm_step >= 500)
        {
            g_pwm_val = 500;
            g_pwm_dir = 0;
#if DEBUG_PWM
            DBG_PRINT("[PWM] Direction changed: DIMMING\r\n");
#endif
        }
        else
        {
            g_pwm_val += g_pwm_step;
        }
    }
    else
    {
        if (g_pwm_val <= g_pwm_step)
        {
            g_pwm_val = 0;
            g_pwm_dir = 1;
#if DEBUG_PWM
            DBG_PRINT("[PWM] Direction changed: BRIGHTENING\r\n");
#endif
        }
        else
        {
            g_pwm_val -= g_pwm_step;
        }
    }
    
    GTIM_TIMX_PWM_CHY_CCRX = g_pwm_val;
}

/**
 * @brief   按键扫描和处理
 */
void key_scan_and_handle(void)
{
    uint8_t key = key_scan(0);
    
    g_debug_key_cnt++;
    
    if (key == KEY0_PRES)
    {
        g_pwm_step = 50;
#if DEBUG_KEY
        DBG_PRINT("[KEY] KEY0 pressed -> Speed: FAST (step=50)\r\n");
#endif
        lcd_show_string(30, 190, 200, 16, 16, "Speed: FAST ", GREEN);
    }
    else if (key == KEY1_PRES)
    {
        g_pwm_step = 10;
#if DEBUG_KEY
        DBG_PRINT("[KEY] KEY1 pressed -> Speed: SLOW (step=10)\r\n");
#endif
        lcd_show_string(30, 190, 200, 16, 16, "Speed: SLOW ", CYAN);
    }
}

/**
 * @brief   每10秒输出一次调试状态摘要
 */
void debug_print_status(void)
{
#if DEBUG_ENABLE
    static uint32_t last_print = 0;
    
    if (g_debug_loop_cnt - last_print >= 1000)  /* 约10秒 */
    {
        last_print = g_debug_loop_cnt;
        printf("\r\n");
        printf("========== DEBUG STATUS ==========\r\n");
        printf("Loop Count:    %lu\r\n", g_debug_loop_cnt);
        printf("RTC Updates:   %lu\r\n", g_debug_rtc_cnt);
        printf("PWM Updates:   %lu\r\n", g_debug_pwm_cnt);
        printf("Capture Count: %lu\r\n", g_debug_cap_cnt);
        printf("Key Scans:     %lu\r\n", g_debug_key_cnt);
        printf("PWM Value:     %u (step=%u, dir=%s)\r\n", 
               g_pwm_val, g_pwm_step, g_pwm_dir ? "UP" : "DOWN");
        printf("Alarm Flag:    %u\r\n", g_alarm_flag);
        printf("==================================\r\n\r\n");
    }
#endif
}

/**
 * @brief   主函数
 */
int main(void)
{
    uint32_t t = 0;
    uint32_t high_time = 0;
    char buf[50];
    
    /*==================== 第1部分: 系统初始化 ====================*/
    sys_stm32_clock_init(336, 8, 2, 7);
    delay_init(168);
    usart_init(84, 115200);
    usmart_dev.init(84);
    
#if DEBUG_INIT
    printf("\r\n\r\n");
    printf("##################################################\r\n");
    printf("#   STM32F407 Comprehensive Experiment           #\r\n");
    printf("#   DEBUG MODE ENABLED                           #\r\n");
    printf("##################################################\r\n\r\n");
    
    DBG_PRINT("[INIT] System clock: 168MHz\r\n");
    DBG_PRINT("[INIT] USART1: 115200 baud\r\n");
    DBG_PRINT("[INIT] USMART initialized\r\n");
#endif
    
    /*==================== 第2部分: 外设初始化 ====================*/
#if DEBUG_INIT
    DBG_PRINT("[INIT] Initializing peripherals...\r\n");
#endif
    
    led_init();
#if DEBUG_INIT
    DBG_PRINT("[INIT]   LED initialized (PF9, PF10)\r\n");
#endif
    
    beep_init();
#if DEBUG_INIT
    DBG_PRINT("[INIT]   BEEP initialized (PF8)\r\n");
#endif
    
    key_init();
#if DEBUG_INIT
    DBG_PRINT("[INIT]   KEY initialized (PE3, PE4)\r\n");
#endif
    
    lcd_init();
#if DEBUG_INIT
    DBG_PRINT("[INIT]   LCD initialized\r\n");
#endif
    
    /*==================== 第3部分: RTC初始化 ====================*/
    rtc_init();
#if DEBUG_INIT
    DBG_PRINT("[INIT]   RTC initialized\r\n");
#endif
    
    rtc_set_wakeup(4, 0);
#if DEBUG_INIT
    DBG_PRINT("[INIT]   RTC wakeup configured (1 second)\r\n");
#endif
    
    rtc_set_alarma(1, 12, 0, 0);
#if DEBUG_INIT
    DBG_PRINT("[INIT]   Alarm A set: Mon 12:00:00\r\n");
#endif
    
    /*==================== 第4部分: 定时器初始化 ====================*/
    gtim_timx_int_init(5000 - 1, 8400 - 1);
#if DEBUG_INIT
    DBG_PRINT("[INIT]   TIM3 initialized (500ms interrupt)\r\n");
#endif
    
    /* 独立看门狗初始化: 分频系数4 (64分频), 重载值500
     * 超时时间 = (4 * 2^4) * 500 / 32000 ≈ 1秒
     * TIM3中断每500ms喂狗一次，确保不会超时复位 */
    iwdg_init(4, 500);
#if DEBUG_INIT
    DBG_PRINT("[INIT]   IWDG initialized (1s timeout, fed every 500ms)\r\n");
#endif
    
    gtim_timx_pwm_chy_init(500 - 1, 84 - 1);
#if DEBUG_INIT
    DBG_PRINT("[INIT]   TIM14 PWM initialized (2KHz, PF9)\r\n");
#endif
    
    gtim_timx_cap_chy_init(0xFFFF, 84 - 1);
#if DEBUG_INIT
    DBG_PRINT("[INIT]   TIM5 Capture initialized (1us, PA0)\r\n");
#endif
    
    /*==================== 第5部分: LCD显示初始化 ====================*/
    lcd_clear(WHITE);
    display_user_info();
    lcd_show_string(30, 190, 200, 16, 16, "Speed: SLOW ", CYAN);
    lcd_show_string(30, 210, 200, 16, 16, "High: ---- us", DARKBLUE);
    
#if DEBUG_INIT
    printf("\r\n");
    DBG_PRINT("[INIT] ========================================\r\n");
    DBG_PRINT("[INIT] All initialization complete!\r\n");
    DBG_PRINT("[INIT] Entering main loop...\r\n");
    DBG_PRINT("[INIT] ========================================\r\n");
    printf("\r\n");
    printf(">>> Connect PF9 -> PA0 for PWM capture! <<<\r\n\r\n");
#endif
    
    /*==================== 第6部分: 主循环 ====================*/
    while (1)
    {
        t++;
        g_debug_loop_cnt++;
        
        /* 每100ms: 更新时间和检查捕获 */
        if ((t % 10) == 0)
        {
            display_rtc_time();
            
            if (g_timxchy_cap_sta & 0x80)
            {
                high_time = g_timxchy_cap_sta & 0x3F;
                high_time *= 65536;
                high_time += g_timxchy_cap_val;
                
                g_debug_cap_cnt++;
                
#if DEBUG_CAPTURE
                DBG_PRINT("[CAP] Capture #%lu: High=%lu us\r\n", 
                          g_debug_cap_cnt, high_time);
#endif
                
                sprintf(buf, "High: %lu us   ", high_time);
                lcd_show_string(30, 210, 200, 16, 16, buf, DARKBLUE);
                
                g_timxchy_cap_sta = 0;
            }
        }
        
        /* 每100ms: 检测闹钟 */
        if ((t % 10) == 0)
        {
            alarm_check_and_handle();
        }
        
        /* 每20ms: 更新呼吸灯 */
        if ((t % 2) == 0)
        {
            breathing_led_update();
        }
        
        /* 每10ms: 扫描按键 */
        key_scan_and_handle();
        
        /* 定期输出调试状态 */
        debug_print_status();
        
        delay_ms(10);
    }
}
