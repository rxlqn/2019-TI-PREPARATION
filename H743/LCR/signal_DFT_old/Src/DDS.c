#include "DDS.h"
#include "math.h"
#include "arm_math.h"

#define Fs 1000000  //10M
__IO uint16_t* u16DDS_DataOut = (__IO uint16_t*)0x38000000;


u8 DDSTable[Table_NUM];
u8 DDSTable2[Table_NUM];

u32 PhaseAccumulator=0;
u32 PhaseDelta=0;

u32 PhaseAccumulator_y=0;
u32 PhaseDelta_y=0;

u32 PhaseAccumulator_n=0;
u32 PhaseDelta_n=0; 
 
#define Freq 1000
//u16 d_t = 0;
#define delta_Phi 3.31612
void InitSinTable(void)
{
		u32 i;
		for( i = 0; i < Table_NUM; i++ )   // 2Π 重新映射 Table_NUM份 产生正弦码表
		{
				 float temp=(float)(2*3.141592653)/(float)Table_NUM*(float)i;
				 DDSTable[i] = 127.0 + 0.6*127.0*arm_sin_f32(temp);	
		}
		for( i = 0; i < Table_NUM; i++ )   // 2Π 重新映射 Table_NUM份 产生正弦码表
		{
				 float temp=(float)(2*3.141592653)/(float)Table_NUM*(float)i;
				 DDSTable2[i] = 127.0 + 127.0*arm_sin_f32(temp+delta_Phi);	
		}
		
		
}

 
void DDS_init()
{
	hdma_bdma_generator0.XferCpltCallback         = DMA1_stream0_FULL_Handler;
    hdma_bdma_generator0.XferHalfCpltCallback     = DMA1_stream0_HALF_Handler;
	HAL_DMAEx_EnableMuxRequestGenerator (&hdma_bdma_generator0);
    HAL_DMA_Start_IT(&hdma_bdma_generator0, (uint32_t)u16DDS_DataOut, (uint32_t)&GPIOB->ODR, 1024);

	HAL_LPTIM_PWM_Start(&hlptim2, 100-1, 0);		//100M 主频
	u32 Fx=Freq;					// 100k
	PhaseDelta = ((double)Fx / Fs) *65536*65536;	// 放大32bit
	u32 Fy=Freq;					// 100k
	PhaseDelta_y = ((double)Fy / Fs) *65536*65536;	// 放大32bit
//	u32 Fn=50000;				// 100k
//	PhaseDelta_n = ((double)Fn / Fs) *65536*65536;	// 放大32bit
	
}


void DMA1_stream0_HALF_Handler(DMA_HandleTypeDef *hdma)							//半满
{
	//@arg DMA_IT_TC: Transfer complete interrupt mask.
    //@arg DMA_IT_HT: Half transfer complete interrupt mask.
		// 半满
	
		for( int i = 0; i < 512; i++ )
		{
			u16DDS_DataOut[i] = DDSTable[(PhaseAccumulator_y >> 22)&(0x3FF)];
			u16DDS_DataOut[i] = (u16DDS_DataOut[i]<<8) + (u16)(DDSTable2[(PhaseAccumulator >> 22)&(0x3FF)]);
			
			PhaseAccumulator += PhaseDelta;
			PhaseAccumulator_y += PhaseDelta_y;

 		}
//		SCB_CleanInvalidateDCache();
}	

void DMA1_stream0_FULL_Handler(DMA_HandleTypeDef *hdma)
{
			// 全满
 		 
		for( int i = 512; i < 1024; i++ )
		{
			u16DDS_DataOut[i] = DDSTable[(PhaseAccumulator_y >> 22)&(0x3FF)];
			u16DDS_DataOut[i] = (u16DDS_DataOut[i]<<8) + (u16)(DDSTable2[(PhaseAccumulator >> 22)&(0x3FF)]);
			
			PhaseAccumulator += PhaseDelta;
			PhaseAccumulator_y += PhaseDelta_y;
//			u16DDS_DataOut[i] = 0.5*DDSTable[(PhaseAccumulator_y >> 22)&(0x3FF)] + 0.5*DDSTable[(PhaseAccumulator_n >> 22)&(0x3FF)];
//			u16DDS_DataOut[i] = (u16DDS_DataOut[i]<<8) + (u16)( 0.5*DDSTable2[(PhaseAccumulator >> 22)&(0x3FF)] + 0.5*DDSTable[(PhaseAccumulator_n >> 22)&(0x3FF)]);

//			PhaseAccumulator += PhaseDelta;
//			PhaseAccumulator_y += PhaseDelta_y;
//			PhaseAccumulator_n += PhaseDelta_n;
 
		}
//		SCB_CleanInvalidateDCache();

}

