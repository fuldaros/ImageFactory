#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <limits.h>
#include <libgen.h>
#include <sys/stat.h>

#include "sha.h"
#include "bootimg.h"

typedef unsigned char byte;

int read_padding(FILE* f, unsigned itemsize, int pagesize)
{
    byte* buf = (byte*)malloc(sizeof(byte) * pagesize);
    unsigned pagemask = pagesize - 1;
    unsigned count;

    if((itemsize & pagemask) == 0) {
        free(buf);
        return 0;
    }

    count = pagesize - (itemsize & pagemask);

    fread(buf, count, 1, f);
    free(buf);
    return count;
}

void write_string_to_file(char* file, char* string)
{
    FILE* f = fopen(file, "w");
    fwrite(string, strlen(string), 1, f);
    fwrite("\n", 1, 1, f);
    fclose(f);
}

int unpackbootimg_usage() {
    printf("unpackbootimg_usage: unpackbootimg\n");
    printf("\t-i|--input boot.img\n");
    printf("\t[ -o|--output output_directory]\n");
    printf("\t[ -p|--pagesize <size-in-hexadecimal> ]\n");
    return 1;
}

int unpackbootimg_main(int argc, char** argv)
{
    char tmp[PATH_MAX];
    char* directory = "./";
    char* filename = NULL;
    int pagesize = 0;
    Config config;

    argc--;
    argv++;
    while(argc > 0){
        char *arg = argv[0];
        char *val = argv[1];
        argc -= 2;
        argv += 2;
        if(!strcmp(arg, "--input") || !strcmp(arg, "-i")) {
            filename = val;
        } else if(!strcmp(arg, "--output") || !strcmp(arg, "-o")) {
            directory = val;
        } else if(!strcmp(arg, "--pagesize") || !strcmp(arg, "-p")) {
            pagesize = strtoul(val, 0, 16);
        } else {
            return unpackbootimg_usage();
        }
    }
    
    if (filename == NULL) {
        return unpackbootimg_usage();
    }
    mkdir(directory, 0755);
    
    int total_read = 0;
    FILE* f = fopen(filename, "rb");
    boot_img_hdr header;


    //printf("Reading header...\n");
    int i;
    for (i = 0; i <= 512; i++) {
        fseek(f, i, SEEK_SET);
        fread(tmp, BOOT_MAGIC_SIZE, 1, f);
        if (memcmp(tmp, BOOT_MAGIC, BOOT_MAGIC_SIZE) == 0)
            break;
    }
    total_read = i;
    if (i > 512) {
        printf("Android boot magic not found.\n");
        return 1;
    }
    fseek(f, i, SEEK_SET);
    printf("Android magic found at: %d\n", i);

    fread(&header, sizeof(header), 1, f);
    if (pagesize == 0) {
        pagesize = header.page_size;
    }
    print_image_info(header);

    // set configs
    config.kernel_base = header.kernel_addr - 0x00008000;
    config.ramdisk_offset = header.ramdisk_addr - header.kernel_addr + 0x00008000;
    config.second_offset = header.second_addr - header.kernel_addr + 0x00008000;
    config.tags_offset = header.tags_addr - header.kernel_addr + 0x00008000;
    config.page_size = header.page_size;
    strncpy((char *)config.cmdline, header.cmdline, BOOT_ARGS_SIZE - 1);
    config.mtk_flag = 0;

    total_read += sizeof(header);
    total_read += read_padding(f, sizeof(header), pagesize);
    printf("\nExtracting kernel...\n");
    sprintf(tmp, "%s/%s", directory, "zImage");
    FILE *k = fopen(tmp, "wb");
    if(!k){
        fclose(f);
        return 1;
    }
    byte* kernel = (byte*)malloc(header.kernel_size);
    //printf("Reading kernel...\n");
    fread(kernel, header.kernel_size, 1, f);
    total_read += header.kernel_size;
    fwrite(kernel, header.kernel_size, 1, k);
    fclose(k);
    free(kernel);

    //printf("total read: %d\n", header.kernel_size);
    total_read += read_padding(f, header.kernel_size, pagesize);
    printf("Extracting ramdisk...\n");
    // Checking Mediatek ramdisk header
    byte* chkram = (byte *) malloc(MTK_MAGIC_SIZE);
    fread(chkram, MTK_MAGIC_SIZE, 1, f);
    if(check_mtk_header(chkram)){
        printf("Processing Mediatek ramdisk header...\n");
        config.mtk_flag = 1;
        config.image_type = get_mtk_image_type(chkram);
    }else{
        fseek(f, -MTK_MAGIC_SIZE, SEEK_CUR);
    }
    free(chkram);
    byte* ramdisk = (byte*)malloc(header.ramdisk_size);
    fread(ramdisk, header.ramdisk_size, 1, f);
    total_read += header.ramdisk_size;
    // if(ramdisk[0] == 0x02 && ramdisk[1] == 0x21){
    //     sprintf(tmp, "%s/%s", directory, "ramdisk.cpio.lz4");
    // } else {
    sprintf(tmp, "%s/%s", directory, "ramdisk.cpio.gz");
    //}
    
    FILE *r = fopen(tmp, "wb");
    if(!r){
        free(ramdisk);
        fclose(f);
        return 1;
    }
    fwrite(ramdisk, header.ramdisk_size, 1, r);
    fclose(r);
    free(ramdisk);
    total_read += read_padding(f, header.ramdisk_size, pagesize);
    printf("Extracting second...\n");
    sprintf(tmp, "%s/%s", directory, "second");
    FILE *s = fopen(tmp, "wb");
    if(!s){
        fclose(f);
        return 1;
    }
    byte* second = (byte*)malloc(header.second_size);
    //printf("Reading second...\n");
    fread(second, header.second_size, 1, f);
    total_read += header.second_size;
    fwrite(second, header.second_size, 1, r);
    fclose(s);
    free(second);

    total_read += read_padding(f, header.second_size, pagesize);
    printf("Extracting dt.img...\n");
    sprintf(tmp, "%s/%s", directory, "dt.img");
    FILE *d = fopen(tmp, "wb");
    if(!d){
        fclose(f);
        return 1;
    }
    byte* dt = (byte*)malloc(header.dt_size);
    //printf("Reading dt...\n");
    fread(dt, header.dt_size, 1, f);
    total_read += header.dt_size;
    fwrite(dt, header.dt_size, 1, r);
    fclose(d);
    free(dt);
    fclose(f);
    // save configs
    printf("Saving configuretion...\n");
    sprintf(tmp, "%s/%s", directory, ".config");
    FILE *cfg = fopen(tmp, "wb");
    if(!cfg){
        fclose(f);
        return 1;
    }
    fwrite(&config, sizeof(config), 1, cfg);
    fclose(cfg);
    //printf("Total Read: %d\n", total_read);
    return 0;
}