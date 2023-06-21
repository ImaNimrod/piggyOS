include config.mk

.PHONY: clean kernel userland run initrd cdrom toolchain 

piggyOS: clean kernel userland

clean:
	$(RM) -r $(BUILD_DIR)
	$(RM) piggyOS.iso
	$(RM) $(ISO_DIR)/boot/piggyOS-kernel.bin
	$(RM) -r $(ISO_DIR)/modules/

kernel: 
	@echo "compiling piggyOS kernel"
	@$(MAKE) -C $(KERNEL_DIR) -f Makefile 

userland:
	@echo "compiling and installing piggyOS userland"
	@$(MAKE) -C $(USERLAND_DIR) -f Makefile 

run:
	$(EMU) -m 512 \
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
	@echo "compiling and installing piggyOS toolchain"
	@$(PWD)/toolchain/build-toolchain.sh
