/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : timers.c
  * @brief          : The driver functions related to the Timers
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include <stdlib.h>
#include "timers.h"

static timer_callback_t timer2_cb = NULL;

void TIM2_IRQHandler() {
    // Clear the flag
    TIM1->SR &= ~(TIM_SR_UIF);

    // Call the callback function if it's already set.
    if(NULL != timer2_cb) {
        timer2_cb();
    }
}

void init_timer_2(uint8_t delay_ms, timer_callback_t callback) {
    // Set the callback function for timer2
    timer2_cb = callback;

    // Enable system clock to TIM2
    RCC->APB2ENR |= RCC_APB1ENR_TIM2EN;

    // Set pre-scalar 8000; so each count will take
    // 1ms delay since we are using 8MHz clock
    TIM2->PSC = 7999;

    // Count 0 - 49 (both inclusive - so total 50)
    TIM2->ARR = 49;

    // Clear the counter register as 0
    TIM2->CNT = 0;

    // Clear the counter reset flag and enable
    // the update interrupt which will trigger
    // on the counter reset.
    TIM2->SR &= ~(TIM_SR_UIF);
    TIM2->DIER |= TIM_DIER_UIE;
    NVIC_EnableIRQ(TIM2_IRQn);

    // Enable the timer.
    TIM2->CR1 |= TIM_CR1_CEN;
}

void set_timer_2_enable(bool enable) {
    if(enable) {
        TIM2->CR1 |= TIM_CR1_CEN;
    } else {
        TIM2->CR1 &= ~TIM_CR1_CEN;
    }
}

