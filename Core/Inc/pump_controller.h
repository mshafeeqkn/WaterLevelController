/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : pump_controller.h
  * @brief          : Header for pump_controller.c file.
  *                   This file contains the defines of function to drive the
  *                   water pump
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
#ifndef __PUMP_CONTROLLER_H
#define __PUMP_CONTROLLER_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdbool.h>
#include "stm32f1xx.h"
#include "common.h"

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
void init_water_pump();
bool is_oneshot_run();
void turn_on_water_pump(uint32_t timeout_sec);
void turn_off_water_pump();
void set_one_shot_pumping_time(uint32_t sec);
uint32_t get_one_shot_pumping_time();
void decr_pumping_time_btn_count_down();
bool is_wlc_automode();

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN Private defines */
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __PUMP_CONTROLLER_H */
