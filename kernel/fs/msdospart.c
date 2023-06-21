#include <fs/msdospart.h>

void read_parttbl(uint16_t drive, mbr_t *mbr) {
    ide_read_sector(drive, 0, (uint8_t *) mbr);

    if (mbr->magic_number != 0xAA55) {
        kpanic("0x%x\n", mbr->magic_number);
        return;
    }
    
    for (int i = 0; i < 4; i++) {
        kprintf("partition %d: ", i);
        if (mbr->primary_part[i].bootable & 0x80) {
            kprintf("bootable, part_id = ");
        } else {
            kprintf("not bootable, part_id = ");
        }

        kprintf("0x%x\n", mbr->primary_part[i].type);
        kprintf("0x%x\n", mbr->primary_part[i].sector_count);
    }
}
