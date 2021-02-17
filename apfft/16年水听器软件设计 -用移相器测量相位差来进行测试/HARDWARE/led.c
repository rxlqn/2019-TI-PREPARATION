#include "led.h"

/*
 * 函数名：Led_Init
 * 描述  ：led初始化函数
 * 输入  ：无
 * 输出  ：无
 */ 
void Led_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;//初始化飞控上面的灯
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_ResetBits(GPIOA,GPIO_Pin_2);
	GPIO_Init(GPIOA, &GPIO_InitStructure);	//初始化航向灯
	
}

