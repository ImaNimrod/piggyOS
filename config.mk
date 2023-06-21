# target platform 
TARGET:=i686-elf

# toolchain 
CC:=$(TARGET)-gcc
LD:=$(TARGET)-ld
AS:=nasm
EMU:=qemu-system-i386

KERNEL_DIR:=./kernel
BUILD_DIR:=./build
CDROM:=piggyOS.iso
DISK:=piggyOS-disk.img

# kernel config options
CONFIG_TEXTMODE=Y
