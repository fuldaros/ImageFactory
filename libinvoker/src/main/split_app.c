#include    <stdlib.h>
#include    <stdio.h>
#include    <string.h>
#include    <strings.h>
#include    <ctype.h>
#include <sys/stat.h>
#include <unistd.h>
#include <limits.h>
#define u8 unsigned char
#define u32 unsigned int
#define u16 unsigned short
#define BUFFER_SIZE 4
typedef struct _image {
  // u32 magic
  u32 packet_size;
  u32 flag;
  char hardware[8];
  u32 filetype;
  u32 data_size;
  char date[16];
  char time[16];
  char filename[32];
  // u8 other[packet_size-92]
} image;

// 字符串真实长度
int realStrlen(char *string) {
  int i = 0;
  char ch = 0;
  while ((ch = *(string + i)) != '\xFF') {
    i++;
  }
  return i;
}

// 去掉字符串尾部 \xFF
void trim_str(char *string) {
  char tmpstr[PATH_MAX];
  strncpy(tmpstr, string, realStrlen(string));
  strcpy(string, tmpstr);
}

// 字符串大写转换小写
void str_tolower(char *string) {
  int i = 0;
  for (i = 0; i < strlen(string); ++i) {
    string[i] = tolower(string[i]);
  }
}

// 重命名镜像文件
void rename_imgfname(char *filename, int number) {
  // 针对MTK, 它们所有Image名字都是INPUT
  char tmp[PATH_MAX];
  if (strcmp(filename, "INPUT") == 0) {
    sprintf(tmp, "output_%02d.img", number);
    strcpy(filename, tmp);
  }
  // 针对普通机型
  else {
    str_tolower(filename);
    snprintf(tmp, sizeof(tmp) - 1, "%s.img", filename);
    strcpy(filename, tmp);
  }
}
int is_need(char *filters[], int sz, char *s) {
  if (sz == 0)
    return 0;
  int p = 0;
  char *arg = filters[p];
  while (arg) {
    if (!strcmp(arg, s))
      return 0;
    p++;
    arg = filters[p];
  }
  return 1;
}
int do_list(FILE * fp) {

  // 镜像信息
  u32 magic = 0xa55aaa55;
  image img;
  int fp_start = 0;
  int number = 1;
  u32 tmp;
  /* for compare magic */
  while (!feof(fp)) {
    fread(&tmp, 1, sizeof(tmp), fp);
    if (tmp == magic) {
      memset(&img, 0, sizeof(img));
      fp_start = (ftell(fp) - 4);
      // 读取镜像相关信息
      fread(&img, 1, sizeof(img), fp);
      trim_str(img.filename);
      rename_imgfname(img.filename, number++);
      printf("%s\n", img.filename);
      fseek(fp, (fp_start + (img.data_size / sizeof(magic)) * sizeof(magic)),
            SEEK_SET);
    }
    if (feof(fp))               /* if the end of file, break! */
      break;
  }
  return 0;
}
int getStringLength(char *string){
    int i=0;
    char ch;
    while ( (ch = *(string + i)) != '\xFF' ) {
        i++;
    }
    return i;
}
int do_split(FILE * fp, char *filters[], int sz) {
    FILE *fd;
    unsigned int tmp;
    unsigned int magic=0xa55aaa55;
    unsigned int packet_size=0;
    unsigned int flag=1;
    unsigned char  hwid[8]="";
    unsigned int filetype=0;
    unsigned int data_size=0;
    unsigned char  date[16]="";
    unsigned char  time[16]="";
    unsigned char  filename[32]="";
    int other_size=0;
    int number=1;
    char basename[32]="";
    char basehwid[8]="";
    int fp_start=0;
    int fp_local=0;
    int i=0,count=0,counts=0;
    int end_point=0;
        while (!feof(fp)) {
        fscanf(fp, "%4c", &tmp);
        if (tmp == magic) {
            fp_start=(ftell(fp)-4);
            fscanf(fp, "%4c",&packet_size);
            fscanf(fp, "%4c",&flag);
            fscanf(fp, "%8c",&hwid);
            fscanf(fp, "%4c",&filetype);
            fscanf(fp, "%4c",&data_size);
            fscanf(fp, "%16c",&date);
            fscanf(fp, "%16c",&time);
            fscanf(fp, "%32c",&filename);
            other_size=( packet_size - 92 );
            fseek(fp, other_size, SEEK_CUR);
            trim_str(filename);
            rename_imgfname(filename, number++);
            strncpy(basehwid, hwid , getStringLength(hwid));
            if(is_need(filters, sz, filename)){
                fseek(fp, (fp_start + (data_size / sizeof(magic)) * sizeof(magic)),
            SEEK_SET);
                continue;
            }
            else
                printf("Spliting %s\n", filename);
            if ((fd=fopen(filename,"wb"))==NULL) {
                fseek(fp, 88, SEEK_CUR);
                continue;
            }
            unsigned char buffer[BUFFER_SIZE];
            counts = 0;
            while ( counts < data_size ){
                if (feof(fp)) {
                    break;
                }
                count = fread(buffer, 1, BUFFER_SIZE, fp);
                fwrite(buffer, 1 , BUFFER_SIZE, fd);
                counts+=count;
            } 
            fclose(fd);
            fseek(fp, (fp_start + (data_size / sizeof(magic)) * sizeof(magic)),
            SEEK_SET);
        }
    }
  return 0;
}

int split_app_main(int argc, char *argv[]) {
  if (argc == 1) {
    printf
      ("split_app : [--list] <UPDATE.APP> [output directory [filter images]]\n");
    return 1;
  }
  int split = 1;
  char *filters[argc];
  int fs = 0;
  char *directory = "./";
  if (argc > 2) {
    if (!strcmp(argv[1], "--list")) {
      split = 0;
      argc--;
      argv++;
    } else {
      directory = argv[2];
      char *arg = argv[3];
      while (arg && fs < argc - 3) {
        filters[fs] = arg;
        printf("Add filter %s ...\n", arg);
        fs++;
        arg = argv[fs + 3];
      }
    }
  }
  if (split) {
    mkdir(directory, 0777);
    chdir(directory);
  }
  // 文件
  char *file;                   /* app文件 */
  FILE *fp;
  file = argv[1];
  if ((fp = fopen(file, "rb")) == NULL) {
    printf("open file %s failure!\n", file);
    exit(1);
  }
  int c = 1;
  if (split)
    c = do_split(fp, filters, fs);
  else
    c = do_list(fp);
  fclose(fp);
  return c;
}