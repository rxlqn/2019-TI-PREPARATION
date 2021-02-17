#include "timer.h"
#include "led.h"
#include "dac.h"
#include "ui.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//定时器 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/5/4
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	 
#include "math.h"
extern u16 sine[1024];
//extern u8 square_wave[1024];
unsigned long int counter = 0;
u16 m;
//#define frequence  10000000
//#define n (u32)(((1024*frequence)/107761000))
//u32 count_num = (u32)(((1024*frequence)/107761000))

extern double amp_div;
extern u32 count_num;
//通用定时器3中断初始化
//arr：自动重装值。
//psc：时钟预分频数
//定时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=定时器工作频率,单位:Mhz
//这里使用的是定时器3!
void TIM3_Int_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);  ///使能TIM3时钟
	
  TIM_TimeBaseInitStructure.TIM_Period = arr; 	//自动重装载值
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc;  //定时器分频
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);//初始化TIM3
	
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE); //允许定时器3更新中断
	TIM_Cmd(TIM3,ENABLE); //使能定时器3
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn; //定时器3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01; //抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x03; //子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void TIM2_Int_Init(u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	
	TIM_TimeBaseInitStructure.TIM_Prescaler = psc;
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式

	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStructure);//初始化TIM3	
}

//定时器3中断服务函数
void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET) //溢出中断
	{		
		DAC_SetChannel1Data(DAC_Align_12b_R,(u16)((double)sine[counter>>22]/amp_div));
		
		switch(counter>>30)
		{
			case 0x00:
			{
				SQU1 = 1;
				SQU2 = 1;
				SQU3 = 0;
				SQU4 = 0;
				
			}break;
			case 0x01:
			{
				SQU1 = 1;
				SQU2 = 0;
				SQU3 = 0;
				SQU4 = 1;
				
			}break;
			case 0x10:
			{
				SQU1 = 0;
				SQU2 = 0;
				SQU3 = 1;
				SQU4 = 1;
				
			}break;
			case 0x11:
			{
				SQU1 = 0;
				SQU4 = 0;
				SQU2 = 1;
				SQU3 = 1;
				
			}break;
		}
		counter += count_num;
	}
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);  //清除中断标志位
}
