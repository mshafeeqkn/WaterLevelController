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
#include "uart.h"
#include "rtc.h"
#include "water_monitor.h"
#include "indicator.h"
#include "pump_controller.h"
#include "voltage_monitor.h"
#include "config_mgr.h"
#include "flash_ops.h"


#define ST_EVERY_n_SEC_START(n)     {\
                                        static uint8_t __uint##n; \
                                        if(((++__uint##n) % (2 *n)) == 0) { \
                                            __uint##n = 0;
#define ST_EVERY_SEC_END()          }}


#define TURN_ON_LED()            turn_led_on(TURN_ON)
#define TURN_OFF_LED()           turn_led_on(TURN_OFF)
#define TOGGLE_LED()             turn_led_on(TURN_TOGGLE)

static volatile bool calc_line_voltage = false;
static volatile bool stopped_forcefully = false;

#ifdef DEBUG_ENABLED
#endif // DEBUG_ENABLED
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


void SysTick_Handler() {
    voltage_level_t volt_level;
    bool oneshot_run = false;

    tank_level_t level = get_tank_water_level();
    set_water_level(level);

    if(true == is_wlc_automode()) {
        volt_level = get_line_voltage_level();
        oneshot_run = is_oneshot_run();
    } else {
        // Don't monitor the line voltage
        // Fake that we have enough voltage
        volt_level = VOLTAGE_GOOD;
    }

    if((level < TANK_LEVEL_40 && volt_level >= VOLTAGE_OK)) {
        turn_on_water_pump(0);
    }

    if(stopped_forcefully == true && volt_level >= VOLTAGE_OK) {
        turn_on_water_pump(0);
        stopped_forcefully = false;
    }

    if(!oneshot_run && volt_level <= VOLTAGE_LOW && level >= TANK_LEVEL_60) {
        stopped_forcefully = true;
        if(volt_level <= VOLTAGE_LOW) {
            turn_off_water_pump();
        }
    }

    if(volt_level <= VOLTAGE_LOW) {
        set_low_voltage_status();
    } else {
        clear_low_voltage_status();
    }

    if(level < TANK_LEVEL_20) {
        turn_on_water_pump(0);
    }

    // Important: This should be the last condition
    if(level == TANK_LEVEL_100) {
        turn_off_water_pump();
    }

    if(true == is_wlc_automode()) {
        // Run the following block in every sec
        ST_EVERY_n_SEC_START(1);
        decr_pumping_time_btn_count_down();
        ST_EVERY_SEC_END();

        // Run the following block in every 5 sec
        ST_EVERY_n_SEC_START(5);
        calc_line_voltage = true;
        ST_EVERY_SEC_END();
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

static void on_rtc_alarm() {
    turn_on_water_pump(0);
}

int main(void) {
    __disable_irq();

#ifdef DEBUG_ENABLED
#endif
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;

    // Configure PC13 pin as output push-pull maximum speed 10MHz
    GPIOC->CRH &= ~(GPIO_CRH_CNF13 | GPIO_CRH_MODE13);
    GPIOC->CRH |= GPIO_CRH_MODE13_0;
    TURN_OFF_LED();

    // Configure system clock (8MHz crystal with no pre-scaler)
    config_sys_clock();

    // Load saved data
    load_flash_data();

    // Enable the UART for debugging purpose
    uart1_setup(UART_TX_ENABLE);

    // Initialize configuration manager
    init_config_mgr();

    // Initialize the LED & buzzer indicator board
    init_indicators();

    // Initialize the tank and pump monitor module
    init_tank_pump_monitor();

    // Initialize the water pump controller module
    init_water_pump();

    // Intialize the voltage monitor
    init_voltage_monitor();

    // Start the systick timer
    init_systick_timer();

    // Initialize RTC
    init_rtc(on_rtc_alarm);

    // Enable IRQ global flag
    __enable_irq();

    // Run for ever
    while(1) {
        if(calc_line_voltage) {
            calc_line_voltage = false;
            measure_current_voltage(2);
        }
    }
}
