#include <drivers/ata.h>

uint8_t *ide_buf = 0;

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

static uint8_t ide_identify(uint8_t bus, uint8_t drive) {
    uint16_t io = 0;

	ide_select_drive(bus, drive);
	if(bus == ATA_PRIMARY) { 
        io = ATA_PRIMARY_IO;
    } else { 
        io = ATA_SECONDARY_IO;
    }

	outb(io + ATA_REG_SECCOUNT0, 0);
	outb(io + ATA_REG_LBA0, 0);
	outb(io + ATA_REG_LBA1, 0);
	outb(io + ATA_REG_LBA2, 0);
	outb(io + ATA_REG_COMMAND, ATA_CMD_IDENTIFY);

    uint8_t status = inb(io + ATA_REG_STATUS);
    if (status) {
        while((inb(io + ATA_REG_STATUS) & ATA_SR_BSY) != 0);
        pm_stat_read: status = inb(io + ATA_REG_STATUS);
		if(status & ATA_SR_ERR) {
            kprintf("%s%s has ERR set. disabled.\n", bus==ATA_PRIMARY?"primary":"secondary", drive==ATA_PRIMARY?" master":" slave");
            return 0;
        }

        while(!(status & ATA_SR_DRQ)) goto pm_stat_read;
        kprintf("%s%s is online.\n", bus==ATA_PRIMARY?"primary":"secondary", drive==ATA_PRIMARY?" master":" slave");

        for(int i = 0; i<256; i++)
			*(uint16_t *)(ide_buf + i*2) = inw(io + ATA_REG_DATA);
    }

    ide_delay(io);
    return 1;
}

static void ata_probe(void) {
    if (ide_identify(ATA_PRIMARY, ATA_MASTER)) {
        char *str = (char *) kmalloc(40);
		for(int i = 0; i < 40; i += 2) {
            str[i] = ide_buf[ATA_IDENT_MODEL + i + 1];
			str[i + 1] = ide_buf[ATA_IDENT_MODEL + i];
        }
    }
    ide_identify(ATA_PRIMARY, ATA_SLAVE);
}

/* read / write functions */

uint8_t ata_read_sector(uint8_t *buf, uint32_t lba, uint8_t drive) {
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
			return 0;
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
	return 1;
}

uint8_t ata_read(uint8_t *buf, uint32_t lba, size_t sectors, uint8_t drive) {
    __asm__ volatile("cli");

	if(!sectors) return 1;

    for (size_t i = 0; i < sectors; i++) {
        ata_read_sector(buf, lba, drive);
        buf += 512;
    }

    __asm__ volatile("sti");

    return 0;
}

uint8_t ata_write_sector(uint8_t *buf, uint32_t lba, uint8_t drive) {
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
			return 0;
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

	return 1;
}

uint8_t ata_write(uint8_t *buf, uint32_t lba, size_t sectors, uint8_t drive) {
    __asm__ volatile("cli");

	if(!sectors) return 1;

    for (size_t i = 0; i < sectors; i++) {
        ata_write_sector(buf, lba, drive);
        buf += 512;
    }
    kprintf("ATA: wrote %d sectors from %d to %d\n", sectors, lba, lba + sectors);

    __asm__ volatile("sti");

    return 0;
}

void ata_init(void) {
    kprintf("searching for ATA drives\n");

    ide_buf = (uint16_t *) kmalloc(512);

    irq_install_handler(ATA_PRIMARY_IRQ, ata_primary_irq);
    irq_install_handler(ATA_SECONDARY_IRQ, ata_secondary_irq);

    ata_probe();
}
