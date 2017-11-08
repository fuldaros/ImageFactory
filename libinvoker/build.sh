#!/bin/bash
ndk-build APP_PROJECT_PATH="./" APP_BUILD_SCRIPT="./Android.mk" NDK_APPLICATION_MK="./Application.mk" NDK_LIBS_OUT="Compiled" NDK_OUT="./obj" $@

