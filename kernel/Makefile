include ../config.mk

# kernel specific variables
KERNEL_CFLAGS := -c -m32 -std=c11 -Os -ffreestanding
KERNEL_CFLAGS += -Wall -Wextra -Wno-unused-function -Wstrict-prototypes -Wno-unused-parameter
KERNEL_CFLAGS += -Wnested-externs -Winline -Wpointer-arith -pedantic 
KERNEL_LDFLAGS := -Os -nostdlib -melf_i386

KERNEL_C_SRCS := $(shell find -name '*.c')
KERNEL_ASM_SRCS := $(shell find -name '*.asm')

KERNEL_HEADERS := include/

KERNEL_C_OBJS := $(patsubst %.c, $(BUILD_DIR)/kernel/%.o, $(KERNEL_C_SRCS))
KERNEL_ASM_OBJS := $(patsubst %.asm, $(BUILD_DIR)/kernel/%.o, $(KERNEL_ASM_SRCS))
KERNEL_OBJS := $(KERNEL_C_OBJS) $(KERNEL_ASM_OBJS)

ifeq ($(CONFIG_TEXTMODE), Y)
	KERNEL_CFLAGS += -D TEXTMODE
endif

.PHONY: kernel
kernel: $(KERNEL_OBJS)
	@echo "  LD   piggyOS-kernel.bin"
	@$(LD) $(KERNEL_LDFLAGS) -T linker.ld $(KERNEL_OBJS) -o $(ISO_DIR)/boot/piggyOS-kernel.bin

$(KERNEL_C_OBJS): $(BUILD_DIR)/kernel/%.o : $(KERNEL_C_SRCS)
	@mkdir -p $(dir $@) 
	@echo "  CC   $(shell basename $@)"
	@$(CC) $(KERNEL_CFLAGS) -I $(KERNEL_HEADERS) $(patsubst $(BUILD_DIR)/kernel/%.o, %.c, $@) -o $@

$(KERNEL_ASM_OBJS): $(BUILD_DIR)/kernel/%.o : $(KERNEL_ASM_SRCS)
	@mkdir -p $(dir $@)
	@echo "  AS   $(shell basename $@)"
	@$(AS) -f elf32 -o $@ $(patsubst $(BUILD_DIR)/kernel/%.o, %.asm, $@)
