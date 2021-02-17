#include "ad9910.h"

/******************************

PA5-----------SCLK
PA7-----------SDIO
PA8-----------MASTER_RESET
PF6-----------CS(IO_RESET)
PG10----------IO_UPDATE
PD4/5/6-------PROFILE0/1/2

******************************/
u8 tempdata[100];
u8 dataflag = 0;
u8 valueflag = 0;
u8 value;

void AD9910_IO_init(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();
    __HAL_RCC_GPIOF_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();

    GPIO_Initure.Pin = GPIO_PIN_8;
    GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_Initure.Pull = GPIO_PULLUP;
    GPIO_Initure.Speed = GPIO_SPEED_FAST;
    HAL_GPIO_Init(GPIOA, &GPIO_Initure);

    GPIO_Initure.Pin = GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6;
    HAL_GPIO_Init(GPIOD, &GPIO_Initure);

    GPIO_Initure.Pin = GPIO_PIN_6;
    HAL_GPIO_Init(GPIOF, &GPIO_Initure);

    GPIO_Initure.Pin = GPIO_PIN_10;
    HAL_GPIO_Init(GPIOG, &GPIO_Initure);

//	GPIO_Initure.Pin=GPIO_PIN_9;
//	GPIO_Initure.Mode=GPIO_MODE_INPUT;
//	HAL_GPIO_Init(GPIOF,&GPIO_Initure);

    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_5, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_6, GPIO_PIN_RESET);

}

void ad9910_CR_init(void)
{
    AD9910_CS_H();
    AD9910_IO_UPDATE_L();
    AD9910_RESET_H();
    HAL_Delay(100);
    AD9910_RESET_L();
    HAL_Delay(100);

    tempdata[0] = 0x03;
    tempdata[1] = 0x1D;
    tempdata[1] = 0x3F;
    tempdata[1] = 0x41;
    tempdata[1] = 0x32;
    AD9910_CS_L();
    HAL_Delay(5);
    SPI1_Start_Transmit(tempdata, 5);
    HAL_Delay(50);
    AD9910_CS_H();
    HAL_Delay(50);





    tempdata[0] = 0x0E;
    tempdata[1] = 0x08;
    tempdata[2] = 0xB5;
    tempdata[3] = 0x00;
    tempdata[4] = 0x00;
    tempdata[5] = 0x02;
    tempdata[6] = 0x8F;
    tempdata[7] = 0x5C;
    tempdata[8] = 0x29;

    AD9910_CS_L();
    HAL_Delay(5);
    SPI1_Start_Transmit(tempdata, 9);
    HAL_Delay(50);
    AD9910_CS_H();
    HAL_Delay(50);


//	tempdata[0]=0x02;
//	tempdata[1]=0x20;
//	tempdata[2]=0x00;
//	AD9910_CS_L();
//	HAL_Delay(5);
//	SPI1_Start_Transmit(tempdata,3);
//	HAL_Delay(100);
//	AD9910_CS_H();
//	HAL_Delay(50);
//
//
//  tempdata[0]=0x03;
//	tempdata[1]=0x00;
//	tempdata[2]=0x03;
//	tempdata[3]=0x04;
//	AD9910_CS_L();
//	HAL_Delay(5);
//	SPI1_Start_Transmit(tempdata,4);
//	HAL_Delay(50);
//	AD9910_CS_H();
//	HAL_Delay(50);


}


void change_dds_freq_amp_phase(u8 ch, u32 freq, u16 amp, u8 phase)
{
    u32 tempfre;
    u16 A_temp;//=0x23ff;
    u16 P_temp = 0;
    tempdata[0] = 0x00;
    switch(ch)
    {
    case 0:
        tempdata[1] = 0x12;
        break;
    case 1:
        tempdata[1] = 0x22;
        break;
    case 2:
        tempdata[1] = 0x42;
        break;
    case 3:
        tempdata[1] = 0x82;
        break;
    }
    tempdata[2] = 0x04;
    tempfre = (u32)freq * 8.5899;
    tempdata[6] = (u8)(tempfre & 0xff);
    tempdata[5] = (u8)(tempfre >> 8 & 0xff);
    tempdata[4] = (u8)(tempfre >> 16 & 0xff);
    tempdata[3] = (u8)(tempfre >> 24 & 0xff);

    tempdata[7] = 0x06;
    A_temp = amp | 0x1000;
    tempdata[10] = (u8)(A_temp & 0x00ff); //低位数据
    tempdata[9] = (u8)(A_temp >> 8); //高位数据
    tempdata[8] = 0;



    tempdata[11] = 0x05;
    P_temp = (u16)phase * 45.511111; //将输入相位差写入，进度1度，45.511111=2^14）/360
    tempdata[13] = (u8)P_temp;
    tempdata[12] = (u8)(P_temp >> 8);

    dataflag = 1;
//	AD9958_CS_L();
////	HAL_Delay(5);
//	SPI1_Start_Transmit(tempdata,14);
////	HAL_Delay(50);
//	AD9958_CS_H();
//	//HAL_Delay(50);
//	IO_Update();

}



void ad9910_test(void)
{

    tempdata[0] = 0x01;
    tempdata[1] = 0xD0;
    tempdata[2] = 0x00;
    tempdata[3] = 0x00;
    AD9910_CS_L();
    HAL_Delay(5);
    SPI1_Start_Transmit(tempdata, 4);
    HAL_Delay(50);
    AD9910_CS_H();
    HAL_Delay(50);


    tempdata[0] = 0x00;
    tempdata[1] = 0x22;
    AD9910_CS_L();
    HAL_Delay(5);
    SPI1_Start_Transmit(tempdata, 2);
    HAL_Delay(50);
    AD9910_CS_H();
    HAL_Delay(50);


    tempdata[0] = 0x02;
    tempdata[1] = 0x20;
    tempdata[2] = 0x00;
    AD9910_CS_L();
    HAL_Delay(5);
    SPI1_Start_Transmit(tempdata, 3);
    HAL_Delay(100);
    AD9910_CS_H();
    HAL_Delay(50);


    tempdata[0] = 0x03;
    tempdata[1] = 0x00;
    tempdata[2] = 0x03;
    tempdata[3] = 0x04;
    AD9910_CS_L();
    HAL_Delay(5);
    SPI1_Start_Transmit(tempdata, 4);
    HAL_Delay(50);
    AD9910_CS_H();
    HAL_Delay(50);

    tempdata[0] = 0x04;
    tempdata[1] = 0x02;
    tempdata[2] = 0x0C;
    tempdata[3] = 0x49;
    tempdata[4] = 0xBA;
    AD9910_CS_L();
    //HAL_Delay(5);
    SPI1_Start_Transmit(tempdata, 5);
    //HAL_Delay(50);
    AD9910_CS_H();
    //HAL_Delay(50);
    IO_Update();

}




void IO_Update(void)
{
    AD9910_IO_UPDATE_L();
    HAL_Delay(10);
    AD9910_IO_UPDATE_H();
    HAL_Delay(10);
    AD9910_IO_UPDATE_L();
}



void ad9910_state_machine(void)
{
    static u8 state = 0;
    switch(state)
    {
    case 0:
        if(dataflag)
        {
            AD9910_CS_L();
            state = 2;
        }
        break;
    case 2:
        SPI1_Start_Transmit(tempdata, 14);
        state = 3;
        break;
    case 3:
        AD9910_IO_UPDATE_L();
        state = 4;
        break;
    case 4:
        AD9910_IO_UPDATE_H();
        state = 5;
//		  if(valueflag==1)
//			{
//		    DAC1_Set_Vol(value*StepV/2);
//				valueflag=0;
//		  }
        break;
    case 5:
        AD9910_IO_UPDATE_L();
        AD9910_CS_H();
        state = 0;
        dataflag = 0;
        break;

    }
}
