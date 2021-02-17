#include "stm32f4xx.h"
#include "usart.h"
#include "delay.h"
#include "my_fft.h"
#include "led.h"
#include "AD7367_SPI.h"
#include "AD7367.h"
#include "adc.h"
#include "hanning_window.h"



extern u8 sample_flag;
float fre;//频率
extern float sin1_fft_inputbuf1[4096];
extern float sin2_fft_inputbuf1[4096];//汉宁窗的自卷积
extern float conv[2047];//卷积窗
#define fl 40000
#define a2r 0.0174532
extern u16 Adc1_Value[2];//ADC1的值
u8 sample_flag1 = 1;
u8 sample_flag2 = 0;
int sample_num = 1;
//10k,28.8度相位差

int main(void)

{
    int i;
    delay_init(168);
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
    Led_Init();
    delay_init(168);  //初始化延时函数
    Adc_Init();
    Adc_DMA_Init();
    ADC_SoftwareStartConv(ADC1);/* 软件启动ADC1转换 */
    Led_Init();
    uart_init(115200);		//初始化串口波特率为115200
    TIM3_Int_Init(24, 83); //
    while(1)
    {
        if(sample_flag)
        {
            sample_flag = 0; //定时器置位标志位
//进行采样，一次采样2048个点，实际用到的只有1024个点,采样2048点只是为了能够好好补偿
            if(sample_flag1)
            {
                sin1_fft_inputbuf1[sample_num * 2] = ((float)Adc1_Value[0] / 4096.0) * 3.3;
                sin2_fft_inputbuf1[sample_num * 2] = ((float)Adc1_Value[1] / 4096.0) * 3.3;
                sample_num++;
                if(sample_num >= 2048) //这里改变了标志位，应该再延时相应时间再采样，不然相当于连续采了相同的值造成很大误差
                {
                    sample_num = 1;
                    sample_flag1 = 0;
                    sample_flag2 = 1;
                }
            }
//进行全相位fft的处理，并由affft得到频率和相位
            if(sample_flag2)
            {
                for(i = 1; i < 2048; i++) //1到2N-1项,0到2n-2循环2n-1次
                {
                    sin1_fft_inputbuf1[i * 2] = sin1_fft_inputbuf1[i * 2] * conv[i - 1]; //每一项都点乘
                    sin2_fft_inputbuf1[i * 2] = sin2_fft_inputbuf1[i * 2] * conv[i - 1]; //每一项都点乘
                }
                for(i = 0; i < 1024; i++) //1到N-1项,循环n-1次
                {
                    sin1_fft_inputbuf1[i * 2] += sin1_fft_inputbuf1[(i + 1024) * 2]; //获得处理后的fft值，只处理实部
                    sin2_fft_inputbuf1[i * 2] += sin2_fft_inputbuf1[(i + 1024) * 2]; //获得处理后的fft值，只处理实部
                }

                fftji41024(fl);
                for(i = 0; i < 4096; i++)
                {
                    sin1_fft_inputbuf1[i] = 0; //数组清零，切记切记，不然会导致计算出错
                    sin2_fft_inputbuf1[i] = 0;
				}
                sample_flag1 = 1;
                sample_flag2 = 0;
            }
        }
    }
}
