/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "arm_math.h"
#include "math.h"
#include "bdma.h"
#include "lptim.h"
#include "DDS.h"
#include "arm_fir_example_f32.h"
#include "arm_iir_example_f32.h"
#include "dft.h"
#include "stdlib.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
float WorkingBuf[4000];
float temp[80] = {0};
float before[80] = {0};
float decode[2000] = {0};
float sync[34] = {0};
//float barker_7[25] = {1, 1, 1, -1, -1, 1, -1,     -1,-1,-1,-1,-1,  -1,-1,-1,-1,-1,  -1,-1,-1,-1,-1,  -1,-1,-1};	//补齐25bit，否则自动填0
float barker_7[25] = {1, 1, 1, -1, -1, 1, -1 };	//补齐25bit，否则自动填0
float filter_buf[2000];
int buffer[2000] = {1};
float resul[50];
float max_val;
//u16 index = 2601;
float diff[200];
float bitstream[50];
u32 max_index;
float DC;
float block[80];
u8 min_1 = 80;
u8 min_0 = 80;
int table_1[28]; //1,2,2,3,4,4,5,6,6,7,8,8,9,10,10,11,12,12,13,14,14,15,16,16
int table_0[28];
u8 map[28] = {1, 2, 2, 3, 4, 4, 5, 6, 6, 7, 8, 8, 9, 10, 10, 11, 12, 12, 13, 14, 14, 15, 16, 16, 17, 18, 18, 19};

float Bit_buffer[50];
u8 bit_index = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void SysTick_interrupt()			//1ms
{
//	static u32 i;
//	if(i==1000)
//	{
//		HAL_GPIO_TogglePin(GPIOE,GPIO_PIN_0);
// 		i=0;
//	}
//	i++;
}

//void Detect(int* decode)
//{
//    for(int i = 0; i < 25; i++)
//    {
//        if(decode[i] > 0)
//            decode[i] = 0;
//        else
//            decode[i] = 1;
//    }
//}

void ArrayShift(float a[], int n, int m)		// 循环右移
{
    while(m--)
    {
        float temp = a[n - 1];
        for(int i = n - 1; i >= 1; i--)
            a[i] = a[i - 1];
        a[0] = temp;
    }
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
    /* USER CODE BEGIN 1 */
    /* Enable I-Cache---------------------------------------------------------*/
//    SCB_EnableICache();

//    /* Enable D-Cache---------------------------------------------------------*/
//    SCB_EnableDCache();
    /* USER CODE END 1 */

    /* MCU Configuration--------------------------------------------------------*/

    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* USER CODE BEGIN Init */

    /* USER CODE END Init */

    /* Configure the system clock */
    SystemClock_Config();

    /* USER CODE BEGIN SysInit */

    /* USER CODE END SysInit */

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_DMA_Init();
    MX_ADC1_Init();

    MX_TIM3_Init();
    MX_UART4_Init();
    MX_BDMA_Init();


    MX_LPTIM2_Init();

    MX_ADC2_Init();///////////////////////////////////////////////

    /* USER CODE BEGIN 2 */

    ADC_Tim3_START();		//fs = 2k
    DDS_init();
//	static u8 state = 0;
    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while(1)
    {
        /* USER CODE END WHILE */
        /* USER CODE BEGIN 3 */
        if(ADC_HALF == 1)
        {
 
            for(int i = 0; i < 40; i++)
            {
                WorkingBuf[i] = ((float)(uhADCxConvertedValue[i] - 2047) * (float)(WorkingBuf[2000 + i] - 2047)) / 4096 + 2047;
            }
            for(int i = 0; i < 1000 - 40; i++)
            {
                WorkingBuf[40 + i] = ((float)(uhADCxConvertedValue[40 + i] - 2047) * (float)(uhADCxConvertedValue[i] - 2047)) / 4096 + 2047;
            }
//            // iir 滤波
//            arm_iir_f32_Lp(WorkingBuf,  filter_buf, 1000);

//            for(int i = 0; i < 1000; i++)					// 新的滤波结果存入缓冲区后端
//            {
//                filter_buf[i] = (filter_buf[i] * 16);
//                if(filter_buf[i] > 1.5 / 3.3 * 65535)
//                {
//                    buffer[i] = 0;
//                }
//                else
//                {
//                    buffer[i] = 1;
//                }

//            }
			
            ADC_HALF = 0;
        }
        if(ADC_FULL == 1)
        {
 
            for(int i = 1000 - 40; i < 2000 - 40; i++)
            {
                WorkingBuf[40 + i] = ((float)(uhADCxConvertedValue[i + 40] - 2047) * (float)(uhADCxConvertedValue[i] - 2047)) / 4096 + 2047;
            }
            for(int i = 0; i < 40; i++)
            {
                WorkingBuf[2000 + i] = (float)uhADCxConvertedValue[2000 - 40 + i] ;
            }
//            arm_iir_f32_Lp(&WorkingBuf[1000],  &filter_buf[1000], 1000);
//            for(int i = 1000; i < 2000; i++)
//            {
//                filter_buf[i] = (filter_buf[i] * 16);
//                if(filter_buf[i] > 1.5 / 3.3 * 65535)
//                    buffer[i] = 0;
//                else
//                    buffer[i] = 1;
//				
//             }

 
            ADC_FULL = 0;
        }
        if(DAC_HALF == 1)
        {
            for(int i = 0; i < 1000; i++)
            {
//                u16DDS_DataOut[i] = 65535 * buffer[i];
				
                u16DDS_DataOut[i] = WorkingBuf[i] * 16;

            }
 
            DAC_HALF = 0;
        }
        if(DAC_FULL == 1)
        {
			for(int i = 1000; i < 2000; i++)
            {
//                u16DDS_DataOut[i] = 65535 * buffer[i];
				
                u16DDS_DataOut[i] = WorkingBuf[i] * 16;
            }						
            DAC_FULL = 0;
        }
        if(Bit_HALF == 1)
        {
            for(int i = 0; i < 25; i++)
            {
                if(uhADC2ConvertedValue[i] > 128)
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
            Bit_HALF = 0;
        }
        if(Bit_FULL == 1)
        {
            for(int i = 25; i < 50; i++)
            {
                if(uhADC2ConvertedValue[i] > 128)
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
            Bit_FULL = 0;
        }
    }
    /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

    /**Supply configuration update enable
    */
    MODIFY_REG(PWR->CR3, PWR_CR3_SCUEN, 0);
    /**Configure the main internal regulator output voltage
    */
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    while((PWR->D3CR & (PWR_D3CR_VOSRDY)) != PWR_D3CR_VOSRDY)
    {

    }
    /**Macro to configure the PLL clock source
    */
    __HAL_RCC_PLL_PLLSOURCE_CONFIG(RCC_PLLSOURCE_HSE);
    /**Initializes the CPU, AHB and APB busses clocks
    */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 1;
    RCC_OscInitStruct.PLL.PLLN = 100;
    RCC_OscInitStruct.PLL.PLLP = 2;
    RCC_OscInitStruct.PLL.PLLQ = 2;
    RCC_OscInitStruct.PLL.PLLR = 2;
    RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_3;
    RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
    RCC_OscInitStruct.PLL.PLLFRACN = 0;
    if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }
    /**Initializes the CPU, AHB and APB busses clocks
    */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                                  | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2
                                  | RCC_CLOCKTYPE_D3PCLK1 | RCC_CLOCKTYPE_D1PCLK1;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
    RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

    if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
    {
        Error_Handler();
    }
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_UART4 | RCC_PERIPHCLK_ADC;
    PeriphClkInitStruct.PLL2.PLL2M = 1;
    PeriphClkInitStruct.PLL2.PLL2N = 19;
    PeriphClkInitStruct.PLL2.PLL2P = 2;
    PeriphClkInitStruct.PLL2.PLL2Q = 2;
    PeriphClkInitStruct.PLL2.PLL2R = 2;
    PeriphClkInitStruct.PLL2.PLL2RGE = RCC_PLL2VCIRANGE_3;
    PeriphClkInitStruct.PLL2.PLL2VCOSEL = RCC_PLL2VCOMEDIUM;
    PeriphClkInitStruct.PLL2.PLL2FRACN = 0;
    PeriphClkInitStruct.Usart234578ClockSelection = RCC_USART234578CLKSOURCE_D2PCLK1;
    PeriphClkInitStruct.AdcClockSelection = RCC_ADCCLKSOURCE_PLL2;
    if(HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
        Error_Handler();
    }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */

    /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
       tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
