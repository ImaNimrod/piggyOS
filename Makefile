include config.mk

.PHONY: clean kernel run initrd cdrom toolchain 

piggyOS: clean kernel 

clean:
	$(RM) -r $(BUILD_DIR)
	$(RM) -r piggyOS.iso
	$(RM) -r $(ISO_DIR)/boot/piggyOS-kernel.bin
	$(RM) -r $(ISO_DIR)/modules/

kernel:
	$(MAKE) -C $(KERNEL_DIR) -f Makefile piggyOS-kernel.bin

run:
	$(EMU) -m 256\
		   -name piggyOS \
		   -cdrom piggyOS.iso \
		   -enable-kvm \
		   -rtc base=localtime \
		   -serial stdio

initrd:
	@mkdir -p $(ISO_DIR)/modules
	tar -H ustar -c -f $(ISO_DIR)/modules/piggyOS-initrd.img initrd

cdrom: initrd
	@grub-file --is-x86-multiboot2 iso/boot/piggyOS-kernel.bin 
	@grub-mkrescue /usr/lib/grub/i386-pc -o piggyOS.iso $(ISO_DIR)

toolchain:
	@$(PWD)/toolchain/build-toolchain.sh
