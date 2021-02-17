
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "bdma.h"
#include "lptim.h"
#define Table_NUM 1024
//extern __attribute__((at(0x38000000))) __IO uint16_t u16DDS_DataOut[Table_NUM];
extern	  u16	DDSTable[Table_NUM];
extern __IO uint16_t* u16DDS_DataOut;
extern u8 DAC_HALF ;
extern u8 DAC_FULL ;

void DMA1_stream0_HALF_Handler(DMA_HandleTypeDef *hdma);
void DMA1_stream0_FULL_Handler(DMA_HandleTypeDef *hdma);

void DDS_init(void);
void InitSinTable(void);
