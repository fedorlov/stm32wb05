#include "main.h"

PKA_HandleTypeDef hpka;

volatile unsigned long SystickHigh;
unsigned long TimClock;

#ifdef  USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line)
{

}
#endif /* USE_FULL_ASSERT */

void __ARM_argv_veneer(void)
{

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
void MX_RTC_Init(int sleeptime)
{
	RTC_HandleTypeDef hrtc = { 0 };

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

	sleeptime = sleeptime * 32768 / 16;

	if(HAL_RTCEx_SetWakeUpTimer(&hrtc, sleeptime, RTC_WAKEUPCLOCK_RTCCLK_DIV16) != HAL_OK)
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
	HAL_GPIO_WritePin(GPIOB, LD3_PIN | LD1_PIN | LD2_PIN, GPIO_PIN_SET);

	/*Configure GPIO pins : LD3_Pin LD1_Pin LD2_Pin */
	GPIO_InitStruct.Pin = LD3_PIN | LD1_PIN | LD2_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	// Подтяжка EN для преобразователя питания
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET);
	GPIO_InitStruct.Pin = GPIO_PIN_11;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	LL_PWR_SetNoPullA(GPIO_PIN_11);

	/*Configure GPIO pin : BTN1_Pin */
	GPIO_InitStruct.Pin = B1_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(B1_GPIO_PORT, &GPIO_InitStruct);

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

void MX_GPIO_Deinit(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };

	HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PA0, PWR_WUP_FALLEDG);

	GPIO_InitStruct.Pin = 0xFFFF & (~(GPIO_PIN_0 | GPIO_PIN_11));
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = 0xFFFF;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

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

	while(1)
	{
		HAL_GPIO_WritePin(GPIOB, LD3_PIN | LD1_PIN | LD2_PIN, GPIO_PIN_RESET);

		for(int i = 0; i < 10000; i++) {}

		HAL_GPIO_WritePin(GPIOB, LD3_PIN | LD1_PIN | LD2_PIN, GPIO_PIN_SET);

		for(int i = 0; i < 500000; i++) {}
	}
	/* USER CODE END Error_Handler_Debug */
}

void SystemClock_Config(uint32_t SYSCLKSource, uint32_t SYSCLKDivider)
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
	RCC_ClkInitStruct.SYSCLKSource = SYSCLKSource;   //RCC_SYSCLKSOURCE_RC64MPLL;
	RCC_ClkInitStruct.SYSCLKDivider = SYSCLKDivider; //RCC_RC64MPLL_DIV2;
	
	if(SYSCLKSource == RCC_SYSCLKSOURCE_RC64MPLL)
	{
		LL_RCC_HSI_Enable();
		__HAL_RCC_RC64MPLL_ENABLE();
	}

	if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_WAIT_STATES_0) != HAL_OK)
	{
		Error_Handler();
	}

	if (SYSCLKSource == RCC_SYSCLKSOURCE_DIRECT_HSE)
	{
		switch(SYSCLKDivider)
		{
			case RCC_DIRECT_HSE_DIV1:  TimClock = SystemCoreClock * 1; break;
			case RCC_DIRECT_HSE_DIV2:  TimClock = SystemCoreClock * 2; break;
			case RCC_DIRECT_HSE_DIV4:  TimClock = SystemCoreClock * 4; break;
			case RCC_DIRECT_HSE_DIV8:  TimClock = SystemCoreClock * 8; break;
			case RCC_DIRECT_HSE_DIV16: TimClock = SystemCoreClock * 16; break;
			case RCC_DIRECT_HSE_DIV32: TimClock = SystemCoreClock * 32; break;

			default: Error_Handler();
		}
	}
	else if (SYSCLKSource == RCC_SYSCLKSOURCE_RC64MPLL)
	{
		switch(SYSCLKDivider)
		{
			case RCC_RC64MPLL_DIV1:  TimClock = SystemCoreClock * 1; break;
			case RCC_RC64MPLL_DIV2:  TimClock = SystemCoreClock * 2; break;
			case RCC_RC64MPLL_DIV4:  TimClock = SystemCoreClock * 4; break;
			case RCC_RC64MPLL_DIV8:  TimClock = SystemCoreClock * 8; break;
			case RCC_RC64MPLL_DIV16: TimClock = SystemCoreClock * 16; break;
			case RCC_RC64MPLL_DIV32: TimClock = SystemCoreClock * 32; break;
			case RCC_RC64MPLL_DIV64: TimClock = SystemCoreClock * 32; break;

			default: Error_Handler();
		}
	}
	else
	{
		Error_Handler();
	}
}

void SystemEnterStopMode(int sleeptime)
{
	MX_GPIO_Deinit();

	SystemClock_Config(RCC_SYSCLKSOURCE_RC64MPLL, RCC_RC64MPLL_DIV2);

	MX_RADIO_Init();
	MX_RADIO_TIMER_Init();

	MX_RTC_Init(sleeptime);

	HAL_PWR_EnterDEEPSTOPMode();
}

#if CFG_LPM_SUPPORTED == 0
void HAL_PWR_WKUPx_Callback(uint32_t WakeupIOs) {}
#endif

void PepiphInit(void)
{
	HAL_Init();
	
#ifdef DISABLE_BLE
	SystemClock_Config(RCC_SYSCLKSOURCE_DIRECT_HSE, RCC_DIRECT_HSE_DIV2);
#else
	SystemClock_Config(RCC_SYSCLKSOURCE_RC64MPLL, RCC_RC64MPLL_DIV1);
#endif

	PeriphCommonClock_Config();

	MX_GPIO_Init();

#ifndef DISABLE_BLE
	RT_DEBUG_GPIO_Init();

	MX_RADIO_Init();

	MX_RADIO_TIMER_Init();

	MX_PKA_Init();
#endif
}

void SysTick_Handler(void)
{
	HAL_IncTick();

	SystickHigh += SysTick->LOAD + 1;
}

