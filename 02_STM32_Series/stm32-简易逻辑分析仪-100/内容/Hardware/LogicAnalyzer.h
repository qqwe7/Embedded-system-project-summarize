#ifndef __LOGICANALYZER_H
#define __LOGICANALYZER_H

#include <stdint.h>

/* 初始化 */
void LA_Init(void);

/* 配置函数 */
void LA_SetSampleRate(uint16_t psc, uint16_t arr);
void LA_SetSampleCount(uint16_t count);
void LA_SetTrigger(uint8_t pin, uint8_t edge);
void LA_DisableTrigger(void);

/* 采样控制 */
void LA_StartCapture(void);
uint8_t LA_IsCaptureComplete(void);

/* 数据访问 */
uint8_t* LA_GetBuffer(void);
uint16_t LA_GetSampleCount(void);
void LA_SendData(void);

#endif
