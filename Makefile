include config.mk

.PHONY: clean kernel initrd cdrom run 

piggyOS: clean kernel 

clean:
	$(RM) -r $(BUILD_DIR)
	$(RM) -r piggyOS.iso
	$(RM) -r ./iso/modules/*

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
	@mkdir -p ./iso/modules
	tar -H ustar -c -f iso/modules/piggyOS-initrd.img initrd

cdrom: initrd
	@grub-file --is-x86-multiboot2 iso/boot/piggyOS-kernel.bin 
	@grub-mkrescue /usr/lib/grub/i386-pc -o $(CDROM) iso/
