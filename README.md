# Thin-FAT-FS
Thin FAT File System (Inspired by Petit FAT File System)
Elm Cha(N) (as what he calls himself) did a wonderful job in his Petit FAT File System. It's been a good resource in my AVR projects. However, to my dismay, I can't make the PFS work in any PIC MCU using CCS C  compiler. I saw a forum on Microchip C18 however and they were able to make it work. But since my preferred PIC compiler is CCS C , when I finally gave up in the troubleshooting to make Petit FS work, the obvious recourse is to write my own FAT access library/program using my preferred compiler. 
Thin FAT File System or simply TFS is similar to PFS in some respect. In writing the library, I adopted routine and  variable names from ChaN's PFS library. However, the implementation has been completely re-written from the way I understand the FAT (16 & 32) design,  hence the new name. On the otherhand, for shorter development time, I re-used Chan's disk access layer library from the PFS package.
The Thin FAT FS has been tested in both mid-range (PIC16F887) and high-end (PIC18F46K22) PIC MCUS. However, no benchmark data is available at the moment.
Not as mature as PFS, TFS definitely requires optimization and it is going to be part of the roadmap for this project. Below are the TFS features:

      1. Supports MBR. You can mount any partition ( up to 4) in an MMC/SD card.
      2. FAT16/FAT32 auto recognition. Both FAT system are monolitihically supported.
      3. No support for subdirectories/folders. Files have to be in the root directory.
      4. Read and/or Limited write support. Files  cannot grow/shrink. And just like the PFS, 
         TFS will not mind Read-Only & Hidden file attributes. TFS works only with existing file
         and it has no facility to create or delete any file. However, I have provided a batch 
         file that can be used to propagate dummy files of virtually any size.
      5. TFS ( tfs.c & tfs.h) are realeased under GNU 3.0. Hence you must be aware of the license conditions.
         
