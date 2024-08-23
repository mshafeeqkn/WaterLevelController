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
#include "rtc.h"
#include "common.h"

#define  SET_RTC_TIME       0x10

static void on_new_command(uint8_t *data, uint8_t len) {
    uint32_t i2c_data;
    switch(data[0]) {
        case SET_RTC_TIME:
            i2c_data = (data[1] | data[2] << 8 | data[3] << 16 | data[4] << 24);
            set_rtc_time(i2c_data);
            break;
    }
}

static void on_rtc_alarm() {
}

void init_config_mgr() {
    init_rtc(on_rtc_alarm);
    i2c_slave_init(I2C_SLAVE_ADDRESS, on_new_command);
}
