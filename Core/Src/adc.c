/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : adc.c
  * @brief          : The driver functions related to the ADC
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
#include "adc.h"

#define MAX_NUM_ADC_CHANNELS 		10

static uint8_t num_adc = 0;
static uint16_t adc_data_buff[MAX_NUM_ADC_CHANNELS] = {0};
static uint8_t adc_chan_num[MAX_NUM_ADC_CHANNELS] = {0};

void inacurate_delay(uint32_t ms) {
    for (volatile uint32_t i = 0; i < ms * 1000; ++i) {
        __NOP();  // No operation (compiler barrier)
    }
}

static void setup_adc_dma() {
    // Enable clock for DMA
    RCC->AHBENR |= RCC_AHBENR_DMA1EN;

    // data from peripheral to memory
    DMA1_Channel1->CCR &= ~(DMA_CCR_DIR);

	// Enable circular buffer
    DMA1_Channel1->CCR |= DMA_CCR_CIRC;

    // Peripheral address is always constant
    DMA1_Channel1->CCR &= ~DMA_CCR_PINC;

    // Enable memory address increment
    DMA1_Channel1->CCR |= DMA_CCR_MINC;

	// Peripheral data size 16 bit
    DMA1_Channel1->CCR |= DMA_CCR_PSIZE_0;

	// Memory data size 16 bit
	DMA1_Channel1->CCR |= DMA_CCR_MSIZE_0;

	// Number of data to be transferred
	DMA1_Channel1->CNDTR = num_adc;
	DMA1_Channel1->CPAR = (uint32_t)&ADC1->DR;
	DMA1_Channel1->CMAR = (uint32_t)adc_data_buff;
	DMA1_Channel1->CCR |= DMA_CCR_EN;
}

void init_adc_module() {
    setup_adc_dma();

    // Enable the clock for analog input port and ADC
    // peripheral
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

    ADC1->CR1 |= ADC_CR1_SCAN;              // ADC in scan mode
    ADC1->CR2 |= (ADC_CR2_CONT    |         // Continuous
                  ADC_CR2_DMA     |         // Enable DMA
                  ADC_CR2_EXTTRIG |         // Start conversion on external trigger
                  ADC_CR2_EXTSEL);          // The external trigger is SWSTART

    // Document RM0008 Section 11.3.1
    // The ADC can be powered-on by setting the ADON bit in the
    // ADC_CR2 register. When the ADON bit is set for the first
    // time, it wakes up the ADC from Power Down mode. Conversion
    // starts when ADON bit is set for a second time by software
    // after ADC power-up time
    ADC1->CR2 |= ADC_CR2_ADON;
    inacurate_delay(100);
    ADC1->CR2 |= ADC_CR2_ADON;

    // Caliberate ADC after each power up
    ADC1->CR2 |= ADC_CR2_CAL;
    while(ADC1->CR2 & ADC_CR2_CAL);
}

void enable_adc_channel(uint8_t chan) {
    // chan - 0 based numbering
    num_adc++;
    adc_chan_num[num_adc-1] = chan;

    // Set the number of channels to be converted
    ADC1->SQR1 &= ~ADC_SQR1_L;
    ADC1->SQR1 |= (num_adc << ADC_SQR1_L_Pos);

    // Configure pin as analog input
    if(chan < 8) {
        GPIOA->CRL &= ~(0xF << (chan * 4));
    } else {
        GPIOB->CRL &= ~(0xF << ((chan - 8) * 4));
    }

    // Add the sequence to convert
    if(num_adc <= 6) {
        ADC1->SQR3 |= (chan + 1) << (num_adc - 1);
    } else {
        ADC1->SQR2 |= (chan + 1) << (num_adc - 1);
    }
}

uint16_t get_adc_value(uint8_t chan) {
    uint8_t i;

    for(i = 0; i < MAX_NUM_ADC_CHANNELS; i++) {
        if(adc_chan_num[i] == chan) {
            return adc_data_buff[i];
        }
    }
    return 0;
}
