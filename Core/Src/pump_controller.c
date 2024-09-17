/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : pump_controller.c
  * @brief          : The driver functions related to the water pump
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
#include "pump_controller.h"
#include "gpio.h"
#include "rtc.h"
#include "timers.h"
#include "led_indicator.h"
#include "pump_tank_monitor.h"
#include "common.h"

#define DRY_RUN_THRESOLD_SEC                20
#define PUMPING_TIME_BUTTON_COUNT_DOWN      4

static volatile uint32_t one_shot_pumping_time_sec   = 120;
static volatile uint8_t  pumping_time_btn_count_down = 0;
static volatile bool oneshot_run = false;

static void on_single_shot_btn_press() {
    oneshot_run = true;
    turn_on_water_pump(one_shot_pumping_time_sec);
}

static void on_pumping_time_btn_pressed() {
    uint32_t rtc_time;
    if((0 == (GPIOB->IDR & GPIO_IDR_IDR1)) &&
            (1 == pumping_time_btn_count_down) ) {
        // This portion will be executed if the button
        // is pressed and hold for PUMPING_TIME_BUTTON_COUNT_DOWN sec
        pumping_time_btn_count_down = 0;
        rtc_time = get_rtc_time();
        set_rtc_alarm_time(rtc_time);
    }

    if(0 != (GPIOB->IDR & GPIO_IDR_IDR1)) {
        // Load maximum count down when the button is pressed
        pumping_time_btn_count_down = PUMPING_TIME_BUTTON_COUNT_DOWN;
    }
}

void decr_pumping_time_btn_count_down() {
    if(pumping_time_btn_count_down > 1) {
        pumping_time_btn_count_down--;
    }
}

void init_water_pump() {
    set_gpio_dir(PUMP_CONTROL_PIN, GPIO_OUTPUT);
    enable_ext_intr(SINGLE_SHOT_PUMP_PIN, EXTI_FALLING, on_single_shot_btn_press);
    enable_ext_intr(PUMP_ON_TIME_SET_PIN,
            EXTI_FALLING|EXTI_RAISING, on_pumping_time_btn_pressed);
    init_1s_timer_3();
    turn_off_water_pump();
}

bool is_oneshot_run() {
    return oneshot_run;
}

void set_one_shot_pumping_time(uint32_t sec) {
    one_shot_pumping_time_sec = sec;
}

uint32_t get_one_shot_pumping_time() {
    return one_shot_pumping_time_sec;
}

static void on_pumping_tick(bool done) {
    static uint8_t dry_run_sec = 0;
    static uint32_t seconds = 0;

    seconds++;
    if(is_water_pumping()) {
        dry_run_sec = 0;
    } else {
        dry_run_sec++;
    }

    if(dry_run_sec > DRY_RUN_THRESOLD_SEC) {
        set_timer_3_enable(false);
        set_gpio_val(PUMP_CONTROL_PIN, 0);
        set_pump_status(PUMP_DRY_RUN);
        seconds = 0;
        dry_run_sec = 0;
    }

    if(done) {
        turn_off_water_pump();
        seconds = 0;
        oneshot_run = false;
    }
}

void turn_on_water_pump(uint32_t timeout_sec) {
    if(PUMP_DRY_RUN == get_pump_status())
        return;

    set_gpio_val(PUMP_CONTROL_PIN, 1);
    run_timer_3(timeout_sec, on_pumping_tick);
    set_pump_status(PUMP_RUN);
}

void turn_off_water_pump() {
    set_gpio_val(PUMP_CONTROL_PIN, 0);
    set_pump_status(PUMP_OFF);
}
