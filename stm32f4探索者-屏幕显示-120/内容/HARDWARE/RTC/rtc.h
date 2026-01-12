	#ifndef __RTC_H
#define __RTC_H	 
#include "sys.h" 
 //////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//RTC 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/5/5
//版本：V1.1
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
//********************************************************************************
//修改说明
//V1.1 20140726
//新增:RTC_Get_Week函数,用于根据年月日信息,得到星期信息.
////////////////////////////////////////////////////////////////////////////////// 


// 底层驱动
u8   My_RTC_Init(void);
ErrorStatus RTC_Set_Time(u8 hour,u8 min,u8 sec,u8 ampm);
ErrorStatus RTC_Set_Date(u8 year,u8 month,u8 date,u8 week);
void RTC_Set_AlarmA(u8 week,u8 hour,u8 min,u8 sec);
void RTC_Set_AlarmB(u8 week,u8 hour,u8 min,u8 sec);
void RTC_Set_WakeUp(u32 wksel,u16 cnt);

// 供 USMart 调用的壳函数（只声明，不实现）
void rtc_set_time(u8 hour,u8 min,u8 sec);
void rtc_set_date(u8 year,u8 month,u8 date,u8 week);
void set_alarm1(u8 week,u8 hour,u8 min);
void set_alarm2(u8 week,u8 hour,u8 min);

#endif

















