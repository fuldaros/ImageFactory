/* tools/mkbootimg/bootimg.h
**
** Copyright 2007, The Android Open Source Project
**
** Licensed under the Apache License, Version 2.0 (the "License"); 
** you may not use this file except in compliance with the License. 
** You may obtain a copy of the License at 
**
**     http://www.apache.org/licenses/LICENSE-2.0 
**
** Unless required by applicable law or agreed to in writing, software 
** distributed under the License is distributed on an "AS IS" BASIS, 
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
** See the License for the specific language governing permissions and 
** limitations under the License.
*/
#include <string.h>
#ifndef _BOOT_IMAGE_H_
#define _BOOT_IMAGE_H_

typedef struct boot_img_hdr boot_img_hdr;

#define BOOT_MAGIC "ANDROID!"
#define BOOT_MAGIC_SIZE 8
#define BOOT_NAME_SIZE 16
#define BOOT_ARGS_SIZE 512
#define BOOT_EXTRA_ARGS_SIZE 1024

// Mediatek
#define MTK_MAGIC_SIZE 512
#define MTK_SIGNATURE_BOOT "ROOTFS"
#define MTK_SIGNATURE_RECOVERY "RECOVERY"
#define MTK_IMAGE_BOOT 1001
#define MTK_IMAGE_RECOVERY 1002
//                        r      o      o    t     f     s
static const unsigned char mtk_boot[6] = {0x52, 0x4f, 0x4f, 0x54, 0x46, 0x53};
//                            r     e     c     o     v     e      r    y
static const char mtk_recovery[8] = {0x52, 0x45, 0x43, 0x4f, 0x56, 0x45, 0x52, 0x59};


struct boot_img_hdr
{
    unsigned char magic[BOOT_MAGIC_SIZE];

    unsigned kernel_size;  /* size in bytes */
    unsigned kernel_addr;  /* physical load addr */

    unsigned ramdisk_size; /* size in bytes */
    unsigned ramdisk_addr; /* physical load addr */

    unsigned second_size;  /* size in bytes */
    unsigned second_addr;  /* physical load addr */

    unsigned tags_addr;    /* physical addr for kernel tags */
    unsigned page_size;    /* flash page size we assume */
    unsigned dt_size;      /* device tree in bytes */
    unsigned unused;       /* future expansion: should be 0 */
    unsigned char name[BOOT_NAME_SIZE]; /* asciiz product name */

    unsigned char cmdline[BOOT_ARGS_SIZE];

    unsigned id[8]; /* timestamp / checksum / sha1 / etc */

    /* Supplemental command line data; kept here to maintain
     * binary compatibility with older versions of mkbootimg */
    unsigned char extra_cmdline[BOOT_EXTRA_ARGS_SIZE];
};
typedef struct Configuration
{
    unsigned char cmdline[BOOT_ARGS_SIZE];
    unsigned kernel_base;
    unsigned ramdisk_offset;
    unsigned second_offset;
    unsigned tags_offset;
    unsigned page_size;
    unsigned mtk_flag;
    unsigned image_type;
} Config;
static inline int check_mtk_header(unsigned char *data){
    if(data[0] == 0x88 && data[1] == 0x16 && data[2] == 0x88 && data[3] == 0x58){
        return 1;
    }
    return 0;
} 
static inline int get_mtk_image_type(unsigned char *data){
    // int i;
    // for(i = 4; i < 8; i++){
    //     printf("%X ", data[i]);
    // }
    // printf("\n");
    if(data[4] == mtk_boot[0] && data[5] == mtk_boot[1] && data[6] == mtk_boot[2] && mtk_boot[7] == mtk_boot[3]){
        return MTK_IMAGE_BOOT;
    }
    return MTK_IMAGE_RECOVERY;
}
static inline void print_image_info(boot_img_hdr header){
    printf("Product Name\t:\t\'%s\'\n", header.name);
    printf("Product ID\t:\t%02x%02x%02x%02x%02x%02x%02x%02x\n", header.id[0], header.id[1], header.id[2], header.id[3], header.id[4], header.id[5], header.id[6], header.id[7]);
    printf("Command Line\t:\t\'%s\'\n", header.cmdline);
    printf("Base Address\t:\t%08x\n", header.kernel_addr - 0x00008000);
    printf("Kernel Size\t:\t%d\n", header.kernel_size);
    printf("Kernel Address\t:\t%08x\n", header.kernel_addr);
    printf("Kernel Offset\t:\t0x00008000\n");
    printf("Ramdisk Size\t:\t%d\n", header.ramdisk_size);
    printf("Ramdisk Address\t:\t%08x\n", header.ramdisk_addr);
    printf("Ramdisk Offset\t:\t%08x\n", header.ramdisk_addr - header.kernel_addr + 0x00008000);
    printf("Second Size\t:\t%d\n", header.second_size);
    printf("Second Address\t:\t%08x\n", header.ramdisk_addr);
    printf("Second Offset\t:\t%08x\n", header.second_addr - header.kernel_addr + 0x00008000);
    printf("Tags Address\t:\t%08x\n", header.tags_addr);
    printf("Tags Offset\t:\t%08x\n", header.tags_addr - header.kernel_addr + 0x00008000);
    printf("Page Size\t:\t%d\n", header.page_size);
    printf("DTB Size\t:\t%d\n", header.dt_size);
}
/*
** +-----------------+ 
** | boot header     | 1 page
** +-----------------+
** | kernel          | n pages  
** +-----------------+
** | ramdisk         | m pages  
** +-----------------+
** | second stage    | o pages
** +-----------------+
** | device tree     | p pages
** +-----------------+
**
** n = (kernel_size + page_size - 1) / page_size
** m = (ramdisk_size + page_size - 1) / page_size
** o = (second_size + page_size - 1) / page_size
** p = (dt_size + page_size - 1) / page_size
**
** 0. all entities are page_size aligned in flash
** 1. kernel and ramdisk are required (size != 0)
** 2. second is optional (second_size == 0 -> no second)
** 3. load each element (kernel, ramdisk, second) at
**    the specified physical address (kernel_addr, etc)
** 4. prepare tags at tag_addr.  kernel_args[] is
**    appended to the kernel commandline in the tags.
** 5. r0 = 0, r1 = MACHINE_TYPE, r2 = tags_addr
** 6. if second_size != 0: jump to second_addr
**    else: jump to kernel_addr
*/

#if 0
typedef struct ptentry ptentry;

struct ptentry {
    char name[16];      /* asciiz partition name    */
    unsigned start;     /* starting block number    */
    unsigned length;    /* length in blocks         */
    unsigned flags;     /* set to zero              */
};

/* MSM Partition Table ATAG
**
** length: 2 + 7 * n
** atag:   0x4d534d70
**         <ptentry> x n
*/
#endif

#endif
