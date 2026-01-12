/**
  ******************************************************************************
  * @file    Serial.c
  * @brief   串口通信模块 - 用于逻辑分析仪与PC通信
  * @note    基于USART1 (PA9-TX, PA10-RX), 波特率115200
  ******************************************************************************
  */

#include "stm32f10x.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "Serial.h"

#define SERIAL_RX_BUF_SIZE  128     // 接收缓冲区大小

static char Serial_RxBuffer[SERIAL_RX_BUF_SIZE];   // 接收缓冲区
static uint8_t Serial_RxIndex = 0;                 // 接收索引
static uint8_t Serial_RxComplete = 0;              // 接收完成标志 (收到换行符)

/**
  * @brief  串口初始化 (USART1, 115200-8-N-1)
  * @param  无
  * @retval 无
  */
void Serial_Init(void)
{
    /* 开启时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    
    /* GPIO初始化 */
    GPIO_InitTypeDef GPIO_InitStructure;
    
    // PA9 - TX (复用推挽输出)
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // PA10 - RX (上拉输入)
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    /* USART初始化 */
    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate = 115200;                               // 波特率
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_Init(USART1, &USART_InitStructure);
    
    /* 中断配置 */
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);  // 开启接收中断
    
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_Init(&NVIC_InitStructure);
    
    /* 使能USART */
    USART_Cmd(USART1, ENABLE);
}

/**
  * @brief  发送一个字节
  * @param  Byte: 要发送的字节
  * @retval 无
  */
void Serial_SendByte(uint8_t Byte)
{
    USART_SendData(USART1, Byte);
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
}

/**
  * @brief  发送数组
  * @param  Array: 数组首地址
  * @param  Length: 数组长度
  * @retval 无
  */
void Serial_SendArray(uint8_t *Array, uint16_t Length)
{
    for (uint16_t i = 0; i < Length; i++)
    {
        Serial_SendByte(Array[i]);
    }
}

/**
  * @brief  发送字符串
  * @param  String: 字符串首地址
  * @retval 无
  */
void Serial_SendString(char *String)
{
    while (*String)
    {
        Serial_SendByte(*String++);
    }
}

/**
  * @brief  格式化打印函数
  * @param  format: 格式化字符串
  * @retval 无
  */
void Serial_Printf(char *format, ...)
{
    char String[256];
    va_list arg;
    va_start(arg, format);
    vsprintf(String, format, arg);
    va_end(arg);
    Serial_SendString(String);
}

/**
  * @brief  检查是否收到一行命令
  * @param  无
  * @retval 1: 收到完整命令, 0: 未收到
  */
uint8_t Serial_GetRxComplete(void)
{
    if (Serial_RxComplete)
    {
        Serial_RxComplete = 0;
        return 1;
    }
    return 0;
}

/**
  * @brief  获取接收缓冲区指针
  * @param  无
  * @retval 接收缓冲区首地址
  */
char* Serial_GetRxBuffer(void)
{
    return Serial_RxBuffer;
}

/**
  * @brief  清空接收缓冲区
  * @param  无
  * @retval 无
  */
void Serial_ClearRxBuffer(void)
{
    memset(Serial_RxBuffer, 0, SERIAL_RX_BUF_SIZE);
    Serial_RxIndex = 0;
}

/**
  * @brief  USART1中断服务函数
  * @param  无
  * @retval 无
  */
void USART1_IRQHandler(void)
{
    if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
    {
        uint8_t data = USART_ReceiveData(USART1);
        
        if (data == '\n' || data == '\r')
        {
            if (Serial_RxIndex > 0)
            {
                Serial_RxBuffer[Serial_RxIndex] = '\0';
                Serial_RxComplete = 1;
            }
        }
        else if (Serial_RxIndex < SERIAL_RX_BUF_SIZE - 1)
        {
            Serial_RxBuffer[Serial_RxIndex++] = data;
        }
        
        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    }
}
