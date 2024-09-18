/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : flash_ops.h
  * @brief          : Header for flash_ops.c file
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
#ifndef __FLASH_OPS_H
#define __FLASH_OPS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx.h"

#define PERS_BASE_ADDRESS 	0x08007800
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
typedef struct {
    uint32_t alarm_time;
    uint32_t run_time;
} flash_data_t;

typedef enum {
    FE_ALARM_TIME,
    FE_RUN_TIME
} flash_enum_t;

/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/

void load_flash_data();
void save_flash_data();
void get_flash_data(flash_enum_t elem, void *data);
void set_flash_data(flash_enum_t elem, void *data);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __FLASH_OPS_H */
