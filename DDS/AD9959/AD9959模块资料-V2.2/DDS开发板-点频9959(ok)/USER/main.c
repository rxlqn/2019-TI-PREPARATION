/**********************************************************
                       康威电子
										 
功能：stm32f103rct6控制，25MHz时钟， AD9959正弦波点频输出，范围0-200M，
			显示：12864cog
接口：控制接口请参照AD9959.h  按键接口请参照key.h
时间：2015/11/10
版本：1.0
作者：康威电子
其他：

更多电子需求，请到淘宝店，康威电子竭诚为您服务 ^_^
https://shop110336474.taobao.com/?spm=a230r.7195193.1997079397.2.Ic3MRJ

**********************************************************/
#include "stm32_config.h"
#include "stdio.h"
#include "led.h"
#include "lcd.h"
#include "AD9959.h"
#include "key.h"
#include "timer.h"
#include "task_manage.h"

extern u8 _return;
int main(void)
{
	
	MY_NVIC_PriorityGroup_Config(NVIC_PriorityGroup_2);	//设置中断分组
	delay_init(72);	//初始化延时函数
	LED_Init();	//初始化LED接口
	key_init();
	Init_AD9959();
	Timerx_Init(99,71);
	initial_lcd();
  delay_ms(300);
  
	while(1)
	{
		KeyRead();
		Set_PointFre(Keycode, 0);
		if(_return)
		{
			_return=0;
			LCD_Refresh_Gram();
		}
		KEY_EXIT();
	}	
}

