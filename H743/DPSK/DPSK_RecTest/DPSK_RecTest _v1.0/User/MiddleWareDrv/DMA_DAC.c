
#include "stm32f4xx.h"

#include "DMA_DAC.h"

#define DAC_DHR12R2_ADDRESS    0x40007414
#define DAC_DHR12R1_ADDRESS    0x40007408
	

__IO uint8_t u8DAC2_Halffull = 0;
__IO uint8_t u8DAC1_Halffull = 0;

uint16_t aSRC_0[160] = {0};
uint16_t aSRC_Pi[160] = {0};
uint16_t aSRC_Buffer[4096] = {0};
uint16_t u16CacheInput[160] = {0};

void DMA1_Stream6_IRQHandler(void)
{
 		int i;
	
		if (DMA1->HISR & DMA_IT_HTIF6)										// Stream0半传输事件完成
 		{ 

// 			 DMA1->LIFCR |= DMA_FLAG_HTIF1;									// 清除通道3半传输完成标志
			 DMA1->HIFCR = (DMA_FLAG_HTIF6 & 0x0fff0fff);									// 清除通道3半传输完成标志

			 u8DAC2_Halffull = 1;												// 前半个缓冲区满
			 
//			 for (i = 0; i < (4000); i++)
//			 {
//					 // 一旦半满中断产生，则把充满缓冲区一半的数据立即读出来
//					 RingBufU16WriteOne(&sADC_RingBuf, uhADCxConvertedValue[i]);
//				 
//			 }

		}// End if (DMA1->HISR & DMA_IT_HTIF6)
		else 
		if (DMA1->HISR & DMA_IT_TCIF6)															// 通道3传输事件完成
 		{ 
						u8DAC2_Halffull = 2;																			// 后半个缓冲区满
 						DMA1->HIFCR = (DMA_FLAG_TCIF6 & 0x0fff0fff);						// 清除Stream0传输完成标志
							
//						for (i = 0; i < (4000); i++)
//						{
//								RingBufU16WriteOne(&sADC_RingBuf, uhADCxConvertedValue[4000 + i]);
//								
//						}
						
		}// End else if (DMA1->HISR & DMA_IT_TCIF6)	
	
}


void DMA1_Stream5_IRQHandler(void)
{
 		int i;
	
		if (DMA1->HISR & DMA_IT_HTIF5)										// Stream0半传输事件完成
 		{ 

// 			 DMA1->LIFCR |= DMA_FLAG_HTIF1;									// 清除通道3半传输完成标志
			 DMA1->HIFCR = (DMA_FLAG_HTIF5 & 0x0fff0fff);									// 清除通道3半传输完成标志

			 u8DAC1_Halffull = 1;												// 前半个缓冲区满
			 
//			 for (i = 0; i < (4000); i++)
//			 {
//					 // 一旦半满中断产生，则把充满缓冲区一半的数据立即读出来
//					 RingBufU16WriteOne(&sADC_RingBuf, uhADCxConvertedValue[i]);
//				 
//			 }

		}// End if (DMA1->HISR & DMA_IT_HTIF6)
		else 
		if (DMA1->HISR & DMA_IT_TCIF5)															// 通道3传输事件完成
 		{ 
						u8DAC1_Halffull = 2;																			// 后半个缓冲区满
 						DMA1->HIFCR = (DMA_FLAG_TCIF5 & 0x0fff0fff);						// 清除Stream0传输完成标志
							
//						for (i = 0; i < (4000); i++)
//						{
//								RingBufU16WriteOne(&sADC_RingBuf, uhADCxConvertedValue[4000 + i]);
//								
//						}
						
		}// End else if (DMA1->HISR & DMA_IT_TCIF6)	
	
}




/**             
  * @brief  TIM6 Configuration
  * @note   TIM6 configuration is based on APB1 frequency
  * @note   TIM6 Update event occurs each TIM6CLK/256   
  * @param  None
  * @retval None
  */
static void TIM6_Config(void)
{
  TIM_TimeBaseInitTypeDef    TIM_TimeBaseStructure;
	
	
  /* TIM6 Periph clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
  
  /* --------------------------------------------------------
  TIM3 input clock (TIM6CLK) is set to 2 * APB1 clock (PCLK1), 
  since APB1 prescaler is different from 1.   
    TIM6CLK = 2 * PCLK1  
    TIM6CLK = HCLK / 2 = SystemCoreClock /2 
          
  TIM6 Update event occurs each TIM6CLK/256 

  Note: 
   SystemCoreClock variable holds HCLK frequency and is defined in system_stm32f4xx.c file.
   Each time the core clock (HCLK) changes, user had to call SystemCoreClockUpdate()
   function to update SystemCoreClock variable value. Otherwise, any configuration
   based on this variable will be incorrect.    

  ----------------------------------------------------------- */
  /* Time base configuration */
  TIM_TimeBaseStructInit(&TIM_TimeBaseStructure); 
  TIM_TimeBaseStructure.TIM_Period = 420- 1;          
  TIM_TimeBaseStructure.TIM_Prescaler = 0;       
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;    
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  
  TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);

  /* TIM6 TRGO selection */
  TIM_SelectOutputTrigger(TIM6, TIM_TRGOSource_Update);
  
  /* TIM6 enable counter */
  TIM_Cmd(TIM6, ENABLE);
}


void DAC_Ch2_SineWaveConfig(uint16_t u16DAC_Buffer[], uint32_t u32DMA_Len)
{
  DMA_InitTypeDef DMA_InitStructure;
	
	NVIC_InitTypeDef NVIC_InitStructure;
	
	DAC_InitTypeDef  DAC_InitStructure;
	
/* Preconfiguration before using DAC----------------------------------------*/
  GPIO_InitTypeDef GPIO_InitStructure;

	
	
	/* DMA1 clock enable */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
  /* GPIOA clock enable (to be used with DAC) */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);                         
  /* DAC Periph clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);	

	
	/* TIM6 Configuration ------------------------------------------------------*/
  TIM6_Config();  
	
	
  /* DAC channel 2 (DAC_OUT2 = PA.5) configuration */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  /* DAC channel2 Configuration */
  DAC_InitStructure.DAC_Trigger = DAC_Trigger_T6_TRGO;
  DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
  DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
  DAC_Init(DAC_Channel_2, &DAC_InitStructure);

	
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream6_IRQn;					// ADC DMA stream0 ch0
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	
  /* DMA1_Stream6 channel7 configuration **************************************/
  DMA_DeInit(DMA1_Stream6);
  DMA_InitStructure.DMA_Channel = DMA_Channel_7;  
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)DAC_DHR12R2_ADDRESS;
  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)u16DAC_Buffer;
  DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
  DMA_InitStructure.DMA_BufferSize = u32DMA_Len;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  DMA_Init(DMA1_Stream6, &DMA_InitStructure);

  /* Enable DMA1_Stream6 */
  DMA_Cmd(DMA1_Stream6, ENABLE);

	DMA_ITConfig(DMA1_Stream6, DMA_IT_TC | DMA_IT_HT,ENABLE);  //配置DMA接收完成和半满后产生中断


  /* Enable DAC Channel2 */
  DAC_Cmd(DAC_Channel_2, ENABLE);

  /* Enable DMA for DAC Channel2 */
  DAC_DMACmd(DAC_Channel_2, ENABLE);
}


void DAC_Ch1_SineWaveConfig(uint16_t u16DAC_Buffer[], uint32_t u32DMA_Len)
{
  DMA_InitTypeDef DMA_InitStructure;
	
	NVIC_InitTypeDef NVIC_InitStructure;
	
	DAC_InitTypeDef  DAC_InitStructure;
	
/* Preconfiguration before using DAC----------------------------------------*/
  GPIO_InitTypeDef GPIO_InitStructure;

	
	/* DMA1 clock enable */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
  /* GPIOA clock enable (to be used with DAC) */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);                         
  /* DAC Periph clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);	
	
	
/* DAC channel 1 (DAC_OUT1 = PA.4) configuration */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

	
	/* TIM6 Configuration ------------------------------------------------------*/
  TIM6_Config();  
	
	
  /* DAC channel1 Configuration */
  DAC_InitStructure.DAC_Trigger = DAC_Trigger_T6_TRGO;
  DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
  DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
  DAC_Init(DAC_Channel_1, &DAC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream5_IRQn;					// ADC DMA stream0 ch0
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* DMA1_Stream5 channel7 configuration **************************************/  
  DMA_DeInit(DMA1_Stream5);
  DMA_InitStructure.DMA_Channel = DMA_Channel_7;  
  DMA_InitStructure.DMA_PeripheralBaseAddr = DAC_DHR12R1_ADDRESS;
  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)u16DAC_Buffer;
  DMA_InitStructure.DMA_BufferSize = u32DMA_Len;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
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

  /* Enable DMA1_Stream5 */
  DMA_Cmd(DMA1_Stream5, ENABLE);
	
	DMA_ITConfig(DMA1_Stream5, DMA_IT_TC | DMA_IT_HT,ENABLE);  //配置DMA接收完成和半满后产生中断
  
  /* Enable DAC Channel1 */
  DAC_Cmd(DAC_Channel_1, ENABLE);

  /* Enable DMA for DAC Channel1 */
  DAC_DMACmd(DAC_Channel_1, ENABLE);
}