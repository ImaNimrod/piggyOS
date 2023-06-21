CC=i686-elf-gcc
CFLAGS=-c -m32 -std=c11 -ffreestanding -O2 -Wall -Wextra
LDFLAGS= -ffreestanding -O2 -nostdlib -nostartfiles -nodefaultlibs -T linker.ld -lgcc
BUILDDIR=build
ISODIR=iso
SRCDIR=src
LIBCINCLUDEDIR=$(SRCDIR)/libc/include
KINCLUDEDIR=$(SRCDIR)/kernel/include 

kernel_source_files := $(shell find $(SRCDIR)/kernel -name *.c)
kernel_object_files := $(patsubst $(SRCDIR)/kernel/%.c, $(BUILDDIR)/kernel/%.o, $(kernel_source_files))

libc_source_files := $(shell find $(SRCDIR)/libc/src -name *.c)
libc_object_files := $(patsubst $(SRCDIR)/libc/src/%.c, $(BUILDDIR)/libc/%.o, $(libc_source_files))

asm_source_files := $(shell find $(SRCDIR)/boot/ -name *.asm)
asm_object_files := $(patsubst $(SRCDIR)/boot/%.asm, $(BUILDDIR)/%.o, $(asm_source_files))

sys_object_files := $(libc_object_files) $(asm_object_files)

$(kernel_object_files): $(BUILDDIR)/kernel/%.o : $(SRCDIR)/kernel/%.c 
	mkdir -p $(dir $@) && \
	$(CC) $(CFLAGS) -I $(KINCLUDEDIR) -I $(LIBCINCLUDEDIR) $(patsubst $(BUILDDIR)/kernel/%.o, $(SRCDIR)/kernel/%.c, $@) -o $@

$(libc_object_files): $(BUILDDIR)/libc/%.o : $(SRCDIR)/libc/src/%.c 
	mkdir -p $(dir $@) && \
	$(CC) $(CFLAGS) -I $(LIBCINCLUDEDIR) $(patsubst $(BUILDDIR)/libc/%.o, $(SRCDIR)/libc/src/%.c, $@) -o $@

$(asm_object_files): $(BUILDDIR)/%.o : $(SRCDIR)/boot/%.asm
	mkdir -p $(dir $@) && \
	nasm -f elf32 $(patsubst $(BUILDDIR)/%.o, $(SRCDIR)/boot/%.asm, $@) -o $@

.PHONY: piggyOS run
piggyOS: $(kernel_object_files) $(sys_object_files)
	nasm -f elf32 -o $(BUILDDIR)/kernel/cpu/interrupt.o $(SRCDIR)/kernel/cpu/interrupt.asm
	$(CC) $(LDFLAGS) -o $(ISODIR)/boot/piggyOS.bin $(kernel_object_files) $(sys_object_files) $(BUILDDIR)/kernel/cpu/interrupt.o && \
	grub-file --is-x86-multiboot2 $(ISODIR)/boot/piggyOS.bin && \
	grub-mkrescue /usr/lib/grub/i386-pc -o piggyOS.iso $(ISODIR)

run:
	qemu-system-i386 -cdrom piggyOS.iso -m 4G
