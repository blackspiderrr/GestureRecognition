/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "adc.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "string.h"
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
uint32_t Data[8];
float a,b,c,d,e,f,g,h;
float Volt[16];
float temp;
uint8_t dma_end_flag=0;

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
	
#include <math.h>

#define NUM_POINTS 200  // ???????
#define DIMENSIONS 16  // ????????
#define NUM_CLASSES 4  // ????
	
float distance(float* point1, float* point2) {
    float sum = 0.0;
    for (int i = 0; i < DIMENSIONS; i++) {
        sum += pow(point1[i] - point2[i], 2);
    }
    return sqrt(sum);
}

int classify(float point[DIMENSIONS], float centroids[NUM_CLASSES][DIMENSIONS]) {
    float min_dist = distance(point, centroids[0]);
    int label = 0;
    for (int i = 1; i < NUM_CLASSES; i++) {
        float dist = distance(point, centroids[i]);
        if (dist < min_dist) {
            min_dist = dist;
            label = i;
        }
    }
    return label;
}
	
int main(void)
{

  /* USER CODE BEGIN 1 */
  char txbuf[32];
	int len;
	int i;
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
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  HAL_ADCEx_Calibration_Start(&hadc1);
	HAL_TIM_Base_Start(&htim3);
  /* USER CODE END 2 */
  float centroids[4][16]={{ 3.103360, 3.132240, 3.137520, 3.130160, 3.157580, 3.023960, 3.090720, 3.123980, 3.147580, 2.977720, 2.765280, 3.134740, 3.156440, 2.525060, 2.406220, 3.122400},
	{3.145294, 3.105804, 3.145569, 3.131412, 2.881627, 1.655510, 2.642333, 3.126039, 3.145627, 1.923314, 2.162745, 3.111725, 3.132235, 3.014078, 2.147471, 3.053843},
	{3.144745, 3.045596, 2.590085, 3.048000, 3.156447, 2.092915, 2.377468, 2.480170, 3.121894, 2.729213, 1.215213, 3.037511, 2.961213, 1.978681, 2.035085, 2.723426},
	{3.143423, 3.134019, 3.148635, 3.130827, 3.159212, 3.151519, 3.140269, 3.138038, 3.148135, 3.127115, 3.125981, 3.142673, 3.156327, 3.150250, 3.130712, 3.141808}};
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		HAL_GPIO_WritePin(LED0_GPIO_Port,LED0_Pin,0);
		HAL_GPIO_WritePin(LED1_GPIO_Port,LED1_Pin,0);
		HAL_GPIO_WritePin(LED2_GPIO_Port,LED2_Pin,0);
		HAL_GPIO_WritePin(BANK0_GPIO_Port,BANK0_Pin,1);
		HAL_GPIO_WritePin(BANK1_GPIO_Port,BANK1_Pin,0);
		HAL_ADC_Start_DMA(&hadc1,Data,8);
		HAL_Delay(400);
		if(dma_end_flag==1)
		{
			dma_end_flag=0;
			for(i=0;i<8;i++)
			{
				Volt[i]=((float)Data[i])*3.3/4090;
			}
		}
		HAL_GPIO_WritePin(BANK0_GPIO_Port,BANK0_Pin,0);
		HAL_GPIO_WritePin(BANK1_GPIO_Port,BANK1_Pin,1);
		HAL_ADC_Start_DMA(&hadc1,Data,8);
		HAL_Delay(400);
		if(dma_end_flag==1)
		{
			dma_end_flag=0;
			for(i=0;i<8;i++)
			{
				Volt[i+8]=((float)Data[i])*3.3/4090;
			}
		}
		for(i=0;i<16;i++)
		{
			memset(txbuf,0,sizeof(txbuf));
			len = sprintf(txbuf,"IN%d=%1.3f\r\n",i,Volt[i]);
		  if(len > 0)
			  HAL_UART_Transmit_IT(&huart1,(uint8_t*)txbuf,len);
			HAL_Delay(200);
		}
		
		
		int new_label = classify(Volt, centroids);
		//Cluster0 Scissors
		if(new_label == 0) 
		{
			HAL_GPIO_WritePin(LED0_GPIO_Port,LED0_Pin,1);
		}
		//Cluster1 Rock
		if(new_label == 1)
		{
			HAL_GPIO_WritePin(LED1_GPIO_Port,LED1_Pin,1);
		}
		//Cluster2 Paper
		if(new_label == 2)
		{
			HAL_GPIO_WritePin(LED2_GPIO_Port,LED2_Pin,1);
		}
		//Cluster3 Nothing
		if(new_label == 3)
		{
			HAL_GPIO_WritePin(LED0_GPIO_Port,LED0_Pin,1);
			HAL_GPIO_WritePin(LED1_GPIO_Port,LED1_Pin,1);
			HAL_GPIO_WritePin(LED2_GPIO_Port,LED2_Pin,1);
		}
		HAL_Delay(500);
    /* USER CODE END WHILE */

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

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV1;
  RCC_OscInitStruct.PLL.PLLN = 8;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	if(hadc->Instance==ADC1)
	{
		dma_end_flag=1;
	}
}
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