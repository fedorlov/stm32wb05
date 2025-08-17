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

	ConversionInit();

	// если нажата кнопка - не запускаем BLE - режим прошивки
	// TODO - добавить отслеживание состояния пинов SWDIO и SWCLK
	if(HAL_GPIO_ReadPin(B1_GPIO_PORT, B1_PIN) == GPIO_PIN_RESET)
	{
		while(1)
		{
			unsigned short leds[] = { LD_GREEN, LD_YELLOW, LD_RED, LD_YELLOW };

			for(int i = 0; i < ARRAY_SIZE(leds); i++)
			{
				HAL_GPIO_WritePin(GPIOB, leds[i], GPIO_PIN_RESET);

				HAL_Delay(500);

				HAL_GPIO_WritePin(GPIOB, leds[i], GPIO_PIN_SET);
			}
		}
	}

	int sleeptime = 5;
	int nosleep = 1;
	
	
	if(HAL_GPIO_ReadPin(B1_GPIO_PORT, B1_PIN) == GPIO_PIN_RESET || nosleep)
	{
		// зажигаем зеленый светодиод и не уходим в сон
		HAL_GPIO_WritePin(GPIOB, LD_YELLOW, GPIO_PIN_RESET);

		MX_APPE_Init(NULL);
		
		while (1)
		{
#ifndef DISABLE_BLE
			 MX_APPE_Process();
#endif

			 // перезагрузка, чтобы при нажатии кнопки можно было прошивать контроллер
			 if(HAL_GPIO_ReadPin(B1_GPIO_PORT, B1_PIN) == GPIO_PIN_RESET)
			 {
				 NVIC_SystemReset();
			 }
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
