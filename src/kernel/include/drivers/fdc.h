#ifndef _KERNEL_FDC_H
#define _KERNEL_FDC_H

#include <cpu/irq.h>
#include <display.h>
#include <drivers/dma.h>
#include <drivers/io_port.h>
#include <drivers/rtc.h>
#include <drivers/timer.h>
#include <memory.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#define FDC_IRQ 6

#define FDC_DMA_CHANNEL 2
#define FLPY_SECTORS_PER_TRACK 18
#define FDC_DMA_BUFFER 0x1000

enum fdc_reg {
    FLOPPY_STATUS_REGISTER_A                = 0x3F0, // read-only
    FLOPPY_FLOPPY_STATUS_REGISTER_B         = 0x3F1, // read-only
    FLOPPY_DIGITAL_OUTPUT_REGISTER          = 0x3F2,
    FLOPPY_TAPE_DRIVE_REGISTER              = 0x3F3,
    FLOPPY_MAIN_STATUS_REGISTER             = 0x3F4, // read-only
    FLOPPY_DATARATE_SELECT_REGISTER         = 0x3F4, // write-only
    FLOPPY_DATA_FIFO                        = 0x3F5,
    FLOPPY_DIGITAL_INPUT_REGISTER           = 0x3F7, // read-only
    FLOPPY_CONFIGURATION_CONTROL_REGISTER   = 0x3F7  // write-only
};

enum fdc_cmd {
    FLOPPY_READ_TRACK =                 2,	// generates IRQ6
    FLOPPY_SPECIFY =                    3,      //  set drive parameters
    FLOPPY_SENSE_DRIVE_STATUS =         4,
    FLOPPY_WRITE_DATA =                 5,      // * write to the disk
    FLOPPY_READ_DATA =                  6,      // * read from the disk
    FLOPPY_RECALIBRATE =                7,      // * seek to cylinder 0
    FLOPPY_SENSE_INTERRUPT =            8,      // * ack IRQ6, get status of last command
    FLOPPY_WRITE_DELETED_DATA =         9,
    FLOPPY_READ_ID =                    10,	// generates IRQ6
    FLOPPY_READ_DELETED_DATA =          12,
    FLOPPY_FORMAT_TRACK =               13,     // *
    FLOPPY_DUMPREG =                    14,
    FLOPPY_SEEK =                       15,     // * seek both heads to cylinder X
    FLOPPY_VERSION =                    16,	// * used during initialization, once
    FLOPPY_SCAN_EQUAL =                 17,
    FLOPPY_PERPENDICULAR_MODE =         18,	// * used during initialization, once, maybe
    FLOPPY_CONFIGURE =                  19,     // * set controller parameters
    FLOPPY_LOCK =                       20,     // * protect controller params from a reset
    FLOPPY_VERIFY =                     22,
    FLOPPY_SCAN_LOW_OR_EQUAL =          25,
    FLOPPY_SCAN_HIGH_OR_EQUAL =         29
};

/* function declarations */
void fdc_init(void);
uint8_t floppy_read(uint8_t* buffer, uint32_t lba, uint32_t sectors);
uint8_t floppy_write(uint8_t *buffer, uint32_t lba, uint32_t sectors);

#endif 
