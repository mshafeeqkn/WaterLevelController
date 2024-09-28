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

#include "uart.h"

// Input water level detect pin
#define     WATER_LEVEL_100_PIN       GPIOA,8
#define     WATER_LEVEL_80_PIN        GPIOB,15
#define     WATER_LEVEL_60_PIN        GPIOB,14
#define     WATER_LEVEL_40_PIN        GPIOB,13
#define     WATER_LEVEL_20_PIN        GPIOB,12

// Input water inflow detect pin
#define     WATER_INFLOW_PIN          GPIOB,11

// Output indicator pin
#define     LED_LEVEL_100_PIN         GPIOA,0
#define     LED_LEVEL_80_PIN          GPIOA,1
#define     LED_LEVEL_60_PIN          GPIOA,3
#define     LED_LEVEL_40_PIN          GPIOA,4
#define     LED_LEVEL_20_PIN          GPIOA,5

// Output Pin to show the inflow of water
#define     PUMP_STATUS_PIN           GPIOA,6

// Output pin to control motor
#define     PUMP_CONTROL_PIN          GPIOB,10

// Input pin to trigger oneshot run
#define     SINGLE_SHOT_PUMP_PIN      GPIOB,0

// Input pin to set daily run time
#define     PUMP_ON_TIME_SET_PIN      GPIOB,1

// Input pin to set auto or manual mode
#define     AUTO_MANUAL_PIN           GPIOA,2

// Analog input pin to monitor the line voltage
#define     VOLTAGE_MONITOR_PIN       /*GPIOA,*/7

// LED to show low voltage in auto mode
#define     LOW_VOLTAGE_INDIC_PIN     GPIOA,11

// Buzzer pin
#define     BUZZER_PIN                GPIOA,12

#define     I2C_SLAVE_ADDRESS         0x28

#define     THRESOLD_VOLTAGE_LEVEL    200

// I2C pins
#define     I2C_SDA_PIN               PORTB,6
#define     I2C_SCL_PIN               PORTB,7

// Serial port pins
#define     UART_RX_PIN               PORTA,10
#define     UART_TX_PIN               PORTA,9

// How many second the pump should keep the
// dry run status.
#define     DRY_RUN_HOLD_TIME         20

// How many seconds the device should detect
// to set the dry run status
#define     DRY_RUN_THRESOLD_SEC      20

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
