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

#define  SET_RTC_TIME           0x10
#define  GET_RTC_TIME           0x20
#define  SET_PUMPING_TIME       0x30

static uint8_t cur_command = 0;

static void on_i2c_event(uint8_t *data, uint8_t len, i2c_mode_t mode) {
    uint32_t i2c_data;
    uint8_t i;

    if(I2C_MODE_RX == mode) {
        cur_command = data[0];
        switch(cur_command) {
            case SET_RTC_TIME:
                i2c_data = (data[1] | data[2] << 8 | data[3] << 16 | data[4] << 24);
                set_rtc_time(i2c_data);
                break;

            case SET_PUMPING_TIME:
                i2c_data = (data[1] | data[2] << 8 | data[3] << 16 | data[4] << 24);
                set_rtc_alarm_time(i2c_data);
                break;
        }
    } else if(I2C_MODE_TX == mode) {
        switch(cur_command) {
            case GET_RTC_TIME:
                i2c_data = get_rtc_time();
                i = 0;
                while(i2c_data) {
                    data[i++] = i2c_data & 0xFF;
                    i2c_data >>= 8;
                }
                break;
        }
    }
}

static void on_rtc_alarm() {
}

void init_config_mgr() {
    init_rtc(on_rtc_alarm);
    i2c_slave_init(I2C_SLAVE_ADDRESS, on_i2c_event);
}
