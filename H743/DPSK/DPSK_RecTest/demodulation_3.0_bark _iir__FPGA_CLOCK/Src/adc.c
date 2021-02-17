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
#include "lptim.h"
#include "dpsk_mod.h"
/* USER CODE BEGIN 0 */
#include "tim.h"
__IO uint16_t uhADCxConvertedValue[ADCCONVERTEDVALUES_BUFFER_SIZE];
__IO uint16_t uhADC2ConvertedValue[50];
extern float WorkingBuf[4000];
extern float filter_buf[2000];
#include "arm_iir_example_f32.h"
#include "arm_math.h"
u8 ADC_HALF = 0 ;
u8 ADC_FULL = 0 ;
u8 Bit_FULL = 0;
u8 Bit_HALF = 0;
extern float bitstream[50];
extern float Bit_buffer[50];
extern float sync[34] ;
extern float max_val;
extern u32 max_index;
extern float barker_7[25]  ;	//补齐25bit，否则自动填0
extern float resul[50];
 uint16_t u16DecodeOutBuf[8192];

/* USER CODE END 0 */

ADC_HandleTypeDef hadc1;
ADC_HandleTypeDef hadc2;
DMA_HandleTypeDef hdma_adc1;
DMA_HandleTypeDef hdma_adc2;

/* ADC1 init function */
void MX_ADC1_Init(void)
{
    ADC_MultiModeTypeDef multimode = {0};
    ADC_ChannelConfTypeDef sConfig = {0};

    /**Common config
    */
    hadc1.Instance = ADC1;
    hadc1.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV2;
    hadc1.Init.Resolution = ADC_RESOLUTION_12B;
    hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
    hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
    hadc1.Init.LowPowerAutoWait = DISABLE;
    hadc1.Init.ContinuousConvMode = DISABLE;
    hadc1.Init.NbrOfConversion = 1;
    hadc1.Init.DiscontinuousConvMode = DISABLE;
    hadc1.Init.ExternalTrigConv = ADC_EXTERNALTRIG_T3_TRGO;
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
    if(HAL_ADC_Init(&hadc1) != HAL_OK)
    {
        Error_Handler();
    }
    /**Configure the ADC multi-mode
    */
    multimode.Mode = ADC_MODE_INDEPENDENT;
    if(HAL_ADCEx_MultiModeConfigChannel(&hadc1, &multimode) != HAL_OK)
    {
        Error_Handler();
    }
    /**Configure Regular Channel
    */
    sConfig.Channel = ADC_CHANNEL_3;
    sConfig.Rank = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = ADC_SAMPLETIME_2CYCLES_5;
    sConfig.SingleDiff = ADC_SINGLE_ENDED;
    sConfig.OffsetNumber = ADC_OFFSET_NONE;
    sConfig.Offset = 0;
    if(HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
    {
        Error_Handler();
    }

}

/* ADC2 init function */
void MX_ADC2_Init(void)
{
    ADC_ChannelConfTypeDef sConfig = {0};

    /** Common config
    */
    hadc2.Instance = ADC2;
    hadc2.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV2;
    hadc2.Init.Resolution = ADC_RESOLUTION_8B;
    hadc2.Init.ScanConvMode = ADC_SCAN_DISABLE;
    hadc2.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
    hadc2.Init.LowPowerAutoWait = DISABLE;
    hadc2.Init.ContinuousConvMode = DISABLE;
    hadc2.Init.NbrOfConversion = 1;
    hadc2.Init.DiscontinuousConvMode = DISABLE;
    hadc2.Init.ExternalTrigConv = ADC_EXTERNALTRIG_EXT_IT11;
    hadc2.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_FALLING;
    hadc2.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DMA_CIRCULAR;
    hadc2.Init.Overrun = ADC_OVR_DATA_PRESERVED;
    hadc2.Init.LeftBitShift = ADC_LEFTBITSHIFT_NONE;
    hadc2.Init.OversamplingMode = DISABLE;
    if(HAL_ADC_Init(&hadc2) != HAL_OK)
    {
        Error_Handler();
    }
    /** Configure Regular Channel
    */
    sConfig.Channel = ADC_CHANNEL_4;
    sConfig.Rank = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
    sConfig.SingleDiff = ADC_SINGLE_ENDED;
    sConfig.OffsetNumber = ADC_OFFSET_NONE;
    sConfig.Offset = 0;
    if(HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK)
    {
        Error_Handler();
    }
}

void HAL_ADC_MspInit(ADC_HandleTypeDef* adcHandle)
{

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    if(adcHandle->Instance == ADC1)
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
        if(HAL_DMA_Init(&hdma_adc1) != HAL_OK)
        {
            Error_Handler();
        }

        __HAL_LINKDMA(adcHandle, DMA_Handle, hdma_adc1);

        /* USER CODE BEGIN ADC1_MspInit 1 */

        /* USER CODE END ADC1_MspInit 1 */
    }
    else if(adcHandle->Instance == ADC2)
    {
        /* USER CODE BEGIN ADC2_MspInit 0 */

        /* USER CODE END ADC2_MspInit 0 */
        /* ADC2 clock enable */

        __HAL_RCC_ADC12_CLK_ENABLE();


        __HAL_RCC_GPIOC_CLK_ENABLE();
        /**ADC2 GPIO Configuration
        PC4     ------> ADC2_INP4
        */
        GPIO_InitStruct.Pin = GPIO_PIN_4;
        GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

        /* ADC2 DMA Init */
        /* ADC2 Init */
        hdma_adc2.Instance = DMA1_Stream1;
        hdma_adc2.Init.Request = DMA_REQUEST_ADC2;
        hdma_adc2.Init.Direction = DMA_PERIPH_TO_MEMORY;
        hdma_adc2.Init.PeriphInc = DMA_PINC_DISABLE;
        hdma_adc2.Init.MemInc = DMA_MINC_ENABLE;
        hdma_adc2.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
        hdma_adc2.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
        hdma_adc2.Init.Mode = DMA_CIRCULAR;
        hdma_adc2.Init.Priority = DMA_PRIORITY_LOW;
        hdma_adc2.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
        if(HAL_DMA_Init(&hdma_adc2) != HAL_OK)
        {
            Error_Handler();
        }

        __HAL_LINKDMA(adcHandle, DMA_Handle, hdma_adc2);

        /* USER CODE BEGIN ADC2_MspInit 1 */

        /* USER CODE END ADC2_MspInit 1 */
    }
}

void HAL_ADC_MspDeInit(ADC_HandleTypeDef* adcHandle)
{

    if(adcHandle->Instance == ADC1)
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
    else if(adcHandle->Instance == ADC2)
    {
        /* USER CODE BEGIN ADC2_MspDeInit 0 */

        /* USER CODE END ADC2_MspDeInit 0 */
        /* Peripheral clock disable */

        __HAL_RCC_ADC12_CLK_DISABLE();


        /**ADC2 GPIO Configuration
        PC4     ------> ADC2_INP4
        */
        HAL_GPIO_DeInit(GPIOC, GPIO_PIN_4);

        /* ADC2 DMA DeInit */
        HAL_DMA_DeInit(adcHandle->DMA_Handle);
        /* USER CODE BEGIN ADC2_MspDeInit 1 */

        /* USER CODE END ADC2_MspDeInit 1 */
    }
}

/* USER CODE BEGIN 1 */
typedef struct
{
    uint8_t u8FlagHalfFull;
    uint8_t u8FlagSync;
    uint16_t u16LevelZeroCounter;
    uint16_t u16LevelOneCounter;
    uint16_t u16PosOfRiseEdge;
    uint32_t u32BitNumGot;
    uint8_t u8Code[8];
	uint8_t u8Flag8;
} tsBitSyncStruct;
tsBitSyncStruct sBitSyncStruct;

 uint8_t u8BitPool[256];
#define		BIT_LEVEL_THRESHHOLD		3
void	BitSync(uint16_t u16InputData[], uint32_t u32Level, uint32_t u32FlagHalforFull,
                tsBitSyncStruct* psBitSyncStruct)
{
    uint32_t i, j;
    uint32_t u32ScanStart, u32ScanEnd;
    uint32_t u32LevelZeroCounter = 0, u32LevelOneCounter = 0, u32LevelState = 1;
    uint32_t u32BitOneCounter = 0, u32BitZeroCounter = 0;
    uint32_t SyncState = 0;
    uint32_t u32Code = 0;

    u32LevelZeroCounter += psBitSyncStruct->u16LevelZeroCounter;
    u32LevelOneCounter += psBitSyncStruct->u16LevelOneCounter;

    // 半满
    if(u32FlagHalforFull == 1)
    {
        u32ScanStart = 0;
        u32ScanEnd = 500;
    }
    else if(u32FlagHalforFull == 2)
    {
        u32ScanStart = 500;
        u32ScanEnd = 1000;
    }
    else
    {
        return;
    }

    {

        // 搜索同步上升沿，即使之前已经同步也可以继续搜索
        for(i = u32ScanStart; i < u32ScanEnd; i++)								// 一次4000个点
        {
            switch(u32LevelState)												// 两个状态 高和低
            {
            case 1:
                if( u16InputData[i] < u32Level)									// 阈值，暂定2000
                {
                    u32LevelZeroCounter++;										// 零电平数量大于阈值，暂定3
                    if(u32LevelZeroCounter > BIT_LEVEL_THRESHHOLD)
                    {
                        // 下降沿												// 超过三个零电平判定为下降沿
                        u32LevelState = 0;					
                        u32BitOneCounter = (u32LevelOneCounter + 5) / 10;		// 一的个数直接由1电平的计数器反解求得

                        if(psBitSyncStruct->u8FlagSync == 1)
                        {
                            // 如果已经同步
                            // 可进行位判决
                            // 根据目前的链路协议，正常情况下u32BitOneCounter不大于8
                            uint32_t k;
                            for(k = 0; k < u32BitOneCounter; k++)
                            {
                                if((psBitSyncStruct->u32BitNumGot + k) > 100)			// 比特池存放100个比特
                                {
                                    psBitSyncStruct->u32BitNumGot = 0;
                                }
                                u8BitPool[psBitSyncStruct->u32BitNumGot + k] = 1;
                            }
                            psBitSyncStruct->u32BitNumGot += u32BitOneCounter;

                            if(psBitSyncStruct->u32BitNumGot >= (16 + 1))
                            {
                                uint32_t m = 0, u32Data = 0;
                                // 转移比特池到Code
                                for(k = 0; k < psBitSyncStruct->u32BitNumGot - 1; k++)
                                {
                                    u32Data = u32Data << 1;
                                    if(u8BitPool[k] == 0)
                                    {
                                        u32Data |= 0x01;
                                    }
                                    m++;
                                    if(m >= 8)
                                    {
                                        m = 0;
                                        psBitSyncStruct->u8Code[k / 8] = u32Data;
                                        u32Data = 0;
                                    }
                                }// End for
                                psBitSyncStruct->u32BitNumGot = 0;
                            }
                        }			// 如果没有同步，则丢弃

                        u32LevelOneCounter = 0;

                    }
                }
                else
                {
                    u32LevelZeroCounter = 0;
                    u32LevelOneCounter++;
                }
                break;
            case 0:
                if( u16InputData[i] >= u32Level)
                {
                    u32LevelOneCounter++;
                    if(u32LevelOneCounter > BIT_LEVEL_THRESHHOLD)
                    {
                        // 上升沿，需要进行回溯
                        u32LevelState = 1;
                        u32BitZeroCounter = (u32LevelZeroCounter + 5) / 10;
                        u32LevelZeroCounter = 0;
                        if(u32BitZeroCounter == 8)
                        {
                            u32BitZeroCounter = 0;

//                            // 同步
//                            SyncState = 1;
							psBitSyncStruct->u8Flag8 = 1;
                            psBitSyncStruct->u8FlagSync = 1;
//                            psBitSyncStruct->u16PosOfRiseEdge = i;

                            // 取走接收数据后，相关变量清0
                            psBitSyncStruct->u32BitNumGot = 0;
//										memset((int8_t *)psBitSyncStruct->u8Code,0,sizeof(psBitSyncStruct->u8Code));
                        }
                        else
                        {
                            uint32_t k;
                            // 当前非同步上升沿
                            SyncState = 0;
                            // 如果已经同步，可进行位判决
                            if(psBitSyncStruct->u8FlagSync == 1)
                            {
                                for(k = 0; k < u32BitZeroCounter; k++)
                                {
                                    if((psBitSyncStruct->u32BitNumGot + k) > 100)
                                    {
                                        psBitSyncStruct->u32BitNumGot = 0;
                                    }
                                    u8BitPool[psBitSyncStruct->u32BitNumGot + k] = 0;
                                }
                                psBitSyncStruct->u32BitNumGot += u32BitZeroCounter;

                                if(psBitSyncStruct->u32BitNumGot >= (16 + 1))
                                {
                                    uint32_t m = 0, u32Data = 0;
                                    // 转移比特池到Code
                                    for(k = 0; k < psBitSyncStruct->u32BitNumGot - 1; k++)
                                    {
                                        u32Data = u32Data << 1;
                                        if(u8BitPool[k] == 0)
                                        {
                                            u32Data |= 0x01;
                                        }
                                        m++;
                                        if(m >= 8)
                                        {
                                            m = 0;
                                            psBitSyncStruct->u8Code[k / 8] = u32Data;
                                            u32Data = 0;
                                        }
                                    }// End for
                                    psBitSyncStruct->u32BitNumGot = 0;
                                }// End if(psBitSyncStruct->u32BitNumGot >= (16+1))
                            }// End if( psBitSyncStruct->u16FlagSync == 1)
                        }// End else
                    }// End if(u32LevelOneCounter > BIT_LEVEL_THRESHHOLD)
                }// End if(u16InputData[i] >= u32Level)
                else
                {
                    u32LevelOneCounter = 0;
                    u32LevelZeroCounter++;

                }
                break;
            default:
                break;
            }
        }// End for( i = 0; i < 4000; i++)
    }//

    if(u32LevelZeroCounter > 500)
    {
        u32LevelZeroCounter = 500;
        psBitSyncStruct->u32BitNumGot = 0;
        memset((int8_t*)psBitSyncStruct->u8Code, 0, sizeof(psBitSyncStruct->u8Code));
    }
    if(u32LevelOneCounter > 500)
    {
        u32LevelOneCounter = 500;
        psBitSyncStruct->u32BitNumGot = 0;
        memset((int8_t*)psBitSyncStruct->u8Code, 0, sizeof(psBitSyncStruct->u8Code));
    }
    psBitSyncStruct->u16LevelZeroCounter = u32LevelZeroCounter;
    psBitSyncStruct->u16LevelOneCounter = u32LevelOneCounter;

    return;
}


void ADC_Tim3_START()
{
    HAL_ADCEx_Calibration_Start(&hadc1, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED);
    HAL_ADCEx_Calibration_Start(&hadc2, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED);

//	GPIO_InitTypeDef GPIO_InitStruct = {0};

//	  /*Configure GPIO pin : PA11 */
//	GPIO_InitStruct.Pin = GPIO_PIN_11;
//	GPIO_InitStruct.Mode = GPIO_MODE_EVT_FALLING;
//	GPIO_InitStruct.Pull = GPIO_NOPULL;
//	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    HAL_ADC_Start_DMA(&hadc1, (uint32_t*)uhADCxConvertedValue, N_bit*N_phase*2);
//	__HAL_TIM_ENABLE(&htim3);
    HAL_ADC_Start_DMA(&hadc2, (uint32_t*)uhADC2ConvertedValue, 50);

    HAL_TIM_Base_Start(&htim3); //使能定时器3和定时器3更新中断：TIM_IT_UPDATE


}

 

void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef* hadc)
{
    /* Invalidate Data Cache to get the updated content of the SRAM on the first half of the ADC converted data buffer: 32 bytes */
    if(hadc->Instance == ADC1)
    {
        for(int i = 0; i < N_phase; i++)
        {
            WorkingBuf[i] = ((float)(uhADCxConvertedValue[i] - 2047) * (float)(WorkingBuf[2*N_phase*N_bit + i] - 2047)) / 4096 + 2047;
        }
        for(int i = 0; i < N_phase*N_bit - N_phase; i++)
        {
            WorkingBuf[N_phase + i] = ((float)(uhADCxConvertedValue[N_phase + i] - 2047) * (float)(uhADCxConvertedValue[i] - 2047)) / 4096 + 2047;
        }
//            // iir 滤波
        arm_iir_f32_Lp(WorkingBuf,  filter_buf, N_phase*N_bit);
		for(int i = 0;i<N_phase*N_bit;i++)
		{
			u16DecodeOutBuf[i] = filter_buf[i];
		}
	    BitSync( u16DecodeOutBuf, 2000, 1, &sBitSyncStruct);

    }
    else
    {

    }
}

/**
  * @brief  Conversion DMA half-transfer callback in non-blocking mode
  * @param  hadc: ADC handle
  * @retval None
  */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    /* Invalidate Data Cache to get the updated content of the SRAM on the first half of the ADC converted data buffer: 32 bytes */
    if(hadc->Instance == ADC1)
    {
        for(int i = N_phase*N_bit - N_phase; i < N_phase*N_bit*2 - N_phase; i++)
        {
            WorkingBuf[N_phase + i] = ((float)(uhADCxConvertedValue[i + N_phase] - 2047) * (float)(uhADCxConvertedValue[i] - 2047)) / 4096 + 2047;
        }
        for(int i = 0; i < N_phase; i++)
        {
            WorkingBuf[N_phase*N_bit*2 + i] = (float)uhADCxConvertedValue[N_phase*N_bit*2 - N_phase + i] ;
        }
        arm_iir_f32_Lp(&WorkingBuf[N_phase*N_bit],  &filter_buf[N_phase*N_bit], N_phase*N_bit);
        
		
		for(int i = N_phase*N_bit;i<2*N_phase*N_bit;i++)
		{
			u16DecodeOutBuf[i] = filter_buf[i];
		}
		BitSync( &u16DecodeOutBuf[N_phase*N_bit], 2000, 2, &sBitSyncStruct);

    }
    else
    {

    }
}
/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
