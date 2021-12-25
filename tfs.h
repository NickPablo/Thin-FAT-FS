/*This file is part of Thin FAT File System (TFS)
    
	Copyright (C) 2021, Nick C. Pablo , nickcpablo@yahoo.com
	
    TFS is a free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    TFS is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with TFS.  If not, see <https://www.gnu.org/licenses/>.

*/

#ifndef _TFS_DEFINED
#define _TFS_DEFINED

#define TFSDEBUG 0				/* Switching this ON will consume more flash */
#define DIRBUFF32 1				/* ON uses 32 bytes for Dir entry, 16 bytes otherwise */
#define BUFFERED_FAT 1			/* ON uses 32 bytes buffer for reading FAT, 2 or 4 bytes otherwise */

/* Type definitions compatible with CCS C */
typedef unsigned int16   	UINT;   /* int must be 16-bit or 32-bit */
typedef unsigned char    	UINT8;   /* char must be 8-bit */
typedef unsigned int16      WORD;   /* 16-bit unsigned integer */
typedef unsigned int16      WCHAR;   /* 16-bit unsigned integer */
typedef unsigned int32      DWORD;   /* 32-bit unsigned integer */

#define   CLUST   DWORD
typedef enum {
   RES_OK = 0,       		/* 0: Function succeeded */
   RES_ERROR = 0x80,        /* 0x80: Disk error */
   RES_NOTRDY,       		/* 0x81: Not ready */
   RES_PARERR,        		/* 0x82: Invalid parameter */
   RES_UNSUPPORTED,   		/* 0x83: Unsupported volume/filesystem */
   RES_UNMOUNTABLE   		/* 0x84: partition entry is beyond 1-4 range */
} DRESULT;


/* Status of Disk Functions */
typedef UINT8   DSTATUS;
#define STA_NOINIT      0x01   /* Drive not initialized */
#define STA_NODISK      0x02   /* No medium in the drive */

/* File status flags */
#define   FA_OPENED   0x01
#define   FA_WPRT     0x02
#define   FA_WIP      0x40

/*---------------------------------------*/


/* File function return code (FRESULT) */

typedef enum {
   FR_OK = 0,         	/* 0 */
   FR_DISK_ERR,      	/* 1 */
   FR_NOT_READY,      	/* 2 */
   FR_NO_FILE,         	/* 3 */
   FR_NOT_OPENED,      	/* 4 */
   FR_NOT_ENABLED,      /* 5 */
   FR_NO_FILESYSTEM   	/* 6 */
} FRESULT;


/*    Disk Partition Information       */
struct prt{
    #if TFSDEBUG
			UINT8 stat;
			DWORD pSz;
	#endif
            UINT8 pType;
            DWORD lba;     
            
} partInf;
/*  File system structure  in the boot record */
struct brs{ 
            UINT8 spc;				/*sectors per cluster */
            WORD  res;				/* reserved sectors */
            UINT8 nFat;				/* no. of FAT   	*/
            DWORD hidSect;			/* no . of hidden sectors */
            DWORD numSect;			/* no. of sectors in partition */
            DWORD spf;				/* sectors per FAT */
            DWORD clstDir;			/* starting cluster of 	directory table in the FAT32 data area */
            WORD  rootDirN;			/* no. of root directory entries in FAT16*/
 }TFS;
 
 typedef struct {
   DWORD  fSize;      	/* File size */
   WORD   fDate;      	/* Last modified date */
   WORD   fTime;      	/* Last modified time */
   UINT8  fAttrib;    	/* Attribute */
   UINT8   flag;        /* File status flags */
   char   fName[13];   	/* File name */
   DWORD  sec;    		/* Absolute current sector address*/
   DWORD  sClust;		/* starting cluster in the directory table */
   DWORD  cClust;		/* current cluster during read/write ops */
   DWORD  rwPtr;		/* read/write pointer ; auto update by r/w operations */
} TFSFILE;
/* Prototypes for disk control functions */

DSTATUS disk_initialize (void);
DRESULT disk_readp (UINT8*, DWORD, WORD, WORD);
DRESULT disk_writep ( UINT8*, DWORD);


 enum fat{F16,F32};
 /*   Global Resource   */
 UINT8 FATWIDTH;
 TFSFILE tfsFil;


 
 
/*    MBR specifics     */
#define     pEntry1     446
#define     pEntry2     462
#define     pEntry3     478
#define     pEntry4     494

/*    Partition Type    */
#define FATUNK    0x0    //Unknown or Nothing
#define FAT12     0x1    //12-bit FAT
#define FAT16     0x4    //16-bit FAT (Partition Smallerthan 32MB)
#define FATEX     0x5    //Extended MS-DOS Partition
#define FAT1632   0x6    //16-bit FAT (Partition Largerthan 32MB)
#define FAT32     0xB    //32-bit FAT (Partition Up to2048GB)
#define FAT32EX   0xC    //Same as 0BH, but uses LBA1 13h Extensions
#define FAT1632EX 0xE    //Same as 06H, but uses LBA1 13h Extensions
#define FATEXEX   0xF    //Same as 05H, but uses LBA1 13h Extensions

 
/*
7      6   5   4   3   2   1   0
Reserved   A   D   V   S   H   R  */
#define ARCHIVE   0x20
#define DIRECTORY 0x10
#define VOLID     0x8
#define SYSTEM    0x4
#define HIDDEN    0x2
#define READONLY  0x1



DRESULT tfs_mount(UINT8 );
FRESULT tfs_openFileDir(void);
FRESULT tfs_locateDir(DWORD * );
FRESULT tfs_fOpen(char * );
FRESULT tfs_getFatEntry(DWORD);
FRESULT tfs_clust2Sect(DWORD *, CLUST *);
FRESULT tfs_fSeek(DWORD );
FRESULT tfs_fRead(UINT8 *, UINT  ,UINT * );
FRESULT tfs_fwrite(UINT8* , UINT , UINT* );


/*  Debugging Functions   */

DRESULT getPartInfo(UINT);
DRESULT dumpSector(DWORD );
DRESULT readBootSector(UINT8 , DWORD);
void dumpCluster(void);
#endif
