
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

#include "tfs.h"
#include <stdlib.h>

#if TFSDEBUG
DRESULT getPartInfo(UINT partEntry){
      UINT8 dBuff[16];
      DRESULT tfsRes;
 

  /* need to read sector 14 & extract the last two bytes [30][31] */
  /* read succeeding sectors to get other paretition info */
      tfsRes =  disk_readp(dBuff,0,partEntry,16);    // read partition table entry
      if(tfsRes) return tfsRes;
      
      partInf.pType  = dBuff[4];
      partInf.lba    = (DWORD) dBuff[0xb]<<24|(DWORD) dBuff[0xa]<<16|(DWORD) dBuff[9]<<8|dBuff[8];
	  #if TFSDEBUG
	  partInf.stat   = dBuff[0];
      partInf.pSz    = (DWORD) dBuff[0xf]<<24|(DWORD) dBuff[0xe]<<16|(DWORD) dBuff[0xd]<<8|dBuff[0xc]; //dBuff[0xc];
      
      printf ("[-----------------------]\r\nPartition Entry #");
      switch (partEntry){
      case pEntry1: printf("1 ");break;
      case pEntry2: printf("2 ");break;
      case pEntry3: printf("3 ");break;
      case pEntry4: printf("4 ");break;
      }
      printf ("Information:\r\n");
      printf("Partition Stat:  %2X ",partInf.stat);
      if (partInf.stat != 0x80) printf("[INACTIVE\/UNKNOWN]\r\n"); else printf("[ACTIVE]\r\n");
      printf("Partition type: ");
      switch (partInf.pType){
         default:
         case FATUNK    : printf("Unknown\r\n"); break;  
         case FAT12     : printf("FAT12\r\n"); break;     
         case FAT16     : printf("tfs16 < 32MB\r\n"); break;     
         case FATEX     : printf("ExtDOS\r\n"); break;    
         case FAT1632   : printf("tfs16 > 32MB\r\n"); break;    
         case FAT32     : printf("tfs32\r\n"); break;    
         case FAT32EX   : printf("tfs32_Ext\r\n"); break;    
         case FAT1632EX : printf("tfs16_Ext\r\n"); break;  
         case FATEXEX   : printf("ExtDOS_Ext\r\n"); break;   
                  }
      printf("LBA sector address: %Lu\r\n", partInf.lba);
      printf("Partition Size: %Lu\r\n", partInf.pSz);
      #endif
      return 0;
}
DRESULT readBootSector(UINT8 fatType, DWORD lba){
         UINT8 rBuff[16];
         DRESULT bs;
        if (fatType) { // tfs16
          bs =  disk_readp(rBuff,lba,0,16);    // 
          if (bs) return bs;
          TFS.spc = rBuff[13];
          TFS.res =(WORD) rBuff[15]<<8|rBuff[14];
          #if TFSDEBUG
          printf("Sectors per cluster: %u\r\n",TFS.spc);
          printf("Reserved sectors: %Lu\r\n",TFS.res);
          #endif
          bs =  disk_readp(rBuff,lba,16,16);    // 
          if (bs) return bs;
          TFS.nFat = rBuff[0];
          TFS.rootDirN =(WORD) rBuff[2]<<8|rBuff[1];
          TFS.spf =(WORD) rBuff[7]<<8|rBuff[6];
          TFS.hidSect = (WORD) rBuff[15]<<8|rBuff[14];
          #if TFSDEBUG
          printf("No. of FATs: %u\r\n",TFS.nFat);
          printf("Root DIR Entries: %Lu\r\n",TFS.rootDirN);
          printf("Sectors per FAT: %Lu\r\n",TFS.spf);
          printf("Hidden sectors: %Lu\r\n",TFS.hidSect);
          #endif
         } else
         {
         bs =  disk_readp(rBuff,lba,0,16);    // 
          if (bs) return bs;
          TFS.spc = rBuff[13];
          TFS.res =(WORD) rBuff[15]<<8|rBuff[14];
          #if TFSDEBUG
           printf("Sectors per cluster: %u\r\n",TFS.spc);
           printf("Reserved Sectors: %Lu\r\n",TFS.res );
          #endif
          bs =  disk_readp(rBuff,lba,16,16);    // 
          if (bs) return bs;
          TFS.nFat = rBuff[0];                 
          TFS.hidSect = (DWORD) rBuff[15]<<24|(DWORD)rBuff[14]<<16|(DWORD)rBuff[13]<<8|rBuff[12];
          #if TFSDEBUG
          printf("No. of FATs: %u\r\n",TFS.nFat );
          printf("No. of Hidden Sectors: %Lu\r\n",TFS.hidSect); 
          #endif
          bs =  disk_readp(rBuff,lba,32,16);    // 
          if (bs) return bs;
          TFS.spf = (DWORD) rBuff[7]<<24|(DWORD)rBuff[6]<<16|(DWORD)rBuff[5]<<8|rBuff[4];
          TFS.clstDir = (DWORD) rBuff[15]<<24|(DWORD)rBuff[14]<<16|(DWORD)rBuff[13]<<8|rBuff[12];
          #if TFSDEBUG
          printf("Sectors per FAT: %Lu\r\n",TFS.spf);
          printf("Root DIR Starting cluster: %Lu\r\n", TFS.clstDir);
          #endif
          
         }

}

DRESULT dumpSector(DWORD sc){
  UINT ctr, offset; 
  DRESULT tRes;
  UINT8 dBuff[16];
 // char adr[8];
//sector dump to test disk interface
  printf("\r\nSector %Lu dump...\r\n", sc);
  offset = 0;
while(offset <512)
  {
   tRes = disk_readp(dBuff,sc,offset,16);    
   if(tRes){ printf("Sector reading error!\r\n"); return tRes;}
   printf("%8LX - ",(DWORD)(sc*512)+ (DWORD)offset); 
   for (ctr = 0; ctr < 16; ctr++) printf("%2X ",dBuff[ctr]);
    printf (" -- ");
   for (ctr = 0; ctr < 16; ctr++)    
    if (dBuff[ctr] <32 || dBuff[ctr] > 126) printf("."); else printf("%c",dBuff[ctr]);
    printf("\r\n");
    offset+= 16;
    }
    
  }
  void dumpCluster(void){
    FRESULT rs;
    UINT8  ss,itr;
    DWORD  sec;
    UINT  offset = 0;
    UINT8  tBuff[16];
    rs = tfs_clust2Sect(&sec,&tfsFil.cClust);
    if(FATWIDTH=F16) {
    
      for (ss=0; ss <TFS.spc; ss++)
         { do{
              rs = disk_readp( tBuff,sec + ss,offset,16);
              for (itr=0; itr<16; itr++)putchar(tBuff[itr]);
              offset+= 16;
              if (offset >=512){ offset = 0;break;} 
              
         } while(1);
         
         } }
    else  {
     
      for (ss=0; ss <TFS.spc; ss++)
         { do{
              rs = disk_readp( tBuff,sec + ss,offset,16);
              
              for (itr=0; itr<16; itr++)putchar(tBuff[itr]);
              offset+= 16;
              if (offset >=512){ offset = 0;break;} 
              
         } while(1);
         
         }
     
    }
     
     
     
     
}
#endif


DRESULT tfs_mount(UINT8 vol){

   DRESULT rs;
   UINT sc;
   UINT8 dBuff[16];
   /*
   vol refers to the mountable partition in the card. Cards are usually formatted like
   standard disks with MBR at sector offset 0.
   vALID Range: 1- 4 pertaining to partition entries 1 to 4 in the MBR
   */
 
   if(vol < 1|| vol > 4) return RES_PARERR;
   rs = disk_initialize();
   if(rs) return rs;
   switch (vol) {
   case 1: sc = pEntry1; break;
   case 2: sc = pEntry2; break;
   case 3: sc = pEntry3; break;
   case 4: sc = pEntry4; break;
   default: return RES_UNMOUNTABLE;
   }
   rs =  disk_readp(dBuff,0,sc,16);    // read partition table entry
   if(rs) return rs;
   //partInf.stat   = dBuff[0];
    switch (dBuff[4]){
      case FAT16:
      case FAT1632: 
      case FAT1632EX: FATWIDTH = F16; 
      #if TFSDEBUG
          printf("FAT System Type: FAT16\r\n");
      #endif
      break;
      case FAT32:
      case FAT32EX: FATWIDTH = F32; 
      #if TFSDEBUG
      printf("FAT System Type: FAT32\r\n");
      #endif
      break;
      default: return RES_UNSUPPORTED;
      }
   
   partInf.lba    = (DWORD) dBuff[0xb]<<24|(DWORD) dBuff[0xa]<<16|(DWORD) dBuff[9]<<8|dBuff[8];
   #if TFSDEBUG
   partInf.pSz    = (DWORD) dBuff[0xf]<<24|(DWORD) dBuff[0xe]<<16|(DWORD) dBuff[0xd]<<8|dBuff[0xc]; 
   
          printf("LBA Sector address: %Lu\r\n",partInf.lba);
          printf("Partition Size: %Lu\r\n", partInf.pSz);
   #endif
   // Load bootsector of the selected volume
    if (FATWIDTH==F16) { // tfs16
          rs =  disk_readp(dBuff,partInf.lba,0,16);    // 
          if (rs) return rs;
          TFS.spc = dBuff[13];
          TFS.res =(WORD) dBuff[15]<<8|dBuff[14];
          #if TFSDEBUG
          printf("Sectors per cluster: %u\r\n",TFS.spc);
          printf("Reserved sectors: %Lu\r\n",TFS.res);
          #endif
          rs =  disk_readp(dBuff,partInf.lba,16,16);    // 
          if (rs) return rs;
          TFS.nFat = dBuff[0];
          TFS.rootDirN =(WORD) dBuff[2]<<8|dBuff[1];
          TFS.spf =(WORD) dBuff[7]<<8|dBuff[6];
          TFS.hidSect = (WORD) dBuff[15]<<8|dBuff[14];
          #if TFSDEBUG
          printf("No. of FATs: %u\r\n",TFS.nFat);
          printf("Root DIR Entries: %Lu\r\n",TFS.rootDirN);
          printf("Sectors per FAT: %Lu\r\n",TFS.spf);
          printf("Hidden sectors: %Lu\r\n",TFS.hidSect);
          #endif
         } else
         {
         rs =  disk_readp(dBuff,partInf.lba,0,16);    // 
          if (rs) return rs;
          TFS.spc = dBuff[13];
          TFS.res =(WORD) dBuff[15]<<8|dBuff[14];
          #if TFSDEBUG
           printf("Sectors per cluster: %u\r\n",TFS.spc);
           printf("Reserved Sectors: %Lu\r\n",TFS.res );
          #endif
          rs =  disk_readp(dBuff,partInf.lba,16,16);    // 
          if (rs) return rs;
          TFS.nFat = dBuff[0];                 
          TFS.hidSect = (DWORD) dBuff[15]<<24|(DWORD)dBuff[14]<<16|(DWORD)dBuff[13]<<8|dBuff[12];
          #if TFSDEBUG
          printf("No. of FATs: %u\r\n",TFS.nFat );
          printf("No. of Hidden Sectors: %Lu\r\n",TFS.hidSect); 
          #endif
          rs =  disk_readp(dBuff,partInf.lba,32,16);    // 
          if (rs) return rs;
          TFS.spf = (DWORD) dBuff[7]<<24|(DWORD)dBuff[6]<<16|(DWORD)dBuff[5]<<8|dBuff[4];
          TFS.clstDir = (DWORD) dBuff[15]<<24|(DWORD)dBuff[14]<<16|(DWORD)dBuff[13]<<8|dBuff[12];
          if(TFS.clstDir < 2) return FR_NO_FILESYSTEM;
          #if TFSDEBUG
          printf("Sectors per FAT: %Lu\r\n",TFS.spf);
          printf("Root DIR Starting cluster: %Lu\r\n", TFS.clstDir);
          #endif
          
         }
}

FRESULT tfs_locateDir(DWORD * sc){
 /* returns sector address of the root directory 
 It must be called only after successful mounting */
 DWORD scadr= 0;
     if (FATWIDTH==F16)
     { scadr = partInf.lba + TFS.res + TFS.nfat*TFS.spf;
       *sc = scadr;
       return FR_OK;
     }
     else 
     if (FATWIDTH==F32) 
     {
      scadr = partInf.lba + TFS.res + (TFS.nfat*TFS.spf) + ((TFS.clstDir-2)*TFS.spc);
       *sc = scadr;
       return FR_OK;
     }
     else 
     return FR_NOT_ENABLED;
}
/* **************************************************** */
/* Open a file in the root directory                    */
/* **************************************************** */
FRESULT tfs_fopenFileDir(void){
/* 
   Offset in the Entry   Length in Bytes   Description
   00   8   Filename
   08   3   Extension
   0B   1   Attribute
   0C   1   Case
   0D   1   Creation time in ms
   0E   2   Creation time
   10   2   Creation date
   12   2   Last access date
   14   2   High word of starting cluster for FAT32
   16   2   Time stamp
   18   2   Date stamp
   1A   2   Starting cluster
   1C   4   Size of the file   
   DIR tfsDir;
   FILINFO tfsFil;
   rtDir gives the sector address of the directory
   */
   #if DIRBUFF32
      DRESULT drs;
   FRESULT frs;
   UINT8   fBuff[32];
   DWORD   sCtr = 0,rtDir,maxDir;
   UINT    offset = 0;
   if (FATWIDTH==F16)
      maxDir = (TFS.rootDirN*32)/512; else
      maxDir = (TFS.spf * 512)/32;
   frs = tfs_locateDir(&rtDir); 
   if(frs) return frs;
   
   #if TFSDEBUG
   printf("Size of Directory Table: %Lu sectors\r\n",maxDir);
   #endif
   /* scan root directory area */
   do{ 
   
   drs = disk_readp(fBuff,rtDir+sCtr,offset,32);
   if(drs) return FR_DISK_ERR;  
   if(fBuff[0]==0) break;
   tfsFil.fAttrib = fBuff[0xB];
   if(fBuff[0]==0xE5||fBuff[0xB] & VOLID || fBuff[0xB] & DIRECTORY) {offset+=32;
                       if (offset >=512){ offset=0; sCtr++; if (sCtr==maxDir) break;}
                       continue;}
   // if file is found in the dir table extract info; else loop until found otherwise, exit                  
   if (!memcmp(fBuff,tfsFil.fname,11))         
      {                 // read next 16 bytes to get cluster & size
         //drs = disk_readp(fBuff,rtDir+sCtr,offset,16);
         if(drs) return FR_DISK_ERR;  
         tfsFil.fSize = (DWORD)fBuff[0x1F]<<24|(DWORD)fBuff[0x1E]<<16|(DWORD)fBuff[0x1D]<<8|fBuff[0x1C];         
         if (FATWIDTH==16)
         tfsFil.sClust = (DWORD)fBuff[0x1B]<<8|fBuff[0x1A];  // get 16 bit cluster entry
         else                                              // get 32 bit cluster entry
          tfsFil.sClust = (DWORD)fBuff[0x15]<<24|(DWORD)fBuff[0x14]<<16|(DWORD)fBuff[0x1B]<<8|fBuff[0x1A]; 
		  /* Set file origin */
          tfsFil.cClust = tfsFil.sClust;  		  
		  tfs_clust2Sect(&tfsFil.sec, &tfsFil.sClust);
		  tfsFil.rwPtr = 0;
		  tfsFil.flag = FA_OPENED;
         #if TFSDEBUG
         printf("File Size: %Lu\r\n",tfsFil.fSize);
         printf("Starting CLuster: %Lu\r\n", tfsFil.sClust);
         #endif
		 offset+= 32; 
         return 0;
      } else      // check next entry
      { offset+=32;
        if (offset >=512){ offset=0; sCtr++; if (sCtr==maxDir) break;}
      }
      
   } while(1);
   
   
 return FR_NO_FILE;
   /* *** *** *** *** *** *** *** */
 #else
   DRESULT drs;
   FRESULT frs;
   UINT8   fBuff[16];
   DWORD   sCtr = 0,rtDir,maxDir;
   UINT    offset = 0;
   if (FATWIDTH==F16)
      maxDir = (TFS.rootDirN*32)/512; else
      maxDir = (TFS.spf * 512)/32;
   frs = tfs_locateDir(&rtDir); 
   if(frs) return frs;
   
   #if TFSDEBUG
   printf("Size of Directory Table: %Lu sectors\r\n",maxDir);
   #endif
   /* scan root directory area */
   do{ 
   
   drs = disk_readp(fBuff,rtDir+sCtr,offset,16);
   if(drs) return FR_DISK_ERR;  
   if(fBuff[0]==0) break;
   tfsFil.fAttrib = fBuff[0xB];
   if(fBuff[0]==0xE5||fBuff[0xB] & VOLID || fBuff[0xB] & DIRECTORY) {offset+=32;
                       if (offset >=512){ offset=0; sCtr++; if (sCtr==maxDir) break;}
                       continue;}
   // if file is found in the dir table extract info; else loop until found otherwise, exit                  
   if (!memcmp(fBuff,tfsFil.fname,11))         
      {  offset+= 16;                // read next 16 bytes to get cluster & size
         drs = disk_readp(fBuff,rtDir+sCtr,offset,16);
         if(drs) return FR_DISK_ERR;  
         tfsFil.fSize = (DWORD)fBuff[0xF]<<24|(DWORD)fBuff[0xE]<<16|(DWORD)fBuff[0xD]<<8|fBuff[0xC];         
         if (FATWIDTH==16)
         tfsFil.sClust = (DWORD)fBuff[0xB]<<8|fBuff[0xA];  // get 16 bit cluster entry
         else                                              // get 32 bit cluster entry
          tfsFil.sClust = (DWORD)fBuff[5]<<24|(DWORD)fBuff[4]<<16|(DWORD)fBuff[0xB]<<8|fBuff[0xA]; 
		  /* Set file origin */
          tfsFil.cClust = tfsFil.sClust;  		  
		  tfs_clust2Sect(&tfsFil.sec, &tfsFil.sClust);
		  tfsFil.rwPtr = 0;
		  tfsFil.flag = FA_OPENED;
         #if TFSDEBUG
         printf("File Size: %Lu\r\n",tfsFil.fSize);
         printf("Starting CLuster: %Lu\r\n", tfsFil.sClust);
         #endif
         return 0;
      } else      // check next entry
      { offset+=32;
        if (offset >=512){ offset=0; sCtr++; if (sCtr==maxDir) break;}
      }
      
   } while(1);
   
   
 return FR_NO_FILE;
 #endif //~DIRBUFF32
 
}

FRESULT tfs_fOpen(char *fn){
  
  UINT8 jj,kk,ll;
  FRESULT rs;
  char * ff = tfsFil.fname;  
  kk=strlen(fn);
  if (kk> 12||fn[0]==' '||kk==0||fn[0]=='.') return FR_NOT_OPENED;  
  memset(tfsFil.fname,0x20,13); // pad all with space       
  for (jj=0; jj<kk; jj++) {
  if(fn[jj]=='.') {jj++;break;} else
  ff[jj]= fn[jj];
  }
  
  for (ll=8; ll<11; ll++){
  if(fn[jj]==0) break;
  ff[ll]= fn[jj];
  jj++;
  }
#if TFSDEBUG
    printf("File Name:%s\r\n",tfsFil.fname);
    
 #endif 
  rs = tfs_fopenFileDir();
  return rs;

  
}
FRESULT tfs_getFatEntry(DWORD nClust){
/* fills the cCluster entry of the tsfFile structure 
   A FAT sector has 256 FAT entries in FAT16 while
   128 in FAT32
   DRESULT disk_readp (UINT8*, DWORD, WORD, WORD); 
   scadr = partInf.lba + TFS.tfs16.res + TFS.tfs16.nfat*TFS.tfs16.spf;*/
   #if BUFFERED_FAT 
	#if TFSDEBUG
    printf("\r\nFATDEBUG enabled\r\n");
	#endif
   DWORD sc,misc;
   union fnt{ 
             UINT8 fatEnt[32];
			 WORD  f16nt[16];
			 DWORD f32nt[8];
			 }fatNtrs;
   WORD  cl;
   FRESULT fs;
   UINT8 nc;  /* number of times the sector will be read 
                   to advance to the target location */
      
      if (FATWIDTH==F16) { //FAT16
	    sc = partInf.lba + TFS.res; /* point to first sector in FAT */
	     //determine sector location of cluster and add to displacement
		 sc += (nClust*2)/512;
		 //determine cluster offset;
		 misc = ((nClust*2)%512)/2; 
		 //determine group number
		 cl = misc/16; 
		 //determine group index
		 nc = misc%16;
		 fs=disk_readp(fatNtrs.fatEnt,sc,cl*32,32); 
		 if (fs) return fs;
		 tfsFil.cClust = (DWORD)fatNtrs.f16nt[nc];
	  } //~FAT16
	  
	  else 
	  
	  {   //FAT32
		 sc = partInf.lba + TFS.res; /* point to first sector in FAT */
	     /*determine sector location of cluster and add to displacement;
		  4 bytes per cluster entry */
		 sc += (nClust*4)/512;
		 //determine cluster offset;
		 misc = ((nClust*4)%512)/4; 
		 //determine group number
		 cl = misc/8; 
		 //determine group index
		 nc = misc%8;
		 fs=disk_readp(fatNtrs.fatEnt,sc,cl*32,32); 
		 if (fs) return fs;
		 tfsFil.cClust = (DWORD)fatNtrs.f32nt[nc];
      #if TFSDEBUG
      printf("Cluster Decoded in FAT: %LX\r\n",tfsFil.cClust);
	  #endif
}
 #else
   DWORD sc,misc;
   UINT8 fatEnt32[4];
   UINT8 fatEnt16[2];
   WORD  sPtr=0;
   WORD  fatEntCnt;
   FRESULT fs;
  
  
      
      if (FATWIDTH==F16) { //FAT16
	     misc = nClust*2;  // a cluster entry has two bytes
		 sc = partInf.lba + TFS.res; /* point to first sector in FAT */
	     if(misc < 512) {	
		        				
		   for(;;){
				fs=disk_readp(fatEnt16,sc,sPtr,2); 
				sPtr+= 2;
				if(sPtr > misc) break;
		     } 		 
			
		 }  else {
		    sc+= misc/512;
			for(;;){
			        fs=disk_readp(fatEnt16,sc,sPtr,2);
					sPtr+=2;
					if(sPtr > misc%512) break;
			       }
		   
		 }
		 
		 tfsFil.cClust = (DWORD)fatEnt16[1]<<8|fatEnt16[0];
	  } 
	  
	  else 
	  
	  {   //FAT32
	     misc = nClust*4;  // a cluster entry has 4 bytes
		 sc = partInf.lba + TFS.res; /* point to first sector in FAT */
		 
	     if(misc < 512) {
		   
		  
		   for(;;){
				fs=disk_readp(fatEnt32,sc,sPtr,4); 
				sPtr+= 4;
				if(sPtr > misc) break;
		     } 
			 
			
		 }  else {
		    sc+= misc/512;
			for(;;){
			        fs=disk_readp(fatEnt32,sc,sPtr,4);
					sPtr+=4;
					if(sPtr > misc%512) break;
			       }
		   
		 }
	 tfsFil.cClust = (DWORD)fatEnt32[3]<<24|(DWORD)fatEnt32[2]<<16|(DWORD)fatEnt32[1]<<8|fatEnt32[0];
	  }
   
      #if TFSDEBUG
      printf("Cluster Decoded in FAT: %LX\r\n",tfsFil.cClust);
	  #endif
   #endif  //~TESTROUTINE
}

FRESULT tfs_clust2Sect(DWORD *sec, CLUST *clst){
   /* returns with sec having a valid pointer to the absoulte sector address */
     if (FATWIDTH==F16)
     { *sec = partInf.lba + TFS.res + TFS.nfat*TFS.spf+ (TFS.rootDirN*32)/512+ ((*clst-2)*TFS.spc);       
       return FR_OK;
     }
     else 
     if (FATWIDTH==F32) 
     {
      *sec = partInf.lba + TFS.res + TFS.nfat*TFS.spf + ((*clst-2)*TFS.spc);       
       return FR_OK;
     }
     else 
     return FR_NOT_ENABLED;
}


FRESULT tfs_fSeek(DWORD fPos){

   CLUST clst;
   DWORD bcs, sect, ifptr;  
  
   if (!(tfsFil.flag & FA_OPENED)) return FR_NOT_OPENED;   

   if (fPos > tfsFil.fsize) fPos = tfsFil.fSize;   /* Limit pointer to file size */
   ifptr = tfsFil.rwPtr;
   tfsFil.rwPtr = 0;
   if (fPos > 0) {
      bcs = (DWORD)TFS.spc * 512;      
      if (ifptr > 0 &&
         (fPos - 1) / bcs >= (ifptr - 1) / bcs) {   
         tfsFil.rwPtr = (ifptr - 1) & ~(bcs - 1);   
         fPos -= tfsFil.rwPtr;
         clst = tfsFil.cClust;
      } else {                     
         clst = tfsFil.sClust;         
         tfsFil.cClust = clst;
      }
      while (fPos > bcs) {            
	     tfs_getFatEntry(clst);
         tfsFil.rwPtr+= bcs;
         fPos -= bcs;
      }
      tfsFil.rwPtr+= fPos;
	  tfs_clust2Sect(&sect,&clst);
        /* Current sector */      
      tfsFil.sec = sect + (tfsFil.rwPtr / 512 & (TFS.spc - 1));
   }

   return FR_OK;


	
   }
  
/* The following functions are heavily wired on the disk layer functions from Elm Chan's 
   PFS library. Hence, they are  modified functions of the same names.
*/
FRESULT tfs_fwrite (
   UINT8* buff,   /* Pointer to the data to be written */
   UINT btw,         /* Number of bytes to write (0:Finalize the current write operation) */
   UINT* bw         /* Pointer to number of bytes written */
)
{
   CLUST clst;
   DWORD sect, remain;
   UINT8 *p = buff;
   UINT8 cs;
   UINT wcnt;
  

   *bw = 0;
   
	if (!(tfsFil.flag & FA_OPENED)) return FR_NOT_OPENED;   
	
   if (!btw) {     
    
      if ((tfsFil.flag & FA_WIP) && disk_writep(0, 0)) return FR_DISK_ERR;
      tfsFil.flag &= ~FA_WIP;	  
	  
      return FR_OK;
   } else {      
      
      if (!(tfsFil.flag & FA_WIP)) {   
         tfsFil.rwPtr &= 0xFFFFFE00;
      }
   }
   
   remain = tfsFil.fsize - tfsFil.rwPtr;
   if (btw > remain) btw = (UINT)remain;         

   while (btw)   {                           
     
     if ((UINT)tfsFil.rwPtr % 512 == 0) {         
         
		 cs = (UINT8)(tfsFil.rwPtr / 512 & (TFS.spc - 1));   
         if (!cs) {                        
            
			if (tfsFil.rwPtr == 0) {            
               clst = tfsFil.sClust;
            } else {
 
				 tfs_getFatEntry(tfsFil.cClust);
            }
            if (clst <= 1) return FR_DISK_ERR;
            
         }
         
		 tfs_clust2Sect(&sect,&tfsFil.cClust);
         if (!sect) return FR_DISK_ERR;
         
		 tfsFil.sec = sect + cs;
         if (disk_writep(0, tfsFil.sec)) return FR_DISK_ERR; 
         
		 tfsFil.flag |= FA_WIP;
      }
      
	  wcnt = 512 - (UINT)tfsFil.rwPtr % 512; 
      if (wcnt > btw) wcnt = btw;
      if (disk_writep(p, wcnt)) return FR_DISK_ERR;   
     
	   tfsFil.rwPtr += wcnt; p += wcnt;  
      btw -= wcnt; *bw += wcnt;
     
	  if ((UINT)tfsFil.rwPtr % 512 == 0) {
		  if (disk_writep(0, 0)) return FR_DISK_ERR;          
		 tfsFil.flag &= ~FA_WIP;
      }
   }

   return FR_OK;
}
   

FRESULT tfs_fRead(UINT8 *buff, UINT  btr,UINT * br){  
   
   DRESULT dr;
   CLUST clst;
   DWORD sect, remain;
   UINT rcnt;
   UINT8 cs, *rbuff = buff;
  


   *br = 0;
    remain = tfsFil.fsize - tfsFil.rwPtr;
   if (btr > remain) btr = (UINT)remain;         	

   while (btr)   {                           		
      if ((tfsFil.rwPtr % 512) == 0) {            	
         cs = (UINT8)(tfsFil.rwPtr / 512 & (TFS.spc - 1));  
         if (!cs) {                        			
            if (tfsFil.rwPtr == 0) {            	
               clst = tfsFil.sClust;
            } else {
               
			   tfs_getfatEntry(tfsFil.cClust);
            }
         
         }
         tfs_clust2sect(&sect,&tfsFil.cClust);      
         
		 tfsFil.sec = sect + cs;
      }
      rcnt = 512 - (UINT)tfsFil.rwPtr % 512;         
      if (rcnt > btr) rcnt = btr;
      dr = disk_readp(rbuff, tfsFil.sec, (UINT)tfsFil.rwPtr % 512, rcnt);
      if (dr) return FR_DISK_ERR; 
      tfsFil.rwPtr += rcnt;                     
      btr -= rcnt; *br += rcnt;               	
      if (rbuff) rbuff += rcnt;               	
   }

   return FR_OK;
}  

