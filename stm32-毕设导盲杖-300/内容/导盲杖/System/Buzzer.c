#include "Buzzer.h"
#include "Delay.h"

/**
 * @brief  初始化蜂鸣器
 */
void Buzzer_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    RCC_APB2PeriphClockCmd(BUZZER_RCC, ENABLE);
    
    GPIO_InitStructure.GPIO_Pin = BUZZER_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(BUZZER_PORT, &GPIO_InitStructure);
    
    GPIO_SetBits(BUZZER_PORT, BUZZER_PIN);
}

/**
 * @brief  蜂鸣器开启
 */
void Buzzer_On(void)
{
    GPIO_ResetBits(BUZZER_PORT, BUZZER_PIN);
}

/**
 * @brief  蜂鸣器关闭
 */
void Buzzer_Off(void)
{
    GPIO_SetBits(BUZZER_PORT, BUZZER_PIN);
}

/**
 * @brief  蜂鸣器状态翻转
 */
void Buzzer_Toggle(void)
{
    if(GPIO_ReadOutputDataBit(BUZZER_PORT, BUZZER_PIN) == SET)
        GPIO_ResetBits(BUZZER_PORT, BUZZER_PIN);
    else
        GPIO_SetBits(BUZZER_PORT, BUZZER_PIN);
}

/**
 * @brief  无源蜂鸣器发声（产生方波）
 * @param  ms: 发声时间(毫秒)
 */
void Buzzer_Beep(uint16_t ms)
{
    uint16_t i;
    for(i = 0; i < ms; i++)
    {
        GPIO_ResetBits(BUZZER_PORT, BUZZER_PIN);
        Delay_us(500);
        GPIO_SetBits(BUZZER_PORT, BUZZER_PIN);
        Delay_us(500);
    }
}
