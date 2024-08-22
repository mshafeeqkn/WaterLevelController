/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : rtc.c
  * @brief          : The driver functions related to the RTC
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
#include "rtc.h"


static alarm_callback_t alarm_cb = NULL;

void RTC_Alarm_IRQHandler() {
    // Clear alarm flag
    RTC->CRL &= ~RTC_CRL_ALRF;

    if(alarm_cb != NULL) {
        return alarm_cb();
    }
}

void init_rtc(alarm_callback_t callback) {
    // Register the callback function
    alarm_cb = callback;

    // Enable clock for backup and power interface
    RCC->APB1ENR |= RCC_APB1ENR_BKPEN;
    RCC->APB1ENR |= RCC_APB1ENR_PWREN;

    // Reset the backup domian
    RCC->BDCR |= RCC_BDCR_BDRST;
    RCC->BDCR &= ~RCC_BDCR_BDRST;

    // Enable LSE and wait until it's ready
    RCC->BDCR |= RCC_BDCR_LSEON;
    while(!(RCC->BDCR & RCC_BDCR_LSERDY));

    // Select LSE as RTC clock source
    RCC->BDCR |= RCC_BDCR_RTCSEL_0;

    // Disable write protection for the backup domain
    PWR->CR |= PWR_CR_DBP;

    // Wait for the write to be completed if any
    while (!(RTC->CRL & RTC_CRL_RTOFF));

    // Enter RTC configuration mode (This enable writes)
    RTC->CRL |= RTC_CRL_CNF;

    // Set prescaler to 32768 to get 1Hz clock
    RTC->PRLL = 0x7FFF;

    // Exit RTC configuration mode
    RTC->CRL &= ~RTC_CRL_CNF;

    // Wait for the write to be completed
    while (!(RTC->CRL & RTC_CRL_RTOFF));

    // Enable alarm interrupt, low priority
    RTC->CRH |= RTC_CRH_ALRIE;
    uint32_t prioritygroup = NVIC_GetPriorityGrouping();
    uint32_t alarm_priority = NVIC_EncodePriority(prioritygroup, 10, 0);
    NVIC_SetPriority(RTC_Alarm_IRQn, alarm_priority);
    NVIC_EnableIRQ(RTC_Alarm_IRQn);

    // Finally, enable the RTC
    RCC->BDCR |= RCC_BDCR_RTCEN;
}

void set_rtc_time(uint32_t seconds) {
    // Wait until last write operations on RTC
    // registers has finished
    while (!(RTC->CRL & RTC_CRL_RSF));

    // Enter RTC in configuration mode
    RTC->CRL |= RTC_CRL_CNF;

    // Set the counter value
    RTC->CNTH = (seconds >> 16) & 0xFFFF;
    RTC->CNTL = seconds & 0xFFFF;

    // Exit RTC in configuration mode
    RTC->CRL &= ~RTC_CRL_CNF;

    // Wait until last write operations on RTC
    // registers has finished
    while (!(RTC->CRL & RTC_CRL_RSF));

}
