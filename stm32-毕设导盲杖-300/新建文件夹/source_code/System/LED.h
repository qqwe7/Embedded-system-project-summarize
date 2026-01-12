#ifndef __LED_H
#define __LED_H

#include "stm32f10x.h"
#include "pin_config.h"

/*============== 函数声明 ==============*/
void LED_Init(void);
void LED_On(void);
void LED_Off(void);
void LED_Toggle(void);

#endif
