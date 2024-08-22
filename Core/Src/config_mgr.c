/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : config_mgr.c
  * @brief          : The driver functions related to the config manager
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
#include "config_mgr.h"
#include "i2c.h"
#include "common.h"

static void on_new_command(uint8_t *data, uint8_t len) {
}

void init_config_mgr() {
    i2c_slave_init(I2C_SLAVE_ADDRESS, on_new_command);
}
