/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "spi.h"
#include "gpio.h"
#include "iic.h"
#include "tim.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "112c04.h"
#include "iictest.h"
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

void mode1();
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* MCU Configuration--------------------------------------------------------*/
  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();
  /* Configure the system clock */
  SystemClock_Config();
  /* Initialize all configured peripherals */
	
  MX_TIM2_Init();
	
	IIC_Init();
	int i = 0;
	float value[50] = {0};
	uint8_t reg1=0XFF,reg2=0XFF,reg3=0XFF,reg4=0XFF;
	ADCreset();
	/////////////////
  IICwreg(wconfig0,0x81);  //配置寄存器0，AINp=AIN0,AINn=AVSS ; PGA disable，一倍增益
	HAL_Delay(1);
	HAL_Delay(1);
	
	/////////////////////
	IICwreg(wconfig1,0x88);  // 1000 1000：  Normal mode，330SPS,Internal reference,continious conversion mode
	HAL_Delay(1);
	HAL_Delay(1);
	
	/////////////////////
	IICwreg(wconfig2,0x01);  //00000000,IDAC current off
	HAL_Delay(1);
	HAL_Delay(1);
	
	//////////////////////////////
	IICwreg(wconfig3,0x02);  //IDAC1,IDAC2,both off,no connection    
	HAL_Delay(1);
	HAL_Delay(1);
	
	
//	SDA_IN();//SDA设置为输入
	
	reg1 = IICrreg(rconfig0);
	reg2 = IICrreg(rconfig1);
	reg3 = IICrreg(rconfig2);
	reg4 = IICrreg(rconfig3);


//能连续读同一个寄存器，要是连续读多个寄存器，只有第一个能读对
//单独读一个寄存器都是对的
//	
	ADCstart();  //0x08
	for(i=0;i<50;i++)
		{
			//value[i] = IICrdata();
			value[i]=ADtransfer2(IICrdata());
		}
  while (1)
  {
  }
}

void mode1()      //测量直流电压
{
	float value[50] = {0};
	float aveV=0,z=0;
	u32 i=0;
	ADCreset();
  IICwreg(wconfig0,0x82);  //配置寄存器0，AINp=AIN0,AINn=AVSS ; PGA disable，一倍增益
	IICwreg(wconfig1,0x88);  // 1000 1000：  Normal mode，330SPS,Internal reference,continious conversion mode
	IICwreg(wconfig2,0x00);  //00000000,IDAC current off
	IICwreg(wconfig3,0x00);  //IDAC1,IDAC2,both off,no connection     

	
	
	ADCstart();  //0x08
	{
		
		for(i=0;i<50;i++)
		{
			value[i] = IICrdata();
			//value[i]=ADtransfer2(IICrdata());
		}	        

		for(i=0;i<50;i++)
		{
			aveV+=value[i];
		}
		aveV=(aveV/50);  
        aveV*=1000;		
	   }		
}


/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 40;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure the main internal regulator output voltage 
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
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
void assert_failed(char *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
