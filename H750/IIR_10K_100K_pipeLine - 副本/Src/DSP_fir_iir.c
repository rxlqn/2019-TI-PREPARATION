
#include "DSP_fir_iir.h"
#include "arm_math.h"

#define numStages  6                /* 2阶IIR滤波的个数 */
static float32_t IIRStateF32[4*numStages];                      /* 状态缓存，大小numTaps + blockSize - 1*/


/* 巴特沃斯低通滤波器系数3000Hz，阻带8000Hz fs = 200000Hz*/    
const float32_t IIRCoeffs32BP[5*numStages] = {
	1.0f,  2.0f,  1.0f,  1.9576933015564808f,   -0.96943717391709916f,       
	1.0f,  2.0f,  1.0f,  1.9019198502274668f,   -0.91332914704808688f,       
	1.0f,  2.0f,  1.0f,  1.8555703149971432f,   -0.86670156876306925f,       
	1.0f,  2.0f,  1.0f,  1.821024845991349f,    -0.83194886730757556f,       
	1.0f,  2.0f,  1.0f,  1.7997697117019655f,   -0.810566227047822f,       
	1.0f,  1.0f,  0.0f,  0.89630084115608444f,  -0.f                   
};



static void arm_iir_f32_Lp(float32_t fInputData[], float32_t fOutputData[], uint32_t u32Len)
{
	static uint8_t u8Flag = 0;
	uint32_t i;
	static arm_biquad_casd_df1_inst_f32 S;
	volatile float32_t f32Tmp;

	if( u8Flag == 0 )
	{
			u8Flag = 1;
			/* 初始化 */
			arm_biquad_cascade_df1_init_f32(&S, numStages, (float32_t *)&IIRCoeffs32BP[0], (float32_t *)&IIRStateF32[0]);
	}
	
	/* IIR滤波 */
 	arm_biquad_cascade_df1_f32(&S, fInputData, fOutputData, u32Len);
	    
}