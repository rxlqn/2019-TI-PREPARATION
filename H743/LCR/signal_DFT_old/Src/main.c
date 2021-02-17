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
#include "bdma.h"
#include "lptim.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "arm_math.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "adc.h"
#include "dac.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#include "DDS.h"
#include "dma.h"
#include "math.h"
#include "dft.h"
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
int ADC1_value[1000];
int ADC2_value[1000];

float Amplitude_1[1000];
float Amplitude_2[1000];

float phi_1[1000];
float phi_2[1000];


u16 Flag_1 = 0, Flag_2 = 0;
float delta_t = 0;
float delta_phi = 0;
float z, r, c;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
//uint32_t               int_TIM2_CNT = 0;
//uint32_t               int_TIM3_CNT = 0;
//uint32_t               int_TIM2_CNT_old = 0;
//uint32_t               int_TIM3_CNT_old = 0;
//uint32_t               Freq = 0;

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void SysTick_interrupt()			//1ms
{
//	static int i;
//	if(i==1000)
//	{
//  		int_TIM3_CNT=TIM3->CNT;
//		int_TIM2_CNT=TIM2->CNT;
//
//		Freq=int_TIM3_CNT_old-int_TIM3_CNT+((int_TIM2_CNT_old-int_TIM2_CNT)<<16);
////		Freq=Freq*4;
//		HAL_GPIO_TogglePin(GPIOE,GPIO_PIN_0);
//		int_TIM2_CNT_old=int_TIM2_CNT;
//		int_TIM3_CNT_old=int_TIM3_CNT;
// 		i=0;
//	}
//	i++;
}

void ArrayShift(int a[], int n, int m);
void correlation(void);

void correlation()						    // 循环卷积
{
    float temp, cor = 0;
    for(int i = 0; i < 1000; i++)
    {
        for(int j = 0; j < 1000; j++)
        {
            temp += (float)ADC1_value[j] / 1000 * (float)ADC2_value[j] / 1000;
        }
        if(temp > cor)
        {
            cor = temp;
            Flag_1 = i;						// 移动次数
        }
        temp = 0;
        ArrayShift(ADC1_value, 1000, 1);		// 右移通道1的数据，固定通道2
    }
}


void ArrayShift(int a[], int n, int m)		// 循环右移
{
    while(m--)
    {
        int temp = a[n - 1];
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
//  SCB_EnableICache();

//  /* Enable D-Cache---------------------------------------------------------*/
//  SCB_EnableDCache();
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
    MX_BDMA_Init();
    MX_UART4_Init();
//  MX_TIM2_Init();
    MX_TIM3_Init();
    MX_LPTIM2_Init();
    /* USER CODE BEGIN 2 */
////Freq_Init();
    MX_DMA_Init();

    MX_ADC2_Init();
    HAL_ADCEx_Calibration_Start(&hadc2, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED);

    MX_ADC1_Init();
    MX_ADC2_Init();

    ADC12_Tim3_START();

    InitSinTable();

    DDS_init();
 

    MX_DAC1_Init();

    /////////////
    if(HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_1, DAC_ALIGN_12B_R, 2047) != HAL_OK) //实测：0.413V  理论：511 0.412V
    {
        /* Setting value Error */
        Error_Handler();
    }
    /*##-4- Enable DAC Channel1 ################################################*/
    if(HAL_DAC_Start(&hdac1, DAC_CHANNEL_1) != HAL_OK)
    {
        /* Start Error */
        Error_Handler();
    }

    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while(1)
    {
        /* USER CODE END WHILE */
        if(ADC_Complete)
        {
            volatile float max1 = 0, max2 = 0;
            volatile float Phi_1, Phi_2, delta;
            volatile int state = 0;
            //1M 采样率
            for(int i = 0; i < 1000; i++)
            {
                ADC1_value[i] = (aADCDualConvertedValues[i] & 0xFFFF);
                ADC2_value[i] = (aADCDualConvertedValues[i] >> 16);
            }

			
            DFT_Calculate(ADC1_value, Amplitude_1, phi_1);
            DFT_Calculate(ADC2_value, Amplitude_2, phi_2);

            max1 = Amplitude_1[1] / 1.003;
            max2 = Amplitude_2[1];
            Phi_1 = phi_1[1];
            Phi_2 = phi_2[1];
            delta_phi = Phi_1 - Phi_2;
//
#define R2 995
//		correlation();
#define F 1000
//		// 提高10bit
// 		delta_t = (float)Flag_1*1024*1/1000000;
//
//		//若精度为1°，则一个周期360个点，即采样率为360*F，最高采样率5M，(2M 有问题)
//		delta_phi = ((int)(delta_t*F*360)>>10)%360;

            r = (max1 / max2 * arm_cos_f32(delta_phi / 360 * 2 * 3.14) - 1) * R2 - 910;
            c = (max1 / max2 * arm_sin_f32(delta_phi / 360 * 2 * 3.14)) * R2;
            c = -1 / (2 * 3.14 * 1000 * c) * 1000000;		//单位uF

            ADC_Complete = 0;
            // 重启ADC_DMA
            HAL_ADC_DeInit(&hadc1);
            HAL_ADC_DeInit(&hadc2);
            MX_DMA_Init();

            MX_ADC2_Init();
            HAL_ADCEx_Calibration_Start(&hadc2, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED);

            MX_ADC1_Init();
            MX_ADC2_Init();
            HAL_ADCEx_MultiModeStart_DMA(&hadc1, (uint32_t*)aADCDualConvertedValues, ADCCONVERTEDVALUES_BUFFER_SIZE);
        }
        /* USER CODE BEGIN 3 */


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
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_UART4 | RCC_PERIPHCLK_LPTIM2;
    PeriphClkInitStruct.Usart234578ClockSelection = RCC_USART234578CLKSOURCE_D2PCLK1;
    PeriphClkInitStruct.Lptim2ClockSelection = RCC_LPTIM2CLKSOURCE_D3PCLK1;
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
