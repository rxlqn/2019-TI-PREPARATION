#include "uart_lcd.h"
#include "usart.h"
uint8_t RxBuf[BUFFERSIZE];

void uart_lcd_init()
{
	 HAL_UART_Receive_IT(&huart4, (uint8_t *)RxBuf, BUFFERSIZE);
}