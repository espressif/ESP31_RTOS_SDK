/* boot.bin        @ 0x00000 */
/* irom1.bin       @ 0xX4000 */
/* irom0_flash.bin @ 0xX0000+0x40000 */

/* Flash Map */
/* |..|...............|...........................................|..|            */
/* ^  ^               ^                                           ^               */
/* |_boot.bin(0x0000) |_irom0_flash.bin(0xX0000+0x40000)          |               */
/*    |_irom1.bin(0xX4000)                                        |_system param area(Flash size - 0x4000) */

/* NOTICE: */ 
/* 1. irom1.bin + irom0_flash.bin = user.ota                                    */
/* 2. irom1.bin and irom0_flash.bin must locate at 0xX4000 and 0xX0000+0x40000, */
/*    here 0xX0000 must 256KB align.                                            */
/* 3. Make sure each user.ota not overlap other.                                */
/* 4. Make sure each user.ota not overlap system param area or user param area. */
/* 5. We support a maximum of 5 user.ota                                        */
/* 6. We support 1MB/2MB/4MB/8MB/16MB flash,                               */
/*    but make suer user.ota not exceed 16MB.                                   */
/* 7. Don't change any seg.                                                     */

MEMORY
{
  dport0_0_seg :                      	org = 0x3FF00000, len = 0x10
  dram0_0_seg  :                       	org = 0x3FFD8000, len = 0x24000
  iram1_0_seg  :                       	org = 0x40040000, len = 0x20000
  irom0_0_seg  :                       	org = 0x40080010, len = 0x37FFF0
  irom0_1_seg  :                        org = 0x3FE04010, len = 0x3BFF0
}

INCLUDE "../ld/eagle.pro.v7.common.ld"