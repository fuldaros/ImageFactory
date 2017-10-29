#!/bin/bash
[ -z $1 ] && exit 1
LIBS="$1"
arr=(arm64-v8a  armeabi-v7a  x86  x86_64)
for arch in ${arr[@]};do
	file=Compiled/$arch/invoker
	target="${LIBS}/$arch"
	rm -rf $target
	mkdir $target
	cp "$file" "$target/libinvoker.so"
	echo "Install $file to $target/libinvoker.so"
done