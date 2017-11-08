#include <string.h>
#include <stdio.h>

int mkbootimg_main(int, char **);
int unpackbootimg_main(int, char **);
int uncpio_main(int, char **);
int mkcpio_main(int, char **);
int sdat2img_main(int, char **);
int img2simg_main(int, char **);
int simg2img_main(int, char **);
int minigzip_main(int, char **);
int split_app_main(int, char **);
int dd_main(int, char **v);
int envalid(char *);
int usage(){
	printf("usage: invoker <function [arguments] >\n");
    printf("\tunpackbootimg    unpack android boot.img\n");
    printf("\tmkbootimg        make android boot.img\n");
    printf("\tsplit_app        split huawei .app file\n");
    printf("\tsdat2img         convert andorid dat file to raw image\n");
    printf("\tsimg2img         convert sparse image to raw image\n");
    printf("\timg2simg         convert raw image to sparse image\n");
    printf("\tminigzip         gzip archives operating functions\n");
    printf("\tenvalid          change file permission to 0755\n");
    printf("\tuncpio           extract cpio archives\n");
    printf("\tmkcpio           make cpio archives\n");
    printf("\tdd               minimal dd function\n");
	return 1;
}

int main(int argc, char *argv[])
{
	if(argc < 2) return usage();
	setbuf(stdout, NULL);
	char *arg = argv[1];
	argc--;
	argv++;
	if(!strcmp(arg, "mkbootimg")){
		return mkbootimg_main(argc, argv);
	}else if(!strcmp(arg, "unpackbootimg")){
		return unpackbootimg_main(argc, argv);
	}else if(!strcmp(arg, "uncpio")){
		return uncpio_main(argc, argv);
	}else if(!strcmp(arg, "mkcpio")){
		return mkcpio_main(argc, argv);
	}else if(!strcmp(arg, "sdat2img")){
		return sdat2img_main(argc, argv);
	}else if(!strcmp(arg, "img2simg")){
		return img2simg_main(argc, argv);
	}else if(!strcmp(arg, "simg2img")){
		return simg2img_main(argc, argv);
	}else if(!strcmp(arg, "minigzip")){
		return minigzip_main(argc, argv);
	}else if(!strcmp(arg, "split_app")){
		return split_app_main(argc, argv);
	}else if(!strcmp(arg, "dd")){
		return dd_main(argc, argv);
	}else if(!strcmp(arg, "envalid")){
		return envalid(argv[1]);
	}
	return 1;
}
