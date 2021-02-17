#include "adc.h" 
//////////////////////////////////////////////////////////////////////////////////	 
/*
PA5 通道5
PA5 通道6
采样率
*/
////////////////////////////////////////////////////////////////////////////////// 		
u16 Adc1_Value[2];//ADC1的值

/*
 * 函数名：fftji41024
 * 描述  ：基四fft
 * 输入  ：采样频率
 * 输出  ：无
 */														   
void  Adc_Init(void)
{    
  GPIO_InitTypeDef      GPIO_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	ADC_InitTypeDef       ADC_InitStructure;
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//使能GPIOC时钟
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); //使能ADC1时钟

 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_6;// 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;//模拟输入
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;//不带上下拉
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化
	
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,ENABLE);	  //ADC1复位
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,DISABLE);	//复位结束

	
  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;//独立模式
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_15Cycles;//两个采样阶段之间的延迟5个时钟
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_1; //DMA模式1
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;//预分频4分频。ADCCLK=PCLK2/4=84/4=21Mhz,ADC时钟最好不要超过36Mhz 
  ADC_CommonInit(&ADC_CommonInitStructure);//初始化
	
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;//12位模式
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;//扫描模式	
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;//开启连续转换
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;//禁止触发检测，使用软件触发
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//右对齐	
  ADC_InitStructure.ADC_NbrOfConversion = 2;//2个通道 
  ADC_Init(ADC1, &ADC_InitStructure);//ADC初始化
	
	ADC_Cmd(ADC1, ENABLE);//开启AD转换器	

  /* Enable DMA request after last transfer (multi-ADC mode) ******************/
  //ADC_MultiModeDMARequestAfterLastTransferCmd(ENABLE);
  ADC_DMACmd(ADC1, ENABLE);

  ADC_RegularChannelConfig(ADC1,ADC_Channel_5,1, ADC_SampleTime_56Cycles     );//
  ADC_RegularChannelConfig(ADC1,ADC_Channel_6,2, ADC_SampleTime_56Cycles     );//
   /* 使能ADC DMA的请求 */
        
   ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);
}	

/*
 * 函数名：Adc_DMA_Init
 * 描述  ：adc的dma初始化
 * 输入  ：无
 * 输出  ：无
 */
void Adc_DMA_Init(void)
{
//ADC1 DMA初始化
	DMA_InitTypeDef DMA_InitStructure;
	RCC->AHB1ENR|=RCC_AHB1Periph_DMA2;
	DMA_InitStructure.DMA_BufferSize =2;
	DMA_InitStructure.DMA_Channel = DMA_Channel_0;//通道0对应ADC1
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&Adc1_Value;   //目标数据位
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_BASE+0x4C;  //ADC1->DR地址
	DMA_InitStructure.DMA_PeripheralBurst =DMA_PeripheralBurst_Single;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_Init(DMA2_Stream0,&DMA_InitStructure);//ADC1对应数据流0
	DMA_Cmd(DMA2_Stream0,ENABLE);//ADC1对应数据流0
	//ADC2 DMA初始化	

// /* DMA2_Stream0 enable */
  DMA_ClearITPendingBit(DMA2_Stream0,DMA_IT_TC);
  DMA_ITConfig(DMA2_Stream0,DMA_IT_TC,ENABLE);

}




