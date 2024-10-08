/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : led_indicator.h
  * @brief          : Header for indicator.c file.
  *                   This file contains the defines of function to drive
  *                   the LED and buzzer indicator
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __INDICATOR_H
#define __INDICATOR_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx.h"
#include "common.h"

typedef enum {
    PUMP_RUN,
    PUMP_DRY_RUN,
    PUMP_OFF
} pump_status_t;

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void init_indicators();
void set_pump_status(pump_status_t status);
void set_water_level(tank_level_t level);
pump_status_t get_pump_status();
void set_low_voltage_status();
void clear_low_voltage_status();
void set_buzzer_on(uint8_t count, uint8_t delay);

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN Private defines */
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __INDICATOR_H */
