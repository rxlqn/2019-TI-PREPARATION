#include "ui.h"
#include "lcd.h"
#include "sys.h"
#include "stdio.h"
#include "touch.h"
#include "ad9959.h"
#include "delay.h"

#define BACK_SCREEN BLACK
#define TABLE_COLOR BLUE
#define TEXT_COLOR LIGHTBLUE

#define stf(x1,y1,x2,y2) tp_dev.x[0]>(u16)x1&&tp_dev.x[0]<(u16)x2&&tp_dev.y[0]>(u16)y1&&tp_dev.y[0]<(u16)y2


int UI_ID = MENU;
u8 amp[32]={0};
u8 fre[32]={0};
u8 phs[32]={0};			//·ù¶ÈÆµÂÊÏàÎ»ÏÔÊ¾×Ö·û´®
u16 x_before;			
u16 y_before;				//´¢´æÉÏ´Î´¥ÃşÎ»ÖÃ£¬·ÀÖ¹Îó´¥
u16 submit_num_amp=1800;
u32 submit_num_fre=10000;
u32 submit_num_phs=0;

void Clear_Menu(void)
{
	LCD_Fill(618,3,797,95,BACK_SCREEN);
	LCD_Fill(618,97,797,191,BACK_SCREEN);
	LCD_Fill(618,193,797,286,BACK_SCREEN);
	LCD_Fill(618,290,797,383,BACK_SCREEN);
	LCD_Fill(618,385,797,477,BACK_SCREEN);
}

void Show_Start(void)
{
	LCD_Clear(BLACK);
	Show_Menu();
	Show_Num_Keyboard();
	Show_State();
}

void Show_Menu(void)
{
	UI_ID = MENU;
	Clear_Menu();
	LCD_ShowString(704-56,48*1-32,500,200,16,"SETAMP");
	LCD_ShowString(704-56,48*3-32,500,200,16,"SETFRE");
	LCD_ShowString(704-48,48*5-32,500,200,16,"SETPHA");
	LCD_ShowString(704-72,48*7-32,500,200,16,"MODULATION");
	LCD_ShowString(704-72,48*9-32,500,200,16,"SYNC");
}	


void Show_Num_Keyboard(void)
{
	LCD_DrawRectangle(18,120+18*1+96*0,18+126,120+18*1+96*1);
	LCD_DrawRectangle(18,120+18*2+96*1,18+126,120+18*2+96*2);
	LCD_DrawRectangle(18,120+18*3+96*2,18+126,120+18*3+96*3);
	
	LCD_DrawRectangle(36+126,120+18*1+96*0,288,120+18*1+96*1);
	LCD_DrawRectangle(36+126,120+18*2+96*1,288,120+18*2+96*2);
	LCD_DrawRectangle(36+126,120+18*3+96*2,288,120+18*3+96*3);
	
	LCD_DrawRectangle(306,120+18*1+96*0,432,120+18*1+96*1);
	LCD_DrawRectangle(306,120+18*2+96*1,432,120+18*2+96*2);
	LCD_DrawRectangle(306,120+18*3+96*2,432,120+18*3+96*3);
	
	LCD_DrawRectangle(450,120+18*1+96*0,576,120+18*1+96*1);
	LCD_DrawRectangle(450,120+18*2+96*1,576,120+18*2+96*2);
	LCD_DrawRectangle(450,120+18*3+96*2,576,120+18*3+96*3);
	POINT_COLOR = TEXT_COLOR;
	
	LCD_ShowChar(69,162,'1',24,0);
	LCD_ShowChar(213,162,'2',24,0);
	LCD_ShowChar(213+18+126,162,'3',24,0);
	LCD_ShowChar(69,162+18+96,'4',24,0);
	LCD_ShowChar(213,162+18+96,'5',24,0);
	LCD_ShowChar(213+18+126,162+18+96,'6',24,0);
	LCD_ShowChar(501,162+18+96,'0',24,0);
	LCD_ShowChar(69,390,'7',24,0);
	LCD_ShowChar(213,390,'8',24,1);
	LCD_ShowChar(213+18+126,390,'9',24,0);
	LCD_ShowString(497,170,200,500,16,"OK");
	LCD_ShowString(489,398,200,500,16,"DEL");
}


void Show_State(void)
{
	sprintf((char*)amp,"amp:%ldmv",submit_num_amp);
	sprintf((char*)fre,"fre:%ldHz",submit_num_fre);
	sprintf((char*)phs,"phs:%ld",submit_num_phs);
	LCD_ShowString(0,0,200,500,16,amp);
	LCD_ShowString(0,40,200,500,16,fre);
	LCD_ShowString(0,80,200,500,16,phs);
}

void Show_Modulation_Choose(void)
{
	Clear_Menu();
	LCD_ShowString(704-56,48*1-32,500,200,16,"AMP");
	LCD_ShowString(704-56,48*3-32,500,200,16,"FRE");
	LCD_ShowString(704-48,48*5-32,500,200,16,"PHA");
}


void Scan_All(void)
{
	
		u8 temp;
	Show_State();
	tp_dev.scan(0); 		
	if((tp_dev.x[0] == x_before)&&(tp_dev.y[0] == y_before))return;
	x_before = tp_dev.x[0];
	y_before = tp_dev.y[0];	
	if(tp_dev.sta&TP_PRES_DOWN)
	{
		switch(UI_ID)
		{
			case MENU:
			{
				
				if(stf(628,13,787,85))
				{
					UI_ID = SET_AMP;
					submit_num_amp = 0;
				}
				else if(stf(628,107,787,181))
				{
					UI_ID = SET_FRE;
					submit_num_fre = 0;
				}
				else if(stf(628,203,787,276))
				{
					UI_ID = SET_PHS;
					submit_num_phs = 0;
				}
				else if(stf(628,300,787,373))
				{
					UI_ID = MODULATION_CHOOSE;
					Show_Modulation_Choose();
				}
				else if(stf(628,390,787,480))
				{
					  AD9959_IO_UpDate();
				}
			}break;
			case SET_AMP:
			{
				if(stf(28,148,134,224))temp = '1';
				else if(stf(172,148,248,224))temp = '2';
				else if(stf(316,148,392,224))temp = '3';
				else if(stf(28,262,134,338))temp = '4';
				else if(stf(172,262,248,338))temp = '5';
				else if(stf(316,262,392,338))temp = '6';
				else if(stf(28,356,134,480))temp = '7';
				else if(stf(172,356,248,480))temp = '8';
				else if(stf(316,356,392,480))temp = '9';
				else if(stf(460,262,566,338))temp = '0';
				else if(stf(460,148,566,224))temp = '*';
				else if(stf(460,356,566,480))temp = '&';
				else temp = '%';
				if(temp != '%')
				{
					if(temp == '*')
					{
						AD9959_Ch(1);
						AD9959_Set_Amp((u16)((double)submit_num_amp/1800.0)*1023.0);
						AD9959_Ch(2);
						AD9959_Set_Amp((u16)((double)submit_num_amp/1800.0)*1023.0);
						AD9959_Ch(3);
						AD9959_Set_Amp((u16)((double)submit_num_amp/1800.0)*1023.0);
						AD9959_Ch(4);
						AD9959_Set_Amp((u16)((double)submit_num_amp/1800.0)*1023.0);
						UI_ID = MENU;
					}
					else
					{
						if(temp != '&')
						{
								submit_num_amp *= 10;
								submit_num_amp += temp-'0';
						}
						else
						{
							submit_num_amp /= 10;
						}
					}
				}
			}break;
			case SET_FRE:
			{
				if(stf(28,148,134,224))temp = '1';
				else if(stf(172,148,248,224))temp = '2';
				else if(stf(316,148,392,224))temp = '3';
				else if(stf(28,262,134,338))temp = '4';
				else if(stf(172,262,248,338))temp = '5';
				else if(stf(316,262,392,338))temp = '6';
				else if(stf(28,356,134,480))temp = '7';
				else if(stf(172,356,248,480))temp = '8';
				else if(stf(316,356,392,480))temp = '9';
				else if(stf(460,262,566,338))temp = '0';
				else if(stf(460,148,566,224))temp = '*';
				else if(stf(460,356,566,480))temp = '&';
				else temp = '%';
				if(temp != '%')
				{
					if(temp == '*')
					{
						AD9959_Ch(1);
						AD9959_Set_Fre(submit_num_fre);
						AD9959_Ch(2);
						AD9959_Set_Fre(submit_num_fre);
						AD9959_Ch(3);
						AD9959_Set_Fre(submit_num_fre);
						AD9959_Ch(4);
						AD9959_Set_Fre(submit_num_fre);
						UI_ID = MENU;
					}
					else
					{
						if(temp != '&')
						{
								submit_num_fre *= 10;
								submit_num_fre += temp-'0';
						}
						else
						{
							submit_num_fre /= 10;
						}
					}
				}
			}break;
			case SET_PHS:
			{
				if(stf(28,148,134,224))temp = '1';
				else if(stf(172,148,248,224))temp = '2';
				else if(stf(316,148,392,224))temp = '3';
				else if(stf(28,262,134,338))temp = '4';
				else if(stf(172,262,248,338))temp = '5';
				else if(stf(316,262,392,338))temp = '6';
				else if(stf(28,356,134,480))temp = '7';
				else if(stf(172,356,248,480))temp = '8';
				else if(stf(316,356,392,480))temp = '9';
				else if(stf(460,262,566,338))temp = '0';
				else if(stf(460,148,566,224))temp = '*';
				else if(stf(460,356,566,480))temp = '&';
				else temp = '%';
				if(temp != '%')
				{
					if(temp == '*')
					{
						AD9959_Ch(1);
						AD9959_Set_Pha((float)submit_num_phs);
						AD9959_Ch(2);
						AD9959_Set_Pha((float)submit_num_phs);
						AD9959_Ch(3);
						AD9959_Set_Pha((float)submit_num_phs);
						AD9959_Ch(4);
						AD9959_Set_Pha((float)submit_num_phs);						
						UI_ID = MENU;
					}
					else
					{
						if(temp != '&')
						{
								submit_num_phs *= 10;
								submit_num_phs += temp-'0';
						}
						else
						{
							submit_num_phs /= 10;
						}
					}
				}
			}break;
			case MODULATION_CHOOSE:
			{
//				if(stf(628,13,787,85))
//				{
//					UI_ID = MENU;
//					Show_Start();
//					AD9959_Mod(1,amp_mod);
//				}
			//	else 
//					if(stf(628,107,787,181))
				{
					UI_ID = MENU;
					Show_Start();
				//	AD9959_Mod(2,fre_mod);
				}
//				else if(stf(628,203,787,276))
//				{
//					UI_ID = MENU;
//					Show_Start();
//					AD9959_Mod(1,phs_mod);
//				}
			}
		}
	}
}











