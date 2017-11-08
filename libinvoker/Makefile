CC := $(CROSS_COMPILE)gcc
CFLAGS := -I src/include -I src/zlib -DANDROID -static -O2 -fPIE -fPIC
LDFLAGS := -I src/include -I src/zlib -DANDROID -static -O2 -fPIE -fPIC
BIN := $(CROSS_COMPILE)invoker
SRC_FILES := src/main/img2simg.c \
	src/main/simg2img.c \
	src/main/mkcpio.c \
	src/main/uncpio.c \
	src/main/minigzip.c \
    src/main/sdat2img.c \
    src/main/split_app.c \
	src/zlib/adler32.c \
    src/zlib/compress.c \
    src/zlib/crc32.c \
    src/zlib/deflate.c  \
    src/zlib/gzclose.c \
    src/zlib/inftrees.c \
    src/zlib/infback.c \
    src/zlib/gzlib.c \
    src/zlib/gzread.c \
    src/zlib/gzwrite.c \
    src/zlib/inflate.c \
    src/zlib/inffast.c \
    src/zlib/trees.c \
    src/zlib/uncompr.c \
    src/zlib/zutil.c \
    src/toolbox/dd.c \
    src/toolbox/envalid.c \
    src/libsparse/backed_block.c \
    src/libsparse/output_file.c \
    src/libsparse/sparse.c \
    src/libsparse/sparse_crc32.c \
    src/libsparse/sparse_err.c \
    src/libsparse/sparse_read.c \
    src/libselinux/callbacks.c \
    src/libselinux/check_context.c \
    src/libselinux/freecon.c \
    src/libselinux/init.c  \
    src/libselinux/label.c \
    src/libselinux/label_file.c \
    src/libselinux/label_android_property.c \
    src/mkbootimg/mkbootimg.c \
    src/mkbootimg/unpackbootimg.c \
    src/mkbootimg/sha.c \
    src/invoker.c
all: $(BIN)
.c.o:
	@echo "	CC $<"
	@$(CC) $(CFLAGS) -c $< -o $@

$(BIN): ${SRC_FILES:.c=.o}
	@echo "	CCLD $(@)"
	@$(CC) $(LDFLAGS) $^ -o $@

.PHONY:clean
clean:
	@echo "	CLEAN UP"
	@rm -f ${SRC_FILES:.c=.o}