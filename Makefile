include config.mk

.PHONY: clean kernel disk cdrom run 

all: clean kernel

clean:
	$(RM) -r $(BUILD_DIR)
	$(RM) -r piggyOS.iso
	$(RM) -r piggyOS-disk.img

kernel:
	$(MAKE) -C $(KERNEL_DIR) -f Makefile kernel


run:
	qemu-system-i386 -m 128M\
					 -rtc base=localtime\
					 -serial stdio\
					 -drive format=raw,file=piggyOS-disk.img,index=0,if=ide\

cdrom: 
	@grub-file --is-x86-multiboot2 iso/boot/piggyOS-kernel.bin 
	@grub-mkrescue /usr/lib/grub/i386-pc -o $(CDROM) iso

IMAGE_SECTORS := 2097152 
MAPPER_LINE := 0 $(IMAGE_SECTORS)linear 7:1 0

disk: 
	@dd if=/dev/zero of=$(DISK) bs=512 count=2097152 > /dev/null 2>&1 
	@echo "partitioning disk image..."
	@cat fdisk.conf | fdisk $(DISK) > /dev/null 2>&1 
	@losetup /dev/loop1 $(DISK)
	@echo $(MAPPER_LINE) | dmsetup create hda
	@kpartx -a /dev/mapper/hda
	@mkfs.ext2 /dev/mapper/hda1
	@mount /dev/mapper/hda1 /mnt
	@echo "installing main files."
	@cp -r ./hdd/* /mnt/
	@echo "installing kernel..."
	@mkdir -p /mnt/boot/
	@cp -r ./iso/boot/* /mnt/boot
	@echo "install grub bootloader..."
	@grub-install --target=i386-pc --boot-directory=/mnt/boot /dev/loop1 > /dev/null 2>&1
	@echo "cleaning up temporary mountpoints"
	@umount /mnt/ 
	@kpartx -d /dev/mapper/hda 
	@dmsetup remove hda 
	@losetup -d /dev/loop1
	@echo "successfully created bootable piggyOS disk"
