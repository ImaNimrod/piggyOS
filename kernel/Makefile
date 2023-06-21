include ../config.mk

# kernel specific variables
KERNEL_CFLAGS := -c -m32 -std=c11 -Os -ffreestanding -Wall -Wextra 
KERNEL_CFLAGS += -Wno-unused-function -Wno-unused-parameter -pedantic
KERNEL_CFLAGS += -mgeneral-regs-only 
KERNEL_LDFLAGS= -Os -nostdlib -T linker.ld 

KERNEL_C_SRCS := $(shell find -name '*.c')
KERNEL_C_OBJS := $(patsubst ./%.c, ../build/kernel/%.o, $(KERNEL_C_SRCS))
KERNEL_ASM_SRCS := $(shell find -name '*.asm')
KERNEL_ASM_OBJS := $(patsubst ./%.asm, ../build/kernel/%.o, $(KERNEL_ASM_SRCS))
KERNEL_HEADERS := ./include

$(KERNEL_C_OBJS): ../build/kernel/%.o : $(KERNEL_C_SRCS)
	@mkdir -p $(dir $@) 
	@echo "  CC          $@"
	@$(CC) $(KERNEL_CFLAGS) -I $(KERNEL_HEADERS) $(patsubst ../build/kernel/%.o, ./%.c, $@) -o $@

$(KERNEL_ASM_OBJS): ../build/kernel/%.o : $(KERNEL_ASM_SRCS)
	@mkdir -p $(dir $@)
	@echo "  AS          $@"
	@$(AS) -f elf32 -o $@ $(patsubst ../build/kernel/%.o, ./%.asm, $@)

KERNEL_OBJS := $(KERNEL_C_OBJS) $(KERNEL_ASM_OBJS) 

.PHONY: piggyOS-kernel.bin
piggyOS-kernel.bin: $(KERNEL_OBJS)
	@echo "  LD          $@"
	@$(LD) $(KERNEL_LDFLAGS) $(KERNEL_OBJS) -o ../iso/boot/piggyOS-kernel.bin
