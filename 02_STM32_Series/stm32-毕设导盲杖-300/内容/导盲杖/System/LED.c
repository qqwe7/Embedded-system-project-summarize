#include "LED.h"

/**
 * @brief  初始化LED
 */
void LED_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    RCC_APB2PeriphClockCmd(LED_RCC, ENABLE);
    
    GPIO_InitStructure.GPIO_Pin = LED_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(LED_PORT, &GPIO_InitStructure);
    
    GPIO_ResetBits(LED_PORT, LED_PIN);  // 默认点亮LED(低电平亮)
}

/**
 * @brief  点亮LED
 */
void LED_On(void)
{
    GPIO_ResetBits(LED_PORT, LED_PIN);
}

/**
 * @brief  熄灭LED
 */
void LED_Off(void)
{
    GPIO_SetBits(LED_PORT, LED_PIN);
}

/**
 * @brief  翻转LED状态
 */
void LED_Toggle(void)
{
    if(GPIO_ReadOutputDataBit(LED_PORT, LED_PIN) == SET)
        GPIO_ResetBits(LED_PORT, LED_PIN);
    else
        GPIO_SetBits(LED_PORT, LED_PIN);
}
