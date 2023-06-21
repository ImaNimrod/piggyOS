#include <drivers/ata.h>

uint16_t *ide_buf = 0;

static void ata_primary_irq(regs_t *r) {
    irq_ack(ATA_PRIMARY_IRQ);
    (void) r;
}

static void ata_secondary_irq(regs_t *r) {
    irq_ack(ATA_SECONDARY_IRQ);
    (void) r;
}

static void ide_delay(uint16_t io_base) {
	for(int i = 0;i < 4; i++)
		inb(io_base + ATA_REG_ALTSTATUS);
}

static void ide_wait(uint16_t io_base) {
    ide_delay(io_base);

    retry:;
	uint8_t status = inb(io_base + ATA_REG_STATUS);
	if(status & ATA_SR_BSY) goto retry;
    retry2:	status = inb(io_base + ATA_REG_STATUS);

	if(status & ATA_SR_ERR) {
		kpanic("ERR set, ATA device failure!\n");
	}
	if(!(status & ATA_SR_DRQ)) goto retry2;
	return;
}

static void ide_select_drive(uint8_t bus, uint8_t i) {
	if(bus == ATA_PRIMARY) {
		if(i == ATA_MASTER)
			outb(ATA_PRIMARY_IO + ATA_REG_HDDEVSEL, 0xA0);
		else outb(ATA_PRIMARY_IO + ATA_REG_HDDEVSEL, 0xB0);
    } else {
		if(i == ATA_MASTER)
			outb(ATA_SECONDARY_IO + ATA_REG_HDDEVSEL, 0xA0);
		else outb(ATA_SECONDARY_IO + ATA_REG_HDDEVSEL, 0xB0);
    }
}

/* read / write functions */

void ide_read_sector(uint8_t drive, uint32_t lba, uint8_t *buf) {
	uint16_t io = 0;
	switch(drive) {
		case (ATA_PRIMARY << 1 | ATA_MASTER):
			io = ATA_PRIMARY_IO;
			drive = ATA_MASTER;
			break;
		case (ATA_PRIMARY << 1 | ATA_SLAVE):
			io = ATA_PRIMARY_IO;
			drive = ATA_SLAVE;
			break;
		case (ATA_SECONDARY << 1 | ATA_MASTER):
			io = ATA_SECONDARY_IO;
			drive = ATA_MASTER;
			break;
		case (ATA_SECONDARY << 1 | ATA_SLAVE):
			io = ATA_SECONDARY_IO;
			drive = ATA_SLAVE;
			break;
		default:
			kpanic("FATAL: unknown drive!\n");
	}

	uint8_t cmd = (drive==ATA_MASTER?0xE0:0xF0);

	outb(io + ATA_REG_HDDEVSEL, (cmd | (uint8_t)((lba >> 24 & 0x0F))));
	outb(io + 1, 0x00);
	outb(io + ATA_REG_SECCOUNT0, 1);
	outb(io + ATA_REG_LBA0, (uint8_t)((lba)));
	outb(io + ATA_REG_LBA1, (uint8_t)((lba) >> 8));
	outb(io + ATA_REG_LBA2, (uint8_t)((lba) >> 16));
	outb(io + ATA_REG_COMMAND, ATA_CMD_READ_PIO);

	ide_wait(io);

	for(int i = 0; i < 256; i++) {
		uint16_t data = inw(io + ATA_REG_DATA);
		*(uint16_t *) (buf + i * 2) = data;
	}

	ide_delay(io);
}

void ide_write_sector(uint8_t drive, uint32_t lba, uint8_t *buf) {
	uint16_t io = 0;
	switch(drive) {
		case (ATA_PRIMARY << 1 | ATA_MASTER):
			io = ATA_PRIMARY_IO;
			drive = ATA_MASTER;
			break;
		case (ATA_PRIMARY << 1 | ATA_SLAVE):
			io = ATA_PRIMARY_IO;
			drive = ATA_SLAVE;
			break;
		case (ATA_SECONDARY << 1 | ATA_MASTER):
			io = ATA_SECONDARY_IO;
			drive = ATA_MASTER;
			break;
		case (ATA_SECONDARY << 1 | ATA_SLAVE):
			io = ATA_SECONDARY_IO;
			drive = ATA_SLAVE;
			break;
		default:
			kpanic("FATAL: unknown drive!\n");
	}

	uint8_t cmd = (drive==ATA_MASTER?0xE0:0xF0);

	outb(io + ATA_REG_HDDEVSEL, (cmd | (uint8_t)((lba >> 24 & 0x0F))));
	outb(io + 1, 0x00);
	outb(io + ATA_REG_SECCOUNT0, 1);
	outb(io + ATA_REG_LBA0, (uint8_t)((lba)));
	outb(io + ATA_REG_LBA1, (uint8_t)((lba) >> 8));
	outb(io + ATA_REG_LBA2, (uint8_t)((lba) >> 16));
	outb(io + ATA_REG_COMMAND, ATA_CMD_WRITE_PIO);

	ide_wait(io);

	for(int i = 0; i < 256; i++) {
		outw(io + ATA_REG_DATA, buf[i]);
        ide_delay(io);
	}
}

void ata_init(void) {
    ide_buf = (uint16_t *) kmalloc(512);

    irq_install_handler(ATA_PRIMARY_IRQ, ata_primary_irq);
    irq_install_handler(ATA_SECONDARY_IRQ, ata_secondary_irq);

    klog(LOG_OK, "ATA devices initialized\n");
}
