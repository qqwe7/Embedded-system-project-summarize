#include "LCD1602.h"
#include "pin_config.h"
#include "Delay.h"
#include <stdio.h>

/* 写指令 */
static void LCD_WriteCmd(uint8_t cmd)
{
    GPIO_ResetBits(LCD_RS_PORT, LCD_RS_PIN); // RS=0 写指令
    
    // 发送高4位
    GPIO_WriteBit(LCD_DATA_PORT, LCD_D7_PIN, (BitAction)((cmd & 0x80) >> 7));
    GPIO_WriteBit(LCD_DATA_PORT, LCD_D6_PIN, (BitAction)((cmd & 0x40) >> 6));
    GPIO_WriteBit(LCD_DATA_PORT, LCD_D5_PIN, (BitAction)((cmd & 0x20) >> 5));
    GPIO_WriteBit(LCD_DATA_PORT, LCD_D4_PIN, (BitAction)((cmd & 0x10) >> 4));
    
    GPIO_SetBits(LCD_EN_PORT, LCD_EN_PIN);
    Delay_us(100);
    GPIO_ResetBits(LCD_EN_PORT, LCD_EN_PIN);
    Delay_us(100);
    
    // 发送低4位
    GPIO_WriteBit(LCD_DATA_PORT, LCD_D7_PIN, (BitAction)((cmd & 0x08) >> 3));
    GPIO_WriteBit(LCD_DATA_PORT, LCD_D6_PIN, (BitAction)((cmd & 0x04) >> 2));
    GPIO_WriteBit(LCD_DATA_PORT, LCD_D5_PIN, (BitAction)((cmd & 0x02) >> 1));
    GPIO_WriteBit(LCD_DATA_PORT, LCD_D4_PIN, (BitAction)((cmd & 0x01) >> 0));
    
    GPIO_SetBits(LCD_EN_PORT, LCD_EN_PIN);
    Delay_us(100);
    GPIO_ResetBits(LCD_EN_PORT, LCD_EN_PIN);
    Delay_ms(2);
}

/* 写数据 */
static void LCD_WriteData(uint8_t data)
{
    GPIO_SetBits(LCD_RS_PORT, LCD_RS_PIN); // RS=1 写数据
    
    // 发送高4位
    GPIO_WriteBit(LCD_DATA_PORT, LCD_D7_PIN, (BitAction)((data & 0x80) >> 7));
    GPIO_WriteBit(LCD_DATA_PORT, LCD_D6_PIN, (BitAction)((data & 0x40) >> 6));
    GPIO_WriteBit(LCD_DATA_PORT, LCD_D5_PIN, (BitAction)((data & 0x20) >> 5));
    GPIO_WriteBit(LCD_DATA_PORT, LCD_D4_PIN, (BitAction)((data & 0x10) >> 4));
    
    GPIO_SetBits(LCD_EN_PORT, LCD_EN_PIN);
    Delay_us(100);
    GPIO_ResetBits(LCD_EN_PORT, LCD_EN_PIN);
    Delay_us(100);
    
    // 发送低4位
    GPIO_WriteBit(LCD_DATA_PORT, LCD_D7_PIN, (BitAction)((data & 0x08) >> 3));
    GPIO_WriteBit(LCD_DATA_PORT, LCD_D6_PIN, (BitAction)((data & 0x04) >> 2));
    GPIO_WriteBit(LCD_DATA_PORT, LCD_D5_PIN, (BitAction)((data & 0x02) >> 1));
    GPIO_WriteBit(LCD_DATA_PORT, LCD_D4_PIN, (BitAction)((data & 0x01) >> 0));
    
    GPIO_SetBits(LCD_EN_PORT, LCD_EN_PIN);
    Delay_us(100);
    GPIO_ResetBits(LCD_EN_PORT, LCD_EN_PIN);
    Delay_us(100);
}

void LCD_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    RCC_APB2PeriphClockCmd(LCD_RS_RCC | LCD_EN_RCC | LCD_DATA_RCC, ENABLE);
    
    // RS, EN
    GPIO_InitStructure.GPIO_Pin = LCD_RS_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(LCD_RS_PORT, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = LCD_EN_PIN;
    GPIO_Init(LCD_EN_PORT, &GPIO_InitStructure);
    
    // D4-D7
    GPIO_InitStructure.GPIO_Pin = LCD_D4_PIN | LCD_D5_PIN | LCD_D6_PIN | LCD_D7_PIN;
    GPIO_Init(LCD_DATA_PORT, &GPIO_InitStructure);
    
    Delay_ms(50);
    
    // 4位模式初始化序列
    LCD_WriteCmd(0x33);
    LCD_WriteCmd(0x32);
    LCD_WriteCmd(0x28); // 4位总线，2行显示，5x7点阵
    LCD_WriteCmd(0x0C); // 显示开，光标关，闪烁关
    LCD_WriteCmd(0x06); // 光标右移
    LCD_WriteCmd(0x01); // 清屏
}

void LCD_Clear(void)
{
    LCD_WriteCmd(0x01);
}

void LCD_ShowString(uint8_t x, uint8_t y, char *str)
{
    uint8_t addr;
    if(y == 0) addr = 0x80 + x;
    else addr = 0xC0 + x;
    
    LCD_WriteCmd(addr);
    while(*str)
    {
        LCD_WriteData(*str++);
    }
}

void LCD_ShowNum(uint8_t x, uint8_t y, uint32_t num, uint8_t length)
{
    char buf[16];
    sprintf(buf, "%d", num);
    LCD_ShowString(x, y, buf);
}

void LCD_ShowFloat(uint8_t x, uint8_t y, float num)
{
    char buf[16];
    sprintf(buf, "%.1f", num);
    LCD_ShowString(x, y, buf);
}
