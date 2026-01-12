#include "OLED.h"
#include "OLED_Font.h"
#include "pin_config.h"
#include "Delay.h"
#include <stdio.h>
#include <string.h>

/* I2C 引脚操作宏 */
#define OLED_W_SCL(x)		GPIO_WriteBit(OLED_SCL_PORT, OLED_SCL_PIN, (BitAction)(x))
#define OLED_W_SDA(x)		GPIO_WriteBit(OLED_SDA_PORT, OLED_SDA_PIN, (BitAction)(x))

// 显存缓冲区 (128x8 bytes)
static uint8_t OLED_GRAM[8][128];

/* I2C 模拟驱动 */
void OLED_I2C_Start(void)
{
	OLED_W_SDA(1);
	OLED_W_SCL(1);
	OLED_W_SDA(0);
	OLED_W_SCL(0);
}

void OLED_I2C_Stop(void)
{
	OLED_W_SDA(0);
	OLED_W_SCL(1);
	OLED_W_SDA(1);
}

void OLED_I2C_SendByte(uint8_t Byte)
{
	uint8_t i;
	for (i = 0; i < 8; i++)
	{
		OLED_W_SDA(Byte & (0x80 >> i));
        Delay_us(1); // 恢复延时
		OLED_W_SCL(1);
        Delay_us(1); 
		OLED_W_SCL(0);
        Delay_us(1);
	}
	OLED_W_SCL(1);	//ACK
    Delay_us(1);
	OLED_W_SCL(0);
    Delay_us(1);
}

void OLED_WriteCommand(uint8_t Command)
{
	OLED_I2C_Start();
	OLED_I2C_SendByte(0x78);
	OLED_I2C_SendByte(0x00);
	OLED_I2C_SendByte(Command); 
	OLED_I2C_Stop();
}

void OLED_WriteData(uint8_t Data)
{
	OLED_I2C_Start();
	OLED_I2C_SendByte(0x78);
	OLED_I2C_SendByte(0x40);
	OLED_I2C_SendByte(Data);
	OLED_I2C_Stop();
}

// 仅仅是更新显存，不操作I2C
void OLED_ShowChar(uint8_t Line, uint8_t Column, char Char)
{      	
	uint8_t i;
    // 保护边界
    if(Line > 4 || Column > 16) return;
    
    // 计算显存位置 (Line 1-4, Column 1-16)
    // 8x16字体，每行占2页(Page)
    uint8_t page = (Line - 1) * 2;
    uint8_t col = (Column - 1) * 8;
    
	for (i = 0; i < 8; i++)
	{
        OLED_GRAM[page][col + i] = OLED_F8x16[Char - ' '][i];         // 上半部分
        OLED_GRAM[page+1][col + i] = OLED_F8x16[Char - ' '][i + 8];   // 下半部分
	}
}

void OLED_ShowString(uint8_t Line, uint8_t Column, char *String)
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i++)
	{
		OLED_ShowChar(Line, Column + i, String[i]);
	}
}

static uint32_t OLED_Pow(uint32_t X, uint32_t Y)
{
	uint32_t Result = 1;
	while (Y--)
	{
		Result *= X;
	}
	return Result;
}

void OLED_ShowNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
	uint8_t i;
	char buf[16];
    sprintf(buf, "%-*d", Length, Number); // 格式化为字符串
    OLED_ShowString(Line, Column, buf);
}

void OLED_ShowFloat(uint8_t Line, uint8_t Column, float Number)
{
    char buf[16];
    sprintf(buf, "%.1f", Number);
    OLED_ShowString(Line, Column, buf);
}

void OLED_Clear(void)
{  
	uint8_t i, j;
	for (j = 0; j < 8; j++)
	{
		for(i = 0; i < 128; i++)
		{
			OLED_GRAM[j][i] = 0x00;
		}
	}
}

// 将显存更新到屏幕 (这是唯一大量操作I2C的地方)
void OLED_UpdateScreen(void)
{
    uint8_t i, j;
    for(j = 0; j < 8; j++)
    {
        OLED_WriteCommand(0xB0 + j);    // 设置页地址
        OLED_WriteCommand(0x00);        // 设置列低地址
        OLED_WriteCommand(0x10);        // 设置列高地址
        
        OLED_I2C_Start();
        OLED_I2C_SendByte(0x78);
        OLED_I2C_SendByte(0x40);        // 连续写数据模式
        
        for(i = 0; i < 128; i++)
        {
            OLED_I2C_SendByte(OLED_GRAM[j][i]);
        }
        OLED_I2C_Stop();
    }
}

void OLED_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
    RCC_APB2PeriphClockCmd(OLED_SCL_RCC | OLED_SDA_RCC, ENABLE);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = OLED_SCL_PIN;
	GPIO_Init(OLED_SCL_PORT, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = OLED_SDA_PIN;
 	GPIO_Init(OLED_SDA_PORT, &GPIO_InitStructure);
    
	OLED_I2C_Start();
    OLED_I2C_Stop();
	
	OLED_WriteCommand(0xAE); // 关闭显示
	OLED_WriteCommand(0x8D); // 电荷泵设置
	OLED_WriteCommand(0x14); // 开启电荷泵
	OLED_WriteCommand(0xAF); // 开启显示
    
	OLED_Clear();
    OLED_UpdateScreen(); 
}
