#include <cpu/desc_tbls.h>
#include <cpu/exception.h>
#include <cpu/irq.h>
#include <display.h>
#include <drivers/ata.h>
#include <drivers/keyboard.h>
#include <drivers/pci.h>
#include <drivers/rtc.h>
#include <drivers/serial.h>
#include <drivers/timer.h>
#include <drivers/vga.h>
#include <fs/fs.h>
#include <fs/msdospart.h>
#include <memory/kheap.h>
#include <memory/pmm.h>
#include <memory/vmm.h>
#include <string.h>
#include <system.h>

void kernel_main(uintptr_t inital_esp) {
    uint32_t inital_stack = inital_esp;
    uint32_t lock = 0;

    #ifdef TEXTMODE
    vga_clear();
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    #endif 

    serial_init();

    /* load descriptor tables and interrupt information */
    gdt_init();
    idt_init();
    tss_init(5, 0x10, 0);
    exception_init();
    irq_init();

    /* initialize physical memory manager */
    pmm_init(1096 * M);

    /* initialize virtual memory manager */
    vmm_init();

    /* initialize kernel kheap */
    kheap_init(KHEAP_START, KHEAP_START + KHEAP_INITIAL_SIZE, KHEAP_MAX_ADDRESS);

    /* initialize devices */
    timer_init(100);
    keyboard_init();
    rtc_init();
    ata_init();
    pci_init();

    vga_set_color(VGA_COLOR_PINK, VGA_COLOR_BLACK);
    kprintf("\nWelecome to piggyOS!\n");

    for(;;) {
        __asm__("hlt");
    }
}

