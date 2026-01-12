#ifndef   __KEY_H
#define   __KEY_H
		 
#include "stm32f4xx.h"

#define KEY0 		GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_4) //PE4
#define KEY1 		GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_3)	//PE3 
#define KEY2 		GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_2) //PE2
#define WK_UP 		GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0)	//PA0

void KEY_Init(void);
uint8_t KEY_Scan(uint8_t); 		

#endif
