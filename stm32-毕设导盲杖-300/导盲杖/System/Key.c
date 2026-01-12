#include "Key.h"
#include "Delay.h"

/**
 * @brief  初始化所有按键
 */
void Key_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    RCC_APB2PeriphClockCmd(KEY1_RCC, ENABLE);
    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    
    GPIO_InitStructure.GPIO_Pin = KEY1_PIN;
    GPIO_Init(KEY1_PORT, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = KEY2_PIN;
    GPIO_Init(KEY2_PORT, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = KEY3_PIN;
    GPIO_Init(KEY3_PORT, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = KEY4_PIN;
    GPIO_Init(KEY4_PORT, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = KEY5_PIN;
    GPIO_Init(KEY5_PORT, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = KEY6_PIN;
    GPIO_Init(KEY6_PORT, &GPIO_InitStructure);
}

/**
 * @brief  扫描按键（带消抖）
 * @retval 按键值
 */
uint8_t Key_Scan(void)
{
    static uint8_t key_up = 1;
    
    if(key_up && (GPIO_ReadInputDataBit(KEY1_PORT, KEY1_PIN) == RESET ||
                  GPIO_ReadInputDataBit(KEY2_PORT, KEY2_PIN) == RESET ||
                  GPIO_ReadInputDataBit(KEY3_PORT, KEY3_PIN) == RESET ||
                  GPIO_ReadInputDataBit(KEY4_PORT, KEY4_PIN) == RESET ||
                  GPIO_ReadInputDataBit(KEY5_PORT, KEY5_PIN) == RESET ||
                  GPIO_ReadInputDataBit(KEY6_PORT, KEY6_PIN) == RESET))
    {
        Delay_ms(10);
        key_up = 0;
        
        if(GPIO_ReadInputDataBit(KEY1_PORT, KEY1_PIN) == RESET) return KEY1_PRESSED;
        if(GPIO_ReadInputDataBit(KEY2_PORT, KEY2_PIN) == RESET) return KEY2_PRESSED;
        if(GPIO_ReadInputDataBit(KEY3_PORT, KEY3_PIN) == RESET) return KEY3_PRESSED;
        if(GPIO_ReadInputDataBit(KEY4_PORT, KEY4_PIN) == RESET) return KEY4_PRESSED;
        if(GPIO_ReadInputDataBit(KEY5_PORT, KEY5_PIN) == RESET) return KEY5_PRESSED;
        if(GPIO_ReadInputDataBit(KEY6_PORT, KEY6_PIN) == RESET) return KEY6_PRESSED;
    }
    else if(GPIO_ReadInputDataBit(KEY1_PORT, KEY1_PIN) == SET &&
            GPIO_ReadInputDataBit(KEY2_PORT, KEY2_PIN) == SET &&
            GPIO_ReadInputDataBit(KEY3_PORT, KEY3_PIN) == SET &&
            GPIO_ReadInputDataBit(KEY4_PORT, KEY4_PIN) == SET &&
            GPIO_ReadInputDataBit(KEY5_PORT, KEY5_PIN) == SET &&
            GPIO_ReadInputDataBit(KEY6_PORT, KEY6_PIN) == SET)
    {
        key_up = 1;
    }
    
    return KEY_NONE;
}
