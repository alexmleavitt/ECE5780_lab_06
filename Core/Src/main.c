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
int main(void)
{
 RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	RCC->APB2ENR  |= RCC_APB2ENR_ADCEN;
	RCC->APB1ENR  |= RCC_APB1ENR_DACEN;
  SystemClock_Config();
	
	
	// Configure the leds
	GPIOC->MODER |= (1<<12) | (1<<14) | (1<<16) | (1<<18);
	GPIOC->MODER &= ~((1<<13) | (1<<15) | (1<<17) | (1<<19));
	GPIOC->OTYPER &= ~((1<<6) | (1<<7) | (1<<8) | (1<<9));
	GPIOC->OSPEEDR &= ~((1<<12) | (1<<14) | (1<<16) | (1<<18));
	GPIOC->PUPDR &= ~((1<<12) | (1<<14) | (1<<16) | (1<<18)
									| (1<<13) | (1<<15) | (1<<17) | (1<<19));
	GPIOC->OSPEEDR &= ~((1<<0) | (1<<1));

	GPIOC->ODR &= ~((1<<6) | (1<<7) | (1<<8) | (1<<9));
	
	// Configure PC0 with ADC_IN10
	GPIOC->MODER |= (1<<1) | (1<<0);
	GPIOC->PUPDR &= ~((1<<1) | (1<<0));
	
	ADC1->CFGR1 |= (1<<4);
	ADC1->CFGR1 &= ~(1<<3); //8-bit resolution
	ADC1->CFGR1 |= (1<<13); //continuous conversion mode
	ADC1->CFGR1 &= ~((1<<11) | (1<<10)); //hardware trigger disabled
	
	ADC1->CHSELR |= (1<<10); // Select channel 10
	
	//ADC1->CR &= ~(1<<0); //ADEN = 0
	//ADC1->CFGR1 &= ~(1<<0); //DMAEN = 0
	ADC1->CR |= (1<<31); //ADCAL = 1
	
	while ((ADC1->CR & (1<<31))){}
	
	//ADC1->ISR &= ~(1<<0); //ADRDY = 0
	ADC1->CR |= (1<<0); //ADEN = 1
	
	while (!(ADC1->ISR & (1<<0))){}
	ADC1->CR |= (1<<2); //start
		
	//PA4 is DAC_OUT1
	GPIOA->MODER |= (1<<8) | (1<<9);
	GPIOA->PUPDR &= ~((1<<8) | (1<<9));
		
	DAC1->CR |= (7<<3);	
	DAC1->SWTRIGR |= (1<<0); // software trigger DAC channel 1
	DAC1->CR |= (1<<0); //enable DAC channel 1
	
	
	
  int voltage;
		
	// Triangle Wave: 8-bit, 32 samples/cycle
	const uint8_t triangle_table[32] = {0,15,31,47,63,79,95,111,127,142,158,174,
	190,206,222,238,254,238,222,206,190,174,158,142,127,111,95,79,63,47,31,15};
 
	while(1)
  {
		
		voltage = ADC1->DR;
		if(voltage < 50)
		{
			GPIOC->ODR &= ~((1<<6) | (1<<7) | (1<<8) | (1<<9));
		}
		else if(voltage < 100)
		{
			GPIOC->ODR &= ~((1<<7) | (1<<8) | (1<<9));
			GPIOC->ODR |= (1<<6);
		}
		else if(voltage < 150)
		{
			GPIOC->ODR &= ~((1<<7) | (1<<9));
			GPIOC->ODR |= (1<<6) | (1<<8);
		}
		else if(voltage < 210)
		{
			GPIOC->ODR &= ~((1<<9));
			GPIOC->ODR |= (1<<6) | (1<<7) | (1<<8);
		}
		else
		{
			GPIOC->ODR |= (1<<6) | (1<<7) | (1<<8) | (1<<9);
		}
		
		
		for(int i = 0; i<32; i++)
		{
			DAC1->DHR8R1 = triangle_table[i];
			HAL_Delay(1);
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
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
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
