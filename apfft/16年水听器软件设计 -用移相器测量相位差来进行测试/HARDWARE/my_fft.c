#include "my_fft.h"

#define FFT_LENGTH		1024 		//FFT长度，默认是1024点FFT
//解析度上不去考虑增加点数
float sin1_fft_inputbuf1 [FFT_LENGTH * 4];	//FFT输入数组是实数
float sin1_fft_outputbuf[FFT_LENGTH];	//FFT输出数组

float sin2_fft_inputbuf1 [FFT_LENGTH * 4];	//FFT输入数组是实数
float sin2_fft_outputbuf[FFT_LENGTH];	//FFT输出数组

float phase1;//保存信号一的相位信息
float phase2;//保存信号二的相位信息
float fre1;
float fre2;
float result;//最终相位差结果

/*
 * 函数名：fftji41024
 * 描述  ：基四fft
 * 输入  ：采样频率
 * 输出  ：无
 */
void fftji41024(float fs)
{
    float max;
    int max_num;
    u16 i;
    arm_cfft_radix4_instance_f32 scfft;
    arm_cfft_radix4_init_f32(&scfft, FFT_LENGTH, 0, 1); //初始化scfft结构体，设定FFT相关参数

    arm_cfft_radix4_f32(&scfft, sin1_fft_inputbuf1);	//FFT计算（基4）
    arm_cmplx_mag_f32(sin1_fft_inputbuf1, sin1_fft_outputbuf, FFT_LENGTH);	//把运算结果复数求模得幅值
    max = sin1_fft_outputbuf[5]; //有直流分量要注意，可以不从一开始开始就进行分析
    for(i = 6; i < FFT_LENGTH / 2; i++) //只要检查到一半就ok，因为是对称的
    {
        if(sin1_fft_outputbuf[i] > max)
        {
            max = sin1_fft_outputbuf[i];
            max_num = i; //保存幅值最大的点
        }
    }
    fre1 = ((float)(max_num) / 1024) * fs; //第0个点对应直流分量，max_num=0的时候为直流分量，fs为采样频率
    phase1 = atan2(sin1_fft_inputbuf1[max_num * 2 + 1], sin1_fft_inputbuf1[max_num * 2]) * 57.3; //虚部除以实部再求反三角函数


    arm_cfft_radix4_init_f32(&scfft, FFT_LENGTH, 0, 1); //初始化scfft结构体，设定FFT相关参数
    arm_cfft_radix4_f32(&scfft, sin2_fft_inputbuf1);	//FFT计算（基4）
    arm_cmplx_mag_f32(sin2_fft_inputbuf1, sin2_fft_outputbuf, FFT_LENGTH);	//把运算结果复数求模得幅值
    max = sin2_fft_outputbuf[5]; //有直流分量要注意，可以不从一开始开始就进行分析
    for(i = 6; i < FFT_LENGTH / 2; i++) //只要检查到一半就ok，因为是对称的
    {
        if(sin2_fft_outputbuf[i] > max)
        {
            max = sin2_fft_outputbuf[i];
            max_num = i; //保存幅值最大的点
        }
    }

    fre2 = ((float)(max_num) / 1024) * fs; //第0个点对应直流分量，max_num=0的时候为直流分量，fs为采样频率
    phase2 = atan2(sin2_fft_inputbuf1[max_num * 2 + 1], sin2_fft_inputbuf1[max_num * 2]) * 57.3; //虚部除以实部再求反三角函数
    result = phase1 - phase2 + 5.0 * fs / 40000.0; //由于通道扫描有时间差，为了给信号的建立留足够的时间，这里会有些延时，这个延时应该跟频率有关系，正比，10k我就给的5.3
    if(result >= 360)
        result -= 360;
    if(result <= -360)
        result += 360;
}
