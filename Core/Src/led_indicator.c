/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : led_indicator.c
  * @brief          : The driver functions related to the LED indicator
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
#include "led_indicator.h"
#include "gpio.h"

static uint8_t on_rep_count  = 0;
static uint8_t off_rep_count = 0;

void init_led_indicators() {
    set_gpio_dir(LED_LEVEL_100_PIN, GPIO_OUTPUT);
    set_gpio_dir(LED_LEVEL_80_PIN, GPIO_OUTPUT);
    set_gpio_dir(LED_LEVEL_60_PIN, GPIO_OUTPUT);
    set_gpio_dir(LED_LEVEL_40_PIN, GPIO_OUTPUT);
    set_gpio_dir(LED_LEVEL_20_PIN, GPIO_OUTPUT);

    set_gpio_dir(LED_DRY_RUN_PIN, GPIO_OUTPUT);

    // Configure timer1 to get 50ms time period to blink
    // the status LED
    // clock freq = 8000000 ~ 0.125us
    // max count = 40000 ~ 5ms
    // prescaler = 10 ~ 50ms
}

static void clear_water_level_indicator() {
    set_gpio_val(LED_LEVEL_100_PIN, 0);
    set_gpio_val(LED_LEVEL_80_PIN, 0);
    set_gpio_val(LED_LEVEL_60_PIN, 0);
    set_gpio_val(LED_LEVEL_40_PIN, 0);
    set_gpio_val(LED_LEVEL_20_PIN, 0);
}

void set_water_level(tank_level_t level) {
    clear_water_level_indicator();
    switch(level) {
        case TANK_LEVEL_100:
            set_gpio_val(LED_LEVEL_100_PIN, 1);
        case TANK_LEVEL_80:
            set_gpio_val(LED_LEVEL_80_PIN, 1);
        case TANK_LEVEL_60:
            set_gpio_val(LED_LEVEL_60_PIN, 1);
        case TANK_LEVEL_40:
            set_gpio_val(LED_LEVEL_40_PIN, 1);
        case TANK_LEVEL_20:
            set_gpio_val(LED_LEVEL_20_PIN, 1);
    }
}

static void set_timer_1_enable(bool enable) {
    // turn on the timer1
}

void set_motor_status(pump_status_t status) {
    if(PUMP_OFF == status) {
        set_timer_1_enable(false);
    } else {
        set_timer_1_enable(true);
        if(PUMP_RUN == status) {
            on_rep_count = 0;
            off_rep_count = 1;
        } else {
            on_rep_count = 0;
            off_rep_count = 4;
        }
}
