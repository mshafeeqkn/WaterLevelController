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
#include "pump_controller.h"
#include "voltage_monitor.h"
#include "flash_ops.h"
#include "common.h"

#define  SET_RTC_TIME           0x10
#define  SET_PUMPING_TIME       0x20
#define  SET_PUMP_RUN_TIME      0x30
#define  WRITE_TO_FLASH         0x40

#define  GET_RTC_TIME           0x80
#define  GET_PUMPING_TIME       0x90
#define  GET_PUMP_RUN_TIME      0xA0
#define  GET_LINE_VOLTAGE       0xB0

static uint8_t cur_command = 0;

static void on_i2c_event(uint8_t *data, uint8_t len, i2c_mode_t mode) {
    uint32_t i2c_data = 0;

    if(I2C_MODE_RX == mode) {
        cur_command = data[0];
        if(cur_command <= 0x7F) {
            i2c_data = (data[1] | data[2] << 8 | data[3] << 16 | data[4] << 24);
        }
#ifdef DEBUG_ENABLED
        uart1_send_string("I2C command: %02X: data: %u\r\n", cur_command, i2c_data);
#endif
        switch(cur_command) {
            case SET_RTC_TIME:
                set_rtc_time(i2c_data);
                break;

            case SET_PUMPING_TIME:
                set_rtc_alarm_time(i2c_data);
                break;

            case SET_PUMP_RUN_TIME:
                set_one_shot_pumping_time(i2c_data);
                break;

            case WRITE_TO_FLASH:
                save_flash_data();
                break;
        }
    } else if(I2C_MODE_TX == mode) {
        switch(cur_command) {
            case GET_RTC_TIME:
                i2c_data = get_rtc_time();
                break;

            case GET_PUMPING_TIME:
                i2c_data = get_rtc_alarm_time();
                break;

            case GET_PUMP_RUN_TIME:
                i2c_data = get_one_shot_pumping_time();
                break;

            case GET_LINE_VOLTAGE:
                i2c_data = measure_current_voltage(5);
                break;
        }
#ifdef DEBUG_ENABLED
        uart1_send_string("i2c_data = %x\r\n", i2c_data);
#endif
        // Set the data to be sent to 'data' variable
        *(uint32_t*)data = i2c_data;
    }
}

void init_config_mgr() {
    i2c_slave_init(I2C_SLAVE_ADDRESS, on_i2c_event);
}
