# target platform 
TARGET:=i686-elf

# toolchain 
CC:=$(TARGET)-gcc
LD:=$(TARGET)-ld
AS:=nasm

KERNEL_DIR:=~/piggyOS/kernel
BUILD_DIR:=~/piggyOS/build
CDROM:=piggyOS.iso
DISK:=piggyOS-disk.img
