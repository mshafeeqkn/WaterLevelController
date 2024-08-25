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
#include "adc.h"
#include "voltage_monitor.h"

#define  VOLT_METER_CHAN            6
#define  VOLT_METER_PRESET_CHAN     7

void init_voltage_monitor() {
    enable_adc_channel(VOLT_METER_CHAN);
    enable_adc_channel(VOLT_METER_PRESET_CHAN);
    init_adc_module();
}

static uint16_t get_adc_volt_meter_val() {
    return get_adc_value(VOLT_METER_CHAN);
}

static uint16_t get_adc_preset_val() {
    return get_adc_value(VOLT_METER_PRESET_CHAN);
}

uint16_t get_current_voltage() {
    return get_adc_volt_meter_val()/get_adc_preset_val();
}
