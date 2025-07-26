#!/bin/bash

SCRIPT_DIR=$(dirname "$0")
export PREFIX="$SCRIPT_DIR/../RetroFlex-BIOS-cross"
export TARGET=i386-elf

wget https://ftp.gnu.org/gnu/binutils/binutils-2.44.tar.gz
tar xpvf binutils-*.tar.gz

wget https://ftp.gnu.org/gnu/gcc/gcc-15.1.0/gcc-15.1.0.tar.gz
tar xpvf gcc-*.tar.gz

cd binutils-2.44
mkdir -p build
cd build

../configure --target=$TARGET --prefix="$PREFIX" --with-sysroot --disable-nls --disable-werror

make -j$(nproc)

make install

cd ../..

cd gcc-15.1.0
mkdir -p build
cd build

../configure --target=$TARGET --prefix="$PREFIX" --disable-nls --enable-languages=c,c++ --disable-libstdc++ --disable-libgcc --disable-libgomp --disable-shared

make all-gcc -j$(nproc)

make install-gcc

rm -r binutils-2.43* gcc-14.2.0*
