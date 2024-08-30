/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "main.h"
#include "pump_tank_monitor.h"
#include "led_indicator.h"
#include "pump_controller.h"
#include "uart.h"
#if 0
#include "config_mgr.h"
#include "voltage_monitor.h"
#endif


#ifdef DEBUG_LED_ENABLED

#define TURN_ON_LED()            turn_led_on(TURN_ON)
#define TURN_OFF_LED()           turn_led_on(TURN_OFF)
#define TOGGLE_LED()             turn_led_on(TURN_TOGGLE)


typedef enum {
    TURN_OFF,
    TURN_ON,
    TURN_TOGGLE
} LedState_t;

void turn_led_on(LedState_t state) {
    if(state == TURN_TOGGLE){
        GPIOC->ODR ^= GPIO_ODR_ODR13;
    } else if(state == TURN_ON) {
        GPIOC->ODR &= ~(GPIO_ODR_ODR13);
    } else {
        GPIOC->ODR |= GPIO_ODR_ODR13;
    }
}

void set_error() {
    TURN_ON_LED();
}

#endif // DEBUG_LED_ENABLED

void SysTick_Handler() {
    tank_level_t level = get_tank_water_level();
    set_water_level(level);
    if(level < TANK_LEVEL_40) {
        turn_on_water_pump(0);
    }

    if(level == TANK_LEVEL_100) {
        turn_off_water_pump();
    }
}
/**
 * @brief Configure the system clock as 8MHz using
 * external crystal oscillator.
 */
static void config_sys_clock() {
    // Enable HSE (High-Speed External) oscillator
    RCC->CR |= RCC_CR_HSEON;
    while ((RCC->CR & RCC_CR_HSERDY) == 0);  // Wait for HSE to be ready

    // Select HSE as the system clock source
    RCC->CFGR &= ~RCC_CFGR_SW;  // Clear SW bits
    RCC->CFGR |= RCC_CFGR_SW_HSE;  // Set SW bits to select HSE as system clock

    // Wait until HSE is used as the system clock source
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSE);
}

static void init_systick_timer() {
    // Clock AHB/8, enable interrupt
    SysTick->CTRL |= (1 << SysTick_CTRL_TICKINT_Pos);
    SysTick->LOAD = 500000-1;
    SysTick->VAL = 0;
    SysTick->CALIB;

    // Enable the counter
    SysTick->CTRL |= (1 << SysTick_CTRL_ENABLE_Pos);
}

/**
  * @brief  The application entry point.
  * @retval int
  */
void delay(uint32_t ms) {
    uint32_t i, j;
    for(j = 0; j < ms; j++) {
        for(i = 0; i < 800; i++) {
            __NOP();
        }
    }
}

int main(void) {
#ifdef DEBUG_LED_ENABLED
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;

    // Configure PC13 pin as output push-pull maximum speed 10MHz
    GPIOC->CRH &= ~(GPIO_CRH_CNF13 | GPIO_CRH_MODE13);
    GPIOC->CRH |= GPIO_CRH_MODE13_0;
    TURN_OFF_LED();
#endif

    __disable_irq();
    config_sys_clock();
    uart1_setup(UART_TX_ENABLE);
    init_led_indicators();
    init_tank_pump_monitor();
    init_water_pump();
    init_systick_timer();
    __enable_irq();

    while(1) {
        uart1_send_string("Sample string\r\n");
        delay(1);
    }
}
