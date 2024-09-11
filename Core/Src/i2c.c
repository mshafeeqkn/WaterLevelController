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

#define I2C_BUFF_SIZE   8

static i2c_callback_t i2c_cb = NULL;
static uint8_t i2c_buff[I2C_BUFF_SIZE];
static uint8_t i2c_buff_index;
static i2c_mode_t i2c_mode = I2C_MODE_UNKNOWN;

void I2C1_EV_IRQHandler() {
    if(I2C1->SR1 & I2C_SR1_ADDR) {
        // Clear the address detect flag by reading SR1
        // SR2 registers
        (void)I2C1->SR1;
        (void)I2C1->SR2;

        // Reset buffer index to 0
        i2c_buff_index = 0;
        if(I2C1->SR2 & I2C_SR2_TRA) {
            // This is Tx mode
            i2c_mode = I2C_MODE_TX;
            i2c_cb(i2c_buff, I2C_BUFF_SIZE, i2c_mode);
        } else {
            // This is Rx mode
            i2c_mode = I2C_MODE_RX;
        }
    }

    if(I2C_MODE_RX == i2c_mode) {
        if(I2C1->SR1 & I2C_SR1_RXNE) {
            // Store data until buffer is full
            if(i2c_buff_index < I2C_BUFF_SIZE) {
                i2c_buff[i2c_buff_index++] = I2C1->DR;
            }
        } else if(I2C1->SR1 & I2C_SR1_STOPF) {
            // Clear the STOPF flag by reading SR1 and
            // writing into CR1.
            (void)I2C1->SR1;
            I2C1->CR1 |= I2C_CR1_PE;

            // Call the registered callback function
            if(NULL != i2c_cb) {
                i2c_cb(i2c_buff, i2c_buff_index, i2c_mode);
            }
            i2c_mode = I2C_MODE_UNKNOWN;
        }
    } else if(I2C_MODE_TX == i2c_mode) {
        if(I2C1->SR1 & I2C_SR1_TXE) {
            // place each byte in the DR
            I2C1->DR = i2c_buff[i2c_buff_index++];
        } else if (I2C1->SR1 & I2C_SR1_AF) {
            // clear the NACK flag
            I2C1->SR1 &= ~I2C_SR1_AF;
            i2c_mode = I2C_MODE_UNKNOWN;
        }
    }
}

void i2c_slave_init(uint8_t addr, i2c_callback_t callback) {
    // set the callback function
    i2c_cb = callback;

    // Enable clock for port B, I2C and alternate funciton IO
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;

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
