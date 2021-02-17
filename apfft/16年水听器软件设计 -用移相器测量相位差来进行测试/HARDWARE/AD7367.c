/*
用normal mode读取两路信号的数据
*/
#include "AD7367.h"
/*
	初始化,power_up得一定时间，初始化IO口后才能启动它
*/
void AD7367_Init()
{
	 AD7367_CNVST(1);//保持高电平至少70μs，powerup时间
	 delay_ms(1);
}