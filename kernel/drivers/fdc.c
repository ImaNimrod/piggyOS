#include <drivers/fdc.h>

static const char *drive_types[8] = {
    "none",
    "360kB 5.25\"",
    "1.2MB 5.25\"",
    "720kB 3.5\"",

    "1.44MB 3.5\"",
    "2.88MB 3.5\"",
    "unknown type",
    "unknown type"
};

static void lba_to_chs(int lba, int *head, int *track, int *sector) {
	*head = (lba % (FLPY_SECTORS_PER_TRACK * 2) ) / FLPY_SECTORS_PER_TRACK;
	*track = lba / (FLPY_SECTORS_PER_TRACK * 2);
	*sector = lba % FLPY_SECTORS_PER_TRACK + 1;
}

static void fdc_irq_handler(regs_t *r) {
    irq_ack(FDC_IRQ);

    (void) r;
}

static void fdc_write_cmd(uint8_t cmd) {
	uint8_t timeout = 0xff;
	while (--timeout) {
        sleep(1); 
		if (inb(FLOPPY_MAIN_STATUS_REGISTER)) {
			outb(FLOPPY_DATA_FIFO, cmd);
			return;
		}
	} 
	kprintf("FATAL: timeout durint %s\n", __func__);
}

static uint8_t fdc_read_data(void) {
    for(int i = 0; i < 600; i++) {
        sleep(1); 
        if (0x80 & inb(FLOPPY_MAIN_STATUS_REGISTER)) {
            return inb(FLOPPY_DATA_FIFO);
        }
    }
    kprintf("%s timed out...", __func__);
    return 0; 
}

static void floppy_sense_interrupt(uint32_t *st0, uint32_t *cyl) {
    fdc_write_cmd(FLOPPY_SENSE_INTERRUPT);

    *st0 = fdc_read_data();
    *cyl = fdc_read_data();
}

void fdc_set_motor(uint8_t drive, uint8_t status) {
	if(drive > 3) {
		return;
	}

	uint8_t motor = 0;
	switch(drive) {
		case 0:
			motor = 0;
			break;
		case 1:
			motor = 1;
			break;
		case 2:
			motor = 2;
			break;
		case 3:
			motor = 3;
			break;
	}

	if(status) {
		outb(FLOPPY_DIGITAL_OUTPUT_REGISTER, drive | motor | 0x0c);
	} else {
		outb(FLOPPY_DIGITAL_OUTPUT_REGISTER, 0x08);
	}
}

void fdc_drive_set(uint8_t step, uint8_t loadt, uint8_t unloadt, uint8_t dma) {
	fdc_write_cmd(FLOPPY_SPECIFY);
	uint8_t data = 0;
	data = ((step&0xf)<<4) | (unloadt & 0xf);
	fdc_write_cmd(data);
	data = ((loadt << 1) | (dma?0:1));
	fdc_write_cmd(data);
}

void fdc_calibrate(uint8_t drive) {
	uint32_t st0, cy1;
	
	fdc_set_motor(drive, 1);
	for(int i = 0; i < 10; i++) {
		fdc_write_cmd(FLOPPY_RECALIBRATE);
		fdc_write_cmd(drive);
        irq_wait(FDC_IRQ);

		floppy_sense_interrupt(&st0, &cy1);
		if(!cy1) {
			fdc_set_motor(drive, 0);
			kprintf("floppy calibration successful!\n");
			return;
		}
	}
}

void fdc_reset(void) {
    outb(FLOPPY_DIGITAL_OUTPUT_REGISTER, 0x00); // disable controller
    sleep(10);
    outb(FLOPPY_DIGITAL_OUTPUT_REGISTER, 0x0C); // enable controller

    irq_wait(FDC_IRQ);
	uint32_t st0, cy1;
	for (int i = 0; i < 4; i++)
		floppy_sense_interrupt(&st0, &cy1);

	kprintf("setting transfer speed to 500Kb/s\n");
	outb(FLOPPY_CONFIGURATION_CONTROL_REGISTER, 0);

	fdc_calibrate(0);

	fdc_drive_set(0x03, 0x10,0xf0, 0x01);
}

static void fdc_dma_init(uint8_t* buffer, uint32_t length) {
	union {
		uint8_t byte[4];
		uint32_t l;
	} a, c;

	a.l = (unsigned) buffer;
	c.l = (unsigned) length -1;

	dma_reset();
	dma_mask_channel(FDC_DMA_CHANNEL);
	dma_reset_flipflop(1);

	dma_set_address(FDC_DMA_CHANNEL, a.byte[0], a.byte[1]);
	dma_reset_flipflop(1);
	dma_set_count( FDC_DMA_CHANNEL, c.byte[0],c.byte[1]);
	dma_set_read ( FDC_DMA_CHANNEL );
	dma_unmask_all();
}

int fdc_seek(unsigned cyli, int head) {
    uint32_t st0, cyl; 

    for (int i = 0; i < 10; i++) {
        fdc_write_cmd(FLOPPY_SEEK);
        fdc_write_cmd(head<<2);
        fdc_write_cmd(cyli);

        irq_wait(FDC_IRQ);
        floppy_sense_interrupt(&st0, &cyl);

        if(st0 & 0xC0) {
            static const char * status[] = { "normal", "error", "invalid", "drive" };
            kprintf("fdc_seek: status = %s\n", status[st0 >> 6]);
            continue;
        }

        if(cyl == cyli) {
            return 0;
        }

    }
    return 1;
}

/* floppy read functions */
static uint8_t floppy_read_sector(uint8_t head, uint8_t track, uint8_t sector) {
    uint32_t st0, cy1;

    fdc_dma_init((uint8_t *) FDC_DMA_BUFFER, 512);
    dma_set_read(FDC_DMA_CHANNEL);
    fdc_write_cmd(FLOPPY_READ_DATA | 0xe0);
    fdc_write_cmd((head << 2)| 0);
    fdc_write_cmd(track);
    fdc_write_cmd(head);
    fdc_write_cmd(sector);
    fdc_write_cmd(0x02);
    fdc_write_cmd(((sector + 1) >= FLPY_SECTORS_PER_TRACK)?FLPY_SECTORS_PER_TRACK:(sector+1));
    fdc_write_cmd(27);
    fdc_write_cmd(0xff);

    irq_wait(FDC_IRQ);

    for(int j = 0; j < 7; j++)
        fdc_read_data();

    floppy_sense_interrupt(&st0, &cy1);
    return 1;
}

static uint8_t* floppy_read_lba(int lba) {
	int head = 0, track = 0, sector = 1;
	int rc = 0;
	lba_to_chs(lba,  &head, &track, &sector);

	fdc_set_motor(0, 1);
	rc = fdc_seek(track, head);
	if(rc) {
        kpanic("floppy failed to seek...");
		return 0;
	}

	floppy_read_sector(head, track, sector);
	fdc_set_motor(0, 0);

	return (uint8_t *) FDC_DMA_BUFFER;
}

uint8_t floppy_read(uint8_t* buffer, uint32_t lba, uint32_t sectors) {
	if(!sectors) return 1;
	uint32_t sectors_read = 0;
	while(sectors_read != sectors) {
		uint8_t *buf = floppy_read_lba(lba + sectors_read);
		if(!buf) return 1;
		memcpy(buffer + sectors_read * 512, buf, 512);
		sectors_read++;
	}
	return 0;
}

uint8_t floppy_write_lba(uint8_t *buf, uint32_t lba) {
	int head = 0, track = 0, sector = 1;
	lba_to_chs(lba, &head, &track, &sector);
	fdc_set_motor(0, 1);
	int rc = fdc_seek(track, head);
	if(rc) {
		kprintf("failed to write to floppy\n");
		return 0;
	}

	memcpy((uint8_t *) FDC_DMA_BUFFER, buf, 512);

	uint32_t st0, cy1;

	fdc_dma_init((uint8_t *) FDC_DMA_BUFFER, 512);
	dma_set_write(FDC_DMA_CHANNEL);
	fdc_write_cmd(FLOPPY_WRITE_DATA | 0xe0);
	fdc_write_cmd((head << 2)| 0);
	fdc_write_cmd(track);
	fdc_write_cmd(head);
	fdc_write_cmd(sector);
	fdc_write_cmd(0x02);
	fdc_write_cmd(((sector+1)>=FLPY_SECTORS_PER_TRACK)?FLPY_SECTORS_PER_TRACK:(sector+1));
	fdc_write_cmd(27);
	fdc_write_cmd(0xff);

    irq_ack(FDC_IRQ);

	for(int j = 0; j < 7; j++)
		fdc_read_data();

	floppy_sense_interrupt(&st0, &cy1);
	return 1;
}

uint8_t floppy_write(uint8_t *buffer, uint32_t lba, uint32_t sectors) {
	if (!sectors) return 0;
	for (uint32_t i = 0; i < sectors; i++) {
		floppy_write_lba(buffer + i * 512, lba + i);
	}

	kprintf("FDC: wrote %d sectors from %d to %d\n", sectors, lba, lba + sectors);
	return 1;
}

void fdc_init(void) {
    kprintf("searching for floppy disks...\n");
    uint8_t cmos = read_cmos(0x10);
    if (cmos >> 4 != 0) {
		kprintf("primary FDC channel detected.\n");
        kprintf("floppy drive 0: %s\n", drive_types[cmos >> 4]);
	} else {
        return;
    }

	if ((cmos & 0x0f) != 0) {
		kprintf("secondary FDC channel detected\n");
        kprintf("floppy drive 1: %s\n", drive_types[cmos & 0x0f]);
	}

    kprintf("enabling floppy support...\n");
    irq_install_handler(FDC_IRQ, fdc_irq_handler);
    fdc_reset();
    kprintf("floppy disk(s) inititalized.\n");
}
