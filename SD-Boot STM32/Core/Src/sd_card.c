/*
 * sdcard.c
 * SD card SPI driver for STM32F4 (HAL-based)
 * Supports SDSC/SDHC cards and FatFs diskio interface
 * Author: Ashish Bansal (Updated)
 */

#include "main.h"
#include "spi.h"
#include "gpio.h"
#include "string.h"
#include "fatfs.h"
#include <stdio.h>

extern SPI_HandleTypeDef hspi1;

#define SD_CS_GPIO_Port GPIOA
#define SD_CS_Pin       GPIO_PIN_4

// SD Commands
#define CMD0   0x40
#define CMD8   0x48
#define CMD17  0x51
#define CMD24  0x58
#define CMD55  0x77
#define CMD58  0x7A
#define ACMD41 0x69

static uint8_t isSDHC = 0;

static inline void SD_CS_LOW(void) {
    HAL_GPIO_WritePin(SD_CS_GPIO_Port, SD_CS_Pin, GPIO_PIN_RESET);
}

static inline void SD_CS_HIGH(void) {
    HAL_GPIO_WritePin(SD_CS_GPIO_Port, SD_CS_Pin, GPIO_PIN_SET);
}

static uint8_t SD_SPI_TxRx(uint8_t data) {
    uint8_t rx;
    HAL_SPI_TransmitReceive(&hspi1, &data, &rx, 1, HAL_MAX_DELAY);
    return rx;
}

static void SD_SendDummyClocks(void) {
    SD_CS_HIGH();
    for (int i = 0; i < 10; i++)
        SD_SPI_TxRx(0xFF);
}

static uint8_t SD_SendCommand(uint8_t cmd, uint32_t arg, uint8_t crc) {
    SD_SPI_TxRx(0xFF); // gap
    SD_CS_LOW();

    SD_SPI_TxRx(cmd);
    SD_SPI_TxRx(arg >> 24);
    SD_SPI_TxRx(arg >> 16);
    SD_SPI_TxRx(arg >> 8);
    SD_SPI_TxRx(arg);
    SD_SPI_TxRx(crc);

    for (int i = 0; i < 10; i++) {
        uint8_t r = SD_SPI_TxRx(0xFF);
        if (!(r & 0x80)) return r;
    }
    return 0xFF;
}

uint8_t SD_Init(void) {
    isSDHC = 0;
    SD_SendDummyClocks();

    printf("command sending\r\n");
    if (SD_SendCommand(CMD0, 0, 0x95) != 0x01)
        return 0;

    printf("command sent\r\n");

    uint8_t r = SD_SendCommand(CMD8, 0x1AA, 0x87);
    if (r == 0x01) {
        for (int i = 0; i < 4; i++) SD_SPI_TxRx(0xFF); // discard

        // Wait for card ready
        while (1) {
            SD_SendCommand(CMD55, 0, 0x01);
            if (SD_SendCommand(ACMD41, 0x40000000, 0x01) == 0x00)
                break;
        }

        if (SD_SendCommand(CMD58, 0, 0x01) == 0x00) {
            uint8_t ocr[4];
            for (int i = 0; i < 4; i++) ocr[i] = SD_SPI_TxRx(0xFF);
            if (ocr[0] & 0x40) isSDHC = 1;
        }
    } else {
        // SDSC
        while (1) {
            SD_SendCommand(CMD55, 0, 0x01);
            if (SD_SendCommand(ACMD41, 0, 0x01) == 0x00)
                break;
        }
    }

    SD_CS_HIGH();
    SD_SPI_TxRx(0xFF);
    return 1;
}

uint8_t SD_ReadBlock(uint32_t blockAddr, uint8_t *buffer) {
    if (!isSDHC) blockAddr <<= 9;

    if (SD_SendCommand(CMD17, blockAddr, 0x01) != 0x00)
        return 0;

    while (SD_SPI_TxRx(0xFF) != 0xFE);

    for (int i = 0; i < 512; i++)
        buffer[i] = SD_SPI_TxRx(0xFF);

    SD_SPI_TxRx(0xFF); // discard CRC
    SD_SPI_TxRx(0xFF);
    SD_CS_HIGH();
    SD_SPI_TxRx(0xFF);
    return 1;
}

uint8_t SD_WriteBlock(uint32_t blockAddr, const uint8_t *buffer) {
    if (!isSDHC) blockAddr <<= 9;

    if (SD_SendCommand(CMD24, blockAddr, 0x01) != 0x00)
        return 0;

    SD_SPI_TxRx(0xFF);
    SD_SPI_TxRx(0xFE); // Start token

    for (int i = 0; i < 512; i++)
        SD_SPI_TxRx(buffer[i]);

    SD_SPI_TxRx(0xFF); // dummy CRC
    SD_SPI_TxRx(0xFF);

    uint8_t resp = SD_SPI_TxRx(0xFF);
    if ((resp & 0x1F) != 0x05)
        return 0;

    while (SD_SPI_TxRx(0xFF) == 0);
    SD_CS_HIGH();
    SD_SPI_TxRx(0xFF);
    return 1;
}

#include "diskio.h"

DSTATUS SD_initialize(void) {
    return SD_Init() ? 0 : STA_NOINIT;
}

DRESULT SD_read(BYTE* buff, DWORD sector, UINT count) {
    for (UINT i = 0; i < count; i++) {
        if (!SD_ReadBlock(sector + i, buff + 512 * i))
            return RES_ERROR;
    }
    return RES_OK;
}

DRESULT SD_write(const BYTE* buff, DWORD sector, UINT count) {
    for (UINT i = 0; i < count; i++) {
        if (!SD_WriteBlock(sector + i, buff + 512 * i))
            return RES_ERROR;
    }
    return RES_OK;
}

DRESULT SD_ioctl(BYTE cmd, void* buff) {
    switch (cmd) {
    case CTRL_SYNC:
        return RES_OK;
    case GET_SECTOR_SIZE:
        *(WORD*)buff = 512;
        return RES_OK;
    case GET_BLOCK_SIZE:
        *(DWORD*)buff = 1;
        return RES_OK;
    case GET_SECTOR_COUNT:
        *(DWORD*)buff = 32768; // example: 16MB card
        return RES_OK;
    default:
        return RES_PARERR;
    }
}
