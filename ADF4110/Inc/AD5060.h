

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __gpio_H
#define __gpio_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"


/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */
extern void AD5060_CS_init(void);
extern void AD5060_setval(u32 val);
/* USER CODE END Private defines */



/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

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
