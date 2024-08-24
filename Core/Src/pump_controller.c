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
#include "pump_controller.h"
#include "gpio.h"
#include "common.h"

void init_water_pump() {
    set_gpio_dir(PUMP_CONTROL_PIN, GPIO_OUTPUT);
    set_gpio_val(PUMP_CONTROL_PIN, 0);
}

void turn_on_water_pump() {
    set_gpio_val(PUMP_CONTROL_PIN, 1);
}

void turn_off_water_pump() {
    set_gpio_val(PUMP_CONTROL_PIN, 0);
}
