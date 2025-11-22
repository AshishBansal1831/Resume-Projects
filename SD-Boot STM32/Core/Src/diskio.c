/*
 * diskio.c
 *
 *  Created on: Jul 3, 2025
 *      Author: Ashish Bansal
 */


/**
 * diskio.c
 * FatFs low-level disk I/O driver for STM32 SPI SD card
 * Maps physical drive 0 to SD card functions in sdcard.c
 */

#include "diskio.h"     // FatFs types
#include "stdint.h"

// Functions implemented in your sdcard.c
extern DSTATUS SD_initialize(void);
extern DRESULT SD_read(BYTE* buff, DWORD sector, UINT count);
extern DRESULT SD_write(const BYTE* buff, DWORD sector, UINT count);
extern DRESULT SD_ioctl(BYTE cmd, void* buff);

// Physical drive number for SD card
#define SD_CARD  0

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/
DSTATUS disk_status(BYTE pdrv)
{
    if (pdrv == SD_CARD)
        return 0;  // Always return OK for simplicity, or cache init state
    return STA_NOINIT;
}

/*-----------------------------------------------------------------------*/
/* Initialize a Drive                                                    */
/*-----------------------------------------------------------------------*/
DSTATUS disk_initialize(BYTE pdrv)
{
    if (pdrv == SD_CARD)
        return SD_initialize();
    return STA_NOINIT;
}

/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/
DRESULT disk_read(BYTE pdrv, BYTE* buff, DWORD sector, UINT count)
{
    if (pdrv == SD_CARD)
        return SD_read(buff, sector, count);
    return RES_PARERR;
}

/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/
DRESULT disk_write(BYTE pdrv, const BYTE* buff, DWORD sector, UINT count)
{
#if _READONLY == 0
    if (pdrv == SD_CARD)
        return SD_write(buff, sector, count);
    return RES_PARERR;
#else
    return RES_WRPRT;
#endif
}

/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/
DRESULT disk_ioctl(BYTE pdrv, BYTE cmd, void* buff)
{
    if (pdrv == SD_CARD)
        return SD_ioctl(cmd, buff);
    return RES_PARERR;
}
