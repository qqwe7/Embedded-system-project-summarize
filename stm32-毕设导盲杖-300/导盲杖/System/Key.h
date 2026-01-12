#ifndef __KEY_H
#define __KEY_H

#include "stm32f10x.h"
#include "pin_config.h"

/*============== 函数声明 ==============*/
void Key_Init(void);
uint8_t Key_Scan(void);

#endif
