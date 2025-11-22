/*
 * app_handler.c
 *
 *  Created on: Oct 28, 2025
 *      Author: Ashish Bansal
 */

#include "stdio.h"
#include "string.h"

#include "sd_card.h"
#include "flash.h"

const uint32_t APP_START_ADDRESS = 0x08008000;
const char *BINARY_FILENAME = "new_firmware.bin";

extern FATFS fs;
extern FIL file;
extern FRESULT res;
extern UINT bw;

void start_firmware_update()
{
    // First Load binary chunk from binary file from SD Card
    uint8_t Result = 0;
    uint8_t DataBuffer[512] = {0};
    UINT  NoByteRead = 0;
    uint16_t Offset = 0;

    // Mount filesystem
    uint8_t ret = f_mount(&fs, "", 1);
    printf("ret - %d\r\n", ret);
    if (ret != FR_OK)
    {
        printf("Culd not mount\r\n");

    }
    // Create and open file
    Result = f_open(&file, BINARY_FILENAME, FA_READ);
    printf("res - %d\r\n", Result);
    if (res != FR_OK)
    {
        printf("File open Failed ERROR CODE - %d\r\n", Result);
        f_mount(NULL, "", 1);
        return;
    }

    if( EraseFlash(2, 4) != HAL_OK) //
    {
        printf("Erase Flash Failed\r\n");
        f_mount(NULL, "", 1);
        return;
    }

    while(f_read(&file, DataBuffer, sizeof(DataBuffer), &NoByteRead) == FR_OK && NoByteRead > 0)
    {
        write_bank(APP_START_ADDRESS + Offset, DataBuffer, NoByteRead);
        Offset += NoByteRead;
    }


    f_close(&file);
    // Unmount
    f_mount(NULL, "", 1);
}
