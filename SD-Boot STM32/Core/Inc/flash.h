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

HAL_StatusTypeDef EraseFlash(const uint32_t , const uint32_t );
HAL_StatusTypeDef write_bank(uint32_t StartAddress, uint8_t *Buffer, uint32_t size);
void disable_write_protection(void);

#endif /* INC_FLASH_H_ */
