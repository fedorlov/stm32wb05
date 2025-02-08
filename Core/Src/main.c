#include "main.h"

ADC_HandleTypeDef hadc1;

RTC_HandleTypeDef hrtc;

void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE | RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure the SYSCLKSource and SYSCLKDivider
  */
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_RC64MPLL;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_RC64MPLL_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_WAIT_STATES_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief Peripherals Common Clock Configuration
  * @retval None
  */
void PeriphCommonClock_Config(void)
{
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Initializes the peripherals clock
  */
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_SMPS;
  PeriphClkInitStruct.SmpsDivSelection = RCC_SMPSCLK_DIV4;

  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef ConfigChannel = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Common config
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ConversionType = ADC_CONVERSION_WITH_DS;
  hadc1.Init.SequenceLength = 1;
  hadc1.Init.SamplingMode = ADC_SAMPLING_AT_START;
  hadc1.Init.SampleRate = ADC_SAMPLE_RATE_16;
  hadc1.Init.InvertOutputMode = ADC_DATA_INVERT_NONE;
  hadc1.Init.Overrun = ADC_NEW_DATA_IS_LOST;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DownSamplerConfig.DataWidth = ADC_DS_DATA_WIDTH_16_BIT;
  hadc1.Init.DownSamplerConfig.DataRatio = ADC_DS_RATIO_1;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  ConfigChannel.Channel = ADC_CHANNEL_VINP0;
  ConfigChannel.Rank = ADC_RANK_1;
  ConfigChannel.VoltRange = ADC_VIN_RANGE_1V2;
  ConfigChannel.CalibrationPoint.Number = ADC_CALIB_NONE;
  ConfigChannel.CalibrationPoint.Gain = 0x00;
  ConfigChannel.CalibrationPoint.Offset = 0x00;
  if (HAL_ADC_ConfigChannel(&hadc1, &ConfigChannel) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief RADIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_RADIO_Init(void)
{

  /* USER CODE BEGIN RADIO_Init 0 */

  /* USER CODE END RADIO_Init 0 */

  RADIO_HandleTypeDef hradio = {0};

  /* USER CODE BEGIN RADIO_Init 1 */

  /* USER CODE END RADIO_Init 1 */
  hradio.Instance = RADIO;
  HAL_RADIO_Init(&hradio);
  /* USER CODE BEGIN RADIO_Init 2 */

  /* USER CODE END RADIO_Init 2 */

}

/**
  * @brief RADIO_TIMER Initialization Function
  * @param None
  * @retval None
  */
static void MX_RADIO_TIMER_Init(void)
{

  /* USER CODE BEGIN RADIO_TIMER_Init 0 */

  /* USER CODE END RADIO_TIMER_Init 0 */

  RADIO_TIMER_InitTypeDef RADIO_TIMER_InitStruct = {0};

  /* USER CODE BEGIN RADIO_TIMER_Init 1 */

  /* USER CODE END RADIO_TIMER_Init 1 */

  if (__HAL_RCC_RADIO_IS_CLK_DISABLED())
  {
    /* Radio Peripheral reset */
    __HAL_RCC_RADIO_FORCE_RESET();
    __HAL_RCC_RADIO_RELEASE_RESET();

    /* Enable Radio peripheral clock */
    __HAL_RCC_RADIO_CLK_ENABLE();
  }
  /* Wait to be sure that the Radio Timer is active */
  while(LL_RADIO_TIMER_GetAbsoluteTime(WAKEUP) < 0x10);
  RADIO_TIMER_InitStruct.XTAL_StartupTime = 320;
  RADIO_TIMER_InitStruct.enableInitialCalibration = FALSE;
  RADIO_TIMER_InitStruct.periodicCalibrationInterval = 0;
  HAL_RADIO_TIMER_Init(&RADIO_TIMER_InitStruct);
  /* USER CODE BEGIN RADIO_TIMER_Init 2 */

  /* USER CODE END RADIO_TIMER_Init 2 */

}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 0x7F;
  hrtc.Init.SynchPrediv = 0x00F9;  // 32 kHz RC/128 - 1
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */
  /** Enable the WakeUp
  */
  if (HAL_RTCEx_SetWakeUpTimer(&hrtc, 8192, RTC_WAKEUPCLOCK_RTCCLK_DIV16) != HAL_OK)
  {
    Error_Handler();
  }

  HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN_RTC, PWR_WUP_RISIEDG);

  __HAL_PWR_CLEAR_FLAG(PWR_WU_FLAG_ALL);
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LD3_Pin|LD1_Pin|LD2_Pin, GPIO_PIN_SET);

  /*Configure GPIO pins : LD3_Pin LD1_Pin LD2_Pin */
  GPIO_InitStruct.Pin = LD3_Pin|LD1_Pin|LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PA2 */
  GPIO_InitStruct.Pin = GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF7_SWDIO;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PA10 */
  GPIO_InitStruct.Pin = GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF1_LCO;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

static void MX_GPIO_Deinit(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /*Configure GPIO pins : LD3_Pin LD1_Pin LD2_Pin */
  GPIO_InitStruct.Pin = LD3_Pin|LD1_Pin|LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_15|GPIO_PIN_14|GPIO_PIN_7|GPIO_PIN_6;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

   LL_PWR_SetNoPullB(LL_PWR_GPIO_BIT_2|LL_PWR_GPIO_BIT_1|LL_PWR_GPIO_BIT_0|LL_PWR_GPIO_BIT_15
                          |LL_PWR_GPIO_BIT_14|LL_PWR_GPIO_BIT_7|LL_PWR_GPIO_BIT_6|LL_PWR_GPIO_BIT_4);

  /**/
  LL_PWR_SetNoPullA(LL_PWR_GPIO_BIT_1|LL_PWR_GPIO_BIT_0|LL_PWR_GPIO_BIT_2|LL_PWR_GPIO_BIT_8|LL_PWR_GPIO_BIT_9
                          |LL_PWR_GPIO_BIT_10|LL_PWR_GPIO_BIT_11);

  HAL_PWREx_EnableGPIORetention();

  __HAL_RCC_GPIOB_CLK_DISABLE();
  __HAL_RCC_GPIOA_CLK_DISABLE();
}

void Error_Handler(void)
{
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	
	MX_GPIO_Init();
	
	while (1)
	{
		HAL_GPIO_WritePin(GPIOB, LD3_Pin|LD1_Pin|LD2_Pin, GPIO_PIN_RESET);
		
		for(int i = 0; i < 10000; i++) {}
		
		HAL_GPIO_WritePin(GPIOB, LD3_Pin|LD1_Pin|LD2_Pin, GPIO_PIN_SET);
		
		for(int i = 0; i < 500000; i++) {}
	}
	/* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT

void assert_failed(uint8_t *file, uint32_t line)
{

}
#endif /* USE_FULL_ASSERT */

void __ARM_argv_veneer(void)
{

}

int main(void)
{	
	HAL_Init();	

	SystemClock_Config();

	PeriphCommonClock_Config();
	
	MX_GPIO_Init();
	MX_ADC1_Init();	
	MX_RADIO_Init();	
	MX_RADIO_TIMER_Init();	
	MX_RTC_Init();
	
	HAL_GPIO_WritePin(GPIOB, LD1_Pin, GPIO_PIN_RESET);

	HAL_Delay(100);

	HAL_GPIO_WritePin(GPIOB, LD3_Pin|LD1_Pin|LD2_Pin, GPIO_PIN_SET);

	while (1)
	{
		SystemClock_Config();

		MX_GPIO_Init();

		//HAL_GPIO_WritePin(GPIOB, LD3_Pin|LD1_Pin|LD2_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOB, LD3_Pin, GPIO_PIN_RESET);

		HAL_Delay(100);

		HAL_GPIO_WritePin(GPIOB, LD3_Pin|LD1_Pin|LD2_Pin, GPIO_PIN_SET);

		MX_GPIO_Deinit();

		MX_RTC_Init();

		HAL_PWR_EnterDEEPSTOPMode();
		  
		//HAL_Delay(2000);

		HAL_RTCEx_DeactivateWakeUpTimer(&hrtc);
		
		HAL_GPIO_WritePin(GPIOB, LD2_Pin, GPIO_PIN_RESET);

		HAL_Delay(100);

		HAL_GPIO_WritePin(GPIOB, LD3_Pin|LD1_Pin|LD2_Pin, GPIO_PIN_SET);
	}
}
