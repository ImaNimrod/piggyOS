# target platform 
PREFIX:=$(PWD)/toolchain/build/bin
TARGET:=i686-piggyOS

# toolchain 
CC:=$(PREFIX)/$(TARGET)-gcc
LD:=$(PREFIX)/$(TARGET)-ld
AS:=nasm
EMU:=qemu-system-i386

KERNEL_DIR:=$(PWD)/kernel
USERLAND_DIR:=$(PWD)/userland
BUILD_DIR:=$(PWD)/build
INITRD_DIR:=$(PWD)/initrd
ISO_DIR:=$(PWD)/iso

# kernel config options
CONFIG_TEXTMODE=Y
