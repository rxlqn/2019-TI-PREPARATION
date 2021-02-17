
/* Includes ------------------------------------------------------------------*/
#include "main.h"

#define Num_T 40	// 一个周期点数
#define N_phase 40  // 一个bit点数	需要两个周期
#define N_bit 25	// bit数

void SendOneFrameDPSK(uint32_t u32DataIn);
void Phase_init(void);
void SendZero(uint32_t u32DataIn);
extern uint16_t aSRC_0[N_phase];
extern uint16_t aSRC_Pi[N_phase];
extern uint16_t aSRC_Buffer[N_bit*N_phase];
