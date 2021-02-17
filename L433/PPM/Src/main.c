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
  * COPYRIGHT(c) 2018 STMicroelectronics
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
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "12864.h"
#include "ADPCM.h"
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
u32 Ch_1, Ch_2 = 0;
short WorkingBuf[1024];
signed char Qerror[1024];
short Output[1024];
u16 len = 0;

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void SysTick_interrupt()
{
    static u16 i;
    if(i == 20)
    {
//		if(ReadKey(1))
//			key1=1;
//		if(ReadKey(2))
//			key2=1;

        i = 0;
    }
    i++;
}

void Average()
{
    Ch_1 = 0;
    Ch_2 = 0;
    for(int i = 0; i < 50; i++)
    {
        Ch_1 += uhADCxConvertedValue[i * 2];
        Ch_2 += uhADCxConvertedValue[i * 2 + 1];
    }
    Ch_1 = Ch_1 / 50;
    Ch_2 = Ch_2 / 50;
}
#ifdef DPCM

int error[512];
int errorQ[512];
int S_input[512];
int R_input[512];


int Quant_Value(int err)
{
    if(err <= 2 && err > -2)
        return 0;
    if(err <= 7 && err > 2)
        return 5;
    if(err <= 13 && err > 7)
        return 10;
    if(err <= 22 && err > 13)
        return 17;
    if(err <= 33 && err > 22)
        return 28;
    if(err <= 45 && err > 33)
        return 39;
    if(err <= 59 && err > 45)
        return 52;
    if(err > 59)
        return 67;

    if(err > -7 && err <= -2)
        return -5;
    if(err > -13 && err <= -7)
        return -10;
    if(err > -22 && err <= -13)
        return -17;
    if(err > -33 && err <= -22)
        return -28;
    if(err > -45 && err <= -33)
        return -39;
    if(err > -59 && err <= -45)
        return -52;
    if(err <= -59)
        return -67;
    return 0;
}

void DPCM_encoder(u8* data)
{
	int predict;

    for(int i = 0; i < 512; i++)
    {
        if(i == 0)
            predict = 128;
        else
            predict = S_input[i - 1];
        error[i] = (int)data[i] - predict;
        errorQ[i] = Quant_Value(error[i]);
        S_input[i] = predict + errorQ[i];
    }
}

void DPCM_decoder()
{
	int predict;

    for(int i = 0; i < 512; i++)
    {
        if(i == 0)
            predict = 128;
        else
            predict = R_input[i - 1];
        R_input[i] = predict + errorQ[i];
    }
}
#endif


#define TxBufSize 2
u8 table[16] = {0x03, 0x05, 0x09, 0x11, 0x21, 0x41, 0x81, 0x06, 0x0A, 0x12, 0x22, 0x42, 0x82, 0x0C, 0x14, 0x24};
uint8_t aTxBuffer[TxBufSize] = {0};

void PPM_Encoder(u8 data)	//脉冲位置调制，一次两个脉冲，C_8^2>16,可传4bit
{
    //分两次传输
    //取低4bit
//	u8 temp = data & 0x0F;
//	HAL_UART_Transmit(&huart1, &table[temp], 1, 0xFFFF);
//	HAL_Delay(1);
//    temp = data >> 4;
//	HAL_UART_Transmit(&huart1, &table[temp], 1, 0xFFFF);
    u8 temp = data & 0x0F;
    aTxBuffer[0] = table[temp];
    temp = data >> 4;
    aTxBuffer[1] = table[temp];
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
    /* USER CODE BEGIN 1 */

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
    MX_USART1_UART_Init();
    MX_TIM6_Init();
    MX_SPI1_Init();
    /* USER CODE BEGIN 2 */
    ADC_Tim6_START();
    LCD_Initial();
//    HAL_UART_Transmit_DMA(&huart1, (uint8_t*)aTxBuffer, TxBufSize);
//    PPM_Encoder(0x13);
struct adpcm_state adpcm;
/* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while(1)
    {
        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */
        if(ADC_HALF)
        {
			for(int i = 0;i<512;i++)
			{
				WorkingBuf[i] = uhADCxConvertedValue[i] - 32767;
			}
			for(int i = 0;i<512;i++)	// 编码前
			{
				printf("%d ",WorkingBuf[i]);
			}
			adpcm_coder(WorkingBuf, Qerror, 512, &adpcm);
			adpcm_decoder(Qerror, Output, 512, &adpcm);
			for(int i = 0;i<512;i++)	// 解码后
			{
				printf("%d ",Output[i]);
			}
            ADC_HALF = 0;
        }
        if(ADC_FULL)
        {

            ADC_FULL = 0;
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
    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

    /** Initializes the CPU, AHB and APB busses clocks
    */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 1;
    RCC_OscInitStruct.PLL.PLLN = 20;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
    RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
    RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
    if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }
    /** Initializes the CPU, AHB and APB busses clocks
    */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                                  | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
    {
        Error_Handler();
    }
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1 | RCC_PERIPHCLK_ADC;
    PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
    PeriphClkInit.AdcClockSelection = RCC_ADCCLKSOURCE_PLLSAI1;
    PeriphClkInit.PLLSAI1.PLLSAI1Source = RCC_PLLSOURCE_HSE;
    PeriphClkInit.PLLSAI1.PLLSAI1M = 1;
    PeriphClkInit.PLLSAI1.PLLSAI1N = 8;
    PeriphClkInit.PLLSAI1.PLLSAI1P = RCC_PLLP_DIV7;
    PeriphClkInit.PLLSAI1.PLLSAI1Q = RCC_PLLQ_DIV2;
    PeriphClkInit.PLLSAI1.PLLSAI1R = RCC_PLLR_DIV2;
    PeriphClkInit.PLLSAI1.PLLSAI1ClockOut = RCC_PLLSAI1_ADC1CLK;
    if(HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
        Error_Handler();
    }
    /** Configure the main internal regulator output voltage
    */
    if(HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
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
void assert_failed(char* file, uint32_t line)
{
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
       tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
