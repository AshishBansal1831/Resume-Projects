/*
 * app_handler.c
 *
 *  Created on: Oct 28, 2025
 *      Author: Ashish Bansal
 */
#include "app_handler.h"
#include "sd_card.h"
#include "flash.h"
#include "fatfs.h"
#include <stdio.h>
#include <string.h>
#include "stm32f4xx.h"  // Needed for SCB and __set_MSP

const uint32_t APP_START_ADDRESS = 0x08008000;
const char *BINARY_FILENAME = "firmw.bin";

extern FATFS fs;
extern FIL file;
extern FRESULT res;
extern UINT bw;

void start_firmware_update()
{
    uint8_t DataBuffer[512] = {0};
    UINT bytesRead = 0;
    uint32_t Offset = 0;

    // Disable write protection before erasing/programming

    // Mount SD filesystem
    if (f_mount(&fs, "", 1) != FR_OK)
    {
        printf("Mount failed!\r\n");
        return;
    }

    // Open firmware binary
    if (f_open(&file, BINARY_FILENAME, FA_READ) != FR_OK)
    {
        printf("File open failed!\r\n");
        f_mount(NULL, "", 1);
        return;
    }

    printf("Erasing flash...\r\n");
    if (EraseFlash(2, 4) != HAL_OK) // Adjust sector count as needed
    {
        printf("Flash erase failed!\r\n");
        f_close(&file);
        f_mount(NULL, "", 1);
        return;
    }
    printf("Erase complete.\r\n");

    FLASH_OBProgramInitTypeDef ob;
    HAL_FLASH_Unlock();
    HAL_FLASH_OB_Unlock();
    HAL_FLASHEx_OBGetConfig(&ob);
    printf("WRP mask = 0x%08lX, WRPState=%lu %d\n", ob.WRPSector, ob.WRPState, ob.OptionType);
    HAL_FLASH_OB_Lock();
    HAL_FLASH_Lock();

    // Program flash in 512-byte chunks
    while (f_read(&file, DataBuffer, sizeof(DataBuffer), &bytesRead) == FR_OK && bytesRead > 0)
    {
        printf("Writing offset 0x%08lX, bytes %u\r\n", Offset, bytesRead);
        if (write_bank(APP_START_ADDRESS + Offset, DataBuffer, bytesRead) != HAL_OK)
        {
            printf("Flash write failed!\r\n");
            break;
        }
        Offset += bytesRead;
    }

    f_close(&file);
    f_mount(NULL, "", 1);

//    printf("Firmware update complete!\r\n");
}
