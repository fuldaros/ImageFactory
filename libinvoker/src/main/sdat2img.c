/*
====================================
          FILE: sdat2img.c
        AUTHOR: crixec@gmail.com 
       CREATED: 2016年8月4日
====================================
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#define BUFFER_SIZE 1024
#define BLOCK_SIZE 4096
#define ERASE_PREFIX "erase "
#define NEW_PREFIX "new "
#define ZERO_PREFIX "zero "

struct Transfer {
  char *CMD_ERA;
  char *CMD_NEW;
  char *CMD_ZERO;
  int VER;
  int COUNT;
};
typedef struct Transfer transfer;
typedef unsigned char byte;
char *read_line(FILE * fp) {
  char *line = (char *)malloc(sizeof(char *) * BUFFER_SIZE);
  fgets(line, BUFFER_SIZE, fp);
  int size = strlen(line);
  // replace \n
  line[size - 1] = '\0';
  return line;
}

int index_of(char *s, char *str) {
  int index = -1;
  int i = 0, j = 0;
  int len = strlen(str);
  int slen = strlen(s);
  if (len < slen) {
    return index;
  }
  char *tmp = (char *)malloc(sizeof(char *) * slen);
  for (; j < len; j++) {
    for (i = 0; i < slen; i++) {
      tmp[i] = str[j + i];
    }
    if (!strcmp(tmp, s)) {
      index = j;
      break;
    }
  }
  return index;
}
char *replace(char *src, char *sub, char *dst) {
  int pos = 0;
  int offset = 0;
  int srcLen, subLen, dstLen;
  char *pRet = NULL;
  srcLen = strlen(src);
  subLen = strlen(sub);
  dstLen = strlen(dst);
  pRet = (char *)malloc(srcLen + dstLen - subLen + 1);
  if (NULL != pRet) {
    pos = strstr(src, sub) - src;
    memcpy(pRet, src, pos);
    offset += pos;
    memcpy(pRet + offset, dst, dstLen);
    offset += dstLen;
    memcpy(pRet + offset, src + pos + subLen, srcLen - pos - subLen);
    offset += srcLen - pos - subLen;
    *(pRet + offset) = '\0';
  }
  return pRet;

}

void read_transfer(FILE * fp, transfer * trans) {
  memset(trans, 0, sizeof(trans));
  fseek(fp, 0, SEEK_SET);
  int i = 0;
  trans->VER = atoi(read_line(fp));
  trans->COUNT = atoi(read_line(fp));
  while (!feof(fp)) {
    char *line = read_line(fp);
    if (index_of(ERASE_PREFIX, line) != -1) {
      trans->CMD_ERA = replace(line, ERASE_PREFIX, "");
    }
    if (index_of(NEW_PREFIX, line) != -1) {
      trans->CMD_NEW = replace(line, NEW_PREFIX, "");
    }
    if (index_of(ZERO_PREFIX, line) != -1) {
      trans->CMD_ZERO = replace(line, ZERO_PREFIX, "");
    }
  }
}
void init_output(FILE * fp, int size) {
  fprintf(stdout, "Writing %d empty blocks...\n", size);
  int blocks = size * BLOCK_SIZE - 1;
  fseek(fp, blocks, SEEK_SET);
  fputc('\0', fp);
  fseek(fp, 0, SEEK_SET);
  fprintf(stdout, "Write OK !\n");
}
int max(int arr[], int len) {
  int i = 0, tmp, max;
  for (; i < len; i++) {
    if (i == 0) {
      max = arr[i];
    }
    tmp = arr[i];
    if (tmp > max) {
      max = tmp;
    }
  }
  return max;
}
int sdat2img_main(int argc, char *argv[]) {

  int tmp[BUFFER_SIZE];
  transfer trans;
  FILE *list;
  FILE *dat;
  FILE *img;
  time_t startTime;
  time_t endTime;
  startTime = time(NULL);
  if (argc < 3) {
    fprintf(stderr,
            "sdat2img <system.transfer.list> <system.new.dat> <system.ext4.img>\n");
    return 1;
  }
  list = fopen(argv[1], "rb");
  if (!list) {
    fprintf(stderr, "unable to open file : %s\n", argv[1]);
    return 1;
  }
  dat = fopen(argv[2], "rb");
  if (!dat) {
    fprintf(stderr, "unable to open file : %s\n", argv[2]);
    fclose(list);
    return 1;
  }
  img = fopen(argv[3], "wb");
  if (!img) {
    fprintf(stderr, "unable to open file : %s\n", argv[3]);
    fclose(list);
    fclose(dat);
    return 1;
  }
  printf("Parsing transfer file...\n");
  read_transfer(list, &trans);
  char *p;
  int s1 = 0, s2 = 0, s3 = 0;
  strtok(trans.CMD_ERA, ",");
  while ((p = strtok(NULL, ","))) {
    tmp[s1 + s2 + s3] = atoi(p);
    s1++;
  }
  int cmd_erase[s1];
  strtok(trans.CMD_NEW, ",");
  while ((p = strtok(NULL, ","))) {
    tmp[s1 + s2 + s3] = atoi(p);
    s2++;
  }
  int cmd_new[s2];
  strtok(trans.CMD_ZERO, ",");
  while ((p = strtok(NULL, ","))) {
    tmp[s1 + s2 + s3] = atoi(p);
    s3++;
  }
  int cmd_zero[s3];
  int i;
  for (i = 0; i < s1; i++) {
    cmd_erase[i] = tmp[i];
  }
  for (i = 0; i < s2; i++) {
    cmd_new[i] = tmp[s1 + i];
  }
  for (i = 0; i < s3; i++) {
    cmd_zero[i] = tmp[s1 + s2 + i];
  }
  init_output(img, max(tmp, s1 + s2 + s3));
  int end = 0, begin = 0, size = 0, j = 0, count = 0;
  while (j < s2) {
    begin = cmd_new[j];
    j++;
    end = cmd_new[j];
    size = end - begin;
    printf("Writing %d blocks Range(%d, %d)...\n", size, begin, end);
    byte *buf = (byte *) malloc(size * BLOCK_SIZE);
    printf("\tReading...\n");
    int r = fread(buf, BLOCK_SIZE, size, dat);
    printf("\tRead %d bytes\n", r);
    fseek(img, begin * BLOCK_SIZE, SEEK_SET);
    printf("\tWriting...\n");
    int w = fwrite(buf, BLOCK_SIZE, size, img);
    printf("\tWrite %d bytes\n", w);
    j++;
  }
  fclose(list);
  fclose(dat);
  fclose(img);
  printf("Finished!\n");
  endTime = time(NULL);
  fprintf(stdout, "Used Time : %f sec\n", difftime(endTime, startTime));
  return 0;
}
