#include "main.h"

PKA_HandleTypeDef hpka;

RTC_HandleTypeDef hrtc;

volatile unsigned long SystickHigh;

#ifdef  USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line)
{

}
#endif /* USE_FULL_ASSERT */

void __ARM_argv_veneer(void)
{

}

void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE | RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.LSEState = RCC_LSE_ON;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}

	/** Configure the SYSCLKSource and SYSCLKDivider
	*/
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_RC64MPLL;
	RCC_ClkInitStruct.SYSCLKDivider = RCC_RC64MPLL_DIV2;

	if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_WAIT_STATES_0) != HAL_OK)
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
	RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = { 0 };

	/** Initializes the peripherals clock
	*/
	PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_SMPS;
	PeriphClkInitStruct.SmpsDivSelection = RCC_SMPSCLK_DIV4;

	if(HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
	{
		Error_Handler();
	}
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

	RADIO_HandleTypeDef hradio = { 0 };

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

	RADIO_TIMER_InitTypeDef RADIO_TIMER_InitStruct = { 0 };

	/* USER CODE BEGIN RADIO_TIMER_Init 1 */

	/* USER CODE END RADIO_TIMER_Init 1 */

	if(__HAL_RCC_RADIO_IS_CLK_DISABLED())
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
  * @brief PKA Initialization Function
  * @param None
  * @retval None
  */
static void MX_PKA_Init(void)
{

	/* USER CODE BEGIN PKA_Init 0 */

	/* USER CODE END PKA_Init 0 */

	/* USER CODE BEGIN PKA_Init 1 */

	/* USER CODE END PKA_Init 1 */
	hpka.Instance = PKA;
	if(HAL_PKA_Init(&hpka) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN PKA_Init 2 */

	/* USER CODE END PKA_Init 2 */

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
	if(HAL_RTC_Init(&hrtc) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN RTC_Init 2 */
	/** Enable the WakeUp
	*/
	if(HAL_RTCEx_SetWakeUpTimer(&hrtc, 8192, RTC_WAKEUPCLOCK_RTCCLK_DIV16) != HAL_OK)
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
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };
	/* USER CODE BEGIN MX_GPIO_Init_1 */
	/* USER CODE END MX_GPIO_Init_1 */

	  /* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOB, LD3_Pin | LD1_Pin | LD2_Pin, GPIO_PIN_SET);

	/*Configure GPIO pins : LD3_Pin LD1_Pin LD2_Pin */
	GPIO_InitStruct.Pin = LD3_Pin | LD1_Pin | LD2_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	/*Configure GPIO pin : BTN1_Pin */
	GPIO_InitStruct.Pin = BTN1_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(BTN1_GPIO_Port, &GPIO_InitStruct);

	RT_DEBUG_GPIO_Init();

	//  /*Configure GPIO pin : PA2 */
	//  GPIO_InitStruct.Pin = GPIO_PIN_2 | GPIO_PIN_3;
	//  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	//  GPIO_InitStruct.Pull = GPIO_PULLUP;
	//  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	//  GPIO_InitStruct.Alternate = GPIO_AF7_SWDIO;
	//  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	  ///*Configure GPIO pin : PA10 */
	  //GPIO_InitStruct.Pin = GPIO_PIN_10;
	  //GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	  //GPIO_InitStruct.Pull = GPIO_NOPULL;
	  //GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	  //GPIO_InitStruct.Alternate = GPIO_AF1_LCO;
	  //HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void Error_Handler(void)
{
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();

	MX_GPIO_Init();

	while(1)
	{
		HAL_GPIO_WritePin(GPIOB, LD3_Pin | LD1_Pin | LD2_Pin, GPIO_PIN_RESET);

		for(int i = 0; i < 10000; i++) {}

		HAL_GPIO_WritePin(GPIOB, LD3_Pin | LD1_Pin | LD2_Pin, GPIO_PIN_SET);

		for(int i = 0; i < 500000; i++) {}
	}
	/* USER CODE END Error_Handler_Debug */
}

void PepiphInit(void)
{
	HAL_Init();

	SystemClock_Config();

	PeriphCommonClock_Config();

	MX_GPIO_Init();
	//MX_RADIO_Init();	
	//MX_RADIO_TIMER_Init();	
	//MX_PKA_Init();
	//MX_RTC_Init();
}

void SysTick_Handler(void)
{
	HAL_IncTick();

	SystickHigh += SysTick->LOAD + 1;
}