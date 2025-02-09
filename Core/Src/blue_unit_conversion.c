/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    blue_unit_conversion.s
  * @author  GPM WBL Application Team
  * @brief   blue_unit_conversion implements the conversion from low speed oscillator
  * independent units (STU) to low speed oscillator dependent units (MTU) and
  * vice-versa according to the input parameters.
  * The first parameter is the quantity to be translated expressed in STU or MTU.
  * The second parameter is the frequency or the period.
  * The third parameter is a threshold to switch to 32 bit multiplication.
  * This routine avoids floating divisions exploiting only integer math, maintaining
  * a good rounding error as well.
  * It embeds a long multiplication algorithm suited for this purpose.
  * The threshold passed as parameter allows performing a simple 32bit multiplication
  * instead of long multiplication, saving computational time for small quantities
  * (when it is needed that the conversion is done as fast as possible).
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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

#if defined(__GNUC__) || defined(__clang__)
uint32_t mul32(uint32_t a, uint32_t b) 
{
    uint32_t result = a * b;     // ”множение двух чисел
	
    uint32_t temp = (128 << 13); // Ёквивалент MOVS r2, #128; LSLS r2, r2, #13
	
    temp += result;              // Ёквивалент ADDS r2, r2, r0
	
    temp >>= 21;                 // Ёквивалент LSRS r2, r2, #21
	
    return temp;                 // Ёквивалент MOV r0, r2 и возврат результата
}

uint32_t blue_unit_conversion(uint32_t time, uint32_t period_freq, uint32_t thr) 
{
    if (time <= thr)
        return mul32(time, period_freq);

    uint32_t low1 = period_freq & 0xFFFF;
    uint32_t high1 = period_freq >> 16;
    uint32_t low2 = time & 0xFFFF;
    uint32_t high2 = time >> 16;

    uint32_t mul1 = high2 * high1;
    uint32_t mul2 = low2 * high2;
    uint32_t mul3 = high1 * low2;
    uint32_t mul4 = low2 * low1;

    uint32_t mid = mul2 + mul3;
    uint32_t mid_low = mid << 16;
    uint32_t mid_high = mid >> 16;

    uint32_t result_low = mid_low + mul4;
    uint32_t result_high = mul1 + mid_high + (result_low < mul4);

    uint32_t adjust = (128 << 13);
    uint64_t result = ((uint64_t)result_high << 32) | result_low;
    result += adjust;
    result >>= 21;

    return (result_high << 11) | (uint32_t)result;
}

#endif
