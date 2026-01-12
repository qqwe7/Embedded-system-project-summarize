/**
  ******************************************************************************
  * @file    LogicAnalyzer.c
  * @brief   简易逻辑分析仪核心模块 (调试版)
  * @note    使用 TIM2 触发 DMA 从 GPIO 端口读取数据进行高速采样
  ******************************************************************************
  */

#include "stm32f10x.h"
#include "LogicAnalyzer.h"
#include "Serial.h"

/* 采样缓冲区 - 使用 SRAM 存储采样数据 */
#define LA_BUFFER_SIZE      1024    // 缓冲区大小 (字节) - 减小以加快测试
static uint8_t LA_SampleBuffer[LA_BUFFER_SIZE];

/* 状态变量 */
static volatile uint8_t LA_SamplingComplete = 0;    // 采样完成标志
static uint16_t LA_SampleRate_PSC = 71;             // 预分频值 (默认 72MHz/72 = 1MHz)
static uint16_t LA_SampleRate_ARR = 99;             // 重装载值 (默认 1MHz/100 = 10kHz采样率)
static uint16_t LA_SampleCount = 256;               // 采样数量 - 减小以加快测试

/* 触发设置 */
static uint8_t LA_TriggerPin = 0;       // 触发引脚 (0-7 对应PA0-PA7)
static uint8_t LA_TriggerEdge = 1;      // 触发边沿 (0=下降沿, 1=上升沿)
static uint8_t LA_TriggerEnabled = 0;   // 是否启用触发

/* 调试计数器 */
static volatile uint32_t LA_DMA_IRQ_Count = 0;

/**
  * @brief  GPIO 输入初始化 (PA0-PA7 作为8通道输入)
  * @param  无
  * @retval 无
  */
static void LA_GPIO_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;       // 上拉输入 (改为上拉)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 |
                                   GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    Serial_SendString("[DEBUG] GPIO Init: PA0-PA7 as input (pull-up)\r\n");
}

/**
  * @brief  定时器2初始化 (作为DMA触发源)
  * @param  无
  * @retval 无
  */
static void LA_TIM_Init(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_Period = LA_SampleRate_ARR;
    TIM_TimeBaseStructure.TIM_Prescaler = LA_SampleRate_PSC;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
    
    /* 使能 TIM2 更新事件触发 DMA 请求 */
    TIM_DMACmd(TIM2, TIM_DMA_Update, ENABLE);
    
    Serial_Printf("[DEBUG] TIM2 Init: PSC=%d, ARR=%d\r\n", LA_SampleRate_PSC, LA_SampleRate_ARR);
}

/**
  * @brief  DMA 初始化 (从 GPIOA->IDR 传输到缓冲区)
  * @param  无
  * @retval 无
  */
static void LA_DMA_Init(void)
{
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    
    DMA_InitTypeDef DMA_InitStructure;
    
    /* DMA1 通道2 对应 TIM2_UP */
    DMA_DeInit(DMA1_Channel2);
    
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(GPIOA->IDR);    // 源: GPIO输入数据寄存器
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)LA_SampleBuffer;       // 目的: 采样缓冲区
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;                      // 外设到内存
    DMA_InitStructure.DMA_BufferSize = LA_SampleCount;                      // 传输数量
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;        // 外设地址不递增
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                 // 内存地址递增
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; // 外设8位
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;         // 内存8位
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                           // 单次模式
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    
    DMA_Init(DMA1_Channel2, &DMA_InitStructure);
    
    /* 配置 DMA 传输完成中断 */
    DMA_ITConfig(DMA1_Channel2, DMA_IT_TC, ENABLE);
    
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    Serial_Printf("[DEBUG] DMA1_Ch2 Init: PeriphAddr=0x%08X, MemAddr=0x%08X, Size=%d\r\n",
                  (uint32_t)&(GPIOA->IDR), (uint32_t)LA_SampleBuffer, LA_SampleCount);
}

/**
  * @brief  逻辑分析仪初始化
  * @param  无
  * @retval 无
  */
void LA_Init(void)
{
    Serial_SendString("\r\n[DEBUG] LA_Init() start\r\n");
    LA_GPIO_Init();
    LA_TIM_Init();
    LA_DMA_Init();
    Serial_SendString("[DEBUG] LA_Init() complete\r\n\r\n");
}

/**
  * @brief  设置采样率
  * @param  psc: 预分频值
  * @param  arr: 重装载值
  * @note   采样率 = 72MHz / (psc+1) / (arr+1)
  * @retval 无
  */
void LA_SetSampleRate(uint16_t psc, uint16_t arr)
{
    LA_SampleRate_PSC = psc;
    LA_SampleRate_ARR = arr;
}

/**
  * @brief  设置采样数量
  * @param  count: 采样数量 (最大 LA_BUFFER_SIZE)
  * @retval 无
  */
void LA_SetSampleCount(uint16_t count)
{
    if (count > LA_BUFFER_SIZE)
        count = LA_BUFFER_SIZE;
    LA_SampleCount = count;
}

/**
  * @brief  设置触发条件
  * @param  pin: 触发引脚 (0-7)
  * @param  edge: 边沿 (0=下降, 1=上升)
  * @retval 无
  */
void LA_SetTrigger(uint8_t pin, uint8_t edge)
{
    LA_TriggerPin = pin & 0x07;
    LA_TriggerEdge = edge;
    LA_TriggerEnabled = 1;
}

/**
  * @brief  禁用触发
  * @param  无
  * @retval 无
  */
void LA_DisableTrigger(void)
{
    LA_TriggerEnabled = 0;
}

/**
  * @brief  开始采样
  * @param  无
  * @retval 无
  */
void LA_StartCapture(void)
{
    Serial_SendString("[DEBUG] LA_StartCapture() called\r\n");
    
    LA_SamplingComplete = 0;
    LA_DMA_IRQ_Count = 0;
    
    /* 读取当前 GPIO 状态 */
    uint16_t gpio_val = GPIOA->IDR & 0xFF;
    Serial_Printf("[DEBUG] Current GPIOA IDR (low 8 bits): 0x%02X\r\n", gpio_val);
    
    /* 停止定时器 */
    TIM_Cmd(TIM2, DISABLE);
    
    /* 重新配置定时器参数 */
    TIM_SetAutoreload(TIM2, LA_SampleRate_ARR);
    TIM_PrescalerConfig(TIM2, LA_SampleRate_PSC, TIM_PSCReloadMode_Immediate);
    TIM_SetCounter(TIM2, 0);
    
    /* 清除 DMA 标志 */
    DMA_ClearFlag(DMA1_FLAG_TC2 | DMA1_FLAG_HT2 | DMA1_FLAG_TE2);
    
    /* 重新配置 DMA */
    DMA_Cmd(DMA1_Channel2, DISABLE);
    DMA1_Channel2->CNDTR = LA_SampleCount;  // 设置传输数量
    DMA1_Channel2->CMAR = (uint32_t)LA_SampleBuffer;  // 设置内存地址
    DMA1_Channel2->CPAR = (uint32_t)&(GPIOA->IDR);    // 设置外设地址
    DMA_Cmd(DMA1_Channel2, ENABLE);
    
    Serial_Printf("[DEBUG] DMA CNDTR=%d, DMA Enabled=%d\r\n", 
                  DMA_GetCurrDataCounter(DMA1_Channel2),
                  (DMA1_Channel2->CCR & DMA_CCR1_EN) ? 1 : 0);
    
    /* 等待触发 (如果启用) - 暂时禁用触发功能进行测试 */
    if (LA_TriggerEnabled)
    {
        Serial_SendString("[DEBUG] Waiting for trigger...\r\n");
        uint8_t prevState = (GPIOA->IDR >> LA_TriggerPin) & 0x01;
        uint8_t currState;
        uint32_t timeout = 0;
        
        while (timeout < 1000000)  // 添加超时
        {
            currState = (GPIOA->IDR >> LA_TriggerPin) & 0x01;
            
            if (LA_TriggerEdge == 1)  // 上升沿
            {
                if (prevState == 0 && currState == 1) break;
            }
            else  // 下降沿
            {
                if (prevState == 1 && currState == 0) break;
            }
            prevState = currState;
            timeout++;
        }
        
        if (timeout >= 1000000)
        {
            Serial_SendString("[DEBUG] Trigger timeout!\r\n");
        }
        else
        {
            Serial_SendString("[DEBUG] Triggered!\r\n");
        }
    }
    
    /* 启动定时器开始采样 */
    TIM_Cmd(TIM2, ENABLE);
    Serial_SendString("[DEBUG] TIM2 started, sampling...\r\n");
    
    /* 等待采样完成 (阻塞方式，带超时) */
    uint32_t wait = 0;
    while (!LA_SamplingComplete && wait < 5000000)
    {
        wait++;
    }
    
    if (LA_SamplingComplete)
    {
        Serial_Printf("[DEBUG] Sampling complete! DMA IRQ count=%d\r\n", LA_DMA_IRQ_Count);
    }
    else
    {
        Serial_SendString("[DEBUG] Sampling timeout!\r\n");
        Serial_Printf("[DEBUG] DMA CNDTR remaining=%d\r\n", DMA_GetCurrDataCounter(DMA1_Channel2));
        Serial_Printf("[DEBUG] TIM2 CNT=%d\r\n", TIM_GetCounter(TIM2));
        
        /* 强制完成 */
        TIM_Cmd(TIM2, DISABLE);
        LA_SamplingComplete = 1;
    }
}

/**
  * @brief  检查采样是否完成
  * @param  无
  * @retval 1: 完成, 0: 进行中
  */
uint8_t LA_IsCaptureComplete(void)
{
    return LA_SamplingComplete;
}

/**
  * @brief  获取采样缓冲区指针
  * @param  无
  * @retval 缓冲区首地址
  */
uint8_t* LA_GetBuffer(void)
{
    return LA_SampleBuffer;
}

/**
  * @brief  获取采样数量
  * @param  无
  * @retval 采样数量
  */
uint16_t LA_GetSampleCount(void)
{
    return LA_SampleCount;
}

/**
  * @brief  发送采样数据到PC
  * @param  无
  * @retval 无
  */
void LA_SendData(void)
{
    /* 发送头标识 */
    Serial_Printf("DATA: (count=%d)\r\n", LA_SampleCount);
    
    /* 发送数据 (十六进制格式) */
    for (uint16_t i = 0; i < LA_SampleCount; i++)
    {
        Serial_Printf("%02X", LA_SampleBuffer[i]);
        
        /* 每32字节换行 */
        if ((i + 1) % 32 == 0)
        {
            Serial_SendString("\r\n");
        }
    }
    
    Serial_SendString("\r\nEND\r\n");
}

/**
  * @brief  DMA1 通道2 中断服务函数 (采样完成)
  * @param  无
  * @retval 无
  */
void DMA1_Channel2_IRQHandler(void)
{
    LA_DMA_IRQ_Count++;
    
    if (DMA_GetITStatus(DMA1_IT_TC2) == SET)
    {
        DMA_ClearITPendingBit(DMA1_IT_TC2);
        TIM_Cmd(TIM2, DISABLE);
        LA_SamplingComplete = 1;
    }
}
