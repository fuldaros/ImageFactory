/* tools/mkbootimg/mkbootimg.c
**
** Copyright 2007, The Android Open Source Project
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in Merging, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdbool.h>

#include "sha.h"
#include "bootimg.h"

static void *load_file(const char *fn, unsigned *_sz, char *image_type)
{
    char *data;
    int sz;
    int fd;
    int offset = 0;

    data = 0;
    fd = open(fn, O_RDONLY);
    if(fd < 0) return 0;

    sz = lseek(fd, 0, SEEK_END);
    if(sz < 0) goto oops;

    if(lseek(fd, 0, SEEK_SET) != 0) goto oops;
    if(image_type != NULL){
        offset = MTK_MAGIC_SIZE;
    }
    data = (char*) malloc(sz + offset);
    if(data == 0) goto oops;
    if(image_type != NULL){
        printf("Merging Mediatek header...\n");
        unsigned char size_hex[4];
        int i;
        data[0] = 0x88;
        data[1] = 0x16;
        data[2] = 0x88;
        data[3] = 0x58;
        sprintf(size_hex, "%08X", sz);
        for(i = 4 ; i < 8 ; i++){
            data[i] = size_hex[i-4];
        }
        if(!strcmp(fn, MTK_SIGNATURE_BOOT)){
            for(i = 8; i < 14 ; i++)
                data[i] = mtk_boot[i-8];
        }
        if(!strcmp(fn, MTK_SIGNATURE_RECOVERY)){
            for(i = 8; i < 16 ; i++)
                data[i] = mtk_recovery[i-8];
        }
        for(; i < 40; i++) data[i] = 0x00;
        for(i = 40; i < MTK_MAGIC_SIZE; i++) data[i] = 0xFF;
    }
    if(read(fd, &data[offset], sz) != sz) goto oops;
    close(fd);

    if(_sz) *_sz = sz + offset;
    return data;

oops:
    close(fd);
    if(data != 0) free(data);
    return 0;
}

int mkbootimg_usage(void)
{
    fprintf(stderr,"mkbootimg_usage: mkbootimg\n"
            "       --config <filename>\n"
            "       --kernel <filename>\n"
            "       [ --ramdisk <filename> ]\n"
            "       [ --second <2ndbootloader-filename> ]\n"
            //"       [ --cmdline <kernel-commandline> ]\n"
            //"       [ --board <boardname> ]\n"
            //"       [ --base <address> ]\n"
            //"       [ --pagesize <pagesize> ]\n"
            "       [ --dt <filename> ]\n"
            //"       [ --ramdisk_offset <address> ]\n"
            //"       [ --second_offset <address> ]\n"
            //"       [ --tags_offset <address> ]\n"
            //"       [ --id ]\n"
            "       -o|--output <filename>\n"
            );
    return 1;
}

static unsigned char padding[131072] = { 0, };

int write_padding(int fd, unsigned pagesize, unsigned itemsize)
{
    unsigned pagemask = pagesize - 1;
    unsigned count;

    if((itemsize & pagemask) == 0) {
        return 0;
    }

    count = pagesize - (itemsize & pagemask);

    if(write(fd, padding, count) != (signed)count) {
        return -1;
    } else {
        return 0;
    }
}
// void *gen_header(const char *fn, unsigned sz)
// {
//     unsigned char *data;
//     //char data_hex[8];
//     unsigned char size_hex[4];
//     int i;
//     data = (unsigned char *) malloc(MTK_MAGIC_SIZE);
//     data[0] = 0x88;
//     data[1] = 0x16;
//     data[2] = 0x88;
//     data[3] = 0x58;
//     sprintf(size_hex, "%08X", sz);
//     for(i = 4 ; i < 8 ; i++){
//         data[i] = size_hex[i-4];
//     }
//     if(!strcmp(fn, MTK_SIGNATURE_BOOT)){
//         for(i = 8; i < 14 ; i++)
//             data[i] = mtk_boot[i-8];
//     }
//     if(!strcmp(fn, MTK_SIGNATURE_RECOVERY)){
//         for(i = 8; i < 16 ; i++)
//             data[i] = mtk_recovery[i-8];
//     }
//     for(; i < 40; i++) data[i] = 0x00;
//     for(i = 40; i < MTK_MAGIC_SIZE; i++) data[i] = 0xFF;
//     return data;
// oops:
//     if(data != 0) free(data);
//     return 0;
// }
int mkbootimg_main(int argc, char **argv)
{
    boot_img_hdr hdr;
    Config config;

    char *config_fn;

    char *kernel_fn = NULL;
    void *kernel_data = NULL;
    char *ramdisk_fn = NULL;
    void *ramdisk_data = NULL;
    char *second_fn = NULL;
    void *second_data = NULL;
    char *cmdline = "";
    char *bootimg = NULL;
    char *board = "";
    char *dt_fn = 0;
    void *dt_data = 0;
    uint32_t pagesize = 2048;
    int fd;
    SHA_CTX ctx;
    const uint8_t* sha;
    unsigned base           = 0x10000000;
    unsigned kernel_offset  = 0x00008000;
    unsigned ramdisk_offset = 0x01000000;
    unsigned second_offset  = 0x00f00000;
    unsigned tags_offset    = 0x00000100;
    size_t cmdlen;

    argc--;
    argv++;

    memset(&hdr, 0, sizeof(hdr));
    while(argc > 0){
        char *arg = argv[0];
        char *val = argv[1];
        argc -= 2;
        argv += 2;
        if(!strcmp(arg, "--output") || !strcmp(arg, "-o")) {
            bootimg = val;
        } else if(!strcmp(arg, "--kernel")) {
            kernel_fn = val;
        } else if(!strcmp(arg, "--ramdisk")) {
            ramdisk_fn = val;
        } else if(!strcmp(arg, "--second")) {
            second_fn = val;
        } else if(!strcmp(arg, "--config")) {
            config_fn = val;
        } else if(!strcmp(arg, "--dt")) {
            dt_fn = val;
        } else {
            return mkbootimg_usage();
        }
    }
       
    if(bootimg == 0) {
        fprintf(stderr,"error: no output filename specified\n");
        return mkbootimg_usage();
    }

    if(kernel_fn == 0) {
        fprintf(stderr,"error: no kernel image specified\n");
        return mkbootimg_usage();
    }

    if(ramdisk_fn == 0) {
        fprintf(stderr,"error: no ramdisk image specified\n");
        return mkbootimg_usage();
    }
    if(config_fn == 0) {
        fprintf(stderr,"error: no config file specified\n");
        return mkbootimg_usage();
    }

    if(strlen(board) >= BOOT_NAME_SIZE) {
        fprintf(stderr,"error: board name too large\n");
        return mkbootimg_usage();
    }
    FILE *fp_info = fopen(config_fn, "rb");
    fread(&config, sizeof(config), 1, fp_info);
    fclose(fp_info);
    base = config.kernel_base;
    cmdline = config.cmdline;
    ramdisk_offset = config.ramdisk_offset;
    second_offset = config.second_offset;
    tags_offset = config.tags_offset;
    pagesize = config.page_size;
    hdr.page_size = pagesize;
    hdr.kernel_addr =  base + kernel_offset;
    hdr.ramdisk_addr = base + ramdisk_offset;
    hdr.second_addr =  base + second_offset;
    hdr.tags_addr =    base + tags_offset;
    char *image_type;
    if(config.image_type == MTK_IMAGE_BOOT){
        image_type = MTK_SIGNATURE_BOOT;
    } else if(config.image_type == MTK_IMAGE_RECOVERY){
        image_type = MTK_SIGNATURE_RECOVERY;
    }
    strcpy((char *) hdr.name, board);

    memcpy(hdr.magic, BOOT_MAGIC, BOOT_MAGIC_SIZE);

    cmdlen = strlen(cmdline);
    if(cmdlen > (BOOT_ARGS_SIZE + BOOT_EXTRA_ARGS_SIZE - 2)) {
        fprintf(stderr,"error: kernel commandline too large\n");
        return 1;
    }
    strncpy((char *)hdr.cmdline, cmdline, BOOT_ARGS_SIZE - 1);
    hdr.cmdline[BOOT_ARGS_SIZE - 1] = '\0';
    if (cmdlen >= (BOOT_ARGS_SIZE - 1)) {
        cmdline += (BOOT_ARGS_SIZE - 1);
        strncpy((char *)hdr.extra_cmdline, cmdline, BOOT_EXTRA_ARGS_SIZE);
    }

    kernel_data = load_file(kernel_fn, &hdr.kernel_size, NULL);
    if(kernel_data == 0) {
        fprintf(stderr,"error: could not load kernel '%s'\n", kernel_fn);
        return 1;
    }

    if(ramdisk_fn == 0) {
        ramdisk_data = 0;
        hdr.ramdisk_size = 0;
    } else {
        ramdisk_data = load_file(ramdisk_fn, &hdr.ramdisk_size, image_type);
        if(ramdisk_data == 0) {
            fprintf(stderr,"error: could not load ramdisk '%s'\n", ramdisk_fn);
            return 1;
        }
    }

    if(second_fn) {
        second_data = load_file(second_fn, &hdr.second_size, NULL);
        if(second_data == 0) {
            fprintf(stderr,"error: could not load secondstage '%s'\n", second_fn);
            return 1;
        }
    }

    if(dt_fn) {
        dt_data = load_file(dt_fn, &hdr.dt_size, NULL);
        if (dt_data == 0) {
            fprintf(stderr,"error: could not load device tree image '%s'\n", dt_fn);
            return 1;
        }
    }

    /* put a hash of the contents in the header so boot images can be
     * differentiated based on their first 2k.
     */
    SHA_init(&ctx);
    SHA_update(&ctx, kernel_data, hdr.kernel_size);
    SHA_update(&ctx, &hdr.kernel_size, sizeof(hdr.kernel_size));
    SHA_update(&ctx, ramdisk_data, hdr.ramdisk_size);
    SHA_update(&ctx, &hdr.ramdisk_size, sizeof(hdr.ramdisk_size));
    SHA_update(&ctx, second_data, hdr.second_size);
    SHA_update(&ctx, &hdr.second_size, sizeof(hdr.second_size));
    if(dt_data) {
        SHA_update(&ctx, dt_data, hdr.dt_size);
        SHA_update(&ctx, &hdr.dt_size, sizeof(hdr.dt_size));
    }
    sha = SHA_final(&ctx);
    memcpy(hdr.id, sha,
           SHA_DIGEST_SIZE > sizeof(hdr.id) ? sizeof(hdr.id) : SHA_DIGEST_SIZE);

    fd = open(bootimg, O_CREAT | O_TRUNC | O_WRONLY, 0644);
    if(fd < 0) {
        fprintf(stderr,"error: could not create '%s'\n", bootimg);
        return 1;
    }

    if(write(fd, &hdr, sizeof(hdr)) != sizeof(hdr)) goto fail;
    if(write_padding(fd, pagesize, sizeof(hdr))) goto fail;
    printf("Merging kernel...\n");
    if(write(fd, kernel_data, hdr.kernel_size) != (ssize_t) hdr.kernel_size) goto fail;
    free(kernel_data);
    if(write_padding(fd, pagesize, hdr.kernel_size)) goto fail;
    // if(config.mtk_flag == 1) {
    //     // generate Mediatek header
    //     printf("Merging Mediatek header...\n");
    //     void *mtk_hdr_data = gen_header(image_type, hdr.ramdisk_size);
    //     if(write(fd, mtk_hdr_data, MTK_MAGIC_SIZE) != MTK_MAGIC_SIZE) goto fail;
    //     free(mtk_hdr_data);
    // }
    printf("Merging ramdisk...\n");
    if(write(fd, ramdisk_data, hdr.ramdisk_size) != (ssize_t) hdr.ramdisk_size) goto fail;
    free(ramdisk_data);
    // if(config.mtk_flag == 1)
    //     hdr.ramdisk_size += MTK_MAGIC_SIZE;
    if(write_padding(fd, pagesize, hdr.ramdisk_size)) goto fail;
    printf("Merging second...\n");
    if(second_data) {
        if(write(fd, second_data, hdr.second_size) != (ssize_t) hdr.second_size) goto fail;
        if(write_padding(fd, pagesize, hdr.second_size)) goto fail;
        free(second_data);
    }
    printf("Merging dt.img...\n");
    if(dt_data) {
        if(write(fd, dt_data, hdr.dt_size) != (ssize_t) hdr.dt_size) goto fail;
        if(write_padding(fd, pagesize, hdr.dt_size)) goto fail;
        free(dt_data);
    }
    print_image_info(hdr);
    return 0;

fail:
    unlink(bootimg);
    close(fd);
    fprintf(stderr,"error: failed Merging '%s': %s\n", bootimg,
            strerror(errno));
    return 1;
}