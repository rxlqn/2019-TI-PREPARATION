
#include "arm_iir_example_f32.h"
#include "arm_math.h"
#include "math_helper.h"
#include "main.h"
#define numStages  4               							    /* 2��IIR�˲��ĸ��� */ //ÿ����Ҫ5��ϵ����4��״̬����
static float32_t IIRStateF32[4 * numStages];                    /* ״̬���棬��СnumTaps + blockSize - 1*/

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



/* ������˹��ͨ�˲���ϵ��10KHz�����100KHz fs = 1MHz*/
const float32_t IIRCoeffs32BP[5 * numStages] =
{
    1.0f,  2.0f,  1.0f,  1.964759689529636f,   -0.98910627798700f,
	1.0f,  2.0f,  1.0f,  1.951664080524511124f,   -0.9692359605741261f,
	1.0f,  2.0f,  1.0f,  1.94596749161428039798f,   -0.95420179468536270f,
	1.0f,  2.0f,  1.0f,  1.944417731521690395f,   -0.9461108700530194f,
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
        /* ��ʼ�� */
        arm_biquad_cascade_df1_init_f32(&S, numStages, (float32_t*)&IIRCoeffs32BP[0], (float32_t*)&IIRStateF32[0]);
    }

    /* IIR�˲� */
    arm_biquad_cascade_df1_f32(&S, fInputData, fOutputData, u32Len);
	
	// ������ϵ��
    float32_t ScaleValue = 0.006086647114f * 0.004392970012f*0.002058575767f*0.000423284632f*0.89125093813;
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