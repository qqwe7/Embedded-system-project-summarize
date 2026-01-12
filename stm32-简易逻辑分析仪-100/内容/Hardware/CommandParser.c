/**
  ******************************************************************************
  * @file    CommandParser.c
  * @brief   命令解析器 - 解析PC发送的控制命令
  ******************************************************************************
  */

#include <string.h>
#include <stdlib.h>
#include "CommandParser.h"
#include "LogicAnalyzer.h"
#include "Serial.h"

/**
  * @brief  解析并执行命令
  * @param  cmd: 命令字符串
  * @retval 0: 成功, -1: 无效命令
  */
int CMD_Parse(char *cmd)
{
    /* 跳过前导空格 */
    while (*cmd == ' ') cmd++;
    
    /* RATE <psc> <arr> - 设置采样率 */
    if (strncmp(cmd, "RATE", 4) == 0)
    {
        cmd += 4;
        uint16_t psc = atoi(cmd);
        while (*cmd && *cmd != ' ') cmd++;
        uint16_t arr = atoi(cmd);
        
        LA_SetSampleRate(psc, arr);
        Serial_Printf("OK: RATE PSC=%d ARR=%d\r\n", psc, arr);
        return 0;
    }
    
    /* COUNT <n> - 设置采样数量 */
    if (strncmp(cmd, "COUNT", 5) == 0)
    {
        cmd += 5;
        uint16_t count = atoi(cmd);
        
        LA_SetSampleCount(count);
        Serial_Printf("OK: COUNT=%d\r\n", count);
        return 0;
    }
    
    /* TRIG <pin> <edge> - 设置触发条件 */
    if (strncmp(cmd, "TRIG", 4) == 0)
    {
        cmd += 4;
        uint8_t pin = atoi(cmd);
        while (*cmd && *cmd != ' ') cmd++;
        uint8_t edge = atoi(cmd);
        
        LA_SetTrigger(pin, edge);
        Serial_Printf("OK: TRIG PIN=%d EDGE=%d\r\n", pin, edge);
        return 0;
    }
    
    /* NOTRIG - 禁用触发 */
    if (strncmp(cmd, "NOTRIG", 6) == 0)
    {
        LA_DisableTrigger();
        Serial_SendString("OK: TRIGGER DISABLED\r\n");
        return 0;
    }
    
    /* CAP - 开始采样 */
    if (strncmp(cmd, "CAP", 3) == 0)
    {
        Serial_SendString("OK: CAPTURING...\r\n");
        LA_StartCapture();
        return 0;
    }
    
    /* STATUS - 查询状态 */
    if (strncmp(cmd, "STATUS", 6) == 0)
    {
        if (LA_IsCaptureComplete())
        {
            Serial_SendString("STATUS: READY\r\n");
        }
        else
        {
            Serial_SendString("STATUS: CAPTURING\r\n");
        }
        return 0;
    }
    
    /* SEND - 发送数据 */
    if (strncmp(cmd, "SEND", 4) == 0)
    {
        LA_SendData();
        return 0;
    }
    
    /* HELP - 帮助 */
    if (strncmp(cmd, "HELP", 4) == 0 || strncmp(cmd, "?", 1) == 0)
    {
        Serial_SendString("\r\n=== Logic Analyzer Commands ===\r\n");
        Serial_SendString("RATE <psc> <arr>  - Set sample rate\r\n");
        Serial_SendString("COUNT <n>         - Set sample count\r\n");
        Serial_SendString("TRIG <pin> <edge> - Set trigger (pin:0-7, edge:0=fall,1=rise)\r\n");
        Serial_SendString("NOTRIG            - Disable trigger\r\n");
        Serial_SendString("CAP               - Start capture\r\n");
        Serial_SendString("STATUS            - Check status\r\n");
        Serial_SendString("SEND              - Send captured data\r\n");
        Serial_SendString("================================\r\n");
        return 0;
    }
    
    /* 未知命令 */
    Serial_Printf("ERR: Unknown command '%s'\r\n", cmd);
    return -1;
}
