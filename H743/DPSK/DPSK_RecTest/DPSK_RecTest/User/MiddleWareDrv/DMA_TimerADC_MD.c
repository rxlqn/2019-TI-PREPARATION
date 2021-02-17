
#include "bsp.h"			
#include "ringbuf.h"

#define			USE_ADC1


#ifdef			USE_ADC3


#define ADCx                     ADC3
#define ADC_CHANNEL              ADC_Channel_7
#define ADCx_CLK                 RCC_APB2Periph_ADC3
#define ADCx_CHANNEL_GPIO_CLK    RCC_AHB1Periph_GPIOF
#define GPIO_PIN                 GPIO_Pin_9
#define GPIO_PORT                GPIOF
#define DMA_CHANNELx             DMA_Channel_2
#define DMA_STREAMx              DMA2_Stream0
#define ADCx_DR_ADDRESS          ((uint32_t)0x4001224C)

#else 
	
	#ifdef		USE_ADC1

	#define ADCx                     ADC1
	#define ADC_CHANNEL              ADC_Channel_11
	#define ADCx_CLK                 RCC_APB2Periph_ADC1
	#define ADCx_CHANNEL_GPIO_CLK    RCC_AHB1Periph_GPIOC
	#define GPIO_PIN                 GPIO_Pin_1
	#define GPIO_PORT                GPIOC
	#define DMA_CHANNELx             DMA_Channel_0
	#define DMA_STREAMx              DMA2_Stream0
	#define ADCx_DR_ADDRESS          ((uint32_t)(&(ADC1->DR)))

	#else 

		#ifdef		USE_ADC2
		#endif
		
	#endif

#endif

__IO uint16_t uhADCxConvertedValue[8192];


__IO uint8_t u8ADC_Halffull = 0;

uint32_t u32ADC __attribute__((at(ADCx_DR_ADDRESS)));

//__IO uint16_t uhADC_QueueData[8192];

//tRingBufU16Object sADC_RingBuf = {
//																	8000,
//																	0,
//																	0,
//																	(uint16_t *)uhADC_QueueData
//																 };


void DMA2_Stream0_IRQHandler(void)
{
 		int i;
	
		if (DMA2->LISR & DMA_IT_HTIF0)										// Stream0半传输事件完�
 		{ 

// 			 DMA1->LIFCR |= DMA_FLAG_HTIF1;									// 清除通道3半传输完成标�
			 DMA2->LIFCR = (DMA_FLAG_HTIF0 & 0x0fff0fff);									// 清除通道3半传输完成标�

			 u8ADC_Halffull = 1;												// 前半个缓冲区�
			 
//			 for (i = 0; i < (4000); i++)
			 {
					 // 一旦半满中断产生，则把充满缓冲区一半的数据立即读出�
//					 RingBufU16WriteOne(&sADC_RingBuf, uhADCxConvertedValue[i]);
				 
			 }

		}// End if (DMA2->LISR & DMA_IT_HTIF0)
		else 
		if (DMA2->LISR & DMA_IT_TCIF0)															// 通道3传输事件完成
 		{ 
						u8ADC_Halffull = 2;																			// 后半个缓冲区�
 						DMA2->LIFCR = (DMA_FLAG_TCIF0 & 0x0fff0fff);						// 清除Stream0传输完成标志
							
//						for (i = 0; i < (4000); i++)
						{
//								RingBufU16WriteOne(&sADC_RingBuf, uhADCxConvertedValue[4000 + i]);
								
						}
						
		}// End else if (DMA1->ISR & DMA1_IT_TC3)		
	
}


/**
  * @brief  ADC3 channel07 with DMA configuration
  * @note   This function Configure the ADC peripheral  
            1) Enable peripheral clocks
            2) DMA2_Stream0 channel2 configuration
            3) Configure ADC Channel7 pin as analog input
            4) Configure ADC3 Channel7 
  * @param  None
  * @retval None
  */
	// ADCCLK的最高工作频�6Mhz
	// 当系统时钟为144Mhz，APB2为HCLK/2，ADC_Prescaler为二分频时，
	// 可以获得此最高工作频�44/4=36Mhz
	// 如果采取其它配置，需注意ADC CLK不能超出36Mhz�
void ADC_Config(uint32_t u32BufferSize)
{
  ADC_InitTypeDef       ADC_InitStructure;
  ADC_CommonInitTypeDef ADC_CommonInitStructure;
  DMA_InitTypeDef       DMA_InitStructure;
  GPIO_InitTypeDef      GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

  /* Enable ADCx, DMA and GPIO clocks ****************************************/ 
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
  RCC_AHB1PeriphClockCmd(ADCx_CHANNEL_GPIO_CLK, ENABLE);  
  RCC_APB2PeriphClockCmd(ADCx_CLK, ENABLE);
  
	NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream0_IRQn;					// ADC DMA stream0 ch0
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	
  /* DMA2 Stream0 channel2 configuration **************************************/
  DMA_InitStructure.DMA_Channel = DMA_CHANNELx;  
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)ADCx_DR_ADDRESS;
  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&uhADCxConvertedValue;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
  DMA_InitStructure.DMA_BufferSize = u32BufferSize;
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
  DMA_Init(DMA_STREAMx, &DMA_InitStructure);
  DMA_Cmd(DMA_STREAMx, ENABLE);
	
	DMA_ITConfig(DMA_STREAMx, DMA_IT_TC | DMA_IT_HT,ENABLE);  //配置DMA接收完成和半满后产生中断
	

  /* Configure ADC3 Channel7 pin as analog input ******************************/
  GPIO_InitStructure.GPIO_Pin = GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
  GPIO_Init(GPIO_PORT, &GPIO_InitStructure);

  /* ADC Common Init **********************************************************/
  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
  ADC_CommonInit(&ADC_CommonInitStructure);

  /* ADC3 Init ****************************************************************/
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_Rising;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T2_CC2;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfConversion = 1;
  ADC_Init(ADCx, &ADC_InitStructure);

  /* ADC3 regular channel7 configuration **************************************/
  ADC_RegularChannelConfig(ADCx, ADC_CHANNEL, 1, ADC_SampleTime_15Cycles);

 /* Enable DMA request after last transfer (Single-ADC mode) */
  ADC_DMARequestAfterLastTransferCmd(ADCx, ENABLE);

  /* Enable ADC3 DMA */
  ADC_DMACmd(ADCx, ENABLE);

  /* Enable ADC3 */
  ADC_Cmd(ADCx, ENABLE);
}



