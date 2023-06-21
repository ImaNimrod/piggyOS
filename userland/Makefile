include ../config.mk

CFLAGS := -c -m32 -g -std=c11 -Os -ffreestanding
CFLAGS += -Wall -Wextra -pedantic -Wstrict-prototypes 
LDFLAGS := -Os -nostdlib -melf_i386

HEADERS := include/
LIBS :=

C_SRCS := init.c
C_OBJS := $(patsubst %.c, $(BUILD_DIR)/userland/%.o, $(C_SRCS))

.PHONY: userland start.o
userland: start.o $(C_OBJS)

start.o: start.asm
	@mkdir -p $(BUILD_DIR)/userland/
	@$(AS) -f elf32 $< -o $(BUILD_DIR)/userland/$@

$(C_OBJS): $(C_SRCS)
	@mkdir -p $(dir $@) 
	@echo "compiling $(basename $(shell basename $@))"
	@$(CC) $(CFLAGS) -I $(HEADERS) $< -o $@
	@$(LD) $(LDFLAGS) -T linker.ld $(BUILD_DIR)/userland/start.o $@ -o $(patsubst $@, $(basename $(INITRD_DIR)/$(shell basename $@)), $@)
