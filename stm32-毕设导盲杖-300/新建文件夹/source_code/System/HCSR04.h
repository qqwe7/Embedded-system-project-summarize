#ifndef __HCSR04_H
#define __HCSR04_H

#include "stm32f10x.h"
#include "pin_config.h"

/*============== 函数声明 ==============*/
void HCSR04_Init(void);
float HCSR04_GetDistance(void);

#endif
