CC=i686-elf-gcc
CFLAGS=-c -m32 -std=c11 -ffreestanding -O2 -Wall -Wextra -fstack-protector -fstack-shuffle
LDFLAGS= -ffreestanding -O2 -nostdlib -T linker.ld -lgcc
SRCDIR=src
ISODIR=iso
INCLUDEDIR=$(SRCDIR)/libc/include
KINCLUDEDIR=$(SRCDIR)/kernel/include

kernel_source_files := $(shell find src/kernel -name *.c)
kernel_object_files := $(patsubst src/kernel/%.c, build/kernel/%.o, $(kernel_source_files))

c_source_files := $(shell find src/libc -name *.c)
c_object_files := $(patsubst src/libc/%.c, build/kernel/%.o, $(c_source_files))

asm_source_files := $(shell find src/boot/ -name *.asm)
asm_object_files := $(patsubst src/boot/%.asm, build/%.o, $(asm_source_files))

sys_object_files := $(c_object_files) $(asm_object_files)

$(kernel_object_files): build/kernel/%.o : $(SRCDIR)/kernel/%.c
	mkdir -p $(dir $@) && \
	$(CC) $(CFLAGS) -I $(KINCLUDEDIR) $(patsubst build/kernel/%.o, $(SRCDIR)/kernel/%.c, $@) -o $@

$(c_object_files): build/%.o : $(SRCDIR)/libc/%.c 
	mkdir -p $(dir %@) && \
	$(CC) $(CFLAGS) -I $(INCLUDEDIR) $(patsubst build/%.o, $(SRCDIR)/libc/%.c, $@) -o $@

$(asm_object_files): build/%.o : $(SRCDIR)/boot/%.asm
	mkdir -p $(dir $@) && \
	nasm -f elf $(patsubst build/%.o, $(SRCDIR)/boot/%.asm, $@) -o $@

.PHONY: piggyOS run
piggyOS: $(kernel_object_files) $(sys_object_files)
	$(CC) $(LDFLAGS) -o $(ISODIR)/boot/piggyOS.bin $(kernel_object_files) $(sys_object_files) && \
	grub-file --is-x86-multiboot2 $(ISODIR)/boot/piggyOS.bin && \
	grub-mkrescue /usr/lib/grub/i386-pc -o piggyOS.iso $(ISODIR)

run:
	qemu-system-i386 -cdrom piggyOS.iso -m 4G
