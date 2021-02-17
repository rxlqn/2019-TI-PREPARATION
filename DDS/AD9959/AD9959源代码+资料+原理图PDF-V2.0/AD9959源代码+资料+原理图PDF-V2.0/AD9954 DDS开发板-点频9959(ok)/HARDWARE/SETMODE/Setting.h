#ifndef __SETTING_H
#define __SETTING_H			  	 
#include "stdlib.h"	    
#include <stdint.h>
#include <stdbool.h>
#define Temp_Max 11
typedef struct
{
	uint32_t num;
	uint8_t Num_Temp[Temp_Max];
	uint8_t name[10];
}Setvalue;	
void Setting_Mode(bool *sta);
void Setting_Init(void);

#endif

