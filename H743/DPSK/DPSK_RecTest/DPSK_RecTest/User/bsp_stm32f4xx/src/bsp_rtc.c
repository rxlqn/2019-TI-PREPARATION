/*
*********************************************************************************************************
*
*	模块名称 : RTC
*	文件名称 : bsp_rtc.c
*	版    本 : V1.0
*	说    明 : RTC底层驱动
*	修改记录 :
*		版本号   日期        作者     说明
*		V1.0    2013-12-11  armfly   正式发布
*
*	Copyright (C), 2013-2014, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#include "bsp.h"


/* 变量 */
RTC_InitTypeDef  RTC_InitStructure;
RTC_TimeTypeDef  RTC_TimeStruct;

/* 仅允许本文件内调用的函数声明 */
static void RTC_Config(void);
static void RTC_AlarmConfig(void);

/*
*********************************************************************************************************
*	函 数 名: bsp_InitRTC
*	功能说明: 初始化RTC
*	形    参：无
*	返 回 值: 无		        
*********************************************************************************************************
*/
void bsp_InitRTC(void)
{
	RTC_Config();
	RTC_AlarmConfig();
}

/*
*********************************************************************************************************
*	函 数 名: RTC_Config
*	功能说明: RTC配置
*	形    参：无
*	返 回 值: 无		        
*********************************************************************************************************
*/
static void RTC_Config(void)
{
	/* 使能PWR时钟 */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

	/* 允许访问RTC */
	PWR_BackupAccessCmd(ENABLE);

	/* 复位RTC域 */
	RCC_BackupResetCmd(ENABLE);
	RCC_BackupResetCmd(DISABLE);

	/* 使能 LSE OSC */
	RCC_LSEConfig(RCC_LSE_ON);

	/* 等待就行 */  
	while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
	{
	}

	/* 选择时钟源 */
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

	/* 配置RTC数寄存器和分频 */
	/* ck_spre(1Hz) = RTCCLK(LSI) /(AsynchPrediv + 1)*(SynchPrediv + 1)*/
	RTC_InitStructure.RTC_AsynchPrediv = 0x7F;
	RTC_InitStructure.RTC_SynchPrediv  = 0xFF;
	RTC_InitStructure.RTC_HourFormat   = RTC_HourFormat_24;
	RTC_Init(&RTC_InitStructure);

	/* 设置时间 00h 00mn 00s AM */
	RTC_TimeStruct.RTC_H12     = RTC_H12_AM;
	RTC_TimeStruct.RTC_Hours   = 0;
	RTC_TimeStruct.RTC_Minutes = 0;
	RTC_TimeStruct.RTC_Seconds = 0;  
	RTC_SetTime(RTC_Format_BCD, &RTC_TimeStruct);
}

/*
*********************************************************************************************************
*	函 数 名: RTC_AlarmConfig
*	功能说明: 闹钟配置
*	形    参：无
*	返 回 值: 无		        
*********************************************************************************************************
*/
static void RTC_AlarmConfig(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;
	RTC_AlarmTypeDef RTC_AlarmStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	/* EXTI 配置 */
	EXTI_ClearITPendingBit(EXTI_Line17);
	EXTI_InitStructure.EXTI_Line = EXTI_Line17;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	/* 使能 RTC Alarm 中断 */
	NVIC_InitStructure.NVIC_IRQChannel = RTC_Alarm_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* 设置 alarm A 屏蔽 */
	RTC_AlarmStructure.RTC_AlarmMask = RTC_AlarmMask_All;
	RTC_AlarmStructure.RTC_AlarmDateWeekDaySel = RTC_AlarmDateWeekDaySel_Date;
	RTC_AlarmStructure.RTC_AlarmDateWeekDay = RTC_Weekday_Monday;
	RTC_AlarmStructure.RTC_AlarmTime.RTC_Hours = 0x0;
	RTC_AlarmStructure.RTC_AlarmTime.RTC_Minutes =0x0;
	RTC_AlarmStructure.RTC_AlarmTime.RTC_Seconds =0x0;
	RTC_SetAlarm(RTC_Format_BCD, RTC_Alarm_A, &RTC_AlarmStructure);

	/* 设置 alarm A 亚秒 和 使能亚秒闹钟 : 每秒8次中断 */
	RTC_AlarmSubSecondConfig(RTC_Alarm_A, 0xFF, RTC_AlarmSubSecondMask_SS14_5);

	/* 使能 alarm A 中断 */
	RTC_ITConfig(RTC_IT_ALRA, ENABLE);
}

