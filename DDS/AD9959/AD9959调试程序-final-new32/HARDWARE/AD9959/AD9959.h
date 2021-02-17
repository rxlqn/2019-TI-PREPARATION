#ifndef __AD9959_H
#define __AD9959_H	 
#include "sys.h"




//应用参考值
#define FRE_REF    8.5904963602764   //频率参考
#define POW_REF    45.51111111   //相位补偿参考

//晶振频率:25M
#define Crystal  25000000



#define AD9959_SDIO0    PEout(5)
#define AD9959_SDIO1    PEout(6)
#define AD9959_SDIO2    PEout(3)
#define AD9959_SDIO3    PEout(4)

#define AD9959_P0       PFout(7)
#define AD9959_P1       PFout(6)
#define AD9959_P2       PBout(7)
#define AD9959_P3       PBout(6)

#define AD9959_UP       PBout(5)
#define AD9959_CS       PBout(4)
#define AD9959_SCK      PBout(3)
#define AD9959_RST      PEout(2)


#define CSR  0x00    //通道选择寄存器
#define FR1  0x01    //功能控制寄存器1
#define FR2  0x02    //功能控制寄存器2
#define CFR  0x03    //通道功能寄存器
#define CFTW0 0x04   //通道频率控制字0
#define CPOW0 0x05   //通道相位补偿字0
#define ACR   0x06   //幅度控制寄存器
#define LSRR  0x07   //线性扫描率，[7:0]Rise,[15:8]fall
#define RDW   0x08   //线性扫描步进字rising
#define FDW   0x09   //线性扫描步进字falling
#define CW1   0x0a   //通道字1
#define CW2   0x0b
#define CW3   0x0c
#define CW4   0x0d
#define CW5   0x0e
#define CW6   0x0f
#define CW7   0x10
#define CW8   0x11
#define CW9   0x12
#define CW10  0x13
#define CW11  0x14
#define CW12  0x15
#define CW13  0x16
#define CW14  0x17
#define CW15  0x18




void AD9959_Start(void);
void AD9959_Reset(void);
void AD9959GPIOInit(void);
void AD9959_IO_UpDate(void);
void AD9959_Init(void);
void AD9959_WByte(unsigned char byte);
void AD9959_Set_Fre(u32 Fout);
void AD9959_Set_Pha(float Pout);
void AD9959_WRrg(u8 reg, u32 data);
void AD9959__Sweep_Fre(u8 Channel,u32 FreS,u32 FreE,float FTstep,float RTstep,u32 FFstep,u32 RFstep,u8 DWELL);
void AD9959__Sweep_Amp(u8 Channel,u16 ampS,u32 ampE,float FTstep,float RTstep,u32 FFstep,u32 RFstep,u8 DWELL);
void AD9959_Sweep_Phase(u8 Channel,u16 phaseS,u32 phaseE,float FTstep,float RTstep,u32 FFstep,u32 RFstep,u8 DWELL);
void AD9959__Sweep_Trigger(u8 Channel);
void AD9959_Single_Output(u8 Channel,u32 Fout,float Pout,u16 amp);
void AD9959_Set_Amp(u16 amp);	
void AD9959_Ch(u8 Channel);

#endif

















