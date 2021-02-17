/**
  ******************************************************************************
  * File Name          : ADC.c
  * Description        : This file provides code for the configuration
  *                      of the ADC instances.
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2019 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "adc.h"

/* USER CODE BEGIN 0 */
#include "tim.h"
__IO uint16_t uhADCxConvertedValue[ADCCONVERTEDVALUES_BUFFER_SIZE];
u16 u16WorkingBuf[1024];
u8 ADC_HALF = 0 ;
u8 ADC_FULL = 0 ;
 float WorkingBuf[4000];
 float filter_buf[2000];
#include "arm_math.h"
 
u8 Bit_FULL = 0;
u8 Bit_HALF = 0;
 float bitstream[500];
 float Bit_buffer[500];
 float sync[34] ;
 float max_val;
 u32 max_index;
float barker_7[25] = {1, 1, 1, -1, -1, 1, -1 };	//补齐25bit，否则自动填0
 float resul[500];

/* USER CODE END 0 */

ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;

/* ADC1 init function */
void MX_ADC1_Init(void)
{
  ADC_MultiModeTypeDef multimode = {0};
  ADC_ChannelConfTypeDef sConfig = {0};

  /**Common config 
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV4;
  hadc1.Init.Resolution = ADC_RESOLUTION_8B;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc1.Init.LowPowerAutoWait = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_EXTERNALTRIG_EXT_IT11;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_FALLING;
  hadc1.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DMA_CIRCULAR;
  hadc1.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  hadc1.Init.LeftBitShift = ADC_LEFTBITSHIFT_NONE;
  hadc1.Init.BoostMode = DISABLE;
  hadc1.Init.OversamplingMode = DISABLE;
//  hadc1.Init.Oversampling.Ratio = 1023;
//  hadc1.Init.Oversampling.RightBitShift = ADC_RIGHTBITSHIFT_10;
//  hadc1.Init.Oversampling.TriggeredMode = ADC_TRIGGEREDMODE_SINGLE_TRIGGER;
//  hadc1.Init.Oversampling.OversamplingStopReset = ADC_REGOVERSAMPLING_CONTINUED_MODE;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }
  /**Configure the ADC multi-mode 
  */
  multimode.Mode = ADC_MODE_INDEPENDENT;
  if (HAL_ADCEx_MultiModeConfigChannel(&hadc1, &multimode) != HAL_OK)
  {
    Error_Handler();
  }
  /**Configure Regular Channel 
  */
  sConfig.Channel = ADC_CHANNEL_3;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  sConfig.SingleDiff = ADC_SINGLE_ENDED;
  sConfig.OffsetNumber = ADC_OFFSET_NONE;
  sConfig.Offset = 0;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

}

void HAL_ADC_MspInit(ADC_HandleTypeDef* adcHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(adcHandle->Instance==ADC1)
  {
  /* USER CODE BEGIN ADC1_MspInit 0 */

  /* USER CODE END ADC1_MspInit 0 */
    /* ADC1 clock enable */
    __HAL_RCC_ADC12_CLK_ENABLE();
  
    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**ADC1 GPIO Configuration    
    PA6     ------> ADC1_INP3 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* ADC1 DMA Init */
    /* ADC1 Init */
    hdma_adc1.Instance = DMA1_Stream0;
    hdma_adc1.Init.Request = DMA_REQUEST_ADC1;
    hdma_adc1.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_adc1.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_adc1.Init.MemInc = DMA_MINC_ENABLE;
    hdma_adc1.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    hdma_adc1.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    hdma_adc1.Init.Mode = DMA_CIRCULAR;
    hdma_adc1.Init.Priority = DMA_PRIORITY_LOW;
    hdma_adc1.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_adc1) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(adcHandle,DMA_Handle,hdma_adc1);

  /* USER CODE BEGIN ADC1_MspInit 1 */

  /* USER CODE END ADC1_MspInit 1 */
  }
}

void HAL_ADC_MspDeInit(ADC_HandleTypeDef* adcHandle)
{

  if(adcHandle->Instance==ADC1)
  {
  /* USER CODE BEGIN ADC1_MspDeInit 0 */

  /* USER CODE END ADC1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_ADC12_CLK_DISABLE();
  
    /**ADC1 GPIO Configuration    
    PA6     ------> ADC1_INP3 
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_6);

    /* ADC1 DMA DeInit */
    HAL_DMA_DeInit(adcHandle->DMA_Handle);
  /* USER CODE BEGIN ADC1_MspDeInit 1 */

  /* USER CODE END ADC1_MspDeInit 1 */
  }
} 

/* USER CODE BEGIN 1 */
void ADC_Tim3_START()
{
 	HAL_ADCEx_Calibration_Start(&hadc1, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED);
 //	__HAL_TIM_ENABLE(&htim6);
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /*Configure GPIO pin : PA11 */
    GPIO_InitStruct.Pin = GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	HAL_ADC_Start_DMA(&hadc1, (uint32_t *)uhADCxConvertedValue, ADCCONVERTEDVALUES_BUFFER_SIZE);
 
 
}
void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef* hadc)
{
  /* Invalidate Data Cache to get the updated content of the SRAM on the first half of the ADC converted data buffer: 32 bytes */ 
    for(int i = 0; i < 25; i++)
    {
        if(uhADCxConvertedValue[i] > 128)
            bitstream[i] = 1;
        else
            bitstream[i] = -1;
        Bit_buffer[i + 25] = bitstream[i]; // 将bit流存入bitBuffer的后25个
    }

    // 从第一个开始解
    arm_correlate_f32(&Bit_buffer[0], 50, barker_7, 7, resul);		// 找第一个barker码
    arm_max_f32(resul, 2 * 50 - 1, &max_val, &max_index);
    if(max_val == 7)												// 含barker头   帧构成 X+7bit_barker+17bit_info
    {
        max_index = max_index - 50 + 1;				// max_index 指向barker码位置,若max_index小于25，则信息位出现barker码

        if(max_index > 25) 							// 虽然找到头，但是信息位不全，故不解码，也不加24，max_index 直接赋值bit_index
        {
        }
        else
        {
            for(int i = max_index + 7; i < max_index + 24; i++)		// 头比特
            {
                sync[i - (max_index + 7)] = Bit_buffer[i];
            }

            for(int i = 0; i < 17; i++)
            {
                if(sync[i] < 0)
                    sync[i] = 0;
                if(i % 4 == 0)
                    printf("%.0f ", sync[i]);
                else
                    printf("%.0f", sync[i]);
            }
            printf("\r\n");

        }
    }
    for(int i = 0; i < 25; i++)				 // 将bitbuffer的后25个移动到前面
    {
        Bit_buffer[i] = Bit_buffer[i + 25];
    }
}

/**
  * @brief  Conversion DMA half-transfer callback in non-blocking mode
  * @param  hadc: ADC handle
  * @retval None
  */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
   /* Invalidate Data Cache to get the updated content of the SRAM on the second half of the ADC converted data buffer: 32 bytes */ 
 
    for(int i = 25; i < 50; i++)
    {
        if(uhADCxConvertedValue[i] > 128)
            bitstream[i] = 1;
        else
            bitstream[i] = -1;
        Bit_buffer[i] = bitstream[i];  // 将bit流存入bitBuffer的后25个
    }

    // 从第一个开始解
    arm_correlate_f32(&Bit_buffer[0], 50, barker_7, 7, resul);		// 找第一个barker码
    arm_max_f32(resul, 2 * 50 - 1, &max_val, &max_index);
    if(max_val == 7)												// 含barker头   帧构成 X+7bit_barker+17bit_info
    {
        max_index = max_index - 50 + 1;				// max_index 指向barker码位置,若max_index小于25，则信息位出现barker码

        if(max_index > 25) 							// 虽然找到头，但是信息位不全，故不解码，也不加24，max_index 直接赋值bit_index
        {
        }
        else
        {
            for(int i = max_index + 7; i < max_index + 24; i++)		// 头比特
            {
                sync[i - (max_index + 7)] = Bit_buffer[i];
            }

            for(int i = 0; i < 17; i++)
            {
                if(sync[i] < 0)
                    sync[i] = 0;
                if(i % 4 == 0)
                    printf("%.0f ", sync[i]);
                else
                    printf("%.0f", sync[i]);
            }
            printf("\r\n");
        }
    }

    for(int i = 0; i < 25; i++)				 // 将bitbuffer的后25个移动到前面
    {
        Bit_buffer[i] = Bit_buffer[i + 25];
    }
}
/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
