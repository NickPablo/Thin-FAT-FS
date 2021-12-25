/*-----------------------------------------------------------------------
/  PFF - Low level disk interface modlue include file    (C)ChaN, 2010
/-----------------------------------------------------------------------*/


#include "tfs.h"

#ifndef _DISKIO_DEFINED
#define _DISKIO_DEFINED

#ifdef __cplusplus
extern "C" {
#endif




/* Status of Disk Functions */
typedef UINT8   DSTATUS;

#define STA_NOINIT      0x01   /* Drive not initialized */
#define STA_NODISK      0x02   /* No medium in the drive */



/*---------------------------------------*/
/* Prototypes for disk control functions */

DSTATUS disk_initialize (void);
DRESULT disk_readp (UINT8*, DWORD, WORD, WORD);
DRESULT disk_writep ( UINT8*, DWORD);


#ifdef __cplusplus
}
#endif

#endif   /* _DISKIO_DEFINED */
