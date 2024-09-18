/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : pump_tank_monitor.c
  * @brief          : The driver functions related to the Tank & Pump operation
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
#include "water_monitor.h"
#include "gpio.h"


void init_tank_pump_monitor() {
    // Configure water level pins as input
    set_gpio_dir(WATER_LEVEL_100_PIN, GPIO_INPUT);
    set_gpio_dir(WATER_LEVEL_80_PIN, GPIO_INPUT);
    set_gpio_dir(WATER_LEVEL_60_PIN, GPIO_INPUT);
    set_gpio_dir(WATER_LEVEL_40_PIN, GPIO_INPUT);
    set_gpio_dir(WATER_LEVEL_20_PIN, GPIO_INPUT);

    // Configure dry run detection pin as input
    set_gpio_dir(WATER_INFLOW_PIN, GPIO_INPUT);
}

tank_level_t get_tank_water_level() {
    if(get_gpio_val(WATER_LEVEL_100_PIN)) {
        return TANK_LEVEL_100;
    }

    if(get_gpio_val(WATER_LEVEL_80_PIN)) {
        return TANK_LEVEL_80;
    }

    if(get_gpio_val(WATER_LEVEL_60_PIN)) {
        return TANK_LEVEL_60;
    }

    if(get_gpio_val(WATER_LEVEL_40_PIN)) {
        return TANK_LEVEL_40;
    }

    if(get_gpio_val(WATER_LEVEL_20_PIN)) {
        return TANK_LEVEL_20;
    }

    return TANK_LEVEL_0;
}

bool is_water_pumping() {
    return get_gpio_val(WATER_INFLOW_PIN);
}
