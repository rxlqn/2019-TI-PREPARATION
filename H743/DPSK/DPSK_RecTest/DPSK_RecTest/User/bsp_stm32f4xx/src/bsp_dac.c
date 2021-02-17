/*
*********************************************************************************************************
*
*	模块名称 : DAC驱动模块
*	文件名称 : bsp_dac.c
*	版    本 : V1.0
*	说    明 : 实现DAC输出阶梯波
*	修改记录 :
*		版本号    日期        作者     说明
*		V1.0    2013-12-30   armfly   正式发布
*
*	Copyright (C), 2013-2014, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/
#include "bsp.h"


#define DAC_DHR12R2_ADDRESS    0x40007414
#define DAC_DHR8R1_ADDRESS     0x40007410

/* 梯形波参考点 */
const uint8_t aEscalator8bit[6] = {0x0, 0x33, 0x66, 0x99, 0xCC, 0xFF};
/*
*********************************************************************************************************
*	函 数 名: bsp_InitDAC
*	功能说明: DAC初始化
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_InitDAC(void)
{   
    TIM6_Config(); 
    DAC_GPIOConfig(); 
    DAC_Ch1_EscalatorConfig();
}

/*
*********************************************************************************************************
*	函 数 名: TIM6_Config
*	功能说明: 配置定时器6，用于触发DAC，每TIM6CLK/256更新一次
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void TIM6_Config(void)
{
     TIM_TimeBaseInitTypeDef    TIM_TimeBaseStructure;
      
    
    /* 使能TIM6时钟 */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
    
    /* --------------------------------------------------------
    system_stm32f4xx.c 文件中 void SetSysClock(void) 函数对时钟的配置如下：

    HCLK = SYSCLK / 1     (AHB1Periph)
    PCLK2 = HCLK / 2      (APB2Periph)
    PCLK1 = HCLK / 4      (APB1Periph)

    因为APB1 prescaler != 1, 所以 APB1上的TIMxCLK = PCLK1 x 2 = SystemCoreClock / 2;
    因为APB2 prescaler != 1, 所以 APB2上的TIMxCLK = PCLK2 x 2 = SystemCoreClock;

    APB1 定时器有 TIM2, TIM3 ,TIM4, TIM5, TIM6, TIM7, TIM12, TIM13, TIM14
    APB2 定时器有 TIM1, TIM8 ,TIM9, TIM10, TIM11
          
    TIM6 更新周期是 = TIM6CLK / （TIM_Period + 1）/（TIM_Prescaler + 1）= TIM6CLK/256
    ----------------------------------------------------------- */
    
    /* 配置TIM6 */
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure); 
    TIM_TimeBaseStructure.TIM_Period = 0xFF;          
    TIM_TimeBaseStructure.TIM_Prescaler = 0;       
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;    
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  
    TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);

    /* TIM6 TRGO 选择 */
    TIM_SelectOutputTrigger(TIM6, TIM_TRGOSource_Update);

    /* 使能TIM6 */
    TIM_Cmd(TIM6, ENABLE);     
}

/*
*********************************************************************************************************
*	函 数 名: DAC_GPIOConfig
*	功能说明: DAC引脚配置
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void DAC_GPIOConfig(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    /*  使能引脚时钟 */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

    /* DAC通道1输出引脚配置（PA.4） */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/*
*********************************************************************************************************
*	函 数 名: DAC_Ch1_EscalatorConfig
*	功能说明: DAC通道1输出阶梯波配置
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void DAC_Ch1_EscalatorConfig(void)
{
    DAC_InitTypeDef  DAC_InitStructure;
    DMA_InitTypeDef DMA_InitStructure;

    /* DAC 外设时钟使能 */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);
    
    /* DMA1 时钟使能 */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
     
    /* DAC 通道1配置 */
    DAC_InitStructure.DAC_Trigger = DAC_Trigger_T6_TRGO;
    DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
    DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
    DAC_Init(DAC_Channel_1, &DAC_InitStructure);
    
    /* DMA1_Stream5 通道7配置 **************************************/  
    DMA_DeInit(DMA1_Stream5);
    DMA_InitStructure.DMA_Channel = DMA_Channel_7;  
    DMA_InitStructure.DMA_PeripheralBaseAddr = DAC_DHR8R1_ADDRESS;
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&aEscalator8bit;
    DMA_InitStructure.DMA_BufferSize = 6;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
    DMA_Init(DMA1_Stream5, &DMA_InitStructure);    

    /* 使能 DMA1_Stream5 */
    DMA_Cmd(DMA1_Stream5, ENABLE);

    /* 使能 DAC 通道1 */
    DAC_Cmd(DAC_Channel_1, ENABLE);

    /* 使能DAC通道1的DMA功能 */
    DAC_DMACmd(DAC_Channel_1, ENABLE); 
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
