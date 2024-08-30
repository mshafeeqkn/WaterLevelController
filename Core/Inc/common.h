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
#ifndef __COMMON_H
#define __COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

#define     WATER_LEVEL_100_PIN       GPIOB,9
#define     WATER_LEVEL_80_PIN        GPIOB,8
#define     WATER_LEVEL_60_PIN        GPIOB,7
#define     WATER_LEVEL_40_PIN        GPIOB,6
#define     WATER_LEVEL_20_PIN        GPIOB,5

#define     WATER_INFLOW_PIN          GPIOB,4

#define     LED_LEVEL_100_PIN         GPIOA,0
#define     LED_LEVEL_80_PIN          GPIOA,1
#define     LED_LEVEL_60_PIN          GPIOA,2
#define     LED_LEVEL_40_PIN          GPIOA,3
#define     LED_LEVEL_20_PIN          GPIOA,4

#define     LED_DRY_RUN_PIN           GPIOA,5

#define     PUMP_CONTROL_PIN          GPIOB,10
#define     SINGLE_SHOT_PUMP_PIN      GPIOB,0

#define     I2C_SLAVE_ADDRESS         0x28

typedef enum {
    TANK_LEVEL_0,
    TANK_LEVEL_20,
    TANK_LEVEL_40,
    TANK_LEVEL_60,
    TANK_LEVEL_80,
    TANK_LEVEL_100
} tank_level_t;

#ifdef __cplusplus
}
#endif

#endif /* __COMMON_H */
