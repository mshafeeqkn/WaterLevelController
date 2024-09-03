/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : pump_controller.c
  * @brief          : The driver functions related to the water pump
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
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include "voltage_monitor.h"
#include "uart.h"

#define     NUM_INPUT_VOLT_SAMPLE        100
#define     VREF_VOLT                    3.3
#define     ADC_RESOLUTION               4095.0
#define     MULTIPLIER                   700.0

volatile uint16_t adc_buff[NUM_INPUT_VOLT_SAMPLE];

static void timer_1_500us_cc1() {
    // Enable system clock to TIM1
    RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;

    // Set pre-scalar 8; so each count will take
    // 1us delay since we are using 8MHz clock
    TIM1->PSC = 7;

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
}


void init_voltage_monitor() {
    timer_1_500us_cc1();

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
    DMA1_Channel1->CMAR = (uint32_t)adc_buff;     // Memory address (adc_buff)
    // DMA1_Channel1->CNDTR = NUM_INPUT_VOLT_SAMPLE;   // Number of data to transfer (100 samples)
    DMA1_Channel1->CCR |= DMA_CCR_MINC;             // Memory increment mode
    DMA1_Channel1->CCR |= DMA_CCR_PSIZE_0;          // Peripheral size 16-bits
	DMA1_Channel1->CCR |= DMA_CCR_MSIZE_0;          // Memory size 16-bits
    // DMA1_Channel1->CCR |= DMA_CCR_CIRC;            // Enable circular mode
    DMA1_Channel1->CCR |= DMA_CCR_EN;              // Enable DMA Channel 1

    // 5. Start ADC conversion
    ADC1->CR2 |= ADC_CR2_SWSTART; // Start conversion of regular channels
}

static void start_adc_conversion(uint8_t count) {
    DMA1_Channel1->CCR &= ~DMA_CCR_EN;              // Enable DMA Channel 1
    DMA1_Channel1->CNDTR = count;                   // Number of data to transfer (100 samples)
    DMA1_Channel1->CCR |= DMA_CCR_EN;               // Enable DMA Channel 1
}

#if DEBUG_ENABLED // TODO: should be moved from here
void double2str(double value, char* str, int precision) {
    // Handle the sign
    if (value < 0) {
        *str++ = '-';
        value = -value;
    }

    // Extract integer part
    // the int_part should be long long; but here we used
    // long as a work around.
    long int_part = (long)value;
    double frac_part = value - int_part;

    // Convert integer part to string
    sprintf(str, "%ld", int_part);

    // Move the pointer to the end of the integer part
    while (*str != '\0') {
        str++;
    }

    // Add the decimal point
    *str++ = '.';

    // Handle the fractional part
    for (int i = 0; i < precision; i++) {
        frac_part *= 10;
        int digit = (int)frac_part;
        *str++ = '0' + digit;
        frac_part -= digit;
    }

    // Null-terminate the string
    *str = '\0';
}
#endif

uint16_t get_current_voltage(uint8_t repeat) {
    uint16_t i, j = 0;
    uint32_t sum;
    uint32_t sq_sum;
    int16_t ac_voltage;
    uint16_t zero_point;
    double reading_voltage;
    double volt_sum = 0;
#ifdef DEBUG_ENABLED
    // char buff[16];
#endif

    if(0 == repeat)
        return 0;

    // Start timer 500us timer to covnert the ADC in every 500uS
    // See the timer_1_500us_cc1 function for the initialization
    TIM1->CR1 |= TIM_CR1_CEN;
    // Enable the DMA to transfer 'NUM_INPUT_VOLT_SAMPLE' converted
    // data into the memory
    start_adc_conversion(NUM_INPUT_VOLT_SAMPLE);

    while(1) {

        // When NUM_INPUT_VOLT_SAMPLE conversions are done the TCIF1
        // flag will be set int the DMA1_ISR
        if(DMA1->ISR & DMA_ISR_TCIF1) {
            DMA1->IFCR |= DMA_ISR_TCIF1;

            // Calculate the DC value (zero point) from which the AC
            // is oscillating
            sum = 0;
            sq_sum = 0;
            for(i = 0; i < NUM_INPUT_VOLT_SAMPLE; i++) {
                sum += adc_buff[i];
            }
            zero_point = sum / NUM_INPUT_VOLT_SAMPLE;

            // Calculate the RMS value of the AC voltage
            // take the square of each sample taken in 500us delay
            // and devide it with maxium sample. Then convert it to
            // actual ADC voltage
            for(i = 0; i < NUM_INPUT_VOLT_SAMPLE; i++) {
                ac_voltage = adc_buff[i] - zero_point;
                sq_sum += (ac_voltage * ac_voltage);
            }

            reading_voltage = sqrt(sq_sum/NUM_INPUT_VOLT_SAMPLE) * VREF_VOLT * MULTIPLIER / ADC_RESOLUTION;
#ifdef DEBUG_ENABLED
            char buff[16];
            double2str(reading_voltage, buff, 7);
            uart1_send_string("voltage = %u - %s\r\n", (uint16_t)reading_voltage, buff);
#endif
            // Try as many times as requested and take the avarage to 
            // get the actual line voltage
            volt_sum += reading_voltage;

            if(repeat > ++j) {
                start_adc_conversion(NUM_INPUT_VOLT_SAMPLE);
            } else {
                break;
            }

        }
    }
#ifdef DEBUG_ENABLED
    uart1_send_string("Average voltage = %u\r\n", (uint16_t)(volt_sum/repeat));
#endif
    return (uint16_t)(volt_sum/repeat);
}
