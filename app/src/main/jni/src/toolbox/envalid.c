#include <sys/stat.h>
#include <stdio.h>
int envalid(char *file){
	if(!file) return 1;
	return chmod(file, 0755);
}
