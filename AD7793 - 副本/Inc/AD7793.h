

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __gpio_H
#define __gpio_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

void AD5060_CS_init(void);
void AD5060_setval(u32 val);


#define SPI1_CS_L() 	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6,GPIO_PIN_RESET)
#define SPI1_CS_H() 	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6,GPIO_PIN_SET)



#ifdef __cplusplus
}
#endif
#endif /*__ pinoutConfig_H */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
