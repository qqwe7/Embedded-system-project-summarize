#include "rtc.h"
#include "led.h"
#include "delay.h"
#include "usart.h"
#include "lcd.h"


NVIC_InitTypeDef   NVIC_InitStructure;

//RTC时间设置
//hour,min,sec:小时,分钟,秒钟
//ampm:@RTC_AM_PM_Definitions  :RTC_H12_AM（24小时制）/RTC_H12_PM （12小时制）
//返回值:SUCEE(1),成功
//       ERROR(0),进入初始化模式失败 
ErrorStatus RTC_Set_Time(u8 hour,u8 min,u8 sec,u8 ampm)
{
	RTC_TimeTypeDef RTC_TimeTypeInitStructure;
	
	RTC_TimeTypeInitStructure.RTC_Hours=hour;
	RTC_TimeTypeInitStructure.RTC_Minutes=min;
	RTC_TimeTypeInitStructure.RTC_Seconds=sec;
	RTC_TimeTypeInitStructure.RTC_H12=ampm;
	
	return RTC_SetTime(RTC_Format_BIN,&RTC_TimeTypeInitStructure);
	
}
//RTC日期设置
//year,month,date:年(0~99),月(1~12),日(0~31)
//week:星期(1~7,0,非法!)
//返回值:SUCEE(1),成功
//       ERROR(0),进入初始化模式失败 
ErrorStatus RTC_Set_Date(u8 year,u8 month,u8 date,u8 week)
{
	
	RTC_DateTypeDef RTC_DateTypeInitStructure;
	RTC_DateTypeInitStructure.RTC_Date=date;
	RTC_DateTypeInitStructure.RTC_Month=month;
	RTC_DateTypeInitStructure.RTC_WeekDay=week;
	RTC_DateTypeInitStructure.RTC_Year=year;
	return RTC_SetDate(RTC_Format_BIN,&RTC_DateTypeInitStructure);
}

//RTC初始化
//返回值:0,初始化成功;
//       1,LSE开启失败;
//       2,进入初始化模式失败;
u8 My_RTC_Init(void)
{
	RTC_InitTypeDef RTC_InitStructure;
	u16 retry = 0X1FFF;
    
    //# 1）使能电源时钟（PWR）以便访问电源控制寄存器，并使能 RTC 及 RTC 后备寄存器写访问
        //系统复位或上电复位后，对RTC和RTC备份寄存器的写访问被禁止，执行以下操作可以使能对RTC及RTC备份寄存器的写访问：
        //1、通过设置寄存器 RCC_APB1ENR 的 PWREN 位来打开电源接口时钟
        //2、电源控制寄存器(PWR_CR)的 DBP 位来使能对 RTC 及 RTC 备份寄存器的访问。DBP：禁止备份域写保护 (Disable backup domain write protection)
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);//使能PWR时钟
	PWR_BackupAccessCmd(ENABLE);	//使能RTC 及 RTC 备份寄存器访问 
	
	if(RTC_ReadBackupRegister(RTC_BKP_DR0)!=0x5050)		//读取RTC备份数据寄存器（RTC_BKP_DR0）的数据，判断是否是第一次配置? !=0x5050为第一次配置，=0x5050表示已经配置过
	{
        //# 2）开启外部低速振荡器，选择 RTC 时钟，并使能
		RCC_LSEConfig(RCC_LSE_ON);  //LSE 开启    
		while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)	//等待外部低速晶振（32.768KHz）就绪 ,RCC备份域控制寄存器(RCC_BDCR)的LSERDY位置1
		{
			retry --;
			delay_ms(10);
			if(retry==0)
				return 1;		       //LSE 开启失败.
		}
			
        RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);		//设置RTC时钟(RTCCLK),选择LSE作为RTC时钟    
        RCC_RTCCLKCmd(ENABLE);	   //使能RTC时钟 

        //# 3）初始化 RTC，设置 RTC 的分频系数，以及配置 RTC 参数
        RTC_InitStructure.RTC_AsynchPrediv = 0x7F; //RTC异步分频系数(1~0X7F) ，默认设为128-1
        RTC_InitStructure.RTC_SynchPrediv  = 0xFF; //RTC同步分频系数(0~7FFF) ，默认设为256-1
        RTC_InitStructure.RTC_HourFormat   = RTC_HourFormat_24;  //RTC设置为,24小时格式
        RTC_Init(&RTC_InitStructure);     //RTC_Init函数在设置 RTC 相关参数之前，会先取消 RTC寄存器写保护，通过向写保护寄存器 RTC_WPR 写入 0XCA 和 0X53 两个数据实现，参数设置好后，最后再开启 RTC 寄存器写保护，往 RTC_WPR 寄存器写入值 0xFF 即可。
     
        //# 4）设置 RTC 的时间
        RTC_Set_Time(17,11,56,0);	//设置时间的时、分、秒、小时制（RTC_H12_AM是24小时制，RTC_H12_PM则是12小时制）。同RTC_Init函数一样，RTC_Set_Time函数在设置 RTC 时间参数之前，要先取消 RTC寄存器写保护，在参数设置好后，最后再开启 RTC 寄存器写保护。
        
        //# 5）设置 RTC 的日期
        RTC_Set_Date(25,12,15,1);		//设置日期的年份，月份，日，星期几。同RTC_Init函数一样，RTC_Set_Date函数，要先取消 RTC寄存器写保护，在参数设置好后，最后再开启 RTC 寄存器写保护。
     
        //# 6）设置 RTC 备份数据寄存器
        RTC_WriteBackupRegister(RTC_BKP_DR0, 0x5050);	//在备份数据寄存器（RTC_BKP_DR0）中写入0x5050标记已经初始化过了，在指定的RTC备份数据寄存器中写入数据。
	} 
 
	return 0;
}

//设置闹钟时间(按星期闹铃,24小时制)
//week:星期几(1~7) @ref  RTC_Alarm_Definitions
//hour,min,sec:小时,分钟,秒钟
void RTC_Set_AlarmA(u8 week,u8 hour,u8 min,u8 sec)
{ 
	EXTI_InitTypeDef   EXTI_InitStructure;
	RTC_AlarmTypeDef   RTC_AlarmTypeInitStructure;
	RTC_TimeTypeDef    RTC_TimeTypeInitStructure;
	
	RTC_AlarmCmd(RTC_Alarm_A, DISABLE);//关闭闹钟A 
	
    RTC_TimeTypeInitStructure.RTC_Hours = hour;  //设置闹钟A：小时
	RTC_TimeTypeInitStructure.RTC_Minutes = min; //设置闹钟A：分钟
	RTC_TimeTypeInitStructure.RTC_Seconds = sec; //设置闹钟A：秒
	RTC_TimeTypeInitStructure.RTC_H12 = RTC_H12_AM; //闹钟A设置为24小时制
  
    RTC_AlarmTypeInitStructure.RTC_AlarmDateWeekDaySel = RTC_AlarmDateWeekDaySel_WeekDay;//选择是闹钟是按日期还是按星期闹,RTC_AlarmDateWeekDaySel_WeekDay按星期闹;RTC_AlarmDateWeekDaySel_Date按日期闹
	RTC_AlarmTypeInitStructure.RTC_AlarmDateWeekDay = week;  //按选择方式，设置闹钟按日期或者星期几闹，取值范围为：week:星期(1~7)或(0~31) 日
	RTC_AlarmTypeInitStructure.RTC_AlarmMask = RTC_AlarmMask_DateWeekDay;//设置闹钟时间掩码，RTC_AlarmMask_DateWeekDay是所有的时、分、秒以及星期几/(或者每月哪一天)都不屏蔽要精确匹配，RTC_AlarmMask_DateWeekDay屏蔽星期几/每月哪一天，每天都会按设定时间闹。 
	RTC_AlarmTypeInitStructure.RTC_AlarmTime = RTC_TimeTypeInitStructure;
    RTC_SetAlarm(RTC_Format_BIN, RTC_Alarm_A, &RTC_AlarmTypeInitStructure);
 
	RTC_ClearITPendingBit(RTC_IT_ALRA);  //清除RTC闹钟A的标志
    EXTI_ClearITPendingBit(EXTI_Line17); //清除LINE17上的中断标志位 
	
	RTC_ITConfig(RTC_IT_ALRA, ENABLE);   //开启闹钟A中断
	RTC_AlarmCmd(RTC_Alarm_A, ENABLE);   //开启闹钟A 
	
    //闹钟中断连接在外部中断线17
	EXTI_InitStructure.EXTI_Line = EXTI_Line17;            //LINE17
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;    //中断事件
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //上升沿触发 
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;              //使能LINE17
    EXTI_Init(&EXTI_InitStructure);//配置

	NVIC_InitStructure.NVIC_IRQChannel = RTC_Alarm_IRQn;        //闹钟中断号
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;//抢占优先
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;       //子优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;             //使能外部中断通道
    NVIC_Init(&NVIC_InitStructure);//配置
}

void RTC_Set_AlarmB(u8 week,u8 hour,u8 min,u8 sec)
{ 
	EXTI_InitTypeDef   EXTI_InitStructure;
	RTC_AlarmTypeDef   RTC_AlarmTypeInitStructure;
	RTC_TimeTypeDef    RTC_TimeTypeInitStructure;
	
	RTC_AlarmCmd(RTC_Alarm_B, DISABLE);//关闭闹钟B 
	
    RTC_TimeTypeInitStructure.RTC_Hours = hour;  //设置闹钟B：小时
	RTC_TimeTypeInitStructure.RTC_Minutes = min; //设置闹钟B：分钟
	RTC_TimeTypeInitStructure.RTC_Seconds = sec; //设置闹钟B：秒
	RTC_TimeTypeInitStructure.RTC_H12 = RTC_H12_AM; //闹钟B设置为24小时制
  
    RTC_AlarmTypeInitStructure.RTC_AlarmDateWeekDaySel = RTC_AlarmDateWeekDaySel_WeekDay;//选择是闹钟是按日期还是按星期闹,RTC_AlarmDateWeekDaySel_WeekDay按星期闹;RTC_AlarmDateWeekDaySel_Date按日期闹
	RTC_AlarmTypeInitStructure.RTC_AlarmDateWeekDay = week;  //按选择方式，设置闹钟按日期或者星期几闹，取值范围为：week:星期(1~7)或(0~31) 日
	RTC_AlarmTypeInitStructure.RTC_AlarmMask = RTC_AlarmMask_DateWeekDay;//设置闹钟时间掩码，RTC_AlarmMask_DateWeekDay是所有的时、分、秒以及星期几/(或者每月哪一天)都不屏蔽要精确匹配，RTC_AlarmMask_DateWeekDay屏蔽星期几/每月哪一天，每天都会按设定时间闹。 
	RTC_AlarmTypeInitStructure.RTC_AlarmTime = RTC_TimeTypeInitStructure;
    RTC_SetAlarm(RTC_Format_BIN, RTC_Alarm_B, &RTC_AlarmTypeInitStructure);
 
	RTC_ClearITPendingBit(RTC_IT_ALRB);  //清除RTC闹钟B的标志
    EXTI_ClearITPendingBit(EXTI_Line17); //清除LINE17上的中断标志位 
	
	RTC_ITConfig(RTC_IT_ALRB, ENABLE);   //开启闹钟B中断
	RTC_AlarmCmd(RTC_Alarm_B, ENABLE);   //开启闹钟B 
	
    //闹钟中断连接在外部中断线17
	EXTI_InitStructure.EXTI_Line = EXTI_Line17;            //LINE17
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;    //中断事件
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //上升沿触发 
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;              //使能LINE17
    EXTI_Init(&EXTI_InitStructure);//配置

	NVIC_InitStructure.NVIC_IRQChannel = RTC_Alarm_IRQn;        //闹钟中断号
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;//抢占优先
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;       //子优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;             //使能外部中断通道
    NVIC_Init(&NVIC_InitStructure);//配置
}

//周期性唤醒定时器设置  
/*wksel:  @ref RTC_Wakeup_Timer_Definitions
#define RTC_WakeUpClock_RTCCLK_Div16        ((uint32_t)0x00000000)
#define RTC_WakeUpClock_RTCCLK_Div8         ((uint32_t)0x00000001)
#define RTC_WakeUpClock_RTCCLK_Div4         ((uint32_t)0x00000002)
#define RTC_WakeUpClock_RTCCLK_Div2         ((uint32_t)0x00000003)
#define RTC_WakeUpClock_CK_SPRE_16bits      ((uint32_t)0x00000004)
#define RTC_WakeUpClock_CK_SPRE_17bits      ((uint32_t)0x00000006)
*/
//cnt:自动重装载值.减到0,产生中断.
void RTC_Set_WakeUp(u32 wksel,u16 cnt)
{ 
	EXTI_InitTypeDef   EXTI_InitStructure;
	
	RTC_WakeUpCmd(DISABLE);      //关闭WAKE UP
	RTC_WakeUpClockConfig(wksel);//唤醒时钟选择
	RTC_SetWakeUpCounter(cnt);   //设置WAKE UP自动重装载寄存器
	RTC_ClearITPendingBit(RTC_IT_WUT);  //清除RTC WAKE UP的标志
    EXTI_ClearITPendingBit(EXTI_Line22);//清除LINE22上的中断标志位 
	 
	RTC_ITConfig(RTC_IT_WUT, ENABLE);//开启WAKE UP 定时器中断
	RTC_WakeUpCmd(ENABLE);           //开启WAKE UP 定时器　
	
	EXTI_InitStructure.EXTI_Line = EXTI_Line22;//LINE22
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//中断事件
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //上升沿触发 
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;//使能LINE22
    EXTI_Init(&EXTI_InitStructure);//配置

	NVIC_InitStructure.NVIC_IRQChannel = RTC_WKUP_IRQn; 
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;//抢占优先级1
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;//子优先级2
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//使能外部中断通道
    NVIC_Init(&NVIC_InitStructure);//配置
}

//RTC闹钟中断服务函数
void RTC_Alarm_IRQHandler(void)
{    
	if(RTC_GetFlagStatus(RTC_FLAG_ALRAF) == SET) //判断是否是ALARM_A产生的中断?
	{
		RTC_ClearFlag(RTC_FLAG_ALRAF);           //清除闹钟A中断标志
		printf("ALARM A!\r\n");                  //串口打印
        Pic1_test(30,250,40,40);                 //LCD屏显示图片1
	}
    else if(RTC_GetFlagStatus(RTC_FLAG_ALRBF) == SET) //判断是否是ALARM_B产生的中断?
	{
		RTC_ClearFlag(RTC_FLAG_ALRBF);           //清除闹钟B中断标志
		printf("ALARM B!\r\n");                  //串口打印
        Pic1_test(30,300,40,40);                 //LCD屏显示图片1
	}
	EXTI_ClearITPendingBit(EXTI_Line17);	     //清除中断线17的中断标志 											 
}

//RTC WAKE UP中断服务函数
void RTC_WKUP_IRQHandler(void)
{    
	if(RTC_GetFlagStatus(RTC_FLAG_WUTF) == SET)  //判断是否是WK_UP产生的中断?
	{ 
		RTC_ClearFlag(RTC_FLAG_WUTF);	         //清除唤醒中断标志
		LED1=!LED1;                              //LED1状态翻转
        //LCD_Clear(RED);                        //清屏
	}   
	EXTI_ClearITPendingBit(EXTI_Line22);         //清除中断线22的中断标志 								
}
 
//=========== 供 USMart 调用的壳函数 ===========
void rtc_set_time(u8 hour,u8 min,u8 sec){ RTC_Set_Time(hour,min,sec,RTC_H12_AM); }
void rtc_set_date(u8 year,u8 month,u8 date,u8 week){ RTC_Set_Date(year,month,date,week); }
void set_alarm1(u8 hour,u8 min,u8 sec){ RTC_Set_AlarmA(1,hour,min,sec); }
void set_alarm2(u8 hour,u8 min,u8 sec){ RTC_Set_AlarmB(1,hour,min,sec); }

