/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : i2c.c
  * @brief          : The driver functions related to the I2C
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
#include <stdlib.h>
#include "i2c.h"

#define I2C_BUFF_SIZE   128

static i2c_callback_t i2c_cb = NULL;
static uint8_t i2c_buff[I2C_BUFF_SIZE];
static uint8_t i2c_buff_index;


void I2C1_EV_IRQHandler() {
    if(I2C1->SR1 & I2C_SR1_ADDR) {
        // Clear the address detect flag by reading SR1
        // SR2 registers
        (void)I2C1->SR1;
        (void)I2C1->SR2;

        // reset buffer index
        i2c_buff_index = 0;
    } else if(I2C1->SR1 & I2C_SR1_RXNE) {
        // Store data until buffer is full
        if(i2c_buff_index < I2C_BUFF_SIZE - 1) {
            i2c_buff[i2c_buff_index++] = I2C1->DR;
        }
    } else if(I2C1->SR1 & I2C_SR1_STOPF) {
        // Clear the STOPF flag by reading SR1 and
        // writing into CR1.
        (void)I2C1->SR1;
        I2C1->CR1 |= I2C_CR1_PE;

        // Append the 0 as last byte
        i2c_buff[i2c_buff_index] = 0;

        // Call the registered callback function
        if(NULL != i2c_cb) {
            i2c_cb(i2c_buff, i2c_buff_index+1);
        }
    }
}

void i2c_slave_init(uint8_t addr, i2c_callback_t callback) {
    // set the callback function
    i2c_cb = callback;

    // Enable clock for port B, I2C and alternate funciton IO
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
    RCC->APB2ENR |= RCC_APB1ENR_I2C1EN;

    // Configure the I2C pins as open-drain output
    GPIOB->CRL |= (GPIO_CRL_CNF6 | GPIO_CRL_MODE6_0);
    GPIOB->CRL |= (GPIO_CRL_CNF7 | GPIO_CRL_MODE7_0);

    // Disable the peripheral enable bit
    I2C1->CR1 &= ~(I2C_CR1_PE);

    // Reset I2C
    I2C1->CR1 |= I2C_CR1_SWRST;
    I2C1->CR1 &= ~I2C_CR1_SWRST;

    // Configure internal clock, raise time
    I2C1->CR2 |= 0x08;
    I2C1->TRISE = 0x9;

    // Clock control register for 100KHz I2C frequency
    // at system clock frequency of 8MHz
    I2C1->CCR = 0x28;

    // Configure the slave address; as per the datasheet
    // bit 14 should be set.
    I2C1->OAR1 = 0x4000 | (addr << 1);

    // Enable the peripheral enable bit and ACK
    I2C1->CR1 |= I2C_CR1_PE;
    I2C1->CR1 |= I2C_CR1_ACK;
    I2C1->CR2 |= I2C_CR2_ITEVTEN;

    uint32_t prioritygroup = NVIC_GetPriorityGrouping();
    uint32_t i2c_priority = NVIC_EncodePriority(prioritygroup, 0, 0);
    NVIC_SetPriority(I2C1_EV_IRQn, i2c_priority);
    NVIC_EnableIRQ(I2C1_EV_IRQn);
}
