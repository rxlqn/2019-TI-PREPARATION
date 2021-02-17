#include <stdio.h>
#include <stdlib.h>
#include <math.h>		
#include "dft.h"
#define PI	3.1415926535	

#define N 1000   //定于序列长度变量
float Input_Squence1[1000];	//输入的原始数据序列 
float Input_Squence2[1000];	//输入的原始数据序列 

float Amplitude1[1000];   //存储幅值计算结果
float Amplitude2[1000];   //存储幅值计算结果

float phi1[1000];
float phi2[1000];

typedef struct				//复数结构体,用于实现傅里叶运算 
{
	double real, imag;
}complex;
complex Result_Point[1000];

void DFT_Calculate_Point(int k,int *Input_Squence)
{
	int n = 0;
	complex Sum_Point;
	complex One_Point[N];
	Sum_Point.real = 0;
	Sum_Point.imag = 0;
	for (n = 0; n < N; n++)
	{
		One_Point[n].real = cos(2 * PI / N * k*n)*Input_Squence[n];  //运用欧拉公式把复数拆分成实部和虚部 
		One_Point[n].imag = -sin(2 * PI / N * k*n)*Input_Squence[n];

		Sum_Point.real += One_Point[n].real;//对每一个点的实部求和
		Sum_Point.imag += One_Point[n].imag;
		//对每一个点的虚部求和
	}
	Result_Point[k].real = Sum_Point.real/N;
	Result_Point[k].imag = Sum_Point.imag/N;
}

void DFT_Calculate(int *Input_Squence,float *Amplitude,float *phi)
{
	int i = 0;
	for (i = 0; i < N; i++)
	{
		if(i == 1)
			DFT_Calculate_Point(i,Input_Squence);
		Amplitude[i] = 2*sqrt(Result_Point[i].real * Result_Point[i].real + Result_Point[i].imag * Result_Point[i].imag);  //计算幅值,直流不用乘2
		phi[i] = atan(Result_Point[i].imag / Result_Point[i].real);
		phi[i] = phi[i] / (2 * PI) * 360 ;
		if (Result_Point[i].imag > 0 & Result_Point[i].real < 0)
			phi[i] = phi[i] + 180;
		if (Result_Point[i].imag < 0 & Result_Point[i].real < 0)
			phi[i] = phi[i] - 180;
	}
}
