/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : gpio.c
  * @brief          : The driver functions related to the GPIO
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
#include "gpio.h"

static ext_intr_callback_t ext_intr_callback[5] = {0};

void set_gpio_dir(stm_gpio_port_t port, uint8_t pin, stm_gpio_dir_t dir) {
    uint8_t         cr_bit_pos;
    uint8_t         reg_val;
    __IO uint32_t*  gpio_reg;

    // Enable clock for corresponding GPIO port
    switch((uint32_t)port) {
        case GPIOA_BASE:
            RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
            break;

        case GPIOB_BASE:
            RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
            break;

        case GPIOC_BASE:
            RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
            break;

        default:
            return;
    }

    // Calculate the MSB position of pin number
    cr_bit_pos = (pin * 4);

    // Get the register name which is to be used to
    // set the GPIO as input or output
    gpio_reg = (cr_bit_pos >= 32) ? &port->CRH : &port->CRL;

    // If the bit position is greater than or equal
    // to 32, take the mode to get correct bit position
    // for 32 bit width register.
    cr_bit_pos %= 32;

    // Set direction to the pin
    if(GPIO_INPUT == dir) {
        // Input mode with pull-down resistor
        reg_val = 8;
    } else {
        // output with maxim 10MHz with general purpose
        // push-pull mode 0b0001
        reg_val = 1;
    }

    // Set the bit value to the corresponding
    // GPIO register.
    *gpio_reg |= (reg_val << cr_bit_pos);
}

void set_gpio_val(stm_gpio_port_t port, uint8_t pin, uint8_t val) {
    // Set the state of pin the output data register
    if(0 == val) {
        port->ODR &= ~(1 << pin);
    } else {
        port->ODR |= (1 << pin);
    }
}

uint8_t get_gpio_val(stm_gpio_port_t port, uint8_t pin) {
    // Return true if the corresponding bit in the GPIOx_IDR
    // bit is set.
    return ((port->IDR & (1 << pin)) != 0);
}

void EXTI0_IRQHandler() {
    EXTI->PR |= EXTI_PR_PR0;
    if(ext_intr_callback[0]) {
        ext_intr_callback[0]();
    }
}

void EXTI1_IRQHandler() {
    EXTI->PR |= EXTI_PR_PR1;
    if(ext_intr_callback[1]) {
        ext_intr_callback[1]();
    }
}

void EXTI2_IRQHandler() {
    EXTI->PR |= EXTI_PR_PR2;
    if(ext_intr_callback[2]) {
        ext_intr_callback[2]();
    }
}

void EXTI3_IRQHandler() {
    EXTI->PR |= EXTI_PR_PR3;
    if(ext_intr_callback[3]) {
        ext_intr_callback[3]();
    }
}

void EXTI4_IRQHandler() {
    EXTI->PR |= EXTI_PR_PR4;
    if(ext_intr_callback[4]) {
        ext_intr_callback[4]();
    }
}

void enable_ext_intr(stm_gpio_port_t port, uint8_t pin, ext_intr_callback_t cb) {
    uint8_t exticr_reg_conf = 0;
    ext_intr_callback[pin] = cb;

    // Configure the port as input with pull down resistor
    set_gpio_dir(port, pin, GPIO_INPUT);
    switch((uint32_t)port) {
        case GPIOA_BASE:
            exticr_reg_conf = 0;
            break;

        case GPIOB_BASE:
            exticr_reg_conf = 1;
            break;

        case GPIOC_BASE:
            exticr_reg_conf = 2;
            break;

        default:
            return;
    }

    // Tie the given pin to the given interrupt line
    AFIO->EXTICR[pin/4] |= exticr_reg_conf << ((pin % 4) * 4);

    // Trigger on falling edge
    EXTI->RTSR |= (1 << pin);

    // Interrupt masking
    EXTI->IMR |= (1 << pin);

    // Clear the interrupt flag
    EXTI->PR |= (1 << pin);

    // Enable interrupt
    // TODO: handle the interrupt line above 4 as it use
    // same IRQ number
    IRQn_Type irq_num = EXTI0_IRQn + pin;
    uint32_t prioritygroup = NVIC_GetPriorityGrouping();
    NVIC_SetPriority(irq_num, NVIC_EncodePriority(prioritygroup, 10, 0));
    NVIC_EnableIRQ(irq_num);
}
