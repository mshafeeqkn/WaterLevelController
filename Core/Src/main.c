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

#define     NUM_INPUT_VOLT_SAMPLE        40

static void timer_1_500us_cc1() {
    // Enable system clock to TIM1
    RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

    // Set PA8 as alternate function output push-pull
    GPIOA->CRH &= ~(GPIO_CRH_MODE8|GPIO_CRH_CNF8);
    GPIOA->CRH |= GPIO_CRH_MODE8_0;
    GPIOA->CRH |= GPIO_CRH_CNF8_1;

    // Set pre-scalar 8; so each count will take
    // 1us delay since we are using 8MHz clock
    TIM1->PSC = 7999;

    // Count 0 - 499 (both inclusive - so total 500)
    TIM1->ARR = 499;

    // Clear the counter register as 0
    TIM1->CNT = 0;

    // Clear the counter reset flag and enable
    // the update interrupt which will trigger
    // on the counter reset.
    TIM1->SR &= ~(TIM_SR_UIF);
    TIM1->DIER |= TIM_DIER_UIE;
    TIM1->CCR1 = 499;
    TIM1->CCMR1 |= (TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_0);

    // Capture compare enable
    TIM1->CCER |= TIM_CCER_CC1E;

    // Enable PA8 (OC) if corresponding OCxE is set in the CCER register
    TIM1->BDTR |= TIM_BDTR_MOE;

    TIM1->CR1 |= TIM_CR1_CEN;
}

volatile uint16_t ADC_Buffer[NUM_INPUT_VOLT_SAMPLE];

void ADC_DMA_Init(void) {
    // 1. Enable the clock for ADC1 and DMA1
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;  // ADC1 clock enable
    RCC->AHBENR  |= RCC_AHBENR_DMA1EN;   // DMA1 clock enable
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

    // 2. Configure the ADC
    ADC1->SQR3 = 6; // Channel 6 as the 1st conversion in regular sequence

    // 3. Configure ADC1 CR2 register
    // ADC1->CR2 |= ADC_CR2_CONT;    // Continuous conversion mode
    ADC1->CR2 |= ADC_CR2_DMA;     // Enable DMA mode
    ADC1->CR2 |= ADC_CR2_ADON;    // Enable ADC
    ADC1->CR2 |= ADC_CR2_EXTTRIG; // Start conversion on external trigger
    ADC1->CR2 &= ~ADC_CR2_EXTSEL; // The external trigger is TIM1 CC1

    // 4. Configure DMA1 Channel 1 for ADC1
    DMA1_Channel1->CPAR = (uint32_t)&ADC1->DR;      // Peripheral address (ADC data register)
    DMA1_Channel1->CMAR = (uint32_t)ADC_Buffer;     // Memory address (ADC_Buffer)
    DMA1_Channel1->CNDTR = NUM_INPUT_VOLT_SAMPLE;   // Number of data to transfer (100 samples)
    DMA1_Channel1->CCR |= DMA_CCR_MINC;             // Memory increment mode
    DMA1_Channel1->CCR |= DMA_CCR_PSIZE_0;          // Peripheral size 16-bits
	DMA1_Channel1->CCR |= DMA_CCR_MSIZE_0;          // Memory size 16-bits
    // DMA1_Channel1->CCR |= DMA_CCR_CIRC;            // Enable circular mode
    DMA1_Channel1->CCR |= DMA_CCR_EN;              // Enable DMA Channel 1

    // 5. Start ADC conversion
    ADC1->CR2 |= ADC_CR2_SWSTART; // Start conversion of regular channels
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
    timer_1_500us_cc1();
    uart1_setup(UART_TX_ENABLE);
    ADC_DMA_Init();
    __enable_irq();

    while (1) {
        while(DMA1_Channel1->CNDTR)
            uart1_send_string("%u\r\n", DMA1_Channel1->CNDTR);

        if(DMA1->ISR & DMA_ISR_TCIF1) {
            DMA1->IFCR |= DMA_ISR_TCIF1;
            for(uint16_t i = 0; i < NUM_INPUT_VOLT_SAMPLE; i++) {
                uart1_send_string("%u %u\r\n", i, ADC_Buffer[i]);
            }
            uart1_send_string("\r\n");
        }
    }
}
