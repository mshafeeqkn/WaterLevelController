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
static timer_callback_t timer3_cb = NULL;

static uint32_t timer2_intr_count = 0;
static uint32_t timer2_rep_count  = 0;
static uint32_t timer3_intr_count = 0;
static uint32_t timer3_rep_count  = 0;

// -----------------------------------------------------------------------------
// Timer 2 related functions - used to get 50ms delay to show the water pump
// status in LED panel.
// -----------------------------------------------------------------------------

void TIM2_IRQHandler() {
    // Clear the flag
    TIM2->SR &= ~(TIM_SR_UIF);

    timer2_intr_count++;

    // Call the callback function if it's already set.
    if(NULL != timer2_cb) {
        timer2_cb(false);
    }

    if(timer2_rep_count != 0 && timer2_intr_count == timer2_rep_count) {
        // Disable the timer once the delay is completed
        set_timer_2_enable(false);
        if(NULL != timer2_cb) {
            timer2_cb(true);
        }

        // Reset values
        timer2_cb = NULL;
        timer2_intr_count = 0;
    }
}

void run_timer_2(uint32_t tick_50ms, timer_callback_t callback) {
    // Set the callback function for timer2
    timer2_cb = callback;
    timer2_rep_count = tick_50ms;

    // Start timer
    set_timer_2_enable(true);
}

void init_50ms_timer_2() {
    // Enable system clock to TIM2
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

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
}

void set_timer_2_enable(bool enable) {
    if(enable) {
        TIM2->CR1 |= TIM_CR1_CEN;
    } else {
        TIM2->CR1 &= ~TIM_CR1_CEN;
    }
}

// -----------------------------------------------------------------------------
// Timer 3 related functions - used to get 1 sec delay. Used to turn the water
// pump on for a particular time
// -----------------------------------------------------------------------------

void TIM3_IRQHandler() {
    // Clear the flag
    TIM3->SR &= ~(TIM_SR_UIF);

    timer3_intr_count++;

    // Call the callback function if it's already set.
    if(NULL != timer3_cb) {
        timer3_cb(false);
    }

    if(timer3_rep_count != 0 && timer3_intr_count == timer3_rep_count) {
        // Disable the timer once the delay is completed
        set_timer_3_enable(false);
        if(NULL != timer3_cb) {
            timer3_cb(true);
        }

        // Reset values
        timer3_cb = NULL;
        timer3_intr_count = 0;
    }
}

void run_timer_3(uint32_t tick_1s, timer_callback_t callback) {
    // Set the callback function for timer3
    timer3_cb = callback;
    timer3_rep_count = tick_1s;

    // Start timer
    set_timer_3_enable(true);
}

void init_1s_timer_3() {
    // Enable system clock to TIM3
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

    // Set pre-scalar 40000; so each count will take
    // 1ms delay since we are using 8MHz clock
    TIM3->PSC = 39999;

    // Count 0 - 199 (both inclusive - so total 200)
    TIM3->ARR = 199;

    // Clear the counter register as 0
    TIM3->CNT = 0;

    // Clear the counter reset flag and enable
    // the update interrupt which will trigger
    // on the counter reset.
    TIM3->SR &= ~(TIM_SR_UIF);
    TIM3->DIER |= TIM_DIER_UIE;
    NVIC_EnableIRQ(TIM3_IRQn);
}

void set_timer_3_enable(bool enable) {
    if(enable) {
        TIM3->CR1 |= TIM_CR1_CEN;
    } else {
        TIM3->CR1 &= ~TIM_CR1_CEN;
    }
}
