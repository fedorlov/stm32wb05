#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

//#define DISABLE_BLE

/* Includes ------------------------------------------------------------------*/
#include "stm32wb0x_hal.h"
#include "app_entry.h"
#include "app_common.h"
#include "app_debug.h"

#include "stm32wb0x_nucleo.h"

void Error_Handler(void);
	
extern volatile unsigned long SystickHigh;
	
static inline unsigned long get_global_timer(void)
{
	unsigned long hi;
	unsigned long lo;
	
	do
	{
		hi = SystickHigh;
		lo = SysTick->VAL;
	}while(hi != SystickHigh);
	
	return hi - lo;
}

// дл€ переменных, которые измен€ютс€ в прерывани€х важно чтобы значение переменной
// попадало в регистр до чтени€ таймера - это реализовано с помощью global_timer_delta
static inline unsigned long global_timer_delta(volatile unsigned long* x) { return get_global_timer() - *x; }

#define GLOBAL_TIMER         (get_global_timer())
#define GLOBAL_DELTA_TIME(x) (global_timer_delta(&(x)))

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

//#define MOTHERBOARD_STM

#define NUCLEO_WB05KZ_BLUETOOTH_SENSOR

#ifdef MOTHERBOARD_STM

	// Blue
	#define LD1_GPIO_PORT                      GPIOB
	#define LD1_GPIO_CLK_ENABLE()              __HAL_RCC_GPIOB_CLK_ENABLE()
	#define LD1_PIN                            GPIO_PIN_1
	#define LD_BLUE                            LD1_PIN

	// Green
	#define LD2_GPIO_PORT                      GPIOB
	#define LD2_GPIO_CLK_ENABLE()              __HAL_RCC_GPIOB_CLK_ENABLE()
	#define LD2_PIN                            GPIO_PIN_4
	#define LD_GREEN                           LD3_PIN

	// Red
	#define LD3_GPIO_PORT                      GPIOB
	#define LD3_GPIO_CLK_ENABLE()              __HAL_RCC_GPIOB_CLK_ENABLE()
	#define LD3_PIN                            GPIO_PIN_2
	#define LD_RED                             LD3_PIN

	#define B1_GPIO_PORT                       GPIOA
	#define B1_GPIO_CLK_ENABLE()               __HAL_RCC_GPIOA_CLK_ENABLE()
	#define B1_PIN                             GPIO_PIN_0
	#define B1_EXTI_IRQn                       GPIOA_IRQn
	#define B1_PWR_WAKEUP                      PWR_WAKEUP_PA0

	#define B2_GPIO_PORT                       GPIOB
	#define B2_GPIO_CLK_ENABLE()               __HAL_RCC_GPIOB_CLK_ENABLE()
	#define B2_PIN                             GPIO_PIN_5
	#define B2_EXTI_IRQn                       GPIOB_IRQn	
	#define B2_PWR_WAKEUP                      PWR_WAKEUP_PB5

	#define B3_GPIO_PORT                       GPIOB
	#define B3_GPIO_CLK_ENABLE()               __HAL_RCC_GPIOB_CLK_ENABLE()
	#define B3_PIN                             GPIO_PIN_14
	#define B3_EXTI_IRQn                       GPIOB_IRQn

	// When PB14 is used as input for User Button 3 (USER3), 
	// GPIO MUST be configured by software with a pull-up
	// to avoid destructive damages.

#else

	// Red
	#define LD1_GPIO_PORT                      GPIOB
	#define LD1_GPIO_CLK_ENABLE()              __HAL_RCC_GPIOB_CLK_ENABLE()
	#define LD1_PIN                            GPIO_PIN_15
	#define LD_RED                             LD1_PIN

	// Yellow
	#define LD2_GPIO_PORT                      GPIOB
	#define LD2_GPIO_CLK_ENABLE()              __HAL_RCC_GPIOB_CLK_ENABLE()
	#define LD2_PIN                            GPIO_PIN_4
	#define LD_YELLOW                          LD2_PIN

	// Green
	#define LD3_GPIO_PORT                      GPIOB
	#define LD3_GPIO_CLK_ENABLE()              __HAL_RCC_GPIOB_CLK_ENABLE()
	#define LD3_PIN                            GPIO_PIN_2
	#define LD_GREEN                           LD3_PIN

	#define B1_GPIO_PORT                       GPIOA
	#define B1_GPIO_CLK_ENABLE()               __HAL_RCC_GPIOA_CLK_ENABLE()
	#define B1_PIN                             GPIO_PIN_0
	#define B1_EXTI_IRQn                       GPIOA_IRQn
	#define B1_PWR_WAKEUP                      PWR_WAKEUP_PA0

	#define B2_GPIO_PORT                       GPIOA
	#define B2_GPIO_CLK_ENABLE()               __HAL_RCC_GPIOA_CLK_ENABLE()
	#define B2_PIN                             GPIO_PIN_1
	#define B2_EXTI_IRQn                       GPIOA_IRQn
	#define B2_PWR_WAKEUP                      PWR_WAKEUP_PA1

	#define B3_GPIO_PORT                       GPIOB
	#define B3_GPIO_CLK_ENABLE()               __HAL_RCC_GPIOB_CLK_ENABLE()
	#define B3_PIN                             GPIO_PIN_14
	#define B3_EXTI_IRQn                       GPIOB_IRQn
	#define B3_PWR_WAKEUP                      PWR_WAKEUP_PB14

	// When PB14 is used as input for User Button 3 (USER3), 
	// GPIO MUST be configured by software with a pull-up
	// to avoid destructive damages.

	#define ADC_CURRENT_PORT     GPIOB
	#define ADC_CURRENT_PIN      GPIO_PIN_3
	#define ADC_CURRENT_CH       ADC_CHANNEL_VINP0

	#define ADC_CURRENT_GND_PORT GPIOB
	#define ADC_CURRENT_GND_PIN  GPIO_PIN_0



	#define ADC_VOLTAGE_PORT     GPIOB
	#define ADC_VOLTAGE_PIN      GPIO_PIN_5
	#define ADC_VOLTAGE_CH       ADC_CHANNEL_VINP3

	#define ADC_VOLTAGE_GND_PORT GPIOB
	#define ADC_VOLTAGE_GND_PIN  GPIO_PIN_0



	#define ADC_VREF_TEMP_PORT    GPIOB
	#define ADC_VREF_TEMP_PIN     GPIO_PIN_1
	#define ADC_VREF_TEMP_CH      ADC_CHANNEL_VINP1	

	#define ADC_VREF_GND_PORT     GPIOA
	#define ADC_VREF_GND_PIN      GPIO_PIN_8

	#define ADC_VREF_EN_PORT      GPIOA
	#define ADC_VREF_EN_PIN       GPIO_PIN_9

	#define ADC_TEMP_GND_PORT    GPIOB
	#define ADC_TEMP_GND_PIN     GPIO_PIN_6

	#define ADC_TEMP_VCC_PORT    GPIOB
	#define ADC_TEMP_VCC_PIN     GPIO_PIN_7

#endif

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
