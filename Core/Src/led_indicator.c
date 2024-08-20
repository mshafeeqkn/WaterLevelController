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

void init_led_indicators() {
    set_gpio_dir(LED_LEVEL_100_PIN, GPIO_OUTPUT);
    set_gpio_dir(LED_LEVEL_80_PIN, GPIO_OUTPUT);
    set_gpio_dir(LED_LEVEL_60_PIN, GPIO_OUTPUT);
    set_gpio_dir(LED_LEVEL_40_PIN, GPIO_OUTPUT);
    set_gpio_dir(LED_LEVEL_20_PIN, GPIO_OUTPUT);

    set_gpio_dir(LED_DRY_RUN_PIN, GPIO_OUTPUT);
}
