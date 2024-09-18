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
#include "flash_ops.h"


static alarm_callback_t alarm_cb = NULL;

void RTC_Alarm_IRQHandler() {
    // Clear alarm flag and external interrupt flag
    RTC->CRL &= ~RTC_CRL_ALRF;
    EXTI->PR |= EXTI_PR_PR17;

    if(alarm_cb != NULL) {
        alarm_cb();
    }
}

void init_rtc(alarm_callback_t callback) {
    uint32_t alarm_time = 0;

    // Register the callback function
    alarm_cb = callback;

    // Enable clock for backup and power interface
    RCC->APB1ENR |= RCC_APB1ENR_BKPEN;
    RCC->APB1ENR |= RCC_APB1ENR_PWREN;

    // Disable write protection for the backup domain
    PWR->CR |= PWR_CR_DBP;

    // Reset the backup domian
    RCC->BDCR |= RCC_BDCR_BDRST;
    RCC->BDCR &= ~RCC_BDCR_BDRST;

    // Enable LSE and wait until it's ready
    RCC->BDCR |= RCC_BDCR_LSEON;
    while(!(RCC->BDCR & RCC_BDCR_LSERDY));

    // Select LSE as RTC clock source
    RCC->BDCR |= RCC_BDCR_RTCSEL_LSE;

    // Finally, enable the RTC
    RCC->BDCR |= RCC_BDCR_RTCEN;

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

    // In STM32 microcontrollers, EXTI Line 17 is dedicated to
    // the RTC alarm event. The EXTI (External Interrupt/Event
    // Controller) allows external lines or internal peripheral
    // events (like the RTC alarm) to generate interrupts or
    // events that the processor can handle.
    EXTI->IMR |= EXTI_IMR_MR17;
    EXTI->EMR &= ~EXTI_EMR_MR17;
    EXTI->RTSR |= EXTI_RTSR_TR17;
    NVIC_SetPriority(RTC_Alarm_IRQn, 0);
    NVIC_EnableIRQ(RTC_Alarm_IRQn);

    set_rtc_time(0);
    get_flash_data(FE_ALARM_TIME, &alarm_time);
    set_rtc_alarm_time(alarm_time);
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

uint32_t get_rtc_time() {
    // Clear the register sync flag and wait until this
    // flag is set to get the fresh data.
    RTC->CRL &= ~RTC_CRL_RSF;
    while(!(RTC->CRL & RTC_CRL_RSF));
    return ((RTC->CNTH << 16) | RTC->CNTL);
}

void set_rtc_alarm_time(uint32_t seconds) {
    // Wait until last write operations on RTC
    // registers has finished
    // while (!(RTC->CRL & RTC_CRL_RSF));
    while (!(RTC->CRL & RTC_CRL_RTOFF));

    // Enter RTC in configuration mode
    RTC->CRL |= RTC_CRL_CNF;

    // Set the counter value
    RTC->ALRH = (seconds >> 16) & 0xFFFF;
    RTC->ALRL = seconds & 0xFFFF;

    // Exit RTC in configuration mode
    RTC->CRL &= ~RTC_CRL_CNF;

    // Wait until last write operations on RTC
    // registers has finished
    // while (!(RTC->CRL & RTC_CRL_RSF));
    while (!(RTC->CRL & RTC_CRL_RTOFF));
    set_flash_data(FE_ALARM_TIME, &seconds);

}

uint32_t get_rtc_alarm_time() {
    // Clear the register sync flag and wait until this
    // flag is set to get the fresh data.
    uint32_t ret;
    get_flash_data(FE_ALARM_TIME, &ret);
    return ret;
}
