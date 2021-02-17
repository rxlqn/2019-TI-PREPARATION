
#include "arm_iir_example_f32.h"
#include "arm_math.h"
#include "math_helper.h"
#include "main.h"
#define numStages  5                /* 2阶IIR滤波的个数 */
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



/* 巴特沃斯低通滤波器系数2000Hz，阻带6000Hz fs = 48000Hz*/
const float32_t IIRCoeffs32BP[5 * numStages] =
{
    1.0f,  2.0f,  1.0f,  1.8211724224354168f,   -0.90371400211417840f,
    1.0f,  2.0f,  1.0f,  1.6700661074475109f,   -0.74575904732430f,
    1.0f,  2.0f,  1.0f,  1.5642604786872837f,   -0.63515795624036441f,
    1.0f,  2.0f,  1.0f,  1.5021445649125366f,   -0.57022674305574538f,
    1.0f,  1.0f,  0.0f,  0.7408551891780725f,   -0.0f,
};


static arm_biquad_casd_df1_inst_f32 S;

static void arm_iir_f32_Lp(float32_t fInputData[], float32_t fOutputData[], uint32_t u32Len)
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

}

void IIR(void)
{
    uint32_t i;
    arm_status status;
    float32_t*  inputF32, *outputF32;
    float32_t ScaleValue;
    /* Initialize input and output buffer pointers */
    inputF32 = &testInput_f32_1kHz_15kHz[0];
    outputF32 = &testOutput[0];

    arm_iir_f32_Lp(inputF32, outputF32, TEST_LENGTH_SAMPLES);

    ScaleValue = 0.02063539491969f * 0.018923234969f * 0.0177243693882f * 0.0170205445358f * 0.1295724054109f;
    for(int i = 0; i < TEST_LENGTH_SAMPLES; i++)
    {
        outputF32[i] = outputF32[i] * ScaleValue;
//		printf("%f ",outputF32[i]);
    }


}