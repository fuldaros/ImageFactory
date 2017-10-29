LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES += src/main/img2simg.c src/main/simg2img.c
LOCAL_SRC_FILES += src/main/mkcpio.c src/main/uncpio.c
LOCAL_SRC_FILES += src/main/minigzip.c
LOCAL_SRC_FILES += src/main/sdat2img.c
LOCAL_SRC_FILES += src/main/split_app.c
LOCAL_SRC_FILES += src/toolbox/dd.c
LOCAL_SRC_FILES += src/toolbox/envalid.c

# libsparse
LOCAL_SRC_FILES +=  src/libsparse/backed_block.c \
    src/libsparse/output_file.c \
    src/libsparse/sparse.c \
    src/libsparse/sparse_crc32.c \
    src/libsparse/sparse_err.c \
    src/libsparse/sparse_read.c
# libz
LOCAL_SRC_FILES += src/zlib/adler32.c src/zlib/compress.c src/zlib/crc32.c src/zlib/deflate.c 
LOCAL_SRC_FILES += src/zlib/gzclose.c src/zlib/inftrees.c src/zlib/infback.c
LOCAL_SRC_FILES += src/zlib/gzlib.c src/zlib/gzread.c src/zlib/gzwrite.c src/zlib/inflate.c
LOCAL_SRC_FILES += src/zlib/inffast.c src/zlib/trees.c src/zlib/uncompr.c src/zlib/zutil.c
# libselinux
LOCAL_SRC_FILES += src/libselinux/callbacks.c src/libselinux/check_context.c
LOCAL_SRC_FILES += src/libselinux/freecon.c src/libselinux/init.c 
LOCAL_SRC_FILES += src/libselinux/label.c src/libselinux/label_file.c src/libselinux/label_android_property.c

LOCAL_C_INCLUDES := src/include
LOCAL_MODULE := libinvoker_main
LOCAL_CFLAGS := -DANDROID

include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)

LOCAL_SRC_FILES += src/mkbootimg/mkbootimg.c src/mkbootimg/unpackbootimg.c src/mkbootimg/sha.c
LOCAL_C_INCLUDES := src/include
LOCAL_MODULE := libbootimg
LOCAL_CFLAGS := -DANDROID -fno-stack-protector
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)

LOCAL_SRC_FILES += src/invoker.c
LOCAL_CFLAGS := -DANDROID
LOCAL_LDFLAGS := -static
LOCAL_MODULE := invoker
LOCAL_STATIC_LIBRARIES := libinvoker_main libbootimg
LOCAL_FORCE_STATIC_EXECUTABLE := true
include $(BUILD_EXECUTABLE)
