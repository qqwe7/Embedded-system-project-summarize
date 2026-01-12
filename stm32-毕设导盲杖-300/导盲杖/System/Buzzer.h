#ifndef __BUZZER_H
#define __BUZZER_H

#include "stm32f10x.h"
#include "pin_config.h"

/*============== 函数声明 ==============*/
void Buzzer_Init(void);
void Buzzer_On(void);
void Buzzer_Off(void);
void Buzzer_Toggle(void);
void Buzzer_Beep(uint16_t ms);

#endif
