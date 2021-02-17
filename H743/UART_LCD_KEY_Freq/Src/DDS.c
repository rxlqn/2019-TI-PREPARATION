#include "DDS.h"
#include "math.h"
#include "arm_math.h"
#include "adc.h"
#define Fs 1000000  //1M
__IO uint16_t* u16DDS_DataOut = (__IO uint16_t*)0x38000000;


u16 DDSTable[Table_NUM];
u32 PhaseAccumulator=0;
u32 PhaseDelta=0;

void InitSinTable(void)
{
		u32 i;
		for( i = 0; i < Table_NUM; i++ )   // 2Π 重新映射 Table_NUM份 产生正弦码表
		{
				 float temp=(float)(2*3.141592653)/(float)Table_NUM*(float)i;
				 DDSTable[i] = 32767.0 + 32767.0*(float)arm_sin_f32(temp);	
		}
}

 
void DDS_init(u32 Fx)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pins : PB0 PB1 PB2 PB10 
                           PB11 PB12 PB13 PB14 
                           PB15 PB3 PB4 PB5 
                           PB6 PB7 PB8 PB9 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_10 
                          |GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14 
                          |GPIO_PIN_15|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5 
                          |GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
 	
	hdma_bdma_generator0.XferCpltCallback         = DMA1_stream0_FULL_Handler;
    hdma_bdma_generator0.XferHalfCpltCallback     = DMA1_stream0_HALF_Handler;
	HAL_DMAEx_EnableMuxRequestGenerator (&hdma_bdma_generator0);
    HAL_DMA_Start_IT(&hdma_bdma_generator0, (uint32_t)u16DDS_DataOut, (uint32_t)&GPIOB->ODR, 1024);

	HAL_LPTIM_PWM_Start(&hlptim2, 100-1, 0);		//100M/2 主频
//	Fx=1000;					// 100k
	PhaseDelta = ((double)Fx / Fs) *65536*65536;	// 放大32bit
}


void DMA1_stream0_HALF_Handler(DMA_HandleTypeDef* hdma)							//半满
{
    //@arg DMA_IT_TC: Transfer complete interrupt mask.
    //@arg DMA_IT_HT: Half transfer complete interrupt mask.
    // 半满

    for(int i = 0; i < 512; i++)
    {
        u16DDS_DataOut[i] = (uint16_t)DDSTable[(PhaseAccumulator >> 22) & (0x3FF)];
        PhaseAccumulator += PhaseDelta;
    }
//    SCB_CleanInvalidateDCache();
}

void DMA1_stream0_FULL_Handler(DMA_HandleTypeDef* hdma)
{
    // 全满

    for(int i = 512; i < 1024; i++)
    {
        u16DDS_DataOut[i] = (uint16_t)DDSTable[(PhaseAccumulator >> 22) & (0x3FF)];
        PhaseAccumulator += PhaseDelta;

    }
//    SCB_CleanInvalidateDCache();

}

