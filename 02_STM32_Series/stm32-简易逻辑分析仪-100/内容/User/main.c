/**
  ******************************************************************************
  * @file    main.c
  * @brief   简易逻辑分析仪 - 主程序
  * @note    适用于 STM32F103C8T6 / GD32F103RCT6
  * @author  123
  ******************************************************************************
  */

#include "stm32f10x.h"
#include "Serial.h"
#include "LogicAnalyzer.h"
#include "CommandParser.h"

/**
  * @brief  初始化测试信号输出 (PB1 - TIM3_CH4 PWM)
  * @note   用于自测：将 PB1 连接到 PA0，可以采集到 1kHz 方波
  * @param  无
  * @retval 无
  */
static void TestSignal_Init(void)
{
    /* 开启时钟 */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
    
    /* GPIO 配置 - PB1 复用推挽输出 */
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;  // 改为 PB1
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    /* TIM3 时基配置 - 产生 1kHz PWM */
    /* 72MHz / 72 / 1000 = 1kHz */
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_Period = 999;           // ARR = 999
    TIM_TimeBaseStructure.TIM_Prescaler = 71;         // PSC = 71
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
    
    /* PWM 配置 - 通道4 (PB1) */
    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_OCStructInit(&TIM_OCInitStructure);  // 先初始化默认值
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 500;              // CCR = 500, 占空比 50%
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OC4Init(TIM3, &TIM_OCInitStructure);  // 改为 OC4
    
    TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);  // 改为 OC4
    TIM_ARRPreloadConfig(TIM3, ENABLE);
    
    /* 启动 TIM3 */
    TIM_Cmd(TIM3, ENABLE);
    
    Serial_SendString("[DEBUG] TIM3 PWM on PB1 initialized (1kHz)\r\n");
}

/**
  * @brief  PC13 LED 闪烁测试 (备用测试方法)
  * @note   将 PC13 连接到 PA0 也可以测试
  * @param  无
  * @retval 无
  */
static void LED_Toggle_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
}

/**
  * @brief  系统初始化
  * @param  无
  * @retval 无
  */
static void System_Init(void)
{
    /* 设置NVIC优先级分组 */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
}

/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{
    /* 系统初始化 */
    System_Init();
    
    /* 串口初始化 (115200-8-N-1) */
    Serial_Init();
    
    /* 逻辑分析仪初始化 */
    LA_Init();
    
    /* 初始化测试信号 (PB0 输出 1kHz 方波) */
    TestSignal_Init();
    
    /* 打印欢迎信息 */
    Serial_SendString("\r\n");
    Serial_SendString("================================\r\n");
    Serial_SendString("  Simple Logic Analyzer v1.0\r\n");
    Serial_SendString("  GD32F103RCT6\r\n");
    Serial_SendString("================================\r\n");
    Serial_SendString("Type 'HELP' for commands.\r\n");
    Serial_SendString("\r\n");
    Serial_SendString("[SELF-TEST] PB1 outputs 1kHz PWM.\r\n");
    Serial_SendString("            Connect PB1 -> PA0 to test.\r\n\r\n");
    
    /* 主循环 */
    while (1)
    {
        /* 检查是否收到串口命令 */
        if (Serial_GetRxComplete())
        {
            char *cmd = Serial_GetRxBuffer();
            CMD_Parse(cmd);
            Serial_ClearRxBuffer();
        }
        
        /* 检查采样是否完成 (只触发一次) */
        static uint8_t lastCaptureState = 0;
        uint8_t currentCaptureState = LA_IsCaptureComplete();
        
        if (currentCaptureState && !lastCaptureState)
        {
            Serial_SendString("CAPTURE COMPLETE\r\n");
            
            /* 自动发送数据 */
            LA_SendData();
        }
        lastCaptureState = currentCaptureState;
    }
}
