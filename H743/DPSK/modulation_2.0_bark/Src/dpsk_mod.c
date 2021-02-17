#include "dpsk_mod.h"
#include "arm_math.h"


//#define N 10		// 一个周期点数
//#define N_phase 20  // 一个bit点数
//#define N_bit 25	// bit数

uint16_t aSRC_0[N_phase] = {0};
uint16_t aSRC_Pi[N_phase] = {0};
uint16_t aSRC_Buffer[N_bit*N_phase] = {0};


float barker_7[7] = {1,1,1,-1,-1,1,-1};
float test[25] = {1,1,1,0,0,1,0,1,0,1,0,1,0,1,0};
float resul[32];
float max_val;
u32 max_index;
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
	
//	 arm_correlate_f32((float32_t *)barker_7,7,(float32_t *)barker_7,7,resul);	
	 arm_correlate_f32(barker_7,7,test,25,resul);	
	 arm_max_f32	(resul,50,&max_val,&max_index );		
}

void SendOneFrameDPSK(uint32_t u32DataIn)
{
	int i,j,index = (1 << (N_bit));	// 共18bit + 7bit
	int barker = 0x72; // 1110010
	
	int ForePhase = 0;			// 初始相位

	if((u32DataIn & 1) == 1)// 取尾bit,判断初始相位
		ForePhase = 180;
	else
		ForePhase = 0;
	
	u32DataIn = u32DataIn + (barker<<18);
	
	for( i = 0; i < N_bit; i++)	// DPSK 调制
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
//	for(int i = 0;i<N_bit*N_phase;i++)
//	{
//		printf("%d ",aSRC_Buffer[i]);
//	}
}