#!/bin/bash
set -e
make clean $@
make CROSS_COMPILE=arm-linux-gnueabi- $@
make clean $@
make CROSS_COMPILE=aarch64-linux-gnu- $@
make clean $@
echo -e "\e[36mOK!\e[0m"