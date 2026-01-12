#ifndef __USART_H
#define __USART_H

#include "stm32f10x.h"
#include "pin_config.h"

/*============== 函数声明 ==============*/
void USART1_Init(uint32_t baudrate);
void USART1_SendByte(uint8_t byte);
void USART1_SendString(char* str);

#endif
