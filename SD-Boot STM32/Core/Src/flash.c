/*
 * flash.c
 *
 *  Created on: Oct 28, 2025
 *      Author: Ashish Bansal
 */

#include "flash.h"
#include "string.h"

HAL_StatusTypeDef write_bank(const uint32_t StartAddress, const uint8_t *Buffer, const uint8_t size)
{
    HAL_StatusTypeDef status = HAL_OK;
    uint32_t address = StartAddress;
    uint64_t data64 = 0;

    HAL_FLASH_Unlock();

    for (uint32_t i = 0; i < size; i += sizeof(uint64_t))
    {
        memcpy(&data64, Buffer + i, sizeof(uint64_t));
        status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, address, data64);
        if (status != HAL_OK)
        {
            break;
        }
        address += 8;
    }

    HAL_FLASH_Lock();
    return status;
}

HAL_StatusTypeDef EraseFlash(const uint32_t StartBank, const uint8_t no_of_banks)
{
    FLASH_EraseInitTypeDef eraseInitStruct;
    uint32_t sectorError;

    HAL_FLASH_Unlock();

    eraseInitStruct.TypeErase = FLASH_TYPEERASE_SECTORS;
    eraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3;
    eraseInitStruct.Sector = StartBank;  // Adjust according to your app start
    eraseInitStruct.NbSectors = no_of_banks;            // Erase till end of flash

    if (HAL_FLASHEx_Erase(&eraseInitStruct, &sectorError) != HAL_OK)
    {
        return HAL_ERROR;
    }

    HAL_FLASH_Lock();
    return HAL_OK;
}
