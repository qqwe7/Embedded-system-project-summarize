#include "HCSR04.h"
#include "Delay.h"

/**
 * @brief  初始化HC-SR04超声波模块
 */
void HCSR04_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    RCC_APB2PeriphClockCmd(HCSR04_TRIG_RCC | HCSR04_ECHO_RCC, ENABLE);
    
    // Trig - 推挽输出
    GPIO_InitStructure.GPIO_Pin = HCSR04_TRIG_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(HCSR04_TRIG_PORT, &GPIO_InitStructure);
    
    // Echo - 浮空输入
    GPIO_InitStructure.GPIO_Pin = HCSR04_ECHO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(HCSR04_ECHO_PORT, &GPIO_InitStructure);
    
    GPIO_ResetBits(HCSR04_TRIG_PORT, HCSR04_TRIG_PIN);
}

/**
 * @brief  获取超声波测量距离
 * @retval 距离(cm)，-1表示超时
 */
float HCSR04_GetDistance(void)
{
    uint32_t timeout;
    uint32_t count = 0;
    
    // 发送10us触发脉冲
    GPIO_SetBits(HCSR04_TRIG_PORT, HCSR04_TRIG_PIN);
    Delay_us(15);
    GPIO_ResetBits(HCSR04_TRIG_PORT, HCSR04_TRIG_PIN);
    
    // 等待Echo变高
    timeout = 100000;
    while(GPIO_ReadInputDataBit(HCSR04_ECHO_PORT, HCSR04_ECHO_PIN) == RESET)
        if(--timeout == 0) return -1;
    
    // 测量高电平时间（直接计数，不延时）
    while(GPIO_ReadInputDataBit(HCSR04_ECHO_PORT, HCSR04_ECHO_PIN) == SET)
    {
        count++;
        if(count > 5000000) return -1;  // 进一步放宽超时，支持大距离
    }
    
    // Proteus校准：
    // 用户持续反馈误差偏小1cm
    // 之前系数41.8偏小，回调至近42.5
    // 设定为 42.4
    float time_us = (float)count * 42.4f;
    
    return time_us / 59.0f;
}
