#include "main.h"
#include "system.h"
#include "conversion.h"

#include <stdio.h>

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
	if(HAL_GPIO_ReadPin(BTN1_GPIO_Port, BTN1_Pin) == GPIO_PIN_RESET)
	{
		// зажигаем зеленый светодиод и не уходим в сон
		HAL_GPIO_WritePin(GPIOB, LD3_Pin, GPIO_PIN_RESET);
		//sleep = 0;
		
		while (1) // при нажатой кнопке не запускаем BLE!
		{
			//SystemClock_Config();

			//MX_GPIO_Init();

			HAL_GPIO_WritePin(GPIOB, LD1_Pin, GPIO_PIN_RESET);

			HAL_Delay(100);

			HAL_GPIO_WritePin(GPIOB, LD1_Pin, GPIO_PIN_SET);
			
			HAL_Delay(2000);	
		}
	}
	else
	{
		HAL_GPIO_WritePin(GPIOB, LD1_Pin, GPIO_PIN_RESET);

		HAL_Delay(50);

		HAL_GPIO_WritePin(GPIOB, LD1_Pin, GPIO_PIN_SET);
	}
	
	// Init code for STM32_BLE
	// при включении BLE - не забыть MX_PKA_Init в PepiphInit
	//MX_APPE_Init(NULL);
	//
	//while (1)
	//{
	//	//MX_APPE_Process();
	//}

	while (1)
	{
		MX_GPIO_Deinit();

		MX_RTC_Init();

		HAL_PWR_EnterDEEPSTOPMode();
	}
}
