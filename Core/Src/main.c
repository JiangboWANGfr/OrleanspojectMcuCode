/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "usart.h"
#include "gpio.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tsl2561.h"
#include "Adafruit_TCS34725.h"
#include "ssd1306_tests.h"
#include "ssd1306.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
	
uint8_t debugStatus=0;
uint8_t state=1;
//Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_700MS, TCS34725_GAIN_1X);
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

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
  MX_I2C1_Init();
  MX_USART1_UART_Init();
	ssd1306_Init();
  /* USER CODE BEGIN 2 */

//  begin();
  // Set persistence filter to generate an interrupt for every RGB Cycle, regardless of the integration limits
//  write8(TCS34725_PERS, TCS34725_PERS_NONE);
// 
	char buffer[256];
  sprintf(buffer, "Start2");
  HAL_UART_Transmit(&huart1, (uint8_t*)buffer, strlen(buffer), HAL_MAX_DELAY);
  HAL_Delay(1000);
	setInterrupt(true);
 

//  traceWrite(txt,20);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	uint16_t lux;
	uint16_t broadband, ir;
	
	int clock = SystemCoreClock;
	char txt[200]={0};
	sprintf(txt,"clock= %d\n",clock);
	HAL_UART_Transmit(&huart1, (uint8_t*)txt, strlen(txt), HAL_MAX_DELAY);
	char rgbinf[50],colorTempinf[50],Luxinf[50];
  while (1)
  {
		//TCS34725
		tcs3272_init();
		uint16_t r, g, b,c, colorTemp, lux;
		HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
    getRawData(&r, &g, &b, &c);
		getRGB(&r, &g, &b);
		colorTemp = calculateColorTemperature(r, g, b);
//    colorTemp = calculateColorTemperature_dn40(r, g, b,c);
		lux = calculateLux(r, g, b);
		
		sprintf(rgbinf,"%d %d %d\n",r,g,b);
		sprintf(colorTempinf,"ColTe=%d",colorTemp);
		
		HAL_UART_Transmit(&huart1, (uint8_t*)rgbinf, strlen(rgbinf), HAL_MAX_DELAY);
		HAL_UART_Transmit(&huart1, (uint8_t*)colorTempinf, strlen(colorTempinf), HAL_MAX_DELAY);
		//TSL2561
		TSL2561_SetTiming(&hi2c1, TSL2561_INTEGRATIONTIME_101MS, TSL2561_GAIN_0X);
		// Check luminosity level and calculate lux
		TSL2561_GetLuminosity(&hi2c1, &broadband, &ir);
		lux = TSL2561_CalculateLux(broadband, ir);
//			int sz=sprintf(buffer, "broadband %u IR %u LUX %lu\r\n", broadband, ir, lux);
		sprintf(Luxinf,"LUX=%d",lux);
		HAL_UART_Transmit(&huart1, (uint8_t*)Luxinf, strlen(Luxinf), HAL_MAX_DELAY);
		
		//SSD1306
//		ssd1306_TestAll();
//		ssd1306_lux(txt);
		ssd1306_showmessage(rgbinf,colorTempinf,Luxinf);
		
		HAL_Delay(500);
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_I2C1;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_HSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
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
  __disable_irq();
  while (1)
  {
  }
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
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

