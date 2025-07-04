#include "main.h"
#include "system.h"
#include "conversion.h"

#include <stdio.h>

int main(void)
{
#ifdef _MSC_VER
	ConversionTest();

	printf("Nothing to do. Exit...");

	return 0;
#endif

	PepiphInit();

	int sleeptime = 5;
	int nosleep = 1;

#ifdef DISABLE_BLE
	ConversionInit();
#endif
	
	// если нажата кнопка или в режиме отладки
	// TODO - добавить отслеживание состояния пинов SWDIO и SWCLK
	if(HAL_GPIO_ReadPin(B1_GPIO_PORT, B1_PIN) == GPIO_PIN_RESET || nosleep)
	{
		// зажигаем зеленый светодиод и не уходим в сон
		HAL_GPIO_WritePin(GPIOB, LD3_PIN, GPIO_PIN_RESET);
		//sleep = 0;

		// Init code for STM32_BLE
		// при включении BLE - не забыть MX_PKA_Init в PepiphInit
#ifndef DISABLE_BLE
		ConversionInit();

		MX_APPE_Init(NULL);
#endif
		
		while (1) // при нажатой кнопке не запускаем BLE!
		{
			//SystemClock_Config();

			//MX_GPIO_Init();

			//HAL_GPIO_WritePin(GPIOB, LD1_PIN, GPIO_PIN_RESET);

			//HAL_Delay(100);

			//HAL_GPIO_WritePin(GPIOB, LD1_PIN, GPIO_PIN_SET);
			
			//HAL_Delay(2000);

#ifndef DISABLE_BLE
			MX_APPE_Process();
#endif
		}
	}
	else
	{
		// в зависимости от напряжения зажигаем один из светодиодов
		if(vin > 5200)
		{
			HAL_GPIO_WritePin(GPIOB, LD3_PIN, GPIO_PIN_RESET);

			HAL_Delay(50);

			HAL_GPIO_WritePin(GPIOB, LD3_PIN, GPIO_PIN_SET);

			sleeptime = 1;
		}
		else if(vin > 4500)
		{
			HAL_GPIO_WritePin(GPIOB, LD3_PIN, GPIO_PIN_RESET);

			HAL_Delay(10);

			HAL_GPIO_WritePin(GPIOB, LD3_PIN, GPIO_PIN_SET);
			
			sleeptime = 2;
		}
		else if(vin > 4000)
		{
			HAL_GPIO_WritePin(GPIOB, LD2_PIN, GPIO_PIN_RESET);

			HAL_Delay(10);

			HAL_GPIO_WritePin(GPIOB, LD2_PIN, GPIO_PIN_SET);

			sleeptime = 4;
		}		
		else if(vin > 3100)
		{
			HAL_GPIO_WritePin(GPIOB, LD1_PIN, GPIO_PIN_RESET);

			HAL_Delay(10);

			HAL_GPIO_WritePin(GPIOB, LD1_PIN, GPIO_PIN_SET);
			
			sleeptime = 20;
		}
		else
		{
			HAL_GPIO_WritePin(GPIOB, LD1_PIN, GPIO_PIN_RESET);

			HAL_Delay(1);

			HAL_GPIO_WritePin(GPIOB, LD1_PIN, GPIO_PIN_SET);	

			sleeptime = 30;
		}
	}

	while (1)
	{
		SystemEnterStopMode(sleeptime);		
	}
}
