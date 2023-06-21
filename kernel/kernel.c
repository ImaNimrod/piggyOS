#include <cpu/gdt.h>
#include <cpu/idt.h>
#include <cpu/irq.h>
#include <cpu/isr.h>
#include <display.h>
#include <drivers/ata.h>
#include <drivers/keyboard.h>
#include <drivers/pci.h>
#include <drivers/rtc.h>
#include <drivers/timer.h>
#include <drivers/vga.h>
#include <fs/fs.h>
#include <fs/msdospart.h>
#include <memory.h>
#include <string.h>

void kernel_main() {
    vga_clear();
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);

    gdt_init();
    idt_init();
    isr_init();
    irq_init();

    // mmu_init(&kernel_end);
    // paging_init();

    timer_init(100);
    keyboard_init();
    rtc_init();
    // ata_init();

    // mbr_t mbr;
    // uint32_t start_sector;
    // uint32_t max_sector;
    // read_parttbl(0, &mbr);
    //
    // for (int i = 0; i < 4; i++) {
    //     if ((mbr.primary_part[i].bootable & 0x80) && mbr.primary_part[i].type == 0x83) {
    //         kprintf("ext2 partition detected\n");
    //         max_sector = mbr.primary_part[i].sector_count;
    //         start_sector = mbr.primary_part[i].lba_first_sector;
    //         ext2_disk_mount(start_sector, max_sector);
    //         break;
    //     }
    // }
    //
    // vfs_install();
    // devfs_install("/dev");

    // mmu_print_state();

    for(;;) {
        __asm__("hlt");
    }
}
