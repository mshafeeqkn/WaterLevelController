/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : pump_tank_monitor.h
  * @brief          : Header for pump_tank_monitor.c file.
  *                   This file contains the funciton to monitor the Tank and Pump
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
#ifndef __PUMP_TANK_MONITOR_H
#define __PUMP_TANK_MONITOR_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdbool.h>
#include "stm32f1xx.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/

typedef enum {
    TANK_LEVEL_0,
    TANK_LEVEL_20,
    TANK_LEVEL_40,
    TANK_LEVEL_60,
    TANK_LEVEL_80,
    TANK_LEVEL_100
} tank_level_t;

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void init_tank_pump_monitor();
tank_level_t get_tank_water_level();
bool is_pump_running_dry();

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN Private defines */
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __PUMP_TANK_MONITOR_H */
