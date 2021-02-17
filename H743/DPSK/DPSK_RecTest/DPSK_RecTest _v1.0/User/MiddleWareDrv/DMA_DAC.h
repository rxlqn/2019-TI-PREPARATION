
#include "stm32f4xx.h"

extern __IO uint8_t u8DAC2_Halffull;
extern __IO uint8_t u8DAC1_Halffull;


extern uint16_t aSRC_0[160]; 
extern uint16_t aSRC_Pi[160];
extern uint16_t aSRC_Buffer[4096];
extern uint16_t u16CacheInput[160];


void DAC_Ch2_SineWaveConfig(uint16_t u16DAC_Buffer[], uint32_t u32DMA_Len);

void DAC_Ch1_SineWaveConfig(uint16_t u16DAC_Buffer[], uint32_t u32DMA_Len);
