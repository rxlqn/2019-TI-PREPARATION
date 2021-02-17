#ifndef _AD9910_H__
#define _AD9910_H__

#include "spi.h"


#define AD9910_RESET_H()           	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8,GPIO_PIN_SET)
#define AD9910_RESET_L()           	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8,GPIO_PIN_RESET)
//#define AD9958_POWER_DOWN_H()       HAL_GPIO_WritePin(GPIOD, GPIO_PIN_11,GPIO_PIN_SET)
//#define AD9958_POWER_DOWN_L()       HAL_GPIO_WritePin(GPIOD, GPIO_PIN_11,GPIO_PIN_RESET)
//#define AD9958_SCLK_H()        		  HAL_GPIO_WritePin(GPIOF, GPIO_Pin_9,GPIO_PIN_SET)
//#define AD9958_SCLK_L()        		  HAL_GPIO_WritePin(GPIOF, GPIO_Pin_9,GPIO_PIN_RESET)
#define AD9910_CS_H()           	  HAL_GPIO_WritePin(GPIOF, GPIO_PIN_6,GPIO_PIN_SET)
#define AD9910_CS_L()           	  HAL_GPIO_WritePin(GPIOF, GPIO_PIN_6,GPIO_PIN_RESET)
#define AD9910_IO_UPDATE_H()        HAL_GPIO_WritePin(GPIOG, GPIO_PIN_10,GPIO_PIN_SET)
#define AD9910_IO_UPDATE_L()        HAL_GPIO_WritePin(GPIOG, GPIO_PIN_10,GPIO_PIN_RESET)


extern u8 valueflag;
extern u8 value;

void AD9910_IO_init(void);
void ad9910_test(void);
void IO_Update(void);
void change_dds_freq_amp_phase(u8 ch,u32 freq,u16 amp,u8 phase);
void ad9910_CR_init(void);
void ad9910_state_machine(void);

#endif
