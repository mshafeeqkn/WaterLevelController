/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : gpio.h
  * @brief          : Header for gpio.c file.
  *                   This file contains the defines of the GPIO driver functions.
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
#ifndef __GPIO_H
#define __GPIO_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/

typedef enum {
    GPIO_INPUT,
    GPIO_OUTPUT
} stm_gpio_dir_t;

typedef GPIO_TypeDef*   stm_gpio_port_t;
typedef void (*ext_intr_callback_t)();

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void set_gpio_dir(stm_gpio_port_t port, uint8_t pin, stm_gpio_dir_t dir);
void set_gpio_val(stm_gpio_port_t port, uint8_t pin, uint8_t val);
uint8_t get_gpio_val(stm_gpio_port_t port, uint8_t pin);
void enable_ext_intr(stm_gpio_port_t port, uint8_t pin, ext_intr_callback_t cb);

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN Private defines */
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __GPIO_H */
