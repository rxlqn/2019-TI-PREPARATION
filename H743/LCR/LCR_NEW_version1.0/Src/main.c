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
#include "bdma.h"
#include "dac.h"
#include "dma.h"
#include "lptim.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "arm_math.h"
#include "bdma.h"
//#include "lptim.h"
#include "DDS.h"
#include "arm_fir_example_f32.h"
#include "arm_iir_example_f32.h"
#include "ADPCM.h"
#include "uart_lcd.h"
#include "dft.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"
#include "interpolation.h"
#include "ad779x.h"

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

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
short WorkingBuf[1024];
signed char Qerror[1024];
short Output[1024];
u16 len = 0;

int ADC1_value[1000];
int ADC2_value[1000];

float Amplitude_1[1000];
float Amplitude_2[1000];

float phi_1[1000];
float phi_2[1000];

int dst1[3000], dst2[3000];

u16 Flag_1 = 0, Flag_2 = 0;
float delta_t = 0;
float delta_phi = 0;
float z, r, c;

float voltage = 0;
int data = 0; 

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void SysTick_interrupt()			//1ms
{
    static u32 i;
    if(i == 1000)
    {

        i = 0;
    }
    i++;
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
//    HAL_Delay(1000);
    /* USER CODE END SysInit */

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_BDMA_Init();
    MX_DMA_Init();
    // ADC 初始化和校准
    MX_ADC2_Init();
    HAL_ADCEx_Calibration_Start(&hadc2, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED);

    MX_ADC1_Init();
    MX_ADC2_Init();

    MX_TIM3_Init();
    MX_UART4_Init();
    MX_LPTIM2_Init();
	MX_SPI3_Init();
    MX_DAC1_Init();
    /* USER CODE BEGIN 2 */
    ADC12_Tim3_START();		//fs = 2k
    InitSinTable();
    DDS_init(num);
    UART_DMA_init();

   AD779X_Init();		// 需要校准

//	printf("SPG(1);\r\n");		// 初始化

    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */

    if(HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_1, DAC_ALIGN_12B_R, 1023) != HAL_OK) //实测：0.413V  理论：511 0.412V
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
    while(1)
    {
        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */

		if(AD779X_CheckReadySW()==AD779X_SR_RDY) 	
		{
			data = AD779X_ReadDataRegister24();
		    voltage = (float)data/(0xffffff )*2.494; 
		}
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		

//        // 设置激励频率
//        if(but_enter == 1)
//        {
//            DDS_init(num);			// 初始值1K
//            but_enter = 0;

//        }
// 
//        if(Clear == 1)
//        {
//            HAL_Delay(500);
//            printf("SPG(1);\r\n");
//            HAL_Delay(500);
//            Clear = 0;
//        }
//        if(ADC_FULL)
//        {
//            volatile float max1 = 0, max2 = 0;
//            volatile float Phi_1, Phi_2, delta;
//            volatile int state = 0;
//            //1M 采样率
//            for(int i = 0; i < 1000; i++)
//            {
//                ADC1_value[i] = (aADCDualConvertedValues[i] & 0xFFFF);		// 主ADC
//                ADC2_value[i] = (aADCDualConvertedValues[i] >> 16);
//				printf("%d ",ADC1_value[i]);
//            }

//            if(num >= 1000)		// 被测信号大于1K
//            {
//                Interpolation(ADC1_value, dst1, 1000, num); // 输出1000长度
//                Interpolation(ADC2_value, dst2, 1000, num); // 输出1000长度
//            }
//            else						// 小于1K，只取前1000个点
//            {
//                for(int i = 0; i < 1000; i++)
//                {
//                    dst1[i] = ADC1_value[i];
//                    dst2[i] = ADC2_value[i];
//                }
//            }
//			for(int i = 0;i<1000;i++)
//				printf("%d ",dst1[i]);
//            DFT_Calculate(dst1, Amplitude_1, phi_1);
//            DFT_Calculate(dst2, Amplitude_2, phi_2);

//            max1 = Amplitude_1[1]  + 0.01;			// U 
//            max2 = Amplitude_2[1]/910 + 0.01;		// I
//            Phi_1 = phi_1[1];
//            Phi_2 = phi_2[1];
//            delta_phi = Phi_1 - Phi_2;
////
//#define R2 995
////		correlation();
//#define F num
//            r = (max1 / max2 * arm_cos_f32(delta_phi / 360 * 2 * 3.14)) ;
//            c = (max1 / max2 * arm_sin_f32(delta_phi / 360 * 2 * 3.14)) ;
//            c = -1 / (2 * 3.14 * F * c) * 1000000;		//单位uF

//            char buf[100];				// 1s 显示一次
//            sprintf(buf, "DS32(586,276,'%.6f    ',1);\r\n", c);
//            printf(buf);

//            HAL_Delay(1000);

//            ADC_FULL = 0;
//            // 重启ADC_DMA
//            HAL_ADC_DeInit(&hadc1);
//            HAL_ADC_DeInit(&hadc2);
////            MX_DMA_Init();

//            MX_ADC2_Init();
//            HAL_ADCEx_Calibration_Start(&hadc2, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED);

//            MX_ADC1_Init();
//            MX_ADC2_Init();
//            HAL_ADCEx_MultiModeStart_DMA(&hadc1, (uint32_t*)aADCDualConvertedValues, ADCCONVERTEDVALUES_BUFFER_SIZE);
//        }

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

    /** Supply configuration update enable
    */
    HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);
    /** Configure the main internal regulator output voltage
    */
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}
    /** Macro to configure the PLL clock source
    */
    __HAL_RCC_PLL_PLLSOURCE_CONFIG(RCC_PLLSOURCE_HSE);
    /** Initializes the CPU, AHB and APB busses clocks
    */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI | RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.HSIState = RCC_HSI_DIV1;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
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
    /** Initializes the CPU, AHB and APB busses clocks
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
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_UART4 | RCC_PERIPHCLK_SPI3
            | RCC_PERIPHCLK_LPTIM2 | RCC_PERIPHCLK_ADC
            | RCC_PERIPHCLK_CKPER;
    PeriphClkInitStruct.PLL2.PLL2M = 1;
    PeriphClkInitStruct.PLL2.PLL2N = 27;
    PeriphClkInitStruct.PLL2.PLL2P = 3;
    PeriphClkInitStruct.PLL2.PLL2Q = 2;
    PeriphClkInitStruct.PLL2.PLL2R = 2;
    PeriphClkInitStruct.PLL2.PLL2RGE = RCC_PLL2VCIRANGE_3;
    PeriphClkInitStruct.PLL2.PLL2VCOSEL = RCC_PLL2VCOWIDE;
    PeriphClkInitStruct.PLL2.PLL2FRACN = 0;
    PeriphClkInitStruct.CkperClockSelection = RCC_CLKPSOURCE_HSI;
    PeriphClkInitStruct.Spi123ClockSelection = RCC_SPI123CLKSOURCE_CLKP;
    PeriphClkInitStruct.Usart234578ClockSelection = RCC_USART234578CLKSOURCE_D2PCLK1;
    PeriphClkInitStruct.Lptim2ClockSelection = RCC_LPTIM2CLKSOURCE_D3PCLK1;
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
