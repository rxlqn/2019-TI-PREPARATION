#include "AD5060.h"
#include "spi.h"

GPIO_InitTypeDef GPIO_Initure = {0};

void AD5060_CS_init(void)
{
__HAL_RCC_GPIOA_CLK_ENABLE();	

GPIO_Initure.Pin=GPIO_PIN_6;           
GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;    
GPIO_Initure.Pull=GPIO_PULLUP;   
GPIO_Initure.Speed=GPIO_SPEED_FAST;	
HAL_GPIO_Init(GPIOA,&GPIO_Initure);
 
HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6,GPIO_PIN_SET);
}

  u32 value[1];

void AD5060_setval(u32 val)
{

  SPI1_CS_L();
//  HAL_Delay(10);
  SPI1_Start_Transmit(&val,1);
                
//  HAL_Delay(10);
  SPI1_CS_H();
  HAL_Delay(1);
}