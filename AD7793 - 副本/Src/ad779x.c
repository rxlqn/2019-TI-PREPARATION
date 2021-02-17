/**
  ******************************************************************************
  * @file    ad779x.c
  * @author  Khusainov Timur
  * @version v0.0.1.3
  * @date    20.09.2011
  * @brief   Analog Devices Sigma-Delta ADC AD7792/AD7793 driver (for single device)
  ******************************************************************************
  * @attention
  * <h2><center>&copy; COPYRIGHT 2011 timypik@gmail.com </center></h2>
  ******************************************************************************
  */

#include "ad779x.h"
#include "spi.h"


u32 SPINUM;
u8  FLAG;
float res0;

void AD7793_Init(void)
{					     
 	RCC->APB2ENR|=1<<2;//先使能外设IO PORTA时钟 							 
	GPIOA->CRL&=0XFFFFF000;//PA0,1,2 推挽输出 
	GPIOA->CRL|=0X00000333;	   
	GPIOA->ODR|=7<<0;     //PC 输出高
}

void AD7793Reset(void)
{
	u8 ResetTime;
	ResetTime=50;
	AD7793_SCLK=1;
	AD7793_CS=0;
	AD7793_DIN=1;
	while(ResetTime--)
	{
		AD7793_SCLK=0;
		delay(20);
		AD7793_SCLK=1;
		delay(20);
	}
//	AD7793_CS=1;
}

void AD7793Write(u8 Data)
{
	u8 data,i;
	data=Data;
	AD7793_SCLK=1;
	AD7793_CS=0;
	AD7793_DIN=0;
	for(i=0;i<8;i++)
	{
		AD7793_SCLK=0;
		if(data&0x80)
		AD7793_DIN=1;
		else
		AD7793_DIN=0;
		delay(10);
		AD7793_SCLK=1;
		delay(10);
		data=data<<1;
	}
//	AD7793_CS=1;
}

u32 AD7793Read(u8 SPIBitNumb)
{
	u32 DATA;
	u8 i;
	AD7793_DIN=0;				//又没有关系？
	AD7793_CS=0;
	AD7793_SCLK=1;
	DATA=0;
	for(i=0;i<SPIBitNumb;i++)
	{
		DATA=DATA<<1;
		AD7793_SCLK=0;
		delay(10);
		if(AD7793_DOUT)
		DATA|=0x01;
		AD7793_SCLK=1;
		delay(10);
	}
//	AD7793_CS=1;
	return DATA;
}

void AD7793init(void)
{
	AD7793Reset();
	delay(200);
	delay(200);
	delay(200);
	AD7793Write(0x28);//IO register
	AD7793Write(0x06);//AD7793Write(0x02),这与IEXC1连接IOUT1,IEXC2连接IOUT2有关

	AD7793Write(0x10);//configuration register
	AD7793Write(0x11);//Gain 2
	AD7793Write(0x10);//external Reference
	
	AD7793Write(0x08);//Mode Register
	AD7793Write(0x00);//Continuous Conversion Mode
	AD7793Write(0x4C);//64KHz 19 f ADC(Hz)
	delay_ms(500);
}


