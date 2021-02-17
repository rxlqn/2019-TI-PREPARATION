
/** \example arm_signal_converge_example_f32.c
  */

#include "arm_math.h"
#include "math_helper.h"
#include "arm_signal_converge_example.h"
#include "main.h"
/* ----------------------------------------------------------------------
** Global defines for the simulation
* ------------------------------------------------------------------- */

#define TEST_LENGTH_SAMPLES  512
#define NUMTAPS               32
#define BLOCKSIZE             32

#define MU                  0.5f
#define NUMFRAMES (TEST_LENGTH_SAMPLES / BLOCKSIZE)

/* ----------------------------------------------------------------------
* Declare LMSNorm state buffers and structure
* ------------------------------------------------------------------- */
float32_t lmsStateF32[NUMTAPS + BLOCKSIZE];
//float32_t errOutput[TEST_LENGTH_SAMPLES];
arm_lms_norm_instance_f32 lmsNorm_instance;
/* ----------------------------------------------------------------------
* Function Declarations for Signal Convergence Example
* ------------------------------------------------------------------- */
arm_status test_signal_converge_example(void);
/* ----------------------------------------------------------------------
* Internal functions
* ------------------------------------------------------------------- */
arm_status test_signal_converge(float32_t* err_signal,
                                uint32_t blockSize);
void getinput(float32_t* input,
              uint32_t fr_cnt,
              uint32_t blockSize);
/* ----------------------------------------------------------------------
* External Declarations for FIR F32 module Test
* ------------------------------------------------------------------- */

float32_t lmsNormCoeff_f32[32];
arm_lms_norm_instance_f32 lmsNorm_instance;
/* ----------------------------------------------------------------------
* Declare I/O buffers
* ------------------------------------------------------------------- */
float32_t wire1[BLOCKSIZE];
float32_t wire2[BLOCKSIZE];
float32_t wire3[BLOCKSIZE];
float32_t err_signal[BLOCKSIZE];
/* ----------------------------------------------------------------------
* Signal converge test
* ------------------------------------------------------------------- */

void LMS(float* input, float* noise, float* errOutput)
{
    uint32_t i;
    arm_status status;
    uint32_t index;

    static uint8_t u8Flag = 0;

    if(u8Flag == 0)
    {
        u8Flag = 1;
        /* Initialize the LMSNorm data structure */
        arm_lms_norm_init_f32(&lmsNorm_instance, NUMTAPS, lmsNormCoeff_f32, lmsStateF32, MU, BLOCKSIZE);
    }
    for(i = 0; i < NUMFRAMES; i++)
    {
        /* Read the input data - uniformly distributed random noise - into wire1 */
        arm_copy_f32(input + (i * BLOCKSIZE), wire1, BLOCKSIZE);
        arm_copy_f32(noise + (i * BLOCKSIZE), wire2, BLOCKSIZE);

        /* Execute the LMS Norm processing function*/
        arm_lms_norm_f32(&lmsNorm_instance, /* LMSNorm instance */
                         wire2,                         /* Input signal */		// 输入位噪声源
                         wire1,                         /* Reference Signal */	// 参考信号为含有噪声的信号源
                         wire3,                         /* Converged Signal */
                         err_signal,                    /* Error Signal, this will become small as the signal converges */
                         BLOCKSIZE);                    /* BlockSize */
        /* apply overall gain */
//        arm_scale_f32(wire3, 5, wire3, BLOCKSIZE);   /* in-place buffer */

        for(int j = 0; j < BLOCKSIZE; j++)
        {
//		  output[i*BLOCKSIZE+j] = wire3[j];
//		  ref[i*BLOCKSIZE+j] = wire1[j];
            errOutput[i * BLOCKSIZE + j] = err_signal[j];
        }

    }

}
