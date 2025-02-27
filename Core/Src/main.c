#include "main.h"
#include "system.h"
#include "conversion.h"

#include <stdio.h>

static void MX_GPIO_Deinit(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PA0, PWR_WUP_FALLEDG);

	/*Configure GPIO pins : LD3_Pin LD1_Pin LD2_Pin */
	GPIO_InitStruct.Pin = LD3_Pin|LD1_Pin|LD2_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_15| GPIO_PIN_14 | GPIO_PIN_7 | GPIO_PIN_6;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_1 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	LL_PWR_SetNoPullA(LL_PWR_GPIO_BIT_1  |  
		              LL_PWR_GPIO_BIT_8  | 
		              LL_PWR_GPIO_BIT_9  |
		              LL_PWR_GPIO_BIT_10 | 
		              LL_PWR_GPIO_BIT_11 );

	//LL_PWR_EnableGPIOPullUp(LL_PWR_GPIO_A, LL_PWR_GPIO_BIT_2 | LL_PWR_GPIO_BIT_3);

	LL_PWR_SetNoPullB(LL_PWR_GPIO_BIT_0  | 
		              LL_PWR_GPIO_BIT_1  |
		              LL_PWR_GPIO_BIT_2  |  
					  LL_PWR_GPIO_BIT_4  | 
		              LL_PWR_GPIO_BIT_6  |
		              LL_PWR_GPIO_BIT_7  |
		              LL_PWR_GPIO_BIT_14 |
		              LL_PWR_GPIO_BIT_15 );

	HAL_PWREx_EnableGPIORetention();

	__HAL_RCC_GPIOB_CLK_DISABLE();
	__HAL_RCC_GPIOA_CLK_DISABLE();
}

int main(void)
{	
#ifdef _MSC_VER
	printf("Nothing to do. Exit...");

	return 0;
#endif

	PepiphInit();

	ConversionInit();

	int disableBLE = 1;
	
	// если нажата кнопка или в режиме отладки
	// TODO - добавить отслеживание состояния пинов SWDIO и SWCLK
	if(HAL_GPIO_ReadPin(BTN1_GPIO_Port, BTN1_Pin) == GPIO_PIN_RESET || disableBLE)
	{
		// зажигаем зеленый светодиод и не уходим в сон
		HAL_GPIO_WritePin(GPIOB, LD2_Pin, GPIO_PIN_RESET);
		//sleep = 0;
		
		while (1) // при нажатой кнопке не запускаем BLE!
		{
			//SystemClock_Config();

			//MX_GPIO_Init();

			HAL_GPIO_WritePin(GPIOB, LD3_Pin, GPIO_PIN_RESET);

			HAL_Delay(100);

			HAL_GPIO_WritePin(GPIOB, LD3_Pin, GPIO_PIN_SET);
			
			HAL_Delay(2000);	
		}
	}
	else
	{
		HAL_GPIO_WritePin(GPIOB, LD1_Pin, GPIO_PIN_RESET);

		HAL_Delay(100);

		HAL_GPIO_WritePin(GPIOB, LD1_Pin, GPIO_PIN_SET);
	}
	
	// Init code for STM32_BLE
	//MX_APPE_Init(NULL);
	
	while (1)
	{
		//MX_APPE_Process();
	}

	while (1)
	{
		//SystemClock_Config();

		//MX_GPIO_Init();

//		HAL_GPIO_WritePin(GPIOB, LD3_Pin, GPIO_PIN_RESET);

//		HAL_Delay(100);

//		HAL_GPIO_WritePin(GPIOB, LD3_Pin, GPIO_PIN_SET);

//		if(sleep)
//		{
//			MX_GPIO_Deinit();

//			MX_RTC_Init();

//			HAL_PWR_EnterDEEPSTOPMode();

//			//HAL_RTCEx_DeactivateWakeUpTimer(&hrtc);
//		}
//		else
//		{
//			HAL_Delay(2000);
//		}
	}
}
