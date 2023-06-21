#ifndef _KERNEL_MSDOSPART_H
#define _KERNEL_MSDOSPART_H

#include <display.h>
#include <drivers/ata.h>
#include <fs/fs.h>
#include <stdint.h>

typedef struct {
	uint8_t  bootable;
	uint8_t  chs_first_sector[3];
	uint8_t  type;
	uint8_t  chs_last_sector[3];
	uint32_t lba_first_sector;
	uint32_t sector_count;
} partition_t;

typedef struct {
    uint8_t bootloader[446];
    partition_t primary_part[4];
    uint16_t magic_number;
} __attribute__((packed)) mbr_t;

/* function declarations */ 
void read_parttbl(uint16_t drive, mbr_t *mbr);

#endif
