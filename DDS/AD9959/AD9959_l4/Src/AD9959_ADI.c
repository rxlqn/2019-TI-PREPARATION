#include "AD9959_ADI.h"


//�Ĵ�������
u8 Reg_Len[25] = {1, 3, 2, 3, 4, 2, 3, 2, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4};		//��Ӧ��ͬ�Ĵ���λ����1*8,2*8��3*8������4*8


void delay_us(u16 time)
{
    u16 i = 0;
    while(time--)
    {
        i = 80; //�Լ�����
        while(i--) ;
    }
}

//AD9959IO��ʼ��
void AD9959_IO_init(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_GPIOA_CLK_ENABLE();
//	__HAL_RCC_GPIOG_CLK_ENABLE();
//	__HAL_RCC_GPIOF_CLK_ENABLE();
//	__HAL_RCC_GPIOD_CLK_ENABLE();

    GPIO_Initure.Pin = GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_5 | GPIO_PIN_7 | GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3;
    GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_Initure.Pull = GPIO_PULLUP;
    GPIO_Initure.Speed = GPIO_SPEED_FAST;
    HAL_GPIO_Init(GPIOA, &GPIO_Initure);

    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);

    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_RESET);
}

void AD9959_Start(void)
{
    AD9959_CS_H;//AD9959_CS_H;
    delay_us(20);
    AD9959_CS_L;//AD9959_CS_L;
    delay_us(20);
}

void AD9959_Reset(void)
{
    AD9959_RST_L;//AD9959_RST_L;
    delay_us(20);
    AD9959_RST_H;//AD9959_RST_H;
    delay_us(20);
    AD9959_RST_L;//AD9959_RST_L;
}

/*
**********************************************************************
*ÿ�ζԼĴ�������֮�����ִ�д˶δ�����ܹ������ݴӻ�����д��Ĵ���
**********************************************************************
*/
void AD9959_IO_UpDate(void)
{
    AD9959_UP_L;//AD9959_UP_L;
    delay_us(20);
    AD9959_UP_H;//AD9959_UP_H;
    delay_us(20);
    AD9959_UP_L;//AD9959_UP_L;
    //delay_us(20);
    AD9959_UP_H;//AD9959_UP_H;

}


void AD9959_Init(void)
{
    AD9959_IO_init();
    AD9959_Start();                        //CS�õͣ�ʹ��DDS
    AD9959_Reset();                        //��dds��λ
    AD9959_WRrg(CSR, 0x00);      //����CSR:ͨ��ѡ�񣬴���ͨ��ģʽѡ�񣬲�ѡ��MSBģʽ
    AD9959_WRrg(FR1, 0xD00000);  //����FR1:ϵͳʱ�Ӵ���255M��PLL��20��75uA,
    AD9959_WRrg(FR2, 0x0000);    //����FR2
    //DDS_SetRegister(CFR,0x000300);
    AD9959_WRrg(CFR, 0x000300);  //����CFR,9.455mA����
    AD9959_IO_UpDate();
}


void AD9959_WByte(unsigned char byte)
{
    u8 bit_ctr;
    for(bit_ctr = 0; bit_ctr < 8; bit_ctr++) //output 8-bit
    {
        if(byte & 0x80)
            AD9959_SDIO0_H;//AD9959_SDIO0_H;
        else
            AD9959_SDIO0_L;//AD9959_SDIO0_L;
        byte = (byte << 1);
        AD9959_SCK_H;//AD9959_SCK_H;
        delay_us(3);
        AD9959_SCK_L;//AD9959_SCK_L;
    }
}


void AD9959_WRrg(u8 reg, u32 data)
{
    u8 i, nbyte;
    nbyte = Reg_Len[reg];
    AD9959_SDIO3_L;//AD9959_SDIO3_L;
    AD9959_CS_L;//AD9959_CS_L;
    AD9959_WByte(reg);
    for(i = 0; i < nbyte; i++)
        AD9959_WByte((data >> ((nbyte - 1 - i) * 8)) & 0xFF);
    AD9959_CS_H;//AD9959_CS_H;
    AD9959_SDIO3_H;//AD9959_SDIO3_H;

//		AD9959_IO_UpDate();

}


//����Ƶ�ʣ�32bit���ȵ�Fout��0-(2^32-1),�������Ƶ�ʰ��������ȱ仯
//���Ƶ�ʴ�0��һ�ٶ��׺��������ɵ�
void AD9959_Set_Fre(u32 Fout)
{
    u32 FTW;
    FTW = (u32)(Fout * FRE_REF);
    AD9959_WRrg(0x04, FTW);
    AD9959_IO_UpDate();
}



//������λ��14bit���ȵ�Phase��0-(2^14-1),����4��ͨ���������������λ�������ȱ仯
//��λ�����0�ȵ�180���ڿɵ�
void AD9959_Set_Pha(float Pout)
{
    int POW;
    POW = (int)(Pout * POW_REF);
    AD9959_WRrg(0x05, POW);
    AD9959_IO_UpDate();
}


//���÷��ȣ�10bit���ȵ�amp��0-1023,����������Ȱ��������ȱ仯
//�������Լ��0-10V���ֵ�ɵ�
void AD9959_Set_Amp(u16 amp)
{
    u32 dat;
    dat = 0x00001000 + amp;	 //Bit 12�򿪣�Amplitude multiplier enable
    AD9959_WRrg(0x06, dat);
    AD9959_IO_UpDate();
}

//ͨ��ѡ��0 1 2 3�ֱ��Ӧ��ͬ���ͨ��
void AD9959_Ch(u8 Channel)
{
    switch(Channel)
    {
    case 0:
        AD9959_WRrg(0x00, 0x10);
        break;
    case 1:
        AD9959_WRrg(0x00, 0x20);
        break;
    case 2:
        AD9959_WRrg(0x00, 0x40);
        break;
    case 3:
        AD9959_WRrg(0x00, 0x80);
        break;
    case 4:
        AD9959_WRrg(0x00, 0xF0);
        break;
    default:
        break;
    }
}

//�����������ģʽ�������õ��������������ͨ����Ƶ�ʣ����ȣ��Լ�����ĳ�ʼ��λ
void AD9959_Single_Output(u8 Channel, u32 Fout, float Pout, u16 amp)
{
    AD9959_Ch(Channel);
    AD9959_Set_Fre(Fout);
    AD9959_Set_Pha(Pout);
    AD9959_Set_Amp(amp);
}

/*
***********************************************************************************
*ɨƵ���ú��������������Channel ͨ��ѡ��
*                       FreS    Ƶ�ʿ�ʼֵ            FreE    Ƶ�ʽ���ֵ
*                       FTstep  �½�ɨƵ����ʱ��      RTstep  ����ɨƵ����ʱ��
*                       FFstep  �½�ɨƵ����Ƶ��      RFstep  ����ɨƵ����Ƶ��
*                       DWELL 1 ʹ�� 0 ��ʹ��
***********************************************************************************
*/
void AD9959__Sweep_Fre(u8 Channel, u32 FreS, u32 FreE, float FTstep, float RTstep, u32 FFstep, u32 RFstep, u8 DWELL)
{
    u32 FTW0, CW_1, RDW0, FDW0;
    u16 RSRR0, FSRR0;
    FTW0 = (u32)(FreS * FRE_REF);
    CW_1 = (u32)(FreE * FRE_REF);
    RDW0 = (u32)(RFstep * FRE_REF);
    FDW0 = (u32)(FFstep * FRE_REF);
    RSRR0 = (u16)(RTstep * 0.4);
    FSRR0 = (u16)(FTstep * 0.4);
    AD9959_Ch(Channel);
    AD9959_WRrg(0x06, 0x0003ff);
    if(DWELL)
        //AD9959_WRrg(0x03,0x80C300);    // dwell enable[23:22] 01 amp sweep 10 fre sweep 11 phase sweep
        AD9959_WRrg(0x03, 0x80c314); //auto clear accumulator
    else
        AD9959_WRrg(0x03, 0x804314);
//     AD9959_WRrg(0x03,0x804300);   //����CFR:Ƶ��ɨƵ��ɨƵʹ��.DWELL DISABLED,û���Զ���
    AD9959_WRrg(0x01, 0xD00000);
    AD9959_WRrg(0x04, FTW0);
    AD9959_WRrg(0x0A, CW_1);
    AD9959_WRrg(0x08, RDW0);
    AD9959_WRrg(0x09, FDW0);
    AD9959_WRrg(0x07, FSRR0 * 256 + RSRR0);

    AD9959_IO_UpDate();
}

void AD9959__Sweep_Amp(u8 Channel, u16 ampS, u32 ampE, float FTstep, float RTstep, u32 FFstep, u32 RFstep, u8 DWELL)
{
    u32 RDW0, FDW0;
    u32 SDW, EDW;
    u16 RSRR0, FSRR0;
    SDW = ampS;
    EDW = ampE << 22;
    RDW0 = (u32)RFstep;
    FDW0 = (u32)FFstep;
    RSRR0 = (u16)(RTstep * 0.4);
    FSRR0 = (u16)(FTstep * 0.4);
    AD9959_Ch(Channel);
    AD9959_WRrg(0x06, 0x0003ff);
    if(DWELL)
        //AD9959_WRrg(0x03,0x80C300);    // dwell enable[23:22] 01 amp sweep 10 fre sweep 11 phase sweep
        AD9959_WRrg(0x03, 0x40c314); //auto clear accumulator
    else
        AD9959_WRrg(0x03, 0x404314);
    //AD9959_WRrg(0x03,0x804300);   //����CFR:Ƶ��ɨƵ��ɨƵʹ��.DWELL DISABLED
    AD9959_WRrg(0x01, 0xD00000);
    AD9959_WRrg(0x06, SDW);
    AD9959_WRrg(0x0A, EDW);
    AD9959_WRrg(0x08, RDW0);
    AD9959_WRrg(0x09, FDW0);
    AD9959_WRrg(0x07, FSRR0 * 256 + RSRR0);
}

void AD9959_Sweep_Phase(u8 Channel, u16 phaseS, u32 phaseE, float FTstep, float RTstep, u32 FFstep, u32 RFstep, u8 DWELL)
{
    u32 RDW0, FDW0;
    u32 SDW, EDW;
    u16 RSRR0, FSRR0;
    SDW = phaseS;
    EDW = phaseE << 18;
    RDW0 = (u32)(RFstep * POW_REF);
    FDW0 = (u32)(FFstep * POW_REF);
    RSRR0 = (u16)(RTstep * 0.4);
    FSRR0 = (u16)(FTstep * 0.4);
    AD9959_Ch(Channel);
    AD9959_WRrg(0x06, 0x0003ff);
    if(DWELL)
        //AD9959_WRrg(0x03,0x80C300);    // dwell enable[23:22] 01 amp sweep 10 fre sweep 11 phase sweep
        AD9959_WRrg(0x03, 0xc0c314); //auto clear accumulator
    else
        AD9959_WRrg(0x03, 0xc04314);
    //AD9959_WRrg(0x03,0x804300);   //����CFR:Ƶ��ɨƵ��ɨƵʹ��.DWELL DISABLED
    AD9959_WRrg(0x01, 0xD00000);
    AD9959_WRrg(0x05, SDW);
    AD9959_WRrg(0x0A, EDW);
    AD9959_WRrg(0x08, RDW0);
    AD9959_WRrg(0x09, FDW0);
    AD9959_WRrg(0x07, FSRR0 * 256 + RSRR0);
}

void AD9959__Sweep_Trigger(u8 Channel)
{
    switch(Channel)
    {
    case 0:
        AD9959_P0_L;//AD9959_P0_L;
        delay_us(5);
        AD9959_P0_H;//AD9959_P0_H;
        delay_us(5);
        AD9959_P0_L;//AD9959_P0_L;
        delay_us(5);
        AD9959_P0_H;//AD9959_P0_H;
        delay_us(5);
        break;
    case 1:
        AD9959_P1_L;//AD9959_P0_L;
        HAL_Delay(1000);
        AD9959_P1_H;//AD9959_P0_H;
//	HAL_Delay(1000);
//        AD9959_P1_L;//AD9959_P0_L;
//	HAL_Delay(1000);
//        AD9959_P1_H;//AD9959_P0_H;
//	HAL_Delay(1000);
        break;
    case 2:
        AD9959_P2_L;//AD9959_P0_L;
        delay_us(5);
        AD9959_P2_H;//AD9959_P0_H;
        delay_us(5);
        AD9959_P2_L;//AD9959_P0_L;
        delay_us(5);
        AD9959_P2_H;//AD9959_P0_H;
        delay_us(5);
        break;
    case 3:
        AD9959_P3_L;//AD9959_P0_L;
        delay_us(5);
        AD9959_P3_H;//AD9959_P0_H;
        delay_us(5);
        AD9959_P3_L;//AD9959_P0_L;
        delay_us(5);
        AD9959_P3_H;//AD9959_P0_H;
        delay_us(5);
        break;
    default:
        break;
    }
}

void Fre_Sweep(u8 Channel, u32 S_Fre,  u32 E_Fre)  // px ���� ���ߵ�ƽ�ӵ͵��ߣ��͵�ƽ�Ӹߵ���
{
    u32 FTW0, CW_1, RDW0, FDW0;

    AD9959_Ch(Channel);


    AD9959_WRrg(0x03, 0x804300); //auto clear accumulator

//    AD9959_WRrg(0x01, 0xD00000);

    //CTW0 address 0x04.���CH0�趨Ƶ��
    FTW0 = (u32)(S_Fre * FRE_REF);
    AD9959_WRrg(0x04, FTW0);

    CW_1 = (u32)(E_Fre * FRE_REF);
    AD9959_WRrg(0x0A, CW_1);


    RDW0 = (u32)(E_Fre - S_Fre) * 0.343597383;
    FDW0 = (u32)(E_Fre - S_Fre) * 0.343597383;

    AD9959_WRrg(0x08, 1);				// Ƶ�ʼ��
    AD9959_WRrg(0x09, 1);

    AD9959_WRrg(0x07, 0xFFFF);			// ʱ�䲽��

    AD9959_IO_UpDate();

}

void Fre_Modulation(u8 Channel, u8 Modul_Level,  u32* Fre_Value)
{

    AD9959_Ch(Channel);
    switch(Modul_Level)
    {
    case 2:
        AD9959_WRrg(0x01, 0xD00000 | 0x000000);
        break;
    case 4:
        AD9959_WRrg(0x01, 0xD00000 | 0x000100);
        break;
    case 8:
        AD9959_WRrg(0x01, 0xD00000 | 0x000200);
        break;
    case 16:
        AD9959_WRrg(0x01, 0xD00000 | 0x000300);
        break;
    }
    AD9959_WRrg(0x03, 0x800300);          //��Ƶģʽ
    int i;
    for(i = 0; i < 16; i++)
    {
        if(i == 0)
        {
            AD9959_WRrg(0x04, (u32)(Fre_Value[i]*FRE_REF));
        }
        else
        {
            AD9959_WRrg(0x0A + i - 1, (u32)(Fre_Value[i]*FRE_REF));
        }
    }
    AD9959_IO_UpDate();

}
