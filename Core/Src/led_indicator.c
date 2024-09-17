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
#include <stdbool.h>
#include "led_indicator.h"
#include "gpio.h"
#include "timers.h"

#define DRY_RUN_HOLD_TIME   20

static volatile uint16_t rep_count = 0;
static volatile uint8_t off_rep_count = 0;
static volatile pump_status_t pump_status = PUMP_OFF;
static volatile uint8_t dry_pump_hold_time = DRY_RUN_HOLD_TIME;
static volatile uint8_t led_state = 0;

#define short_delay()     for(int i = 0; i < 150; i++)

static void on_timer_2_tick(bool done) {
    set_gpio_val(LED_LEVEL_100_PIN, led_state >> 4);
    short_delay();
    set_gpio_val(LED_LEVEL_100_PIN, 0);
    set_gpio_val(LED_LEVEL_80_PIN, led_state >> 3);
    short_delay();
    set_gpio_val(LED_LEVEL_80_PIN, 0);
    set_gpio_val(LED_LEVEL_60_PIN, led_state >> 2);
    short_delay();
    set_gpio_val(LED_LEVEL_60_PIN, 0);
    set_gpio_val(LED_LEVEL_40_PIN, led_state >> 1);
    short_delay();
    set_gpio_val(LED_LEVEL_40_PIN, 0);
    set_gpio_val(LED_LEVEL_20_PIN, led_state >> 0);
    short_delay();
    set_gpio_val(LED_LEVEL_20_PIN, 0);

    if(0 == off_rep_count) {
        set_gpio_val(LED_DRY_RUN_PIN, 0);
    } else {
        if(0 == rep_count) {
            // Turn on at the beginning of count
            set_gpio_val(LED_DRY_RUN_PIN, 1);
        } else if(off_rep_count == rep_count) {
            // Turn of based on the status of motor
            set_gpio_val(LED_DRY_RUN_PIN, 0);
        }
        rep_count++;

        if(rep_count > 300) {
            // This case execute in every 30 * timer 2 tick = 1.5sec
            rep_count = 0;
            dry_pump_hold_time--;

            if(get_pump_status() == PUMP_DRY_RUN) {
                if(dry_pump_hold_time == 0) {
                    set_pump_status(PUMP_OFF);
                    dry_pump_hold_time = DRY_RUN_HOLD_TIME;
                }
            }
        }
    }
}

void init_led_indicators() {
    set_gpio_dir(LED_LEVEL_100_PIN, GPIO_OUTPUT);
    set_gpio_dir(LED_LEVEL_80_PIN, GPIO_OUTPUT);
    set_gpio_dir(LED_LEVEL_60_PIN, GPIO_OUTPUT);
    set_gpio_dir(LED_LEVEL_40_PIN, GPIO_OUTPUT);
    set_gpio_dir(LED_LEVEL_20_PIN, GPIO_OUTPUT);

    set_gpio_dir(LED_DRY_RUN_PIN, GPIO_OUTPUT);
    init_50ms_timer_2();

    // Run timer 2 forever and set status LED as off
    run_timer_2(0, on_timer_2_tick);
    set_pump_status(PUMP_OFF);
}

void set_water_level(tank_level_t level) {
    led_state = 0;
    switch(level) {
        case TANK_LEVEL_100:
            led_state |= 0x10;
        case TANK_LEVEL_80:
            led_state |= 0x08;
        case TANK_LEVEL_60:
            led_state |= 0x04;
        case TANK_LEVEL_40:
            led_state |= 0x02;
        case TANK_LEVEL_20:
            led_state |= 0x01;
        case TANK_LEVEL_0:
            break;
    }
}

pump_status_t get_pump_status() {
    return pump_status;
}

void set_pump_status(pump_status_t status) {
    pump_status = status;
    if(PUMP_OFF == status) {
        off_rep_count = 0;
        // set_timer_2_enable(false);
    } else {
        set_timer_2_enable(true);
        if(PUMP_RUN == status) {
            off_rep_count = 10;
        } else {
            off_rep_count = 150;
        }
    }
}
