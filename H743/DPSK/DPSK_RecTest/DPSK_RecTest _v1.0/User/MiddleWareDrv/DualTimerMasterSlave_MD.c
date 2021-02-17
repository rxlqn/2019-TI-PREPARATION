
#include "bsp.h"			/* 底层硬件驱动 */

#define	ADCNUM					256

void DualTimer_Init(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef         TIM_OCInitStructure;	
  GPIO_InitTypeDef GPIO_InitStructure;
	
//   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);//使能GPIOC和AFIO端口功能
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	
	// 如果是Stm32f103
	// 定时器2 PWM频率为72000000/72/10=100Khz
	// 定时器1 PWM频率为72000000/72/(10*ADCNUM*2)
	// 如果是Stm32f407，APB1最高频率168Mhz/4=42Mhz
	// CMSIS默认配置
	/* PCLK2 = HCLK / 2*/
  /* PCLK1 = HCLK / 4*/
	// 因此APB1的频率是42M，由于分频系数是4不等于1，则
	// 定时器2 使用APB1，因此PWM频率为HCLK/4 * 2 /TIM_Prescaler/10 = 100Khz
	// 定时器1 使用APB2，TIMCLK = 2 * HCLK / 2 = 168000000
	// PWM频率为168000000/TIM_Prescaler/(10*ADCNUM*2) = 100Khz/(ADCNUM*2)
	
	//初始化定时器 TIM1
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

	//初始化定时器 TIM2
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

	/* TIM1 configuration ------------------------------------------------------*/ 
  /* Time Base configuration */
  TIM_TimeBaseStructInit(&TIM_TimeBaseStructure); 
  TIM_TimeBaseStructure.TIM_Period = 1000000/500 - 1;		// 10*(ADCNUM)*2 - 1;       			// 采样频率   
  TIM_TimeBaseStructure.TIM_Prescaler = 168/1 - 1;       
  TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;    
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  
  TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
  /* TIM1 channel2 configuration in PWM mode */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; 
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;                
  TIM_OCInitStructure.TIM_Pulse = 10*(ADCNUM)/4;							//10*1*(ADCNUM);
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;         
  TIM_OC2Init(TIM1, &TIM_OCInitStructure);

	/* Master Mode selection */
  TIM_SelectOutputTrigger(TIM1, TIM_TRGOSource_OC2Ref);

  /* Select the Master Slave Mode */
  TIM_SelectMasterSlaveMode(TIM1, TIM_MasterSlaveMode_Enable);
	
	
  /* TIM2 configuration ------------------------------------------------------*/ 
  /* Time Base configuration */
  TIM_TimeBaseStructInit(&TIM_TimeBaseStructure); 
  TIM_TimeBaseStructure.TIM_Period = 10 - 1;          // 400khz 
  TIM_TimeBaseStructure.TIM_Prescaler = 84/4 - 1;     // 4000khz  
  TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;    
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
  /* TIM2 channel2 configuration in PWM mode */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; 
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;                
  TIM_OCInitStructure.TIM_Pulse = 10/2 ;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;         
  TIM_OC2Init(TIM2, &TIM_OCInitStructure);

	TIM_SelectInputTrigger(TIM2,TIM_TS_ITR0);
	
	TIM_SelectSlaveMode(TIM2, TIM_SlaveMode_Gated);

//	/* TIM1 counter enable */
//  TIM_Cmd(TIM1, ENABLE);
	/* TIM1 main Output Enable */
  TIM_CtrlPWMOutputs(TIM1, ENABLE);  

//	/* TIM2 counter enable */
//  TIM_Cmd(TIM2, ENABLE);
	/* TIM1 main Output Enable */
  TIM_CtrlPWMOutputs(TIM2, ENABLE);  

//   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//使能GPIOC和AFIO端口功能
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
	GPIO_Init(GPIOA, &GPIO_InitStructure);								//初始化GPIOA端口

	/* Connect TIM1 pins to AF2 */  
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_TIM2);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_TIM1); 
	
	// 必须先使能从定时器Timer2，否则会有同步问题
  TIM_Cmd(TIM2, ENABLE);
	
	/* TIM1 counter enable */
  TIM_Cmd(TIM1, ENABLE);

}

