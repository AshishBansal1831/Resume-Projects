/*
 * sd_card.h
 *
 *  Created on: May 29, 2025
 *      Author: Ashish Bansal
 */

#ifndef SD_CARD_H
#define SD_CARD_H

#include <stdint.h>
#include "ff.h"      // For DSTATUS, DRESULT, BYTE, DWORD, UINT types
#include "diskio.h"
// Initialize SD card (SPI mode)
// Returns 0 on failure, 1 on success
uint8_t SD_Init(void);

// Read a single 512-byte block from SD card
// blockAddr = block number (sector), buffer must be at least 512 bytes
// Returns 1 on success, 0 on failure
uint8_t SD_ReadBlock(uint32_t blockAddr, uint8_t *buffer);

// Write a single 512-byte block to SD card
// blockAddr = block number (sector), buffer must be at least 512 bytes
// Returns 1 on success, 0 on failure
uint8_t SD_WriteBlock(uint32_t blockAddr, const uint8_t *buffer);

// FatFs diskio functions
DSTATUS SD_initialize(void);
DRESULT SD_read(BYTE* buff, DWORD sector, UINT count);
DRESULT SD_write(const BYTE* buff, DWORD sector, UINT count);
DRESULT SD_ioctl(BYTE cmd, void* buff);

#endif // SD_CARD_H

