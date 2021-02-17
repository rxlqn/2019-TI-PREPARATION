#include "stm32f10x.h"
#include "stm32f10x_spi.h"
#include "stm32f10x_gpio.h"
#include "AD9959.h"
#include "delay.h"

void AD9959_RCC_Init(void);
void AD9959_GPIO_Init(void);
void AD9959_500M_Init(void);

//ch1 配置
 Chrip_Config Ch1_ChripConfig={
	1,					//开
	0,					//连续
	1000000,		//1M
	5000000,		//5M
	1,					//1ms
	10,					//10ms
	10.00,			//10.0v
	0.0,				//0度
};


//ch2 配置
 Chrip_Config Ch2_ChripConfig={
	0,					//关
	0,					//连续
	1000000,		//1M
	5000000,		//5M
	1,					//1ms
	10,					//10ms
	10.00,			//10.0v
	0.0,				//0度
};

//ch2 配置
 Chrip_Config Ch3_ChripConfig={
	0,					//关
	0,					//连续
	1000000,		//1M
	5000000,		//5M
	1,					//1ms
	10,					//10ms
	10.00,			//10.0v
	0.0,				//0度
};

//ch4 配置
 Chrip_Config Ch4_ChripConfig={
	0,					//关
	0,					//连续
	1000000,		//1M
	5000000,		//5M
	1,					//1ms
	10,					//10ms
	10.00,			//10.0v
	0.0,				//0度
};


//寄存器地址
/********************************************************************************************************************/
#define CSR  	0x00 							 //CSR 通道选择寄存器，包括通道选择，串行 3 线 通信模式，数据传输首先高低位设置
																 //default Value = 0xF0 详细请参见AD9958 datasheet Table 27	
															 	 //Bit7: CH1 enable,Bit6: CH0 enable,
/*********single-bit 2-wire mode***********/
u8 CSR_DATA0[1] = {0x10}; 			 // 开 CH0, 
u8 CSR_DATA1[1] = {0x20};				 // 开 CH1,
u8 CSR_DATA2[1] = {0x40};			 		// 开 CH2,
u8 CSR_DATA3[1] = {0x80}; 				 // 开 CH3,
/******************************************/
/*********single-bit 3-wire mode***********/
//u8 CSR_DATA0[1] = {0x12}; 			 // 开 CH0, 
//u8 CSR_DATA1[1] = {0x22};				 // 开 CH1,
//u8 CSR_DATA2[1] = {0x42};			 	 // 开 CH2,
//u8 CSR_DATA3[1] = {0x82}; 			 // 开 CH3,
/******************************************/

#define FR1 	0x01							 //Function Register 1								功能寄存器1
u8 FR1data[3] = {0xD3,0x00,0x00};//default Value = 0x000000;   20倍频;  Charge pump control = 75uA
                                 //FR1<23> -- VCO gain control =0时 system clock below 160 MHz; 
                                 //             =1时, the high range (system clock above 255 MHz

#define FR2  	0x02							 //Function Register 2								功能寄存器2
u8 FR2data[2] = {0x00,0x00};		 //default Value = 0x0000


#define CFR  	0x03 							 //Channel Function Register 				通道功能寄存器
//u8 CFRdata[3] = {0x00,0x23,0x37};//default Value = 0x000300

//u8 CFRdata[3] = {0x80,0x43,0x00};		//Linear Sweep enable,frequency modulation, DAC Full Scale, no_dwell disable
//u8 CFRdata[3] = {0x80,0xC3,0x00};		//Linear Sweep enable,frequency modulation, DAC Full Scale, no_dwell enable

#define CFTW0 0x04							 //Channel Frequency Tuning Word 0		通道频率控制字0
//25MHZ 无源晶振20倍频
//u8 ChannelFrequencyTuningWord0data[4] = {0x00,0x0d,0x1B,0x71}; //OUT 100KHZ 
//u8 ChannelFrequencyTuningWord0data[4] = {0x00,0x83,0x12,0x6F}; //OUT 1MHZ		 										
//u8 ChannelFrequencyTuningWord0data[4] = {0x05,0x1E,0xB8,0x52};   //OUT 10MHZ	 
//u8 ChannelFrequencyTuningWord0data[4] = {0x0a,0x3d,0x70,0xa4};   //OUT 20MHZ	  
//u8 ChannelFrequencyTuningWord0data[4] = {0x28,0xF5,0xC2,0x8F}; //OUT 80MHZ  
//u8 CFTW0data0[4] = {0x33,0x33,0x33,0x33}; //OUT 100MHZ

//u8 ChannelFrequencyTuningWord0data1[4] = {0x00,0x0d,0x1B,0x71}; //OUT 100KHZ 
//u8 ChannelFrequencyTuningWord0data1[4] = {0x00,0x83,0x12,0x6F}; //OUT 1MHZ		 										
//u8 ChannelFrequencyTuningWord0data1[4] = {0x05,0x1E,0xB8,0x52};   //OUT 10MHZ	 
//u8 ChannelFrequencyTuningWord0data1[4] = {0x28,0xF5,0xC2,0x8F}; //OUT 80MHZ  
//u8 CFTWodata1[4] = {0x33,0x33,0x33,0x33}; //OUT 100MHZ

#define CPOW0 0x05			//Channel Phase Offset Word 0				通道相位控制字0
u8 CPOW0data[2] = {0x00,0x00};//default Value = 0x0000   @ = POW/2^14*360


#define ACR   0x06			//Amplitude Control Register				幅度控制寄存器
u8 ACRdata[3] = {0x00,0x10,0x00};//default Value = 0x--0000 Rest = 18.91/Iout 


#define LSRR 	0x07			//Linear Sweep Ramp Rate						线性扫描频率寄存器
u8 LSRRdata[2] = {0x00,0x00};//default Value = 0x----


#define RDW		0x08			//LSR Rising Delta Word							三角形上升沿寄存器
u8 RDWdata[4] = {0x00,0x00,0x00,0x00};//default Value = 0x--------

#define FDW		0x09			//LSR Falling Delta Word						三角形下降沿寄存器
u8 FDWdata[4] = {0x00,0x00,0x00,0x00};//default Value = 0x--------

#define CW1 0x0A			//Frequency tuning word[31:0] or phase word[31:18] or amplitude word[31:22]

/***********************************************************************************************************************/

/***********************************************函数定义****************************************************************/

void AD9959_Reset(void)
{
	RESET_1();
	delay_us(1);
	RESET_0();
}

void IO9959_Init(void)
{
	SCLK_1();
	PWR_0();
	RESET_0();
	CS_Disable();
	
	PS0_Set_Low();
	PS1_Set_Low();
	PS2_Set_Low();
	PS3_Set_Low();
}


//读寄存器 single-bit 3-wire mode有效
void ReadFromeAD9959(u8 addr,u8 num,u8 *RegisterData)
{
	u8 i;
	signed char RegisterIndex = 0;
	u8 ReceiveData = 0;
	
	SPI_Write_Byte(addr | 0x80);			//读寄存器的时候，地址最高位要置1
	
	for(RegisterIndex = num; RegisterIndex >0 ;RegisterIndex--)
	{
		for(i = 0; i < 8; i++ )
		{
			SCLK_0();
			ReceiveData <<= 1;
			if( GPIO_ReadInputDataBit(AD9959_GPIO_SDIO2,AD9959_SDIO2) )
			{
				ReceiveData  |= 1 ;
			}
			delay_us(1);
			SCLK_1();
			delay_us(1);
		}
		*(RegisterData + RegisterIndex -1) = ReceiveData;
	}
}


//向AD9959写入一个一个字节，MSB
void SPI_Write_Byte(u8 data)
{
	u8 i =0;
	for(i = 0; i < 8; i++)
	{
		SCLK_0();
		delay_us(1);
		if( (data&0x80) == 0x80)
			SDIO0_1();
		else
			SDIO0_0();
		data = data << 1;
		delay_us(1);
		SCLK_1();
		delay_us(1);
	}
}

//	AD9959写寄存器函数
//	command要写入指令的指针
//	num指令有几个字节
//	addr寄存器地址
void AD9959_Write_Reg(u8 addr,u8 num,u8 *command)
{
	u8 i = 0;
	//寄存器
	SPI_Write_Byte(addr);
	
	
	//写指令	
	for(i = 0; i< num; i++)
	{		
		SPI_Write_Byte(command[i]);		//注意command[0]是所要发送指令的最高的8位
		delay_us(1);
	}
}


//更新寄存器
void AD9959_Updata_First(void)
{
 IO9959_Init();
 CS_Enable();
 SDIO3_1();
 delay_us(40);
 SDIO3_0();
 IO_UPDATE_0();
}
//
void AD9959_Updata_Final(void)
{
 CS_Disable();
 delay_us(20);
 IO_UPDATE_1(); 
}

//
void AD9959_Update(void)
{

 IO_UPDATE_0();
 delay_us(1);
 IO_UPDATE_1(); 
}



void AD9959_SingleTone_Init(void)
{
	AD9959_Updata_First();
//	AD9959_Write_Reg(FR2,2,FR2data);
	AD9959_Write_Reg(FR1,3,FR1data);			//20倍频，AD9959工作在500M
	AD9959_Updata_Final();
}

//SingleTone模式设定某一通道的频率
//Channel 要改变通道
//Freq	  要写入的频率
void AD9959_SingleTone_Freq(u8 Channel,u32 Freq)
{
	
	u8 Ch_temp[1];														//通道控制变量
	u32 Temp;     														//频率转换中间变量  
	u8 CFTW0data[4] ={0x00,0x00,0x00,0x00};		//中间变量	
	Temp=(u32)Freq*8.589934592;	  							 		//将输入频率因子分为四个字节  4.294967296=(2^32)/500000000
	CFTW0data[3]=(u8)Temp;
	CFTW0data[2]=(u8)(Temp>>8);
	CFTW0data[1]=(u8)(Temp>>16);
	CFTW0data[0]=(u8)(Temp>>24);
	
	//通道选择
	switch(Channel)
	{
		 case 0:Ch_temp[0] = CSR_DATA0[0];break; //控制寄存器写入Channel通道
		 case 1:Ch_temp[0] = CSR_DATA1[0];break;
		 case 2:Ch_temp[0] = CSR_DATA2[0];break;
		 case 3:Ch_temp[0] = CSR_DATA3[0];break;
	}
	
	
//	AD9959_Updata_First();
//	IO_UPDATE_0();
	AD9959_Write_Reg(CSR,1,Ch_temp);						//开通道
	AD9959_Write_Reg(CFTW0,4,CFTW0data);				//写入频率
//	IO_UPDATE_1();delay_us(1);
//	IO_UPDATE_0();
//	AD9959_Updata_Final();

}

//==============更新幅度====================================
//Channel 通道
//Amplitude 幅度控制字  <1023
void AD9959_SingleTone_Amp(u8 Channel,u16 Amplitude)
{ 
	u8 Ch_temp[1];																				//通道控制变量
	u16 A_temp;  																					//幅度控制中间变量
	A_temp = Amplitude | 0x1000;													//ACR[12] = 1  amplitude multiplier enabled
	ACRdata[2]=(u8)A_temp;  															//低位数据
	ACRdata[1]=(u8)(A_temp>>8); 													//高位数据
	
	//通道选择
	switch(Channel)
	{
		 case 0:Ch_temp[0] = CSR_DATA0[0];break; //控制寄存器写入Channel通道
		 case 1:Ch_temp[0] = CSR_DATA1[0];break;
		 case 2:Ch_temp[0] = CSR_DATA2[0];break;
		 case 3:Ch_temp[0] = CSR_DATA3[0];break;
	}
	
	AD9959_Updata_First();
	AD9959_Write_Reg(CSR,1,Ch_temp);						//开通道
	AD9959_Write_Reg(ACR,3,ACRdata);						//写入幅度
	AD9959_Updata_Final();	
}

//相位
void AD9959_SingleTone_Phase(u8 Channel,u16 Phase)
{
	u8 Ch_temp[1];																				//通道控制变量
	u16 P_temp=0;																					
	P_temp=(u16)Phase*45.511111;													//将输入相位差写入，进度1度，45.511111=2^14）/360
	CPOW0data[1]=(u8)P_temp;															//低位数据
	CPOW0data[0]=(u8)(P_temp>>8);													//高位数据
	
	//通道选择
	switch(Channel)
	{
		 case 0:Ch_temp[0] = CSR_DATA0[0];break; //控制寄存器写入Channel通道
		 case 1:Ch_temp[0] = CSR_DATA1[0];break;
		 case 2:Ch_temp[0] = CSR_DATA2[0];break;
		 case 3:Ch_temp[0] = CSR_DATA3[0];break;
	}
	
//	AD9959_Updata_First();
	AD9959_Write_Reg(CSR,1,Ch_temp);						//开通道
	AD9959_Write_Reg(CPOW0,2,CPOW0data);				//写入相位
//	AD9959_Updata_Final();	
}	 


void AD9959_Phase(u16 Phase)
{
	u16 P_temp=0;																					
	P_temp=(u16)Phase*45.511111;													//将输入相位差写入，进度1度，45.511111=2^14）/360
	CPOW0data[1]=(u8)P_temp;															//低位数据
	CPOW0data[0]=(u8)(P_temp>>8);													//高位数据
	
	AD9959_Write_Reg(CPOW0,2,CPOW0data);				//写入相位
	
}
	



void AD9959_Freq(u32 Freq)
{
	u32 Temp;     														//频率转换中间变量  
	u8 CFTW0data[4] ={0x00,0x00,0x00,0x00};		//中间变量	
	Temp=(u32)Freq*8.589934592;	  							 		//将输入频率因子分为四个字节  4.294967296=(2^32)/500000000
	CFTW0data[3]=(u8)Temp;
	CFTW0data[2]=(u8)(Temp>>8);
	CFTW0data[1]=(u8)(Temp>>16);
	CFTW0data[0]=(u8)(Temp>>24);
	
	AD9959_Write_Reg(CFTW0,4,CFTW0data);				//写入频率

}







//四通道同频
void AD9959_Set_Freq(u32 Freq)
{
	u32 Temp;     														//频率转换中间变量  
	u8 CFTW0data[4] ={0x00,0x00,0x00,0x00};		//中间变量	
	Temp=(u32)Freq*8.59;	  							 		//将输入频率因子分为四个字节  4.294967296=(2^32)/500000000
	CFTW0data[3]=(u8)Temp;
	CFTW0data[2]=(u8)(Temp>>8);
	CFTW0data[1]=(u8)(Temp>>16);
	CFTW0data[0]=(u8)(Temp>>24);
	
	IO_UPDATE_0();
	SPI_Write_Byte(CFTW0);
	SPI_Write_Byte(CFTW0data[0]);
	SPI_Write_Byte(CFTW0data[1]);
	SPI_Write_Byte(CFTW0data[2]);
	SPI_Write_Byte(CFTW0data[3]);
	
	IO_UPDATE_1();delay_us(10);
	IO_UPDATE_0();
}

//设置4个通道的频率
void AD9959_SetFrequency4Channel(u32 Freq0,u32 Freq1,u32 Freq2,u32 Freq3)
{
	AD9959_Updata_First();
	AD9959_SingleTone_Freq(0,Freq0);
	AD9959_SingleTone_Freq(1,Freq1);
	AD9959_SingleTone_Freq(2,Freq2);
	AD9959_SingleTone_Freq(3,Freq3);			//singletone模式向通道3写入频率控制字
	AD9959_Updata_Final();	
}


void AD9959_LinearSweep(u32 Start_Freq,u32 End_Freq,u32 Delta_Up_Freq,u32 Delta_Down_Freq,u8 Up_Ramp,u8 Down_Ramp)
{
//		u8 Ch_temp[1];														//通道控制变量
	u32 Temp = 0;     																//频率转换中间变量  
	u8 Temp_Start_Freq[4] ={0x00,0x00,0x00,0x00};	//起始频率中间变量  要写入CFTW0 0x04	
	u8 Temp_End_Freq[4] ={0x00,0x00,0x00,0x00};		//终止频率中间变量  要写入CW1   0x0A
	u8 Temp_Delta_Up_Freq[4] ={0x00,0x00,0x00,0x00};	
	u8 Temp_Delta_Down_Freq[4] ={0x00,0x00,0x00,0x00};	
//	u8 LSRRData[2] = {0x00,0x00};									
	
	//Start_Freq		CFTW0
	Temp=(u32)Start_Freq*8.589934592;	  		//将输入频率因子分为四个字节  4.294967296=(2^32)/500000000
	Temp_Start_Freq[3]=(u8)Temp;
	Temp_Start_Freq[2]=(u8)(Temp>>8);
	Temp_Start_Freq[1]=(u8)(Temp>>16);
	Temp_Start_Freq[0]=(u8)(Temp>>24);
	
	//End_Freq
	Temp = 0;
	Temp=(u32)End_Freq*8.589934592;	  		//将输入频率因子分为四个字节  4.294967296=(2^32)/500000000
	Temp_End_Freq[3]=(u8)Temp;
	Temp_End_Freq[2]=(u8)(Temp>>8);
	Temp_End_Freq[1]=(u8)(Temp>>16);
	Temp_End_Freq[0]=(u8)(Temp>>24);
	
	//Delta_Up_Freq
	Temp = 0;
	Temp=(u32)Delta_Up_Freq*8.589934592;	  		//将输入频率因子分为四个字节  4.294967296=(2^32)/500000000
	Temp_Delta_Up_Freq[3]=(u8)Temp;
	Temp_Delta_Up_Freq[2]=(u8)(Temp>>8);
	Temp_Delta_Up_Freq[1]=(u8)(Temp>>16);
	Temp_Delta_Up_Freq[0]=(u8)(Temp>>24);
	
	//Delta_Down_Freq
	Temp = 0;
	Temp=(u32)Delta_Down_Freq*8.589934592;	  		//将输入频率因子分为四个字节  4.294967296=(2^32)/500000000
	Temp_Delta_Down_Freq[3]=(u8)Temp;
	Temp_Delta_Down_Freq[2]=(u8)(Temp>>8);
	Temp_Delta_Down_Freq[1]=(u8)(Temp>>16);
	Temp_Delta_Down_Freq[0]=(u8)(Temp>>24);


	//LSRR 0x07
	LSRRdata[0] = Down_Ramp;			//高8位
	LSRRdata[1] = Up_Ramp;				//低8位

	AD9959_Updata_First();
	
	AD9959_Write_Reg(CFTW0,4,Temp_Start_Freq);				//start
	AD9959_Write_Reg(CW1,4,Temp_End_Freq);						//end
	AD9959_Write_Reg(RDW,4,Temp_Delta_Up_Freq);				//Delta_Up_Freq
	AD9959_Write_Reg(FDW,4,Temp_Delta_Down_Freq);			//Delta_Down_Freq
	AD9959_Write_Reg(LSRR,2,LSRRdata);								//Up_Ramp and Down_Ramp
//	AD9959_Write_Reg(CFR,3,CFRdata);									//Linear Sweep 设置
	AD9959_Updata_Final();

}

void AD9959_Chrip_StartFreq(u32 StartFreq)
{
	u32 Temp = 0;     																//频率转换中间变量  
	u8 Temp_Start_Freq[4] ={0x00,0x00,0x00,0x00};	//起始频率中间变量  要写入CFTW0 0x04	
	
//	IO_UPDATE_0();
	//Start_Freq		CFTW0
	Temp=(u32)StartFreq*8.589934592;	  		//将输入频率因子分为四个字节  4.294967296=(2^32)/500000000
	Temp_Start_Freq[3]=(u8)Temp;
	Temp_Start_Freq[2]=(u8)(Temp>>8);
	Temp_Start_Freq[1]=(u8)(Temp>>16);
	Temp_Start_Freq[0]=(u8)(Temp>>24);
	
	SPI_Write_Byte(CFTW0);
	SPI_Write_Byte(Temp_Start_Freq[0]);
	SPI_Write_Byte(Temp_Start_Freq[1]);
	SPI_Write_Byte(Temp_Start_Freq[2]);
	SPI_Write_Byte(Temp_Start_Freq[3]);
//	IO_UPDATE_1();delay_us(1);
//	IO_UPDATE_0();
}

void AD9959_Chrip_EndFreq(u32 EndFreq)
{
	
	u32 Temp = 0;  
	u8 Temp_End_Freq[4] ={0x00,0x00,0x00,0x00};		//终止频率中间变量  要写入CW1   0x0A

	
//	IO_UPDATE_0();
	Temp=(u32)EndFreq*8.589934592;	  		//将输入频率因子分为四个字节  4.294967296=(2^32)/500000000
	Temp_End_Freq[3]=(u8)Temp;
	Temp_End_Freq[2]=(u8)(Temp>>8);
	Temp_End_Freq[1]=(u8)(Temp>>16);
	Temp_End_Freq[0]=(u8)(Temp>>24);

	SPI_Write_Byte(0x0A);
	SPI_Write_Byte(Temp_End_Freq[0]);
	SPI_Write_Byte(Temp_End_Freq[1]);
	SPI_Write_Byte(Temp_End_Freq[2]);
	SPI_Write_Byte(Temp_End_Freq[3]);
//	IO_UPDATE_1();delay_us(1);
//	IO_UPDATE_0();
}
	
void AD9959_Chrip_Delta_Up_Freq(u32 Delta_Up_Freq)
{
	u32 Temp = 0; 
	u8 Temp_Delta_Up_Freq[4] ={0x00,0x00,0x00,0x00};	

//	IO_UPDATE_0();
	Temp=(u32)Delta_Up_Freq*8.589934592;	  		//将输入频率因子分为四个字节  4.294967296=(2^32)/500000000
	Temp_Delta_Up_Freq[3]=(u8)Temp;
	Temp_Delta_Up_Freq[2]=(u8)(Temp>>8);
	Temp_Delta_Up_Freq[1]=(u8)(Temp>>16);
	Temp_Delta_Up_Freq[0]=(u8)(Temp>>24);
	
	SPI_Write_Byte(RDW);
	SPI_Write_Byte(Temp_Delta_Up_Freq[0]);
	SPI_Write_Byte(Temp_Delta_Up_Freq[1]);
	SPI_Write_Byte(Temp_Delta_Up_Freq[2]);
	SPI_Write_Byte(Temp_Delta_Up_Freq[3]);
//	IO_UPDATE_1();delay_us(1);
//	IO_UPDATE_0();
}


void AD9959_Chrip_Delta_Down_Freq(u32 Delta_Down_Freq)
{
	u32 Temp = 0; 
	u8 Temp_Delta_Down_Freq[4] ={0x00,0x00,0x00,0x00};	

	IO_UPDATE_0();
	Temp=(u32)Delta_Down_Freq*8.589934592;	  		//将输入频率因子分为四个字节  4.294967296=(2^32)/500000000
	Temp_Delta_Down_Freq[3]=(u8)Temp;
	Temp_Delta_Down_Freq[2]=(u8)(Temp>>8);
	Temp_Delta_Down_Freq[1]=(u8)(Temp>>16);
	Temp_Delta_Down_Freq[0]=(u8)(Temp>>24);
	
	SPI_Write_Byte(FDW);
	SPI_Write_Byte(Temp_Delta_Down_Freq[0]);
	SPI_Write_Byte(Temp_Delta_Down_Freq[1]);
	SPI_Write_Byte(Temp_Delta_Down_Freq[2]);
	SPI_Write_Byte(Temp_Delta_Down_Freq[3]);
	IO_UPDATE_1();delay_us(1);
	IO_UPDATE_0();
}


void AD9959_Chrip_Delta_Ramp(u8 Up_Ramp,u8 Down_Ramp)
{
	//LSRR 0x07
//	IO_UPDATE_0();
	LSRRdata[0] = Down_Ramp;			//高8位
	LSRRdata[1] = Up_Ramp;				//低8位
	
	SPI_Write_Byte(LSRR);
	SPI_Write_Byte(LSRRdata[0]);
	SPI_Write_Byte(LSRRdata[1]);
//	IO_UPDATE_1();delay_us(1);
//	IO_UPDATE_0();

}

void AD9959_Init(void)
{
	AD9959_RCC_Init();
  AD9959_GPIO_Init();
	AD9959_500M_Init();
}
void AD9959_RCC_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD,ENABLE);
}

void AD9959_GPIO_Init(void)
{
GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin = AD9959_P0;										//PB0		P0
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;
	GPIO_Init(AD9959_GPIO_P0,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = AD9959_P1;										//PC6		P1
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;
	GPIO_Init(AD9959_GPIO_P1,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = AD9959_P2;									//PB13   P2
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;
	GPIO_Init(AD9959_GPIO_P2,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = AD9959_P3;									  //PA0			P3
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;
	GPIO_Init(AD9959_GPIO_P3,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = AD9959_SCLK;									//PB10		SCLK
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;
	GPIO_Init(AD9959_GPIO_SCLK,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = AD9959_SDIO0;									//PB11		SDIO_0
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;
	GPIO_Init(AD9959_GPIO_SDIO0,&GPIO_InitStructure);
	
	
	GPIO_InitStructure.GPIO_Pin = AD9959_SDIO1;									//PD12		SDIO_1
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;
	GPIO_Init(AD9959_GPIO_SDIO1,&GPIO_InitStructure);	
	
	GPIO_InitStructure.GPIO_Pin = AD9959_SDIO2;										//PA8			SDIO_2			三线模式
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU ;
	GPIO_Init(AD9959_GPIO_SDIO2,&GPIO_InitStructure);	
	
	GPIO_InitStructure.GPIO_Pin = AD9959_SDIO3;									  //PE0			SDIO_3
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;
	GPIO_Init(AD9959_GPIO_SDIO3,&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = AD9959_RESET;									//PC12		RESET
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;
	GPIO_Init(AD9959_GPIO_RESET,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = AD9959_PWR;										//PD2			PWR
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;
	GPIO_Init(AD9959_GPIO_PWR,&GPIO_InitStructure);	
	
	
	GPIO_InitStructure.GPIO_Pin = AD9959_CS;										//PE2			CS
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;
	GPIO_Init(AD9959_GPIO_CS,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = AD9959_UPDATE;										//PE3			IO_UP
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;
	GPIO_Init(AD9959_GPIO_UPDATE,&GPIO_InitStructure);
}


//ad9959 500M工作时钟初始化
void AD9959_500M_Init(void)
{
	//AD9959系统时钟设为500M
	IO_UPDATE_0();
	SPI_Write_Byte(0x01);delay_us(1);
	SPI_Write_Byte(0xD3);
	SPI_Write_Byte(0x00);
	SPI_Write_Byte(0x00);
	IO_UPDATE_1();delay_us(10);
	IO_UPDATE_0();
	delay_us(30);

}










