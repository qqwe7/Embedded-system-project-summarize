#include "stm32f10x.h"

static uint8_t fac_us = 0;
static uint16_t fac_ms = 0;

/**
  * @brief  初始化延时函数，由SystemInit调用或在main中初始化
  * @param  无
  * @retval 无
  */
void Delay_Init(void)
{
    // 系统时钟频率 SystemCoreClock 单位为Hz
    // SysTick的时钟源选择HCLK/8 或 HCLK
    // 这里我们选择外部时钟源 HCLK/8
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
    
    fac_us = SystemCoreClock / 8000000;  // 比如72MHz/8M=9, 8MHz/8M=1
    fac_ms = (uint16_t)fac_us * 1000;
}

/**
  * @brief  微秒级延时
  * @param  xus 延时时长
  * @retval 无
  */
void Delay_us(uint32_t xus)
{
    uint32_t temp;
    
    // 如果没有初始化，尝试自动初始化（假设默认72MHz或自动计算）
    if(fac_us == 0) Delay_Init();
    
    SysTick->LOAD = xus * fac_us;           // 时间加载
    SysTick->VAL = 0x00;                    // 清空计数器
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk; // 开始倒数
    
    do
    {
        temp = SysTick->CTRL;
    } while((temp & 0x01) && !(temp & (1<<16))); // 等待时间到达
    
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk; // 关闭计数器
    SysTick->VAL = 0x00;                       // 清空计数器
}

/**
  * @brief  毫秒级延时
  * @param  xms 延时时长
  * @retval 无
  */
void Delay_ms(uint32_t xms)
{
    uint32_t temp;
    
    if(fac_us == 0) Delay_Init();
    
    SysTick->LOAD = (uint32_t)xms * fac_ms;
    SysTick->VAL = 0x00;
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
    
    do
    {
        temp = SysTick->CTRL;
    } while((temp & 0x01) && !(temp & (1<<16)));
    
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
    SysTick->VAL = 0x00;
}

/**
  * @brief  秒级延时
  * @param  xs 延时时长
  * @retval 无
  */
void Delay_s(uint32_t xs)
{
	while(xs--)
	{
		Delay_ms(1000);
	}
} 
