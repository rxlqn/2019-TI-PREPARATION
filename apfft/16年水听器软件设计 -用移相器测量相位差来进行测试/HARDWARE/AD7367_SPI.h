#ifndef __AD7367_SPI_H
#define __AD7367_SPI_H

#include "stm32f4xx.h"
#include "delay.h"
#define AD7367_CS(X)			  (X==0)?GPIO_ResetBits(GPIOA,GPIO_Pin_4):GPIO_SetBits(GPIOA,GPIO_Pin_4)
//CS使数据输出脚退出三态
#define AD7367_CNVST(X)			(X==0)?GPIO_ResetBits(GPIOA,GPIO_Pin_3):GPIO_SetBits(GPIOA,GPIO_Pin_3)
//开始转换
#define	  AD7367_SCK_LOW  GPIO_ResetBits(GPIOA,GPIO_Pin_5)
#define	  AD7367_SCK_HIGN GPIO_SetBits(GPIOA,GPIO_Pin_5)
#define   AD7367_CS_LOW   GPIO_ResetBits(GPIOA,GPIO_Pin_4)
#define   AD7367_CS_HIGN  GPIO_SetBits(GPIOA,GPIO_Pin_4)
#define   AD7367_MISO  GPIO_Pin_6
#define   GPIO_SPI   GPIOA

void AD7367_swapByte(void);
void SPI1_Init(void);
void AD7367_Read_Reg(void);
u8 SPI1_Read4byte(void);
#endif


