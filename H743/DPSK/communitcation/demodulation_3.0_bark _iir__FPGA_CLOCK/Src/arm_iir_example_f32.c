
#include "arm_iir_example_f32.h"
#include "arm_math.h"
#include "math_helper.h"
#include "main.h"
#define numStages  4               							    /* 2阶IIR滤波的个数 */ //每个需要5个系数，4个状态变量
static float32_t IIRStateF32[4 * numStages];                    /* 状态缓存，大小numTaps + blockSize - 1*/

/* ----------------------------------------------------------------------
** Macro Defines
** ------------------------------------------------------------------- */

#define TEST_LENGTH_SAMPLES  320
#define SNR_THRESHOLD_F32    140.0f


/* -------------------------------------------------------------------
 * The input signal and reference output (computed with MATLAB)
 * are defined externally in arm_fir_lpf_data.c.
 * ------------------------------------------------------------------- */

extern float32_t testInput_f32_1kHz_15kHz[TEST_LENGTH_SAMPLES];
extern float32_t refOutput[TEST_LENGTH_SAMPLES];

/* -------------------------------------------------------------------
 * Declare Test output buffer
 * ------------------------------------------------------------------- */

static float32_t testOutput[TEST_LENGTH_SAMPLES];
float32_t  snr;



/* 巴特沃斯低通滤波器系数10KHz，阻带100KHz fs = 1MHz*/
const float32_t IIRCoeffs32BP[5 * numStages] =
{
    1.0f,  2.0f,  1.0f,  1.75003840778074f,   -0.87092600990064f,
	1.0f,  2.0f,  1.0f,  1.5635829854908f,   -0.6715907852003664f,
	1.0f,  2.0f,  1.0f,  1.4456939392925f,   -0.5455583039506745f,
	1.0f,  2.0f,  1.0f,  1.389015953208f,   -0.4849651661760f,
//    1.0f,  1.0f,  0.0f,  0.7533862577174909f,  -0.0f,
};


static arm_biquad_casd_df1_inst_f32 S;

 void arm_iir_f32_Lp(float fInputData[], float fOutputData[], uint32_t u32Len)
{
    static uint8_t u8Flag = 0;
    uint32_t i;
    volatile float32_t f32Tmp;

    if(u8Flag == 0)
    {
        u8Flag = 1;
        /* 初始化 */
        arm_biquad_cascade_df1_init_f32(&S, numStages, (float32_t*)&IIRCoeffs32BP[0], (float32_t*)&IIRStateF32[0]);
    }

    /* IIR滤波 */
    arm_biquad_cascade_df1_f32(&S, fInputData, fOutputData, u32Len);
	
	// 乘缩放系数
    float32_t ScaleValue = 0.03022190052997f * 0.02700194992736f*0.02496609116453f*0.023987303241903;
    for(int i = 0; i < u32Len; i++)
    {
        fOutputData[i] = fOutputData[i] * ScaleValue;
    }
}

//void IIR(void)
//{
//    uint32_t i;
//    arm_status status;
//    float32_t*  inputF32, *outputF32;
//    float32_t ScaleValue;
//    /* Initialize input and output buffer pointers */
//    inputF32 = &testInput_f32_1kHz_15kHz[0];
//    outputF32 = &testOutput[0];

//    arm_iir_f32_Lp(inputF32, outputF32, TEST_LENGTH_SAMPLES);

//    ScaleValue = 0.02063539491969f * 0.018923234969f * 0.0177243693882f * 0.0170205445358f * 0.1295724054109f;
//    for(int i = 0; i < TEST_LENGTH_SAMPLES; i++)
//    {
//        outputF32[i] = outputF32[i] * ScaleValue;
////		printf("%f ",outputF32[i]);
//    }
//}