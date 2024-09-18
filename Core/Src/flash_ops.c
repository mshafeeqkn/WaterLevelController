/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : flash_ops.c
  * @brief          : Functions for using the flash as persistant storage
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
#include "flash_ops.h"

volatile static flash_data_t flash_data;

static void flash_unlock(void) {
    FLASH->KEYR = FLASH_KEY1;
    FLASH->KEYR = FLASH_KEY2;
}

static void flash_lock(void) {
    FLASH->CR |= FLASH_CR_LOCK;
}

static void flash_erase_page(uint32_t page_address) {
    // Ensure no operation is ongoing
    while (FLASH->SR & FLASH_SR_BSY);

    FLASH->CR |= FLASH_CR_PER;
    FLASH->AR = page_address;
    FLASH->CR |= FLASH_CR_STRT;

    while (FLASH->SR & FLASH_SR_BSY);

    if (FLASH->SR & FLASH_SR_EOP) {
        FLASH->SR |= FLASH_SR_EOP;
    }

    FLASH->CR &= ~FLASH_CR_PER;
}

static void flash_program_half_word(uint32_t address, uint16_t data) {
    // Ensure no operation is ongoing
    while (FLASH->SR & FLASH_SR_BSY);

    FLASH->CR |= FLASH_CR_PG;
    *(__IO uint16_t*)address = data;

    while (FLASH->SR & FLASH_SR_BSY);

    if (FLASH->SR & FLASH_SR_EOP) {
        FLASH->SR |= FLASH_SR_EOP;
    }

    FLASH->CR &= ~FLASH_CR_PG;
}

static uint32_t read_32bit_data(uint32_t address) {
    uint32_t ret = *(__IO uint16_t*)address;
    ret |= ((*(__IO uint16_t*)(address + 2)) << 16);
    return ret;
}

static void flash_read_struct(uint32_t address, volatile flash_data_t *data) {
    data->alarm_time = read_32bit_data(address);
    data->run_time = read_32bit_data(address+4);
}

static void flash_write_struct(uint32_t address, volatile flash_data_t *data) {
    uint16_t *ptr = (uint16_t *)data;
    uint8_t size = sizeof(flash_data_t);

    // Unlock Flash
    flash_unlock();

    // Erase the page
    flash_erase_page(address);

    // Write the structure data
    for(int i = 0; i < size; i++) {
        flash_program_half_word(address + (i * 2), *(ptr + i));
    }

    // Lock Flash
    flash_lock();
}

void set_flash_data(flash_enum_t elem, void *data) {
    switch(elem) {
        case FE_ALARM_TIME:
            flash_data.alarm_time = *(uint32_t*)data;
            break;

        case FE_RUN_TIME:
            flash_data.run_time = *(uint32_t*)data;
            break;
    }
}

void get_flash_data(flash_enum_t elem, void *data) {
    switch(elem) {
        case FE_ALARM_TIME:
             *(uint32_t*)data = flash_data.alarm_time;
            break;

        case FE_RUN_TIME:
             *(uint32_t*)data = flash_data.run_time;
            break;
    }
}

void load_flash_data() {
    flash_read_struct(PERS_BASE_ADDRESS, &flash_data);
}

void save_flash_data() {
    flash_write_struct(PERS_BASE_ADDRESS, &flash_data);
}
