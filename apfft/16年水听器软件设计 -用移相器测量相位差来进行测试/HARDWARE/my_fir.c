#include "my_fir.h"
#define TEST_LENGTH_SAMPLES 1024
#define BLOCK_SIZE           32  //FIR一次处理的点数。这个跟系统的系数有关系么
#define NUM_TAPS             21

u32 blockSize = BLOCK_SIZE;
u32 numBlocks = TEST_LENGTH_SAMPLES / BLOCK_SIZE;

//汉宁窗，20阶FIR，群延时10个采样点，这个系数其实就是单位冲击响应序列
const float fir_coefficient[NUM_TAPS] =
{
    0.003265932435, 0.001156421145, 0.000564396265,  0.01225693803,  0.02870825306,
    -3.065940308e-017, -0.09563499689,  -0.1614501774, -0.06700689346,   0.1469002515,
    0.2618361712,   0.1469002515, -0.06700689346,  -0.1614501774, -0.09563499689,
    -3.065940308e-017,  0.02870825306,  0.01225693803, 0.000564396265, 0.001156421145,
    0.003265932435
};
float inputbuf[TEST_LENGTH_SAMPLES];	//FFT输入数组
float Outputbuf[TEST_LENGTH_SAMPLES];	//FFT输入数组
static void hanning_fir()//汉宁窗FIR滤波器
{
    u32 i;
    arm_fir_instance_f32 S;
    float* inputF32, *outputF32; //输入输出指针
    for(i = 0; i < TEST_LENGTH_SAMPLES; i++) //生成信号序列
    {
        inputbuf[i] = 100 +
                      10 * arm_sin_f32(2 * PI * i * 30 / TEST_LENGTH_SAMPLES) +
                      30 * arm_sin_f32(2 * PI * i * 4 / TEST_LENGTH_SAMPLES) +
                      50 * arm_cos_f32(2 * PI * i * 8 / TEST_LENGTH_SAMPLES);	//生成输入信号实部
    }

    inputF32 = &inputbuf[0]; //初始化输入
    outputF32 = &Outputbuf[0];
    //	for(i=0;i<TEST_LENGTH_SAMPLES;i++)
    //   arm_fir_init_f32(&S,NUM_TAPS,(float*)&firCoeffs32);
}