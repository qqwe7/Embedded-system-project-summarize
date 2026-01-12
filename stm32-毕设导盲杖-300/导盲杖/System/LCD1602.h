#ifndef __LCD1602_H
#define __LCD1602_H

#include "stm32f10x.h"

void LCD_Init(void);
void LCD_Clear(void);
void LCD_ShowString(uint8_t x, uint8_t y, char *str);
void LCD_ShowNum(uint8_t x, uint8_t y, uint32_t num, uint8_t length);
void LCD_ShowFloat(uint8_t x, uint8_t y, float num);

#endif
