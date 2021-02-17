#include "AD9959.h"
#include "delay.h"




//寄存器长度
u8 Reg_Len[25]={1,3,2,3,4,2,3,2,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4};		//对应不同寄存器位数，1*8,2*8，3*8，或者4*8




void AD9959GPIOInit(void)
{
	 GPIO_InitTypeDef  GPIO_InitStructure;
	
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB|
												 RCC_AHB1Periph_GPIOE|
												 RCC_AHB1Periph_GPIOF ,ENABLE);
	
	
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_3|GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;//上拉
  GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIO
	
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_6|GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;//上拉
  GPIO_Init(GPIOF, &GPIO_InitStructure);//初始化GPIO
	
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;//上拉
  GPIO_Init(GPIOE, &GPIO_InitStructure);//初始化GPIO

}

void AD9959_Start(void)
{
  AD9959_CS=1;//AD9959_CS_H;
  delay_us(20);
  AD9959_CS=0;//AD9959_CS_L;
  delay_us(20);
} 

void AD9959_Reset(void)
{
	AD9959_RST=0;//AD9959_RST_L;
	delay_us(20);
	AD9959_RST=1;//AD9959_RST_H;
	delay_us(20);
	AD9959_RST=0;//AD9959_RST_L;
}

/*
**********************************************************************
*每次对寄存器操作之后必须执行此段代码才能够将数据从缓冲区写入寄存器
**********************************************************************
*/
void AD9959_IO_UpDate(void)
{
	AD9959_UP=0;//AD9959_UP_L;
  	delay_us(20);
	AD9959_UP=1;//AD9959_UP_H;
	delay_us(20);
	AD9959_UP=0;//AD9959_UP_L;
	//delay_us(20);
	AD9959_UP=1;//AD9959_UP_H;
		
}


void AD9959_Init(void)
{
  AD9959GPIOInit();
  AD9959_Start();                        //CS置低，使能DDS
  AD9959_Reset();                        //给dds复位
  AD9959_WRrg(CSR,0x00);       //配置CSR:通道选择，串行通信模式选择，并选择MSB模式
  AD9959_WRrg(FR1,0xD00000);   //配置FR1:系统时钟大于255M，PLL：20，75uA,
  AD9959_WRrg(FR2,0x0000);     //配置FR2
  //DDS_SetRegister(CFR,0x000300);  
  AD9959_WRrg(CFR,0x000300);   //配置CFR,9.455mA电流
  AD9959_IO_UpDate();
}






void AD9959_WByte(unsigned char byte)   
{
   u8 bit_ctr;   
   for(bit_ctr=0;bit_ctr<8;bit_ctr++)   //output 8-bit
   {
       if(byte&0x80)  
          AD9959_SDIO0=1;//AD9959_SDIO0_H;
       else
          AD9959_SDIO0=0;//AD9959_SDIO0_L; 
       byte=(byte<<1); 
       AD9959_SCK=1;//AD9959_SCK_H;
			 delay_us(3);
       AD9959_SCK=0;//AD9959_SCK_L;
    }   
}


void AD9959_WRrg(u8 reg, u32 data)
{
  u8 i, nbyte;
  nbyte= Reg_Len[reg];
  AD9959_SDIO3=0;//AD9959_SDIO3_L;
  AD9959_CS=0;//AD9959_CS_L;
  AD9959_WByte(reg);
  for(i=0;i<nbyte;i++)
    AD9959_WByte((data>>((nbyte-1-i)*8))&0xFF);
  AD9959_CS=1;//AD9959_CS_H;
  AD9959_SDIO3=1;//AD9959_SDIO3_H;
}


//设置频率，32bit精度的Fout：0-(2^32-1),控制输出频率按步进均匀变化
//输出频率从0到一百多兆赫兹连续可调
void AD9959_Set_Fre(u32 Fout)
{
  u32 FTW;
  FTW = (u32)( Fout * FRE_REF ); 
  AD9959_WRrg(0x04,FTW);
	AD9959_IO_UpDate();
}



//设置相位，14bit精度的Phase：0-(2^14-1),控制4个通道的输出按任意相位步进均匀变化
//相位差可在0度到180度内可调
void AD9959_Set_Pha(float Pout)
{
  int POW;
  POW = (int)( Pout * POW_REF );
  AD9959_WRrg(0x05,POW);
	AD9959_IO_UpDate();
}


//设置幅度，10bit精度的amp：0-1023,控制输出幅度按步进均匀变化
//输出幅度约在0-10V峰峰值可调
void AD9959_Set_Amp(u16 amp)															  
{
  u32 dat;
  dat = 0x00001000+amp;	  //Bit 12打开，Amplitude multiplier enable
  AD9959_WRrg(0x06,dat);
	AD9959_IO_UpDate();
}

//通道选择；0 1 2 3分别对应不同输出通道
void AD9959_Ch(u8 Channel)
{
  switch(Channel)
  {
  case 0:AD9959_WRrg(0x00,0x10);break;
  case 1:AD9959_WRrg(0x00,0x20);break;
  case 2:AD9959_WRrg(0x00,0x40);break;
  case 3:AD9959_WRrg(0x00,0x80);break;
	case 4:AD9959_WRrg(0x00,0xF0);break;
  default:break;
  }
}

//单个波形输出模式，可设置单个波形所输出的通道，频率，幅度，以及输出的初始相位
void AD9959_Single_Output(u8 Channel,u32 Fout,float Pout,u16 amp)
{
	 AD9959_Ch(Channel);
   AD9959_Set_Fre(Fout);
   AD9959_Set_Pha(Pout);
   AD9959_Set_Amp(amp);
}

/*
***********************************************************************************
*扫频设置函数：输入参数：Channel 通道选择
*                       FreS    频率开始值            FreE    频率结束值                       
*                       FTstep  下降扫频步进时间      RTstep  上升扫频步进时间
*                       FFstep  下降扫频步进频率      RFstep  上升扫频步进频率
*                       DWELL 1 使能 0 不使能
***********************************************************************************
*/
void AD9959__Sweep_Fre(u8 Channel,u32 FreS,u32 FreE,float FTstep,float RTstep,u32 FFstep,u32 RFstep,u8 DWELL)
{  
   u32 FTW0,CW_1,RDW0,FDW0;
   u16 RSRR0,FSRR0;
   FTW0 = (u32)( FreS * FRE_REF ); 
   CW_1 = (u32)( FreE * FRE_REF ); 
   RDW0 = (u32)( RFstep * FRE_REF ); 
   FDW0 = (u32)( FFstep * FRE_REF ); 
   RSRR0 = (u16)(RTstep*0.4);
   FSRR0 = (u16)(FTstep*0.4);
   AD9959_Ch(Channel); 
   AD9959_WRrg(0x06,0x0003ff);
   if(DWELL)
     //AD9959_WRrg(0x03,0x80C300);    // dwell enable[23:22] 01 amp sweep 10 fre sweep 11 phase sweep
     AD9959_WRrg(0x03,0x80c314);//auto clear accumulator
   else
   	 AD9959_WRrg(0x03,0x804314);
     //AD9959_WRrg(0x03,0x804300);   //配置CFR:频率扫频，扫频使能.DWELL DISABLED
   AD9959_WRrg(0x01,0xD00000); 
   AD9959_WRrg(0x04,FTW0);
   AD9959_WRrg(0x0A,CW_1);
   AD9959_WRrg(0x08,RDW0);
   AD9959_WRrg(0x09,FDW0);
   AD9959_WRrg(0x07,FSRR0*256+RSRR0);

   AD9959_IO_UpDate();
}

void AD9959__Sweep_Amp(u8 Channel,u16 ampS,u32 ampE,float FTstep,float RTstep,u32 FFstep,u32 RFstep,u8 DWELL)
{
	 u32 RDW0,FDW0;
	 u32 SDW,EDW;
   u16 RSRR0,FSRR0;
   SDW = ampS;
   EDW = ampE<<22;
   RDW0 = (u32)RFstep; 
   FDW0 = (u32)FFstep; 
   RSRR0 = (u16)(RTstep*0.4);
   FSRR0 = (u16)(FTstep*0.4);
   AD9959_Ch(Channel); 
   AD9959_WRrg(0x06,0x0003ff);
   if(DWELL)
     //AD9959_WRrg(0x03,0x80C300);    // dwell enable[23:22] 01 amp sweep 10 fre sweep 11 phase sweep
     AD9959_WRrg(0x03,0x40c314);//auto clear accumulator
   else
   	 AD9959_WRrg(0x03,0x404314);
     //AD9959_WRrg(0x03,0x804300);   //配置CFR:频率扫频，扫频使能.DWELL DISABLED
   AD9959_WRrg(0x01,0xD00000); 
   AD9959_WRrg(0x06,SDW);
   AD9959_WRrg(0x0A,EDW);
   AD9959_WRrg(0x08,RDW0);
   AD9959_WRrg(0x09,FDW0);
   AD9959_WRrg(0x07,FSRR0*256+RSRR0);
}

void AD9959_Sweep_Phase(u8 Channel,u16 phaseS,u32 phaseE,float FTstep,float RTstep,u32 FFstep,u32 RFstep,u8 DWELL)
{
	 u32 RDW0,FDW0;
	 u32 SDW,EDW;
   u16 RSRR0,FSRR0;
   SDW = phaseS;
   EDW = phaseE<<18;
   RDW0 = (u32)(RFstep * POW_REF); 
   FDW0 = (u32)(FFstep * POW_REF); 
   RSRR0 = (u16)(RTstep*0.4);
   FSRR0 = (u16)(FTstep*0.4);
   AD9959_Ch(Channel); 
   AD9959_WRrg(0x06,0x0003ff);
   if(DWELL)
     //AD9959_WRrg(0x03,0x80C300);    // dwell enable[23:22] 01 amp sweep 10 fre sweep 11 phase sweep
     AD9959_WRrg(0x03,0xc0c314);//auto clear accumulator
   else
   	 AD9959_WRrg(0x03,0xc04314);
     //AD9959_WRrg(0x03,0x804300);   //配置CFR:频率扫频，扫频使能.DWELL DISABLED
   AD9959_WRrg(0x01,0xD00000); 
   AD9959_WRrg(0x05,SDW);
   AD9959_WRrg(0x0A,EDW);
   AD9959_WRrg(0x08,RDW0);
   AD9959_WRrg(0x09,FDW0);
   AD9959_WRrg(0x07,FSRR0*256+RSRR0);
}

void AD9959__Sweep_Trigger(u8 Channel)
{
  switch (Channel){
  case 0:
  	AD9959_P0=0;//AD9959_P0_L;
  	delay_us(5);
  	AD9959_P0=1;//AD9959_P0_H;
  	delay_us(5);
  	AD9959_P0=0;//AD9959_P0_L;
  	delay_us(5);
  	AD9959_P0=1;//AD9959_P0_H;
  	delay_us(5);
    break;
  case 1:
  	AD9959_P0=0;//AD9959_P0_L;
  	delay_us(5);
  	AD9959_P0=1;//AD9959_P0_H;
  	delay_us(5);
  	AD9959_P0=0;//AD9959_P0_L;
  	delay_us(5);
  	AD9959_P0=1;//AD9959_P0_H;
  	delay_us(5);
    break;
  case 2:
  	AD9959_P0=0;//AD9959_P0_L;
  	delay_us(5);
  	AD9959_P0=1;//AD9959_P0_H;
  	delay_us(5);
  	AD9959_P0=0;//AD9959_P0_L;
  	delay_us(5);
  	AD9959_P0=1;//AD9959_P0_H;
  	delay_us(5);
    break;
  case 3:
  	AD9959_P0=0;//AD9959_P0_L;
  	delay_us(5);
  	AD9959_P0=1;//AD9959_P0_H;
  	delay_us(5);
  	AD9959_P0=0;//AD9959_P0_L;
  	delay_us(5);
  	AD9959_P0=1;//AD9959_P0_H;
  	delay_us(5);
    break;
   default:break;
  }     
}


