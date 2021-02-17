
#include "bsp.h"			


uint16_t SPI1_Rx_Buff[4];   //DMA指向内存地址，存放SPI接收数据
uint16_t SPI1_Tx_Buff[4]={0xd000,0xd000,0xd000,0xd000}; //存放SPI发送数据

void SPI2_Init(void)
{         
     GPIO_InitTypeDef  GPIO_InitStructure;
     SPI_InitTypeDef  SPI_InitStructure;

     RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);//使能GPIOB时钟
     RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,ENABLE);//使能SPI1时钟

     //PA5,6,7初始化设置
     GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;//PA5~7复用功能输出        
     GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
     GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
     GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
     GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;//上拉  
     GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化

     GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF_SPI2);//PA5复用为 SPI1
     GPIO_PinAFConfig(GPIOB, GPIO_PinSource14, GPIO_AF_SPI2);//PA6复用为 SPI1
     GPIO_PinAFConfig(GPIOB, GPIO_PinSource15, GPIO_AF_SPI2);//PA7复用为 SPI1

     //这里只针对SPI口初始化
     RCC_APB1PeriphResetCmd(RCC_APB1Periph_SPI2,ENABLE);//复位SPI1
     RCC_APB1PeriphResetCmd(RCC_APB1Periph_SPI2,DISABLE);//停止复位SPI1

     SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex; 
     SPI_InitStructure.SPI_Mode = SPI_Mode_Master;                //为主SPI
     SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;                //SPI发送接收16位帧结构
     SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;                //串行同步时钟的空闲状态为低电平
     SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;        //串行同步时钟的第二个跳变沿（上升或下降）数据被采样
     SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;                //NSS信号由软件管理
     SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;  //波特率预分频值为4(21MHz)
     SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;        //数据从MSB位开始
     SPI_InitStructure.SPI_CRCPolynomial = 7;        //CRC值计算的多项式
     SPI_Init(SPI2, &SPI_InitStructure); 

     SPI_Cmd(SPI2, ENABLE); //使能SPI外设
}


void SPI1_Init(void)
{         
     GPIO_InitTypeDef  GPIO_InitStructure;
     SPI_InitTypeDef  SPI_InitStructure;

     RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);//使能GPIOA时钟
     RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);//使能SPI1时钟

     //PA5,6,7初始化设置
     GPIO_InitStructure.GPIO_Pin =GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;//PA5~7复用功能输出        
     GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
     GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
     GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
     GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;//上拉  
     GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化

     GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_SPI1);//PA5复用为 SPI1
     GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_SPI1);//PA6复用为 SPI1
     GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_SPI1);//PA7复用为 SPI1

     //这里只针对SPI口初始化
     RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1,ENABLE);//复位SPI1
     RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1,DISABLE);//停止复位SPI1

     SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex; 
     SPI_InitStructure.SPI_Mode = SPI_Mode_Master;                //为主SPI
     SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;                //SPI发送接收16位帧结构
     SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;                //串行同步时钟的空闲状态为低电平
     SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;        //串行同步时钟的第二个跳变沿（上升或下降）数据被采样
     SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;                //NSS信号由软件管理
     SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;  //波特率预分频值为4(21MHz)
     SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;        //数据从MSB位开始
     SPI_InitStructure.SPI_CRCPolynomial = 7;        //CRC值计算的多项式
     SPI_Init(SPI1, &SPI_InitStructure); 

     SPI_Cmd(SPI1, ENABLE); //使能SPI外设
}

void DMA1_Config(void)
{
     DMA_InitTypeDef DMA_InitStructure;
     
     RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);

     /* DMA disable*/
     DMA_Cmd(DMA1_Stream5, DISABLE);
     DMA_Cmd(DMA1_Stream6, DISABLE);
     
     DMA_DeInit(DMA1_Stream5);
     DMA_DeInit(DMA1_Stream6);
      
     //   SPI1 RX DMA 配置  Stream2 
     DMA_InitStructure.DMA_Channel = DMA_Channel_3;
     DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&SPI2->DR;         //指定DMA的外设基地址为SPI1的数据地址
     DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)SPI1_Rx_Buff;         //指定DMA的内存基地址
     DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;                 //DMA传输方向为读外设 写到内存
     DMA_InitStructure.DMA_BufferSize = 4;//DataSize;                            //传输数量(0-65535，不能为0)
     DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;        //失能外设地址增长
     DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                 //使能内存地址增长 免去FOR循环
     DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;        //PSIZE=16bit
     DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_HalfWord;        //MSIZE=16bit
     DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;//DMA_Mode_Normal;//       //DMA模式为非循环模式,非循环模式只进行单次传输。
     DMA_InitStructure.DMA_Priority = DMA_Priority_High;                     //优先权为高
     DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;                  //失能FIFO模式
     DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;       //FIFO的阀值为半满
     DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;                    //内存突发传输为单一
     DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;     //外设突发传输为单一
     DMA_Init(DMA1_Stream5, &DMA_InitStructure);                             //初始化DMA2_Stream2
     //DMA_ITConfig(DMA2_Stream2, DMA_IT_TC, ENABLE);//使能传输完成中断 
     
     //   SPI1 TX DMA 配置   Stream3
     DMA_InitStructure.DMA_Channel = DMA_Channel_3;        
     DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&SPI2->DR;         //指定DMA的外设基地址为SPI1的数据地址
     DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)SPI1_Tx_Buff;         //指定DMA的内存基地址
     DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;                 //DMA传输方向为读内存，写外设
     DMA_InitStructure.DMA_BufferSize = 4;//DataSize;                            //传输数量(0-65535，不能为0)
     DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;        //失能外设地址增长
     DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                 //失能内存地址增长
     DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;        //PSIZE=16bit
     DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_HalfWord;        //MSIZE=16bit           
     DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;//DMA_Mode_Normal;//       //DMA模式为非循环模式,非循环模式只进行单次传输。
     DMA_InitStructure.DMA_Priority = DMA_Priority_Medium ;                  //优先权为中等
     DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;                  //失能FIFO模式
     DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;       //FIFO的阀值为半满
     DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;                    //内存突发传输为单一
     DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;     //外设突发传输为单一
     DMA_Init(DMA1_Stream6, &DMA_InitStructure);                             //初始化DMA2_Stream3
     //DMA_ITConfig(DMA2_Stream3, DMA_IT_TC, ENABLE);                        //因为是发送虚拟数据。不需要中断
     
     /* DMA enable*/
     DMA_Cmd(DMA1_Stream5, ENABLE);
     DMA_Cmd(DMA1_Stream6, ENABLE);
}


void DMA2_Config(void)
{
     DMA_InitTypeDef DMA_InitStructure;
     
     RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);

     /* DMA disable*/
     DMA_Cmd(DMA2_Stream2, DISABLE);
     DMA_Cmd(DMA2_Stream3, DISABLE);
     
     DMA_DeInit(DMA2_Stream2);
     DMA_DeInit(DMA2_Stream3);
      
     //   SPI1 RX DMA 配置  Stream2 
     DMA_InitStructure.DMA_Channel = DMA_Channel_6;
     DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&SPI1->DR;         //指定DMA的外设基地址为SPI1的数据地址
     DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)SPI1_Rx_Buff;         //指定DMA的内存基地址
     DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;                 //DMA传输方向为读外设 写到内存
     DMA_InitStructure.DMA_BufferSize = 4;//DataSize;                            //传输数量(0-65535，不能为0)
     DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;        //失能外设地址增长
     DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                 //使能内存地址增长 免去FOR循环
     DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;        //PSIZE=16bit
     DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_HalfWord;        //MSIZE=16bit
     DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;//DMA_Mode_Normal;//       //DMA模式为非循环模式,非循环模式只进行单次传输。
     DMA_InitStructure.DMA_Priority = DMA_Priority_High;                     //优先权为高
     DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;                  //失能FIFO模式
     DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;       //FIFO的阀值为半满
     DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;                    //内存突发传输为单一
     DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;     //外设突发传输为单一
     DMA_Init(DMA2_Stream2, &DMA_InitStructure);                             //初始化DMA2_Stream2
     //DMA_ITConfig(DMA2_Stream2, DMA_IT_TC, ENABLE);//使能传输完成中断 
     
     //   SPI1 TX DMA 配置   Stream3
     DMA_InitStructure.DMA_Channel = DMA_Channel_6;        
     DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&SPI1->DR;         //指定DMA的外设基地址为SPI1的数据地址
     DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)SPI1_Tx_Buff;         //指定DMA的内存基地址
     DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;                 //DMA传输方向为读内存，写外设
     DMA_InitStructure.DMA_BufferSize = 4;//DataSize;                            //传输数量(0-65535，不能为0)
     DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;        //失能外设地址增长
     DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                 //失能内存地址增长
     DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;        //PSIZE=16bit
     DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_HalfWord;        //MSIZE=16bit           
     DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;//DMA_Mode_Normal;//       //DMA模式为非循环模式,非循环模式只进行单次传输。
     DMA_InitStructure.DMA_Priority = DMA_Priority_Medium ;                  //优先权为中等
     DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;                  //失能FIFO模式
     DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;       //FIFO的阀值为半满
     DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;                    //内存突发传输为单一
     DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;     //外设突发传输为单一
     DMA_Init(DMA2_Stream3, &DMA_InitStructure);                             //初始化DMA2_Stream3
     //DMA_ITConfig(DMA2_Stream3, DMA_IT_TC, ENABLE);                        //因为是发送虚拟数据。不需要中断
     
     /* DMA enable*/
     DMA_Cmd(DMA2_Stream2, ENABLE);
     DMA_Cmd(DMA2_Stream3, ENABLE);
}

void TIM2_Init(u16 period)//period设置24以1MHz采样，period设置240以100KHz采样，period设置2400以10KHz采样，period设置24000以1KHz采样，
{
     GPIO_InitTypeDef  GPIO_InitStructure;
     
     TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
     TIM_OCInitTypeDef TIM_OCInitStructure;

     RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//使能GPIOE时钟
     RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
     
     GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0 |GPIO_Pin_1 | GPIO_Pin_2 |GPIO_Pin_3;
     GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF;			//模式必须为复用
     //GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
     GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
     GPIO_InitStructure.GPIO_Speed=GPIO_Speed_100MHz;//频率为快速
     GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;
     GPIO_Init(GPIOA, &GPIO_InitStructure);
     GPIO_PinAFConfig(GPIOA, GPIO_PinSource0, GPIO_AF_TIM2);//PE8 作为 AD的/convst信号
     GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_TIM2);
     GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_TIM2);//PE12 作为 AD的/CS信号
     GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_TIM2);
     
     //初始化
     TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //死区控制用。
     TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //向上计数
     TIM_TimeBaseStructure.TIM_Prescaler = 7 - 1;   //Timer clock = 168M /(TIM_Prescaler+1) = 24M
     TIM_TimeBaseStructure.TIM_RepetitionCounter = 4;
     TIM_TimeBaseStructure.TIM_Period = period - 1;
     TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStructure);
     
     //配置输出比较，产生PWM方波
     TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;//PWM1为正常占空比模式，PWM2为反极性模式
     TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
     TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
     TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
     TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;//输出反相 TIM_OCNPolarity_Low;//输出同相，
     TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
     TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;

     TIM_OCInitStructure.TIM_Pulse = 1;//ccr1;//PE8 作为 AD的/convst信号
     TIM_OC1Init(TIM2,&TIM_OCInitStructure);//触发DMA2_Stream3 channel6
     
     TIM_OCInitStructure.TIM_Pulse = 1;//ccr2;
     TIM_OC2Init(TIM2,&TIM_OCInitStructure);//触发DMA2_Stream2 channel6
     
     TIM_OCInitStructure.TIM_Pulse = 24;//ccr3;//PE12 作为 AD的/CS信号
     TIM_OC3Init(TIM2,&TIM_OCInitStructure);

     TIM_Cmd(TIM2,ENABLE);
     TIM_CtrlPWMOutputs(TIM2,ENABLE);
     
     /* TIM1 DMA 请求使能 */
     TIM_DMACmd(TIM2, TIM_DMA_CC1 | TIM_DMA_CC2, ENABLE);//
}

void TIM1_Init(u16 period)//period设置24以1MHz采样，period设置240以100KHz采样，period设置2400以10KHz采样，period设置24000以1KHz采样，
{
     GPIO_InitTypeDef  GPIO_InitStructure;
     
     TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
     TIM_OCInitTypeDef TIM_OCInitStructure;

     RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);//使能GPIOE时钟
     RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
     
     GPIO_InitStructure.GPIO_Pin=GPIO_Pin_8 |GPIO_Pin_9 | GPIO_Pin_12 |GPIO_Pin_13;
     GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF;//模式必须为复用
     //GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
     GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
     GPIO_InitStructure.GPIO_Speed=GPIO_Speed_100MHz;//频率为快速
     GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;
     GPIO_Init(GPIOE, &GPIO_InitStructure);
     GPIO_PinAFConfig(GPIOE, GPIO_PinSource8, GPIO_AF_TIM1);//PE8 作为 AD的/convst信号
     GPIO_PinAFConfig(GPIOE, GPIO_PinSource9, GPIO_AF_TIM1);
     GPIO_PinAFConfig(GPIOE, GPIO_PinSource12, GPIO_AF_TIM1);//PE12 作为 AD的/CS信号
     GPIO_PinAFConfig(GPIOE, GPIO_PinSource13, GPIO_AF_TIM1);
     
     //初始化
     TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //死区控制用。
     TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //向上计数
     TIM_TimeBaseStructure.TIM_Prescaler = 7 - 1;   //Timer clock = 168M /(TIM_Prescaler+1) = 24M
     TIM_TimeBaseStructure.TIM_RepetitionCounter = 4;
     TIM_TimeBaseStructure.TIM_Period = period - 1;
     TIM_TimeBaseInit(TIM1,&TIM_TimeBaseStructure);
     
     //配置输出比较，产生PWM方波
     TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;//PWM1为正常占空比模式，PWM2为反极性模式
     TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
     TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
     TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
     TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;//输出反相 TIM_OCNPolarity_Low;//输出同相，
     TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
     TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;

     TIM_OCInitStructure.TIM_Pulse = 1;//ccr1;//PE8 作为 AD的/convst信号
     TIM_OC1Init(TIM1,&TIM_OCInitStructure);//触发DMA2_Stream3 channel6
     
     TIM_OCInitStructure.TIM_Pulse = 1;//ccr2;
     TIM_OC2Init(TIM1,&TIM_OCInitStructure);//触发DMA2_Stream2 channel6
     
     TIM_OCInitStructure.TIM_Pulse = 24;//ccr3;//PE12 作为 AD的/CS信号
     TIM_OC3Init(TIM1,&TIM_OCInitStructure);

     TIM_Cmd(TIM1,ENABLE);
     TIM_CtrlPWMOutputs(TIM1,ENABLE);
     
     /* TIM1 DMA 请求使能 */
     TIM_DMACmd(TIM1, TIM_DMA_CC1 | TIM_DMA_CC2, ENABLE);//
}

