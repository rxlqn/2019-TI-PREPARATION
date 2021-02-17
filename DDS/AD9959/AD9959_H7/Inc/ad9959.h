#ifndef _AD9959_H__
#define _AD9959_H__

#include "main.h"
#include "spi.h"


#define AD9958_RESET_H()           	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8,GPIO_PIN_SET)
#define AD9958_RESET_L()           	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8,GPIO_PIN_RESET)
#define AD9958_POWER_DOWN_H()       HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9,GPIO_PIN_SET)
#define AD9958_POWER_DOWN_L()       HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9,GPIO_PIN_RESET)
//    PA5     ------> SPI1_SCK

//    PA7     ------> SPI1_MOSI 
#define AD9958_CS_H()           	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10,GPIO_PIN_SET)
#define AD9958_CS_L()           	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10,GPIO_PIN_RESET)
#define AD9958_IO_UPDATE_H()        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11,GPIO_PIN_SET)
#define AD9958_IO_UPDATE_L()        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11,GPIO_PIN_RESET)


void AD9959_IO_init(void);
void ad9959_test(void);
void IO_Update(void);
void change_dds_freq_amp_phase(u8 ch,u32 freq,u16 amp,u8 phase);
void ad9959_CR_init(void);
void ad9959_state_machine(void);

#endif
