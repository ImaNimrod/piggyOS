include config.mk

.PHONY: clean kernel cdrom run 

piggyOS: clean kernel 

clean:
	$(RM) -r $(BUILD_DIR)
	$(RM) -r piggyOS.iso
	$(RM) -r ./iso/modules/*

kernel:
	$(MAKE) -C $(KERNEL_DIR) -f Makefile piggyOS-kernel.bin

run:
	$(EMU) -m 1G\
		   -cdrom piggyOS.iso\
		   -rtc base=localtime\
		   -serial stdio

initrd:
	@mkdir -p ./iso/modules 
	@genext2fs -d ./rootfs -q -b 16384 ./iso/modules/piggyOS-initrd.img

cdrom: initrd 
	@grub-file --is-x86-multiboot2 iso/boot/piggyOS-kernel.bin 
	@grub-mkrescue /usr/lib/grub/i386-pc -o $(CDROM) iso/
