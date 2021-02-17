/**
  ******************************************************************************
  * File Name          : USART.c
  * Description        : This file provides code for the configuration
  *                      of the USART instances.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "usart.h"

/* USER CODE BEGIN 0 */
uint8_t aRxBuffer[RXBUFFERSIZE];
u8 UART_HALF = 0;
u8 UART_FULL = 0;
#include "string.h"
#include "stdlib.h" 
/* USER CODE END 0 */

UART_HandleTypeDef huart4;
DMA_HandleTypeDef hdma_uart4_rx;

/* UART4 init function */
void MX_UART4_Init(void)
{

  huart4.Instance = UART4;
  huart4.Init.BaudRate = 115200;
  huart4.Init.WordLength = UART_WORDLENGTH_8B;
  huart4.Init.StopBits = UART_STOPBITS_1;
  huart4.Init.Parity = UART_PARITY_NONE;
  huart4.Init.Mode = UART_MODE_TX_RX;
  huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart4.Init.OverSampling = UART_OVERSAMPLING_16;
  huart4.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart4.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart4.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart4) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart4, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart4, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart4) != HAL_OK)
  {
    Error_Handler();
  }

}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(uartHandle->Instance==UART4)
  {
  /* USER CODE BEGIN UART4_MspInit 0 */

  /* USER CODE END UART4_MspInit 0 */
    /* UART4 clock enable */
    __HAL_RCC_UART4_CLK_ENABLE();
  
    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**UART4 GPIO Configuration    
    PA0     ------> UART4_TX
    PA1     ------> UART4_RX 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF8_UART4;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* UART4 DMA Init */
    /* UART4_RX Init */
    hdma_uart4_rx.Instance = DMA1_Stream2;
    hdma_uart4_rx.Init.Request = DMA_REQUEST_UART4_RX;
    hdma_uart4_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_uart4_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_uart4_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_uart4_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_uart4_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_uart4_rx.Init.Mode = DMA_CIRCULAR;
    hdma_uart4_rx.Init.Priority = DMA_PRIORITY_LOW;
    hdma_uart4_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_uart4_rx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(uartHandle,hdmarx,hdma_uart4_rx);

    /* UART4 interrupt Init */
    HAL_NVIC_SetPriority(UART4_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(UART4_IRQn);
  /* USER CODE BEGIN UART4_MspInit 1 */

  /* USER CODE END UART4_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==UART4)
  {
  /* USER CODE BEGIN UART4_MspDeInit 0 */

  /* USER CODE END UART4_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_UART4_CLK_DISABLE();
  
    /**UART4 GPIO Configuration    
    PA0     ------> UART4_TX
    PA1     ------> UART4_RX 
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_0|GPIO_PIN_1);

    /* UART4 DMA DeInit */
    HAL_DMA_DeInit(uartHandle->hdmarx);

    /* UART4 interrupt Deinit */
    HAL_NVIC_DisableIRQ(UART4_IRQn);
  /* USER CODE BEGIN UART4_MspDeInit 1 */

  /* USER CODE END UART4_MspDeInit 1 */
  }
} 

/* USER CODE BEGIN 1 */
/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
    /* Place your implementation of fputc here */
    /* e.g. write a character to the USART1 and Loop until the end of transmission */
    HAL_UART_Transmit(&huart4, (uint8_t*)&ch, 1, 0xFFFF);
    return ch;
}


void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef* huart)
{
    UART_HALF = 1;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart)
{
    UART_FULL = 1;
}

void UART_DMA_init()
{
    HAL_UART_Receive_DMA(&huart4, (uint8_t*)&aRxBuffer, RXBUFFERSIZE);
    __HAL_UART_ENABLE_IT(&huart4, UART_IT_IDLE);

}
 
/* 串口接收空闲中断 */
void UsartReceive_IDLE(UART_HandleTypeDef* huart)
{
    if((__HAL_UART_GET_FLAG(&huart4, UART_FLAG_IDLE) != RESET))
    {
        __HAL_UART_CLEAR_IDLEFLAG(&huart4);
		int i = huart->Instance->ISR;
            i = huart->Instance->RDR;
        HAL_UART_DMAStop(&huart4);
		Button();
        memset(aRxBuffer, 0, RXBUFFERSIZE);
//        HAL_DMA_DeInit(&hdma_uart4_rx);
//        HAL_DMA_Init(&hdma_uart4_rx);
        HAL_UART_Receive_DMA(&huart4, (uint8_t*)&aRxBuffer, RXBUFFERSIZE);
    }
}

u8 but1 = 0; 
u8 but2 = 0; 
u8 but3 = 0; 
u8 but4 = 0; 
u8 but5 = 0; 
u8 but6 = 0; 
u8 but_enter = 0;
u32 num = 50000;
u8 OK = 0;
u8 Clear = 0;

void Button(void)
{
    if(strcmp((char*)&aRxBuffer, "[BN:1]\r\n") == 0)
    {
        but1 = 1;
		HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_0);
    }
    if(strcmp((char*)&aRxBuffer, "[BN:2]\r\n") == 0)
    {
        but2 = 1;
    }
    if(strcmp((char*)&aRxBuffer, "[BN:3]\r\n") == 0)
    {
        but3 = 1;
    }
    if(strcmp((char*)&aRxBuffer, "[BN:4]\r\n") == 0)
    {
        but4 = 1;
    }
    if(strcmp((char*)&aRxBuffer, "[BN:5]\r\n") == 0)
    {
        but5 = 1;
    }
    if(strcmp((char*)&aRxBuffer, "[BN:6]\r\n") == 0)
    {
        but6 = 1;
    }
	if(strcmp((char*)&aRxBuffer, "OK\r\n") == 0)
	{
		OK = 1;
	}
	if(strcmp((char*)&aRxBuffer, "[BN:12]\r\n") == 0)
	{
		Clear = 1;
	}
	///////  读取键盘模组数字  //////////
	char tempt[100] = {0};
	char temp_num[100] = {0};
	memcpy(tempt,aRxBuffer,1);
	if(strcmp(tempt, "{") == 0)			// 判断是否按下enter
    {
        but_enter = 1;
		int i = 0;
		num = 0;
		while(1)
		{
			if(aRxBuffer[i+1]==0x7d)		// 判断“}”
				break;
			temp_num[i] = aRxBuffer[i+1];
			num = num*10;
			num = num + temp_num[i] - 0x30;
			i++;
		}
//		num = atoi(temp_num);
    }

}

//void GpuSend(char * bufsend)
//{	u8 i=0;
//	while (1)
//	{	if (bufsend[i]!=0)
//		{	 
//			HAL_UART_Transmit(&huart4, (uint8_t*)&bufsend[i], 1, 0xFFFF);
//			while(__HAL_UART_GET_FLAG(&huart4, UART_IT_TXE) == RESET){};  //等待发送缓冲为空
//			i++;
//		}
//		else return;
//	}
//}
/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
