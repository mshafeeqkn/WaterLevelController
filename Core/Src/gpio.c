/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : gpio.c
  * @brief          : The driver functions related to the GPIO
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
#include "gpio.h"

void set_gpio_dir(stm_gpio_port_t port, uint8_t pin, stm_gpio_dir_t dir) {
}

void set_gpio_val(stm_gpio_port_t port, uint8_t pin, uint8_t val) {
}

uint8_t get_gpio_val(stm_gpio_port_t port, uint8_t pin) {
    return 0;
}
