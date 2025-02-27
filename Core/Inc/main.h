#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

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


#define LD1_Pin       GPIO_PIN_1 // Blue
#define LD1_GPIO_Port GPIOB
#define LD2_Pin       GPIO_PIN_4 // Green
#define LD2_GPIO_Port GPIOB
#define LD3_Pin       GPIO_PIN_2 // Red
#define LD3_GPIO_Port GPIOB

#define BTN1_Pin       GPIO_PIN_0
#define BTN1_GPIO_Port GPIOA
#define BTN2_Pin       GPIO_PIN_5
#define BTN2_GPIO_Port GPIOB
#define BTN3_Pin       GPIO_PIN_14
#define BTN3_GPIO_Port GPIOB

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
