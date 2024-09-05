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
#include "timers.h"
#include "led_indicator.h"
#include "pump_tank_monitor.h"
#include "common.h"

#define DRY_RUN_THRESOLD_SEC        20

static uint32_t one_shot_pumping_time_sec = 120;

static void on_single_shot_btn_press() {
    // TODO: It will trigger on startup and automatically
    // start the pump on system reset - need to address
    // this issue
    // turn_on_water_pump(one_shot_pumping_time_sec);
}

void init_water_pump() {
    set_gpio_dir(PUMP_CONTROL_PIN, GPIO_OUTPUT);
    enable_ext_intr(SINGLE_SHOT_PUMP_PIN, on_single_shot_btn_press);
    init_1s_timer_3();
    turn_off_water_pump();
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
        uart1_send_string("Pump status: Dry Run\r\n");
    }

    if(done) {
        turn_off_water_pump();
        seconds = 0;
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
