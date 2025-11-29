/*
 * flash.c
 *
 *  Created on: Oct 28, 2025
 *      Author: Ashish Bansal
 */
#include "flash.h"
#include "string.h"
#include "stdio.h"

HAL_StatusTypeDef write_bank(uint32_t Address, uint8_t *Buffer, uint32_t Size)
{
    __disable_irq();
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR |
                           FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);

    HAL_FLASH_Unlock();

    uint32_t data64;

    for (uint32_t i = 0; i < Size; i += 4)
    {
        // Clear previous data
        data64 = UINT32_MAX;

        // Copy remaining bytes, max 8
        uint32_t chunk_size = 4;
        memcpy(&data64, Buffer + i, 4);

        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Address, data64) != HAL_OK)
        {
            printf("HAL_FLASH_Program failed at 0x%08lX (status=%lu)\n",
                   Address, HAL_FLASH_GetError());
            HAL_FLASH_Lock();
            __enable_irq();
            return HAL_ERROR;
        }

        Address += 4;
    }

    HAL_FLASH_Lock();
    __enable_irq();
    return HAL_OK;
}

HAL_StatusTypeDef EraseFlash(uint32_t StartSector, uint32_t SectorCount)
{
    FLASH_EraseInitTypeDef eraseInit;
    uint32_t SectorError;

    eraseInit.TypeErase     = FLASH_TYPEERASE_SECTORS;
    eraseInit.Sector        = StartSector;
    eraseInit.NbSectors     = SectorCount;
    eraseInit.VoltageRange  = FLASH_VOLTAGE_RANGE_3;

    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR |
                           FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);

    HAL_FLASH_Unlock();
    HAL_StatusTypeDef status = HAL_FLASHEx_Erase(&eraseInit, &SectorError);
    HAL_FLASH_Lock();

    if (status != HAL_OK)
        printf("Erase failed at sector %lu\n", SectorError);

    return status;
}

void disable_write_protection(void)
{
    FLASH_OBProgramInitTypeDef obInit;

    HAL_FLASH_Unlock();
    HAL_FLASH_OB_Unlock();

    HAL_FLASHEx_OBGetConfig(&obInit);

    obInit.OptionType = OPTIONBYTE_WRP;
    obInit.WRPState   = OB_WRPSTATE_DISABLE;

    // Disable write protection for sectors 2 onward (where your app is)
    obInit.WRPSector  = OB_WRP_SECTOR_2 | OB_WRP_SECTOR_3 |
                        OB_WRP_SECTOR_4 | OB_WRP_SECTOR_5;

    if (HAL_FLASHEx_OBProgram(&obInit) != HAL_OK)
    {
        printf("Failed to disable write protection!\n");
        return;
    }

//    HAL_FLASH_OB_Launch();  // MCU will reset automatically
}
