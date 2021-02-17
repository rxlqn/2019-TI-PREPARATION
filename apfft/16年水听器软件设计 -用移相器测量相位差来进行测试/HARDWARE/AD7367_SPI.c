#include "AD7367_SPI.h"
u16 ADC_date[4];//4个字节DMA传输，采样频率用中断来控制

//转换时间最多1.25μs,给转换触发信号到开始转换最小为40ns，时间留2μs
void SPI1_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
  SPI_InitTypeDef  SPI_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//使能GPIOA时钟
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);//使能SPI1时钟
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_3; //AD7367转换信号和片选信号
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);	//初始化指定IO
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;                                             //复用功能
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                                           //推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;                                       //100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;                                             //上拉
  GPIO_Init(GPIOA, &GPIO_InitStructure);    
	
	AD7367_CNVST(1);
	AD7367_CS(1);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_SPI1);//SCK
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_SPI1);//MISO

	SPI_Cmd(SPI1,DISABLE);  
	
	SPI_I2S_DeInit(SPI1);
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1,ENABLE);//复位SPI1
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1,DISABLE);//停止复位SPI1
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_RxOnly ;  //设置成只接收模式，输出SCK
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		//SPI主模式下才会发出脉冲
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		//发送接收8位帧结构
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;		//时钟悬空高
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;	//数据捕获于第1个时钟沿即下降沿
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		//NSS信号由软件控制
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;		//定义波特率预分频的值:波特率预分频值为16（72M/8=9M）
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	//数据传输从MSB位开始
	
	SPI_InitStructure.SPI_CRCPolynomial = 7;	//CRC值计算的多项式，这个不清楚，有可能出问题
	
	SPI_Init(SPI1, &SPI_InitStructure);  //根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器
	SPI_Cmd(SPI1,DISABLE);//使能SPI1

}

/*
normal mode读取AD的值
 */ 
int sign1;
int sign2;
u32 date;//直接存储四个字节的数据
void AD7367_Read_Reg()
{
	u8 i=1;
	uint8_t reg_val;
	AD7367_SCK_HIGN;//空闲为高
	AD7367_CNVST(0);//相当于给触发信号，下降沿给触发信号,采样保持开始
  while(i--);//延时大约80ns，脉冲信号必须足够短，保证处于正常模式
	AD7367_CNVST(1);//
  delay_us(2);//转换时间最多是1.25μs,测试大概为0.5μs
	AD7367_CS(0);//立刻退出三态，立刻就输出MSB
  SPI1_Read4byte();//
	AD7367_CS(1); //关闭片选，读取完四个字节，恢复高阻态。
	
	//将读回的数据进行处理
	date=ADC_date[0]<<24|ADC_date[1]<<16|ADC_date[2]<<8|ADC_date[3];
	date=date>>4;
	sign1=date&0x1fff;//第一位是符号位，1为负零为正，是补码表示的
	if(date&0x2000)//说明是补码
		  sign1=-1*((~sign1)+1);

	sign2=(date>>14)&0x1fff;
	if((date>>14)&0x2000)
		   sign2=-1*((~sign2)+1);
	date=0;
	//转换完有个 t quiet，最小是50ns
}

/*
  用于读取4个字节的数据从AD7367中
 */ 
u8 SPI1_Read4byte()
{	
	u8 i;
	u32 retry=0;
	SPI_Cmd(SPI1,ENABLE);//使能SPI1
   for(i=0;i<4;i++)
  {	
			delay_us(1);//转换时间最多是1.25μs,测试大概为0.5μs
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET) //检查指定的SPI标志位设置与否:接受缓存非空标志位
		{
		  retry++;
			if(retry>200000)return 0;
		}
		ADC_date[i]=SPI1->DR;//读的太慢应该会数据会被覆盖，每次读取RXNE都会被清零，继续连续转换
	}			
  SPI_Cmd(SPI1,DISABLE);//使能SPI1	
}
/*********************模拟SPI**************************/


void AD7367_swapByte()
{  
    u8 i = 0;
	 
    for(i=0;i<28;i++)
    {   
			  delay_us(1);
			  AD7367_SCK_HIGN;//对于软件SPI应该在高电平期间读数据
        if(GPIO_ReadInputDataBit(GPIO_SPI,AD7367_MISO))
        {
            date|=1;//先传来的是MSB，MSB是符号位
					  delay_us(1);
        }
		    	date = date<<1;
           AD7367_SCK_LOW;//下降沿过后只有7ns，硬件读取应该是在这个时候
    }
}