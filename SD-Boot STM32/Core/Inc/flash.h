/*
 * flash.h
 *
 *  Created on: Oct 28, 2025
 *      Author: Ashish Bansal
 */

#ifndef INC_FLASH_H_
#define INC_FLASH_H_

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_flash.h"
#include "stm32f4xx_hal_flash_ex.h"

HAL_StatusTypeDef EraseFlash(const uint32_t StartBank, const uint8_t no_of_banks);
HAL_StatusTypeDef write_bank(const uint32_t StartAddress, const uint8_t *Buffer, const uint8_t size);

#endif /* INC_FLASH_H_ */
