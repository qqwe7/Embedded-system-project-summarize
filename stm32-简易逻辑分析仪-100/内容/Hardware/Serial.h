#ifndef __SERIAL_H
#define __SERIAL_H

#include <stdint.h>

void Serial_Init(void);
void Serial_SendByte(uint8_t Byte);
void Serial_SendArray(uint8_t *Array, uint16_t Length);
void Serial_SendString(char *String);
void Serial_Printf(char *format, ...);

uint8_t Serial_GetRxComplete(void);
char* Serial_GetRxBuffer(void);
void Serial_ClearRxBuffer(void);

#endif
