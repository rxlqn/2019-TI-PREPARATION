#include "dpsk_mod.h"
#include "arm_math.h"


//#define N 10		// 一个周期点数
//#define N_phase 20  // 一个bit点数
//#define N_bit 25	// bit数

uint16_t aSRC_0[N_phase] = {0};
uint16_t aSRC_Pi[N_phase] = {0};
uint16_t aSRC_Buffer[N_bit*N_phase] = {0};
uint16_t aSRC_Buffer_0[N_bit*N_phase] = {0};
extern u8 flag;
//void Phase_init()	// 16个周期的正弦
//{
//	// Phase 0
//	for(int i = 0; i < 160; i++)
//	{
//		aSRC_0[i] = 2000*(arm_sin_f32(i*2*(float)3.1415926545/(float)10.) + (float)1.); 
//	}
//	// Phase Pi
//	for(int i = 0; i < 160; i++)
//	{
//		aSRC_Pi[i] = 2000*(-arm_sin_f32(i*2*(float)3.1415926545/(float)10.) + (float)1.); 
//	}
//}

void Phase_init()	// 一个bit的周期数 N_phase/N 
{
	// Phase 0
	for(int i = 0; i < N_phase; i++)
	{
		aSRC_0[i] = 32767*0.8*(arm_sin_f32(i*2*(float)3.1415926545/(float)Num_T)) + (float)1*32767.; 
	}
	// Phase Pi
	for(int i = 0; i < N_phase; i++)
	{
		aSRC_Pi[i] = 32767*0.8*(-arm_sin_f32(i*2*(float)3.1415926545/(float)Num_T)) + (float)1*32767.; 
	}
}

void SendZero(uint32_t u32DataIn)
{
	int i,j,index = (1 << (N_bit - 1));	// 共24bit + 1bit
	
	int ForePhase = 0;			// 初始相位置0
	for( j = 0; j < N_phase; j++)	
	{
		aSRC_Buffer_0[j] = aSRC_0[j]; 
	}
	
//	int barker = 0x72; // 1110010

//	u32DataIn = u32DataIn + (barker<<17);
	
	for( i = 1; i < N_bit; i++)	// 24bit信息位
	{
		
			index = index >> 1;
		
			if(ForePhase == 0)			   // 前一个相位为0
			{
					if( u32DataIn & index) // 数据为1
					{
							for(j = 0; j < N_phase; j++)
							{
									aSRC_Buffer_0[N_phase*i + j] = aSRC_Pi[j];
							}
							ForePhase = 180;
					}
					else					// 数据为0
					{
							for(j = 0; j < N_phase; j++)
							{
									aSRC_Buffer_0[N_phase*i + j] = aSRC_0[j];
							}
							ForePhase = 0;
					}
			}
			else		// ForePhase = 180
			{
					if( u32DataIn & index)
					{
							for(j = 0; j < N_phase; j++)
							{
									aSRC_Buffer_0[N_phase*i + j] = aSRC_0[j];
							}
							ForePhase = 0;
					}
					else
					{
							for(j = 0; j < N_phase; j++)
							{
									aSRC_Buffer_0[N_phase*i + j] = aSRC_Pi[j];
							}
							ForePhase = 180;
					}
			}
		
	}
	
}


void SendOneFrameDPSK(uint32_t u32DataIn)
{
	int i,j,index = (1 << (N_bit - 1));	// 共24bit + 1bit
 
	int ForePhase = 0;			// 初始相位置0
	for( j = 0; j < N_phase; j++)	
	{
		aSRC_Buffer[j] = aSRC_0[j]; 
	}
	
	int barker = 0xff; // 1110010

	u32DataIn = u32DataIn + (barker<<16);
	
	for( i = 1; i < N_bit; i++)	// 24bit信息位
	{
		
			index = index >> 1;
		
			if(ForePhase == 0)			   // 前一个相位为0
			{
					if( u32DataIn & index) // 数据为1
					{
							for(j = 0; j < N_phase; j++)
							{
									aSRC_Buffer[N_phase*i + j] = aSRC_Pi[j];
							}
							ForePhase = 180;
					}
					else					// 数据为0
					{
							for(j = 0; j < N_phase; j++)
							{
									aSRC_Buffer[N_phase*i + j] = aSRC_0[j];
							}
							ForePhase = 0;
					}
			}
			else		// ForePhase = 180
			{
					if( u32DataIn & index)
					{
							for(j = 0; j < N_phase; j++)
							{
									aSRC_Buffer[N_phase*i + j] = aSRC_0[j];
							}
							ForePhase = 0;
					}
					else
					{
							for(j = 0; j < N_phase; j++)
							{
									aSRC_Buffer[N_phase*i + j] = aSRC_Pi[j];
							}
							ForePhase = 180;
					}
			}
		
	}
		flag = 1;

}