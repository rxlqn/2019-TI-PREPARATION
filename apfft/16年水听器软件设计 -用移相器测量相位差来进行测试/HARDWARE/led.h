#ifndef __LED_H
#define __LED_H

#include "stm32f4xx.h"
#define LED_TOGGLE   GPIOA->ODR ^= GPIO_Pin_2;// πledµ∆…¡∂Ø
void Led_Init(void);

#endif



